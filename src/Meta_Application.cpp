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

#include "Meta_Application.h"

#include "Meta_Module.h"
#include "Meta_Application_Script.h"
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

// [<start includes>]
//nyi
#include "Meta_List.h"
#include "Meta_View.h"
#include "Meta_Class.h"
#include "Meta_Field.h"
#include "Meta_Model.h"
#include "Meta_Package.h"
#ifndef _WIN32
#  include <sys/stat.h>
#endif
// [<finish includes>]

using namespace std;

// [<start namespaces>]
// [<finish namespaces>]

template< > inline string to_string( const Meta_Application_Script& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_Workgroup& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

inline void from_string( Meta_Application_Script& c, const string& s )
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

#include "Meta_Application.cmh"

const int32_t c_version = 1;

const char* const c_field_id_Actions = "127104";
const char* const c_field_id_Add_Modules_Automatically = "127128";
const char* const c_field_id_Allow_Duplicate_Logins = "127108";
const char* const c_field_id_Allow_Module_Switching = "127109";
const char* const c_field_id_Auto_Login_Days = "127124";
const char* const c_field_id_Blockchain_Id = "127135";
const char* const c_field_id_Create_Database = "127125";
const char* const c_field_id_Created_Database = "127126";
const char* const c_field_id_Creation_Script = "302225";
const char* const c_field_id_Default_Image_Height = "127114";
const char* const c_field_id_Default_Image_Width = "127113";
const char* const c_field_id_Default_List_Print_Row_Limit = "127118";
const char* const c_field_id_Default_List_Row_Limit = "127115";
const char* const c_field_id_Default_Max_Attached_File_Size = "127127";
const char* const c_field_id_Default_Multiline_Max_Rows = "127123";
const char* const c_field_id_Default_Multiline_Min_Rows = "127116";
const char* const c_field_id_Default_Multiline_Text_Limit = "127117";
const char* const c_field_id_Default_Multiline_Text_Trunc = "127119";
const char* const c_field_id_Encrypt_Dynamic_Content = "127130";
const char* const c_field_id_Generate_Details = "127106";
const char* const c_field_id_Generate_Status = "127105";
const char* const c_field_id_Generate_Type = "127120";
const char* const c_field_id_Installing_Script = "127138";
const char* const c_field_id_Keep_Existing_Data = "127129";
const char* const c_field_id_Module_Prefix = "127103";
const char* const c_field_id_Name = "127101";
const char* const c_field_id_Print_Lists_With_Check_Boxes = "127111";
const char* const c_field_id_Print_Lists_With_Row_Numbers = "127112";
const char* const c_field_id_Registration_Key = "127132";
const char* const c_field_id_Show_Inaccessible_Modules = "127110";
const char* const c_field_id_Type = "127136";
const char* const c_field_id_Use_Check_Boxes_for_Bools = "127121";
const char* const c_field_id_Use_Embedded_Images = "127131";
const char* const c_field_id_Use_Script = "127137";
const char* const c_field_id_Use_TLS_Sessions = "127134";
const char* const c_field_id_Use_URL_Checksum = "127107";
const char* const c_field_id_Use_Vertical_Menu = "127133";
const char* const c_field_id_Version = "127102";
const char* const c_field_id_Workgroup = "302220";
const char* const c_field_id_Year_Created = "127139";

const char* const c_field_name_Actions = "Actions";
const char* const c_field_name_Add_Modules_Automatically = "Add_Modules_Automatically";
const char* const c_field_name_Allow_Duplicate_Logins = "Allow_Duplicate_Logins";
const char* const c_field_name_Allow_Module_Switching = "Allow_Module_Switching";
const char* const c_field_name_Auto_Login_Days = "Auto_Login_Days";
const char* const c_field_name_Blockchain_Id = "Blockchain_Id";
const char* const c_field_name_Create_Database = "Create_Database";
const char* const c_field_name_Created_Database = "Created_Database";
const char* const c_field_name_Creation_Script = "Creation_Script";
const char* const c_field_name_Default_Image_Height = "Default_Image_Height";
const char* const c_field_name_Default_Image_Width = "Default_Image_Width";
const char* const c_field_name_Default_List_Print_Row_Limit = "Default_List_Print_Row_Limit";
const char* const c_field_name_Default_List_Row_Limit = "Default_List_Row_Limit";
const char* const c_field_name_Default_Max_Attached_File_Size = "Default_Max_Attached_File_Size";
const char* const c_field_name_Default_Multiline_Max_Rows = "Default_Multiline_Max_Rows";
const char* const c_field_name_Default_Multiline_Min_Rows = "Default_Multiline_Min_Rows";
const char* const c_field_name_Default_Multiline_Text_Limit = "Default_Multiline_Text_Limit";
const char* const c_field_name_Default_Multiline_Text_Trunc = "Default_Multiline_Text_Trunc";
const char* const c_field_name_Encrypt_Dynamic_Content = "Encrypt_Dynamic_Content";
const char* const c_field_name_Generate_Details = "Generate_Details";
const char* const c_field_name_Generate_Status = "Generate_Status";
const char* const c_field_name_Generate_Type = "Generate_Type";
const char* const c_field_name_Installing_Script = "Installing_Script";
const char* const c_field_name_Keep_Existing_Data = "Keep_Existing_Data";
const char* const c_field_name_Module_Prefix = "Module_Prefix";
const char* const c_field_name_Name = "Name";
const char* const c_field_name_Print_Lists_With_Check_Boxes = "Print_Lists_With_Check_Boxes";
const char* const c_field_name_Print_Lists_With_Row_Numbers = "Print_Lists_With_Row_Numbers";
const char* const c_field_name_Registration_Key = "Registration_Key";
const char* const c_field_name_Show_Inaccessible_Modules = "Show_Inaccessible_Modules";
const char* const c_field_name_Type = "Type";
const char* const c_field_name_Use_Check_Boxes_for_Bools = "Use_Check_Boxes_for_Bools";
const char* const c_field_name_Use_Embedded_Images = "Use_Embedded_Images";
const char* const c_field_name_Use_Script = "Use_Script";
const char* const c_field_name_Use_TLS_Sessions = "Use_TLS_Sessions";
const char* const c_field_name_Use_URL_Checksum = "Use_URL_Checksum";
const char* const c_field_name_Use_Vertical_Menu = "Use_Vertical_Menu";
const char* const c_field_name_Version = "Version";
const char* const c_field_name_Workgroup = "Workgroup";
const char* const c_field_name_Year_Created = "Year_Created";

const char* const c_field_display_name_Actions = "field_application_actions";
const char* const c_field_display_name_Add_Modules_Automatically = "field_application_add_modules_automatically";
const char* const c_field_display_name_Allow_Duplicate_Logins = "field_application_allow_duplicate_logins";
const char* const c_field_display_name_Allow_Module_Switching = "field_application_allow_module_switching";
const char* const c_field_display_name_Auto_Login_Days = "field_application_auto_login_days";
const char* const c_field_display_name_Blockchain_Id = "field_application_blockchain_id";
const char* const c_field_display_name_Create_Database = "field_application_create_database";
const char* const c_field_display_name_Created_Database = "field_application_created_database";
const char* const c_field_display_name_Creation_Script = "field_application_creation_script";
const char* const c_field_display_name_Default_Image_Height = "field_application_default_image_height";
const char* const c_field_display_name_Default_Image_Width = "field_application_default_image_width";
const char* const c_field_display_name_Default_List_Print_Row_Limit = "field_application_default_list_print_row_limit";
const char* const c_field_display_name_Default_List_Row_Limit = "field_application_default_list_row_limit";
const char* const c_field_display_name_Default_Max_Attached_File_Size = "field_application_default_max_attached_file_size";
const char* const c_field_display_name_Default_Multiline_Max_Rows = "field_application_default_multiline_max_rows";
const char* const c_field_display_name_Default_Multiline_Min_Rows = "field_application_default_multiline_min_rows";
const char* const c_field_display_name_Default_Multiline_Text_Limit = "field_application_default_multiline_text_limit";
const char* const c_field_display_name_Default_Multiline_Text_Trunc = "field_application_default_multiline_text_trunc";
const char* const c_field_display_name_Encrypt_Dynamic_Content = "field_application_encrypt_dynamic_content";
const char* const c_field_display_name_Generate_Details = "field_application_generate_details";
const char* const c_field_display_name_Generate_Status = "field_application_generate_status";
const char* const c_field_display_name_Generate_Type = "field_application_generate_type";
const char* const c_field_display_name_Installing_Script = "field_application_installing_script";
const char* const c_field_display_name_Keep_Existing_Data = "field_application_keep_existing_data";
const char* const c_field_display_name_Module_Prefix = "field_application_module_prefix";
const char* const c_field_display_name_Name = "field_application_name";
const char* const c_field_display_name_Print_Lists_With_Check_Boxes = "field_application_print_lists_with_check_boxes";
const char* const c_field_display_name_Print_Lists_With_Row_Numbers = "field_application_print_lists_with_row_numbers";
const char* const c_field_display_name_Registration_Key = "field_application_registration_key";
const char* const c_field_display_name_Show_Inaccessible_Modules = "field_application_show_inaccessible_modules";
const char* const c_field_display_name_Type = "field_application_type";
const char* const c_field_display_name_Use_Check_Boxes_for_Bools = "field_application_use_check_boxes_for_bools";
const char* const c_field_display_name_Use_Embedded_Images = "field_application_use_embedded_images";
const char* const c_field_display_name_Use_Script = "field_application_use_script";
const char* const c_field_display_name_Use_TLS_Sessions = "field_application_use_tls_sessions";
const char* const c_field_display_name_Use_URL_Checksum = "field_application_use_url_checksum";
const char* const c_field_display_name_Use_Vertical_Menu = "field_application_use_vertical_menu";
const char* const c_field_display_name_Version = "field_application_version";
const char* const c_field_display_name_Workgroup = "field_application_workgroup";
const char* const c_field_display_name_Year_Created = "field_application_year_created";

const int c_num_fields = 40;

const char* const c_all_sorted_field_ids[ ] =
{
   "127101",
   "127102",
   "127103",
   "127104",
   "127105",
   "127106",
   "127107",
   "127108",
   "127109",
   "127110",
   "127111",
   "127112",
   "127113",
   "127114",
   "127115",
   "127116",
   "127117",
   "127118",
   "127119",
   "127120",
   "127121",
   "127123",
   "127124",
   "127125",
   "127126",
   "127127",
   "127128",
   "127129",
   "127130",
   "127131",
   "127132",
   "127133",
   "127134",
   "127135",
   "127136",
   "127137",
   "127138",
   "127139",
   "302220",
   "302225"
};

const char* const c_all_sorted_field_names[ ] =
{
   "Actions",
   "Add_Modules_Automatically",
   "Allow_Duplicate_Logins",
   "Allow_Module_Switching",
   "Auto_Login_Days",
   "Blockchain_Id",
   "Create_Database",
   "Created_Database",
   "Creation_Script",
   "Default_Image_Height",
   "Default_Image_Width",
   "Default_List_Print_Row_Limit",
   "Default_List_Row_Limit",
   "Default_Max_Attached_File_Size",
   "Default_Multiline_Max_Rows",
   "Default_Multiline_Min_Rows",
   "Default_Multiline_Text_Limit",
   "Default_Multiline_Text_Trunc",
   "Encrypt_Dynamic_Content",
   "Generate_Details",
   "Generate_Status",
   "Generate_Type",
   "Installing_Script",
   "Keep_Existing_Data",
   "Module_Prefix",
   "Name",
   "Print_Lists_With_Check_Boxes",
   "Print_Lists_With_Row_Numbers",
   "Registration_Key",
   "Show_Inaccessible_Modules",
   "Type",
   "Use_Check_Boxes_for_Bools",
   "Use_Embedded_Images",
   "Use_Script",
   "Use_TLS_Sessions",
   "Use_URL_Checksum",
   "Use_Vertical_Menu",
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

const int c_num_transient_fields = 5;

const char* const c_transient_sorted_field_ids[ ] =
{
   "127106",
   "127125",
   "127128",
   "127135",
   "127136"
};

const char* const c_transient_sorted_field_names[ ] =
{
   "Add_Modules_Automatically",
   "Blockchain_Id",
   "Create_Database",
   "Generate_Details",
   "Type"
};

inline bool is_transient_field( const string& field )
{
   return binary_search( c_transient_sorted_field_ids,
    c_transient_sorted_field_ids + c_num_transient_fields, field.c_str( ), compare )
    || binary_search( c_transient_sorted_field_names,
    c_transient_sorted_field_names + c_num_transient_fields, field.c_str( ), compare );
}

const char* const c_procedure_id_Generate = "127410";
const char* const c_procedure_id_Generate_File_Links = "127430";
const char* const c_procedure_id_Generate_Modules = "127420";
const char* const c_procedure_id_Generate_Upgrade_DDL = "127440";
const char* const c_procedure_id_Test_Proc_1 = "127495";
const char* const c_procedure_id_Test_Proc_2 = "127497";

const uint64_t c_modifier_Has_No_Application_Log = UINT64_C( 0x100 );
const uint64_t c_modifier_Has_No_Installation_Scripts = UINT64_C( 0x200 );
const uint64_t c_modifier_Is_Installing_Script = UINT64_C( 0x400 );
const uint64_t c_modifier_Is_Non_Traditional = UINT64_C( 0x800 );
const uint64_t c_modifier_Is_Not_Full_Generate = UINT64_C( 0x1000 );
const uint64_t c_modifier_Is_Not_Using_Script = UINT64_C( 0x2000 );
const uint64_t c_modifier_Is_Traditional = UINT64_C( 0x4000 );
const uint64_t c_modifier_Is_Using_Script = UINT64_C( 0x8000 );
const uint64_t c_modifier_Was_Cloned = UINT64_C( 0x10000 );

domain_string_max_size< 30 > g_Blockchain_Id_domain;
aggregate_domain< string,
 domain_string_identifier_format,
 domain_string_max_size< 30 > > g_Module_Prefix_domain;
aggregate_domain< string,
 domain_string_identifier_format,
 domain_string_max_size< 30 > > g_Name_domain;
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

typedef map< string, Meta_Application* > external_aliases_container;
typedef external_aliases_container::const_iterator external_aliases_const_iterator;
typedef external_aliases_container::value_type external_aliases_value_type;

typedef map< size_t, Meta_Application* > external_aliases_lookup_container;
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

string g_default_Actions = string( "127410" );
bool g_default_Add_Modules_Automatically = bool( 1 );
bool g_default_Allow_Duplicate_Logins = bool( 0 );
bool g_default_Allow_Module_Switching = bool( 1 );
int g_default_Auto_Login_Days = int( 0 );
string g_default_Blockchain_Id = string( );
bool g_default_Create_Database = bool( 1 );
bool g_default_Created_Database = bool( 0 );
string g_default_Creation_Script = string( );
int g_default_Default_Image_Height = int( 75 );
int g_default_Default_Image_Width = int( 100 );
int g_default_Default_List_Print_Row_Limit = int( 4000 );
int g_default_Default_List_Row_Limit = int( 20 );
int g_default_Default_Max_Attached_File_Size = int( 25000000 );
int g_default_Default_Multiline_Max_Rows = int( 50 );
int g_default_Default_Multiline_Min_Rows = int( 10 );
int g_default_Default_Multiline_Text_Limit = int( 2000 );
int g_default_Default_Multiline_Text_Trunc = int( 50 );
bool g_default_Encrypt_Dynamic_Content = bool( 0 );
string g_default_Generate_Details = string( );
string g_default_Generate_Status = string( );
int g_default_Generate_Type = int( 0 );
bool g_default_Installing_Script = bool( 0 );
bool g_default_Keep_Existing_Data = bool( 1 );
string g_default_Module_Prefix = string( );
string g_default_Name = string( );
bool g_default_Print_Lists_With_Check_Boxes = bool( 0 );
bool g_default_Print_Lists_With_Row_Numbers = bool( 0 );
string g_default_Registration_Key = string( );
bool g_default_Show_Inaccessible_Modules = bool( 0 );
bool g_default_Type = bool( 1 );
bool g_default_Use_Check_Boxes_for_Bools = bool( 1 );
bool g_default_Use_Embedded_Images = bool( 0 );
bool g_default_Use_Script = bool( 0 );
bool g_default_Use_TLS_Sessions = bool( 0 );
bool g_default_Use_URL_Checksum = bool( 0 );
bool g_default_Use_Vertical_Menu = bool( 0 );
string g_default_Version = string( "0.1" );
string g_default_Workgroup = string( );
int g_default_Year_Created = int( );

set< int > g_app_auto_days_enum;
set< int > g_app_print_row_limit_enum;
set< int > g_app_list_row_limit_enum;
set< int > g_app_text_rows_enum;
set< int > g_app_text_limit_enum;
set< int > g_app_text_trunc_limit_enum;
set< int > g_app_generate_type_enum;
set< bool > g_app_type_enum;

const int c_enum_app_auto_days_0( 0 );
const int c_enum_app_auto_days_1( 1 );
const int c_enum_app_auto_days_2( 2 );
const int c_enum_app_auto_days_3( 3 );
const int c_enum_app_auto_days_7( 7 );
const int c_enum_app_auto_days_10( 10 );
const int c_enum_app_auto_days_20( 20 );
const int c_enum_app_auto_days_30( 30 );
const int c_enum_app_auto_days_60( 60 );
const int c_enum_app_auto_days_90( 90 );
const int c_enum_app_auto_days_180( 180 );
const int c_enum_app_auto_days_365( 365 );

string get_enum_string_app_auto_days( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for app_auto_days" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_app_auto_days_0";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_app_auto_days_1";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_app_auto_days_2";
   else if( to_string( val ) == to_string( "3" ) )
      string_name = "enum_app_auto_days_3";
   else if( to_string( val ) == to_string( "7" ) )
      string_name = "enum_app_auto_days_7";
   else if( to_string( val ) == to_string( "10" ) )
      string_name = "enum_app_auto_days_10";
   else if( to_string( val ) == to_string( "20" ) )
      string_name = "enum_app_auto_days_20";
   else if( to_string( val ) == to_string( "30" ) )
      string_name = "enum_app_auto_days_30";
   else if( to_string( val ) == to_string( "60" ) )
      string_name = "enum_app_auto_days_60";
   else if( to_string( val ) == to_string( "90" ) )
      string_name = "enum_app_auto_days_90";
   else if( to_string( val ) == to_string( "180" ) )
      string_name = "enum_app_auto_days_180";
   else if( to_string( val ) == to_string( "365" ) )
      string_name = "enum_app_auto_days_365";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for app_auto_days" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_app_print_row_limit_1000( 1000 );
const int c_enum_app_print_row_limit_2000( 2000 );
const int c_enum_app_print_row_limit_3000( 3000 );
const int c_enum_app_print_row_limit_4000( 4000 );
const int c_enum_app_print_row_limit_5000( 5000 );
const int c_enum_app_print_row_limit_10000( 10000 );
const int c_enum_app_print_row_limit_20000( 20000 );

string get_enum_string_app_print_row_limit( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for app_print_row_limit" );
   else if( to_string( val ) == to_string( "1000" ) )
      string_name = "enum_app_print_row_limit_1000";
   else if( to_string( val ) == to_string( "2000" ) )
      string_name = "enum_app_print_row_limit_2000";
   else if( to_string( val ) == to_string( "3000" ) )
      string_name = "enum_app_print_row_limit_3000";
   else if( to_string( val ) == to_string( "4000" ) )
      string_name = "enum_app_print_row_limit_4000";
   else if( to_string( val ) == to_string( "5000" ) )
      string_name = "enum_app_print_row_limit_5000";
   else if( to_string( val ) == to_string( "10000" ) )
      string_name = "enum_app_print_row_limit_10000";
   else if( to_string( val ) == to_string( "20000" ) )
      string_name = "enum_app_print_row_limit_20000";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for app_print_row_limit" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_app_list_row_limit_10( 10 );
const int c_enum_app_list_row_limit_15( 15 );
const int c_enum_app_list_row_limit_20( 20 );
const int c_enum_app_list_row_limit_25( 25 );
const int c_enum_app_list_row_limit_30( 30 );
const int c_enum_app_list_row_limit_40( 40 );
const int c_enum_app_list_row_limit_50( 50 );
const int c_enum_app_list_row_limit_100( 100 );

string get_enum_string_app_list_row_limit( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for app_list_row_limit" );
   else if( to_string( val ) == to_string( "10" ) )
      string_name = "enum_app_list_row_limit_10";
   else if( to_string( val ) == to_string( "15" ) )
      string_name = "enum_app_list_row_limit_15";
   else if( to_string( val ) == to_string( "20" ) )
      string_name = "enum_app_list_row_limit_20";
   else if( to_string( val ) == to_string( "25" ) )
      string_name = "enum_app_list_row_limit_25";
   else if( to_string( val ) == to_string( "30" ) )
      string_name = "enum_app_list_row_limit_30";
   else if( to_string( val ) == to_string( "40" ) )
      string_name = "enum_app_list_row_limit_40";
   else if( to_string( val ) == to_string( "50" ) )
      string_name = "enum_app_list_row_limit_50";
   else if( to_string( val ) == to_string( "100" ) )
      string_name = "enum_app_list_row_limit_100";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for app_list_row_limit" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_app_text_rows_5( 5 );
const int c_enum_app_text_rows_10( 10 );
const int c_enum_app_text_rows_15( 15 );
const int c_enum_app_text_rows_20( 20 );
const int c_enum_app_text_rows_25( 25 );
const int c_enum_app_text_rows_30( 30 );
const int c_enum_app_text_rows_40( 40 );
const int c_enum_app_text_rows_50( 50 );
const int c_enum_app_text_rows_100( 100 );

string get_enum_string_app_text_rows( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for app_text_rows" );
   else if( to_string( val ) == to_string( "5" ) )
      string_name = "enum_app_text_rows_5";
   else if( to_string( val ) == to_string( "10" ) )
      string_name = "enum_app_text_rows_10";
   else if( to_string( val ) == to_string( "15" ) )
      string_name = "enum_app_text_rows_15";
   else if( to_string( val ) == to_string( "20" ) )
      string_name = "enum_app_text_rows_20";
   else if( to_string( val ) == to_string( "25" ) )
      string_name = "enum_app_text_rows_25";
   else if( to_string( val ) == to_string( "30" ) )
      string_name = "enum_app_text_rows_30";
   else if( to_string( val ) == to_string( "40" ) )
      string_name = "enum_app_text_rows_40";
   else if( to_string( val ) == to_string( "50" ) )
      string_name = "enum_app_text_rows_50";
   else if( to_string( val ) == to_string( "100" ) )
      string_name = "enum_app_text_rows_100";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for app_text_rows" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_app_text_limit_1000( 1000 );
const int c_enum_app_text_limit_1500( 1500 );
const int c_enum_app_text_limit_2000( 2000 );
const int c_enum_app_text_limit_2500( 2500 );
const int c_enum_app_text_limit_3000( 3000 );
const int c_enum_app_text_limit_4000( 4000 );
const int c_enum_app_text_limit_5000( 5000 );
const int c_enum_app_text_limit_10000( 10000 );
const int c_enum_app_text_limit_20000( 20000 );
const int c_enum_app_text_limit_30000( 30000 );
const int c_enum_app_text_limit_40000( 40000 );
const int c_enum_app_text_limit_50000( 50000 );
const int c_enum_app_text_limit_60000( 60000 );

string get_enum_string_app_text_limit( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for app_text_limit" );
   else if( to_string( val ) == to_string( "1000" ) )
      string_name = "enum_app_text_limit_1000";
   else if( to_string( val ) == to_string( "1500" ) )
      string_name = "enum_app_text_limit_1500";
   else if( to_string( val ) == to_string( "2000" ) )
      string_name = "enum_app_text_limit_2000";
   else if( to_string( val ) == to_string( "2500" ) )
      string_name = "enum_app_text_limit_2500";
   else if( to_string( val ) == to_string( "3000" ) )
      string_name = "enum_app_text_limit_3000";
   else if( to_string( val ) == to_string( "4000" ) )
      string_name = "enum_app_text_limit_4000";
   else if( to_string( val ) == to_string( "5000" ) )
      string_name = "enum_app_text_limit_5000";
   else if( to_string( val ) == to_string( "10000" ) )
      string_name = "enum_app_text_limit_10000";
   else if( to_string( val ) == to_string( "20000" ) )
      string_name = "enum_app_text_limit_20000";
   else if( to_string( val ) == to_string( "30000" ) )
      string_name = "enum_app_text_limit_30000";
   else if( to_string( val ) == to_string( "40000" ) )
      string_name = "enum_app_text_limit_40000";
   else if( to_string( val ) == to_string( "50000" ) )
      string_name = "enum_app_text_limit_50000";
   else if( to_string( val ) == to_string( "60000" ) )
      string_name = "enum_app_text_limit_60000";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for app_text_limit" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_app_text_trunc_limit_10( 10 );
const int c_enum_app_text_trunc_limit_15( 15 );
const int c_enum_app_text_trunc_limit_20( 20 );
const int c_enum_app_text_trunc_limit_25( 25 );
const int c_enum_app_text_trunc_limit_30( 30 );
const int c_enum_app_text_trunc_limit_40( 40 );
const int c_enum_app_text_trunc_limit_50( 50 );
const int c_enum_app_text_trunc_limit_75( 75 );
const int c_enum_app_text_trunc_limit_100( 100 );

string get_enum_string_app_text_trunc_limit( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for app_text_trunc_limit" );
   else if( to_string( val ) == to_string( "10" ) )
      string_name = "enum_app_text_trunc_limit_10";
   else if( to_string( val ) == to_string( "15" ) )
      string_name = "enum_app_text_trunc_limit_15";
   else if( to_string( val ) == to_string( "20" ) )
      string_name = "enum_app_text_trunc_limit_20";
   else if( to_string( val ) == to_string( "25" ) )
      string_name = "enum_app_text_trunc_limit_25";
   else if( to_string( val ) == to_string( "30" ) )
      string_name = "enum_app_text_trunc_limit_30";
   else if( to_string( val ) == to_string( "40" ) )
      string_name = "enum_app_text_trunc_limit_40";
   else if( to_string( val ) == to_string( "50" ) )
      string_name = "enum_app_text_trunc_limit_50";
   else if( to_string( val ) == to_string( "75" ) )
      string_name = "enum_app_text_trunc_limit_75";
   else if( to_string( val ) == to_string( "100" ) )
      string_name = "enum_app_text_trunc_limit_100";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for app_text_trunc_limit" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_app_generate_type_Full_Generate( 0 );
const int c_enum_app_generate_type_Skip_DB_Upgrade( 1 );
const int c_enum_app_generate_type_Application_Settings( 2 );

string get_enum_string_app_generate_type( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for app_generate_type" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_app_generate_type_Full_Generate";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_app_generate_type_Skip_DB_Upgrade";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_app_generate_type_Application_Settings";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for app_generate_type" );

   return get_module_string( lower( string_name ) );
}

const bool c_enum_app_type_Blockchain( 0 );
const bool c_enum_app_type_Traditional( 1 );

string get_enum_string_app_type( bool val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for app_type" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_app_type_Blockchain";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_app_type_Traditional";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for app_type" );

   return get_module_string( lower( string_name ) );
}

// [<start anonymous>]
//nyi
const char* const c_Meta_Application_Has_No_Scripts = "Meta_Application_Has_No_Scripts";
// [<finish anonymous>]

}

registration< Meta_Application > Application_registration( get_class_registry( ), "127100" );

class Meta_Application_command_functor;

class Meta_Application_command_handler : public command_handler
{
   friend class Meta_Application_command_functor;

   public:
   Meta_Application_command_handler( ) : p_Meta_Application( 0 ) { }

   void set_Meta_Application( Meta_Application* p_new_Meta_Application ) { p_Meta_Application = p_new_Meta_Application; }

   void handle_unknown_command( const string& command, const string& cmd_and_args )
   {
      throw runtime_error( "unknown command '" + command + "'" );
   }

   void handle_invalid_command( const command_parser& parser, const string& cmd_and_args )
   {
      throw runtime_error( "invalid command usage '" + cmd_and_args + "'" );
   }

   private:
   Meta_Application* p_Meta_Application;

   protected:
   string retval;
};

class Meta_Application_command_functor : public command_functor
{
   public:
   Meta_Application_command_functor( Meta_Application_command_handler& handler )
    : command_functor( handler ),
    cmd_handler( handler )
   {
   }

   void operator ( )( const string& command, const parameter_info& parameters );

   private:
   Meta_Application_command_handler& cmd_handler;
};

command_functor* Meta_Application_command_functor_factory( const string& /*name*/, command_handler& handler )
{
   return new Meta_Application_command_functor( dynamic_cast< Meta_Application_command_handler& >( handler ) );
}

void Meta_Application_command_functor::operator ( )( const string& command, const parameter_info& parameters )
{
   if( command == c_cmd_Meta_Application_key )
   {
      bool want_fixed( has_parm_val( parameters, c_cmd_Meta_Application_key_fixed ) );

      if( !want_fixed )
         cmd_handler.retval = cmd_handler.p_Meta_Application->get_key( );
      else
         cmd_handler.retval = cmd_handler.p_Meta_Application->get_fixed_key( );
   }
   else if( command == c_cmd_Meta_Application_ver )
   {
      string ver_rev( to_string( cmd_handler.p_Meta_Application->get_version( ) ) );
      ver_rev += "." + to_string( cmd_handler.p_Meta_Application->get_revision( ) );

      cmd_handler.retval = ver_rev;
   }
   else if( command == c_cmd_Meta_Application_get )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Application_get_field_name ) );

      bool handled = false;

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for getter call" );

      if( !handled && ( ( field_name == c_field_id_Actions ) || ( field_name == c_field_name_Actions ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Application->Actions( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Add_Modules_Automatically ) || ( field_name == c_field_name_Add_Modules_Automatically ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Application->Add_Modules_Automatically( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Duplicate_Logins ) || ( field_name == c_field_name_Allow_Duplicate_Logins ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Application->Allow_Duplicate_Logins( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Module_Switching ) || ( field_name == c_field_name_Allow_Module_Switching ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Application->Allow_Module_Switching( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Auto_Login_Days ) || ( field_name == c_field_name_Auto_Login_Days ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Application->Auto_Login_Days( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Blockchain_Id ) || ( field_name == c_field_name_Blockchain_Id ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Application->Blockchain_Id( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Create_Database ) || ( field_name == c_field_name_Create_Database ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Application->Create_Database( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Created_Database ) || ( field_name == c_field_name_Created_Database ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Application->Created_Database( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Creation_Script ) || ( field_name == c_field_name_Creation_Script ) ) )
      {
         handled = true;

         string_getter< Meta_Application_Script >( cmd_handler.p_Meta_Application->Creation_Script( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Default_Image_Height ) || ( field_name == c_field_name_Default_Image_Height ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Application->Default_Image_Height( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Default_Image_Width ) || ( field_name == c_field_name_Default_Image_Width ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Application->Default_Image_Width( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Default_List_Print_Row_Limit ) || ( field_name == c_field_name_Default_List_Print_Row_Limit ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Application->Default_List_Print_Row_Limit( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Default_List_Row_Limit ) || ( field_name == c_field_name_Default_List_Row_Limit ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Application->Default_List_Row_Limit( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Default_Max_Attached_File_Size ) || ( field_name == c_field_name_Default_Max_Attached_File_Size ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Application->Default_Max_Attached_File_Size( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Default_Multiline_Max_Rows ) || ( field_name == c_field_name_Default_Multiline_Max_Rows ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Application->Default_Multiline_Max_Rows( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Default_Multiline_Min_Rows ) || ( field_name == c_field_name_Default_Multiline_Min_Rows ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Application->Default_Multiline_Min_Rows( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Default_Multiline_Text_Limit ) || ( field_name == c_field_name_Default_Multiline_Text_Limit ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Application->Default_Multiline_Text_Limit( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Default_Multiline_Text_Trunc ) || ( field_name == c_field_name_Default_Multiline_Text_Trunc ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Application->Default_Multiline_Text_Trunc( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Encrypt_Dynamic_Content ) || ( field_name == c_field_name_Encrypt_Dynamic_Content ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Application->Encrypt_Dynamic_Content( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Generate_Details ) || ( field_name == c_field_name_Generate_Details ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Application->Generate_Details( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Generate_Status ) || ( field_name == c_field_name_Generate_Status ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Application->Generate_Status( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Generate_Type ) || ( field_name == c_field_name_Generate_Type ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Application->Generate_Type( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Installing_Script ) || ( field_name == c_field_name_Installing_Script ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Application->Installing_Script( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Keep_Existing_Data ) || ( field_name == c_field_name_Keep_Existing_Data ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Application->Keep_Existing_Data( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Module_Prefix ) || ( field_name == c_field_name_Module_Prefix ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Application->Module_Prefix( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Name ) || ( field_name == c_field_name_Name ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Application->Name( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Print_Lists_With_Check_Boxes ) || ( field_name == c_field_name_Print_Lists_With_Check_Boxes ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Application->Print_Lists_With_Check_Boxes( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Print_Lists_With_Row_Numbers ) || ( field_name == c_field_name_Print_Lists_With_Row_Numbers ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Application->Print_Lists_With_Row_Numbers( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Registration_Key ) || ( field_name == c_field_name_Registration_Key ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Application->Registration_Key( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Show_Inaccessible_Modules ) || ( field_name == c_field_name_Show_Inaccessible_Modules ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Application->Show_Inaccessible_Modules( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Type ) || ( field_name == c_field_name_Type ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Application->Type( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Check_Boxes_for_Bools ) || ( field_name == c_field_name_Use_Check_Boxes_for_Bools ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Application->Use_Check_Boxes_for_Bools( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Embedded_Images ) || ( field_name == c_field_name_Use_Embedded_Images ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Application->Use_Embedded_Images( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Script ) || ( field_name == c_field_name_Use_Script ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Application->Use_Script( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Use_TLS_Sessions ) || ( field_name == c_field_name_Use_TLS_Sessions ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Application->Use_TLS_Sessions( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Use_URL_Checksum ) || ( field_name == c_field_name_Use_URL_Checksum ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Application->Use_URL_Checksum( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Vertical_Menu ) || ( field_name == c_field_name_Use_Vertical_Menu ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Application->Use_Vertical_Menu( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Version ) || ( field_name == c_field_name_Version ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Application->Version( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Workgroup ) || ( field_name == c_field_name_Workgroup ) ) )
      {
         handled = true;

         string_getter< Meta_Workgroup >( cmd_handler.p_Meta_Application->Workgroup( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Year_Created ) || ( field_name == c_field_name_Year_Created ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Application->Year_Created( ), cmd_handler.retval );
      }

      if( !handled )
         throw runtime_error( "unknown field name '" + field_name + "' for getter call" );
   }
   else if( command == c_cmd_Meta_Application_set )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Application_set_field_name ) );
      string field_value( get_parm_val( parameters, c_cmd_Meta_Application_set_field_value ) );

      bool handled = false;

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for setter call" );

      if( !handled && ( ( field_name == c_field_id_Actions ) || ( field_name == c_field_name_Actions ) ) )
      {
         handled = true;

         func_string_setter< Meta_Application, string >(
          *cmd_handler.p_Meta_Application, &Meta_Application::Actions, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Add_Modules_Automatically ) || ( field_name == c_field_name_Add_Modules_Automatically ) ) )
      {
         handled = true;

         func_string_setter< Meta_Application, bool >(
          *cmd_handler.p_Meta_Application, &Meta_Application::Add_Modules_Automatically, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Duplicate_Logins ) || ( field_name == c_field_name_Allow_Duplicate_Logins ) ) )
      {
         handled = true;

         func_string_setter< Meta_Application, bool >(
          *cmd_handler.p_Meta_Application, &Meta_Application::Allow_Duplicate_Logins, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Module_Switching ) || ( field_name == c_field_name_Allow_Module_Switching ) ) )
      {
         handled = true;

         func_string_setter< Meta_Application, bool >(
          *cmd_handler.p_Meta_Application, &Meta_Application::Allow_Module_Switching, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Auto_Login_Days ) || ( field_name == c_field_name_Auto_Login_Days ) ) )
      {
         handled = true;

         func_string_setter< Meta_Application, int >(
          *cmd_handler.p_Meta_Application, &Meta_Application::Auto_Login_Days, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Blockchain_Id ) || ( field_name == c_field_name_Blockchain_Id ) ) )
      {
         handled = true;

         func_string_setter< Meta_Application, string >(
          *cmd_handler.p_Meta_Application, &Meta_Application::Blockchain_Id, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Create_Database ) || ( field_name == c_field_name_Create_Database ) ) )
      {
         handled = true;

         func_string_setter< Meta_Application, bool >(
          *cmd_handler.p_Meta_Application, &Meta_Application::Create_Database, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Created_Database ) || ( field_name == c_field_name_Created_Database ) ) )
      {
         handled = true;

         func_string_setter< Meta_Application, bool >(
          *cmd_handler.p_Meta_Application, &Meta_Application::Created_Database, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Creation_Script ) || ( field_name == c_field_name_Creation_Script ) ) )
      {
         handled = true;

         func_string_setter< Meta_Application, Meta_Application_Script >(
          *cmd_handler.p_Meta_Application, &Meta_Application::Creation_Script, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Default_Image_Height ) || ( field_name == c_field_name_Default_Image_Height ) ) )
      {
         handled = true;

         func_string_setter< Meta_Application, int >(
          *cmd_handler.p_Meta_Application, &Meta_Application::Default_Image_Height, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Default_Image_Width ) || ( field_name == c_field_name_Default_Image_Width ) ) )
      {
         handled = true;

         func_string_setter< Meta_Application, int >(
          *cmd_handler.p_Meta_Application, &Meta_Application::Default_Image_Width, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Default_List_Print_Row_Limit ) || ( field_name == c_field_name_Default_List_Print_Row_Limit ) ) )
      {
         handled = true;

         func_string_setter< Meta_Application, int >(
          *cmd_handler.p_Meta_Application, &Meta_Application::Default_List_Print_Row_Limit, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Default_List_Row_Limit ) || ( field_name == c_field_name_Default_List_Row_Limit ) ) )
      {
         handled = true;

         func_string_setter< Meta_Application, int >(
          *cmd_handler.p_Meta_Application, &Meta_Application::Default_List_Row_Limit, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Default_Max_Attached_File_Size ) || ( field_name == c_field_name_Default_Max_Attached_File_Size ) ) )
      {
         handled = true;

         func_string_setter< Meta_Application, int >(
          *cmd_handler.p_Meta_Application, &Meta_Application::Default_Max_Attached_File_Size, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Default_Multiline_Max_Rows ) || ( field_name == c_field_name_Default_Multiline_Max_Rows ) ) )
      {
         handled = true;

         func_string_setter< Meta_Application, int >(
          *cmd_handler.p_Meta_Application, &Meta_Application::Default_Multiline_Max_Rows, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Default_Multiline_Min_Rows ) || ( field_name == c_field_name_Default_Multiline_Min_Rows ) ) )
      {
         handled = true;

         func_string_setter< Meta_Application, int >(
          *cmd_handler.p_Meta_Application, &Meta_Application::Default_Multiline_Min_Rows, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Default_Multiline_Text_Limit ) || ( field_name == c_field_name_Default_Multiline_Text_Limit ) ) )
      {
         handled = true;

         func_string_setter< Meta_Application, int >(
          *cmd_handler.p_Meta_Application, &Meta_Application::Default_Multiline_Text_Limit, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Default_Multiline_Text_Trunc ) || ( field_name == c_field_name_Default_Multiline_Text_Trunc ) ) )
      {
         handled = true;

         func_string_setter< Meta_Application, int >(
          *cmd_handler.p_Meta_Application, &Meta_Application::Default_Multiline_Text_Trunc, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Encrypt_Dynamic_Content ) || ( field_name == c_field_name_Encrypt_Dynamic_Content ) ) )
      {
         handled = true;

         func_string_setter< Meta_Application, bool >(
          *cmd_handler.p_Meta_Application, &Meta_Application::Encrypt_Dynamic_Content, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Generate_Details ) || ( field_name == c_field_name_Generate_Details ) ) )
      {
         handled = true;

         func_string_setter< Meta_Application, string >(
          *cmd_handler.p_Meta_Application, &Meta_Application::Generate_Details, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Generate_Status ) || ( field_name == c_field_name_Generate_Status ) ) )
      {
         handled = true;

         func_string_setter< Meta_Application, string >(
          *cmd_handler.p_Meta_Application, &Meta_Application::Generate_Status, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Generate_Type ) || ( field_name == c_field_name_Generate_Type ) ) )
      {
         handled = true;

         func_string_setter< Meta_Application, int >(
          *cmd_handler.p_Meta_Application, &Meta_Application::Generate_Type, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Installing_Script ) || ( field_name == c_field_name_Installing_Script ) ) )
      {
         handled = true;

         func_string_setter< Meta_Application, bool >(
          *cmd_handler.p_Meta_Application, &Meta_Application::Installing_Script, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Keep_Existing_Data ) || ( field_name == c_field_name_Keep_Existing_Data ) ) )
      {
         handled = true;

         func_string_setter< Meta_Application, bool >(
          *cmd_handler.p_Meta_Application, &Meta_Application::Keep_Existing_Data, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Module_Prefix ) || ( field_name == c_field_name_Module_Prefix ) ) )
      {
         handled = true;

         func_string_setter< Meta_Application, string >(
          *cmd_handler.p_Meta_Application, &Meta_Application::Module_Prefix, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Name ) || ( field_name == c_field_name_Name ) ) )
      {
         handled = true;

         func_string_setter< Meta_Application, string >(
          *cmd_handler.p_Meta_Application, &Meta_Application::Name, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Print_Lists_With_Check_Boxes ) || ( field_name == c_field_name_Print_Lists_With_Check_Boxes ) ) )
      {
         handled = true;

         func_string_setter< Meta_Application, bool >(
          *cmd_handler.p_Meta_Application, &Meta_Application::Print_Lists_With_Check_Boxes, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Print_Lists_With_Row_Numbers ) || ( field_name == c_field_name_Print_Lists_With_Row_Numbers ) ) )
      {
         handled = true;

         func_string_setter< Meta_Application, bool >(
          *cmd_handler.p_Meta_Application, &Meta_Application::Print_Lists_With_Row_Numbers, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Registration_Key ) || ( field_name == c_field_name_Registration_Key ) ) )
      {
         handled = true;

         func_string_setter< Meta_Application, string >(
          *cmd_handler.p_Meta_Application, &Meta_Application::Registration_Key, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Show_Inaccessible_Modules ) || ( field_name == c_field_name_Show_Inaccessible_Modules ) ) )
      {
         handled = true;

         func_string_setter< Meta_Application, bool >(
          *cmd_handler.p_Meta_Application, &Meta_Application::Show_Inaccessible_Modules, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Type ) || ( field_name == c_field_name_Type ) ) )
      {
         handled = true;

         func_string_setter< Meta_Application, bool >(
          *cmd_handler.p_Meta_Application, &Meta_Application::Type, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Check_Boxes_for_Bools ) || ( field_name == c_field_name_Use_Check_Boxes_for_Bools ) ) )
      {
         handled = true;

         func_string_setter< Meta_Application, bool >(
          *cmd_handler.p_Meta_Application, &Meta_Application::Use_Check_Boxes_for_Bools, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Embedded_Images ) || ( field_name == c_field_name_Use_Embedded_Images ) ) )
      {
         handled = true;

         func_string_setter< Meta_Application, bool >(
          *cmd_handler.p_Meta_Application, &Meta_Application::Use_Embedded_Images, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Script ) || ( field_name == c_field_name_Use_Script ) ) )
      {
         handled = true;

         func_string_setter< Meta_Application, bool >(
          *cmd_handler.p_Meta_Application, &Meta_Application::Use_Script, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Use_TLS_Sessions ) || ( field_name == c_field_name_Use_TLS_Sessions ) ) )
      {
         handled = true;

         func_string_setter< Meta_Application, bool >(
          *cmd_handler.p_Meta_Application, &Meta_Application::Use_TLS_Sessions, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Use_URL_Checksum ) || ( field_name == c_field_name_Use_URL_Checksum ) ) )
      {
         handled = true;

         func_string_setter< Meta_Application, bool >(
          *cmd_handler.p_Meta_Application, &Meta_Application::Use_URL_Checksum, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Vertical_Menu ) || ( field_name == c_field_name_Use_Vertical_Menu ) ) )
      {
         handled = true;

         func_string_setter< Meta_Application, bool >(
          *cmd_handler.p_Meta_Application, &Meta_Application::Use_Vertical_Menu, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Version ) || ( field_name == c_field_name_Version ) ) )
      {
         handled = true;

         func_string_setter< Meta_Application, string >(
          *cmd_handler.p_Meta_Application, &Meta_Application::Version, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Workgroup ) || ( field_name == c_field_name_Workgroup ) ) )
      {
         handled = true;

         func_string_setter< Meta_Application, Meta_Workgroup >(
          *cmd_handler.p_Meta_Application, &Meta_Application::Workgroup, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Year_Created ) || ( field_name == c_field_name_Year_Created ) ) )
      {
         handled = true;

         func_string_setter< Meta_Application, int >(
          *cmd_handler.p_Meta_Application, &Meta_Application::Year_Created, field_value );
      }

      if( !handled )
         throw runtime_error( "unknown field name '" + field_name + "' for setter call" );

      cmd_handler.retval = c_okay;
   }
   else if( command == c_cmd_Meta_Application_cmd )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Application_cmd_field_name ) );
      string cmd_and_args( get_parm_val( parameters, c_cmd_Meta_Application_cmd_cmd_and_args ) );

      cmd_handler.retval.erase( );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for command call" );
      else if( ( field_name == c_field_id_Creation_Script ) || ( field_name == c_field_name_Creation_Script ) )
         cmd_handler.retval = cmd_handler.p_Meta_Application->Creation_Script( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Workgroup ) || ( field_name == c_field_name_Workgroup ) )
         cmd_handler.retval = cmd_handler.p_Meta_Application->Workgroup( ).execute( cmd_and_args );
      else
         throw runtime_error( "unknown field name '" + field_name + "' for command call" );
   }
   else if( command == c_cmd_Meta_Application_Generate )
   {
      cmd_handler.p_Meta_Application->Generate( );

      cmd_handler.retval.erase( );

   }
   else if( command == c_cmd_Meta_Application_Generate_File_Links )
   {
      cmd_handler.p_Meta_Application->Generate_File_Links( );

      cmd_handler.retval.erase( );

   }
   else if( command == c_cmd_Meta_Application_Generate_Modules )
   {
      cmd_handler.p_Meta_Application->Generate_Modules( );

      cmd_handler.retval.erase( );

   }
   else if( command == c_cmd_Meta_Application_Generate_Upgrade_DDL )
   {
      cmd_handler.p_Meta_Application->Generate_Upgrade_DDL( );

      cmd_handler.retval.erase( );

   }
   else if( command == c_cmd_Meta_Application_Test_Proc_1 )
   {
      string Output;

      cmd_handler.p_Meta_Application->Test_Proc_1( Output );

      cmd_handler.retval.erase( );

      append_value( cmd_handler.retval, Output );
   }
   else if( command == c_cmd_Meta_Application_Test_Proc_2 )
   {
      string Input( get_parm_val_from_string< string >( parameters, c_cmd_Meta_Application_Test_Proc_2_Input ) );
      string Output;

      cmd_handler.p_Meta_Application->Test_Proc_2( Input, Output );

      cmd_handler.retval.erase( );

      append_value( cmd_handler.retval, Output );
   }
}

struct Meta_Application::impl : public Meta_Application_command_handler
{
   impl( Meta_Application& o )
    :
    cp_obj( &o ),
    total_child_relationships( 0 )
   {
      p_obj = &o;
      set_Meta_Application( &o );

      add_commands( 0, Meta_Application_command_functor_factory,
       ARRAY_PTR_AND_SIZE( Meta_Application_command_definitions ) );
   }

   Meta_Application& get_obj( ) const
   {
      return *cp_obj;
   }

   const string& impl_Actions( ) const { return lazy_fetch( p_obj ), v_Actions; }
   void impl_Actions( const string& Actions ) { sanity_check( Actions ); v_Actions = Actions; }

   bool impl_Add_Modules_Automatically( ) const { return lazy_fetch( p_obj ), v_Add_Modules_Automatically; }
   void impl_Add_Modules_Automatically( bool Add_Modules_Automatically ) { v_Add_Modules_Automatically = Add_Modules_Automatically; }

   bool impl_Allow_Duplicate_Logins( ) const { return lazy_fetch( p_obj ), v_Allow_Duplicate_Logins; }
   void impl_Allow_Duplicate_Logins( bool Allow_Duplicate_Logins ) { v_Allow_Duplicate_Logins = Allow_Duplicate_Logins; }

   bool impl_Allow_Module_Switching( ) const { return lazy_fetch( p_obj ), v_Allow_Module_Switching; }
   void impl_Allow_Module_Switching( bool Allow_Module_Switching ) { v_Allow_Module_Switching = Allow_Module_Switching; }

   int impl_Auto_Login_Days( ) const { return lazy_fetch( p_obj ), v_Auto_Login_Days; }
   void impl_Auto_Login_Days( int Auto_Login_Days ) { v_Auto_Login_Days = Auto_Login_Days; }

   const string& impl_Blockchain_Id( ) const { return lazy_fetch( p_obj ), v_Blockchain_Id; }
   void impl_Blockchain_Id( const string& Blockchain_Id ) { sanity_check( Blockchain_Id ); v_Blockchain_Id = Blockchain_Id; }

   bool impl_Create_Database( ) const { return lazy_fetch( p_obj ), v_Create_Database; }
   void impl_Create_Database( bool Create_Database ) { v_Create_Database = Create_Database; }

   bool impl_Created_Database( ) const { return lazy_fetch( p_obj ), v_Created_Database; }
   void impl_Created_Database( bool Created_Database ) { v_Created_Database = Created_Database; }

   int impl_Default_Image_Height( ) const { return lazy_fetch( p_obj ), v_Default_Image_Height; }
   void impl_Default_Image_Height( int Default_Image_Height ) { v_Default_Image_Height = Default_Image_Height; }

   int impl_Default_Image_Width( ) const { return lazy_fetch( p_obj ), v_Default_Image_Width; }
   void impl_Default_Image_Width( int Default_Image_Width ) { v_Default_Image_Width = Default_Image_Width; }

   int impl_Default_List_Print_Row_Limit( ) const { return lazy_fetch( p_obj ), v_Default_List_Print_Row_Limit; }
   void impl_Default_List_Print_Row_Limit( int Default_List_Print_Row_Limit ) { v_Default_List_Print_Row_Limit = Default_List_Print_Row_Limit; }

   int impl_Default_List_Row_Limit( ) const { return lazy_fetch( p_obj ), v_Default_List_Row_Limit; }
   void impl_Default_List_Row_Limit( int Default_List_Row_Limit ) { v_Default_List_Row_Limit = Default_List_Row_Limit; }

   int impl_Default_Max_Attached_File_Size( ) const { return lazy_fetch( p_obj ), v_Default_Max_Attached_File_Size; }
   void impl_Default_Max_Attached_File_Size( int Default_Max_Attached_File_Size ) { v_Default_Max_Attached_File_Size = Default_Max_Attached_File_Size; }

   int impl_Default_Multiline_Max_Rows( ) const { return lazy_fetch( p_obj ), v_Default_Multiline_Max_Rows; }
   void impl_Default_Multiline_Max_Rows( int Default_Multiline_Max_Rows ) { v_Default_Multiline_Max_Rows = Default_Multiline_Max_Rows; }

   int impl_Default_Multiline_Min_Rows( ) const { return lazy_fetch( p_obj ), v_Default_Multiline_Min_Rows; }
   void impl_Default_Multiline_Min_Rows( int Default_Multiline_Min_Rows ) { v_Default_Multiline_Min_Rows = Default_Multiline_Min_Rows; }

   int impl_Default_Multiline_Text_Limit( ) const { return lazy_fetch( p_obj ), v_Default_Multiline_Text_Limit; }
   void impl_Default_Multiline_Text_Limit( int Default_Multiline_Text_Limit ) { v_Default_Multiline_Text_Limit = Default_Multiline_Text_Limit; }

   int impl_Default_Multiline_Text_Trunc( ) const { return lazy_fetch( p_obj ), v_Default_Multiline_Text_Trunc; }
   void impl_Default_Multiline_Text_Trunc( int Default_Multiline_Text_Trunc ) { v_Default_Multiline_Text_Trunc = Default_Multiline_Text_Trunc; }

   bool impl_Encrypt_Dynamic_Content( ) const { return lazy_fetch( p_obj ), v_Encrypt_Dynamic_Content; }
   void impl_Encrypt_Dynamic_Content( bool Encrypt_Dynamic_Content ) { v_Encrypt_Dynamic_Content = Encrypt_Dynamic_Content; }

   const string& impl_Generate_Details( ) const { return lazy_fetch( p_obj ), v_Generate_Details; }
   void impl_Generate_Details( const string& Generate_Details ) { sanity_check( Generate_Details ); v_Generate_Details = Generate_Details; }

   const string& impl_Generate_Status( ) const { return lazy_fetch( p_obj ), v_Generate_Status; }
   void impl_Generate_Status( const string& Generate_Status ) { sanity_check( Generate_Status ); v_Generate_Status = Generate_Status; }

   int impl_Generate_Type( ) const { return lazy_fetch( p_obj ), v_Generate_Type; }
   void impl_Generate_Type( int Generate_Type ) { v_Generate_Type = Generate_Type; }

   bool impl_Installing_Script( ) const { return lazy_fetch( p_obj ), v_Installing_Script; }
   void impl_Installing_Script( bool Installing_Script ) { v_Installing_Script = Installing_Script; }

   bool impl_Keep_Existing_Data( ) const { return lazy_fetch( p_obj ), v_Keep_Existing_Data; }
   void impl_Keep_Existing_Data( bool Keep_Existing_Data ) { v_Keep_Existing_Data = Keep_Existing_Data; }

   const string& impl_Module_Prefix( ) const { return lazy_fetch( p_obj ), v_Module_Prefix; }
   void impl_Module_Prefix( const string& Module_Prefix ) { sanity_check( Module_Prefix ); v_Module_Prefix = Module_Prefix; }

   const string& impl_Name( ) const { return lazy_fetch( p_obj ), v_Name; }
   void impl_Name( const string& Name ) { sanity_check( Name ); v_Name = Name; }

   bool impl_Print_Lists_With_Check_Boxes( ) const { return lazy_fetch( p_obj ), v_Print_Lists_With_Check_Boxes; }
   void impl_Print_Lists_With_Check_Boxes( bool Print_Lists_With_Check_Boxes ) { v_Print_Lists_With_Check_Boxes = Print_Lists_With_Check_Boxes; }

   bool impl_Print_Lists_With_Row_Numbers( ) const { return lazy_fetch( p_obj ), v_Print_Lists_With_Row_Numbers; }
   void impl_Print_Lists_With_Row_Numbers( bool Print_Lists_With_Row_Numbers ) { v_Print_Lists_With_Row_Numbers = Print_Lists_With_Row_Numbers; }

   const string& impl_Registration_Key( ) const { return lazy_fetch( p_obj ), v_Registration_Key; }
   void impl_Registration_Key( const string& Registration_Key ) { sanity_check( Registration_Key ); v_Registration_Key = Registration_Key; }

   bool impl_Show_Inaccessible_Modules( ) const { return lazy_fetch( p_obj ), v_Show_Inaccessible_Modules; }
   void impl_Show_Inaccessible_Modules( bool Show_Inaccessible_Modules ) { v_Show_Inaccessible_Modules = Show_Inaccessible_Modules; }

   bool impl_Type( ) const { return lazy_fetch( p_obj ), v_Type; }
   void impl_Type( bool Type ) { v_Type = Type; }

   bool impl_Use_Check_Boxes_for_Bools( ) const { return lazy_fetch( p_obj ), v_Use_Check_Boxes_for_Bools; }
   void impl_Use_Check_Boxes_for_Bools( bool Use_Check_Boxes_for_Bools ) { v_Use_Check_Boxes_for_Bools = Use_Check_Boxes_for_Bools; }

   bool impl_Use_Embedded_Images( ) const { return lazy_fetch( p_obj ), v_Use_Embedded_Images; }
   void impl_Use_Embedded_Images( bool Use_Embedded_Images ) { v_Use_Embedded_Images = Use_Embedded_Images; }

   bool impl_Use_Script( ) const { return lazy_fetch( p_obj ), v_Use_Script; }
   void impl_Use_Script( bool Use_Script ) { v_Use_Script = Use_Script; }

   bool impl_Use_TLS_Sessions( ) const { return lazy_fetch( p_obj ), v_Use_TLS_Sessions; }
   void impl_Use_TLS_Sessions( bool Use_TLS_Sessions ) { v_Use_TLS_Sessions = Use_TLS_Sessions; }

   bool impl_Use_URL_Checksum( ) const { return lazy_fetch( p_obj ), v_Use_URL_Checksum; }
   void impl_Use_URL_Checksum( bool Use_URL_Checksum ) { v_Use_URL_Checksum = Use_URL_Checksum; }

   bool impl_Use_Vertical_Menu( ) const { return lazy_fetch( p_obj ), v_Use_Vertical_Menu; }
   void impl_Use_Vertical_Menu( bool Use_Vertical_Menu ) { v_Use_Vertical_Menu = Use_Vertical_Menu; }

   const string& impl_Version( ) const { return lazy_fetch( p_obj ), v_Version; }
   void impl_Version( const string& Version ) { sanity_check( Version ); v_Version = Version; }

   int impl_Year_Created( ) const { return lazy_fetch( p_obj ), v_Year_Created; }
   void impl_Year_Created( int Year_Created ) { v_Year_Created = Year_Created; }

   Meta_Application_Script& impl_Creation_Script( )
   {
      if( !cp_Creation_Script )
      {
         cp_Creation_Script.init( );

         p_obj->setup_graph_parent( *cp_Creation_Script, c_field_id_Creation_Script, v_Creation_Script );
      }
      return *cp_Creation_Script;
   }

   const Meta_Application_Script& impl_Creation_Script( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Creation_Script )
      {
         cp_Creation_Script.init( );

         p_obj->setup_graph_parent( *cp_Creation_Script, c_field_id_Creation_Script, v_Creation_Script );
      }
      return *cp_Creation_Script;
   }

   void impl_Creation_Script( const string& key )
   {
      class_base_accessor cba( impl_Creation_Script( ) );
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

   Meta_Module& impl_child_Module( )
   {
      if( !cp_child_Module )
      {
         cp_child_Module.init( );

         p_obj->setup_graph_parent( *cp_child_Module, "302210" );
      }
      return *cp_child_Module;
   }

   const Meta_Module& impl_child_Module( ) const
   {
      if( !cp_child_Module )
      {
         cp_child_Module.init( );

         p_obj->setup_graph_parent( *cp_child_Module, "302210" );
      }
      return *cp_child_Module;
   }

   void impl_Generate( );

   void impl_Generate_File_Links( );

   void impl_Generate_Modules( );

   void impl_Generate_Upgrade_DDL( );

   void impl_Test_Proc_1( string& Output );

   void impl_Test_Proc_2( const string& Input, string& Output );

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

   Meta_Application* p_obj;
   class_pointer< Meta_Application > cp_obj;

   mutable set< string > required_transients;

   // [<start members>]
   // [<finish members>]

   size_t total_child_relationships;

   string v_Actions;
   bool v_Add_Modules_Automatically;
   bool v_Allow_Duplicate_Logins;
   bool v_Allow_Module_Switching;
   int v_Auto_Login_Days;
   string v_Blockchain_Id;
   bool v_Create_Database;
   bool v_Created_Database;
   int v_Default_Image_Height;
   int v_Default_Image_Width;
   int v_Default_List_Print_Row_Limit;
   int v_Default_List_Row_Limit;
   int v_Default_Max_Attached_File_Size;
   int v_Default_Multiline_Max_Rows;
   int v_Default_Multiline_Min_Rows;
   int v_Default_Multiline_Text_Limit;
   int v_Default_Multiline_Text_Trunc;
   bool v_Encrypt_Dynamic_Content;
   string v_Generate_Details;
   string v_Generate_Status;
   int v_Generate_Type;
   bool v_Installing_Script;
   bool v_Keep_Existing_Data;
   string v_Module_Prefix;
   string v_Name;
   bool v_Print_Lists_With_Check_Boxes;
   bool v_Print_Lists_With_Row_Numbers;
   string v_Registration_Key;
   bool v_Show_Inaccessible_Modules;
   bool v_Type;
   bool v_Use_Check_Boxes_for_Bools;
   bool v_Use_Embedded_Images;
   bool v_Use_Script;
   bool v_Use_TLS_Sessions;
   bool v_Use_URL_Checksum;
   bool v_Use_Vertical_Menu;
   string v_Version;
   int v_Year_Created;

   string v_Creation_Script;
   mutable class_pointer< Meta_Application_Script > cp_Creation_Script;

   string v_Workgroup;
   mutable class_pointer< Meta_Workgroup > cp_Workgroup;

   mutable class_pointer< Meta_Module > cp_child_Module;
};

void Meta_Application::impl::impl_Generate( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start Generate_impl>]
//nyi
   // NOTE: If a recovery is in progress then do nothing.
   if( storage_locked_for_admin( ) )
      return;

   if( get_obj( ).child_Module( ).iterate_forwards( ) )
      get_obj( ).child_Module( ).iterate_stop( );
   else
      throw runtime_error( "Generate requires at least one Module." );

   string generate_log_file( get_raw_session_variable(
    get_special_var_name( e_special_var_generate_log_file ) ) );

   bool skip_exec = !generate_log_file.empty( );

   if( !skip_exec )
      set_system_variable( "@" + storage_name( ) + "_protect", "1" );

   bool async = true;

   if( get_obj( ).get_variable( get_special_var_name( e_special_var_async ) ) == "0"
    || get_obj( ).get_variable( get_special_var_name( e_special_var_async ) ) == "false"
    || get_session_variable( get_special_var_name( e_special_var_allow_async ) ) == "0"
    || get_session_variable( get_special_var_name( e_special_var_allow_async ) ) == "false" )
      async = false;

   // NOTE: The UI allows this to be set so use this as the value during
   // the generate but put the value back to its last saved value after.
   int gen_type = get_obj( ).Generate_Type( );
   bool keep_data = get_obj( ).Keep_Existing_Data( );

   get_obj( ).op_update( );

   int orig_gen_type = get_obj( ).Generate_Type( );
   bool orig_keep_data = get_obj( ).Keep_Existing_Data( );

   get_obj( ).Generate_Type( gen_type );
   get_obj( ).Keep_Existing_Data( keep_data );

   string generate_script( get_obj( ).Name( ) + ".generate" );

   try
   {
      string app_dir( lower( get_obj( ).Name( ) ) );
      string web_dir_var( "$WEBDIR" );
      string app_vars( get_obj( ).Name( ) + ".app.vars.xrep" );
      string modules_list( get_obj( ).Name( ) + ".modules.lst" );

      set< string > old_modules;

      if( exists_file( modules_list ) )
         read_file_lines( modules_list, old_modules );

      if( generate_log_file.empty( ) )
      {
         generate_log_file = get_obj( ).Name( ) + ".generate.log";

         file_remove( generate_log_file );
      }

      string stage_script_1( get_obj( ).Name( ) + ".generate.1.cin" );
      string stage_script_2( get_obj( ).Name( ) + ".generate.2.cin" );
      string stage_script_3( get_obj( ).Name( ) + ".generate.3.cin" );
      string stage_script_4( get_obj( ).Name( ) + ".generate.4.cin" );
      string stage_script_x( get_obj( ).Name( ) + ".generate.x.cin" );

      string upgrade_script( get_obj( ).Name( ) + ".upgrade.cin" );

      ofstream outv( app_vars.c_str( ) );
      if( !outv )
         throw runtime_error( "unexpected error opening '" + app_vars + "' for output" );

      ofstream outm( modules_list.c_str( ) );
      if( !outm )
         throw runtime_error( "unexpected error opening '" + modules_list + "' for output" );

      ofstream outs( generate_script.c_str( ) );
      if( !outs )
         throw runtime_error( "unexpected error opening '" + generate_script + "' for output" );

      ofstream outss1( stage_script_1.c_str( ) );
      if( !outss1 )
         throw runtime_error( "unexpected error opening '" + stage_script_1 + "' for output" );

      ofstream outss2( stage_script_2.c_str( ) );
      if( !outss2 )
         throw runtime_error( "unexpected error opening '" + stage_script_2 + "' for output" );

      ofstream outss3( stage_script_3.c_str( ) );
      if( !outss3 )
         throw runtime_error( "unexpected error opening '" + stage_script_3 + "' for output" );

      ofstream outss4( stage_script_4.c_str( ) );
      if( !outss4 )
         throw runtime_error( "unexpected error opening '" + stage_script_4 + "' for output" );

      ofstream outssx( stage_script_x.c_str( ) );
      if( !outssx )
         throw runtime_error( "unexpected error opening '" + stage_script_x + "' for output" );

      ofstream outupg( upgrade_script.c_str( ) );
      if( !outupg )
         throw runtime_error( "unexpected error opening '" + upgrade_script + "' for output" );

      outs << "#!/bin/bash\n\n";

      outs << "export WEBDIR=" << get_web_root( ) << "\n\n";
      outs << "echo Starting Generate... >" << generate_log_file << "\n";

      outs << "\necho \"" << get_obj( ).Name( ) << "\" > .app_name\n\n";

      // NOTE: The 'identity.txt' and 'encrypted.txt' files are created as symbolic links
      // in order to simplify a development environment (so that the same files are being
      // used for unlocking the system identity by all applications and Meta).
      outs << "if [ ! -d " << web_dir_var << "/" << app_dir << " ]; then\n"
       << " ./setup " << get_obj( ).Name( ) << " " << app_dir << " >>" << generate_log_file
       << "\nln -s $WEBDIR/" << c_meta_app_directory << "/identity.txt $WEBDIR/" << app_dir
       << "\nln -s $WEBDIR/" << c_meta_app_directory << "/encrypted.txt $WEBDIR/" << app_dir
       << "\nfi\n\n";

      string standard_client_args( "-quiet -no_prompt" );

      string rpc_password( get_rpc_password( ) );

      if( !rpc_password.empty( ) )
         standard_client_args += " -rpc_unlock=" + rpc_password;

      outs << "touch $WEBDIR/" << app_dir << "/ciyam_interface.stop\n";

      outv << "\x60{\x60$year_created\x60=\x60'" << get_obj( ).Year_Created( ) << "\x60'\x60}\n";

      string url_opts;

      if( get_obj( ).Use_URL_Checksum( ) )
         url_opts = "use_checksum";

      outv << "\x60{\x60$url_opts\x60=\x60'" << url_opts << "\x60'\x60}\n";

      outv << "\x60{\x60$row_limit\x60=\x60'" << get_obj( ).Default_List_Row_Limit( ) << "\x60'\x60}\n";

      string menu_opts;

      if( get_obj( ).Use_Vertical_Menu( ) )
         menu_opts = "use_vertical_menu";

      if( get_obj( ).Show_Inaccessible_Modules( ) )
      {
         if( !menu_opts.empty( ) )
            menu_opts += '+';
         menu_opts = "show_inaccessible_modules";
      }

      outv << "\x60{\x60$menu_opts\x60=\x60'" << menu_opts << "\x60'\x60}\n";

      outv << "\x60{\x60$login_days\x60=\x60'" << get_obj( ).Auto_Login_Days( ) << "\x60'\x60}\n";

      string login_opts;

      if( get_obj( ).Allow_Duplicate_Logins( ) )
         login_opts = "allow_multiple";

      if( get_obj( ).Allow_Module_Switching( ) )
      {
         if( !login_opts.empty( ) )
            login_opts += '+';
         login_opts += "allow_switching";
      }

      outv << "\x60{\x60$login_opts\x60=\x60'" << login_opts << "\x60'\x60}\n";

      outv << "\x60{\x60$notes_rmin\x60=\x60'" << get_obj( ).Default_Multiline_Min_Rows( ) << "\x60'\x60}\n";
      outv << "\x60{\x60$notes_rmax\x60=\x60'" << get_obj( ).Default_Multiline_Max_Rows( ) << "\x60'\x60}\n";
      outv << "\x60{\x60$notes_trunc\x60=\x60'" << get_obj( ).Default_Multiline_Text_Trunc( ) << "\x60'\x60}\n";
      outv << "\x60{\x60$notes_limit\x60=\x60'" << get_obj( ).Default_Multiline_Text_Limit( ) << "\x60'\x60}\n";

      outv << "\x60{\x60$print_limit\x60=\x60'" << get_obj( ).Default_List_Print_Row_Limit( ) << "\x60'\x60}\n";

      outv << "\x60{\x60$filesize_limit\x60=\x60'" << get_obj( ).Default_Max_Attached_File_Size( ) << "\x60'\x60}\n";

      string use_tls( "false" );

      if( get_obj( ).Use_TLS_Sessions( ) )
         use_tls = "true";

      string embed_images( "false" );

      if( get_obj( ).Use_Embedded_Images( ) )
         embed_images = "true";

      string encrypt_data( "false" );

      if( get_obj( ).Encrypt_Dynamic_Content( ) )
         encrypt_data = "true";

      string checkbox_bools( "false" );

      if( get_obj( ).Use_Check_Boxes_for_Bools( ) )
         checkbox_bools = "true";

      string print_list_ops;

      if( get_obj( ).Print_Lists_With_Check_Boxes( ) )
         print_list_ops = "show_checks";

      if( get_obj( ).Print_Lists_With_Row_Numbers( ) )
      {
         if( !print_list_ops.empty( ) )
            print_list_ops += '+';
         print_list_ops += "show_numbers";
      }

      outv << "\x60{\x60$print_list_ops\x60=\x60'" << print_list_ops << "\x60'\x60}\n";

      outv << "\x60{\x60$use_tls\x60=\x60'" << use_tls << "\x60'\x60}\n";

      outv << "\x60{\x60$blockchain\x60=\x60'" << get_obj( ).Blockchain_Id( ) << "\x60'\x60}\n";

      outv << "\x60{\x60$image_width\x60=\x60'" << get_obj( ).Default_Image_Width( ) << "\x60'\x60}\n";
      outv << "\x60{\x60$image_height\x60=\x60'" << get_obj( ).Default_Image_Height( ) << "\x60'\x60}\n";

      outv << "\x60{\x60$embed_images\x60=\x60'" << embed_images << "\x60'\x60}\n";
      outv << "\x60{\x60$encrypt_data\x60=\x60'" << encrypt_data << "\x60'\x60}\n";

      outv << "\x60{\x60$rpc_password\x60=\x60'" << rpc_password << "\x60'\x60}\n";

      outv << "\x60{\x60$checkbox_bools\x60=\x60'" << checkbox_bools << "\x60'\x60}\n";

      outv << "\x60{\x60$storage_name\x60=\x60'" << get_obj( ).Name( ) << "\x60'\x60}\n";
      outv << "\x60{\x60$module_prefix\x60=\x60'" << get_obj( ).Module_Prefix( ) << "\x60'\x60}\n";

      string all_modules;

      vector< string > modules;

      set< string > active_modules;
      map< string, string > module_packages;

      string key_info( FIELD_ID( Meta, Module, Order ) );

      key_info += ' ';

      if( get_obj( ).child_Module( ).iterate_forwards( key_info ) )
      {
         do
         {
            bool is_active = false;

            if( get_obj( ).child_Module( ).Model( ).child_Package( ).iterate_forwards( ) )
            {
               do
               {
                  module_packages.insert(
                   make_pair( get_obj( ).child_Module( ).Model( ).child_Package( ).Name( ),
                   get_obj( ).child_Module( ).Model( ).child_Package( ).Type_Name( ) ) );
               } while( get_obj( ).child_Module( ).Model( ).child_Package( ).iterate_next( ) );
            }

            if( old_modules.count( get_obj( ).child_Module( ).Model( ).Name( ) ) )
               old_modules.erase( get_obj( ).child_Module( ).Model( ).Name( ) );

            if( get_obj( ).child_Module( ).Model( ).child_View( ).iterate_forwards( ) )
            {
               is_active = true;
               get_obj( ).child_Module( ).Model( ).child_View( ).iterate_stop( );
            }
            else if( get_obj( ).child_Module( ).Model( ).child_List( ).iterate_forwards( ) )
            {
               is_active = true;
               get_obj( ).child_Module( ).Model( ).child_List( ).iterate_stop( );
            }

            if( is_active )
               active_modules.insert( get_obj( ).child_Module( ).Model( ).Name( ) );

            if( !all_modules.empty( ) )
               all_modules += ' ';

            all_modules += get_obj( ).child_Module( ).Model( ).Name( );

            outm << get_obj( ).child_Module( ).Model( ).Name( ) << '\n';

            modules.push_back( get_obj( ).child_Module( ).Model( ).Name( ) );

         } while( get_obj( ).child_Module( ).iterate_next( ) );
      }

      vector< string > package_training;

      if( !module_packages.empty( ) )
      {
         outs << "if [ -f " << web_dir_var << "/" << app_dir << "/extkeys.txt ]; then\n"
          << " rm " << web_dir_var << "/" << app_dir << "/extkeys.txt"
          << "\nfi\n\n";

         for( map< string, string >::iterator i = module_packages.begin( ); i != module_packages.end( ); ++i )
         {
            outs << "if [ -f " << i->second << "_extkeys.txt ]; then\n"
             << " cat " << i->second << "_extkeys.txt >>" << web_dir_var << "/" << app_dir << "/extkeys.txt"
             << "\nfi\n\n";

            string algo_training_script( "train_" + lower( i->second ) + "_algos.cin" );

            if( exists_file( algo_training_script ) )
               package_training.push_back( algo_training_script );
         }
      }

      for( map< string, string >::iterator i = module_packages.begin( ); i != module_packages.end( ); ++i )
      {
         outs << "./copy_icons " << i->first << " "
          << i->second << " " << app_dir << " " << web_dir_var << " >>" << generate_log_file << "\n";
      }

      if( !module_packages.empty( ) )
         outs << '\n';

      outv << "\x60{\x60$modules\x60=\x60'" << all_modules << "\x60'\x60}\n";

      outss1 << "storage_init " << storage_name( ) << "\n";

      outss1 << "pe sys " << c_ciyam_dummy_date << " " << get_obj( ).get_module_id( )
       << " " << get_obj( ).get_class_id( ) << " " << get_obj( ).get_key( )
       << " -" << to_string( c_procedure_id_Generate_Modules ) << "\n";

      outss1 << "pu sys " << c_ciyam_dummy_date << " " << get_obj( ).get_module_id( )
       << " " << get_obj( ).get_class_id( ) << " " << get_obj( ).get_key( ) << " \""
       << get_obj( ).static_get_field_id( e_field_id_Generate_Status ) << "=Generating Source...\"\n";

      outss1 << "quit\n";

      if( get_obj( ).Generate_Type( ) < c_enum_app_generate_type_Application_Settings )
         outs << "./ciyam_client " << standard_client_args << " -no_stderr < "
          << get_obj( ).Name( ) << ".generate.1.cin >>" << generate_log_file << "\n";

      if( get_obj( ).Generate_Type( ) < c_enum_app_generate_type_Application_Settings )
      {
         for( size_t i = 0; i < modules.size( ); i++ )
         {
            string module_alias( modules.size( ) == 1 ? string( "index" ) : modules[ i ] );

            if( !get_obj( ).Module_Prefix( ).empty( ) && modules[ i ].find( get_obj( ).Module_Prefix( ) ) == 0 )
               module_alias.erase( 0, get_obj( ).Module_Prefix( ).size( ) );

            if( !active_modules.count( modules[ i ] ) )
               outs << "./genmodule -rdbms " << modules[ i ]
                << " " << app_dir << " >>" << generate_log_file << " 2>&1\n";
            else
               outs << "./genmodule -rdbms " << modules[ i ]
                << " " << app_dir << " " << lower( module_alias ) << " >>" << generate_log_file << " 2>&1\n";
         }
      }
      outs << "./genfcgi " << get_obj( ).Name( ) << " " << app_dir << " >>" << generate_log_file << "\n";

      for( set< string >::iterator i = old_modules.begin( ); i != old_modules.end( ); ++i )
      {
         if( !get_obj( ).Created_Database( ) )
            outs << "./remove_app_module " << *i << " " << app_dir << " >>" << generate_log_file << "\n";
         else
            outs << "./remove_app_module " << *i << " " << app_dir << " " << get_obj( ).Name( ) << " >>" << generate_log_file << "\n";

         if( get_obj( ).Created_Database( ) )
         {
            string class_list( *i + ".classes.lst" );

            if( exists_file( class_list ) )
            {
               set< string > old_class_names;

               read_file_lines( class_list, old_class_names );

               string obsolete_ddl( get_obj( ).Name( ) + ".obsolete.sql" );

               ofstream outf( obsolete_ddl.c_str( ), ios::out | ios::app );

               if( !outf )
                  throw runtime_error( "unable to open '" + obsolete_ddl + "' for output" );

               for( set< string >::iterator j = old_class_names.begin( ); j != old_class_names.end( ); ++j )
                  outf << "DROP TABLE IF EXISTS T_" << *i << '_' << *j << ";\n";

               outf.flush( );

               if( !outf.good( ) )
                  throw runtime_error( "file output stream '" + obsolete_ddl + "' is bad" );
            }
         }
      }

      outss2 << "storage_init " << storage_name( ) << "\n";

      outss2 << "pu sys " << c_ciyam_dummy_date << " " << get_obj( ).get_module_id( )
       << " " << get_obj( ).get_class_id( ) << " " << get_obj( ).get_key( ) << " \""
       << get_obj( ).static_get_field_id( e_field_id_Generate_Status ) << "=Updating Links...\"\n";

      outss2 << "pe sys " << c_ciyam_dummy_date << " " << get_obj( ).get_module_id( )
       << " " << get_obj( ).get_class_id( ) << " " << get_obj( ).get_key( )
       << " -" << to_string( c_procedure_id_Generate_File_Links ) << "\n";

      outss2 << ".quit\n";

      if( get_obj( ).Generate_Type( ) < c_enum_app_generate_type_Application_Settings )
      {
         outs << "\necho Updating Links... >>" << generate_log_file << "\n";
         outs << "./ciyam_client " << standard_client_args << " -no_stderr < " << get_obj( ).Name( ) << ".generate.2.cin\n";
      }

      outss3 << "storage_init " << storage_name( ) << "\n";

      outss3 << "pu sys " << c_ciyam_dummy_date << " " << get_obj( ).get_module_id( )
       << " " << get_obj( ).get_class_id( ) << " " << get_obj( ).get_key( ) << " \""
       << get_obj( ).static_get_field_id( e_field_id_Generate_Status ) << "=Performing Make...\"\n";

      outss3 << ".quit\n";

      if( get_obj( ).Generate_Type( ) < c_enum_app_generate_type_Application_Settings )
      {
         outs << "\necho Starting Make... >>" << generate_log_file << "\n";

         outs << "./ciyam_client " << standard_client_args << " -no_stderr < " << get_obj( ).Name( ) << ".generate.3.cin\n";

         outs << "if [ -f make.dtm ]; then\n";
         outs << " rm make.dtm\n";
         outs << "fi\n";

         outs << "make " << all_modules << " dtm >>" << generate_log_file << " 2>&1\n";
         outs << "echo Finished Make... >>" << generate_log_file << "\n\n";

         outs << "if [ -f make.dtm ]; then\n";

         for( size_t i = 0; i < modules.size( ); i++ )
         {
            outs << " if [ -f " << modules[ i ] << ".cleanup.sh ]; then\n";
            outs << "  chmod a+x " << modules[ i ] << ".cleanup.sh\n";
            outs << "  ./" << modules[ i ] << ".cleanup.sh\n";
            outs << "  rm " << modules[ i ] << ".cleanup.sh\n";
            outs << " fi\n";
         }

         outs << "fi\n\n";
      }

      outss4 << "storage_init " << storage_name( ) << "\n";

      outss4 << "pe sys " << c_ciyam_dummy_date << " " << get_obj( ).get_module_id( )
       << " " << get_obj( ).get_class_id( ) << " " << get_obj( ).get_key( )
       << " " << to_string( c_procedure_id_Generate_Upgrade_DDL ) << "\n";

      outss4 << "pu sys " << c_ciyam_dummy_date << " " << get_obj( ).get_module_id( )
       << " " << get_obj( ).get_class_id( ) << " " << get_obj( ).get_key( ) << " \""
       << get_obj( ).static_get_field_id( e_field_id_Generate_Status ) << "=Upgrading DB...\"\n";
      outss4 << "quit\n";

      outupg << "#Starting DB Rebuild...\n";

      outupg << "CIYAM_STORAGE=" << get_obj( ).Name( ) << "\n";
      outupg << "CIYAM_OUTPUT_COMMANDS=1\n";

      if( !keep_data )
         outupg << "CIYAM_SKIP_EXPORT_DATA=1\n";

      outupg << "<restore.cin\n";

      outupg << "#Finished DB Rebuild...\n";

      if( !package_training.empty( ) )
      {
         outupg << "#Starting algo training...\n";

         for( size_t i = 0; i < package_training.size( ); i++ )
            outupg << "<" << package_training[ i ] << "\n";

         string algos_file_name( get_obj( ).Name( ) + ".algos.lst" );

         outupg << ".session_variable @algos \"save " + algos_file_name + "\"\n";
         outupg << ".session_variable @algos \"kill *\"\n";

         outupg << "#Finished algo training...\n";
      }

      outupg << ".quit\n";

      if( get_obj( ).Generate_Type( ) < c_enum_app_generate_type_Skip_DB_Upgrade )
      {
         outs << "if [ -f make.dtm ]; then\n";

         if( !get_obj( ).Keep_Existing_Data( ) )
         {
            outs << " if [ -f " << get_obj( ).Name( ) << ".log ]; then\n";
            outs << "  rm " << get_obj( ).Name( ) << ".log\n";
            outs << " fi\n";
            outs << " if [ -f " << get_obj( ).Name( ) << c_dead_keys_suffix << " ]; then\n";
            outs << "  rm " << get_obj( ).Name( ) << c_dead_keys_suffix << "\n";
            outs << " fi\n";
         }

         outs << " if [ ! -f " << get_obj( ).Name( ) << ".log ]; then\n";
         outs << "  echo \"[0]" << uuid( ).as_string( ) << "\" > " << get_obj( ).Name( ) << ".log\n";
         outs << "  echo \"[1];module ==> none\" >> " << get_obj( ).Name( ) << ".log\n";
         outs << "  echo \"[2];dummy init\" >> " << get_obj( ).Name( ) << ".log\n";
         outs << " fi\n";

         outs << " if [ -f autoscript.sio.new ]; then\n";
         outs << "  ./update autoscript.sio autoscript.sio.new >>" << generate_log_file << "\n";
         outs << " fi\n";

         outs << " if [ -f manuscript.sio.new ]; then\n";
         outs << "  ./update manuscript.sio manuscript.sio.new >>" << generate_log_file << "\n";
         outs << " fi\n";

         outs << " ./ciyam_client " << standard_client_args << " -no_stderr < " << get_obj( ).Name( ) << ".generate.4.cin\n";

         outs << " ./ciyam_client -echo " << standard_client_args << " -no_stderr < " << get_obj( ).Name( ) << ".upgrade.cin >>" << generate_log_file << "\n";

         outs << "fi\n";
      }

      outssx << ">" << get_obj( ).Name( ) << ".map.new\n";
      outssx << "<output_all_mapped_meta_data.cin\n";
      outssx << ">\n";

      outssx << "storage_init " << storage_name( ) << "\n";

      outssx << "pu sys " << c_ciyam_dummy_date << " " << get_obj( ).get_module_id( )
       << " " << get_obj( ).get_class_id( ) << " " << get_obj( ).get_key( ) << " \""
       << get_obj( ).static_get_field_id( e_field_id_Actions ) << "=127410,"
       << get_obj( ).static_get_field_id( e_field_id_Generate_Type ) << "=" << to_string( orig_gen_type ) << ","
       << get_obj( ).static_get_field_id( e_field_id_Keep_Existing_Data ) << "=" << to_string( orig_keep_data ) << ","
       << get_obj( ).static_get_field_id( e_field_id_Generate_Status ) << "=Generated\"\n";

      outssx << "system_variable @" << storage_name( ) << "_protect \"\"\n";

      if( async )
         outssx << ".session_lock -release -at_term " << session_id( ) << "\n"; // see NOTE below...

      outssx << ".quit\n";

      outs << "\n./ciyam_client -quiet " << standard_client_args << " < " << get_obj( ).Name( ) << ".generate.x.cin\n";

      outs << "\nrm $WEBDIR/" << app_dir << "/ciyam_interface.stop\n";
      outs << "echo Finished Generate... >>" << generate_log_file << "\n";

      outv.flush( );
      if( !outv.good( ) )
         throw runtime_error( "app vars output stream is bad" );

      outm.flush( );
      if( !outm.good( ) )
         throw runtime_error( "module list output stream is bad" );

      outs.flush( );
      if( !outs.good( ) )
         throw runtime_error( "module script output stream is bad" );

      outss1.flush( );
      if( !outss1.good( ) )
         throw runtime_error( "stage script 1 output stream is bad" );

      outss2.flush( );
      if( !outss2.good( ) )
         throw runtime_error( "stage script 2 output stream is bad" );

      outss3.flush( );
      if( !outss3.good( ) )
         throw runtime_error( "stage script 3 output stream is bad" );

      outss4.flush( );
      if( !outss4.good( ) )
         throw runtime_error( "stage script 4 output stream is bad" );

      outssx.flush( );
      if( !outss3.good( ) )
         throw runtime_error( "stage script x output stream is bad" );

      outupg.flush( );
      if( !outupg.good( ) )
         throw runtime_error( "upgrade script output stream is bad" );

      if( skip_exec )
         get_obj( ).Generate_Status( "Generated Script" );
      else
      {
         get_obj( ).Actions( "" );
         get_obj( ).Generate_Status( "Generating Metadata..." );
      }

      get_obj( ).op_apply( );
   }
   catch( ... )
   {
      set_system_variable( "@" + storage_name( ) + "_protect", "" );
      throw;
   }

   // NOTE: If the thread that has spawned the child process is terminated (due
   // to client deciding to finish its session) then this can potentially cause
   // big troubles due to resource inheritance so the session is captured prior
   // to the async request and will be released at the end of the script.
   if( async && !skip_exec )
      capture_session( session_id( ) );

   chmod( generate_script.c_str( ), 0777 );

   if( !skip_exec )
      exec_system( "./" + generate_script, async );
   // [<finish Generate_impl>]
}

void Meta_Application::impl::impl_Generate_File_Links( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start Generate_File_Links_impl>]
//nyi
   // NOTE: If a recovery is in progress then do nothing.
   if( storage_locked_for_admin( ) )
      return;

   generate_new_script_sio_files( );

   if( get_obj( ).child_Module( ).iterate_forwards( ) )
   {
      do
      {
         get_obj( ).child_Module( ).Model( ).Generate_File_Links( );
      } while( get_obj( ).child_Module( ).iterate_next( ) );
   }
   // [<finish Generate_File_Links_impl>]
}

void Meta_Application::impl::impl_Generate_Modules( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start Generate_Modules_impl>]
//nyi
   // NOTE: If a recovery is in progress then do nothing.
   if( storage_locked_for_admin( ) )
      return;

   string key_info( FIELD_ID( Meta, Module, Order ) );

   key_info += ' ';

   if( get_obj( ).child_Module( ).iterate_forwards( key_info ) )
   {
      do
      {
         get_obj( ).child_Module( ).Model( ).Generate( );

      } while( get_obj( ).child_Module( ).iterate_next( ) );
   }
   // [<finish Generate_Modules_impl>]
}

void Meta_Application::impl::impl_Generate_Upgrade_DDL( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start Generate_Upgrade_DDL_impl>]
//nyi
   // NOTE: If a recovery is in progress then do nothing.
   if( storage_locked_for_admin( ) )
      return;

   vector< string > modules;

   string key_info( FIELD_ID( Meta, Module, Order ) );

   key_info += ' ';

   if( get_obj( ).child_Module( ).iterate_forwards( key_info ) )
   {
      do
      {
         modules.push_back( get_obj( ).child_Module( ).Model( ).Name( ) );
      } while( get_obj( ).child_Module( ).iterate_next( ) );
   }

   bool app_is_module = false;

   if( modules.size( ) == 1 && modules[ 0 ] == get_obj( ).Name( ) )
      app_is_module = true;

   string upgrade_ddl( get_obj( ).Name( ) + ".upgrade.sql" );

   if( exists_file( upgrade_ddl ) )
      remove_file( upgrade_ddl );

   string obsolete_ddl( get_obj( ).Name( ) + ".obsolete.sql" );

   if( exists_file( obsolete_ddl ) )
   {
      ifstream inpf( obsolete_ddl.c_str( ) );
      ofstream outf( upgrade_ddl.c_str( ), ios::out | ios::app );

      if( !inpf )
         throw runtime_error( "unable to open file '" + obsolete_ddl + "' for input" );

      if( !outf )
         throw runtime_error( "unable to open file '" + upgrade_ddl + "' for output" );

      outf << inpf.rdbuf( );

      outf.flush( );
      if( !outf.good( ) )
         throw runtime_error( "upgrade DDL output stream is bad" );
   }

   if( !app_is_module )
   {
      for( size_t i = 0; i < modules.size( ); i++ )
      {
         string next_upgrade_ddl( modules[ i ] + ".upgrade.sql" );
         if( exists_file( next_upgrade_ddl ) )
         {
            ifstream inpf( next_upgrade_ddl.c_str( ) );
            ofstream outf( upgrade_ddl.c_str( ), ios::out | ios::app );

            if( !inpf )
               throw runtime_error( "unable to open file '" + next_upgrade_ddl + "' for input" );

            if( !outf )
               throw runtime_error( "unable to open file '" + upgrade_ddl + "' for output" );

            outf << inpf.rdbuf( );

            outf.flush( );
            if( !outf.good( ) )
               throw runtime_error( "upgrade DDL output stream is bad" );
         }
      }
   }
   // [<finish Generate_Upgrade_DDL_impl>]
}

void Meta_Application::impl::impl_Test_Proc_1( string& Output )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start Test_Proc_1_impl>]
//nyi
   Output = get_obj( ).get_variable( "test_var" );
   Output += " " + get_obj( ).Name( );
   // [<finish Test_Proc_1_impl>]
}

void Meta_Application::impl::impl_Test_Proc_2( const string& Input, string& Output )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start Test_Proc_2_impl>]
//nyi
   Output = get_obj( ).get_variable( "test_var" );
   Output += " " + get_obj( ).Name( );
   Output += " " + Input;
   Output += " " + get_session_variable( "test_var" );
   // [<finish Test_Proc_2_impl>]
}

string Meta_Application::impl::get_field_value( int field ) const
{
   string retval;

   switch( field )
   {
      case 0:
      retval = to_string( impl_Actions( ) );
      break;

      case 1:
      retval = to_string( impl_Add_Modules_Automatically( ) );
      break;

      case 2:
      retval = to_string( impl_Allow_Duplicate_Logins( ) );
      break;

      case 3:
      retval = to_string( impl_Allow_Module_Switching( ) );
      break;

      case 4:
      retval = to_string( impl_Auto_Login_Days( ) );
      break;

      case 5:
      retval = to_string( impl_Blockchain_Id( ) );
      break;

      case 6:
      retval = to_string( impl_Create_Database( ) );
      break;

      case 7:
      retval = to_string( impl_Created_Database( ) );
      break;

      case 8:
      retval = to_string( impl_Creation_Script( ) );
      break;

      case 9:
      retval = to_string( impl_Default_Image_Height( ) );
      break;

      case 10:
      retval = to_string( impl_Default_Image_Width( ) );
      break;

      case 11:
      retval = to_string( impl_Default_List_Print_Row_Limit( ) );
      break;

      case 12:
      retval = to_string( impl_Default_List_Row_Limit( ) );
      break;

      case 13:
      retval = to_string( impl_Default_Max_Attached_File_Size( ) );
      break;

      case 14:
      retval = to_string( impl_Default_Multiline_Max_Rows( ) );
      break;

      case 15:
      retval = to_string( impl_Default_Multiline_Min_Rows( ) );
      break;

      case 16:
      retval = to_string( impl_Default_Multiline_Text_Limit( ) );
      break;

      case 17:
      retval = to_string( impl_Default_Multiline_Text_Trunc( ) );
      break;

      case 18:
      retval = to_string( impl_Encrypt_Dynamic_Content( ) );
      break;

      case 19:
      retval = to_string( impl_Generate_Details( ) );
      break;

      case 20:
      retval = to_string( impl_Generate_Status( ) );
      break;

      case 21:
      retval = to_string( impl_Generate_Type( ) );
      break;

      case 22:
      retval = to_string( impl_Installing_Script( ) );
      break;

      case 23:
      retval = to_string( impl_Keep_Existing_Data( ) );
      break;

      case 24:
      retval = to_string( impl_Module_Prefix( ) );
      break;

      case 25:
      retval = to_string( impl_Name( ) );
      break;

      case 26:
      retval = to_string( impl_Print_Lists_With_Check_Boxes( ) );
      break;

      case 27:
      retval = to_string( impl_Print_Lists_With_Row_Numbers( ) );
      break;

      case 28:
      retval = to_string( impl_Registration_Key( ) );
      break;

      case 29:
      retval = to_string( impl_Show_Inaccessible_Modules( ) );
      break;

      case 30:
      retval = to_string( impl_Type( ) );
      break;

      case 31:
      retval = to_string( impl_Use_Check_Boxes_for_Bools( ) );
      break;

      case 32:
      retval = to_string( impl_Use_Embedded_Images( ) );
      break;

      case 33:
      retval = to_string( impl_Use_Script( ) );
      break;

      case 34:
      retval = to_string( impl_Use_TLS_Sessions( ) );
      break;

      case 35:
      retval = to_string( impl_Use_URL_Checksum( ) );
      break;

      case 36:
      retval = to_string( impl_Use_Vertical_Menu( ) );
      break;

      case 37:
      retval = to_string( impl_Version( ) );
      break;

      case 38:
      retval = to_string( impl_Workgroup( ) );
      break;

      case 39:
      retval = to_string( impl_Year_Created( ) );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in get field value" );
   }

   return retval;
}

void Meta_Application::impl::set_field_value( int field, const string& value )
{
   switch( field )
   {
      case 0:
      func_string_setter< Meta_Application::impl, string >( *this, &Meta_Application::impl::impl_Actions, value );
      break;

      case 1:
      func_string_setter< Meta_Application::impl, bool >( *this, &Meta_Application::impl::impl_Add_Modules_Automatically, value );
      break;

      case 2:
      func_string_setter< Meta_Application::impl, bool >( *this, &Meta_Application::impl::impl_Allow_Duplicate_Logins, value );
      break;

      case 3:
      func_string_setter< Meta_Application::impl, bool >( *this, &Meta_Application::impl::impl_Allow_Module_Switching, value );
      break;

      case 4:
      func_string_setter< Meta_Application::impl, int >( *this, &Meta_Application::impl::impl_Auto_Login_Days, value );
      break;

      case 5:
      func_string_setter< Meta_Application::impl, string >( *this, &Meta_Application::impl::impl_Blockchain_Id, value );
      break;

      case 6:
      func_string_setter< Meta_Application::impl, bool >( *this, &Meta_Application::impl::impl_Create_Database, value );
      break;

      case 7:
      func_string_setter< Meta_Application::impl, bool >( *this, &Meta_Application::impl::impl_Created_Database, value );
      break;

      case 8:
      func_string_setter< Meta_Application::impl, Meta_Application_Script >( *this, &Meta_Application::impl::impl_Creation_Script, value );
      break;

      case 9:
      func_string_setter< Meta_Application::impl, int >( *this, &Meta_Application::impl::impl_Default_Image_Height, value );
      break;

      case 10:
      func_string_setter< Meta_Application::impl, int >( *this, &Meta_Application::impl::impl_Default_Image_Width, value );
      break;

      case 11:
      func_string_setter< Meta_Application::impl, int >( *this, &Meta_Application::impl::impl_Default_List_Print_Row_Limit, value );
      break;

      case 12:
      func_string_setter< Meta_Application::impl, int >( *this, &Meta_Application::impl::impl_Default_List_Row_Limit, value );
      break;

      case 13:
      func_string_setter< Meta_Application::impl, int >( *this, &Meta_Application::impl::impl_Default_Max_Attached_File_Size, value );
      break;

      case 14:
      func_string_setter< Meta_Application::impl, int >( *this, &Meta_Application::impl::impl_Default_Multiline_Max_Rows, value );
      break;

      case 15:
      func_string_setter< Meta_Application::impl, int >( *this, &Meta_Application::impl::impl_Default_Multiline_Min_Rows, value );
      break;

      case 16:
      func_string_setter< Meta_Application::impl, int >( *this, &Meta_Application::impl::impl_Default_Multiline_Text_Limit, value );
      break;

      case 17:
      func_string_setter< Meta_Application::impl, int >( *this, &Meta_Application::impl::impl_Default_Multiline_Text_Trunc, value );
      break;

      case 18:
      func_string_setter< Meta_Application::impl, bool >( *this, &Meta_Application::impl::impl_Encrypt_Dynamic_Content, value );
      break;

      case 19:
      func_string_setter< Meta_Application::impl, string >( *this, &Meta_Application::impl::impl_Generate_Details, value );
      break;

      case 20:
      func_string_setter< Meta_Application::impl, string >( *this, &Meta_Application::impl::impl_Generate_Status, value );
      break;

      case 21:
      func_string_setter< Meta_Application::impl, int >( *this, &Meta_Application::impl::impl_Generate_Type, value );
      break;

      case 22:
      func_string_setter< Meta_Application::impl, bool >( *this, &Meta_Application::impl::impl_Installing_Script, value );
      break;

      case 23:
      func_string_setter< Meta_Application::impl, bool >( *this, &Meta_Application::impl::impl_Keep_Existing_Data, value );
      break;

      case 24:
      func_string_setter< Meta_Application::impl, string >( *this, &Meta_Application::impl::impl_Module_Prefix, value );
      break;

      case 25:
      func_string_setter< Meta_Application::impl, string >( *this, &Meta_Application::impl::impl_Name, value );
      break;

      case 26:
      func_string_setter< Meta_Application::impl, bool >( *this, &Meta_Application::impl::impl_Print_Lists_With_Check_Boxes, value );
      break;

      case 27:
      func_string_setter< Meta_Application::impl, bool >( *this, &Meta_Application::impl::impl_Print_Lists_With_Row_Numbers, value );
      break;

      case 28:
      func_string_setter< Meta_Application::impl, string >( *this, &Meta_Application::impl::impl_Registration_Key, value );
      break;

      case 29:
      func_string_setter< Meta_Application::impl, bool >( *this, &Meta_Application::impl::impl_Show_Inaccessible_Modules, value );
      break;

      case 30:
      func_string_setter< Meta_Application::impl, bool >( *this, &Meta_Application::impl::impl_Type, value );
      break;

      case 31:
      func_string_setter< Meta_Application::impl, bool >( *this, &Meta_Application::impl::impl_Use_Check_Boxes_for_Bools, value );
      break;

      case 32:
      func_string_setter< Meta_Application::impl, bool >( *this, &Meta_Application::impl::impl_Use_Embedded_Images, value );
      break;

      case 33:
      func_string_setter< Meta_Application::impl, bool >( *this, &Meta_Application::impl::impl_Use_Script, value );
      break;

      case 34:
      func_string_setter< Meta_Application::impl, bool >( *this, &Meta_Application::impl::impl_Use_TLS_Sessions, value );
      break;

      case 35:
      func_string_setter< Meta_Application::impl, bool >( *this, &Meta_Application::impl::impl_Use_URL_Checksum, value );
      break;

      case 36:
      func_string_setter< Meta_Application::impl, bool >( *this, &Meta_Application::impl::impl_Use_Vertical_Menu, value );
      break;

      case 37:
      func_string_setter< Meta_Application::impl, string >( *this, &Meta_Application::impl::impl_Version, value );
      break;

      case 38:
      func_string_setter< Meta_Application::impl, Meta_Workgroup >( *this, &Meta_Application::impl::impl_Workgroup, value );
      break;

      case 39:
      func_string_setter< Meta_Application::impl, int >( *this, &Meta_Application::impl::impl_Year_Created, value );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in set field value" );
   }
}

void Meta_Application::impl::set_field_default( int field )
{
   switch( field )
   {
      case 0:
      impl_Actions( g_default_Actions );
      break;

      case 1:
      impl_Add_Modules_Automatically( g_default_Add_Modules_Automatically );
      break;

      case 2:
      impl_Allow_Duplicate_Logins( g_default_Allow_Duplicate_Logins );
      break;

      case 3:
      impl_Allow_Module_Switching( g_default_Allow_Module_Switching );
      break;

      case 4:
      impl_Auto_Login_Days( g_default_Auto_Login_Days );
      break;

      case 5:
      impl_Blockchain_Id( g_default_Blockchain_Id );
      break;

      case 6:
      impl_Create_Database( g_default_Create_Database );
      break;

      case 7:
      impl_Created_Database( g_default_Created_Database );
      break;

      case 8:
      impl_Creation_Script( g_default_Creation_Script );
      break;

      case 9:
      impl_Default_Image_Height( g_default_Default_Image_Height );
      break;

      case 10:
      impl_Default_Image_Width( g_default_Default_Image_Width );
      break;

      case 11:
      impl_Default_List_Print_Row_Limit( g_default_Default_List_Print_Row_Limit );
      break;

      case 12:
      impl_Default_List_Row_Limit( g_default_Default_List_Row_Limit );
      break;

      case 13:
      impl_Default_Max_Attached_File_Size( g_default_Default_Max_Attached_File_Size );
      break;

      case 14:
      impl_Default_Multiline_Max_Rows( g_default_Default_Multiline_Max_Rows );
      break;

      case 15:
      impl_Default_Multiline_Min_Rows( g_default_Default_Multiline_Min_Rows );
      break;

      case 16:
      impl_Default_Multiline_Text_Limit( g_default_Default_Multiline_Text_Limit );
      break;

      case 17:
      impl_Default_Multiline_Text_Trunc( g_default_Default_Multiline_Text_Trunc );
      break;

      case 18:
      impl_Encrypt_Dynamic_Content( g_default_Encrypt_Dynamic_Content );
      break;

      case 19:
      impl_Generate_Details( g_default_Generate_Details );
      break;

      case 20:
      impl_Generate_Status( g_default_Generate_Status );
      break;

      case 21:
      impl_Generate_Type( g_default_Generate_Type );
      break;

      case 22:
      impl_Installing_Script( g_default_Installing_Script );
      break;

      case 23:
      impl_Keep_Existing_Data( g_default_Keep_Existing_Data );
      break;

      case 24:
      impl_Module_Prefix( g_default_Module_Prefix );
      break;

      case 25:
      impl_Name( g_default_Name );
      break;

      case 26:
      impl_Print_Lists_With_Check_Boxes( g_default_Print_Lists_With_Check_Boxes );
      break;

      case 27:
      impl_Print_Lists_With_Row_Numbers( g_default_Print_Lists_With_Row_Numbers );
      break;

      case 28:
      impl_Registration_Key( g_default_Registration_Key );
      break;

      case 29:
      impl_Show_Inaccessible_Modules( g_default_Show_Inaccessible_Modules );
      break;

      case 30:
      impl_Type( g_default_Type );
      break;

      case 31:
      impl_Use_Check_Boxes_for_Bools( g_default_Use_Check_Boxes_for_Bools );
      break;

      case 32:
      impl_Use_Embedded_Images( g_default_Use_Embedded_Images );
      break;

      case 33:
      impl_Use_Script( g_default_Use_Script );
      break;

      case 34:
      impl_Use_TLS_Sessions( g_default_Use_TLS_Sessions );
      break;

      case 35:
      impl_Use_URL_Checksum( g_default_Use_URL_Checksum );
      break;

      case 36:
      impl_Use_Vertical_Menu( g_default_Use_Vertical_Menu );
      break;

      case 37:
      impl_Version( g_default_Version );
      break;

      case 38:
      impl_Workgroup( g_default_Workgroup );
      break;

      case 39:
      impl_Year_Created( g_default_Year_Created );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in set field default" );
   }
}

bool Meta_Application::impl::is_field_default( int field ) const
{
   bool retval = false;

   switch( field )
   {
      case 0:
      retval = ( v_Actions == g_default_Actions );
      break;

      case 1:
      retval = ( v_Add_Modules_Automatically == g_default_Add_Modules_Automatically );
      break;

      case 2:
      retval = ( v_Allow_Duplicate_Logins == g_default_Allow_Duplicate_Logins );
      break;

      case 3:
      retval = ( v_Allow_Module_Switching == g_default_Allow_Module_Switching );
      break;

      case 4:
      retval = ( v_Auto_Login_Days == g_default_Auto_Login_Days );
      break;

      case 5:
      retval = ( v_Blockchain_Id == g_default_Blockchain_Id );
      break;

      case 6:
      retval = ( v_Create_Database == g_default_Create_Database );
      break;

      case 7:
      retval = ( v_Created_Database == g_default_Created_Database );
      break;

      case 8:
      retval = ( v_Creation_Script == g_default_Creation_Script );
      break;

      case 9:
      retval = ( v_Default_Image_Height == g_default_Default_Image_Height );
      break;

      case 10:
      retval = ( v_Default_Image_Width == g_default_Default_Image_Width );
      break;

      case 11:
      retval = ( v_Default_List_Print_Row_Limit == g_default_Default_List_Print_Row_Limit );
      break;

      case 12:
      retval = ( v_Default_List_Row_Limit == g_default_Default_List_Row_Limit );
      break;

      case 13:
      retval = ( v_Default_Max_Attached_File_Size == g_default_Default_Max_Attached_File_Size );
      break;

      case 14:
      retval = ( v_Default_Multiline_Max_Rows == g_default_Default_Multiline_Max_Rows );
      break;

      case 15:
      retval = ( v_Default_Multiline_Min_Rows == g_default_Default_Multiline_Min_Rows );
      break;

      case 16:
      retval = ( v_Default_Multiline_Text_Limit == g_default_Default_Multiline_Text_Limit );
      break;

      case 17:
      retval = ( v_Default_Multiline_Text_Trunc == g_default_Default_Multiline_Text_Trunc );
      break;

      case 18:
      retval = ( v_Encrypt_Dynamic_Content == g_default_Encrypt_Dynamic_Content );
      break;

      case 19:
      retval = ( v_Generate_Details == g_default_Generate_Details );
      break;

      case 20:
      retval = ( v_Generate_Status == g_default_Generate_Status );
      break;

      case 21:
      retval = ( v_Generate_Type == g_default_Generate_Type );
      break;

      case 22:
      retval = ( v_Installing_Script == g_default_Installing_Script );
      break;

      case 23:
      retval = ( v_Keep_Existing_Data == g_default_Keep_Existing_Data );
      break;

      case 24:
      retval = ( v_Module_Prefix == g_default_Module_Prefix );
      break;

      case 25:
      retval = ( v_Name == g_default_Name );
      break;

      case 26:
      retval = ( v_Print_Lists_With_Check_Boxes == g_default_Print_Lists_With_Check_Boxes );
      break;

      case 27:
      retval = ( v_Print_Lists_With_Row_Numbers == g_default_Print_Lists_With_Row_Numbers );
      break;

      case 28:
      retval = ( v_Registration_Key == g_default_Registration_Key );
      break;

      case 29:
      retval = ( v_Show_Inaccessible_Modules == g_default_Show_Inaccessible_Modules );
      break;

      case 30:
      retval = ( v_Type == g_default_Type );
      break;

      case 31:
      retval = ( v_Use_Check_Boxes_for_Bools == g_default_Use_Check_Boxes_for_Bools );
      break;

      case 32:
      retval = ( v_Use_Embedded_Images == g_default_Use_Embedded_Images );
      break;

      case 33:
      retval = ( v_Use_Script == g_default_Use_Script );
      break;

      case 34:
      retval = ( v_Use_TLS_Sessions == g_default_Use_TLS_Sessions );
      break;

      case 35:
      retval = ( v_Use_URL_Checksum == g_default_Use_URL_Checksum );
      break;

      case 36:
      retval = ( v_Use_Vertical_Menu == g_default_Use_Vertical_Menu );
      break;

      case 37:
      retval = ( v_Version == g_default_Version );
      break;

      case 38:
      retval = ( v_Workgroup == g_default_Workgroup );
      break;

      case 39:
      retval = ( v_Year_Created == g_default_Year_Created );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in is_field_default" );
   }

   return retval;
}

uint64_t Meta_Application::impl::get_state( ) const
{
   uint64_t state = 0;

   // [(start protect_equal)] 600500
   if( check_equal( get_obj( ).Actions( ), "" ) )
      state |= ( c_state_undeletable | c_state_is_changing );
   // [(finish protect_equal)] 600500

   // [(start modifier_field_value)] 600520
   if( get_obj( ).Generate_Type( ) != 0 )
      state |= c_modifier_Is_Not_Full_Generate;
   // [(finish modifier_field_value)] 600520

   // [(start modifier_field_value)] 600525
   if( get_obj( ).Type( ) == 1 )
      state |= c_modifier_Is_Traditional;
   // [(finish modifier_field_value)] 600525

   // [(start modifier_field_value)] 600526
   if( get_obj( ).Type( ) == 0 )
      state |= c_modifier_Is_Non_Traditional;
   // [(finish modifier_field_value)] 600526

   // [(start modifier_field_value)] 600527
   if( get_obj( ).Use_Script( ) == 1 )
      state |= c_modifier_Is_Using_Script;
   // [(finish modifier_field_value)] 600527

   // [(start modifier_field_value)] 600528
   if( get_obj( ).Use_Script( ) == 0 )
      state |= c_modifier_Is_Not_Using_Script;
   // [(finish modifier_field_value)] 600528

   // [(start modifier_field_value)] 600529
   if( get_obj( ).Installing_Script( ) == 1 )
      state |= c_modifier_Is_Installing_Script;
   // [(finish modifier_field_value)] 600529

   // [<start get_state>]
//nyi
   if( !get_obj( ).get_clone_key( ).empty( )
    || !get_obj( ).get_variable( get_special_var_name( e_special_var_cloned ) ).empty( ) )
      state |= c_modifier_Was_Cloned;

   if( !exists_file( get_obj( ).Name( ) + ".log" ) )
      state |= c_modifier_Has_No_Application_Log;

   if( !get_session_variable( c_Meta_Application_Has_No_Scripts ).empty( ) )
      state |= c_modifier_Has_No_Installation_Scripts;

   if( get_obj( ).Installing_Script( ) )
      state |= ( c_state_is_changing | c_state_unactionable );
   else if( !is_null( get_obj( ).get_key( ) ) )
      state |= c_modifier_Is_Not_Using_Script;
   // [<finish get_state>]

   return state;
}

string Meta_Application::impl::get_state_names( ) const
{
   string state_names;
   uint64_t state = get_state( );

   if( state & c_modifier_Has_No_Application_Log )
      state_names += "|" + string( "Has_No_Application_Log" );
   if( state & c_modifier_Has_No_Installation_Scripts )
      state_names += "|" + string( "Has_No_Installation_Scripts" );
   if( state & c_modifier_Is_Installing_Script )
      state_names += "|" + string( "Is_Installing_Script" );
   if( state & c_modifier_Is_Non_Traditional )
      state_names += "|" + string( "Is_Non_Traditional" );
   if( state & c_modifier_Is_Not_Full_Generate )
      state_names += "|" + string( "Is_Not_Full_Generate" );
   if( state & c_modifier_Is_Not_Using_Script )
      state_names += "|" + string( "Is_Not_Using_Script" );
   if( state & c_modifier_Is_Traditional )
      state_names += "|" + string( "Is_Traditional" );
   if( state & c_modifier_Is_Using_Script )
      state_names += "|" + string( "Is_Using_Script" );
   if( state & c_modifier_Was_Cloned )
      state_names += "|" + string( "Was_Cloned" );

   return state_names.empty( ) ? state_names : state_names.substr( 1 );
}

const string& Meta_Application::impl::execute( const string& cmd_and_args )
{
   execute_command( cmd_and_args );
   return retval;
}

void Meta_Application::impl::clear_foreign_key( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( ( field == c_field_id_Creation_Script ) || ( field == c_field_name_Creation_Script ) )
      impl_Creation_Script( "" );
   else if( ( field == c_field_id_Workgroup ) || ( field == c_field_name_Workgroup ) )
      impl_Workgroup( "" );
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Application::impl::set_foreign_key_value( const string& field, const string& value )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for value: " + value );
   else if( ( field == c_field_id_Creation_Script ) || ( field == c_field_name_Creation_Script ) )
      v_Creation_Script = value;
   else if( ( field == c_field_id_Workgroup ) || ( field == c_field_name_Workgroup ) )
      v_Workgroup = value;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

const string& Meta_Application::impl::get_foreign_key_value( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( ( field == c_field_id_Creation_Script ) || ( field == c_field_name_Creation_Script ) )
      return v_Creation_Script;
   else if( ( field == c_field_id_Workgroup ) || ( field == c_field_name_Workgroup ) )
      return v_Workgroup;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Application::impl::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Creation_Script, v_Creation_Script ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Workgroup, v_Workgroup ) );
}

void Meta_Application::impl::add_extra_fixed_info( vector< pair< string, string > >& fixed_info ) const
{
   ( void )fixed_info;

   // [<start add_extra_fixed_info>]
   // [<finish add_extra_fixed_info>]
}

void Meta_Application::impl::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   ( void )paging_info;

   // [<start add_extra_paging_info>]
   // [<finish add_extra_paging_info>]
}

void Meta_Application::impl::clear( )
{
   v_Actions = g_default_Actions;
   v_Add_Modules_Automatically = g_default_Add_Modules_Automatically;
   v_Allow_Duplicate_Logins = g_default_Allow_Duplicate_Logins;
   v_Allow_Module_Switching = g_default_Allow_Module_Switching;
   v_Auto_Login_Days = g_default_Auto_Login_Days;
   v_Blockchain_Id = g_default_Blockchain_Id;
   v_Create_Database = g_default_Create_Database;
   v_Created_Database = g_default_Created_Database;
   v_Default_Image_Height = g_default_Default_Image_Height;
   v_Default_Image_Width = g_default_Default_Image_Width;
   v_Default_List_Print_Row_Limit = g_default_Default_List_Print_Row_Limit;
   v_Default_List_Row_Limit = g_default_Default_List_Row_Limit;
   v_Default_Max_Attached_File_Size = g_default_Default_Max_Attached_File_Size;
   v_Default_Multiline_Max_Rows = g_default_Default_Multiline_Max_Rows;
   v_Default_Multiline_Min_Rows = g_default_Default_Multiline_Min_Rows;
   v_Default_Multiline_Text_Limit = g_default_Default_Multiline_Text_Limit;
   v_Default_Multiline_Text_Trunc = g_default_Default_Multiline_Text_Trunc;
   v_Encrypt_Dynamic_Content = g_default_Encrypt_Dynamic_Content;
   v_Generate_Details = g_default_Generate_Details;
   v_Generate_Status = g_default_Generate_Status;
   v_Generate_Type = g_default_Generate_Type;
   v_Installing_Script = g_default_Installing_Script;
   v_Keep_Existing_Data = g_default_Keep_Existing_Data;
   v_Module_Prefix = g_default_Module_Prefix;
   v_Name = g_default_Name;
   v_Print_Lists_With_Check_Boxes = g_default_Print_Lists_With_Check_Boxes;
   v_Print_Lists_With_Row_Numbers = g_default_Print_Lists_With_Row_Numbers;
   v_Registration_Key = g_default_Registration_Key;
   v_Show_Inaccessible_Modules = g_default_Show_Inaccessible_Modules;
   v_Type = g_default_Type;
   v_Use_Check_Boxes_for_Bools = g_default_Use_Check_Boxes_for_Bools;
   v_Use_Embedded_Images = g_default_Use_Embedded_Images;
   v_Use_Script = g_default_Use_Script;
   v_Use_TLS_Sessions = g_default_Use_TLS_Sessions;
   v_Use_URL_Checksum = g_default_Use_URL_Checksum;
   v_Use_Vertical_Menu = g_default_Use_Vertical_Menu;
   v_Version = g_default_Version;
   v_Year_Created = g_default_Year_Created;

   v_Creation_Script = string( );
   if( cp_Creation_Script )
      p_obj->setup_foreign_key( *cp_Creation_Script, v_Creation_Script );

   v_Workgroup = string( );
   if( cp_Workgroup )
      p_obj->setup_foreign_key( *cp_Workgroup, v_Workgroup );
}

bool Meta_Application::impl::value_will_be_provided( const string& field_name )
{
   ( void )field_name;

   // [<start value_will_be_provided>]
   // [<finish value_will_be_provided>]

   return false;
}

void Meta_Application::impl::validate(
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
       c_str_field_must_not_be_empty_field, get_module_string( c_field_display_name_Name ) ) ) ) );

   if( is_null( v_Version ) && !value_will_be_provided( c_field_name_Version ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Version,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_field_must_not_be_empty_field, get_module_string( c_field_display_name_Version ) ) ) ) );

   if( v_Workgroup.empty( ) && !value_will_be_provided( c_field_name_Workgroup ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Workgroup,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_field_must_not_be_empty_field, get_module_string( c_field_display_name_Workgroup ) ) ) ) );

   if( !is_null( v_Blockchain_Id )
    && ( v_Blockchain_Id != g_default_Blockchain_Id
    || !value_will_be_provided( c_field_name_Blockchain_Id ) )
    && !g_Blockchain_Id_domain.is_valid( v_Blockchain_Id, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Blockchain_Id,
       get_module_string( c_field_display_name_Blockchain_Id ) + " " + error_message ) );

   if( !is_null( v_Module_Prefix )
    && ( v_Module_Prefix != g_default_Module_Prefix
    || !value_will_be_provided( c_field_name_Module_Prefix ) )
    && !g_Module_Prefix_domain.is_valid( v_Module_Prefix, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Module_Prefix,
       get_module_string( c_field_display_name_Module_Prefix ) + " " + error_message ) );

   if( !is_null( v_Name )
    && ( v_Name != g_default_Name
    || !value_will_be_provided( c_field_name_Name ) )
    && !g_Name_domain.is_valid( v_Name, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Name,
       get_module_string( c_field_display_name_Name ) + " " + error_message ) );

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

   if( !g_app_auto_days_enum.count( v_Auto_Login_Days ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Auto_Login_Days,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_field_has_invalid_value_field, get_module_string( c_field_display_name_Auto_Login_Days ) ) ) ) );

   if( !g_app_print_row_limit_enum.count( v_Default_List_Print_Row_Limit ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Default_List_Print_Row_Limit,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_field_has_invalid_value_field, get_module_string( c_field_display_name_Default_List_Print_Row_Limit ) ) ) ) );

   if( !g_app_list_row_limit_enum.count( v_Default_List_Row_Limit ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Default_List_Row_Limit,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_field_has_invalid_value_field, get_module_string( c_field_display_name_Default_List_Row_Limit ) ) ) ) );

   if( !g_app_text_rows_enum.count( v_Default_Multiline_Max_Rows ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Default_Multiline_Max_Rows,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_field_has_invalid_value_field, get_module_string( c_field_display_name_Default_Multiline_Max_Rows ) ) ) ) );

   if( !g_app_text_rows_enum.count( v_Default_Multiline_Min_Rows ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Default_Multiline_Min_Rows,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_field_has_invalid_value_field, get_module_string( c_field_display_name_Default_Multiline_Min_Rows ) ) ) ) );

   if( !g_app_text_limit_enum.count( v_Default_Multiline_Text_Limit ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Default_Multiline_Text_Limit,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_field_has_invalid_value_field, get_module_string( c_field_display_name_Default_Multiline_Text_Limit ) ) ) ) );

   if( !g_app_text_trunc_limit_enum.count( v_Default_Multiline_Text_Trunc ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Default_Multiline_Text_Trunc,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_field_has_invalid_value_field, get_module_string( c_field_display_name_Default_Multiline_Text_Trunc ) ) ) ) );

   if( !g_app_generate_type_enum.count( v_Generate_Type ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Generate_Type,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_field_has_invalid_value_field, get_module_string( c_field_display_name_Generate_Type ) ) ) ) );

   if( !g_app_type_enum.count( v_Type ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Type,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_field_has_invalid_value_field, get_module_string( c_field_display_name_Type ) ) ) ) );

   // [(start check_cond_non_null)] 600530
   if( get_obj( ).Use_Script( ) && is_null( get_obj( ).Creation_Script( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_Creation_Script, get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_field_must_not_be_empty_field, get_module_string( c_field_display_name_Creation_Script ) ) ) ) );
   // [(finish check_cond_non_null)] 600530

   // [<start validate>]
   // [<finish validate>]
}

void Meta_Application::impl::validate_set_fields(
 set< string >& fields_set, validation_error_container* p_validation_errors )
{
   ( void )fields_set;

   if( !p_validation_errors )
      throw runtime_error( "unexpected null validation_errors container" );

   string error_message;
   validate_formatter vf;

   if( !is_null( v_Blockchain_Id )
    && ( fields_set.count( c_field_id_Blockchain_Id ) || fields_set.count( c_field_name_Blockchain_Id ) )
    && !g_Blockchain_Id_domain.is_valid( v_Blockchain_Id, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Blockchain_Id,
       get_module_string( c_field_display_name_Blockchain_Id ) + " " + error_message ) );

   if( !is_null( v_Module_Prefix )
    && ( fields_set.count( c_field_id_Module_Prefix ) || fields_set.count( c_field_name_Module_Prefix ) )
    && !g_Module_Prefix_domain.is_valid( v_Module_Prefix, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Module_Prefix,
       get_module_string( c_field_display_name_Module_Prefix ) + " " + error_message ) );

   if( !is_null( v_Name )
    && ( fields_set.count( c_field_id_Name ) || fields_set.count( c_field_name_Name ) )
    && !g_Name_domain.is_valid( v_Name, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Name,
       get_module_string( c_field_display_name_Name ) + " " + error_message ) );

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

void Meta_Application::impl::after_fetch( )
{
   if( !get_obj( ).get_is_iterating( ) || get_obj( ).get_is_starting_iteration( ) )
      get_required_transients( );

   if( cp_Creation_Script )
      p_obj->setup_foreign_key( *cp_Creation_Script, v_Creation_Script );

   if( cp_Workgroup )
      p_obj->setup_foreign_key( *cp_Workgroup, v_Workgroup );

   post_init( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start transient_field_from_file)] 600510
   if( !get_obj( ).get_key( ).empty( )
    && ( get_obj( ).needs_field_value( "Generate_Details" )
    || required_transients.count( "Generate_Details" ) ) )
      get_obj( ).Generate_Details( load_file( get_obj( ).Name( ) + ".generate.log", true ) );
   // [(finish transient_field_from_file)] 600510

   // [<start after_fetch>]
//nyi
   if( !get_obj( ).get_key( ).empty( ) && !get_obj( ).get_graph_parent( ) )
   {
      string blockchain_identity( get_system_variable(
       get_special_var_name( e_special_var_blockchain_backup_identity ) ) );

      get_obj( ).Type( blockchain_identity.empty( ) );

      if( get_obj( ).child_Module( ).iterate_forwards( ) )
      {
         do
         {
            if( !get_obj( ).child_Module( ).Model( ).Type( ) )
            {
               get_obj( ).Type( 0 );
               get_obj( ).child_Module( ).iterate_stop( );

               break;
            }
         } while( get_obj( ).child_Module( ).iterate_next( ) );
      }

      if( !get_obj( ).Type( ) )
      {
         get_obj( ).Blockchain_Id( get_system_variable(
          get_special_var_name( e_special_var_blockchain_backup_identity ) ) );

         get_obj( ).Encrypt_Dynamic_Content( true );
      }
   }
   // [<finish after_fetch>]
}

void Meta_Application::impl::finalise_fetch( bool skip_set_original )
{
   if( !skip_set_original && !get_obj( ).get_key( ).empty( ) )
      get_obj( ).set_new_original_values( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start finalise_fetch>]
   // [<finish finalise_fetch>]
}

void Meta_Application::impl::at_create( )
{
   // [<start at_create>]
//nyi
   if( !get_obj( ).get_clone_key( ).empty( ) )
   {
      get_obj( ).Use_Script( false );
      get_obj( ).Creation_Script( "" );
   }
   else
      set_session_variable( c_Meta_Application_Has_No_Scripts, "" );
   // [<finish at_create>]
}

void Meta_Application::impl::post_init( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start post_init>]
   // [<finish post_init>]
}

void Meta_Application::impl::to_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   if( !get_obj( ).get_is_preparing( ) )
      post_init( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start to_store>]
//nyi
   if( is_create && get_obj( ).get_key( ).empty( ) && !get_obj( ).get_clone_key( ).empty( ) )
      get_obj( ).Name( get_obj( ).Name( ) + "_Copy" ); // FUTURE: This should be a module string.

   if( is_create && get_obj( ).get_key( ).empty( ) )
   {
      class_pointer< Meta_Application_Script > cp_scripts( e_create_instance );

      if( cp_scripts->iterate_forwards( ) )
         cp_scripts->iterate_stop( );
      else
         set_session_variable( c_Meta_Application_Has_No_Scripts, "1" );
   }
   // [<finish to_store>]
}

void Meta_Application::impl::for_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start for_store>]
//nyi
   bool skip_create_db = storage_locked_for_admin( );

   if( has_session_variable( get_special_var_name( e_special_var_force_db_create ) ) )
   {
      string names( get_session_variable( get_special_var_name( e_special_var_force_db_create ) ) );

      set< string > all_names;

      split( names, all_names );

      if( all_names.count( get_obj( ).Name( ) ) )
         skip_create_db = false;
   }

   if( is_create && !skip_create_db && get_obj( ).Create_Database( ) )
   {
      string create_script( "./create_db" );
      string tmp_filename( "~" + get_uuid( ) );

      string script( create_script + " "
       + get_obj( ).Name( ) + " " + get_sql_password( ) + " >" + tmp_filename + " 2>&1" );

      exec_system( script );

      if( exists_file( tmp_filename ) )
      {
         string output( load_file( tmp_filename ) );

         // NOTE: If the DB name has been used before then an error will occur due to the
         // existence of the DB user id so first try the script again with the "-rebuild"
         // option which will then not produce an error if this was the case.
         if( !output.empty( ) )
         {
            string script( create_script + " -rebuild "
             + get_obj( ).Name( ) + " " + get_sql_password( ) + " >" + tmp_filename + " 2>&1" );

            exec_system( script, false );
            output = load_file( tmp_filename );
         }

         remove_file( tmp_filename );

         if( !output.empty( ) )
            throw runtime_error( output );

         get_obj( ).Created_Database( true );
      }
   }

   if( is_create && !get_obj( ).get_clone_key( ).empty( ) )
   {
      class_pointer< Meta_Application > cp_clone( e_create_instance );

      cp_clone->perform_fetch( get_obj( ).get_clone_key( ) );

      if( cp_clone->child_Module( ).iterate_forwards( ) )
      {
         int n = 0;

         do
         {
            get_obj( ).child_Module( ).op_create( construct_key_from_int( get_obj( ).get_key( ), ++n ) );
            get_obj( ).child_Module( ).Application( get_obj( ).get_key( ) );
            get_obj( ).child_Module( ).Model( cp_clone->child_Module( ).Model( ) );
            get_obj( ).child_Module( ).op_apply( );

         } while( cp_clone->child_Module( ).iterate_next( ) );
      }
   }

   if( is_create && !storage_locked_for_admin( ) && !is_null( get_obj( ).Creation_Script( ) ) )
   {
      if( get_obj( ).Workgroup( ).child_Model( ).iterate_forwards( ) )
      {
         do
         {
            if( get_obj( ).Workgroup( ).child_Model( ).Name( ) == get_obj( ).Name( ) )
            {
               get_obj( ).Workgroup( ).child_Model( ).iterate_stop( );

               throw runtime_error( "There is an existing Model with the name '" + get_obj( ).Name( ) + "'." ); // FUTURE: This should be a module string.
            }
         } while( get_obj( ).Workgroup( ).child_Model( ).iterate_next( ) );
      }

      get_obj( ).Installing_Script( true );

      get_obj( ).Version( get_obj( ).Creation_Script( ).Version( ) );

      get_obj( ).Generate_Status( "Installing Packages..." ); // FUTURE: This should be a module string.

      set_session_variable( get_special_var_name( e_special_var_key ), get_obj( ).get_key( ) );
      set_session_variable( get_special_var_name( e_special_var_grp ), get_obj( ).Workgroup( ).get_key( ) );
      set_session_variable( get_special_var_name( e_special_var_name ), get_obj( ).Name( ) );
      set_session_variable( get_special_var_name( e_special_var_script ), get_obj( ).Creation_Script( ).Script_Name( ) );

      bool async = true;

      if( get_obj( ).get_variable( get_special_var_name( e_special_var_async ) ) == "0"
       || get_obj( ).get_variable( get_special_var_name( e_special_var_async ) ) == "false"
       || get_session_variable( get_special_var_name( e_special_var_allow_async ) ) == "0"
       || get_session_variable( get_special_var_name( e_special_var_allow_async ) ) == "false" )
         async = false;

      if( !async )
         output_progress_message( "Installing Packages..." ); // FUTURE: This should be a module string.

      run_script( "install_application", async, true );
   }
   // [<finish for_store>]
}

void Meta_Application::impl::after_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_store>]
//nyi
   if( is_create && get_obj( ).Add_Modules_Automatically( ) )
   {
      if( get_obj( ).Workgroup( ).child_Model( ).iterate_forwards( ) )
      {
         int child_num = 0;

         do
         {
            bool add_module = false;

            if( get_obj( ).Module_Prefix( ).empty( ) )
            {
               if( get_obj( ).Name( ) == get_obj( ).Workgroup( ).child_Model( ).Name( ) )
                  add_module = true;
            }
            else
            {
               string::size_type pos = get_obj( ).Workgroup( ).child_Model( ).Name( ).find( get_obj( ).Module_Prefix( ) );

               if( pos == 0 )
                  add_module = true;
            }

            if( add_module )
            {
               get_obj( ).child_Module( ).op_create( construct_key_from_int( get_obj( ).get_key( ), ++child_num ) );

               get_obj( ).child_Module( ).Application( get_obj( ).get_key( ) );
               get_obj( ).child_Module( ).Model( get_obj( ).Workgroup( ).child_Model( ).get_key( ) );

               get_obj( ).child_Module( ).op_apply( );

               if( get_obj( ).Module_Prefix( ).empty( ) )
                  break;
            }

         } while( get_obj( ).Workgroup( ).child_Model( ).iterate_next( ) );
      }
   }
   // [<finish after_store>]
}

bool Meta_Application::impl::can_destroy( bool is_internal )
{
   uint64_t state = p_obj->get_state( );

   bool retval = ( is_internal || !( state & c_state_undeletable ) );

   // [<start can_destroy>]
   // [<finish can_destroy>]

   return retval;
}

void Meta_Application::impl::for_destroy( bool is_internal )
{
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start for_destroy>]
//nyi
   if( !storage_locked_for_admin( ) )
   {
      if( get_obj( ).Created_Database( ) )
      {
         string drop_script( "./drop_db" );

         string script( drop_script + " "
          + get_obj( ).Name( ) + " " + get_sql_password( ) );

         exec_system( script, false );
      }

      string remove_script( "./remove_app" );
      string script( remove_script + " " + get_obj( ).Name( ) );

      exec_system( script, false );

      string app_dir( get_web_root( ) + "/" + lower( get_obj( ).Name( ) ) );

      set_session_variable( get_special_var_name( e_special_var_storage ), get_obj( ).Name( ) );

      run_script( "remove_unsubmitted", false );

      if( exists_file( app_dir + "/fcgi.sio" ) )
      {
         string remove_script( "./remove_fcgi" );

         string script( remove_script + " " + lower( get_obj( ).Name( ) ) + " " + get_web_root( ) );

         exec_system( script, false );
      }
   }
   // [<finish for_destroy>]
}

void Meta_Application::impl::after_destroy( bool is_internal )
{
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_destroy>]
   // [<finish after_destroy>]
}

void Meta_Application::impl::set_default_values( )
{
   clear( );
}

bool Meta_Application::impl::is_filtered( ) const
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start is_filtered>]
   // [<finish is_filtered>]

   return false;
}

void Meta_Application::impl::get_required_transients( ) const
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

Meta_Application::Meta_Application( )
{
   set_version( c_version );

   p_impl = new impl( *this );
}

Meta_Application::~Meta_Application( )
{
   cleanup( );
   delete p_impl;
}

const string& Meta_Application::Actions( ) const
{
   return p_impl->impl_Actions( );
}

void Meta_Application::Actions( const string& Actions )
{
   p_impl->impl_Actions( Actions );
}

bool Meta_Application::Add_Modules_Automatically( ) const
{
   return p_impl->impl_Add_Modules_Automatically( );
}

void Meta_Application::Add_Modules_Automatically( bool Add_Modules_Automatically )
{
   p_impl->impl_Add_Modules_Automatically( Add_Modules_Automatically );
}

bool Meta_Application::Allow_Duplicate_Logins( ) const
{
   return p_impl->impl_Allow_Duplicate_Logins( );
}

void Meta_Application::Allow_Duplicate_Logins( bool Allow_Duplicate_Logins )
{
   p_impl->impl_Allow_Duplicate_Logins( Allow_Duplicate_Logins );
}

bool Meta_Application::Allow_Module_Switching( ) const
{
   return p_impl->impl_Allow_Module_Switching( );
}

void Meta_Application::Allow_Module_Switching( bool Allow_Module_Switching )
{
   p_impl->impl_Allow_Module_Switching( Allow_Module_Switching );
}

int Meta_Application::Auto_Login_Days( ) const
{
   return p_impl->impl_Auto_Login_Days( );
}

void Meta_Application::Auto_Login_Days( int Auto_Login_Days )
{
   p_impl->impl_Auto_Login_Days( Auto_Login_Days );
}

const string& Meta_Application::Blockchain_Id( ) const
{
   return p_impl->impl_Blockchain_Id( );
}

void Meta_Application::Blockchain_Id( const string& Blockchain_Id )
{
   p_impl->impl_Blockchain_Id( Blockchain_Id );
}

bool Meta_Application::Create_Database( ) const
{
   return p_impl->impl_Create_Database( );
}

void Meta_Application::Create_Database( bool Create_Database )
{
   p_impl->impl_Create_Database( Create_Database );
}

bool Meta_Application::Created_Database( ) const
{
   return p_impl->impl_Created_Database( );
}

void Meta_Application::Created_Database( bool Created_Database )
{
   p_impl->impl_Created_Database( Created_Database );
}

int Meta_Application::Default_Image_Height( ) const
{
   return p_impl->impl_Default_Image_Height( );
}

void Meta_Application::Default_Image_Height( int Default_Image_Height )
{
   p_impl->impl_Default_Image_Height( Default_Image_Height );
}

int Meta_Application::Default_Image_Width( ) const
{
   return p_impl->impl_Default_Image_Width( );
}

void Meta_Application::Default_Image_Width( int Default_Image_Width )
{
   p_impl->impl_Default_Image_Width( Default_Image_Width );
}

int Meta_Application::Default_List_Print_Row_Limit( ) const
{
   return p_impl->impl_Default_List_Print_Row_Limit( );
}

void Meta_Application::Default_List_Print_Row_Limit( int Default_List_Print_Row_Limit )
{
   p_impl->impl_Default_List_Print_Row_Limit( Default_List_Print_Row_Limit );
}

int Meta_Application::Default_List_Row_Limit( ) const
{
   return p_impl->impl_Default_List_Row_Limit( );
}

void Meta_Application::Default_List_Row_Limit( int Default_List_Row_Limit )
{
   p_impl->impl_Default_List_Row_Limit( Default_List_Row_Limit );
}

int Meta_Application::Default_Max_Attached_File_Size( ) const
{
   return p_impl->impl_Default_Max_Attached_File_Size( );
}

void Meta_Application::Default_Max_Attached_File_Size( int Default_Max_Attached_File_Size )
{
   p_impl->impl_Default_Max_Attached_File_Size( Default_Max_Attached_File_Size );
}

int Meta_Application::Default_Multiline_Max_Rows( ) const
{
   return p_impl->impl_Default_Multiline_Max_Rows( );
}

void Meta_Application::Default_Multiline_Max_Rows( int Default_Multiline_Max_Rows )
{
   p_impl->impl_Default_Multiline_Max_Rows( Default_Multiline_Max_Rows );
}

int Meta_Application::Default_Multiline_Min_Rows( ) const
{
   return p_impl->impl_Default_Multiline_Min_Rows( );
}

void Meta_Application::Default_Multiline_Min_Rows( int Default_Multiline_Min_Rows )
{
   p_impl->impl_Default_Multiline_Min_Rows( Default_Multiline_Min_Rows );
}

int Meta_Application::Default_Multiline_Text_Limit( ) const
{
   return p_impl->impl_Default_Multiline_Text_Limit( );
}

void Meta_Application::Default_Multiline_Text_Limit( int Default_Multiline_Text_Limit )
{
   p_impl->impl_Default_Multiline_Text_Limit( Default_Multiline_Text_Limit );
}

int Meta_Application::Default_Multiline_Text_Trunc( ) const
{
   return p_impl->impl_Default_Multiline_Text_Trunc( );
}

void Meta_Application::Default_Multiline_Text_Trunc( int Default_Multiline_Text_Trunc )
{
   p_impl->impl_Default_Multiline_Text_Trunc( Default_Multiline_Text_Trunc );
}

bool Meta_Application::Encrypt_Dynamic_Content( ) const
{
   return p_impl->impl_Encrypt_Dynamic_Content( );
}

void Meta_Application::Encrypt_Dynamic_Content( bool Encrypt_Dynamic_Content )
{
   p_impl->impl_Encrypt_Dynamic_Content( Encrypt_Dynamic_Content );
}

const string& Meta_Application::Generate_Details( ) const
{
   return p_impl->impl_Generate_Details( );
}

void Meta_Application::Generate_Details( const string& Generate_Details )
{
   p_impl->impl_Generate_Details( Generate_Details );
}

const string& Meta_Application::Generate_Status( ) const
{
   return p_impl->impl_Generate_Status( );
}

void Meta_Application::Generate_Status( const string& Generate_Status )
{
   p_impl->impl_Generate_Status( Generate_Status );
}

int Meta_Application::Generate_Type( ) const
{
   return p_impl->impl_Generate_Type( );
}

void Meta_Application::Generate_Type( int Generate_Type )
{
   p_impl->impl_Generate_Type( Generate_Type );
}

bool Meta_Application::Installing_Script( ) const
{
   return p_impl->impl_Installing_Script( );
}

void Meta_Application::Installing_Script( bool Installing_Script )
{
   p_impl->impl_Installing_Script( Installing_Script );
}

bool Meta_Application::Keep_Existing_Data( ) const
{
   return p_impl->impl_Keep_Existing_Data( );
}

void Meta_Application::Keep_Existing_Data( bool Keep_Existing_Data )
{
   p_impl->impl_Keep_Existing_Data( Keep_Existing_Data );
}

const string& Meta_Application::Module_Prefix( ) const
{
   return p_impl->impl_Module_Prefix( );
}

void Meta_Application::Module_Prefix( const string& Module_Prefix )
{
   p_impl->impl_Module_Prefix( Module_Prefix );
}

const string& Meta_Application::Name( ) const
{
   return p_impl->impl_Name( );
}

void Meta_Application::Name( const string& Name )
{
   p_impl->impl_Name( Name );
}

bool Meta_Application::Print_Lists_With_Check_Boxes( ) const
{
   return p_impl->impl_Print_Lists_With_Check_Boxes( );
}

void Meta_Application::Print_Lists_With_Check_Boxes( bool Print_Lists_With_Check_Boxes )
{
   p_impl->impl_Print_Lists_With_Check_Boxes( Print_Lists_With_Check_Boxes );
}

bool Meta_Application::Print_Lists_With_Row_Numbers( ) const
{
   return p_impl->impl_Print_Lists_With_Row_Numbers( );
}

void Meta_Application::Print_Lists_With_Row_Numbers( bool Print_Lists_With_Row_Numbers )
{
   p_impl->impl_Print_Lists_With_Row_Numbers( Print_Lists_With_Row_Numbers );
}

const string& Meta_Application::Registration_Key( ) const
{
   return p_impl->impl_Registration_Key( );
}

void Meta_Application::Registration_Key( const string& Registration_Key )
{
   p_impl->impl_Registration_Key( Registration_Key );
}

bool Meta_Application::Show_Inaccessible_Modules( ) const
{
   return p_impl->impl_Show_Inaccessible_Modules( );
}

void Meta_Application::Show_Inaccessible_Modules( bool Show_Inaccessible_Modules )
{
   p_impl->impl_Show_Inaccessible_Modules( Show_Inaccessible_Modules );
}

bool Meta_Application::Type( ) const
{
   return p_impl->impl_Type( );
}

void Meta_Application::Type( bool Type )
{
   p_impl->impl_Type( Type );
}

bool Meta_Application::Use_Check_Boxes_for_Bools( ) const
{
   return p_impl->impl_Use_Check_Boxes_for_Bools( );
}

void Meta_Application::Use_Check_Boxes_for_Bools( bool Use_Check_Boxes_for_Bools )
{
   p_impl->impl_Use_Check_Boxes_for_Bools( Use_Check_Boxes_for_Bools );
}

bool Meta_Application::Use_Embedded_Images( ) const
{
   return p_impl->impl_Use_Embedded_Images( );
}

void Meta_Application::Use_Embedded_Images( bool Use_Embedded_Images )
{
   p_impl->impl_Use_Embedded_Images( Use_Embedded_Images );
}

bool Meta_Application::Use_Script( ) const
{
   return p_impl->impl_Use_Script( );
}

void Meta_Application::Use_Script( bool Use_Script )
{
   p_impl->impl_Use_Script( Use_Script );
}

bool Meta_Application::Use_TLS_Sessions( ) const
{
   return p_impl->impl_Use_TLS_Sessions( );
}

void Meta_Application::Use_TLS_Sessions( bool Use_TLS_Sessions )
{
   p_impl->impl_Use_TLS_Sessions( Use_TLS_Sessions );
}

bool Meta_Application::Use_URL_Checksum( ) const
{
   return p_impl->impl_Use_URL_Checksum( );
}

void Meta_Application::Use_URL_Checksum( bool Use_URL_Checksum )
{
   p_impl->impl_Use_URL_Checksum( Use_URL_Checksum );
}

bool Meta_Application::Use_Vertical_Menu( ) const
{
   return p_impl->impl_Use_Vertical_Menu( );
}

void Meta_Application::Use_Vertical_Menu( bool Use_Vertical_Menu )
{
   p_impl->impl_Use_Vertical_Menu( Use_Vertical_Menu );
}

const string& Meta_Application::Version( ) const
{
   return p_impl->impl_Version( );
}

void Meta_Application::Version( const string& Version )
{
   p_impl->impl_Version( Version );
}

int Meta_Application::Year_Created( ) const
{
   return p_impl->impl_Year_Created( );
}

void Meta_Application::Year_Created( int Year_Created )
{
   p_impl->impl_Year_Created( Year_Created );
}

Meta_Application_Script& Meta_Application::Creation_Script( )
{
   return p_impl->impl_Creation_Script( );
}

const Meta_Application_Script& Meta_Application::Creation_Script( ) const
{
   return p_impl->impl_Creation_Script( );
}

void Meta_Application::Creation_Script( const string& key )
{
   p_impl->impl_Creation_Script( key );
}

Meta_Workgroup& Meta_Application::Workgroup( )
{
   return p_impl->impl_Workgroup( );
}

const Meta_Workgroup& Meta_Application::Workgroup( ) const
{
   return p_impl->impl_Workgroup( );
}

void Meta_Application::Workgroup( const string& key )
{
   p_impl->impl_Workgroup( key );
}

Meta_Module& Meta_Application::child_Module( )
{
   return p_impl->impl_child_Module( );
}

const Meta_Module& Meta_Application::child_Module( ) const
{
   return p_impl->impl_child_Module( );
}

void Meta_Application::Generate( )
{
   p_impl->impl_Generate( );
}

void Meta_Application::Generate_File_Links( )
{
   p_impl->impl_Generate_File_Links( );
}

void Meta_Application::Generate_Modules( )
{
   p_impl->impl_Generate_Modules( );
}

void Meta_Application::Generate_Upgrade_DDL( )
{
   p_impl->impl_Generate_Upgrade_DDL( );
}

void Meta_Application::Test_Proc_1( string& Output )
{
   p_impl->impl_Test_Proc_1( Output );
}

void Meta_Application::Test_Proc_2( const string& Input, string& Output )
{
   p_impl->impl_Test_Proc_2( Input, Output );
}

string Meta_Application::get_field_value( int field ) const
{
   return p_impl->get_field_value( field );
}

void Meta_Application::set_field_value( int field, const string& value )
{
   p_impl->set_field_value( field, value );
}

void Meta_Application::set_field_default( int field )
{
   return set_field_default( ( field_id )( field + 1 ) );
}

void Meta_Application::set_field_default( field_id id )
{
   p_impl->set_field_default( ( int )id - 1 );
}

void Meta_Application::set_field_default( const string& field )
{
   p_impl->set_field_default( get_field_num( field ) );
}

bool Meta_Application::is_field_default( int field ) const
{
   return is_field_default( ( field_id )( field + 1 ) );
}

bool Meta_Application::is_field_default( field_id id ) const
{
   return p_impl->is_field_default( ( int )id - 1 );
}

bool Meta_Application::is_field_default( const string& field ) const
{
   return p_impl->is_field_default( get_field_num( field ) );
}

bool Meta_Application::is_field_encrypted( int field ) const
{
   return static_is_field_encrypted( ( field_id )( field + 1 ) );
}

bool Meta_Application::is_field_transient( int field ) const
{
   return static_is_field_transient( ( field_id )( field + 1 ) );
}

string Meta_Application::get_field_id( int field ) const
{
   return static_get_field_id( ( field_id )( field + 1 ) );
}

string Meta_Application::get_field_name( int field ) const
{
   return static_get_field_name( ( field_id )( field + 1 ) );
}

int Meta_Application::get_field_num( const string& field ) const
{
   int rc = static_get_field_num( field );

   if( rc < 0 )
      throw runtime_error( "unknown field name/id '" + field + "' in get_field_num( )" );

   return rc;
}

bool Meta_Application::has_field_changed( const string& field ) const
{
   return class_base::has_field_changed( get_field_num( field ) );
}

uint64_t Meta_Application::get_state( ) const
{
   uint64_t state = 0;

   state |= p_impl->get_state( );

   return state;
}

const string& Meta_Application::execute( const string& cmd_and_args )
{
   return p_impl->execute( cmd_and_args );
}

void Meta_Application::clear( )
{
   p_impl->clear( );
}

void Meta_Application::validate( uint64_t state, bool is_internal )
{
   p_impl->validate( state, is_internal, &validation_errors );
}

void Meta_Application::validate_set_fields( set< string >& fields_set )
{
   p_impl->validate_set_fields( fields_set, &validation_errors );
}

void Meta_Application::after_fetch( )
{
   p_impl->after_fetch( );
}

void Meta_Application::finalise_fetch( bool skip_set_original )
{
   p_impl->finalise_fetch( skip_set_original );
}

void Meta_Application::at_create( )
{
   p_impl->at_create( );
}

void Meta_Application::post_init( )
{
   p_impl->post_init( );
}

void Meta_Application::to_store( bool is_create, bool is_internal )
{
   p_impl->to_store( is_create, is_internal );
}

void Meta_Application::for_store( bool is_create, bool is_internal )
{
   p_impl->for_store( is_create, is_internal );
}

void Meta_Application::after_store( bool is_create, bool is_internal )
{
   p_impl->after_store( is_create, is_internal );
}

bool Meta_Application::can_destroy( bool is_internal )
{
   return p_impl->can_destroy( is_internal );
}

void Meta_Application::for_destroy( bool is_internal )
{
   p_impl->for_destroy( is_internal );
}

void Meta_Application::after_destroy( bool is_internal )
{
   p_impl->after_destroy( is_internal );
}

void Meta_Application::set_default_values( )
{
   p_impl->set_default_values( );
}

bool Meta_Application::is_filtered( ) const
{
   return p_impl->is_filtered( );
}

const char* Meta_Application::get_field_id(
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
   else if( name == c_field_name_Add_Modules_Automatically )
   {
      p_id = c_field_id_Add_Modules_Automatically;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Allow_Duplicate_Logins )
   {
      p_id = c_field_id_Allow_Duplicate_Logins;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Allow_Module_Switching )
   {
      p_id = c_field_id_Allow_Module_Switching;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Auto_Login_Days )
   {
      p_id = c_field_id_Auto_Login_Days;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Blockchain_Id )
   {
      p_id = c_field_id_Blockchain_Id;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Create_Database )
   {
      p_id = c_field_id_Create_Database;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Created_Database )
   {
      p_id = c_field_id_Created_Database;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Creation_Script )
   {
      p_id = c_field_id_Creation_Script;

      if( p_type_name )
         *p_type_name = "Meta_Application_Script";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Default_Image_Height )
   {
      p_id = c_field_id_Default_Image_Height;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Default_Image_Width )
   {
      p_id = c_field_id_Default_Image_Width;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Default_List_Print_Row_Limit )
   {
      p_id = c_field_id_Default_List_Print_Row_Limit;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Default_List_Row_Limit )
   {
      p_id = c_field_id_Default_List_Row_Limit;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Default_Max_Attached_File_Size )
   {
      p_id = c_field_id_Default_Max_Attached_File_Size;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Default_Multiline_Max_Rows )
   {
      p_id = c_field_id_Default_Multiline_Max_Rows;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Default_Multiline_Min_Rows )
   {
      p_id = c_field_id_Default_Multiline_Min_Rows;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Default_Multiline_Text_Limit )
   {
      p_id = c_field_id_Default_Multiline_Text_Limit;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Default_Multiline_Text_Trunc )
   {
      p_id = c_field_id_Default_Multiline_Text_Trunc;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Encrypt_Dynamic_Content )
   {
      p_id = c_field_id_Encrypt_Dynamic_Content;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Generate_Details )
   {
      p_id = c_field_id_Generate_Details;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Generate_Status )
   {
      p_id = c_field_id_Generate_Status;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Generate_Type )
   {
      p_id = c_field_id_Generate_Type;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Installing_Script )
   {
      p_id = c_field_id_Installing_Script;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Keep_Existing_Data )
   {
      p_id = c_field_id_Keep_Existing_Data;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Module_Prefix )
   {
      p_id = c_field_id_Module_Prefix;

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
   else if( name == c_field_name_Print_Lists_With_Check_Boxes )
   {
      p_id = c_field_id_Print_Lists_With_Check_Boxes;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Print_Lists_With_Row_Numbers )
   {
      p_id = c_field_id_Print_Lists_With_Row_Numbers;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Registration_Key )
   {
      p_id = c_field_id_Registration_Key;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Show_Inaccessible_Modules )
   {
      p_id = c_field_id_Show_Inaccessible_Modules;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Type )
   {
      p_id = c_field_id_Type;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Use_Check_Boxes_for_Bools )
   {
      p_id = c_field_id_Use_Check_Boxes_for_Bools;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Use_Embedded_Images )
   {
      p_id = c_field_id_Use_Embedded_Images;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Use_Script )
   {
      p_id = c_field_id_Use_Script;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Use_TLS_Sessions )
   {
      p_id = c_field_id_Use_TLS_Sessions;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Use_URL_Checksum )
   {
      p_id = c_field_id_Use_URL_Checksum;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Use_Vertical_Menu )
   {
      p_id = c_field_id_Use_Vertical_Menu;

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

const char* Meta_Application::get_field_name(
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
   else if( id == c_field_id_Add_Modules_Automatically )
   {
      p_name = c_field_name_Add_Modules_Automatically;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Allow_Duplicate_Logins )
   {
      p_name = c_field_name_Allow_Duplicate_Logins;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Allow_Module_Switching )
   {
      p_name = c_field_name_Allow_Module_Switching;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Auto_Login_Days )
   {
      p_name = c_field_name_Auto_Login_Days;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Blockchain_Id )
   {
      p_name = c_field_name_Blockchain_Id;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Create_Database )
   {
      p_name = c_field_name_Create_Database;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Created_Database )
   {
      p_name = c_field_name_Created_Database;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Creation_Script )
   {
      p_name = c_field_name_Creation_Script;

      if( p_type_name )
         *p_type_name = "Meta_Application_Script";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Default_Image_Height )
   {
      p_name = c_field_name_Default_Image_Height;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Default_Image_Width )
   {
      p_name = c_field_name_Default_Image_Width;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Default_List_Print_Row_Limit )
   {
      p_name = c_field_name_Default_List_Print_Row_Limit;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Default_List_Row_Limit )
   {
      p_name = c_field_name_Default_List_Row_Limit;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Default_Max_Attached_File_Size )
   {
      p_name = c_field_name_Default_Max_Attached_File_Size;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Default_Multiline_Max_Rows )
   {
      p_name = c_field_name_Default_Multiline_Max_Rows;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Default_Multiline_Min_Rows )
   {
      p_name = c_field_name_Default_Multiline_Min_Rows;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Default_Multiline_Text_Limit )
   {
      p_name = c_field_name_Default_Multiline_Text_Limit;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Default_Multiline_Text_Trunc )
   {
      p_name = c_field_name_Default_Multiline_Text_Trunc;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Encrypt_Dynamic_Content )
   {
      p_name = c_field_name_Encrypt_Dynamic_Content;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Generate_Details )
   {
      p_name = c_field_name_Generate_Details;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Generate_Status )
   {
      p_name = c_field_name_Generate_Status;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Generate_Type )
   {
      p_name = c_field_name_Generate_Type;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Installing_Script )
   {
      p_name = c_field_name_Installing_Script;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Keep_Existing_Data )
   {
      p_name = c_field_name_Keep_Existing_Data;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Module_Prefix )
   {
      p_name = c_field_name_Module_Prefix;

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
   else if( id == c_field_id_Print_Lists_With_Check_Boxes )
   {
      p_name = c_field_name_Print_Lists_With_Check_Boxes;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Print_Lists_With_Row_Numbers )
   {
      p_name = c_field_name_Print_Lists_With_Row_Numbers;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Registration_Key )
   {
      p_name = c_field_name_Registration_Key;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Show_Inaccessible_Modules )
   {
      p_name = c_field_name_Show_Inaccessible_Modules;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Type )
   {
      p_name = c_field_name_Type;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Use_Check_Boxes_for_Bools )
   {
      p_name = c_field_name_Use_Check_Boxes_for_Bools;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Use_Embedded_Images )
   {
      p_name = c_field_name_Use_Embedded_Images;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Use_Script )
   {
      p_name = c_field_name_Use_Script;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Use_TLS_Sessions )
   {
      p_name = c_field_name_Use_TLS_Sessions;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Use_URL_Checksum )
   {
      p_name = c_field_name_Use_URL_Checksum;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Use_Vertical_Menu )
   {
      p_name = c_field_name_Use_Vertical_Menu;

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

string& Meta_Application::get_group_field_name( ) const
{
   return g_group_field_name;
}

string& Meta_Application::get_level_field_name( ) const
{
   return g_level_field_name;
}

string& Meta_Application::get_order_field_name( ) const
{
   return g_order_field_name;
}

string& Meta_Application::get_owner_field_name( ) const
{
   return g_owner_field_name;
}

bool Meta_Application::is_file_field( const string& id_or_name ) const
{
   return g_file_field_ids.count( id_or_name ) || g_file_field_names.count( id_or_name );
}

void Meta_Application::get_file_field_names( vector< string >& file_field_names ) const
{
   for( set< string >::const_iterator ci = g_file_field_names.begin( ); ci != g_file_field_names.end( ); ++ci )
      file_field_names.push_back( *ci );
}

string Meta_Application::get_field_uom_symbol( const string& id_or_name ) const
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
   if( ( id_or_name == c_field_id_Add_Modules_Automatically ) || ( id_or_name == c_field_name_Add_Modules_Automatically ) )
   {
      name = string( c_field_display_name_Add_Modules_Automatically );
      get_module_string( c_field_display_name_Add_Modules_Automatically, &next );
   }
   if( ( id_or_name == c_field_id_Allow_Duplicate_Logins ) || ( id_or_name == c_field_name_Allow_Duplicate_Logins ) )
   {
      name = string( c_field_display_name_Allow_Duplicate_Logins );
      get_module_string( c_field_display_name_Allow_Duplicate_Logins, &next );
   }
   if( ( id_or_name == c_field_id_Allow_Module_Switching ) || ( id_or_name == c_field_name_Allow_Module_Switching ) )
   {
      name = string( c_field_display_name_Allow_Module_Switching );
      get_module_string( c_field_display_name_Allow_Module_Switching, &next );
   }
   if( ( id_or_name == c_field_id_Auto_Login_Days ) || ( id_or_name == c_field_name_Auto_Login_Days ) )
   {
      name = string( c_field_display_name_Auto_Login_Days );
      get_module_string( c_field_display_name_Auto_Login_Days, &next );
   }
   if( ( id_or_name == c_field_id_Blockchain_Id ) || ( id_or_name == c_field_name_Blockchain_Id ) )
   {
      name = string( c_field_display_name_Blockchain_Id );
      get_module_string( c_field_display_name_Blockchain_Id, &next );
   }
   if( ( id_or_name == c_field_id_Create_Database ) || ( id_or_name == c_field_name_Create_Database ) )
   {
      name = string( c_field_display_name_Create_Database );
      get_module_string( c_field_display_name_Create_Database, &next );
   }
   if( ( id_or_name == c_field_id_Created_Database ) || ( id_or_name == c_field_name_Created_Database ) )
   {
      name = string( c_field_display_name_Created_Database );
      get_module_string( c_field_display_name_Created_Database, &next );
   }
   if( ( id_or_name == c_field_id_Creation_Script ) || ( id_or_name == c_field_name_Creation_Script ) )
   {
      name = string( c_field_display_name_Creation_Script );
      get_module_string( c_field_display_name_Creation_Script, &next );
   }
   if( ( id_or_name == c_field_id_Default_Image_Height ) || ( id_or_name == c_field_name_Default_Image_Height ) )
   {
      name = string( c_field_display_name_Default_Image_Height );
      get_module_string( c_field_display_name_Default_Image_Height, &next );
   }
   if( ( id_or_name == c_field_id_Default_Image_Width ) || ( id_or_name == c_field_name_Default_Image_Width ) )
   {
      name = string( c_field_display_name_Default_Image_Width );
      get_module_string( c_field_display_name_Default_Image_Width, &next );
   }
   if( ( id_or_name == c_field_id_Default_List_Print_Row_Limit ) || ( id_or_name == c_field_name_Default_List_Print_Row_Limit ) )
   {
      name = string( c_field_display_name_Default_List_Print_Row_Limit );
      get_module_string( c_field_display_name_Default_List_Print_Row_Limit, &next );
   }
   if( ( id_or_name == c_field_id_Default_List_Row_Limit ) || ( id_or_name == c_field_name_Default_List_Row_Limit ) )
   {
      name = string( c_field_display_name_Default_List_Row_Limit );
      get_module_string( c_field_display_name_Default_List_Row_Limit, &next );
   }
   if( ( id_or_name == c_field_id_Default_Max_Attached_File_Size ) || ( id_or_name == c_field_name_Default_Max_Attached_File_Size ) )
   {
      name = string( c_field_display_name_Default_Max_Attached_File_Size );
      get_module_string( c_field_display_name_Default_Max_Attached_File_Size, &next );
   }
   if( ( id_or_name == c_field_id_Default_Multiline_Max_Rows ) || ( id_or_name == c_field_name_Default_Multiline_Max_Rows ) )
   {
      name = string( c_field_display_name_Default_Multiline_Max_Rows );
      get_module_string( c_field_display_name_Default_Multiline_Max_Rows, &next );
   }
   if( ( id_or_name == c_field_id_Default_Multiline_Min_Rows ) || ( id_or_name == c_field_name_Default_Multiline_Min_Rows ) )
   {
      name = string( c_field_display_name_Default_Multiline_Min_Rows );
      get_module_string( c_field_display_name_Default_Multiline_Min_Rows, &next );
   }
   if( ( id_or_name == c_field_id_Default_Multiline_Text_Limit ) || ( id_or_name == c_field_name_Default_Multiline_Text_Limit ) )
   {
      name = string( c_field_display_name_Default_Multiline_Text_Limit );
      get_module_string( c_field_display_name_Default_Multiline_Text_Limit, &next );
   }
   if( ( id_or_name == c_field_id_Default_Multiline_Text_Trunc ) || ( id_or_name == c_field_name_Default_Multiline_Text_Trunc ) )
   {
      name = string( c_field_display_name_Default_Multiline_Text_Trunc );
      get_module_string( c_field_display_name_Default_Multiline_Text_Trunc, &next );
   }
   if( ( id_or_name == c_field_id_Encrypt_Dynamic_Content ) || ( id_or_name == c_field_name_Encrypt_Dynamic_Content ) )
   {
      name = string( c_field_display_name_Encrypt_Dynamic_Content );
      get_module_string( c_field_display_name_Encrypt_Dynamic_Content, &next );
   }
   if( ( id_or_name == c_field_id_Generate_Details ) || ( id_or_name == c_field_name_Generate_Details ) )
   {
      name = string( c_field_display_name_Generate_Details );
      get_module_string( c_field_display_name_Generate_Details, &next );
   }
   if( ( id_or_name == c_field_id_Generate_Status ) || ( id_or_name == c_field_name_Generate_Status ) )
   {
      name = string( c_field_display_name_Generate_Status );
      get_module_string( c_field_display_name_Generate_Status, &next );
   }
   if( ( id_or_name == c_field_id_Generate_Type ) || ( id_or_name == c_field_name_Generate_Type ) )
   {
      name = string( c_field_display_name_Generate_Type );
      get_module_string( c_field_display_name_Generate_Type, &next );
   }
   if( ( id_or_name == c_field_id_Installing_Script ) || ( id_or_name == c_field_name_Installing_Script ) )
   {
      name = string( c_field_display_name_Installing_Script );
      get_module_string( c_field_display_name_Installing_Script, &next );
   }
   if( ( id_or_name == c_field_id_Keep_Existing_Data ) || ( id_or_name == c_field_name_Keep_Existing_Data ) )
   {
      name = string( c_field_display_name_Keep_Existing_Data );
      get_module_string( c_field_display_name_Keep_Existing_Data, &next );
   }
   if( ( id_or_name == c_field_id_Module_Prefix ) || ( id_or_name == c_field_name_Module_Prefix ) )
   {
      name = string( c_field_display_name_Module_Prefix );
      get_module_string( c_field_display_name_Module_Prefix, &next );
   }
   if( ( id_or_name == c_field_id_Name ) || ( id_or_name == c_field_name_Name ) )
   {
      name = string( c_field_display_name_Name );
      get_module_string( c_field_display_name_Name, &next );
   }
   if( ( id_or_name == c_field_id_Print_Lists_With_Check_Boxes ) || ( id_or_name == c_field_name_Print_Lists_With_Check_Boxes ) )
   {
      name = string( c_field_display_name_Print_Lists_With_Check_Boxes );
      get_module_string( c_field_display_name_Print_Lists_With_Check_Boxes, &next );
   }
   if( ( id_or_name == c_field_id_Print_Lists_With_Row_Numbers ) || ( id_or_name == c_field_name_Print_Lists_With_Row_Numbers ) )
   {
      name = string( c_field_display_name_Print_Lists_With_Row_Numbers );
      get_module_string( c_field_display_name_Print_Lists_With_Row_Numbers, &next );
   }
   if( ( id_or_name == c_field_id_Registration_Key ) || ( id_or_name == c_field_name_Registration_Key ) )
   {
      name = string( c_field_display_name_Registration_Key );
      get_module_string( c_field_display_name_Registration_Key, &next );
   }
   if( ( id_or_name == c_field_id_Show_Inaccessible_Modules ) || ( id_or_name == c_field_name_Show_Inaccessible_Modules ) )
   {
      name = string( c_field_display_name_Show_Inaccessible_Modules );
      get_module_string( c_field_display_name_Show_Inaccessible_Modules, &next );
   }
   if( ( id_or_name == c_field_id_Type ) || ( id_or_name == c_field_name_Type ) )
   {
      name = string( c_field_display_name_Type );
      get_module_string( c_field_display_name_Type, &next );
   }
   if( ( id_or_name == c_field_id_Use_Check_Boxes_for_Bools ) || ( id_or_name == c_field_name_Use_Check_Boxes_for_Bools ) )
   {
      name = string( c_field_display_name_Use_Check_Boxes_for_Bools );
      get_module_string( c_field_display_name_Use_Check_Boxes_for_Bools, &next );
   }
   if( ( id_or_name == c_field_id_Use_Embedded_Images ) || ( id_or_name == c_field_name_Use_Embedded_Images ) )
   {
      name = string( c_field_display_name_Use_Embedded_Images );
      get_module_string( c_field_display_name_Use_Embedded_Images, &next );
   }
   if( ( id_or_name == c_field_id_Use_Script ) || ( id_or_name == c_field_name_Use_Script ) )
   {
      name = string( c_field_display_name_Use_Script );
      get_module_string( c_field_display_name_Use_Script, &next );
   }
   if( ( id_or_name == c_field_id_Use_TLS_Sessions ) || ( id_or_name == c_field_name_Use_TLS_Sessions ) )
   {
      name = string( c_field_display_name_Use_TLS_Sessions );
      get_module_string( c_field_display_name_Use_TLS_Sessions, &next );
   }
   if( ( id_or_name == c_field_id_Use_URL_Checksum ) || ( id_or_name == c_field_name_Use_URL_Checksum ) )
   {
      name = string( c_field_display_name_Use_URL_Checksum );
      get_module_string( c_field_display_name_Use_URL_Checksum, &next );
   }
   if( ( id_or_name == c_field_id_Use_Vertical_Menu ) || ( id_or_name == c_field_name_Use_Vertical_Menu ) )
   {
      name = string( c_field_display_name_Use_Vertical_Menu );
      get_module_string( c_field_display_name_Use_Vertical_Menu, &next );
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

string Meta_Application::get_field_display_name( const string& id_or_name ) const
{
   string display_name;

   if( id_or_name.empty( ) )
      throw runtime_error( "unexpected empty field id_or_name for get_field_display_name" );
   if( ( id_or_name == c_field_id_Actions ) || ( id_or_name == c_field_name_Actions ) )
      display_name = get_module_string( c_field_display_name_Actions );
   if( ( id_or_name == c_field_id_Add_Modules_Automatically ) || ( id_or_name == c_field_name_Add_Modules_Automatically ) )
      display_name = get_module_string( c_field_display_name_Add_Modules_Automatically );
   if( ( id_or_name == c_field_id_Allow_Duplicate_Logins ) || ( id_or_name == c_field_name_Allow_Duplicate_Logins ) )
      display_name = get_module_string( c_field_display_name_Allow_Duplicate_Logins );
   if( ( id_or_name == c_field_id_Allow_Module_Switching ) || ( id_or_name == c_field_name_Allow_Module_Switching ) )
      display_name = get_module_string( c_field_display_name_Allow_Module_Switching );
   if( ( id_or_name == c_field_id_Auto_Login_Days ) || ( id_or_name == c_field_name_Auto_Login_Days ) )
      display_name = get_module_string( c_field_display_name_Auto_Login_Days );
   if( ( id_or_name == c_field_id_Blockchain_Id ) || ( id_or_name == c_field_name_Blockchain_Id ) )
      display_name = get_module_string( c_field_display_name_Blockchain_Id );
   if( ( id_or_name == c_field_id_Create_Database ) || ( id_or_name == c_field_name_Create_Database ) )
      display_name = get_module_string( c_field_display_name_Create_Database );
   if( ( id_or_name == c_field_id_Created_Database ) || ( id_or_name == c_field_name_Created_Database ) )
      display_name = get_module_string( c_field_display_name_Created_Database );
   if( ( id_or_name == c_field_id_Creation_Script ) || ( id_or_name == c_field_name_Creation_Script ) )
      display_name = get_module_string( c_field_display_name_Creation_Script );
   if( ( id_or_name == c_field_id_Default_Image_Height ) || ( id_or_name == c_field_name_Default_Image_Height ) )
      display_name = get_module_string( c_field_display_name_Default_Image_Height );
   if( ( id_or_name == c_field_id_Default_Image_Width ) || ( id_or_name == c_field_name_Default_Image_Width ) )
      display_name = get_module_string( c_field_display_name_Default_Image_Width );
   if( ( id_or_name == c_field_id_Default_List_Print_Row_Limit ) || ( id_or_name == c_field_name_Default_List_Print_Row_Limit ) )
      display_name = get_module_string( c_field_display_name_Default_List_Print_Row_Limit );
   if( ( id_or_name == c_field_id_Default_List_Row_Limit ) || ( id_or_name == c_field_name_Default_List_Row_Limit ) )
      display_name = get_module_string( c_field_display_name_Default_List_Row_Limit );
   if( ( id_or_name == c_field_id_Default_Max_Attached_File_Size ) || ( id_or_name == c_field_name_Default_Max_Attached_File_Size ) )
      display_name = get_module_string( c_field_display_name_Default_Max_Attached_File_Size );
   if( ( id_or_name == c_field_id_Default_Multiline_Max_Rows ) || ( id_or_name == c_field_name_Default_Multiline_Max_Rows ) )
      display_name = get_module_string( c_field_display_name_Default_Multiline_Max_Rows );
   if( ( id_or_name == c_field_id_Default_Multiline_Min_Rows ) || ( id_or_name == c_field_name_Default_Multiline_Min_Rows ) )
      display_name = get_module_string( c_field_display_name_Default_Multiline_Min_Rows );
   if( ( id_or_name == c_field_id_Default_Multiline_Text_Limit ) || ( id_or_name == c_field_name_Default_Multiline_Text_Limit ) )
      display_name = get_module_string( c_field_display_name_Default_Multiline_Text_Limit );
   if( ( id_or_name == c_field_id_Default_Multiline_Text_Trunc ) || ( id_or_name == c_field_name_Default_Multiline_Text_Trunc ) )
      display_name = get_module_string( c_field_display_name_Default_Multiline_Text_Trunc );
   if( ( id_or_name == c_field_id_Encrypt_Dynamic_Content ) || ( id_or_name == c_field_name_Encrypt_Dynamic_Content ) )
      display_name = get_module_string( c_field_display_name_Encrypt_Dynamic_Content );
   if( ( id_or_name == c_field_id_Generate_Details ) || ( id_or_name == c_field_name_Generate_Details ) )
      display_name = get_module_string( c_field_display_name_Generate_Details );
   if( ( id_or_name == c_field_id_Generate_Status ) || ( id_or_name == c_field_name_Generate_Status ) )
      display_name = get_module_string( c_field_display_name_Generate_Status );
   if( ( id_or_name == c_field_id_Generate_Type ) || ( id_or_name == c_field_name_Generate_Type ) )
      display_name = get_module_string( c_field_display_name_Generate_Type );
   if( ( id_or_name == c_field_id_Installing_Script ) || ( id_or_name == c_field_name_Installing_Script ) )
      display_name = get_module_string( c_field_display_name_Installing_Script );
   if( ( id_or_name == c_field_id_Keep_Existing_Data ) || ( id_or_name == c_field_name_Keep_Existing_Data ) )
      display_name = get_module_string( c_field_display_name_Keep_Existing_Data );
   if( ( id_or_name == c_field_id_Module_Prefix ) || ( id_or_name == c_field_name_Module_Prefix ) )
      display_name = get_module_string( c_field_display_name_Module_Prefix );
   if( ( id_or_name == c_field_id_Name ) || ( id_or_name == c_field_name_Name ) )
      display_name = get_module_string( c_field_display_name_Name );
   if( ( id_or_name == c_field_id_Print_Lists_With_Check_Boxes ) || ( id_or_name == c_field_name_Print_Lists_With_Check_Boxes ) )
      display_name = get_module_string( c_field_display_name_Print_Lists_With_Check_Boxes );
   if( ( id_or_name == c_field_id_Print_Lists_With_Row_Numbers ) || ( id_or_name == c_field_name_Print_Lists_With_Row_Numbers ) )
      display_name = get_module_string( c_field_display_name_Print_Lists_With_Row_Numbers );
   if( ( id_or_name == c_field_id_Registration_Key ) || ( id_or_name == c_field_name_Registration_Key ) )
      display_name = get_module_string( c_field_display_name_Registration_Key );
   if( ( id_or_name == c_field_id_Show_Inaccessible_Modules ) || ( id_or_name == c_field_name_Show_Inaccessible_Modules ) )
      display_name = get_module_string( c_field_display_name_Show_Inaccessible_Modules );
   if( ( id_or_name == c_field_id_Type ) || ( id_or_name == c_field_name_Type ) )
      display_name = get_module_string( c_field_display_name_Type );
   if( ( id_or_name == c_field_id_Use_Check_Boxes_for_Bools ) || ( id_or_name == c_field_name_Use_Check_Boxes_for_Bools ) )
      display_name = get_module_string( c_field_display_name_Use_Check_Boxes_for_Bools );
   if( ( id_or_name == c_field_id_Use_Embedded_Images ) || ( id_or_name == c_field_name_Use_Embedded_Images ) )
      display_name = get_module_string( c_field_display_name_Use_Embedded_Images );
   if( ( id_or_name == c_field_id_Use_Script ) || ( id_or_name == c_field_name_Use_Script ) )
      display_name = get_module_string( c_field_display_name_Use_Script );
   if( ( id_or_name == c_field_id_Use_TLS_Sessions ) || ( id_or_name == c_field_name_Use_TLS_Sessions ) )
      display_name = get_module_string( c_field_display_name_Use_TLS_Sessions );
   if( ( id_or_name == c_field_id_Use_URL_Checksum ) || ( id_or_name == c_field_name_Use_URL_Checksum ) )
      display_name = get_module_string( c_field_display_name_Use_URL_Checksum );
   if( ( id_or_name == c_field_id_Use_Vertical_Menu ) || ( id_or_name == c_field_name_Use_Vertical_Menu ) )
      display_name = get_module_string( c_field_display_name_Use_Vertical_Menu );
   if( ( id_or_name == c_field_id_Version ) || ( id_or_name == c_field_name_Version ) )
      display_name = get_module_string( c_field_display_name_Version );
   if( ( id_or_name == c_field_id_Workgroup ) || ( id_or_name == c_field_name_Workgroup ) )
      display_name = get_module_string( c_field_display_name_Workgroup );
   if( ( id_or_name == c_field_id_Year_Created ) || ( id_or_name == c_field_name_Year_Created ) )
      display_name = get_module_string( c_field_display_name_Year_Created );

   return display_name;
}

void Meta_Application::clear_foreign_key( const string& field )
{
   p_impl->clear_foreign_key( field );
}

void Meta_Application::set_foreign_key_value( const string& field, const string& value )
{
   p_impl->set_foreign_key_value( field, value );
}

const string& Meta_Application::get_foreign_key_value( const string& field )
{
   return p_impl->get_foreign_key_value( field );
}

void Meta_Application::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   p_impl->get_foreign_key_values( foreign_key_values );
}

void Meta_Application::setup_foreign_key( Meta_Application_Script& o, const string& value )
{
   static_cast< Meta_Application_Script& >( o ).set_key( value );
}

void Meta_Application::setup_foreign_key( Meta_Workgroup& o, const string& value )
{
   static_cast< Meta_Workgroup& >( o ).set_key( value );
}

void Meta_Application::setup_graph_parent( Meta_Module& o, const string& foreign_key_field )
{
   static_cast< Meta_Module& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Application::setup_graph_parent(
 Meta_Application_Script& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Application_Script& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Application_Script& >( o ).set_key( init_value );
}

void Meta_Application::setup_graph_parent(
 Meta_Workgroup& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Workgroup& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Workgroup& >( o ).set_key( init_value );
}

size_t Meta_Application::get_total_child_relationships( ) const
{
   return p_impl->total_child_relationships;
}

void Meta_Application::set_total_child_relationships( size_t new_total_child_relationships ) const
{
   p_impl->total_child_relationships = new_total_child_relationships;
}

size_t Meta_Application::get_num_foreign_key_children( bool is_internal ) const
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

class_base* Meta_Application::get_next_foreign_key_child(
 size_t child_num, string& next_child_field, cascade_op op, bool is_internal )
{
   class_base* p_class_base = 0;

   if( child_num >= 1 )
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
            next_child_field = "302210";
            p_class_base = &child_Module( );
         }
         break;
      }
   }

   return p_class_base;
}

void Meta_Application::add_extra_fixed_info( vector< pair< string, string > >& fixed_info ) const
{
   p_impl->add_extra_fixed_info( fixed_info );
}

void Meta_Application::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   p_impl->add_extra_paging_info( paging_info );
}

string Meta_Application::get_class_id( ) const
{
   return static_class_id( );
}

string Meta_Application::get_class_name( ) const
{
   return static_class_name( );
}

string Meta_Application::get_plural_name( ) const
{
   return static_plural_name( );
}

string Meta_Application::get_module_id( ) const
{
   return static_module_id( );
}

string Meta_Application::get_module_name( ) const
{
   return static_module_name( );
}

string Meta_Application::get_display_name( bool plural ) const
{
   string key( plural ? "plural_" : "class_" );

   key += "application";

   return get_module_string( key );
}

string Meta_Application::get_raw_variable( const std::string& name ) const
{
   if( name == g_state_names_variable )
      return p_impl->get_state_names( );
   else
      return class_base::get_raw_variable( name );
}

string Meta_Application::get_create_instance_info( ) const
{
   return "";
}

string Meta_Application::get_update_instance_info( ) const
{
   return "";
}

string Meta_Application::get_destroy_instance_info( ) const
{
   return "";
}

string Meta_Application::get_execute_procedure_info( const string& procedure_id ) const
{
   string retval;

   if( procedure_id.empty( ) )
      throw runtime_error( "unexpected empty procedure_id for get_execute_procedure_info" );
   else if( procedure_id == "127410" ) // i.e. Generate
      retval = "";
   else if( procedure_id == "127430" ) // i.e. Generate_File_Links
      retval = "";
   else if( procedure_id == "127420" ) // i.e. Generate_Modules
      retval = "";
   else if( procedure_id == "127440" ) // i.e. Generate_Upgrade_DDL
      retval = "";
   else if( procedure_id == "127495" ) // i.e. Test_Proc_1
      retval = "";
   else if( procedure_id == "127497" ) // i.e. Test_Proc_2
      retval = "";

   return retval;
}

bool Meta_Application::get_is_alias( ) const
{
   return false;
}

void Meta_Application::get_alias_base_info( pair< string, string >& alias_base_info ) const
{
   ( void )alias_base_info;
}

void Meta_Application::get_base_class_info( vector< pair< string, string > >& base_class_info ) const
{
   ( void )base_class_info;
}

class_base& Meta_Application::get_or_create_graph_child( const string& context )
{
   class_base* p_class_base( 0 );

   string::size_type pos = context.find( '.' );
   string sub_context( context.substr( 0, pos ) );

   if( sub_context.empty( ) )
      throw runtime_error( "unexpected empty sub-context" );
   else if( ( sub_context == "_302210" ) || ( sub_context == "child_Module" ) )
      p_class_base = &child_Module( );
   else if( ( sub_context == c_field_id_Creation_Script ) || ( sub_context == c_field_name_Creation_Script ) )
      p_class_base = &Creation_Script( );
   else if( ( sub_context == c_field_id_Workgroup ) || ( sub_context == c_field_name_Workgroup ) )
      p_class_base = &Workgroup( );

   if( !p_class_base )
      throw runtime_error( "unknown sub-context '" + sub_context + "'" );

   if( pos != string::npos )
      p_class_base = &p_class_base->get_or_create_graph_child( context.substr( pos + 1 ) );

   return *p_class_base;
}

void Meta_Application::get_sql_column_names(
 vector< string >& names, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   names.push_back( "C_Actions" );
   names.push_back( "C_Allow_Duplicate_Logins" );
   names.push_back( "C_Allow_Module_Switching" );
   names.push_back( "C_Auto_Login_Days" );
   names.push_back( "C_Created_Database" );
   names.push_back( "C_Creation_Script" );
   names.push_back( "C_Default_Image_Height" );
   names.push_back( "C_Default_Image_Width" );
   names.push_back( "C_Default_List_Print_Row_Limit" );
   names.push_back( "C_Default_List_Row_Limit" );
   names.push_back( "C_Default_Max_Attached_File_Size" );
   names.push_back( "C_Default_Multiline_Max_Rows" );
   names.push_back( "C_Default_Multiline_Min_Rows" );
   names.push_back( "C_Default_Multiline_Text_Limit" );
   names.push_back( "C_Default_Multiline_Text_Trunc" );
   names.push_back( "C_Encrypt_Dynamic_Content" );
   names.push_back( "C_Generate_Status" );
   names.push_back( "C_Generate_Type" );
   names.push_back( "C_Installing_Script" );
   names.push_back( "C_Keep_Existing_Data" );
   names.push_back( "C_Module_Prefix" );
   names.push_back( "C_Name" );
   names.push_back( "C_Print_Lists_With_Check_Boxes" );
   names.push_back( "C_Print_Lists_With_Row_Numbers" );
   names.push_back( "C_Registration_Key" );
   names.push_back( "C_Show_Inaccessible_Modules" );
   names.push_back( "C_Use_Check_Boxes_for_Bools" );
   names.push_back( "C_Use_Embedded_Images" );
   names.push_back( "C_Use_Script" );
   names.push_back( "C_Use_TLS_Sessions" );
   names.push_back( "C_Use_URL_Checksum" );
   names.push_back( "C_Use_Vertical_Menu" );
   names.push_back( "C_Version" );
   names.push_back( "C_Workgroup" );
   names.push_back( "C_Year_Created" );

   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;
}

void Meta_Application::get_sql_column_values(
 vector< string >& values, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   values.push_back( sql_quote( to_string( Actions( ) ) ) );
   values.push_back( to_string( Allow_Duplicate_Logins( ) ) );
   values.push_back( to_string( Allow_Module_Switching( ) ) );
   values.push_back( to_string( Auto_Login_Days( ) ) );
   values.push_back( to_string( Created_Database( ) ) );
   values.push_back( sql_quote( to_string( Creation_Script( ) ) ) );
   values.push_back( to_string( Default_Image_Height( ) ) );
   values.push_back( to_string( Default_Image_Width( ) ) );
   values.push_back( to_string( Default_List_Print_Row_Limit( ) ) );
   values.push_back( to_string( Default_List_Row_Limit( ) ) );
   values.push_back( to_string( Default_Max_Attached_File_Size( ) ) );
   values.push_back( to_string( Default_Multiline_Max_Rows( ) ) );
   values.push_back( to_string( Default_Multiline_Min_Rows( ) ) );
   values.push_back( to_string( Default_Multiline_Text_Limit( ) ) );
   values.push_back( to_string( Default_Multiline_Text_Trunc( ) ) );
   values.push_back( to_string( Encrypt_Dynamic_Content( ) ) );
   values.push_back( sql_quote( to_string( Generate_Status( ) ) ) );
   values.push_back( to_string( Generate_Type( ) ) );
   values.push_back( to_string( Installing_Script( ) ) );
   values.push_back( to_string( Keep_Existing_Data( ) ) );
   values.push_back( sql_quote( to_string( Module_Prefix( ) ) ) );
   values.push_back( sql_quote( to_string( Name( ) ) ) );
   values.push_back( to_string( Print_Lists_With_Check_Boxes( ) ) );
   values.push_back( to_string( Print_Lists_With_Row_Numbers( ) ) );
   values.push_back( sql_quote( to_string( Registration_Key( ) ) ) );
   values.push_back( to_string( Show_Inaccessible_Modules( ) ) );
   values.push_back( to_string( Use_Check_Boxes_for_Bools( ) ) );
   values.push_back( to_string( Use_Embedded_Images( ) ) );
   values.push_back( to_string( Use_Script( ) ) );
   values.push_back( to_string( Use_TLS_Sessions( ) ) );
   values.push_back( to_string( Use_URL_Checksum( ) ) );
   values.push_back( to_string( Use_Vertical_Menu( ) ) );
   values.push_back( sql_quote( to_string( Version( ) ) ) );
   values.push_back( sql_quote( to_string( Workgroup( ) ) ) );
   values.push_back( to_string( Year_Created( ) ) );

   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;
}

void Meta_Application::get_required_field_names(
 set< string >& names, bool use_transients, set< string >* p_dependents ) const
{
   set< string > local_dependents;
   set< string >& dependents( p_dependents ? *p_dependents : local_dependents );

   get_always_required_field_names( names, use_transients, dependents );

   // [<start get_required_field_names>]
   // [<finish get_required_field_names>]
}

void Meta_Application::get_always_required_field_names(
 set< string >& names, bool use_transients, set< string >& dependents ) const
{
   ( void )names;
   ( void )dependents;
   ( void )use_transients;

   // [(start protect_equal)] 600500
   dependents.insert( "Actions" );

   if( ( use_transients && is_field_transient( e_field_id_Actions ) )
    || ( !use_transients && !is_field_transient( e_field_id_Actions ) ) )
      names.insert( "Actions" );
   // [(finish protect_equal)] 600500

   // [(start modifier_field_value)] 600520
   dependents.insert( "Generate_Type" ); // (for Is_Not_Full_Generate modifier)

   if( ( use_transients && is_field_transient( e_field_id_Generate_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Generate_Type ) ) )
      names.insert( "Generate_Type" );
   // [(finish modifier_field_value)] 600520

   // [(start modifier_field_value)] 600525
   dependents.insert( "Type" ); // (for Is_Traditional modifier)

   if( ( use_transients && is_field_transient( e_field_id_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Type ) ) )
      names.insert( "Type" );
   // [(finish modifier_field_value)] 600525

   // [(start modifier_field_value)] 600526
   dependents.insert( "Type" ); // (for Is_Non_Traditional modifier)

   if( ( use_transients && is_field_transient( e_field_id_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Type ) ) )
      names.insert( "Type" );
   // [(finish modifier_field_value)] 600526

   // [(start modifier_field_value)] 600527
   dependents.insert( "Use_Script" ); // (for Is_Using_Script modifier)

   if( ( use_transients && is_field_transient( e_field_id_Use_Script ) )
    || ( !use_transients && !is_field_transient( e_field_id_Use_Script ) ) )
      names.insert( "Use_Script" );
   // [(finish modifier_field_value)] 600527

   // [(start modifier_field_value)] 600528
   dependents.insert( "Use_Script" ); // (for Is_Not_Using_Script modifier)

   if( ( use_transients && is_field_transient( e_field_id_Use_Script ) )
    || ( !use_transients && !is_field_transient( e_field_id_Use_Script ) ) )
      names.insert( "Use_Script" );
   // [(finish modifier_field_value)] 600528

   // [(start modifier_field_value)] 600529
   dependents.insert( "Installing_Script" ); // (for Is_Installing_Script modifier)

   if( ( use_transients && is_field_transient( e_field_id_Installing_Script ) )
    || ( !use_transients && !is_field_transient( e_field_id_Installing_Script ) ) )
      names.insert( "Installing_Script" );
   // [(finish modifier_field_value)] 600529

   // [<start get_always_required_field_names>]
   // [<finish get_always_required_field_names>]
}

void Meta_Application::get_transient_replacement_field_names( const string& name, vector< string >& names ) const
{
   ( void )name;
   ( void )names;

   // [<start get_transient_replacement_field_names>]
   // [<finish get_transient_replacement_field_names>]
}

void Meta_Application::do_generate_sql( generate_sql_type type,
 vector< string >& sql_stmts, set< string >& tx_key_info, vector< string >* p_sql_undo_stmts ) const
{
   generate_sql( static_class_name( ), type, sql_stmts, tx_key_info, p_sql_undo_stmts );
}

const char* Meta_Application::static_resolved_module_id( )
{
   return static_module_id( );
}

const char* Meta_Application::static_resolved_module_name( )
{
   return static_module_name( );
}

const char* Meta_Application::static_lock_class_id( )
{
   return "127100";
}

const char* Meta_Application::static_check_class_name( )
{
   return "Application";
}

const char* Meta_Application::static_persistence_extra( )
{
   return "";
}

bool Meta_Application::static_has_derivations( )
{
   return !g_derivations.empty( );
}

void Meta_Application::static_get_class_info( class_info_container& class_info )
{
   class_info.push_back( "100.127100" );
}

void Meta_Application::static_get_field_info( field_info_container& all_field_info )
{
   all_field_info.push_back( field_info( "127104", "Actions", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "127128", "Add_Modules_Automatically", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "127108", "Allow_Duplicate_Logins", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "127109", "Allow_Module_Switching", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "127124", "Auto_Login_Days", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "127135", "Blockchain_Id", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "127125", "Create_Database", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "127126", "Created_Database", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "302225", "Creation_Script", "Meta_Application_Script", false, "", "" ) );
   all_field_info.push_back( field_info( "127114", "Default_Image_Height", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "127113", "Default_Image_Width", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "127118", "Default_List_Print_Row_Limit", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "127115", "Default_List_Row_Limit", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "127127", "Default_Max_Attached_File_Size", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "127123", "Default_Multiline_Max_Rows", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "127116", "Default_Multiline_Min_Rows", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "127117", "Default_Multiline_Text_Limit", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "127119", "Default_Multiline_Text_Trunc", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "127130", "Encrypt_Dynamic_Content", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "127106", "Generate_Details", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "127105", "Generate_Status", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "127120", "Generate_Type", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "127138", "Installing_Script", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "127129", "Keep_Existing_Data", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "127103", "Module_Prefix", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "127101", "Name", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "127111", "Print_Lists_With_Check_Boxes", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "127112", "Print_Lists_With_Row_Numbers", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "127132", "Registration_Key", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "127110", "Show_Inaccessible_Modules", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "127136", "Type", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "127121", "Use_Check_Boxes_for_Bools", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "127131", "Use_Embedded_Images", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "127137", "Use_Script", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "127134", "Use_TLS_Sessions", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "127107", "Use_URL_Checksum", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "127133", "Use_Vertical_Menu", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "127102", "Version", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "302220", "Workgroup", "Meta_Workgroup", true, "", "" ) );
   all_field_info.push_back( field_info( "127139", "Year_Created", "int", false, "", "" ) );
}

void Meta_Application::static_get_foreign_key_info( foreign_key_info_container& foreign_key_info )
{
   ( void )foreign_key_info;

   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Creation_Script, make_pair( "Meta.127100", "Meta_Application_Script" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Workgroup, make_pair( "Meta.127100", "Meta_Workgroup" ) ) );
}

int Meta_Application::static_get_num_fields( bool* p_done, const string* p_class_name )
{
   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;

   return c_num_fields;
}

bool Meta_Application::static_is_field_encrypted( field_id id )
{
   return is_encrypted_field( static_get_field_id( id ) );
}

bool Meta_Application::static_is_field_transient( field_id id )
{
   return is_transient_field( static_get_field_id( id ) );
}

const char* Meta_Application::static_get_field_id( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "127104";
      break;

      case 2:
      p_id = "127128";
      break;

      case 3:
      p_id = "127108";
      break;

      case 4:
      p_id = "127109";
      break;

      case 5:
      p_id = "127124";
      break;

      case 6:
      p_id = "127135";
      break;

      case 7:
      p_id = "127125";
      break;

      case 8:
      p_id = "127126";
      break;

      case 9:
      p_id = "302225";
      break;

      case 10:
      p_id = "127114";
      break;

      case 11:
      p_id = "127113";
      break;

      case 12:
      p_id = "127118";
      break;

      case 13:
      p_id = "127115";
      break;

      case 14:
      p_id = "127127";
      break;

      case 15:
      p_id = "127123";
      break;

      case 16:
      p_id = "127116";
      break;

      case 17:
      p_id = "127117";
      break;

      case 18:
      p_id = "127119";
      break;

      case 19:
      p_id = "127130";
      break;

      case 20:
      p_id = "127106";
      break;

      case 21:
      p_id = "127105";
      break;

      case 22:
      p_id = "127120";
      break;

      case 23:
      p_id = "127138";
      break;

      case 24:
      p_id = "127129";
      break;

      case 25:
      p_id = "127103";
      break;

      case 26:
      p_id = "127101";
      break;

      case 27:
      p_id = "127111";
      break;

      case 28:
      p_id = "127112";
      break;

      case 29:
      p_id = "127132";
      break;

      case 30:
      p_id = "127110";
      break;

      case 31:
      p_id = "127136";
      break;

      case 32:
      p_id = "127121";
      break;

      case 33:
      p_id = "127131";
      break;

      case 34:
      p_id = "127137";
      break;

      case 35:
      p_id = "127134";
      break;

      case 36:
      p_id = "127107";
      break;

      case 37:
      p_id = "127133";
      break;

      case 38:
      p_id = "127102";
      break;

      case 39:
      p_id = "302220";
      break;

      case 40:
      p_id = "127139";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Application" );

   return p_id;
}

const char* Meta_Application::static_get_field_name( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "Actions";
      break;

      case 2:
      p_id = "Add_Modules_Automatically";
      break;

      case 3:
      p_id = "Allow_Duplicate_Logins";
      break;

      case 4:
      p_id = "Allow_Module_Switching";
      break;

      case 5:
      p_id = "Auto_Login_Days";
      break;

      case 6:
      p_id = "Blockchain_Id";
      break;

      case 7:
      p_id = "Create_Database";
      break;

      case 8:
      p_id = "Created_Database";
      break;

      case 9:
      p_id = "Creation_Script";
      break;

      case 10:
      p_id = "Default_Image_Height";
      break;

      case 11:
      p_id = "Default_Image_Width";
      break;

      case 12:
      p_id = "Default_List_Print_Row_Limit";
      break;

      case 13:
      p_id = "Default_List_Row_Limit";
      break;

      case 14:
      p_id = "Default_Max_Attached_File_Size";
      break;

      case 15:
      p_id = "Default_Multiline_Max_Rows";
      break;

      case 16:
      p_id = "Default_Multiline_Min_Rows";
      break;

      case 17:
      p_id = "Default_Multiline_Text_Limit";
      break;

      case 18:
      p_id = "Default_Multiline_Text_Trunc";
      break;

      case 19:
      p_id = "Encrypt_Dynamic_Content";
      break;

      case 20:
      p_id = "Generate_Details";
      break;

      case 21:
      p_id = "Generate_Status";
      break;

      case 22:
      p_id = "Generate_Type";
      break;

      case 23:
      p_id = "Installing_Script";
      break;

      case 24:
      p_id = "Keep_Existing_Data";
      break;

      case 25:
      p_id = "Module_Prefix";
      break;

      case 26:
      p_id = "Name";
      break;

      case 27:
      p_id = "Print_Lists_With_Check_Boxes";
      break;

      case 28:
      p_id = "Print_Lists_With_Row_Numbers";
      break;

      case 29:
      p_id = "Registration_Key";
      break;

      case 30:
      p_id = "Show_Inaccessible_Modules";
      break;

      case 31:
      p_id = "Type";
      break;

      case 32:
      p_id = "Use_Check_Boxes_for_Bools";
      break;

      case 33:
      p_id = "Use_Embedded_Images";
      break;

      case 34:
      p_id = "Use_Script";
      break;

      case 35:
      p_id = "Use_TLS_Sessions";
      break;

      case 36:
      p_id = "Use_URL_Checksum";
      break;

      case 37:
      p_id = "Use_Vertical_Menu";
      break;

      case 38:
      p_id = "Version";
      break;

      case 39:
      p_id = "Workgroup";
      break;

      case 40:
      p_id = "Year_Created";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Application" );

   return p_id;
}

int Meta_Application::static_get_field_num( const string& field )
{
   int rc = 0;

   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for static_get_field_num( )" );
   else if( field == c_field_id_Actions || field == c_field_name_Actions )
      rc += 1;
   else if( field == c_field_id_Add_Modules_Automatically || field == c_field_name_Add_Modules_Automatically )
      rc += 2;
   else if( field == c_field_id_Allow_Duplicate_Logins || field == c_field_name_Allow_Duplicate_Logins )
      rc += 3;
   else if( field == c_field_id_Allow_Module_Switching || field == c_field_name_Allow_Module_Switching )
      rc += 4;
   else if( field == c_field_id_Auto_Login_Days || field == c_field_name_Auto_Login_Days )
      rc += 5;
   else if( field == c_field_id_Blockchain_Id || field == c_field_name_Blockchain_Id )
      rc += 6;
   else if( field == c_field_id_Create_Database || field == c_field_name_Create_Database )
      rc += 7;
   else if( field == c_field_id_Created_Database || field == c_field_name_Created_Database )
      rc += 8;
   else if( field == c_field_id_Creation_Script || field == c_field_name_Creation_Script )
      rc += 9;
   else if( field == c_field_id_Default_Image_Height || field == c_field_name_Default_Image_Height )
      rc += 10;
   else if( field == c_field_id_Default_Image_Width || field == c_field_name_Default_Image_Width )
      rc += 11;
   else if( field == c_field_id_Default_List_Print_Row_Limit || field == c_field_name_Default_List_Print_Row_Limit )
      rc += 12;
   else if( field == c_field_id_Default_List_Row_Limit || field == c_field_name_Default_List_Row_Limit )
      rc += 13;
   else if( field == c_field_id_Default_Max_Attached_File_Size || field == c_field_name_Default_Max_Attached_File_Size )
      rc += 14;
   else if( field == c_field_id_Default_Multiline_Max_Rows || field == c_field_name_Default_Multiline_Max_Rows )
      rc += 15;
   else if( field == c_field_id_Default_Multiline_Min_Rows || field == c_field_name_Default_Multiline_Min_Rows )
      rc += 16;
   else if( field == c_field_id_Default_Multiline_Text_Limit || field == c_field_name_Default_Multiline_Text_Limit )
      rc += 17;
   else if( field == c_field_id_Default_Multiline_Text_Trunc || field == c_field_name_Default_Multiline_Text_Trunc )
      rc += 18;
   else if( field == c_field_id_Encrypt_Dynamic_Content || field == c_field_name_Encrypt_Dynamic_Content )
      rc += 19;
   else if( field == c_field_id_Generate_Details || field == c_field_name_Generate_Details )
      rc += 20;
   else if( field == c_field_id_Generate_Status || field == c_field_name_Generate_Status )
      rc += 21;
   else if( field == c_field_id_Generate_Type || field == c_field_name_Generate_Type )
      rc += 22;
   else if( field == c_field_id_Installing_Script || field == c_field_name_Installing_Script )
      rc += 23;
   else if( field == c_field_id_Keep_Existing_Data || field == c_field_name_Keep_Existing_Data )
      rc += 24;
   else if( field == c_field_id_Module_Prefix || field == c_field_name_Module_Prefix )
      rc += 25;
   else if( field == c_field_id_Name || field == c_field_name_Name )
      rc += 26;
   else if( field == c_field_id_Print_Lists_With_Check_Boxes || field == c_field_name_Print_Lists_With_Check_Boxes )
      rc += 27;
   else if( field == c_field_id_Print_Lists_With_Row_Numbers || field == c_field_name_Print_Lists_With_Row_Numbers )
      rc += 28;
   else if( field == c_field_id_Registration_Key || field == c_field_name_Registration_Key )
      rc += 29;
   else if( field == c_field_id_Show_Inaccessible_Modules || field == c_field_name_Show_Inaccessible_Modules )
      rc += 30;
   else if( field == c_field_id_Type || field == c_field_name_Type )
      rc += 31;
   else if( field == c_field_id_Use_Check_Boxes_for_Bools || field == c_field_name_Use_Check_Boxes_for_Bools )
      rc += 32;
   else if( field == c_field_id_Use_Embedded_Images || field == c_field_name_Use_Embedded_Images )
      rc += 33;
   else if( field == c_field_id_Use_Script || field == c_field_name_Use_Script )
      rc += 34;
   else if( field == c_field_id_Use_TLS_Sessions || field == c_field_name_Use_TLS_Sessions )
      rc += 35;
   else if( field == c_field_id_Use_URL_Checksum || field == c_field_name_Use_URL_Checksum )
      rc += 36;
   else if( field == c_field_id_Use_Vertical_Menu || field == c_field_name_Use_Vertical_Menu )
      rc += 37;
   else if( field == c_field_id_Version || field == c_field_name_Version )
      rc += 38;
   else if( field == c_field_id_Workgroup || field == c_field_name_Workgroup )
      rc += 39;
   else if( field == c_field_id_Year_Created || field == c_field_name_Year_Created )
      rc += 40;

   return rc - 1;
}

procedure_info_container& Meta_Application::static_get_procedure_info( )
{
   static bool initialised = false;
   static procedure_info_container procedures;

   if( !initialised )
   {
      initialised = true;
      procedures.insert( make_pair( "127410", procedure_info( "Generate" ) ) );
      procedures.insert( make_pair( "127430", procedure_info( "Generate_File_Links" ) ) );
      procedures.insert( make_pair( "127420", procedure_info( "Generate_Modules" ) ) );
      procedures.insert( make_pair( "127440", procedure_info( "Generate_Upgrade_DDL" ) ) );
      procedures.insert( make_pair( "127495", procedure_info( "Test_Proc_1" ) ) );
      procedures.insert( make_pair( "127497", procedure_info( "Test_Proc_2" ) ) );
   }

   return procedures;
}

string Meta_Application::static_get_sql_columns( )
{
   string sql_columns;

   sql_columns += 
    "C_Key_ VARCHAR(75),"
    "C_Ver_ SMALLINT UNSIGNED NOT NULL,"
    "C_Rev_ BIGINT UNSIGNED NOT NULL,"
    "C_Sec_ BIGINT UNSIGNED NOT NULL,"
    "C_Typ_ VARCHAR(16) NOT NULL,"
    "C_Actions VARCHAR(200) NOT NULL,"
    "C_Allow_Duplicate_Logins INTEGER NOT NULL,"
    "C_Allow_Module_Switching INTEGER NOT NULL,"
    "C_Auto_Login_Days INTEGER NOT NULL,"
    "C_Created_Database INTEGER NOT NULL,"
    "C_Creation_Script VARCHAR(75) NOT NULL,"
    "C_Default_Image_Height INTEGER NOT NULL,"
    "C_Default_Image_Width INTEGER NOT NULL,"
    "C_Default_List_Print_Row_Limit INTEGER NOT NULL,"
    "C_Default_List_Row_Limit INTEGER NOT NULL,"
    "C_Default_Max_Attached_File_Size INTEGER NOT NULL,"
    "C_Default_Multiline_Max_Rows INTEGER NOT NULL,"
    "C_Default_Multiline_Min_Rows INTEGER NOT NULL,"
    "C_Default_Multiline_Text_Limit INTEGER NOT NULL,"
    "C_Default_Multiline_Text_Trunc INTEGER NOT NULL,"
    "C_Encrypt_Dynamic_Content INTEGER NOT NULL,"
    "C_Generate_Status VARCHAR(200) NOT NULL,"
    "C_Generate_Type INTEGER NOT NULL,"
    "C_Installing_Script INTEGER NOT NULL,"
    "C_Keep_Existing_Data INTEGER NOT NULL,"
    "C_Module_Prefix VARCHAR(200) NOT NULL,"
    "C_Name VARCHAR(200) NOT NULL,"
    "C_Print_Lists_With_Check_Boxes INTEGER NOT NULL,"
    "C_Print_Lists_With_Row_Numbers INTEGER NOT NULL,"
    "C_Registration_Key VARCHAR(256) NOT NULL,"
    "C_Show_Inaccessible_Modules INTEGER NOT NULL,"
    "C_Use_Check_Boxes_for_Bools INTEGER NOT NULL,"
    "C_Use_Embedded_Images INTEGER NOT NULL,"
    "C_Use_Script INTEGER NOT NULL,"
    "C_Use_TLS_Sessions INTEGER NOT NULL,"
    "C_Use_URL_Checksum INTEGER NOT NULL,"
    "C_Use_Vertical_Menu INTEGER NOT NULL,"
    "C_Version VARCHAR(256) NOT NULL,"
    "C_Workgroup VARCHAR(75) NOT NULL,"
    "C_Year_Created INTEGER NOT NULL,"
    "PRIMARY KEY(C_Key_)";

   return sql_columns;
}

void Meta_Application::static_get_text_search_fields( vector< string >& fields )
{
   ( void )fields;
}

void Meta_Application::static_get_all_enum_pairs( vector< pair< string, string > >& pairs )
{
   pairs.push_back( make_pair( "enum_app_auto_days_0", get_enum_string_app_auto_days( 0 ) ) );
   pairs.push_back( make_pair( "enum_app_auto_days_1", get_enum_string_app_auto_days( 1 ) ) );
   pairs.push_back( make_pair( "enum_app_auto_days_2", get_enum_string_app_auto_days( 2 ) ) );
   pairs.push_back( make_pair( "enum_app_auto_days_3", get_enum_string_app_auto_days( 3 ) ) );
   pairs.push_back( make_pair( "enum_app_auto_days_7", get_enum_string_app_auto_days( 7 ) ) );
   pairs.push_back( make_pair( "enum_app_auto_days_10", get_enum_string_app_auto_days( 10 ) ) );
   pairs.push_back( make_pair( "enum_app_auto_days_20", get_enum_string_app_auto_days( 20 ) ) );
   pairs.push_back( make_pair( "enum_app_auto_days_30", get_enum_string_app_auto_days( 30 ) ) );
   pairs.push_back( make_pair( "enum_app_auto_days_60", get_enum_string_app_auto_days( 60 ) ) );
   pairs.push_back( make_pair( "enum_app_auto_days_90", get_enum_string_app_auto_days( 90 ) ) );
   pairs.push_back( make_pair( "enum_app_auto_days_180", get_enum_string_app_auto_days( 180 ) ) );
   pairs.push_back( make_pair( "enum_app_auto_days_365", get_enum_string_app_auto_days( 365 ) ) );

   pairs.push_back( make_pair( "enum_app_print_row_limit_1000", get_enum_string_app_print_row_limit( 1000 ) ) );
   pairs.push_back( make_pair( "enum_app_print_row_limit_2000", get_enum_string_app_print_row_limit( 2000 ) ) );
   pairs.push_back( make_pair( "enum_app_print_row_limit_3000", get_enum_string_app_print_row_limit( 3000 ) ) );
   pairs.push_back( make_pair( "enum_app_print_row_limit_4000", get_enum_string_app_print_row_limit( 4000 ) ) );
   pairs.push_back( make_pair( "enum_app_print_row_limit_5000", get_enum_string_app_print_row_limit( 5000 ) ) );
   pairs.push_back( make_pair( "enum_app_print_row_limit_10000", get_enum_string_app_print_row_limit( 10000 ) ) );
   pairs.push_back( make_pair( "enum_app_print_row_limit_20000", get_enum_string_app_print_row_limit( 20000 ) ) );

   pairs.push_back( make_pair( "enum_app_list_row_limit_10", get_enum_string_app_list_row_limit( 10 ) ) );
   pairs.push_back( make_pair( "enum_app_list_row_limit_15", get_enum_string_app_list_row_limit( 15 ) ) );
   pairs.push_back( make_pair( "enum_app_list_row_limit_20", get_enum_string_app_list_row_limit( 20 ) ) );
   pairs.push_back( make_pair( "enum_app_list_row_limit_25", get_enum_string_app_list_row_limit( 25 ) ) );
   pairs.push_back( make_pair( "enum_app_list_row_limit_30", get_enum_string_app_list_row_limit( 30 ) ) );
   pairs.push_back( make_pair( "enum_app_list_row_limit_40", get_enum_string_app_list_row_limit( 40 ) ) );
   pairs.push_back( make_pair( "enum_app_list_row_limit_50", get_enum_string_app_list_row_limit( 50 ) ) );
   pairs.push_back( make_pair( "enum_app_list_row_limit_100", get_enum_string_app_list_row_limit( 100 ) ) );

   pairs.push_back( make_pair( "enum_app_text_rows_5", get_enum_string_app_text_rows( 5 ) ) );
   pairs.push_back( make_pair( "enum_app_text_rows_10", get_enum_string_app_text_rows( 10 ) ) );
   pairs.push_back( make_pair( "enum_app_text_rows_15", get_enum_string_app_text_rows( 15 ) ) );
   pairs.push_back( make_pair( "enum_app_text_rows_20", get_enum_string_app_text_rows( 20 ) ) );
   pairs.push_back( make_pair( "enum_app_text_rows_25", get_enum_string_app_text_rows( 25 ) ) );
   pairs.push_back( make_pair( "enum_app_text_rows_30", get_enum_string_app_text_rows( 30 ) ) );
   pairs.push_back( make_pair( "enum_app_text_rows_40", get_enum_string_app_text_rows( 40 ) ) );
   pairs.push_back( make_pair( "enum_app_text_rows_50", get_enum_string_app_text_rows( 50 ) ) );
   pairs.push_back( make_pair( "enum_app_text_rows_100", get_enum_string_app_text_rows( 100 ) ) );

   pairs.push_back( make_pair( "enum_app_text_limit_1000", get_enum_string_app_text_limit( 1000 ) ) );
   pairs.push_back( make_pair( "enum_app_text_limit_1500", get_enum_string_app_text_limit( 1500 ) ) );
   pairs.push_back( make_pair( "enum_app_text_limit_2000", get_enum_string_app_text_limit( 2000 ) ) );
   pairs.push_back( make_pair( "enum_app_text_limit_2500", get_enum_string_app_text_limit( 2500 ) ) );
   pairs.push_back( make_pair( "enum_app_text_limit_3000", get_enum_string_app_text_limit( 3000 ) ) );
   pairs.push_back( make_pair( "enum_app_text_limit_4000", get_enum_string_app_text_limit( 4000 ) ) );
   pairs.push_back( make_pair( "enum_app_text_limit_5000", get_enum_string_app_text_limit( 5000 ) ) );
   pairs.push_back( make_pair( "enum_app_text_limit_10000", get_enum_string_app_text_limit( 10000 ) ) );
   pairs.push_back( make_pair( "enum_app_text_limit_20000", get_enum_string_app_text_limit( 20000 ) ) );
   pairs.push_back( make_pair( "enum_app_text_limit_30000", get_enum_string_app_text_limit( 30000 ) ) );
   pairs.push_back( make_pair( "enum_app_text_limit_40000", get_enum_string_app_text_limit( 40000 ) ) );
   pairs.push_back( make_pair( "enum_app_text_limit_50000", get_enum_string_app_text_limit( 50000 ) ) );
   pairs.push_back( make_pair( "enum_app_text_limit_60000", get_enum_string_app_text_limit( 60000 ) ) );

   pairs.push_back( make_pair( "enum_app_text_trunc_limit_10", get_enum_string_app_text_trunc_limit( 10 ) ) );
   pairs.push_back( make_pair( "enum_app_text_trunc_limit_15", get_enum_string_app_text_trunc_limit( 15 ) ) );
   pairs.push_back( make_pair( "enum_app_text_trunc_limit_20", get_enum_string_app_text_trunc_limit( 20 ) ) );
   pairs.push_back( make_pair( "enum_app_text_trunc_limit_25", get_enum_string_app_text_trunc_limit( 25 ) ) );
   pairs.push_back( make_pair( "enum_app_text_trunc_limit_30", get_enum_string_app_text_trunc_limit( 30 ) ) );
   pairs.push_back( make_pair( "enum_app_text_trunc_limit_40", get_enum_string_app_text_trunc_limit( 40 ) ) );
   pairs.push_back( make_pair( "enum_app_text_trunc_limit_50", get_enum_string_app_text_trunc_limit( 50 ) ) );
   pairs.push_back( make_pair( "enum_app_text_trunc_limit_75", get_enum_string_app_text_trunc_limit( 75 ) ) );
   pairs.push_back( make_pair( "enum_app_text_trunc_limit_100", get_enum_string_app_text_trunc_limit( 100 ) ) );

   pairs.push_back( make_pair( "enum_app_generate_type_0", get_enum_string_app_generate_type( 0 ) ) );
   pairs.push_back( make_pair( "enum_app_generate_type_1", get_enum_string_app_generate_type( 1 ) ) );
   pairs.push_back( make_pair( "enum_app_generate_type_2", get_enum_string_app_generate_type( 2 ) ) );

   pairs.push_back( make_pair( "enum_app_type_0", get_enum_string_app_type( 0 ) ) );
   pairs.push_back( make_pair( "enum_app_type_1", get_enum_string_app_type( 1 ) ) );
}

void Meta_Application::static_get_all_index_pairs( vector< pair< string, string > >& pairs )
{
   pairs.push_back( make_pair( "Workgroup,Name", "string,string" ) );
}

void Meta_Application::static_get_all_unique_indexes( vector< string >& unique_indexes )
{
   unique_indexes.push_back( "Workgroup,Name" );
}

void Meta_Application::static_get_sql_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_Workgroup,C_Name" );
}

void Meta_Application::static_get_sql_unique_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_Workgroup,C_Name" );
}

void Meta_Application::static_insert_derivation( const string& module_and_class_id )
{
   g_derivations.insert( module_and_class_id );
}

void Meta_Application::static_remove_derivation( const string& module_and_class_id )
{
   if( g_derivations.count( module_and_class_id ) )
      g_derivations.erase( module_and_class_id );
}

void Meta_Application::static_insert_external_alias( const string& module_and_class_id, Meta_Application* p_instance )
{
   g_external_aliases.insert( external_aliases_value_type( module_and_class_id, p_instance ) );
}

void Meta_Application::static_remove_external_alias( const string& module_and_class_id )
{
   if( g_external_aliases.count( module_and_class_id ) )
   {
      delete g_external_aliases[ module_and_class_id ];
      g_external_aliases.erase( module_and_class_id );
   }
}

void Meta_Application::static_class_init( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for init" );

   g_state_names_variable = get_special_var_name( e_special_var_state_names );

   g_app_auto_days_enum.insert( 0 );
   g_app_auto_days_enum.insert( 1 );
   g_app_auto_days_enum.insert( 2 );
   g_app_auto_days_enum.insert( 3 );
   g_app_auto_days_enum.insert( 7 );
   g_app_auto_days_enum.insert( 10 );
   g_app_auto_days_enum.insert( 20 );
   g_app_auto_days_enum.insert( 30 );
   g_app_auto_days_enum.insert( 60 );
   g_app_auto_days_enum.insert( 90 );
   g_app_auto_days_enum.insert( 180 );
   g_app_auto_days_enum.insert( 365 );

   g_app_print_row_limit_enum.insert( 1000 );
   g_app_print_row_limit_enum.insert( 2000 );
   g_app_print_row_limit_enum.insert( 3000 );
   g_app_print_row_limit_enum.insert( 4000 );
   g_app_print_row_limit_enum.insert( 5000 );
   g_app_print_row_limit_enum.insert( 10000 );
   g_app_print_row_limit_enum.insert( 20000 );

   g_app_list_row_limit_enum.insert( 10 );
   g_app_list_row_limit_enum.insert( 15 );
   g_app_list_row_limit_enum.insert( 20 );
   g_app_list_row_limit_enum.insert( 25 );
   g_app_list_row_limit_enum.insert( 30 );
   g_app_list_row_limit_enum.insert( 40 );
   g_app_list_row_limit_enum.insert( 50 );
   g_app_list_row_limit_enum.insert( 100 );

   g_app_text_rows_enum.insert( 5 );
   g_app_text_rows_enum.insert( 10 );
   g_app_text_rows_enum.insert( 15 );
   g_app_text_rows_enum.insert( 20 );
   g_app_text_rows_enum.insert( 25 );
   g_app_text_rows_enum.insert( 30 );
   g_app_text_rows_enum.insert( 40 );
   g_app_text_rows_enum.insert( 50 );
   g_app_text_rows_enum.insert( 100 );

   g_app_text_limit_enum.insert( 1000 );
   g_app_text_limit_enum.insert( 1500 );
   g_app_text_limit_enum.insert( 2000 );
   g_app_text_limit_enum.insert( 2500 );
   g_app_text_limit_enum.insert( 3000 );
   g_app_text_limit_enum.insert( 4000 );
   g_app_text_limit_enum.insert( 5000 );
   g_app_text_limit_enum.insert( 10000 );
   g_app_text_limit_enum.insert( 20000 );
   g_app_text_limit_enum.insert( 30000 );
   g_app_text_limit_enum.insert( 40000 );
   g_app_text_limit_enum.insert( 50000 );
   g_app_text_limit_enum.insert( 60000 );

   g_app_text_trunc_limit_enum.insert( 10 );
   g_app_text_trunc_limit_enum.insert( 15 );
   g_app_text_trunc_limit_enum.insert( 20 );
   g_app_text_trunc_limit_enum.insert( 25 );
   g_app_text_trunc_limit_enum.insert( 30 );
   g_app_text_trunc_limit_enum.insert( 40 );
   g_app_text_trunc_limit_enum.insert( 50 );
   g_app_text_trunc_limit_enum.insert( 75 );
   g_app_text_trunc_limit_enum.insert( 100 );

   g_app_generate_type_enum.insert( 0 );
   g_app_generate_type_enum.insert( 1 );
   g_app_generate_type_enum.insert( 2 );

   g_app_type_enum.insert( 0 );
   g_app_type_enum.insert( 1 );

   // [<start static_class_init>]
   // [<finish static_class_init>]
}

void Meta_Application::static_class_term( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for term" );

   // [<start static_class_term>]
   // [<finish static_class_term>]
}
