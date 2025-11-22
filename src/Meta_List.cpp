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

#include "Meta_List.h"

#include "Meta_List_Field.h"
#include "Meta_Package_Option.h"
#include "Meta_Modifier.h"
#include "Meta_Permission.h"
#include "Meta_Class.h"
#include "Meta_Model.h"
#include "Meta_Field.h"
#include "Meta_List_Type.h"

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
#include "Meta_List_Field_Type.h"
// [<finish includes>]

using namespace std;

// [<start namespaces>]
// [<finish namespaces>]

template< > inline string to_string( const Meta_Modifier& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

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

template< > inline string to_string( const Meta_Field& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_List_Type& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

inline void from_string( Meta_Modifier& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
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

inline void from_string( Meta_Field& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_List_Type& c, const string& s )
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

#include "Meta_List.cmh"

const int32_t c_version = 1;

const char* const c_field_id_Access_Parent_Modifier = "301998";
const char* const c_field_id_Access_Permission = "301993";
const char* const c_field_id_Access_Restriction = "122103";
const char* const c_field_id_Allow_Anonymous_Access = "122145";
const char* const c_field_id_Allow_Quick_Link = "122119";
const char* const c_field_id_Allow_Text_Search = "122114";
const char* const c_field_id_Class = "301992";
const char* const c_field_id_Create_Only_If_Default_Other = "122140";
const char* const c_field_id_Create_Parent_Modifier = "301998a";
const char* const c_field_id_Create_Permission = "301994";
const char* const c_field_id_Create_Restriction = "122104";
const char* const c_field_id_Delete_Direction = "122151";
const char* const c_field_id_Destroy_Only_If_Default_Other = "122142";
const char* const c_field_id_Destroy_Parent_Modifier = "301998b";
const char* const c_field_id_Destroy_Permission = "301995";
const char* const c_field_id_Destroy_Restriction = "122105";
const char* const c_field_id_Direction = "122112";
const char* const c_field_id_Display_Only_If_Default_Other = "122141";
const char* const c_field_id_Display_Row_Limit = "122113";
const char* const c_field_id_Display_Security_Level = "122139";
const char* const c_field_id_Display_Sub_Totals = "122152";
const char* const c_field_id_Display_Totals = "122150";
const char* const c_field_id_File_Links_Always_As_Single = "122149";
const char* const c_field_id_Id = "122111";
const char* const c_field_id_Ignore_Implicit_Ordering = "122135";
const char* const c_field_id_Ignore_State_For_Display = "122148";
const char* const c_field_id_Ignore_Unactionable_Records = "122120";
const char* const c_field_id_Ignore_Uneditable_Parent = "122138";
const char* const c_field_id_Ignore_Username_Filter = "122124";
const char* const c_field_id_Is_Admin = "122147";
const char* const c_field_id_Is_Child = "122102";
const char* const c_field_id_Is_Home = "122121";
const char* const c_field_id_Is_Not_Anonymous = "122146";
const char* const c_field_id_Is_Variation = "122110";
const char* const c_field_id_Limit_Scroll_And_New = "122116";
const char* const c_field_id_Model = "301990";
const char* const c_field_id_Multiline_Truncate_For_Print = "122143";
const char* const c_field_id_Name = "122101";
const char* const c_field_id_Number_Multiple_Pages = "122144";
const char* const c_field_id_PDF_Font_Type = "122137";
const char* const c_field_id_PDF_List_Type = "122136";
const char* const c_field_id_Parent_Class = "301997";
const char* const c_field_id_Parent_Field = "301996";
const char* const c_field_id_Print_Restriction = "122123";
const char* const c_field_id_Print_Without_Highlight = "122118";
const char* const c_field_id_Search_Option_Limit = "122117";
const char* const c_field_id_Sort_Rows_In_UI = "122122";
const char* const c_field_id_Style = "122107";
const char* const c_field_id_Text_Match_Highlight = "122115";
const char* const c_field_id_Title = "122106";
const char* const c_field_id_Type = "301991";
const char* const c_field_id_Variation_Name = "122109";

const char* const c_field_name_Access_Parent_Modifier = "Access_Parent_Modifier";
const char* const c_field_name_Access_Permission = "Access_Permission";
const char* const c_field_name_Access_Restriction = "Access_Restriction";
const char* const c_field_name_Allow_Anonymous_Access = "Allow_Anonymous_Access";
const char* const c_field_name_Allow_Quick_Link = "Allow_Quick_Link";
const char* const c_field_name_Allow_Text_Search = "Allow_Text_Search";
const char* const c_field_name_Class = "Class";
const char* const c_field_name_Create_Only_If_Default_Other = "Create_Only_If_Default_Other";
const char* const c_field_name_Create_Parent_Modifier = "Create_Parent_Modifier";
const char* const c_field_name_Create_Permission = "Create_Permission";
const char* const c_field_name_Create_Restriction = "Create_Restriction";
const char* const c_field_name_Delete_Direction = "Delete_Direction";
const char* const c_field_name_Destroy_Only_If_Default_Other = "Destroy_Only_If_Default_Other";
const char* const c_field_name_Destroy_Parent_Modifier = "Destroy_Parent_Modifier";
const char* const c_field_name_Destroy_Permission = "Destroy_Permission";
const char* const c_field_name_Destroy_Restriction = "Destroy_Restriction";
const char* const c_field_name_Direction = "Direction";
const char* const c_field_name_Display_Only_If_Default_Other = "Display_Only_If_Default_Other";
const char* const c_field_name_Display_Row_Limit = "Display_Row_Limit";
const char* const c_field_name_Display_Security_Level = "Display_Security_Level";
const char* const c_field_name_Display_Sub_Totals = "Display_Sub_Totals";
const char* const c_field_name_Display_Totals = "Display_Totals";
const char* const c_field_name_File_Links_Always_As_Single = "File_Links_Always_As_Single";
const char* const c_field_name_Id = "Id";
const char* const c_field_name_Ignore_Implicit_Ordering = "Ignore_Implicit_Ordering";
const char* const c_field_name_Ignore_State_For_Display = "Ignore_State_For_Display";
const char* const c_field_name_Ignore_Unactionable_Records = "Ignore_Unactionable_Records";
const char* const c_field_name_Ignore_Uneditable_Parent = "Ignore_Uneditable_Parent";
const char* const c_field_name_Ignore_Username_Filter = "Ignore_Username_Filter";
const char* const c_field_name_Is_Admin = "Is_Admin";
const char* const c_field_name_Is_Child = "Is_Child";
const char* const c_field_name_Is_Home = "Is_Home";
const char* const c_field_name_Is_Not_Anonymous = "Is_Not_Anonymous";
const char* const c_field_name_Is_Variation = "Is_Variation";
const char* const c_field_name_Limit_Scroll_And_New = "Limit_Scroll_And_New";
const char* const c_field_name_Model = "Model";
const char* const c_field_name_Multiline_Truncate_For_Print = "Multiline_Truncate_For_Print";
const char* const c_field_name_Name = "Name";
const char* const c_field_name_Number_Multiple_Pages = "Number_Multiple_Pages";
const char* const c_field_name_PDF_Font_Type = "PDF_Font_Type";
const char* const c_field_name_PDF_List_Type = "PDF_List_Type";
const char* const c_field_name_Parent_Class = "Parent_Class";
const char* const c_field_name_Parent_Field = "Parent_Field";
const char* const c_field_name_Print_Restriction = "Print_Restriction";
const char* const c_field_name_Print_Without_Highlight = "Print_Without_Highlight";
const char* const c_field_name_Search_Option_Limit = "Search_Option_Limit";
const char* const c_field_name_Sort_Rows_In_UI = "Sort_Rows_In_UI";
const char* const c_field_name_Style = "Style";
const char* const c_field_name_Text_Match_Highlight = "Text_Match_Highlight";
const char* const c_field_name_Title = "Title";
const char* const c_field_name_Type = "Type";
const char* const c_field_name_Variation_Name = "Variation_Name";

const char* const c_field_display_name_Access_Parent_Modifier = "field_list_access_parent_modifier";
const char* const c_field_display_name_Access_Permission = "field_list_access_permission";
const char* const c_field_display_name_Access_Restriction = "field_list_access_restriction";
const char* const c_field_display_name_Allow_Anonymous_Access = "field_list_allow_anonymous_access";
const char* const c_field_display_name_Allow_Quick_Link = "field_list_allow_quick_link";
const char* const c_field_display_name_Allow_Text_Search = "field_list_allow_text_search";
const char* const c_field_display_name_Class = "field_list_class";
const char* const c_field_display_name_Create_Only_If_Default_Other = "field_list_create_only_if_default_other";
const char* const c_field_display_name_Create_Parent_Modifier = "field_list_create_parent_modifier";
const char* const c_field_display_name_Create_Permission = "field_list_create_permission";
const char* const c_field_display_name_Create_Restriction = "field_list_create_restriction";
const char* const c_field_display_name_Delete_Direction = "field_list_delete_direction";
const char* const c_field_display_name_Destroy_Only_If_Default_Other = "field_list_destroy_only_if_default_other";
const char* const c_field_display_name_Destroy_Parent_Modifier = "field_list_destroy_parent_modifier";
const char* const c_field_display_name_Destroy_Permission = "field_list_destroy_permission";
const char* const c_field_display_name_Destroy_Restriction = "field_list_destroy_restriction";
const char* const c_field_display_name_Direction = "field_list_direction";
const char* const c_field_display_name_Display_Only_If_Default_Other = "field_list_display_only_if_default_other";
const char* const c_field_display_name_Display_Row_Limit = "field_list_display_row_limit";
const char* const c_field_display_name_Display_Security_Level = "field_list_display_security_level";
const char* const c_field_display_name_Display_Sub_Totals = "field_list_display_sub_totals";
const char* const c_field_display_name_Display_Totals = "field_list_display_totals";
const char* const c_field_display_name_File_Links_Always_As_Single = "field_list_file_links_always_as_single";
const char* const c_field_display_name_Id = "field_list_id";
const char* const c_field_display_name_Ignore_Implicit_Ordering = "field_list_ignore_implicit_ordering";
const char* const c_field_display_name_Ignore_State_For_Display = "field_list_ignore_state_for_display";
const char* const c_field_display_name_Ignore_Unactionable_Records = "field_list_ignore_unactionable_records";
const char* const c_field_display_name_Ignore_Uneditable_Parent = "field_list_ignore_uneditable_parent";
const char* const c_field_display_name_Ignore_Username_Filter = "field_list_ignore_username_filter";
const char* const c_field_display_name_Is_Admin = "field_list_is_admin";
const char* const c_field_display_name_Is_Child = "field_list_is_child";
const char* const c_field_display_name_Is_Home = "field_list_is_home";
const char* const c_field_display_name_Is_Not_Anonymous = "field_list_is_not_anonymous";
const char* const c_field_display_name_Is_Variation = "field_list_is_variation";
const char* const c_field_display_name_Limit_Scroll_And_New = "field_list_limit_scroll_and_new";
const char* const c_field_display_name_Model = "field_list_model";
const char* const c_field_display_name_Multiline_Truncate_For_Print = "field_list_multiline_truncate_for_print";
const char* const c_field_display_name_Name = "field_list_name";
const char* const c_field_display_name_Number_Multiple_Pages = "field_list_number_multiple_pages";
const char* const c_field_display_name_PDF_Font_Type = "field_list_pdf_font_type";
const char* const c_field_display_name_PDF_List_Type = "field_list_pdf_list_type";
const char* const c_field_display_name_Parent_Class = "field_list_parent_class";
const char* const c_field_display_name_Parent_Field = "field_list_parent_field";
const char* const c_field_display_name_Print_Restriction = "field_list_print_restriction";
const char* const c_field_display_name_Print_Without_Highlight = "field_list_print_without_highlight";
const char* const c_field_display_name_Search_Option_Limit = "field_list_search_option_limit";
const char* const c_field_display_name_Sort_Rows_In_UI = "field_list_sort_rows_in_ui";
const char* const c_field_display_name_Style = "field_list_style";
const char* const c_field_display_name_Text_Match_Highlight = "field_list_text_match_highlight";
const char* const c_field_display_name_Title = "field_list_title";
const char* const c_field_display_name_Type = "field_list_type";
const char* const c_field_display_name_Variation_Name = "field_list_variation_name";

const int c_num_fields = 52;

const char* const c_all_sorted_field_ids[ ] =
{
   "122101",
   "122102",
   "122103",
   "122104",
   "122105",
   "122106",
   "122107",
   "122109",
   "122110",
   "122111",
   "122112",
   "122113",
   "122114",
   "122115",
   "122116",
   "122117",
   "122118",
   "122119",
   "122120",
   "122121",
   "122122",
   "122123",
   "122124",
   "122135",
   "122136",
   "122137",
   "122138",
   "122139",
   "122140",
   "122141",
   "122142",
   "122143",
   "122144",
   "122145",
   "122146",
   "122147",
   "122148",
   "122149",
   "122150",
   "122151",
   "122152",
   "301990",
   "301991",
   "301992",
   "301993",
   "301994",
   "301995",
   "301996",
   "301997",
   "301998",
   "301998a",
   "301998b"
};

const char* const c_all_sorted_field_names[ ] =
{
   "Access_Parent_Modifier",
   "Access_Permission",
   "Access_Restriction",
   "Allow_Anonymous_Access",
   "Allow_Quick_Link",
   "Allow_Text_Search",
   "Class",
   "Create_Only_If_Default_Other",
   "Create_Parent_Modifier",
   "Create_Permission",
   "Create_Restriction",
   "Delete_Direction",
   "Destroy_Only_If_Default_Other",
   "Destroy_Parent_Modifier",
   "Destroy_Permission",
   "Destroy_Restriction",
   "Direction",
   "Display_Only_If_Default_Other",
   "Display_Row_Limit",
   "Display_Security_Level",
   "Display_Sub_Totals",
   "Display_Totals",
   "File_Links_Always_As_Single",
   "Id",
   "Ignore_Implicit_Ordering",
   "Ignore_State_For_Display",
   "Ignore_Unactionable_Records",
   "Ignore_Uneditable_Parent",
   "Ignore_Username_Filter",
   "Is_Admin",
   "Is_Child",
   "Is_Home",
   "Is_Not_Anonymous",
   "Is_Variation",
   "Limit_Scroll_And_New",
   "Model",
   "Multiline_Truncate_For_Print",
   "Name",
   "Number_Multiple_Pages",
   "PDF_Font_Type",
   "PDF_List_Type",
   "Parent_Class",
   "Parent_Field",
   "Print_Restriction",
   "Print_Without_Highlight",
   "Search_Option_Limit",
   "Sort_Rows_In_UI",
   "Style",
   "Text_Match_Highlight",
   "Title",
   "Type",
   "Variation_Name"
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
   "122101"
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

const char* const c_procedure_id_Generate_PDF_List = "122430";

const uint64_t c_modifier_Anonymous_Disallowed = UINT64_C( 0x100 );
const uint64_t c_modifier_Cannot_Text_Search = UINT64_C( 0x200 );
const uint64_t c_modifier_Is_Admin = UINT64_C( 0x400 );
const uint64_t c_modifier_Is_Child = UINT64_C( 0x800 );
const uint64_t c_modifier_Is_Home = UINT64_C( 0x1000 );
const uint64_t c_modifier_Is_Not_Child = UINT64_C( 0x2000 );
const uint64_t c_modifier_Is_Not_Search_Style = UINT64_C( 0x4000 );
const uint64_t c_modifier_Is_Not_Unlimited = UINT64_C( 0x8000 );
const uint64_t c_modifier_PDF_List_Is_Custom = UINT64_C( 0x10000 );
const uint64_t c_modifier_PDF_List_Is_None = UINT64_C( 0x20000 );
const uint64_t c_modifier_Protect_Access = UINT64_C( 0x40000 );
const uint64_t c_modifier_Protect_Create = UINT64_C( 0x80000 );
const uint64_t c_modifier_Protect_Destroy = UINT64_C( 0x100000 );
const uint64_t c_modifier_Will_Sort_Rows_In_UI = UINT64_C( 0x200000 );

aggregate_domain< string,
 domain_string_identifier_format,
 domain_string_max_size< 30 > > g_Id_domain;
domain_string_max_size< 100 > g_Name_domain;
domain_string_max_size< 100 > g_Title_domain;
domain_string_max_size< 100 > g_Variation_Name_domain;

string g_group_field_name;
string g_level_field_name;
string g_order_field_name;
string g_owner_field_name;

string g_state_names_variable;

set< string > g_derivations;

set< string > g_file_field_ids;
set< string > g_file_field_names;

typedef map< string, Meta_List* > external_aliases_container;
typedef external_aliases_container::const_iterator external_aliases_const_iterator;
typedef external_aliases_container::value_type external_aliases_value_type;

typedef map< size_t, Meta_List* > external_aliases_lookup_container;
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

string g_default_Access_Parent_Modifier = string( );
string g_default_Access_Permission = string( );
int g_default_Access_Restriction = int( 0 );
bool g_default_Allow_Anonymous_Access = bool( 0 );
bool g_default_Allow_Quick_Link = bool( 0 );
bool g_default_Allow_Text_Search = bool( 0 );
string g_default_Class = string( );
bool g_default_Create_Only_If_Default_Other = bool( 0 );
string g_default_Create_Parent_Modifier = string( );
string g_default_Create_Permission = string( );
int g_default_Create_Restriction = int( 0 );
int g_default_Delete_Direction = int( 0 );
bool g_default_Destroy_Only_If_Default_Other = bool( 0 );
string g_default_Destroy_Parent_Modifier = string( );
string g_default_Destroy_Permission = string( );
int g_default_Destroy_Restriction = int( 0 );
int g_default_Direction = int( 0 );
bool g_default_Display_Only_If_Default_Other = bool( 0 );
int g_default_Display_Row_Limit = int( 0 );
bool g_default_Display_Security_Level = bool( 0 );
bool g_default_Display_Sub_Totals = bool( 1 );
bool g_default_Display_Totals = bool( 1 );
bool g_default_File_Links_Always_As_Single = bool( 0 );
string g_default_Id = string( );
bool g_default_Ignore_Implicit_Ordering = bool( 0 );
bool g_default_Ignore_State_For_Display = bool( 0 );
bool g_default_Ignore_Unactionable_Records = bool( 0 );
bool g_default_Ignore_Uneditable_Parent = bool( 0 );
bool g_default_Ignore_Username_Filter = bool( 0 );
bool g_default_Is_Admin = bool( 0 );
bool g_default_Is_Child = bool( 0 );
bool g_default_Is_Home = bool( 0 );
bool g_default_Is_Not_Anonymous = bool( 0 );
bool g_default_Is_Variation = bool( 0 );
bool g_default_Limit_Scroll_And_New = bool( 0 );
string g_default_Model = string( );
bool g_default_Multiline_Truncate_For_Print = bool( 1 );
string g_default_Name = string( );
bool g_default_Number_Multiple_Pages = bool( 1 );
int g_default_PDF_Font_Type = int( 0 );
int g_default_PDF_List_Type = int( 0 );
string g_default_Parent_Class = string( );
string g_default_Parent_Field = string( );
int g_default_Print_Restriction = int( 0 );
bool g_default_Print_Without_Highlight = bool( 0 );
int g_default_Search_Option_Limit = int( 0 );
bool g_default_Sort_Rows_In_UI = bool( 0 );
int g_default_Style = int( 0 );
int g_default_Text_Match_Highlight = int( 0 );
string g_default_Title = string( "*" );
string g_default_Type = string( );
string g_default_Variation_Name = string( );

set< int > g_list_restrict_enum;
set< int > g_list_direction_enum;
set< int > g_list_display_row_limit_enum;
set< int > g_pdf_font_type_enum;
set< int > g_list_pdf_list_type_enum;
set< int > g_list_print_restrict_enum;
set< int > g_list_search_opt_limit_enum;
set< int > g_list_style_enum;
set< int > g_list_text_match_highlight_enum;

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

const int c_enum_list_direction_forward( 0 );
const int c_enum_list_direction_reverse( 1 );

string get_enum_string_list_direction( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for list_direction" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_list_direction_forward";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_list_direction_reverse";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for list_direction" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_list_display_row_limit_default( 0 );
const int c_enum_list_display_row_limit_1( 1 );
const int c_enum_list_display_row_limit_2( 2 );
const int c_enum_list_display_row_limit_3( 3 );
const int c_enum_list_display_row_limit_4( 4 );
const int c_enum_list_display_row_limit_5( 5 );
const int c_enum_list_display_row_limit_6( 6 );
const int c_enum_list_display_row_limit_7( 7 );
const int c_enum_list_display_row_limit_8( 8 );
const int c_enum_list_display_row_limit_9( 9 );
const int c_enum_list_display_row_limit_10( 10 );
const int c_enum_list_display_row_limit_12( 12 );
const int c_enum_list_display_row_limit_15( 15 );
const int c_enum_list_display_row_limit_20( 20 );
const int c_enum_list_display_row_limit_25( 25 );
const int c_enum_list_display_row_limit_30( 30 );
const int c_enum_list_display_row_limit_40( 40 );
const int c_enum_list_display_row_limit_50( 50 );
const int c_enum_list_display_row_limit_75( 75 );
const int c_enum_list_display_row_limit_100( 100 );
const int c_enum_list_display_row_limit_200( 200 );
const int c_enum_list_display_row_limit_500( 500 );
const int c_enum_list_display_row_limit_unlimited( 999 );

string get_enum_string_list_display_row_limit( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for list_display_row_limit" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_list_display_row_limit_default";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_list_display_row_limit_1";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_list_display_row_limit_2";
   else if( to_string( val ) == to_string( "3" ) )
      string_name = "enum_list_display_row_limit_3";
   else if( to_string( val ) == to_string( "4" ) )
      string_name = "enum_list_display_row_limit_4";
   else if( to_string( val ) == to_string( "5" ) )
      string_name = "enum_list_display_row_limit_5";
   else if( to_string( val ) == to_string( "6" ) )
      string_name = "enum_list_display_row_limit_6";
   else if( to_string( val ) == to_string( "7" ) )
      string_name = "enum_list_display_row_limit_7";
   else if( to_string( val ) == to_string( "8" ) )
      string_name = "enum_list_display_row_limit_8";
   else if( to_string( val ) == to_string( "9" ) )
      string_name = "enum_list_display_row_limit_9";
   else if( to_string( val ) == to_string( "10" ) )
      string_name = "enum_list_display_row_limit_10";
   else if( to_string( val ) == to_string( "12" ) )
      string_name = "enum_list_display_row_limit_12";
   else if( to_string( val ) == to_string( "15" ) )
      string_name = "enum_list_display_row_limit_15";
   else if( to_string( val ) == to_string( "20" ) )
      string_name = "enum_list_display_row_limit_20";
   else if( to_string( val ) == to_string( "25" ) )
      string_name = "enum_list_display_row_limit_25";
   else if( to_string( val ) == to_string( "30" ) )
      string_name = "enum_list_display_row_limit_30";
   else if( to_string( val ) == to_string( "40" ) )
      string_name = "enum_list_display_row_limit_40";
   else if( to_string( val ) == to_string( "50" ) )
      string_name = "enum_list_display_row_limit_50";
   else if( to_string( val ) == to_string( "75" ) )
      string_name = "enum_list_display_row_limit_75";
   else if( to_string( val ) == to_string( "100" ) )
      string_name = "enum_list_display_row_limit_100";
   else if( to_string( val ) == to_string( "200" ) )
      string_name = "enum_list_display_row_limit_200";
   else if( to_string( val ) == to_string( "500" ) )
      string_name = "enum_list_display_row_limit_500";
   else if( to_string( val ) == to_string( "999" ) )
      string_name = "enum_list_display_row_limit_unlimited";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for list_display_row_limit" );

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

const int c_enum_list_pdf_list_type_none( 0 );
const int c_enum_list_pdf_list_type_Standard_Columnar( 1 );
const int c_enum_list_pdf_list_type_Simplified_Columnar( 2 );
const int c_enum_list_pdf_list_type_Customised( 99 );

string get_enum_string_list_pdf_list_type( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for list_pdf_list_type" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_list_pdf_list_type_none";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_list_pdf_list_type_Standard_Columnar";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_list_pdf_list_type_Simplified_Columnar";
   else if( to_string( val ) == to_string( "99" ) )
      string_name = "enum_list_pdf_list_type_Customised";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for list_pdf_list_type" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_list_print_restrict_none( 0 );
const int c_enum_list_print_restrict_admin_only( 1 );
const int c_enum_list_print_restrict_denied_always( 2 );

string get_enum_string_list_print_restrict( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for list_print_restrict" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_list_print_restrict_none";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_list_print_restrict_admin_only";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_list_print_restrict_denied_always";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for list_print_restrict" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_list_search_opt_limit_default( 0 );
const int c_enum_list_search_opt_limit_1( 1 );
const int c_enum_list_search_opt_limit_2( 2 );
const int c_enum_list_search_opt_limit_3( 3 );
const int c_enum_list_search_opt_limit_4( 4 );
const int c_enum_list_search_opt_limit_5( 5 );
const int c_enum_list_search_opt_limit_6( 6 );
const int c_enum_list_search_opt_limit_7( 7 );

string get_enum_string_list_search_opt_limit( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for list_search_opt_limit" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_list_search_opt_limit_default";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_list_search_opt_limit_1";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_list_search_opt_limit_2";
   else if( to_string( val ) == to_string( "3" ) )
      string_name = "enum_list_search_opt_limit_3";
   else if( to_string( val ) == to_string( "4" ) )
      string_name = "enum_list_search_opt_limit_4";
   else if( to_string( val ) == to_string( "5" ) )
      string_name = "enum_list_search_opt_limit_5";
   else if( to_string( val ) == to_string( "6" ) )
      string_name = "enum_list_search_opt_limit_6";
   else if( to_string( val ) == to_string( "7" ) )
      string_name = "enum_list_search_opt_limit_7";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for list_search_opt_limit" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_list_style_simple( 0 );
const int c_enum_list_style_search( 1 );

string get_enum_string_list_style( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for list_style" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_list_style_simple";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_list_style_search";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for list_style" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_list_text_match_highlight_none( 0 );
const int c_enum_list_text_match_highlight_highlight( 1 );
const int c_enum_list_text_match_highlight_highlight1( 2 );

string get_enum_string_list_text_match_highlight( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for list_text_match_highlight" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_list_text_match_highlight_none";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_list_text_match_highlight_highlight";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_list_text_match_highlight_highlight1";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for list_text_match_highlight" );

   return get_module_string( lower( string_name ) );
}

// [<start anonymous>]
// [<finish anonymous>]

}

registration< Meta_List > List_registration( get_class_registry( ), "122100" );

class Meta_List_command_functor;

class Meta_List_command_handler : public command_handler
{
   friend class Meta_List_command_functor;

   public:
   Meta_List_command_handler( ) : p_Meta_List( 0 ) { }

   void set_Meta_List( Meta_List* p_new_Meta_List ) { p_Meta_List = p_new_Meta_List; }

   void handle_unknown_command( const string& command, const string& cmd_and_args )
   {
      throw runtime_error( "unknown command '" + command + "'" );
   }

   void handle_invalid_command( const command_parser& parser, const string& cmd_and_args )
   {
      throw runtime_error( "invalid command usage '" + cmd_and_args + "'" );
   }

   private:
   Meta_List* p_Meta_List;

   protected:
   string retval;
};

class Meta_List_command_functor : public command_functor
{
   public:
   Meta_List_command_functor( Meta_List_command_handler& handler )
    : command_functor( handler ),
    cmd_handler( handler )
   {
   }

   void operator ( )( const string& command, const parameter_info& parameters );

   private:
   Meta_List_command_handler& cmd_handler;
};

command_functor* Meta_List_command_functor_factory( const string& /*name*/, command_handler& handler )
{
   return new Meta_List_command_functor( dynamic_cast< Meta_List_command_handler& >( handler ) );
}

void Meta_List_command_functor::operator ( )( const string& command, const parameter_info& parameters )
{
   if( command == c_cmd_Meta_List_key )
   {
      bool want_fixed( has_parm_val( parameters, c_cmd_Meta_List_key_fixed ) );

      if( !want_fixed )
         cmd_handler.retval = cmd_handler.p_Meta_List->get_key( );
      else
         cmd_handler.retval = cmd_handler.p_Meta_List->get_fixed_key( );
   }
   else if( command == c_cmd_Meta_List_ver )
   {
      string ver_rev( to_string( cmd_handler.p_Meta_List->get_version( ) ) );
      ver_rev += "." + to_string( cmd_handler.p_Meta_List->get_revision( ) );

      cmd_handler.retval = ver_rev;
   }
   else if( command == c_cmd_Meta_List_get )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_List_get_field_name ) );

      bool handled = false;

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for getter call" );

      if( !handled && ( ( field_name == c_field_id_Access_Parent_Modifier ) || ( field_name == c_field_name_Access_Parent_Modifier ) ) )
      {
         handled = true;

         string_getter< Meta_Modifier >( cmd_handler.p_Meta_List->Access_Parent_Modifier( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Access_Permission ) || ( field_name == c_field_name_Access_Permission ) ) )
      {
         handled = true;

         string_getter< Meta_Permission >( cmd_handler.p_Meta_List->Access_Permission( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Access_Restriction ) || ( field_name == c_field_name_Access_Restriction ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_List->Access_Restriction( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Anonymous_Access ) || ( field_name == c_field_name_Allow_Anonymous_Access ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List->Allow_Anonymous_Access( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Quick_Link ) || ( field_name == c_field_name_Allow_Quick_Link ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List->Allow_Quick_Link( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Text_Search ) || ( field_name == c_field_name_Allow_Text_Search ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List->Allow_Text_Search( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Class ) || ( field_name == c_field_name_Class ) ) )
      {
         handled = true;

         string_getter< Meta_Class >( cmd_handler.p_Meta_List->Class( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Create_Only_If_Default_Other ) || ( field_name == c_field_name_Create_Only_If_Default_Other ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List->Create_Only_If_Default_Other( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Create_Parent_Modifier ) || ( field_name == c_field_name_Create_Parent_Modifier ) ) )
      {
         handled = true;

         string_getter< Meta_Modifier >( cmd_handler.p_Meta_List->Create_Parent_Modifier( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Create_Permission ) || ( field_name == c_field_name_Create_Permission ) ) )
      {
         handled = true;

         string_getter< Meta_Permission >( cmd_handler.p_Meta_List->Create_Permission( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Create_Restriction ) || ( field_name == c_field_name_Create_Restriction ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_List->Create_Restriction( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Delete_Direction ) || ( field_name == c_field_name_Delete_Direction ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_List->Delete_Direction( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Destroy_Only_If_Default_Other ) || ( field_name == c_field_name_Destroy_Only_If_Default_Other ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List->Destroy_Only_If_Default_Other( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Destroy_Parent_Modifier ) || ( field_name == c_field_name_Destroy_Parent_Modifier ) ) )
      {
         handled = true;

         string_getter< Meta_Modifier >( cmd_handler.p_Meta_List->Destroy_Parent_Modifier( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Destroy_Permission ) || ( field_name == c_field_name_Destroy_Permission ) ) )
      {
         handled = true;

         string_getter< Meta_Permission >( cmd_handler.p_Meta_List->Destroy_Permission( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Destroy_Restriction ) || ( field_name == c_field_name_Destroy_Restriction ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_List->Destroy_Restriction( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Direction ) || ( field_name == c_field_name_Direction ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_List->Direction( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Display_Only_If_Default_Other ) || ( field_name == c_field_name_Display_Only_If_Default_Other ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List->Display_Only_If_Default_Other( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Display_Row_Limit ) || ( field_name == c_field_name_Display_Row_Limit ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_List->Display_Row_Limit( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Display_Security_Level ) || ( field_name == c_field_name_Display_Security_Level ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List->Display_Security_Level( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Display_Sub_Totals ) || ( field_name == c_field_name_Display_Sub_Totals ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List->Display_Sub_Totals( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Display_Totals ) || ( field_name == c_field_name_Display_Totals ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List->Display_Totals( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_File_Links_Always_As_Single ) || ( field_name == c_field_name_File_Links_Always_As_Single ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List->File_Links_Always_As_Single( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Id ) || ( field_name == c_field_name_Id ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_List->Id( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Ignore_Implicit_Ordering ) || ( field_name == c_field_name_Ignore_Implicit_Ordering ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List->Ignore_Implicit_Ordering( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Ignore_State_For_Display ) || ( field_name == c_field_name_Ignore_State_For_Display ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List->Ignore_State_For_Display( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Ignore_Unactionable_Records ) || ( field_name == c_field_name_Ignore_Unactionable_Records ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List->Ignore_Unactionable_Records( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Ignore_Uneditable_Parent ) || ( field_name == c_field_name_Ignore_Uneditable_Parent ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List->Ignore_Uneditable_Parent( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Ignore_Username_Filter ) || ( field_name == c_field_name_Ignore_Username_Filter ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List->Ignore_Username_Filter( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Is_Admin ) || ( field_name == c_field_name_Is_Admin ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List->Is_Admin( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Is_Child ) || ( field_name == c_field_name_Is_Child ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List->Is_Child( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Is_Home ) || ( field_name == c_field_name_Is_Home ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List->Is_Home( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Is_Not_Anonymous ) || ( field_name == c_field_name_Is_Not_Anonymous ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List->Is_Not_Anonymous( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Is_Variation ) || ( field_name == c_field_name_Is_Variation ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List->Is_Variation( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Limit_Scroll_And_New ) || ( field_name == c_field_name_Limit_Scroll_And_New ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List->Limit_Scroll_And_New( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Model ) || ( field_name == c_field_name_Model ) ) )
      {
         handled = true;

         string_getter< Meta_Model >( cmd_handler.p_Meta_List->Model( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Multiline_Truncate_For_Print ) || ( field_name == c_field_name_Multiline_Truncate_For_Print ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List->Multiline_Truncate_For_Print( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Name ) || ( field_name == c_field_name_Name ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_List->Name( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Number_Multiple_Pages ) || ( field_name == c_field_name_Number_Multiple_Pages ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List->Number_Multiple_Pages( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_PDF_Font_Type ) || ( field_name == c_field_name_PDF_Font_Type ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_List->PDF_Font_Type( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_PDF_List_Type ) || ( field_name == c_field_name_PDF_List_Type ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_List->PDF_List_Type( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Parent_Class ) || ( field_name == c_field_name_Parent_Class ) ) )
      {
         handled = true;

         string_getter< Meta_Class >( cmd_handler.p_Meta_List->Parent_Class( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Parent_Field ) || ( field_name == c_field_name_Parent_Field ) ) )
      {
         handled = true;

         string_getter< Meta_Field >( cmd_handler.p_Meta_List->Parent_Field( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Print_Restriction ) || ( field_name == c_field_name_Print_Restriction ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_List->Print_Restriction( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Print_Without_Highlight ) || ( field_name == c_field_name_Print_Without_Highlight ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List->Print_Without_Highlight( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Search_Option_Limit ) || ( field_name == c_field_name_Search_Option_Limit ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_List->Search_Option_Limit( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Sort_Rows_In_UI ) || ( field_name == c_field_name_Sort_Rows_In_UI ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_List->Sort_Rows_In_UI( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Style ) || ( field_name == c_field_name_Style ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_List->Style( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Text_Match_Highlight ) || ( field_name == c_field_name_Text_Match_Highlight ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_List->Text_Match_Highlight( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Title ) || ( field_name == c_field_name_Title ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_List->Title( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Type ) || ( field_name == c_field_name_Type ) ) )
      {
         handled = true;

         string_getter< Meta_List_Type >( cmd_handler.p_Meta_List->Type( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Variation_Name ) || ( field_name == c_field_name_Variation_Name ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_List->Variation_Name( ), cmd_handler.retval );
      }

      if( !handled )
         throw runtime_error( "unknown field name '" + field_name + "' for getter call" );
   }
   else if( command == c_cmd_Meta_List_set )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_List_set_field_name ) );
      string field_value( get_parm_val( parameters, c_cmd_Meta_List_set_field_value ) );

      bool handled = false;

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for setter call" );

      if( !handled && ( ( field_name == c_field_id_Access_Parent_Modifier ) || ( field_name == c_field_name_Access_Parent_Modifier ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, Meta_Modifier >(
          *cmd_handler.p_Meta_List, &Meta_List::Access_Parent_Modifier, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Access_Permission ) || ( field_name == c_field_name_Access_Permission ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, Meta_Permission >(
          *cmd_handler.p_Meta_List, &Meta_List::Access_Permission, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Access_Restriction ) || ( field_name == c_field_name_Access_Restriction ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, int >(
          *cmd_handler.p_Meta_List, &Meta_List::Access_Restriction, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Anonymous_Access ) || ( field_name == c_field_name_Allow_Anonymous_Access ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, bool >(
          *cmd_handler.p_Meta_List, &Meta_List::Allow_Anonymous_Access, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Quick_Link ) || ( field_name == c_field_name_Allow_Quick_Link ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, bool >(
          *cmd_handler.p_Meta_List, &Meta_List::Allow_Quick_Link, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Text_Search ) || ( field_name == c_field_name_Allow_Text_Search ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, bool >(
          *cmd_handler.p_Meta_List, &Meta_List::Allow_Text_Search, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Class ) || ( field_name == c_field_name_Class ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, Meta_Class >(
          *cmd_handler.p_Meta_List, &Meta_List::Class, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Create_Only_If_Default_Other ) || ( field_name == c_field_name_Create_Only_If_Default_Other ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, bool >(
          *cmd_handler.p_Meta_List, &Meta_List::Create_Only_If_Default_Other, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Create_Parent_Modifier ) || ( field_name == c_field_name_Create_Parent_Modifier ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, Meta_Modifier >(
          *cmd_handler.p_Meta_List, &Meta_List::Create_Parent_Modifier, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Create_Permission ) || ( field_name == c_field_name_Create_Permission ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, Meta_Permission >(
          *cmd_handler.p_Meta_List, &Meta_List::Create_Permission, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Create_Restriction ) || ( field_name == c_field_name_Create_Restriction ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, int >(
          *cmd_handler.p_Meta_List, &Meta_List::Create_Restriction, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Delete_Direction ) || ( field_name == c_field_name_Delete_Direction ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, int >(
          *cmd_handler.p_Meta_List, &Meta_List::Delete_Direction, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Destroy_Only_If_Default_Other ) || ( field_name == c_field_name_Destroy_Only_If_Default_Other ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, bool >(
          *cmd_handler.p_Meta_List, &Meta_List::Destroy_Only_If_Default_Other, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Destroy_Parent_Modifier ) || ( field_name == c_field_name_Destroy_Parent_Modifier ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, Meta_Modifier >(
          *cmd_handler.p_Meta_List, &Meta_List::Destroy_Parent_Modifier, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Destroy_Permission ) || ( field_name == c_field_name_Destroy_Permission ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, Meta_Permission >(
          *cmd_handler.p_Meta_List, &Meta_List::Destroy_Permission, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Destroy_Restriction ) || ( field_name == c_field_name_Destroy_Restriction ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, int >(
          *cmd_handler.p_Meta_List, &Meta_List::Destroy_Restriction, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Direction ) || ( field_name == c_field_name_Direction ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, int >(
          *cmd_handler.p_Meta_List, &Meta_List::Direction, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Display_Only_If_Default_Other ) || ( field_name == c_field_name_Display_Only_If_Default_Other ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, bool >(
          *cmd_handler.p_Meta_List, &Meta_List::Display_Only_If_Default_Other, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Display_Row_Limit ) || ( field_name == c_field_name_Display_Row_Limit ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, int >(
          *cmd_handler.p_Meta_List, &Meta_List::Display_Row_Limit, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Display_Security_Level ) || ( field_name == c_field_name_Display_Security_Level ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, bool >(
          *cmd_handler.p_Meta_List, &Meta_List::Display_Security_Level, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Display_Sub_Totals ) || ( field_name == c_field_name_Display_Sub_Totals ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, bool >(
          *cmd_handler.p_Meta_List, &Meta_List::Display_Sub_Totals, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Display_Totals ) || ( field_name == c_field_name_Display_Totals ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, bool >(
          *cmd_handler.p_Meta_List, &Meta_List::Display_Totals, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_File_Links_Always_As_Single ) || ( field_name == c_field_name_File_Links_Always_As_Single ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, bool >(
          *cmd_handler.p_Meta_List, &Meta_List::File_Links_Always_As_Single, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Id ) || ( field_name == c_field_name_Id ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, string >(
          *cmd_handler.p_Meta_List, &Meta_List::Id, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Ignore_Implicit_Ordering ) || ( field_name == c_field_name_Ignore_Implicit_Ordering ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, bool >(
          *cmd_handler.p_Meta_List, &Meta_List::Ignore_Implicit_Ordering, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Ignore_State_For_Display ) || ( field_name == c_field_name_Ignore_State_For_Display ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, bool >(
          *cmd_handler.p_Meta_List, &Meta_List::Ignore_State_For_Display, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Ignore_Unactionable_Records ) || ( field_name == c_field_name_Ignore_Unactionable_Records ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, bool >(
          *cmd_handler.p_Meta_List, &Meta_List::Ignore_Unactionable_Records, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Ignore_Uneditable_Parent ) || ( field_name == c_field_name_Ignore_Uneditable_Parent ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, bool >(
          *cmd_handler.p_Meta_List, &Meta_List::Ignore_Uneditable_Parent, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Ignore_Username_Filter ) || ( field_name == c_field_name_Ignore_Username_Filter ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, bool >(
          *cmd_handler.p_Meta_List, &Meta_List::Ignore_Username_Filter, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Is_Admin ) || ( field_name == c_field_name_Is_Admin ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, bool >(
          *cmd_handler.p_Meta_List, &Meta_List::Is_Admin, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Is_Child ) || ( field_name == c_field_name_Is_Child ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, bool >(
          *cmd_handler.p_Meta_List, &Meta_List::Is_Child, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Is_Home ) || ( field_name == c_field_name_Is_Home ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, bool >(
          *cmd_handler.p_Meta_List, &Meta_List::Is_Home, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Is_Not_Anonymous ) || ( field_name == c_field_name_Is_Not_Anonymous ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, bool >(
          *cmd_handler.p_Meta_List, &Meta_List::Is_Not_Anonymous, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Is_Variation ) || ( field_name == c_field_name_Is_Variation ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, bool >(
          *cmd_handler.p_Meta_List, &Meta_List::Is_Variation, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Limit_Scroll_And_New ) || ( field_name == c_field_name_Limit_Scroll_And_New ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, bool >(
          *cmd_handler.p_Meta_List, &Meta_List::Limit_Scroll_And_New, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Model ) || ( field_name == c_field_name_Model ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, Meta_Model >(
          *cmd_handler.p_Meta_List, &Meta_List::Model, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Multiline_Truncate_For_Print ) || ( field_name == c_field_name_Multiline_Truncate_For_Print ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, bool >(
          *cmd_handler.p_Meta_List, &Meta_List::Multiline_Truncate_For_Print, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Name ) || ( field_name == c_field_name_Name ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, string >(
          *cmd_handler.p_Meta_List, &Meta_List::Name, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Number_Multiple_Pages ) || ( field_name == c_field_name_Number_Multiple_Pages ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, bool >(
          *cmd_handler.p_Meta_List, &Meta_List::Number_Multiple_Pages, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_PDF_Font_Type ) || ( field_name == c_field_name_PDF_Font_Type ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, int >(
          *cmd_handler.p_Meta_List, &Meta_List::PDF_Font_Type, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_PDF_List_Type ) || ( field_name == c_field_name_PDF_List_Type ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, int >(
          *cmd_handler.p_Meta_List, &Meta_List::PDF_List_Type, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Parent_Class ) || ( field_name == c_field_name_Parent_Class ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, Meta_Class >(
          *cmd_handler.p_Meta_List, &Meta_List::Parent_Class, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Parent_Field ) || ( field_name == c_field_name_Parent_Field ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, Meta_Field >(
          *cmd_handler.p_Meta_List, &Meta_List::Parent_Field, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Print_Restriction ) || ( field_name == c_field_name_Print_Restriction ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, int >(
          *cmd_handler.p_Meta_List, &Meta_List::Print_Restriction, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Print_Without_Highlight ) || ( field_name == c_field_name_Print_Without_Highlight ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, bool >(
          *cmd_handler.p_Meta_List, &Meta_List::Print_Without_Highlight, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Search_Option_Limit ) || ( field_name == c_field_name_Search_Option_Limit ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, int >(
          *cmd_handler.p_Meta_List, &Meta_List::Search_Option_Limit, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Sort_Rows_In_UI ) || ( field_name == c_field_name_Sort_Rows_In_UI ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, bool >(
          *cmd_handler.p_Meta_List, &Meta_List::Sort_Rows_In_UI, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Style ) || ( field_name == c_field_name_Style ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, int >(
          *cmd_handler.p_Meta_List, &Meta_List::Style, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Text_Match_Highlight ) || ( field_name == c_field_name_Text_Match_Highlight ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, int >(
          *cmd_handler.p_Meta_List, &Meta_List::Text_Match_Highlight, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Title ) || ( field_name == c_field_name_Title ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, string >(
          *cmd_handler.p_Meta_List, &Meta_List::Title, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Type ) || ( field_name == c_field_name_Type ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, Meta_List_Type >(
          *cmd_handler.p_Meta_List, &Meta_List::Type, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Variation_Name ) || ( field_name == c_field_name_Variation_Name ) ) )
      {
         handled = true;

         func_string_setter< Meta_List, string >(
          *cmd_handler.p_Meta_List, &Meta_List::Variation_Name, field_value );
      }

      if( !handled )
         throw runtime_error( "unknown field name '" + field_name + "' for setter call" );

      cmd_handler.retval = c_okay;
   }
   else if( command == c_cmd_Meta_List_cmd )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_List_cmd_field_name ) );
      string cmd_and_args( get_parm_val( parameters, c_cmd_Meta_List_cmd_cmd_and_args ) );

      cmd_handler.retval.erase( );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for command call" );
      else if( ( field_name == c_field_id_Access_Parent_Modifier ) || ( field_name == c_field_name_Access_Parent_Modifier ) )
         cmd_handler.retval = cmd_handler.p_Meta_List->Access_Parent_Modifier( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Access_Permission ) || ( field_name == c_field_name_Access_Permission ) )
         cmd_handler.retval = cmd_handler.p_Meta_List->Access_Permission( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Class ) || ( field_name == c_field_name_Class ) )
         cmd_handler.retval = cmd_handler.p_Meta_List->Class( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Create_Parent_Modifier ) || ( field_name == c_field_name_Create_Parent_Modifier ) )
         cmd_handler.retval = cmd_handler.p_Meta_List->Create_Parent_Modifier( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Create_Permission ) || ( field_name == c_field_name_Create_Permission ) )
         cmd_handler.retval = cmd_handler.p_Meta_List->Create_Permission( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Destroy_Parent_Modifier ) || ( field_name == c_field_name_Destroy_Parent_Modifier ) )
         cmd_handler.retval = cmd_handler.p_Meta_List->Destroy_Parent_Modifier( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Destroy_Permission ) || ( field_name == c_field_name_Destroy_Permission ) )
         cmd_handler.retval = cmd_handler.p_Meta_List->Destroy_Permission( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Model ) || ( field_name == c_field_name_Model ) )
         cmd_handler.retval = cmd_handler.p_Meta_List->Model( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Parent_Class ) || ( field_name == c_field_name_Parent_Class ) )
         cmd_handler.retval = cmd_handler.p_Meta_List->Parent_Class( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Parent_Field ) || ( field_name == c_field_name_Parent_Field ) )
         cmd_handler.retval = cmd_handler.p_Meta_List->Parent_Field( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Type ) || ( field_name == c_field_name_Type ) )
         cmd_handler.retval = cmd_handler.p_Meta_List->Type( ).execute( cmd_and_args );
      else
         throw runtime_error( "unknown field name '" + field_name + "' for command call" );
   }
   else if( command == c_cmd_Meta_List_Generate_PDF_List )
   {
      int Variation_Num( get_parm_val_from_string< int >( parameters, c_cmd_Meta_List_Generate_PDF_List_Variation_Num ) );

      cmd_handler.p_Meta_List->Generate_PDF_List( Variation_Num );

      cmd_handler.retval.erase( );

   }
}

struct Meta_List::impl : public Meta_List_command_handler
{
   impl( Meta_List& o )
    :
    cp_obj( &o ),
    total_child_relationships( 0 )
   {
      p_obj = &o;
      set_Meta_List( &o );

      add_commands( 0, Meta_List_command_functor_factory,
       ARRAY_PTR_AND_SIZE( Meta_List_command_definitions ) );
   }

   Meta_List& get_obj( ) const
   {
      return *cp_obj;
   }

   int impl_Access_Restriction( ) const { return lazy_fetch( p_obj ), v_Access_Restriction; }
   void impl_Access_Restriction( int Access_Restriction ) { v_Access_Restriction = Access_Restriction; }

   bool impl_Allow_Anonymous_Access( ) const { return lazy_fetch( p_obj ), v_Allow_Anonymous_Access; }
   void impl_Allow_Anonymous_Access( bool Allow_Anonymous_Access ) { v_Allow_Anonymous_Access = Allow_Anonymous_Access; }

   bool impl_Allow_Quick_Link( ) const { return lazy_fetch( p_obj ), v_Allow_Quick_Link; }
   void impl_Allow_Quick_Link( bool Allow_Quick_Link ) { v_Allow_Quick_Link = Allow_Quick_Link; }

   bool impl_Allow_Text_Search( ) const { return lazy_fetch( p_obj ), v_Allow_Text_Search; }
   void impl_Allow_Text_Search( bool Allow_Text_Search ) { v_Allow_Text_Search = Allow_Text_Search; }

   bool impl_Create_Only_If_Default_Other( ) const { return lazy_fetch( p_obj ), v_Create_Only_If_Default_Other; }
   void impl_Create_Only_If_Default_Other( bool Create_Only_If_Default_Other ) { v_Create_Only_If_Default_Other = Create_Only_If_Default_Other; }

   int impl_Create_Restriction( ) const { return lazy_fetch( p_obj ), v_Create_Restriction; }
   void impl_Create_Restriction( int Create_Restriction ) { v_Create_Restriction = Create_Restriction; }

   int impl_Delete_Direction( ) const { return lazy_fetch( p_obj ), v_Delete_Direction; }
   void impl_Delete_Direction( int Delete_Direction ) { v_Delete_Direction = Delete_Direction; }

   bool impl_Destroy_Only_If_Default_Other( ) const { return lazy_fetch( p_obj ), v_Destroy_Only_If_Default_Other; }
   void impl_Destroy_Only_If_Default_Other( bool Destroy_Only_If_Default_Other ) { v_Destroy_Only_If_Default_Other = Destroy_Only_If_Default_Other; }

   int impl_Destroy_Restriction( ) const { return lazy_fetch( p_obj ), v_Destroy_Restriction; }
   void impl_Destroy_Restriction( int Destroy_Restriction ) { v_Destroy_Restriction = Destroy_Restriction; }

   int impl_Direction( ) const { return lazy_fetch( p_obj ), v_Direction; }
   void impl_Direction( int Direction ) { v_Direction = Direction; }

   bool impl_Display_Only_If_Default_Other( ) const { return lazy_fetch( p_obj ), v_Display_Only_If_Default_Other; }
   void impl_Display_Only_If_Default_Other( bool Display_Only_If_Default_Other ) { v_Display_Only_If_Default_Other = Display_Only_If_Default_Other; }

   int impl_Display_Row_Limit( ) const { return lazy_fetch( p_obj ), v_Display_Row_Limit; }
   void impl_Display_Row_Limit( int Display_Row_Limit ) { v_Display_Row_Limit = Display_Row_Limit; }

   bool impl_Display_Security_Level( ) const { return lazy_fetch( p_obj ), v_Display_Security_Level; }
   void impl_Display_Security_Level( bool Display_Security_Level ) { v_Display_Security_Level = Display_Security_Level; }

   bool impl_Display_Sub_Totals( ) const { return lazy_fetch( p_obj ), v_Display_Sub_Totals; }
   void impl_Display_Sub_Totals( bool Display_Sub_Totals ) { v_Display_Sub_Totals = Display_Sub_Totals; }

   bool impl_Display_Totals( ) const { return lazy_fetch( p_obj ), v_Display_Totals; }
   void impl_Display_Totals( bool Display_Totals ) { v_Display_Totals = Display_Totals; }

   bool impl_File_Links_Always_As_Single( ) const { return lazy_fetch( p_obj ), v_File_Links_Always_As_Single; }
   void impl_File_Links_Always_As_Single( bool File_Links_Always_As_Single ) { v_File_Links_Always_As_Single = File_Links_Always_As_Single; }

   const string& impl_Id( ) const { return lazy_fetch( p_obj ), v_Id; }
   void impl_Id( const string& Id ) { sanity_check( Id ); v_Id = Id; }

   bool impl_Ignore_Implicit_Ordering( ) const { return lazy_fetch( p_obj ), v_Ignore_Implicit_Ordering; }
   void impl_Ignore_Implicit_Ordering( bool Ignore_Implicit_Ordering ) { v_Ignore_Implicit_Ordering = Ignore_Implicit_Ordering; }

   bool impl_Ignore_State_For_Display( ) const { return lazy_fetch( p_obj ), v_Ignore_State_For_Display; }
   void impl_Ignore_State_For_Display( bool Ignore_State_For_Display ) { v_Ignore_State_For_Display = Ignore_State_For_Display; }

   bool impl_Ignore_Unactionable_Records( ) const { return lazy_fetch( p_obj ), v_Ignore_Unactionable_Records; }
   void impl_Ignore_Unactionable_Records( bool Ignore_Unactionable_Records ) { v_Ignore_Unactionable_Records = Ignore_Unactionable_Records; }

   bool impl_Ignore_Uneditable_Parent( ) const { return lazy_fetch( p_obj ), v_Ignore_Uneditable_Parent; }
   void impl_Ignore_Uneditable_Parent( bool Ignore_Uneditable_Parent ) { v_Ignore_Uneditable_Parent = Ignore_Uneditable_Parent; }

   bool impl_Ignore_Username_Filter( ) const { return lazy_fetch( p_obj ), v_Ignore_Username_Filter; }
   void impl_Ignore_Username_Filter( bool Ignore_Username_Filter ) { v_Ignore_Username_Filter = Ignore_Username_Filter; }

   bool impl_Is_Admin( ) const { return lazy_fetch( p_obj ), v_Is_Admin; }
   void impl_Is_Admin( bool Is_Admin ) { v_Is_Admin = Is_Admin; }

   bool impl_Is_Child( ) const { return lazy_fetch( p_obj ), v_Is_Child; }
   void impl_Is_Child( bool Is_Child ) { v_Is_Child = Is_Child; }

   bool impl_Is_Home( ) const { return lazy_fetch( p_obj ), v_Is_Home; }
   void impl_Is_Home( bool Is_Home ) { v_Is_Home = Is_Home; }

   bool impl_Is_Not_Anonymous( ) const { return lazy_fetch( p_obj ), v_Is_Not_Anonymous; }
   void impl_Is_Not_Anonymous( bool Is_Not_Anonymous ) { v_Is_Not_Anonymous = Is_Not_Anonymous; }

   bool impl_Is_Variation( ) const { return lazy_fetch( p_obj ), v_Is_Variation; }
   void impl_Is_Variation( bool Is_Variation ) { v_Is_Variation = Is_Variation; }

   bool impl_Limit_Scroll_And_New( ) const { return lazy_fetch( p_obj ), v_Limit_Scroll_And_New; }
   void impl_Limit_Scroll_And_New( bool Limit_Scroll_And_New ) { v_Limit_Scroll_And_New = Limit_Scroll_And_New; }

   bool impl_Multiline_Truncate_For_Print( ) const { return lazy_fetch( p_obj ), v_Multiline_Truncate_For_Print; }
   void impl_Multiline_Truncate_For_Print( bool Multiline_Truncate_For_Print ) { v_Multiline_Truncate_For_Print = Multiline_Truncate_For_Print; }

   const string& impl_Name( ) const { return lazy_fetch( p_obj ), v_Name; }
   void impl_Name( const string& Name ) { sanity_check( Name ); v_Name = Name; }

   bool impl_Number_Multiple_Pages( ) const { return lazy_fetch( p_obj ), v_Number_Multiple_Pages; }
   void impl_Number_Multiple_Pages( bool Number_Multiple_Pages ) { v_Number_Multiple_Pages = Number_Multiple_Pages; }

   int impl_PDF_Font_Type( ) const { return lazy_fetch( p_obj ), v_PDF_Font_Type; }
   void impl_PDF_Font_Type( int PDF_Font_Type ) { v_PDF_Font_Type = PDF_Font_Type; }

   int impl_PDF_List_Type( ) const { return lazy_fetch( p_obj ), v_PDF_List_Type; }
   void impl_PDF_List_Type( int PDF_List_Type ) { v_PDF_List_Type = PDF_List_Type; }

   int impl_Print_Restriction( ) const { return lazy_fetch( p_obj ), v_Print_Restriction; }
   void impl_Print_Restriction( int Print_Restriction ) { v_Print_Restriction = Print_Restriction; }

   bool impl_Print_Without_Highlight( ) const { return lazy_fetch( p_obj ), v_Print_Without_Highlight; }
   void impl_Print_Without_Highlight( bool Print_Without_Highlight ) { v_Print_Without_Highlight = Print_Without_Highlight; }

   int impl_Search_Option_Limit( ) const { return lazy_fetch( p_obj ), v_Search_Option_Limit; }
   void impl_Search_Option_Limit( int Search_Option_Limit ) { v_Search_Option_Limit = Search_Option_Limit; }

   bool impl_Sort_Rows_In_UI( ) const { return lazy_fetch( p_obj ), v_Sort_Rows_In_UI; }
   void impl_Sort_Rows_In_UI( bool Sort_Rows_In_UI ) { v_Sort_Rows_In_UI = Sort_Rows_In_UI; }

   int impl_Style( ) const { return lazy_fetch( p_obj ), v_Style; }
   void impl_Style( int Style ) { v_Style = Style; }

   int impl_Text_Match_Highlight( ) const { return lazy_fetch( p_obj ), v_Text_Match_Highlight; }
   void impl_Text_Match_Highlight( int Text_Match_Highlight ) { v_Text_Match_Highlight = Text_Match_Highlight; }

   const string& impl_Title( ) const { return lazy_fetch( p_obj ), v_Title; }
   void impl_Title( const string& Title ) { sanity_check( Title ); v_Title = Title; }

   const string& impl_Variation_Name( ) const { return lazy_fetch( p_obj ), v_Variation_Name; }
   void impl_Variation_Name( const string& Variation_Name ) { sanity_check( Variation_Name ); v_Variation_Name = Variation_Name; }

   Meta_Modifier& impl_Access_Parent_Modifier( )
   {
      if( !cp_Access_Parent_Modifier )
      {
         cp_Access_Parent_Modifier.init( );

         p_obj->setup_graph_parent( *cp_Access_Parent_Modifier, c_field_id_Access_Parent_Modifier, v_Access_Parent_Modifier );
      }
      return *cp_Access_Parent_Modifier;
   }

   const Meta_Modifier& impl_Access_Parent_Modifier( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Access_Parent_Modifier )
      {
         cp_Access_Parent_Modifier.init( );

         p_obj->setup_graph_parent( *cp_Access_Parent_Modifier, c_field_id_Access_Parent_Modifier, v_Access_Parent_Modifier );
      }
      return *cp_Access_Parent_Modifier;
   }

   void impl_Access_Parent_Modifier( const string& key )
   {
      class_base_accessor cba( impl_Access_Parent_Modifier( ) );
      cba.set_key( key );
   }

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

   Meta_Modifier& impl_Create_Parent_Modifier( )
   {
      if( !cp_Create_Parent_Modifier )
      {
         cp_Create_Parent_Modifier.init( );

         p_obj->setup_graph_parent( *cp_Create_Parent_Modifier, c_field_id_Create_Parent_Modifier, v_Create_Parent_Modifier );
      }
      return *cp_Create_Parent_Modifier;
   }

   const Meta_Modifier& impl_Create_Parent_Modifier( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Create_Parent_Modifier )
      {
         cp_Create_Parent_Modifier.init( );

         p_obj->setup_graph_parent( *cp_Create_Parent_Modifier, c_field_id_Create_Parent_Modifier, v_Create_Parent_Modifier );
      }
      return *cp_Create_Parent_Modifier;
   }

   void impl_Create_Parent_Modifier( const string& key )
   {
      class_base_accessor cba( impl_Create_Parent_Modifier( ) );
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

   Meta_Modifier& impl_Destroy_Parent_Modifier( )
   {
      if( !cp_Destroy_Parent_Modifier )
      {
         cp_Destroy_Parent_Modifier.init( );

         p_obj->setup_graph_parent( *cp_Destroy_Parent_Modifier, c_field_id_Destroy_Parent_Modifier, v_Destroy_Parent_Modifier );
      }
      return *cp_Destroy_Parent_Modifier;
   }

   const Meta_Modifier& impl_Destroy_Parent_Modifier( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Destroy_Parent_Modifier )
      {
         cp_Destroy_Parent_Modifier.init( );

         p_obj->setup_graph_parent( *cp_Destroy_Parent_Modifier, c_field_id_Destroy_Parent_Modifier, v_Destroy_Parent_Modifier );
      }
      return *cp_Destroy_Parent_Modifier;
   }

   void impl_Destroy_Parent_Modifier( const string& key )
   {
      class_base_accessor cba( impl_Destroy_Parent_Modifier( ) );
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

   Meta_Field& impl_Parent_Field( )
   {
      if( !cp_Parent_Field )
      {
         cp_Parent_Field.init( );

         p_obj->setup_graph_parent( *cp_Parent_Field, c_field_id_Parent_Field, v_Parent_Field );
      }
      return *cp_Parent_Field;
   }

   const Meta_Field& impl_Parent_Field( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Parent_Field )
      {
         cp_Parent_Field.init( );

         p_obj->setup_graph_parent( *cp_Parent_Field, c_field_id_Parent_Field, v_Parent_Field );
      }
      return *cp_Parent_Field;
   }

   void impl_Parent_Field( const string& key )
   {
      class_base_accessor cba( impl_Parent_Field( ) );
      cba.set_key( key );
   }

   Meta_List_Type& impl_Type( )
   {
      if( !cp_Type )
      {
         cp_Type.init( );

         p_obj->setup_graph_parent( *cp_Type, c_field_id_Type, v_Type );
      }
      return *cp_Type;
   }

   const Meta_List_Type& impl_Type( ) const
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

         p_obj->setup_graph_parent( *cp_child_Class_Created, "300632" );
      }
      return *cp_child_Class_Created;
   }

   const Meta_Class& impl_child_Class_Created( ) const
   {
      if( !cp_child_Class_Created )
      {
         cp_child_Class_Created.init( );

         p_obj->setup_graph_parent( *cp_child_Class_Created, "300632" );
      }
      return *cp_child_Class_Created;
   }

   Meta_List_Field& impl_child_List_Field( )
   {
      if( !cp_child_List_Field )
      {
         cp_child_List_Field.init( );

         p_obj->setup_graph_parent( *cp_child_List_Field, "302100" );
      }
      return *cp_child_List_Field;
   }

   const Meta_List_Field& impl_child_List_Field( ) const
   {
      if( !cp_child_List_Field )
      {
         cp_child_List_Field.init( );

         p_obj->setup_graph_parent( *cp_child_List_Field, "302100" );
      }
      return *cp_child_List_Field;
   }

   Meta_Package_Option& impl_child_Package_Option( )
   {
      if( !cp_child_Package_Option )
      {
         cp_child_Package_Option.init( );

         p_obj->setup_graph_parent( *cp_child_Package_Option, "302831" );
      }
      return *cp_child_Package_Option;
   }

   const Meta_Package_Option& impl_child_Package_Option( ) const
   {
      if( !cp_child_Package_Option )
      {
         cp_child_Package_Option.init( );

         p_obj->setup_graph_parent( *cp_child_Package_Option, "302831" );
      }
      return *cp_child_Package_Option;
   }

   void impl_Generate_PDF_List( int Variation_Num );

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

   Meta_List* p_obj;
   class_pointer< Meta_List > cp_obj;

   mutable set< string > required_transients;

   // [<start members>]
   // [<finish members>]

   size_t total_child_relationships;

   int v_Access_Restriction;
   bool v_Allow_Anonymous_Access;
   bool v_Allow_Quick_Link;
   bool v_Allow_Text_Search;
   bool v_Create_Only_If_Default_Other;
   int v_Create_Restriction;
   int v_Delete_Direction;
   bool v_Destroy_Only_If_Default_Other;
   int v_Destroy_Restriction;
   int v_Direction;
   bool v_Display_Only_If_Default_Other;
   int v_Display_Row_Limit;
   bool v_Display_Security_Level;
   bool v_Display_Sub_Totals;
   bool v_Display_Totals;
   bool v_File_Links_Always_As_Single;
   string v_Id;
   bool v_Ignore_Implicit_Ordering;
   bool v_Ignore_State_For_Display;
   bool v_Ignore_Unactionable_Records;
   bool v_Ignore_Uneditable_Parent;
   bool v_Ignore_Username_Filter;
   bool v_Is_Admin;
   bool v_Is_Child;
   bool v_Is_Home;
   bool v_Is_Not_Anonymous;
   bool v_Is_Variation;
   bool v_Limit_Scroll_And_New;
   bool v_Multiline_Truncate_For_Print;
   string v_Name;
   bool v_Number_Multiple_Pages;
   int v_PDF_Font_Type;
   int v_PDF_List_Type;
   int v_Print_Restriction;
   bool v_Print_Without_Highlight;
   int v_Search_Option_Limit;
   bool v_Sort_Rows_In_UI;
   int v_Style;
   int v_Text_Match_Highlight;
   string v_Title;
   string v_Variation_Name;

   string v_Access_Parent_Modifier;
   mutable class_pointer< Meta_Modifier > cp_Access_Parent_Modifier;

   string v_Access_Permission;
   mutable class_pointer< Meta_Permission > cp_Access_Permission;

   string v_Class;
   mutable class_pointer< Meta_Class > cp_Class;

   string v_Create_Parent_Modifier;
   mutable class_pointer< Meta_Modifier > cp_Create_Parent_Modifier;

   string v_Create_Permission;
   mutable class_pointer< Meta_Permission > cp_Create_Permission;

   string v_Destroy_Parent_Modifier;
   mutable class_pointer< Meta_Modifier > cp_Destroy_Parent_Modifier;

   string v_Destroy_Permission;
   mutable class_pointer< Meta_Permission > cp_Destroy_Permission;

   string v_Model;
   mutable class_pointer< Meta_Model > cp_Model;

   string v_Parent_Class;
   mutable class_pointer< Meta_Class > cp_Parent_Class;

   string v_Parent_Field;
   mutable class_pointer< Meta_Field > cp_Parent_Field;

   string v_Type;
   mutable class_pointer< Meta_List_Type > cp_Type;

   mutable class_pointer< Meta_Class > cp_child_Class_Created;
   mutable class_pointer< Meta_List_Field > cp_child_List_Field;
   mutable class_pointer< Meta_Package_Option > cp_child_Package_Option;
};

void Meta_List::impl::impl_Generate_PDF_List( int Variation_Num )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start Generate_PDF_List_impl>]
//nyi
   string list_ext;

   if( get_obj( ).Type( ).get_key( ) == "group" )
      list_ext = ".glist";
   else if( get_obj( ).Type( ).get_key( ) == "non_group" )
      list_ext = ".nglist";
   else if( get_obj( ).Type( ).get_key( ) == "normal" )
      list_ext = ".list";
   else if( get_obj( ).Type( ).get_key( ) == "user" )
      list_ext = ".ulist";
   else if( get_obj( ).Type( ).get_key( ) == "non_user" )
      list_ext = ".nulist";
   else
      throw runtime_error( "unexpected list type '"
       + get_obj( ).Type( ).get_key( ) + "' for Generate_PDF_List" );

   if( Variation_Num )
      list_ext += to_string( Variation_Num );

   string sh_filename( get_obj( ).Model( ).Name( ) );
   sh_filename += "_" + get_obj( ).Class( ).Name( ) + list_ext + ".pdf.sh";

   string sio_filename( get_obj( ).Model( ).Name( ) );
   sio_filename += "_" + get_obj( ).Class( ).Name( ) + list_ext + ".pdf.sio";

   string vars_filename( get_obj( ).Model( ).Name( ) );
   vars_filename += "_" + get_obj( ).Class( ).Name( ) + list_ext + ".pdf.vars.xrep";

   if( get_obj( ).PDF_List_Type( ) == c_enum_list_pdf_list_type_Standard_Columnar
    || get_obj( ).PDF_List_Type( ) == c_enum_list_pdf_list_type_Simplified_Columnar )
   {
      string key_info( to_string( Meta_List_Field::static_get_field_id( Meta_List_Field::e_field_id_Order ) ) + ' ' );
      if( get_obj( ).child_List_Field( ).iterate_forwards( key_info ) )
      {
         string fields;
         string tfields;

         string sfields;
         string stfields;
         string st1fields;
         string st2fields;

         string extra_special;

         int num_fields = 0;
         int num_tfields = 0;

         int num_sfields = 0;
         int num_stfields = 0;
         int num_st1fields = 0;
         int num_st2fields = 0;

         vector< string > field_names;
         vector< string > query_fields;
         vector< string > summary_fields;

         set< string > html_fields;

         set< string > wide_fields;
         set< string > notes_fields;
         set< string > wider_fields;
         set< string > hidden_fields;
         set< string > narrow_fields;

         set< string > total_fields;

         set< string > large_font_fields;
         set< string > small_font_fields;
         set< string > larger_font_fields;
         set< string > smaller_font_fields;

         map< string, string > headers;

         map< string, string > total_masks;

         map< string, string > extras;
         map< string, string > prefixes;
         map< string, string > suffixes;

         map< string, string > uom_fields;
         map< string, string > trunc_fields;

         map< string, string > date_fields;
         map< string, string > time_fields;
         map< string, string > numeric_fields;
         map< string, string > datetime_fields;

         map< string, string > restrict_fields;
         map< string, string > permission_fields;

         do
         {
            if( get_obj( ).child_List_Field( ).Type( ).get_key( ) != "field" )
               continue;

            if( get_obj( ).child_List_Field( ).Print_Type( ) == 1 ) // i.e. non_print
               continue;

            string name, cname;

            if( !is_null( get_obj( ).child_List_Field( ).Source_Field( ) ) )
            {
               name = get_obj( ).child_List_Field( ).Source_Field( ).Name( );
               query_fields.push_back( name );

               if( get_obj( ).child_List_Field( ).Print_Type( ) == 4 // i.e. print_summary
                || get_obj( ).child_List_Field( ).Print_Type( ) == 6 ) // i.e. print_only_summary
                  summary_fields.push_back( name );
               else
               {
                  field_names.push_back( name );

                  if( get_obj( ).PDF_List_Type( ) == c_enum_list_pdf_list_type_Standard_Columnar )
                     headers.insert( make_pair( name, "@" + name ) );
               }

               Meta_Field* p_type_field = &get_obj( ).child_List_Field( ).Source_Field( );
               if( !is_null( get_obj( ).child_List_Field( ).Use_Type_Field( ) ) )
                  p_type_field = &get_obj( ).child_List_Field( ).Use_Type_Field( );

               if( !is_null( p_type_field->Enum( ) ) )
               {
                  extras.insert( make_pair( name, "indirect" ) );

                  prefixes.insert(
                   make_pair( name, "@enum_" + p_type_field->Enum( ).Name( ) + "_" ) );
               }

               if( p_type_field->Extra( ) == 9 ) // i.e. html
                  html_fields.insert( name );
               else if( p_type_field->Extra( ) == 31 ) // i.e. special
                  extra_special = name;
               else if( p_type_field->Extra( ) == 32 ) // i.e. prefix_special
                  prefixes.insert( make_pair( name, "@special" ) );
               else if( p_type_field->Extra( ) == 33 ) // i.e. suffix_special
                  suffixes.insert( make_pair( name, "@special" ) );

               if( get_obj( ).child_List_Field( ).Access_Restriction( ) == 4 ) // i.e. denied_always
                  hidden_fields.insert( name );
               else if( p_type_field->Extra( ) == 4 // i.e. notes
                || p_type_field->Extra( ) == 9 ) // i.e. html
               {
                  notes_fields.insert( name );
                  wider_fields.insert( name );

                  trunc_fields.insert( make_pair( name,
                   to_string( get_obj( ).child_List_Field( ).Notes_Truncation( ) ) ) );
               }
               else if( p_type_field->Type( ).Primitive( ) == 0 // i.e. string
                && p_type_field->Type( ).Max_Size( ) <= 10 )
                  narrow_fields.insert( name );
               else if( p_type_field->Type( ).Primitive( ) == 0 // i.e. string
                && p_type_field->Type( ).Max_Size( ) >= 125 )
                  wider_fields.insert( name );
               else if( p_type_field->Type( ).Primitive( ) == 0 // i.e. string
                && p_type_field->Type( ).Max_Size( ) >= 50 )
                  wide_fields.insert( name );
               else if( p_type_field->Type( ).Primitive( ) == 6 ) // i.e. bool
                  narrow_fields.insert( name );
               else if( p_type_field->Type( ).Primitive( ) >= 1 // i.e. datetime
                && p_type_field->Type( ).Primitive( ) <= 5 ) // i.e. int
               {
                  string domain_mask, tmask;

                  string domain_type( meta_field_domain_type(
                   p_type_field->Enum( ).Id( ),
                   p_type_field->Primitive( ),
                   p_type_field->Type( ).Max_Size( ),
                   p_type_field->Type( ).Min_Value( ),
                   p_type_field->Type( ).Max_Value( ),
                   p_type_field->Type( ).Numeric_Digits( ),
                   p_type_field->Type( ).Numeric_Decimals( ),
                   p_type_field->Type( ).String_Domain( ),
                   p_type_field->Type( ).Date_Precision( ),
                   p_type_field->Type( ).Time_Precision( ),
                   p_type_field->Type( ).Show_Plus_Sign( ),
                   p_type_field->Type( ).Zero_Padding( ),
                   p_type_field->Type( ).Fraction_Limit( ), domain_mask, &tmask ) );

                  int uom = p_type_field->UOM( );

                  if( uom == 999 )
                     uom_fields.insert( make_pair( name, "uom" ) );
                  else if( uom != 0 )
                     uom_fields.insert( make_pair( name, meta_field_uom( uom ) ) );

                  if( p_type_field->Type( ).Primitive( ) != 1 // i.e. datetime
                   && domain_mask.size( ) <= 10 && is_null( p_type_field->Enum( ) ) )
                     narrow_fields.insert( name );

                  if( p_type_field->Type( ).Primitive( ) == 1 ) // i.e. datetime
                     datetime_fields.insert( make_pair( name, domain_mask ) );
                  else if( p_type_field->Type( ).Primitive( ) == 2 ) // i.e. date
                     date_fields.insert( make_pair( name, domain_mask ) );
                  else if( p_type_field->Type( ).Primitive( ) == 3 ) // i.e. time
                     time_fields.insert( make_pair( name, domain_mask ) );
                  else if( p_type_field->Type( ).Primitive( ) == 4 // i.e. numeric
                   || p_type_field->Type( ).Primitive( ) == 5 ) // i.e. int
                  {
                     total_masks.insert( make_pair( name, tmask ) );
                     numeric_fields.insert( make_pair( name, domain_mask ) );
                  }
               }
            }
            else if( !is_null( get_obj( ).child_List_Field( ).Source_Parent( ) ) )
            {
               name = get_obj( ).child_List_Field( ).Source_Parent( ).Name( );
               cname = get_obj( ).child_List_Field( ).Source_Child( ).Name( );

               query_fields.push_back( name + "." + cname );

               name += "_" + cname;

               if( get_obj( ).child_List_Field( ).Print_Type( ) == 4 // i.e. print_summary
                || get_obj( ).child_List_Field( ).Print_Type( ) == 6 ) // i.e. print_only_summary
                  summary_fields.push_back( name );
               else
               {
                  field_names.push_back( name );
                  headers.insert( make_pair( name, "@" + name ) );
               }

               Meta_Field* p_type_field = &get_obj( ).child_List_Field( ).Source_Child( );
               if( !is_null( get_obj( ).child_List_Field( ).Use_Type_Field( ) ) )
                  p_type_field = &get_obj( ).child_List_Field( ).Use_Type_Field( );

               if( !is_null( p_type_field->Enum( ) ) )
               {
                  extras.insert( make_pair( name, "indirect" ) );
                  prefixes.insert( make_pair( name, "@enum_" + p_type_field->Enum( ).Name( ) + "_" ) );
               }
            }
            else
               continue;

            if( get_obj( ).child_List_Field( ).Print_Type( ) != 4 // i.e. print_summary
             && get_obj( ).child_List_Field( ).Print_Type( ) != 6 ) // i.e. print_only_summary
            {
               if( !fields.empty( ) )
                  fields += " ";
               fields += "f" + to_string( ++num_fields );

               // NOTE: Double the number of fields to allow for their "headers".
               if( get_obj( ).PDF_List_Type( ) == c_enum_list_pdf_list_type_Standard_Columnar )
                  fields += " f" + to_string( ++num_fields );
            }
            else
            {
               if( !sfields.empty( ) )
                  sfields += " ";
               sfields += "sf" + to_string( ++num_sfields );
            }

            if( get_obj( ).child_List_Field( ).Print_Type( ) == 3 // i.e. print_total
             || get_obj( ).child_List_Field( ).Print_Type( ) == 5 ) // i.e. print_only_total
            {
               total_fields.insert( name );

               if( get_obj( ).Display_Totals( )
                && get_obj( ).PDF_List_Type( ) == c_enum_list_pdf_list_type_Standard_Columnar )
               {
                  if( !tfields.empty( ) )
                     tfields += " ";
                  tfields += "tf" + to_string( ++num_tfields );
               }

               if( get_obj( ).Display_Sub_Totals( ) )
               {
                  if( !stfields.empty( ) )
                     stfields += " ";
                  stfields += "stf" + to_string( ++num_stfields );

                  if( !st1fields.empty( ) )
                     st1fields += " ";
                  st1fields += "st1f" + to_string( ++num_st1fields );

                  if( !st2fields.empty( ) )
                     st2fields += " ";
                  st2fields += "st2f" + to_string( ++num_st2fields );
               }
            }

            switch( get_obj( ).child_List_Field( ).Font_Size( ) )
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
                + to_string( get_obj( ).child_List_Field( ).Font_Size( ) ) + " in generate_pdf_list" );
            }

            switch( get_obj( ).child_List_Field( ).Access_Restriction( ) )
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
               throw runtime_error( "unexpected Access_Restriction value #"
                + to_string( get_obj( ).child_List_Field( ).Access_Restriction( ) ) + " in generate_pdf_list" );
            }

            if( !is_null( get_obj( ).child_List_Field( ).Access_Permission( ) ) )
               permission_fields.insert( make_pair( name, get_obj( ).child_List_Field( ).Access_Permission( ).Name( ) ) );

         } while( get_obj( ).child_List_Field( ).iterate_next( ) );

         string qfields;
         for( size_t i = 0; i < query_fields.size( ); i++ )
         {
            if( !qfields.empty( ) )
               qfields += ',';
            qfields += query_fields[ i ];
         }

         string rfields;
         set< string > summaries;
         set< string > summaries_used;
         for( size_t i = 0; i < summary_fields.size( ); i++ )
            summaries.insert( summary_fields[ i ] );

         for( size_t i = 0; i < query_fields.size( ); i++ )
         {
            if( !rfields.empty( ) )
               rfields += ',';

            string name( query_fields[ i ] );
            string::size_type pos = name.find( '.' );
            if( pos != string::npos )
               name[ pos ] = '_';

            if( summaries.count( name )
             && !summaries_used.count( name ) )
            {
               rfields += '!';
               summaries_used.insert( name );
            }

            rfields += query_fields[ i ];
         }

         int page_width;
         if( get_obj( ).PDF_List_Type( ) == c_enum_list_pdf_list_type_Standard_Columnar )
            page_width = 800; // i.e. assumes A4 landscape reserving 50 for side padding
         else
            page_width = 550; // i.e. assumes A4 portrait reserving 50 for side padding

         int total_units = 0;
         for( size_t i = 0; i < field_names.size( ); i++ )
         {
            if( narrow_fields.count( field_names[ i ] ) )
               total_units += 2;
            else if( wide_fields.count( field_names[ i ] ) )
               total_units += 4;
            else if( wider_fields.count( field_names[ i ] ) )
               total_units += 9;
            else if( !hidden_fields.count( field_names[ i ] ) )
               total_units += 3;
         }

         int unit_size = page_width / total_units;

         string pdf_sio_xrep_name;
         switch( summary_fields.size( ) )
         {
            case 0:
            if( get_obj( ).PDF_List_Type( ) == c_enum_list_pdf_list_type_Standard_Columnar )
               pdf_sio_xrep_name = "Standard_A4_List.pdf.sio.xrep";
            else
               pdf_sio_xrep_name = "Simplified_A4_List.pdf.sio.xrep";
            break;

            case 1:
            if( get_obj( ).PDF_List_Type( ) == c_enum_list_pdf_list_type_Standard_Columnar )
               pdf_sio_xrep_name = "Standard_A4_List_Summary1.pdf.sio.xrep";
            else
               pdf_sio_xrep_name = "Simplified_A4_List_Summary1.pdf.sio.xrep";
            break;

            case 2:
            if( get_obj( ).PDF_List_Type( ) == c_enum_list_pdf_list_type_Standard_Columnar )
               pdf_sio_xrep_name = "Standard_A4_List_Summary2.pdf.sio.xrep";
            else
               pdf_sio_xrep_name = "Simplified_A4_List_Summary2.pdf.sio.xrep";
            break;

            default:
            throw runtime_error( "maximum number of print summary fields is 2" );
         }

         ofstream batf( sh_filename.c_str( ) );

         batf << "./xrep @" << pdf_sio_xrep_name << " vars=" << vars_filename << " >" << sio_filename << ".new\n";
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
            font_bname = "SimHei,Bold";
            font_encoding = "GB-EUC-H";
            break;

            default:
            throw runtime_error( "unexpected PDF_Font_Type #"
             + to_string( get_obj( ).PDF_Font_Type( ) ) + " in Generate_PDF_List" );
         }

         varsf << "\x60{\x60$font_name\x60=\x60'" << font_name << "\x60'\x60}\n";
         varsf << "\x60{\x60$font_bname\x60=\x60'" << font_bname << "\x60'\x60}\n";
         varsf << "\x60{\x60$font_encoding\x60=\x60'" << font_encoding << "\x60'\x60}\n";

         string page_count;
         if( get_obj( ).Number_Multiple_Pages( ) )
            page_count = "@@page_count";
         varsf << "\x60{\x60$page_count\x60=\x60'" << page_count << "\x60'\x60}\n";

         varsf << "\x60{\x60$display_totals\x60=\x60'" << get_obj( ).Display_Totals( ) << "\x60'\x60}\n";
         varsf << "\x60{\x60$display_sub_totals\x60=\x60'" << get_obj( ).Display_Sub_Totals( ) << "\x60'\x60}\n";

         varsf << "\x60{\x60}\n";
         varsf << "\x60{\x60$fields\x60=\x60'" << fields << "\x60'\x60}\n";
         varsf << "\x60{\x60$qfields\x60=\x60'" << qfields << "\x60'\x60}\n";
         varsf << "\x60{\x60$rfields\x60=\x60'" << rfields << "\x60'\x60}\n";
         if( !tfields.empty( ) )
            varsf << "\x60{\x60$tfields\x60=\x60'" << tfields << "\x60'\x60}\n";

         if( !sfields.empty( ) )
         {
            varsf << "\x60{\x60$sfields\x60=\x60'" << sfields << "\x60'\x60}\n";

            if( get_obj( ).Display_Sub_Totals( ) )
            {
               if( summary_fields.size( ) < 2 )
               {
                  if( !stfields.empty( ) )
                     varsf << "\x60{\x60$stfields\x60=\x60'" << stfields << "\x60'\x60}\n";
               }
               else
               {
                  varsf << "\x60{\x60$st1fields\x60=\x60'" << st1fields << "\x60'\x60}\n";
                  varsf << "\x60{\x60$st2fields\x60=\x60'" << st2fields << "\x60'\x60}\n";
               }
            }
         }

         int shpos = 0;

         for( size_t i = 0; i < summary_fields.size( ); i++ )
         {
            string next_field( summary_fields[ i ] );

            varsf << "\x60{\x60}\n";
            varsf << "\x60{\x60$sf" << to_string( i + 1 )
             << "_pos\x60=\x60'+" << shpos << ",+0\x60'\x60}\n";

            varsf << "\x60{\x60$sf" << to_string( i + 1 )
             << "_name\x60=\x60'" << next_field << "\x60'\x60}\n";

            varsf << "\x60{\x60$sf" << to_string( i + 1 )
             << "_size\x60=\x60'growable\x60'\x60}\n";

            if( summary_fields.size( ) < 2 )
               varsf << "\x60{\x60$sf" << to_string( i + 1 ) << "_group\x60=\x60'summary\x60'\x60}\n";
            else
               varsf << "\x60{\x60$sf" << to_string( i + 1 )
                << "_group\x60=\x60'summary" << ( i + 1 ) << "\x60'\x60}\n";

            if( i == 0 )
               varsf << "\x60{\x60$sf" << to_string( i + 1 )
                << "_font\x60=\x60'label\x60'\x60}\n";
            else
            {
               if( large_font_fields.count( next_field ) )
                  varsf << "\x60{\x60$sf" << to_string( i + 1 )
                   << "_font\x60=\x60'value_large\x60'\x60}\n";
               else if( small_font_fields.count( next_field ) )
                  varsf << "\x60{\x60$sf" << to_string( i + 1 )
                   << "_font\x60=\x60'value_small\x60'\x60}\n";
               else if( larger_font_fields.count( next_field ) )
                  varsf << "\x60{\x60$sf" << to_string( i + 1 )
                   << "_font\x60=\x60'value_larger\x60'\x60}\n";
               else if( smaller_font_fields.count( next_field ) )
                  varsf << "\x60{\x60$sf" << to_string( i + 1 )
                   << "_font\x60=\x60'value_smaller\x60'\x60}\n";
               else
                  varsf << "\x60{\x60$sf" << to_string( i + 1 )
                   << "_font\x60=\x60'value\x60'\x60}\n";
            }

            if( hidden_fields.count( next_field ) )
               varsf << "\x60{\x60$sf" << to_string( i + 1 )
                << "_width\x60=\x60'0\x60'\x60}\n";
            else
               varsf << "\x60{\x60$sf" << to_string( i + 1 )
                << "_width\x60=\x60'200\x60'\x60}\n";

            varsf << "\x60{\x60$sf" << to_string( i + 1 )
             << "_height\x60=\x60'1\x60'\x60}\n";

            varsf << "\x60{\x60$sf" << to_string( i + 1 )
             << "_padding\x60=\x60'0,3\x60'\x60}\n";

            varsf << "\x60{\x60$sf" << to_string( i + 1 )
             << "_data\x60=\x60'" << next_field << "\x60'\x60}\n";

            if( summary_fields.size( ) < 2 )
               varsf << "\x60{\x60$sf" << to_string( i + 1 ) << "_assign\x60=\x60'@summary\x60'\x60}\n";
            else
               varsf << "\x60{\x60$sf" << to_string( i + 1 )
                << "_assign\x60=\x60'@summary" << ( i + 1 ) << "\x60'\x60}\n";

            if( prefixes.count( next_field ) )
            {
               string prefix_value = prefixes[ next_field ];

               if( prefix_value == "@special" )
                  prefix_value = extra_special;

               if( !prefix_value.empty( ) )
                  varsf << "\x60{\x60$sf" << to_string( i + 1 )
                   << "_prefix\x60=\x60'" << prefix_value << "\x60'\x60}\n";
            }

            if( suffixes.count( next_field ) )
            {
               string suffix_value = suffixes[ next_field ];

               if( suffix_value == "@special" )
                  suffix_value = extra_special;

               if( !suffix_value.empty( ) )
                  varsf << "\x60{\x60$sf" << to_string( i + 1 )
                   << "_suffix\x60=\x60'" << suffix_value << "\x60'\x60}\n";
            }

            if( !hidden_fields.count( next_field ) )
               shpos += 10;
         }

         int fnum = 1;
         int hpos = 25; // i.e. after left side padding

         for( size_t i = 0; i < field_names.size( ); i++ )
         {
            string next_field( field_names[ i ] );

            if( headers.count( next_field ) )
            {
               varsf << "\x60{\x60}\n";
               varsf << "\x60{\x60$f" << to_string( fnum )
                << "_pos\x60=\x60'+" << hpos << ",+70\x60'\x60}\n";

               varsf << "\x60{\x60$f" << to_string( fnum )
                << "_name\x60=\x60'" << headers[ next_field ] << "\x60'\x60}\n";

               if( restrict_fields.count( next_field ) )
                  varsf << "\x60{\x60$f" << to_string( fnum )
                   << "_rest\x60=\x60'" << restrict_fields[ next_field ] << "\x60'\x60}\n";

               if( permission_fields.count( next_field ) )
                  varsf << "\x60{\x60$f" << to_string( fnum )
                   << "_perm\x60=\x60'" << permission_fields[ next_field ] << "\x60'\x60}\n";

               int width = 0;

               if( !hidden_fields.count( next_field ) )
               {
                  if( narrow_fields.count( next_field ) )
                     width = unit_size * 2;
                  else if( wide_fields.count( next_field ) )
                     width = unit_size * 4;
                  else if( wider_fields.count( next_field ) )
                     width = unit_size * 9;
                  else
                     width = unit_size * 3;

                  hpos += width;
                  width -= 10; // i.e. make sure there is some gap between fields
               }

               varsf << "\x60{\x60$f" << to_string( fnum )
                << "_size\x60=\x60'growable\x60'\x60}\n";

               varsf << "\x60{\x60$f" << to_string( fnum )
                << "_group\x60=\x60'@headers\x60'\x60}\n";

               varsf << "\x60{\x60$f" << to_string( fnum )
                << "_font\x60=\x60'label\x60'\x60}\n";

               varsf << "\x60{\x60$f" << to_string( fnum )
                << "_width\x60=\x60'" << width << "\x60'\x60}\n";

               varsf << "\x60{\x60$f" << to_string( fnum )
                << "_height\x60=\x60'1\x60'\x60}\n";

               if( numeric_fields.count( next_field ) && !prefixes.count( next_field ) )
                  varsf << "\x60{\x60$f" << to_string( fnum )
                   << "_alignment\x60=\x60'right\x60'\x60}\n";

               varsf << "\x60{\x60$f" << to_string( fnum )
                << "_data\x60=\x60'" << headers[ next_field ] << "\x60'\x60}\n";

               if( uom_fields.count( next_field ) )
                  varsf << "\x60{\x60$f" << to_string( fnum )
                   << "_suffix\x60=\x60' (" << uom_fields[ next_field ] << ")\x60'\x60}\n";

               ++fnum;
            }
         }

         hpos = 0;
         map< string, int > field_hpos;
         map< string, int > field_widths;

         for( size_t i = 0; i < field_names.size( ); i++ )
         {
            string next_field( field_names[ i ] );

            varsf << "\x60{\x60}\n";
            varsf << "\x60{\x60$f" << to_string( fnum )
             << "_pos\x60=\x60'+" << hpos << ",+0\x60'\x60}\n";

            field_hpos.insert( make_pair( next_field, hpos ) );

            varsf << "\x60{\x60$f" << to_string( fnum )
             << "_name\x60=\x60'" << next_field << "\x60'\x60}\n";

            if( restrict_fields.count( next_field ) )
               varsf << "\x60{\x60$f" << to_string( fnum )
                << "_rest\x60=\x60'" << restrict_fields[ next_field ] << "\x60'\x60}\n";

            if( permission_fields.count( next_field ) )
               varsf << "\x60{\x60$f" << to_string( fnum )
                << "_perm\x60=\x60'" << permission_fields[ next_field ] << "\x60'\x60}\n";

            int width = 0;

            if( !hidden_fields.count( next_field ) )
            {
               if( narrow_fields.count( next_field ) )
                  width = unit_size * 2;
               else if( wide_fields.count( next_field ) )
                  width = unit_size * 4;
               else if( wider_fields.count( next_field ) )
                  width = unit_size * 9;
               else
                  width = unit_size * 3;

               hpos += width;
               width -= 10; // i.e. make sure there is some gap between fields
            }

            field_widths.insert( make_pair( next_field, width ) );

            varsf << "\x60{\x60$f" << to_string( fnum )
             << "_size\x60=\x60'growable\x60'\x60}\n";

            if( extras.count( next_field ) )
               varsf << "\x60{\x60$f" << to_string( fnum )
                << "_extra\x60=\x60'" << extras[ next_field ] << "\x60'\x60}\n";

            varsf << "\x60{\x60$f" << to_string( fnum )
             << "_group\x60=\x60'data\x60'\x60}\n";

            if( large_font_fields.count( next_field ) )
               varsf << "\x60{\x60$f" << to_string( fnum )
                << "_font\x60=\x60'value_large\x60'\x60}\n";
            else if( larger_font_fields.count( next_field ) )
               varsf << "\x60{\x60$f" << to_string( fnum )
                << "_font\x60=\x60'value_larger\x60'\x60}\n";
            else if( small_font_fields.count( next_field ) )
               varsf << "\x60{\x60$f" << to_string( fnum )
                << "_font\x60=\x60'value_small\x60'\x60}\n";
            else if( smaller_font_fields.count( next_field ) )
               varsf << "\x60{\x60$f" << to_string( fnum )
                << "_font\x60=\x60'value_smaller\x60'\x60}\n";
            else
               varsf << "\x60{\x60$f" << to_string( fnum )
                << "_font\x60=\x60'value\x60'\x60}\n";

            if( date_fields.count( next_field ) )
               varsf << "\x60{\x60$f" << to_string( fnum )
                << "_mask\x60=\x60'" << date_fields[ next_field ] << "\x60'\x60}\n";
            else if( time_fields.count( next_field ) )
               varsf << "\x60{\x60$f" << to_string( fnum )
                << "_mask\x60=\x60'" << time_fields[ next_field ] << "\x60'\x60}\n";
            else if( numeric_fields.count( next_field ) )
               varsf << "\x60{\x60$f" << to_string( fnum )
                << "_mask\x60=\x60'" << numeric_fields[ next_field ] << "\x60'\x60}\n";
            else if( datetime_fields.count( next_field ) )
               varsf << "\x60{\x60$f" << to_string( fnum )
                << "_mask\x60=\x60'" << datetime_fields[ next_field ] << "\x60'\x60}\n";

            varsf << "\x60{\x60$f" << to_string( fnum )
             << "_width\x60=\x60'" << width << "\x60'\x60}\n";

            varsf << "\x60{\x60$f" << to_string( fnum )
             << "_height\x60=\x60'1\x60'\x60}\n";

            varsf << "\x60{\x60$f" << to_string( fnum )
             << "_padding\x60=\x60'0,3\x60'\x60}\n";

            if( html_fields.count( next_field ) )
               varsf << "\x60{\x60$f" << to_string( fnum )
                << "_special\x60=\x60'html\x60'\x60}\n";

            if( trunc_fields.count( next_field ) )
               varsf << "\x60{\x60$f" << to_string( fnum )
                << "_truncate\x60=\x60'" << trunc_fields[ next_field ] << "\x60'\x60}\n";

            if( numeric_fields.count( next_field ) && !prefixes.count( next_field ) )
               varsf << "\x60{\x60$f" << to_string( fnum )
                << "_alignment\x60=\x60'right\x60'\x60}\n";

            varsf << "\x60{\x60$f" << to_string( fnum )
             << "_data\x60=\x60'" << next_field << "\x60'\x60}\n";

            string dtype;
            if( date_fields.count( next_field ) )
               dtype = "date";
            else if( time_fields.count( next_field ) )
               dtype = "time";
            else if( numeric_fields.count( next_field ) )
            {
               dtype = "numeric";
               if( total_fields.count( next_field ) )
                  dtype += ":total";
            }
            else if( datetime_fields.count( next_field ) )
               dtype = "datetime";

            if( !dtype.empty( ) )
               varsf << "\x60{\x60$f" << to_string( fnum )
                << "_dtype\x60=\x60'" << dtype << "\x60'\x60}\n";

            if( prefixes.count( next_field ) )
            {
               string prefix_value = prefixes[ next_field ];

               if( prefix_value == "@special" )
                  prefix_value = extra_special;

               if( !prefix_value.empty( ) )
                  varsf << "\x60{\x60$f" << to_string( fnum )
                   << "_prefix\x60=\x60'" << prefix_value << "\x60'\x60}\n";
            }

            if( suffixes.count( next_field ) )
            {
               string suffix_value = suffixes[ next_field ];

               if( suffix_value == "@special" )
                  suffix_value = extra_special;

               if( !suffix_value.empty( ) )
                  varsf << "\x60{\x60$f" << to_string( fnum )
                   << "_suffix\x60=\x60'" << suffix_value << "\x60'\x60}\n";
            }

            ++fnum;
         }

         if( !sfields.empty( ) && !stfields.empty( ) )
         {
            for( size_t i = 0; i < summary_fields.size( ); i++ )
            {
               fnum = 1;
               for( size_t j = 0; j < field_names.size( ); j++ )
               {
                  string next_field( field_names[ j ] );

                  if( total_fields.count( next_field ) )
                  {
                     if( !field_hpos.count( next_field ) )
                        throw runtime_error( "unpexected hpos missing for field '" + next_field + "'" );

                     string prefix = "stf";
                     string suffix = "_";

                     if( summary_fields.size( ) > 1 )
                     {
                        prefix = "st" + to_string( i + 1 ) + "f";
                        suffix = to_string( i + 1 ) + "_";
                     }

                     varsf << "\x60{\x60}\n";

                     if( get_obj( ).PDF_List_Type( ) == c_enum_list_pdf_list_type_Standard_Columnar )
                        varsf << "\x60{\x60$" << prefix << to_string( fnum )
                         << "_pos\x60=\x60'+" << field_hpos[ next_field ] << ",+0\x60'\x60}\n";
                     else
                        varsf << "\x60{\x60$" << prefix << to_string( fnum )
                         << "_pos\x60=\x60'+" << field_hpos[ next_field ] << ",+10\x60'\x60}\n";

                     varsf << "\x60{\x60$" << prefix << to_string( fnum )
                      << "_name\x60=\x60'@@total_summary" << suffix << next_field << "\x60'\x60}\n";

                     if( restrict_fields.count( next_field ) )
                        varsf << "\x60{\x60$" << prefix << to_string( fnum )
                         << "_rest\x60=\x60'" << restrict_fields[ next_field ] << "\x60'\x60}\n";

                     if( permission_fields.count( next_field ) )
                        varsf << "\x60{\x60$" << prefix << to_string( fnum )
                         << "_perm\x60=\x60'" << permission_fields[ next_field ] << "\x60'\x60}\n";

                     varsf << "\x60{\x60$" << prefix << to_string( fnum )
                      << "_size\x60=\x60'growable\x60'\x60}\n";

                     if( summary_fields.size( ) < 2 )
                        varsf << "\x60{\x60$" << prefix << to_string( fnum )
                         << "_group\x60=\x60'subtotals\x60'\x60}\n";
                     else
                        varsf << "\x60{\x60$" << prefix << to_string( fnum )
                         << "_group\x60=\x60'subtotal" << to_string( i + 1 ) << "\x60'\x60}\n";

                     if( get_obj( ).PDF_List_Type( ) == c_enum_list_pdf_list_type_Standard_Columnar )
                        varsf << "\x60{\x60$" << prefix << to_string( fnum )
                         << "_font\x60=\x60'value\x60'\x60}\n";
                     else
                        varsf << "\x60{\x60$" << prefix << to_string( fnum )
                         << "_font\x60=\x60'label\x60'\x60}\n";

                     if( total_masks.count( next_field ) )
                        varsf << "\x60{\x60$" << prefix << to_string( fnum )
                         << "_mask\x60=\x60'" << total_masks[ next_field ] << "\x60'\x60}\n";

                     if( !field_widths.count( next_field ) )
                        throw runtime_error( "unpexected width missing for field '" + next_field + "'" );

                     varsf << "\x60{\x60$" << prefix << to_string( fnum )
                      << "_width\x60=\x60'" << field_widths[ next_field ] << "\x60'\x60}\n";

                     varsf << "\x60{\x60$" << prefix << to_string( fnum )
                      << "_height\x60=\x60'1\x60'\x60}\n";

                     if( numeric_fields.count( next_field ) )
                        varsf << "\x60{\x60$" << prefix << to_string( fnum )
                         << "_alignment\x60=\x60'right\x60'\x60}\n";

                     varsf << "\x60{\x60$" << prefix << to_string( fnum )
                      << "_data\x60=\x60'@@total_summary" << suffix << next_field << "\x60'\x60}\n";

                     string dtype;
                     if( date_fields.count( next_field ) )
                        dtype = "date";
                     else if( time_fields.count( next_field ) )
                        dtype = "time";
                     else if( numeric_fields.count( next_field ) )
                        dtype = "numeric";
                     else if( datetime_fields.count( next_field ) )
                        dtype = "datetime";

                     if( !dtype.empty( ) )
                        varsf << "\x60{\x60$" << prefix << to_string( fnum )
                         << "_dtype\x60=\x60'" << dtype << "\x60'\x60}\n";

                     ++fnum;
                  }
               }
            }
         }

         if( !tfields.empty( ) )
         {
            fnum = 1;
            for( size_t i = 0; i < field_names.size( ); i++ )
            {
               string next_field( field_names[ i ] );

               if( total_fields.count( next_field ) )
               {
                  if( !field_hpos.count( next_field ) )
                     throw runtime_error( "unpexected hpos missing for field '" + next_field + "'" );

                  varsf << "\x60{\x60}\n";
                  varsf << "\x60{\x60$tf" << to_string( fnum )
                   << "_pos\x60=\x60'+" << field_hpos[ next_field ] << ",+0\x60'\x60}\n";

                  varsf << "\x60{\x60$tf" << to_string( fnum )
                   << "_name\x60=\x60'@@total_" << next_field << "\x60'\x60}\n";

                  if( restrict_fields.count( next_field ) )
                     varsf << "\x60{\x60$tf" << to_string( fnum )
                      << "_rest\x60=\x60'" << restrict_fields[ next_field ] << "\x60'\x60}\n";

                  if( permission_fields.count( next_field ) )
                     varsf << "\x60{\x60$tf" << to_string( fnum )
                      << "_perm\x60=\x60'" << permission_fields[ next_field ] << "\x60'\x60}\n";

                  varsf << "\x60{\x60$tf" << to_string( fnum )
                   << "_size\x60=\x60'growable\x60'\x60}\n";

                  varsf << "\x60{\x60$tf" << to_string( fnum )
                   << "_group\x60=\x60'totals\x60'\x60}\n";

                  varsf << "\x60{\x60$tf" << to_string( fnum )
                   << "_font\x60=\x60'value\x60'\x60}\n";

                  if( total_masks.count( next_field ) )
                     varsf << "\x60{\x60$tf" << to_string( fnum )
                      << "_mask\x60=\x60'" << total_masks[ next_field ] << "\x60'\x60}\n";

                  if( !field_widths.count( next_field ) )
                     throw runtime_error( "unpexected width missing for field '" + next_field + "'" );

                  varsf << "\x60{\x60$tf" << to_string( fnum )
                   << "_width\x60=\x60'" << field_widths[ next_field ] << "\x60'\x60}\n";

                  varsf << "\x60{\x60$tf" << to_string( fnum )
                   << "_height\x60=\x60'1\x60'\x60}\n";

                  if( numeric_fields.count( next_field ) )
                     varsf << "\x60{\x60$tf" << to_string( fnum )
                      << "_alignment\x60=\x60'right\x60'\x60}\n";

                  varsf << "\x60{\x60$tf" << to_string( fnum )
                   << "_data\x60=\x60'@@total_" << next_field << "\x60'\x60}\n";

                  string dtype;
                  if( date_fields.count( next_field ) )
                     dtype = "date";
                  else if( time_fields.count( next_field ) )
                     dtype = "time";
                  else if( numeric_fields.count( next_field ) )
                     dtype = "numeric";
                  else if( datetime_fields.count( next_field ) )
                     dtype = "datetime";

                  if( !dtype.empty( ) )
                     varsf << "\x60{\x60$tf" << to_string( fnum )
                      << "_dtype\x60=\x60'" << dtype << "\x60'\x60}\n";

                  ++fnum;
               }
            }
         }

         varsf.flush( );
         if( !varsf.good( ) )
            throw runtime_error( "'" + vars_filename + "' output stream is bad" );
      }
   }
   // [<finish Generate_PDF_List_impl>]
}

string Meta_List::impl::get_field_value( int field ) const
{
   string retval;

   switch( field )
   {
      case 0:
      retval = to_string( impl_Access_Parent_Modifier( ) );
      break;

      case 1:
      retval = to_string( impl_Access_Permission( ) );
      break;

      case 2:
      retval = to_string( impl_Access_Restriction( ) );
      break;

      case 3:
      retval = to_string( impl_Allow_Anonymous_Access( ) );
      break;

      case 4:
      retval = to_string( impl_Allow_Quick_Link( ) );
      break;

      case 5:
      retval = to_string( impl_Allow_Text_Search( ) );
      break;

      case 6:
      retval = to_string( impl_Class( ) );
      break;

      case 7:
      retval = to_string( impl_Create_Only_If_Default_Other( ) );
      break;

      case 8:
      retval = to_string( impl_Create_Parent_Modifier( ) );
      break;

      case 9:
      retval = to_string( impl_Create_Permission( ) );
      break;

      case 10:
      retval = to_string( impl_Create_Restriction( ) );
      break;

      case 11:
      retval = to_string( impl_Delete_Direction( ) );
      break;

      case 12:
      retval = to_string( impl_Destroy_Only_If_Default_Other( ) );
      break;

      case 13:
      retval = to_string( impl_Destroy_Parent_Modifier( ) );
      break;

      case 14:
      retval = to_string( impl_Destroy_Permission( ) );
      break;

      case 15:
      retval = to_string( impl_Destroy_Restriction( ) );
      break;

      case 16:
      retval = to_string( impl_Direction( ) );
      break;

      case 17:
      retval = to_string( impl_Display_Only_If_Default_Other( ) );
      break;

      case 18:
      retval = to_string( impl_Display_Row_Limit( ) );
      break;

      case 19:
      retval = to_string( impl_Display_Security_Level( ) );
      break;

      case 20:
      retval = to_string( impl_Display_Sub_Totals( ) );
      break;

      case 21:
      retval = to_string( impl_Display_Totals( ) );
      break;

      case 22:
      retval = to_string( impl_File_Links_Always_As_Single( ) );
      break;

      case 23:
      retval = to_string( impl_Id( ) );
      break;

      case 24:
      retval = to_string( impl_Ignore_Implicit_Ordering( ) );
      break;

      case 25:
      retval = to_string( impl_Ignore_State_For_Display( ) );
      break;

      case 26:
      retval = to_string( impl_Ignore_Unactionable_Records( ) );
      break;

      case 27:
      retval = to_string( impl_Ignore_Uneditable_Parent( ) );
      break;

      case 28:
      retval = to_string( impl_Ignore_Username_Filter( ) );
      break;

      case 29:
      retval = to_string( impl_Is_Admin( ) );
      break;

      case 30:
      retval = to_string( impl_Is_Child( ) );
      break;

      case 31:
      retval = to_string( impl_Is_Home( ) );
      break;

      case 32:
      retval = to_string( impl_Is_Not_Anonymous( ) );
      break;

      case 33:
      retval = to_string( impl_Is_Variation( ) );
      break;

      case 34:
      retval = to_string( impl_Limit_Scroll_And_New( ) );
      break;

      case 35:
      retval = to_string( impl_Model( ) );
      break;

      case 36:
      retval = to_string( impl_Multiline_Truncate_For_Print( ) );
      break;

      case 37:
      retval = to_string( impl_Name( ) );
      break;

      case 38:
      retval = to_string( impl_Number_Multiple_Pages( ) );
      break;

      case 39:
      retval = to_string( impl_PDF_Font_Type( ) );
      break;

      case 40:
      retval = to_string( impl_PDF_List_Type( ) );
      break;

      case 41:
      retval = to_string( impl_Parent_Class( ) );
      break;

      case 42:
      retval = to_string( impl_Parent_Field( ) );
      break;

      case 43:
      retval = to_string( impl_Print_Restriction( ) );
      break;

      case 44:
      retval = to_string( impl_Print_Without_Highlight( ) );
      break;

      case 45:
      retval = to_string( impl_Search_Option_Limit( ) );
      break;

      case 46:
      retval = to_string( impl_Sort_Rows_In_UI( ) );
      break;

      case 47:
      retval = to_string( impl_Style( ) );
      break;

      case 48:
      retval = to_string( impl_Text_Match_Highlight( ) );
      break;

      case 49:
      retval = to_string( impl_Title( ) );
      break;

      case 50:
      retval = to_string( impl_Type( ) );
      break;

      case 51:
      retval = to_string( impl_Variation_Name( ) );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in get field value" );
   }

   return retval;
}

void Meta_List::impl::set_field_value( int field, const string& value )
{
   switch( field )
   {
      case 0:
      func_string_setter< Meta_List::impl, Meta_Modifier >( *this, &Meta_List::impl::impl_Access_Parent_Modifier, value );
      break;

      case 1:
      func_string_setter< Meta_List::impl, Meta_Permission >( *this, &Meta_List::impl::impl_Access_Permission, value );
      break;

      case 2:
      func_string_setter< Meta_List::impl, int >( *this, &Meta_List::impl::impl_Access_Restriction, value );
      break;

      case 3:
      func_string_setter< Meta_List::impl, bool >( *this, &Meta_List::impl::impl_Allow_Anonymous_Access, value );
      break;

      case 4:
      func_string_setter< Meta_List::impl, bool >( *this, &Meta_List::impl::impl_Allow_Quick_Link, value );
      break;

      case 5:
      func_string_setter< Meta_List::impl, bool >( *this, &Meta_List::impl::impl_Allow_Text_Search, value );
      break;

      case 6:
      func_string_setter< Meta_List::impl, Meta_Class >( *this, &Meta_List::impl::impl_Class, value );
      break;

      case 7:
      func_string_setter< Meta_List::impl, bool >( *this, &Meta_List::impl::impl_Create_Only_If_Default_Other, value );
      break;

      case 8:
      func_string_setter< Meta_List::impl, Meta_Modifier >( *this, &Meta_List::impl::impl_Create_Parent_Modifier, value );
      break;

      case 9:
      func_string_setter< Meta_List::impl, Meta_Permission >( *this, &Meta_List::impl::impl_Create_Permission, value );
      break;

      case 10:
      func_string_setter< Meta_List::impl, int >( *this, &Meta_List::impl::impl_Create_Restriction, value );
      break;

      case 11:
      func_string_setter< Meta_List::impl, int >( *this, &Meta_List::impl::impl_Delete_Direction, value );
      break;

      case 12:
      func_string_setter< Meta_List::impl, bool >( *this, &Meta_List::impl::impl_Destroy_Only_If_Default_Other, value );
      break;

      case 13:
      func_string_setter< Meta_List::impl, Meta_Modifier >( *this, &Meta_List::impl::impl_Destroy_Parent_Modifier, value );
      break;

      case 14:
      func_string_setter< Meta_List::impl, Meta_Permission >( *this, &Meta_List::impl::impl_Destroy_Permission, value );
      break;

      case 15:
      func_string_setter< Meta_List::impl, int >( *this, &Meta_List::impl::impl_Destroy_Restriction, value );
      break;

      case 16:
      func_string_setter< Meta_List::impl, int >( *this, &Meta_List::impl::impl_Direction, value );
      break;

      case 17:
      func_string_setter< Meta_List::impl, bool >( *this, &Meta_List::impl::impl_Display_Only_If_Default_Other, value );
      break;

      case 18:
      func_string_setter< Meta_List::impl, int >( *this, &Meta_List::impl::impl_Display_Row_Limit, value );
      break;

      case 19:
      func_string_setter< Meta_List::impl, bool >( *this, &Meta_List::impl::impl_Display_Security_Level, value );
      break;

      case 20:
      func_string_setter< Meta_List::impl, bool >( *this, &Meta_List::impl::impl_Display_Sub_Totals, value );
      break;

      case 21:
      func_string_setter< Meta_List::impl, bool >( *this, &Meta_List::impl::impl_Display_Totals, value );
      break;

      case 22:
      func_string_setter< Meta_List::impl, bool >( *this, &Meta_List::impl::impl_File_Links_Always_As_Single, value );
      break;

      case 23:
      func_string_setter< Meta_List::impl, string >( *this, &Meta_List::impl::impl_Id, value );
      break;

      case 24:
      func_string_setter< Meta_List::impl, bool >( *this, &Meta_List::impl::impl_Ignore_Implicit_Ordering, value );
      break;

      case 25:
      func_string_setter< Meta_List::impl, bool >( *this, &Meta_List::impl::impl_Ignore_State_For_Display, value );
      break;

      case 26:
      func_string_setter< Meta_List::impl, bool >( *this, &Meta_List::impl::impl_Ignore_Unactionable_Records, value );
      break;

      case 27:
      func_string_setter< Meta_List::impl, bool >( *this, &Meta_List::impl::impl_Ignore_Uneditable_Parent, value );
      break;

      case 28:
      func_string_setter< Meta_List::impl, bool >( *this, &Meta_List::impl::impl_Ignore_Username_Filter, value );
      break;

      case 29:
      func_string_setter< Meta_List::impl, bool >( *this, &Meta_List::impl::impl_Is_Admin, value );
      break;

      case 30:
      func_string_setter< Meta_List::impl, bool >( *this, &Meta_List::impl::impl_Is_Child, value );
      break;

      case 31:
      func_string_setter< Meta_List::impl, bool >( *this, &Meta_List::impl::impl_Is_Home, value );
      break;

      case 32:
      func_string_setter< Meta_List::impl, bool >( *this, &Meta_List::impl::impl_Is_Not_Anonymous, value );
      break;

      case 33:
      func_string_setter< Meta_List::impl, bool >( *this, &Meta_List::impl::impl_Is_Variation, value );
      break;

      case 34:
      func_string_setter< Meta_List::impl, bool >( *this, &Meta_List::impl::impl_Limit_Scroll_And_New, value );
      break;

      case 35:
      func_string_setter< Meta_List::impl, Meta_Model >( *this, &Meta_List::impl::impl_Model, value );
      break;

      case 36:
      func_string_setter< Meta_List::impl, bool >( *this, &Meta_List::impl::impl_Multiline_Truncate_For_Print, value );
      break;

      case 37:
      func_string_setter< Meta_List::impl, string >( *this, &Meta_List::impl::impl_Name, value );
      break;

      case 38:
      func_string_setter< Meta_List::impl, bool >( *this, &Meta_List::impl::impl_Number_Multiple_Pages, value );
      break;

      case 39:
      func_string_setter< Meta_List::impl, int >( *this, &Meta_List::impl::impl_PDF_Font_Type, value );
      break;

      case 40:
      func_string_setter< Meta_List::impl, int >( *this, &Meta_List::impl::impl_PDF_List_Type, value );
      break;

      case 41:
      func_string_setter< Meta_List::impl, Meta_Class >( *this, &Meta_List::impl::impl_Parent_Class, value );
      break;

      case 42:
      func_string_setter< Meta_List::impl, Meta_Field >( *this, &Meta_List::impl::impl_Parent_Field, value );
      break;

      case 43:
      func_string_setter< Meta_List::impl, int >( *this, &Meta_List::impl::impl_Print_Restriction, value );
      break;

      case 44:
      func_string_setter< Meta_List::impl, bool >( *this, &Meta_List::impl::impl_Print_Without_Highlight, value );
      break;

      case 45:
      func_string_setter< Meta_List::impl, int >( *this, &Meta_List::impl::impl_Search_Option_Limit, value );
      break;

      case 46:
      func_string_setter< Meta_List::impl, bool >( *this, &Meta_List::impl::impl_Sort_Rows_In_UI, value );
      break;

      case 47:
      func_string_setter< Meta_List::impl, int >( *this, &Meta_List::impl::impl_Style, value );
      break;

      case 48:
      func_string_setter< Meta_List::impl, int >( *this, &Meta_List::impl::impl_Text_Match_Highlight, value );
      break;

      case 49:
      func_string_setter< Meta_List::impl, string >( *this, &Meta_List::impl::impl_Title, value );
      break;

      case 50:
      func_string_setter< Meta_List::impl, Meta_List_Type >( *this, &Meta_List::impl::impl_Type, value );
      break;

      case 51:
      func_string_setter< Meta_List::impl, string >( *this, &Meta_List::impl::impl_Variation_Name, value );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in set field value" );
   }
}

void Meta_List::impl::set_field_default( int field )
{
   switch( field )
   {
      case 0:
      impl_Access_Parent_Modifier( g_default_Access_Parent_Modifier );
      break;

      case 1:
      impl_Access_Permission( g_default_Access_Permission );
      break;

      case 2:
      impl_Access_Restriction( g_default_Access_Restriction );
      break;

      case 3:
      impl_Allow_Anonymous_Access( g_default_Allow_Anonymous_Access );
      break;

      case 4:
      impl_Allow_Quick_Link( g_default_Allow_Quick_Link );
      break;

      case 5:
      impl_Allow_Text_Search( g_default_Allow_Text_Search );
      break;

      case 6:
      impl_Class( g_default_Class );
      break;

      case 7:
      impl_Create_Only_If_Default_Other( g_default_Create_Only_If_Default_Other );
      break;

      case 8:
      impl_Create_Parent_Modifier( g_default_Create_Parent_Modifier );
      break;

      case 9:
      impl_Create_Permission( g_default_Create_Permission );
      break;

      case 10:
      impl_Create_Restriction( g_default_Create_Restriction );
      break;

      case 11:
      impl_Delete_Direction( g_default_Delete_Direction );
      break;

      case 12:
      impl_Destroy_Only_If_Default_Other( g_default_Destroy_Only_If_Default_Other );
      break;

      case 13:
      impl_Destroy_Parent_Modifier( g_default_Destroy_Parent_Modifier );
      break;

      case 14:
      impl_Destroy_Permission( g_default_Destroy_Permission );
      break;

      case 15:
      impl_Destroy_Restriction( g_default_Destroy_Restriction );
      break;

      case 16:
      impl_Direction( g_default_Direction );
      break;

      case 17:
      impl_Display_Only_If_Default_Other( g_default_Display_Only_If_Default_Other );
      break;

      case 18:
      impl_Display_Row_Limit( g_default_Display_Row_Limit );
      break;

      case 19:
      impl_Display_Security_Level( g_default_Display_Security_Level );
      break;

      case 20:
      impl_Display_Sub_Totals( g_default_Display_Sub_Totals );
      break;

      case 21:
      impl_Display_Totals( g_default_Display_Totals );
      break;

      case 22:
      impl_File_Links_Always_As_Single( g_default_File_Links_Always_As_Single );
      break;

      case 23:
      impl_Id( g_default_Id );
      break;

      case 24:
      impl_Ignore_Implicit_Ordering( g_default_Ignore_Implicit_Ordering );
      break;

      case 25:
      impl_Ignore_State_For_Display( g_default_Ignore_State_For_Display );
      break;

      case 26:
      impl_Ignore_Unactionable_Records( g_default_Ignore_Unactionable_Records );
      break;

      case 27:
      impl_Ignore_Uneditable_Parent( g_default_Ignore_Uneditable_Parent );
      break;

      case 28:
      impl_Ignore_Username_Filter( g_default_Ignore_Username_Filter );
      break;

      case 29:
      impl_Is_Admin( g_default_Is_Admin );
      break;

      case 30:
      impl_Is_Child( g_default_Is_Child );
      break;

      case 31:
      impl_Is_Home( g_default_Is_Home );
      break;

      case 32:
      impl_Is_Not_Anonymous( g_default_Is_Not_Anonymous );
      break;

      case 33:
      impl_Is_Variation( g_default_Is_Variation );
      break;

      case 34:
      impl_Limit_Scroll_And_New( g_default_Limit_Scroll_And_New );
      break;

      case 35:
      impl_Model( g_default_Model );
      break;

      case 36:
      impl_Multiline_Truncate_For_Print( g_default_Multiline_Truncate_For_Print );
      break;

      case 37:
      impl_Name( g_default_Name );
      break;

      case 38:
      impl_Number_Multiple_Pages( g_default_Number_Multiple_Pages );
      break;

      case 39:
      impl_PDF_Font_Type( g_default_PDF_Font_Type );
      break;

      case 40:
      impl_PDF_List_Type( g_default_PDF_List_Type );
      break;

      case 41:
      impl_Parent_Class( g_default_Parent_Class );
      break;

      case 42:
      impl_Parent_Field( g_default_Parent_Field );
      break;

      case 43:
      impl_Print_Restriction( g_default_Print_Restriction );
      break;

      case 44:
      impl_Print_Without_Highlight( g_default_Print_Without_Highlight );
      break;

      case 45:
      impl_Search_Option_Limit( g_default_Search_Option_Limit );
      break;

      case 46:
      impl_Sort_Rows_In_UI( g_default_Sort_Rows_In_UI );
      break;

      case 47:
      impl_Style( g_default_Style );
      break;

      case 48:
      impl_Text_Match_Highlight( g_default_Text_Match_Highlight );
      break;

      case 49:
      impl_Title( g_default_Title );
      break;

      case 50:
      impl_Type( g_default_Type );
      break;

      case 51:
      impl_Variation_Name( g_default_Variation_Name );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in set field default" );
   }
}

bool Meta_List::impl::is_field_default( int field ) const
{
   bool retval = false;

   switch( field )
   {
      case 0:
      retval = ( v_Access_Parent_Modifier == g_default_Access_Parent_Modifier );
      break;

      case 1:
      retval = ( v_Access_Permission == g_default_Access_Permission );
      break;

      case 2:
      retval = ( v_Access_Restriction == g_default_Access_Restriction );
      break;

      case 3:
      retval = ( v_Allow_Anonymous_Access == g_default_Allow_Anonymous_Access );
      break;

      case 4:
      retval = ( v_Allow_Quick_Link == g_default_Allow_Quick_Link );
      break;

      case 5:
      retval = ( v_Allow_Text_Search == g_default_Allow_Text_Search );
      break;

      case 6:
      retval = ( v_Class == g_default_Class );
      break;

      case 7:
      retval = ( v_Create_Only_If_Default_Other == g_default_Create_Only_If_Default_Other );
      break;

      case 8:
      retval = ( v_Create_Parent_Modifier == g_default_Create_Parent_Modifier );
      break;

      case 9:
      retval = ( v_Create_Permission == g_default_Create_Permission );
      break;

      case 10:
      retval = ( v_Create_Restriction == g_default_Create_Restriction );
      break;

      case 11:
      retval = ( v_Delete_Direction == g_default_Delete_Direction );
      break;

      case 12:
      retval = ( v_Destroy_Only_If_Default_Other == g_default_Destroy_Only_If_Default_Other );
      break;

      case 13:
      retval = ( v_Destroy_Parent_Modifier == g_default_Destroy_Parent_Modifier );
      break;

      case 14:
      retval = ( v_Destroy_Permission == g_default_Destroy_Permission );
      break;

      case 15:
      retval = ( v_Destroy_Restriction == g_default_Destroy_Restriction );
      break;

      case 16:
      retval = ( v_Direction == g_default_Direction );
      break;

      case 17:
      retval = ( v_Display_Only_If_Default_Other == g_default_Display_Only_If_Default_Other );
      break;

      case 18:
      retval = ( v_Display_Row_Limit == g_default_Display_Row_Limit );
      break;

      case 19:
      retval = ( v_Display_Security_Level == g_default_Display_Security_Level );
      break;

      case 20:
      retval = ( v_Display_Sub_Totals == g_default_Display_Sub_Totals );
      break;

      case 21:
      retval = ( v_Display_Totals == g_default_Display_Totals );
      break;

      case 22:
      retval = ( v_File_Links_Always_As_Single == g_default_File_Links_Always_As_Single );
      break;

      case 23:
      retval = ( v_Id == g_default_Id );
      break;

      case 24:
      retval = ( v_Ignore_Implicit_Ordering == g_default_Ignore_Implicit_Ordering );
      break;

      case 25:
      retval = ( v_Ignore_State_For_Display == g_default_Ignore_State_For_Display );
      break;

      case 26:
      retval = ( v_Ignore_Unactionable_Records == g_default_Ignore_Unactionable_Records );
      break;

      case 27:
      retval = ( v_Ignore_Uneditable_Parent == g_default_Ignore_Uneditable_Parent );
      break;

      case 28:
      retval = ( v_Ignore_Username_Filter == g_default_Ignore_Username_Filter );
      break;

      case 29:
      retval = ( v_Is_Admin == g_default_Is_Admin );
      break;

      case 30:
      retval = ( v_Is_Child == g_default_Is_Child );
      break;

      case 31:
      retval = ( v_Is_Home == g_default_Is_Home );
      break;

      case 32:
      retval = ( v_Is_Not_Anonymous == g_default_Is_Not_Anonymous );
      break;

      case 33:
      retval = ( v_Is_Variation == g_default_Is_Variation );
      break;

      case 34:
      retval = ( v_Limit_Scroll_And_New == g_default_Limit_Scroll_And_New );
      break;

      case 35:
      retval = ( v_Model == g_default_Model );
      break;

      case 36:
      retval = ( v_Multiline_Truncate_For_Print == g_default_Multiline_Truncate_For_Print );
      break;

      case 37:
      retval = ( v_Name == g_default_Name );
      break;

      case 38:
      retval = ( v_Number_Multiple_Pages == g_default_Number_Multiple_Pages );
      break;

      case 39:
      retval = ( v_PDF_Font_Type == g_default_PDF_Font_Type );
      break;

      case 40:
      retval = ( v_PDF_List_Type == g_default_PDF_List_Type );
      break;

      case 41:
      retval = ( v_Parent_Class == g_default_Parent_Class );
      break;

      case 42:
      retval = ( v_Parent_Field == g_default_Parent_Field );
      break;

      case 43:
      retval = ( v_Print_Restriction == g_default_Print_Restriction );
      break;

      case 44:
      retval = ( v_Print_Without_Highlight == g_default_Print_Without_Highlight );
      break;

      case 45:
      retval = ( v_Search_Option_Limit == g_default_Search_Option_Limit );
      break;

      case 46:
      retval = ( v_Sort_Rows_In_UI == g_default_Sort_Rows_In_UI );
      break;

      case 47:
      retval = ( v_Style == g_default_Style );
      break;

      case 48:
      retval = ( v_Text_Match_Highlight == g_default_Text_Match_Highlight );
      break;

      case 49:
      retval = ( v_Title == g_default_Title );
      break;

      case 50:
      retval = ( v_Type == g_default_Type );
      break;

      case 51:
      retval = ( v_Variation_Name == g_default_Variation_Name );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in is_field_default" );
   }

   return retval;
}

uint64_t Meta_List::impl::get_state( ) const
{
   uint64_t state = 0;

   // [(start modifier_field_value)] 600427
   if( get_obj( ).PDF_List_Type( ) == 0 )
      state |= c_modifier_PDF_List_Is_None;
   // [(finish modifier_field_value)] 600427

   // [(start modifier_field_value)] 600428
   if( get_obj( ).PDF_List_Type( ) == 99 )
      state |= c_modifier_PDF_List_Is_Custom;
   // [(finish modifier_field_value)] 600428

   // [(start modifier_field_value)] 600430
   if( get_obj( ).Is_Child( ) == true )
      state |= c_modifier_Is_Child;
   // [(finish modifier_field_value)] 600430

   // [(start modifier_field_value)] 600431
   if( get_obj( ).Is_Child( ) == false )
      state |= c_modifier_Is_Not_Child;
   // [(finish modifier_field_value)] 600431

   // [(start modifier_field_value)] 600432
   if( get_obj( ).Allow_Text_Search( ) == false )
      state |= c_modifier_Cannot_Text_Search;
   // [(finish modifier_field_value)] 600432

   // [(start modifier_field_value)] 600433
   if( get_obj( ).Style( ) != 1 ) // i.e. search
      state |= c_modifier_Is_Not_Search_Style;
   // [(finish modifier_field_value)] 600433

   // [(start modifier_field_value)] 600434
   if( get_obj( ).Is_Home( ) == true )
      state |= c_modifier_Is_Home;
   // [(finish modifier_field_value)] 600434

   // [(start modifier_field_value)] 600434a
   if( get_obj( ).Is_Not_Anonymous( ) == true )
      state |= c_modifier_Anonymous_Disallowed;
   // [(finish modifier_field_value)] 600434a

   // [(start modifier_field_value)] 600434b
   if( get_obj( ).Access_Restriction( ) != 0 )
      state |= c_modifier_Anonymous_Disallowed;
   // [(finish modifier_field_value)] 600434b

   // [(start modifier_field_null)] 600434c
   if( !is_null( get_obj( ).Access_Permission( ) ) )
      state |= c_modifier_Anonymous_Disallowed;
   // [(finish modifier_field_null)] 600434c

   // [(start modifier_field_value)] 600435
   if( get_obj( ).Display_Row_Limit( ) != 999 )
      state |= c_modifier_Is_Not_Unlimited;
   // [(finish modifier_field_value)] 600435

   // [(start modifier_field_value)] 600436
   if( get_obj( ).Sort_Rows_In_UI( ) == true )
      state |= c_modifier_Will_Sort_Rows_In_UI;
   // [(finish modifier_field_value)] 600436

   // [(start modifier_field_value)] 600437
   if( get_obj( ).Is_Admin( ) == true )
      state |= c_modifier_Is_Admin;
   // [(finish modifier_field_value)] 600437

   // [<start get_state>]
//nyi
   // NOTE: Anonymous is permitted even if access is
   // restricted (for "home" to become "home_anon").
   if( get_obj( ).Is_Home( ) == true )
      state &= ~c_modifier_Anonymous_Disallowed;

   if( get_obj( ).Class( ).Access_Restriction( ) != 0
    || !is_null( get_obj( ).Class( ).Access_Permission( ) ) )
      state |= c_modifier_Protect_Access;

   if( get_obj( ).Class( ).Create_Restriction( ) != 0
    || !is_null( get_obj( ).Class( ).Create_Permission( ) ) )
      state |= c_modifier_Protect_Create;

   if( get_obj( ).Class( ).Destroy_Restriction( ) != 0
    || !is_null( get_obj( ).Class( ).Destroy_Permission( ) ) )
      state |= c_modifier_Protect_Destroy;
   // [<finish get_state>]

   return state;
}

string Meta_List::impl::get_state_names( ) const
{
   string state_names;
   uint64_t state = get_state( );

   if( state & c_modifier_Anonymous_Disallowed )
      state_names += "|" + string( "Anonymous_Disallowed" );
   if( state & c_modifier_Cannot_Text_Search )
      state_names += "|" + string( "Cannot_Text_Search" );
   if( state & c_modifier_Is_Admin )
      state_names += "|" + string( "Is_Admin" );
   if( state & c_modifier_Is_Child )
      state_names += "|" + string( "Is_Child" );
   if( state & c_modifier_Is_Home )
      state_names += "|" + string( "Is_Home" );
   if( state & c_modifier_Is_Not_Child )
      state_names += "|" + string( "Is_Not_Child" );
   if( state & c_modifier_Is_Not_Search_Style )
      state_names += "|" + string( "Is_Not_Search_Style" );
   if( state & c_modifier_Is_Not_Unlimited )
      state_names += "|" + string( "Is_Not_Unlimited" );
   if( state & c_modifier_PDF_List_Is_Custom )
      state_names += "|" + string( "PDF_List_Is_Custom" );
   if( state & c_modifier_PDF_List_Is_None )
      state_names += "|" + string( "PDF_List_Is_None" );
   if( state & c_modifier_Protect_Access )
      state_names += "|" + string( "Protect_Access" );
   if( state & c_modifier_Protect_Create )
      state_names += "|" + string( "Protect_Create" );
   if( state & c_modifier_Protect_Destroy )
      state_names += "|" + string( "Protect_Destroy" );
   if( state & c_modifier_Will_Sort_Rows_In_UI )
      state_names += "|" + string( "Will_Sort_Rows_In_UI" );

   return state_names.empty( ) ? state_names : state_names.substr( 1 );
}

const string& Meta_List::impl::execute( const string& cmd_and_args )
{
   execute_command( cmd_and_args );
   return retval;
}

void Meta_List::impl::clear_foreign_key( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( ( field == c_field_id_Access_Parent_Modifier ) || ( field == c_field_name_Access_Parent_Modifier ) )
      impl_Access_Parent_Modifier( "" );
   else if( ( field == c_field_id_Access_Permission ) || ( field == c_field_name_Access_Permission ) )
      impl_Access_Permission( "" );
   else if( ( field == c_field_id_Class ) || ( field == c_field_name_Class ) )
      impl_Class( "" );
   else if( ( field == c_field_id_Create_Parent_Modifier ) || ( field == c_field_name_Create_Parent_Modifier ) )
      impl_Create_Parent_Modifier( "" );
   else if( ( field == c_field_id_Create_Permission ) || ( field == c_field_name_Create_Permission ) )
      impl_Create_Permission( "" );
   else if( ( field == c_field_id_Destroy_Parent_Modifier ) || ( field == c_field_name_Destroy_Parent_Modifier ) )
      impl_Destroy_Parent_Modifier( "" );
   else if( ( field == c_field_id_Destroy_Permission ) || ( field == c_field_name_Destroy_Permission ) )
      impl_Destroy_Permission( "" );
   else if( ( field == c_field_id_Model ) || ( field == c_field_name_Model ) )
      impl_Model( "" );
   else if( ( field == c_field_id_Parent_Class ) || ( field == c_field_name_Parent_Class ) )
      impl_Parent_Class( "" );
   else if( ( field == c_field_id_Parent_Field ) || ( field == c_field_name_Parent_Field ) )
      impl_Parent_Field( "" );
   else if( ( field == c_field_id_Type ) || ( field == c_field_name_Type ) )
      impl_Type( "" );
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_List::impl::set_foreign_key_value( const string& field, const string& value )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for value: " + value );
   else if( ( field == c_field_id_Access_Parent_Modifier ) || ( field == c_field_name_Access_Parent_Modifier ) )
      v_Access_Parent_Modifier = value;
   else if( ( field == c_field_id_Access_Permission ) || ( field == c_field_name_Access_Permission ) )
      v_Access_Permission = value;
   else if( ( field == c_field_id_Class ) || ( field == c_field_name_Class ) )
      v_Class = value;
   else if( ( field == c_field_id_Create_Parent_Modifier ) || ( field == c_field_name_Create_Parent_Modifier ) )
      v_Create_Parent_Modifier = value;
   else if( ( field == c_field_id_Create_Permission ) || ( field == c_field_name_Create_Permission ) )
      v_Create_Permission = value;
   else if( ( field == c_field_id_Destroy_Parent_Modifier ) || ( field == c_field_name_Destroy_Parent_Modifier ) )
      v_Destroy_Parent_Modifier = value;
   else if( ( field == c_field_id_Destroy_Permission ) || ( field == c_field_name_Destroy_Permission ) )
      v_Destroy_Permission = value;
   else if( ( field == c_field_id_Model ) || ( field == c_field_name_Model ) )
      v_Model = value;
   else if( ( field == c_field_id_Parent_Class ) || ( field == c_field_name_Parent_Class ) )
      v_Parent_Class = value;
   else if( ( field == c_field_id_Parent_Field ) || ( field == c_field_name_Parent_Field ) )
      v_Parent_Field = value;
   else if( ( field == c_field_id_Type ) || ( field == c_field_name_Type ) )
      v_Type = value;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

const string& Meta_List::impl::get_foreign_key_value( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( ( field == c_field_id_Access_Parent_Modifier ) || ( field == c_field_name_Access_Parent_Modifier ) )
      return v_Access_Parent_Modifier;
   else if( ( field == c_field_id_Access_Permission ) || ( field == c_field_name_Access_Permission ) )
      return v_Access_Permission;
   else if( ( field == c_field_id_Class ) || ( field == c_field_name_Class ) )
      return v_Class;
   else if( ( field == c_field_id_Create_Parent_Modifier ) || ( field == c_field_name_Create_Parent_Modifier ) )
      return v_Create_Parent_Modifier;
   else if( ( field == c_field_id_Create_Permission ) || ( field == c_field_name_Create_Permission ) )
      return v_Create_Permission;
   else if( ( field == c_field_id_Destroy_Parent_Modifier ) || ( field == c_field_name_Destroy_Parent_Modifier ) )
      return v_Destroy_Parent_Modifier;
   else if( ( field == c_field_id_Destroy_Permission ) || ( field == c_field_name_Destroy_Permission ) )
      return v_Destroy_Permission;
   else if( ( field == c_field_id_Model ) || ( field == c_field_name_Model ) )
      return v_Model;
   else if( ( field == c_field_id_Parent_Class ) || ( field == c_field_name_Parent_Class ) )
      return v_Parent_Class;
   else if( ( field == c_field_id_Parent_Field ) || ( field == c_field_name_Parent_Field ) )
      return v_Parent_Field;
   else if( ( field == c_field_id_Type ) || ( field == c_field_name_Type ) )
      return v_Type;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_List::impl::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Access_Parent_Modifier, v_Access_Parent_Modifier ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Access_Permission, v_Access_Permission ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Class, v_Class ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Create_Parent_Modifier, v_Create_Parent_Modifier ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Create_Permission, v_Create_Permission ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Destroy_Parent_Modifier, v_Destroy_Parent_Modifier ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Destroy_Permission, v_Destroy_Permission ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Model, v_Model ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Parent_Class, v_Parent_Class ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Parent_Field, v_Parent_Field ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Type, v_Type ) );
}

void Meta_List::impl::add_extra_fixed_info( vector< pair< string, string > >& fixed_info ) const
{
   ( void )fixed_info;

   // [<start add_extra_fixed_info>]
   // [<finish add_extra_fixed_info>]
}

void Meta_List::impl::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   ( void )paging_info;

   // [<start add_extra_paging_info>]
   // [<finish add_extra_paging_info>]
}

void Meta_List::impl::clear( )
{
   v_Access_Restriction = g_default_Access_Restriction;
   v_Allow_Anonymous_Access = g_default_Allow_Anonymous_Access;
   v_Allow_Quick_Link = g_default_Allow_Quick_Link;
   v_Allow_Text_Search = g_default_Allow_Text_Search;
   v_Create_Only_If_Default_Other = g_default_Create_Only_If_Default_Other;
   v_Create_Restriction = g_default_Create_Restriction;
   v_Delete_Direction = g_default_Delete_Direction;
   v_Destroy_Only_If_Default_Other = g_default_Destroy_Only_If_Default_Other;
   v_Destroy_Restriction = g_default_Destroy_Restriction;
   v_Direction = g_default_Direction;
   v_Display_Only_If_Default_Other = g_default_Display_Only_If_Default_Other;
   v_Display_Row_Limit = g_default_Display_Row_Limit;
   v_Display_Security_Level = g_default_Display_Security_Level;
   v_Display_Sub_Totals = g_default_Display_Sub_Totals;
   v_Display_Totals = g_default_Display_Totals;
   v_File_Links_Always_As_Single = g_default_File_Links_Always_As_Single;
   v_Id = g_default_Id;
   v_Ignore_Implicit_Ordering = g_default_Ignore_Implicit_Ordering;
   v_Ignore_State_For_Display = g_default_Ignore_State_For_Display;
   v_Ignore_Unactionable_Records = g_default_Ignore_Unactionable_Records;
   v_Ignore_Uneditable_Parent = g_default_Ignore_Uneditable_Parent;
   v_Ignore_Username_Filter = g_default_Ignore_Username_Filter;
   v_Is_Admin = g_default_Is_Admin;
   v_Is_Child = g_default_Is_Child;
   v_Is_Home = g_default_Is_Home;
   v_Is_Not_Anonymous = g_default_Is_Not_Anonymous;
   v_Is_Variation = g_default_Is_Variation;
   v_Limit_Scroll_And_New = g_default_Limit_Scroll_And_New;
   v_Multiline_Truncate_For_Print = g_default_Multiline_Truncate_For_Print;
   v_Name = g_default_Name;
   v_Number_Multiple_Pages = g_default_Number_Multiple_Pages;
   v_PDF_Font_Type = g_default_PDF_Font_Type;
   v_PDF_List_Type = g_default_PDF_List_Type;
   v_Print_Restriction = g_default_Print_Restriction;
   v_Print_Without_Highlight = g_default_Print_Without_Highlight;
   v_Search_Option_Limit = g_default_Search_Option_Limit;
   v_Sort_Rows_In_UI = g_default_Sort_Rows_In_UI;
   v_Style = g_default_Style;
   v_Text_Match_Highlight = g_default_Text_Match_Highlight;
   v_Title = g_default_Title;
   v_Variation_Name = g_default_Variation_Name;

   v_Access_Parent_Modifier = string( );
   if( cp_Access_Parent_Modifier )
      p_obj->setup_foreign_key( *cp_Access_Parent_Modifier, v_Access_Parent_Modifier );

   v_Access_Permission = string( );
   if( cp_Access_Permission )
      p_obj->setup_foreign_key( *cp_Access_Permission, v_Access_Permission );

   v_Class = string( );
   if( cp_Class )
      p_obj->setup_foreign_key( *cp_Class, v_Class );

   v_Create_Parent_Modifier = string( );
   if( cp_Create_Parent_Modifier )
      p_obj->setup_foreign_key( *cp_Create_Parent_Modifier, v_Create_Parent_Modifier );

   v_Create_Permission = string( );
   if( cp_Create_Permission )
      p_obj->setup_foreign_key( *cp_Create_Permission, v_Create_Permission );

   v_Destroy_Parent_Modifier = string( );
   if( cp_Destroy_Parent_Modifier )
      p_obj->setup_foreign_key( *cp_Destroy_Parent_Modifier, v_Destroy_Parent_Modifier );

   v_Destroy_Permission = string( );
   if( cp_Destroy_Permission )
      p_obj->setup_foreign_key( *cp_Destroy_Permission, v_Destroy_Permission );

   v_Model = string( );
   if( cp_Model )
      p_obj->setup_foreign_key( *cp_Model, v_Model );

   v_Parent_Class = string( );
   if( cp_Parent_Class )
      p_obj->setup_foreign_key( *cp_Parent_Class, v_Parent_Class );

   v_Parent_Field = string( );
   if( cp_Parent_Field )
      p_obj->setup_foreign_key( *cp_Parent_Field, v_Parent_Field );

   v_Type = string( );
   if( cp_Type )
      p_obj->setup_foreign_key( *cp_Type, v_Type );
}

bool Meta_List::impl::value_will_be_provided( const string& field_name )
{
   ( void )field_name;

   // [(start parent_auto_int_inc)] 600418
   if( field_name == "Id" )
      return true;
   // [(finish parent_auto_int_inc)] 600418

   // [<start value_will_be_provided>]
   // [<finish value_will_be_provided>]

   return false;
}

void Meta_List::impl::validate(
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

   if( is_null( v_Title ) && !value_will_be_provided( c_field_name_Title ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Title,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_field_must_not_be_empty_field, get_module_string( c_field_display_name_Title ) ) ) ) );

   if( v_Class.empty( ) && !value_will_be_provided( c_field_name_Class ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Class,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_field_must_not_be_empty_field, get_module_string( c_field_display_name_Class ) ) ) ) );

   if( v_Model.empty( ) && !value_will_be_provided( c_field_name_Model ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Model,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_field_must_not_be_empty_field, get_module_string( c_field_display_name_Model ) ) ) ) );

   if( v_Type.empty( ) && !value_will_be_provided( c_field_name_Type ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Type,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_field_must_not_be_empty_field, get_module_string( c_field_display_name_Type ) ) ) ) );

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

   if( !is_null( v_Variation_Name )
    && ( v_Variation_Name != g_default_Variation_Name
    || !value_will_be_provided( c_field_name_Variation_Name ) )
    && !g_Variation_Name_domain.is_valid( v_Variation_Name, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Variation_Name,
       get_module_string( c_field_display_name_Variation_Name ) + " " + error_message ) );

   if( !g_list_restrict_enum.count( v_Access_Restriction ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Access_Restriction,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_field_has_invalid_value_field, get_module_string( c_field_display_name_Access_Restriction ) ) ) ) );

   if( !g_list_restrict_enum.count( v_Create_Restriction ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Create_Restriction,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_field_has_invalid_value_field, get_module_string( c_field_display_name_Create_Restriction ) ) ) ) );

   if( !g_list_direction_enum.count( v_Delete_Direction ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Delete_Direction,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_field_has_invalid_value_field, get_module_string( c_field_display_name_Delete_Direction ) ) ) ) );

   if( !g_list_restrict_enum.count( v_Destroy_Restriction ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Destroy_Restriction,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_field_has_invalid_value_field, get_module_string( c_field_display_name_Destroy_Restriction ) ) ) ) );

   if( !g_list_direction_enum.count( v_Direction ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Direction,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_field_has_invalid_value_field, get_module_string( c_field_display_name_Direction ) ) ) ) );

   if( !g_list_display_row_limit_enum.count( v_Display_Row_Limit ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Display_Row_Limit,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_field_has_invalid_value_field, get_module_string( c_field_display_name_Display_Row_Limit ) ) ) ) );

   if( !g_pdf_font_type_enum.count( v_PDF_Font_Type ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_PDF_Font_Type,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_field_has_invalid_value_field, get_module_string( c_field_display_name_PDF_Font_Type ) ) ) ) );

   if( !g_list_pdf_list_type_enum.count( v_PDF_List_Type ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_PDF_List_Type,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_field_has_invalid_value_field, get_module_string( c_field_display_name_PDF_List_Type ) ) ) ) );

   if( !g_list_print_restrict_enum.count( v_Print_Restriction ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Print_Restriction,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_field_has_invalid_value_field, get_module_string( c_field_display_name_Print_Restriction ) ) ) ) );

   if( !g_list_search_opt_limit_enum.count( v_Search_Option_Limit ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Search_Option_Limit,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_field_has_invalid_value_field, get_module_string( c_field_display_name_Search_Option_Limit ) ) ) ) );

   if( !g_list_style_enum.count( v_Style ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Style,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_field_has_invalid_value_field, get_module_string( c_field_display_name_Style ) ) ) ) );

   if( !g_list_text_match_highlight_enum.count( v_Text_Match_Highlight ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Text_Match_Highlight,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_field_has_invalid_value_field, get_module_string( c_field_display_name_Text_Match_Highlight ) ) ) ) );

   // [(start check_cond_non_null)] 600440
   if( get_obj( ).Is_Child( ) && is_null( get_obj( ).Parent_Field( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_Parent_Field, get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_field_must_not_be_empty_field, get_module_string( c_field_display_name_Parent_Field ) ) ) ) );
   // [(finish check_cond_non_null)] 600440

   // [<start validate>]
   // [<finish validate>]
}

void Meta_List::impl::validate_set_fields(
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

   if( !is_null( v_Variation_Name )
    && ( fields_set.count( c_field_id_Variation_Name ) || fields_set.count( c_field_name_Variation_Name ) )
    && !g_Variation_Name_domain.is_valid( v_Variation_Name, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Variation_Name,
       get_module_string( c_field_display_name_Variation_Name ) + " " + error_message ) );
}

void Meta_List::impl::after_fetch( )
{
   if( !get_obj( ).get_is_iterating( ) || get_obj( ).get_is_starting_iteration( ) )
      get_required_transients( );

   if( cp_Access_Parent_Modifier )
      p_obj->setup_foreign_key( *cp_Access_Parent_Modifier, v_Access_Parent_Modifier );

   if( cp_Access_Permission )
      p_obj->setup_foreign_key( *cp_Access_Permission, v_Access_Permission );

   if( cp_Class )
      p_obj->setup_foreign_key( *cp_Class, v_Class );

   if( cp_Create_Parent_Modifier )
      p_obj->setup_foreign_key( *cp_Create_Parent_Modifier, v_Create_Parent_Modifier );

   if( cp_Create_Permission )
      p_obj->setup_foreign_key( *cp_Create_Permission, v_Create_Permission );

   if( cp_Destroy_Parent_Modifier )
      p_obj->setup_foreign_key( *cp_Destroy_Parent_Modifier, v_Destroy_Parent_Modifier );

   if( cp_Destroy_Permission )
      p_obj->setup_foreign_key( *cp_Destroy_Permission, v_Destroy_Permission );

   if( cp_Model )
      p_obj->setup_foreign_key( *cp_Model, v_Model );

   if( cp_Parent_Class )
      p_obj->setup_foreign_key( *cp_Parent_Class, v_Parent_Class );

   if( cp_Parent_Field )
      p_obj->setup_foreign_key( *cp_Parent_Field, v_Parent_Field );

   if( cp_Type )
      p_obj->setup_foreign_key( *cp_Type, v_Type );

   post_init( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start field_from_search_replace)] 600420
   if( !get_obj( ).get_key( ).empty( )
    && !get_obj( ).get_is_for_peer( )
    && ( get_obj( ).needs_field_value( "Name" )
    || required_transients.count( "Name" ) ) )
   {
      string str( get_obj( ).Type( ).List_Name( ) );

      get_obj( ).Name( str );

      get_obj( ).add_search_replacement( "Name", "{class}", to_rep_string( get_obj( ).Class( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Name", "{pfield}", to_rep_string( get_obj( ).Parent_Field( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Name", "{vname}", to_rep_string( get_obj( ).Variation_Name( ) ) );

      get_obj( ).set_search_replace_separator( "Name", '_' );
   }
   // [(finish field_from_search_replace)] 600420

   // [<start after_fetch>]
//nyi
   if( get_obj( ).Class( ).Access_Restriction( ) != 0 )
      get_obj( ).Access_Restriction( get_obj( ).Class( ).Access_Restriction( ) );

   if( get_obj( ).Class( ).Create_Restriction( ) != 0 )
      get_obj( ).Create_Restriction( get_obj( ).Class( ).Create_Restriction( ) );

   if( get_obj( ).Class( ).Destroy_Restriction( ) != 0 )
      get_obj( ).Destroy_Restriction( get_obj( ).Class( ).Destroy_Restriction( ) );

   if( !is_null( get_obj( ).Class( ).Access_Permission( ) ) )
      get_obj( ).Access_Permission( get_obj( ).Class( ).Access_Permission( ) );

   if( !is_null( get_obj( ).Class( ).Create_Permission( ) ) )
      get_obj( ).Create_Permission( get_obj( ).Class( ).Create_Permission( ) );

   if( !is_null( get_obj( ).Class( ).Destroy_Permission( ) ) )
      get_obj( ).Destroy_Permission( get_obj( ).Class( ).Destroy_Permission( ) );
   // [<finish after_fetch>]
}

void Meta_List::impl::completed_fetch( bool skip_set_original )
{
   if( !skip_set_original && !get_obj( ).get_key( ).empty( ) )
      get_obj( ).set_new_original_values( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start completed_fetch>]
   // [<finish completed_fetch>]
}

void Meta_List::impl::at_create( )
{
   // [(start fk_default)] 600400
   if( is_null( get_obj( ).Type( ) ) )
      get_obj( ).Type( "normal" );
   // [(finish fk_default)] 600400

   // [<start at_create>]
   // [<finish at_create>]
}

void Meta_List::impl::post_init( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start post_init>]
   // [<finish post_init>]
}

void Meta_List::impl::to_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   if( !get_obj( ).get_is_preparing( ) )
      post_init( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start field_from_other_field)] 600415
   if( get_obj( ).Is_Variation( ) == true )
      get_obj( ).Variation_Name( get_obj( ).Title( ) );
   else
      get_obj( ).Variation_Name( "" );
   // [(finish field_from_other_field)] 600415

   // [(start field_from_other_field)] 600425
   get_obj( ).Is_Child( get_obj( ).Type( ).Is_Child( ) );
   // [(finish field_from_other_field)] 600425

   // [(start field_from_other_field)] 600426
   get_obj( ).Is_Home( get_obj( ).Type( ).Is_Home( ) );
   // [(finish field_from_other_field)] 600426

   // [(start field_from_other_field)] 600426a
   get_obj( ).Is_Not_Anonymous( get_obj( ).Type( ).Is_Not_Anonymous( ) );
   // [(finish field_from_other_field)] 600426a

   // [(start field_from_other_field)] 600426b
   get_obj( ).Is_Admin( get_obj( ).Type( ).Is_Admin( ) );
   // [(finish field_from_other_field)] 600426b

   // [(start field_from_other_field)] 600429
   get_obj( ).Parent_Class( get_obj( ).Parent_Field( ).Parent_Class( ) );
   // [(finish field_from_other_field)] 600429

   // [(start default_to_field)] 600429a
   if( ( is_create || get_obj( ).get_key( ).empty( ) )
    && get_obj( ).get_clone_key( ).empty( )
    && ( get_obj( ).Allow_Anonymous_Access( ) == g_default_Allow_Anonymous_Access )
    && get_obj( ).Type( ).Is_Not_Anonymous( ) == true )
      get_obj( ).Allow_Anonymous_Access( get_obj( ).Type( ).Dummy_0( ) );
   // [(finish default_to_field)] 600429a

   // [<start to_store>]
   // [<finish to_store>]
}

void Meta_List::impl::for_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start parent_auto_int_inc)] 600418
   if( is_create && is_null( get_obj( ).Id( ) ) )
   {
      if( is_null( get_obj( ).Model( ) ) )
         throw runtime_error( "unexpected empty Model" );

      get_obj( ).Model( ).op_update( get_obj( ).Model( ), FIELD_NAME( Meta, Model, Next_List_Id ) );

      get_obj( ).Id( get_obj( ).Model( ).Next_List_Id( ) );

      get_obj( ).Model( ).Next_List_Id( auto_int_increment( get_obj( ).Model( ).Next_List_Id( ) ) );
      get_obj( ).Model( ).op_apply( );
   }
   // [(finish parent_auto_int_inc)] 600418

   // [<start for_store>]
//nyi
   if( get_obj( ).Is_Admin( ) )
      get_obj( ).Access_Restriction( 2 ); // i.e. admin_only
   // [<finish for_store>]
}

void Meta_List::impl::after_store( bool is_create, bool is_internal )
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

         if( cp_parent->child_List( ).iterate_forwards( ) )
         {
            set< string > names;
            do
            {
               if( names.count( cp_parent->child_List( ).Name( ) ) )
                  throw runtime_error( "This List has already been created." );

               names.insert( cp_parent->child_List( ).Name( ) );

            } while( cp_parent->child_List( ).iterate_next( ) );
         }
      }
   }
   // [<finish after_store>]
}

bool Meta_List::impl::can_destroy( bool is_internal )
{
   uint64_t state = p_obj->get_state( );

   bool retval = ( is_internal || !( state & c_state_undeletable ) );

   // [<start can_destroy>]
   // [<finish can_destroy>]

   return retval;
}

void Meta_List::impl::for_destroy( bool is_internal )
{
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start for_destroy>]
   // [<finish for_destroy>]
}

void Meta_List::impl::after_destroy( bool is_internal )
{
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_destroy>]
   // [<finish after_destroy>]
}

void Meta_List::impl::set_default_values( )
{
   clear( );
}

bool Meta_List::impl::is_filtered( ) const
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start is_filtered>]
   // [<finish is_filtered>]

   return false;
}

void Meta_List::impl::get_required_transients( ) const
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

Meta_List::Meta_List( )
{
   set_version( c_version );

   p_impl = new impl( *this );
}

Meta_List::~Meta_List( )
{
   cleanup( );
   delete p_impl;
}

int Meta_List::Access_Restriction( ) const
{
   return p_impl->impl_Access_Restriction( );
}

void Meta_List::Access_Restriction( int Access_Restriction )
{
   p_impl->impl_Access_Restriction( Access_Restriction );
}

bool Meta_List::Allow_Anonymous_Access( ) const
{
   return p_impl->impl_Allow_Anonymous_Access( );
}

void Meta_List::Allow_Anonymous_Access( bool Allow_Anonymous_Access )
{
   p_impl->impl_Allow_Anonymous_Access( Allow_Anonymous_Access );
}

bool Meta_List::Allow_Quick_Link( ) const
{
   return p_impl->impl_Allow_Quick_Link( );
}

void Meta_List::Allow_Quick_Link( bool Allow_Quick_Link )
{
   p_impl->impl_Allow_Quick_Link( Allow_Quick_Link );
}

bool Meta_List::Allow_Text_Search( ) const
{
   return p_impl->impl_Allow_Text_Search( );
}

void Meta_List::Allow_Text_Search( bool Allow_Text_Search )
{
   p_impl->impl_Allow_Text_Search( Allow_Text_Search );
}

bool Meta_List::Create_Only_If_Default_Other( ) const
{
   return p_impl->impl_Create_Only_If_Default_Other( );
}

void Meta_List::Create_Only_If_Default_Other( bool Create_Only_If_Default_Other )
{
   p_impl->impl_Create_Only_If_Default_Other( Create_Only_If_Default_Other );
}

int Meta_List::Create_Restriction( ) const
{
   return p_impl->impl_Create_Restriction( );
}

void Meta_List::Create_Restriction( int Create_Restriction )
{
   p_impl->impl_Create_Restriction( Create_Restriction );
}

int Meta_List::Delete_Direction( ) const
{
   return p_impl->impl_Delete_Direction( );
}

void Meta_List::Delete_Direction( int Delete_Direction )
{
   p_impl->impl_Delete_Direction( Delete_Direction );
}

bool Meta_List::Destroy_Only_If_Default_Other( ) const
{
   return p_impl->impl_Destroy_Only_If_Default_Other( );
}

void Meta_List::Destroy_Only_If_Default_Other( bool Destroy_Only_If_Default_Other )
{
   p_impl->impl_Destroy_Only_If_Default_Other( Destroy_Only_If_Default_Other );
}

int Meta_List::Destroy_Restriction( ) const
{
   return p_impl->impl_Destroy_Restriction( );
}

void Meta_List::Destroy_Restriction( int Destroy_Restriction )
{
   p_impl->impl_Destroy_Restriction( Destroy_Restriction );
}

int Meta_List::Direction( ) const
{
   return p_impl->impl_Direction( );
}

void Meta_List::Direction( int Direction )
{
   p_impl->impl_Direction( Direction );
}

bool Meta_List::Display_Only_If_Default_Other( ) const
{
   return p_impl->impl_Display_Only_If_Default_Other( );
}

void Meta_List::Display_Only_If_Default_Other( bool Display_Only_If_Default_Other )
{
   p_impl->impl_Display_Only_If_Default_Other( Display_Only_If_Default_Other );
}

int Meta_List::Display_Row_Limit( ) const
{
   return p_impl->impl_Display_Row_Limit( );
}

void Meta_List::Display_Row_Limit( int Display_Row_Limit )
{
   p_impl->impl_Display_Row_Limit( Display_Row_Limit );
}

bool Meta_List::Display_Security_Level( ) const
{
   return p_impl->impl_Display_Security_Level( );
}

void Meta_List::Display_Security_Level( bool Display_Security_Level )
{
   p_impl->impl_Display_Security_Level( Display_Security_Level );
}

bool Meta_List::Display_Sub_Totals( ) const
{
   return p_impl->impl_Display_Sub_Totals( );
}

void Meta_List::Display_Sub_Totals( bool Display_Sub_Totals )
{
   p_impl->impl_Display_Sub_Totals( Display_Sub_Totals );
}

bool Meta_List::Display_Totals( ) const
{
   return p_impl->impl_Display_Totals( );
}

void Meta_List::Display_Totals( bool Display_Totals )
{
   p_impl->impl_Display_Totals( Display_Totals );
}

bool Meta_List::File_Links_Always_As_Single( ) const
{
   return p_impl->impl_File_Links_Always_As_Single( );
}

void Meta_List::File_Links_Always_As_Single( bool File_Links_Always_As_Single )
{
   p_impl->impl_File_Links_Always_As_Single( File_Links_Always_As_Single );
}

const string& Meta_List::Id( ) const
{
   return p_impl->impl_Id( );
}

void Meta_List::Id( const string& Id )
{
   p_impl->impl_Id( Id );
}

bool Meta_List::Ignore_Implicit_Ordering( ) const
{
   return p_impl->impl_Ignore_Implicit_Ordering( );
}

void Meta_List::Ignore_Implicit_Ordering( bool Ignore_Implicit_Ordering )
{
   p_impl->impl_Ignore_Implicit_Ordering( Ignore_Implicit_Ordering );
}

bool Meta_List::Ignore_State_For_Display( ) const
{
   return p_impl->impl_Ignore_State_For_Display( );
}

void Meta_List::Ignore_State_For_Display( bool Ignore_State_For_Display )
{
   p_impl->impl_Ignore_State_For_Display( Ignore_State_For_Display );
}

bool Meta_List::Ignore_Unactionable_Records( ) const
{
   return p_impl->impl_Ignore_Unactionable_Records( );
}

void Meta_List::Ignore_Unactionable_Records( bool Ignore_Unactionable_Records )
{
   p_impl->impl_Ignore_Unactionable_Records( Ignore_Unactionable_Records );
}

bool Meta_List::Ignore_Uneditable_Parent( ) const
{
   return p_impl->impl_Ignore_Uneditable_Parent( );
}

void Meta_List::Ignore_Uneditable_Parent( bool Ignore_Uneditable_Parent )
{
   p_impl->impl_Ignore_Uneditable_Parent( Ignore_Uneditable_Parent );
}

bool Meta_List::Ignore_Username_Filter( ) const
{
   return p_impl->impl_Ignore_Username_Filter( );
}

void Meta_List::Ignore_Username_Filter( bool Ignore_Username_Filter )
{
   p_impl->impl_Ignore_Username_Filter( Ignore_Username_Filter );
}

bool Meta_List::Is_Admin( ) const
{
   return p_impl->impl_Is_Admin( );
}

void Meta_List::Is_Admin( bool Is_Admin )
{
   p_impl->impl_Is_Admin( Is_Admin );
}

bool Meta_List::Is_Child( ) const
{
   return p_impl->impl_Is_Child( );
}

void Meta_List::Is_Child( bool Is_Child )
{
   p_impl->impl_Is_Child( Is_Child );
}

bool Meta_List::Is_Home( ) const
{
   return p_impl->impl_Is_Home( );
}

void Meta_List::Is_Home( bool Is_Home )
{
   p_impl->impl_Is_Home( Is_Home );
}

bool Meta_List::Is_Not_Anonymous( ) const
{
   return p_impl->impl_Is_Not_Anonymous( );
}

void Meta_List::Is_Not_Anonymous( bool Is_Not_Anonymous )
{
   p_impl->impl_Is_Not_Anonymous( Is_Not_Anonymous );
}

bool Meta_List::Is_Variation( ) const
{
   return p_impl->impl_Is_Variation( );
}

void Meta_List::Is_Variation( bool Is_Variation )
{
   p_impl->impl_Is_Variation( Is_Variation );
}

bool Meta_List::Limit_Scroll_And_New( ) const
{
   return p_impl->impl_Limit_Scroll_And_New( );
}

void Meta_List::Limit_Scroll_And_New( bool Limit_Scroll_And_New )
{
   p_impl->impl_Limit_Scroll_And_New( Limit_Scroll_And_New );
}

bool Meta_List::Multiline_Truncate_For_Print( ) const
{
   return p_impl->impl_Multiline_Truncate_For_Print( );
}

void Meta_List::Multiline_Truncate_For_Print( bool Multiline_Truncate_For_Print )
{
   p_impl->impl_Multiline_Truncate_For_Print( Multiline_Truncate_For_Print );
}

const string& Meta_List::Name( ) const
{
   return p_impl->impl_Name( );
}

void Meta_List::Name( const string& Name )
{
   p_impl->impl_Name( Name );
}

bool Meta_List::Number_Multiple_Pages( ) const
{
   return p_impl->impl_Number_Multiple_Pages( );
}

void Meta_List::Number_Multiple_Pages( bool Number_Multiple_Pages )
{
   p_impl->impl_Number_Multiple_Pages( Number_Multiple_Pages );
}

int Meta_List::PDF_Font_Type( ) const
{
   return p_impl->impl_PDF_Font_Type( );
}

void Meta_List::PDF_Font_Type( int PDF_Font_Type )
{
   p_impl->impl_PDF_Font_Type( PDF_Font_Type );
}

int Meta_List::PDF_List_Type( ) const
{
   return p_impl->impl_PDF_List_Type( );
}

void Meta_List::PDF_List_Type( int PDF_List_Type )
{
   p_impl->impl_PDF_List_Type( PDF_List_Type );
}

int Meta_List::Print_Restriction( ) const
{
   return p_impl->impl_Print_Restriction( );
}

void Meta_List::Print_Restriction( int Print_Restriction )
{
   p_impl->impl_Print_Restriction( Print_Restriction );
}

bool Meta_List::Print_Without_Highlight( ) const
{
   return p_impl->impl_Print_Without_Highlight( );
}

void Meta_List::Print_Without_Highlight( bool Print_Without_Highlight )
{
   p_impl->impl_Print_Without_Highlight( Print_Without_Highlight );
}

int Meta_List::Search_Option_Limit( ) const
{
   return p_impl->impl_Search_Option_Limit( );
}

void Meta_List::Search_Option_Limit( int Search_Option_Limit )
{
   p_impl->impl_Search_Option_Limit( Search_Option_Limit );
}

bool Meta_List::Sort_Rows_In_UI( ) const
{
   return p_impl->impl_Sort_Rows_In_UI( );
}

void Meta_List::Sort_Rows_In_UI( bool Sort_Rows_In_UI )
{
   p_impl->impl_Sort_Rows_In_UI( Sort_Rows_In_UI );
}

int Meta_List::Style( ) const
{
   return p_impl->impl_Style( );
}

void Meta_List::Style( int Style )
{
   p_impl->impl_Style( Style );
}

int Meta_List::Text_Match_Highlight( ) const
{
   return p_impl->impl_Text_Match_Highlight( );
}

void Meta_List::Text_Match_Highlight( int Text_Match_Highlight )
{
   p_impl->impl_Text_Match_Highlight( Text_Match_Highlight );
}

const string& Meta_List::Title( ) const
{
   return p_impl->impl_Title( );
}

void Meta_List::Title( const string& Title )
{
   p_impl->impl_Title( Title );
}

const string& Meta_List::Variation_Name( ) const
{
   return p_impl->impl_Variation_Name( );
}

void Meta_List::Variation_Name( const string& Variation_Name )
{
   p_impl->impl_Variation_Name( Variation_Name );
}

Meta_Modifier& Meta_List::Access_Parent_Modifier( )
{
   return p_impl->impl_Access_Parent_Modifier( );
}

const Meta_Modifier& Meta_List::Access_Parent_Modifier( ) const
{
   return p_impl->impl_Access_Parent_Modifier( );
}

void Meta_List::Access_Parent_Modifier( const string& key )
{
   p_impl->impl_Access_Parent_Modifier( key );
}

Meta_Permission& Meta_List::Access_Permission( )
{
   return p_impl->impl_Access_Permission( );
}

const Meta_Permission& Meta_List::Access_Permission( ) const
{
   return p_impl->impl_Access_Permission( );
}

void Meta_List::Access_Permission( const string& key )
{
   p_impl->impl_Access_Permission( key );
}

Meta_Class& Meta_List::Class( )
{
   return p_impl->impl_Class( );
}

const Meta_Class& Meta_List::Class( ) const
{
   return p_impl->impl_Class( );
}

void Meta_List::Class( const string& key )
{
   p_impl->impl_Class( key );
}

Meta_Modifier& Meta_List::Create_Parent_Modifier( )
{
   return p_impl->impl_Create_Parent_Modifier( );
}

const Meta_Modifier& Meta_List::Create_Parent_Modifier( ) const
{
   return p_impl->impl_Create_Parent_Modifier( );
}

void Meta_List::Create_Parent_Modifier( const string& key )
{
   p_impl->impl_Create_Parent_Modifier( key );
}

Meta_Permission& Meta_List::Create_Permission( )
{
   return p_impl->impl_Create_Permission( );
}

const Meta_Permission& Meta_List::Create_Permission( ) const
{
   return p_impl->impl_Create_Permission( );
}

void Meta_List::Create_Permission( const string& key )
{
   p_impl->impl_Create_Permission( key );
}

Meta_Modifier& Meta_List::Destroy_Parent_Modifier( )
{
   return p_impl->impl_Destroy_Parent_Modifier( );
}

const Meta_Modifier& Meta_List::Destroy_Parent_Modifier( ) const
{
   return p_impl->impl_Destroy_Parent_Modifier( );
}

void Meta_List::Destroy_Parent_Modifier( const string& key )
{
   p_impl->impl_Destroy_Parent_Modifier( key );
}

Meta_Permission& Meta_List::Destroy_Permission( )
{
   return p_impl->impl_Destroy_Permission( );
}

const Meta_Permission& Meta_List::Destroy_Permission( ) const
{
   return p_impl->impl_Destroy_Permission( );
}

void Meta_List::Destroy_Permission( const string& key )
{
   p_impl->impl_Destroy_Permission( key );
}

Meta_Model& Meta_List::Model( )
{
   return p_impl->impl_Model( );
}

const Meta_Model& Meta_List::Model( ) const
{
   return p_impl->impl_Model( );
}

void Meta_List::Model( const string& key )
{
   p_impl->impl_Model( key );
}

Meta_Class& Meta_List::Parent_Class( )
{
   return p_impl->impl_Parent_Class( );
}

const Meta_Class& Meta_List::Parent_Class( ) const
{
   return p_impl->impl_Parent_Class( );
}

void Meta_List::Parent_Class( const string& key )
{
   p_impl->impl_Parent_Class( key );
}

Meta_Field& Meta_List::Parent_Field( )
{
   return p_impl->impl_Parent_Field( );
}

const Meta_Field& Meta_List::Parent_Field( ) const
{
   return p_impl->impl_Parent_Field( );
}

void Meta_List::Parent_Field( const string& key )
{
   p_impl->impl_Parent_Field( key );
}

Meta_List_Type& Meta_List::Type( )
{
   return p_impl->impl_Type( );
}

const Meta_List_Type& Meta_List::Type( ) const
{
   return p_impl->impl_Type( );
}

void Meta_List::Type( const string& key )
{
   p_impl->impl_Type( key );
}

Meta_Class& Meta_List::child_Class_Created( )
{
   return p_impl->impl_child_Class_Created( );
}

const Meta_Class& Meta_List::child_Class_Created( ) const
{
   return p_impl->impl_child_Class_Created( );
}

Meta_List_Field& Meta_List::child_List_Field( )
{
   return p_impl->impl_child_List_Field( );
}

const Meta_List_Field& Meta_List::child_List_Field( ) const
{
   return p_impl->impl_child_List_Field( );
}

Meta_Package_Option& Meta_List::child_Package_Option( )
{
   return p_impl->impl_child_Package_Option( );
}

const Meta_Package_Option& Meta_List::child_Package_Option( ) const
{
   return p_impl->impl_child_Package_Option( );
}

void Meta_List::Generate_PDF_List( int Variation_Num )
{
   p_impl->impl_Generate_PDF_List( Variation_Num );
}

string Meta_List::get_field_value( int field ) const
{
   return p_impl->get_field_value( field );
}

void Meta_List::set_field_value( int field, const string& value )
{
   p_impl->set_field_value( field, value );
}

void Meta_List::set_field_default( int field )
{
   return set_field_default( ( field_id )( field + 1 ) );
}

void Meta_List::set_field_default( field_id id )
{
   p_impl->set_field_default( ( int )id - 1 );
}

void Meta_List::set_field_default( const string& field )
{
   p_impl->set_field_default( get_field_num( field ) );
}

bool Meta_List::is_field_default( int field ) const
{
   return is_field_default( ( field_id )( field + 1 ) );
}

bool Meta_List::is_field_default( field_id id ) const
{
   return p_impl->is_field_default( ( int )id - 1 );
}

bool Meta_List::is_field_default( const string& field ) const
{
   return p_impl->is_field_default( get_field_num( field ) );
}

bool Meta_List::is_field_encrypted( int field ) const
{
   return static_is_field_encrypted( ( field_id )( field + 1 ) );
}

bool Meta_List::is_field_transient( int field ) const
{
   return static_is_field_transient( ( field_id )( field + 1 ) );
}

string Meta_List::get_field_id( int field ) const
{
   return static_get_field_id( ( field_id )( field + 1 ) );
}

string Meta_List::get_field_name( int field ) const
{
   return static_get_field_name( ( field_id )( field + 1 ) );
}

int Meta_List::get_field_num( const string& field ) const
{
   int rc = static_get_field_num( field );

   if( rc < 0 )
      throw runtime_error( "unknown field name/id '" + field + "' in get_field_num( )" );

   return rc;
}

bool Meta_List::has_field_changed( const string& field ) const
{
   return class_base::has_field_changed( get_field_num( field ) );
}

uint64_t Meta_List::get_state( ) const
{
   uint64_t state = 0;

   state |= p_impl->get_state( );

   return state;
}

const string& Meta_List::execute( const string& cmd_and_args )
{
   return p_impl->execute( cmd_and_args );
}

void Meta_List::clear( )
{
   p_impl->clear( );
}

void Meta_List::validate( uint64_t state, bool is_internal )
{
   p_impl->validate( state, is_internal, &validation_errors );
}

void Meta_List::validate_set_fields( set< string >& fields_set )
{
   p_impl->validate_set_fields( fields_set, &validation_errors );
}

void Meta_List::after_fetch( )
{
   p_impl->after_fetch( );
}

void Meta_List::completed_fetch( bool skip_set_original )
{
   p_impl->completed_fetch( skip_set_original );
}

void Meta_List::at_create( )
{
   p_impl->at_create( );
}

void Meta_List::post_init( )
{
   p_impl->post_init( );
}

void Meta_List::to_store( bool is_create, bool is_internal )
{
   p_impl->to_store( is_create, is_internal );
}

void Meta_List::for_store( bool is_create, bool is_internal )
{
   p_impl->for_store( is_create, is_internal );
}

void Meta_List::after_store( bool is_create, bool is_internal )
{
   p_impl->after_store( is_create, is_internal );
}

bool Meta_List::can_destroy( bool is_internal )
{
   return p_impl->can_destroy( is_internal );
}

void Meta_List::for_destroy( bool is_internal )
{
   p_impl->for_destroy( is_internal );
}

void Meta_List::after_destroy( bool is_internal )
{
   p_impl->after_destroy( is_internal );
}

void Meta_List::set_default_values( )
{
   p_impl->set_default_values( );
}

bool Meta_List::is_filtered( ) const
{
   return p_impl->is_filtered( );
}

const char* Meta_List::get_field_id(
 const string& name, bool* p_sql_numeric, string* p_type_name ) const
{
   const char* p_id( 0 );

   if( name.empty( ) )
      throw runtime_error( "unexpected empty field name for get_field_id" );
   else if( name == c_field_name_Access_Parent_Modifier )
   {
      p_id = c_field_id_Access_Parent_Modifier;

      if( p_type_name )
         *p_type_name = "Meta_Modifier";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
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
   else if( name == c_field_name_Allow_Anonymous_Access )
   {
      p_id = c_field_id_Allow_Anonymous_Access;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Allow_Quick_Link )
   {
      p_id = c_field_id_Allow_Quick_Link;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Allow_Text_Search )
   {
      p_id = c_field_id_Allow_Text_Search;

      if( p_type_name )
         *p_type_name = "bool";

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
   else if( name == c_field_name_Create_Only_If_Default_Other )
   {
      p_id = c_field_id_Create_Only_If_Default_Other;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Create_Parent_Modifier )
   {
      p_id = c_field_id_Create_Parent_Modifier;

      if( p_type_name )
         *p_type_name = "Meta_Modifier";

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
   else if( name == c_field_name_Delete_Direction )
   {
      p_id = c_field_id_Delete_Direction;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Destroy_Only_If_Default_Other )
   {
      p_id = c_field_id_Destroy_Only_If_Default_Other;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Destroy_Parent_Modifier )
   {
      p_id = c_field_id_Destroy_Parent_Modifier;

      if( p_type_name )
         *p_type_name = "Meta_Modifier";

      if( p_sql_numeric )
         *p_sql_numeric = false;
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
   else if( name == c_field_name_Direction )
   {
      p_id = c_field_id_Direction;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Display_Only_If_Default_Other )
   {
      p_id = c_field_id_Display_Only_If_Default_Other;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Display_Row_Limit )
   {
      p_id = c_field_id_Display_Row_Limit;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Display_Security_Level )
   {
      p_id = c_field_id_Display_Security_Level;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Display_Sub_Totals )
   {
      p_id = c_field_id_Display_Sub_Totals;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Display_Totals )
   {
      p_id = c_field_id_Display_Totals;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
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
   else if( name == c_field_name_Ignore_Implicit_Ordering )
   {
      p_id = c_field_id_Ignore_Implicit_Ordering;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Ignore_State_For_Display )
   {
      p_id = c_field_id_Ignore_State_For_Display;

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
   else if( name == c_field_name_Ignore_Uneditable_Parent )
   {
      p_id = c_field_id_Ignore_Uneditable_Parent;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Ignore_Username_Filter )
   {
      p_id = c_field_id_Ignore_Username_Filter;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Is_Admin )
   {
      p_id = c_field_id_Is_Admin;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Is_Child )
   {
      p_id = c_field_id_Is_Child;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Is_Home )
   {
      p_id = c_field_id_Is_Home;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Is_Not_Anonymous )
   {
      p_id = c_field_id_Is_Not_Anonymous;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Is_Variation )
   {
      p_id = c_field_id_Is_Variation;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Limit_Scroll_And_New )
   {
      p_id = c_field_id_Limit_Scroll_And_New;

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
   else if( name == c_field_name_Multiline_Truncate_For_Print )
   {
      p_id = c_field_id_Multiline_Truncate_For_Print;

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
   else if( name == c_field_name_Number_Multiple_Pages )
   {
      p_id = c_field_id_Number_Multiple_Pages;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_PDF_Font_Type )
   {
      p_id = c_field_id_PDF_Font_Type;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_PDF_List_Type )
   {
      p_id = c_field_id_PDF_List_Type;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Parent_Class )
   {
      p_id = c_field_id_Parent_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Parent_Field )
   {
      p_id = c_field_id_Parent_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Print_Restriction )
   {
      p_id = c_field_id_Print_Restriction;

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
   else if( name == c_field_name_Search_Option_Limit )
   {
      p_id = c_field_id_Search_Option_Limit;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Sort_Rows_In_UI )
   {
      p_id = c_field_id_Sort_Rows_In_UI;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Style )
   {
      p_id = c_field_id_Style;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Text_Match_Highlight )
   {
      p_id = c_field_id_Text_Match_Highlight;

      if( p_type_name )
         *p_type_name = "int";

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
         *p_type_name = "Meta_List_Type";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Variation_Name )
   {
      p_id = c_field_id_Variation_Name;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }

   return p_id;
}

const char* Meta_List::get_field_name(
 const string& id, bool* p_sql_numeric, string* p_type_name ) const
{
   const char* p_name( 0 );

   if( id.empty( ) )
      throw runtime_error( "unexpected empty field id for get_field_name" );
   else if( id == c_field_id_Access_Parent_Modifier )
   {
      p_name = c_field_name_Access_Parent_Modifier;

      if( p_type_name )
         *p_type_name = "Meta_Modifier";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
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
   else if( id == c_field_id_Allow_Anonymous_Access )
   {
      p_name = c_field_name_Allow_Anonymous_Access;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Allow_Quick_Link )
   {
      p_name = c_field_name_Allow_Quick_Link;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Allow_Text_Search )
   {
      p_name = c_field_name_Allow_Text_Search;

      if( p_type_name )
         *p_type_name = "bool";

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
   else if( id == c_field_id_Create_Only_If_Default_Other )
   {
      p_name = c_field_name_Create_Only_If_Default_Other;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Create_Parent_Modifier )
   {
      p_name = c_field_name_Create_Parent_Modifier;

      if( p_type_name )
         *p_type_name = "Meta_Modifier";

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
   else if( id == c_field_id_Delete_Direction )
   {
      p_name = c_field_name_Delete_Direction;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Destroy_Only_If_Default_Other )
   {
      p_name = c_field_name_Destroy_Only_If_Default_Other;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Destroy_Parent_Modifier )
   {
      p_name = c_field_name_Destroy_Parent_Modifier;

      if( p_type_name )
         *p_type_name = "Meta_Modifier";

      if( p_sql_numeric )
         *p_sql_numeric = false;
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
   else if( id == c_field_id_Direction )
   {
      p_name = c_field_name_Direction;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Display_Only_If_Default_Other )
   {
      p_name = c_field_name_Display_Only_If_Default_Other;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Display_Row_Limit )
   {
      p_name = c_field_name_Display_Row_Limit;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Display_Security_Level )
   {
      p_name = c_field_name_Display_Security_Level;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Display_Sub_Totals )
   {
      p_name = c_field_name_Display_Sub_Totals;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Display_Totals )
   {
      p_name = c_field_name_Display_Totals;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
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
   else if( id == c_field_id_Ignore_Implicit_Ordering )
   {
      p_name = c_field_name_Ignore_Implicit_Ordering;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Ignore_State_For_Display )
   {
      p_name = c_field_name_Ignore_State_For_Display;

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
   else if( id == c_field_id_Ignore_Uneditable_Parent )
   {
      p_name = c_field_name_Ignore_Uneditable_Parent;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Ignore_Username_Filter )
   {
      p_name = c_field_name_Ignore_Username_Filter;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Is_Admin )
   {
      p_name = c_field_name_Is_Admin;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Is_Child )
   {
      p_name = c_field_name_Is_Child;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Is_Home )
   {
      p_name = c_field_name_Is_Home;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Is_Not_Anonymous )
   {
      p_name = c_field_name_Is_Not_Anonymous;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Is_Variation )
   {
      p_name = c_field_name_Is_Variation;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Limit_Scroll_And_New )
   {
      p_name = c_field_name_Limit_Scroll_And_New;

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
   else if( id == c_field_id_Multiline_Truncate_For_Print )
   {
      p_name = c_field_name_Multiline_Truncate_For_Print;

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
   else if( id == c_field_id_Number_Multiple_Pages )
   {
      p_name = c_field_name_Number_Multiple_Pages;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_PDF_Font_Type )
   {
      p_name = c_field_name_PDF_Font_Type;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_PDF_List_Type )
   {
      p_name = c_field_name_PDF_List_Type;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Parent_Class )
   {
      p_name = c_field_name_Parent_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Parent_Field )
   {
      p_name = c_field_name_Parent_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Print_Restriction )
   {
      p_name = c_field_name_Print_Restriction;

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
   else if( id == c_field_id_Search_Option_Limit )
   {
      p_name = c_field_name_Search_Option_Limit;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Sort_Rows_In_UI )
   {
      p_name = c_field_name_Sort_Rows_In_UI;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Style )
   {
      p_name = c_field_name_Style;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Text_Match_Highlight )
   {
      p_name = c_field_name_Text_Match_Highlight;

      if( p_type_name )
         *p_type_name = "int";

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
         *p_type_name = "Meta_List_Type";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Variation_Name )
   {
      p_name = c_field_name_Variation_Name;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }

   return p_name;
}

string& Meta_List::get_group_field_name( ) const
{
   return g_group_field_name;
}

string& Meta_List::get_level_field_name( ) const
{
   return g_level_field_name;
}

string& Meta_List::get_order_field_name( ) const
{
   return g_order_field_name;
}

string& Meta_List::get_owner_field_name( ) const
{
   return g_owner_field_name;
}

bool Meta_List::is_file_field( const string& id_or_name ) const
{
   return g_file_field_ids.count( id_or_name ) || g_file_field_names.count( id_or_name );
}

void Meta_List::get_file_field_names( vector< string >& file_field_names ) const
{
   for( set< string >::const_iterator ci = g_file_field_names.begin( ); ci != g_file_field_names.end( ); ++ci )
      file_field_names.push_back( *ci );
}

string Meta_List::get_field_uom_symbol( const string& id_or_name ) const
{
   string uom_symbol;

   string name;
   pair< string, string > next;

   if( id_or_name.empty( ) )
      throw runtime_error( "unexpected empty field id_or_name for get_field_uom_symbol" );
   if( ( id_or_name == c_field_id_Access_Parent_Modifier ) || ( id_or_name == c_field_name_Access_Parent_Modifier ) )
   {
      name = string( c_field_display_name_Access_Parent_Modifier );
      get_module_string( c_field_display_name_Access_Parent_Modifier, &next );
   }
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
   if( ( id_or_name == c_field_id_Allow_Anonymous_Access ) || ( id_or_name == c_field_name_Allow_Anonymous_Access ) )
   {
      name = string( c_field_display_name_Allow_Anonymous_Access );
      get_module_string( c_field_display_name_Allow_Anonymous_Access, &next );
   }
   if( ( id_or_name == c_field_id_Allow_Quick_Link ) || ( id_or_name == c_field_name_Allow_Quick_Link ) )
   {
      name = string( c_field_display_name_Allow_Quick_Link );
      get_module_string( c_field_display_name_Allow_Quick_Link, &next );
   }
   if( ( id_or_name == c_field_id_Allow_Text_Search ) || ( id_or_name == c_field_name_Allow_Text_Search ) )
   {
      name = string( c_field_display_name_Allow_Text_Search );
      get_module_string( c_field_display_name_Allow_Text_Search, &next );
   }
   if( ( id_or_name == c_field_id_Class ) || ( id_or_name == c_field_name_Class ) )
   {
      name = string( c_field_display_name_Class );
      get_module_string( c_field_display_name_Class, &next );
   }
   if( ( id_or_name == c_field_id_Create_Only_If_Default_Other ) || ( id_or_name == c_field_name_Create_Only_If_Default_Other ) )
   {
      name = string( c_field_display_name_Create_Only_If_Default_Other );
      get_module_string( c_field_display_name_Create_Only_If_Default_Other, &next );
   }
   if( ( id_or_name == c_field_id_Create_Parent_Modifier ) || ( id_or_name == c_field_name_Create_Parent_Modifier ) )
   {
      name = string( c_field_display_name_Create_Parent_Modifier );
      get_module_string( c_field_display_name_Create_Parent_Modifier, &next );
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
   if( ( id_or_name == c_field_id_Delete_Direction ) || ( id_or_name == c_field_name_Delete_Direction ) )
   {
      name = string( c_field_display_name_Delete_Direction );
      get_module_string( c_field_display_name_Delete_Direction, &next );
   }
   if( ( id_or_name == c_field_id_Destroy_Only_If_Default_Other ) || ( id_or_name == c_field_name_Destroy_Only_If_Default_Other ) )
   {
      name = string( c_field_display_name_Destroy_Only_If_Default_Other );
      get_module_string( c_field_display_name_Destroy_Only_If_Default_Other, &next );
   }
   if( ( id_or_name == c_field_id_Destroy_Parent_Modifier ) || ( id_or_name == c_field_name_Destroy_Parent_Modifier ) )
   {
      name = string( c_field_display_name_Destroy_Parent_Modifier );
      get_module_string( c_field_display_name_Destroy_Parent_Modifier, &next );
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
   if( ( id_or_name == c_field_id_Direction ) || ( id_or_name == c_field_name_Direction ) )
   {
      name = string( c_field_display_name_Direction );
      get_module_string( c_field_display_name_Direction, &next );
   }
   if( ( id_or_name == c_field_id_Display_Only_If_Default_Other ) || ( id_or_name == c_field_name_Display_Only_If_Default_Other ) )
   {
      name = string( c_field_display_name_Display_Only_If_Default_Other );
      get_module_string( c_field_display_name_Display_Only_If_Default_Other, &next );
   }
   if( ( id_or_name == c_field_id_Display_Row_Limit ) || ( id_or_name == c_field_name_Display_Row_Limit ) )
   {
      name = string( c_field_display_name_Display_Row_Limit );
      get_module_string( c_field_display_name_Display_Row_Limit, &next );
   }
   if( ( id_or_name == c_field_id_Display_Security_Level ) || ( id_or_name == c_field_name_Display_Security_Level ) )
   {
      name = string( c_field_display_name_Display_Security_Level );
      get_module_string( c_field_display_name_Display_Security_Level, &next );
   }
   if( ( id_or_name == c_field_id_Display_Sub_Totals ) || ( id_or_name == c_field_name_Display_Sub_Totals ) )
   {
      name = string( c_field_display_name_Display_Sub_Totals );
      get_module_string( c_field_display_name_Display_Sub_Totals, &next );
   }
   if( ( id_or_name == c_field_id_Display_Totals ) || ( id_or_name == c_field_name_Display_Totals ) )
   {
      name = string( c_field_display_name_Display_Totals );
      get_module_string( c_field_display_name_Display_Totals, &next );
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
   if( ( id_or_name == c_field_id_Ignore_Implicit_Ordering ) || ( id_or_name == c_field_name_Ignore_Implicit_Ordering ) )
   {
      name = string( c_field_display_name_Ignore_Implicit_Ordering );
      get_module_string( c_field_display_name_Ignore_Implicit_Ordering, &next );
   }
   if( ( id_or_name == c_field_id_Ignore_State_For_Display ) || ( id_or_name == c_field_name_Ignore_State_For_Display ) )
   {
      name = string( c_field_display_name_Ignore_State_For_Display );
      get_module_string( c_field_display_name_Ignore_State_For_Display, &next );
   }
   if( ( id_or_name == c_field_id_Ignore_Unactionable_Records ) || ( id_or_name == c_field_name_Ignore_Unactionable_Records ) )
   {
      name = string( c_field_display_name_Ignore_Unactionable_Records );
      get_module_string( c_field_display_name_Ignore_Unactionable_Records, &next );
   }
   if( ( id_or_name == c_field_id_Ignore_Uneditable_Parent ) || ( id_or_name == c_field_name_Ignore_Uneditable_Parent ) )
   {
      name = string( c_field_display_name_Ignore_Uneditable_Parent );
      get_module_string( c_field_display_name_Ignore_Uneditable_Parent, &next );
   }
   if( ( id_or_name == c_field_id_Ignore_Username_Filter ) || ( id_or_name == c_field_name_Ignore_Username_Filter ) )
   {
      name = string( c_field_display_name_Ignore_Username_Filter );
      get_module_string( c_field_display_name_Ignore_Username_Filter, &next );
   }
   if( ( id_or_name == c_field_id_Is_Admin ) || ( id_or_name == c_field_name_Is_Admin ) )
   {
      name = string( c_field_display_name_Is_Admin );
      get_module_string( c_field_display_name_Is_Admin, &next );
   }
   if( ( id_or_name == c_field_id_Is_Child ) || ( id_or_name == c_field_name_Is_Child ) )
   {
      name = string( c_field_display_name_Is_Child );
      get_module_string( c_field_display_name_Is_Child, &next );
   }
   if( ( id_or_name == c_field_id_Is_Home ) || ( id_or_name == c_field_name_Is_Home ) )
   {
      name = string( c_field_display_name_Is_Home );
      get_module_string( c_field_display_name_Is_Home, &next );
   }
   if( ( id_or_name == c_field_id_Is_Not_Anonymous ) || ( id_or_name == c_field_name_Is_Not_Anonymous ) )
   {
      name = string( c_field_display_name_Is_Not_Anonymous );
      get_module_string( c_field_display_name_Is_Not_Anonymous, &next );
   }
   if( ( id_or_name == c_field_id_Is_Variation ) || ( id_or_name == c_field_name_Is_Variation ) )
   {
      name = string( c_field_display_name_Is_Variation );
      get_module_string( c_field_display_name_Is_Variation, &next );
   }
   if( ( id_or_name == c_field_id_Limit_Scroll_And_New ) || ( id_or_name == c_field_name_Limit_Scroll_And_New ) )
   {
      name = string( c_field_display_name_Limit_Scroll_And_New );
      get_module_string( c_field_display_name_Limit_Scroll_And_New, &next );
   }
   if( ( id_or_name == c_field_id_Model ) || ( id_or_name == c_field_name_Model ) )
   {
      name = string( c_field_display_name_Model );
      get_module_string( c_field_display_name_Model, &next );
   }
   if( ( id_or_name == c_field_id_Multiline_Truncate_For_Print ) || ( id_or_name == c_field_name_Multiline_Truncate_For_Print ) )
   {
      name = string( c_field_display_name_Multiline_Truncate_For_Print );
      get_module_string( c_field_display_name_Multiline_Truncate_For_Print, &next );
   }
   if( ( id_or_name == c_field_id_Name ) || ( id_or_name == c_field_name_Name ) )
   {
      name = string( c_field_display_name_Name );
      get_module_string( c_field_display_name_Name, &next );
   }
   if( ( id_or_name == c_field_id_Number_Multiple_Pages ) || ( id_or_name == c_field_name_Number_Multiple_Pages ) )
   {
      name = string( c_field_display_name_Number_Multiple_Pages );
      get_module_string( c_field_display_name_Number_Multiple_Pages, &next );
   }
   if( ( id_or_name == c_field_id_PDF_Font_Type ) || ( id_or_name == c_field_name_PDF_Font_Type ) )
   {
      name = string( c_field_display_name_PDF_Font_Type );
      get_module_string( c_field_display_name_PDF_Font_Type, &next );
   }
   if( ( id_or_name == c_field_id_PDF_List_Type ) || ( id_or_name == c_field_name_PDF_List_Type ) )
   {
      name = string( c_field_display_name_PDF_List_Type );
      get_module_string( c_field_display_name_PDF_List_Type, &next );
   }
   if( ( id_or_name == c_field_id_Parent_Class ) || ( id_or_name == c_field_name_Parent_Class ) )
   {
      name = string( c_field_display_name_Parent_Class );
      get_module_string( c_field_display_name_Parent_Class, &next );
   }
   if( ( id_or_name == c_field_id_Parent_Field ) || ( id_or_name == c_field_name_Parent_Field ) )
   {
      name = string( c_field_display_name_Parent_Field );
      get_module_string( c_field_display_name_Parent_Field, &next );
   }
   if( ( id_or_name == c_field_id_Print_Restriction ) || ( id_or_name == c_field_name_Print_Restriction ) )
   {
      name = string( c_field_display_name_Print_Restriction );
      get_module_string( c_field_display_name_Print_Restriction, &next );
   }
   if( ( id_or_name == c_field_id_Print_Without_Highlight ) || ( id_or_name == c_field_name_Print_Without_Highlight ) )
   {
      name = string( c_field_display_name_Print_Without_Highlight );
      get_module_string( c_field_display_name_Print_Without_Highlight, &next );
   }
   if( ( id_or_name == c_field_id_Search_Option_Limit ) || ( id_or_name == c_field_name_Search_Option_Limit ) )
   {
      name = string( c_field_display_name_Search_Option_Limit );
      get_module_string( c_field_display_name_Search_Option_Limit, &next );
   }
   if( ( id_or_name == c_field_id_Sort_Rows_In_UI ) || ( id_or_name == c_field_name_Sort_Rows_In_UI ) )
   {
      name = string( c_field_display_name_Sort_Rows_In_UI );
      get_module_string( c_field_display_name_Sort_Rows_In_UI, &next );
   }
   if( ( id_or_name == c_field_id_Style ) || ( id_or_name == c_field_name_Style ) )
   {
      name = string( c_field_display_name_Style );
      get_module_string( c_field_display_name_Style, &next );
   }
   if( ( id_or_name == c_field_id_Text_Match_Highlight ) || ( id_or_name == c_field_name_Text_Match_Highlight ) )
   {
      name = string( c_field_display_name_Text_Match_Highlight );
      get_module_string( c_field_display_name_Text_Match_Highlight, &next );
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
   if( ( id_or_name == c_field_id_Variation_Name ) || ( id_or_name == c_field_name_Variation_Name ) )
   {
      name = string( c_field_display_name_Variation_Name );
      get_module_string( c_field_display_name_Variation_Name, &next );
   }

   // NOTE: It is being assumed here that the customised UOM symbol for a field (if it
   // has one) will be in the module string that immediately follows that of its name.
   if( next.first.find( name + "_(" ) == 0 )
      uom_symbol = next.second;

   return uom_symbol;
}

string Meta_List::get_field_display_name( const string& id_or_name ) const
{
   string display_name;

   if( id_or_name.empty( ) )
      throw runtime_error( "unexpected empty field id_or_name for get_field_display_name" );
   if( ( id_or_name == c_field_id_Access_Parent_Modifier ) || ( id_or_name == c_field_name_Access_Parent_Modifier ) )
      display_name = get_module_string( c_field_display_name_Access_Parent_Modifier );
   if( ( id_or_name == c_field_id_Access_Permission ) || ( id_or_name == c_field_name_Access_Permission ) )
      display_name = get_module_string( c_field_display_name_Access_Permission );
   if( ( id_or_name == c_field_id_Access_Restriction ) || ( id_or_name == c_field_name_Access_Restriction ) )
      display_name = get_module_string( c_field_display_name_Access_Restriction );
   if( ( id_or_name == c_field_id_Allow_Anonymous_Access ) || ( id_or_name == c_field_name_Allow_Anonymous_Access ) )
      display_name = get_module_string( c_field_display_name_Allow_Anonymous_Access );
   if( ( id_or_name == c_field_id_Allow_Quick_Link ) || ( id_or_name == c_field_name_Allow_Quick_Link ) )
      display_name = get_module_string( c_field_display_name_Allow_Quick_Link );
   if( ( id_or_name == c_field_id_Allow_Text_Search ) || ( id_or_name == c_field_name_Allow_Text_Search ) )
      display_name = get_module_string( c_field_display_name_Allow_Text_Search );
   if( ( id_or_name == c_field_id_Class ) || ( id_or_name == c_field_name_Class ) )
      display_name = get_module_string( c_field_display_name_Class );
   if( ( id_or_name == c_field_id_Create_Only_If_Default_Other ) || ( id_or_name == c_field_name_Create_Only_If_Default_Other ) )
      display_name = get_module_string( c_field_display_name_Create_Only_If_Default_Other );
   if( ( id_or_name == c_field_id_Create_Parent_Modifier ) || ( id_or_name == c_field_name_Create_Parent_Modifier ) )
      display_name = get_module_string( c_field_display_name_Create_Parent_Modifier );
   if( ( id_or_name == c_field_id_Create_Permission ) || ( id_or_name == c_field_name_Create_Permission ) )
      display_name = get_module_string( c_field_display_name_Create_Permission );
   if( ( id_or_name == c_field_id_Create_Restriction ) || ( id_or_name == c_field_name_Create_Restriction ) )
      display_name = get_module_string( c_field_display_name_Create_Restriction );
   if( ( id_or_name == c_field_id_Delete_Direction ) || ( id_or_name == c_field_name_Delete_Direction ) )
      display_name = get_module_string( c_field_display_name_Delete_Direction );
   if( ( id_or_name == c_field_id_Destroy_Only_If_Default_Other ) || ( id_or_name == c_field_name_Destroy_Only_If_Default_Other ) )
      display_name = get_module_string( c_field_display_name_Destroy_Only_If_Default_Other );
   if( ( id_or_name == c_field_id_Destroy_Parent_Modifier ) || ( id_or_name == c_field_name_Destroy_Parent_Modifier ) )
      display_name = get_module_string( c_field_display_name_Destroy_Parent_Modifier );
   if( ( id_or_name == c_field_id_Destroy_Permission ) || ( id_or_name == c_field_name_Destroy_Permission ) )
      display_name = get_module_string( c_field_display_name_Destroy_Permission );
   if( ( id_or_name == c_field_id_Destroy_Restriction ) || ( id_or_name == c_field_name_Destroy_Restriction ) )
      display_name = get_module_string( c_field_display_name_Destroy_Restriction );
   if( ( id_or_name == c_field_id_Direction ) || ( id_or_name == c_field_name_Direction ) )
      display_name = get_module_string( c_field_display_name_Direction );
   if( ( id_or_name == c_field_id_Display_Only_If_Default_Other ) || ( id_or_name == c_field_name_Display_Only_If_Default_Other ) )
      display_name = get_module_string( c_field_display_name_Display_Only_If_Default_Other );
   if( ( id_or_name == c_field_id_Display_Row_Limit ) || ( id_or_name == c_field_name_Display_Row_Limit ) )
      display_name = get_module_string( c_field_display_name_Display_Row_Limit );
   if( ( id_or_name == c_field_id_Display_Security_Level ) || ( id_or_name == c_field_name_Display_Security_Level ) )
      display_name = get_module_string( c_field_display_name_Display_Security_Level );
   if( ( id_or_name == c_field_id_Display_Sub_Totals ) || ( id_or_name == c_field_name_Display_Sub_Totals ) )
      display_name = get_module_string( c_field_display_name_Display_Sub_Totals );
   if( ( id_or_name == c_field_id_Display_Totals ) || ( id_or_name == c_field_name_Display_Totals ) )
      display_name = get_module_string( c_field_display_name_Display_Totals );
   if( ( id_or_name == c_field_id_File_Links_Always_As_Single ) || ( id_or_name == c_field_name_File_Links_Always_As_Single ) )
      display_name = get_module_string( c_field_display_name_File_Links_Always_As_Single );
   if( ( id_or_name == c_field_id_Id ) || ( id_or_name == c_field_name_Id ) )
      display_name = get_module_string( c_field_display_name_Id );
   if( ( id_or_name == c_field_id_Ignore_Implicit_Ordering ) || ( id_or_name == c_field_name_Ignore_Implicit_Ordering ) )
      display_name = get_module_string( c_field_display_name_Ignore_Implicit_Ordering );
   if( ( id_or_name == c_field_id_Ignore_State_For_Display ) || ( id_or_name == c_field_name_Ignore_State_For_Display ) )
      display_name = get_module_string( c_field_display_name_Ignore_State_For_Display );
   if( ( id_or_name == c_field_id_Ignore_Unactionable_Records ) || ( id_or_name == c_field_name_Ignore_Unactionable_Records ) )
      display_name = get_module_string( c_field_display_name_Ignore_Unactionable_Records );
   if( ( id_or_name == c_field_id_Ignore_Uneditable_Parent ) || ( id_or_name == c_field_name_Ignore_Uneditable_Parent ) )
      display_name = get_module_string( c_field_display_name_Ignore_Uneditable_Parent );
   if( ( id_or_name == c_field_id_Ignore_Username_Filter ) || ( id_or_name == c_field_name_Ignore_Username_Filter ) )
      display_name = get_module_string( c_field_display_name_Ignore_Username_Filter );
   if( ( id_or_name == c_field_id_Is_Admin ) || ( id_or_name == c_field_name_Is_Admin ) )
      display_name = get_module_string( c_field_display_name_Is_Admin );
   if( ( id_or_name == c_field_id_Is_Child ) || ( id_or_name == c_field_name_Is_Child ) )
      display_name = get_module_string( c_field_display_name_Is_Child );
   if( ( id_or_name == c_field_id_Is_Home ) || ( id_or_name == c_field_name_Is_Home ) )
      display_name = get_module_string( c_field_display_name_Is_Home );
   if( ( id_or_name == c_field_id_Is_Not_Anonymous ) || ( id_or_name == c_field_name_Is_Not_Anonymous ) )
      display_name = get_module_string( c_field_display_name_Is_Not_Anonymous );
   if( ( id_or_name == c_field_id_Is_Variation ) || ( id_or_name == c_field_name_Is_Variation ) )
      display_name = get_module_string( c_field_display_name_Is_Variation );
   if( ( id_or_name == c_field_id_Limit_Scroll_And_New ) || ( id_or_name == c_field_name_Limit_Scroll_And_New ) )
      display_name = get_module_string( c_field_display_name_Limit_Scroll_And_New );
   if( ( id_or_name == c_field_id_Model ) || ( id_or_name == c_field_name_Model ) )
      display_name = get_module_string( c_field_display_name_Model );
   if( ( id_or_name == c_field_id_Multiline_Truncate_For_Print ) || ( id_or_name == c_field_name_Multiline_Truncate_For_Print ) )
      display_name = get_module_string( c_field_display_name_Multiline_Truncate_For_Print );
   if( ( id_or_name == c_field_id_Name ) || ( id_or_name == c_field_name_Name ) )
      display_name = get_module_string( c_field_display_name_Name );
   if( ( id_or_name == c_field_id_Number_Multiple_Pages ) || ( id_or_name == c_field_name_Number_Multiple_Pages ) )
      display_name = get_module_string( c_field_display_name_Number_Multiple_Pages );
   if( ( id_or_name == c_field_id_PDF_Font_Type ) || ( id_or_name == c_field_name_PDF_Font_Type ) )
      display_name = get_module_string( c_field_display_name_PDF_Font_Type );
   if( ( id_or_name == c_field_id_PDF_List_Type ) || ( id_or_name == c_field_name_PDF_List_Type ) )
      display_name = get_module_string( c_field_display_name_PDF_List_Type );
   if( ( id_or_name == c_field_id_Parent_Class ) || ( id_or_name == c_field_name_Parent_Class ) )
      display_name = get_module_string( c_field_display_name_Parent_Class );
   if( ( id_or_name == c_field_id_Parent_Field ) || ( id_or_name == c_field_name_Parent_Field ) )
      display_name = get_module_string( c_field_display_name_Parent_Field );
   if( ( id_or_name == c_field_id_Print_Restriction ) || ( id_or_name == c_field_name_Print_Restriction ) )
      display_name = get_module_string( c_field_display_name_Print_Restriction );
   if( ( id_or_name == c_field_id_Print_Without_Highlight ) || ( id_or_name == c_field_name_Print_Without_Highlight ) )
      display_name = get_module_string( c_field_display_name_Print_Without_Highlight );
   if( ( id_or_name == c_field_id_Search_Option_Limit ) || ( id_or_name == c_field_name_Search_Option_Limit ) )
      display_name = get_module_string( c_field_display_name_Search_Option_Limit );
   if( ( id_or_name == c_field_id_Sort_Rows_In_UI ) || ( id_or_name == c_field_name_Sort_Rows_In_UI ) )
      display_name = get_module_string( c_field_display_name_Sort_Rows_In_UI );
   if( ( id_or_name == c_field_id_Style ) || ( id_or_name == c_field_name_Style ) )
      display_name = get_module_string( c_field_display_name_Style );
   if( ( id_or_name == c_field_id_Text_Match_Highlight ) || ( id_or_name == c_field_name_Text_Match_Highlight ) )
      display_name = get_module_string( c_field_display_name_Text_Match_Highlight );
   if( ( id_or_name == c_field_id_Title ) || ( id_or_name == c_field_name_Title ) )
      display_name = get_module_string( c_field_display_name_Title );
   if( ( id_or_name == c_field_id_Type ) || ( id_or_name == c_field_name_Type ) )
      display_name = get_module_string( c_field_display_name_Type );
   if( ( id_or_name == c_field_id_Variation_Name ) || ( id_or_name == c_field_name_Variation_Name ) )
      display_name = get_module_string( c_field_display_name_Variation_Name );

   return display_name;
}

void Meta_List::clear_foreign_key( const string& field )
{
   p_impl->clear_foreign_key( field );
}

void Meta_List::set_foreign_key_value( const string& field, const string& value )
{
   p_impl->set_foreign_key_value( field, value );
}

const string& Meta_List::get_foreign_key_value( const string& field )
{
   return p_impl->get_foreign_key_value( field );
}

void Meta_List::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   p_impl->get_foreign_key_values( foreign_key_values );
}

void Meta_List::setup_foreign_key( Meta_Modifier& o, const string& value )
{
   static_cast< Meta_Modifier& >( o ).set_key( value );
}

void Meta_List::setup_foreign_key( Meta_Permission& o, const string& value )
{
   static_cast< Meta_Permission& >( o ).set_key( value );
}

void Meta_List::setup_foreign_key( Meta_Class& o, const string& value )
{
   static_cast< Meta_Class& >( o ).set_key( value );
}

void Meta_List::setup_foreign_key( Meta_Model& o, const string& value )
{
   static_cast< Meta_Model& >( o ).set_key( value );
}

void Meta_List::setup_foreign_key( Meta_Field& o, const string& value )
{
   static_cast< Meta_Field& >( o ).set_key( value );
}

void Meta_List::setup_foreign_key( Meta_List_Type& o, const string& value )
{
   static_cast< Meta_List_Type& >( o ).set_key( value );
}

void Meta_List::setup_graph_parent( Meta_Class& o, const string& foreign_key_field )
{
   static_cast< Meta_Class& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_List::setup_graph_parent( Meta_List_Field& o, const string& foreign_key_field )
{
   static_cast< Meta_List_Field& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_List::setup_graph_parent( Meta_Package_Option& o, const string& foreign_key_field )
{
   static_cast< Meta_Package_Option& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_List::setup_graph_parent(
 Meta_Modifier& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Modifier& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Modifier& >( o ).set_key( init_value );
}

void Meta_List::setup_graph_parent(
 Meta_Permission& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Permission& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Permission& >( o ).set_key( init_value );
}

void Meta_List::setup_graph_parent(
 Meta_Class& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Class& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Class& >( o ).set_key( init_value );
}

void Meta_List::setup_graph_parent(
 Meta_Model& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Model& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Model& >( o ).set_key( init_value );
}

void Meta_List::setup_graph_parent(
 Meta_Field& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Field& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Field& >( o ).set_key( init_value );
}

void Meta_List::setup_graph_parent(
 Meta_List_Type& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_List_Type& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_List_Type& >( o ).set_key( init_value );
}

size_t Meta_List::get_total_child_relationships( ) const
{
   return p_impl->total_child_relationships;
}

void Meta_List::set_total_child_relationships( size_t new_total_child_relationships ) const
{
   p_impl->total_child_relationships = new_total_child_relationships;
}

size_t Meta_List::get_num_foreign_key_children( bool is_internal ) const
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

class_base* Meta_List::get_next_foreign_key_child(
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
            next_child_field = "300632";
            p_class_base = &child_Class_Created( );
         }
         break;

         case 1:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "302100";
            p_class_base = &child_List_Field( );
         }
         break;

         case 2:
         if( op == e_cascade_op_unlink )
         {
            next_child_field = "302831";
            p_class_base = &child_Package_Option( );
         }
         break;
      }
   }

   return p_class_base;
}

void Meta_List::add_extra_fixed_info( vector< pair< string, string > >& fixed_info ) const
{
   p_impl->add_extra_fixed_info( fixed_info );
}

void Meta_List::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   p_impl->add_extra_paging_info( paging_info );
}

string Meta_List::get_class_id( ) const
{
   return static_class_id( );
}

string Meta_List::get_class_name( ) const
{
   return static_class_name( );
}

string Meta_List::get_plural_name( ) const
{
   return static_plural_name( );
}

string Meta_List::get_module_id( ) const
{
   return static_module_id( );
}

string Meta_List::get_module_name( ) const
{
   return static_module_name( );
}

string Meta_List::get_display_name( bool plural ) const
{
   string key( plural ? "plural_" : "class_" );

   key += "list";

   return get_module_string( key );
}

string Meta_List::get_raw_variable( const std::string& name ) const
{
   if( name == g_state_names_variable )
      return p_impl->get_state_names( );
   else
      return class_base::get_raw_variable( name );
}

string Meta_List::get_create_instance_info( ) const
{
   return "";
}

string Meta_List::get_update_instance_info( ) const
{
   return "";
}

string Meta_List::get_destroy_instance_info( ) const
{
   return "";
}

string Meta_List::get_execute_procedure_info( const string& procedure_id ) const
{
   string retval;

   if( procedure_id.empty( ) )
      throw runtime_error( "unexpected empty procedure_id for get_execute_procedure_info" );
   else if( procedure_id == "122430" ) // i.e. Generate_PDF_List
      retval = "";

   return retval;
}

bool Meta_List::get_is_alias( ) const
{
   return false;
}

void Meta_List::get_alias_base_info( pair< string, string >& alias_base_info ) const
{
   ( void )alias_base_info;
}

void Meta_List::get_base_class_info( vector< pair< string, string > >& base_class_info ) const
{
   ( void )base_class_info;
}

class_base& Meta_List::get_or_create_graph_child( const string& context )
{
   class_base* p_class_base( 0 );

   string::size_type pos = context.find( '.' );
   string sub_context( context.substr( 0, pos ) );

   if( sub_context.empty( ) )
      throw runtime_error( "unexpected empty sub-context" );
   else if( ( sub_context == "_300632" ) || ( sub_context == "child_Class_Created" ) )
      p_class_base = &child_Class_Created( );
   else if( ( sub_context == "_302100" ) || ( sub_context == "child_List_Field" ) )
      p_class_base = &child_List_Field( );
   else if( ( sub_context == "_302831" ) || ( sub_context == "child_Package_Option" ) )
      p_class_base = &child_Package_Option( );
   else if( ( sub_context == c_field_id_Access_Parent_Modifier ) || ( sub_context == c_field_name_Access_Parent_Modifier ) )
      p_class_base = &Access_Parent_Modifier( );
   else if( ( sub_context == c_field_id_Access_Permission ) || ( sub_context == c_field_name_Access_Permission ) )
      p_class_base = &Access_Permission( );
   else if( ( sub_context == c_field_id_Class ) || ( sub_context == c_field_name_Class ) )
      p_class_base = &Class( );
   else if( ( sub_context == c_field_id_Create_Parent_Modifier ) || ( sub_context == c_field_name_Create_Parent_Modifier ) )
      p_class_base = &Create_Parent_Modifier( );
   else if( ( sub_context == c_field_id_Create_Permission ) || ( sub_context == c_field_name_Create_Permission ) )
      p_class_base = &Create_Permission( );
   else if( ( sub_context == c_field_id_Destroy_Parent_Modifier ) || ( sub_context == c_field_name_Destroy_Parent_Modifier ) )
      p_class_base = &Destroy_Parent_Modifier( );
   else if( ( sub_context == c_field_id_Destroy_Permission ) || ( sub_context == c_field_name_Destroy_Permission ) )
      p_class_base = &Destroy_Permission( );
   else if( ( sub_context == c_field_id_Model ) || ( sub_context == c_field_name_Model ) )
      p_class_base = &Model( );
   else if( ( sub_context == c_field_id_Parent_Class ) || ( sub_context == c_field_name_Parent_Class ) )
      p_class_base = &Parent_Class( );
   else if( ( sub_context == c_field_id_Parent_Field ) || ( sub_context == c_field_name_Parent_Field ) )
      p_class_base = &Parent_Field( );
   else if( ( sub_context == c_field_id_Type ) || ( sub_context == c_field_name_Type ) )
      p_class_base = &Type( );

   if( !p_class_base )
      throw runtime_error( "unknown sub-context '" + sub_context + "'" );

   if( pos != string::npos )
      p_class_base = &p_class_base->get_or_create_graph_child( context.substr( pos + 1 ) );

   return *p_class_base;
}

void Meta_List::get_sql_column_names(
 vector< string >& names, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   names.push_back( "C_Access_Parent_Modifier" );
   names.push_back( "C_Access_Permission" );
   names.push_back( "C_Access_Restriction" );
   names.push_back( "C_Allow_Anonymous_Access" );
   names.push_back( "C_Allow_Quick_Link" );
   names.push_back( "C_Allow_Text_Search" );
   names.push_back( "C_Class" );
   names.push_back( "C_Create_Only_If_Default_Other" );
   names.push_back( "C_Create_Parent_Modifier" );
   names.push_back( "C_Create_Permission" );
   names.push_back( "C_Create_Restriction" );
   names.push_back( "C_Delete_Direction" );
   names.push_back( "C_Destroy_Only_If_Default_Other" );
   names.push_back( "C_Destroy_Parent_Modifier" );
   names.push_back( "C_Destroy_Permission" );
   names.push_back( "C_Destroy_Restriction" );
   names.push_back( "C_Direction" );
   names.push_back( "C_Display_Only_If_Default_Other" );
   names.push_back( "C_Display_Row_Limit" );
   names.push_back( "C_Display_Security_Level" );
   names.push_back( "C_Display_Sub_Totals" );
   names.push_back( "C_Display_Totals" );
   names.push_back( "C_File_Links_Always_As_Single" );
   names.push_back( "C_Id" );
   names.push_back( "C_Ignore_Implicit_Ordering" );
   names.push_back( "C_Ignore_State_For_Display" );
   names.push_back( "C_Ignore_Unactionable_Records" );
   names.push_back( "C_Ignore_Uneditable_Parent" );
   names.push_back( "C_Ignore_Username_Filter" );
   names.push_back( "C_Is_Admin" );
   names.push_back( "C_Is_Child" );
   names.push_back( "C_Is_Home" );
   names.push_back( "C_Is_Not_Anonymous" );
   names.push_back( "C_Is_Variation" );
   names.push_back( "C_Limit_Scroll_And_New" );
   names.push_back( "C_Model" );
   names.push_back( "C_Multiline_Truncate_For_Print" );
   names.push_back( "C_Number_Multiple_Pages" );
   names.push_back( "C_PDF_Font_Type" );
   names.push_back( "C_PDF_List_Type" );
   names.push_back( "C_Parent_Class" );
   names.push_back( "C_Parent_Field" );
   names.push_back( "C_Print_Restriction" );
   names.push_back( "C_Print_Without_Highlight" );
   names.push_back( "C_Search_Option_Limit" );
   names.push_back( "C_Sort_Rows_In_UI" );
   names.push_back( "C_Style" );
   names.push_back( "C_Text_Match_Highlight" );
   names.push_back( "C_Title" );
   names.push_back( "C_Type" );
   names.push_back( "C_Variation_Name" );

   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;
}

void Meta_List::get_sql_column_values(
 vector< string >& values, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   values.push_back( sql_quote( to_string( Access_Parent_Modifier( ) ) ) );
   values.push_back( sql_quote( to_string( Access_Permission( ) ) ) );
   values.push_back( to_string( Access_Restriction( ) ) );
   values.push_back( to_string( Allow_Anonymous_Access( ) ) );
   values.push_back( to_string( Allow_Quick_Link( ) ) );
   values.push_back( to_string( Allow_Text_Search( ) ) );
   values.push_back( sql_quote( to_string( Class( ) ) ) );
   values.push_back( to_string( Create_Only_If_Default_Other( ) ) );
   values.push_back( sql_quote( to_string( Create_Parent_Modifier( ) ) ) );
   values.push_back( sql_quote( to_string( Create_Permission( ) ) ) );
   values.push_back( to_string( Create_Restriction( ) ) );
   values.push_back( to_string( Delete_Direction( ) ) );
   values.push_back( to_string( Destroy_Only_If_Default_Other( ) ) );
   values.push_back( sql_quote( to_string( Destroy_Parent_Modifier( ) ) ) );
   values.push_back( sql_quote( to_string( Destroy_Permission( ) ) ) );
   values.push_back( to_string( Destroy_Restriction( ) ) );
   values.push_back( to_string( Direction( ) ) );
   values.push_back( to_string( Display_Only_If_Default_Other( ) ) );
   values.push_back( to_string( Display_Row_Limit( ) ) );
   values.push_back( to_string( Display_Security_Level( ) ) );
   values.push_back( to_string( Display_Sub_Totals( ) ) );
   values.push_back( to_string( Display_Totals( ) ) );
   values.push_back( to_string( File_Links_Always_As_Single( ) ) );
   values.push_back( sql_quote( to_string( Id( ) ) ) );
   values.push_back( to_string( Ignore_Implicit_Ordering( ) ) );
   values.push_back( to_string( Ignore_State_For_Display( ) ) );
   values.push_back( to_string( Ignore_Unactionable_Records( ) ) );
   values.push_back( to_string( Ignore_Uneditable_Parent( ) ) );
   values.push_back( to_string( Ignore_Username_Filter( ) ) );
   values.push_back( to_string( Is_Admin( ) ) );
   values.push_back( to_string( Is_Child( ) ) );
   values.push_back( to_string( Is_Home( ) ) );
   values.push_back( to_string( Is_Not_Anonymous( ) ) );
   values.push_back( to_string( Is_Variation( ) ) );
   values.push_back( to_string( Limit_Scroll_And_New( ) ) );
   values.push_back( sql_quote( to_string( Model( ) ) ) );
   values.push_back( to_string( Multiline_Truncate_For_Print( ) ) );
   values.push_back( to_string( Number_Multiple_Pages( ) ) );
   values.push_back( to_string( PDF_Font_Type( ) ) );
   values.push_back( to_string( PDF_List_Type( ) ) );
   values.push_back( sql_quote( to_string( Parent_Class( ) ) ) );
   values.push_back( sql_quote( to_string( Parent_Field( ) ) ) );
   values.push_back( to_string( Print_Restriction( ) ) );
   values.push_back( to_string( Print_Without_Highlight( ) ) );
   values.push_back( to_string( Search_Option_Limit( ) ) );
   values.push_back( to_string( Sort_Rows_In_UI( ) ) );
   values.push_back( to_string( Style( ) ) );
   values.push_back( to_string( Text_Match_Highlight( ) ) );
   values.push_back( sql_quote( to_string( Title( ) ) ) );
   values.push_back( sql_quote( to_string( Type( ) ) ) );
   values.push_back( sql_quote( to_string( Variation_Name( ) ) ) );

   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;
}

void Meta_List::get_required_field_names(
 set< string >& names, bool use_transients, set< string >* p_dependents ) const
{
   set< string > local_dependents;
   set< string >& dependents( p_dependents ? *p_dependents : local_dependents );

   get_always_required_field_names( names, use_transients, dependents );

   // [(start field_from_other_field)] 600415
   if( needs_field_value( "Variation_Name", dependents ) )
   {
      dependents.insert( "Title" );

      if( ( use_transients && is_field_transient( e_field_id_Title ) )
       || ( !use_transients && !is_field_transient( e_field_id_Title ) ) )
         names.insert( "Title" );

      dependents.insert( "Is_Variation" );

      if( ( use_transients && is_field_transient( e_field_id_Is_Variation ) )
       || ( !use_transients && !is_field_transient( e_field_id_Is_Variation ) ) )
         names.insert( "Is_Variation" );
   }
   // [(finish field_from_other_field)] 600415

   // [(start field_from_search_replace)] 600420
   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Class" );

      if( ( use_transients && is_field_transient( e_field_id_Class ) )
       || ( !use_transients && !is_field_transient( e_field_id_Class ) ) )
         names.insert( "Class" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Parent_Field" );

      if( ( use_transients && is_field_transient( e_field_id_Parent_Field ) )
       || ( !use_transients && !is_field_transient( e_field_id_Parent_Field ) ) )
         names.insert( "Parent_Field" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Variation_Name" );

      if( ( use_transients && is_field_transient( e_field_id_Variation_Name ) )
       || ( !use_transients && !is_field_transient( e_field_id_Variation_Name ) ) )
         names.insert( "Variation_Name" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Type" );

      if( ( use_transients && is_field_transient( e_field_id_Type ) )
       || ( !use_transients && !is_field_transient( e_field_id_Type ) ) )
         names.insert( "Type" );
   }
   // [(finish field_from_search_replace)] 600420

   // [(start field_from_other_field)] 600425
   if( needs_field_value( "Is_Child", dependents ) )
   {
      dependents.insert( "Type" );

      if( ( use_transients && is_field_transient( e_field_id_Type ) )
       || ( !use_transients && !is_field_transient( e_field_id_Type ) ) )
         names.insert( "Type" );
   }
   // [(finish field_from_other_field)] 600425

   // [(start field_from_other_field)] 600426
   if( needs_field_value( "Is_Home", dependents ) )
   {
      dependents.insert( "Type" );

      if( ( use_transients && is_field_transient( e_field_id_Type ) )
       || ( !use_transients && !is_field_transient( e_field_id_Type ) ) )
         names.insert( "Type" );
   }
   // [(finish field_from_other_field)] 600426

   // [(start field_from_other_field)] 600426a
   if( needs_field_value( "Is_Not_Anonymous", dependents ) )
   {
      dependents.insert( "Type" );

      if( ( use_transients && is_field_transient( e_field_id_Type ) )
       || ( !use_transients && !is_field_transient( e_field_id_Type ) ) )
         names.insert( "Type" );
   }
   // [(finish field_from_other_field)] 600426a

   // [(start field_from_other_field)] 600426b
   if( needs_field_value( "Is_Admin", dependents ) )
   {
      dependents.insert( "Type" );

      if( ( use_transients && is_field_transient( e_field_id_Type ) )
       || ( !use_transients && !is_field_transient( e_field_id_Type ) ) )
         names.insert( "Type" );
   }
   // [(finish field_from_other_field)] 600426b

   // [(start field_from_other_field)] 600429
   if( needs_field_value( "Parent_Class", dependents ) )
   {
      dependents.insert( "Parent_Field" );

      if( ( use_transients && is_field_transient( e_field_id_Parent_Field ) )
       || ( !use_transients && !is_field_transient( e_field_id_Parent_Field ) ) )
         names.insert( "Parent_Field" );
   }
   // [(finish field_from_other_field)] 600429

   // [<start get_required_field_names>]
   // [<finish get_required_field_names>]
}

void Meta_List::get_always_required_field_names(
 set< string >& names, bool use_transients, set< string >& dependents ) const
{
   ( void )names;
   ( void )dependents;
   ( void )use_transients;

   // [(start modifier_field_value)] 600427
   dependents.insert( "PDF_List_Type" ); // (for PDF_List_Is_None modifier)

   if( ( use_transients && is_field_transient( e_field_id_PDF_List_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_PDF_List_Type ) ) )
      names.insert( "PDF_List_Type" );
   // [(finish modifier_field_value)] 600427

   // [(start modifier_field_value)] 600428
   dependents.insert( "PDF_List_Type" ); // (for PDF_List_Is_Custom modifier)

   if( ( use_transients && is_field_transient( e_field_id_PDF_List_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_PDF_List_Type ) ) )
      names.insert( "PDF_List_Type" );
   // [(finish modifier_field_value)] 600428

   // [(start modifier_field_value)] 600430
   dependents.insert( "Is_Child" ); // (for Is_Child modifier)

   if( ( use_transients && is_field_transient( e_field_id_Is_Child ) )
    || ( !use_transients && !is_field_transient( e_field_id_Is_Child ) ) )
      names.insert( "Is_Child" );
   // [(finish modifier_field_value)] 600430

   // [(start modifier_field_value)] 600431
   dependents.insert( "Is_Child" ); // (for Is_Not_Child modifier)

   if( ( use_transients && is_field_transient( e_field_id_Is_Child ) )
    || ( !use_transients && !is_field_transient( e_field_id_Is_Child ) ) )
      names.insert( "Is_Child" );
   // [(finish modifier_field_value)] 600431

   // [(start modifier_field_value)] 600432
   dependents.insert( "Allow_Text_Search" ); // (for Cannot_Text_Search modifier)

   if( ( use_transients && is_field_transient( e_field_id_Allow_Text_Search ) )
    || ( !use_transients && !is_field_transient( e_field_id_Allow_Text_Search ) ) )
      names.insert( "Allow_Text_Search" );
   // [(finish modifier_field_value)] 600432

   // [(start modifier_field_value)] 600433
   dependents.insert( "Style" ); // (for Is_Not_Search_Style modifier)

   if( ( use_transients && is_field_transient( e_field_id_Style ) )
    || ( !use_transients && !is_field_transient( e_field_id_Style ) ) )
      names.insert( "Style" );
   // [(finish modifier_field_value)] 600433

   // [(start modifier_field_value)] 600434
   dependents.insert( "Is_Home" ); // (for Is_Home modifier)

   if( ( use_transients && is_field_transient( e_field_id_Is_Home ) )
    || ( !use_transients && !is_field_transient( e_field_id_Is_Home ) ) )
      names.insert( "Is_Home" );
   // [(finish modifier_field_value)] 600434

   // [(start modifier_field_value)] 600434a
   dependents.insert( "Is_Not_Anonymous" ); // (for Anonymous_Disallowed modifier)

   if( ( use_transients && is_field_transient( e_field_id_Is_Not_Anonymous ) )
    || ( !use_transients && !is_field_transient( e_field_id_Is_Not_Anonymous ) ) )
      names.insert( "Is_Not_Anonymous" );
   // [(finish modifier_field_value)] 600434a

   // [(start modifier_field_value)] 600434b
   dependents.insert( "Access_Restriction" ); // (for Anonymous_Disallowed modifier)

   if( ( use_transients && is_field_transient( e_field_id_Access_Restriction ) )
    || ( !use_transients && !is_field_transient( e_field_id_Access_Restriction ) ) )
      names.insert( "Access_Restriction" );
   // [(finish modifier_field_value)] 600434b

   // [(start modifier_field_null)] 600434c
   dependents.insert( "Access_Permission" ); // (for Anonymous_Disallowed modifier)

   if( ( use_transients && is_field_transient( e_field_id_Access_Permission ) )
    || ( !use_transients && !is_field_transient( e_field_id_Access_Permission ) ) )
      names.insert( "Access_Permission" );
   // [(finish modifier_field_null)] 600434c

   // [(start modifier_field_value)] 600435
   dependents.insert( "Display_Row_Limit" ); // (for Is_Not_Unlimited modifier)

   if( ( use_transients && is_field_transient( e_field_id_Display_Row_Limit ) )
    || ( !use_transients && !is_field_transient( e_field_id_Display_Row_Limit ) ) )
      names.insert( "Display_Row_Limit" );
   // [(finish modifier_field_value)] 600435

   // [(start modifier_field_value)] 600436
   dependents.insert( "Sort_Rows_In_UI" ); // (for Will_Sort_Rows_In_UI modifier)

   if( ( use_transients && is_field_transient( e_field_id_Sort_Rows_In_UI ) )
    || ( !use_transients && !is_field_transient( e_field_id_Sort_Rows_In_UI ) ) )
      names.insert( "Sort_Rows_In_UI" );
   // [(finish modifier_field_value)] 600436

   // [(start modifier_field_value)] 600437
   dependents.insert( "Is_Admin" ); // (for Is_Admin modifier)

   if( ( use_transients && is_field_transient( e_field_id_Is_Admin ) )
    || ( !use_transients && !is_field_transient( e_field_id_Is_Admin ) ) )
      names.insert( "Is_Admin" );
   // [(finish modifier_field_value)] 600437

   // [<start get_always_required_field_names>]
   // [<finish get_always_required_field_names>]
}

void Meta_List::get_transient_replacement_field_names( const string& name, vector< string >& names ) const
{
   ( void )name;
   ( void )names;

   // [<start get_transient_replacement_field_names>]
   // [<finish get_transient_replacement_field_names>]
}

void Meta_List::do_generate_sql( generate_sql_type type,
 vector< string >& sql_stmts, set< string >& tx_key_info, vector< string >* p_sql_undo_stmts ) const
{
   generate_sql( static_class_name( ), type, sql_stmts, tx_key_info, p_sql_undo_stmts );
}

const char* Meta_List::static_resolved_module_id( )
{
   return static_module_id( );
}

const char* Meta_List::static_resolved_module_name( )
{
   return static_module_name( );
}

const char* Meta_List::static_lock_class_id( )
{
   return "122100";
}

const char* Meta_List::static_check_class_name( )
{
   return "List";
}

const char* Meta_List::static_persistence_extra( )
{
   return "";
}

bool Meta_List::static_has_derivations( )
{
   return !g_derivations.empty( );
}

void Meta_List::static_get_class_info( class_info_container& class_info )
{
   class_info.push_back( "100.122100" );
}

void Meta_List::static_get_field_info( field_info_container& all_field_info )
{
   all_field_info.push_back( field_info( "301998", "Access_Parent_Modifier", "Meta_Modifier", false, "", "" ) );
   all_field_info.push_back( field_info( "301993", "Access_Permission", "Meta_Permission", false, "", "" ) );
   all_field_info.push_back( field_info( "122103", "Access_Restriction", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "122145", "Allow_Anonymous_Access", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "122119", "Allow_Quick_Link", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "122114", "Allow_Text_Search", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "301992", "Class", "Meta_Class", true, "", "" ) );
   all_field_info.push_back( field_info( "122140", "Create_Only_If_Default_Other", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "301998a", "Create_Parent_Modifier", "Meta_Modifier", false, "", "" ) );
   all_field_info.push_back( field_info( "301994", "Create_Permission", "Meta_Permission", false, "", "" ) );
   all_field_info.push_back( field_info( "122104", "Create_Restriction", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "122151", "Delete_Direction", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "122142", "Destroy_Only_If_Default_Other", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "301998b", "Destroy_Parent_Modifier", "Meta_Modifier", false, "", "" ) );
   all_field_info.push_back( field_info( "301995", "Destroy_Permission", "Meta_Permission", false, "", "" ) );
   all_field_info.push_back( field_info( "122105", "Destroy_Restriction", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "122112", "Direction", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "122141", "Display_Only_If_Default_Other", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "122113", "Display_Row_Limit", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "122139", "Display_Security_Level", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "122152", "Display_Sub_Totals", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "122150", "Display_Totals", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "122149", "File_Links_Always_As_Single", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "122111", "Id", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "122135", "Ignore_Implicit_Ordering", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "122148", "Ignore_State_For_Display", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "122120", "Ignore_Unactionable_Records", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "122138", "Ignore_Uneditable_Parent", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "122124", "Ignore_Username_Filter", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "122147", "Is_Admin", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "122102", "Is_Child", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "122121", "Is_Home", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "122146", "Is_Not_Anonymous", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "122110", "Is_Variation", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "122116", "Limit_Scroll_And_New", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "301990", "Model", "Meta_Model", true, "", "" ) );
   all_field_info.push_back( field_info( "122143", "Multiline_Truncate_For_Print", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "122101", "Name", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "122144", "Number_Multiple_Pages", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "122137", "PDF_Font_Type", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "122136", "PDF_List_Type", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "301997", "Parent_Class", "Meta_Class", false, "", "" ) );
   all_field_info.push_back( field_info( "301996", "Parent_Field", "Meta_Field", false, "", "" ) );
   all_field_info.push_back( field_info( "122123", "Print_Restriction", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "122118", "Print_Without_Highlight", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "122117", "Search_Option_Limit", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "122122", "Sort_Rows_In_UI", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "122107", "Style", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "122115", "Text_Match_Highlight", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "122106", "Title", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "301991", "Type", "Meta_List_Type", true, "", "" ) );
   all_field_info.push_back( field_info( "122109", "Variation_Name", "string", false, "", "" ) );
}

void Meta_List::static_get_foreign_key_info( foreign_key_info_container& foreign_key_info )
{
   ( void )foreign_key_info;

   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Access_Parent_Modifier, make_pair( "Meta.122100", "Meta_Modifier" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Access_Permission, make_pair( "Meta.122100", "Meta_Permission" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Class, make_pair( "Meta.122100", "Meta_Class" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Create_Parent_Modifier, make_pair( "Meta.122100", "Meta_Modifier" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Create_Permission, make_pair( "Meta.122100", "Meta_Permission" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Destroy_Parent_Modifier, make_pair( "Meta.122100", "Meta_Modifier" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Destroy_Permission, make_pair( "Meta.122100", "Meta_Permission" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Model, make_pair( "Meta.122100", "Meta_Model" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Parent_Class, make_pair( "Meta.122100", "Meta_Class" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Parent_Field, make_pair( "Meta.122100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Type, make_pair( "Meta.122100", "Meta_List_Type" ) ) );
}

int Meta_List::static_get_num_fields( bool* p_done, const string* p_class_name )
{
   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;

   return c_num_fields;
}

bool Meta_List::static_is_field_encrypted( field_id id )
{
   return is_encrypted_field( static_get_field_id( id ) );
}

bool Meta_List::static_is_field_transient( field_id id )
{
   return is_transient_field( static_get_field_id( id ) );
}

const char* Meta_List::static_get_field_id( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "301998";
      break;

      case 2:
      p_id = "301993";
      break;

      case 3:
      p_id = "122103";
      break;

      case 4:
      p_id = "122145";
      break;

      case 5:
      p_id = "122119";
      break;

      case 6:
      p_id = "122114";
      break;

      case 7:
      p_id = "301992";
      break;

      case 8:
      p_id = "122140";
      break;

      case 9:
      p_id = "301998a";
      break;

      case 10:
      p_id = "301994";
      break;

      case 11:
      p_id = "122104";
      break;

      case 12:
      p_id = "122151";
      break;

      case 13:
      p_id = "122142";
      break;

      case 14:
      p_id = "301998b";
      break;

      case 15:
      p_id = "301995";
      break;

      case 16:
      p_id = "122105";
      break;

      case 17:
      p_id = "122112";
      break;

      case 18:
      p_id = "122141";
      break;

      case 19:
      p_id = "122113";
      break;

      case 20:
      p_id = "122139";
      break;

      case 21:
      p_id = "122152";
      break;

      case 22:
      p_id = "122150";
      break;

      case 23:
      p_id = "122149";
      break;

      case 24:
      p_id = "122111";
      break;

      case 25:
      p_id = "122135";
      break;

      case 26:
      p_id = "122148";
      break;

      case 27:
      p_id = "122120";
      break;

      case 28:
      p_id = "122138";
      break;

      case 29:
      p_id = "122124";
      break;

      case 30:
      p_id = "122147";
      break;

      case 31:
      p_id = "122102";
      break;

      case 32:
      p_id = "122121";
      break;

      case 33:
      p_id = "122146";
      break;

      case 34:
      p_id = "122110";
      break;

      case 35:
      p_id = "122116";
      break;

      case 36:
      p_id = "301990";
      break;

      case 37:
      p_id = "122143";
      break;

      case 38:
      p_id = "122101";
      break;

      case 39:
      p_id = "122144";
      break;

      case 40:
      p_id = "122137";
      break;

      case 41:
      p_id = "122136";
      break;

      case 42:
      p_id = "301997";
      break;

      case 43:
      p_id = "301996";
      break;

      case 44:
      p_id = "122123";
      break;

      case 45:
      p_id = "122118";
      break;

      case 46:
      p_id = "122117";
      break;

      case 47:
      p_id = "122122";
      break;

      case 48:
      p_id = "122107";
      break;

      case 49:
      p_id = "122115";
      break;

      case 50:
      p_id = "122106";
      break;

      case 51:
      p_id = "301991";
      break;

      case 52:
      p_id = "122109";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class List" );

   return p_id;
}

const char* Meta_List::static_get_field_name( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "Access_Parent_Modifier";
      break;

      case 2:
      p_id = "Access_Permission";
      break;

      case 3:
      p_id = "Access_Restriction";
      break;

      case 4:
      p_id = "Allow_Anonymous_Access";
      break;

      case 5:
      p_id = "Allow_Quick_Link";
      break;

      case 6:
      p_id = "Allow_Text_Search";
      break;

      case 7:
      p_id = "Class";
      break;

      case 8:
      p_id = "Create_Only_If_Default_Other";
      break;

      case 9:
      p_id = "Create_Parent_Modifier";
      break;

      case 10:
      p_id = "Create_Permission";
      break;

      case 11:
      p_id = "Create_Restriction";
      break;

      case 12:
      p_id = "Delete_Direction";
      break;

      case 13:
      p_id = "Destroy_Only_If_Default_Other";
      break;

      case 14:
      p_id = "Destroy_Parent_Modifier";
      break;

      case 15:
      p_id = "Destroy_Permission";
      break;

      case 16:
      p_id = "Destroy_Restriction";
      break;

      case 17:
      p_id = "Direction";
      break;

      case 18:
      p_id = "Display_Only_If_Default_Other";
      break;

      case 19:
      p_id = "Display_Row_Limit";
      break;

      case 20:
      p_id = "Display_Security_Level";
      break;

      case 21:
      p_id = "Display_Sub_Totals";
      break;

      case 22:
      p_id = "Display_Totals";
      break;

      case 23:
      p_id = "File_Links_Always_As_Single";
      break;

      case 24:
      p_id = "Id";
      break;

      case 25:
      p_id = "Ignore_Implicit_Ordering";
      break;

      case 26:
      p_id = "Ignore_State_For_Display";
      break;

      case 27:
      p_id = "Ignore_Unactionable_Records";
      break;

      case 28:
      p_id = "Ignore_Uneditable_Parent";
      break;

      case 29:
      p_id = "Ignore_Username_Filter";
      break;

      case 30:
      p_id = "Is_Admin";
      break;

      case 31:
      p_id = "Is_Child";
      break;

      case 32:
      p_id = "Is_Home";
      break;

      case 33:
      p_id = "Is_Not_Anonymous";
      break;

      case 34:
      p_id = "Is_Variation";
      break;

      case 35:
      p_id = "Limit_Scroll_And_New";
      break;

      case 36:
      p_id = "Model";
      break;

      case 37:
      p_id = "Multiline_Truncate_For_Print";
      break;

      case 38:
      p_id = "Name";
      break;

      case 39:
      p_id = "Number_Multiple_Pages";
      break;

      case 40:
      p_id = "PDF_Font_Type";
      break;

      case 41:
      p_id = "PDF_List_Type";
      break;

      case 42:
      p_id = "Parent_Class";
      break;

      case 43:
      p_id = "Parent_Field";
      break;

      case 44:
      p_id = "Print_Restriction";
      break;

      case 45:
      p_id = "Print_Without_Highlight";
      break;

      case 46:
      p_id = "Search_Option_Limit";
      break;

      case 47:
      p_id = "Sort_Rows_In_UI";
      break;

      case 48:
      p_id = "Style";
      break;

      case 49:
      p_id = "Text_Match_Highlight";
      break;

      case 50:
      p_id = "Title";
      break;

      case 51:
      p_id = "Type";
      break;

      case 52:
      p_id = "Variation_Name";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class List" );

   return p_id;
}

int Meta_List::static_get_field_num( const string& field )
{
   int rc = 0;

   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for static_get_field_num( )" );
   else if( ( field == c_field_id_Access_Parent_Modifier ) || ( field == c_field_name_Access_Parent_Modifier ) )
      rc += 1;
   else if( ( field == c_field_id_Access_Permission ) || ( field == c_field_name_Access_Permission ) )
      rc += 2;
   else if( ( field == c_field_id_Access_Restriction ) || ( field == c_field_name_Access_Restriction ) )
      rc += 3;
   else if( ( field == c_field_id_Allow_Anonymous_Access ) || ( field == c_field_name_Allow_Anonymous_Access ) )
      rc += 4;
   else if( ( field == c_field_id_Allow_Quick_Link ) || ( field == c_field_name_Allow_Quick_Link ) )
      rc += 5;
   else if( ( field == c_field_id_Allow_Text_Search ) || ( field == c_field_name_Allow_Text_Search ) )
      rc += 6;
   else if( ( field == c_field_id_Class ) || ( field == c_field_name_Class ) )
      rc += 7;
   else if( ( field == c_field_id_Create_Only_If_Default_Other ) || ( field == c_field_name_Create_Only_If_Default_Other ) )
      rc += 8;
   else if( ( field == c_field_id_Create_Parent_Modifier ) || ( field == c_field_name_Create_Parent_Modifier ) )
      rc += 9;
   else if( ( field == c_field_id_Create_Permission ) || ( field == c_field_name_Create_Permission ) )
      rc += 10;
   else if( ( field == c_field_id_Create_Restriction ) || ( field == c_field_name_Create_Restriction ) )
      rc += 11;
   else if( ( field == c_field_id_Delete_Direction ) || ( field == c_field_name_Delete_Direction ) )
      rc += 12;
   else if( ( field == c_field_id_Destroy_Only_If_Default_Other ) || ( field == c_field_name_Destroy_Only_If_Default_Other ) )
      rc += 13;
   else if( ( field == c_field_id_Destroy_Parent_Modifier ) || ( field == c_field_name_Destroy_Parent_Modifier ) )
      rc += 14;
   else if( ( field == c_field_id_Destroy_Permission ) || ( field == c_field_name_Destroy_Permission ) )
      rc += 15;
   else if( ( field == c_field_id_Destroy_Restriction ) || ( field == c_field_name_Destroy_Restriction ) )
      rc += 16;
   else if( ( field == c_field_id_Direction ) || ( field == c_field_name_Direction ) )
      rc += 17;
   else if( ( field == c_field_id_Display_Only_If_Default_Other ) || ( field == c_field_name_Display_Only_If_Default_Other ) )
      rc += 18;
   else if( ( field == c_field_id_Display_Row_Limit ) || ( field == c_field_name_Display_Row_Limit ) )
      rc += 19;
   else if( ( field == c_field_id_Display_Security_Level ) || ( field == c_field_name_Display_Security_Level ) )
      rc += 20;
   else if( ( field == c_field_id_Display_Sub_Totals ) || ( field == c_field_name_Display_Sub_Totals ) )
      rc += 21;
   else if( ( field == c_field_id_Display_Totals ) || ( field == c_field_name_Display_Totals ) )
      rc += 22;
   else if( ( field == c_field_id_File_Links_Always_As_Single ) || ( field == c_field_name_File_Links_Always_As_Single ) )
      rc += 23;
   else if( ( field == c_field_id_Id ) || ( field == c_field_name_Id ) )
      rc += 24;
   else if( ( field == c_field_id_Ignore_Implicit_Ordering ) || ( field == c_field_name_Ignore_Implicit_Ordering ) )
      rc += 25;
   else if( ( field == c_field_id_Ignore_State_For_Display ) || ( field == c_field_name_Ignore_State_For_Display ) )
      rc += 26;
   else if( ( field == c_field_id_Ignore_Unactionable_Records ) || ( field == c_field_name_Ignore_Unactionable_Records ) )
      rc += 27;
   else if( ( field == c_field_id_Ignore_Uneditable_Parent ) || ( field == c_field_name_Ignore_Uneditable_Parent ) )
      rc += 28;
   else if( ( field == c_field_id_Ignore_Username_Filter ) || ( field == c_field_name_Ignore_Username_Filter ) )
      rc += 29;
   else if( ( field == c_field_id_Is_Admin ) || ( field == c_field_name_Is_Admin ) )
      rc += 30;
   else if( ( field == c_field_id_Is_Child ) || ( field == c_field_name_Is_Child ) )
      rc += 31;
   else if( ( field == c_field_id_Is_Home ) || ( field == c_field_name_Is_Home ) )
      rc += 32;
   else if( ( field == c_field_id_Is_Not_Anonymous ) || ( field == c_field_name_Is_Not_Anonymous ) )
      rc += 33;
   else if( ( field == c_field_id_Is_Variation ) || ( field == c_field_name_Is_Variation ) )
      rc += 34;
   else if( ( field == c_field_id_Limit_Scroll_And_New ) || ( field == c_field_name_Limit_Scroll_And_New ) )
      rc += 35;
   else if( ( field == c_field_id_Model ) || ( field == c_field_name_Model ) )
      rc += 36;
   else if( ( field == c_field_id_Multiline_Truncate_For_Print ) || ( field == c_field_name_Multiline_Truncate_For_Print ) )
      rc += 37;
   else if( ( field == c_field_id_Name ) || ( field == c_field_name_Name ) )
      rc += 38;
   else if( ( field == c_field_id_Number_Multiple_Pages ) || ( field == c_field_name_Number_Multiple_Pages ) )
      rc += 39;
   else if( ( field == c_field_id_PDF_Font_Type ) || ( field == c_field_name_PDF_Font_Type ) )
      rc += 40;
   else if( ( field == c_field_id_PDF_List_Type ) || ( field == c_field_name_PDF_List_Type ) )
      rc += 41;
   else if( ( field == c_field_id_Parent_Class ) || ( field == c_field_name_Parent_Class ) )
      rc += 42;
   else if( ( field == c_field_id_Parent_Field ) || ( field == c_field_name_Parent_Field ) )
      rc += 43;
   else if( ( field == c_field_id_Print_Restriction ) || ( field == c_field_name_Print_Restriction ) )
      rc += 44;
   else if( ( field == c_field_id_Print_Without_Highlight ) || ( field == c_field_name_Print_Without_Highlight ) )
      rc += 45;
   else if( ( field == c_field_id_Search_Option_Limit ) || ( field == c_field_name_Search_Option_Limit ) )
      rc += 46;
   else if( ( field == c_field_id_Sort_Rows_In_UI ) || ( field == c_field_name_Sort_Rows_In_UI ) )
      rc += 47;
   else if( ( field == c_field_id_Style ) || ( field == c_field_name_Style ) )
      rc += 48;
   else if( ( field == c_field_id_Text_Match_Highlight ) || ( field == c_field_name_Text_Match_Highlight ) )
      rc += 49;
   else if( ( field == c_field_id_Title ) || ( field == c_field_name_Title ) )
      rc += 50;
   else if( ( field == c_field_id_Type ) || ( field == c_field_name_Type ) )
      rc += 51;
   else if( ( field == c_field_id_Variation_Name ) || ( field == c_field_name_Variation_Name ) )
      rc += 52;

   return rc - 1;
}

procedure_info_container& Meta_List::static_get_procedure_info( )
{
   static bool initialised = false;
   static procedure_info_container procedures;

   if( !initialised )
   {
      initialised = true;
      procedures.insert( make_pair( "122430", procedure_info( "Generate_PDF_List" ) ) );
   }

   return procedures;
}

string Meta_List::static_get_sql_columns( )
{
   string sql_columns;

   sql_columns += 
    "C_Key_ VARCHAR(75),"
    "C_Ver_ SMALLINT UNSIGNED NOT NULL,"
    "C_Rev_ BIGINT UNSIGNED NOT NULL,"
    "C_Sec_ BIGINT UNSIGNED NOT NULL,"
    "C_Typ_ VARCHAR(16) NOT NULL,"
    "C_Access_Parent_Modifier VARCHAR(75) NOT NULL,"
    "C_Access_Permission VARCHAR(75) NOT NULL,"
    "C_Access_Restriction INTEGER NOT NULL,"
    "C_Allow_Anonymous_Access INTEGER NOT NULL,"
    "C_Allow_Quick_Link INTEGER NOT NULL,"
    "C_Allow_Text_Search INTEGER NOT NULL,"
    "C_Class VARCHAR(75) NOT NULL,"
    "C_Create_Only_If_Default_Other INTEGER NOT NULL,"
    "C_Create_Parent_Modifier VARCHAR(75) NOT NULL,"
    "C_Create_Permission VARCHAR(75) NOT NULL,"
    "C_Create_Restriction INTEGER NOT NULL,"
    "C_Delete_Direction INTEGER NOT NULL,"
    "C_Destroy_Only_If_Default_Other INTEGER NOT NULL,"
    "C_Destroy_Parent_Modifier VARCHAR(75) NOT NULL,"
    "C_Destroy_Permission VARCHAR(75) NOT NULL,"
    "C_Destroy_Restriction INTEGER NOT NULL,"
    "C_Direction INTEGER NOT NULL,"
    "C_Display_Only_If_Default_Other INTEGER NOT NULL,"
    "C_Display_Row_Limit INTEGER NOT NULL,"
    "C_Display_Security_Level INTEGER NOT NULL,"
    "C_Display_Sub_Totals INTEGER NOT NULL,"
    "C_Display_Totals INTEGER NOT NULL,"
    "C_File_Links_Always_As_Single INTEGER NOT NULL,"
    "C_Id VARCHAR(200) NOT NULL,"
    "C_Ignore_Implicit_Ordering INTEGER NOT NULL,"
    "C_Ignore_State_For_Display INTEGER NOT NULL,"
    "C_Ignore_Unactionable_Records INTEGER NOT NULL,"
    "C_Ignore_Uneditable_Parent INTEGER NOT NULL,"
    "C_Ignore_Username_Filter INTEGER NOT NULL,"
    "C_Is_Admin INTEGER NOT NULL,"
    "C_Is_Child INTEGER NOT NULL,"
    "C_Is_Home INTEGER NOT NULL,"
    "C_Is_Not_Anonymous INTEGER NOT NULL,"
    "C_Is_Variation INTEGER NOT NULL,"
    "C_Limit_Scroll_And_New INTEGER NOT NULL,"
    "C_Model VARCHAR(75) NOT NULL,"
    "C_Multiline_Truncate_For_Print INTEGER NOT NULL,"
    "C_Number_Multiple_Pages INTEGER NOT NULL,"
    "C_PDF_Font_Type INTEGER NOT NULL,"
    "C_PDF_List_Type INTEGER NOT NULL,"
    "C_Parent_Class VARCHAR(75) NOT NULL,"
    "C_Parent_Field VARCHAR(75) NOT NULL,"
    "C_Print_Restriction INTEGER NOT NULL,"
    "C_Print_Without_Highlight INTEGER NOT NULL,"
    "C_Search_Option_Limit INTEGER NOT NULL,"
    "C_Sort_Rows_In_UI INTEGER NOT NULL,"
    "C_Style INTEGER NOT NULL,"
    "C_Text_Match_Highlight INTEGER NOT NULL,"
    "C_Title VARCHAR(256) NOT NULL,"
    "C_Type VARCHAR(256) NOT NULL,"
    "C_Variation_Name VARCHAR(256) NOT NULL,"
    "PRIMARY KEY(C_Key_)";

   return sql_columns;
}

void Meta_List::static_get_text_search_fields( vector< string >& fields )
{
   ( void )fields;
}

void Meta_List::static_get_all_enum_pairs( vector< pair< string, string > >& pairs )
{
   pairs.push_back( make_pair( "enum_list_restrict_0", get_enum_string_list_restrict( 0 ) ) );
   pairs.push_back( make_pair( "enum_list_restrict_1", get_enum_string_list_restrict( 1 ) ) );
   pairs.push_back( make_pair( "enum_list_restrict_2", get_enum_string_list_restrict( 2 ) ) );
   pairs.push_back( make_pair( "enum_list_restrict_3", get_enum_string_list_restrict( 3 ) ) );
   pairs.push_back( make_pair( "enum_list_restrict_4", get_enum_string_list_restrict( 4 ) ) );

   pairs.push_back( make_pair( "enum_list_direction_0", get_enum_string_list_direction( 0 ) ) );
   pairs.push_back( make_pair( "enum_list_direction_1", get_enum_string_list_direction( 1 ) ) );

   pairs.push_back( make_pair( "enum_list_display_row_limit_0", get_enum_string_list_display_row_limit( 0 ) ) );
   pairs.push_back( make_pair( "enum_list_display_row_limit_1", get_enum_string_list_display_row_limit( 1 ) ) );
   pairs.push_back( make_pair( "enum_list_display_row_limit_2", get_enum_string_list_display_row_limit( 2 ) ) );
   pairs.push_back( make_pair( "enum_list_display_row_limit_3", get_enum_string_list_display_row_limit( 3 ) ) );
   pairs.push_back( make_pair( "enum_list_display_row_limit_4", get_enum_string_list_display_row_limit( 4 ) ) );
   pairs.push_back( make_pair( "enum_list_display_row_limit_5", get_enum_string_list_display_row_limit( 5 ) ) );
   pairs.push_back( make_pair( "enum_list_display_row_limit_6", get_enum_string_list_display_row_limit( 6 ) ) );
   pairs.push_back( make_pair( "enum_list_display_row_limit_7", get_enum_string_list_display_row_limit( 7 ) ) );
   pairs.push_back( make_pair( "enum_list_display_row_limit_8", get_enum_string_list_display_row_limit( 8 ) ) );
   pairs.push_back( make_pair( "enum_list_display_row_limit_9", get_enum_string_list_display_row_limit( 9 ) ) );
   pairs.push_back( make_pair( "enum_list_display_row_limit_10", get_enum_string_list_display_row_limit( 10 ) ) );
   pairs.push_back( make_pair( "enum_list_display_row_limit_12", get_enum_string_list_display_row_limit( 12 ) ) );
   pairs.push_back( make_pair( "enum_list_display_row_limit_15", get_enum_string_list_display_row_limit( 15 ) ) );
   pairs.push_back( make_pair( "enum_list_display_row_limit_20", get_enum_string_list_display_row_limit( 20 ) ) );
   pairs.push_back( make_pair( "enum_list_display_row_limit_25", get_enum_string_list_display_row_limit( 25 ) ) );
   pairs.push_back( make_pair( "enum_list_display_row_limit_30", get_enum_string_list_display_row_limit( 30 ) ) );
   pairs.push_back( make_pair( "enum_list_display_row_limit_40", get_enum_string_list_display_row_limit( 40 ) ) );
   pairs.push_back( make_pair( "enum_list_display_row_limit_50", get_enum_string_list_display_row_limit( 50 ) ) );
   pairs.push_back( make_pair( "enum_list_display_row_limit_75", get_enum_string_list_display_row_limit( 75 ) ) );
   pairs.push_back( make_pair( "enum_list_display_row_limit_100", get_enum_string_list_display_row_limit( 100 ) ) );
   pairs.push_back( make_pair( "enum_list_display_row_limit_200", get_enum_string_list_display_row_limit( 200 ) ) );
   pairs.push_back( make_pair( "enum_list_display_row_limit_500", get_enum_string_list_display_row_limit( 500 ) ) );
   pairs.push_back( make_pair( "enum_list_display_row_limit_999", get_enum_string_list_display_row_limit( 999 ) ) );

   pairs.push_back( make_pair( "enum_pdf_font_type_0", get_enum_string_pdf_font_type( 0 ) ) );
   pairs.push_back( make_pair( "enum_pdf_font_type_1", get_enum_string_pdf_font_type( 1 ) ) );
   pairs.push_back( make_pair( "enum_pdf_font_type_20", get_enum_string_pdf_font_type( 20 ) ) );
   pairs.push_back( make_pair( "enum_pdf_font_type_21", get_enum_string_pdf_font_type( 21 ) ) );

   pairs.push_back( make_pair( "enum_list_pdf_list_type_0", get_enum_string_list_pdf_list_type( 0 ) ) );
   pairs.push_back( make_pair( "enum_list_pdf_list_type_1", get_enum_string_list_pdf_list_type( 1 ) ) );
   pairs.push_back( make_pair( "enum_list_pdf_list_type_2", get_enum_string_list_pdf_list_type( 2 ) ) );
   pairs.push_back( make_pair( "enum_list_pdf_list_type_99", get_enum_string_list_pdf_list_type( 99 ) ) );

   pairs.push_back( make_pair( "enum_list_print_restrict_0", get_enum_string_list_print_restrict( 0 ) ) );
   pairs.push_back( make_pair( "enum_list_print_restrict_1", get_enum_string_list_print_restrict( 1 ) ) );
   pairs.push_back( make_pair( "enum_list_print_restrict_2", get_enum_string_list_print_restrict( 2 ) ) );

   pairs.push_back( make_pair( "enum_list_search_opt_limit_0", get_enum_string_list_search_opt_limit( 0 ) ) );
   pairs.push_back( make_pair( "enum_list_search_opt_limit_1", get_enum_string_list_search_opt_limit( 1 ) ) );
   pairs.push_back( make_pair( "enum_list_search_opt_limit_2", get_enum_string_list_search_opt_limit( 2 ) ) );
   pairs.push_back( make_pair( "enum_list_search_opt_limit_3", get_enum_string_list_search_opt_limit( 3 ) ) );
   pairs.push_back( make_pair( "enum_list_search_opt_limit_4", get_enum_string_list_search_opt_limit( 4 ) ) );
   pairs.push_back( make_pair( "enum_list_search_opt_limit_5", get_enum_string_list_search_opt_limit( 5 ) ) );
   pairs.push_back( make_pair( "enum_list_search_opt_limit_6", get_enum_string_list_search_opt_limit( 6 ) ) );
   pairs.push_back( make_pair( "enum_list_search_opt_limit_7", get_enum_string_list_search_opt_limit( 7 ) ) );

   pairs.push_back( make_pair( "enum_list_style_0", get_enum_string_list_style( 0 ) ) );
   pairs.push_back( make_pair( "enum_list_style_1", get_enum_string_list_style( 1 ) ) );

   pairs.push_back( make_pair( "enum_list_text_match_highlight_0", get_enum_string_list_text_match_highlight( 0 ) ) );
   pairs.push_back( make_pair( "enum_list_text_match_highlight_1", get_enum_string_list_text_match_highlight( 1 ) ) );
   pairs.push_back( make_pair( "enum_list_text_match_highlight_2", get_enum_string_list_text_match_highlight( 2 ) ) );
}

void Meta_List::static_get_all_index_pairs( vector< pair< string, string > >& pairs )
{
   pairs.push_back( make_pair( "Model,Id", "string,string" ) );
}

void Meta_List::static_get_all_unique_indexes( vector< string >& unique_indexes )
{
   unique_indexes.push_back( "Model,Id" );
}

void Meta_List::static_get_sql_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_Model,C_Id" );
}

void Meta_List::static_get_sql_unique_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_Model,C_Id" );
}

void Meta_List::static_insert_derivation( const string& module_and_class_id )
{
   g_derivations.insert( module_and_class_id );
}

void Meta_List::static_remove_derivation( const string& module_and_class_id )
{
   if( g_derivations.count( module_and_class_id ) )
      g_derivations.erase( module_and_class_id );
}

void Meta_List::static_insert_external_alias( const string& module_and_class_id, Meta_List* p_instance )
{
   g_external_aliases.insert( external_aliases_value_type( module_and_class_id, p_instance ) );
}

void Meta_List::static_remove_external_alias( const string& module_and_class_id )
{
   if( g_external_aliases.count( module_and_class_id ) )
   {
      delete g_external_aliases[ module_and_class_id ];
      g_external_aliases.erase( module_and_class_id );
   }
}

void Meta_List::static_class_init( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for init" );

   g_state_names_variable = get_special_var_name( e_special_var_state_names );

   g_list_restrict_enum.insert( 0 );
   g_list_restrict_enum.insert( 1 );
   g_list_restrict_enum.insert( 2 );
   g_list_restrict_enum.insert( 3 );
   g_list_restrict_enum.insert( 4 );

   g_list_direction_enum.insert( 0 );
   g_list_direction_enum.insert( 1 );

   g_list_display_row_limit_enum.insert( 0 );
   g_list_display_row_limit_enum.insert( 1 );
   g_list_display_row_limit_enum.insert( 2 );
   g_list_display_row_limit_enum.insert( 3 );
   g_list_display_row_limit_enum.insert( 4 );
   g_list_display_row_limit_enum.insert( 5 );
   g_list_display_row_limit_enum.insert( 6 );
   g_list_display_row_limit_enum.insert( 7 );
   g_list_display_row_limit_enum.insert( 8 );
   g_list_display_row_limit_enum.insert( 9 );
   g_list_display_row_limit_enum.insert( 10 );
   g_list_display_row_limit_enum.insert( 12 );
   g_list_display_row_limit_enum.insert( 15 );
   g_list_display_row_limit_enum.insert( 20 );
   g_list_display_row_limit_enum.insert( 25 );
   g_list_display_row_limit_enum.insert( 30 );
   g_list_display_row_limit_enum.insert( 40 );
   g_list_display_row_limit_enum.insert( 50 );
   g_list_display_row_limit_enum.insert( 75 );
   g_list_display_row_limit_enum.insert( 100 );
   g_list_display_row_limit_enum.insert( 200 );
   g_list_display_row_limit_enum.insert( 500 );
   g_list_display_row_limit_enum.insert( 999 );

   g_pdf_font_type_enum.insert( 0 );
   g_pdf_font_type_enum.insert( 1 );
   g_pdf_font_type_enum.insert( 20 );
   g_pdf_font_type_enum.insert( 21 );

   g_list_pdf_list_type_enum.insert( 0 );
   g_list_pdf_list_type_enum.insert( 1 );
   g_list_pdf_list_type_enum.insert( 2 );
   g_list_pdf_list_type_enum.insert( 99 );

   g_list_print_restrict_enum.insert( 0 );
   g_list_print_restrict_enum.insert( 1 );
   g_list_print_restrict_enum.insert( 2 );

   g_list_search_opt_limit_enum.insert( 0 );
   g_list_search_opt_limit_enum.insert( 1 );
   g_list_search_opt_limit_enum.insert( 2 );
   g_list_search_opt_limit_enum.insert( 3 );
   g_list_search_opt_limit_enum.insert( 4 );
   g_list_search_opt_limit_enum.insert( 5 );
   g_list_search_opt_limit_enum.insert( 6 );
   g_list_search_opt_limit_enum.insert( 7 );

   g_list_style_enum.insert( 0 );
   g_list_style_enum.insert( 1 );

   g_list_text_match_highlight_enum.insert( 0 );
   g_list_text_match_highlight_enum.insert( 1 );
   g_list_text_match_highlight_enum.insert( 2 );

   // [<start static_class_init>]
   // [<finish static_class_init>]
}

void Meta_List::static_class_term( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for term" );

   // [<start static_class_term>]
   // [<finish static_class_term>]
}
