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

#include "Meta_View.h"

#include "Meta_Package_Option.h"
#include "Meta_View_Field.h"
#include "Meta_Permission.h"
#include "Meta_Class.h"
#include "Meta_Model.h"
#include "Meta_View_Type.h"

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
#include "Meta_Enum.h"
#include "Meta_Type.h"
#include "Meta_Field.h"
// [<finish includes>]

using namespace std;

// [<start namespaces>]
// [<finish namespaces>]

template< > inline string to_string( const Meta_Permission& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_Class& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_Model& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_View_Type& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

inline void from_string( Meta_Permission& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_Class& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_Model& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_View_Type& c, const string& s )
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

#include "Meta_View.cmh"

const int32_t c_version = 1;

const char* const c_field_id_Access_Permission = "301825";
const char* const c_field_id_Access_Restriction = "118102";
const char* const c_field_id_Allow_Copy_Action = "118113";
const char* const c_field_id_Allow_Printable_Version = "118109";
const char* const c_field_id_Auto_Back_After_Save = "118115";
const char* const c_field_id_Change_Permission = "301830";
const char* const c_field_id_Change_Restriction = "118103";
const char* const c_field_id_Class = "301820";
const char* const c_field_id_File_Links_Always_As_Single = "118117";
const char* const c_field_id_Id = "118105";
const char* const c_field_id_Ignore_Parent_Record = "118118";
const char* const c_field_id_Ignore_Unactionable_Records = "118116";
const char* const c_field_id_Model = "301800";
const char* const c_field_id_Name = "118101";
const char* const c_field_id_PDF_Font_Type = "118112";
const char* const c_field_id_PDF_View_Type = "118111";
const char* const c_field_id_Print_Without_Highlight = "118106";
const char* const c_field_id_Title = "118104";
const char* const c_field_id_Type = "301810";
const char* const c_field_id_Type_Key = "118110";
const char* const c_field_id_Use_First_Row_As_Header = "118114";

const char* const c_field_name_Access_Permission = "Access_Permission";
const char* const c_field_name_Access_Restriction = "Access_Restriction";
const char* const c_field_name_Allow_Copy_Action = "Allow_Copy_Action";
const char* const c_field_name_Allow_Printable_Version = "Allow_Printable_Version";
const char* const c_field_name_Auto_Back_After_Save = "Auto_Back_After_Save";
const char* const c_field_name_Change_Permission = "Change_Permission";
const char* const c_field_name_Change_Restriction = "Change_Restriction";
const char* const c_field_name_Class = "Class";
const char* const c_field_name_File_Links_Always_As_Single = "File_Links_Always_As_Single";
const char* const c_field_name_Id = "Id";
const char* const c_field_name_Ignore_Parent_Record = "Ignore_Parent_Record";
const char* const c_field_name_Ignore_Unactionable_Records = "Ignore_Unactionable_Records";
const char* const c_field_name_Model = "Model";
const char* const c_field_name_Name = "Name";
const char* const c_field_name_PDF_Font_Type = "PDF_Font_Type";
const char* const c_field_name_PDF_View_Type = "PDF_View_Type";
const char* const c_field_name_Print_Without_Highlight = "Print_Without_Highlight";
const char* const c_field_name_Title = "Title";
const char* const c_field_name_Type = "Type";
const char* const c_field_name_Type_Key = "Type_Key";
const char* const c_field_name_Use_First_Row_As_Header = "Use_First_Row_As_Header";

const char* const c_field_display_name_Access_Permission = "field_view_access_permission";
const char* const c_field_display_name_Access_Restriction = "field_view_access_restriction";
const char* const c_field_display_name_Allow_Copy_Action = "field_view_allow_copy_action";
const char* const c_field_display_name_Allow_Printable_Version = "field_view_allow_printable_version";
const char* const c_field_display_name_Auto_Back_After_Save = "field_view_auto_back_after_save";
const char* const c_field_display_name_Change_Permission = "field_view_change_permission";
const char* const c_field_display_name_Change_Restriction = "field_view_change_restriction";
const char* const c_field_display_name_Class = "field_view_class";
const char* const c_field_display_name_File_Links_Always_As_Single = "field_view_file_links_always_as_single";
const char* const c_field_display_name_Id = "field_view_id";
const char* const c_field_display_name_Ignore_Parent_Record = "field_view_ignore_parent_record";
const char* const c_field_display_name_Ignore_Unactionable_Records = "field_view_ignore_unactionable_records";
const char* const c_field_display_name_Model = "field_view_model";
const char* const c_field_display_name_Name = "field_view_name";
const char* const c_field_display_name_PDF_Font_Type = "field_view_pdf_font_type";
const char* const c_field_display_name_PDF_View_Type = "field_view_pdf_view_type";
const char* const c_field_display_name_Print_Without_Highlight = "field_view_print_without_highlight";
const char* const c_field_display_name_Title = "field_view_title";
const char* const c_field_display_name_Type = "field_view_type";
const char* const c_field_display_name_Type_Key = "field_view_type_key";
const char* const c_field_display_name_Use_First_Row_As_Header = "field_view_use_first_row_as_header";

const int c_num_fields = 21;

const char* const c_all_sorted_field_ids[ ] =
{
   "118101",
   "118102",
   "118103",
   "118104",
   "118105",
   "118106",
   "118109",
   "118110",
   "118111",
   "118112",
   "118113",
   "118114",
   "118115",
   "118116",
   "118117",
   "118118",
   "301800",
   "301810",
   "301820",
   "301825",
   "301830"
};

const char* const c_all_sorted_field_names[ ] =
{
   "Access_Permission",
   "Access_Restriction",
   "Allow_Copy_Action",
   "Allow_Printable_Version",
   "Auto_Back_After_Save",
   "Change_Permission",
   "Change_Restriction",
   "Class",
   "File_Links_Always_As_Single",
   "Id",
   "Ignore_Parent_Record",
   "Ignore_Unactionable_Records",
   "Model",
   "Name",
   "PDF_Font_Type",
   "PDF_View_Type",
   "Print_Without_Highlight",
   "Title",
   "Type",
   "Type_Key",
   "Use_First_Row_As_Header"
};

inline bool compare( const char* p_s1, const char* p_s2 ) { return strcmp( p_s1, p_s2 ) < 0; }

inline bool has_field( const string& field )
{
   return binary_search( c_all_sorted_field_ids, c_all_sorted_field_ids + c_num_fields, field.c_str( ), compare )
    || binary_search( c_all_sorted_field_names, c_all_sorted_field_names + c_num_fields, field.c_str( ), compare );
}

const int c_num_encrypted_fields = 0;

bool is_encrypted_field( const string& ) { static bool false_value( false ); return false_value; }

const int c_num_transient_fields = 1;

const char* const c_transient_sorted_field_ids[ ] =
{
   "118101"
};

const char* const c_transient_sorted_field_names[ ] =
{
   "Name"
};

inline bool is_transient_field( const string& field )
{
   return binary_search( c_transient_sorted_field_ids,
    c_transient_sorted_field_ids + c_num_transient_fields, field.c_str( ), compare )
    || binary_search( c_transient_sorted_field_names,
    c_transient_sorted_field_names + c_num_transient_fields, field.c_str( ), compare );
}

const char* const c_procedure_id_Generate_PDF_View = "118410";

const uint64_t c_modifier_Is_Not_Print_Version = UINT64_C( 0x100 );
const uint64_t c_modifier_Is_Print_Version = UINT64_C( 0x200 );
const uint64_t c_modifier_PDF_View_Is_Custom = UINT64_C( 0x400 );
const uint64_t c_modifier_PDF_View_Is_None = UINT64_C( 0x800 );
const uint64_t c_modifier_Printing_Disallowed = UINT64_C( 0x1000 );
const uint64_t c_modifier_Protect_Access = UINT64_C( 0x2000 );
const uint64_t c_modifier_Protect_Change = UINT64_C( 0x4000 );

aggregate_domain< string,
 domain_string_identifier_format,
 domain_string_max_size< 30 > > g_Id_domain;
domain_string_max_size< 100 > g_Name_domain;
domain_string_max_size< 100 > g_Title_domain;

string g_group_field_name;
string g_level_field_name;
string g_order_field_name;
string g_owner_field_name;

string g_state_names_variable;

set< string > g_derivations;

set< string > g_file_field_ids;
set< string > g_file_field_names;

typedef map< string, Meta_View* > external_aliases_container;
typedef external_aliases_container::const_iterator external_aliases_const_iterator;
typedef external_aliases_container::value_type external_aliases_value_type;

typedef map< size_t, Meta_View* > external_aliases_lookup_container;
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
bool g_default_Allow_Copy_Action = bool( 0 );
bool g_default_Allow_Printable_Version = bool( 0 );
bool g_default_Auto_Back_After_Save = bool( 0 );
string g_default_Change_Permission = string( );
int g_default_Change_Restriction = int( 0 );
string g_default_Class = string( );
bool g_default_File_Links_Always_As_Single = bool( 0 );
string g_default_Id = string( );
bool g_default_Ignore_Parent_Record = bool( 0 );
bool g_default_Ignore_Unactionable_Records = bool( 0 );
string g_default_Model = string( );
string g_default_Name = string( );
int g_default_PDF_Font_Type = int( 0 );
int g_default_PDF_View_Type = int( 0 );
bool g_default_Print_Without_Highlight = bool( 0 );
string g_default_Title = string( "?" );
string g_default_Type = string( );
string g_default_Type_Key = string( );
bool g_default_Use_First_Row_As_Header = bool( 0 );

set< int > g_view_access_restrict_enum;
set< int > g_view_change_restrict_enum;
set< int > g_pdf_font_type_enum;
set< int > g_view_pdf_view_type_enum;

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

const int c_enum_pdf_font_type_Times_Roman( 0 );
const int c_enum_pdf_font_type_Helvetica( 1 );
const int c_enum_pdf_font_type_SimSun( 20 );
const int c_enum_pdf_font_type_SimHei( 21 );

string get_enum_string_pdf_font_type( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for pdf_font_type" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_pdf_font_type_Times_Roman";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_pdf_font_type_Helvetica";
   else if( to_string( val ) == to_string( "20" ) )
      string_name = "enum_pdf_font_type_SimSun";
   else if( to_string( val ) == to_string( "21" ) )
      string_name = "enum_pdf_font_type_SimHei";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for pdf_font_type" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_view_pdf_view_type_none( 0 );
const int c_enum_view_pdf_view_type_Standard_2_Column( 2 );
const int c_enum_view_pdf_view_type_Customised( 99 );

string get_enum_string_view_pdf_view_type( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for view_pdf_view_type" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_view_pdf_view_type_none";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_view_pdf_view_type_Standard_2_Column";
   else if( to_string( val ) == to_string( "99" ) )
      string_name = "enum_view_pdf_view_type_Customised";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for view_pdf_view_type" );

   return get_module_string( lower( string_name ) );
}

// [<start anonymous>]
// [<finish anonymous>]

}

registration< Meta_View > View_registration( get_class_registry( ), "118100" );

class Meta_View_command_functor;

class Meta_View_command_handler : public command_handler
{
   friend class Meta_View_command_functor;

   public:
   Meta_View_command_handler( ) : p_Meta_View( 0 ) { }

   void set_Meta_View( Meta_View* p_new_Meta_View ) { p_Meta_View = p_new_Meta_View; }

   void handle_unknown_command( const string& command, const string& cmd_and_args )
   {
      throw runtime_error( "unknown command '" + command + "'" );
   }

   void handle_invalid_command( const command_parser& parser, const string& cmd_and_args )
   {
      throw runtime_error( "invalid command usage '" + cmd_and_args + "'" );
   }

   private:
   Meta_View* p_Meta_View;

   protected:
   string retval;
};

class Meta_View_command_functor : public command_functor
{
   public:
   Meta_View_command_functor( Meta_View_command_handler& handler )
    : command_functor( handler ),
    cmd_handler( handler )
   {
   }

   void operator ( )( const string& command, const parameter_info& parameters );

   private:
   Meta_View_command_handler& cmd_handler;
};

command_functor* Meta_View_command_functor_factory( const string& /*name*/, command_handler& handler )
{
   return new Meta_View_command_functor( dynamic_cast< Meta_View_command_handler& >( handler ) );
}

void Meta_View_command_functor::operator ( )( const string& command, const parameter_info& parameters )
{
   if( command == c_cmd_Meta_View_key )
   {
      bool want_fixed( has_parm_val( parameters, c_cmd_Meta_View_key_fixed ) );

      if( !want_fixed )
         cmd_handler.retval = cmd_handler.p_Meta_View->get_key( );
      else
         cmd_handler.retval = cmd_handler.p_Meta_View->get_fixed_key( );
   }
   else if( command == c_cmd_Meta_View_ver )
   {
      string ver_rev( to_string( cmd_handler.p_Meta_View->get_version( ) ) );
      ver_rev += "." + to_string( cmd_handler.p_Meta_View->get_revision( ) );

      cmd_handler.retval = ver_rev;
   }
   else if( command == c_cmd_Meta_View_get )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_View_get_field_name ) );

      bool handled = false;

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for getter call" );

      if( !handled && ( ( field_name == c_field_id_Access_Permission ) || ( field_name == c_field_name_Access_Permission ) ) )
      {
         handled = true;

         string_getter< Meta_Permission >( cmd_handler.p_Meta_View->Access_Permission( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Access_Restriction ) || ( field_name == c_field_name_Access_Restriction ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_View->Access_Restriction( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Copy_Action ) || ( field_name == c_field_name_Allow_Copy_Action ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_View->Allow_Copy_Action( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Printable_Version ) || ( field_name == c_field_name_Allow_Printable_Version ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_View->Allow_Printable_Version( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Auto_Back_After_Save ) || ( field_name == c_field_name_Auto_Back_After_Save ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_View->Auto_Back_After_Save( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Change_Permission ) || ( field_name == c_field_name_Change_Permission ) ) )
      {
         handled = true;

         string_getter< Meta_Permission >( cmd_handler.p_Meta_View->Change_Permission( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Change_Restriction ) || ( field_name == c_field_name_Change_Restriction ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_View->Change_Restriction( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Class ) || ( field_name == c_field_name_Class ) ) )
      {
         handled = true;

         string_getter< Meta_Class >( cmd_handler.p_Meta_View->Class( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_File_Links_Always_As_Single ) || ( field_name == c_field_name_File_Links_Always_As_Single ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_View->File_Links_Always_As_Single( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Id ) || ( field_name == c_field_name_Id ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_View->Id( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Ignore_Parent_Record ) || ( field_name == c_field_name_Ignore_Parent_Record ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_View->Ignore_Parent_Record( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Ignore_Unactionable_Records ) || ( field_name == c_field_name_Ignore_Unactionable_Records ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_View->Ignore_Unactionable_Records( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Model ) || ( field_name == c_field_name_Model ) ) )
      {
         handled = true;

         string_getter< Meta_Model >( cmd_handler.p_Meta_View->Model( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Name ) || ( field_name == c_field_name_Name ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_View->Name( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_PDF_Font_Type ) || ( field_name == c_field_name_PDF_Font_Type ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_View->PDF_Font_Type( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_PDF_View_Type ) || ( field_name == c_field_name_PDF_View_Type ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_View->PDF_View_Type( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Print_Without_Highlight ) || ( field_name == c_field_name_Print_Without_Highlight ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_View->Print_Without_Highlight( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Title ) || ( field_name == c_field_name_Title ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_View->Title( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Type ) || ( field_name == c_field_name_Type ) ) )
      {
         handled = true;

         string_getter< Meta_View_Type >( cmd_handler.p_Meta_View->Type( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Type_Key ) || ( field_name == c_field_name_Type_Key ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_View->Type_Key( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Use_First_Row_As_Header ) || ( field_name == c_field_name_Use_First_Row_As_Header ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_View->Use_First_Row_As_Header( ), cmd_handler.retval );
      }

      if( !handled )
         throw runtime_error( "unknown field name '" + field_name + "' for getter call" );
   }
   else if( command == c_cmd_Meta_View_set )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_View_set_field_name ) );
      string field_value( get_parm_val( parameters, c_cmd_Meta_View_set_field_value ) );

      bool handled = false;

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for setter call" );

      if( !handled && ( ( field_name == c_field_id_Access_Permission ) || ( field_name == c_field_name_Access_Permission ) ) )
      {
         handled = true;

         func_string_setter< Meta_View, Meta_Permission >(
          *cmd_handler.p_Meta_View, &Meta_View::Access_Permission, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Access_Restriction ) || ( field_name == c_field_name_Access_Restriction ) ) )
      {
         handled = true;

         func_string_setter< Meta_View, int >(
          *cmd_handler.p_Meta_View, &Meta_View::Access_Restriction, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Copy_Action ) || ( field_name == c_field_name_Allow_Copy_Action ) ) )
      {
         handled = true;

         func_string_setter< Meta_View, bool >(
          *cmd_handler.p_Meta_View, &Meta_View::Allow_Copy_Action, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Printable_Version ) || ( field_name == c_field_name_Allow_Printable_Version ) ) )
      {
         handled = true;

         func_string_setter< Meta_View, bool >(
          *cmd_handler.p_Meta_View, &Meta_View::Allow_Printable_Version, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Auto_Back_After_Save ) || ( field_name == c_field_name_Auto_Back_After_Save ) ) )
      {
         handled = true;

         func_string_setter< Meta_View, bool >(
          *cmd_handler.p_Meta_View, &Meta_View::Auto_Back_After_Save, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Change_Permission ) || ( field_name == c_field_name_Change_Permission ) ) )
      {
         handled = true;

         func_string_setter< Meta_View, Meta_Permission >(
          *cmd_handler.p_Meta_View, &Meta_View::Change_Permission, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Change_Restriction ) || ( field_name == c_field_name_Change_Restriction ) ) )
      {
         handled = true;

         func_string_setter< Meta_View, int >(
          *cmd_handler.p_Meta_View, &Meta_View::Change_Restriction, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Class ) || ( field_name == c_field_name_Class ) ) )
      {
         handled = true;

         func_string_setter< Meta_View, Meta_Class >(
          *cmd_handler.p_Meta_View, &Meta_View::Class, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_File_Links_Always_As_Single ) || ( field_name == c_field_name_File_Links_Always_As_Single ) ) )
      {
         handled = true;

         func_string_setter< Meta_View, bool >(
          *cmd_handler.p_Meta_View, &Meta_View::File_Links_Always_As_Single, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Id ) || ( field_name == c_field_name_Id ) ) )
      {
         handled = true;

         func_string_setter< Meta_View, string >(
          *cmd_handler.p_Meta_View, &Meta_View::Id, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Ignore_Parent_Record ) || ( field_name == c_field_name_Ignore_Parent_Record ) ) )
      {
         handled = true;

         func_string_setter< Meta_View, bool >(
          *cmd_handler.p_Meta_View, &Meta_View::Ignore_Parent_Record, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Ignore_Unactionable_Records ) || ( field_name == c_field_name_Ignore_Unactionable_Records ) ) )
      {
         handled = true;

         func_string_setter< Meta_View, bool >(
          *cmd_handler.p_Meta_View, &Meta_View::Ignore_Unactionable_Records, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Model ) || ( field_name == c_field_name_Model ) ) )
      {
         handled = true;

         func_string_setter< Meta_View, Meta_Model >(
          *cmd_handler.p_Meta_View, &Meta_View::Model, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Name ) || ( field_name == c_field_name_Name ) ) )
      {
         handled = true;

         func_string_setter< Meta_View, string >(
          *cmd_handler.p_Meta_View, &Meta_View::Name, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_PDF_Font_Type ) || ( field_name == c_field_name_PDF_Font_Type ) ) )
      {
         handled = true;

         func_string_setter< Meta_View, int >(
          *cmd_handler.p_Meta_View, &Meta_View::PDF_Font_Type, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_PDF_View_Type ) || ( field_name == c_field_name_PDF_View_Type ) ) )
      {
         handled = true;

         func_string_setter< Meta_View, int >(
          *cmd_handler.p_Meta_View, &Meta_View::PDF_View_Type, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Print_Without_Highlight ) || ( field_name == c_field_name_Print_Without_Highlight ) ) )
      {
         handled = true;

         func_string_setter< Meta_View, bool >(
          *cmd_handler.p_Meta_View, &Meta_View::Print_Without_Highlight, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Title ) || ( field_name == c_field_name_Title ) ) )
      {
         handled = true;

         func_string_setter< Meta_View, string >(
          *cmd_handler.p_Meta_View, &Meta_View::Title, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Type ) || ( field_name == c_field_name_Type ) ) )
      {
         handled = true;

         func_string_setter< Meta_View, Meta_View_Type >(
          *cmd_handler.p_Meta_View, &Meta_View::Type, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Type_Key ) || ( field_name == c_field_name_Type_Key ) ) )
      {
         handled = true;

         func_string_setter< Meta_View, string >(
          *cmd_handler.p_Meta_View, &Meta_View::Type_Key, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Use_First_Row_As_Header ) || ( field_name == c_field_name_Use_First_Row_As_Header ) ) )
      {
         handled = true;

         func_string_setter< Meta_View, bool >(
          *cmd_handler.p_Meta_View, &Meta_View::Use_First_Row_As_Header, field_value );
      }

      if( !handled )
         throw runtime_error( "unknown field name '" + field_name + "' for setter call" );

      cmd_handler.retval = c_okay;
   }
   else if( command == c_cmd_Meta_View_cmd )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_View_cmd_field_name ) );
      string cmd_and_args( get_parm_val( parameters, c_cmd_Meta_View_cmd_cmd_and_args ) );

      cmd_handler.retval.erase( );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for command call" );
      else if( ( field_name == c_field_id_Access_Permission ) || ( field_name == c_field_name_Access_Permission ) )
         cmd_handler.retval = cmd_handler.p_Meta_View->Access_Permission( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Change_Permission ) || ( field_name == c_field_name_Change_Permission ) )
         cmd_handler.retval = cmd_handler.p_Meta_View->Change_Permission( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Class ) || ( field_name == c_field_name_Class ) )
         cmd_handler.retval = cmd_handler.p_Meta_View->Class( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Model ) || ( field_name == c_field_name_Model ) )
         cmd_handler.retval = cmd_handler.p_Meta_View->Model( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Type ) || ( field_name == c_field_name_Type ) )
         cmd_handler.retval = cmd_handler.p_Meta_View->Type( ).execute( cmd_and_args );
      else
         throw runtime_error( "unknown field name '" + field_name + "' for command call" );
   }
   else if( command == c_cmd_Meta_View_Generate_PDF_View )
   {
      cmd_handler.p_Meta_View->Generate_PDF_View( );

      cmd_handler.retval.erase( );

   }
}

struct Meta_View::impl : public Meta_View_command_handler
{
   impl( Meta_View& o )
    :
    cp_obj( &o ),
    total_child_relationships( 0 )
   {
      p_obj = &o;
      set_Meta_View( &o );

      add_commands( 0, Meta_View_command_functor_factory,
       ARRAY_PTR_AND_SIZE( Meta_View_command_definitions ) );
   }

   Meta_View& get_obj( ) const
   {
      return *cp_obj;
   }

   int impl_Access_Restriction( ) const { return lazy_fetch( p_obj ), v_Access_Restriction; }
   void impl_Access_Restriction( int Access_Restriction ) { v_Access_Restriction = Access_Restriction; }

   bool impl_Allow_Copy_Action( ) const { return lazy_fetch( p_obj ), v_Allow_Copy_Action; }
   void impl_Allow_Copy_Action( bool Allow_Copy_Action ) { v_Allow_Copy_Action = Allow_Copy_Action; }

   bool impl_Allow_Printable_Version( ) const { return lazy_fetch( p_obj ), v_Allow_Printable_Version; }
   void impl_Allow_Printable_Version( bool Allow_Printable_Version ) { v_Allow_Printable_Version = Allow_Printable_Version; }

   bool impl_Auto_Back_After_Save( ) const { return lazy_fetch( p_obj ), v_Auto_Back_After_Save; }
   void impl_Auto_Back_After_Save( bool Auto_Back_After_Save ) { v_Auto_Back_After_Save = Auto_Back_After_Save; }

   int impl_Change_Restriction( ) const { return lazy_fetch( p_obj ), v_Change_Restriction; }
   void impl_Change_Restriction( int Change_Restriction ) { v_Change_Restriction = Change_Restriction; }

   bool impl_File_Links_Always_As_Single( ) const { return lazy_fetch( p_obj ), v_File_Links_Always_As_Single; }
   void impl_File_Links_Always_As_Single( bool File_Links_Always_As_Single ) { v_File_Links_Always_As_Single = File_Links_Always_As_Single; }

   const string& impl_Id( ) const { return lazy_fetch( p_obj ), v_Id; }
   void impl_Id( const string& Id ) { sanity_check( Id ); v_Id = Id; }

   bool impl_Ignore_Parent_Record( ) const { return lazy_fetch( p_obj ), v_Ignore_Parent_Record; }
   void impl_Ignore_Parent_Record( bool Ignore_Parent_Record ) { v_Ignore_Parent_Record = Ignore_Parent_Record; }

   bool impl_Ignore_Unactionable_Records( ) const { return lazy_fetch( p_obj ), v_Ignore_Unactionable_Records; }
   void impl_Ignore_Unactionable_Records( bool Ignore_Unactionable_Records ) { v_Ignore_Unactionable_Records = Ignore_Unactionable_Records; }

   const string& impl_Name( ) const { return lazy_fetch( p_obj ), v_Name; }
   void impl_Name( const string& Name ) { sanity_check( Name ); v_Name = Name; }

   int impl_PDF_Font_Type( ) const { return lazy_fetch( p_obj ), v_PDF_Font_Type; }
   void impl_PDF_Font_Type( int PDF_Font_Type ) { v_PDF_Font_Type = PDF_Font_Type; }

   int impl_PDF_View_Type( ) const { return lazy_fetch( p_obj ), v_PDF_View_Type; }
   void impl_PDF_View_Type( int PDF_View_Type ) { v_PDF_View_Type = PDF_View_Type; }

   bool impl_Print_Without_Highlight( ) const { return lazy_fetch( p_obj ), v_Print_Without_Highlight; }
   void impl_Print_Without_Highlight( bool Print_Without_Highlight ) { v_Print_Without_Highlight = Print_Without_Highlight; }

   const string& impl_Title( ) const { return lazy_fetch( p_obj ), v_Title; }
   void impl_Title( const string& Title ) { sanity_check( Title ); v_Title = Title; }

   const string& impl_Type_Key( ) const { return lazy_fetch( p_obj ), v_Type_Key; }
   void impl_Type_Key( const string& Type_Key ) { sanity_check( Type_Key ); v_Type_Key = Type_Key; }

   bool impl_Use_First_Row_As_Header( ) const { return lazy_fetch( p_obj ), v_Use_First_Row_As_Header; }
   void impl_Use_First_Row_As_Header( bool Use_First_Row_As_Header ) { v_Use_First_Row_As_Header = Use_First_Row_As_Header; }

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

   Meta_Class& impl_Class( )
   {
      if( !cp_Class )
      {
         cp_Class.init( );

         p_obj->setup_graph_parent( *cp_Class, c_field_id_Class, v_Class );
      }
      return *cp_Class;
   }

   const Meta_Class& impl_Class( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Class )
      {
         cp_Class.init( );

         p_obj->setup_graph_parent( *cp_Class, c_field_id_Class, v_Class );
      }
      return *cp_Class;
   }

   void impl_Class( const string& key )
   {
      class_base_accessor cba( impl_Class( ) );
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

   Meta_View_Type& impl_Type( )
   {
      if( !cp_Type )
      {
         cp_Type.init( );

         p_obj->setup_graph_parent( *cp_Type, c_field_id_Type, v_Type );
      }
      return *cp_Type;
   }

   const Meta_View_Type& impl_Type( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Type )
      {
         cp_Type.init( );

         p_obj->setup_graph_parent( *cp_Type, c_field_id_Type, v_Type );
      }
      return *cp_Type;
   }

   void impl_Type( const string& key )
   {
      class_base_accessor cba( impl_Type( ) );
      cba.set_key( key );
   }

   Meta_Class& impl_child_Class_Created( )
   {
      if( !cp_child_Class_Created )
      {
         cp_child_Class_Created.init( );

         p_obj->setup_graph_parent( *cp_child_Class_Created, "300631" );
      }
      return *cp_child_Class_Created;
   }

   const Meta_Class& impl_child_Class_Created( ) const
   {
      if( !cp_child_Class_Created )
      {
         cp_child_Class_Created.init( );

         p_obj->setup_graph_parent( *cp_child_Class_Created, "300631" );
      }
      return *cp_child_Class_Created;
   }

   Meta_Package_Option& impl_child_Package_Option( )
   {
      if( !cp_child_Package_Option )
      {
         cp_child_Package_Option.init( );

         p_obj->setup_graph_parent( *cp_child_Package_Option, "302830c" );
      }
      return *cp_child_Package_Option;
   }

   const Meta_Package_Option& impl_child_Package_Option( ) const
   {
      if( !cp_child_Package_Option )
      {
         cp_child_Package_Option.init( );

         p_obj->setup_graph_parent( *cp_child_Package_Option, "302830c" );
      }
      return *cp_child_Package_Option;
   }

   Meta_View_Field& impl_child_View_Field( )
   {
      if( !cp_child_View_Field )
      {
         cp_child_View_Field.init( );

         p_obj->setup_graph_parent( *cp_child_View_Field, "301900" );
      }
      return *cp_child_View_Field;
   }

   const Meta_View_Field& impl_child_View_Field( ) const
   {
      if( !cp_child_View_Field )
      {
         cp_child_View_Field.init( );

         p_obj->setup_graph_parent( *cp_child_View_Field, "301900" );
      }
      return *cp_child_View_Field;
   }

   void impl_Generate_PDF_View( );

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

   Meta_View* p_obj;
   class_pointer< Meta_View > cp_obj;

   mutable set< string > required_transients;

   // [<start members>]
   // [<finish members>]

   size_t total_child_relationships;

   int v_Access_Restriction;
   bool v_Allow_Copy_Action;
   bool v_Allow_Printable_Version;
   bool v_Auto_Back_After_Save;
   int v_Change_Restriction;
   bool v_File_Links_Always_As_Single;
   string v_Id;
   bool v_Ignore_Parent_Record;
   bool v_Ignore_Unactionable_Records;
   string v_Name;
   int v_PDF_Font_Type;
   int v_PDF_View_Type;
   bool v_Print_Without_Highlight;
   string v_Title;
   string v_Type_Key;
   bool v_Use_First_Row_As_Header;

   string v_Access_Permission;
   mutable class_pointer< Meta_Permission > cp_Access_Permission;

   string v_Change_Permission;
   mutable class_pointer< Meta_Permission > cp_Change_Permission;

   string v_Class;
   mutable class_pointer< Meta_Class > cp_Class;

   string v_Model;
   mutable class_pointer< Meta_Model > cp_Model;

   string v_Type;
   mutable class_pointer< Meta_View_Type > cp_Type;

   mutable class_pointer< Meta_Class > cp_child_Class_Created;
   mutable class_pointer< Meta_Package_Option > cp_child_Package_Option;
   mutable class_pointer< Meta_View_Field > cp_child_View_Field;
};

void Meta_View::impl::impl_Generate_PDF_View( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start Generate_PDF_View_impl>]
//nyi
   string sh_filename( get_obj( ).Model( ).Name( ) );
   sh_filename += "_" + get_obj( ).Class( ).Name( ) + ".pdf.sh";

   string sio_filename( get_obj( ).Model( ).Name( ) );
   sio_filename += "_" + get_obj( ).Class( ).Name( ) + ".pdf.sio";

   string vars_filename( get_obj( ).Model( ).Name( ) );
   vars_filename += "_" + get_obj( ).Class( ).Name( ) + ".pdf.vars.xrep";

   if( get_obj( ).PDF_View_Type( ) == 2 ) // i.e. Standard 2 Column
   {
      string key_info( to_string( Meta_View_Field::static_get_field_id( Meta_View_Field::e_field_id_Order ) ) + ' ' );

      if( get_obj( ).child_View_Field( ).iterate_forwards( key_info ) )
      {
         string groups;

         int num_groups = 1;
         bool start_new_group = false;

         set< int > notes_groups;

         string fields;

         int num_fields = 0;

         string extra_special;

         vector< string > field_names;

         set< string > html_fields;

         set< string > wide_fields;
         set< string > notes_fields;
         set< string > hidden_fields;

         set< string > large_font_fields;
         set< string > small_font_fields;
         set< string > larger_font_fields;
         set< string > smaller_font_fields;

         map< string, string > extras;
         map< string, string > prefixes;
         map< string, string > suffixes;

         map< string, string > uom_fields;

         map< string, string > date_fields;
         map< string, string > time_fields;
         map< string, string > numeric_fields;
         map< string, string > datetime_fields;

         map< string, string > restrict_fields;
         map< string, string > permission_fields;

         do
         {
            string name;

            if( !is_null( get_obj( ).child_View_Field( ).Source_Field( ) ) )
            {
               name = get_obj( ).child_View_Field( ).Source_Field( ).Name( );

               field_names.push_back( "@" + name );
               field_names.push_back( name );

               if( !is_null( get_obj( ).child_View_Field( ).Source_Field( ).Enum( ) ) )
               {
                  extras.insert( make_pair( name, "indirect" ) );
                  prefixes.insert( make_pair( name, "@enum_" + get_obj( ).child_View_Field( ).Source_Field( ).Enum( ).Name( ) + "_" ) );
               }

               if( get_obj( ).child_View_Field( ).Source_Field( ).Extra( ) == 9 ) // i.e. html
                  html_fields.insert( name );
               else if( get_obj( ).child_View_Field( ).Source_Field( ).Extra( ) == 31 ) // i.e. special
                  extra_special = name;
               else if( get_obj( ).child_View_Field( ).Source_Field( ).Extra( ) == 32 ) // i.e. prefix_special
                  prefixes.insert( make_pair( name, "@special" ) );
               else if( get_obj( ).child_View_Field( ).Source_Field( ).Extra( ) == 33 ) // i.e. suffix_special
                  suffixes.insert( make_pair( name, "@special" ) );

               if( get_obj( ).child_View_Field( ).Access_Restriction( ) == 4 ) // i.e. denied_always
                  hidden_fields.insert( name );
               else if( get_obj( ).child_View_Field( ).Source_Field( ).Extra( ) == 4 // i.e. notes
                || get_obj( ).child_View_Field( ).Source_Field( ).Extra( ) == 9 ) // i.e. html
                  notes_fields.insert( name );
               else if( get_obj( ).child_View_Field( ).Source_Field( ).Type( ).Primitive( ) == 0 // i.e. string
                && get_obj( ).child_View_Field( ).Source_Field( ).Type( ).Max_Size( ) >= 50 )
                  wide_fields.insert( name );
               else if( get_obj( ).child_View_Field( ).Source_Field( ).Type( ).Primitive( ) >= 1 // i.e. datetime
                && get_obj( ).child_View_Field( ).Source_Field( ).Type( ).Primitive( ) <= 5 ) // i.e. int
               {
                  string domain_mask;

                  string domain_type( meta_field_domain_type(
                   get_obj( ).child_View_Field( ).Source_Field( ).Enum( ).Id( ),
                   get_obj( ).child_View_Field( ).Source_Field( ).Primitive( ),
                   get_obj( ).child_View_Field( ).Source_Field( ).Type( ).Max_Size( ),
                   get_obj( ).child_View_Field( ).Source_Field( ).Type( ).Min_Value( ),
                   get_obj( ).child_View_Field( ).Source_Field( ).Type( ).Max_Value( ),
                   get_obj( ).child_View_Field( ).Source_Field( ).Type( ).Numeric_Digits( ),
                   get_obj( ).child_View_Field( ).Source_Field( ).Type( ).Numeric_Decimals( ),
                   get_obj( ).child_View_Field( ).Source_Field( ).Type( ).String_Domain( ),
                   get_obj( ).child_View_Field( ).Source_Field( ).Type( ).Date_Precision( ),
                   get_obj( ).child_View_Field( ).Source_Field( ).Type( ).Time_Precision( ),
                   get_obj( ).child_View_Field( ).Source_Field( ).Type( ).Show_Plus_Sign( ),
                   get_obj( ).child_View_Field( ).Source_Field( ).Type( ).Zero_Padding( ),
                   get_obj( ).child_View_Field( ).Source_Field( ).Type( ).Fraction_Limit( ), domain_mask ) );

                  int uom = get_obj( ).child_View_Field( ).Source_Field( ).UOM( );

                  if( uom == 999 )
                     uom_fields.insert( make_pair( name, "uom" ) );
                  else if( uom != 0 )
                     uom_fields.insert( make_pair( name, meta_field_uom( uom ) ) );

                  if( get_obj( ).child_View_Field( ).Source_Field( ).Type( ).Primitive( ) == 4 // i.e. numeric
                   || get_obj( ).child_View_Field( ).Source_Field( ).Type( ).Primitive( ) == 5 ) // i.e. int
                     numeric_fields.insert( make_pair( name, domain_mask ) );
                  else if( get_obj( ).child_View_Field( ).Source_Field( ).Type( ).Primitive( ) == 1 ) // i.e. datetime
                     datetime_fields.insert( make_pair( name, domain_mask ) );
                  else if( get_obj( ).child_View_Field( ).Source_Field( ).Type( ).Primitive( ) == 2 ) // i.e. date
                     date_fields.insert( make_pair( name, domain_mask ) );
                  else if( get_obj( ).child_View_Field( ).Source_Field( ).Type( ).Primitive( ) == 3 ) // i.e. time
                     time_fields.insert( make_pair( name, domain_mask ) );
               }
            }
            else if( !is_null( get_obj( ).child_View_Field( ).Source_Parent( ) ) )
            {
               name = get_obj( ).child_View_Field( ).Source_Parent( ).Name( );
               name += "_" + get_obj( ).child_View_Field( ).Source_Child( ).Name( );

               field_names.push_back( "@" + name );
               field_names.push_back( name );

               if( !is_null( get_obj( ).child_View_Field( ).Source_Child( ).Enum( ) ) )
               {
                  extras.insert( make_pair( name, "indirect" ) );
                  prefixes.insert( make_pair( name, "@enum_" + get_obj( ).child_View_Field( ).Source_Child( ).Enum( ).Name( ) + "_" ) );
               }
            }
            else
               throw runtime_error( "unknown field type for '"
                + get_obj( ).child_View_Field( ).Name( ) + "' in PDF generation for view '" + get_obj( ).Name( ) + "'" );

            switch( get_obj( ).child_View_Field( ).Font_Size( ) )
            {
               case 0:
               break;

               case 1:
               large_font_fields.insert( name );
               break;

               case 2:
               larger_font_fields.insert( name );
               break;

               case 5:
               small_font_fields.insert( name );
               break;

               case 6:
               smaller_font_fields.insert( name );
               break;

               default:
               throw runtime_error( "unexpected Font_Size value #"
                + to_string( get_obj( ).child_View_Field( ).Font_Size( ) ) + " in generate_pdf_list" );
            }

            switch( get_obj( ).child_View_Field( ).Access_Restriction( ) )
            {
               case 0:
               case 4: // NOTE: Hidden fields are taken care of above.
               break;

               case 1:
               restrict_fields.insert( make_pair( name, "owner_only" ) );
               break;

               case 2:
               restrict_fields.insert( make_pair( name, "admin_only" ) );
               break;

               case 3:
               restrict_fields.insert( make_pair( name, "admin_owner" ) );
               break;

               default:
               throw runtime_error( "unexpected access restriction value #"
                + to_string( get_obj( ).child_View_Field( ).Access_Restriction( ) ) + " in generate_pdf_view" );
            }

            if( !is_null( get_obj( ).child_View_Field( ).Access_Permission( ) ) )
               permission_fields.insert( make_pair( name, get_obj( ).child_View_Field( ).Access_Permission( ).Name( ) ) );

            if( start_new_group || notes_fields.count( name ) )
            {
               if( !groups.empty( ) )
                  groups += " ";
               groups += "g" + to_string( num_groups );
            }

            if( notes_fields.count( name ) )
            {
               notes_groups.insert( ++num_groups );

               start_new_group = true;
            }
            else if( start_new_group )
            {
               ++num_groups;
               start_new_group = false;
            }

            if( !fields.empty( ) )
               fields += " ";
            fields += "f" + to_string( ++num_fields );
            fields += " f" + to_string( ++num_fields );

         } while( get_obj( ).child_View_Field( ).iterate_next( ) );

         ofstream batf( sh_filename.c_str( ) );

         batf << "./xrep @Standard_A4_View.pdf.sio.xrep vars=" << vars_filename << " >" << sio_filename << ".new\n";
         batf << "./update " << sio_filename << " " << sio_filename << ".new\n";

         batf.flush( );
         if( !batf.good( ) )
            throw runtime_error( "'" + sh_filename + "' output stream is bad" );

         ofstream varsf( vars_filename.c_str( ) );
         if( !varsf )
            throw runtime_error( "unable to open file '" + vars_filename + "' for output" );

         string font_name;
         string font_bname;
         string font_encoding;

         switch( get_obj( ).PDF_Font_Type( ) )
         {
            case 0:
            font_name = "Times-Roman";
            font_bname = "Times-Bold";
            font_encoding = "CP1250";
            break;

            case 1:
            font_name = "Helvetica";
            font_bname = "Helvetica-Bold";
            font_encoding = "CP1250";
            break;

            case 20:
            font_name = "SimSun";
            font_bname = "SimSun,Bold";
            font_encoding = "GB-EUC-H";
            break;

            case 21:
            font_name = "SimHei";
            font_name = "SimHei,Bold";
            font_encoding = "GB-EUC-H";
            break;

            default:
            throw runtime_error( "unexpected PDF_Font_Type #"
             + to_string( get_obj( ).PDF_Font_Type( ) ) + " in Generate_PDF_List" );
         }

         varsf << "\x60{\x60$font_name\x60=\x60'" << font_name << "\x60'\x60}\n";
         varsf << "\x60{\x60$font_bname\x60=\x60'" << font_bname << "\x60'\x60}\n";
         varsf << "\x60{\x60$font_encoding\x60=\x60'" << font_encoding << "\x60'\x60}\n";

         if( !groups.empty( ) )
         {
            varsf << "\x60{\x60}\n";
            varsf << "\x60{\x60$groups\x60=\x60'" << groups << "\x60'\x60}\n";

            for( int i = 1; i < num_groups; i++ )
            {
               varsf << "\x60{\x60}\n";

               varsf << "\x60{\x60$g" << to_string( i ) << "_pos\x60=\x60'+0,"
                << ( notes_groups.count( i ) ? "+20" : "+5" ) << "\x60'\x60}\n";

               varsf << "\x60{\x60$g" << to_string( i )
                << "_name\x60=\x60'extra" << to_string( i ) << "\x60'\x60}\n";
            }
         }

         int vpos = 0;
         int group = 0;

         bool need_new_group = false;
         bool is_second_column = false;

         string value_color( "80,80,80" );

         varsf << "\x60{\x60}\n";
         varsf << "\x60{\x60$fields\x60=\x60'" << fields << "\x60'\x60}\n";
         for( size_t i = 0; i < field_names.size( ); i++ )
         {
            varsf << "\x60{\x60}\n";

            string next_field( field_names[ i ] );

            string test_field( next_field );

            bool is_label = false;
            if( !next_field.empty( ) && next_field[ 0 ] == '@' )
            {
               is_label = true;
               test_field = next_field.substr( 1 );

               if( notes_fields.count( test_field ) )
               {
                  ++group;
                  vpos = 0;
                  need_new_group = true;
                  is_second_column = false;
               }
               else if( need_new_group )
               {
                  ++group;
                  vpos = 0;
                  need_new_group = false;
                  is_second_column = false;
               }
            }

            bool is_growable = false;
            if( notes_fields.count( test_field ) )
               is_growable = true;

            if( is_label && wide_fields.count( test_field ) )
            {
               if( is_second_column )
               {
                  vpos += 30;
                  is_second_column = false;
               }
            }

            int hpos = 0;

            if( is_second_column )
               hpos = 300;

            if( !is_label )
               hpos += 85;

            int width = 85;

            if( !is_label )
            {
               if( wide_fields.count( next_field ) )
                  width = 500;
               else if( notes_fields.count( next_field ) )
                  width = 470;
               else
                  width = 200;
            }

            if( hidden_fields.count( test_field ) )
               width = 0;

            int height = 2;
            if( is_growable )
               height = 1;

            varsf << "\x60{\x60$f" << to_string( i + 1 )
             << "_pos\x60=\x60'+" << hpos << ",+" << vpos << "\x60'\x60}\n";

            varsf << "\x60{\x60$f" << to_string( i + 1 )
             << "_name\x60=\x60'" << next_field << "\x60'\x60}\n";

            if( restrict_fields.count( test_field ) )
               varsf << "\x60{\x60$f" << to_string( i + 1 )
                << "_rest\x60=\x60'" << restrict_fields[ test_field ] << "\x60'\x60}\n";

            if( permission_fields.count( test_field ) )
               varsf << "\x60{\x60$f" << to_string( i + 1 )
                << "_perm\x60=\x60'" << permission_fields[ test_field ] << "\x60'\x60}\n";

            if( is_growable )
               varsf << "\x60{\x60$f" << to_string( i + 1 )
                << "_size\x60=\x60'growable\x60'\x60}\n";

            if( extras.count( next_field ) )
               varsf << "\x60{\x60$f" << to_string( i + 1 )
                << "_extra\x60=\x60'" << extras[ next_field ] << "\x60'\x60}\n";

            string next_group;
            if( group == 0 )
               next_group = "basic";
            else
               next_group = "extra" + to_string( group );

            varsf << "\x60{\x60$f" << to_string( i + 1 )
             << "_group\x60=\x60'" << next_group << "\x60'\x60}\n";

            if( is_label )
               varsf << "\x60{\x60$f" << to_string( i + 1 ) << "_font\x60=\x60'label\x60'\x60}\n";
            else
            {
               if( large_font_fields.count( next_field ) )
                  varsf << "\x60{\x60$f" << to_string( i + 1 ) << "_font\x60=\x60'value_large\x60'\x60}\n";
               else if( small_font_fields.count( next_field ) )
                  varsf << "\x60{\x60$f" << to_string( i + 1 ) << "_font\x60=\x60'value_small\x60'\x60}\n";
               else if( larger_font_fields.count( next_field ) )
                  varsf << "\x60{\x60$f" << to_string( i + 1 ) << "_font\x60=\x60'value_larger\x60'\x60}\n";
               else if( smaller_font_fields.count( next_field ) )
                  varsf << "\x60{\x60$f" << to_string( i + 1 ) << "_font\x60=\x60'value_smaller\x60'\x60}\n";
               else
                  varsf << "\x60{\x60$f" << to_string( i + 1 ) << "_font\x60=\x60'value\x60'\x60}\n";
            }

            if( date_fields.count( next_field ) )
               varsf << "\x60{\x60$f" << to_string( i + 1 )
                << "_mask\x60=\x60'" << date_fields[ next_field ] << "\x60'\x60}\n";
            else if( time_fields.count( next_field ) )
               varsf << "\x60{\x60$f" << to_string( i + 1 )
                << "_mask\x60=\x60'" << time_fields[ next_field ] << "\x60'\x60}\n";
            else if( numeric_fields.count( next_field ) )
               varsf << "\x60{\x60$f" << to_string( i + 1 )
                << "_mask\x60=\x60'" << numeric_fields[ next_field ] << "\x60'\x60}\n";
            else if( datetime_fields.count( next_field ) )
               varsf << "\x60{\x60$f" << to_string( i + 1 )
                << "_mask\x60=\x60'" << datetime_fields[ next_field ] << "\x60'\x60}\n";

            if( !is_label )
               varsf << "\x60{\x60$f" << to_string( i + 1 )
                << "_color\x60=\x60'" << value_color << "\x60'\x60}\n";

            varsf << "\x60{\x60$f" << to_string( i + 1 )
             << "_width\x60=\x60'" << to_string( width ) << "\x60'\x60}\n";

            varsf << "\x60{\x60$f" << to_string( i + 1 )
             << "_height\x60=\x60'" << to_string( height ) << "\x60'\x60}\n";

            varsf << "\x60{\x60$f" << to_string( i + 1 )
             << "_padding\x60=\x60'" << ( is_label ? "0" : "10" ) << ",0\x60'\x60}\n";

            if( notes_fields.count( next_field ) )
               varsf << "\x60{\x60$f" << to_string( i + 1 )
                << "_spacing\x60=\x60'3\x60'\x60}\n";

            if( html_fields.count( next_field ) )
               varsf << "\x60{\x60$f" << to_string( i + 1 )
                << "_special\x60=\x60'html\x60'\x60}\n";

            varsf << "\x60{\x60$f" << to_string( i + 1 )
             << "_alignment\x60=\x60'" << ( is_label ? "right" : "left" ) << "\x60'\x60}\n";

            varsf << "\x60{\x60$f" << to_string( i + 1 )
             << "_data\x60=\x60'" << next_field << "\x60'\x60}\n";

            if( date_fields.count( next_field ) )
               varsf << "\x60{\x60$f" << to_string( i + 1 )
                << "_dtype\x60=\x60'date\x60'\x60}\n";
            else if( time_fields.count( next_field ) )
               varsf << "\x60{\x60$f" << to_string( i + 1 )
                << "_dtype\x60=\x60'time\x60'\x60}\n";
            else if( numeric_fields.count( next_field ) )
               varsf << "\x60{\x60$f" << to_string( i + 1 )
                << "_dtype\x60=\x60'numeric\x60'\x60}\n";
            else if( datetime_fields.count( next_field ) )
               varsf << "\x60{\x60$f" << to_string( i + 1 )
                << "_dtype\x60=\x60'datetime\x60'\x60}\n";

            if( prefixes.count( next_field ) )
            {
               string prefix_value = prefixes[ next_field ];

               if( prefix_value == "@special" )
                  prefix_value = extra_special;

               if( !prefix_value.empty( ) )
                  varsf << "\x60{\x60$f" << to_string( i + 1 )
                   << "_prefix\x60=\x60'" << prefix_value << "\x60'\x60}\n";
            }

            if( suffixes.count( next_field ) )
            {
               string suffix_value = suffixes[ next_field ];

               if( suffix_value == "@special" )
                  suffix_value = extra_special;

               if( !suffix_value.empty( ) )
                  varsf << "\x60{\x60$f" << to_string( i + 1 )
                   << "_suffix\x60=\x60'" << suffix_value << "\x60'\x60}\n";
            }
            else if( is_label && uom_fields.count( next_field ) )
               varsf << "\x60{\x60$f" << to_string( i + 1 )
                << "_suffix\x60=\x60' (" << uom_fields[ next_field ] << ")\x60'\x60}\n";

            if( !is_label )
            {
               if( !is_second_column && !wide_fields.count( next_field ) )
                  is_second_column = true;
               else
               {
                  vpos += 30;
                  is_second_column = false;
               }
            }
         }

         varsf.flush( );
         if( !varsf.good( ) )
            throw runtime_error( "'" + vars_filename + "' output stream is bad" );
      }
   }
   // [<finish Generate_PDF_View_impl>]
}

string Meta_View::impl::get_field_value( int field ) const
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
      retval = to_string( impl_Allow_Copy_Action( ) );
      break;

      case 3:
      retval = to_string( impl_Allow_Printable_Version( ) );
      break;

      case 4:
      retval = to_string( impl_Auto_Back_After_Save( ) );
      break;

      case 5:
      retval = to_string( impl_Change_Permission( ) );
      break;

      case 6:
      retval = to_string( impl_Change_Restriction( ) );
      break;

      case 7:
      retval = to_string( impl_Class( ) );
      break;

      case 8:
      retval = to_string( impl_File_Links_Always_As_Single( ) );
      break;

      case 9:
      retval = to_string( impl_Id( ) );
      break;

      case 10:
      retval = to_string( impl_Ignore_Parent_Record( ) );
      break;

      case 11:
      retval = to_string( impl_Ignore_Unactionable_Records( ) );
      break;

      case 12:
      retval = to_string( impl_Model( ) );
      break;

      case 13:
      retval = to_string( impl_Name( ) );
      break;

      case 14:
      retval = to_string( impl_PDF_Font_Type( ) );
      break;

      case 15:
      retval = to_string( impl_PDF_View_Type( ) );
      break;

      case 16:
      retval = to_string( impl_Print_Without_Highlight( ) );
      break;

      case 17:
      retval = to_string( impl_Title( ) );
      break;

      case 18:
      retval = to_string( impl_Type( ) );
      break;

      case 19:
      retval = to_string( impl_Type_Key( ) );
      break;

      case 20:
      retval = to_string( impl_Use_First_Row_As_Header( ) );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in get field value" );
   }

   return retval;
}

void Meta_View::impl::set_field_value( int field, const string& value )
{
   switch( field )
   {
      case 0:
      func_string_setter< Meta_View::impl, Meta_Permission >( *this, &Meta_View::impl::impl_Access_Permission, value );
      break;

      case 1:
      func_string_setter< Meta_View::impl, int >( *this, &Meta_View::impl::impl_Access_Restriction, value );
      break;

      case 2:
      func_string_setter< Meta_View::impl, bool >( *this, &Meta_View::impl::impl_Allow_Copy_Action, value );
      break;

      case 3:
      func_string_setter< Meta_View::impl, bool >( *this, &Meta_View::impl::impl_Allow_Printable_Version, value );
      break;

      case 4:
      func_string_setter< Meta_View::impl, bool >( *this, &Meta_View::impl::impl_Auto_Back_After_Save, value );
      break;

      case 5:
      func_string_setter< Meta_View::impl, Meta_Permission >( *this, &Meta_View::impl::impl_Change_Permission, value );
      break;

      case 6:
      func_string_setter< Meta_View::impl, int >( *this, &Meta_View::impl::impl_Change_Restriction, value );
      break;

      case 7:
      func_string_setter< Meta_View::impl, Meta_Class >( *this, &Meta_View::impl::impl_Class, value );
      break;

      case 8:
      func_string_setter< Meta_View::impl, bool >( *this, &Meta_View::impl::impl_File_Links_Always_As_Single, value );
      break;

      case 9:
      func_string_setter< Meta_View::impl, string >( *this, &Meta_View::impl::impl_Id, value );
      break;

      case 10:
      func_string_setter< Meta_View::impl, bool >( *this, &Meta_View::impl::impl_Ignore_Parent_Record, value );
      break;

      case 11:
      func_string_setter< Meta_View::impl, bool >( *this, &Meta_View::impl::impl_Ignore_Unactionable_Records, value );
      break;

      case 12:
      func_string_setter< Meta_View::impl, Meta_Model >( *this, &Meta_View::impl::impl_Model, value );
      break;

      case 13:
      func_string_setter< Meta_View::impl, string >( *this, &Meta_View::impl::impl_Name, value );
      break;

      case 14:
      func_string_setter< Meta_View::impl, int >( *this, &Meta_View::impl::impl_PDF_Font_Type, value );
      break;

      case 15:
      func_string_setter< Meta_View::impl, int >( *this, &Meta_View::impl::impl_PDF_View_Type, value );
      break;

      case 16:
      func_string_setter< Meta_View::impl, bool >( *this, &Meta_View::impl::impl_Print_Without_Highlight, value );
      break;

      case 17:
      func_string_setter< Meta_View::impl, string >( *this, &Meta_View::impl::impl_Title, value );
      break;

      case 18:
      func_string_setter< Meta_View::impl, Meta_View_Type >( *this, &Meta_View::impl::impl_Type, value );
      break;

      case 19:
      func_string_setter< Meta_View::impl, string >( *this, &Meta_View::impl::impl_Type_Key, value );
      break;

      case 20:
      func_string_setter< Meta_View::impl, bool >( *this, &Meta_View::impl::impl_Use_First_Row_As_Header, value );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in set field value" );
   }
}

void Meta_View::impl::set_field_default( int field )
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
      impl_Allow_Copy_Action( g_default_Allow_Copy_Action );
      break;

      case 3:
      impl_Allow_Printable_Version( g_default_Allow_Printable_Version );
      break;

      case 4:
      impl_Auto_Back_After_Save( g_default_Auto_Back_After_Save );
      break;

      case 5:
      impl_Change_Permission( g_default_Change_Permission );
      break;

      case 6:
      impl_Change_Restriction( g_default_Change_Restriction );
      break;

      case 7:
      impl_Class( g_default_Class );
      break;

      case 8:
      impl_File_Links_Always_As_Single( g_default_File_Links_Always_As_Single );
      break;

      case 9:
      impl_Id( g_default_Id );
      break;

      case 10:
      impl_Ignore_Parent_Record( g_default_Ignore_Parent_Record );
      break;

      case 11:
      impl_Ignore_Unactionable_Records( g_default_Ignore_Unactionable_Records );
      break;

      case 12:
      impl_Model( g_default_Model );
      break;

      case 13:
      impl_Name( g_default_Name );
      break;

      case 14:
      impl_PDF_Font_Type( g_default_PDF_Font_Type );
      break;

      case 15:
      impl_PDF_View_Type( g_default_PDF_View_Type );
      break;

      case 16:
      impl_Print_Without_Highlight( g_default_Print_Without_Highlight );
      break;

      case 17:
      impl_Title( g_default_Title );
      break;

      case 18:
      impl_Type( g_default_Type );
      break;

      case 19:
      impl_Type_Key( g_default_Type_Key );
      break;

      case 20:
      impl_Use_First_Row_As_Header( g_default_Use_First_Row_As_Header );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in set field default" );
   }
}

bool Meta_View::impl::is_field_default( int field ) const
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
      retval = ( v_Allow_Copy_Action == g_default_Allow_Copy_Action );
      break;

      case 3:
      retval = ( v_Allow_Printable_Version == g_default_Allow_Printable_Version );
      break;

      case 4:
      retval = ( v_Auto_Back_After_Save == g_default_Auto_Back_After_Save );
      break;

      case 5:
      retval = ( v_Change_Permission == g_default_Change_Permission );
      break;

      case 6:
      retval = ( v_Change_Restriction == g_default_Change_Restriction );
      break;

      case 7:
      retval = ( v_Class == g_default_Class );
      break;

      case 8:
      retval = ( v_File_Links_Always_As_Single == g_default_File_Links_Always_As_Single );
      break;

      case 9:
      retval = ( v_Id == g_default_Id );
      break;

      case 10:
      retval = ( v_Ignore_Parent_Record == g_default_Ignore_Parent_Record );
      break;

      case 11:
      retval = ( v_Ignore_Unactionable_Records == g_default_Ignore_Unactionable_Records );
      break;

      case 12:
      retval = ( v_Model == g_default_Model );
      break;

      case 13:
      retval = ( v_Name == g_default_Name );
      break;

      case 14:
      retval = ( v_PDF_Font_Type == g_default_PDF_Font_Type );
      break;

      case 15:
      retval = ( v_PDF_View_Type == g_default_PDF_View_Type );
      break;

      case 16:
      retval = ( v_Print_Without_Highlight == g_default_Print_Without_Highlight );
      break;

      case 17:
      retval = ( v_Title == g_default_Title );
      break;

      case 18:
      retval = ( v_Type == g_default_Type );
      break;

      case 19:
      retval = ( v_Type_Key == g_default_Type_Key );
      break;

      case 20:
      retval = ( v_Use_First_Row_As_Header == g_default_Use_First_Row_As_Header );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in is_field_default" );
   }

   return retval;
}

uint64_t Meta_View::impl::get_state( ) const
{
   uint64_t state = 0;

   // [(start modifier_field_value)] 600330
   if( get_obj( ).Type( ) == "print" )
      state |= c_modifier_Is_Print_Version;
   // [(finish modifier_field_value)] 600330

   // [(start modifier_field_value)] 600335
   if( get_obj( ).Type( ) != "print" )
      state |= c_modifier_Is_Not_Print_Version;
   // [(finish modifier_field_value)] 600335

   // [(start modifier_field_value)] 600340
   if( get_obj( ).Allow_Printable_Version( ) == false )
      state |= c_modifier_Printing_Disallowed;
   // [(finish modifier_field_value)] 600340

   // [(start modifier_field_value)] 600341
   if( get_obj( ).PDF_View_Type( ) == 0 )
      state |= c_modifier_PDF_View_Is_None;
   // [(finish modifier_field_value)] 600341

   // [(start modifier_field_value)] 600342
   if( get_obj( ).PDF_View_Type( ) == 99 )
      state |= c_modifier_PDF_View_Is_Custom;
   // [(finish modifier_field_value)] 600342

   // [<start get_state>]
//nyi
   if( get_obj( ).Class( ).Access_Restriction( ) != 0
    || !is_null( get_obj( ).Class( ).Access_Permission( ) ) )
      state |= c_modifier_Protect_Access;

   if( get_obj( ).Class( ).Change_Restriction( ) != 0
    || !is_null( get_obj( ).Class( ).Change_Permission( ) ) )
      state |= c_modifier_Protect_Change;
   // [<finish get_state>]

   return state;
}

string Meta_View::impl::get_state_names( ) const
{
   string state_names;
   uint64_t state = get_state( );

   if( state & c_modifier_Is_Not_Print_Version )
      state_names += "|" + string( "Is_Not_Print_Version" );
   if( state & c_modifier_Is_Print_Version )
      state_names += "|" + string( "Is_Print_Version" );
   if( state & c_modifier_PDF_View_Is_Custom )
      state_names += "|" + string( "PDF_View_Is_Custom" );
   if( state & c_modifier_PDF_View_Is_None )
      state_names += "|" + string( "PDF_View_Is_None" );
   if( state & c_modifier_Printing_Disallowed )
      state_names += "|" + string( "Printing_Disallowed" );
   if( state & c_modifier_Protect_Access )
      state_names += "|" + string( "Protect_Access" );
   if( state & c_modifier_Protect_Change )
      state_names += "|" + string( "Protect_Change" );

   return state_names.empty( ) ? state_names : state_names.substr( 1 );
}

const string& Meta_View::impl::execute( const string& cmd_and_args )
{
   execute_command( cmd_and_args );
   return retval;
}

void Meta_View::impl::clear_foreign_key( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( ( field == c_field_id_Access_Permission ) || ( field == c_field_name_Access_Permission ) )
      impl_Access_Permission( "" );
   else if( ( field == c_field_id_Change_Permission ) || ( field == c_field_name_Change_Permission ) )
      impl_Change_Permission( "" );
   else if( ( field == c_field_id_Class ) || ( field == c_field_name_Class ) )
      impl_Class( "" );
   else if( ( field == c_field_id_Model ) || ( field == c_field_name_Model ) )
      impl_Model( "" );
   else if( ( field == c_field_id_Type ) || ( field == c_field_name_Type ) )
      impl_Type( "" );
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_View::impl::set_foreign_key_value( const string& field, const string& value )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for value: " + value );
   else if( ( field == c_field_id_Access_Permission ) || ( field == c_field_name_Access_Permission ) )
      v_Access_Permission = value;
   else if( ( field == c_field_id_Change_Permission ) || ( field == c_field_name_Change_Permission ) )
      v_Change_Permission = value;
   else if( ( field == c_field_id_Class ) || ( field == c_field_name_Class ) )
      v_Class = value;
   else if( ( field == c_field_id_Model ) || ( field == c_field_name_Model ) )
      v_Model = value;
   else if( ( field == c_field_id_Type ) || ( field == c_field_name_Type ) )
      v_Type = value;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

const string& Meta_View::impl::get_foreign_key_value( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( ( field == c_field_id_Access_Permission ) || ( field == c_field_name_Access_Permission ) )
      return v_Access_Permission;
   else if( ( field == c_field_id_Change_Permission ) || ( field == c_field_name_Change_Permission ) )
      return v_Change_Permission;
   else if( ( field == c_field_id_Class ) || ( field == c_field_name_Class ) )
      return v_Class;
   else if( ( field == c_field_id_Model ) || ( field == c_field_name_Model ) )
      return v_Model;
   else if( ( field == c_field_id_Type ) || ( field == c_field_name_Type ) )
      return v_Type;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_View::impl::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Access_Permission, v_Access_Permission ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Change_Permission, v_Change_Permission ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Class, v_Class ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Model, v_Model ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Type, v_Type ) );
}

void Meta_View::impl::add_extra_fixed_info( vector< pair< string, string > >& fixed_info ) const
{
   ( void )fixed_info;

   // [<start add_extra_fixed_info>]
   // [<finish add_extra_fixed_info>]
}

void Meta_View::impl::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   ( void )paging_info;

   // [<start add_extra_paging_info>]
   // [<finish add_extra_paging_info>]
}

void Meta_View::impl::clear( )
{
   v_Access_Restriction = g_default_Access_Restriction;
   v_Allow_Copy_Action = g_default_Allow_Copy_Action;
   v_Allow_Printable_Version = g_default_Allow_Printable_Version;
   v_Auto_Back_After_Save = g_default_Auto_Back_After_Save;
   v_Change_Restriction = g_default_Change_Restriction;
   v_File_Links_Always_As_Single = g_default_File_Links_Always_As_Single;
   v_Id = g_default_Id;
   v_Ignore_Parent_Record = g_default_Ignore_Parent_Record;
   v_Ignore_Unactionable_Records = g_default_Ignore_Unactionable_Records;
   v_Name = g_default_Name;
   v_PDF_Font_Type = g_default_PDF_Font_Type;
   v_PDF_View_Type = g_default_PDF_View_Type;
   v_Print_Without_Highlight = g_default_Print_Without_Highlight;
   v_Title = g_default_Title;
   v_Type_Key = g_default_Type_Key;
   v_Use_First_Row_As_Header = g_default_Use_First_Row_As_Header;

   v_Access_Permission = string( );
   if( cp_Access_Permission )
      p_obj->setup_foreign_key( *cp_Access_Permission, v_Access_Permission );

   v_Change_Permission = string( );
   if( cp_Change_Permission )
      p_obj->setup_foreign_key( *cp_Change_Permission, v_Change_Permission );

   v_Class = string( );
   if( cp_Class )
      p_obj->setup_foreign_key( *cp_Class, v_Class );

   v_Model = string( );
   if( cp_Model )
      p_obj->setup_foreign_key( *cp_Model, v_Model );

   v_Type = string( );
   if( cp_Type )
      p_obj->setup_foreign_key( *cp_Type, v_Type );
}

bool Meta_View::impl::value_will_be_provided( const string& field_name )
{
   ( void )field_name;

   // [(start parent_auto_int_inc)] 600315
   if( field_name == "Id" )
      return true;
   // [(finish parent_auto_int_inc)] 600315

   // [<start value_will_be_provided>]
   // [<finish value_will_be_provided>]

   return false;
}

void Meta_View::impl::validate(
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
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Id ) ) ) ) );

   if( is_null( v_Title ) && !value_will_be_provided( c_field_name_Title ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Title,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Title ) ) ) ) );

   if( is_null( v_Type_Key ) && !value_will_be_provided( c_field_name_Type_Key ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Type_Key,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Type_Key ) ) ) ) );

   if( v_Class.empty( ) && !value_will_be_provided( c_field_name_Class ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Class,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Class ) ) ) ) );

   if( v_Model.empty( ) && !value_will_be_provided( c_field_name_Model ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Model,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Model ) ) ) ) );

   if( v_Type.empty( ) && !value_will_be_provided( c_field_name_Type ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Type,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Type ) ) ) ) );

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

   if( !is_null( v_Title )
    && ( v_Title != g_default_Title
    || !value_will_be_provided( c_field_name_Title ) )
    && !g_Title_domain.is_valid( v_Title, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Title,
       get_module_string( c_field_display_name_Title ) + " " + error_message ) );

   if( !g_view_access_restrict_enum.count( v_Access_Restriction ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Access_Restriction,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Access_Restriction ) ) ) ) );

   if( !g_view_change_restrict_enum.count( v_Change_Restriction ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Change_Restriction,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_Change_Restriction ) ) ) ) );

   if( !g_pdf_font_type_enum.count( v_PDF_Font_Type ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_PDF_Font_Type,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_PDF_Font_Type ) ) ) ) );

   if( !g_view_pdf_view_type_enum.count( v_PDF_View_Type ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_PDF_View_Type,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_parm_field_has_invalid_value_field, get_module_string( c_field_display_name_PDF_View_Type ) ) ) ) );

   // [<start validate>]
   // [<finish validate>]
}

void Meta_View::impl::validate_set_fields(
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

   if( !is_null( v_Title )
    && ( fields_set.count( c_field_id_Title ) || fields_set.count( c_field_name_Title ) )
    && !g_Title_domain.is_valid( v_Title, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Title,
       get_module_string( c_field_display_name_Title ) + " " + error_message ) );
}

void Meta_View::impl::after_fetch( )
{
   if( !get_obj( ).get_is_iterating( ) || get_obj( ).get_is_starting_iteration( ) )
      get_required_transients( );

   if( cp_Access_Permission )
      p_obj->setup_foreign_key( *cp_Access_Permission, v_Access_Permission );

   if( cp_Change_Permission )
      p_obj->setup_foreign_key( *cp_Change_Permission, v_Change_Permission );

   if( cp_Class )
      p_obj->setup_foreign_key( *cp_Class, v_Class );

   if( cp_Model )
      p_obj->setup_foreign_key( *cp_Model, v_Model );

   if( cp_Type )
      p_obj->setup_foreign_key( *cp_Type, v_Type );

   post_init( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start field_from_search_replace)] 600320
   if( !get_obj( ).get_key( ).empty( )
    && !get_obj( ).get_is_for_peer( )
    && ( get_obj( ).needs_field_value( "Name" )
    || required_transients.count( "Name" ) ) )
   {
      string str( get_obj( ).Type( ).View_Name( ) );

      get_obj( ).Name( str );

      get_obj( ).add_search_replacement( "Name", "{class}", to_rep_string( get_obj( ).Class( ).Name( ) ) );

      get_obj( ).set_search_replace_separator( "Name", '_' );
   }
   // [(finish field_from_search_replace)] 600320

   // [<start after_fetch>]
//nyi
   if( get_obj( ).Class( ).Access_Restriction( ) != 0 )
      get_obj( ).Access_Restriction( get_obj( ).Class( ).Access_Restriction( ) );

   if( get_obj( ).Class( ).Change_Restriction( ) != 0 )
      get_obj( ).Change_Restriction( get_obj( ).Class( ).Change_Restriction( ) );

   if( !is_null( get_obj( ).Class( ).Access_Permission( ) ) )
      get_obj( ).Access_Permission( get_obj( ).Class( ).Access_Permission( ) );

   if( !is_null( get_obj( ).Class( ).Change_Permission( ) ) )
      get_obj( ).Change_Permission( get_obj( ).Class( ).Change_Permission( ) );
   // [<finish after_fetch>]
}

void Meta_View::impl::finalise_fetch( bool skip_set_original )
{
   if( !skip_set_original && !get_obj( ).get_key( ).empty( ) )
      get_obj( ).set_new_original_values( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start finalise_fetch>]
   // [<finish finalise_fetch>]
}

void Meta_View::impl::at_create( )
{
   // [(start fk_default)] 600300
   if( is_null( get_obj( ).Type( ) ) )
      get_obj( ).Type( "normal" );
   // [(finish fk_default)] 600300

   // [<start at_create>]
   // [<finish at_create>]
}

void Meta_View::impl::post_init( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start post_init>]
   // [<finish post_init>]
}

void Meta_View::impl::to_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   if( !get_obj( ).get_is_preparing( ) )
      post_init( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start default_to_field)] 600310
   if( ( is_create || get_obj( ).get_key( ).empty( ) )
    && get_obj( ).get_clone_key( ).empty( )
    && ( get_obj( ).Name( ) == g_default_Name ) )
      get_obj( ).Name( get_obj( ).Type( ).View_Name( ) );
   // [(finish default_to_field)] 600310

   // [(start field_from_other_field)] 600311
   get_obj( ).Type_Key( get_obj( ).Type( ) );
   // [(finish field_from_other_field)] 600311

   // [<start to_store>]
   // [<finish to_store>]
}

void Meta_View::impl::for_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start parent_auto_int_inc)] 600315
   if( is_create && is_null( get_obj( ).Id( ) ) )
   {
      if( is_null( get_obj( ).Model( ) ) )
         throw runtime_error( "unexpected empty Model" );

      get_obj( ).Model( ).op_update( get_obj( ).Model( ), FIELD_NAME( Meta, Model, Next_View_Id ) );

      get_obj( ).Id( get_obj( ).Model( ).Next_View_Id( ) );

      get_obj( ).Model( ).Next_View_Id( auto_int_increment( get_obj( ).Model( ).Next_View_Id( ) ) );
      get_obj( ).Model( ).op_apply( );
   }
   // [(finish parent_auto_int_inc)] 600315

   // [<start for_store>]
   // [<finish for_store>]
}

void Meta_View::impl::after_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_store>]
//nyi
   // NOTE: Due to being transient detecting a duplicate Name can
   // only done after the record is stored (as its value is given
   // to it in after_fetch).
   if( !session_skip_validation( ) )
   {
      class_pointer< Meta_Model > cp_parent( e_create_instance );

      if( !get_obj( ).Model( ).get_key( ).empty( ) )
      {
         cp_parent->perform_fetch( get_obj( ).Model( ).get_key( ) );

         if( cp_parent->child_View( ).iterate_forwards( ) )
         {
            set< string > names;
            do
            {
               if( names.count( cp_parent->child_View( ).Name( ) ) )
                  throw runtime_error( "This View has already been created." );

               names.insert( cp_parent->child_View( ).Name( ) );

            } while( cp_parent->child_View( ).iterate_next( ) );
         }
      }
   }
   // [<finish after_store>]
}

bool Meta_View::impl::can_destroy( bool is_internal )
{
   uint64_t state = p_obj->get_state( );

   bool retval = ( is_internal || !( state & c_state_undeletable ) );

   // [<start can_destroy>]
   // [<finish can_destroy>]

   return retval;
}

void Meta_View::impl::for_destroy( bool is_internal )
{
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start for_destroy>]
   // [<finish for_destroy>]
}

void Meta_View::impl::after_destroy( bool is_internal )
{
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_destroy>]
   // [<finish after_destroy>]
}

void Meta_View::impl::set_default_values( )
{
   clear( );
}

bool Meta_View::impl::is_filtered( ) const
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start is_filtered>]
   // [<finish is_filtered>]

   return false;
}

void Meta_View::impl::get_required_transients( ) const
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

Meta_View::Meta_View( )
{
   set_version( c_version );

   p_impl = new impl( *this );
}

Meta_View::~Meta_View( )
{
   cleanup( );
   delete p_impl;
}

int Meta_View::Access_Restriction( ) const
{
   return p_impl->impl_Access_Restriction( );
}

void Meta_View::Access_Restriction( int Access_Restriction )
{
   p_impl->impl_Access_Restriction( Access_Restriction );
}

bool Meta_View::Allow_Copy_Action( ) const
{
   return p_impl->impl_Allow_Copy_Action( );
}

void Meta_View::Allow_Copy_Action( bool Allow_Copy_Action )
{
   p_impl->impl_Allow_Copy_Action( Allow_Copy_Action );
}

bool Meta_View::Allow_Printable_Version( ) const
{
   return p_impl->impl_Allow_Printable_Version( );
}

void Meta_View::Allow_Printable_Version( bool Allow_Printable_Version )
{
   p_impl->impl_Allow_Printable_Version( Allow_Printable_Version );
}

bool Meta_View::Auto_Back_After_Save( ) const
{
   return p_impl->impl_Auto_Back_After_Save( );
}

void Meta_View::Auto_Back_After_Save( bool Auto_Back_After_Save )
{
   p_impl->impl_Auto_Back_After_Save( Auto_Back_After_Save );
}

int Meta_View::Change_Restriction( ) const
{
   return p_impl->impl_Change_Restriction( );
}

void Meta_View::Change_Restriction( int Change_Restriction )
{
   p_impl->impl_Change_Restriction( Change_Restriction );
}

bool Meta_View::File_Links_Always_As_Single( ) const
{
   return p_impl->impl_File_Links_Always_As_Single( );
}

void Meta_View::File_Links_Always_As_Single( bool File_Links_Always_As_Single )
{
   p_impl->impl_File_Links_Always_As_Single( File_Links_Always_As_Single );
}

const string& Meta_View::Id( ) const
{
   return p_impl->impl_Id( );
}

void Meta_View::Id( const string& Id )
{
   p_impl->impl_Id( Id );
}

bool Meta_View::Ignore_Parent_Record( ) const
{
   return p_impl->impl_Ignore_Parent_Record( );
}

void Meta_View::Ignore_Parent_Record( bool Ignore_Parent_Record )
{
   p_impl->impl_Ignore_Parent_Record( Ignore_Parent_Record );
}

bool Meta_View::Ignore_Unactionable_Records( ) const
{
   return p_impl->impl_Ignore_Unactionable_Records( );
}

void Meta_View::Ignore_Unactionable_Records( bool Ignore_Unactionable_Records )
{
   p_impl->impl_Ignore_Unactionable_Records( Ignore_Unactionable_Records );
}

const string& Meta_View::Name( ) const
{
   return p_impl->impl_Name( );
}

void Meta_View::Name( const string& Name )
{
   p_impl->impl_Name( Name );
}

int Meta_View::PDF_Font_Type( ) const
{
   return p_impl->impl_PDF_Font_Type( );
}

void Meta_View::PDF_Font_Type( int PDF_Font_Type )
{
   p_impl->impl_PDF_Font_Type( PDF_Font_Type );
}

int Meta_View::PDF_View_Type( ) const
{
   return p_impl->impl_PDF_View_Type( );
}

void Meta_View::PDF_View_Type( int PDF_View_Type )
{
   p_impl->impl_PDF_View_Type( PDF_View_Type );
}

bool Meta_View::Print_Without_Highlight( ) const
{
   return p_impl->impl_Print_Without_Highlight( );
}

void Meta_View::Print_Without_Highlight( bool Print_Without_Highlight )
{
   p_impl->impl_Print_Without_Highlight( Print_Without_Highlight );
}

const string& Meta_View::Title( ) const
{
   return p_impl->impl_Title( );
}

void Meta_View::Title( const string& Title )
{
   p_impl->impl_Title( Title );
}

const string& Meta_View::Type_Key( ) const
{
   return p_impl->impl_Type_Key( );
}

void Meta_View::Type_Key( const string& Type_Key )
{
   p_impl->impl_Type_Key( Type_Key );
}

bool Meta_View::Use_First_Row_As_Header( ) const
{
   return p_impl->impl_Use_First_Row_As_Header( );
}

void Meta_View::Use_First_Row_As_Header( bool Use_First_Row_As_Header )
{
   p_impl->impl_Use_First_Row_As_Header( Use_First_Row_As_Header );
}

Meta_Permission& Meta_View::Access_Permission( )
{
   return p_impl->impl_Access_Permission( );
}

const Meta_Permission& Meta_View::Access_Permission( ) const
{
   return p_impl->impl_Access_Permission( );
}

void Meta_View::Access_Permission( const string& key )
{
   p_impl->impl_Access_Permission( key );
}

Meta_Permission& Meta_View::Change_Permission( )
{
   return p_impl->impl_Change_Permission( );
}

const Meta_Permission& Meta_View::Change_Permission( ) const
{
   return p_impl->impl_Change_Permission( );
}

void Meta_View::Change_Permission( const string& key )
{
   p_impl->impl_Change_Permission( key );
}

Meta_Class& Meta_View::Class( )
{
   return p_impl->impl_Class( );
}

const Meta_Class& Meta_View::Class( ) const
{
   return p_impl->impl_Class( );
}

void Meta_View::Class( const string& key )
{
   p_impl->impl_Class( key );
}

Meta_Model& Meta_View::Model( )
{
   return p_impl->impl_Model( );
}

const Meta_Model& Meta_View::Model( ) const
{
   return p_impl->impl_Model( );
}

void Meta_View::Model( const string& key )
{
   p_impl->impl_Model( key );
}

Meta_View_Type& Meta_View::Type( )
{
   return p_impl->impl_Type( );
}

const Meta_View_Type& Meta_View::Type( ) const
{
   return p_impl->impl_Type( );
}

void Meta_View::Type( const string& key )
{
   p_impl->impl_Type( key );
}

Meta_Class& Meta_View::child_Class_Created( )
{
   return p_impl->impl_child_Class_Created( );
}

const Meta_Class& Meta_View::child_Class_Created( ) const
{
   return p_impl->impl_child_Class_Created( );
}

Meta_Package_Option& Meta_View::child_Package_Option( )
{
   return p_impl->impl_child_Package_Option( );
}

const Meta_Package_Option& Meta_View::child_Package_Option( ) const
{
   return p_impl->impl_child_Package_Option( );
}

Meta_View_Field& Meta_View::child_View_Field( )
{
   return p_impl->impl_child_View_Field( );
}

const Meta_View_Field& Meta_View::child_View_Field( ) const
{
   return p_impl->impl_child_View_Field( );
}

void Meta_View::Generate_PDF_View( )
{
   p_impl->impl_Generate_PDF_View( );
}

string Meta_View::get_field_value( int field ) const
{
   return p_impl->get_field_value( field );
}

void Meta_View::set_field_value( int field, const string& value )
{
   p_impl->set_field_value( field, value );
}

void Meta_View::set_field_default( int field )
{
   return set_field_default( ( field_id )( field + 1 ) );
}

void Meta_View::set_field_default( field_id id )
{
   p_impl->set_field_default( ( int )id - 1 );
}

void Meta_View::set_field_default( const string& field )
{
   p_impl->set_field_default( get_field_num( field ) );
}

bool Meta_View::is_field_default( int field ) const
{
   return is_field_default( ( field_id )( field + 1 ) );
}

bool Meta_View::is_field_default( field_id id ) const
{
   return p_impl->is_field_default( ( int )id - 1 );
}

bool Meta_View::is_field_default( const string& field ) const
{
   return p_impl->is_field_default( get_field_num( field ) );
}

bool Meta_View::is_field_encrypted( int field ) const
{
   return static_is_field_encrypted( ( field_id )( field + 1 ) );
}

bool Meta_View::is_field_transient( int field ) const
{
   return static_is_field_transient( ( field_id )( field + 1 ) );
}

string Meta_View::get_field_id( int field ) const
{
   return static_get_field_id( ( field_id )( field + 1 ) );
}

string Meta_View::get_field_name( int field ) const
{
   return static_get_field_name( ( field_id )( field + 1 ) );
}

int Meta_View::get_field_num( const string& field ) const
{
   int rc = static_get_field_num( field );

   if( rc < 0 )
      throw runtime_error( "unknown field name/id '" + field + "' in get_field_num( )" );

   return rc;
}

bool Meta_View::has_field_changed( const string& field ) const
{
   return class_base::has_field_changed( get_field_num( field ) );
}

uint64_t Meta_View::get_state( ) const
{
   uint64_t state = 0;

   state |= p_impl->get_state( );

   return state;
}

const string& Meta_View::execute( const string& cmd_and_args )
{
   return p_impl->execute( cmd_and_args );
}

void Meta_View::clear( )
{
   p_impl->clear( );
}

void Meta_View::validate( uint64_t state, bool is_internal )
{
   p_impl->validate( state, is_internal, &validation_errors );
}

void Meta_View::validate_set_fields( set< string >& fields_set )
{
   p_impl->validate_set_fields( fields_set, &validation_errors );
}

void Meta_View::after_fetch( )
{
   p_impl->after_fetch( );
}

void Meta_View::finalise_fetch( bool skip_set_original )
{
   p_impl->finalise_fetch( skip_set_original );
}

void Meta_View::at_create( )
{
   p_impl->at_create( );
}

void Meta_View::post_init( )
{
   p_impl->post_init( );
}

void Meta_View::to_store( bool is_create, bool is_internal )
{
   p_impl->to_store( is_create, is_internal );
}

void Meta_View::for_store( bool is_create, bool is_internal )
{
   p_impl->for_store( is_create, is_internal );
}

void Meta_View::after_store( bool is_create, bool is_internal )
{
   p_impl->after_store( is_create, is_internal );
}

bool Meta_View::can_destroy( bool is_internal )
{
   return p_impl->can_destroy( is_internal );
}

void Meta_View::for_destroy( bool is_internal )
{
   p_impl->for_destroy( is_internal );
}

void Meta_View::after_destroy( bool is_internal )
{
   p_impl->after_destroy( is_internal );
}

void Meta_View::set_default_values( )
{
   p_impl->set_default_values( );
}

bool Meta_View::is_filtered( ) const
{
   return p_impl->is_filtered( );
}

const char* Meta_View::get_field_id(
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
   else if( name == c_field_name_Allow_Copy_Action )
   {
      p_id = c_field_id_Allow_Copy_Action;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Allow_Printable_Version )
   {
      p_id = c_field_id_Allow_Printable_Version;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Auto_Back_After_Save )
   {
      p_id = c_field_id_Auto_Back_After_Save;

      if( p_type_name )
         *p_type_name = "bool";

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
   else if( name == c_field_name_Class )
   {
      p_id = c_field_id_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_File_Links_Always_As_Single )
   {
      p_id = c_field_id_File_Links_Always_As_Single;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Id )
   {
      p_id = c_field_id_Id;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Ignore_Parent_Record )
   {
      p_id = c_field_id_Ignore_Parent_Record;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Ignore_Unactionable_Records )
   {
      p_id = c_field_id_Ignore_Unactionable_Records;

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
   else if( name == c_field_name_PDF_Font_Type )
   {
      p_id = c_field_id_PDF_Font_Type;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_PDF_View_Type )
   {
      p_id = c_field_id_PDF_View_Type;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Print_Without_Highlight )
   {
      p_id = c_field_id_Print_Without_Highlight;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Title )
   {
      p_id = c_field_id_Title;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Type )
   {
      p_id = c_field_id_Type;

      if( p_type_name )
         *p_type_name = "Meta_View_Type";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Type_Key )
   {
      p_id = c_field_id_Type_Key;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Use_First_Row_As_Header )
   {
      p_id = c_field_id_Use_First_Row_As_Header;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }

   return p_id;
}

const char* Meta_View::get_field_name(
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
   else if( id == c_field_id_Allow_Copy_Action )
   {
      p_name = c_field_name_Allow_Copy_Action;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Allow_Printable_Version )
   {
      p_name = c_field_name_Allow_Printable_Version;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Auto_Back_After_Save )
   {
      p_name = c_field_name_Auto_Back_After_Save;

      if( p_type_name )
         *p_type_name = "bool";

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
   else if( id == c_field_id_Class )
   {
      p_name = c_field_name_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_File_Links_Always_As_Single )
   {
      p_name = c_field_name_File_Links_Always_As_Single;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Id )
   {
      p_name = c_field_name_Id;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Ignore_Parent_Record )
   {
      p_name = c_field_name_Ignore_Parent_Record;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Ignore_Unactionable_Records )
   {
      p_name = c_field_name_Ignore_Unactionable_Records;

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
   else if( id == c_field_id_PDF_Font_Type )
   {
      p_name = c_field_name_PDF_Font_Type;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_PDF_View_Type )
   {
      p_name = c_field_name_PDF_View_Type;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Print_Without_Highlight )
   {
      p_name = c_field_name_Print_Without_Highlight;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Title )
   {
      p_name = c_field_name_Title;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Type )
   {
      p_name = c_field_name_Type;

      if( p_type_name )
         *p_type_name = "Meta_View_Type";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Type_Key )
   {
      p_name = c_field_name_Type_Key;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Use_First_Row_As_Header )
   {
      p_name = c_field_name_Use_First_Row_As_Header;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }

   return p_name;
}

string& Meta_View::get_group_field_name( ) const
{
   return g_group_field_name;
}

string& Meta_View::get_level_field_name( ) const
{
   return g_level_field_name;
}

string& Meta_View::get_order_field_name( ) const
{
   return g_order_field_name;
}

string& Meta_View::get_owner_field_name( ) const
{
   return g_owner_field_name;
}

bool Meta_View::is_file_field( const string& id_or_name ) const
{
   return g_file_field_ids.count( id_or_name ) || g_file_field_names.count( id_or_name );
}

void Meta_View::get_file_field_names( vector< string >& file_field_names ) const
{
   for( set< string >::const_iterator ci = g_file_field_names.begin( ); ci != g_file_field_names.end( ); ++ci )
      file_field_names.push_back( *ci );
}

string Meta_View::get_field_uom_symbol( const string& id_or_name ) const
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
   if( ( id_or_name == c_field_id_Allow_Copy_Action ) || ( id_or_name == c_field_name_Allow_Copy_Action ) )
   {
      name = string( c_field_display_name_Allow_Copy_Action );
      get_module_string( c_field_display_name_Allow_Copy_Action, &next );
   }
   if( ( id_or_name == c_field_id_Allow_Printable_Version ) || ( id_or_name == c_field_name_Allow_Printable_Version ) )
   {
      name = string( c_field_display_name_Allow_Printable_Version );
      get_module_string( c_field_display_name_Allow_Printable_Version, &next );
   }
   if( ( id_or_name == c_field_id_Auto_Back_After_Save ) || ( id_or_name == c_field_name_Auto_Back_After_Save ) )
   {
      name = string( c_field_display_name_Auto_Back_After_Save );
      get_module_string( c_field_display_name_Auto_Back_After_Save, &next );
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
   if( ( id_or_name == c_field_id_Class ) || ( id_or_name == c_field_name_Class ) )
   {
      name = string( c_field_display_name_Class );
      get_module_string( c_field_display_name_Class, &next );
   }
   if( ( id_or_name == c_field_id_File_Links_Always_As_Single ) || ( id_or_name == c_field_name_File_Links_Always_As_Single ) )
   {
      name = string( c_field_display_name_File_Links_Always_As_Single );
      get_module_string( c_field_display_name_File_Links_Always_As_Single, &next );
   }
   if( ( id_or_name == c_field_id_Id ) || ( id_or_name == c_field_name_Id ) )
   {
      name = string( c_field_display_name_Id );
      get_module_string( c_field_display_name_Id, &next );
   }
   if( ( id_or_name == c_field_id_Ignore_Parent_Record ) || ( id_or_name == c_field_name_Ignore_Parent_Record ) )
   {
      name = string( c_field_display_name_Ignore_Parent_Record );
      get_module_string( c_field_display_name_Ignore_Parent_Record, &next );
   }
   if( ( id_or_name == c_field_id_Ignore_Unactionable_Records ) || ( id_or_name == c_field_name_Ignore_Unactionable_Records ) )
   {
      name = string( c_field_display_name_Ignore_Unactionable_Records );
      get_module_string( c_field_display_name_Ignore_Unactionable_Records, &next );
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
   if( ( id_or_name == c_field_id_PDF_Font_Type ) || ( id_or_name == c_field_name_PDF_Font_Type ) )
   {
      name = string( c_field_display_name_PDF_Font_Type );
      get_module_string( c_field_display_name_PDF_Font_Type, &next );
   }
   if( ( id_or_name == c_field_id_PDF_View_Type ) || ( id_or_name == c_field_name_PDF_View_Type ) )
   {
      name = string( c_field_display_name_PDF_View_Type );
      get_module_string( c_field_display_name_PDF_View_Type, &next );
   }
   if( ( id_or_name == c_field_id_Print_Without_Highlight ) || ( id_or_name == c_field_name_Print_Without_Highlight ) )
   {
      name = string( c_field_display_name_Print_Without_Highlight );
      get_module_string( c_field_display_name_Print_Without_Highlight, &next );
   }
   if( ( id_or_name == c_field_id_Title ) || ( id_or_name == c_field_name_Title ) )
   {
      name = string( c_field_display_name_Title );
      get_module_string( c_field_display_name_Title, &next );
   }
   if( ( id_or_name == c_field_id_Type ) || ( id_or_name == c_field_name_Type ) )
   {
      name = string( c_field_display_name_Type );
      get_module_string( c_field_display_name_Type, &next );
   }
   if( ( id_or_name == c_field_id_Type_Key ) || ( id_or_name == c_field_name_Type_Key ) )
   {
      name = string( c_field_display_name_Type_Key );
      get_module_string( c_field_display_name_Type_Key, &next );
   }
   if( ( id_or_name == c_field_id_Use_First_Row_As_Header ) || ( id_or_name == c_field_name_Use_First_Row_As_Header ) )
   {
      name = string( c_field_display_name_Use_First_Row_As_Header );
      get_module_string( c_field_display_name_Use_First_Row_As_Header, &next );
   }

   // NOTE: It is being assumed here that the customised UOM symbol for a field (if it
   // has one) will be in the module string that immediately follows that of its name.
   if( next.first.find( name + "_(" ) == 0 )
      uom_symbol = next.second;

   return uom_symbol;
}

string Meta_View::get_field_display_name( const string& id_or_name ) const
{
   string display_name;

   if( id_or_name.empty( ) )
      throw runtime_error( "unexpected empty field id_or_name for get_field_display_name" );
   if( ( id_or_name == c_field_id_Access_Permission ) || ( id_or_name == c_field_name_Access_Permission ) )
      display_name = get_module_string( c_field_display_name_Access_Permission );
   if( ( id_or_name == c_field_id_Access_Restriction ) || ( id_or_name == c_field_name_Access_Restriction ) )
      display_name = get_module_string( c_field_display_name_Access_Restriction );
   if( ( id_or_name == c_field_id_Allow_Copy_Action ) || ( id_or_name == c_field_name_Allow_Copy_Action ) )
      display_name = get_module_string( c_field_display_name_Allow_Copy_Action );
   if( ( id_or_name == c_field_id_Allow_Printable_Version ) || ( id_or_name == c_field_name_Allow_Printable_Version ) )
      display_name = get_module_string( c_field_display_name_Allow_Printable_Version );
   if( ( id_or_name == c_field_id_Auto_Back_After_Save ) || ( id_or_name == c_field_name_Auto_Back_After_Save ) )
      display_name = get_module_string( c_field_display_name_Auto_Back_After_Save );
   if( ( id_or_name == c_field_id_Change_Permission ) || ( id_or_name == c_field_name_Change_Permission ) )
      display_name = get_module_string( c_field_display_name_Change_Permission );
   if( ( id_or_name == c_field_id_Change_Restriction ) || ( id_or_name == c_field_name_Change_Restriction ) )
      display_name = get_module_string( c_field_display_name_Change_Restriction );
   if( ( id_or_name == c_field_id_Class ) || ( id_or_name == c_field_name_Class ) )
      display_name = get_module_string( c_field_display_name_Class );
   if( ( id_or_name == c_field_id_File_Links_Always_As_Single ) || ( id_or_name == c_field_name_File_Links_Always_As_Single ) )
      display_name = get_module_string( c_field_display_name_File_Links_Always_As_Single );
   if( ( id_or_name == c_field_id_Id ) || ( id_or_name == c_field_name_Id ) )
      display_name = get_module_string( c_field_display_name_Id );
   if( ( id_or_name == c_field_id_Ignore_Parent_Record ) || ( id_or_name == c_field_name_Ignore_Parent_Record ) )
      display_name = get_module_string( c_field_display_name_Ignore_Parent_Record );
   if( ( id_or_name == c_field_id_Ignore_Unactionable_Records ) || ( id_or_name == c_field_name_Ignore_Unactionable_Records ) )
      display_name = get_module_string( c_field_display_name_Ignore_Unactionable_Records );
   if( ( id_or_name == c_field_id_Model ) || ( id_or_name == c_field_name_Model ) )
      display_name = get_module_string( c_field_display_name_Model );
   if( ( id_or_name == c_field_id_Name ) || ( id_or_name == c_field_name_Name ) )
      display_name = get_module_string( c_field_display_name_Name );
   if( ( id_or_name == c_field_id_PDF_Font_Type ) || ( id_or_name == c_field_name_PDF_Font_Type ) )
      display_name = get_module_string( c_field_display_name_PDF_Font_Type );
   if( ( id_or_name == c_field_id_PDF_View_Type ) || ( id_or_name == c_field_name_PDF_View_Type ) )
      display_name = get_module_string( c_field_display_name_PDF_View_Type );
   if( ( id_or_name == c_field_id_Print_Without_Highlight ) || ( id_or_name == c_field_name_Print_Without_Highlight ) )
      display_name = get_module_string( c_field_display_name_Print_Without_Highlight );
   if( ( id_or_name == c_field_id_Title ) || ( id_or_name == c_field_name_Title ) )
      display_name = get_module_string( c_field_display_name_Title );
   if( ( id_or_name == c_field_id_Type ) || ( id_or_name == c_field_name_Type ) )
      display_name = get_module_string( c_field_display_name_Type );
   if( ( id_or_name == c_field_id_Type_Key ) || ( id_or_name == c_field_name_Type_Key ) )
      display_name = get_module_string( c_field_display_name_Type_Key );
   if( ( id_or_name == c_field_id_Use_First_Row_As_Header ) || ( id_or_name == c_field_name_Use_First_Row_As_Header ) )
      display_name = get_module_string( c_field_display_name_Use_First_Row_As_Header );

   return display_name;
}

void Meta_View::clear_foreign_key( const string& field )
{
   p_impl->clear_foreign_key( field );
}

void Meta_View::set_foreign_key_value( const string& field, const string& value )
{
   p_impl->set_foreign_key_value( field, value );
}

const string& Meta_View::get_foreign_key_value( const string& field )
{
   return p_impl->get_foreign_key_value( field );
}

void Meta_View::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   p_impl->get_foreign_key_values( foreign_key_values );
}

void Meta_View::setup_foreign_key( Meta_Permission& o, const string& value )
{
   static_cast< Meta_Permission& >( o ).set_key( value );
}

void Meta_View::setup_foreign_key( Meta_Class& o, const string& value )
{
   static_cast< Meta_Class& >( o ).set_key( value );
}

void Meta_View::setup_foreign_key( Meta_Model& o, const string& value )
{
   static_cast< Meta_Model& >( o ).set_key( value );
}

void Meta_View::setup_foreign_key( Meta_View_Type& o, const string& value )
{
   static_cast< Meta_View_Type& >( o ).set_key( value );
}

void Meta_View::setup_graph_parent( Meta_Class& o, const string& foreign_key_field )
{
   static_cast< Meta_Class& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_View::setup_graph_parent( Meta_Package_Option& o, const string& foreign_key_field )
{
   static_cast< Meta_Package_Option& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_View::setup_graph_parent( Meta_View_Field& o, const string& foreign_key_field )
{
   static_cast< Meta_View_Field& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_View::setup_graph_parent(
 Meta_Permission& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Permission& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Permission& >( o ).set_key( init_value );
}

void Meta_View::setup_graph_parent(
 Meta_Class& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Class& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Class& >( o ).set_key( init_value );
}

void Meta_View::setup_graph_parent(
 Meta_Model& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Model& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Model& >( o ).set_key( init_value );
}

void Meta_View::setup_graph_parent(
 Meta_View_Type& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_View_Type& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_View_Type& >( o ).set_key( init_value );
}

size_t Meta_View::get_total_child_relationships( ) const
{
   return p_impl->total_child_relationships;
}

void Meta_View::set_total_child_relationships( size_t new_total_child_relationships ) const
{
   p_impl->total_child_relationships = new_total_child_relationships;
}

size_t Meta_View::get_num_foreign_key_children( bool is_internal ) const
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

class_base* Meta_View::get_next_foreign_key_child(
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
            next_child_field = "300631";
            p_class_base = &child_Class_Created( );
         }
         break;

         case 1:
         if( op == e_cascade_op_unlink )
         {
            next_child_field = "302830c";
            p_class_base = &child_Package_Option( );
         }
         break;

         case 2:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "301900";
            p_class_base = &child_View_Field( );
         }
         break;
      }
   }

   return p_class_base;
}

void Meta_View::add_extra_fixed_info( vector< pair< string, string > >& fixed_info ) const
{
   p_impl->add_extra_fixed_info( fixed_info );
}

void Meta_View::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   p_impl->add_extra_paging_info( paging_info );
}

string Meta_View::get_class_id( ) const
{
   return static_class_id( );
}

string Meta_View::get_class_name( ) const
{
   return static_class_name( );
}

string Meta_View::get_plural_name( ) const
{
   return static_plural_name( );
}

string Meta_View::get_module_id( ) const
{
   return static_module_id( );
}

string Meta_View::get_module_name( ) const
{
   return static_module_name( );
}

string Meta_View::get_display_name( bool plural ) const
{
   string key( plural ? "plural_" : "class_" );

   key += "view";

   return get_module_string( key );
}

string Meta_View::get_raw_variable( const std::string& name ) const
{
   if( name == g_state_names_variable )
      return p_impl->get_state_names( );
   else
      return class_base::get_raw_variable( name );
}

string Meta_View::get_create_instance_info( ) const
{
   return "";
}

string Meta_View::get_update_instance_info( ) const
{
   return "";
}

string Meta_View::get_destroy_instance_info( ) const
{
   return "";
}

string Meta_View::get_execute_procedure_info( const string& procedure_id ) const
{
   string retval;

   if( procedure_id.empty( ) )
      throw runtime_error( "unexpected empty procedure_id for get_execute_procedure_info" );
   else if( procedure_id == "118410" ) // i.e. Generate_PDF_View
      retval = "";

   return retval;
}

bool Meta_View::get_is_alias( ) const
{
   return false;
}

void Meta_View::get_alias_base_info( pair< string, string >& alias_base_info ) const
{
   ( void )alias_base_info;
}

void Meta_View::get_base_class_info( vector< pair< string, string > >& base_class_info ) const
{
   ( void )base_class_info;
}

class_base& Meta_View::get_or_create_graph_child( const string& context )
{
   class_base* p_class_base( 0 );

   string::size_type pos = context.find( '.' );
   string sub_context( context.substr( 0, pos ) );

   if( sub_context.empty( ) )
      throw runtime_error( "unexpected empty sub-context" );
   else if( ( sub_context == "_300631" ) || ( sub_context == "child_Class_Created" ) )
      p_class_base = &child_Class_Created( );
   else if( ( sub_context == "_302830c" ) || ( sub_context == "child_Package_Option" ) )
      p_class_base = &child_Package_Option( );
   else if( ( sub_context == "_301900" ) || ( sub_context == "child_View_Field" ) )
      p_class_base = &child_View_Field( );
   else if( ( sub_context == c_field_id_Access_Permission ) || ( sub_context == c_field_name_Access_Permission ) )
      p_class_base = &Access_Permission( );
   else if( ( sub_context == c_field_id_Change_Permission ) || ( sub_context == c_field_name_Change_Permission ) )
      p_class_base = &Change_Permission( );
   else if( ( sub_context == c_field_id_Class ) || ( sub_context == c_field_name_Class ) )
      p_class_base = &Class( );
   else if( ( sub_context == c_field_id_Model ) || ( sub_context == c_field_name_Model ) )
      p_class_base = &Model( );
   else if( ( sub_context == c_field_id_Type ) || ( sub_context == c_field_name_Type ) )
      p_class_base = &Type( );

   if( !p_class_base )
      throw runtime_error( "unknown sub-context '" + sub_context + "'" );

   if( pos != string::npos )
      p_class_base = &p_class_base->get_or_create_graph_child( context.substr( pos + 1 ) );

   return *p_class_base;
}

void Meta_View::get_sql_column_names(
 vector< string >& names, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   names.push_back( "C_Access_Permission" );
   names.push_back( "C_Access_Restriction" );
   names.push_back( "C_Allow_Copy_Action" );
   names.push_back( "C_Allow_Printable_Version" );
   names.push_back( "C_Auto_Back_After_Save" );
   names.push_back( "C_Change_Permission" );
   names.push_back( "C_Change_Restriction" );
   names.push_back( "C_Class" );
   names.push_back( "C_File_Links_Always_As_Single" );
   names.push_back( "C_Id" );
   names.push_back( "C_Ignore_Parent_Record" );
   names.push_back( "C_Ignore_Unactionable_Records" );
   names.push_back( "C_Model" );
   names.push_back( "C_PDF_Font_Type" );
   names.push_back( "C_PDF_View_Type" );
   names.push_back( "C_Print_Without_Highlight" );
   names.push_back( "C_Title" );
   names.push_back( "C_Type" );
   names.push_back( "C_Type_Key" );
   names.push_back( "C_Use_First_Row_As_Header" );

   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;
}

void Meta_View::get_sql_column_values(
 vector< string >& values, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   values.push_back( sql_quote( to_string( Access_Permission( ) ) ) );
   values.push_back( to_string( Access_Restriction( ) ) );
   values.push_back( to_string( Allow_Copy_Action( ) ) );
   values.push_back( to_string( Allow_Printable_Version( ) ) );
   values.push_back( to_string( Auto_Back_After_Save( ) ) );
   values.push_back( sql_quote( to_string( Change_Permission( ) ) ) );
   values.push_back( to_string( Change_Restriction( ) ) );
   values.push_back( sql_quote( to_string( Class( ) ) ) );
   values.push_back( to_string( File_Links_Always_As_Single( ) ) );
   values.push_back( sql_quote( to_string( Id( ) ) ) );
   values.push_back( to_string( Ignore_Parent_Record( ) ) );
   values.push_back( to_string( Ignore_Unactionable_Records( ) ) );
   values.push_back( sql_quote( to_string( Model( ) ) ) );
   values.push_back( to_string( PDF_Font_Type( ) ) );
   values.push_back( to_string( PDF_View_Type( ) ) );
   values.push_back( to_string( Print_Without_Highlight( ) ) );
   values.push_back( sql_quote( to_string( Title( ) ) ) );
   values.push_back( sql_quote( to_string( Type( ) ) ) );
   values.push_back( sql_quote( to_string( Type_Key( ) ) ) );
   values.push_back( to_string( Use_First_Row_As_Header( ) ) );

   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;
}

void Meta_View::get_required_field_names(
 set< string >& names, bool use_transients, set< string >* p_dependents ) const
{
   set< string > local_dependents;
   set< string >& dependents( p_dependents ? *p_dependents : local_dependents );

   get_always_required_field_names( names, use_transients, dependents );

   // [(start field_from_other_field)] 600311
   if( needs_field_value( "Type_Key", dependents ) )
   {
      dependents.insert( "Type" );

      if( ( use_transients && is_field_transient( e_field_id_Type ) )
       || ( !use_transients && !is_field_transient( e_field_id_Type ) ) )
         names.insert( "Type" );
   }
   // [(finish field_from_other_field)] 600311

   // [(start field_from_search_replace)] 600320
   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Class" );

      if( ( use_transients && is_field_transient( e_field_id_Class ) )
       || ( !use_transients && !is_field_transient( e_field_id_Class ) ) )
         names.insert( "Class" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Type" );

      if( ( use_transients && is_field_transient( e_field_id_Type ) )
       || ( !use_transients && !is_field_transient( e_field_id_Type ) ) )
         names.insert( "Type" );
   }
   // [(finish field_from_search_replace)] 600320

   // [<start get_required_field_names>]
   // [<finish get_required_field_names>]
}

void Meta_View::get_always_required_field_names(
 set< string >& names, bool use_transients, set< string >& dependents ) const
{
   ( void )names;
   ( void )dependents;
   ( void )use_transients;

   // [(start modifier_field_value)] 600330
   dependents.insert( "Type" ); // (for Is_Print_Version modifier)

   if( ( use_transients && is_field_transient( e_field_id_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Type ) ) )
      names.insert( "Type" );
   // [(finish modifier_field_value)] 600330

   // [(start modifier_field_value)] 600335
   dependents.insert( "Type" ); // (for Is_Not_Print_Version modifier)

   if( ( use_transients && is_field_transient( e_field_id_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Type ) ) )
      names.insert( "Type" );
   // [(finish modifier_field_value)] 600335

   // [(start modifier_field_value)] 600340
   dependents.insert( "Allow_Printable_Version" ); // (for Printing_Disallowed modifier)

   if( ( use_transients && is_field_transient( e_field_id_Allow_Printable_Version ) )
    || ( !use_transients && !is_field_transient( e_field_id_Allow_Printable_Version ) ) )
      names.insert( "Allow_Printable_Version" );
   // [(finish modifier_field_value)] 600340

   // [(start modifier_field_value)] 600341
   dependents.insert( "PDF_View_Type" ); // (for PDF_View_Is_None modifier)

   if( ( use_transients && is_field_transient( e_field_id_PDF_View_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_PDF_View_Type ) ) )
      names.insert( "PDF_View_Type" );
   // [(finish modifier_field_value)] 600341

   // [(start modifier_field_value)] 600342
   dependents.insert( "PDF_View_Type" ); // (for PDF_View_Is_Custom modifier)

   if( ( use_transients && is_field_transient( e_field_id_PDF_View_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_PDF_View_Type ) ) )
      names.insert( "PDF_View_Type" );
   // [(finish modifier_field_value)] 600342

   // [<start get_always_required_field_names>]
   // [<finish get_always_required_field_names>]
}

void Meta_View::get_transient_replacement_field_names( const string& name, vector< string >& names ) const
{
   ( void )name;
   ( void )names;

   // [<start get_transient_replacement_field_names>]
   // [<finish get_transient_replacement_field_names>]
}

void Meta_View::do_generate_sql( generate_sql_type type,
 vector< string >& sql_stmts, set< string >& tx_key_info, vector< string >* p_sql_undo_stmts ) const
{
   generate_sql( static_class_name( ), type, sql_stmts, tx_key_info, p_sql_undo_stmts );
}

const char* Meta_View::static_resolved_module_id( )
{
   return static_module_id( );
}

const char* Meta_View::static_resolved_module_name( )
{
   return static_module_name( );
}

const char* Meta_View::static_lock_class_id( )
{
   return "118100";
}

const char* Meta_View::static_check_class_name( )
{
   return "View";
}

const char* Meta_View::static_persistence_extra( )
{
   return "";
}

bool Meta_View::static_has_derivations( )
{
   return !g_derivations.empty( );
}

void Meta_View::static_get_class_info( class_info_container& class_info )
{
   class_info.push_back( "100.118100" );
}

void Meta_View::static_get_field_info( field_info_container& all_field_info )
{
   all_field_info.push_back( field_info( "301825", "Access_Permission", "Meta_Permission", false, "", "" ) );
   all_field_info.push_back( field_info( "118102", "Access_Restriction", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "118113", "Allow_Copy_Action", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "118109", "Allow_Printable_Version", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "118115", "Auto_Back_After_Save", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "301830", "Change_Permission", "Meta_Permission", false, "", "" ) );
   all_field_info.push_back( field_info( "118103", "Change_Restriction", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "301820", "Class", "Meta_Class", true, "", "" ) );
   all_field_info.push_back( field_info( "118117", "File_Links_Always_As_Single", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "118105", "Id", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "118118", "Ignore_Parent_Record", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "118116", "Ignore_Unactionable_Records", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "301800", "Model", "Meta_Model", true, "", "" ) );
   all_field_info.push_back( field_info( "118101", "Name", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "118112", "PDF_Font_Type", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "118111", "PDF_View_Type", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "118106", "Print_Without_Highlight", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "118104", "Title", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "301810", "Type", "Meta_View_Type", true, "", "" ) );
   all_field_info.push_back( field_info( "118110", "Type_Key", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "118114", "Use_First_Row_As_Header", "bool", false, "", "" ) );
}

void Meta_View::static_get_foreign_key_info( foreign_key_info_container& foreign_key_info )
{
   ( void )foreign_key_info;

   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Access_Permission, make_pair( "Meta.118100", "Meta_Permission" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Change_Permission, make_pair( "Meta.118100", "Meta_Permission" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Class, make_pair( "Meta.118100", "Meta_Class" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Model, make_pair( "Meta.118100", "Meta_Model" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Type, make_pair( "Meta.118100", "Meta_View_Type" ) ) );
}

int Meta_View::static_get_num_fields( bool* p_done, const string* p_class_name )
{
   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;

   return c_num_fields;
}

bool Meta_View::static_is_field_encrypted( field_id id )
{
   return is_encrypted_field( static_get_field_id( id ) );
}

bool Meta_View::static_is_field_transient( field_id id )
{
   return is_transient_field( static_get_field_id( id ) );
}

const char* Meta_View::static_get_field_id( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "301825";
      break;

      case 2:
      p_id = "118102";
      break;

      case 3:
      p_id = "118113";
      break;

      case 4:
      p_id = "118109";
      break;

      case 5:
      p_id = "118115";
      break;

      case 6:
      p_id = "301830";
      break;

      case 7:
      p_id = "118103";
      break;

      case 8:
      p_id = "301820";
      break;

      case 9:
      p_id = "118117";
      break;

      case 10:
      p_id = "118105";
      break;

      case 11:
      p_id = "118118";
      break;

      case 12:
      p_id = "118116";
      break;

      case 13:
      p_id = "301800";
      break;

      case 14:
      p_id = "118101";
      break;

      case 15:
      p_id = "118112";
      break;

      case 16:
      p_id = "118111";
      break;

      case 17:
      p_id = "118106";
      break;

      case 18:
      p_id = "118104";
      break;

      case 19:
      p_id = "301810";
      break;

      case 20:
      p_id = "118110";
      break;

      case 21:
      p_id = "118114";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class View" );

   return p_id;
}

const char* Meta_View::static_get_field_name( field_id id )
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
      p_id = "Allow_Copy_Action";
      break;

      case 4:
      p_id = "Allow_Printable_Version";
      break;

      case 5:
      p_id = "Auto_Back_After_Save";
      break;

      case 6:
      p_id = "Change_Permission";
      break;

      case 7:
      p_id = "Change_Restriction";
      break;

      case 8:
      p_id = "Class";
      break;

      case 9:
      p_id = "File_Links_Always_As_Single";
      break;

      case 10:
      p_id = "Id";
      break;

      case 11:
      p_id = "Ignore_Parent_Record";
      break;

      case 12:
      p_id = "Ignore_Unactionable_Records";
      break;

      case 13:
      p_id = "Model";
      break;

      case 14:
      p_id = "Name";
      break;

      case 15:
      p_id = "PDF_Font_Type";
      break;

      case 16:
      p_id = "PDF_View_Type";
      break;

      case 17:
      p_id = "Print_Without_Highlight";
      break;

      case 18:
      p_id = "Title";
      break;

      case 19:
      p_id = "Type";
      break;

      case 20:
      p_id = "Type_Key";
      break;

      case 21:
      p_id = "Use_First_Row_As_Header";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class View" );

   return p_id;
}

int Meta_View::static_get_field_num( const string& field )
{
   int rc = 0;

   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for static_get_field_num( )" );
   else if( field == c_field_id_Access_Permission || field == c_field_name_Access_Permission )
      rc += 1;
   else if( field == c_field_id_Access_Restriction || field == c_field_name_Access_Restriction )
      rc += 2;
   else if( field == c_field_id_Allow_Copy_Action || field == c_field_name_Allow_Copy_Action )
      rc += 3;
   else if( field == c_field_id_Allow_Printable_Version || field == c_field_name_Allow_Printable_Version )
      rc += 4;
   else if( field == c_field_id_Auto_Back_After_Save || field == c_field_name_Auto_Back_After_Save )
      rc += 5;
   else if( field == c_field_id_Change_Permission || field == c_field_name_Change_Permission )
      rc += 6;
   else if( field == c_field_id_Change_Restriction || field == c_field_name_Change_Restriction )
      rc += 7;
   else if( field == c_field_id_Class || field == c_field_name_Class )
      rc += 8;
   else if( field == c_field_id_File_Links_Always_As_Single || field == c_field_name_File_Links_Always_As_Single )
      rc += 9;
   else if( field == c_field_id_Id || field == c_field_name_Id )
      rc += 10;
   else if( field == c_field_id_Ignore_Parent_Record || field == c_field_name_Ignore_Parent_Record )
      rc += 11;
   else if( field == c_field_id_Ignore_Unactionable_Records || field == c_field_name_Ignore_Unactionable_Records )
      rc += 12;
   else if( field == c_field_id_Model || field == c_field_name_Model )
      rc += 13;
   else if( field == c_field_id_Name || field == c_field_name_Name )
      rc += 14;
   else if( field == c_field_id_PDF_Font_Type || field == c_field_name_PDF_Font_Type )
      rc += 15;
   else if( field == c_field_id_PDF_View_Type || field == c_field_name_PDF_View_Type )
      rc += 16;
   else if( field == c_field_id_Print_Without_Highlight || field == c_field_name_Print_Without_Highlight )
      rc += 17;
   else if( field == c_field_id_Title || field == c_field_name_Title )
      rc += 18;
   else if( field == c_field_id_Type || field == c_field_name_Type )
      rc += 19;
   else if( field == c_field_id_Type_Key || field == c_field_name_Type_Key )
      rc += 20;
   else if( field == c_field_id_Use_First_Row_As_Header || field == c_field_name_Use_First_Row_As_Header )
      rc += 21;

   return rc - 1;
}

procedure_info_container& Meta_View::static_get_procedure_info( )
{
   static bool initialised = false;
   static procedure_info_container procedures;

   if( !initialised )
   {
      initialised = true;
      procedures.insert( make_pair( "118410", procedure_info( "Generate_PDF_View" ) ) );
   }

   return procedures;
}

string Meta_View::static_get_sql_columns( )
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
    "C_Allow_Copy_Action INTEGER NOT NULL,"
    "C_Allow_Printable_Version INTEGER NOT NULL,"
    "C_Auto_Back_After_Save INTEGER NOT NULL,"
    "C_Change_Permission VARCHAR(75) NOT NULL,"
    "C_Change_Restriction INTEGER NOT NULL,"
    "C_Class VARCHAR(75) NOT NULL,"
    "C_File_Links_Always_As_Single INTEGER NOT NULL,"
    "C_Id VARCHAR(200) NOT NULL,"
    "C_Ignore_Parent_Record INTEGER NOT NULL,"
    "C_Ignore_Unactionable_Records INTEGER NOT NULL,"
    "C_Model VARCHAR(75) NOT NULL,"
    "C_PDF_Font_Type INTEGER NOT NULL,"
    "C_PDF_View_Type INTEGER NOT NULL,"
    "C_Print_Without_Highlight INTEGER NOT NULL,"
    "C_Title VARCHAR(200) NOT NULL,"
    "C_Type VARCHAR(75) NOT NULL,"
    "C_Type_Key VARCHAR(200) NOT NULL,"
    "C_Use_First_Row_As_Header INTEGER NOT NULL,"
    "PRIMARY KEY(C_Key_)";

   return sql_columns;
}

void Meta_View::static_get_text_search_fields( vector< string >& fields )
{
   ( void )fields;
}

void Meta_View::static_get_all_enum_pairs( vector< pair< string, string > >& pairs )
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

   pairs.push_back( make_pair( "enum_pdf_font_type_0", get_enum_string_pdf_font_type( 0 ) ) );
   pairs.push_back( make_pair( "enum_pdf_font_type_1", get_enum_string_pdf_font_type( 1 ) ) );
   pairs.push_back( make_pair( "enum_pdf_font_type_20", get_enum_string_pdf_font_type( 20 ) ) );
   pairs.push_back( make_pair( "enum_pdf_font_type_21", get_enum_string_pdf_font_type( 21 ) ) );

   pairs.push_back( make_pair( "enum_view_pdf_view_type_0", get_enum_string_view_pdf_view_type( 0 ) ) );
   pairs.push_back( make_pair( "enum_view_pdf_view_type_2", get_enum_string_view_pdf_view_type( 2 ) ) );
   pairs.push_back( make_pair( "enum_view_pdf_view_type_99", get_enum_string_view_pdf_view_type( 99 ) ) );
}

void Meta_View::static_get_all_index_pairs( vector< pair< string, string > >& pairs )
{
   pairs.push_back( make_pair( "Model,Id", "string,string" ) );
}

void Meta_View::static_get_all_unique_indexes( vector< string >& unique_indexes )
{
   unique_indexes.push_back( "Model,Id" );
}

void Meta_View::static_get_sql_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_Model,C_Id" );
}

void Meta_View::static_get_sql_unique_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_Model,C_Id" );
}

void Meta_View::static_insert_derivation( const string& module_and_class_id )
{
   g_derivations.insert( module_and_class_id );
}

void Meta_View::static_remove_derivation( const string& module_and_class_id )
{
   if( g_derivations.count( module_and_class_id ) )
      g_derivations.erase( module_and_class_id );
}

void Meta_View::static_insert_external_alias( const string& module_and_class_id, Meta_View* p_instance )
{
   g_external_aliases.insert( external_aliases_value_type( module_and_class_id, p_instance ) );
}

void Meta_View::static_remove_external_alias( const string& module_and_class_id )
{
   if( g_external_aliases.count( module_and_class_id ) )
   {
      delete g_external_aliases[ module_and_class_id ];
      g_external_aliases.erase( module_and_class_id );
   }
}

void Meta_View::static_class_init( const char* p_module_name )
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

   g_pdf_font_type_enum.insert( 0 );
   g_pdf_font_type_enum.insert( 1 );
   g_pdf_font_type_enum.insert( 20 );
   g_pdf_font_type_enum.insert( 21 );

   g_view_pdf_view_type_enum.insert( 0 );
   g_view_pdf_view_type_enum.insert( 2 );
   g_view_pdf_view_type_enum.insert( 99 );

   // [<start static_class_init>]
   // [<finish static_class_init>]
}

void Meta_View::static_class_term( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for term" );

   // [<start static_class_term>]
   // [<finish static_class_term>]
}
