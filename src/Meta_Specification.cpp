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

#include "Meta_Specification.h"

#include "Meta_List_Field.h"
#include "Meta_View_Field.h"
#include "Meta_Class.h"
#include "Meta_Relationship.h"
#include "Meta_Specification_Type.h"
#include "Meta_Enum.h"
#include "Meta_Enum_Item.h"
#include "Meta_Field.h"
#include "Meta_Model.h"
#include "Meta_Modifier.h"
#include "Meta_Permission.h"
#include "Meta_Procedure.h"
#include "Meta_Specification.h"
#include "Meta_Procedure_Arg.h"

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

// [(start copy_child_links_from_fk)] 600170
#include "Meta_Specification_Type.h"
// [(finish copy_child_links_from_fk)] 600170

// [<start includes>]
// [<finish includes>]

using namespace std;

// [<start namespaces>]
// [<finish namespaces>]

template< > inline string to_string( const Meta_Class& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_Relationship& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_Specification_Type& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_Enum& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_Enum_Item& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_Field& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_Model& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_Modifier& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_Permission& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_Procedure& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_Specification& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_Procedure_Arg& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

inline void from_string( Meta_Class& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_Relationship& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_Specification_Type& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_Enum& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_Enum_Item& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_Field& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_Model& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_Modifier& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_Permission& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_Procedure& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_Specification& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_Procedure_Arg& c, const string& s )
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

#include "Meta_Specification.cmh"

const int32_t c_version = 1;

const char* const c_field_id_Actions = "115108";
const char* const c_field_id_All_Strings = "115124";
const char* const c_field_id_All_Vars = "115123";
const char* const c_field_id_Can_Access_Below_Model = "115133";
const char* const c_field_id_Child_Rel_Child_Class = "301491";
const char* const c_field_id_Child_Rel_Field_Key = "115121";
const char* const c_field_id_Child_Relationship = "301475";
const char* const c_field_id_Child_Specification_Type = "301406";
const char* const c_field_id_Class = "301420";
const char* const c_field_id_Comments = "115112";
const char* const c_field_id_Enum = "301460";
const char* const c_field_id_Enum_Item = "301470";
const char* const c_field_id_Enum_Item_2 = "301471";
const char* const c_field_id_Enum_Item_3 = "301472";
const char* const c_field_id_Enum_Item_4 = "301473";
const char* const c_field_id_Enum_Item_5 = "301474";
const char* const c_field_id_Field = "301430";
const char* const c_field_id_Field_Class = "301492";
const char* const c_field_id_Field_Pairs = "115120";
const char* const c_field_id_Field_Values = "115113";
const char* const c_field_id_Fields = "115126";
const char* const c_field_id_Id = "115117";
const char* const c_field_id_Is_Required_For_UI_Gen = "115128";
const char* const c_field_id_Model = "301400";
const char* const c_field_id_Modifier = "301440";
const char* const c_field_id_Name = "115101";
const char* const c_field_id_Options = "115104";
const char* const c_field_id_Order = "115105";
const char* const c_field_id_Other_Class = "301421";
const char* const c_field_id_Other_Class_Field = "301437";
const char* const c_field_id_Other_Field = "301435";
const char* const c_field_id_Other_Field_2 = "301436";
const char* const c_field_id_Other_Modifier = "301441";
const char* const c_field_id_Other_Modifier_2 = "301442";
const char* const c_field_id_Other_Permission = "301331";
const char* const c_field_id_Other_Permission_2 = "301332";
const char* const c_field_id_Other_Procedure = "301451";
const char* const c_field_id_Other_Procedure_2 = "301452";
const char* const c_field_id_Other_Source_Child = "301496";
const char* const c_field_id_Other_Source_Child_2 = "301497";
const char* const c_field_id_Parent_Specification = "301405";
const char* const c_field_id_Permission = "301499";
const char* const c_field_id_Procedure = "301450";
const char* const c_field_id_Procedure_Arg = "301498";
const char* const c_field_id_Procedure_Arg_2 = "301477";
const char* const c_field_id_Procedure_Arg_3 = "301478";
const char* const c_field_id_Protect_Child_Rel = "115122";
const char* const c_field_id_Protect_Procedure = "115115";
const char* const c_field_id_Protect_Source_Parent = "115111";
const char* const c_field_id_Restrict_Values = "115118";
const char* const c_field_id_Source_Child = "301495";
const char* const c_field_id_Source_Child_Class = "301489";
const char* const c_field_id_Source_Class = "301494";
const char* const c_field_id_Source_Field = "301481";
const char* const c_field_id_Source_Field_Class = "301482";
const char* const c_field_id_Source_Grandchild = "301476";
const char* const c_field_id_Source_Parent = "301480";
const char* const c_field_id_Source_Parent_Class = "301490";
const char* const c_field_id_Specification_Type = "301410";
const char* const c_field_id_Strings = "115119";
const char* const c_field_id_Test_Child = "301488";
const char* const c_field_id_Test_Field = "301485";
const char* const c_field_id_Test_Field_Class = "301493";
const char* const c_field_id_Test_Parent = "301486";
const char* const c_field_id_Test_Parent_Class = "301487";
const char* const c_field_id_Test_Value = "115103";
const char* const c_field_id_Test_Value_Label = "115132";
const char* const c_field_id_Use_Source_Parent = "115107";
const char* const c_field_id_Use_Test_Parent_Child = "115110";
const char* const c_field_id_Value = "115102";
const char* const c_field_id_Value_Label = "115131";
const char* const c_field_id_Value_Left_Part = "115129";
const char* const c_field_id_Value_Literal = "115116";
const char* const c_field_id_Value_Numeric_String = "115127";
const char* const c_field_id_Value_Right_Part = "115130";
const char* const c_field_id_Value_String = "115125";
const char* const c_field_id_Vars = "115114";

const char* const c_field_name_Actions = "Actions";
const char* const c_field_name_All_Strings = "All_Strings";
const char* const c_field_name_All_Vars = "All_Vars";
const char* const c_field_name_Can_Access_Below_Model = "Can_Access_Below_Model";
const char* const c_field_name_Child_Rel_Child_Class = "Child_Rel_Child_Class";
const char* const c_field_name_Child_Rel_Field_Key = "Child_Rel_Field_Key";
const char* const c_field_name_Child_Relationship = "Child_Relationship";
const char* const c_field_name_Child_Specification_Type = "Child_Specification_Type";
const char* const c_field_name_Class = "Class";
const char* const c_field_name_Comments = "Comments";
const char* const c_field_name_Enum = "Enum";
const char* const c_field_name_Enum_Item = "Enum_Item";
const char* const c_field_name_Enum_Item_2 = "Enum_Item_2";
const char* const c_field_name_Enum_Item_3 = "Enum_Item_3";
const char* const c_field_name_Enum_Item_4 = "Enum_Item_4";
const char* const c_field_name_Enum_Item_5 = "Enum_Item_5";
const char* const c_field_name_Field = "Field";
const char* const c_field_name_Field_Class = "Field_Class";
const char* const c_field_name_Field_Pairs = "Field_Pairs";
const char* const c_field_name_Field_Values = "Field_Values";
const char* const c_field_name_Fields = "Fields";
const char* const c_field_name_Id = "Id";
const char* const c_field_name_Is_Required_For_UI_Gen = "Is_Required_For_UI_Gen";
const char* const c_field_name_Model = "Model";
const char* const c_field_name_Modifier = "Modifier";
const char* const c_field_name_Name = "Name";
const char* const c_field_name_Options = "Options";
const char* const c_field_name_Order = "Order";
const char* const c_field_name_Other_Class = "Other_Class";
const char* const c_field_name_Other_Class_Field = "Other_Class_Field";
const char* const c_field_name_Other_Field = "Other_Field";
const char* const c_field_name_Other_Field_2 = "Other_Field_2";
const char* const c_field_name_Other_Modifier = "Other_Modifier";
const char* const c_field_name_Other_Modifier_2 = "Other_Modifier_2";
const char* const c_field_name_Other_Permission = "Other_Permission";
const char* const c_field_name_Other_Permission_2 = "Other_Permission_2";
const char* const c_field_name_Other_Procedure = "Other_Procedure";
const char* const c_field_name_Other_Procedure_2 = "Other_Procedure_2";
const char* const c_field_name_Other_Source_Child = "Other_Source_Child";
const char* const c_field_name_Other_Source_Child_2 = "Other_Source_Child_2";
const char* const c_field_name_Parent_Specification = "Parent_Specification";
const char* const c_field_name_Permission = "Permission";
const char* const c_field_name_Procedure = "Procedure";
const char* const c_field_name_Procedure_Arg = "Procedure_Arg";
const char* const c_field_name_Procedure_Arg_2 = "Procedure_Arg_2";
const char* const c_field_name_Procedure_Arg_3 = "Procedure_Arg_3";
const char* const c_field_name_Protect_Child_Rel = "Protect_Child_Rel";
const char* const c_field_name_Protect_Procedure = "Protect_Procedure";
const char* const c_field_name_Protect_Source_Parent = "Protect_Source_Parent";
const char* const c_field_name_Restrict_Values = "Restrict_Values";
const char* const c_field_name_Source_Child = "Source_Child";
const char* const c_field_name_Source_Child_Class = "Source_Child_Class";
const char* const c_field_name_Source_Class = "Source_Class";
const char* const c_field_name_Source_Field = "Source_Field";
const char* const c_field_name_Source_Field_Class = "Source_Field_Class";
const char* const c_field_name_Source_Grandchild = "Source_Grandchild";
const char* const c_field_name_Source_Parent = "Source_Parent";
const char* const c_field_name_Source_Parent_Class = "Source_Parent_Class";
const char* const c_field_name_Specification_Type = "Specification_Type";
const char* const c_field_name_Strings = "Strings";
const char* const c_field_name_Test_Child = "Test_Child";
const char* const c_field_name_Test_Field = "Test_Field";
const char* const c_field_name_Test_Field_Class = "Test_Field_Class";
const char* const c_field_name_Test_Parent = "Test_Parent";
const char* const c_field_name_Test_Parent_Class = "Test_Parent_Class";
const char* const c_field_name_Test_Value = "Test_Value";
const char* const c_field_name_Test_Value_Label = "Test_Value_Label";
const char* const c_field_name_Use_Source_Parent = "Use_Source_Parent";
const char* const c_field_name_Use_Test_Parent_Child = "Use_Test_Parent_Child";
const char* const c_field_name_Value = "Value";
const char* const c_field_name_Value_Label = "Value_Label";
const char* const c_field_name_Value_Left_Part = "Value_Left_Part";
const char* const c_field_name_Value_Literal = "Value_Literal";
const char* const c_field_name_Value_Numeric_String = "Value_Numeric_String";
const char* const c_field_name_Value_Right_Part = "Value_Right_Part";
const char* const c_field_name_Value_String = "Value_String";
const char* const c_field_name_Vars = "Vars";

const char* const c_field_display_name_Actions = "field_specification_actions";
const char* const c_field_display_name_All_Strings = "field_specification_all_strings";
const char* const c_field_display_name_All_Vars = "field_specification_all_vars";
const char* const c_field_display_name_Can_Access_Below_Model = "field_specification_can_access_below_model";
const char* const c_field_display_name_Child_Rel_Child_Class = "field_specification_child_rel_child_class";
const char* const c_field_display_name_Child_Rel_Field_Key = "field_specification_child_rel_field_key";
const char* const c_field_display_name_Child_Relationship = "field_specification_child_relationship";
const char* const c_field_display_name_Child_Specification_Type = "field_specification_child_specification_type";
const char* const c_field_display_name_Class = "field_specification_class";
const char* const c_field_display_name_Comments = "field_specification_comments";
const char* const c_field_display_name_Enum = "field_specification_enum";
const char* const c_field_display_name_Enum_Item = "field_specification_enum_item";
const char* const c_field_display_name_Enum_Item_2 = "field_specification_enum_item_2";
const char* const c_field_display_name_Enum_Item_3 = "field_specification_enum_item_3";
const char* const c_field_display_name_Enum_Item_4 = "field_specification_enum_item_4";
const char* const c_field_display_name_Enum_Item_5 = "field_specification_enum_item_5";
const char* const c_field_display_name_Field = "field_specification_field";
const char* const c_field_display_name_Field_Class = "field_specification_field_class";
const char* const c_field_display_name_Field_Pairs = "field_specification_field_pairs";
const char* const c_field_display_name_Field_Values = "field_specification_field_values";
const char* const c_field_display_name_Fields = "field_specification_fields";
const char* const c_field_display_name_Id = "field_specification_id";
const char* const c_field_display_name_Is_Required_For_UI_Gen = "field_specification_is_required_for_ui_gen";
const char* const c_field_display_name_Model = "field_specification_model";
const char* const c_field_display_name_Modifier = "field_specification_modifier";
const char* const c_field_display_name_Name = "field_specification_name";
const char* const c_field_display_name_Options = "field_specification_options";
const char* const c_field_display_name_Order = "field_specification_order";
const char* const c_field_display_name_Other_Class = "field_specification_other_class";
const char* const c_field_display_name_Other_Class_Field = "field_specification_other_class_field";
const char* const c_field_display_name_Other_Field = "field_specification_other_field";
const char* const c_field_display_name_Other_Field_2 = "field_specification_other_field_2";
const char* const c_field_display_name_Other_Modifier = "field_specification_other_modifier";
const char* const c_field_display_name_Other_Modifier_2 = "field_specification_other_modifier_2";
const char* const c_field_display_name_Other_Permission = "field_specification_other_permission";
const char* const c_field_display_name_Other_Permission_2 = "field_specification_other_permission_2";
const char* const c_field_display_name_Other_Procedure = "field_specification_other_procedure";
const char* const c_field_display_name_Other_Procedure_2 = "field_specification_other_procedure_2";
const char* const c_field_display_name_Other_Source_Child = "field_specification_other_source_child";
const char* const c_field_display_name_Other_Source_Child_2 = "field_specification_other_source_child_2";
const char* const c_field_display_name_Parent_Specification = "field_specification_parent_specification";
const char* const c_field_display_name_Permission = "field_specification_permission";
const char* const c_field_display_name_Procedure = "field_specification_procedure";
const char* const c_field_display_name_Procedure_Arg = "field_specification_procedure_arg";
const char* const c_field_display_name_Procedure_Arg_2 = "field_specification_procedure_arg_2";
const char* const c_field_display_name_Procedure_Arg_3 = "field_specification_procedure_arg_3";
const char* const c_field_display_name_Protect_Child_Rel = "field_specification_protect_child_rel";
const char* const c_field_display_name_Protect_Procedure = "field_specification_protect_procedure";
const char* const c_field_display_name_Protect_Source_Parent = "field_specification_protect_source_parent";
const char* const c_field_display_name_Restrict_Values = "field_specification_restrict_values";
const char* const c_field_display_name_Source_Child = "field_specification_source_child";
const char* const c_field_display_name_Source_Child_Class = "field_specification_source_child_class";
const char* const c_field_display_name_Source_Class = "field_specification_source_class";
const char* const c_field_display_name_Source_Field = "field_specification_source_field";
const char* const c_field_display_name_Source_Field_Class = "field_specification_source_field_class";
const char* const c_field_display_name_Source_Grandchild = "field_specification_source_grandchild";
const char* const c_field_display_name_Source_Parent = "field_specification_source_parent";
const char* const c_field_display_name_Source_Parent_Class = "field_specification_source_parent_class";
const char* const c_field_display_name_Specification_Type = "field_specification_specification_type";
const char* const c_field_display_name_Strings = "field_specification_strings";
const char* const c_field_display_name_Test_Child = "field_specification_test_child";
const char* const c_field_display_name_Test_Field = "field_specification_test_field";
const char* const c_field_display_name_Test_Field_Class = "field_specification_test_field_class";
const char* const c_field_display_name_Test_Parent = "field_specification_test_parent";
const char* const c_field_display_name_Test_Parent_Class = "field_specification_test_parent_class";
const char* const c_field_display_name_Test_Value = "field_specification_test_value";
const char* const c_field_display_name_Test_Value_Label = "field_specification_test_value_label";
const char* const c_field_display_name_Use_Source_Parent = "field_specification_use_source_parent";
const char* const c_field_display_name_Use_Test_Parent_Child = "field_specification_use_test_parent_child";
const char* const c_field_display_name_Value = "field_specification_value";
const char* const c_field_display_name_Value_Label = "field_specification_value_label";
const char* const c_field_display_name_Value_Left_Part = "field_specification_value_left_part";
const char* const c_field_display_name_Value_Literal = "field_specification_value_literal";
const char* const c_field_display_name_Value_Numeric_String = "field_specification_value_numeric_string";
const char* const c_field_display_name_Value_Right_Part = "field_specification_value_right_part";
const char* const c_field_display_name_Value_String = "field_specification_value_string";
const char* const c_field_display_name_Vars = "field_specification_vars";

const int c_num_fields = 77;

const char* const c_all_sorted_field_ids[ ] =
{
   "115101",
   "115102",
   "115103",
   "115104",
   "115105",
   "115107",
   "115108",
   "115110",
   "115111",
   "115112",
   "115113",
   "115114",
   "115115",
   "115116",
   "115117",
   "115118",
   "115119",
   "115120",
   "115121",
   "115122",
   "115123",
   "115124",
   "115125",
   "115126",
   "115127",
   "115128",
   "115129",
   "115130",
   "115131",
   "115132",
   "115133",
   "301331",
   "301332",
   "301400",
   "301405",
   "301406",
   "301410",
   "301420",
   "301421",
   "301430",
   "301435",
   "301436",
   "301437",
   "301440",
   "301441",
   "301442",
   "301450",
   "301451",
   "301452",
   "301460",
   "301470",
   "301471",
   "301472",
   "301473",
   "301474",
   "301475",
   "301476",
   "301477",
   "301478",
   "301480",
   "301481",
   "301482",
   "301485",
   "301486",
   "301487",
   "301488",
   "301489",
   "301490",
   "301491",
   "301492",
   "301493",
   "301494",
   "301495",
   "301496",
   "301497",
   "301498",
   "301499"
};

const char* const c_all_sorted_field_names[ ] =
{
   "Actions",
   "All_Strings",
   "All_Vars",
   "Can_Access_Below_Model",
   "Child_Rel_Child_Class",
   "Child_Rel_Field_Key",
   "Child_Relationship",
   "Child_Specification_Type",
   "Class",
   "Comments",
   "Enum",
   "Enum_Item",
   "Enum_Item_2",
   "Enum_Item_3",
   "Enum_Item_4",
   "Enum_Item_5",
   "Field",
   "Field_Class",
   "Field_Pairs",
   "Field_Values",
   "Fields",
   "Id",
   "Is_Required_For_UI_Gen",
   "Model",
   "Modifier",
   "Name",
   "Options",
   "Order",
   "Other_Class",
   "Other_Class_Field",
   "Other_Field",
   "Other_Field_2",
   "Other_Modifier",
   "Other_Modifier_2",
   "Other_Permission",
   "Other_Permission_2",
   "Other_Procedure",
   "Other_Procedure_2",
   "Other_Source_Child",
   "Other_Source_Child_2",
   "Parent_Specification",
   "Permission",
   "Procedure",
   "Procedure_Arg",
   "Procedure_Arg_2",
   "Procedure_Arg_3",
   "Protect_Child_Rel",
   "Protect_Procedure",
   "Protect_Source_Parent",
   "Restrict_Values",
   "Source_Child",
   "Source_Child_Class",
   "Source_Class",
   "Source_Field",
   "Source_Field_Class",
   "Source_Grandchild",
   "Source_Parent",
   "Source_Parent_Class",
   "Specification_Type",
   "Strings",
   "Test_Child",
   "Test_Field",
   "Test_Field_Class",
   "Test_Parent",
   "Test_Parent_Class",
   "Test_Value",
   "Test_Value_Label",
   "Use_Source_Parent",
   "Use_Test_Parent_Child",
   "Value",
   "Value_Label",
   "Value_Left_Part",
   "Value_Literal",
   "Value_Numeric_String",
   "Value_Right_Part",
   "Value_String",
   "Vars"
};

inline bool compare( const char* p_s1, const char* p_s2 ) { return strcmp( p_s1, p_s2 ) < 0; }

inline bool has_field( const string& field )
{
   return binary_search( c_all_sorted_field_ids, c_all_sorted_field_ids + c_num_fields, field.c_str( ), compare )
    || binary_search( c_all_sorted_field_names, c_all_sorted_field_names + c_num_fields, field.c_str( ), compare );
}

const int c_num_encrypted_fields = 0;

bool is_encrypted_field( const string& ) { static bool false_value( false ); return false_value; }

const int c_num_transient_fields = 4;

const char* const c_transient_sorted_field_ids[ ] =
{
   "115114",
   "115119",
   "115123",
   "115124"
};

const char* const c_transient_sorted_field_names[ ] =
{
   "All_Strings",
   "All_Vars",
   "Strings",
   "Vars"
};

inline bool is_transient_field( const string& field )
{
   return binary_search( c_transient_sorted_field_ids,
    c_transient_sorted_field_ids + c_num_transient_fields, field.c_str( ), compare )
    || binary_search( c_transient_sorted_field_names,
    c_transient_sorted_field_names + c_num_transient_fields, field.c_str( ), compare );
}

const char* const c_procedure_id_Add_Arg_1 = "115439";
const char* const c_procedure_id_Add_Arg_2 = "115440";
const char* const c_procedure_id_Add_Child_Info = "115432";
const char* const c_procedure_id_Add_Destination_Info = "115447";
const char* const c_procedure_id_Add_Extra_Info = "115443";
const char* const c_procedure_id_Add_Field = "115430";
const char* const c_procedure_id_Add_Field_Info = "115446";
const char* const c_procedure_id_Add_Field_Pair = "115431";
const char* const c_procedure_id_Add_Grandchild_Info = "115438";
const char* const c_procedure_id_Add_Info = "115441";
const char* const c_procedure_id_Add_Next = "115444";
const char* const c_procedure_id_Add_Opt_Info = "115442";
const char* const c_procedure_id_Add_Other_Info = "115445";
const char* const c_procedure_id_Add_Secondary = "115433";
const char* const c_procedure_id_Add_Source_Info = "115437";
const char* const c_procedure_id_Add_Stats_1 = "115434";
const char* const c_procedure_id_Add_Stats_2 = "115435";
const char* const c_procedure_id_Add_Stats_Info = "115436";
const char* const c_procedure_id_Generate_All_Strings = "115510";
const char* const c_procedure_id_Generate_All_Vars = "115500";
const char* const c_procedure_id_Move_Down = "115420";
const char* const c_procedure_id_Move_Up = "115410";

const uint64_t c_modifier_Hide_Child_Relationship = UINT64_C( 0x100 );
const uint64_t c_modifier_Hide_Enum = UINT64_C( 0x200 );
const uint64_t c_modifier_Hide_Enum_Item = UINT64_C( 0x400 );
const uint64_t c_modifier_Hide_Enum_Item_2 = UINT64_C( 0x800 );
const uint64_t c_modifier_Hide_Enum_Item_3 = UINT64_C( 0x1000 );
const uint64_t c_modifier_Hide_Enum_Item_4 = UINT64_C( 0x2000 );
const uint64_t c_modifier_Hide_Enum_Item_5 = UINT64_C( 0x4000 );
const uint64_t c_modifier_Hide_Field = UINT64_C( 0x8000 );
const uint64_t c_modifier_Hide_Modifier = UINT64_C( 0x10000 );
const uint64_t c_modifier_Hide_Options = UINT64_C( 0x20000 );
const uint64_t c_modifier_Hide_Other_Class = UINT64_C( 0x40000 );
const uint64_t c_modifier_Hide_Other_Class_Field = UINT64_C( 0x80000 );
const uint64_t c_modifier_Hide_Other_Field = UINT64_C( 0x100000 );
const uint64_t c_modifier_Hide_Other_Field_2 = UINT64_C( 0x200000 );
const uint64_t c_modifier_Hide_Other_Modifier = UINT64_C( 0x400000 );
const uint64_t c_modifier_Hide_Other_Modifier_2 = UINT64_C( 0x800000 );
const uint64_t c_modifier_Hide_Other_Permission = UINT64_C( 0x1000000 );
const uint64_t c_modifier_Hide_Other_Permission_2 = UINT64_C( 0x2000000 );
const uint64_t c_modifier_Hide_Other_Procedure = UINT64_C( 0x4000000 );
const uint64_t c_modifier_Hide_Other_Procedure_2 = UINT64_C( 0x8000000 );
const uint64_t c_modifier_Hide_Other_Source_Child = UINT64_C( 0x10000000 );
const uint64_t c_modifier_Hide_Other_Source_Child_2 = UINT64_C( 0x20000000 );
const uint64_t c_modifier_Hide_Permission = UINT64_C( 0x40000000 );
const uint64_t c_modifier_Hide_Procedure = UINT64_C( 0x80000000 );
const uint64_t c_modifier_Hide_Procedure_Arg = UINT64_C( 0x100000000 );
const uint64_t c_modifier_Hide_Procedure_Arg_2 = UINT64_C( 0x200000000 );
const uint64_t c_modifier_Hide_Procedure_Arg_3 = UINT64_C( 0x400000000 );
const uint64_t c_modifier_Hide_Source_Child = UINT64_C( 0x800000000 );
const uint64_t c_modifier_Hide_Source_Class = UINT64_C( 0x1000000000 );
const uint64_t c_modifier_Hide_Source_Field = UINT64_C( 0x2000000000 );
const uint64_t c_modifier_Hide_Source_Grandchild = UINT64_C( 0x4000000000 );
const uint64_t c_modifier_Hide_Source_Parent = UINT64_C( 0x8000000000 );
const uint64_t c_modifier_Hide_Test_Child = UINT64_C( 0x10000000000 );
const uint64_t c_modifier_Hide_Test_Field = UINT64_C( 0x20000000000 );
const uint64_t c_modifier_Hide_Test_Parent = UINT64_C( 0x40000000000 );
const uint64_t c_modifier_Hide_Test_Value = UINT64_C( 0x80000000000 );
const uint64_t c_modifier_Hide_Use_Source_Parent = UINT64_C( 0x100000000000 );
const uint64_t c_modifier_Hide_Use_Test_Parent_Child = UINT64_C( 0x200000000000 );
const uint64_t c_modifier_Hide_Value = UINT64_C( 0x400000000000 );
const uint64_t c_modifier_Is_Comment = UINT64_C( 0x800000000000 );
const uint64_t c_modifier_Protect_Child_Relationship = UINT64_C( 0x1000000000000 );
const uint64_t c_modifier_Protect_Class = UINT64_C( 0x2000000000000 );
const uint64_t c_modifier_Protect_Enum = UINT64_C( 0x4000000000000 );
const uint64_t c_modifier_Protect_Other_Class = UINT64_C( 0x8000000000000 );
const uint64_t c_modifier_Protect_Procedure = UINT64_C( 0x10000000000000 );
const uint64_t c_modifier_Protect_Source_Parent = UINT64_C( 0x20000000000000 );

domain_string_max_size< 100 > g_Comments_domain;
aggregate_domain< string,
 domain_string_identifier_format,
 domain_string_max_size< 30 > > g_Id_domain;
domain_string_max_size< 200 > g_Name_domain;
domain_string_max_size< 100 > g_Options_domain;
domain_string_max_size< 100 > g_Test_Value_domain;
domain_string_max_size< 200 > g_Value_domain;

string g_group_field_name;
string g_level_field_name;
string g_order_field_name( "Order" );
string g_owner_field_name;

string g_state_names_variable;

set< string > g_derivations;

set< string > g_file_field_ids;
set< string > g_file_field_names;

typedef map< string, Meta_Specification* > external_aliases_container;
typedef external_aliases_container::const_iterator external_aliases_const_iterator;
typedef external_aliases_container::value_type external_aliases_value_type;

typedef map< size_t, Meta_Specification* > external_aliases_lookup_container;
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
string g_default_All_Strings = string( );
string g_default_All_Vars = string( );
bool g_default_Can_Access_Below_Model = bool( 0 );
string g_default_Child_Rel_Child_Class = string( );
string g_default_Child_Rel_Field_Key = string( );
string g_default_Child_Relationship = string( );
string g_default_Child_Specification_Type = string( );
string g_default_Class = string( );
string g_default_Comments = string( );
string g_default_Enum = string( );
string g_default_Enum_Item = string( );
string g_default_Enum_Item_2 = string( );
string g_default_Enum_Item_3 = string( );
string g_default_Enum_Item_4 = string( );
string g_default_Enum_Item_5 = string( );
string g_default_Field = string( );
string g_default_Field_Class = string( );
string g_default_Field_Pairs = string( );
string g_default_Field_Values = string( );
string g_default_Fields = string( );
string g_default_Id = string( );
bool g_default_Is_Required_For_UI_Gen = bool( 0 );
string g_default_Model = string( );
string g_default_Modifier = string( );
string g_default_Name = string( );
string g_default_Options = string( );
string g_default_Order = string( );
string g_default_Other_Class = string( );
string g_default_Other_Class_Field = string( );
string g_default_Other_Field = string( );
string g_default_Other_Field_2 = string( );
string g_default_Other_Modifier = string( );
string g_default_Other_Modifier_2 = string( );
string g_default_Other_Permission = string( );
string g_default_Other_Permission_2 = string( );
string g_default_Other_Procedure = string( );
string g_default_Other_Procedure_2 = string( );
string g_default_Other_Source_Child = string( );
string g_default_Other_Source_Child_2 = string( );
string g_default_Parent_Specification = string( );
string g_default_Permission = string( );
string g_default_Procedure = string( );
string g_default_Procedure_Arg = string( );
string g_default_Procedure_Arg_2 = string( );
string g_default_Procedure_Arg_3 = string( );
bool g_default_Protect_Child_Rel = bool( 0 );
bool g_default_Protect_Procedure = bool( 0 );
bool g_default_Protect_Source_Parent = bool( 0 );
string g_default_Restrict_Values = string( );
string g_default_Source_Child = string( );
string g_default_Source_Child_Class = string( );
string g_default_Source_Class = string( );
string g_default_Source_Field = string( );
string g_default_Source_Field_Class = string( );
string g_default_Source_Grandchild = string( );
string g_default_Source_Parent = string( );
string g_default_Source_Parent_Class = string( );
string g_default_Specification_Type = string( );
string g_default_Strings = string( );
string g_default_Test_Child = string( );
string g_default_Test_Field = string( );
string g_default_Test_Field_Class = string( );
string g_default_Test_Parent = string( );
string g_default_Test_Parent_Class = string( );
string g_default_Test_Value = string( );
string g_default_Test_Value_Label = string( );
bool g_default_Use_Source_Parent = bool( 0 );
bool g_default_Use_Test_Parent_Child = bool( 0 );
string g_default_Value = string( );
string g_default_Value_Label = string( );
string g_default_Value_Left_Part = string( );
string g_default_Value_Literal = string( );
string g_default_Value_Numeric_String = string( );
string g_default_Value_Right_Part = string( );
string g_default_Value_String = string( );
string g_default_Vars = string( );

// [<start anonymous>]
// [<finish anonymous>]

}

registration< Meta_Specification > Specification_registration( get_class_registry( ), "115100" );

class Meta_Specification_command_functor;

class Meta_Specification_command_handler : public command_handler
{
   friend class Meta_Specification_command_functor;

   public:
   Meta_Specification_command_handler( ) : p_Meta_Specification( 0 ) { }

   void set_Meta_Specification( Meta_Specification* p_new_Meta_Specification ) { p_Meta_Specification = p_new_Meta_Specification; }

   void handle_unknown_command( const string& command, const string& cmd_and_args )
   {
      throw runtime_error( "unknown command '" + command + "'" );
   }

   void handle_invalid_command( const command_parser& parser, const string& cmd_and_args )
   {
      throw runtime_error( "invalid command usage '" + cmd_and_args + "'" );
   }

   private:
   Meta_Specification* p_Meta_Specification;

   protected:
   string retval;
};

class Meta_Specification_command_functor : public command_functor
{
   public:
   Meta_Specification_command_functor( Meta_Specification_command_handler& handler )
    : command_functor( handler ),
    cmd_handler( handler )
   {
   }

   void operator ( )( const string& command, const parameter_info& parameters );

   private:
   Meta_Specification_command_handler& cmd_handler;
};

command_functor* Meta_Specification_command_functor_factory( const string& /*name*/, command_handler& handler )
{
   return new Meta_Specification_command_functor( dynamic_cast< Meta_Specification_command_handler& >( handler ) );
}

void Meta_Specification_command_functor::operator ( )( const string& command, const parameter_info& parameters )
{
   if( command == c_cmd_Meta_Specification_key )
   {
      bool want_fixed( has_parm_val( parameters, c_cmd_Meta_Specification_key_fixed ) );

      if( !want_fixed )
         cmd_handler.retval = cmd_handler.p_Meta_Specification->get_key( );
      else
         cmd_handler.retval = cmd_handler.p_Meta_Specification->get_fixed_key( );
   }
   else if( command == c_cmd_Meta_Specification_ver )
   {
      string ver_rev( to_string( cmd_handler.p_Meta_Specification->get_version( ) ) );
      ver_rev += "." + to_string( cmd_handler.p_Meta_Specification->get_revision( ) );

      cmd_handler.retval = ver_rev;
   }
   else if( command == c_cmd_Meta_Specification_get )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Specification_get_field_name ) );

      bool handled = false;

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for getter call" );

      if( !handled && ( ( field_name == c_field_id_Actions ) || ( field_name == c_field_name_Actions ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Specification->Actions( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_All_Strings ) || ( field_name == c_field_name_All_Strings ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Specification->All_Strings( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_All_Vars ) || ( field_name == c_field_name_All_Vars ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Specification->All_Vars( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Can_Access_Below_Model ) || ( field_name == c_field_name_Can_Access_Below_Model ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification->Can_Access_Below_Model( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Child_Rel_Child_Class ) || ( field_name == c_field_name_Child_Rel_Child_Class ) ) )
      {
         handled = true;

         string_getter< Meta_Class >( cmd_handler.p_Meta_Specification->Child_Rel_Child_Class( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Child_Rel_Field_Key ) || ( field_name == c_field_name_Child_Rel_Field_Key ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Specification->Child_Rel_Field_Key( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Child_Relationship ) || ( field_name == c_field_name_Child_Relationship ) ) )
      {
         handled = true;

         string_getter< Meta_Relationship >( cmd_handler.p_Meta_Specification->Child_Relationship( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Child_Specification_Type ) || ( field_name == c_field_name_Child_Specification_Type ) ) )
      {
         handled = true;

         string_getter< Meta_Specification_Type >( cmd_handler.p_Meta_Specification->Child_Specification_Type( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Class ) || ( field_name == c_field_name_Class ) ) )
      {
         handled = true;

         string_getter< Meta_Class >( cmd_handler.p_Meta_Specification->Class( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Comments ) || ( field_name == c_field_name_Comments ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Specification->Comments( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Enum ) || ( field_name == c_field_name_Enum ) ) )
      {
         handled = true;

         string_getter< Meta_Enum >( cmd_handler.p_Meta_Specification->Enum( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Enum_Item ) || ( field_name == c_field_name_Enum_Item ) ) )
      {
         handled = true;

         string_getter< Meta_Enum_Item >( cmd_handler.p_Meta_Specification->Enum_Item( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Enum_Item_2 ) || ( field_name == c_field_name_Enum_Item_2 ) ) )
      {
         handled = true;

         string_getter< Meta_Enum_Item >( cmd_handler.p_Meta_Specification->Enum_Item_2( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Enum_Item_3 ) || ( field_name == c_field_name_Enum_Item_3 ) ) )
      {
         handled = true;

         string_getter< Meta_Enum_Item >( cmd_handler.p_Meta_Specification->Enum_Item_3( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Enum_Item_4 ) || ( field_name == c_field_name_Enum_Item_4 ) ) )
      {
         handled = true;

         string_getter< Meta_Enum_Item >( cmd_handler.p_Meta_Specification->Enum_Item_4( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Enum_Item_5 ) || ( field_name == c_field_name_Enum_Item_5 ) ) )
      {
         handled = true;

         string_getter< Meta_Enum_Item >( cmd_handler.p_Meta_Specification->Enum_Item_5( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Field ) || ( field_name == c_field_name_Field ) ) )
      {
         handled = true;

         string_getter< Meta_Field >( cmd_handler.p_Meta_Specification->Field( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Field_Class ) || ( field_name == c_field_name_Field_Class ) ) )
      {
         handled = true;

         string_getter< Meta_Class >( cmd_handler.p_Meta_Specification->Field_Class( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Field_Pairs ) || ( field_name == c_field_name_Field_Pairs ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Specification->Field_Pairs( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Field_Values ) || ( field_name == c_field_name_Field_Values ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Specification->Field_Values( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Fields ) || ( field_name == c_field_name_Fields ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Specification->Fields( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Id ) || ( field_name == c_field_name_Id ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Specification->Id( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Is_Required_For_UI_Gen ) || ( field_name == c_field_name_Is_Required_For_UI_Gen ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification->Is_Required_For_UI_Gen( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Model ) || ( field_name == c_field_name_Model ) ) )
      {
         handled = true;

         string_getter< Meta_Model >( cmd_handler.p_Meta_Specification->Model( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Modifier ) || ( field_name == c_field_name_Modifier ) ) )
      {
         handled = true;

         string_getter< Meta_Modifier >( cmd_handler.p_Meta_Specification->Modifier( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Name ) || ( field_name == c_field_name_Name ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Specification->Name( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Options ) || ( field_name == c_field_name_Options ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Specification->Options( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Order ) || ( field_name == c_field_name_Order ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Specification->Order( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Other_Class ) || ( field_name == c_field_name_Other_Class ) ) )
      {
         handled = true;

         string_getter< Meta_Class >( cmd_handler.p_Meta_Specification->Other_Class( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Other_Class_Field ) || ( field_name == c_field_name_Other_Class_Field ) ) )
      {
         handled = true;

         string_getter< Meta_Field >( cmd_handler.p_Meta_Specification->Other_Class_Field( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Other_Field ) || ( field_name == c_field_name_Other_Field ) ) )
      {
         handled = true;

         string_getter< Meta_Field >( cmd_handler.p_Meta_Specification->Other_Field( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Other_Field_2 ) || ( field_name == c_field_name_Other_Field_2 ) ) )
      {
         handled = true;

         string_getter< Meta_Field >( cmd_handler.p_Meta_Specification->Other_Field_2( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Other_Modifier ) || ( field_name == c_field_name_Other_Modifier ) ) )
      {
         handled = true;

         string_getter< Meta_Modifier >( cmd_handler.p_Meta_Specification->Other_Modifier( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Other_Modifier_2 ) || ( field_name == c_field_name_Other_Modifier_2 ) ) )
      {
         handled = true;

         string_getter< Meta_Modifier >( cmd_handler.p_Meta_Specification->Other_Modifier_2( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Other_Permission ) || ( field_name == c_field_name_Other_Permission ) ) )
      {
         handled = true;

         string_getter< Meta_Permission >( cmd_handler.p_Meta_Specification->Other_Permission( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Other_Permission_2 ) || ( field_name == c_field_name_Other_Permission_2 ) ) )
      {
         handled = true;

         string_getter< Meta_Permission >( cmd_handler.p_Meta_Specification->Other_Permission_2( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Other_Procedure ) || ( field_name == c_field_name_Other_Procedure ) ) )
      {
         handled = true;

         string_getter< Meta_Procedure >( cmd_handler.p_Meta_Specification->Other_Procedure( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Other_Procedure_2 ) || ( field_name == c_field_name_Other_Procedure_2 ) ) )
      {
         handled = true;

         string_getter< Meta_Procedure >( cmd_handler.p_Meta_Specification->Other_Procedure_2( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Other_Source_Child ) || ( field_name == c_field_name_Other_Source_Child ) ) )
      {
         handled = true;

         string_getter< Meta_Field >( cmd_handler.p_Meta_Specification->Other_Source_Child( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Other_Source_Child_2 ) || ( field_name == c_field_name_Other_Source_Child_2 ) ) )
      {
         handled = true;

         string_getter< Meta_Field >( cmd_handler.p_Meta_Specification->Other_Source_Child_2( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Parent_Specification ) || ( field_name == c_field_name_Parent_Specification ) ) )
      {
         handled = true;

         string_getter< Meta_Specification >( cmd_handler.p_Meta_Specification->Parent_Specification( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Permission ) || ( field_name == c_field_name_Permission ) ) )
      {
         handled = true;

         string_getter< Meta_Permission >( cmd_handler.p_Meta_Specification->Permission( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Procedure ) || ( field_name == c_field_name_Procedure ) ) )
      {
         handled = true;

         string_getter< Meta_Procedure >( cmd_handler.p_Meta_Specification->Procedure( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Procedure_Arg ) || ( field_name == c_field_name_Procedure_Arg ) ) )
      {
         handled = true;

         string_getter< Meta_Procedure_Arg >( cmd_handler.p_Meta_Specification->Procedure_Arg( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Procedure_Arg_2 ) || ( field_name == c_field_name_Procedure_Arg_2 ) ) )
      {
         handled = true;

         string_getter< Meta_Procedure_Arg >( cmd_handler.p_Meta_Specification->Procedure_Arg_2( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Procedure_Arg_3 ) || ( field_name == c_field_name_Procedure_Arg_3 ) ) )
      {
         handled = true;

         string_getter< Meta_Procedure_Arg >( cmd_handler.p_Meta_Specification->Procedure_Arg_3( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Protect_Child_Rel ) || ( field_name == c_field_name_Protect_Child_Rel ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification->Protect_Child_Rel( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Protect_Procedure ) || ( field_name == c_field_name_Protect_Procedure ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification->Protect_Procedure( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Protect_Source_Parent ) || ( field_name == c_field_name_Protect_Source_Parent ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification->Protect_Source_Parent( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Restrict_Values ) || ( field_name == c_field_name_Restrict_Values ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Specification->Restrict_Values( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Child ) || ( field_name == c_field_name_Source_Child ) ) )
      {
         handled = true;

         string_getter< Meta_Field >( cmd_handler.p_Meta_Specification->Source_Child( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Child_Class ) || ( field_name == c_field_name_Source_Child_Class ) ) )
      {
         handled = true;

         string_getter< Meta_Class >( cmd_handler.p_Meta_Specification->Source_Child_Class( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Class ) || ( field_name == c_field_name_Source_Class ) ) )
      {
         handled = true;

         string_getter< Meta_Class >( cmd_handler.p_Meta_Specification->Source_Class( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Field ) || ( field_name == c_field_name_Source_Field ) ) )
      {
         handled = true;

         string_getter< Meta_Field >( cmd_handler.p_Meta_Specification->Source_Field( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Field_Class ) || ( field_name == c_field_name_Source_Field_Class ) ) )
      {
         handled = true;

         string_getter< Meta_Class >( cmd_handler.p_Meta_Specification->Source_Field_Class( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Grandchild ) || ( field_name == c_field_name_Source_Grandchild ) ) )
      {
         handled = true;

         string_getter< Meta_Field >( cmd_handler.p_Meta_Specification->Source_Grandchild( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Parent ) || ( field_name == c_field_name_Source_Parent ) ) )
      {
         handled = true;

         string_getter< Meta_Field >( cmd_handler.p_Meta_Specification->Source_Parent( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Parent_Class ) || ( field_name == c_field_name_Source_Parent_Class ) ) )
      {
         handled = true;

         string_getter< Meta_Class >( cmd_handler.p_Meta_Specification->Source_Parent_Class( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Specification_Type ) || ( field_name == c_field_name_Specification_Type ) ) )
      {
         handled = true;

         string_getter< Meta_Specification_Type >( cmd_handler.p_Meta_Specification->Specification_Type( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Strings ) || ( field_name == c_field_name_Strings ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Specification->Strings( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Test_Child ) || ( field_name == c_field_name_Test_Child ) ) )
      {
         handled = true;

         string_getter< Meta_Field >( cmd_handler.p_Meta_Specification->Test_Child( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Test_Field ) || ( field_name == c_field_name_Test_Field ) ) )
      {
         handled = true;

         string_getter< Meta_Field >( cmd_handler.p_Meta_Specification->Test_Field( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Test_Field_Class ) || ( field_name == c_field_name_Test_Field_Class ) ) )
      {
         handled = true;

         string_getter< Meta_Class >( cmd_handler.p_Meta_Specification->Test_Field_Class( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Test_Parent ) || ( field_name == c_field_name_Test_Parent ) ) )
      {
         handled = true;

         string_getter< Meta_Field >( cmd_handler.p_Meta_Specification->Test_Parent( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Test_Parent_Class ) || ( field_name == c_field_name_Test_Parent_Class ) ) )
      {
         handled = true;

         string_getter< Meta_Class >( cmd_handler.p_Meta_Specification->Test_Parent_Class( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Test_Value ) || ( field_name == c_field_name_Test_Value ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Specification->Test_Value( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Test_Value_Label ) || ( field_name == c_field_name_Test_Value_Label ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Specification->Test_Value_Label( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Source_Parent ) || ( field_name == c_field_name_Use_Source_Parent ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification->Use_Source_Parent( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Test_Parent_Child ) || ( field_name == c_field_name_Use_Test_Parent_Child ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification->Use_Test_Parent_Child( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Value ) || ( field_name == c_field_name_Value ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Specification->Value( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Value_Label ) || ( field_name == c_field_name_Value_Label ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Specification->Value_Label( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Value_Left_Part ) || ( field_name == c_field_name_Value_Left_Part ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Specification->Value_Left_Part( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Value_Literal ) || ( field_name == c_field_name_Value_Literal ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Specification->Value_Literal( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Value_Numeric_String ) || ( field_name == c_field_name_Value_Numeric_String ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Specification->Value_Numeric_String( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Value_Right_Part ) || ( field_name == c_field_name_Value_Right_Part ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Specification->Value_Right_Part( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Value_String ) || ( field_name == c_field_name_Value_String ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Specification->Value_String( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Vars ) || ( field_name == c_field_name_Vars ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Specification->Vars( ), cmd_handler.retval );
      }

      if( !handled )
         throw runtime_error( "unknown field name '" + field_name + "' for getter call" );
   }
   else if( command == c_cmd_Meta_Specification_set )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Specification_set_field_name ) );
      string field_value( get_parm_val( parameters, c_cmd_Meta_Specification_set_field_value ) );

      bool handled = false;

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for setter call" );

      if( !handled && ( ( field_name == c_field_id_Actions ) || ( field_name == c_field_name_Actions ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, string >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Actions, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_All_Strings ) || ( field_name == c_field_name_All_Strings ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, string >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::All_Strings, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_All_Vars ) || ( field_name == c_field_name_All_Vars ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, string >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::All_Vars, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Can_Access_Below_Model ) || ( field_name == c_field_name_Can_Access_Below_Model ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, bool >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Can_Access_Below_Model, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Child_Rel_Child_Class ) || ( field_name == c_field_name_Child_Rel_Child_Class ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, Meta_Class >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Child_Rel_Child_Class, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Child_Rel_Field_Key ) || ( field_name == c_field_name_Child_Rel_Field_Key ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, string >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Child_Rel_Field_Key, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Child_Relationship ) || ( field_name == c_field_name_Child_Relationship ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, Meta_Relationship >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Child_Relationship, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Child_Specification_Type ) || ( field_name == c_field_name_Child_Specification_Type ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, Meta_Specification_Type >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Child_Specification_Type, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Class ) || ( field_name == c_field_name_Class ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, Meta_Class >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Class, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Comments ) || ( field_name == c_field_name_Comments ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, string >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Comments, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Enum ) || ( field_name == c_field_name_Enum ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, Meta_Enum >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Enum, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Enum_Item ) || ( field_name == c_field_name_Enum_Item ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, Meta_Enum_Item >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Enum_Item, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Enum_Item_2 ) || ( field_name == c_field_name_Enum_Item_2 ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, Meta_Enum_Item >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Enum_Item_2, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Enum_Item_3 ) || ( field_name == c_field_name_Enum_Item_3 ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, Meta_Enum_Item >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Enum_Item_3, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Enum_Item_4 ) || ( field_name == c_field_name_Enum_Item_4 ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, Meta_Enum_Item >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Enum_Item_4, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Enum_Item_5 ) || ( field_name == c_field_name_Enum_Item_5 ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, Meta_Enum_Item >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Enum_Item_5, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Field ) || ( field_name == c_field_name_Field ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, Meta_Field >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Field, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Field_Class ) || ( field_name == c_field_name_Field_Class ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, Meta_Class >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Field_Class, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Field_Pairs ) || ( field_name == c_field_name_Field_Pairs ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, string >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Field_Pairs, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Field_Values ) || ( field_name == c_field_name_Field_Values ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, string >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Field_Values, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Fields ) || ( field_name == c_field_name_Fields ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, string >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Fields, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Id ) || ( field_name == c_field_name_Id ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, string >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Id, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Is_Required_For_UI_Gen ) || ( field_name == c_field_name_Is_Required_For_UI_Gen ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, bool >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Is_Required_For_UI_Gen, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Model ) || ( field_name == c_field_name_Model ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, Meta_Model >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Model, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Modifier ) || ( field_name == c_field_name_Modifier ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, Meta_Modifier >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Modifier, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Name ) || ( field_name == c_field_name_Name ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, string >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Name, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Options ) || ( field_name == c_field_name_Options ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, string >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Options, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Order ) || ( field_name == c_field_name_Order ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, string >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Order, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Other_Class ) || ( field_name == c_field_name_Other_Class ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, Meta_Class >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Other_Class, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Other_Class_Field ) || ( field_name == c_field_name_Other_Class_Field ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, Meta_Field >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Other_Class_Field, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Other_Field ) || ( field_name == c_field_name_Other_Field ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, Meta_Field >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Other_Field, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Other_Field_2 ) || ( field_name == c_field_name_Other_Field_2 ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, Meta_Field >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Other_Field_2, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Other_Modifier ) || ( field_name == c_field_name_Other_Modifier ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, Meta_Modifier >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Other_Modifier, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Other_Modifier_2 ) || ( field_name == c_field_name_Other_Modifier_2 ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, Meta_Modifier >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Other_Modifier_2, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Other_Permission ) || ( field_name == c_field_name_Other_Permission ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, Meta_Permission >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Other_Permission, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Other_Permission_2 ) || ( field_name == c_field_name_Other_Permission_2 ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, Meta_Permission >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Other_Permission_2, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Other_Procedure ) || ( field_name == c_field_name_Other_Procedure ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, Meta_Procedure >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Other_Procedure, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Other_Procedure_2 ) || ( field_name == c_field_name_Other_Procedure_2 ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, Meta_Procedure >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Other_Procedure_2, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Other_Source_Child ) || ( field_name == c_field_name_Other_Source_Child ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, Meta_Field >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Other_Source_Child, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Other_Source_Child_2 ) || ( field_name == c_field_name_Other_Source_Child_2 ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, Meta_Field >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Other_Source_Child_2, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Parent_Specification ) || ( field_name == c_field_name_Parent_Specification ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, Meta_Specification >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Parent_Specification, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Permission ) || ( field_name == c_field_name_Permission ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, Meta_Permission >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Permission, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Procedure ) || ( field_name == c_field_name_Procedure ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, Meta_Procedure >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Procedure, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Procedure_Arg ) || ( field_name == c_field_name_Procedure_Arg ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, Meta_Procedure_Arg >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Procedure_Arg, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Procedure_Arg_2 ) || ( field_name == c_field_name_Procedure_Arg_2 ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, Meta_Procedure_Arg >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Procedure_Arg_2, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Procedure_Arg_3 ) || ( field_name == c_field_name_Procedure_Arg_3 ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, Meta_Procedure_Arg >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Procedure_Arg_3, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Protect_Child_Rel ) || ( field_name == c_field_name_Protect_Child_Rel ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, bool >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Protect_Child_Rel, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Protect_Procedure ) || ( field_name == c_field_name_Protect_Procedure ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, bool >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Protect_Procedure, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Protect_Source_Parent ) || ( field_name == c_field_name_Protect_Source_Parent ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, bool >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Protect_Source_Parent, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Restrict_Values ) || ( field_name == c_field_name_Restrict_Values ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, string >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Restrict_Values, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Child ) || ( field_name == c_field_name_Source_Child ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, Meta_Field >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Source_Child, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Child_Class ) || ( field_name == c_field_name_Source_Child_Class ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, Meta_Class >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Source_Child_Class, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Class ) || ( field_name == c_field_name_Source_Class ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, Meta_Class >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Source_Class, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Field ) || ( field_name == c_field_name_Source_Field ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, Meta_Field >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Source_Field, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Field_Class ) || ( field_name == c_field_name_Source_Field_Class ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, Meta_Class >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Source_Field_Class, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Grandchild ) || ( field_name == c_field_name_Source_Grandchild ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, Meta_Field >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Source_Grandchild, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Parent ) || ( field_name == c_field_name_Source_Parent ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, Meta_Field >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Source_Parent, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Parent_Class ) || ( field_name == c_field_name_Source_Parent_Class ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, Meta_Class >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Source_Parent_Class, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Specification_Type ) || ( field_name == c_field_name_Specification_Type ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, Meta_Specification_Type >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Specification_Type, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Strings ) || ( field_name == c_field_name_Strings ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, string >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Strings, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Test_Child ) || ( field_name == c_field_name_Test_Child ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, Meta_Field >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Test_Child, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Test_Field ) || ( field_name == c_field_name_Test_Field ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, Meta_Field >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Test_Field, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Test_Field_Class ) || ( field_name == c_field_name_Test_Field_Class ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, Meta_Class >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Test_Field_Class, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Test_Parent ) || ( field_name == c_field_name_Test_Parent ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, Meta_Field >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Test_Parent, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Test_Parent_Class ) || ( field_name == c_field_name_Test_Parent_Class ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, Meta_Class >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Test_Parent_Class, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Test_Value ) || ( field_name == c_field_name_Test_Value ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, string >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Test_Value, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Test_Value_Label ) || ( field_name == c_field_name_Test_Value_Label ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, string >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Test_Value_Label, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Source_Parent ) || ( field_name == c_field_name_Use_Source_Parent ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, bool >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Use_Source_Parent, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Test_Parent_Child ) || ( field_name == c_field_name_Use_Test_Parent_Child ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, bool >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Use_Test_Parent_Child, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Value ) || ( field_name == c_field_name_Value ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, string >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Value, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Value_Label ) || ( field_name == c_field_name_Value_Label ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, string >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Value_Label, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Value_Left_Part ) || ( field_name == c_field_name_Value_Left_Part ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, string >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Value_Left_Part, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Value_Literal ) || ( field_name == c_field_name_Value_Literal ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, string >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Value_Literal, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Value_Numeric_String ) || ( field_name == c_field_name_Value_Numeric_String ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, string >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Value_Numeric_String, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Value_Right_Part ) || ( field_name == c_field_name_Value_Right_Part ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, string >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Value_Right_Part, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Value_String ) || ( field_name == c_field_name_Value_String ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, string >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Value_String, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Vars ) || ( field_name == c_field_name_Vars ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification, string >(
          *cmd_handler.p_Meta_Specification, &Meta_Specification::Vars, field_value );
      }

      if( !handled )
         throw runtime_error( "unknown field name '" + field_name + "' for setter call" );

      cmd_handler.retval = c_okay;
   }
   else if( command == c_cmd_Meta_Specification_cmd )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Specification_cmd_field_name ) );
      string cmd_and_args( get_parm_val( parameters, c_cmd_Meta_Specification_cmd_cmd_and_args ) );

      cmd_handler.retval.erase( );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for command call" );
      else if( ( field_name == c_field_id_Child_Rel_Child_Class ) || ( field_name == c_field_name_Child_Rel_Child_Class ) )
         cmd_handler.retval = cmd_handler.p_Meta_Specification->Child_Rel_Child_Class( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Child_Relationship ) || ( field_name == c_field_name_Child_Relationship ) )
         cmd_handler.retval = cmd_handler.p_Meta_Specification->Child_Relationship( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Child_Specification_Type ) || ( field_name == c_field_name_Child_Specification_Type ) )
         cmd_handler.retval = cmd_handler.p_Meta_Specification->Child_Specification_Type( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Class ) || ( field_name == c_field_name_Class ) )
         cmd_handler.retval = cmd_handler.p_Meta_Specification->Class( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Enum ) || ( field_name == c_field_name_Enum ) )
         cmd_handler.retval = cmd_handler.p_Meta_Specification->Enum( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Enum_Item ) || ( field_name == c_field_name_Enum_Item ) )
         cmd_handler.retval = cmd_handler.p_Meta_Specification->Enum_Item( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Enum_Item_2 ) || ( field_name == c_field_name_Enum_Item_2 ) )
         cmd_handler.retval = cmd_handler.p_Meta_Specification->Enum_Item_2( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Enum_Item_3 ) || ( field_name == c_field_name_Enum_Item_3 ) )
         cmd_handler.retval = cmd_handler.p_Meta_Specification->Enum_Item_3( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Enum_Item_4 ) || ( field_name == c_field_name_Enum_Item_4 ) )
         cmd_handler.retval = cmd_handler.p_Meta_Specification->Enum_Item_4( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Enum_Item_5 ) || ( field_name == c_field_name_Enum_Item_5 ) )
         cmd_handler.retval = cmd_handler.p_Meta_Specification->Enum_Item_5( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Field ) || ( field_name == c_field_name_Field ) )
         cmd_handler.retval = cmd_handler.p_Meta_Specification->Field( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Field_Class ) || ( field_name == c_field_name_Field_Class ) )
         cmd_handler.retval = cmd_handler.p_Meta_Specification->Field_Class( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Model ) || ( field_name == c_field_name_Model ) )
         cmd_handler.retval = cmd_handler.p_Meta_Specification->Model( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Modifier ) || ( field_name == c_field_name_Modifier ) )
         cmd_handler.retval = cmd_handler.p_Meta_Specification->Modifier( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Other_Class ) || ( field_name == c_field_name_Other_Class ) )
         cmd_handler.retval = cmd_handler.p_Meta_Specification->Other_Class( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Other_Class_Field ) || ( field_name == c_field_name_Other_Class_Field ) )
         cmd_handler.retval = cmd_handler.p_Meta_Specification->Other_Class_Field( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Other_Field ) || ( field_name == c_field_name_Other_Field ) )
         cmd_handler.retval = cmd_handler.p_Meta_Specification->Other_Field( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Other_Field_2 ) || ( field_name == c_field_name_Other_Field_2 ) )
         cmd_handler.retval = cmd_handler.p_Meta_Specification->Other_Field_2( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Other_Modifier ) || ( field_name == c_field_name_Other_Modifier ) )
         cmd_handler.retval = cmd_handler.p_Meta_Specification->Other_Modifier( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Other_Modifier_2 ) || ( field_name == c_field_name_Other_Modifier_2 ) )
         cmd_handler.retval = cmd_handler.p_Meta_Specification->Other_Modifier_2( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Other_Permission ) || ( field_name == c_field_name_Other_Permission ) )
         cmd_handler.retval = cmd_handler.p_Meta_Specification->Other_Permission( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Other_Permission_2 ) || ( field_name == c_field_name_Other_Permission_2 ) )
         cmd_handler.retval = cmd_handler.p_Meta_Specification->Other_Permission_2( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Other_Procedure ) || ( field_name == c_field_name_Other_Procedure ) )
         cmd_handler.retval = cmd_handler.p_Meta_Specification->Other_Procedure( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Other_Procedure_2 ) || ( field_name == c_field_name_Other_Procedure_2 ) )
         cmd_handler.retval = cmd_handler.p_Meta_Specification->Other_Procedure_2( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Other_Source_Child ) || ( field_name == c_field_name_Other_Source_Child ) )
         cmd_handler.retval = cmd_handler.p_Meta_Specification->Other_Source_Child( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Other_Source_Child_2 ) || ( field_name == c_field_name_Other_Source_Child_2 ) )
         cmd_handler.retval = cmd_handler.p_Meta_Specification->Other_Source_Child_2( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Parent_Specification ) || ( field_name == c_field_name_Parent_Specification ) )
         cmd_handler.retval = cmd_handler.p_Meta_Specification->Parent_Specification( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Permission ) || ( field_name == c_field_name_Permission ) )
         cmd_handler.retval = cmd_handler.p_Meta_Specification->Permission( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Procedure ) || ( field_name == c_field_name_Procedure ) )
         cmd_handler.retval = cmd_handler.p_Meta_Specification->Procedure( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Procedure_Arg ) || ( field_name == c_field_name_Procedure_Arg ) )
         cmd_handler.retval = cmd_handler.p_Meta_Specification->Procedure_Arg( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Procedure_Arg_2 ) || ( field_name == c_field_name_Procedure_Arg_2 ) )
         cmd_handler.retval = cmd_handler.p_Meta_Specification->Procedure_Arg_2( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Procedure_Arg_3 ) || ( field_name == c_field_name_Procedure_Arg_3 ) )
         cmd_handler.retval = cmd_handler.p_Meta_Specification->Procedure_Arg_3( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Source_Child ) || ( field_name == c_field_name_Source_Child ) )
         cmd_handler.retval = cmd_handler.p_Meta_Specification->Source_Child( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Source_Child_Class ) || ( field_name == c_field_name_Source_Child_Class ) )
         cmd_handler.retval = cmd_handler.p_Meta_Specification->Source_Child_Class( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Source_Class ) || ( field_name == c_field_name_Source_Class ) )
         cmd_handler.retval = cmd_handler.p_Meta_Specification->Source_Class( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Source_Field ) || ( field_name == c_field_name_Source_Field ) )
         cmd_handler.retval = cmd_handler.p_Meta_Specification->Source_Field( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Source_Field_Class ) || ( field_name == c_field_name_Source_Field_Class ) )
         cmd_handler.retval = cmd_handler.p_Meta_Specification->Source_Field_Class( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Source_Grandchild ) || ( field_name == c_field_name_Source_Grandchild ) )
         cmd_handler.retval = cmd_handler.p_Meta_Specification->Source_Grandchild( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Source_Parent ) || ( field_name == c_field_name_Source_Parent ) )
         cmd_handler.retval = cmd_handler.p_Meta_Specification->Source_Parent( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Source_Parent_Class ) || ( field_name == c_field_name_Source_Parent_Class ) )
         cmd_handler.retval = cmd_handler.p_Meta_Specification->Source_Parent_Class( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Specification_Type ) || ( field_name == c_field_name_Specification_Type ) )
         cmd_handler.retval = cmd_handler.p_Meta_Specification->Specification_Type( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Test_Child ) || ( field_name == c_field_name_Test_Child ) )
         cmd_handler.retval = cmd_handler.p_Meta_Specification->Test_Child( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Test_Field ) || ( field_name == c_field_name_Test_Field ) )
         cmd_handler.retval = cmd_handler.p_Meta_Specification->Test_Field( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Test_Field_Class ) || ( field_name == c_field_name_Test_Field_Class ) )
         cmd_handler.retval = cmd_handler.p_Meta_Specification->Test_Field_Class( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Test_Parent ) || ( field_name == c_field_name_Test_Parent ) )
         cmd_handler.retval = cmd_handler.p_Meta_Specification->Test_Parent( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Test_Parent_Class ) || ( field_name == c_field_name_Test_Parent_Class ) )
         cmd_handler.retval = cmd_handler.p_Meta_Specification->Test_Parent_Class( ).execute( cmd_and_args );
      else
         throw runtime_error( "unknown field name '" + field_name + "' for command call" );
   }
   else if( command == c_cmd_Meta_Specification_Add_Arg_1 )
   {
      cmd_handler.p_Meta_Specification->Add_Arg_1( );

      cmd_handler.retval.erase( );

   }
   else if( command == c_cmd_Meta_Specification_Add_Arg_2 )
   {
      cmd_handler.p_Meta_Specification->Add_Arg_2( );

      cmd_handler.retval.erase( );

   }
   else if( command == c_cmd_Meta_Specification_Add_Child_Info )
   {
      cmd_handler.p_Meta_Specification->Add_Child_Info( );

      cmd_handler.retval.erase( );

   }
   else if( command == c_cmd_Meta_Specification_Add_Destination_Info )
   {
      cmd_handler.p_Meta_Specification->Add_Destination_Info( );

      cmd_handler.retval.erase( );

   }
   else if( command == c_cmd_Meta_Specification_Add_Extra_Info )
   {
      cmd_handler.p_Meta_Specification->Add_Extra_Info( );

      cmd_handler.retval.erase( );

   }
   else if( command == c_cmd_Meta_Specification_Add_Field )
   {
      cmd_handler.p_Meta_Specification->Add_Field( );

      cmd_handler.retval.erase( );

   }
   else if( command == c_cmd_Meta_Specification_Add_Field_Info )
   {
      cmd_handler.p_Meta_Specification->Add_Field_Info( );

      cmd_handler.retval.erase( );

   }
   else if( command == c_cmd_Meta_Specification_Add_Field_Pair )
   {
      cmd_handler.p_Meta_Specification->Add_Field_Pair( );

      cmd_handler.retval.erase( );

   }
   else if( command == c_cmd_Meta_Specification_Add_Grandchild_Info )
   {
      cmd_handler.p_Meta_Specification->Add_Grandchild_Info( );

      cmd_handler.retval.erase( );

   }
   else if( command == c_cmd_Meta_Specification_Add_Info )
   {
      cmd_handler.p_Meta_Specification->Add_Info( );

      cmd_handler.retval.erase( );

   }
   else if( command == c_cmd_Meta_Specification_Add_Next )
   {
      cmd_handler.p_Meta_Specification->Add_Next( );

      cmd_handler.retval.erase( );

   }
   else if( command == c_cmd_Meta_Specification_Add_Opt_Info )
   {
      cmd_handler.p_Meta_Specification->Add_Opt_Info( );

      cmd_handler.retval.erase( );

   }
   else if( command == c_cmd_Meta_Specification_Add_Other_Info )
   {
      cmd_handler.p_Meta_Specification->Add_Other_Info( );

      cmd_handler.retval.erase( );

   }
   else if( command == c_cmd_Meta_Specification_Add_Secondary )
   {
      cmd_handler.p_Meta_Specification->Add_Secondary( );

      cmd_handler.retval.erase( );

   }
   else if( command == c_cmd_Meta_Specification_Add_Source_Info )
   {
      cmd_handler.p_Meta_Specification->Add_Source_Info( );

      cmd_handler.retval.erase( );

   }
   else if( command == c_cmd_Meta_Specification_Add_Stats_1 )
   {
      cmd_handler.p_Meta_Specification->Add_Stats_1( );

      cmd_handler.retval.erase( );

   }
   else if( command == c_cmd_Meta_Specification_Add_Stats_2 )
   {
      cmd_handler.p_Meta_Specification->Add_Stats_2( );

      cmd_handler.retval.erase( );

   }
   else if( command == c_cmd_Meta_Specification_Add_Stats_Info )
   {
      cmd_handler.p_Meta_Specification->Add_Stats_Info( );

      cmd_handler.retval.erase( );

   }
   else if( command == c_cmd_Meta_Specification_Generate_All_Strings )
   {
      string Parent_Prefix( get_parm_val_from_string< string >( parameters, c_cmd_Meta_Specification_Generate_All_Strings_Parent_Prefix ) );
      string All_Strings;

      cmd_handler.p_Meta_Specification->Generate_All_Strings( All_Strings, Parent_Prefix );

      cmd_handler.retval.erase( );

      append_value( cmd_handler.retval, All_Strings );
   }
   else if( command == c_cmd_Meta_Specification_Generate_All_Vars )
   {
      string Parent_Prefix( get_parm_val_from_string< string >( parameters, c_cmd_Meta_Specification_Generate_All_Vars_Parent_Prefix ) );
      string All_Vars;

      cmd_handler.p_Meta_Specification->Generate_All_Vars( All_Vars, Parent_Prefix );

      cmd_handler.retval.erase( );

      append_value( cmd_handler.retval, All_Vars );
   }
   else if( command == c_cmd_Meta_Specification_Move_Down )
   {
      string Restrict_Fields( get_parm_val_from_string< string >( parameters, c_cmd_Meta_Specification_Move_Down_Restrict_Fields ) );
      string Restrict_Values( get_parm_val_from_string< string >( parameters, c_cmd_Meta_Specification_Move_Down_Restrict_Values ) );

      cmd_handler.p_Meta_Specification->Move_Down( Restrict_Fields, Restrict_Values );

      cmd_handler.retval.erase( );

   }
   else if( command == c_cmd_Meta_Specification_Move_Up )
   {
      string Restrict_Fields( get_parm_val_from_string< string >( parameters, c_cmd_Meta_Specification_Move_Up_Restrict_Fields ) );
      string Restrict_Values( get_parm_val_from_string< string >( parameters, c_cmd_Meta_Specification_Move_Up_Restrict_Values ) );

      cmd_handler.p_Meta_Specification->Move_Up( Restrict_Fields, Restrict_Values );

      cmd_handler.retval.erase( );

   }
}

struct Meta_Specification::impl : public Meta_Specification_command_handler
{
   impl( Meta_Specification& o )
    :
    cp_obj( &o ),
    total_child_relationships( 0 )
   {
      p_obj = &o;
      set_Meta_Specification( &o );

      add_commands( 0, Meta_Specification_command_functor_factory,
       ARRAY_PTR_AND_SIZE( Meta_Specification_command_definitions ) );
   }

   Meta_Specification& get_obj( ) const
   {
      return *cp_obj;
   }

   const string& impl_Actions( ) const { return lazy_fetch( p_obj ), v_Actions; }
   void impl_Actions( const string& Actions ) { sanity_check( Actions ); v_Actions = Actions; }

   const string& impl_All_Strings( ) const { return lazy_fetch( p_obj ), v_All_Strings; }
   void impl_All_Strings( const string& All_Strings ) { sanity_check( All_Strings ); v_All_Strings = All_Strings; }

   const string& impl_All_Vars( ) const { return lazy_fetch( p_obj ), v_All_Vars; }
   void impl_All_Vars( const string& All_Vars ) { sanity_check( All_Vars ); v_All_Vars = All_Vars; }

   bool impl_Can_Access_Below_Model( ) const { return lazy_fetch( p_obj ), v_Can_Access_Below_Model; }
   void impl_Can_Access_Below_Model( bool Can_Access_Below_Model ) { v_Can_Access_Below_Model = Can_Access_Below_Model; }

   const string& impl_Child_Rel_Field_Key( ) const { return lazy_fetch( p_obj ), v_Child_Rel_Field_Key; }
   void impl_Child_Rel_Field_Key( const string& Child_Rel_Field_Key ) { sanity_check( Child_Rel_Field_Key ); v_Child_Rel_Field_Key = Child_Rel_Field_Key; }

   const string& impl_Comments( ) const { return lazy_fetch( p_obj ), v_Comments; }
   void impl_Comments( const string& Comments ) { sanity_check( Comments ); v_Comments = Comments; }

   const string& impl_Field_Pairs( ) const { return lazy_fetch( p_obj ), v_Field_Pairs; }
   void impl_Field_Pairs( const string& Field_Pairs ) { sanity_check( Field_Pairs ); v_Field_Pairs = Field_Pairs; }

   const string& impl_Field_Values( ) const { return lazy_fetch( p_obj ), v_Field_Values; }
   void impl_Field_Values( const string& Field_Values ) { sanity_check( Field_Values ); v_Field_Values = Field_Values; }

   const string& impl_Fields( ) const { return lazy_fetch( p_obj ), v_Fields; }
   void impl_Fields( const string& Fields ) { sanity_check( Fields ); v_Fields = Fields; }

   const string& impl_Id( ) const { return lazy_fetch( p_obj ), v_Id; }
   void impl_Id( const string& Id ) { sanity_check( Id ); v_Id = Id; }

   bool impl_Is_Required_For_UI_Gen( ) const { return lazy_fetch( p_obj ), v_Is_Required_For_UI_Gen; }
   void impl_Is_Required_For_UI_Gen( bool Is_Required_For_UI_Gen ) { v_Is_Required_For_UI_Gen = Is_Required_For_UI_Gen; }

   const string& impl_Name( ) const { return lazy_fetch( p_obj ), v_Name; }
   void impl_Name( const string& Name ) { sanity_check( Name ); v_Name = Name; }

   const string& impl_Options( ) const { return lazy_fetch( p_obj ), v_Options; }
   void impl_Options( const string& Options ) { sanity_check( Options ); v_Options = Options; }

   const string& impl_Order( ) const { return lazy_fetch( p_obj ), v_Order; }
   void impl_Order( const string& Order ) { sanity_check( Order ); v_Order = Order; }

   bool impl_Protect_Child_Rel( ) const { return lazy_fetch( p_obj ), v_Protect_Child_Rel; }
   void impl_Protect_Child_Rel( bool Protect_Child_Rel ) { v_Protect_Child_Rel = Protect_Child_Rel; }

   bool impl_Protect_Procedure( ) const { return lazy_fetch( p_obj ), v_Protect_Procedure; }
   void impl_Protect_Procedure( bool Protect_Procedure ) { v_Protect_Procedure = Protect_Procedure; }

   bool impl_Protect_Source_Parent( ) const { return lazy_fetch( p_obj ), v_Protect_Source_Parent; }
   void impl_Protect_Source_Parent( bool Protect_Source_Parent ) { v_Protect_Source_Parent = Protect_Source_Parent; }

   const string& impl_Restrict_Values( ) const { return lazy_fetch( p_obj ), v_Restrict_Values; }
   void impl_Restrict_Values( const string& Restrict_Values ) { sanity_check( Restrict_Values ); v_Restrict_Values = Restrict_Values; }

   const string& impl_Strings( ) const { return lazy_fetch( p_obj ), v_Strings; }
   void impl_Strings( const string& Strings ) { sanity_check( Strings ); v_Strings = Strings; }

   const string& impl_Test_Value( ) const { return lazy_fetch( p_obj ), v_Test_Value; }
   void impl_Test_Value( const string& Test_Value ) { sanity_check( Test_Value ); v_Test_Value = Test_Value; }

   const string& impl_Test_Value_Label( ) const { return lazy_fetch( p_obj ), v_Test_Value_Label; }
   void impl_Test_Value_Label( const string& Test_Value_Label ) { sanity_check( Test_Value_Label ); v_Test_Value_Label = Test_Value_Label; }

   bool impl_Use_Source_Parent( ) const { return lazy_fetch( p_obj ), v_Use_Source_Parent; }
   void impl_Use_Source_Parent( bool Use_Source_Parent ) { v_Use_Source_Parent = Use_Source_Parent; }

   bool impl_Use_Test_Parent_Child( ) const { return lazy_fetch( p_obj ), v_Use_Test_Parent_Child; }
   void impl_Use_Test_Parent_Child( bool Use_Test_Parent_Child ) { v_Use_Test_Parent_Child = Use_Test_Parent_Child; }

   const string& impl_Value( ) const { return lazy_fetch( p_obj ), v_Value; }
   void impl_Value( const string& Value ) { sanity_check( Value ); v_Value = Value; }

   const string& impl_Value_Label( ) const { return lazy_fetch( p_obj ), v_Value_Label; }
   void impl_Value_Label( const string& Value_Label ) { sanity_check( Value_Label ); v_Value_Label = Value_Label; }

   const string& impl_Value_Left_Part( ) const { return lazy_fetch( p_obj ), v_Value_Left_Part; }
   void impl_Value_Left_Part( const string& Value_Left_Part ) { sanity_check( Value_Left_Part ); v_Value_Left_Part = Value_Left_Part; }

   const string& impl_Value_Literal( ) const { return lazy_fetch( p_obj ), v_Value_Literal; }
   void impl_Value_Literal( const string& Value_Literal ) { sanity_check( Value_Literal ); v_Value_Literal = Value_Literal; }

   const string& impl_Value_Numeric_String( ) const { return lazy_fetch( p_obj ), v_Value_Numeric_String; }
   void impl_Value_Numeric_String( const string& Value_Numeric_String ) { sanity_check( Value_Numeric_String ); v_Value_Numeric_String = Value_Numeric_String; }

   const string& impl_Value_Right_Part( ) const { return lazy_fetch( p_obj ), v_Value_Right_Part; }
   void impl_Value_Right_Part( const string& Value_Right_Part ) { sanity_check( Value_Right_Part ); v_Value_Right_Part = Value_Right_Part; }

   const string& impl_Value_String( ) const { return lazy_fetch( p_obj ), v_Value_String; }
   void impl_Value_String( const string& Value_String ) { sanity_check( Value_String ); v_Value_String = Value_String; }

   const string& impl_Vars( ) const { return lazy_fetch( p_obj ), v_Vars; }
   void impl_Vars( const string& Vars ) { sanity_check( Vars ); v_Vars = Vars; }

   Meta_Class& impl_Child_Rel_Child_Class( )
   {
      if( !cp_Child_Rel_Child_Class )
      {
         cp_Child_Rel_Child_Class.init( );

         p_obj->setup_graph_parent( *cp_Child_Rel_Child_Class, c_field_id_Child_Rel_Child_Class, v_Child_Rel_Child_Class );
      }
      return *cp_Child_Rel_Child_Class;
   }

   const Meta_Class& impl_Child_Rel_Child_Class( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Child_Rel_Child_Class )
      {
         cp_Child_Rel_Child_Class.init( );

         p_obj->setup_graph_parent( *cp_Child_Rel_Child_Class, c_field_id_Child_Rel_Child_Class, v_Child_Rel_Child_Class );
      }
      return *cp_Child_Rel_Child_Class;
   }

   void impl_Child_Rel_Child_Class( const string& key )
   {
      class_base_accessor cba( impl_Child_Rel_Child_Class( ) );
      cba.set_key( key );
   }

   Meta_Relationship& impl_Child_Relationship( )
   {
      if( !cp_Child_Relationship )
      {
         cp_Child_Relationship.init( );

         p_obj->setup_graph_parent( *cp_Child_Relationship, c_field_id_Child_Relationship, v_Child_Relationship );
      }
      return *cp_Child_Relationship;
   }

   const Meta_Relationship& impl_Child_Relationship( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Child_Relationship )
      {
         cp_Child_Relationship.init( );

         p_obj->setup_graph_parent( *cp_Child_Relationship, c_field_id_Child_Relationship, v_Child_Relationship );
      }
      return *cp_Child_Relationship;
   }

   void impl_Child_Relationship( const string& key )
   {
      class_base_accessor cba( impl_Child_Relationship( ) );
      cba.set_key( key );
   }

   Meta_Specification_Type& impl_Child_Specification_Type( )
   {
      if( !cp_Child_Specification_Type )
      {
         cp_Child_Specification_Type.init( );

         p_obj->setup_graph_parent( *cp_Child_Specification_Type, c_field_id_Child_Specification_Type, v_Child_Specification_Type );
      }
      return *cp_Child_Specification_Type;
   }

   const Meta_Specification_Type& impl_Child_Specification_Type( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Child_Specification_Type )
      {
         cp_Child_Specification_Type.init( );

         p_obj->setup_graph_parent( *cp_Child_Specification_Type, c_field_id_Child_Specification_Type, v_Child_Specification_Type );
      }
      return *cp_Child_Specification_Type;
   }

   void impl_Child_Specification_Type( const string& key )
   {
      class_base_accessor cba( impl_Child_Specification_Type( ) );
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

   Meta_Enum& impl_Enum( )
   {
      if( !cp_Enum )
      {
         cp_Enum.init( );

         p_obj->setup_graph_parent( *cp_Enum, c_field_id_Enum, v_Enum );
      }
      return *cp_Enum;
   }

   const Meta_Enum& impl_Enum( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Enum )
      {
         cp_Enum.init( );

         p_obj->setup_graph_parent( *cp_Enum, c_field_id_Enum, v_Enum );
      }
      return *cp_Enum;
   }

   void impl_Enum( const string& key )
   {
      class_base_accessor cba( impl_Enum( ) );
      cba.set_key( key );
   }

   Meta_Enum_Item& impl_Enum_Item( )
   {
      if( !cp_Enum_Item )
      {
         cp_Enum_Item.init( );

         p_obj->setup_graph_parent( *cp_Enum_Item, c_field_id_Enum_Item, v_Enum_Item );
      }
      return *cp_Enum_Item;
   }

   const Meta_Enum_Item& impl_Enum_Item( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Enum_Item )
      {
         cp_Enum_Item.init( );

         p_obj->setup_graph_parent( *cp_Enum_Item, c_field_id_Enum_Item, v_Enum_Item );
      }
      return *cp_Enum_Item;
   }

   void impl_Enum_Item( const string& key )
   {
      class_base_accessor cba( impl_Enum_Item( ) );
      cba.set_key( key );
   }

   Meta_Enum_Item& impl_Enum_Item_2( )
   {
      if( !cp_Enum_Item_2 )
      {
         cp_Enum_Item_2.init( );

         p_obj->setup_graph_parent( *cp_Enum_Item_2, c_field_id_Enum_Item_2, v_Enum_Item_2 );
      }
      return *cp_Enum_Item_2;
   }

   const Meta_Enum_Item& impl_Enum_Item_2( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Enum_Item_2 )
      {
         cp_Enum_Item_2.init( );

         p_obj->setup_graph_parent( *cp_Enum_Item_2, c_field_id_Enum_Item_2, v_Enum_Item_2 );
      }
      return *cp_Enum_Item_2;
   }

   void impl_Enum_Item_2( const string& key )
   {
      class_base_accessor cba( impl_Enum_Item_2( ) );
      cba.set_key( key );
   }

   Meta_Enum_Item& impl_Enum_Item_3( )
   {
      if( !cp_Enum_Item_3 )
      {
         cp_Enum_Item_3.init( );

         p_obj->setup_graph_parent( *cp_Enum_Item_3, c_field_id_Enum_Item_3, v_Enum_Item_3 );
      }
      return *cp_Enum_Item_3;
   }

   const Meta_Enum_Item& impl_Enum_Item_3( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Enum_Item_3 )
      {
         cp_Enum_Item_3.init( );

         p_obj->setup_graph_parent( *cp_Enum_Item_3, c_field_id_Enum_Item_3, v_Enum_Item_3 );
      }
      return *cp_Enum_Item_3;
   }

   void impl_Enum_Item_3( const string& key )
   {
      class_base_accessor cba( impl_Enum_Item_3( ) );
      cba.set_key( key );
   }

   Meta_Enum_Item& impl_Enum_Item_4( )
   {
      if( !cp_Enum_Item_4 )
      {
         cp_Enum_Item_4.init( );

         p_obj->setup_graph_parent( *cp_Enum_Item_4, c_field_id_Enum_Item_4, v_Enum_Item_4 );
      }
      return *cp_Enum_Item_4;
   }

   const Meta_Enum_Item& impl_Enum_Item_4( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Enum_Item_4 )
      {
         cp_Enum_Item_4.init( );

         p_obj->setup_graph_parent( *cp_Enum_Item_4, c_field_id_Enum_Item_4, v_Enum_Item_4 );
      }
      return *cp_Enum_Item_4;
   }

   void impl_Enum_Item_4( const string& key )
   {
      class_base_accessor cba( impl_Enum_Item_4( ) );
      cba.set_key( key );
   }

   Meta_Enum_Item& impl_Enum_Item_5( )
   {
      if( !cp_Enum_Item_5 )
      {
         cp_Enum_Item_5.init( );

         p_obj->setup_graph_parent( *cp_Enum_Item_5, c_field_id_Enum_Item_5, v_Enum_Item_5 );
      }
      return *cp_Enum_Item_5;
   }

   const Meta_Enum_Item& impl_Enum_Item_5( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Enum_Item_5 )
      {
         cp_Enum_Item_5.init( );

         p_obj->setup_graph_parent( *cp_Enum_Item_5, c_field_id_Enum_Item_5, v_Enum_Item_5 );
      }
      return *cp_Enum_Item_5;
   }

   void impl_Enum_Item_5( const string& key )
   {
      class_base_accessor cba( impl_Enum_Item_5( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Field( )
   {
      if( !cp_Field )
      {
         cp_Field.init( );

         p_obj->setup_graph_parent( *cp_Field, c_field_id_Field, v_Field );
      }
      return *cp_Field;
   }

   const Meta_Field& impl_Field( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Field )
      {
         cp_Field.init( );

         p_obj->setup_graph_parent( *cp_Field, c_field_id_Field, v_Field );
      }
      return *cp_Field;
   }

   void impl_Field( const string& key )
   {
      class_base_accessor cba( impl_Field( ) );
      cba.set_key( key );
   }

   Meta_Class& impl_Field_Class( )
   {
      if( !cp_Field_Class )
      {
         cp_Field_Class.init( );

         p_obj->setup_graph_parent( *cp_Field_Class, c_field_id_Field_Class, v_Field_Class );
      }
      return *cp_Field_Class;
   }

   const Meta_Class& impl_Field_Class( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Field_Class )
      {
         cp_Field_Class.init( );

         p_obj->setup_graph_parent( *cp_Field_Class, c_field_id_Field_Class, v_Field_Class );
      }
      return *cp_Field_Class;
   }

   void impl_Field_Class( const string& key )
   {
      class_base_accessor cba( impl_Field_Class( ) );
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

   Meta_Modifier& impl_Modifier( )
   {
      if( !cp_Modifier )
      {
         cp_Modifier.init( );

         p_obj->setup_graph_parent( *cp_Modifier, c_field_id_Modifier, v_Modifier );
      }
      return *cp_Modifier;
   }

   const Meta_Modifier& impl_Modifier( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Modifier )
      {
         cp_Modifier.init( );

         p_obj->setup_graph_parent( *cp_Modifier, c_field_id_Modifier, v_Modifier );
      }
      return *cp_Modifier;
   }

   void impl_Modifier( const string& key )
   {
      class_base_accessor cba( impl_Modifier( ) );
      cba.set_key( key );
   }

   Meta_Class& impl_Other_Class( )
   {
      if( !cp_Other_Class )
      {
         cp_Other_Class.init( );

         p_obj->setup_graph_parent( *cp_Other_Class, c_field_id_Other_Class, v_Other_Class );
      }
      return *cp_Other_Class;
   }

   const Meta_Class& impl_Other_Class( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Other_Class )
      {
         cp_Other_Class.init( );

         p_obj->setup_graph_parent( *cp_Other_Class, c_field_id_Other_Class, v_Other_Class );
      }
      return *cp_Other_Class;
   }

   void impl_Other_Class( const string& key )
   {
      class_base_accessor cba( impl_Other_Class( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Other_Class_Field( )
   {
      if( !cp_Other_Class_Field )
      {
         cp_Other_Class_Field.init( );

         p_obj->setup_graph_parent( *cp_Other_Class_Field, c_field_id_Other_Class_Field, v_Other_Class_Field );
      }
      return *cp_Other_Class_Field;
   }

   const Meta_Field& impl_Other_Class_Field( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Other_Class_Field )
      {
         cp_Other_Class_Field.init( );

         p_obj->setup_graph_parent( *cp_Other_Class_Field, c_field_id_Other_Class_Field, v_Other_Class_Field );
      }
      return *cp_Other_Class_Field;
   }

   void impl_Other_Class_Field( const string& key )
   {
      class_base_accessor cba( impl_Other_Class_Field( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Other_Field( )
   {
      if( !cp_Other_Field )
      {
         cp_Other_Field.init( );

         p_obj->setup_graph_parent( *cp_Other_Field, c_field_id_Other_Field, v_Other_Field );
      }
      return *cp_Other_Field;
   }

   const Meta_Field& impl_Other_Field( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Other_Field )
      {
         cp_Other_Field.init( );

         p_obj->setup_graph_parent( *cp_Other_Field, c_field_id_Other_Field, v_Other_Field );
      }
      return *cp_Other_Field;
   }

   void impl_Other_Field( const string& key )
   {
      class_base_accessor cba( impl_Other_Field( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Other_Field_2( )
   {
      if( !cp_Other_Field_2 )
      {
         cp_Other_Field_2.init( );

         p_obj->setup_graph_parent( *cp_Other_Field_2, c_field_id_Other_Field_2, v_Other_Field_2 );
      }
      return *cp_Other_Field_2;
   }

   const Meta_Field& impl_Other_Field_2( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Other_Field_2 )
      {
         cp_Other_Field_2.init( );

         p_obj->setup_graph_parent( *cp_Other_Field_2, c_field_id_Other_Field_2, v_Other_Field_2 );
      }
      return *cp_Other_Field_2;
   }

   void impl_Other_Field_2( const string& key )
   {
      class_base_accessor cba( impl_Other_Field_2( ) );
      cba.set_key( key );
   }

   Meta_Modifier& impl_Other_Modifier( )
   {
      if( !cp_Other_Modifier )
      {
         cp_Other_Modifier.init( );

         p_obj->setup_graph_parent( *cp_Other_Modifier, c_field_id_Other_Modifier, v_Other_Modifier );
      }
      return *cp_Other_Modifier;
   }

   const Meta_Modifier& impl_Other_Modifier( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Other_Modifier )
      {
         cp_Other_Modifier.init( );

         p_obj->setup_graph_parent( *cp_Other_Modifier, c_field_id_Other_Modifier, v_Other_Modifier );
      }
      return *cp_Other_Modifier;
   }

   void impl_Other_Modifier( const string& key )
   {
      class_base_accessor cba( impl_Other_Modifier( ) );
      cba.set_key( key );
   }

   Meta_Modifier& impl_Other_Modifier_2( )
   {
      if( !cp_Other_Modifier_2 )
      {
         cp_Other_Modifier_2.init( );

         p_obj->setup_graph_parent( *cp_Other_Modifier_2, c_field_id_Other_Modifier_2, v_Other_Modifier_2 );
      }
      return *cp_Other_Modifier_2;
   }

   const Meta_Modifier& impl_Other_Modifier_2( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Other_Modifier_2 )
      {
         cp_Other_Modifier_2.init( );

         p_obj->setup_graph_parent( *cp_Other_Modifier_2, c_field_id_Other_Modifier_2, v_Other_Modifier_2 );
      }
      return *cp_Other_Modifier_2;
   }

   void impl_Other_Modifier_2( const string& key )
   {
      class_base_accessor cba( impl_Other_Modifier_2( ) );
      cba.set_key( key );
   }

   Meta_Permission& impl_Other_Permission( )
   {
      if( !cp_Other_Permission )
      {
         cp_Other_Permission.init( );

         p_obj->setup_graph_parent( *cp_Other_Permission, c_field_id_Other_Permission, v_Other_Permission );
      }
      return *cp_Other_Permission;
   }

   const Meta_Permission& impl_Other_Permission( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Other_Permission )
      {
         cp_Other_Permission.init( );

         p_obj->setup_graph_parent( *cp_Other_Permission, c_field_id_Other_Permission, v_Other_Permission );
      }
      return *cp_Other_Permission;
   }

   void impl_Other_Permission( const string& key )
   {
      class_base_accessor cba( impl_Other_Permission( ) );
      cba.set_key( key );
   }

   Meta_Permission& impl_Other_Permission_2( )
   {
      if( !cp_Other_Permission_2 )
      {
         cp_Other_Permission_2.init( );

         p_obj->setup_graph_parent( *cp_Other_Permission_2, c_field_id_Other_Permission_2, v_Other_Permission_2 );
      }
      return *cp_Other_Permission_2;
   }

   const Meta_Permission& impl_Other_Permission_2( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Other_Permission_2 )
      {
         cp_Other_Permission_2.init( );

         p_obj->setup_graph_parent( *cp_Other_Permission_2, c_field_id_Other_Permission_2, v_Other_Permission_2 );
      }
      return *cp_Other_Permission_2;
   }

   void impl_Other_Permission_2( const string& key )
   {
      class_base_accessor cba( impl_Other_Permission_2( ) );
      cba.set_key( key );
   }

   Meta_Procedure& impl_Other_Procedure( )
   {
      if( !cp_Other_Procedure )
      {
         cp_Other_Procedure.init( );

         p_obj->setup_graph_parent( *cp_Other_Procedure, c_field_id_Other_Procedure, v_Other_Procedure );
      }
      return *cp_Other_Procedure;
   }

   const Meta_Procedure& impl_Other_Procedure( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Other_Procedure )
      {
         cp_Other_Procedure.init( );

         p_obj->setup_graph_parent( *cp_Other_Procedure, c_field_id_Other_Procedure, v_Other_Procedure );
      }
      return *cp_Other_Procedure;
   }

   void impl_Other_Procedure( const string& key )
   {
      class_base_accessor cba( impl_Other_Procedure( ) );
      cba.set_key( key );
   }

   Meta_Procedure& impl_Other_Procedure_2( )
   {
      if( !cp_Other_Procedure_2 )
      {
         cp_Other_Procedure_2.init( );

         p_obj->setup_graph_parent( *cp_Other_Procedure_2, c_field_id_Other_Procedure_2, v_Other_Procedure_2 );
      }
      return *cp_Other_Procedure_2;
   }

   const Meta_Procedure& impl_Other_Procedure_2( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Other_Procedure_2 )
      {
         cp_Other_Procedure_2.init( );

         p_obj->setup_graph_parent( *cp_Other_Procedure_2, c_field_id_Other_Procedure_2, v_Other_Procedure_2 );
      }
      return *cp_Other_Procedure_2;
   }

   void impl_Other_Procedure_2( const string& key )
   {
      class_base_accessor cba( impl_Other_Procedure_2( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Other_Source_Child( )
   {
      if( !cp_Other_Source_Child )
      {
         cp_Other_Source_Child.init( );

         p_obj->setup_graph_parent( *cp_Other_Source_Child, c_field_id_Other_Source_Child, v_Other_Source_Child );
      }
      return *cp_Other_Source_Child;
   }

   const Meta_Field& impl_Other_Source_Child( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Other_Source_Child )
      {
         cp_Other_Source_Child.init( );

         p_obj->setup_graph_parent( *cp_Other_Source_Child, c_field_id_Other_Source_Child, v_Other_Source_Child );
      }
      return *cp_Other_Source_Child;
   }

   void impl_Other_Source_Child( const string& key )
   {
      class_base_accessor cba( impl_Other_Source_Child( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Other_Source_Child_2( )
   {
      if( !cp_Other_Source_Child_2 )
      {
         cp_Other_Source_Child_2.init( );

         p_obj->setup_graph_parent( *cp_Other_Source_Child_2, c_field_id_Other_Source_Child_2, v_Other_Source_Child_2 );
      }
      return *cp_Other_Source_Child_2;
   }

   const Meta_Field& impl_Other_Source_Child_2( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Other_Source_Child_2 )
      {
         cp_Other_Source_Child_2.init( );

         p_obj->setup_graph_parent( *cp_Other_Source_Child_2, c_field_id_Other_Source_Child_2, v_Other_Source_Child_2 );
      }
      return *cp_Other_Source_Child_2;
   }

   void impl_Other_Source_Child_2( const string& key )
   {
      class_base_accessor cba( impl_Other_Source_Child_2( ) );
      cba.set_key( key );
   }

   Meta_Specification& impl_Parent_Specification( )
   {
      if( !cp_Parent_Specification )
      {
         cp_Parent_Specification.init( );

         p_obj->setup_graph_parent( *cp_Parent_Specification, c_field_id_Parent_Specification, v_Parent_Specification );
      }
      return *cp_Parent_Specification;
   }

   const Meta_Specification& impl_Parent_Specification( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Parent_Specification )
      {
         cp_Parent_Specification.init( );

         p_obj->setup_graph_parent( *cp_Parent_Specification, c_field_id_Parent_Specification, v_Parent_Specification );
      }
      return *cp_Parent_Specification;
   }

   void impl_Parent_Specification( const string& key )
   {
      class_base_accessor cba( impl_Parent_Specification( ) );
      cba.set_key( key );
   }

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

   Meta_Procedure& impl_Procedure( )
   {
      if( !cp_Procedure )
      {
         cp_Procedure.init( );

         p_obj->setup_graph_parent( *cp_Procedure, c_field_id_Procedure, v_Procedure );
      }
      return *cp_Procedure;
   }

   const Meta_Procedure& impl_Procedure( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Procedure )
      {
         cp_Procedure.init( );

         p_obj->setup_graph_parent( *cp_Procedure, c_field_id_Procedure, v_Procedure );
      }
      return *cp_Procedure;
   }

   void impl_Procedure( const string& key )
   {
      class_base_accessor cba( impl_Procedure( ) );
      cba.set_key( key );
   }

   Meta_Procedure_Arg& impl_Procedure_Arg( )
   {
      if( !cp_Procedure_Arg )
      {
         cp_Procedure_Arg.init( );

         p_obj->setup_graph_parent( *cp_Procedure_Arg, c_field_id_Procedure_Arg, v_Procedure_Arg );
      }
      return *cp_Procedure_Arg;
   }

   const Meta_Procedure_Arg& impl_Procedure_Arg( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Procedure_Arg )
      {
         cp_Procedure_Arg.init( );

         p_obj->setup_graph_parent( *cp_Procedure_Arg, c_field_id_Procedure_Arg, v_Procedure_Arg );
      }
      return *cp_Procedure_Arg;
   }

   void impl_Procedure_Arg( const string& key )
   {
      class_base_accessor cba( impl_Procedure_Arg( ) );
      cba.set_key( key );
   }

   Meta_Procedure_Arg& impl_Procedure_Arg_2( )
   {
      if( !cp_Procedure_Arg_2 )
      {
         cp_Procedure_Arg_2.init( );

         p_obj->setup_graph_parent( *cp_Procedure_Arg_2, c_field_id_Procedure_Arg_2, v_Procedure_Arg_2 );
      }
      return *cp_Procedure_Arg_2;
   }

   const Meta_Procedure_Arg& impl_Procedure_Arg_2( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Procedure_Arg_2 )
      {
         cp_Procedure_Arg_2.init( );

         p_obj->setup_graph_parent( *cp_Procedure_Arg_2, c_field_id_Procedure_Arg_2, v_Procedure_Arg_2 );
      }
      return *cp_Procedure_Arg_2;
   }

   void impl_Procedure_Arg_2( const string& key )
   {
      class_base_accessor cba( impl_Procedure_Arg_2( ) );
      cba.set_key( key );
   }

   Meta_Procedure_Arg& impl_Procedure_Arg_3( )
   {
      if( !cp_Procedure_Arg_3 )
      {
         cp_Procedure_Arg_3.init( );

         p_obj->setup_graph_parent( *cp_Procedure_Arg_3, c_field_id_Procedure_Arg_3, v_Procedure_Arg_3 );
      }
      return *cp_Procedure_Arg_3;
   }

   const Meta_Procedure_Arg& impl_Procedure_Arg_3( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Procedure_Arg_3 )
      {
         cp_Procedure_Arg_3.init( );

         p_obj->setup_graph_parent( *cp_Procedure_Arg_3, c_field_id_Procedure_Arg_3, v_Procedure_Arg_3 );
      }
      return *cp_Procedure_Arg_3;
   }

   void impl_Procedure_Arg_3( const string& key )
   {
      class_base_accessor cba( impl_Procedure_Arg_3( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Source_Child( )
   {
      if( !cp_Source_Child )
      {
         cp_Source_Child.init( );

         p_obj->setup_graph_parent( *cp_Source_Child, c_field_id_Source_Child, v_Source_Child );
      }
      return *cp_Source_Child;
   }

   const Meta_Field& impl_Source_Child( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Source_Child )
      {
         cp_Source_Child.init( );

         p_obj->setup_graph_parent( *cp_Source_Child, c_field_id_Source_Child, v_Source_Child );
      }
      return *cp_Source_Child;
   }

   void impl_Source_Child( const string& key )
   {
      class_base_accessor cba( impl_Source_Child( ) );
      cba.set_key( key );
   }

   Meta_Class& impl_Source_Child_Class( )
   {
      if( !cp_Source_Child_Class )
      {
         cp_Source_Child_Class.init( );

         p_obj->setup_graph_parent( *cp_Source_Child_Class, c_field_id_Source_Child_Class, v_Source_Child_Class );
      }
      return *cp_Source_Child_Class;
   }

   const Meta_Class& impl_Source_Child_Class( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Source_Child_Class )
      {
         cp_Source_Child_Class.init( );

         p_obj->setup_graph_parent( *cp_Source_Child_Class, c_field_id_Source_Child_Class, v_Source_Child_Class );
      }
      return *cp_Source_Child_Class;
   }

   void impl_Source_Child_Class( const string& key )
   {
      class_base_accessor cba( impl_Source_Child_Class( ) );
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

   Meta_Field& impl_Source_Field( )
   {
      if( !cp_Source_Field )
      {
         cp_Source_Field.init( );

         p_obj->setup_graph_parent( *cp_Source_Field, c_field_id_Source_Field, v_Source_Field );
      }
      return *cp_Source_Field;
   }

   const Meta_Field& impl_Source_Field( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Source_Field )
      {
         cp_Source_Field.init( );

         p_obj->setup_graph_parent( *cp_Source_Field, c_field_id_Source_Field, v_Source_Field );
      }
      return *cp_Source_Field;
   }

   void impl_Source_Field( const string& key )
   {
      class_base_accessor cba( impl_Source_Field( ) );
      cba.set_key( key );
   }

   Meta_Class& impl_Source_Field_Class( )
   {
      if( !cp_Source_Field_Class )
      {
         cp_Source_Field_Class.init( );

         p_obj->setup_graph_parent( *cp_Source_Field_Class, c_field_id_Source_Field_Class, v_Source_Field_Class );
      }
      return *cp_Source_Field_Class;
   }

   const Meta_Class& impl_Source_Field_Class( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Source_Field_Class )
      {
         cp_Source_Field_Class.init( );

         p_obj->setup_graph_parent( *cp_Source_Field_Class, c_field_id_Source_Field_Class, v_Source_Field_Class );
      }
      return *cp_Source_Field_Class;
   }

   void impl_Source_Field_Class( const string& key )
   {
      class_base_accessor cba( impl_Source_Field_Class( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Source_Grandchild( )
   {
      if( !cp_Source_Grandchild )
      {
         cp_Source_Grandchild.init( );

         p_obj->setup_graph_parent( *cp_Source_Grandchild, c_field_id_Source_Grandchild, v_Source_Grandchild );
      }
      return *cp_Source_Grandchild;
   }

   const Meta_Field& impl_Source_Grandchild( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Source_Grandchild )
      {
         cp_Source_Grandchild.init( );

         p_obj->setup_graph_parent( *cp_Source_Grandchild, c_field_id_Source_Grandchild, v_Source_Grandchild );
      }
      return *cp_Source_Grandchild;
   }

   void impl_Source_Grandchild( const string& key )
   {
      class_base_accessor cba( impl_Source_Grandchild( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Source_Parent( )
   {
      if( !cp_Source_Parent )
      {
         cp_Source_Parent.init( );

         p_obj->setup_graph_parent( *cp_Source_Parent, c_field_id_Source_Parent, v_Source_Parent );
      }
      return *cp_Source_Parent;
   }

   const Meta_Field& impl_Source_Parent( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Source_Parent )
      {
         cp_Source_Parent.init( );

         p_obj->setup_graph_parent( *cp_Source_Parent, c_field_id_Source_Parent, v_Source_Parent );
      }
      return *cp_Source_Parent;
   }

   void impl_Source_Parent( const string& key )
   {
      class_base_accessor cba( impl_Source_Parent( ) );
      cba.set_key( key );
   }

   Meta_Class& impl_Source_Parent_Class( )
   {
      if( !cp_Source_Parent_Class )
      {
         cp_Source_Parent_Class.init( );

         p_obj->setup_graph_parent( *cp_Source_Parent_Class, c_field_id_Source_Parent_Class, v_Source_Parent_Class );
      }
      return *cp_Source_Parent_Class;
   }

   const Meta_Class& impl_Source_Parent_Class( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Source_Parent_Class )
      {
         cp_Source_Parent_Class.init( );

         p_obj->setup_graph_parent( *cp_Source_Parent_Class, c_field_id_Source_Parent_Class, v_Source_Parent_Class );
      }
      return *cp_Source_Parent_Class;
   }

   void impl_Source_Parent_Class( const string& key )
   {
      class_base_accessor cba( impl_Source_Parent_Class( ) );
      cba.set_key( key );
   }

   Meta_Specification_Type& impl_Specification_Type( )
   {
      if( !cp_Specification_Type )
      {
         cp_Specification_Type.init( );

         p_obj->setup_graph_parent( *cp_Specification_Type, c_field_id_Specification_Type, v_Specification_Type );
      }
      return *cp_Specification_Type;
   }

   const Meta_Specification_Type& impl_Specification_Type( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Specification_Type )
      {
         cp_Specification_Type.init( );

         p_obj->setup_graph_parent( *cp_Specification_Type, c_field_id_Specification_Type, v_Specification_Type );
      }
      return *cp_Specification_Type;
   }

   void impl_Specification_Type( const string& key )
   {
      class_base_accessor cba( impl_Specification_Type( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Test_Child( )
   {
      if( !cp_Test_Child )
      {
         cp_Test_Child.init( );

         p_obj->setup_graph_parent( *cp_Test_Child, c_field_id_Test_Child, v_Test_Child );
      }
      return *cp_Test_Child;
   }

   const Meta_Field& impl_Test_Child( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Test_Child )
      {
         cp_Test_Child.init( );

         p_obj->setup_graph_parent( *cp_Test_Child, c_field_id_Test_Child, v_Test_Child );
      }
      return *cp_Test_Child;
   }

   void impl_Test_Child( const string& key )
   {
      class_base_accessor cba( impl_Test_Child( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Test_Field( )
   {
      if( !cp_Test_Field )
      {
         cp_Test_Field.init( );

         p_obj->setup_graph_parent( *cp_Test_Field, c_field_id_Test_Field, v_Test_Field );
      }
      return *cp_Test_Field;
   }

   const Meta_Field& impl_Test_Field( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Test_Field )
      {
         cp_Test_Field.init( );

         p_obj->setup_graph_parent( *cp_Test_Field, c_field_id_Test_Field, v_Test_Field );
      }
      return *cp_Test_Field;
   }

   void impl_Test_Field( const string& key )
   {
      class_base_accessor cba( impl_Test_Field( ) );
      cba.set_key( key );
   }

   Meta_Class& impl_Test_Field_Class( )
   {
      if( !cp_Test_Field_Class )
      {
         cp_Test_Field_Class.init( );

         p_obj->setup_graph_parent( *cp_Test_Field_Class, c_field_id_Test_Field_Class, v_Test_Field_Class );
      }
      return *cp_Test_Field_Class;
   }

   const Meta_Class& impl_Test_Field_Class( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Test_Field_Class )
      {
         cp_Test_Field_Class.init( );

         p_obj->setup_graph_parent( *cp_Test_Field_Class, c_field_id_Test_Field_Class, v_Test_Field_Class );
      }
      return *cp_Test_Field_Class;
   }

   void impl_Test_Field_Class( const string& key )
   {
      class_base_accessor cba( impl_Test_Field_Class( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Test_Parent( )
   {
      if( !cp_Test_Parent )
      {
         cp_Test_Parent.init( );

         p_obj->setup_graph_parent( *cp_Test_Parent, c_field_id_Test_Parent, v_Test_Parent );
      }
      return *cp_Test_Parent;
   }

   const Meta_Field& impl_Test_Parent( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Test_Parent )
      {
         cp_Test_Parent.init( );

         p_obj->setup_graph_parent( *cp_Test_Parent, c_field_id_Test_Parent, v_Test_Parent );
      }
      return *cp_Test_Parent;
   }

   void impl_Test_Parent( const string& key )
   {
      class_base_accessor cba( impl_Test_Parent( ) );
      cba.set_key( key );
   }

   Meta_Class& impl_Test_Parent_Class( )
   {
      if( !cp_Test_Parent_Class )
      {
         cp_Test_Parent_Class.init( );

         p_obj->setup_graph_parent( *cp_Test_Parent_Class, c_field_id_Test_Parent_Class, v_Test_Parent_Class );
      }
      return *cp_Test_Parent_Class;
   }

   const Meta_Class& impl_Test_Parent_Class( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Test_Parent_Class )
      {
         cp_Test_Parent_Class.init( );

         p_obj->setup_graph_parent( *cp_Test_Parent_Class, c_field_id_Test_Parent_Class, v_Test_Parent_Class );
      }
      return *cp_Test_Parent_Class;
   }

   void impl_Test_Parent_Class( const string& key )
   {
      class_base_accessor cba( impl_Test_Parent_Class( ) );
      cba.set_key( key );
   }

   Meta_Specification& impl_child_Specification_Parent( )
   {
      if( !cp_child_Specification_Parent )
      {
         cp_child_Specification_Parent.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Parent, "301405" );
      }
      return *cp_child_Specification_Parent;
   }

   const Meta_Specification& impl_child_Specification_Parent( ) const
   {
      if( !cp_child_Specification_Parent )
      {
         cp_child_Specification_Parent.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Parent, "301405" );
      }
      return *cp_child_Specification_Parent;
   }

   Meta_List_Field& impl_child_List_Field_Restriction_Spec( )
   {
      if( !cp_child_List_Field_Restriction_Spec )
      {
         cp_child_List_Field_Restriction_Spec.init( );

         p_obj->setup_graph_parent( *cp_child_List_Field_Restriction_Spec, "302180" );
      }
      return *cp_child_List_Field_Restriction_Spec;
   }

   const Meta_List_Field& impl_child_List_Field_Restriction_Spec( ) const
   {
      if( !cp_child_List_Field_Restriction_Spec )
      {
         cp_child_List_Field_Restriction_Spec.init( );

         p_obj->setup_graph_parent( *cp_child_List_Field_Restriction_Spec, "302180" );
      }
      return *cp_child_List_Field_Restriction_Spec;
   }

   Meta_View_Field& impl_child_View_Field_Restriction_Spec( )
   {
      if( !cp_child_View_Field_Restriction_Spec )
      {
         cp_child_View_Field_Restriction_Spec.init( );

         p_obj->setup_graph_parent( *cp_child_View_Field_Restriction_Spec, "301980" );
      }
      return *cp_child_View_Field_Restriction_Spec;
   }

   const Meta_View_Field& impl_child_View_Field_Restriction_Spec( ) const
   {
      if( !cp_child_View_Field_Restriction_Spec )
      {
         cp_child_View_Field_Restriction_Spec.init( );

         p_obj->setup_graph_parent( *cp_child_View_Field_Restriction_Spec, "301980" );
      }
      return *cp_child_View_Field_Restriction_Spec;
   }

   void impl_Add_Arg_1( );

   void impl_Add_Arg_2( );

   void impl_Add_Child_Info( );

   void impl_Add_Destination_Info( );

   void impl_Add_Extra_Info( );

   void impl_Add_Field( );

   void impl_Add_Field_Info( );

   void impl_Add_Field_Pair( );

   void impl_Add_Grandchild_Info( );

   void impl_Add_Info( );

   void impl_Add_Next( );

   void impl_Add_Opt_Info( );

   void impl_Add_Other_Info( );

   void impl_Add_Secondary( );

   void impl_Add_Source_Info( );

   void impl_Add_Stats_1( );

   void impl_Add_Stats_2( );

   void impl_Add_Stats_Info( );

   void impl_Generate_All_Strings( string& All_Strings, const string& Parent_Prefix );

   void impl_Generate_All_Vars( string& All_Vars, const string& Parent_Prefix );

   void impl_Move_Down( const string& Restrict_Fields, const string& Restrict_Values );

   void impl_Move_Up( const string& Restrict_Fields, const string& Restrict_Values );

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

   Meta_Specification* p_obj;
   class_pointer< Meta_Specification > cp_obj;

   mutable set< string > required_transients;

   // [<start members>]
   // [<finish members>]

   size_t total_child_relationships;

   string v_Actions;
   string v_All_Strings;
   string v_All_Vars;
   bool v_Can_Access_Below_Model;
   string v_Child_Rel_Field_Key;
   string v_Comments;
   string v_Field_Pairs;
   string v_Field_Values;
   string v_Fields;
   string v_Id;
   bool v_Is_Required_For_UI_Gen;
   string v_Name;
   string v_Options;
   string v_Order;
   bool v_Protect_Child_Rel;
   bool v_Protect_Procedure;
   bool v_Protect_Source_Parent;
   string v_Restrict_Values;
   string v_Strings;
   string v_Test_Value;
   string v_Test_Value_Label;
   bool v_Use_Source_Parent;
   bool v_Use_Test_Parent_Child;
   string v_Value;
   string v_Value_Label;
   string v_Value_Left_Part;
   string v_Value_Literal;
   string v_Value_Numeric_String;
   string v_Value_Right_Part;
   string v_Value_String;
   string v_Vars;

   string v_Child_Rel_Child_Class;
   mutable class_pointer< Meta_Class > cp_Child_Rel_Child_Class;

   string v_Child_Relationship;
   mutable class_pointer< Meta_Relationship > cp_Child_Relationship;

   string v_Child_Specification_Type;
   mutable class_pointer< Meta_Specification_Type > cp_Child_Specification_Type;

   string v_Class;
   mutable class_pointer< Meta_Class > cp_Class;

   string v_Enum;
   mutable class_pointer< Meta_Enum > cp_Enum;

   string v_Enum_Item;
   mutable class_pointer< Meta_Enum_Item > cp_Enum_Item;

   string v_Enum_Item_2;
   mutable class_pointer< Meta_Enum_Item > cp_Enum_Item_2;

   string v_Enum_Item_3;
   mutable class_pointer< Meta_Enum_Item > cp_Enum_Item_3;

   string v_Enum_Item_4;
   mutable class_pointer< Meta_Enum_Item > cp_Enum_Item_4;

   string v_Enum_Item_5;
   mutable class_pointer< Meta_Enum_Item > cp_Enum_Item_5;

   string v_Field;
   mutable class_pointer< Meta_Field > cp_Field;

   string v_Field_Class;
   mutable class_pointer< Meta_Class > cp_Field_Class;

   string v_Model;
   mutable class_pointer< Meta_Model > cp_Model;

   string v_Modifier;
   mutable class_pointer< Meta_Modifier > cp_Modifier;

   string v_Other_Class;
   mutable class_pointer< Meta_Class > cp_Other_Class;

   string v_Other_Class_Field;
   mutable class_pointer< Meta_Field > cp_Other_Class_Field;

   string v_Other_Field;
   mutable class_pointer< Meta_Field > cp_Other_Field;

   string v_Other_Field_2;
   mutable class_pointer< Meta_Field > cp_Other_Field_2;

   string v_Other_Modifier;
   mutable class_pointer< Meta_Modifier > cp_Other_Modifier;

   string v_Other_Modifier_2;
   mutable class_pointer< Meta_Modifier > cp_Other_Modifier_2;

   string v_Other_Permission;
   mutable class_pointer< Meta_Permission > cp_Other_Permission;

   string v_Other_Permission_2;
   mutable class_pointer< Meta_Permission > cp_Other_Permission_2;

   string v_Other_Procedure;
   mutable class_pointer< Meta_Procedure > cp_Other_Procedure;

   string v_Other_Procedure_2;
   mutable class_pointer< Meta_Procedure > cp_Other_Procedure_2;

   string v_Other_Source_Child;
   mutable class_pointer< Meta_Field > cp_Other_Source_Child;

   string v_Other_Source_Child_2;
   mutable class_pointer< Meta_Field > cp_Other_Source_Child_2;

   string v_Parent_Specification;
   mutable class_pointer< Meta_Specification > cp_Parent_Specification;

   string v_Permission;
   mutable class_pointer< Meta_Permission > cp_Permission;

   string v_Procedure;
   mutable class_pointer< Meta_Procedure > cp_Procedure;

   string v_Procedure_Arg;
   mutable class_pointer< Meta_Procedure_Arg > cp_Procedure_Arg;

   string v_Procedure_Arg_2;
   mutable class_pointer< Meta_Procedure_Arg > cp_Procedure_Arg_2;

   string v_Procedure_Arg_3;
   mutable class_pointer< Meta_Procedure_Arg > cp_Procedure_Arg_3;

   string v_Source_Child;
   mutable class_pointer< Meta_Field > cp_Source_Child;

   string v_Source_Child_Class;
   mutable class_pointer< Meta_Class > cp_Source_Child_Class;

   string v_Source_Class;
   mutable class_pointer< Meta_Class > cp_Source_Class;

   string v_Source_Field;
   mutable class_pointer< Meta_Field > cp_Source_Field;

   string v_Source_Field_Class;
   mutable class_pointer< Meta_Class > cp_Source_Field_Class;

   string v_Source_Grandchild;
   mutable class_pointer< Meta_Field > cp_Source_Grandchild;

   string v_Source_Parent;
   mutable class_pointer< Meta_Field > cp_Source_Parent;

   string v_Source_Parent_Class;
   mutable class_pointer< Meta_Class > cp_Source_Parent_Class;

   string v_Specification_Type;
   mutable class_pointer< Meta_Specification_Type > cp_Specification_Type;

   string v_Test_Child;
   mutable class_pointer< Meta_Field > cp_Test_Child;

   string v_Test_Field;
   mutable class_pointer< Meta_Field > cp_Test_Field;

   string v_Test_Field_Class;
   mutable class_pointer< Meta_Class > cp_Test_Field_Class;

   string v_Test_Parent;
   mutable class_pointer< Meta_Field > cp_Test_Parent;

   string v_Test_Parent_Class;
   mutable class_pointer< Meta_Class > cp_Test_Parent_Class;

   mutable class_pointer< Meta_Specification > cp_child_Specification_Parent;
   mutable class_pointer< Meta_List_Field > cp_child_List_Field_Restriction_Spec;
   mutable class_pointer< Meta_View_Field > cp_child_View_Field_Restriction_Spec;
};

void Meta_Specification::impl::impl_Add_Arg_1( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start Add_Arg_1_impl>]
   // [<finish Add_Arg_1_impl>]
}

void Meta_Specification::impl::impl_Add_Arg_2( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start Add_Arg_2_impl>]
   // [<finish Add_Arg_2_impl>]
}

void Meta_Specification::impl::impl_Add_Child_Info( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start Add_Child_Info_impl>]
   // [<finish Add_Child_Info_impl>]
}

void Meta_Specification::impl::impl_Add_Destination_Info( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start Add_Destination_Info_impl>]
   // [<finish Add_Destination_Info_impl>]
}

void Meta_Specification::impl::impl_Add_Extra_Info( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start Add_Extra_Info_impl>]
   // [<finish Add_Extra_Info_impl>]
}

void Meta_Specification::impl::impl_Add_Field( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start Add_Field_impl>]
   // [<finish Add_Field_impl>]
}

void Meta_Specification::impl::impl_Add_Field_Info( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start Add_Field_Info_impl>]
   // [<finish Add_Field_Info_impl>]
}

void Meta_Specification::impl::impl_Add_Field_Pair( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start Add_Field_Pair_impl>]
   // [<finish Add_Field_Pair_impl>]
}

void Meta_Specification::impl::impl_Add_Grandchild_Info( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start Add_Grandchild_Info_impl>]
   // [<finish Add_Grandchild_Info_impl>]
}

void Meta_Specification::impl::impl_Add_Info( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start Add_Info_impl>]
   // [<finish Add_Info_impl>]
}

void Meta_Specification::impl::impl_Add_Next( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start Add_Next_impl>]
   // [<finish Add_Next_impl>]
}

void Meta_Specification::impl::impl_Add_Opt_Info( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start Add_Opt_Info_impl>]
   // [<finish Add_Opt_Info_impl>]
}

void Meta_Specification::impl::impl_Add_Other_Info( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start Add_Other_Info_impl>]
   // [<finish Add_Other_Info_impl>]
}

void Meta_Specification::impl::impl_Add_Secondary( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start Add_Secondary_impl>]
   // [<finish Add_Secondary_impl>]
}

void Meta_Specification::impl::impl_Add_Source_Info( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start Add_Source_Info_impl>]
   // [<finish Add_Source_Info_impl>]
}

void Meta_Specification::impl::impl_Add_Stats_1( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start Add_Stats_1_impl>]
   // [<finish Add_Stats_1_impl>]
}

void Meta_Specification::impl::impl_Add_Stats_2( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start Add_Stats_2_impl>]
   // [<finish Add_Stats_2_impl>]
}

void Meta_Specification::impl::impl_Add_Stats_Info( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start Add_Stats_Info_impl>]
   // [<finish Add_Stats_Info_impl>]
}

void Meta_Specification::impl::impl_Generate_All_Strings( string& All_Strings, const string& Parent_Prefix )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start Generate_All_Strings_impl>]
//nyi
   // NOTE: If a recovery is in progress or a cascade is in progress then do nothing.
   if( storage_locked_for_admin( ) || get_obj( ).get_is_being_cascaded( ) )
      return;

   if( get_obj( ).get_key( ).empty( ) )
      throw runtime_error( "unexpected call to Meta_Specification::Generate_All_Strings" );

   string strings( get_obj( ).Strings( ) );

   if( get_obj( ).child_Specification_Parent( ).iterate_forwards( "", to_string( c_field_id_Strings ) ) )
   {
      do
      {
         string child_strings;
         get_obj( ).child_Specification_Parent( ).Generate_All_Strings( child_strings, get_obj( ).Specification_Type( ).Child_Prefix( ) );

         if( !child_strings.empty( ) )
         {
            if( !strings.empty( ) )
               strings += '\n';
            strings += child_strings;
         }

      } while( get_obj( ).child_Specification_Parent( ).iterate_next( ) );
   }

   map< string, string > all_strings;
   string initial_strings( strings );
   string::size_type pos = 0;

   while( !strings.empty( ) )
   {
      pos = strings.find( '\n' );
      string next( strings.substr( 0, pos ) );

      if( !Parent_Prefix.empty( ) )
         next = Parent_Prefix + "_" + next;

      string::size_type npos = next.find( '=' );
      if( npos == string::npos )
         throw runtime_error( "unexpected format for specification strings '" + initial_strings + "'" );

      all_strings[ next.substr( 0, npos ) ] = next.substr( npos + 1 );

      if( pos == string::npos )
         break;

      strings.erase( 0, pos + 1 );
   }

   for( map< string, string >::const_iterator i = all_strings.begin( ), end = all_strings.end( ); i != end; ++i )
   {
      if( !All_Strings.empty( ) )
         All_Strings += '\n';
      All_Strings += i->first + '=' + i->second;
   }
   // [<finish Generate_All_Strings_impl>]
}

void Meta_Specification::impl::impl_Generate_All_Vars( string& All_Vars, const string& Parent_Prefix )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start Generate_All_Vars_impl>]
//nyi
   // NOTE: If a recovery is in progress or a cascade is in progress then do nothing.
   if( storage_locked_for_admin( ) || get_obj( ).get_is_being_cascaded( ) )
      return;

   if( get_obj( ).get_key( ).empty( ) )
      throw runtime_error( "unexpected call to Meta_Specification::Generate_All_Vars" );

   string vars( get_obj( ).Vars( ) );

   string def_vars( get_obj( ).Specification_Type( ).Default_Child_Vars( ) );
   if( !def_vars.empty( ) )
   {
      if( !vars.empty( ) )
         vars += '\n';
      vars += def_vars;
   }

   if( get_obj( ).child_Specification_Parent( ).iterate_forwards( "", to_string( c_field_id_Vars ) ) )
   {
      do
      {
         string child_vars;
         get_obj( ).child_Specification_Parent( ).Generate_All_Vars( child_vars, get_obj( ).Specification_Type( ).Child_Prefix( ) );

         if( !child_vars.empty( ) )
         {
            if( !vars.empty( ) )
               vars += '\n';
            vars += child_vars;
         }

      } while( get_obj( ).child_Specification_Parent( ).iterate_next( ) );
   }

   map< string, string > all_vars;
   string initial_vars( vars );
   string::size_type pos = 0;

   while( !vars.empty( ) )
   {
      pos = vars.find( '\n' );
      string next( vars.substr( 0, pos ) );

      if( !Parent_Prefix.empty( ) )
         next = Parent_Prefix + "_" + next;

      string::size_type npos = next.find( '=' );
      if( npos == string::npos )
         throw runtime_error( "unexpected format for specification vars '" + initial_vars + "'" );

      all_vars[ next.substr( 0, npos ) ] = next.substr( npos + 1 );

      if( pos == string::npos )
         break;

      vars.erase( 0, pos + 1 );
   }

   for( map< string, string >::const_iterator i = all_vars.begin( ), end = all_vars.end( ); i != end; ++i )
   {
      if( !All_Vars.empty( ) )
         All_Vars += '\n';
      All_Vars += i->first + "=" + i->second;
   }
   // [<finish Generate_All_Vars_impl>]
}

void Meta_Specification::impl::impl_Move_Down( const string& Restrict_Fields, const string& Restrict_Values )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start move_up_and_down)] 600169
   transaction_start( );
   try
   {
      // NOTE: Empty code block for scope purposes.
      {
         get_obj( ).op_update( get_obj( ).get_key( ), c_field_name_Order );

         class_pointer< Meta_Specification > cp_other( e_create_instance );

         int num_fixed = 0;
         if( !Restrict_Fields.empty( ) )
            num_fixed = split_count( Restrict_Fields );

         string key_info( Restrict_Fields );
         if( num_fixed )
            key_info += ',';
         key_info += string( c_field_id_Order );

         if( !num_fixed )
            key_info += ' ' + get_obj( ).Order( );
         else
         {
            key_info += '#' + to_string( num_fixed );
            key_info += ' ' + Restrict_Values + ',' + get_obj( ).Order( );
         }

         // NOTE: Only the first record is required so set the row limit to 1.
         if( cp_other->iterate_forwards( key_info, string( c_field_name_Order ), false, 1 ) )
         {
            string old_val( get_obj( ).Order( ) );
            string new_val( cp_other->Order( ) );

            get_obj( ).Order( gen_key( ) );
            get_obj( ).op_apply( );

            get_obj( ).op_update( get_obj( ).get_key( ), c_field_name_Order );

            cp_other->op_update( cp_other->get_key( ), c_field_name_Order );
            cp_other->Order( old_val );
            cp_other->op_apply( );

            get_obj( ).Order( new_val );
            get_obj( ).op_apply( );

            cp_other->iterate_stop( );
         }
         else
            get_obj( ).op_cancel( );
      }

      transaction_commit( );
   }
   catch( ... )
   {
      transaction_rollback( );
      throw;
   }
   // [(finish move_up_and_down)] 600169

   // [<start Move_Down_impl>]
//nyi
   // NOTE: The "Move Up/Down" implementation uses "minimal" updates which is fine except in the
   // case when the Specification being moved is part of a "field list". So if the Specification
   // Type matches a known field list "field" type then force an update (on the grandparent).
   string spec_type( get_obj( ).Specification_Type( ).get_key( ) );

   if( spec_type == "field" || spec_type == "field_pair" || spec_type == "field_pair2"
    || spec_type == "field_pair_g" || spec_type == "field_restrict" || spec_type == "field_source_and_value" )
   {
      class_pointer< Meta_Specification > cp_parent( &get_obj( ).Parent_Specification( ).Parent_Specification( ) );

      if( !cp_parent->get_key( ).empty( ) )
      {
         cp_parent->op_update( );
         cp_parent->op_apply( );
      }
   }
   // [<finish Move_Down_impl>]
}

void Meta_Specification::impl::impl_Move_Up( const string& Restrict_Fields, const string& Restrict_Values )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start move_up_and_down)] 600169
   transaction_start( );
   try
   {
      // NOTE: Empty code block for scope purposes.
      {
         get_obj( ).op_update( get_obj( ).get_key( ), c_field_name_Order );

         class_pointer< Meta_Specification > cp_other( e_create_instance );

         int num_fixed = 0;
         if( !Restrict_Fields.empty( ) )
            num_fixed = split_count( Restrict_Fields );

         string key_info( Restrict_Fields );
         if( num_fixed )
            key_info += ',';
         key_info += string( c_field_id_Order );

         if( !num_fixed )
            key_info += ' ' + get_obj( ).Order( );
         else
         {
            key_info += '#' + to_string( num_fixed );
            key_info += ' ' + Restrict_Values + ',' + get_obj( ).Order( );
         }

         // NOTE: Only the first record is required so set the row limit to 1.
         if( cp_other->iterate_backwards( key_info, string( c_field_name_Order ), false, 1 ) )
         {
            string old_val( get_obj( ).Order( ) );
            string new_val( cp_other->Order( ) );

            get_obj( ).Order( gen_key( ) );
            get_obj( ).op_apply( );

            get_obj( ).op_update( get_obj( ).get_key( ), c_field_name_Order );

            cp_other->op_update( cp_other->get_key( ), c_field_name_Order );
            cp_other->Order( old_val );
            cp_other->op_apply( );

            get_obj( ).Order( new_val );
            get_obj( ).op_apply( );

            cp_other->iterate_stop( );
         }
         else
            get_obj( ).op_cancel( );
      }

      transaction_commit( );
   }
   catch( ... )
   {
      transaction_rollback( );
      throw;
   }
   // [(finish move_up_and_down)] 600169

   // [<start Move_Up_impl>]
//nyi
   // NOTE: The "Move Up/Down" implementation uses "minimal" updates which is fine except in the
   // case when the Specification being moved is part of a "field list". So if the Specification
   // Type matches a known field list "field" type then force an update (on the grandparent).
   string spec_type( get_obj( ).Specification_Type( ).get_key( ) );

   if( spec_type == "field" || spec_type == "field_pair" || spec_type == "field_pair2"
    || spec_type == "field_pair_g" || spec_type == "field_restrict" || spec_type == "field_source_and_value" )
   {
      class_pointer< Meta_Specification > cp_parent( &get_obj( ).Parent_Specification( ).Parent_Specification( ) );

      if( !cp_parent->get_key( ).empty( ) )
      {
         cp_parent->op_update( );
         cp_parent->op_apply( );
      }
   }
   // [<finish Move_Up_impl>]
}

string Meta_Specification::impl::get_field_value( int field ) const
{
   string retval;

   switch( field )
   {
      case 0:
      retval = to_string( impl_Actions( ) );
      break;

      case 1:
      retval = to_string( impl_All_Strings( ) );
      break;

      case 2:
      retval = to_string( impl_All_Vars( ) );
      break;

      case 3:
      retval = to_string( impl_Can_Access_Below_Model( ) );
      break;

      case 4:
      retval = to_string( impl_Child_Rel_Child_Class( ) );
      break;

      case 5:
      retval = to_string( impl_Child_Rel_Field_Key( ) );
      break;

      case 6:
      retval = to_string( impl_Child_Relationship( ) );
      break;

      case 7:
      retval = to_string( impl_Child_Specification_Type( ) );
      break;

      case 8:
      retval = to_string( impl_Class( ) );
      break;

      case 9:
      retval = to_string( impl_Comments( ) );
      break;

      case 10:
      retval = to_string( impl_Enum( ) );
      break;

      case 11:
      retval = to_string( impl_Enum_Item( ) );
      break;

      case 12:
      retval = to_string( impl_Enum_Item_2( ) );
      break;

      case 13:
      retval = to_string( impl_Enum_Item_3( ) );
      break;

      case 14:
      retval = to_string( impl_Enum_Item_4( ) );
      break;

      case 15:
      retval = to_string( impl_Enum_Item_5( ) );
      break;

      case 16:
      retval = to_string( impl_Field( ) );
      break;

      case 17:
      retval = to_string( impl_Field_Class( ) );
      break;

      case 18:
      retval = to_string( impl_Field_Pairs( ) );
      break;

      case 19:
      retval = to_string( impl_Field_Values( ) );
      break;

      case 20:
      retval = to_string( impl_Fields( ) );
      break;

      case 21:
      retval = to_string( impl_Id( ) );
      break;

      case 22:
      retval = to_string( impl_Is_Required_For_UI_Gen( ) );
      break;

      case 23:
      retval = to_string( impl_Model( ) );
      break;

      case 24:
      retval = to_string( impl_Modifier( ) );
      break;

      case 25:
      retval = to_string( impl_Name( ) );
      break;

      case 26:
      retval = to_string( impl_Options( ) );
      break;

      case 27:
      retval = to_string( impl_Order( ) );
      break;

      case 28:
      retval = to_string( impl_Other_Class( ) );
      break;

      case 29:
      retval = to_string( impl_Other_Class_Field( ) );
      break;

      case 30:
      retval = to_string( impl_Other_Field( ) );
      break;

      case 31:
      retval = to_string( impl_Other_Field_2( ) );
      break;

      case 32:
      retval = to_string( impl_Other_Modifier( ) );
      break;

      case 33:
      retval = to_string( impl_Other_Modifier_2( ) );
      break;

      case 34:
      retval = to_string( impl_Other_Permission( ) );
      break;

      case 35:
      retval = to_string( impl_Other_Permission_2( ) );
      break;

      case 36:
      retval = to_string( impl_Other_Procedure( ) );
      break;

      case 37:
      retval = to_string( impl_Other_Procedure_2( ) );
      break;

      case 38:
      retval = to_string( impl_Other_Source_Child( ) );
      break;

      case 39:
      retval = to_string( impl_Other_Source_Child_2( ) );
      break;

      case 40:
      retval = to_string( impl_Parent_Specification( ) );
      break;

      case 41:
      retval = to_string( impl_Permission( ) );
      break;

      case 42:
      retval = to_string( impl_Procedure( ) );
      break;

      case 43:
      retval = to_string( impl_Procedure_Arg( ) );
      break;

      case 44:
      retval = to_string( impl_Procedure_Arg_2( ) );
      break;

      case 45:
      retval = to_string( impl_Procedure_Arg_3( ) );
      break;

      case 46:
      retval = to_string( impl_Protect_Child_Rel( ) );
      break;

      case 47:
      retval = to_string( impl_Protect_Procedure( ) );
      break;

      case 48:
      retval = to_string( impl_Protect_Source_Parent( ) );
      break;

      case 49:
      retval = to_string( impl_Restrict_Values( ) );
      break;

      case 50:
      retval = to_string( impl_Source_Child( ) );
      break;

      case 51:
      retval = to_string( impl_Source_Child_Class( ) );
      break;

      case 52:
      retval = to_string( impl_Source_Class( ) );
      break;

      case 53:
      retval = to_string( impl_Source_Field( ) );
      break;

      case 54:
      retval = to_string( impl_Source_Field_Class( ) );
      break;

      case 55:
      retval = to_string( impl_Source_Grandchild( ) );
      break;

      case 56:
      retval = to_string( impl_Source_Parent( ) );
      break;

      case 57:
      retval = to_string( impl_Source_Parent_Class( ) );
      break;

      case 58:
      retval = to_string( impl_Specification_Type( ) );
      break;

      case 59:
      retval = to_string( impl_Strings( ) );
      break;

      case 60:
      retval = to_string( impl_Test_Child( ) );
      break;

      case 61:
      retval = to_string( impl_Test_Field( ) );
      break;

      case 62:
      retval = to_string( impl_Test_Field_Class( ) );
      break;

      case 63:
      retval = to_string( impl_Test_Parent( ) );
      break;

      case 64:
      retval = to_string( impl_Test_Parent_Class( ) );
      break;

      case 65:
      retval = to_string( impl_Test_Value( ) );
      break;

      case 66:
      retval = to_string( impl_Test_Value_Label( ) );
      break;

      case 67:
      retval = to_string( impl_Use_Source_Parent( ) );
      break;

      case 68:
      retval = to_string( impl_Use_Test_Parent_Child( ) );
      break;

      case 69:
      retval = to_string( impl_Value( ) );
      break;

      case 70:
      retval = to_string( impl_Value_Label( ) );
      break;

      case 71:
      retval = to_string( impl_Value_Left_Part( ) );
      break;

      case 72:
      retval = to_string( impl_Value_Literal( ) );
      break;

      case 73:
      retval = to_string( impl_Value_Numeric_String( ) );
      break;

      case 74:
      retval = to_string( impl_Value_Right_Part( ) );
      break;

      case 75:
      retval = to_string( impl_Value_String( ) );
      break;

      case 76:
      retval = to_string( impl_Vars( ) );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in get field value" );
   }

   return retval;
}

void Meta_Specification::impl::set_field_value( int field, const string& value )
{
   switch( field )
   {
      case 0:
      func_string_setter< Meta_Specification::impl, string >( *this, &Meta_Specification::impl::impl_Actions, value );
      break;

      case 1:
      func_string_setter< Meta_Specification::impl, string >( *this, &Meta_Specification::impl::impl_All_Strings, value );
      break;

      case 2:
      func_string_setter< Meta_Specification::impl, string >( *this, &Meta_Specification::impl::impl_All_Vars, value );
      break;

      case 3:
      func_string_setter< Meta_Specification::impl, bool >( *this, &Meta_Specification::impl::impl_Can_Access_Below_Model, value );
      break;

      case 4:
      func_string_setter< Meta_Specification::impl, Meta_Class >( *this, &Meta_Specification::impl::impl_Child_Rel_Child_Class, value );
      break;

      case 5:
      func_string_setter< Meta_Specification::impl, string >( *this, &Meta_Specification::impl::impl_Child_Rel_Field_Key, value );
      break;

      case 6:
      func_string_setter< Meta_Specification::impl, Meta_Relationship >( *this, &Meta_Specification::impl::impl_Child_Relationship, value );
      break;

      case 7:
      func_string_setter< Meta_Specification::impl, Meta_Specification_Type >( *this, &Meta_Specification::impl::impl_Child_Specification_Type, value );
      break;

      case 8:
      func_string_setter< Meta_Specification::impl, Meta_Class >( *this, &Meta_Specification::impl::impl_Class, value );
      break;

      case 9:
      func_string_setter< Meta_Specification::impl, string >( *this, &Meta_Specification::impl::impl_Comments, value );
      break;

      case 10:
      func_string_setter< Meta_Specification::impl, Meta_Enum >( *this, &Meta_Specification::impl::impl_Enum, value );
      break;

      case 11:
      func_string_setter< Meta_Specification::impl, Meta_Enum_Item >( *this, &Meta_Specification::impl::impl_Enum_Item, value );
      break;

      case 12:
      func_string_setter< Meta_Specification::impl, Meta_Enum_Item >( *this, &Meta_Specification::impl::impl_Enum_Item_2, value );
      break;

      case 13:
      func_string_setter< Meta_Specification::impl, Meta_Enum_Item >( *this, &Meta_Specification::impl::impl_Enum_Item_3, value );
      break;

      case 14:
      func_string_setter< Meta_Specification::impl, Meta_Enum_Item >( *this, &Meta_Specification::impl::impl_Enum_Item_4, value );
      break;

      case 15:
      func_string_setter< Meta_Specification::impl, Meta_Enum_Item >( *this, &Meta_Specification::impl::impl_Enum_Item_5, value );
      break;

      case 16:
      func_string_setter< Meta_Specification::impl, Meta_Field >( *this, &Meta_Specification::impl::impl_Field, value );
      break;

      case 17:
      func_string_setter< Meta_Specification::impl, Meta_Class >( *this, &Meta_Specification::impl::impl_Field_Class, value );
      break;

      case 18:
      func_string_setter< Meta_Specification::impl, string >( *this, &Meta_Specification::impl::impl_Field_Pairs, value );
      break;

      case 19:
      func_string_setter< Meta_Specification::impl, string >( *this, &Meta_Specification::impl::impl_Field_Values, value );
      break;

      case 20:
      func_string_setter< Meta_Specification::impl, string >( *this, &Meta_Specification::impl::impl_Fields, value );
      break;

      case 21:
      func_string_setter< Meta_Specification::impl, string >( *this, &Meta_Specification::impl::impl_Id, value );
      break;

      case 22:
      func_string_setter< Meta_Specification::impl, bool >( *this, &Meta_Specification::impl::impl_Is_Required_For_UI_Gen, value );
      break;

      case 23:
      func_string_setter< Meta_Specification::impl, Meta_Model >( *this, &Meta_Specification::impl::impl_Model, value );
      break;

      case 24:
      func_string_setter< Meta_Specification::impl, Meta_Modifier >( *this, &Meta_Specification::impl::impl_Modifier, value );
      break;

      case 25:
      func_string_setter< Meta_Specification::impl, string >( *this, &Meta_Specification::impl::impl_Name, value );
      break;

      case 26:
      func_string_setter< Meta_Specification::impl, string >( *this, &Meta_Specification::impl::impl_Options, value );
      break;

      case 27:
      func_string_setter< Meta_Specification::impl, string >( *this, &Meta_Specification::impl::impl_Order, value );
      break;

      case 28:
      func_string_setter< Meta_Specification::impl, Meta_Class >( *this, &Meta_Specification::impl::impl_Other_Class, value );
      break;

      case 29:
      func_string_setter< Meta_Specification::impl, Meta_Field >( *this, &Meta_Specification::impl::impl_Other_Class_Field, value );
      break;

      case 30:
      func_string_setter< Meta_Specification::impl, Meta_Field >( *this, &Meta_Specification::impl::impl_Other_Field, value );
      break;

      case 31:
      func_string_setter< Meta_Specification::impl, Meta_Field >( *this, &Meta_Specification::impl::impl_Other_Field_2, value );
      break;

      case 32:
      func_string_setter< Meta_Specification::impl, Meta_Modifier >( *this, &Meta_Specification::impl::impl_Other_Modifier, value );
      break;

      case 33:
      func_string_setter< Meta_Specification::impl, Meta_Modifier >( *this, &Meta_Specification::impl::impl_Other_Modifier_2, value );
      break;

      case 34:
      func_string_setter< Meta_Specification::impl, Meta_Permission >( *this, &Meta_Specification::impl::impl_Other_Permission, value );
      break;

      case 35:
      func_string_setter< Meta_Specification::impl, Meta_Permission >( *this, &Meta_Specification::impl::impl_Other_Permission_2, value );
      break;

      case 36:
      func_string_setter< Meta_Specification::impl, Meta_Procedure >( *this, &Meta_Specification::impl::impl_Other_Procedure, value );
      break;

      case 37:
      func_string_setter< Meta_Specification::impl, Meta_Procedure >( *this, &Meta_Specification::impl::impl_Other_Procedure_2, value );
      break;

      case 38:
      func_string_setter< Meta_Specification::impl, Meta_Field >( *this, &Meta_Specification::impl::impl_Other_Source_Child, value );
      break;

      case 39:
      func_string_setter< Meta_Specification::impl, Meta_Field >( *this, &Meta_Specification::impl::impl_Other_Source_Child_2, value );
      break;

      case 40:
      func_string_setter< Meta_Specification::impl, Meta_Specification >( *this, &Meta_Specification::impl::impl_Parent_Specification, value );
      break;

      case 41:
      func_string_setter< Meta_Specification::impl, Meta_Permission >( *this, &Meta_Specification::impl::impl_Permission, value );
      break;

      case 42:
      func_string_setter< Meta_Specification::impl, Meta_Procedure >( *this, &Meta_Specification::impl::impl_Procedure, value );
      break;

      case 43:
      func_string_setter< Meta_Specification::impl, Meta_Procedure_Arg >( *this, &Meta_Specification::impl::impl_Procedure_Arg, value );
      break;

      case 44:
      func_string_setter< Meta_Specification::impl, Meta_Procedure_Arg >( *this, &Meta_Specification::impl::impl_Procedure_Arg_2, value );
      break;

      case 45:
      func_string_setter< Meta_Specification::impl, Meta_Procedure_Arg >( *this, &Meta_Specification::impl::impl_Procedure_Arg_3, value );
      break;

      case 46:
      func_string_setter< Meta_Specification::impl, bool >( *this, &Meta_Specification::impl::impl_Protect_Child_Rel, value );
      break;

      case 47:
      func_string_setter< Meta_Specification::impl, bool >( *this, &Meta_Specification::impl::impl_Protect_Procedure, value );
      break;

      case 48:
      func_string_setter< Meta_Specification::impl, bool >( *this, &Meta_Specification::impl::impl_Protect_Source_Parent, value );
      break;

      case 49:
      func_string_setter< Meta_Specification::impl, string >( *this, &Meta_Specification::impl::impl_Restrict_Values, value );
      break;

      case 50:
      func_string_setter< Meta_Specification::impl, Meta_Field >( *this, &Meta_Specification::impl::impl_Source_Child, value );
      break;

      case 51:
      func_string_setter< Meta_Specification::impl, Meta_Class >( *this, &Meta_Specification::impl::impl_Source_Child_Class, value );
      break;

      case 52:
      func_string_setter< Meta_Specification::impl, Meta_Class >( *this, &Meta_Specification::impl::impl_Source_Class, value );
      break;

      case 53:
      func_string_setter< Meta_Specification::impl, Meta_Field >( *this, &Meta_Specification::impl::impl_Source_Field, value );
      break;

      case 54:
      func_string_setter< Meta_Specification::impl, Meta_Class >( *this, &Meta_Specification::impl::impl_Source_Field_Class, value );
      break;

      case 55:
      func_string_setter< Meta_Specification::impl, Meta_Field >( *this, &Meta_Specification::impl::impl_Source_Grandchild, value );
      break;

      case 56:
      func_string_setter< Meta_Specification::impl, Meta_Field >( *this, &Meta_Specification::impl::impl_Source_Parent, value );
      break;

      case 57:
      func_string_setter< Meta_Specification::impl, Meta_Class >( *this, &Meta_Specification::impl::impl_Source_Parent_Class, value );
      break;

      case 58:
      func_string_setter< Meta_Specification::impl, Meta_Specification_Type >( *this, &Meta_Specification::impl::impl_Specification_Type, value );
      break;

      case 59:
      func_string_setter< Meta_Specification::impl, string >( *this, &Meta_Specification::impl::impl_Strings, value );
      break;

      case 60:
      func_string_setter< Meta_Specification::impl, Meta_Field >( *this, &Meta_Specification::impl::impl_Test_Child, value );
      break;

      case 61:
      func_string_setter< Meta_Specification::impl, Meta_Field >( *this, &Meta_Specification::impl::impl_Test_Field, value );
      break;

      case 62:
      func_string_setter< Meta_Specification::impl, Meta_Class >( *this, &Meta_Specification::impl::impl_Test_Field_Class, value );
      break;

      case 63:
      func_string_setter< Meta_Specification::impl, Meta_Field >( *this, &Meta_Specification::impl::impl_Test_Parent, value );
      break;

      case 64:
      func_string_setter< Meta_Specification::impl, Meta_Class >( *this, &Meta_Specification::impl::impl_Test_Parent_Class, value );
      break;

      case 65:
      func_string_setter< Meta_Specification::impl, string >( *this, &Meta_Specification::impl::impl_Test_Value, value );
      break;

      case 66:
      func_string_setter< Meta_Specification::impl, string >( *this, &Meta_Specification::impl::impl_Test_Value_Label, value );
      break;

      case 67:
      func_string_setter< Meta_Specification::impl, bool >( *this, &Meta_Specification::impl::impl_Use_Source_Parent, value );
      break;

      case 68:
      func_string_setter< Meta_Specification::impl, bool >( *this, &Meta_Specification::impl::impl_Use_Test_Parent_Child, value );
      break;

      case 69:
      func_string_setter< Meta_Specification::impl, string >( *this, &Meta_Specification::impl::impl_Value, value );
      break;

      case 70:
      func_string_setter< Meta_Specification::impl, string >( *this, &Meta_Specification::impl::impl_Value_Label, value );
      break;

      case 71:
      func_string_setter< Meta_Specification::impl, string >( *this, &Meta_Specification::impl::impl_Value_Left_Part, value );
      break;

      case 72:
      func_string_setter< Meta_Specification::impl, string >( *this, &Meta_Specification::impl::impl_Value_Literal, value );
      break;

      case 73:
      func_string_setter< Meta_Specification::impl, string >( *this, &Meta_Specification::impl::impl_Value_Numeric_String, value );
      break;

      case 74:
      func_string_setter< Meta_Specification::impl, string >( *this, &Meta_Specification::impl::impl_Value_Right_Part, value );
      break;

      case 75:
      func_string_setter< Meta_Specification::impl, string >( *this, &Meta_Specification::impl::impl_Value_String, value );
      break;

      case 76:
      func_string_setter< Meta_Specification::impl, string >( *this, &Meta_Specification::impl::impl_Vars, value );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in set field value" );
   }
}

void Meta_Specification::impl::set_field_default( int field )
{
   switch( field )
   {
      case 0:
      impl_Actions( g_default_Actions );
      break;

      case 1:
      impl_All_Strings( g_default_All_Strings );
      break;

      case 2:
      impl_All_Vars( g_default_All_Vars );
      break;

      case 3:
      impl_Can_Access_Below_Model( g_default_Can_Access_Below_Model );
      break;

      case 4:
      impl_Child_Rel_Child_Class( g_default_Child_Rel_Child_Class );
      break;

      case 5:
      impl_Child_Rel_Field_Key( g_default_Child_Rel_Field_Key );
      break;

      case 6:
      impl_Child_Relationship( g_default_Child_Relationship );
      break;

      case 7:
      impl_Child_Specification_Type( g_default_Child_Specification_Type );
      break;

      case 8:
      impl_Class( g_default_Class );
      break;

      case 9:
      impl_Comments( g_default_Comments );
      break;

      case 10:
      impl_Enum( g_default_Enum );
      break;

      case 11:
      impl_Enum_Item( g_default_Enum_Item );
      break;

      case 12:
      impl_Enum_Item_2( g_default_Enum_Item_2 );
      break;

      case 13:
      impl_Enum_Item_3( g_default_Enum_Item_3 );
      break;

      case 14:
      impl_Enum_Item_4( g_default_Enum_Item_4 );
      break;

      case 15:
      impl_Enum_Item_5( g_default_Enum_Item_5 );
      break;

      case 16:
      impl_Field( g_default_Field );
      break;

      case 17:
      impl_Field_Class( g_default_Field_Class );
      break;

      case 18:
      impl_Field_Pairs( g_default_Field_Pairs );
      break;

      case 19:
      impl_Field_Values( g_default_Field_Values );
      break;

      case 20:
      impl_Fields( g_default_Fields );
      break;

      case 21:
      impl_Id( g_default_Id );
      break;

      case 22:
      impl_Is_Required_For_UI_Gen( g_default_Is_Required_For_UI_Gen );
      break;

      case 23:
      impl_Model( g_default_Model );
      break;

      case 24:
      impl_Modifier( g_default_Modifier );
      break;

      case 25:
      impl_Name( g_default_Name );
      break;

      case 26:
      impl_Options( g_default_Options );
      break;

      case 27:
      impl_Order( g_default_Order );
      break;

      case 28:
      impl_Other_Class( g_default_Other_Class );
      break;

      case 29:
      impl_Other_Class_Field( g_default_Other_Class_Field );
      break;

      case 30:
      impl_Other_Field( g_default_Other_Field );
      break;

      case 31:
      impl_Other_Field_2( g_default_Other_Field_2 );
      break;

      case 32:
      impl_Other_Modifier( g_default_Other_Modifier );
      break;

      case 33:
      impl_Other_Modifier_2( g_default_Other_Modifier_2 );
      break;

      case 34:
      impl_Other_Permission( g_default_Other_Permission );
      break;

      case 35:
      impl_Other_Permission_2( g_default_Other_Permission_2 );
      break;

      case 36:
      impl_Other_Procedure( g_default_Other_Procedure );
      break;

      case 37:
      impl_Other_Procedure_2( g_default_Other_Procedure_2 );
      break;

      case 38:
      impl_Other_Source_Child( g_default_Other_Source_Child );
      break;

      case 39:
      impl_Other_Source_Child_2( g_default_Other_Source_Child_2 );
      break;

      case 40:
      impl_Parent_Specification( g_default_Parent_Specification );
      break;

      case 41:
      impl_Permission( g_default_Permission );
      break;

      case 42:
      impl_Procedure( g_default_Procedure );
      break;

      case 43:
      impl_Procedure_Arg( g_default_Procedure_Arg );
      break;

      case 44:
      impl_Procedure_Arg_2( g_default_Procedure_Arg_2 );
      break;

      case 45:
      impl_Procedure_Arg_3( g_default_Procedure_Arg_3 );
      break;

      case 46:
      impl_Protect_Child_Rel( g_default_Protect_Child_Rel );
      break;

      case 47:
      impl_Protect_Procedure( g_default_Protect_Procedure );
      break;

      case 48:
      impl_Protect_Source_Parent( g_default_Protect_Source_Parent );
      break;

      case 49:
      impl_Restrict_Values( g_default_Restrict_Values );
      break;

      case 50:
      impl_Source_Child( g_default_Source_Child );
      break;

      case 51:
      impl_Source_Child_Class( g_default_Source_Child_Class );
      break;

      case 52:
      impl_Source_Class( g_default_Source_Class );
      break;

      case 53:
      impl_Source_Field( g_default_Source_Field );
      break;

      case 54:
      impl_Source_Field_Class( g_default_Source_Field_Class );
      break;

      case 55:
      impl_Source_Grandchild( g_default_Source_Grandchild );
      break;

      case 56:
      impl_Source_Parent( g_default_Source_Parent );
      break;

      case 57:
      impl_Source_Parent_Class( g_default_Source_Parent_Class );
      break;

      case 58:
      impl_Specification_Type( g_default_Specification_Type );
      break;

      case 59:
      impl_Strings( g_default_Strings );
      break;

      case 60:
      impl_Test_Child( g_default_Test_Child );
      break;

      case 61:
      impl_Test_Field( g_default_Test_Field );
      break;

      case 62:
      impl_Test_Field_Class( g_default_Test_Field_Class );
      break;

      case 63:
      impl_Test_Parent( g_default_Test_Parent );
      break;

      case 64:
      impl_Test_Parent_Class( g_default_Test_Parent_Class );
      break;

      case 65:
      impl_Test_Value( g_default_Test_Value );
      break;

      case 66:
      impl_Test_Value_Label( g_default_Test_Value_Label );
      break;

      case 67:
      impl_Use_Source_Parent( g_default_Use_Source_Parent );
      break;

      case 68:
      impl_Use_Test_Parent_Child( g_default_Use_Test_Parent_Child );
      break;

      case 69:
      impl_Value( g_default_Value );
      break;

      case 70:
      impl_Value_Label( g_default_Value_Label );
      break;

      case 71:
      impl_Value_Left_Part( g_default_Value_Left_Part );
      break;

      case 72:
      impl_Value_Literal( g_default_Value_Literal );
      break;

      case 73:
      impl_Value_Numeric_String( g_default_Value_Numeric_String );
      break;

      case 74:
      impl_Value_Right_Part( g_default_Value_Right_Part );
      break;

      case 75:
      impl_Value_String( g_default_Value_String );
      break;

      case 76:
      impl_Vars( g_default_Vars );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in set field default" );
   }
}

bool Meta_Specification::impl::is_field_default( int field ) const
{
   bool retval = false;

   switch( field )
   {
      case 0:
      retval = ( v_Actions == g_default_Actions );
      break;

      case 1:
      retval = ( v_All_Strings == g_default_All_Strings );
      break;

      case 2:
      retval = ( v_All_Vars == g_default_All_Vars );
      break;

      case 3:
      retval = ( v_Can_Access_Below_Model == g_default_Can_Access_Below_Model );
      break;

      case 4:
      retval = ( v_Child_Rel_Child_Class == g_default_Child_Rel_Child_Class );
      break;

      case 5:
      retval = ( v_Child_Rel_Field_Key == g_default_Child_Rel_Field_Key );
      break;

      case 6:
      retval = ( v_Child_Relationship == g_default_Child_Relationship );
      break;

      case 7:
      retval = ( v_Child_Specification_Type == g_default_Child_Specification_Type );
      break;

      case 8:
      retval = ( v_Class == g_default_Class );
      break;

      case 9:
      retval = ( v_Comments == g_default_Comments );
      break;

      case 10:
      retval = ( v_Enum == g_default_Enum );
      break;

      case 11:
      retval = ( v_Enum_Item == g_default_Enum_Item );
      break;

      case 12:
      retval = ( v_Enum_Item_2 == g_default_Enum_Item_2 );
      break;

      case 13:
      retval = ( v_Enum_Item_3 == g_default_Enum_Item_3 );
      break;

      case 14:
      retval = ( v_Enum_Item_4 == g_default_Enum_Item_4 );
      break;

      case 15:
      retval = ( v_Enum_Item_5 == g_default_Enum_Item_5 );
      break;

      case 16:
      retval = ( v_Field == g_default_Field );
      break;

      case 17:
      retval = ( v_Field_Class == g_default_Field_Class );
      break;

      case 18:
      retval = ( v_Field_Pairs == g_default_Field_Pairs );
      break;

      case 19:
      retval = ( v_Field_Values == g_default_Field_Values );
      break;

      case 20:
      retval = ( v_Fields == g_default_Fields );
      break;

      case 21:
      retval = ( v_Id == g_default_Id );
      break;

      case 22:
      retval = ( v_Is_Required_For_UI_Gen == g_default_Is_Required_For_UI_Gen );
      break;

      case 23:
      retval = ( v_Model == g_default_Model );
      break;

      case 24:
      retval = ( v_Modifier == g_default_Modifier );
      break;

      case 25:
      retval = ( v_Name == g_default_Name );
      break;

      case 26:
      retval = ( v_Options == g_default_Options );
      break;

      case 27:
      retval = ( v_Order == g_default_Order );
      break;

      case 28:
      retval = ( v_Other_Class == g_default_Other_Class );
      break;

      case 29:
      retval = ( v_Other_Class_Field == g_default_Other_Class_Field );
      break;

      case 30:
      retval = ( v_Other_Field == g_default_Other_Field );
      break;

      case 31:
      retval = ( v_Other_Field_2 == g_default_Other_Field_2 );
      break;

      case 32:
      retval = ( v_Other_Modifier == g_default_Other_Modifier );
      break;

      case 33:
      retval = ( v_Other_Modifier_2 == g_default_Other_Modifier_2 );
      break;

      case 34:
      retval = ( v_Other_Permission == g_default_Other_Permission );
      break;

      case 35:
      retval = ( v_Other_Permission_2 == g_default_Other_Permission_2 );
      break;

      case 36:
      retval = ( v_Other_Procedure == g_default_Other_Procedure );
      break;

      case 37:
      retval = ( v_Other_Procedure_2 == g_default_Other_Procedure_2 );
      break;

      case 38:
      retval = ( v_Other_Source_Child == g_default_Other_Source_Child );
      break;

      case 39:
      retval = ( v_Other_Source_Child_2 == g_default_Other_Source_Child_2 );
      break;

      case 40:
      retval = ( v_Parent_Specification == g_default_Parent_Specification );
      break;

      case 41:
      retval = ( v_Permission == g_default_Permission );
      break;

      case 42:
      retval = ( v_Procedure == g_default_Procedure );
      break;

      case 43:
      retval = ( v_Procedure_Arg == g_default_Procedure_Arg );
      break;

      case 44:
      retval = ( v_Procedure_Arg_2 == g_default_Procedure_Arg_2 );
      break;

      case 45:
      retval = ( v_Procedure_Arg_3 == g_default_Procedure_Arg_3 );
      break;

      case 46:
      retval = ( v_Protect_Child_Rel == g_default_Protect_Child_Rel );
      break;

      case 47:
      retval = ( v_Protect_Procedure == g_default_Protect_Procedure );
      break;

      case 48:
      retval = ( v_Protect_Source_Parent == g_default_Protect_Source_Parent );
      break;

      case 49:
      retval = ( v_Restrict_Values == g_default_Restrict_Values );
      break;

      case 50:
      retval = ( v_Source_Child == g_default_Source_Child );
      break;

      case 51:
      retval = ( v_Source_Child_Class == g_default_Source_Child_Class );
      break;

      case 52:
      retval = ( v_Source_Class == g_default_Source_Class );
      break;

      case 53:
      retval = ( v_Source_Field == g_default_Source_Field );
      break;

      case 54:
      retval = ( v_Source_Field_Class == g_default_Source_Field_Class );
      break;

      case 55:
      retval = ( v_Source_Grandchild == g_default_Source_Grandchild );
      break;

      case 56:
      retval = ( v_Source_Parent == g_default_Source_Parent );
      break;

      case 57:
      retval = ( v_Source_Parent_Class == g_default_Source_Parent_Class );
      break;

      case 58:
      retval = ( v_Specification_Type == g_default_Specification_Type );
      break;

      case 59:
      retval = ( v_Strings == g_default_Strings );
      break;

      case 60:
      retval = ( v_Test_Child == g_default_Test_Child );
      break;

      case 61:
      retval = ( v_Test_Field == g_default_Test_Field );
      break;

      case 62:
      retval = ( v_Test_Field_Class == g_default_Test_Field_Class );
      break;

      case 63:
      retval = ( v_Test_Parent == g_default_Test_Parent );
      break;

      case 64:
      retval = ( v_Test_Parent_Class == g_default_Test_Parent_Class );
      break;

      case 65:
      retval = ( v_Test_Value == g_default_Test_Value );
      break;

      case 66:
      retval = ( v_Test_Value_Label == g_default_Test_Value_Label );
      break;

      case 67:
      retval = ( v_Use_Source_Parent == g_default_Use_Source_Parent );
      break;

      case 68:
      retval = ( v_Use_Test_Parent_Child == g_default_Use_Test_Parent_Child );
      break;

      case 69:
      retval = ( v_Value == g_default_Value );
      break;

      case 70:
      retval = ( v_Value_Label == g_default_Value_Label );
      break;

      case 71:
      retval = ( v_Value_Left_Part == g_default_Value_Left_Part );
      break;

      case 72:
      retval = ( v_Value_Literal == g_default_Value_Literal );
      break;

      case 73:
      retval = ( v_Value_Numeric_String == g_default_Value_Numeric_String );
      break;

      case 74:
      retval = ( v_Value_Right_Part == g_default_Value_Right_Part );
      break;

      case 75:
      retval = ( v_Value_String == g_default_Value_String );
      break;

      case 76:
      retval = ( v_Vars == g_default_Vars );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in is_field_default" );
   }

   return retval;
}

uint64_t Meta_Specification::impl::get_state( ) const
{
   uint64_t state = 0;

   // [(start modifier_field_value)] 600101
   if( get_obj( ).Specification_Type( ) == "comment" )
      state |= c_modifier_Is_Comment;
   // [(finish modifier_field_value)] 600101

   // [(start modifier_field_value)] 600101a
   if( get_obj( ).Specification_Type( ).Allow_Other_Class( ) == false )
      state |= c_modifier_Hide_Other_Class;
   // [(finish modifier_field_value)] 600101a

   // [(start modifier_field_value)] 600101b
   if( get_obj( ).Specification_Type( ).Allow_Field( ) == false )
      state |= c_modifier_Hide_Field;
   // [(finish modifier_field_value)] 600101b

   // [(start modifier_field_value)] 600102a
   if( get_obj( ).Specification_Type( ).Allow_Other_Field( ) == false )
      state |= c_modifier_Hide_Other_Field;
   // [(finish modifier_field_value)] 600102a

   // [(start modifier_field_value)] 600102c
   if( get_obj( ).Specification_Type( ).Allow_Other_Class_Field( ) == false )
      state |= c_modifier_Hide_Other_Class_Field;
   // [(finish modifier_field_value)] 600102c

   // [(start modifier_field_value)] 600103
   if( get_obj( ).Specification_Type( ).Allow_Procedure( ) == false )
      state |= c_modifier_Hide_Procedure;
   // [(finish modifier_field_value)] 600103

   // [(start modifier_field_value)] 600104
   if( get_obj( ).Specification_Type( ).Allow_Procedure_Arg( ) == false )
      state |= c_modifier_Hide_Procedure_Arg;
   // [(finish modifier_field_value)] 600104

   // [(start modifier_field_value)] 600104a
   if( get_obj( ).Specification_Type( ).Needs_Procedure_Arg_2( ) == false )
      state |= c_modifier_Hide_Procedure_Arg_2;
   // [(finish modifier_field_value)] 600104a

   // [(start modifier_field_value)] 600104b
   if( get_obj( ).Specification_Type( ).Needs_Procedure_Arg_3( ) == false )
      state |= c_modifier_Hide_Procedure_Arg_3;
   // [(finish modifier_field_value)] 600104b

   // [(start modifier_field_value)] 600104c
   if( get_obj( ).Specification_Type( ).Allow_Other_Procedure( ) == false )
      state |= c_modifier_Hide_Other_Procedure;
   // [(finish modifier_field_value)] 600104c

   // [(start modifier_field_value)] 600104d
   if( get_obj( ).Specification_Type( ).Allow_Other_Procedure_2( ) == false )
      state |= c_modifier_Hide_Other_Procedure_2;
   // [(finish modifier_field_value)] 600104d

   // [(start modifier_field_value)] 600105
   if( get_obj( ).Specification_Type( ).Allow_Modifier( ) == false )
      state |= c_modifier_Hide_Modifier;
   // [(finish modifier_field_value)] 600105

   // [(start modifier_field_value)] 600105a
   if( get_obj( ).Specification_Type( ).Allow_Other_Modifier( ) == false )
      state |= c_modifier_Hide_Other_Modifier;
   // [(finish modifier_field_value)] 600105a

   // [(start modifier_field_value)] 600105b
   if( get_obj( ).Specification_Type( ).Allow_Other_Modifier_2( ) == false )
      state |= c_modifier_Hide_Other_Modifier_2;
   // [(finish modifier_field_value)] 600105b

   // [(start modifier_field_value)] 600106
   if( get_obj( ).Specification_Type( ).Allow_Enum( ) == false )
      state |= c_modifier_Hide_Enum;
   // [(finish modifier_field_value)] 600106

   // [(start modifier_field_value)] 600107
   if( get_obj( ).Specification_Type( ).Allow_Enum_Item( ) == false )
      state |= c_modifier_Hide_Enum_Item;
   // [(finish modifier_field_value)] 600107

   // [(start modifier_field_value)] 600107a
   if( get_obj( ).Specification_Type( ).Needs_Enum_Item_2( ) == false )
      state |= c_modifier_Hide_Enum_Item_2;
   // [(finish modifier_field_value)] 600107a

   // [(start modifier_field_value)] 600107b
   if( get_obj( ).Specification_Type( ).Needs_Enum_Item_3( ) == false )
      state |= c_modifier_Hide_Enum_Item_3;
   // [(finish modifier_field_value)] 600107b

   // [(start modifier_field_value)] 600107c
   if( get_obj( ).Specification_Type( ).Needs_Enum_Item_4( ) == false )
      state |= c_modifier_Hide_Enum_Item_4;
   // [(finish modifier_field_value)] 600107c

   // [(start modifier_field_value)] 600107d
   if( get_obj( ).Specification_Type( ).Needs_Enum_Item_5( ) == false )
      state |= c_modifier_Hide_Enum_Item_5;
   // [(finish modifier_field_value)] 600107d

   // [(start modifier_field_value)] 600108
   if( get_obj( ).Specification_Type( ).Allow_Value( ) == false )
      state |= c_modifier_Hide_Value;
   // [(finish modifier_field_value)] 600108

   // [(start modifier_field_value)] 600109
   if( get_obj( ).Specification_Type( ).Allow_Source_Parent( ) == false )
      state |= c_modifier_Hide_Source_Parent;
   // [(finish modifier_field_value)] 600109

   // [(start modifier_field_value)] 600110
   if( get_obj( ).Specification_Type( ).Allow_Source_Field( ) == false )
      state |= c_modifier_Hide_Source_Field;
   // [(finish modifier_field_value)] 600110

   // [(start modifier_field_value)] 600111
   if( get_obj( ).Specification_Type( ).Allow_Test_Field( ) == false )
      state |= c_modifier_Hide_Test_Field;
   // [(finish modifier_field_value)] 600111

   // [(start modifier_field_value)] 600112
   if( get_obj( ).Specification_Type( ).Allow_Test_Value( ) == false )
      state |= c_modifier_Hide_Test_Value;
   // [(finish modifier_field_value)] 600112

   // [(start modifier_field_value)] 600113
   if( get_obj( ).Specification_Type( ).Allow_Options( ) == false )
      state |= c_modifier_Hide_Options;
   // [(finish modifier_field_value)] 600113

   // [(start modifier_field_value)] 600114
   if( get_obj( ).Specification_Type( ).Allow_Source_Child( ) == false )
      state |= c_modifier_Hide_Source_Child;
   // [(finish modifier_field_value)] 600114

   // [(start modifier_field_value)] 600115
   if( get_obj( ).Specification_Type( ).Allow_FK_Source_Field_Choice( ) == false )
      state |= c_modifier_Hide_Use_Source_Parent;
   // [(finish modifier_field_value)] 600115

   // [(start modifier_field_value)] 600116
   if( get_obj( ).Specification_Type( ).Allow_FK_Source_Field_Choice( ) && get_obj( ).Use_Source_Parent( ) == false )
      state |= c_modifier_Hide_Source_Parent;
   // [(finish modifier_field_value)] 600116

   // [(start modifier_field_value)] 600117
   if( get_obj( ).Specification_Type( ).Allow_FK_Source_Field_Choice( ) && get_obj( ).Use_Source_Parent( ) == false )
      state |= c_modifier_Hide_Source_Child;
   // [(finish modifier_field_value)] 600117

   // [(start modifier_field_value)] 600118
   if( get_obj( ).Specification_Type( ).Allow_FK_Source_Field_Choice( ) && get_obj( ).Use_Source_Parent( ) == true )
      state |= c_modifier_Hide_Source_Field;
   // [(finish modifier_field_value)] 600118

   // [(start modifier_field_value)] 600119
   if( get_obj( ).Specification_Type( ).Allow_Permission( ) == false )
      state |= c_modifier_Hide_Permission;
   // [(finish modifier_field_value)] 600119

   // [(start modifier_field_value)] 600119a
   if( get_obj( ).Specification_Type( ).Needs_Other_Permission( ) == false )
      state |= c_modifier_Hide_Other_Permission;
   // [(finish modifier_field_value)] 600119a

   // [(start modifier_field_value)] 600119b
   if( get_obj( ).Specification_Type( ).Needs_Other_Permission_2( ) == false )
      state |= c_modifier_Hide_Other_Permission_2;
   // [(finish modifier_field_value)] 600119b

   // [(start modifier_field_value)] 600120
   if( get_obj( ).Protect_Source_Parent( ) == true )
      state |= c_modifier_Protect_Source_Parent;
   // [(finish modifier_field_value)] 600120

   // [(start modifier_field_value)] 600121
   if( get_obj( ).Specification_Type( ).Protect_Source_Parent( ) == true )
      state |= c_modifier_Protect_Source_Parent;
   // [(finish modifier_field_value)] 600121

   // [(start modifier_field_value)] 600122
   if( get_obj( ).Specification_Type( ).Allow_Child_Relationship( ) == false )
      state |= c_modifier_Hide_Child_Relationship;
   // [(finish modifier_field_value)] 600122

   // [(start modifier_field_value)] 600123
   if( !get_obj( ).get_key( ).empty( ) && get_obj( ).Specification_Type( ).Protect_Class_From_Update( ) == true )
      state |= c_modifier_Protect_Class;
   // [(finish modifier_field_value)] 600123

   // [(start modifier_field_value)] 600124
   if( get_obj( ).Specification_Type( ).Allow_FK_Test_Field_Choice( ) == false )
      state |= c_modifier_Hide_Use_Test_Parent_Child;
   // [(finish modifier_field_value)] 600124

   // [(start modifier_field_value)] 600125
   if( get_obj( ).Specification_Type( ).Allow_FK_Test_Field_Choice( ) && get_obj( ).Use_Test_Parent_Child( ) == false )
      state |= c_modifier_Hide_Test_Parent;
   // [(finish modifier_field_value)] 600125

   // [(start modifier_field_value)] 600126
   if( get_obj( ).Specification_Type( ).Allow_FK_Test_Field_Choice( ) && get_obj( ).Use_Test_Parent_Child( ) == false )
      state |= c_modifier_Hide_Test_Child;
   // [(finish modifier_field_value)] 600126

   // [(start modifier_field_value)] 600127
   if( get_obj( ).Specification_Type( ).Allow_FK_Test_Field_Choice( ) && get_obj( ).Use_Test_Parent_Child( ) == true )
      state |= c_modifier_Hide_Test_Field;
   // [(finish modifier_field_value)] 600127

   // [(start modifier_field_value)] 600128
   if( get_obj( ).Protect_Child_Rel( ) == true )
      state |= c_modifier_Protect_Child_Relationship;
   // [(finish modifier_field_value)] 600128

   // [(start modifier_field_value)] 610102b
   if( get_obj( ).Specification_Type( ).Allow_Other_Field_2( ) == false )
      state |= c_modifier_Hide_Other_Field_2;
   // [(finish modifier_field_value)] 610102b

   // [(start modifier_field_value)] 610114
   if( get_obj( ).Specification_Type( ).Allow_Other_Source_Child( ) == false )
      state |= c_modifier_Hide_Other_Source_Child;
   // [(finish modifier_field_value)] 610114

   // [(start modifier_field_value)] 610117
   if( get_obj( ).Specification_Type( ).Allow_FK_Source_Field_Choice( ) && get_obj( ).Use_Source_Parent( ) == false )
      state |= c_modifier_Hide_Other_Source_Child;
   // [(finish modifier_field_value)] 610117

   // [(start modifier_field_value)] 610128
   if( !get_obj( ).get_key( ).empty( ) && get_obj( ).Specification_Type( ).Protect_Child_Rel_From_Update( ) == true )
      state |= c_modifier_Protect_Child_Relationship;
   // [(finish modifier_field_value)] 610128

   // [(start modifier_field_value)] 610181
   if( get_obj( ).Protect_Procedure( ) == true )
      state |= c_modifier_Protect_Procedure;
   // [(finish modifier_field_value)] 610181

   // [(start modifier_field_value)] 620114
   if( get_obj( ).Specification_Type( ).Allow_Other_Source_Child_2( ) == false )
      state |= c_modifier_Hide_Other_Source_Child_2;
   // [(finish modifier_field_value)] 620114

   // [(start modifier_field_value)] 620117
   if( get_obj( ).Specification_Type( ).Allow_FK_Source_Field_Choice( ) && get_obj( ).Use_Source_Parent( ) == false )
      state |= c_modifier_Hide_Other_Source_Child_2;
   // [(finish modifier_field_value)] 620117

   // [(start modifier_field_value)] 620128
   if( get_obj( ).Specification_Type( ).Protect_Class_From_Edit( ) == true )
      state |= c_modifier_Protect_Class;
   // [(finish modifier_field_value)] 620128

   // [(start modifier_field_value)] 620146
   if( get_obj( ).Specification_Type( ).Allow_Source_Class( ) == false )
      state |= c_modifier_Hide_Source_Class;
   // [(finish modifier_field_value)] 620146

   // [(start modifier_field_value)] 620181
   if( get_obj( ).Specification_Type( ).Protect_Procedure_From_Edit( ) == true )
      state |= c_modifier_Protect_Procedure;
   // [(finish modifier_field_value)] 620181

   // [(start modifier_field_value)] 630114
   if( get_obj( ).Specification_Type( ).Allow_Source_Grandchild( ) == false )
      state |= c_modifier_Hide_Source_Grandchild;
   // [(finish modifier_field_value)] 630114

   // [(start modifier_field_value)] 630117
   if( get_obj( ).Specification_Type( ).Allow_FK_Source_Field_Choice( ) && get_obj( ).Use_Source_Parent( ) == false )
      state |= c_modifier_Hide_Source_Grandchild;
   // [(finish modifier_field_value)] 630117

   // [(start modifier_field_value)] 630128
   if( get_obj( ).Specification_Type( ).Use_Field_Enum( ) == true )
      state |= c_modifier_Protect_Enum;
   // [(finish modifier_field_value)] 630128

   // [(start modifier_field_value)] 630128a
   if( get_obj( ).Specification_Type( ).Use_Source_Field_Enum( ) == true )
      state |= c_modifier_Protect_Enum;
   // [(finish modifier_field_value)] 630128a

   // [(start modifier_field_value)] 630128b
   if( get_obj( ).Specification_Type( ).Use_Source_Field_Or_Child_Enum( ) == true )
      state |= c_modifier_Protect_Enum;
   // [(finish modifier_field_value)] 630128b

   // [(start modifier_field_value)] 630129
   if( get_obj( ).Specification_Type( ).Protect_Other_Class_From_Edit( ) == true )
      state |= c_modifier_Protect_Other_Class;
   // [(finish modifier_field_value)] 630129

   // [<start get_state>]
   // [<finish get_state>]

   return state;
}

string Meta_Specification::impl::get_state_names( ) const
{
   string state_names;
   uint64_t state = get_state( );

   if( state & c_modifier_Hide_Child_Relationship )
      state_names += "|" + string( "Hide_Child_Relationship" );
   if( state & c_modifier_Hide_Enum )
      state_names += "|" + string( "Hide_Enum" );
   if( state & c_modifier_Hide_Enum_Item )
      state_names += "|" + string( "Hide_Enum_Item" );
   if( state & c_modifier_Hide_Enum_Item_2 )
      state_names += "|" + string( "Hide_Enum_Item_2" );
   if( state & c_modifier_Hide_Enum_Item_3 )
      state_names += "|" + string( "Hide_Enum_Item_3" );
   if( state & c_modifier_Hide_Enum_Item_4 )
      state_names += "|" + string( "Hide_Enum_Item_4" );
   if( state & c_modifier_Hide_Enum_Item_5 )
      state_names += "|" + string( "Hide_Enum_Item_5" );
   if( state & c_modifier_Hide_Field )
      state_names += "|" + string( "Hide_Field" );
   if( state & c_modifier_Hide_Modifier )
      state_names += "|" + string( "Hide_Modifier" );
   if( state & c_modifier_Hide_Options )
      state_names += "|" + string( "Hide_Options" );
   if( state & c_modifier_Hide_Other_Class )
      state_names += "|" + string( "Hide_Other_Class" );
   if( state & c_modifier_Hide_Other_Class_Field )
      state_names += "|" + string( "Hide_Other_Class_Field" );
   if( state & c_modifier_Hide_Other_Field )
      state_names += "|" + string( "Hide_Other_Field" );
   if( state & c_modifier_Hide_Other_Field_2 )
      state_names += "|" + string( "Hide_Other_Field_2" );
   if( state & c_modifier_Hide_Other_Modifier )
      state_names += "|" + string( "Hide_Other_Modifier" );
   if( state & c_modifier_Hide_Other_Modifier_2 )
      state_names += "|" + string( "Hide_Other_Modifier_2" );
   if( state & c_modifier_Hide_Other_Permission )
      state_names += "|" + string( "Hide_Other_Permission" );
   if( state & c_modifier_Hide_Other_Permission_2 )
      state_names += "|" + string( "Hide_Other_Permission_2" );
   if( state & c_modifier_Hide_Other_Procedure )
      state_names += "|" + string( "Hide_Other_Procedure" );
   if( state & c_modifier_Hide_Other_Procedure_2 )
      state_names += "|" + string( "Hide_Other_Procedure_2" );
   if( state & c_modifier_Hide_Other_Source_Child )
      state_names += "|" + string( "Hide_Other_Source_Child" );
   if( state & c_modifier_Hide_Other_Source_Child_2 )
      state_names += "|" + string( "Hide_Other_Source_Child_2" );
   if( state & c_modifier_Hide_Permission )
      state_names += "|" + string( "Hide_Permission" );
   if( state & c_modifier_Hide_Procedure )
      state_names += "|" + string( "Hide_Procedure" );
   if( state & c_modifier_Hide_Procedure_Arg )
      state_names += "|" + string( "Hide_Procedure_Arg" );
   if( state & c_modifier_Hide_Procedure_Arg_2 )
      state_names += "|" + string( "Hide_Procedure_Arg_2" );
   if( state & c_modifier_Hide_Procedure_Arg_3 )
      state_names += "|" + string( "Hide_Procedure_Arg_3" );
   if( state & c_modifier_Hide_Source_Child )
      state_names += "|" + string( "Hide_Source_Child" );
   if( state & c_modifier_Hide_Source_Class )
      state_names += "|" + string( "Hide_Source_Class" );
   if( state & c_modifier_Hide_Source_Field )
      state_names += "|" + string( "Hide_Source_Field" );
   if( state & c_modifier_Hide_Source_Grandchild )
      state_names += "|" + string( "Hide_Source_Grandchild" );
   if( state & c_modifier_Hide_Source_Parent )
      state_names += "|" + string( "Hide_Source_Parent" );
   if( state & c_modifier_Hide_Test_Child )
      state_names += "|" + string( "Hide_Test_Child" );
   if( state & c_modifier_Hide_Test_Field )
      state_names += "|" + string( "Hide_Test_Field" );
   if( state & c_modifier_Hide_Test_Parent )
      state_names += "|" + string( "Hide_Test_Parent" );
   if( state & c_modifier_Hide_Test_Value )
      state_names += "|" + string( "Hide_Test_Value" );
   if( state & c_modifier_Hide_Use_Source_Parent )
      state_names += "|" + string( "Hide_Use_Source_Parent" );
   if( state & c_modifier_Hide_Use_Test_Parent_Child )
      state_names += "|" + string( "Hide_Use_Test_Parent_Child" );
   if( state & c_modifier_Hide_Value )
      state_names += "|" + string( "Hide_Value" );
   if( state & c_modifier_Is_Comment )
      state_names += "|" + string( "Is_Comment" );
   if( state & c_modifier_Protect_Child_Relationship )
      state_names += "|" + string( "Protect_Child_Relationship" );
   if( state & c_modifier_Protect_Class )
      state_names += "|" + string( "Protect_Class" );
   if( state & c_modifier_Protect_Enum )
      state_names += "|" + string( "Protect_Enum" );
   if( state & c_modifier_Protect_Other_Class )
      state_names += "|" + string( "Protect_Other_Class" );
   if( state & c_modifier_Protect_Procedure )
      state_names += "|" + string( "Protect_Procedure" );
   if( state & c_modifier_Protect_Source_Parent )
      state_names += "|" + string( "Protect_Source_Parent" );

   return state_names.empty( ) ? state_names : state_names.substr( 1 );
}

const string& Meta_Specification::impl::execute( const string& cmd_and_args )
{
   execute_command( cmd_and_args );
   return retval;
}

void Meta_Specification::impl::clear_foreign_key( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( ( field == c_field_id_Child_Rel_Child_Class ) || ( field == c_field_name_Child_Rel_Child_Class ) )
      impl_Child_Rel_Child_Class( "" );
   else if( ( field == c_field_id_Child_Relationship ) || ( field == c_field_name_Child_Relationship ) )
      impl_Child_Relationship( "" );
   else if( ( field == c_field_id_Child_Specification_Type ) || ( field == c_field_name_Child_Specification_Type ) )
      impl_Child_Specification_Type( "" );
   else if( ( field == c_field_id_Class ) || ( field == c_field_name_Class ) )
      impl_Class( "" );
   else if( ( field == c_field_id_Enum ) || ( field == c_field_name_Enum ) )
      impl_Enum( "" );
   else if( ( field == c_field_id_Enum_Item ) || ( field == c_field_name_Enum_Item ) )
      impl_Enum_Item( "" );
   else if( ( field == c_field_id_Enum_Item_2 ) || ( field == c_field_name_Enum_Item_2 ) )
      impl_Enum_Item_2( "" );
   else if( ( field == c_field_id_Enum_Item_3 ) || ( field == c_field_name_Enum_Item_3 ) )
      impl_Enum_Item_3( "" );
   else if( ( field == c_field_id_Enum_Item_4 ) || ( field == c_field_name_Enum_Item_4 ) )
      impl_Enum_Item_4( "" );
   else if( ( field == c_field_id_Enum_Item_5 ) || ( field == c_field_name_Enum_Item_5 ) )
      impl_Enum_Item_5( "" );
   else if( ( field == c_field_id_Field ) || ( field == c_field_name_Field ) )
      impl_Field( "" );
   else if( ( field == c_field_id_Field_Class ) || ( field == c_field_name_Field_Class ) )
      impl_Field_Class( "" );
   else if( ( field == c_field_id_Model ) || ( field == c_field_name_Model ) )
      impl_Model( "" );
   else if( ( field == c_field_id_Modifier ) || ( field == c_field_name_Modifier ) )
      impl_Modifier( "" );
   else if( ( field == c_field_id_Other_Class ) || ( field == c_field_name_Other_Class ) )
      impl_Other_Class( "" );
   else if( ( field == c_field_id_Other_Class_Field ) || ( field == c_field_name_Other_Class_Field ) )
      impl_Other_Class_Field( "" );
   else if( ( field == c_field_id_Other_Field ) || ( field == c_field_name_Other_Field ) )
      impl_Other_Field( "" );
   else if( ( field == c_field_id_Other_Field_2 ) || ( field == c_field_name_Other_Field_2 ) )
      impl_Other_Field_2( "" );
   else if( ( field == c_field_id_Other_Modifier ) || ( field == c_field_name_Other_Modifier ) )
      impl_Other_Modifier( "" );
   else if( ( field == c_field_id_Other_Modifier_2 ) || ( field == c_field_name_Other_Modifier_2 ) )
      impl_Other_Modifier_2( "" );
   else if( ( field == c_field_id_Other_Permission ) || ( field == c_field_name_Other_Permission ) )
      impl_Other_Permission( "" );
   else if( ( field == c_field_id_Other_Permission_2 ) || ( field == c_field_name_Other_Permission_2 ) )
      impl_Other_Permission_2( "" );
   else if( ( field == c_field_id_Other_Procedure ) || ( field == c_field_name_Other_Procedure ) )
      impl_Other_Procedure( "" );
   else if( ( field == c_field_id_Other_Procedure_2 ) || ( field == c_field_name_Other_Procedure_2 ) )
      impl_Other_Procedure_2( "" );
   else if( ( field == c_field_id_Other_Source_Child ) || ( field == c_field_name_Other_Source_Child ) )
      impl_Other_Source_Child( "" );
   else if( ( field == c_field_id_Other_Source_Child_2 ) || ( field == c_field_name_Other_Source_Child_2 ) )
      impl_Other_Source_Child_2( "" );
   else if( ( field == c_field_id_Parent_Specification ) || ( field == c_field_name_Parent_Specification ) )
      impl_Parent_Specification( "" );
   else if( ( field == c_field_id_Permission ) || ( field == c_field_name_Permission ) )
      impl_Permission( "" );
   else if( ( field == c_field_id_Procedure ) || ( field == c_field_name_Procedure ) )
      impl_Procedure( "" );
   else if( ( field == c_field_id_Procedure_Arg ) || ( field == c_field_name_Procedure_Arg ) )
      impl_Procedure_Arg( "" );
   else if( ( field == c_field_id_Procedure_Arg_2 ) || ( field == c_field_name_Procedure_Arg_2 ) )
      impl_Procedure_Arg_2( "" );
   else if( ( field == c_field_id_Procedure_Arg_3 ) || ( field == c_field_name_Procedure_Arg_3 ) )
      impl_Procedure_Arg_3( "" );
   else if( ( field == c_field_id_Source_Child ) || ( field == c_field_name_Source_Child ) )
      impl_Source_Child( "" );
   else if( ( field == c_field_id_Source_Child_Class ) || ( field == c_field_name_Source_Child_Class ) )
      impl_Source_Child_Class( "" );
   else if( ( field == c_field_id_Source_Class ) || ( field == c_field_name_Source_Class ) )
      impl_Source_Class( "" );
   else if( ( field == c_field_id_Source_Field ) || ( field == c_field_name_Source_Field ) )
      impl_Source_Field( "" );
   else if( ( field == c_field_id_Source_Field_Class ) || ( field == c_field_name_Source_Field_Class ) )
      impl_Source_Field_Class( "" );
   else if( ( field == c_field_id_Source_Grandchild ) || ( field == c_field_name_Source_Grandchild ) )
      impl_Source_Grandchild( "" );
   else if( ( field == c_field_id_Source_Parent ) || ( field == c_field_name_Source_Parent ) )
      impl_Source_Parent( "" );
   else if( ( field == c_field_id_Source_Parent_Class ) || ( field == c_field_name_Source_Parent_Class ) )
      impl_Source_Parent_Class( "" );
   else if( ( field == c_field_id_Specification_Type ) || ( field == c_field_name_Specification_Type ) )
      impl_Specification_Type( "" );
   else if( ( field == c_field_id_Test_Child ) || ( field == c_field_name_Test_Child ) )
      impl_Test_Child( "" );
   else if( ( field == c_field_id_Test_Field ) || ( field == c_field_name_Test_Field ) )
      impl_Test_Field( "" );
   else if( ( field == c_field_id_Test_Field_Class ) || ( field == c_field_name_Test_Field_Class ) )
      impl_Test_Field_Class( "" );
   else if( ( field == c_field_id_Test_Parent ) || ( field == c_field_name_Test_Parent ) )
      impl_Test_Parent( "" );
   else if( ( field == c_field_id_Test_Parent_Class ) || ( field == c_field_name_Test_Parent_Class ) )
      impl_Test_Parent_Class( "" );
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Specification::impl::set_foreign_key_value( const string& field, const string& value )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for value: " + value );
   else if( ( field == c_field_id_Child_Rel_Child_Class ) || ( field == c_field_name_Child_Rel_Child_Class ) )
      v_Child_Rel_Child_Class = value;
   else if( ( field == c_field_id_Child_Relationship ) || ( field == c_field_name_Child_Relationship ) )
      v_Child_Relationship = value;
   else if( ( field == c_field_id_Child_Specification_Type ) || ( field == c_field_name_Child_Specification_Type ) )
      v_Child_Specification_Type = value;
   else if( ( field == c_field_id_Class ) || ( field == c_field_name_Class ) )
      v_Class = value;
   else if( ( field == c_field_id_Enum ) || ( field == c_field_name_Enum ) )
      v_Enum = value;
   else if( ( field == c_field_id_Enum_Item ) || ( field == c_field_name_Enum_Item ) )
      v_Enum_Item = value;
   else if( ( field == c_field_id_Enum_Item_2 ) || ( field == c_field_name_Enum_Item_2 ) )
      v_Enum_Item_2 = value;
   else if( ( field == c_field_id_Enum_Item_3 ) || ( field == c_field_name_Enum_Item_3 ) )
      v_Enum_Item_3 = value;
   else if( ( field == c_field_id_Enum_Item_4 ) || ( field == c_field_name_Enum_Item_4 ) )
      v_Enum_Item_4 = value;
   else if( ( field == c_field_id_Enum_Item_5 ) || ( field == c_field_name_Enum_Item_5 ) )
      v_Enum_Item_5 = value;
   else if( ( field == c_field_id_Field ) || ( field == c_field_name_Field ) )
      v_Field = value;
   else if( ( field == c_field_id_Field_Class ) || ( field == c_field_name_Field_Class ) )
      v_Field_Class = value;
   else if( ( field == c_field_id_Model ) || ( field == c_field_name_Model ) )
      v_Model = value;
   else if( ( field == c_field_id_Modifier ) || ( field == c_field_name_Modifier ) )
      v_Modifier = value;
   else if( ( field == c_field_id_Other_Class ) || ( field == c_field_name_Other_Class ) )
      v_Other_Class = value;
   else if( ( field == c_field_id_Other_Class_Field ) || ( field == c_field_name_Other_Class_Field ) )
      v_Other_Class_Field = value;
   else if( ( field == c_field_id_Other_Field ) || ( field == c_field_name_Other_Field ) )
      v_Other_Field = value;
   else if( ( field == c_field_id_Other_Field_2 ) || ( field == c_field_name_Other_Field_2 ) )
      v_Other_Field_2 = value;
   else if( ( field == c_field_id_Other_Modifier ) || ( field == c_field_name_Other_Modifier ) )
      v_Other_Modifier = value;
   else if( ( field == c_field_id_Other_Modifier_2 ) || ( field == c_field_name_Other_Modifier_2 ) )
      v_Other_Modifier_2 = value;
   else if( ( field == c_field_id_Other_Permission ) || ( field == c_field_name_Other_Permission ) )
      v_Other_Permission = value;
   else if( ( field == c_field_id_Other_Permission_2 ) || ( field == c_field_name_Other_Permission_2 ) )
      v_Other_Permission_2 = value;
   else if( ( field == c_field_id_Other_Procedure ) || ( field == c_field_name_Other_Procedure ) )
      v_Other_Procedure = value;
   else if( ( field == c_field_id_Other_Procedure_2 ) || ( field == c_field_name_Other_Procedure_2 ) )
      v_Other_Procedure_2 = value;
   else if( ( field == c_field_id_Other_Source_Child ) || ( field == c_field_name_Other_Source_Child ) )
      v_Other_Source_Child = value;
   else if( ( field == c_field_id_Other_Source_Child_2 ) || ( field == c_field_name_Other_Source_Child_2 ) )
      v_Other_Source_Child_2 = value;
   else if( ( field == c_field_id_Parent_Specification ) || ( field == c_field_name_Parent_Specification ) )
      v_Parent_Specification = value;
   else if( ( field == c_field_id_Permission ) || ( field == c_field_name_Permission ) )
      v_Permission = value;
   else if( ( field == c_field_id_Procedure ) || ( field == c_field_name_Procedure ) )
      v_Procedure = value;
   else if( ( field == c_field_id_Procedure_Arg ) || ( field == c_field_name_Procedure_Arg ) )
      v_Procedure_Arg = value;
   else if( ( field == c_field_id_Procedure_Arg_2 ) || ( field == c_field_name_Procedure_Arg_2 ) )
      v_Procedure_Arg_2 = value;
   else if( ( field == c_field_id_Procedure_Arg_3 ) || ( field == c_field_name_Procedure_Arg_3 ) )
      v_Procedure_Arg_3 = value;
   else if( ( field == c_field_id_Source_Child ) || ( field == c_field_name_Source_Child ) )
      v_Source_Child = value;
   else if( ( field == c_field_id_Source_Child_Class ) || ( field == c_field_name_Source_Child_Class ) )
      v_Source_Child_Class = value;
   else if( ( field == c_field_id_Source_Class ) || ( field == c_field_name_Source_Class ) )
      v_Source_Class = value;
   else if( ( field == c_field_id_Source_Field ) || ( field == c_field_name_Source_Field ) )
      v_Source_Field = value;
   else if( ( field == c_field_id_Source_Field_Class ) || ( field == c_field_name_Source_Field_Class ) )
      v_Source_Field_Class = value;
   else if( ( field == c_field_id_Source_Grandchild ) || ( field == c_field_name_Source_Grandchild ) )
      v_Source_Grandchild = value;
   else if( ( field == c_field_id_Source_Parent ) || ( field == c_field_name_Source_Parent ) )
      v_Source_Parent = value;
   else if( ( field == c_field_id_Source_Parent_Class ) || ( field == c_field_name_Source_Parent_Class ) )
      v_Source_Parent_Class = value;
   else if( ( field == c_field_id_Specification_Type ) || ( field == c_field_name_Specification_Type ) )
      v_Specification_Type = value;
   else if( ( field == c_field_id_Test_Child ) || ( field == c_field_name_Test_Child ) )
      v_Test_Child = value;
   else if( ( field == c_field_id_Test_Field ) || ( field == c_field_name_Test_Field ) )
      v_Test_Field = value;
   else if( ( field == c_field_id_Test_Field_Class ) || ( field == c_field_name_Test_Field_Class ) )
      v_Test_Field_Class = value;
   else if( ( field == c_field_id_Test_Parent ) || ( field == c_field_name_Test_Parent ) )
      v_Test_Parent = value;
   else if( ( field == c_field_id_Test_Parent_Class ) || ( field == c_field_name_Test_Parent_Class ) )
      v_Test_Parent_Class = value;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

const string& Meta_Specification::impl::get_foreign_key_value( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( ( field == c_field_id_Child_Rel_Child_Class ) || ( field == c_field_name_Child_Rel_Child_Class ) )
      return v_Child_Rel_Child_Class;
   else if( ( field == c_field_id_Child_Relationship ) || ( field == c_field_name_Child_Relationship ) )
      return v_Child_Relationship;
   else if( ( field == c_field_id_Child_Specification_Type ) || ( field == c_field_name_Child_Specification_Type ) )
      return v_Child_Specification_Type;
   else if( ( field == c_field_id_Class ) || ( field == c_field_name_Class ) )
      return v_Class;
   else if( ( field == c_field_id_Enum ) || ( field == c_field_name_Enum ) )
      return v_Enum;
   else if( ( field == c_field_id_Enum_Item ) || ( field == c_field_name_Enum_Item ) )
      return v_Enum_Item;
   else if( ( field == c_field_id_Enum_Item_2 ) || ( field == c_field_name_Enum_Item_2 ) )
      return v_Enum_Item_2;
   else if( ( field == c_field_id_Enum_Item_3 ) || ( field == c_field_name_Enum_Item_3 ) )
      return v_Enum_Item_3;
   else if( ( field == c_field_id_Enum_Item_4 ) || ( field == c_field_name_Enum_Item_4 ) )
      return v_Enum_Item_4;
   else if( ( field == c_field_id_Enum_Item_5 ) || ( field == c_field_name_Enum_Item_5 ) )
      return v_Enum_Item_5;
   else if( ( field == c_field_id_Field ) || ( field == c_field_name_Field ) )
      return v_Field;
   else if( ( field == c_field_id_Field_Class ) || ( field == c_field_name_Field_Class ) )
      return v_Field_Class;
   else if( ( field == c_field_id_Model ) || ( field == c_field_name_Model ) )
      return v_Model;
   else if( ( field == c_field_id_Modifier ) || ( field == c_field_name_Modifier ) )
      return v_Modifier;
   else if( ( field == c_field_id_Other_Class ) || ( field == c_field_name_Other_Class ) )
      return v_Other_Class;
   else if( ( field == c_field_id_Other_Class_Field ) || ( field == c_field_name_Other_Class_Field ) )
      return v_Other_Class_Field;
   else if( ( field == c_field_id_Other_Field ) || ( field == c_field_name_Other_Field ) )
      return v_Other_Field;
   else if( ( field == c_field_id_Other_Field_2 ) || ( field == c_field_name_Other_Field_2 ) )
      return v_Other_Field_2;
   else if( ( field == c_field_id_Other_Modifier ) || ( field == c_field_name_Other_Modifier ) )
      return v_Other_Modifier;
   else if( ( field == c_field_id_Other_Modifier_2 ) || ( field == c_field_name_Other_Modifier_2 ) )
      return v_Other_Modifier_2;
   else if( ( field == c_field_id_Other_Permission ) || ( field == c_field_name_Other_Permission ) )
      return v_Other_Permission;
   else if( ( field == c_field_id_Other_Permission_2 ) || ( field == c_field_name_Other_Permission_2 ) )
      return v_Other_Permission_2;
   else if( ( field == c_field_id_Other_Procedure ) || ( field == c_field_name_Other_Procedure ) )
      return v_Other_Procedure;
   else if( ( field == c_field_id_Other_Procedure_2 ) || ( field == c_field_name_Other_Procedure_2 ) )
      return v_Other_Procedure_2;
   else if( ( field == c_field_id_Other_Source_Child ) || ( field == c_field_name_Other_Source_Child ) )
      return v_Other_Source_Child;
   else if( ( field == c_field_id_Other_Source_Child_2 ) || ( field == c_field_name_Other_Source_Child_2 ) )
      return v_Other_Source_Child_2;
   else if( ( field == c_field_id_Parent_Specification ) || ( field == c_field_name_Parent_Specification ) )
      return v_Parent_Specification;
   else if( ( field == c_field_id_Permission ) || ( field == c_field_name_Permission ) )
      return v_Permission;
   else if( ( field == c_field_id_Procedure ) || ( field == c_field_name_Procedure ) )
      return v_Procedure;
   else if( ( field == c_field_id_Procedure_Arg ) || ( field == c_field_name_Procedure_Arg ) )
      return v_Procedure_Arg;
   else if( ( field == c_field_id_Procedure_Arg_2 ) || ( field == c_field_name_Procedure_Arg_2 ) )
      return v_Procedure_Arg_2;
   else if( ( field == c_field_id_Procedure_Arg_3 ) || ( field == c_field_name_Procedure_Arg_3 ) )
      return v_Procedure_Arg_3;
   else if( ( field == c_field_id_Source_Child ) || ( field == c_field_name_Source_Child ) )
      return v_Source_Child;
   else if( ( field == c_field_id_Source_Child_Class ) || ( field == c_field_name_Source_Child_Class ) )
      return v_Source_Child_Class;
   else if( ( field == c_field_id_Source_Class ) || ( field == c_field_name_Source_Class ) )
      return v_Source_Class;
   else if( ( field == c_field_id_Source_Field ) || ( field == c_field_name_Source_Field ) )
      return v_Source_Field;
   else if( ( field == c_field_id_Source_Field_Class ) || ( field == c_field_name_Source_Field_Class ) )
      return v_Source_Field_Class;
   else if( ( field == c_field_id_Source_Grandchild ) || ( field == c_field_name_Source_Grandchild ) )
      return v_Source_Grandchild;
   else if( ( field == c_field_id_Source_Parent ) || ( field == c_field_name_Source_Parent ) )
      return v_Source_Parent;
   else if( ( field == c_field_id_Source_Parent_Class ) || ( field == c_field_name_Source_Parent_Class ) )
      return v_Source_Parent_Class;
   else if( ( field == c_field_id_Specification_Type ) || ( field == c_field_name_Specification_Type ) )
      return v_Specification_Type;
   else if( ( field == c_field_id_Test_Child ) || ( field == c_field_name_Test_Child ) )
      return v_Test_Child;
   else if( ( field == c_field_id_Test_Field ) || ( field == c_field_name_Test_Field ) )
      return v_Test_Field;
   else if( ( field == c_field_id_Test_Field_Class ) || ( field == c_field_name_Test_Field_Class ) )
      return v_Test_Field_Class;
   else if( ( field == c_field_id_Test_Parent ) || ( field == c_field_name_Test_Parent ) )
      return v_Test_Parent;
   else if( ( field == c_field_id_Test_Parent_Class ) || ( field == c_field_name_Test_Parent_Class ) )
      return v_Test_Parent_Class;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Specification::impl::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Child_Rel_Child_Class, v_Child_Rel_Child_Class ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Child_Relationship, v_Child_Relationship ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Child_Specification_Type, v_Child_Specification_Type ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Class, v_Class ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Enum, v_Enum ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Enum_Item, v_Enum_Item ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Enum_Item_2, v_Enum_Item_2 ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Enum_Item_3, v_Enum_Item_3 ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Enum_Item_4, v_Enum_Item_4 ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Enum_Item_5, v_Enum_Item_5 ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Field, v_Field ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Field_Class, v_Field_Class ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Model, v_Model ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Modifier, v_Modifier ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Other_Class, v_Other_Class ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Other_Class_Field, v_Other_Class_Field ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Other_Field, v_Other_Field ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Other_Field_2, v_Other_Field_2 ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Other_Modifier, v_Other_Modifier ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Other_Modifier_2, v_Other_Modifier_2 ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Other_Permission, v_Other_Permission ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Other_Permission_2, v_Other_Permission_2 ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Other_Procedure, v_Other_Procedure ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Other_Procedure_2, v_Other_Procedure_2 ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Other_Source_Child, v_Other_Source_Child ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Other_Source_Child_2, v_Other_Source_Child_2 ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Parent_Specification, v_Parent_Specification ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Permission, v_Permission ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Procedure, v_Procedure ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Procedure_Arg, v_Procedure_Arg ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Procedure_Arg_2, v_Procedure_Arg_2 ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Procedure_Arg_3, v_Procedure_Arg_3 ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Source_Child, v_Source_Child ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Source_Child_Class, v_Source_Child_Class ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Source_Class, v_Source_Class ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Source_Field, v_Source_Field ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Source_Field_Class, v_Source_Field_Class ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Source_Grandchild, v_Source_Grandchild ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Source_Parent, v_Source_Parent ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Source_Parent_Class, v_Source_Parent_Class ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Specification_Type, v_Specification_Type ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Test_Child, v_Test_Child ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Test_Field, v_Test_Field ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Test_Field_Class, v_Test_Field_Class ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Test_Parent, v_Test_Parent ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Test_Parent_Class, v_Test_Parent_Class ) );
}

void Meta_Specification::impl::add_extra_fixed_info( vector< pair< string, string > >& fixed_info ) const
{
   ( void )fixed_info;

   // [<start add_extra_fixed_info>]
   // [<finish add_extra_fixed_info>]
}

void Meta_Specification::impl::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   ( void )paging_info;

   // [<start add_extra_paging_info>]
   // [<finish add_extra_paging_info>]
}

void Meta_Specification::impl::clear( )
{
   v_Actions = g_default_Actions;
   v_All_Strings = g_default_All_Strings;
   v_All_Vars = g_default_All_Vars;
   v_Can_Access_Below_Model = g_default_Can_Access_Below_Model;
   v_Child_Rel_Field_Key = g_default_Child_Rel_Field_Key;
   v_Comments = g_default_Comments;
   v_Field_Pairs = g_default_Field_Pairs;
   v_Field_Values = g_default_Field_Values;
   v_Fields = g_default_Fields;
   v_Id = g_default_Id;
   v_Is_Required_For_UI_Gen = g_default_Is_Required_For_UI_Gen;
   v_Name = g_default_Name;
   v_Options = g_default_Options;
   v_Order = g_default_Order;
   v_Protect_Child_Rel = g_default_Protect_Child_Rel;
   v_Protect_Procedure = g_default_Protect_Procedure;
   v_Protect_Source_Parent = g_default_Protect_Source_Parent;
   v_Restrict_Values = g_default_Restrict_Values;
   v_Strings = g_default_Strings;
   v_Test_Value = g_default_Test_Value;
   v_Test_Value_Label = g_default_Test_Value_Label;
   v_Use_Source_Parent = g_default_Use_Source_Parent;
   v_Use_Test_Parent_Child = g_default_Use_Test_Parent_Child;
   v_Value = g_default_Value;
   v_Value_Label = g_default_Value_Label;
   v_Value_Left_Part = g_default_Value_Left_Part;
   v_Value_Literal = g_default_Value_Literal;
   v_Value_Numeric_String = g_default_Value_Numeric_String;
   v_Value_Right_Part = g_default_Value_Right_Part;
   v_Value_String = g_default_Value_String;
   v_Vars = g_default_Vars;

   v_Child_Rel_Child_Class = string( );
   if( cp_Child_Rel_Child_Class )
      p_obj->setup_foreign_key( *cp_Child_Rel_Child_Class, v_Child_Rel_Child_Class );

   v_Child_Relationship = string( );
   if( cp_Child_Relationship )
      p_obj->setup_foreign_key( *cp_Child_Relationship, v_Child_Relationship );

   v_Child_Specification_Type = string( );
   if( cp_Child_Specification_Type )
      p_obj->setup_foreign_key( *cp_Child_Specification_Type, v_Child_Specification_Type );

   v_Class = string( );
   if( cp_Class )
      p_obj->setup_foreign_key( *cp_Class, v_Class );

   v_Enum = string( );
   if( cp_Enum )
      p_obj->setup_foreign_key( *cp_Enum, v_Enum );

   v_Enum_Item = string( );
   if( cp_Enum_Item )
      p_obj->setup_foreign_key( *cp_Enum_Item, v_Enum_Item );

   v_Enum_Item_2 = string( );
   if( cp_Enum_Item_2 )
      p_obj->setup_foreign_key( *cp_Enum_Item_2, v_Enum_Item_2 );

   v_Enum_Item_3 = string( );
   if( cp_Enum_Item_3 )
      p_obj->setup_foreign_key( *cp_Enum_Item_3, v_Enum_Item_3 );

   v_Enum_Item_4 = string( );
   if( cp_Enum_Item_4 )
      p_obj->setup_foreign_key( *cp_Enum_Item_4, v_Enum_Item_4 );

   v_Enum_Item_5 = string( );
   if( cp_Enum_Item_5 )
      p_obj->setup_foreign_key( *cp_Enum_Item_5, v_Enum_Item_5 );

   v_Field = string( );
   if( cp_Field )
      p_obj->setup_foreign_key( *cp_Field, v_Field );

   v_Field_Class = string( );
   if( cp_Field_Class )
      p_obj->setup_foreign_key( *cp_Field_Class, v_Field_Class );

   v_Model = string( );
   if( cp_Model )
      p_obj->setup_foreign_key( *cp_Model, v_Model );

   v_Modifier = string( );
   if( cp_Modifier )
      p_obj->setup_foreign_key( *cp_Modifier, v_Modifier );

   v_Other_Class = string( );
   if( cp_Other_Class )
      p_obj->setup_foreign_key( *cp_Other_Class, v_Other_Class );

   v_Other_Class_Field = string( );
   if( cp_Other_Class_Field )
      p_obj->setup_foreign_key( *cp_Other_Class_Field, v_Other_Class_Field );

   v_Other_Field = string( );
   if( cp_Other_Field )
      p_obj->setup_foreign_key( *cp_Other_Field, v_Other_Field );

   v_Other_Field_2 = string( );
   if( cp_Other_Field_2 )
      p_obj->setup_foreign_key( *cp_Other_Field_2, v_Other_Field_2 );

   v_Other_Modifier = string( );
   if( cp_Other_Modifier )
      p_obj->setup_foreign_key( *cp_Other_Modifier, v_Other_Modifier );

   v_Other_Modifier_2 = string( );
   if( cp_Other_Modifier_2 )
      p_obj->setup_foreign_key( *cp_Other_Modifier_2, v_Other_Modifier_2 );

   v_Other_Permission = string( );
   if( cp_Other_Permission )
      p_obj->setup_foreign_key( *cp_Other_Permission, v_Other_Permission );

   v_Other_Permission_2 = string( );
   if( cp_Other_Permission_2 )
      p_obj->setup_foreign_key( *cp_Other_Permission_2, v_Other_Permission_2 );

   v_Other_Procedure = string( );
   if( cp_Other_Procedure )
      p_obj->setup_foreign_key( *cp_Other_Procedure, v_Other_Procedure );

   v_Other_Procedure_2 = string( );
   if( cp_Other_Procedure_2 )
      p_obj->setup_foreign_key( *cp_Other_Procedure_2, v_Other_Procedure_2 );

   v_Other_Source_Child = string( );
   if( cp_Other_Source_Child )
      p_obj->setup_foreign_key( *cp_Other_Source_Child, v_Other_Source_Child );

   v_Other_Source_Child_2 = string( );
   if( cp_Other_Source_Child_2 )
      p_obj->setup_foreign_key( *cp_Other_Source_Child_2, v_Other_Source_Child_2 );

   v_Parent_Specification = string( );
   if( cp_Parent_Specification )
      p_obj->setup_foreign_key( *cp_Parent_Specification, v_Parent_Specification );

   v_Permission = string( );
   if( cp_Permission )
      p_obj->setup_foreign_key( *cp_Permission, v_Permission );

   v_Procedure = string( );
   if( cp_Procedure )
      p_obj->setup_foreign_key( *cp_Procedure, v_Procedure );

   v_Procedure_Arg = string( );
   if( cp_Procedure_Arg )
      p_obj->setup_foreign_key( *cp_Procedure_Arg, v_Procedure_Arg );

   v_Procedure_Arg_2 = string( );
   if( cp_Procedure_Arg_2 )
      p_obj->setup_foreign_key( *cp_Procedure_Arg_2, v_Procedure_Arg_2 );

   v_Procedure_Arg_3 = string( );
   if( cp_Procedure_Arg_3 )
      p_obj->setup_foreign_key( *cp_Procedure_Arg_3, v_Procedure_Arg_3 );

   v_Source_Child = string( );
   if( cp_Source_Child )
      p_obj->setup_foreign_key( *cp_Source_Child, v_Source_Child );

   v_Source_Child_Class = string( );
   if( cp_Source_Child_Class )
      p_obj->setup_foreign_key( *cp_Source_Child_Class, v_Source_Child_Class );

   v_Source_Class = string( );
   if( cp_Source_Class )
      p_obj->setup_foreign_key( *cp_Source_Class, v_Source_Class );

   v_Source_Field = string( );
   if( cp_Source_Field )
      p_obj->setup_foreign_key( *cp_Source_Field, v_Source_Field );

   v_Source_Field_Class = string( );
   if( cp_Source_Field_Class )
      p_obj->setup_foreign_key( *cp_Source_Field_Class, v_Source_Field_Class );

   v_Source_Grandchild = string( );
   if( cp_Source_Grandchild )
      p_obj->setup_foreign_key( *cp_Source_Grandchild, v_Source_Grandchild );

   v_Source_Parent = string( );
   if( cp_Source_Parent )
      p_obj->setup_foreign_key( *cp_Source_Parent, v_Source_Parent );

   v_Source_Parent_Class = string( );
   if( cp_Source_Parent_Class )
      p_obj->setup_foreign_key( *cp_Source_Parent_Class, v_Source_Parent_Class );

   v_Specification_Type = string( );
   if( cp_Specification_Type )
      p_obj->setup_foreign_key( *cp_Specification_Type, v_Specification_Type );

   v_Test_Child = string( );
   if( cp_Test_Child )
      p_obj->setup_foreign_key( *cp_Test_Child, v_Test_Child );

   v_Test_Field = string( );
   if( cp_Test_Field )
      p_obj->setup_foreign_key( *cp_Test_Field, v_Test_Field );

   v_Test_Field_Class = string( );
   if( cp_Test_Field_Class )
      p_obj->setup_foreign_key( *cp_Test_Field_Class, v_Test_Field_Class );

   v_Test_Parent = string( );
   if( cp_Test_Parent )
      p_obj->setup_foreign_key( *cp_Test_Parent, v_Test_Parent );

   v_Test_Parent_Class = string( );
   if( cp_Test_Parent_Class )
      p_obj->setup_foreign_key( *cp_Test_Parent_Class, v_Test_Parent_Class );
}

bool Meta_Specification::impl::value_will_be_provided( const string& field_name )
{
   ( void )field_name;

   // [(start parent_auto_int_inc)] 610194
   if( field_name == "Id" )
      return true;
   // [(finish parent_auto_int_inc)] 610194

   // [<start value_will_be_provided>]
   // [<finish value_will_be_provided>]

   return false;
}

void Meta_Specification::impl::validate(
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

   if( is_null( v_Name ) && !value_will_be_provided( c_field_name_Name ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Name,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Name ) ) ) ) );

   if( is_null( v_Order ) && !value_will_be_provided( c_field_name_Order ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Order,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Order ) ) ) ) );

   if( v_Class.empty( ) && !value_will_be_provided( c_field_name_Class ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Class,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Class ) ) ) ) );

   if( v_Model.empty( ) && !value_will_be_provided( c_field_name_Model ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Model,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Model ) ) ) ) );

   if( v_Specification_Type.empty( ) && !value_will_be_provided( c_field_name_Specification_Type ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Specification_Type,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Specification_Type ) ) ) ) );

   if( !is_null( v_Comments )
    && ( v_Comments != g_default_Comments
    || !value_will_be_provided( c_field_name_Comments ) )
    && !g_Comments_domain.is_valid( v_Comments, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Comments,
       get_module_string( c_field_display_name_Comments ) + " " + error_message ) );

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

   if( !is_null( v_Options )
    && ( v_Options != g_default_Options
    || !value_will_be_provided( c_field_name_Options ) )
    && !g_Options_domain.is_valid( v_Options, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Options,
       get_module_string( c_field_display_name_Options ) + " " + error_message ) );

   if( !is_null( v_Test_Value )
    && ( v_Test_Value != g_default_Test_Value
    || !value_will_be_provided( c_field_name_Test_Value ) )
    && !g_Test_Value_domain.is_valid( v_Test_Value, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Test_Value,
       get_module_string( c_field_display_name_Test_Value ) + " " + error_message ) );

   if( !is_null( v_Value )
    && ( v_Value != g_default_Value
    || !value_will_be_provided( c_field_name_Value ) )
    && !g_Value_domain.is_valid( v_Value, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Value,
       get_module_string( c_field_display_name_Value ) + " " + error_message ) );

   // [(start check_cond_non_null)] 600130
   if( get_obj( ).Specification_Type( ).Needs_Class( ) && is_null( get_obj( ).Class( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_Class, get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Class ) ) ) ) );
   // [(finish check_cond_non_null)] 600130

   // [(start check_cond_non_null)] 600130a
   if( get_obj( ).Specification_Type( ).Needs_Other_Class( ) && is_null( get_obj( ).Other_Class( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_Other_Class, get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Other_Class ) ) ) ) );
   // [(finish check_cond_non_null)] 600130a

   // [(start check_cond_non_null)] 600131
   if( get_obj( ).Specification_Type( ).Needs_Field( ) && is_null( get_obj( ).Field( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_Field, get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Field ) ) ) ) );
   // [(finish check_cond_non_null)] 600131

   // [(start check_cond_non_null)] 600132
   if( get_obj( ).Specification_Type( ).Needs_Procedure( ) && is_null( get_obj( ).Procedure( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_Procedure, get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Procedure ) ) ) ) );
   // [(finish check_cond_non_null)] 600132

   // [(start check_cond_non_null)] 600133
   if( get_obj( ).Specification_Type( ).Needs_Procedure_Arg( ) && is_null( get_obj( ).Procedure_Arg( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_Procedure_Arg, get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Procedure_Arg ) ) ) ) );
   // [(finish check_cond_non_null)] 600133

   // [(start check_cond_non_null)] 600133a
   if( get_obj( ).Specification_Type( ).Needs_Procedure_Arg_2( ) && is_null( get_obj( ).Procedure_Arg_2( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_Procedure_Arg_2, get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Procedure_Arg_2 ) ) ) ) );
   // [(finish check_cond_non_null)] 600133a

   // [(start check_cond_non_null)] 600133b
   if( get_obj( ).Specification_Type( ).Needs_Procedure_Arg_3( ) && is_null( get_obj( ).Procedure_Arg_3( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_Procedure_Arg_3, get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Procedure_Arg_3 ) ) ) ) );
   // [(finish check_cond_non_null)] 600133b

   // [(start check_cond_non_null)] 600133c
   if( get_obj( ).Specification_Type( ).Needs_Other_Procedure( ) && is_null( get_obj( ).Other_Procedure( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_Other_Procedure, get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Other_Procedure ) ) ) ) );
   // [(finish check_cond_non_null)] 600133c

   // [(start check_cond_non_null)] 600133d
   if( get_obj( ).Specification_Type( ).Needs_Other_Procedure_2( ) && is_null( get_obj( ).Other_Procedure_2( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_Other_Procedure_2, get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Other_Procedure_2 ) ) ) ) );
   // [(finish check_cond_non_null)] 600133d

   // [(start check_cond_non_null)] 600134
   if( get_obj( ).Specification_Type( ).Needs_Modifier( ) && is_null( get_obj( ).Modifier( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_Modifier, get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Modifier ) ) ) ) );
   // [(finish check_cond_non_null)] 600134

   // [(start check_cond_non_null)] 600134a
   if( get_obj( ).Specification_Type( ).Needs_Other_Modifier( ) && is_null( get_obj( ).Other_Modifier( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_Other_Modifier, get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Other_Modifier ) ) ) ) );
   // [(finish check_cond_non_null)] 600134a

   // [(start check_cond_non_null)] 600134b
   if( get_obj( ).Specification_Type( ).Needs_Other_Modifier_2( ) && is_null( get_obj( ).Other_Modifier_2( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_Other_Modifier_2, get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Other_Modifier_2 ) ) ) ) );
   // [(finish check_cond_non_null)] 600134b

   // [(start check_cond_non_null)] 600135
   if( get_obj( ).Specification_Type( ).Needs_Enum( ) && is_null( get_obj( ).Enum( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_Enum, get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Enum ) ) ) ) );
   // [(finish check_cond_non_null)] 600135

   // [(start check_cond_non_null)] 600136
   if( get_obj( ).Specification_Type( ).Needs_Enum_Item( ) && is_null( get_obj( ).Enum_Item( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_Enum_Item, get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Enum_Item ) ) ) ) );
   // [(finish check_cond_non_null)] 600136

   // [(start check_cond_non_null)] 600136a
   if( get_obj( ).Specification_Type( ).Needs_Enum_Item_2( ) && is_null( get_obj( ).Enum_Item_2( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_Enum_Item_2, get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Enum_Item_2 ) ) ) ) );
   // [(finish check_cond_non_null)] 600136a

   // [(start check_cond_non_null)] 600136b
   if( get_obj( ).Specification_Type( ).Needs_Enum_Item_3( ) && is_null( get_obj( ).Enum_Item_3( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_Enum_Item_3, get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Enum_Item_3 ) ) ) ) );
   // [(finish check_cond_non_null)] 600136b

   // [(start check_cond_non_null)] 600136c
   if( get_obj( ).Specification_Type( ).Needs_Enum_Item_4( ) && is_null( get_obj( ).Enum_Item_4( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_Enum_Item_4, get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Enum_Item_4 ) ) ) ) );
   // [(finish check_cond_non_null)] 600136c

   // [(start check_cond_non_null)] 600136d
   if( get_obj( ).Specification_Type( ).Needs_Enum_Item_5( ) && is_null( get_obj( ).Enum_Item_5( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_Enum_Item_5, get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Enum_Item_5 ) ) ) ) );
   // [(finish check_cond_non_null)] 600136d

   // [(start check_cond_non_null)] 600137
   if( get_obj( ).Specification_Type( ).Needs_Value( ) && is_null( get_obj( ).Value( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_Value, get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Value ) ) ) ) );
   // [(finish check_cond_non_null)] 600137

   // [(start check_cond_non_null)] 600138
   if( get_obj( ).Specification_Type( ).Needs_Source_Parent( ) && is_null( get_obj( ).Source_Parent( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_Source_Parent, get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Source_Parent ) ) ) ) );
   // [(finish check_cond_non_null)] 600138

   // [(start check_cond_non_null)] 600139
   if( get_obj( ).Specification_Type( ).Needs_Source_Field( ) && is_null( get_obj( ).Source_Field( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_Source_Field, get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Source_Field ) ) ) ) );
   // [(finish check_cond_non_null)] 600139

   // [(start check_cond_non_null)] 600140
   if( get_obj( ).Specification_Type( ).Needs_Test_Field( ) && !get_obj( ).Use_Test_Parent_Child( ) && is_null( get_obj( ).Test_Field( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_Test_Field, get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Test_Field ) ) ) ) );
   // [(finish check_cond_non_null)] 600140

   // [(start check_cond_non_null)] 600141
   if( get_obj( ).Specification_Type( ).Needs_Test_Field( ) && get_obj( ).Use_Test_Parent_Child( ) && is_null( get_obj( ).Test_Child( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_Test_Child, get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Test_Child ) ) ) ) );
   // [(finish check_cond_non_null)] 600141

   // [(start check_cond_non_null)] 600142
   if( get_obj( ).Specification_Type( ).Needs_Test_Value( ) && is_null( get_obj( ).Test_Value( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_Test_Value, get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Test_Value ) ) ) ) );
   // [(finish check_cond_non_null)] 600142

   // [(start check_cond_non_null)] 600143
   if( get_obj( ).Specification_Type( ).Needs_Source_Child( ) && is_null( get_obj( ).Source_Child( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_Source_Child, get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Source_Child ) ) ) ) );
   // [(finish check_cond_non_null)] 600143

   // [(start check_cond_non_null)] 600144
   if( get_obj( ).Specification_Type( ).Needs_Source_Field_Or_Child( ) && !get_obj( ).Use_Source_Parent( ) && is_null( get_obj( ).Source_Field( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_Source_Field, get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Source_Field ) ) ) ) );
   // [(finish check_cond_non_null)] 600144

   // [(start check_cond_non_null)] 600145
   if( get_obj( ).Specification_Type( ).Needs_Source_Field_Or_Child( ) && get_obj( ).Use_Source_Parent( ) && is_null( get_obj( ).Source_Child( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_Source_Child, get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Source_Child ) ) ) ) );
   // [(finish check_cond_non_null)] 600145

   // [(start check_cond_non_null)] 600146
   if( get_obj( ).Specification_Type( ).Needs_Child_Relationship( ) && is_null( get_obj( ).Child_Relationship( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_Child_Relationship, get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Child_Relationship ) ) ) ) );
   // [(finish check_cond_non_null)] 600146

   // [(start meta_spec_field_type)] 600147
   // NOTE: Scope for local variables.
   {
      string error;

      int fld_primitive = -1;
      if( !get_obj( ).Field( ).get_key( ).empty( ) )
         fld_primitive = get_obj( ).Field( ).Primitive( );

      int test_primitive = -1;
      if( get_obj( ).Test_Child( ).get_key( ).empty( ) )
      {
         if( !get_obj( ).Test_Field( ).get_key( ).empty( ) )
            test_primitive = get_obj( ).Test_Field( ).Primitive( );
      }
      else
         test_primitive = get_obj( ).Test_Child( ).Primitive( );

      int source_primitive = -1;
      if( get_obj( ).Source_Child( ).get_key( ).empty( ) )
      {
         if( !get_obj( ).Source_Field( ).get_key( ).empty( ) )
            source_primitive = get_obj( ).Source_Field( ).Primitive( );
      }
      else
         source_primitive = get_obj( ).Source_Child( ).Primitive( );

      get_obj( ).Specification_Type( ).Is_Valid_Field_Type(
       get_obj( ).Class( ).get_key( ), error,
       get_obj( ).Field( ).Parent_Class( ).get_key( ), fld_primitive,
       get_obj( ).Procedure_Arg( ).Primitive( ), get_obj( ).Procedure_Arg( ).Type( ),
       get_obj( ).Source_Parent( ).Parent_Class( ).get_key( ), source_primitive, test_primitive, get_obj( ).Test_Value( ) );

      if( !error.empty( ) )
      {
         p_validation_errors->insert( construct_validation_error( vf.num, "",
          get_string_message( GS( c_str_error ), make_pair( c_str_parm_error_error, error ) ) ) );
      }

      if( get_obj( ).Specification_Type( ).Test_Field_Type_Match( ) && get_obj( ).Field( ).Primitive( ) != test_primitive )
         p_validation_errors->insert( construct_validation_error( vf.num, "",
          get_string_message( GS( c_str_field_types_must_match ), make_pair(
          c_str_parm_field_types_must_match_field1, get_module_string( c_field_display_name_Field ) ),
          make_pair( c_str_parm_field_types_must_match_field2, get_module_string( c_field_display_name_Test_Field ) ) ) ) );

      if( get_obj( ).Specification_Type( ).Source_Field_Type_Match( ) && get_obj( ).Field( ).Primitive( ) != source_primitive )
         p_validation_errors->insert( construct_validation_error( vf.num, "",
          get_string_message( GS( c_str_field_types_must_match ), make_pair(
          c_str_parm_field_types_must_match_field1, get_module_string( c_field_display_name_Field ) ),
          make_pair( c_str_parm_field_types_must_match_field2, get_module_string( c_field_display_name_Source_Field ) ) ) ) );

      if( get_obj( ).Specification_Type( ).Source_Field_Needs_Test_Field( )
       && get_obj( ).Test_Field( ).get_key( ).empty( ) && !get_obj( ).Source_Field( ).get_key( ).empty( ) )
         p_validation_errors->insert( construct_validation_error( vf.num, "",
          get_string_message( GS( c_str_field_must_be_empty_match ), make_pair(
          c_str_parm_field_must_be_empty_match_field2, get_module_string( c_field_display_name_Source_Field ) ),
          make_pair( c_str_parm_field_must_be_empty_match_field1, get_module_string( c_field_display_name_Test_Field ) ) ) ) );

      if( get_obj( ).Specification_Type( ).Child_Relationship_Class_Match( )
       && get_obj( ).Class( ).get_key( ) != get_obj( ).Child_Relationship( ).Child_Class( ).get_key( ) )
         p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Child_Relationship,
          get_module_string( c_field_display_name_Child_Relationship ) + " must be a self-relationship." ) ); // FUTURE: Should be in module_strings...

      if( get_obj( ).Specification_Type( ).Child_Rel_Parent_Match( )
       && get_obj( ).Parent_Specification( ).Class( ).get_key( ) != get_obj( ).Child_Relationship( ).Child_Class( ).get_key( ) )
         p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Child_Relationship,
          get_module_string( c_field_display_name_Child_Relationship ) + " must match the parent specification." ) ); // FUTURE: Should be in module_strings...

      if( get_obj( ).Specification_Type( ).Child_Rel_Grandparent_Match( )
       && get_obj( ).Parent_Specification( ).Parent_Specification( ).Class( ).get_key( ) != get_obj( ).Child_Relationship( ).Child_Class( ).get_key( ) )
         p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Child_Relationship,
          get_module_string( c_field_display_name_Child_Relationship ) + " must match the grandparent specification." ) ); // FUTURE: Should be in module_strings...

      if( get_obj( ).Specification_Type( ).get_key( ) == "user_info"
       && ( get_obj( ).Test_Field( ).Extra( ) != 10 // i.e. password
       && get_obj( ).Test_Field( ).Extra( ) != 26 ) ) // i.e. hpassword
         p_validation_errors->insert( construct_validation_error( vf.num, "", "Test Field is not a valid 'password' field." ) ); // FUTURE: Should be in module_strings...
   }
   // [(finish meta_spec_field_type)] 600147

   // [(start check_null_match)] 600177
   if( is_null( get_obj( ).Test_Child( ) ) && !is_null( get_obj( ).Test_Parent( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_Test_Child, get_string_message( GS( c_str_field_must_be_empty_match ), make_pair(
       c_str_parm_field_must_be_empty_match_field2, get_module_string( c_field_display_name_Test_Parent ) ),
       make_pair( c_str_parm_field_must_be_empty_match_field1, get_module_string( c_field_display_name_Test_Child ) ) ) ) );
   // [(finish check_null_match)] 600177

   // [(start check_cond_non_null)] 610131
   if( get_obj( ).Specification_Type( ).Needs_Other_Field( ) && is_null( get_obj( ).Other_Field( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_Other_Field, get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Other_Field ) ) ) ) );
   // [(finish check_cond_non_null)] 610131

   // [(start check_cond_non_null)] 610143
   if( get_obj( ).Specification_Type( ).Needs_Other_Source_Child( ) && is_null( get_obj( ).Other_Source_Child( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_Other_Source_Child, get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Other_Source_Child ) ) ) ) );
   // [(finish check_cond_non_null)] 610143

   // [(start check_cond_non_null)] 610146
   if( get_obj( ).Specification_Type( ).Needs_Permission( ) && is_null( get_obj( ).Permission( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_Permission, get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Permission ) ) ) ) );
   // [(finish check_cond_non_null)] 610146

   // [(start check_cond_non_null)] 620131
   if( get_obj( ).Specification_Type( ).Needs_Other_Field_2( ) && is_null( get_obj( ).Other_Field_2( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_Other_Field_2, get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Other_Field_2 ) ) ) ) );
   // [(finish check_cond_non_null)] 620131

   // [(start check_cond_non_null)] 620143
   if( get_obj( ).Specification_Type( ).Needs_Other_Source_Child_2( ) && is_null( get_obj( ).Other_Source_Child_2( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_Other_Source_Child_2, get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Other_Source_Child_2 ) ) ) ) );
   // [(finish check_cond_non_null)] 620143

   // [(start check_cond_non_null)] 630131
   if( get_obj( ).Specification_Type( ).Needs_Other_Class_Field( ) && is_null( get_obj( ).Other_Class_Field( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_Other_Class_Field, get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Other_Class_Field ) ) ) ) );
   // [(finish check_cond_non_null)] 630131

   // [<start validate>]
   // [<finish validate>]
}

void Meta_Specification::impl::validate_set_fields(
 set< string >& fields_set, validation_error_container* p_validation_errors )
{
   ( void )fields_set;

   if( !p_validation_errors )
      throw runtime_error( "unexpected null validation_errors container" );

   string error_message;
   validate_formatter vf;

   if( !is_null( v_Comments )
    && ( fields_set.count( c_field_id_Comments ) || fields_set.count( c_field_name_Comments ) )
    && !g_Comments_domain.is_valid( v_Comments, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Comments,
       get_module_string( c_field_display_name_Comments ) + " " + error_message ) );

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

   if( !is_null( v_Options )
    && ( fields_set.count( c_field_id_Options ) || fields_set.count( c_field_name_Options ) )
    && !g_Options_domain.is_valid( v_Options, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Options,
       get_module_string( c_field_display_name_Options ) + " " + error_message ) );

   if( !is_null( v_Test_Value )
    && ( fields_set.count( c_field_id_Test_Value ) || fields_set.count( c_field_name_Test_Value ) )
    && !g_Test_Value_domain.is_valid( v_Test_Value, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Test_Value,
       get_module_string( c_field_display_name_Test_Value ) + " " + error_message ) );

   if( !is_null( v_Value )
    && ( fields_set.count( c_field_id_Value ) || fields_set.count( c_field_name_Value ) )
    && !g_Value_domain.is_valid( v_Value, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Value,
       get_module_string( c_field_display_name_Value ) + " " + error_message ) );
}

void Meta_Specification::impl::after_fetch( )
{
   if( !get_obj( ).get_is_iterating( ) || get_obj( ).get_is_starting_iteration( ) )
      get_required_transients( );

   if( cp_Child_Rel_Child_Class )
      p_obj->setup_foreign_key( *cp_Child_Rel_Child_Class, v_Child_Rel_Child_Class );

   if( cp_Child_Relationship )
      p_obj->setup_foreign_key( *cp_Child_Relationship, v_Child_Relationship );

   if( cp_Child_Specification_Type )
      p_obj->setup_foreign_key( *cp_Child_Specification_Type, v_Child_Specification_Type );

   if( cp_Class )
      p_obj->setup_foreign_key( *cp_Class, v_Class );

   if( cp_Enum )
      p_obj->setup_foreign_key( *cp_Enum, v_Enum );

   if( cp_Enum_Item )
      p_obj->setup_foreign_key( *cp_Enum_Item, v_Enum_Item );

   if( cp_Enum_Item_2 )
      p_obj->setup_foreign_key( *cp_Enum_Item_2, v_Enum_Item_2 );

   if( cp_Enum_Item_3 )
      p_obj->setup_foreign_key( *cp_Enum_Item_3, v_Enum_Item_3 );

   if( cp_Enum_Item_4 )
      p_obj->setup_foreign_key( *cp_Enum_Item_4, v_Enum_Item_4 );

   if( cp_Enum_Item_5 )
      p_obj->setup_foreign_key( *cp_Enum_Item_5, v_Enum_Item_5 );

   if( cp_Field )
      p_obj->setup_foreign_key( *cp_Field, v_Field );

   if( cp_Field_Class )
      p_obj->setup_foreign_key( *cp_Field_Class, v_Field_Class );

   if( cp_Model )
      p_obj->setup_foreign_key( *cp_Model, v_Model );

   if( cp_Modifier )
      p_obj->setup_foreign_key( *cp_Modifier, v_Modifier );

   if( cp_Other_Class )
      p_obj->setup_foreign_key( *cp_Other_Class, v_Other_Class );

   if( cp_Other_Class_Field )
      p_obj->setup_foreign_key( *cp_Other_Class_Field, v_Other_Class_Field );

   if( cp_Other_Field )
      p_obj->setup_foreign_key( *cp_Other_Field, v_Other_Field );

   if( cp_Other_Field_2 )
      p_obj->setup_foreign_key( *cp_Other_Field_2, v_Other_Field_2 );

   if( cp_Other_Modifier )
      p_obj->setup_foreign_key( *cp_Other_Modifier, v_Other_Modifier );

   if( cp_Other_Modifier_2 )
      p_obj->setup_foreign_key( *cp_Other_Modifier_2, v_Other_Modifier_2 );

   if( cp_Other_Permission )
      p_obj->setup_foreign_key( *cp_Other_Permission, v_Other_Permission );

   if( cp_Other_Permission_2 )
      p_obj->setup_foreign_key( *cp_Other_Permission_2, v_Other_Permission_2 );

   if( cp_Other_Procedure )
      p_obj->setup_foreign_key( *cp_Other_Procedure, v_Other_Procedure );

   if( cp_Other_Procedure_2 )
      p_obj->setup_foreign_key( *cp_Other_Procedure_2, v_Other_Procedure_2 );

   if( cp_Other_Source_Child )
      p_obj->setup_foreign_key( *cp_Other_Source_Child, v_Other_Source_Child );

   if( cp_Other_Source_Child_2 )
      p_obj->setup_foreign_key( *cp_Other_Source_Child_2, v_Other_Source_Child_2 );

   if( cp_Parent_Specification )
      p_obj->setup_foreign_key( *cp_Parent_Specification, v_Parent_Specification );

   if( cp_Permission )
      p_obj->setup_foreign_key( *cp_Permission, v_Permission );

   if( cp_Procedure )
      p_obj->setup_foreign_key( *cp_Procedure, v_Procedure );

   if( cp_Procedure_Arg )
      p_obj->setup_foreign_key( *cp_Procedure_Arg, v_Procedure_Arg );

   if( cp_Procedure_Arg_2 )
      p_obj->setup_foreign_key( *cp_Procedure_Arg_2, v_Procedure_Arg_2 );

   if( cp_Procedure_Arg_3 )
      p_obj->setup_foreign_key( *cp_Procedure_Arg_3, v_Procedure_Arg_3 );

   if( cp_Source_Child )
      p_obj->setup_foreign_key( *cp_Source_Child, v_Source_Child );

   if( cp_Source_Child_Class )
      p_obj->setup_foreign_key( *cp_Source_Child_Class, v_Source_Child_Class );

   if( cp_Source_Class )
      p_obj->setup_foreign_key( *cp_Source_Class, v_Source_Class );

   if( cp_Source_Field )
      p_obj->setup_foreign_key( *cp_Source_Field, v_Source_Field );

   if( cp_Source_Field_Class )
      p_obj->setup_foreign_key( *cp_Source_Field_Class, v_Source_Field_Class );

   if( cp_Source_Grandchild )
      p_obj->setup_foreign_key( *cp_Source_Grandchild, v_Source_Grandchild );

   if( cp_Source_Parent )
      p_obj->setup_foreign_key( *cp_Source_Parent, v_Source_Parent );

   if( cp_Source_Parent_Class )
      p_obj->setup_foreign_key( *cp_Source_Parent_Class, v_Source_Parent_Class );

   if( cp_Specification_Type )
      p_obj->setup_foreign_key( *cp_Specification_Type, v_Specification_Type );

   if( cp_Test_Child )
      p_obj->setup_foreign_key( *cp_Test_Child, v_Test_Child );

   if( cp_Test_Field )
      p_obj->setup_foreign_key( *cp_Test_Field, v_Test_Field );

   if( cp_Test_Field_Class )
      p_obj->setup_foreign_key( *cp_Test_Field_Class, v_Test_Field_Class );

   if( cp_Test_Parent )
      p_obj->setup_foreign_key( *cp_Test_Parent, v_Test_Parent );

   if( cp_Test_Parent_Class )
      p_obj->setup_foreign_key( *cp_Test_Parent_Class, v_Test_Parent_Class );

   post_init( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start field_from_search_replace)] 600207
   if( !get_obj( ).get_key( ).empty( )
    && !get_obj( ).get_is_for_peer( )
    && ( get_obj( ).needs_field_value( "Vars" )
    || required_transients.count( "Vars" ) ) )
   {
      string str( get_obj( ).Specification_Type( ).Specification_Vars( ) );

      get_obj( ).Vars( str );

      get_obj( ).add_search_replacement( "Vars", "{id}", to_rep_string( get_obj( ).Id( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{model}", to_rep_string( get_obj( ).Model( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{model_id}", to_rep_string( get_obj( ).Model( ).Id( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{class}", to_rep_string( get_obj( ).Class( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{class_id}", to_rep_string( get_obj( ).Class( ).Id( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{oclass}", to_rep_string( get_obj( ).Other_Class( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{oclass_id}", to_rep_string( get_obj( ).Other_Class( ).Id( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{field}", to_rep_string( get_obj( ).Field( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{fclass}", to_rep_string( get_obj( ).Field_Class( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{field_id}", to_rep_string( get_obj( ).Field( ).Id( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{fmandatory}", to_rep_string( get_obj( ).Field( ).Mandatory( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{ftransient}", to_rep_string( get_obj( ).Field( ).Transient( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{fdecimals}", to_rep_string( get_obj( ).Field( ).Numeric_Decimals( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{fisfk}", to_rep_string( get_obj( ).Field( ).Is_Foreign_Key( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{fistexttype}", to_rep_string( get_obj( ).Field( ).Is_Text_Type( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{fpclass}", to_rep_string( get_obj( ).Field( ).Parent_Class_Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{ofield}", to_rep_string( get_obj( ).Other_Field( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{ofield_id}", to_rep_string( get_obj( ).Other_Field( ).Id( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{ofisfk}", to_rep_string( get_obj( ).Other_Field( ).Is_Foreign_Key( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{o2field}", to_rep_string( get_obj( ).Other_Field_2( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{o2field_id}", to_rep_string( get_obj( ).Other_Field_2( ).Id( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{ocfield}", to_rep_string( get_obj( ).Other_Class_Field( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{ocfield_id}", to_rep_string( get_obj( ).Other_Class_Field( ).Id( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{modifier}", to_rep_string( get_obj( ).Modifier( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{omodifier}", to_rep_string( get_obj( ).Other_Modifier( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{o2modifier}", to_rep_string( get_obj( ).Other_Modifier_2( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{spclass}", to_rep_string( get_obj( ).Source_Parent_Class( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{spfield}", to_rep_string( get_obj( ).Source_Parent( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{spfield_id}", to_rep_string( get_obj( ).Source_Parent( ).Id( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{scclass}", to_rep_string( get_obj( ).Source_Child_Class( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{scfield}", to_rep_string( get_obj( ).Source_Child( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{scfield_id}", to_rep_string( get_obj( ).Source_Child( ).Id( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{oscfield}", to_rep_string( get_obj( ).Other_Source_Child( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{oscfield_id}", to_rep_string( get_obj( ).Other_Source_Child( ).Id( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{o2scfield}", to_rep_string( get_obj( ).Other_Source_Child_2( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{o2scfield_id}", to_rep_string( get_obj( ).Other_Source_Child_2( ).Id( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{sfield}", to_rep_string( get_obj( ).Source_Field( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{sfield_id}", to_rep_string( get_obj( ).Source_Field( ).Id( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{sgcfield}", to_rep_string( get_obj( ).Source_Grandchild( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{sfisfk}", to_rep_string( get_obj( ).Source_Field( ).Is_Foreign_Key( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{sfistexttype}", to_rep_string( get_obj( ).Source_Field( ).Is_Text_Type( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{scfistexttype}", to_rep_string( get_obj( ).Source_Child( ).Is_Text_Type( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{tpfield}", to_rep_string( get_obj( ).Test_Parent( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{tcfield}", to_rep_string( get_obj( ).Test_Child( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{tcfistexttype}", to_rep_string( get_obj( ).Test_Child( ).Is_Text_Type( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{tfield}", to_rep_string( get_obj( ).Test_Field( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{tfield_id}", to_rep_string( get_obj( ).Test_Field( ).Id( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{tfistexttype}", to_rep_string( get_obj( ).Test_Field( ).Is_Text_Type( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{tfclass}", to_rep_string( get_obj( ).Test_Field_Class( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{tvalue}", to_rep_string( get_obj( ).Test_Value( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{permission}", to_rep_string( get_obj( ).Permission( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{permission_id}", to_rep_string( get_obj( ).Permission( ).Id( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{opermission}", to_rep_string( get_obj( ).Other_Permission( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{opermission_id}", to_rep_string( get_obj( ).Other_Permission( ).Id( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{o2permission}", to_rep_string( get_obj( ).Other_Permission_2( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{o2permission_id}", to_rep_string( get_obj( ).Other_Permission_2( ).Id( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{procedure}", to_rep_string( get_obj( ).Procedure( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{procedure_id}", to_rep_string( get_obj( ).Procedure( ).Id( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{procedure_arg}", to_rep_string( get_obj( ).Procedure_Arg( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{procedure_arg2}", to_rep_string( get_obj( ).Procedure_Arg_2( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{procedure_arg3}", to_rep_string( get_obj( ).Procedure_Arg_3( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{oprocedure}", to_rep_string( get_obj( ).Other_Procedure( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{oprocedure_id}", to_rep_string( get_obj( ).Other_Procedure( ).Id( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{o2procedure}", to_rep_string( get_obj( ).Other_Procedure_2( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{o2procedure_id}", to_rep_string( get_obj( ).Other_Procedure_2( ).Id( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{cname}", to_rep_string( get_obj( ).Child_Relationship( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{child}", to_rep_string( get_obj( ).Child_Relationship( ).Child_Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{cclass}", to_rep_string( get_obj( ).Child_Relationship( ).Child_Class_Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{cclass_id}", to_rep_string( get_obj( ).Child_Relationship( ).Child_Class_Id( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{primitive}", to_rep_string( get_obj( ).Field( ).Primitive( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{enum}", to_rep_string( get_obj( ).Enum( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{enum_item}", to_rep_string( get_obj( ).Enum_Item( ).Label( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{enum_value}", to_rep_string( get_obj( ).Enum_Item( ).Value( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{enum2_item}", to_rep_string( get_obj( ).Enum_Item_2( ).Label( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{enum2_value}", to_rep_string( get_obj( ).Enum_Item_2( ).Value( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{enum3_item}", to_rep_string( get_obj( ).Enum_Item_3( ).Label( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{enum3_value}", to_rep_string( get_obj( ).Enum_Item_3( ).Value( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{enum4_item}", to_rep_string( get_obj( ).Enum_Item_4( ).Label( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{enum4_value}", to_rep_string( get_obj( ).Enum_Item_4( ).Value( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{enum5_item}", to_rep_string( get_obj( ).Enum_Item_5( ).Label( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{enum5_value}", to_rep_string( get_obj( ).Enum_Item_5( ).Value( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{value}", to_rep_string( get_obj( ).Value( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{value_literal}", to_rep_string( get_obj( ).Value_Literal( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{value_string}", to_rep_string( get_obj( ).Value_String( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{value_leftpart}", to_rep_string( get_obj( ).Value_Left_Part( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{value_rightpart}", to_rep_string( get_obj( ).Value_Right_Part( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{field_values}", to_rep_string( get_obj( ).Field_Values( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{fields}", to_rep_string( get_obj( ).Fields( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{field_pairs}", to_rep_string( get_obj( ).Field_Pairs( ) ) );

      vector< string > options_set;
      split_string( get_obj( ).Options( ), options_set, ' ' );

      for( size_t i = 0; i < options_set.size( ); i++ )
      {
         string next_search;

         string next_opt( options_set[ i ] );
         string::size_type pos = next_opt.find( '=' );

         string next_val;
         if( pos != string::npos )
            next_val = next_opt.substr( pos + 1 );

         next_search = "{?" + next_opt + "}";
         get_obj( ).add_search_replacement( "Vars", next_search, "true" );

         if( pos != string::npos )
         {
            next_search = "{#" + next_opt.substr( 0, pos ) + "}";
            get_obj( ).add_search_replacement( "Vars", next_search, next_val );
         }
      }
   }
   // [(finish field_from_search_replace)] 600207

   // [(start field_from_search_replace)] 600208
   if( !get_obj( ).get_key( ).empty( )
    && !get_obj( ).get_is_for_peer( )
    && ( get_obj( ).needs_field_value( "Strings" )
    || required_transients.count( "Strings" ) ) )
   {
      string str( get_obj( ).Specification_Type( ).Specification_Strings( ) );

      get_obj( ).Strings( str );

      get_obj( ).add_search_replacement( "Strings", "{model}", to_rep_string( get_obj( ).Model( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Strings", "{class}", to_rep_string( get_obj( ).Class( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Strings", "{field}", to_rep_string( get_obj( ).Field( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Strings", "{fclass}", to_rep_string( get_obj( ).Field_Class( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Strings", "{modifier}", to_rep_string( get_obj( ).Modifier( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Strings", "{spfield}", to_rep_string( get_obj( ).Source_Parent( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Strings", "{scfield}", to_rep_string( get_obj( ).Source_Child( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Strings", "{sfield}", to_rep_string( get_obj( ).Source_Field( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Strings", "{tpfield}", to_rep_string( get_obj( ).Test_Parent( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Strings", "{tcfield}", to_rep_string( get_obj( ).Test_Child( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Strings", "{tfield}", to_rep_string( get_obj( ).Test_Field( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Strings", "{tfclass}", to_rep_string( get_obj( ).Test_Field_Class( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Strings", "{tvalue}", to_rep_string( get_obj( ).Test_Value( ) ) );
      get_obj( ).add_search_replacement( "Strings", "{procedure}", to_rep_string( get_obj( ).Procedure( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Strings", "{child}", to_rep_string( get_obj( ).Child_Relationship( ).Child_Name( ) ) );
      get_obj( ).add_search_replacement( "Strings", "{enum}", to_rep_string( get_obj( ).Enum( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Strings", "{enum_item}", to_rep_string( get_obj( ).Enum_Item( ).Label( ) ) );
      get_obj( ).add_search_replacement( "Strings", "{enum_value}", to_rep_string( get_obj( ).Enum_Item( ).Value( ) ) );
      get_obj( ).add_search_replacement( "Strings", "{value}", to_rep_string( get_obj( ).Value( ) ) );
      get_obj( ).add_search_replacement( "Strings", "{value_leftpart}", to_rep_string( get_obj( ).Value_Left_Part( ) ) );
      get_obj( ).add_search_replacement( "Strings", "{value_rightpart}", to_rep_string( get_obj( ).Value_Right_Part( ) ) );
   }
   // [(finish field_from_search_replace)] 600208

   // [<start after_fetch>]
//nyi
   // NOTE: Restriction specification names can be problematic because one such
   // name can end up being identical to that of another simply due to the step
   // by step nature of adding fields to the field list (this can especially be
   // problemic if shorter such names preceed longer ones). In order to prevent
   // this from being an issue the "id" is appended to the name itself but here
   // this is removed to make it look neater for display purposes.
   if( get_obj( ).Specification_Type( ).get_key( ) == "restriction" )
   {
      string name( get_obj( ).Name( ) );
      string id_suffix( "_" + get_obj( ).Id( ) );

      if( name.length( ) > id_suffix.length( )
       && name.substr( name.length( ) - id_suffix.length( ) ) == id_suffix )
      {
         name.erase( name.length( ) - id_suffix.length( ) );
         get_obj( ).Name( name );
      }
   }
   // [<finish after_fetch>]
}

void Meta_Specification::impl::finalise_fetch( bool skip_set_original )
{
   if( !skip_set_original && !get_obj( ).get_key( ).empty( ) )
      get_obj( ).set_new_original_values( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start field_from_procedure_call)] 600209
   if( !get_obj( ).get_key( ).empty( )
    && ( get_obj( ).needs_field_value( "All_Vars" )
    || required_transients.count( "All_Vars" ) ) )
   {
      string str;
      get_obj( ).Generate_All_Vars( str, "" );
      get_obj( ).All_Vars( str );
   }
   // [(finish field_from_procedure_call)] 600209

   // [(start field_from_procedure_call)] 600210
   if( !get_obj( ).get_key( ).empty( )
    && ( get_obj( ).needs_field_value( "All_Strings" )
    || required_transients.count( "All_Strings" ) ) )
   {
      string str;
      get_obj( ).Generate_All_Strings( str, "" );
      get_obj( ).All_Strings( str );
   }
   // [(finish field_from_procedure_call)] 600210

   // [<start finalise_fetch>]
   // [<finish finalise_fetch>]
}

void Meta_Specification::impl::at_create( )
{
   // [<start at_create>]
   // [<finish at_create>]
}

void Meta_Specification::impl::post_init( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start post_init>]
   // [<finish post_init>]
}

void Meta_Specification::impl::to_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   if( !get_obj( ).get_is_preparing( ) )
      post_init( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start meta_spec_field_type)] 600147
   if( is_create && get_obj( ).Specification_Type( ).get_key( ) == "user_info"
    && !get_obj( ).Class( ).get_key( ).empty( ) && is_null( get_obj( ).Source_Parent( ) ) )
   {
      if( !get_obj( ).Class( ).child_Field( ).get_is_iterating( )
       && get_obj( ).Class( ).child_Field( ).iterate_forwards( ) )
      {
         do
         {
            if( get_obj( ).Class( ).child_Field( ).Extra( ) == -2 ) // i.e. user_group
            {
               get_obj( ).Source_Parent( get_obj( ).Class( ).child_Field( ) );
               get_obj( ).Class( ).child_Field( ).iterate_stop( );
               break;
            }
         } while( get_obj( ).Class( ).child_Field( ).iterate_next( ) );
      }
   }
   // [(finish meta_spec_field_type)] 600147

   // [(start default_to_field)] 600148
   if( ( is_create || get_obj( ).get_key( ).empty( ) )
    && get_obj( ).get_clone_key( ).empty( )
    && ( get_obj( ).Model( ) == g_default_Model )
    && !to_string( get_obj( ).Parent_Specification( ) ).empty( ) )
      get_obj( ).Model( get_obj( ).Parent_Specification( ).Model( ) );
   // [(finish default_to_field)] 600148

   // [(start default_to_field)] 600149
   if( ( is_create || get_obj( ).get_key( ).empty( ) )
    && get_obj( ).get_clone_key( ).empty( )
    && ( get_obj( ).Specification_Type( ) == g_default_Specification_Type )
    && !to_string( get_obj( ).Parent_Specification( ) ).empty( ) )
      get_obj( ).Specification_Type( get_obj( ).Parent_Specification( ).Child_Specification_Type( ) );
   // [(finish default_to_field)] 600149

   // [(start default_to_field)] 600150
   if( ( is_create || get_obj( ).get_key( ).empty( ) )
    && get_obj( ).get_clone_key( ).empty( )
    && ( get_obj( ).Class( ) == g_default_Class )
    && get_obj( ).Specification_Type( ).Use_Parent_Class( ) == true )
      get_obj( ).Class( get_obj( ).Parent_Specification( ).Class( ) );
   // [(finish default_to_field)] 600150

   // [(start default_to_field)] 600150a
   if( ( is_create || get_obj( ).get_key( ).empty( ) )
    && get_obj( ).get_clone_key( ).empty( )
    && ( get_obj( ).Other_Class( ) == g_default_Other_Class )
    && get_obj( ).Specification_Type( ).Use_Parent_Other_Class( ) == true )
      get_obj( ).Other_Class( get_obj( ).Parent_Specification( ).Other_Class( ) );
   // [(finish default_to_field)] 600150a

   // [(start default_to_field)] 600151
   if( ( is_create || get_obj( ).get_key( ).empty( ) )
    && get_obj( ).get_clone_key( ).empty( )
    && ( get_obj( ).Class( ) == g_default_Class )
    && get_obj( ).Specification_Type( ).Use_Parent_Source_Parent_For_Class( ) == true )
      get_obj( ).Class( get_obj( ).Parent_Specification( ).Source_Parent_Class( ) );
   // [(finish default_to_field)] 600151

   // [(start default_to_field)] 600152
   if( ( is_create || get_obj( ).get_key( ).empty( ) )
    && get_obj( ).get_clone_key( ).empty( )
    && ( get_obj( ).Class( ) == g_default_Class )
    && get_obj( ).Specification_Type( ).Use_Parent_Child_Rel_For_Class( ) == true )
      get_obj( ).Class( get_obj( ).Parent_Specification( ).Child_Rel_Child_Class( ) );
   // [(finish default_to_field)] 600152

   // [(start default_to_field)] 600152a
   if( ( is_create || get_obj( ).get_key( ).empty( ) )
    && get_obj( ).get_clone_key( ).empty( )
    && ( get_obj( ).Other_Class( ) == g_default_Other_Class )
    && get_obj( ).Specification_Type( ).Use_Parent_Child_Rel_For_Other_Class( ) == true )
      get_obj( ).Other_Class( get_obj( ).Parent_Specification( ).Child_Rel_Child_Class( ) );
   // [(finish default_to_field)] 600152a

   // [(start default_to_field)] 600153
   if( ( is_create || get_obj( ).get_key( ).empty( ) )
    && get_obj( ).get_clone_key( ).empty( )
    && ( get_obj( ).Class( ) == g_default_Class )
    && get_obj( ).Specification_Type( ).Use_Parent_Field_For_Class( ) == true )
      get_obj( ).Class( get_obj( ).Parent_Specification( ).Field_Class( ) );
   // [(finish default_to_field)] 600153

   // [(start default_to_field)] 600153a
   if( ( is_create || get_obj( ).get_key( ).empty( ) )
    && get_obj( ).get_clone_key( ).empty( )
    && ( get_obj( ).Class( ) == g_default_Class )
    && get_obj( ).Specification_Type( ).Use_Parent_Source_Field_For_Class( ) == true )
      get_obj( ).Class( get_obj( ).Parent_Specification( ).Source_Field_Class( ) );
   // [(finish default_to_field)] 600153a

   // [(start default_to_field)] 600153b
   if( ( is_create || get_obj( ).get_key( ).empty( ) )
    && get_obj( ).get_clone_key( ).empty( )
    && ( get_obj( ).Class( ) == g_default_Class )
    && get_obj( ).Specification_Type( ).Use_Parent_Other_Class_For_Class( ) == true )
      get_obj( ).Class( get_obj( ).Parent_Specification( ).Other_Class( ) );
   // [(finish default_to_field)] 600153b

   // [(start default_to_field)] 600154
   if( ( is_create || get_obj( ).get_key( ).empty( ) )
    && get_obj( ).get_clone_key( ).empty( )
    && ( get_obj( ).Source_Parent( ) == g_default_Source_Parent )
    && get_obj( ).Specification_Type( ).Use_Parent_Source_Parent( ) == true )
      get_obj( ).Source_Parent( get_obj( ).Parent_Specification( ).Source_Parent( ) );
   // [(finish default_to_field)] 600154

   // [(start default_to_field)] 600155
   if( ( is_create || get_obj( ).get_key( ).empty( ) )
    && get_obj( ).get_clone_key( ).empty( )
    && ( get_obj( ).Source_Parent( ) == g_default_Source_Parent )
    && get_obj( ).Specification_Type( ).Use_Parent_Child_Rel_As_Source_Parent( ) == true )
      get_obj( ).Source_Parent( get_obj( ).Parent_Specification( ).Child_Rel_Field_Key( ) );
   // [(finish default_to_field)] 600155

   // [(start default_to_field)] 600156
   if( ( is_create || get_obj( ).get_key( ).empty( ) )
    && get_obj( ).get_clone_key( ).empty( )
    && ( get_obj( ).Source_Class( ) == g_default_Source_Class )
    && get_obj( ).Specification_Type( ).Use_Parent_Source_Class( ) == true )
      get_obj( ).Source_Class( get_obj( ).Parent_Specification( ).Source_Class( ) );
   // [(finish default_to_field)] 600156

   // [(start default_to_field)] 600157
   if( ( is_create || get_obj( ).get_key( ).empty( ) )
    && get_obj( ).get_clone_key( ).empty( )
    && ( get_obj( ).Actions( ) == g_default_Actions ) )
      get_obj( ).Actions( get_obj( ).Specification_Type( ).Specification_Actions( ) );
   // [(finish default_to_field)] 600157

   // [(start default_to_field)] 600158
   if( ( is_create || get_obj( ).get_key( ).empty( ) )
    && get_obj( ).get_clone_key( ).empty( )
    && ( get_obj( ).Child_Specification_Type( ) == g_default_Child_Specification_Type ) )
      get_obj( ).Child_Specification_Type( get_obj( ).Specification_Type( ).Child_Specification_Type( ) );
   // [(finish default_to_field)] 600158

   // [(start default_to_field)] 600159
   if( ( is_create || get_obj( ).get_key( ).empty( ) )
    && get_obj( ).get_clone_key( ).empty( )
    && ( get_obj( ).Model( ) == g_default_Model ) )
      get_obj( ).Model( get_obj( ).Class( ).Model( ) );
   // [(finish default_to_field)] 600159

   // [(start default_to_field)] 600160
   if( ( is_create || get_obj( ).get_key( ).empty( ) )
    && get_obj( ).get_clone_key( ).empty( )
    && ( get_obj( ).Source_Class( ) == g_default_Source_Class ) )
      get_obj( ).Source_Class( get_obj( ).Class( ) );
   // [(finish default_to_field)] 600160

   // [(start field_clear_on_changed)] 600161
   if( !is_create && get_obj( ).Class( ).has_changed( ) )
      get_obj( ).Source_Parent( string( ) );
   // [(finish field_clear_on_changed)] 600161

   // [(start field_clear)] 600162
   if( get_obj( ).Specification_Type( ).Allow_FK_Source_Field_Choice( ) && get_obj( ).Use_Source_Parent( ) )
      get_obj( ).Source_Field( string( ) );
   // [(finish field_clear)] 600162

   // [(start field_clear)] 600163
   if( get_obj( ).Specification_Type( ).Allow_FK_Source_Field_Choice( ) && !get_obj( ).Use_Source_Parent( ) )
      get_obj( ).Source_Parent( string( ) );
   // [(finish field_clear)] 600163

   // [(start field_clear)] 600164
   if( get_obj( ).Specification_Type( ).Allow_FK_Source_Field_Choice( ) && !get_obj( ).Use_Source_Parent( ) )
      get_obj( ).Source_Child( string( ) );
   // [(finish field_clear)] 600164

   // [(start field_clear_on_changed)] 600165
   if( !is_create && get_obj( ).Class( ).has_changed( ) )
      get_obj( ).Child_Relationship( string( ) );
   // [(finish field_clear_on_changed)] 600165

   // [(start field_from_other_field)] 600166
   get_obj( ).Source_Parent_Class( get_obj( ).Source_Parent( ).Parent_Class( ) );
   // [(finish field_from_other_field)] 600166

   // [(start default_to_field)] 600166a
   if( ( is_create || get_obj( ).get_key( ).empty( ) )
    && get_obj( ).get_clone_key( ).empty( )
    && ( get_obj( ).Source_Parent_Class( ) == g_default_Source_Parent_Class )
    && get_obj( ).Specification_Type( ).Use_Class_As_Source_Parent_Class( ) == true )
      get_obj( ).Source_Parent_Class( get_obj( ).Class( ) );
   // [(finish default_to_field)] 600166a

   // [(start default_to_field)] 600167
   if( ( is_create || get_obj( ).get_key( ).empty( ) )
    && get_obj( ).get_clone_key( ).empty( )
    && ( get_obj( ).Name( ) == g_default_Name ) )
      get_obj( ).Name( get_obj( ).Specification_Type( ).Specification_Name( ) );
   // [(finish default_to_field)] 600167

   // [(start default_from_key)] 600168
   if( !get_obj( ).get_clone_key( ).empty( ) || ( is_create && ( get_obj( ).Order( ) == g_default_Order ) ) )
      get_obj( ).Order( get_obj( ).get_key( ) );
   // [(finish default_from_key)] 600168

   // [(start field_clear_on_changed)] 600171
   if( get_obj( ).Specification_Type( ).Use_Field_Enum( ) && get_obj( ).Class( ).has_changed( ) )
      get_obj( ).Enum( string( ) );
   // [(finish field_clear_on_changed)] 600171

   // [(start field_clear_on_changed)] 600171a
   if( get_obj( ).Specification_Type( ).Use_Source_Field_Enum( ) && get_obj( ).Class( ).has_changed( ) )
      get_obj( ).Enum( string( ) );
   // [(finish field_clear_on_changed)] 600171a

   // [(start field_clear_on_changed)] 600171b
   if( get_obj( ).Specification_Type( ).Use_Source_Field_Or_Child_Enum( ) && get_obj( ).Class( ).has_changed( ) )
      get_obj( ).Enum( string( ) );
   // [(finish field_clear_on_changed)] 600171b

   // [(start field_clear_on_changed)] 600172
   if( !is_create && get_obj( ).Class( ).has_changed( ) )
      get_obj( ).Test_Parent( string( ) );
   // [(finish field_clear_on_changed)] 600172

   // [(start field_from_other_field)] 600173
   get_obj( ).Test_Parent_Class( get_obj( ).Test_Parent( ).Parent_Class( ) );
   // [(finish field_from_other_field)] 600173

   // [(start field_clear)] 600174
   if( get_obj( ).Specification_Type( ).Allow_FK_Test_Field_Choice( ) && get_obj( ).Use_Test_Parent_Child( ) )
      get_obj( ).Test_Field( string( ) );
   // [(finish field_clear)] 600174

   // [(start field_clear)] 600175
   if( get_obj( ).Specification_Type( ).Allow_FK_Test_Field_Choice( ) && !get_obj( ).Use_Test_Parent_Child( ) )
      get_obj( ).Test_Parent( string( ) );
   // [(finish field_clear)] 600175

   // [(start field_clear)] 600176
   if( get_obj( ).Specification_Type( ).Allow_FK_Test_Field_Choice( ) && !get_obj( ).Use_Test_Parent_Child( ) )
      get_obj( ).Test_Child( string( ) );
   // [(finish field_clear)] 600176

   // [(start field_from_other_field)] 600178
   get_obj( ).Child_Rel_Field_Key( get_obj( ).Child_Relationship( ).Field_Key( ) );
   // [(finish field_from_other_field)] 600178

   // [(start field_from_other_field)] 600179
   get_obj( ).Child_Rel_Child_Class( get_obj( ).Child_Relationship( ).Child_Class( ) );
   // [(finish field_from_other_field)] 600179

   // [(start default_to_field)] 600181
   if( ( is_create || get_obj( ).get_key( ).empty( ) )
    && get_obj( ).get_clone_key( ).empty( )
    && ( get_obj( ).Procedure( ) == g_default_Procedure )
    && get_obj( ).Specification_Type( ).Use_Parent_Procedure( ) == true )
      get_obj( ).Procedure( get_obj( ).Parent_Specification( ).Procedure( ) );
   // [(finish default_to_field)] 600181

   // [(start field_from_other_field)] 600182
   get_obj( ).Is_Required_For_UI_Gen( get_obj( ).Specification_Type( ).Is_Required_For_UI_Gen( ) );
   // [(finish field_from_other_field)] 600182

   // [(start field_from_other_field)] 600182a
   get_obj( ).Can_Access_Below_Model( get_obj( ).Specification_Type( ).Can_Access_Below_Model( ) );
   // [(finish field_from_other_field)] 600182a

   // [(start field_from_other_field)] 600183
   if( !is_null( get_obj( ).Source_Field( ) )
    && get_obj( ).Specification_Type( ).Set_Field_To_Source_Field_Or_Child( ) == true )
      get_obj( ).Field( get_obj( ).Source_Field( ) );
   // [(finish field_from_other_field)] 600183

   // [(start field_from_other_field)] 600184
   if( !is_null( get_obj( ).Source_Child( ) )
    && get_obj( ).Specification_Type( ).Set_Field_To_Source_Field_Or_Child( ) == true )
      get_obj( ).Field( get_obj( ).Source_Child( ) );
   // [(finish field_from_other_field)] 600184

   // [(start field_from_other_field)] 600185
   get_obj( ).Value_Literal( get_obj( ).Value( ) );
   // [(finish field_from_other_field)] 600185

   // [(start field_from_other_field)] 600186
   if( !is_null( get_obj( ).Value( ) )
    && get_obj( ).Field( ).Is_Text_Type( ) == true )
      get_obj( ).Value_Literal( quoted_literal( get_obj( ).Value( ) ) );
   // [(finish field_from_other_field)] 600186

   // [(start field_from_other_field)] 600187
   get_obj( ).Value_String( unquoted_literal( get_obj( ).Value( ) ) );
   // [(finish field_from_other_field)] 600187

   // [(start field_from_other_field)] 600188
   get_obj( ).Value_Numeric_String( numeric_name( get_obj( ).Value( ) ) );
   // [(finish field_from_other_field)] 600188

   // [(start field_from_other_field)] 600189
   get_obj( ).Value_Left_Part( value_leftpart( get_obj( ).Value( ) ) );
   // [(finish field_from_other_field)] 600189

   // [(start field_from_other_field)] 600190
   get_obj( ).Value_Right_Part( value_rightpart( get_obj( ).Value( ) ) );
   // [(finish field_from_other_field)] 600190

   // [(start field_from_other_field)] 600190a
   get_obj( ).Value_Label( value_label( get_obj( ).Value( ) ) );
   // [(finish field_from_other_field)] 600190a

   // [(start field_from_other_field)] 600190b
   get_obj( ).Test_Value_Label( value_label( get_obj( ).Test_Value( ) ) );
   // [(finish field_from_other_field)] 600190b

   // [(start field_from_other_field)] 600191
   if( get_obj( ).Specification_Type( ).Use_Field_Enum( ) == true )
      get_obj( ).Enum( get_obj( ).Field( ).Enum( ) );
   // [(finish field_from_other_field)] 600191

   // [(start field_from_other_field)] 600191a
   if( get_obj( ).Specification_Type( ).Use_Source_Field_Enum( ) == true )
      get_obj( ).Enum( get_obj( ).Source_Field( ).Enum( ) );
   // [(finish field_from_other_field)] 600191a

   // [(start field_from_other_field)] 600191b
   if( get_obj( ).Specification_Type( ).Use_Source_Field_Or_Child_Enum( ) == true )
      get_obj( ).Enum( get_obj( ).Source_Field( ).Enum( ) );
   // [(finish field_from_other_field)] 600191b

   // [(start field_from_other_field)] 600191c
   if( !is_null( get_obj( ).Source_Child( ) )
    && get_obj( ).Specification_Type( ).Use_Source_Field_Or_Child_Enum( ) == true )
      get_obj( ).Enum( get_obj( ).Source_Child( ).Enum( ) );
   // [(finish field_from_other_field)] 600191c

   // [(start field_from_other_field)] 600192
   get_obj( ).Field_Class( get_obj( ).Field( ).Parent_Class( ) );
   // [(finish field_from_other_field)] 600192

   // [(start field_from_other_field)] 600193
   get_obj( ).Test_Field_Class( get_obj( ).Test_Field( ).Parent_Class( ) );
   // [(finish field_from_other_field)] 600193

   // [(start field_from_other_field)] 600194
   get_obj( ).Source_Child_Class( get_obj( ).Source_Child( ).Parent_Class( ) );
   // [(finish field_from_other_field)] 600194

   // [(start meta_spec_field_values)] 600197
   // NOTE: Scope for local variables.
   {
      string str, rstr;
      if( !is_create
       && !get_obj( ).child_Specification_Parent( ).get_is_iterating( )
       && get_obj( ).child_Specification_Parent( ).iterate_forwards( ) )
      {
         do
         {
            if( get_obj( ).child_Specification_Parent( ).Specification_Type( ).get_key( ) == "restriction_field_list" )
            {
               string key_info( FIELD_ID( Meta, Specification, Order ) );
               key_info += ' ';

               if( !get_obj( ).child_Specification_Parent( ).child_Specification_Parent( ).get_is_iterating( )
                && get_obj( ).child_Specification_Parent( ).child_Specification_Parent( ).iterate_forwards( key_info ) )
               {
                  do
                  {
                     if( !rstr.empty( ) )
                        rstr += "+";

                     string value( get_obj( ).child_Specification_Parent( ).child_Specification_Parent( ).Value( ) );
                     if( !value.empty( ) && value[ 0 ] == '@' )
                     {
                        if( value != "@order" )
                           rstr += value + ":";
                        value.erase( );
                     }

                     rstr += get_obj( ).child_Specification_Parent( ).child_Specification_Parent( ).Field( ).Id( );

                     if( value == "<none>" )
                        rstr += "=";
                     else if( !value.empty( ) )
                        rstr += "=" + value;

                     if( !str.empty( ) )
                        str += "_and_";

                     str += get_obj( ).child_Specification_Parent( ).child_Specification_Parent( ).Name( );
                  } while( get_obj( ).child_Specification_Parent( ).child_Specification_Parent( ).iterate_next( ) );
               }

               get_obj( ).child_Specification_Parent( ).iterate_stop( );
               break;
            }
         } while( get_obj( ).child_Specification_Parent( ).iterate_next( ) );
      }

      if( get_obj( ).Specification_Type( ).get_key( ) == "restriction" )
      {
         if( !get_obj( ).Value( ).empty( ) )
         {
            if( !rstr.empty( ) )
               rstr += "+";
            rstr += get_obj( ).Value( );
         }
      }

      if( !is_create || get_obj( ).Specification_Type( ).get_key( ) == "restriction" )
      {
         get_obj( ).Field_Values( str );
         get_obj( ).Restrict_Values( rstr );
      }
   }
   // [(finish meta_spec_field_values)] 600197

   // [(start field_clear)] 600198
   get_obj( ).Fields( string( ) );
   // [(finish field_clear)] 600198

   // [(start field_clear)] 600199
   get_obj( ).Field_Pairs( string( ) );
   // [(finish field_clear)] 600199

   // [(start meta_spec_field_pairs)] 600200
   if( !is_create
    && !get_obj( ).child_Specification_Parent( ).get_is_iterating( )
    && get_obj( ).child_Specification_Parent( ).iterate_forwards( ) )
   {
      string str( get_obj( ).Fields( ) );
      do
      {
         if( get_obj( ).child_Specification_Parent( ).Name( ) == "field_list" )
         {
            string key_info( FIELD_ID( Meta, Specification, Order ) );
            key_info += ' ';

            if( !get_obj( ).child_Specification_Parent( ).child_Specification_Parent( ).get_is_iterating( )
             && get_obj( ).child_Specification_Parent( ).child_Specification_Parent( ).iterate_forwards( key_info ) )
            {
               do
               {
                  if( !str.empty( ) )
                     str += " ";

                  str += get_obj( ).child_Specification_Parent( ).child_Specification_Parent( ).Field( ).Name( );

               } while( get_obj( ).child_Specification_Parent( ).child_Specification_Parent( ).iterate_next( ) );
            }

            get_obj( ).child_Specification_Parent( ).iterate_stop( );
            break;
         }

      } while( get_obj( ).child_Specification_Parent( ).iterate_next( ) );

      get_obj( ).Fields( str );
   }
   // [(finish meta_spec_field_pairs)] 600200

   // [(start meta_spec_field_pairs)] 600200a
   if( !is_create
    && !get_obj( ).child_Specification_Parent( ).get_is_iterating( )
    && get_obj( ).child_Specification_Parent( ).iterate_forwards( ) )
   {
      string str( get_obj( ).Fields( ) );
      do
      {
         if( get_obj( ).child_Specification_Parent( ).Name( ) == "field_svlist" )
         {
            string key_info( FIELD_ID( Meta, Specification, Order ) );
            key_info += ' ';

            if( !get_obj( ).child_Specification_Parent( ).child_Specification_Parent( ).get_is_iterating( )
             && get_obj( ).child_Specification_Parent( ).child_Specification_Parent( ).iterate_forwards( key_info ) )
            {
               do
               {
                  if( !str.empty( ) )
                     str += " ";

                  str += get_obj( ).child_Specification_Parent( ).child_Specification_Parent( ).Source_Parent( ).Name( );
                  str += get_obj( ).child_Specification_Parent( ).child_Specification_Parent( ).Source_Field( ).Name( );

                  str += ",";
                  str += get_obj( ).child_Specification_Parent( ).child_Specification_Parent( ).Source_Child( ).Name( );

                  str += ",";
                  str += get_obj( ).child_Specification_Parent( ).child_Specification_Parent( ).Value( );

               } while( get_obj( ).child_Specification_Parent( ).child_Specification_Parent( ).iterate_next( ) );
            }

            get_obj( ).child_Specification_Parent( ).iterate_stop( );
            break;
         }

      } while( get_obj( ).child_Specification_Parent( ).iterate_next( ) );

      get_obj( ).Fields( str );
   }
   // [(finish meta_spec_field_pairs)] 600200a

   // [(start meta_spec_field_pairs)] 600201
   if( !is_create
    && !get_obj( ).child_Specification_Parent( ).get_is_iterating( )
    && get_obj( ).child_Specification_Parent( ).iterate_forwards( ) )
   {
      string str( get_obj( ).Field_Pairs( ) );
      do
      {
         if( get_obj( ).child_Specification_Parent( ).Name( ) == "field_pair_list" )
         {
            string key_info( FIELD_ID( Meta, Specification, Order ) );
            key_info += ' ';

            if( !get_obj( ).child_Specification_Parent( ).child_Specification_Parent( ).get_is_iterating( )
             && get_obj( ).child_Specification_Parent( ).child_Specification_Parent( ).iterate_forwards( key_info ) )
            {
               do
               {
                  if( !str.empty( ) )
                     str += " ";

                  str += get_obj( ).child_Specification_Parent( ).child_Specification_Parent( ).Field( ).Name( );

                  str += ",";
                  str += get_obj( ).child_Specification_Parent( ).child_Specification_Parent( ).Source_Child( ).Name( );

                  str += ",";
                  str += get_obj( ).child_Specification_Parent( ).child_Specification_Parent( ).Field( ).Parent_Class_Name( );

               } while( get_obj( ).child_Specification_Parent( ).child_Specification_Parent( ).iterate_next( ) );
            }

            get_obj( ).child_Specification_Parent( ).iterate_stop( );
            break;
         }

      } while( get_obj( ).child_Specification_Parent( ).iterate_next( ) );

      get_obj( ).Field_Pairs( str );
   }
   // [(finish meta_spec_field_pairs)] 600201

   // [(start meta_spec_field_pairs)] 600202
   if( !is_create
    && !get_obj( ).child_Specification_Parent( ).get_is_iterating( )
    && get_obj( ).child_Specification_Parent( ).iterate_forwards( ) )
   {
      string str( get_obj( ).Field_Pairs( ) );
      do
      {
         if( get_obj( ).child_Specification_Parent( ).Name( ) == "field_pair2_list" )
         {
            string key_info( FIELD_ID( Meta, Specification, Order ) );
            key_info += ' ';

            if( !get_obj( ).child_Specification_Parent( ).child_Specification_Parent( ).get_is_iterating( )
             && get_obj( ).child_Specification_Parent( ).child_Specification_Parent( ).iterate_forwards( key_info ) )
            {
               do
               {
                  if( !str.empty( ) )
                     str += " ";

                  str += get_obj( ).child_Specification_Parent( ).child_Specification_Parent( ).Field( ).Name( );

                  str += ",";
                  str += get_obj( ).child_Specification_Parent( ).child_Specification_Parent( ).Source_Field( ).Name( );

                  str += ",";
                  str += get_obj( ).child_Specification_Parent( ).child_Specification_Parent( ).Field( ).Parent_Class_Name( );

               } while( get_obj( ).child_Specification_Parent( ).child_Specification_Parent( ).iterate_next( ) );
            }

            get_obj( ).child_Specification_Parent( ).iterate_stop( );
            break;
         }

      } while( get_obj( ).child_Specification_Parent( ).iterate_next( ) );

      get_obj( ).Field_Pairs( str );
   }
   // [(finish meta_spec_field_pairs)] 600202

   // [(start meta_spec_field_pairs)] 600203
   if( !is_create
    && !get_obj( ).child_Specification_Parent( ).get_is_iterating( )
    && get_obj( ).child_Specification_Parent( ).iterate_forwards( ) )
   {
      string str( get_obj( ).Field_Pairs( ) );
      do
      {
         if( get_obj( ).child_Specification_Parent( ).Name( ) == "field_pair_g_list" )
         {
            string key_info( FIELD_ID( Meta, Specification, Order ) );
            key_info += ' ';

            if( !get_obj( ).child_Specification_Parent( ).child_Specification_Parent( ).get_is_iterating( )
             && get_obj( ).child_Specification_Parent( ).child_Specification_Parent( ).iterate_forwards( key_info ) )
            {
               do
               {
                  if( !str.empty( ) )
                     str += " ";

                  str += get_obj( ).child_Specification_Parent( ).child_Specification_Parent( ).Field( ).Name( );

                  str += ",";
                  str += get_obj( ).child_Specification_Parent( ).child_Specification_Parent( ).Source_Grandchild( ).Name( );

                  str += ",";
                  str += get_obj( ).child_Specification_Parent( ).child_Specification_Parent( ).Field( ).Parent_Class_Name( );

               } while( get_obj( ).child_Specification_Parent( ).child_Specification_Parent( ).iterate_next( ) );
            }

            get_obj( ).child_Specification_Parent( ).iterate_stop( );
            break;
         }

      } while( get_obj( ).child_Specification_Parent( ).iterate_next( ) );

      get_obj( ).Field_Pairs( str );
   }
   // [(finish meta_spec_field_pairs)] 600203

   // [(start meta_spec_field_pairs)] 600203a
   if( !is_create
    && !get_obj( ).child_Specification_Parent( ).get_is_iterating( )
    && get_obj( ).child_Specification_Parent( ).iterate_forwards( ) )
   {
      string str( get_obj( ).Field_Pairs( ) );
      do
      {
         if( get_obj( ).child_Specification_Parent( ).Name( ) == "field_ocpair_list" )
         {
            string key_info( FIELD_ID( Meta, Specification, Order ) );
            key_info += ' ';

            if( !get_obj( ).child_Specification_Parent( ).child_Specification_Parent( ).get_is_iterating( )
             && get_obj( ).child_Specification_Parent( ).child_Specification_Parent( ).iterate_forwards( key_info ) )
            {
               do
               {
                  if( !str.empty( ) )
                     str += " ";

                  str += get_obj( ).child_Specification_Parent( ).child_Specification_Parent( ).Field( ).Name( );
                  str += ",";
                  str += get_obj( ).child_Specification_Parent( ).child_Specification_Parent( ).Other_Class_Field( ).Name( );

               } while( get_obj( ).child_Specification_Parent( ).child_Specification_Parent( ).iterate_next( ) );
            }

            get_obj( ).child_Specification_Parent( ).iterate_stop( );
            break;
         }

      } while( get_obj( ).child_Specification_Parent( ).iterate_next( ) );

      get_obj( ).Field_Pairs( str );
   }
   // [(finish meta_spec_field_pairs)] 600203a

   // [(start field_from_search_replace)] 600204
   if( get_obj( ).get_is_editing( )
    && !get_obj( ).get_is_for_peer( ) )
   {
      string str( get_obj( ).Specification_Type( ).Specification_Name( ) );

      get_obj( ).Name( str );

      get_obj( ).add_search_replacement( "Name", "{id}", to_rep_string( get_obj( ).Id( ) ) );
      get_obj( ).add_search_replacement( "Name", "{model}", to_rep_string( get_obj( ).Model( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Name", "{class}", to_rep_string( get_obj( ).Class( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Name", "{field}", to_rep_string( get_obj( ).Field( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Name", "{ofield}", to_rep_string( get_obj( ).Other_Field( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Name", "{o2field}", to_rep_string( get_obj( ).Other_Field_2( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Name", "{modifier}", to_rep_string( get_obj( ).Modifier( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Name", "{omodifier}", to_rep_string( get_obj( ).Other_Modifier( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Name", "{o2modifier}", to_rep_string( get_obj( ).Other_Modifier_2( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Name", "{oclass}", to_rep_string( get_obj( ).Other_Class( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Name", "{ocfield}", to_rep_string( get_obj( ).Other_Class_Field( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Name", "{ocfield_id}", to_rep_string( get_obj( ).Other_Class_Field( ).Id( ) ) );
      get_obj( ).add_search_replacement( "Name", "{sclass}", to_rep_string( get_obj( ).Source_Class( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Name", "{spfield}", to_rep_string( get_obj( ).Source_Parent( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Name", "{spclass}", to_rep_string( get_obj( ).Source_Parent_Class( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Name", "{scfield}", to_rep_string( get_obj( ).Source_Child( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Name", "{oscfield}", to_rep_string( get_obj( ).Other_Source_Child( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Name", "{o2scfield}", to_rep_string( get_obj( ).Other_Source_Child_2( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Name", "{sfield}", to_rep_string( get_obj( ).Source_Field( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Name", "{sgcfield}", to_rep_string( get_obj( ).Source_Grandchild( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Name", "{tpfield}", to_rep_string( get_obj( ).Test_Parent( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Name", "{tcfield}", to_rep_string( get_obj( ).Test_Child( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Name", "{tfield}", to_rep_string( get_obj( ).Test_Field( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Name", "{tvalue}", to_rep_string( get_obj( ).Test_Value( ) ) );
      get_obj( ).add_search_replacement( "Name", "{tvalue_label}", to_rep_string( get_obj( ).Test_Value_Label( ) ) );
      get_obj( ).add_search_replacement( "Name", "{permission}", to_rep_string( get_obj( ).Permission( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Name", "{opermission}", to_rep_string( get_obj( ).Other_Permission( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Name", "{o2permission}", to_rep_string( get_obj( ).Other_Permission_2( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Name", "{procedure}", to_rep_string( get_obj( ).Procedure( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Name", "{procedure_arg}", to_rep_string( get_obj( ).Procedure_Arg( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Name", "{oprocedure}", to_rep_string( get_obj( ).Other_Procedure( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Name", "{o2procedure}", to_rep_string( get_obj( ).Other_Procedure_2( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Name", "{cname}", to_rep_string( get_obj( ).Child_Relationship( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Name", "{child}", to_rep_string( get_obj( ).Child_Relationship( ).Child_Name( ) ) );
      get_obj( ).add_search_replacement( "Name", "{cclass}", to_rep_string( get_obj( ).Child_Relationship( ).Child_Class_Name( ) ) );
      get_obj( ).add_search_replacement( "Name", "{enum}", to_rep_string( get_obj( ).Enum( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Name", "{enum_item}", to_rep_string( get_obj( ).Enum_Item( ).Label( ) ) );
      get_obj( ).add_search_replacement( "Name", "{value}", to_rep_string( get_obj( ).Value( ) ) );
      get_obj( ).add_search_replacement( "Name", "{value_label}", to_rep_string( get_obj( ).Value_Label( ) ) );
      get_obj( ).add_search_replacement( "Name", "{value_numstr}", to_rep_string( get_obj( ).Value_Numeric_String( ) ) );
      get_obj( ).add_search_replacement( "Name", "{field_values}", to_rep_string( get_obj( ).Field_Values( ) ) );
      get_obj( ).add_search_replacement( "Name", "{fields}", to_rep_string( get_obj( ).Fields( ) ) );
      get_obj( ).add_search_replacement( "Name", "{field_pairs}", to_rep_string( get_obj( ).Field_Pairs( ) ) );

      get_obj( ).set_search_replace_separator( "Name", '_' );

      get_obj( ).set_search_replace_has_opt_prefixing( "Name" );
   }
   // [(finish field_from_search_replace)] 600204

   // [(start field_clear)] 610164
   if( get_obj( ).Specification_Type( ).Allow_FK_Source_Field_Choice( ) && !get_obj( ).Use_Source_Parent( ) )
      get_obj( ).Other_Source_Child( string( ) );
   // [(finish field_clear)] 610164

   // [(start field_from_other_field)] 610192
   get_obj( ).Source_Field_Class( get_obj( ).Source_Field( ).Parent_Class( ) );
   // [(finish field_from_other_field)] 610192

   // [(start field_clear)] 620164
   if( get_obj( ).Specification_Type( ).Allow_FK_Source_Field_Choice( ) && !get_obj( ).Use_Source_Parent( ) )
      get_obj( ).Other_Source_Child_2( string( ) );
   // [(finish field_clear)] 620164

   // [<start to_store>]
   // [<finish to_store>]
}

void Meta_Specification::impl::for_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start parent_auto_int_inc)] 610194
   if( is_create && is_null( get_obj( ).Id( ) ) )
   {
      if( is_null( get_obj( ).Model( ) ) )
         throw runtime_error( "unexpected empty Model" );

      get_obj( ).Model( ).op_update( get_obj( ).Model( ), FIELD_NAME( Meta, Model, Next_Specification_Id ) );

      get_obj( ).Id( get_obj( ).Model( ).Next_Specification_Id( ) );

      get_obj( ).Model( ).Next_Specification_Id( auto_int_increment( get_obj( ).Model( ).Next_Specification_Id( ) ) );
      get_obj( ).Model( ).op_apply( );
   }
   // [(finish parent_auto_int_inc)] 610194

   // [<start for_store>]
//nyi
   if( is_create && get_obj( ).Specification_Type( ).Has_Next_Specification_Info( ) == true )
   {
      class_pointer< Meta_Specification > cp_parent( &get_obj( ).Parent_Specification( ) );

      cp_parent->op_update( );

      cp_parent->Actions( get_obj( ).Specification_Type( ).Next_Specification_Actions( ) );
      cp_parent->Child_Specification_Type( get_obj( ).Specification_Type( ).Next_Child_Specification_Type( ) );
      cp_parent->Protect_Source_Parent( get_obj( ).Specification_Type( ).Next_Protect_Source_Parent( ) );
      cp_parent->Protect_Procedure( get_obj( ).Specification_Type( ).Next_Protect_Procedure( ) );
      cp_parent->Protect_Child_Rel( get_obj( ).Specification_Type( ).Next_Protect_Child_Rel( ) );

      cp_parent->op_apply( );
   }

   if( !get_obj( ).Options( ).empty( ) )
      get_obj( ).Name( get_obj( ).Name( ) + " " + get_obj( ).Options( ) );
   // [<finish for_store>]
}

void Meta_Specification::impl::after_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start copy_child_links_from_fk)] 600170
   if( is_create
    && get_obj( ).get_clone_key( ).empty( )
    && get_obj( ).Specification_Type( ).child_Specification_Type_Parent( ).iterate_forwards( ) )
   {
      int child_num = 0;

      do
      {
         string child_key( construct_key_from_int( get_obj( ).get_key( ), ++child_num ) );

         get_obj( ).child_Specification_Parent( ).op_create( child_key );

         get_obj( ).child_Specification_Parent( ).Parent_Specification( get_obj( ).get_key( ) );
         get_obj( ).child_Specification_Parent( ).Specification_Type( get_obj( ).Specification_Type( ).child_Specification_Type_Parent( ).get_key( ) );

         copy_field_or_file_and_field( get_obj( ).child_Specification_Parent( ), child_key,
          "Actions", get_obj( ).Specification_Type( ).child_Specification_Type_Parent( ), "Specification_Actions", storage_locked_for_admin( ) );

         get_obj( ).child_Specification_Parent( ).op_apply( );

      } while( get_obj( ).Specification_Type( ).child_Specification_Type_Parent( ).iterate_next( ) );
   }
   // [(finish copy_child_links_from_fk)] 600170

   // [(start update_parent_cascade)] 620194
   if( get_obj( ).Specification_Type( ).get_key( ) == "field" )
   {
      class_pointer< Meta_Specification > cp_parent( &get_obj( ).Parent_Specification( ).Parent_Specification( ) );

      if( !cp_parent->get_key( ).empty( ) && !is_destroy_locked_by_own_session( *cp_parent ) )
      {
         cp_parent->op_update( );
         cp_parent->op_apply( );
      }
   }
   // [(finish update_parent_cascade)] 620194

   // [(start update_parent_cascade)] 630194
   if( get_obj( ).Specification_Type( ).get_key( ) == "field_pair" )
   {
      class_pointer< Meta_Specification > cp_parent( &get_obj( ).Parent_Specification( ).Parent_Specification( ) );

      if( !cp_parent->get_key( ).empty( ) && !is_destroy_locked_by_own_session( *cp_parent ) )
      {
         cp_parent->op_update( );
         cp_parent->op_apply( );
      }
   }
   // [(finish update_parent_cascade)] 630194

   // [(start update_parent_cascade)] 640194
   if( get_obj( ).Specification_Type( ).get_key( ) == "field_pair2" )
   {
      class_pointer< Meta_Specification > cp_parent( &get_obj( ).Parent_Specification( ).Parent_Specification( ) );

      if( !cp_parent->get_key( ).empty( ) && !is_destroy_locked_by_own_session( *cp_parent ) )
      {
         cp_parent->op_update( );
         cp_parent->op_apply( );
      }
   }
   // [(finish update_parent_cascade)] 640194

   // [(start update_parent_cascade)] 650194
   if( get_obj( ).Specification_Type( ).get_key( ) == "field_pair_g" )
   {
      class_pointer< Meta_Specification > cp_parent( &get_obj( ).Parent_Specification( ).Parent_Specification( ) );

      if( !cp_parent->get_key( ).empty( ) && !is_destroy_locked_by_own_session( *cp_parent ) )
      {
         cp_parent->op_update( );
         cp_parent->op_apply( );
      }
   }
   // [(finish update_parent_cascade)] 650194

   // [(start update_parent_cascade)] 660194
   if( get_obj( ).Specification_Type( ).get_key( ) == "field_restrict" )
   {
      class_pointer< Meta_Specification > cp_parent( &get_obj( ).Parent_Specification( ).Parent_Specification( ) );

      if( !cp_parent->get_key( ).empty( ) && !is_destroy_locked_by_own_session( *cp_parent ) )
      {
         cp_parent->op_update( );
         cp_parent->op_apply( );
      }
   }
   // [(finish update_parent_cascade)] 660194

   // [(start update_parent_cascade)] 670194
   if( get_obj( ).Specification_Type( ).get_key( ) == "field_source_and_value" )
   {
      class_pointer< Meta_Specification > cp_parent( &get_obj( ).Parent_Specification( ).Parent_Specification( ) );

      if( !cp_parent->get_key( ).empty( ) && !is_destroy_locked_by_own_session( *cp_parent ) )
      {
         cp_parent->op_update( );
         cp_parent->op_apply( );
      }
   }
   // [(finish update_parent_cascade)] 670194

   // [(start update_parent_cascade)] 680194
   if( get_obj( ).Specification_Type( ).get_key( ) == "home_message_stats_info" )
   {
      class_pointer< Meta_Specification > cp_parent( &get_obj( ).Parent_Specification( ) );

      if( !cp_parent->get_key( ).empty( ) && !is_destroy_locked_by_own_session( *cp_parent ) )
      {
         cp_parent->op_update( );
         cp_parent->op_apply( );
      }
   }
   // [(finish update_parent_cascade)] 680194

   // [(start update_parent_cascade)] 690194
   if( get_obj( ).Specification_Type( ).get_key( ) == "field_ocpair" )
   {
      class_pointer< Meta_Specification > cp_parent( &get_obj( ).Parent_Specification( ).Parent_Specification( ) );

      if( !cp_parent->get_key( ).empty( ) && !is_destroy_locked_by_own_session( *cp_parent ) )
      {
         cp_parent->op_update( );
         cp_parent->op_apply( );
      }
   }
   // [(finish update_parent_cascade)] 690194

   // [<start after_store>]
   // [<finish after_store>]
}

bool Meta_Specification::impl::can_destroy( bool is_internal )
{
   uint64_t state = p_obj->get_state( );

   bool retval = ( is_internal || !( state & c_state_undeletable ) );

   // [(start destroy_restrict)] 600180
   if( !is_internal && ( get_obj( ).Specification_Type( ).Is_System( ) == true ) )
      retval = false;
   // [(finish destroy_restrict)] 600180

   // [<start can_destroy>]
   // [<finish can_destroy>]

   return retval;
}

void Meta_Specification::impl::for_destroy( bool is_internal )
{
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start for_destroy>]
   // [<finish for_destroy>]
}

void Meta_Specification::impl::after_destroy( bool is_internal )
{
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start update_parent_cascade)] 620194
   if( !get_obj( ).get_is_being_cascaded( )
    && !is_destroy_locked_by_own_session( get_obj( ).Parent_Specification( ) )
    && get_obj( ).Specification_Type( ).get_key( ) == "field" )
   {
      class_pointer< Meta_Specification > cp_parent( &get_obj( ).Parent_Specification( ).Parent_Specification( ) );

      if( !cp_parent->get_key( ).empty( ) && !is_destroy_locked_by_own_session( *cp_parent ) )
      {
         cp_parent->op_update( );
         cp_parent->op_apply( );
      }
   }
   // [(finish update_parent_cascade)] 620194

   // [(start update_parent_cascade)] 630194
   if( !get_obj( ).get_is_being_cascaded( )
    && !is_destroy_locked_by_own_session( get_obj( ).Parent_Specification( ) )
    && get_obj( ).Specification_Type( ).get_key( ) == "field_pair" )
   {
      class_pointer< Meta_Specification > cp_parent( &get_obj( ).Parent_Specification( ).Parent_Specification( ) );

      if( !cp_parent->get_key( ).empty( ) && !is_destroy_locked_by_own_session( *cp_parent ) )
      {
         cp_parent->op_update( );
         cp_parent->op_apply( );
      }
   }
   // [(finish update_parent_cascade)] 630194

   // [(start update_parent_cascade)] 640194
   if( !get_obj( ).get_is_being_cascaded( )
    && !is_destroy_locked_by_own_session( get_obj( ).Parent_Specification( ) )
    && get_obj( ).Specification_Type( ).get_key( ) == "field_pair2" )
   {
      class_pointer< Meta_Specification > cp_parent( &get_obj( ).Parent_Specification( ).Parent_Specification( ) );

      if( !cp_parent->get_key( ).empty( ) && !is_destroy_locked_by_own_session( *cp_parent ) )
      {
         cp_parent->op_update( );
         cp_parent->op_apply( );
      }
   }
   // [(finish update_parent_cascade)] 640194

   // [(start update_parent_cascade)] 650194
   if( !get_obj( ).get_is_being_cascaded( )
    && !is_destroy_locked_by_own_session( get_obj( ).Parent_Specification( ) )
    && get_obj( ).Specification_Type( ).get_key( ) == "field_pair_g" )
   {
      class_pointer< Meta_Specification > cp_parent( &get_obj( ).Parent_Specification( ).Parent_Specification( ) );

      if( !cp_parent->get_key( ).empty( ) && !is_destroy_locked_by_own_session( *cp_parent ) )
      {
         cp_parent->op_update( );
         cp_parent->op_apply( );
      }
   }
   // [(finish update_parent_cascade)] 650194

   // [(start update_parent_cascade)] 660194
   if( !get_obj( ).get_is_being_cascaded( )
    && !is_destroy_locked_by_own_session( get_obj( ).Parent_Specification( ) )
    && get_obj( ).Specification_Type( ).get_key( ) == "field_restrict" )
   {
      class_pointer< Meta_Specification > cp_parent( &get_obj( ).Parent_Specification( ).Parent_Specification( ) );

      if( !cp_parent->get_key( ).empty( ) && !is_destroy_locked_by_own_session( *cp_parent ) )
      {
         cp_parent->op_update( );
         cp_parent->op_apply( );
      }
   }
   // [(finish update_parent_cascade)] 660194

   // [(start update_parent_cascade)] 670194
   if( !get_obj( ).get_is_being_cascaded( )
    && !is_destroy_locked_by_own_session( get_obj( ).Parent_Specification( ) )
    && get_obj( ).Specification_Type( ).get_key( ) == "field_source_and_value" )
   {
      class_pointer< Meta_Specification > cp_parent( &get_obj( ).Parent_Specification( ).Parent_Specification( ) );

      if( !cp_parent->get_key( ).empty( ) && !is_destroy_locked_by_own_session( *cp_parent ) )
      {
         cp_parent->op_update( );
         cp_parent->op_apply( );
      }
   }
   // [(finish update_parent_cascade)] 670194

   // [(start update_parent_cascade)] 680194
   if( !get_obj( ).get_is_being_cascaded( )
    && !is_destroy_locked_by_own_session( get_obj( ).Parent_Specification( ) )
    && get_obj( ).Specification_Type( ).get_key( ) == "home_message_stats_info" )
   {
      class_pointer< Meta_Specification > cp_parent( &get_obj( ).Parent_Specification( ) );

      if( !cp_parent->get_key( ).empty( ) && !is_destroy_locked_by_own_session( *cp_parent ) )
      {
         cp_parent->op_update( );
         cp_parent->op_apply( );
      }
   }
   // [(finish update_parent_cascade)] 680194

   // [(start update_parent_cascade)] 690194
   if( !get_obj( ).get_is_being_cascaded( )
    && !is_destroy_locked_by_own_session( get_obj( ).Parent_Specification( ) )
    && get_obj( ).Specification_Type( ).get_key( ) == "field_ocpair" )
   {
      class_pointer< Meta_Specification > cp_parent( &get_obj( ).Parent_Specification( ).Parent_Specification( ) );

      if( !cp_parent->get_key( ).empty( ) && !is_destroy_locked_by_own_session( *cp_parent ) )
      {
         cp_parent->op_update( );
         cp_parent->op_apply( );
      }
   }
   // [(finish update_parent_cascade)] 690194

   // [<start after_destroy>]
   // [<finish after_destroy>]
}

void Meta_Specification::impl::set_default_values( )
{
   clear( );
}

bool Meta_Specification::impl::is_filtered( ) const
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start is_filtered>]
   // [<finish is_filtered>]

   return false;
}

void Meta_Specification::impl::get_required_transients( ) const
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

Meta_Specification::Meta_Specification( )
{
   set_version( c_version );

   p_impl = new impl( *this );
}

Meta_Specification::~Meta_Specification( )
{
   cleanup( );
   delete p_impl;
}

const string& Meta_Specification::Actions( ) const
{
   return p_impl->impl_Actions( );
}

void Meta_Specification::Actions( const string& Actions )
{
   p_impl->impl_Actions( Actions );
}

const string& Meta_Specification::All_Strings( ) const
{
   return p_impl->impl_All_Strings( );
}

void Meta_Specification::All_Strings( const string& All_Strings )
{
   p_impl->impl_All_Strings( All_Strings );
}

const string& Meta_Specification::All_Vars( ) const
{
   return p_impl->impl_All_Vars( );
}

void Meta_Specification::All_Vars( const string& All_Vars )
{
   p_impl->impl_All_Vars( All_Vars );
}

bool Meta_Specification::Can_Access_Below_Model( ) const
{
   return p_impl->impl_Can_Access_Below_Model( );
}

void Meta_Specification::Can_Access_Below_Model( bool Can_Access_Below_Model )
{
   p_impl->impl_Can_Access_Below_Model( Can_Access_Below_Model );
}

const string& Meta_Specification::Child_Rel_Field_Key( ) const
{
   return p_impl->impl_Child_Rel_Field_Key( );
}

void Meta_Specification::Child_Rel_Field_Key( const string& Child_Rel_Field_Key )
{
   p_impl->impl_Child_Rel_Field_Key( Child_Rel_Field_Key );
}

const string& Meta_Specification::Comments( ) const
{
   return p_impl->impl_Comments( );
}

void Meta_Specification::Comments( const string& Comments )
{
   p_impl->impl_Comments( Comments );
}

const string& Meta_Specification::Field_Pairs( ) const
{
   return p_impl->impl_Field_Pairs( );
}

void Meta_Specification::Field_Pairs( const string& Field_Pairs )
{
   p_impl->impl_Field_Pairs( Field_Pairs );
}

const string& Meta_Specification::Field_Values( ) const
{
   return p_impl->impl_Field_Values( );
}

void Meta_Specification::Field_Values( const string& Field_Values )
{
   p_impl->impl_Field_Values( Field_Values );
}

const string& Meta_Specification::Fields( ) const
{
   return p_impl->impl_Fields( );
}

void Meta_Specification::Fields( const string& Fields )
{
   p_impl->impl_Fields( Fields );
}

const string& Meta_Specification::Id( ) const
{
   return p_impl->impl_Id( );
}

void Meta_Specification::Id( const string& Id )
{
   p_impl->impl_Id( Id );
}

bool Meta_Specification::Is_Required_For_UI_Gen( ) const
{
   return p_impl->impl_Is_Required_For_UI_Gen( );
}

void Meta_Specification::Is_Required_For_UI_Gen( bool Is_Required_For_UI_Gen )
{
   p_impl->impl_Is_Required_For_UI_Gen( Is_Required_For_UI_Gen );
}

const string& Meta_Specification::Name( ) const
{
   return p_impl->impl_Name( );
}

void Meta_Specification::Name( const string& Name )
{
   p_impl->impl_Name( Name );
}

const string& Meta_Specification::Options( ) const
{
   return p_impl->impl_Options( );
}

void Meta_Specification::Options( const string& Options )
{
   p_impl->impl_Options( Options );
}

const string& Meta_Specification::Order( ) const
{
   return p_impl->impl_Order( );
}

void Meta_Specification::Order( const string& Order )
{
   p_impl->impl_Order( Order );
}

bool Meta_Specification::Protect_Child_Rel( ) const
{
   return p_impl->impl_Protect_Child_Rel( );
}

void Meta_Specification::Protect_Child_Rel( bool Protect_Child_Rel )
{
   p_impl->impl_Protect_Child_Rel( Protect_Child_Rel );
}

bool Meta_Specification::Protect_Procedure( ) const
{
   return p_impl->impl_Protect_Procedure( );
}

void Meta_Specification::Protect_Procedure( bool Protect_Procedure )
{
   p_impl->impl_Protect_Procedure( Protect_Procedure );
}

bool Meta_Specification::Protect_Source_Parent( ) const
{
   return p_impl->impl_Protect_Source_Parent( );
}

void Meta_Specification::Protect_Source_Parent( bool Protect_Source_Parent )
{
   p_impl->impl_Protect_Source_Parent( Protect_Source_Parent );
}

const string& Meta_Specification::Restrict_Values( ) const
{
   return p_impl->impl_Restrict_Values( );
}

void Meta_Specification::Restrict_Values( const string& Restrict_Values )
{
   p_impl->impl_Restrict_Values( Restrict_Values );
}

const string& Meta_Specification::Strings( ) const
{
   return p_impl->impl_Strings( );
}

void Meta_Specification::Strings( const string& Strings )
{
   p_impl->impl_Strings( Strings );
}

const string& Meta_Specification::Test_Value( ) const
{
   return p_impl->impl_Test_Value( );
}

void Meta_Specification::Test_Value( const string& Test_Value )
{
   p_impl->impl_Test_Value( Test_Value );
}

const string& Meta_Specification::Test_Value_Label( ) const
{
   return p_impl->impl_Test_Value_Label( );
}

void Meta_Specification::Test_Value_Label( const string& Test_Value_Label )
{
   p_impl->impl_Test_Value_Label( Test_Value_Label );
}

bool Meta_Specification::Use_Source_Parent( ) const
{
   return p_impl->impl_Use_Source_Parent( );
}

void Meta_Specification::Use_Source_Parent( bool Use_Source_Parent )
{
   p_impl->impl_Use_Source_Parent( Use_Source_Parent );
}

bool Meta_Specification::Use_Test_Parent_Child( ) const
{
   return p_impl->impl_Use_Test_Parent_Child( );
}

void Meta_Specification::Use_Test_Parent_Child( bool Use_Test_Parent_Child )
{
   p_impl->impl_Use_Test_Parent_Child( Use_Test_Parent_Child );
}

const string& Meta_Specification::Value( ) const
{
   return p_impl->impl_Value( );
}

void Meta_Specification::Value( const string& Value )
{
   p_impl->impl_Value( Value );
}

const string& Meta_Specification::Value_Label( ) const
{
   return p_impl->impl_Value_Label( );
}

void Meta_Specification::Value_Label( const string& Value_Label )
{
   p_impl->impl_Value_Label( Value_Label );
}

const string& Meta_Specification::Value_Left_Part( ) const
{
   return p_impl->impl_Value_Left_Part( );
}

void Meta_Specification::Value_Left_Part( const string& Value_Left_Part )
{
   p_impl->impl_Value_Left_Part( Value_Left_Part );
}

const string& Meta_Specification::Value_Literal( ) const
{
   return p_impl->impl_Value_Literal( );
}

void Meta_Specification::Value_Literal( const string& Value_Literal )
{
   p_impl->impl_Value_Literal( Value_Literal );
}

const string& Meta_Specification::Value_Numeric_String( ) const
{
   return p_impl->impl_Value_Numeric_String( );
}

void Meta_Specification::Value_Numeric_String( const string& Value_Numeric_String )
{
   p_impl->impl_Value_Numeric_String( Value_Numeric_String );
}

const string& Meta_Specification::Value_Right_Part( ) const
{
   return p_impl->impl_Value_Right_Part( );
}

void Meta_Specification::Value_Right_Part( const string& Value_Right_Part )
{
   p_impl->impl_Value_Right_Part( Value_Right_Part );
}

const string& Meta_Specification::Value_String( ) const
{
   return p_impl->impl_Value_String( );
}

void Meta_Specification::Value_String( const string& Value_String )
{
   p_impl->impl_Value_String( Value_String );
}

const string& Meta_Specification::Vars( ) const
{
   return p_impl->impl_Vars( );
}

void Meta_Specification::Vars( const string& Vars )
{
   p_impl->impl_Vars( Vars );
}

Meta_Class& Meta_Specification::Child_Rel_Child_Class( )
{
   return p_impl->impl_Child_Rel_Child_Class( );
}

const Meta_Class& Meta_Specification::Child_Rel_Child_Class( ) const
{
   return p_impl->impl_Child_Rel_Child_Class( );
}

void Meta_Specification::Child_Rel_Child_Class( const string& key )
{
   p_impl->impl_Child_Rel_Child_Class( key );
}

Meta_Relationship& Meta_Specification::Child_Relationship( )
{
   return p_impl->impl_Child_Relationship( );
}

const Meta_Relationship& Meta_Specification::Child_Relationship( ) const
{
   return p_impl->impl_Child_Relationship( );
}

void Meta_Specification::Child_Relationship( const string& key )
{
   p_impl->impl_Child_Relationship( key );
}

Meta_Specification_Type& Meta_Specification::Child_Specification_Type( )
{
   return p_impl->impl_Child_Specification_Type( );
}

const Meta_Specification_Type& Meta_Specification::Child_Specification_Type( ) const
{
   return p_impl->impl_Child_Specification_Type( );
}

void Meta_Specification::Child_Specification_Type( const string& key )
{
   p_impl->impl_Child_Specification_Type( key );
}

Meta_Class& Meta_Specification::Class( )
{
   return p_impl->impl_Class( );
}

const Meta_Class& Meta_Specification::Class( ) const
{
   return p_impl->impl_Class( );
}

void Meta_Specification::Class( const string& key )
{
   p_impl->impl_Class( key );
}

Meta_Enum& Meta_Specification::Enum( )
{
   return p_impl->impl_Enum( );
}

const Meta_Enum& Meta_Specification::Enum( ) const
{
   return p_impl->impl_Enum( );
}

void Meta_Specification::Enum( const string& key )
{
   p_impl->impl_Enum( key );
}

Meta_Enum_Item& Meta_Specification::Enum_Item( )
{
   return p_impl->impl_Enum_Item( );
}

const Meta_Enum_Item& Meta_Specification::Enum_Item( ) const
{
   return p_impl->impl_Enum_Item( );
}

void Meta_Specification::Enum_Item( const string& key )
{
   p_impl->impl_Enum_Item( key );
}

Meta_Enum_Item& Meta_Specification::Enum_Item_2( )
{
   return p_impl->impl_Enum_Item_2( );
}

const Meta_Enum_Item& Meta_Specification::Enum_Item_2( ) const
{
   return p_impl->impl_Enum_Item_2( );
}

void Meta_Specification::Enum_Item_2( const string& key )
{
   p_impl->impl_Enum_Item_2( key );
}

Meta_Enum_Item& Meta_Specification::Enum_Item_3( )
{
   return p_impl->impl_Enum_Item_3( );
}

const Meta_Enum_Item& Meta_Specification::Enum_Item_3( ) const
{
   return p_impl->impl_Enum_Item_3( );
}

void Meta_Specification::Enum_Item_3( const string& key )
{
   p_impl->impl_Enum_Item_3( key );
}

Meta_Enum_Item& Meta_Specification::Enum_Item_4( )
{
   return p_impl->impl_Enum_Item_4( );
}

const Meta_Enum_Item& Meta_Specification::Enum_Item_4( ) const
{
   return p_impl->impl_Enum_Item_4( );
}

void Meta_Specification::Enum_Item_4( const string& key )
{
   p_impl->impl_Enum_Item_4( key );
}

Meta_Enum_Item& Meta_Specification::Enum_Item_5( )
{
   return p_impl->impl_Enum_Item_5( );
}

const Meta_Enum_Item& Meta_Specification::Enum_Item_5( ) const
{
   return p_impl->impl_Enum_Item_5( );
}

void Meta_Specification::Enum_Item_5( const string& key )
{
   p_impl->impl_Enum_Item_5( key );
}

Meta_Field& Meta_Specification::Field( )
{
   return p_impl->impl_Field( );
}

const Meta_Field& Meta_Specification::Field( ) const
{
   return p_impl->impl_Field( );
}

void Meta_Specification::Field( const string& key )
{
   p_impl->impl_Field( key );
}

Meta_Class& Meta_Specification::Field_Class( )
{
   return p_impl->impl_Field_Class( );
}

const Meta_Class& Meta_Specification::Field_Class( ) const
{
   return p_impl->impl_Field_Class( );
}

void Meta_Specification::Field_Class( const string& key )
{
   p_impl->impl_Field_Class( key );
}

Meta_Model& Meta_Specification::Model( )
{
   return p_impl->impl_Model( );
}

const Meta_Model& Meta_Specification::Model( ) const
{
   return p_impl->impl_Model( );
}

void Meta_Specification::Model( const string& key )
{
   p_impl->impl_Model( key );
}

Meta_Modifier& Meta_Specification::Modifier( )
{
   return p_impl->impl_Modifier( );
}

const Meta_Modifier& Meta_Specification::Modifier( ) const
{
   return p_impl->impl_Modifier( );
}

void Meta_Specification::Modifier( const string& key )
{
   p_impl->impl_Modifier( key );
}

Meta_Class& Meta_Specification::Other_Class( )
{
   return p_impl->impl_Other_Class( );
}

const Meta_Class& Meta_Specification::Other_Class( ) const
{
   return p_impl->impl_Other_Class( );
}

void Meta_Specification::Other_Class( const string& key )
{
   p_impl->impl_Other_Class( key );
}

Meta_Field& Meta_Specification::Other_Class_Field( )
{
   return p_impl->impl_Other_Class_Field( );
}

const Meta_Field& Meta_Specification::Other_Class_Field( ) const
{
   return p_impl->impl_Other_Class_Field( );
}

void Meta_Specification::Other_Class_Field( const string& key )
{
   p_impl->impl_Other_Class_Field( key );
}

Meta_Field& Meta_Specification::Other_Field( )
{
   return p_impl->impl_Other_Field( );
}

const Meta_Field& Meta_Specification::Other_Field( ) const
{
   return p_impl->impl_Other_Field( );
}

void Meta_Specification::Other_Field( const string& key )
{
   p_impl->impl_Other_Field( key );
}

Meta_Field& Meta_Specification::Other_Field_2( )
{
   return p_impl->impl_Other_Field_2( );
}

const Meta_Field& Meta_Specification::Other_Field_2( ) const
{
   return p_impl->impl_Other_Field_2( );
}

void Meta_Specification::Other_Field_2( const string& key )
{
   p_impl->impl_Other_Field_2( key );
}

Meta_Modifier& Meta_Specification::Other_Modifier( )
{
   return p_impl->impl_Other_Modifier( );
}

const Meta_Modifier& Meta_Specification::Other_Modifier( ) const
{
   return p_impl->impl_Other_Modifier( );
}

void Meta_Specification::Other_Modifier( const string& key )
{
   p_impl->impl_Other_Modifier( key );
}

Meta_Modifier& Meta_Specification::Other_Modifier_2( )
{
   return p_impl->impl_Other_Modifier_2( );
}

const Meta_Modifier& Meta_Specification::Other_Modifier_2( ) const
{
   return p_impl->impl_Other_Modifier_2( );
}

void Meta_Specification::Other_Modifier_2( const string& key )
{
   p_impl->impl_Other_Modifier_2( key );
}

Meta_Permission& Meta_Specification::Other_Permission( )
{
   return p_impl->impl_Other_Permission( );
}

const Meta_Permission& Meta_Specification::Other_Permission( ) const
{
   return p_impl->impl_Other_Permission( );
}

void Meta_Specification::Other_Permission( const string& key )
{
   p_impl->impl_Other_Permission( key );
}

Meta_Permission& Meta_Specification::Other_Permission_2( )
{
   return p_impl->impl_Other_Permission_2( );
}

const Meta_Permission& Meta_Specification::Other_Permission_2( ) const
{
   return p_impl->impl_Other_Permission_2( );
}

void Meta_Specification::Other_Permission_2( const string& key )
{
   p_impl->impl_Other_Permission_2( key );
}

Meta_Procedure& Meta_Specification::Other_Procedure( )
{
   return p_impl->impl_Other_Procedure( );
}

const Meta_Procedure& Meta_Specification::Other_Procedure( ) const
{
   return p_impl->impl_Other_Procedure( );
}

void Meta_Specification::Other_Procedure( const string& key )
{
   p_impl->impl_Other_Procedure( key );
}

Meta_Procedure& Meta_Specification::Other_Procedure_2( )
{
   return p_impl->impl_Other_Procedure_2( );
}

const Meta_Procedure& Meta_Specification::Other_Procedure_2( ) const
{
   return p_impl->impl_Other_Procedure_2( );
}

void Meta_Specification::Other_Procedure_2( const string& key )
{
   p_impl->impl_Other_Procedure_2( key );
}

Meta_Field& Meta_Specification::Other_Source_Child( )
{
   return p_impl->impl_Other_Source_Child( );
}

const Meta_Field& Meta_Specification::Other_Source_Child( ) const
{
   return p_impl->impl_Other_Source_Child( );
}

void Meta_Specification::Other_Source_Child( const string& key )
{
   p_impl->impl_Other_Source_Child( key );
}

Meta_Field& Meta_Specification::Other_Source_Child_2( )
{
   return p_impl->impl_Other_Source_Child_2( );
}

const Meta_Field& Meta_Specification::Other_Source_Child_2( ) const
{
   return p_impl->impl_Other_Source_Child_2( );
}

void Meta_Specification::Other_Source_Child_2( const string& key )
{
   p_impl->impl_Other_Source_Child_2( key );
}

Meta_Specification& Meta_Specification::Parent_Specification( )
{
   return p_impl->impl_Parent_Specification( );
}

const Meta_Specification& Meta_Specification::Parent_Specification( ) const
{
   return p_impl->impl_Parent_Specification( );
}

void Meta_Specification::Parent_Specification( const string& key )
{
   p_impl->impl_Parent_Specification( key );
}

Meta_Permission& Meta_Specification::Permission( )
{
   return p_impl->impl_Permission( );
}

const Meta_Permission& Meta_Specification::Permission( ) const
{
   return p_impl->impl_Permission( );
}

void Meta_Specification::Permission( const string& key )
{
   p_impl->impl_Permission( key );
}

Meta_Procedure& Meta_Specification::Procedure( )
{
   return p_impl->impl_Procedure( );
}

const Meta_Procedure& Meta_Specification::Procedure( ) const
{
   return p_impl->impl_Procedure( );
}

void Meta_Specification::Procedure( const string& key )
{
   p_impl->impl_Procedure( key );
}

Meta_Procedure_Arg& Meta_Specification::Procedure_Arg( )
{
   return p_impl->impl_Procedure_Arg( );
}

const Meta_Procedure_Arg& Meta_Specification::Procedure_Arg( ) const
{
   return p_impl->impl_Procedure_Arg( );
}

void Meta_Specification::Procedure_Arg( const string& key )
{
   p_impl->impl_Procedure_Arg( key );
}

Meta_Procedure_Arg& Meta_Specification::Procedure_Arg_2( )
{
   return p_impl->impl_Procedure_Arg_2( );
}

const Meta_Procedure_Arg& Meta_Specification::Procedure_Arg_2( ) const
{
   return p_impl->impl_Procedure_Arg_2( );
}

void Meta_Specification::Procedure_Arg_2( const string& key )
{
   p_impl->impl_Procedure_Arg_2( key );
}

Meta_Procedure_Arg& Meta_Specification::Procedure_Arg_3( )
{
   return p_impl->impl_Procedure_Arg_3( );
}

const Meta_Procedure_Arg& Meta_Specification::Procedure_Arg_3( ) const
{
   return p_impl->impl_Procedure_Arg_3( );
}

void Meta_Specification::Procedure_Arg_3( const string& key )
{
   p_impl->impl_Procedure_Arg_3( key );
}

Meta_Field& Meta_Specification::Source_Child( )
{
   return p_impl->impl_Source_Child( );
}

const Meta_Field& Meta_Specification::Source_Child( ) const
{
   return p_impl->impl_Source_Child( );
}

void Meta_Specification::Source_Child( const string& key )
{
   p_impl->impl_Source_Child( key );
}

Meta_Class& Meta_Specification::Source_Child_Class( )
{
   return p_impl->impl_Source_Child_Class( );
}

const Meta_Class& Meta_Specification::Source_Child_Class( ) const
{
   return p_impl->impl_Source_Child_Class( );
}

void Meta_Specification::Source_Child_Class( const string& key )
{
   p_impl->impl_Source_Child_Class( key );
}

Meta_Class& Meta_Specification::Source_Class( )
{
   return p_impl->impl_Source_Class( );
}

const Meta_Class& Meta_Specification::Source_Class( ) const
{
   return p_impl->impl_Source_Class( );
}

void Meta_Specification::Source_Class( const string& key )
{
   p_impl->impl_Source_Class( key );
}

Meta_Field& Meta_Specification::Source_Field( )
{
   return p_impl->impl_Source_Field( );
}

const Meta_Field& Meta_Specification::Source_Field( ) const
{
   return p_impl->impl_Source_Field( );
}

void Meta_Specification::Source_Field( const string& key )
{
   p_impl->impl_Source_Field( key );
}

Meta_Class& Meta_Specification::Source_Field_Class( )
{
   return p_impl->impl_Source_Field_Class( );
}

const Meta_Class& Meta_Specification::Source_Field_Class( ) const
{
   return p_impl->impl_Source_Field_Class( );
}

void Meta_Specification::Source_Field_Class( const string& key )
{
   p_impl->impl_Source_Field_Class( key );
}

Meta_Field& Meta_Specification::Source_Grandchild( )
{
   return p_impl->impl_Source_Grandchild( );
}

const Meta_Field& Meta_Specification::Source_Grandchild( ) const
{
   return p_impl->impl_Source_Grandchild( );
}

void Meta_Specification::Source_Grandchild( const string& key )
{
   p_impl->impl_Source_Grandchild( key );
}

Meta_Field& Meta_Specification::Source_Parent( )
{
   return p_impl->impl_Source_Parent( );
}

const Meta_Field& Meta_Specification::Source_Parent( ) const
{
   return p_impl->impl_Source_Parent( );
}

void Meta_Specification::Source_Parent( const string& key )
{
   p_impl->impl_Source_Parent( key );
}

Meta_Class& Meta_Specification::Source_Parent_Class( )
{
   return p_impl->impl_Source_Parent_Class( );
}

const Meta_Class& Meta_Specification::Source_Parent_Class( ) const
{
   return p_impl->impl_Source_Parent_Class( );
}

void Meta_Specification::Source_Parent_Class( const string& key )
{
   p_impl->impl_Source_Parent_Class( key );
}

Meta_Specification_Type& Meta_Specification::Specification_Type( )
{
   return p_impl->impl_Specification_Type( );
}

const Meta_Specification_Type& Meta_Specification::Specification_Type( ) const
{
   return p_impl->impl_Specification_Type( );
}

void Meta_Specification::Specification_Type( const string& key )
{
   p_impl->impl_Specification_Type( key );
}

Meta_Field& Meta_Specification::Test_Child( )
{
   return p_impl->impl_Test_Child( );
}

const Meta_Field& Meta_Specification::Test_Child( ) const
{
   return p_impl->impl_Test_Child( );
}

void Meta_Specification::Test_Child( const string& key )
{
   p_impl->impl_Test_Child( key );
}

Meta_Field& Meta_Specification::Test_Field( )
{
   return p_impl->impl_Test_Field( );
}

const Meta_Field& Meta_Specification::Test_Field( ) const
{
   return p_impl->impl_Test_Field( );
}

void Meta_Specification::Test_Field( const string& key )
{
   p_impl->impl_Test_Field( key );
}

Meta_Class& Meta_Specification::Test_Field_Class( )
{
   return p_impl->impl_Test_Field_Class( );
}

const Meta_Class& Meta_Specification::Test_Field_Class( ) const
{
   return p_impl->impl_Test_Field_Class( );
}

void Meta_Specification::Test_Field_Class( const string& key )
{
   p_impl->impl_Test_Field_Class( key );
}

Meta_Field& Meta_Specification::Test_Parent( )
{
   return p_impl->impl_Test_Parent( );
}

const Meta_Field& Meta_Specification::Test_Parent( ) const
{
   return p_impl->impl_Test_Parent( );
}

void Meta_Specification::Test_Parent( const string& key )
{
   p_impl->impl_Test_Parent( key );
}

Meta_Class& Meta_Specification::Test_Parent_Class( )
{
   return p_impl->impl_Test_Parent_Class( );
}

const Meta_Class& Meta_Specification::Test_Parent_Class( ) const
{
   return p_impl->impl_Test_Parent_Class( );
}

void Meta_Specification::Test_Parent_Class( const string& key )
{
   p_impl->impl_Test_Parent_Class( key );
}

Meta_Specification& Meta_Specification::child_Specification_Parent( )
{
   return p_impl->impl_child_Specification_Parent( );
}

const Meta_Specification& Meta_Specification::child_Specification_Parent( ) const
{
   return p_impl->impl_child_Specification_Parent( );
}

Meta_List_Field& Meta_Specification::child_List_Field_Restriction_Spec( )
{
   return p_impl->impl_child_List_Field_Restriction_Spec( );
}

const Meta_List_Field& Meta_Specification::child_List_Field_Restriction_Spec( ) const
{
   return p_impl->impl_child_List_Field_Restriction_Spec( );
}

Meta_View_Field& Meta_Specification::child_View_Field_Restriction_Spec( )
{
   return p_impl->impl_child_View_Field_Restriction_Spec( );
}

const Meta_View_Field& Meta_Specification::child_View_Field_Restriction_Spec( ) const
{
   return p_impl->impl_child_View_Field_Restriction_Spec( );
}

void Meta_Specification::Add_Arg_1( )
{
   p_impl->impl_Add_Arg_1( );
}

void Meta_Specification::Add_Arg_2( )
{
   p_impl->impl_Add_Arg_2( );
}

void Meta_Specification::Add_Child_Info( )
{
   p_impl->impl_Add_Child_Info( );
}

void Meta_Specification::Add_Destination_Info( )
{
   p_impl->impl_Add_Destination_Info( );
}

void Meta_Specification::Add_Extra_Info( )
{
   p_impl->impl_Add_Extra_Info( );
}

void Meta_Specification::Add_Field( )
{
   p_impl->impl_Add_Field( );
}

void Meta_Specification::Add_Field_Info( )
{
   p_impl->impl_Add_Field_Info( );
}

void Meta_Specification::Add_Field_Pair( )
{
   p_impl->impl_Add_Field_Pair( );
}

void Meta_Specification::Add_Grandchild_Info( )
{
   p_impl->impl_Add_Grandchild_Info( );
}

void Meta_Specification::Add_Info( )
{
   p_impl->impl_Add_Info( );
}

void Meta_Specification::Add_Next( )
{
   p_impl->impl_Add_Next( );
}

void Meta_Specification::Add_Opt_Info( )
{
   p_impl->impl_Add_Opt_Info( );
}

void Meta_Specification::Add_Other_Info( )
{
   p_impl->impl_Add_Other_Info( );
}

void Meta_Specification::Add_Secondary( )
{
   p_impl->impl_Add_Secondary( );
}

void Meta_Specification::Add_Source_Info( )
{
   p_impl->impl_Add_Source_Info( );
}

void Meta_Specification::Add_Stats_1( )
{
   p_impl->impl_Add_Stats_1( );
}

void Meta_Specification::Add_Stats_2( )
{
   p_impl->impl_Add_Stats_2( );
}

void Meta_Specification::Add_Stats_Info( )
{
   p_impl->impl_Add_Stats_Info( );
}

void Meta_Specification::Generate_All_Strings( string& All_Strings, const string& Parent_Prefix )
{
   p_impl->impl_Generate_All_Strings( All_Strings, Parent_Prefix );
}

void Meta_Specification::Generate_All_Vars( string& All_Vars, const string& Parent_Prefix )
{
   p_impl->impl_Generate_All_Vars( All_Vars, Parent_Prefix );
}

void Meta_Specification::Move_Down( const string& Restrict_Fields, const string& Restrict_Values )
{
   p_impl->impl_Move_Down( Restrict_Fields, Restrict_Values );
}

void Meta_Specification::Move_Up( const string& Restrict_Fields, const string& Restrict_Values )
{
   p_impl->impl_Move_Up( Restrict_Fields, Restrict_Values );
}

string Meta_Specification::get_field_value( int field ) const
{
   return p_impl->get_field_value( field );
}

void Meta_Specification::set_field_value( int field, const string& value )
{
   p_impl->set_field_value( field, value );
}

void Meta_Specification::set_field_default( int field )
{
   return set_field_default( ( field_id )( field + 1 ) );
}

void Meta_Specification::set_field_default( field_id id )
{
   p_impl->set_field_default( ( int )id - 1 );
}

void Meta_Specification::set_field_default( const string& field )
{
   p_impl->set_field_default( get_field_num( field ) );
}

bool Meta_Specification::is_field_default( int field ) const
{
   return is_field_default( ( field_id )( field + 1 ) );
}

bool Meta_Specification::is_field_default( field_id id ) const
{
   return p_impl->is_field_default( ( int )id - 1 );
}

bool Meta_Specification::is_field_default( const string& field ) const
{
   return p_impl->is_field_default( get_field_num( field ) );
}

bool Meta_Specification::is_field_encrypted( int field ) const
{
   return static_is_field_encrypted( ( field_id )( field + 1 ) );
}

bool Meta_Specification::is_field_transient( int field ) const
{
   return static_is_field_transient( ( field_id )( field + 1 ) );
}

string Meta_Specification::get_field_id( int field ) const
{
   return static_get_field_id( ( field_id )( field + 1 ) );
}

string Meta_Specification::get_field_name( int field ) const
{
   return static_get_field_name( ( field_id )( field + 1 ) );
}

int Meta_Specification::get_field_num( const string& field ) const
{
   int rc = static_get_field_num( field );

   if( rc < 0 )
      throw runtime_error( "unknown field name/id '" + field + "' in get_field_num( )" );

   return rc;
}

bool Meta_Specification::has_field_changed( const string& field ) const
{
   return class_base::has_field_changed( get_field_num( field ) );
}

uint64_t Meta_Specification::get_state( ) const
{
   uint64_t state = 0;

   state |= p_impl->get_state( );

   return state;
}

const string& Meta_Specification::execute( const string& cmd_and_args )
{
   return p_impl->execute( cmd_and_args );
}

void Meta_Specification::clear( )
{
   p_impl->clear( );
}

void Meta_Specification::validate( uint64_t state, bool is_internal )
{
   p_impl->validate( state, is_internal, &validation_errors );
}

void Meta_Specification::validate_set_fields( set< string >& fields_set )
{
   p_impl->validate_set_fields( fields_set, &validation_errors );
}

void Meta_Specification::after_fetch( )
{
   p_impl->after_fetch( );
}

void Meta_Specification::finalise_fetch( bool skip_set_original )
{
   p_impl->finalise_fetch( skip_set_original );
}

void Meta_Specification::at_create( )
{
   p_impl->at_create( );
}

void Meta_Specification::post_init( )
{
   p_impl->post_init( );
}

void Meta_Specification::to_store( bool is_create, bool is_internal )
{
   p_impl->to_store( is_create, is_internal );
}

void Meta_Specification::for_store( bool is_create, bool is_internal )
{
   p_impl->for_store( is_create, is_internal );
}

void Meta_Specification::after_store( bool is_create, bool is_internal )
{
   p_impl->after_store( is_create, is_internal );
}

bool Meta_Specification::can_destroy( bool is_internal )
{
   return p_impl->can_destroy( is_internal );
}

void Meta_Specification::for_destroy( bool is_internal )
{
   p_impl->for_destroy( is_internal );
}

void Meta_Specification::after_destroy( bool is_internal )
{
   p_impl->after_destroy( is_internal );
}

void Meta_Specification::set_default_values( )
{
   p_impl->set_default_values( );
}

bool Meta_Specification::is_filtered( ) const
{
   return p_impl->is_filtered( );
}

const char* Meta_Specification::get_field_id(
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
   else if( name == c_field_name_All_Strings )
   {
      p_id = c_field_id_All_Strings;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_All_Vars )
   {
      p_id = c_field_id_All_Vars;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Can_Access_Below_Model )
   {
      p_id = c_field_id_Can_Access_Below_Model;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Child_Rel_Child_Class )
   {
      p_id = c_field_id_Child_Rel_Child_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Child_Rel_Field_Key )
   {
      p_id = c_field_id_Child_Rel_Field_Key;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Child_Relationship )
   {
      p_id = c_field_id_Child_Relationship;

      if( p_type_name )
         *p_type_name = "Meta_Relationship";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Child_Specification_Type )
   {
      p_id = c_field_id_Child_Specification_Type;

      if( p_type_name )
         *p_type_name = "Meta_Specification_Type";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Class )
   {
      p_id = c_field_id_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Comments )
   {
      p_id = c_field_id_Comments;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Enum )
   {
      p_id = c_field_id_Enum;

      if( p_type_name )
         *p_type_name = "Meta_Enum";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Enum_Item )
   {
      p_id = c_field_id_Enum_Item;

      if( p_type_name )
         *p_type_name = "Meta_Enum_Item";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Enum_Item_2 )
   {
      p_id = c_field_id_Enum_Item_2;

      if( p_type_name )
         *p_type_name = "Meta_Enum_Item";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Enum_Item_3 )
   {
      p_id = c_field_id_Enum_Item_3;

      if( p_type_name )
         *p_type_name = "Meta_Enum_Item";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Enum_Item_4 )
   {
      p_id = c_field_id_Enum_Item_4;

      if( p_type_name )
         *p_type_name = "Meta_Enum_Item";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Enum_Item_5 )
   {
      p_id = c_field_id_Enum_Item_5;

      if( p_type_name )
         *p_type_name = "Meta_Enum_Item";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Field )
   {
      p_id = c_field_id_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Field_Class )
   {
      p_id = c_field_id_Field_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Field_Pairs )
   {
      p_id = c_field_id_Field_Pairs;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Field_Values )
   {
      p_id = c_field_id_Field_Values;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Fields )
   {
      p_id = c_field_id_Fields;

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
   else if( name == c_field_name_Is_Required_For_UI_Gen )
   {
      p_id = c_field_id_Is_Required_For_UI_Gen;

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
   else if( name == c_field_name_Modifier )
   {
      p_id = c_field_id_Modifier;

      if( p_type_name )
         *p_type_name = "Meta_Modifier";

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
   else if( name == c_field_name_Options )
   {
      p_id = c_field_id_Options;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Order )
   {
      p_id = c_field_id_Order;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Other_Class )
   {
      p_id = c_field_id_Other_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Other_Class_Field )
   {
      p_id = c_field_id_Other_Class_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Other_Field )
   {
      p_id = c_field_id_Other_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Other_Field_2 )
   {
      p_id = c_field_id_Other_Field_2;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Other_Modifier )
   {
      p_id = c_field_id_Other_Modifier;

      if( p_type_name )
         *p_type_name = "Meta_Modifier";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Other_Modifier_2 )
   {
      p_id = c_field_id_Other_Modifier_2;

      if( p_type_name )
         *p_type_name = "Meta_Modifier";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Other_Permission )
   {
      p_id = c_field_id_Other_Permission;

      if( p_type_name )
         *p_type_name = "Meta_Permission";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Other_Permission_2 )
   {
      p_id = c_field_id_Other_Permission_2;

      if( p_type_name )
         *p_type_name = "Meta_Permission";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Other_Procedure )
   {
      p_id = c_field_id_Other_Procedure;

      if( p_type_name )
         *p_type_name = "Meta_Procedure";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Other_Procedure_2 )
   {
      p_id = c_field_id_Other_Procedure_2;

      if( p_type_name )
         *p_type_name = "Meta_Procedure";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Other_Source_Child )
   {
      p_id = c_field_id_Other_Source_Child;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Other_Source_Child_2 )
   {
      p_id = c_field_id_Other_Source_Child_2;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Parent_Specification )
   {
      p_id = c_field_id_Parent_Specification;

      if( p_type_name )
         *p_type_name = "Meta_Specification";

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
   else if( name == c_field_name_Procedure )
   {
      p_id = c_field_id_Procedure;

      if( p_type_name )
         *p_type_name = "Meta_Procedure";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Procedure_Arg )
   {
      p_id = c_field_id_Procedure_Arg;

      if( p_type_name )
         *p_type_name = "Meta_Procedure_Arg";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Procedure_Arg_2 )
   {
      p_id = c_field_id_Procedure_Arg_2;

      if( p_type_name )
         *p_type_name = "Meta_Procedure_Arg";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Procedure_Arg_3 )
   {
      p_id = c_field_id_Procedure_Arg_3;

      if( p_type_name )
         *p_type_name = "Meta_Procedure_Arg";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Protect_Child_Rel )
   {
      p_id = c_field_id_Protect_Child_Rel;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Protect_Procedure )
   {
      p_id = c_field_id_Protect_Procedure;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Protect_Source_Parent )
   {
      p_id = c_field_id_Protect_Source_Parent;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Restrict_Values )
   {
      p_id = c_field_id_Restrict_Values;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Source_Child )
   {
      p_id = c_field_id_Source_Child;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Source_Child_Class )
   {
      p_id = c_field_id_Source_Child_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

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
   else if( name == c_field_name_Source_Field )
   {
      p_id = c_field_id_Source_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Source_Field_Class )
   {
      p_id = c_field_id_Source_Field_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Source_Grandchild )
   {
      p_id = c_field_id_Source_Grandchild;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Source_Parent )
   {
      p_id = c_field_id_Source_Parent;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Source_Parent_Class )
   {
      p_id = c_field_id_Source_Parent_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Specification_Type )
   {
      p_id = c_field_id_Specification_Type;

      if( p_type_name )
         *p_type_name = "Meta_Specification_Type";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Strings )
   {
      p_id = c_field_id_Strings;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Test_Child )
   {
      p_id = c_field_id_Test_Child;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Test_Field )
   {
      p_id = c_field_id_Test_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Test_Field_Class )
   {
      p_id = c_field_id_Test_Field_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Test_Parent )
   {
      p_id = c_field_id_Test_Parent;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Test_Parent_Class )
   {
      p_id = c_field_id_Test_Parent_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Test_Value )
   {
      p_id = c_field_id_Test_Value;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Test_Value_Label )
   {
      p_id = c_field_id_Test_Value_Label;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Use_Source_Parent )
   {
      p_id = c_field_id_Use_Source_Parent;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Use_Test_Parent_Child )
   {
      p_id = c_field_id_Use_Test_Parent_Child;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Value )
   {
      p_id = c_field_id_Value;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Value_Label )
   {
      p_id = c_field_id_Value_Label;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Value_Left_Part )
   {
      p_id = c_field_id_Value_Left_Part;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Value_Literal )
   {
      p_id = c_field_id_Value_Literal;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Value_Numeric_String )
   {
      p_id = c_field_id_Value_Numeric_String;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Value_Right_Part )
   {
      p_id = c_field_id_Value_Right_Part;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Value_String )
   {
      p_id = c_field_id_Value_String;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Vars )
   {
      p_id = c_field_id_Vars;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }

   return p_id;
}

const char* Meta_Specification::get_field_name(
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
   else if( id == c_field_id_All_Strings )
   {
      p_name = c_field_name_All_Strings;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_All_Vars )
   {
      p_name = c_field_name_All_Vars;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Can_Access_Below_Model )
   {
      p_name = c_field_name_Can_Access_Below_Model;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Child_Rel_Child_Class )
   {
      p_name = c_field_name_Child_Rel_Child_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Child_Rel_Field_Key )
   {
      p_name = c_field_name_Child_Rel_Field_Key;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Child_Relationship )
   {
      p_name = c_field_name_Child_Relationship;

      if( p_type_name )
         *p_type_name = "Meta_Relationship";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Child_Specification_Type )
   {
      p_name = c_field_name_Child_Specification_Type;

      if( p_type_name )
         *p_type_name = "Meta_Specification_Type";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Class )
   {
      p_name = c_field_name_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Comments )
   {
      p_name = c_field_name_Comments;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Enum )
   {
      p_name = c_field_name_Enum;

      if( p_type_name )
         *p_type_name = "Meta_Enum";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Enum_Item )
   {
      p_name = c_field_name_Enum_Item;

      if( p_type_name )
         *p_type_name = "Meta_Enum_Item";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Enum_Item_2 )
   {
      p_name = c_field_name_Enum_Item_2;

      if( p_type_name )
         *p_type_name = "Meta_Enum_Item";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Enum_Item_3 )
   {
      p_name = c_field_name_Enum_Item_3;

      if( p_type_name )
         *p_type_name = "Meta_Enum_Item";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Enum_Item_4 )
   {
      p_name = c_field_name_Enum_Item_4;

      if( p_type_name )
         *p_type_name = "Meta_Enum_Item";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Enum_Item_5 )
   {
      p_name = c_field_name_Enum_Item_5;

      if( p_type_name )
         *p_type_name = "Meta_Enum_Item";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Field )
   {
      p_name = c_field_name_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Field_Class )
   {
      p_name = c_field_name_Field_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Field_Pairs )
   {
      p_name = c_field_name_Field_Pairs;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Field_Values )
   {
      p_name = c_field_name_Field_Values;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Fields )
   {
      p_name = c_field_name_Fields;

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
   else if( id == c_field_id_Is_Required_For_UI_Gen )
   {
      p_name = c_field_name_Is_Required_For_UI_Gen;

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
   else if( id == c_field_id_Modifier )
   {
      p_name = c_field_name_Modifier;

      if( p_type_name )
         *p_type_name = "Meta_Modifier";

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
   else if( id == c_field_id_Options )
   {
      p_name = c_field_name_Options;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Order )
   {
      p_name = c_field_name_Order;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Other_Class )
   {
      p_name = c_field_name_Other_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Other_Class_Field )
   {
      p_name = c_field_name_Other_Class_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Other_Field )
   {
      p_name = c_field_name_Other_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Other_Field_2 )
   {
      p_name = c_field_name_Other_Field_2;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Other_Modifier )
   {
      p_name = c_field_name_Other_Modifier;

      if( p_type_name )
         *p_type_name = "Meta_Modifier";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Other_Modifier_2 )
   {
      p_name = c_field_name_Other_Modifier_2;

      if( p_type_name )
         *p_type_name = "Meta_Modifier";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Other_Permission )
   {
      p_name = c_field_name_Other_Permission;

      if( p_type_name )
         *p_type_name = "Meta_Permission";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Other_Permission_2 )
   {
      p_name = c_field_name_Other_Permission_2;

      if( p_type_name )
         *p_type_name = "Meta_Permission";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Other_Procedure )
   {
      p_name = c_field_name_Other_Procedure;

      if( p_type_name )
         *p_type_name = "Meta_Procedure";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Other_Procedure_2 )
   {
      p_name = c_field_name_Other_Procedure_2;

      if( p_type_name )
         *p_type_name = "Meta_Procedure";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Other_Source_Child )
   {
      p_name = c_field_name_Other_Source_Child;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Other_Source_Child_2 )
   {
      p_name = c_field_name_Other_Source_Child_2;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Parent_Specification )
   {
      p_name = c_field_name_Parent_Specification;

      if( p_type_name )
         *p_type_name = "Meta_Specification";

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
   else if( id == c_field_id_Procedure )
   {
      p_name = c_field_name_Procedure;

      if( p_type_name )
         *p_type_name = "Meta_Procedure";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Procedure_Arg )
   {
      p_name = c_field_name_Procedure_Arg;

      if( p_type_name )
         *p_type_name = "Meta_Procedure_Arg";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Procedure_Arg_2 )
   {
      p_name = c_field_name_Procedure_Arg_2;

      if( p_type_name )
         *p_type_name = "Meta_Procedure_Arg";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Procedure_Arg_3 )
   {
      p_name = c_field_name_Procedure_Arg_3;

      if( p_type_name )
         *p_type_name = "Meta_Procedure_Arg";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Protect_Child_Rel )
   {
      p_name = c_field_name_Protect_Child_Rel;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Protect_Procedure )
   {
      p_name = c_field_name_Protect_Procedure;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Protect_Source_Parent )
   {
      p_name = c_field_name_Protect_Source_Parent;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Restrict_Values )
   {
      p_name = c_field_name_Restrict_Values;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Source_Child )
   {
      p_name = c_field_name_Source_Child;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Source_Child_Class )
   {
      p_name = c_field_name_Source_Child_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

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
   else if( id == c_field_id_Source_Field )
   {
      p_name = c_field_name_Source_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Source_Field_Class )
   {
      p_name = c_field_name_Source_Field_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Source_Grandchild )
   {
      p_name = c_field_name_Source_Grandchild;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Source_Parent )
   {
      p_name = c_field_name_Source_Parent;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Source_Parent_Class )
   {
      p_name = c_field_name_Source_Parent_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Specification_Type )
   {
      p_name = c_field_name_Specification_Type;

      if( p_type_name )
         *p_type_name = "Meta_Specification_Type";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Strings )
   {
      p_name = c_field_name_Strings;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Test_Child )
   {
      p_name = c_field_name_Test_Child;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Test_Field )
   {
      p_name = c_field_name_Test_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Test_Field_Class )
   {
      p_name = c_field_name_Test_Field_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Test_Parent )
   {
      p_name = c_field_name_Test_Parent;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Test_Parent_Class )
   {
      p_name = c_field_name_Test_Parent_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Test_Value )
   {
      p_name = c_field_name_Test_Value;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Test_Value_Label )
   {
      p_name = c_field_name_Test_Value_Label;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Use_Source_Parent )
   {
      p_name = c_field_name_Use_Source_Parent;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Use_Test_Parent_Child )
   {
      p_name = c_field_name_Use_Test_Parent_Child;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Value )
   {
      p_name = c_field_name_Value;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Value_Label )
   {
      p_name = c_field_name_Value_Label;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Value_Left_Part )
   {
      p_name = c_field_name_Value_Left_Part;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Value_Literal )
   {
      p_name = c_field_name_Value_Literal;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Value_Numeric_String )
   {
      p_name = c_field_name_Value_Numeric_String;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Value_Right_Part )
   {
      p_name = c_field_name_Value_Right_Part;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Value_String )
   {
      p_name = c_field_name_Value_String;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Vars )
   {
      p_name = c_field_name_Vars;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }

   return p_name;
}

string& Meta_Specification::get_group_field_name( ) const
{
   return g_group_field_name;
}

string& Meta_Specification::get_level_field_name( ) const
{
   return g_level_field_name;
}

string& Meta_Specification::get_order_field_name( ) const
{
   return g_order_field_name;
}

string& Meta_Specification::get_owner_field_name( ) const
{
   return g_owner_field_name;
}

bool Meta_Specification::is_file_field( const string& id_or_name ) const
{
   return g_file_field_ids.count( id_or_name ) || g_file_field_names.count( id_or_name );
}

void Meta_Specification::get_file_field_names( vector< string >& file_field_names ) const
{
   for( set< string >::const_iterator ci = g_file_field_names.begin( ); ci != g_file_field_names.end( ); ++ci )
      file_field_names.push_back( *ci );
}

string Meta_Specification::get_field_uom_symbol( const string& id_or_name ) const
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
   if( ( id_or_name == c_field_id_All_Strings ) || ( id_or_name == c_field_name_All_Strings ) )
   {
      name = string( c_field_display_name_All_Strings );
      get_module_string( c_field_display_name_All_Strings, &next );
   }
   if( ( id_or_name == c_field_id_All_Vars ) || ( id_or_name == c_field_name_All_Vars ) )
   {
      name = string( c_field_display_name_All_Vars );
      get_module_string( c_field_display_name_All_Vars, &next );
   }
   if( ( id_or_name == c_field_id_Can_Access_Below_Model ) || ( id_or_name == c_field_name_Can_Access_Below_Model ) )
   {
      name = string( c_field_display_name_Can_Access_Below_Model );
      get_module_string( c_field_display_name_Can_Access_Below_Model, &next );
   }
   if( ( id_or_name == c_field_id_Child_Rel_Child_Class ) || ( id_or_name == c_field_name_Child_Rel_Child_Class ) )
   {
      name = string( c_field_display_name_Child_Rel_Child_Class );
      get_module_string( c_field_display_name_Child_Rel_Child_Class, &next );
   }
   if( ( id_or_name == c_field_id_Child_Rel_Field_Key ) || ( id_or_name == c_field_name_Child_Rel_Field_Key ) )
   {
      name = string( c_field_display_name_Child_Rel_Field_Key );
      get_module_string( c_field_display_name_Child_Rel_Field_Key, &next );
   }
   if( ( id_or_name == c_field_id_Child_Relationship ) || ( id_or_name == c_field_name_Child_Relationship ) )
   {
      name = string( c_field_display_name_Child_Relationship );
      get_module_string( c_field_display_name_Child_Relationship, &next );
   }
   if( ( id_or_name == c_field_id_Child_Specification_Type ) || ( id_or_name == c_field_name_Child_Specification_Type ) )
   {
      name = string( c_field_display_name_Child_Specification_Type );
      get_module_string( c_field_display_name_Child_Specification_Type, &next );
   }
   if( ( id_or_name == c_field_id_Class ) || ( id_or_name == c_field_name_Class ) )
   {
      name = string( c_field_display_name_Class );
      get_module_string( c_field_display_name_Class, &next );
   }
   if( ( id_or_name == c_field_id_Comments ) || ( id_or_name == c_field_name_Comments ) )
   {
      name = string( c_field_display_name_Comments );
      get_module_string( c_field_display_name_Comments, &next );
   }
   if( ( id_or_name == c_field_id_Enum ) || ( id_or_name == c_field_name_Enum ) )
   {
      name = string( c_field_display_name_Enum );
      get_module_string( c_field_display_name_Enum, &next );
   }
   if( ( id_or_name == c_field_id_Enum_Item ) || ( id_or_name == c_field_name_Enum_Item ) )
   {
      name = string( c_field_display_name_Enum_Item );
      get_module_string( c_field_display_name_Enum_Item, &next );
   }
   if( ( id_or_name == c_field_id_Enum_Item_2 ) || ( id_or_name == c_field_name_Enum_Item_2 ) )
   {
      name = string( c_field_display_name_Enum_Item_2 );
      get_module_string( c_field_display_name_Enum_Item_2, &next );
   }
   if( ( id_or_name == c_field_id_Enum_Item_3 ) || ( id_or_name == c_field_name_Enum_Item_3 ) )
   {
      name = string( c_field_display_name_Enum_Item_3 );
      get_module_string( c_field_display_name_Enum_Item_3, &next );
   }
   if( ( id_or_name == c_field_id_Enum_Item_4 ) || ( id_or_name == c_field_name_Enum_Item_4 ) )
   {
      name = string( c_field_display_name_Enum_Item_4 );
      get_module_string( c_field_display_name_Enum_Item_4, &next );
   }
   if( ( id_or_name == c_field_id_Enum_Item_5 ) || ( id_or_name == c_field_name_Enum_Item_5 ) )
   {
      name = string( c_field_display_name_Enum_Item_5 );
      get_module_string( c_field_display_name_Enum_Item_5, &next );
   }
   if( ( id_or_name == c_field_id_Field ) || ( id_or_name == c_field_name_Field ) )
   {
      name = string( c_field_display_name_Field );
      get_module_string( c_field_display_name_Field, &next );
   }
   if( ( id_or_name == c_field_id_Field_Class ) || ( id_or_name == c_field_name_Field_Class ) )
   {
      name = string( c_field_display_name_Field_Class );
      get_module_string( c_field_display_name_Field_Class, &next );
   }
   if( ( id_or_name == c_field_id_Field_Pairs ) || ( id_or_name == c_field_name_Field_Pairs ) )
   {
      name = string( c_field_display_name_Field_Pairs );
      get_module_string( c_field_display_name_Field_Pairs, &next );
   }
   if( ( id_or_name == c_field_id_Field_Values ) || ( id_or_name == c_field_name_Field_Values ) )
   {
      name = string( c_field_display_name_Field_Values );
      get_module_string( c_field_display_name_Field_Values, &next );
   }
   if( ( id_or_name == c_field_id_Fields ) || ( id_or_name == c_field_name_Fields ) )
   {
      name = string( c_field_display_name_Fields );
      get_module_string( c_field_display_name_Fields, &next );
   }
   if( ( id_or_name == c_field_id_Id ) || ( id_or_name == c_field_name_Id ) )
   {
      name = string( c_field_display_name_Id );
      get_module_string( c_field_display_name_Id, &next );
   }
   if( ( id_or_name == c_field_id_Is_Required_For_UI_Gen ) || ( id_or_name == c_field_name_Is_Required_For_UI_Gen ) )
   {
      name = string( c_field_display_name_Is_Required_For_UI_Gen );
      get_module_string( c_field_display_name_Is_Required_For_UI_Gen, &next );
   }
   if( ( id_or_name == c_field_id_Model ) || ( id_or_name == c_field_name_Model ) )
   {
      name = string( c_field_display_name_Model );
      get_module_string( c_field_display_name_Model, &next );
   }
   if( ( id_or_name == c_field_id_Modifier ) || ( id_or_name == c_field_name_Modifier ) )
   {
      name = string( c_field_display_name_Modifier );
      get_module_string( c_field_display_name_Modifier, &next );
   }
   if( ( id_or_name == c_field_id_Name ) || ( id_or_name == c_field_name_Name ) )
   {
      name = string( c_field_display_name_Name );
      get_module_string( c_field_display_name_Name, &next );
   }
   if( ( id_or_name == c_field_id_Options ) || ( id_or_name == c_field_name_Options ) )
   {
      name = string( c_field_display_name_Options );
      get_module_string( c_field_display_name_Options, &next );
   }
   if( ( id_or_name == c_field_id_Order ) || ( id_or_name == c_field_name_Order ) )
   {
      name = string( c_field_display_name_Order );
      get_module_string( c_field_display_name_Order, &next );
   }
   if( ( id_or_name == c_field_id_Other_Class ) || ( id_or_name == c_field_name_Other_Class ) )
   {
      name = string( c_field_display_name_Other_Class );
      get_module_string( c_field_display_name_Other_Class, &next );
   }
   if( ( id_or_name == c_field_id_Other_Class_Field ) || ( id_or_name == c_field_name_Other_Class_Field ) )
   {
      name = string( c_field_display_name_Other_Class_Field );
      get_module_string( c_field_display_name_Other_Class_Field, &next );
   }
   if( ( id_or_name == c_field_id_Other_Field ) || ( id_or_name == c_field_name_Other_Field ) )
   {
      name = string( c_field_display_name_Other_Field );
      get_module_string( c_field_display_name_Other_Field, &next );
   }
   if( ( id_or_name == c_field_id_Other_Field_2 ) || ( id_or_name == c_field_name_Other_Field_2 ) )
   {
      name = string( c_field_display_name_Other_Field_2 );
      get_module_string( c_field_display_name_Other_Field_2, &next );
   }
   if( ( id_or_name == c_field_id_Other_Modifier ) || ( id_or_name == c_field_name_Other_Modifier ) )
   {
      name = string( c_field_display_name_Other_Modifier );
      get_module_string( c_field_display_name_Other_Modifier, &next );
   }
   if( ( id_or_name == c_field_id_Other_Modifier_2 ) || ( id_or_name == c_field_name_Other_Modifier_2 ) )
   {
      name = string( c_field_display_name_Other_Modifier_2 );
      get_module_string( c_field_display_name_Other_Modifier_2, &next );
   }
   if( ( id_or_name == c_field_id_Other_Permission ) || ( id_or_name == c_field_name_Other_Permission ) )
   {
      name = string( c_field_display_name_Other_Permission );
      get_module_string( c_field_display_name_Other_Permission, &next );
   }
   if( ( id_or_name == c_field_id_Other_Permission_2 ) || ( id_or_name == c_field_name_Other_Permission_2 ) )
   {
      name = string( c_field_display_name_Other_Permission_2 );
      get_module_string( c_field_display_name_Other_Permission_2, &next );
   }
   if( ( id_or_name == c_field_id_Other_Procedure ) || ( id_or_name == c_field_name_Other_Procedure ) )
   {
      name = string( c_field_display_name_Other_Procedure );
      get_module_string( c_field_display_name_Other_Procedure, &next );
   }
   if( ( id_or_name == c_field_id_Other_Procedure_2 ) || ( id_or_name == c_field_name_Other_Procedure_2 ) )
   {
      name = string( c_field_display_name_Other_Procedure_2 );
      get_module_string( c_field_display_name_Other_Procedure_2, &next );
   }
   if( ( id_or_name == c_field_id_Other_Source_Child ) || ( id_or_name == c_field_name_Other_Source_Child ) )
   {
      name = string( c_field_display_name_Other_Source_Child );
      get_module_string( c_field_display_name_Other_Source_Child, &next );
   }
   if( ( id_or_name == c_field_id_Other_Source_Child_2 ) || ( id_or_name == c_field_name_Other_Source_Child_2 ) )
   {
      name = string( c_field_display_name_Other_Source_Child_2 );
      get_module_string( c_field_display_name_Other_Source_Child_2, &next );
   }
   if( ( id_or_name == c_field_id_Parent_Specification ) || ( id_or_name == c_field_name_Parent_Specification ) )
   {
      name = string( c_field_display_name_Parent_Specification );
      get_module_string( c_field_display_name_Parent_Specification, &next );
   }
   if( ( id_or_name == c_field_id_Permission ) || ( id_or_name == c_field_name_Permission ) )
   {
      name = string( c_field_display_name_Permission );
      get_module_string( c_field_display_name_Permission, &next );
   }
   if( ( id_or_name == c_field_id_Procedure ) || ( id_or_name == c_field_name_Procedure ) )
   {
      name = string( c_field_display_name_Procedure );
      get_module_string( c_field_display_name_Procedure, &next );
   }
   if( ( id_or_name == c_field_id_Procedure_Arg ) || ( id_or_name == c_field_name_Procedure_Arg ) )
   {
      name = string( c_field_display_name_Procedure_Arg );
      get_module_string( c_field_display_name_Procedure_Arg, &next );
   }
   if( ( id_or_name == c_field_id_Procedure_Arg_2 ) || ( id_or_name == c_field_name_Procedure_Arg_2 ) )
   {
      name = string( c_field_display_name_Procedure_Arg_2 );
      get_module_string( c_field_display_name_Procedure_Arg_2, &next );
   }
   if( ( id_or_name == c_field_id_Procedure_Arg_3 ) || ( id_or_name == c_field_name_Procedure_Arg_3 ) )
   {
      name = string( c_field_display_name_Procedure_Arg_3 );
      get_module_string( c_field_display_name_Procedure_Arg_3, &next );
   }
   if( ( id_or_name == c_field_id_Protect_Child_Rel ) || ( id_or_name == c_field_name_Protect_Child_Rel ) )
   {
      name = string( c_field_display_name_Protect_Child_Rel );
      get_module_string( c_field_display_name_Protect_Child_Rel, &next );
   }
   if( ( id_or_name == c_field_id_Protect_Procedure ) || ( id_or_name == c_field_name_Protect_Procedure ) )
   {
      name = string( c_field_display_name_Protect_Procedure );
      get_module_string( c_field_display_name_Protect_Procedure, &next );
   }
   if( ( id_or_name == c_field_id_Protect_Source_Parent ) || ( id_or_name == c_field_name_Protect_Source_Parent ) )
   {
      name = string( c_field_display_name_Protect_Source_Parent );
      get_module_string( c_field_display_name_Protect_Source_Parent, &next );
   }
   if( ( id_or_name == c_field_id_Restrict_Values ) || ( id_or_name == c_field_name_Restrict_Values ) )
   {
      name = string( c_field_display_name_Restrict_Values );
      get_module_string( c_field_display_name_Restrict_Values, &next );
   }
   if( ( id_or_name == c_field_id_Source_Child ) || ( id_or_name == c_field_name_Source_Child ) )
   {
      name = string( c_field_display_name_Source_Child );
      get_module_string( c_field_display_name_Source_Child, &next );
   }
   if( ( id_or_name == c_field_id_Source_Child_Class ) || ( id_or_name == c_field_name_Source_Child_Class ) )
   {
      name = string( c_field_display_name_Source_Child_Class );
      get_module_string( c_field_display_name_Source_Child_Class, &next );
   }
   if( ( id_or_name == c_field_id_Source_Class ) || ( id_or_name == c_field_name_Source_Class ) )
   {
      name = string( c_field_display_name_Source_Class );
      get_module_string( c_field_display_name_Source_Class, &next );
   }
   if( ( id_or_name == c_field_id_Source_Field ) || ( id_or_name == c_field_name_Source_Field ) )
   {
      name = string( c_field_display_name_Source_Field );
      get_module_string( c_field_display_name_Source_Field, &next );
   }
   if( ( id_or_name == c_field_id_Source_Field_Class ) || ( id_or_name == c_field_name_Source_Field_Class ) )
   {
      name = string( c_field_display_name_Source_Field_Class );
      get_module_string( c_field_display_name_Source_Field_Class, &next );
   }
   if( ( id_or_name == c_field_id_Source_Grandchild ) || ( id_or_name == c_field_name_Source_Grandchild ) )
   {
      name = string( c_field_display_name_Source_Grandchild );
      get_module_string( c_field_display_name_Source_Grandchild, &next );
   }
   if( ( id_or_name == c_field_id_Source_Parent ) || ( id_or_name == c_field_name_Source_Parent ) )
   {
      name = string( c_field_display_name_Source_Parent );
      get_module_string( c_field_display_name_Source_Parent, &next );
   }
   if( ( id_or_name == c_field_id_Source_Parent_Class ) || ( id_or_name == c_field_name_Source_Parent_Class ) )
   {
      name = string( c_field_display_name_Source_Parent_Class );
      get_module_string( c_field_display_name_Source_Parent_Class, &next );
   }
   if( ( id_or_name == c_field_id_Specification_Type ) || ( id_or_name == c_field_name_Specification_Type ) )
   {
      name = string( c_field_display_name_Specification_Type );
      get_module_string( c_field_display_name_Specification_Type, &next );
   }
   if( ( id_or_name == c_field_id_Strings ) || ( id_or_name == c_field_name_Strings ) )
   {
      name = string( c_field_display_name_Strings );
      get_module_string( c_field_display_name_Strings, &next );
   }
   if( ( id_or_name == c_field_id_Test_Child ) || ( id_or_name == c_field_name_Test_Child ) )
   {
      name = string( c_field_display_name_Test_Child );
      get_module_string( c_field_display_name_Test_Child, &next );
   }
   if( ( id_or_name == c_field_id_Test_Field ) || ( id_or_name == c_field_name_Test_Field ) )
   {
      name = string( c_field_display_name_Test_Field );
      get_module_string( c_field_display_name_Test_Field, &next );
   }
   if( ( id_or_name == c_field_id_Test_Field_Class ) || ( id_or_name == c_field_name_Test_Field_Class ) )
   {
      name = string( c_field_display_name_Test_Field_Class );
      get_module_string( c_field_display_name_Test_Field_Class, &next );
   }
   if( ( id_or_name == c_field_id_Test_Parent ) || ( id_or_name == c_field_name_Test_Parent ) )
   {
      name = string( c_field_display_name_Test_Parent );
      get_module_string( c_field_display_name_Test_Parent, &next );
   }
   if( ( id_or_name == c_field_id_Test_Parent_Class ) || ( id_or_name == c_field_name_Test_Parent_Class ) )
   {
      name = string( c_field_display_name_Test_Parent_Class );
      get_module_string( c_field_display_name_Test_Parent_Class, &next );
   }
   if( ( id_or_name == c_field_id_Test_Value ) || ( id_or_name == c_field_name_Test_Value ) )
   {
      name = string( c_field_display_name_Test_Value );
      get_module_string( c_field_display_name_Test_Value, &next );
   }
   if( ( id_or_name == c_field_id_Test_Value_Label ) || ( id_or_name == c_field_name_Test_Value_Label ) )
   {
      name = string( c_field_display_name_Test_Value_Label );
      get_module_string( c_field_display_name_Test_Value_Label, &next );
   }
   if( ( id_or_name == c_field_id_Use_Source_Parent ) || ( id_or_name == c_field_name_Use_Source_Parent ) )
   {
      name = string( c_field_display_name_Use_Source_Parent );
      get_module_string( c_field_display_name_Use_Source_Parent, &next );
   }
   if( ( id_or_name == c_field_id_Use_Test_Parent_Child ) || ( id_or_name == c_field_name_Use_Test_Parent_Child ) )
   {
      name = string( c_field_display_name_Use_Test_Parent_Child );
      get_module_string( c_field_display_name_Use_Test_Parent_Child, &next );
   }
   if( ( id_or_name == c_field_id_Value ) || ( id_or_name == c_field_name_Value ) )
   {
      name = string( c_field_display_name_Value );
      get_module_string( c_field_display_name_Value, &next );
   }
   if( ( id_or_name == c_field_id_Value_Label ) || ( id_or_name == c_field_name_Value_Label ) )
   {
      name = string( c_field_display_name_Value_Label );
      get_module_string( c_field_display_name_Value_Label, &next );
   }
   if( ( id_or_name == c_field_id_Value_Left_Part ) || ( id_or_name == c_field_name_Value_Left_Part ) )
   {
      name = string( c_field_display_name_Value_Left_Part );
      get_module_string( c_field_display_name_Value_Left_Part, &next );
   }
   if( ( id_or_name == c_field_id_Value_Literal ) || ( id_or_name == c_field_name_Value_Literal ) )
   {
      name = string( c_field_display_name_Value_Literal );
      get_module_string( c_field_display_name_Value_Literal, &next );
   }
   if( ( id_or_name == c_field_id_Value_Numeric_String ) || ( id_or_name == c_field_name_Value_Numeric_String ) )
   {
      name = string( c_field_display_name_Value_Numeric_String );
      get_module_string( c_field_display_name_Value_Numeric_String, &next );
   }
   if( ( id_or_name == c_field_id_Value_Right_Part ) || ( id_or_name == c_field_name_Value_Right_Part ) )
   {
      name = string( c_field_display_name_Value_Right_Part );
      get_module_string( c_field_display_name_Value_Right_Part, &next );
   }
   if( ( id_or_name == c_field_id_Value_String ) || ( id_or_name == c_field_name_Value_String ) )
   {
      name = string( c_field_display_name_Value_String );
      get_module_string( c_field_display_name_Value_String, &next );
   }
   if( ( id_or_name == c_field_id_Vars ) || ( id_or_name == c_field_name_Vars ) )
   {
      name = string( c_field_display_name_Vars );
      get_module_string( c_field_display_name_Vars, &next );
   }

   // NOTE: It is being assumed here that the customised UOM symbol for a field (if it
   // has one) will be in the module string that immediately follows that of its name.
   if( next.first.find( name + "_(" ) == 0 )
      uom_symbol = next.second;

   return uom_symbol;
}

string Meta_Specification::get_field_display_name( const string& id_or_name ) const
{
   string display_name;

   if( id_or_name.empty( ) )
      throw runtime_error( "unexpected empty field id_or_name for get_field_display_name" );
   if( ( id_or_name == c_field_id_Actions ) || ( id_or_name == c_field_name_Actions ) )
      display_name = get_module_string( c_field_display_name_Actions );
   if( ( id_or_name == c_field_id_All_Strings ) || ( id_or_name == c_field_name_All_Strings ) )
      display_name = get_module_string( c_field_display_name_All_Strings );
   if( ( id_or_name == c_field_id_All_Vars ) || ( id_or_name == c_field_name_All_Vars ) )
      display_name = get_module_string( c_field_display_name_All_Vars );
   if( ( id_or_name == c_field_id_Can_Access_Below_Model ) || ( id_or_name == c_field_name_Can_Access_Below_Model ) )
      display_name = get_module_string( c_field_display_name_Can_Access_Below_Model );
   if( ( id_or_name == c_field_id_Child_Rel_Child_Class ) || ( id_or_name == c_field_name_Child_Rel_Child_Class ) )
      display_name = get_module_string( c_field_display_name_Child_Rel_Child_Class );
   if( ( id_or_name == c_field_id_Child_Rel_Field_Key ) || ( id_or_name == c_field_name_Child_Rel_Field_Key ) )
      display_name = get_module_string( c_field_display_name_Child_Rel_Field_Key );
   if( ( id_or_name == c_field_id_Child_Relationship ) || ( id_or_name == c_field_name_Child_Relationship ) )
      display_name = get_module_string( c_field_display_name_Child_Relationship );
   if( ( id_or_name == c_field_id_Child_Specification_Type ) || ( id_or_name == c_field_name_Child_Specification_Type ) )
      display_name = get_module_string( c_field_display_name_Child_Specification_Type );
   if( ( id_or_name == c_field_id_Class ) || ( id_or_name == c_field_name_Class ) )
      display_name = get_module_string( c_field_display_name_Class );
   if( ( id_or_name == c_field_id_Comments ) || ( id_or_name == c_field_name_Comments ) )
      display_name = get_module_string( c_field_display_name_Comments );
   if( ( id_or_name == c_field_id_Enum ) || ( id_or_name == c_field_name_Enum ) )
      display_name = get_module_string( c_field_display_name_Enum );
   if( ( id_or_name == c_field_id_Enum_Item ) || ( id_or_name == c_field_name_Enum_Item ) )
      display_name = get_module_string( c_field_display_name_Enum_Item );
   if( ( id_or_name == c_field_id_Enum_Item_2 ) || ( id_or_name == c_field_name_Enum_Item_2 ) )
      display_name = get_module_string( c_field_display_name_Enum_Item_2 );
   if( ( id_or_name == c_field_id_Enum_Item_3 ) || ( id_or_name == c_field_name_Enum_Item_3 ) )
      display_name = get_module_string( c_field_display_name_Enum_Item_3 );
   if( ( id_or_name == c_field_id_Enum_Item_4 ) || ( id_or_name == c_field_name_Enum_Item_4 ) )
      display_name = get_module_string( c_field_display_name_Enum_Item_4 );
   if( ( id_or_name == c_field_id_Enum_Item_5 ) || ( id_or_name == c_field_name_Enum_Item_5 ) )
      display_name = get_module_string( c_field_display_name_Enum_Item_5 );
   if( ( id_or_name == c_field_id_Field ) || ( id_or_name == c_field_name_Field ) )
      display_name = get_module_string( c_field_display_name_Field );
   if( ( id_or_name == c_field_id_Field_Class ) || ( id_or_name == c_field_name_Field_Class ) )
      display_name = get_module_string( c_field_display_name_Field_Class );
   if( ( id_or_name == c_field_id_Field_Pairs ) || ( id_or_name == c_field_name_Field_Pairs ) )
      display_name = get_module_string( c_field_display_name_Field_Pairs );
   if( ( id_or_name == c_field_id_Field_Values ) || ( id_or_name == c_field_name_Field_Values ) )
      display_name = get_module_string( c_field_display_name_Field_Values );
   if( ( id_or_name == c_field_id_Fields ) || ( id_or_name == c_field_name_Fields ) )
      display_name = get_module_string( c_field_display_name_Fields );
   if( ( id_or_name == c_field_id_Id ) || ( id_or_name == c_field_name_Id ) )
      display_name = get_module_string( c_field_display_name_Id );
   if( ( id_or_name == c_field_id_Is_Required_For_UI_Gen ) || ( id_or_name == c_field_name_Is_Required_For_UI_Gen ) )
      display_name = get_module_string( c_field_display_name_Is_Required_For_UI_Gen );
   if( ( id_or_name == c_field_id_Model ) || ( id_or_name == c_field_name_Model ) )
      display_name = get_module_string( c_field_display_name_Model );
   if( ( id_or_name == c_field_id_Modifier ) || ( id_or_name == c_field_name_Modifier ) )
      display_name = get_module_string( c_field_display_name_Modifier );
   if( ( id_or_name == c_field_id_Name ) || ( id_or_name == c_field_name_Name ) )
      display_name = get_module_string( c_field_display_name_Name );
   if( ( id_or_name == c_field_id_Options ) || ( id_or_name == c_field_name_Options ) )
      display_name = get_module_string( c_field_display_name_Options );
   if( ( id_or_name == c_field_id_Order ) || ( id_or_name == c_field_name_Order ) )
      display_name = get_module_string( c_field_display_name_Order );
   if( ( id_or_name == c_field_id_Other_Class ) || ( id_or_name == c_field_name_Other_Class ) )
      display_name = get_module_string( c_field_display_name_Other_Class );
   if( ( id_or_name == c_field_id_Other_Class_Field ) || ( id_or_name == c_field_name_Other_Class_Field ) )
      display_name = get_module_string( c_field_display_name_Other_Class_Field );
   if( ( id_or_name == c_field_id_Other_Field ) || ( id_or_name == c_field_name_Other_Field ) )
      display_name = get_module_string( c_field_display_name_Other_Field );
   if( ( id_or_name == c_field_id_Other_Field_2 ) || ( id_or_name == c_field_name_Other_Field_2 ) )
      display_name = get_module_string( c_field_display_name_Other_Field_2 );
   if( ( id_or_name == c_field_id_Other_Modifier ) || ( id_or_name == c_field_name_Other_Modifier ) )
      display_name = get_module_string( c_field_display_name_Other_Modifier );
   if( ( id_or_name == c_field_id_Other_Modifier_2 ) || ( id_or_name == c_field_name_Other_Modifier_2 ) )
      display_name = get_module_string( c_field_display_name_Other_Modifier_2 );
   if( ( id_or_name == c_field_id_Other_Permission ) || ( id_or_name == c_field_name_Other_Permission ) )
      display_name = get_module_string( c_field_display_name_Other_Permission );
   if( ( id_or_name == c_field_id_Other_Permission_2 ) || ( id_or_name == c_field_name_Other_Permission_2 ) )
      display_name = get_module_string( c_field_display_name_Other_Permission_2 );
   if( ( id_or_name == c_field_id_Other_Procedure ) || ( id_or_name == c_field_name_Other_Procedure ) )
      display_name = get_module_string( c_field_display_name_Other_Procedure );
   if( ( id_or_name == c_field_id_Other_Procedure_2 ) || ( id_or_name == c_field_name_Other_Procedure_2 ) )
      display_name = get_module_string( c_field_display_name_Other_Procedure_2 );
   if( ( id_or_name == c_field_id_Other_Source_Child ) || ( id_or_name == c_field_name_Other_Source_Child ) )
      display_name = get_module_string( c_field_display_name_Other_Source_Child );
   if( ( id_or_name == c_field_id_Other_Source_Child_2 ) || ( id_or_name == c_field_name_Other_Source_Child_2 ) )
      display_name = get_module_string( c_field_display_name_Other_Source_Child_2 );
   if( ( id_or_name == c_field_id_Parent_Specification ) || ( id_or_name == c_field_name_Parent_Specification ) )
      display_name = get_module_string( c_field_display_name_Parent_Specification );
   if( ( id_or_name == c_field_id_Permission ) || ( id_or_name == c_field_name_Permission ) )
      display_name = get_module_string( c_field_display_name_Permission );
   if( ( id_or_name == c_field_id_Procedure ) || ( id_or_name == c_field_name_Procedure ) )
      display_name = get_module_string( c_field_display_name_Procedure );
   if( ( id_or_name == c_field_id_Procedure_Arg ) || ( id_or_name == c_field_name_Procedure_Arg ) )
      display_name = get_module_string( c_field_display_name_Procedure_Arg );
   if( ( id_or_name == c_field_id_Procedure_Arg_2 ) || ( id_or_name == c_field_name_Procedure_Arg_2 ) )
      display_name = get_module_string( c_field_display_name_Procedure_Arg_2 );
   if( ( id_or_name == c_field_id_Procedure_Arg_3 ) || ( id_or_name == c_field_name_Procedure_Arg_3 ) )
      display_name = get_module_string( c_field_display_name_Procedure_Arg_3 );
   if( ( id_or_name == c_field_id_Protect_Child_Rel ) || ( id_or_name == c_field_name_Protect_Child_Rel ) )
      display_name = get_module_string( c_field_display_name_Protect_Child_Rel );
   if( ( id_or_name == c_field_id_Protect_Procedure ) || ( id_or_name == c_field_name_Protect_Procedure ) )
      display_name = get_module_string( c_field_display_name_Protect_Procedure );
   if( ( id_or_name == c_field_id_Protect_Source_Parent ) || ( id_or_name == c_field_name_Protect_Source_Parent ) )
      display_name = get_module_string( c_field_display_name_Protect_Source_Parent );
   if( ( id_or_name == c_field_id_Restrict_Values ) || ( id_or_name == c_field_name_Restrict_Values ) )
      display_name = get_module_string( c_field_display_name_Restrict_Values );
   if( ( id_or_name == c_field_id_Source_Child ) || ( id_or_name == c_field_name_Source_Child ) )
      display_name = get_module_string( c_field_display_name_Source_Child );
   if( ( id_or_name == c_field_id_Source_Child_Class ) || ( id_or_name == c_field_name_Source_Child_Class ) )
      display_name = get_module_string( c_field_display_name_Source_Child_Class );
   if( ( id_or_name == c_field_id_Source_Class ) || ( id_or_name == c_field_name_Source_Class ) )
      display_name = get_module_string( c_field_display_name_Source_Class );
   if( ( id_or_name == c_field_id_Source_Field ) || ( id_or_name == c_field_name_Source_Field ) )
      display_name = get_module_string( c_field_display_name_Source_Field );
   if( ( id_or_name == c_field_id_Source_Field_Class ) || ( id_or_name == c_field_name_Source_Field_Class ) )
      display_name = get_module_string( c_field_display_name_Source_Field_Class );
   if( ( id_or_name == c_field_id_Source_Grandchild ) || ( id_or_name == c_field_name_Source_Grandchild ) )
      display_name = get_module_string( c_field_display_name_Source_Grandchild );
   if( ( id_or_name == c_field_id_Source_Parent ) || ( id_or_name == c_field_name_Source_Parent ) )
      display_name = get_module_string( c_field_display_name_Source_Parent );
   if( ( id_or_name == c_field_id_Source_Parent_Class ) || ( id_or_name == c_field_name_Source_Parent_Class ) )
      display_name = get_module_string( c_field_display_name_Source_Parent_Class );
   if( ( id_or_name == c_field_id_Specification_Type ) || ( id_or_name == c_field_name_Specification_Type ) )
      display_name = get_module_string( c_field_display_name_Specification_Type );
   if( ( id_or_name == c_field_id_Strings ) || ( id_or_name == c_field_name_Strings ) )
      display_name = get_module_string( c_field_display_name_Strings );
   if( ( id_or_name == c_field_id_Test_Child ) || ( id_or_name == c_field_name_Test_Child ) )
      display_name = get_module_string( c_field_display_name_Test_Child );
   if( ( id_or_name == c_field_id_Test_Field ) || ( id_or_name == c_field_name_Test_Field ) )
      display_name = get_module_string( c_field_display_name_Test_Field );
   if( ( id_or_name == c_field_id_Test_Field_Class ) || ( id_or_name == c_field_name_Test_Field_Class ) )
      display_name = get_module_string( c_field_display_name_Test_Field_Class );
   if( ( id_or_name == c_field_id_Test_Parent ) || ( id_or_name == c_field_name_Test_Parent ) )
      display_name = get_module_string( c_field_display_name_Test_Parent );
   if( ( id_or_name == c_field_id_Test_Parent_Class ) || ( id_or_name == c_field_name_Test_Parent_Class ) )
      display_name = get_module_string( c_field_display_name_Test_Parent_Class );
   if( ( id_or_name == c_field_id_Test_Value ) || ( id_or_name == c_field_name_Test_Value ) )
      display_name = get_module_string( c_field_display_name_Test_Value );
   if( ( id_or_name == c_field_id_Test_Value_Label ) || ( id_or_name == c_field_name_Test_Value_Label ) )
      display_name = get_module_string( c_field_display_name_Test_Value_Label );
   if( ( id_or_name == c_field_id_Use_Source_Parent ) || ( id_or_name == c_field_name_Use_Source_Parent ) )
      display_name = get_module_string( c_field_display_name_Use_Source_Parent );
   if( ( id_or_name == c_field_id_Use_Test_Parent_Child ) || ( id_or_name == c_field_name_Use_Test_Parent_Child ) )
      display_name = get_module_string( c_field_display_name_Use_Test_Parent_Child );
   if( ( id_or_name == c_field_id_Value ) || ( id_or_name == c_field_name_Value ) )
      display_name = get_module_string( c_field_display_name_Value );
   if( ( id_or_name == c_field_id_Value_Label ) || ( id_or_name == c_field_name_Value_Label ) )
      display_name = get_module_string( c_field_display_name_Value_Label );
   if( ( id_or_name == c_field_id_Value_Left_Part ) || ( id_or_name == c_field_name_Value_Left_Part ) )
      display_name = get_module_string( c_field_display_name_Value_Left_Part );
   if( ( id_or_name == c_field_id_Value_Literal ) || ( id_or_name == c_field_name_Value_Literal ) )
      display_name = get_module_string( c_field_display_name_Value_Literal );
   if( ( id_or_name == c_field_id_Value_Numeric_String ) || ( id_or_name == c_field_name_Value_Numeric_String ) )
      display_name = get_module_string( c_field_display_name_Value_Numeric_String );
   if( ( id_or_name == c_field_id_Value_Right_Part ) || ( id_or_name == c_field_name_Value_Right_Part ) )
      display_name = get_module_string( c_field_display_name_Value_Right_Part );
   if( ( id_or_name == c_field_id_Value_String ) || ( id_or_name == c_field_name_Value_String ) )
      display_name = get_module_string( c_field_display_name_Value_String );
   if( ( id_or_name == c_field_id_Vars ) || ( id_or_name == c_field_name_Vars ) )
      display_name = get_module_string( c_field_display_name_Vars );

   return display_name;
}

void Meta_Specification::clear_foreign_key( const string& field )
{
   p_impl->clear_foreign_key( field );
}

void Meta_Specification::set_foreign_key_value( const string& field, const string& value )
{
   p_impl->set_foreign_key_value( field, value );
}

const string& Meta_Specification::get_foreign_key_value( const string& field )
{
   return p_impl->get_foreign_key_value( field );
}

void Meta_Specification::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   p_impl->get_foreign_key_values( foreign_key_values );
}

void Meta_Specification::setup_foreign_key( Meta_Class& o, const string& value )
{
   static_cast< Meta_Class& >( o ).set_key( value );
}

void Meta_Specification::setup_foreign_key( Meta_Relationship& o, const string& value )
{
   static_cast< Meta_Relationship& >( o ).set_key( value );
}

void Meta_Specification::setup_foreign_key( Meta_Specification_Type& o, const string& value )
{
   static_cast< Meta_Specification_Type& >( o ).set_key( value );
}

void Meta_Specification::setup_foreign_key( Meta_Enum& o, const string& value )
{
   static_cast< Meta_Enum& >( o ).set_key( value );
}

void Meta_Specification::setup_foreign_key( Meta_Enum_Item& o, const string& value )
{
   static_cast< Meta_Enum_Item& >( o ).set_key( value );
}

void Meta_Specification::setup_foreign_key( Meta_Field& o, const string& value )
{
   static_cast< Meta_Field& >( o ).set_key( value );
}

void Meta_Specification::setup_foreign_key( Meta_Model& o, const string& value )
{
   static_cast< Meta_Model& >( o ).set_key( value );
}

void Meta_Specification::setup_foreign_key( Meta_Modifier& o, const string& value )
{
   static_cast< Meta_Modifier& >( o ).set_key( value );
}

void Meta_Specification::setup_foreign_key( Meta_Permission& o, const string& value )
{
   static_cast< Meta_Permission& >( o ).set_key( value );
}

void Meta_Specification::setup_foreign_key( Meta_Procedure& o, const string& value )
{
   static_cast< Meta_Procedure& >( o ).set_key( value );
}

void Meta_Specification::setup_foreign_key( Meta_Specification& o, const string& value )
{
   static_cast< Meta_Specification& >( o ).set_key( value );
}

void Meta_Specification::setup_foreign_key( Meta_Procedure_Arg& o, const string& value )
{
   static_cast< Meta_Procedure_Arg& >( o ).set_key( value );
}

void Meta_Specification::setup_graph_parent( Meta_Specification& o, const string& foreign_key_field )
{
   static_cast< Meta_Specification& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Specification::setup_graph_parent( Meta_List_Field& o, const string& foreign_key_field )
{
   static_cast< Meta_List_Field& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Specification::setup_graph_parent( Meta_View_Field& o, const string& foreign_key_field )
{
   static_cast< Meta_View_Field& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Specification::setup_graph_parent(
 Meta_Class& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Class& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Class& >( o ).set_key( init_value );
}

void Meta_Specification::setup_graph_parent(
 Meta_Relationship& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Relationship& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Relationship& >( o ).set_key( init_value );
}

void Meta_Specification::setup_graph_parent(
 Meta_Specification_Type& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Specification_Type& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Specification_Type& >( o ).set_key( init_value );
}

void Meta_Specification::setup_graph_parent(
 Meta_Enum& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Enum& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Enum& >( o ).set_key( init_value );
}

void Meta_Specification::setup_graph_parent(
 Meta_Enum_Item& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Enum_Item& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Enum_Item& >( o ).set_key( init_value );
}

void Meta_Specification::setup_graph_parent(
 Meta_Field& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Field& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Field& >( o ).set_key( init_value );
}

void Meta_Specification::setup_graph_parent(
 Meta_Model& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Model& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Model& >( o ).set_key( init_value );
}

void Meta_Specification::setup_graph_parent(
 Meta_Modifier& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Modifier& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Modifier& >( o ).set_key( init_value );
}

void Meta_Specification::setup_graph_parent(
 Meta_Permission& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Permission& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Permission& >( o ).set_key( init_value );
}

void Meta_Specification::setup_graph_parent(
 Meta_Procedure& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Procedure& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Procedure& >( o ).set_key( init_value );
}

void Meta_Specification::setup_graph_parent(
 Meta_Specification& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Specification& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Specification& >( o ).set_key( init_value );
}

void Meta_Specification::setup_graph_parent(
 Meta_Procedure_Arg& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Procedure_Arg& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Procedure_Arg& >( o ).set_key( init_value );
}

size_t Meta_Specification::get_total_child_relationships( ) const
{
   return p_impl->total_child_relationships;
}

void Meta_Specification::set_total_child_relationships( size_t new_total_child_relationships ) const
{
   p_impl->total_child_relationships = new_total_child_relationships;
}

size_t Meta_Specification::get_num_foreign_key_children( bool is_internal ) const
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

class_base* Meta_Specification::get_next_foreign_key_child(
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
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "301405";
            p_class_base = &child_Specification_Parent( );
         }
         break;

         case 1:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "302180";
            p_class_base = &child_List_Field_Restriction_Spec( );
         }
         break;

         case 2:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "301980";
            p_class_base = &child_View_Field_Restriction_Spec( );
         }
         break;
      }
   }

   return p_class_base;
}

void Meta_Specification::add_extra_fixed_info( vector< pair< string, string > >& fixed_info ) const
{
   p_impl->add_extra_fixed_info( fixed_info );
}

void Meta_Specification::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   p_impl->add_extra_paging_info( paging_info );
}

string Meta_Specification::get_class_id( ) const
{
   return static_class_id( );
}

string Meta_Specification::get_class_name( ) const
{
   return static_class_name( );
}

string Meta_Specification::get_plural_name( ) const
{
   return static_plural_name( );
}

string Meta_Specification::get_module_id( ) const
{
   return static_module_id( );
}

string Meta_Specification::get_module_name( ) const
{
   return static_module_name( );
}

string Meta_Specification::get_display_name( bool plural ) const
{
   string key( plural ? "plural_" : "class_" );

   key += "specification";

   return get_module_string( key );
}

string Meta_Specification::get_raw_variable( const std::string& name ) const
{
   if( name == g_state_names_variable )
      return p_impl->get_state_names( );
   else
      return class_base::get_raw_variable( name );
}

string Meta_Specification::get_create_instance_info( ) const
{
   return "";
}

string Meta_Specification::get_update_instance_info( ) const
{
   return "";
}

string Meta_Specification::get_destroy_instance_info( ) const
{
   return "";
}

string Meta_Specification::get_execute_procedure_info( const string& procedure_id ) const
{
   string retval;

   if( procedure_id.empty( ) )
      throw runtime_error( "unexpected empty procedure_id for get_execute_procedure_info" );
   else if( procedure_id == "115439" ) // i.e. Add_Arg_1
      retval = "";
   else if( procedure_id == "115440" ) // i.e. Add_Arg_2
      retval = "";
   else if( procedure_id == "115432" ) // i.e. Add_Child_Info
      retval = "";
   else if( procedure_id == "115447" ) // i.e. Add_Destination_Info
      retval = "";
   else if( procedure_id == "115443" ) // i.e. Add_Extra_Info
      retval = "";
   else if( procedure_id == "115430" ) // i.e. Add_Field
      retval = "";
   else if( procedure_id == "115446" ) // i.e. Add_Field_Info
      retval = "";
   else if( procedure_id == "115431" ) // i.e. Add_Field_Pair
      retval = "";
   else if( procedure_id == "115438" ) // i.e. Add_Grandchild_Info
      retval = "";
   else if( procedure_id == "115441" ) // i.e. Add_Info
      retval = "";
   else if( procedure_id == "115444" ) // i.e. Add_Next
      retval = "";
   else if( procedure_id == "115442" ) // i.e. Add_Opt_Info
      retval = "";
   else if( procedure_id == "115445" ) // i.e. Add_Other_Info
      retval = "";
   else if( procedure_id == "115433" ) // i.e. Add_Secondary
      retval = "";
   else if( procedure_id == "115437" ) // i.e. Add_Source_Info
      retval = "";
   else if( procedure_id == "115434" ) // i.e. Add_Stats_1
      retval = "";
   else if( procedure_id == "115435" ) // i.e. Add_Stats_2
      retval = "";
   else if( procedure_id == "115436" ) // i.e. Add_Stats_Info
      retval = "";
   else if( procedure_id == "115510" ) // i.e. Generate_All_Strings
      retval = "";
   else if( procedure_id == "115500" ) // i.e. Generate_All_Vars
      retval = "";
   else if( procedure_id == "115420" ) // i.e. Move_Down
      retval = "";
   else if( procedure_id == "115410" ) // i.e. Move_Up
      retval = "";

   return retval;
}

bool Meta_Specification::get_is_alias( ) const
{
   return false;
}

void Meta_Specification::get_alias_base_info( pair< string, string >& alias_base_info ) const
{
   ( void )alias_base_info;
}

void Meta_Specification::get_base_class_info( vector< pair< string, string > >& base_class_info ) const
{
   ( void )base_class_info;
}

class_base& Meta_Specification::get_or_create_graph_child( const string& context )
{
   class_base* p_class_base( 0 );

   string::size_type pos = context.find( '.' );
   string sub_context( context.substr( 0, pos ) );

   if( sub_context.empty( ) )
      throw runtime_error( "unexpected empty sub-context" );
   else if( ( sub_context == "_301405" ) || ( sub_context == "child_Specification_Parent" ) )
      p_class_base = &child_Specification_Parent( );
   else if( ( sub_context == "_302180" ) || ( sub_context == "child_List_Field_Restriction_Spec" ) )
      p_class_base = &child_List_Field_Restriction_Spec( );
   else if( ( sub_context == "_301980" ) || ( sub_context == "child_View_Field_Restriction_Spec" ) )
      p_class_base = &child_View_Field_Restriction_Spec( );
   else if( ( sub_context == c_field_id_Child_Rel_Child_Class ) || ( sub_context == c_field_name_Child_Rel_Child_Class ) )
      p_class_base = &Child_Rel_Child_Class( );
   else if( ( sub_context == c_field_id_Child_Relationship ) || ( sub_context == c_field_name_Child_Relationship ) )
      p_class_base = &Child_Relationship( );
   else if( ( sub_context == c_field_id_Child_Specification_Type ) || ( sub_context == c_field_name_Child_Specification_Type ) )
      p_class_base = &Child_Specification_Type( );
   else if( ( sub_context == c_field_id_Class ) || ( sub_context == c_field_name_Class ) )
      p_class_base = &Class( );
   else if( ( sub_context == c_field_id_Enum ) || ( sub_context == c_field_name_Enum ) )
      p_class_base = &Enum( );
   else if( ( sub_context == c_field_id_Enum_Item ) || ( sub_context == c_field_name_Enum_Item ) )
      p_class_base = &Enum_Item( );
   else if( ( sub_context == c_field_id_Enum_Item_2 ) || ( sub_context == c_field_name_Enum_Item_2 ) )
      p_class_base = &Enum_Item_2( );
   else if( ( sub_context == c_field_id_Enum_Item_3 ) || ( sub_context == c_field_name_Enum_Item_3 ) )
      p_class_base = &Enum_Item_3( );
   else if( ( sub_context == c_field_id_Enum_Item_4 ) || ( sub_context == c_field_name_Enum_Item_4 ) )
      p_class_base = &Enum_Item_4( );
   else if( ( sub_context == c_field_id_Enum_Item_5 ) || ( sub_context == c_field_name_Enum_Item_5 ) )
      p_class_base = &Enum_Item_5( );
   else if( ( sub_context == c_field_id_Field ) || ( sub_context == c_field_name_Field ) )
      p_class_base = &Field( );
   else if( ( sub_context == c_field_id_Field_Class ) || ( sub_context == c_field_name_Field_Class ) )
      p_class_base = &Field_Class( );
   else if( ( sub_context == c_field_id_Model ) || ( sub_context == c_field_name_Model ) )
      p_class_base = &Model( );
   else if( ( sub_context == c_field_id_Modifier ) || ( sub_context == c_field_name_Modifier ) )
      p_class_base = &Modifier( );
   else if( ( sub_context == c_field_id_Other_Class ) || ( sub_context == c_field_name_Other_Class ) )
      p_class_base = &Other_Class( );
   else if( ( sub_context == c_field_id_Other_Class_Field ) || ( sub_context == c_field_name_Other_Class_Field ) )
      p_class_base = &Other_Class_Field( );
   else if( ( sub_context == c_field_id_Other_Field ) || ( sub_context == c_field_name_Other_Field ) )
      p_class_base = &Other_Field( );
   else if( ( sub_context == c_field_id_Other_Field_2 ) || ( sub_context == c_field_name_Other_Field_2 ) )
      p_class_base = &Other_Field_2( );
   else if( ( sub_context == c_field_id_Other_Modifier ) || ( sub_context == c_field_name_Other_Modifier ) )
      p_class_base = &Other_Modifier( );
   else if( ( sub_context == c_field_id_Other_Modifier_2 ) || ( sub_context == c_field_name_Other_Modifier_2 ) )
      p_class_base = &Other_Modifier_2( );
   else if( ( sub_context == c_field_id_Other_Permission ) || ( sub_context == c_field_name_Other_Permission ) )
      p_class_base = &Other_Permission( );
   else if( ( sub_context == c_field_id_Other_Permission_2 ) || ( sub_context == c_field_name_Other_Permission_2 ) )
      p_class_base = &Other_Permission_2( );
   else if( ( sub_context == c_field_id_Other_Procedure ) || ( sub_context == c_field_name_Other_Procedure ) )
      p_class_base = &Other_Procedure( );
   else if( ( sub_context == c_field_id_Other_Procedure_2 ) || ( sub_context == c_field_name_Other_Procedure_2 ) )
      p_class_base = &Other_Procedure_2( );
   else if( ( sub_context == c_field_id_Other_Source_Child ) || ( sub_context == c_field_name_Other_Source_Child ) )
      p_class_base = &Other_Source_Child( );
   else if( ( sub_context == c_field_id_Other_Source_Child_2 ) || ( sub_context == c_field_name_Other_Source_Child_2 ) )
      p_class_base = &Other_Source_Child_2( );
   else if( ( sub_context == c_field_id_Parent_Specification ) || ( sub_context == c_field_name_Parent_Specification ) )
      p_class_base = &Parent_Specification( );
   else if( ( sub_context == c_field_id_Permission ) || ( sub_context == c_field_name_Permission ) )
      p_class_base = &Permission( );
   else if( ( sub_context == c_field_id_Procedure ) || ( sub_context == c_field_name_Procedure ) )
      p_class_base = &Procedure( );
   else if( ( sub_context == c_field_id_Procedure_Arg ) || ( sub_context == c_field_name_Procedure_Arg ) )
      p_class_base = &Procedure_Arg( );
   else if( ( sub_context == c_field_id_Procedure_Arg_2 ) || ( sub_context == c_field_name_Procedure_Arg_2 ) )
      p_class_base = &Procedure_Arg_2( );
   else if( ( sub_context == c_field_id_Procedure_Arg_3 ) || ( sub_context == c_field_name_Procedure_Arg_3 ) )
      p_class_base = &Procedure_Arg_3( );
   else if( ( sub_context == c_field_id_Source_Child ) || ( sub_context == c_field_name_Source_Child ) )
      p_class_base = &Source_Child( );
   else if( ( sub_context == c_field_id_Source_Child_Class ) || ( sub_context == c_field_name_Source_Child_Class ) )
      p_class_base = &Source_Child_Class( );
   else if( ( sub_context == c_field_id_Source_Class ) || ( sub_context == c_field_name_Source_Class ) )
      p_class_base = &Source_Class( );
   else if( ( sub_context == c_field_id_Source_Field ) || ( sub_context == c_field_name_Source_Field ) )
      p_class_base = &Source_Field( );
   else if( ( sub_context == c_field_id_Source_Field_Class ) || ( sub_context == c_field_name_Source_Field_Class ) )
      p_class_base = &Source_Field_Class( );
   else if( ( sub_context == c_field_id_Source_Grandchild ) || ( sub_context == c_field_name_Source_Grandchild ) )
      p_class_base = &Source_Grandchild( );
   else if( ( sub_context == c_field_id_Source_Parent ) || ( sub_context == c_field_name_Source_Parent ) )
      p_class_base = &Source_Parent( );
   else if( ( sub_context == c_field_id_Source_Parent_Class ) || ( sub_context == c_field_name_Source_Parent_Class ) )
      p_class_base = &Source_Parent_Class( );
   else if( ( sub_context == c_field_id_Specification_Type ) || ( sub_context == c_field_name_Specification_Type ) )
      p_class_base = &Specification_Type( );
   else if( ( sub_context == c_field_id_Test_Child ) || ( sub_context == c_field_name_Test_Child ) )
      p_class_base = &Test_Child( );
   else if( ( sub_context == c_field_id_Test_Field ) || ( sub_context == c_field_name_Test_Field ) )
      p_class_base = &Test_Field( );
   else if( ( sub_context == c_field_id_Test_Field_Class ) || ( sub_context == c_field_name_Test_Field_Class ) )
      p_class_base = &Test_Field_Class( );
   else if( ( sub_context == c_field_id_Test_Parent ) || ( sub_context == c_field_name_Test_Parent ) )
      p_class_base = &Test_Parent( );
   else if( ( sub_context == c_field_id_Test_Parent_Class ) || ( sub_context == c_field_name_Test_Parent_Class ) )
      p_class_base = &Test_Parent_Class( );

   if( !p_class_base )
      throw runtime_error( "unknown sub-context '" + sub_context + "'" );

   if( pos != string::npos )
      p_class_base = &p_class_base->get_or_create_graph_child( context.substr( pos + 1 ) );

   return *p_class_base;
}

void Meta_Specification::get_sql_column_names(
 vector< string >& names, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   names.push_back( "C_Actions" );
   names.push_back( "C_Can_Access_Below_Model" );
   names.push_back( "C_Child_Rel_Child_Class" );
   names.push_back( "C_Child_Rel_Field_Key" );
   names.push_back( "C_Child_Relationship" );
   names.push_back( "C_Child_Specification_Type" );
   names.push_back( "C_Class" );
   names.push_back( "C_Comments" );
   names.push_back( "C_Enum" );
   names.push_back( "C_Enum_Item" );
   names.push_back( "C_Enum_Item_2" );
   names.push_back( "C_Enum_Item_3" );
   names.push_back( "C_Enum_Item_4" );
   names.push_back( "C_Enum_Item_5" );
   names.push_back( "C_Field" );
   names.push_back( "C_Field_Class" );
   names.push_back( "C_Field_Pairs" );
   names.push_back( "C_Field_Values" );
   names.push_back( "C_Fields" );
   names.push_back( "C_Id" );
   names.push_back( "C_Is_Required_For_UI_Gen" );
   names.push_back( "C_Model" );
   names.push_back( "C_Modifier" );
   names.push_back( "C_Name" );
   names.push_back( "C_Options" );
   names.push_back( "C_Order" );
   names.push_back( "C_Other_Class" );
   names.push_back( "C_Other_Class_Field" );
   names.push_back( "C_Other_Field" );
   names.push_back( "C_Other_Field_2" );
   names.push_back( "C_Other_Modifier" );
   names.push_back( "C_Other_Modifier_2" );
   names.push_back( "C_Other_Permission" );
   names.push_back( "C_Other_Permission_2" );
   names.push_back( "C_Other_Procedure" );
   names.push_back( "C_Other_Procedure_2" );
   names.push_back( "C_Other_Source_Child" );
   names.push_back( "C_Other_Source_Child_2" );
   names.push_back( "C_Parent_Specification" );
   names.push_back( "C_Permission" );
   names.push_back( "C_Procedure" );
   names.push_back( "C_Procedure_Arg" );
   names.push_back( "C_Procedure_Arg_2" );
   names.push_back( "C_Procedure_Arg_3" );
   names.push_back( "C_Protect_Child_Rel" );
   names.push_back( "C_Protect_Procedure" );
   names.push_back( "C_Protect_Source_Parent" );
   names.push_back( "C_Restrict_Values" );
   names.push_back( "C_Source_Child" );
   names.push_back( "C_Source_Child_Class" );
   names.push_back( "C_Source_Class" );
   names.push_back( "C_Source_Field" );
   names.push_back( "C_Source_Field_Class" );
   names.push_back( "C_Source_Grandchild" );
   names.push_back( "C_Source_Parent" );
   names.push_back( "C_Source_Parent_Class" );
   names.push_back( "C_Specification_Type" );
   names.push_back( "C_Test_Child" );
   names.push_back( "C_Test_Field" );
   names.push_back( "C_Test_Field_Class" );
   names.push_back( "C_Test_Parent" );
   names.push_back( "C_Test_Parent_Class" );
   names.push_back( "C_Test_Value" );
   names.push_back( "C_Test_Value_Label" );
   names.push_back( "C_Use_Source_Parent" );
   names.push_back( "C_Use_Test_Parent_Child" );
   names.push_back( "C_Value" );
   names.push_back( "C_Value_Label" );
   names.push_back( "C_Value_Left_Part" );
   names.push_back( "C_Value_Literal" );
   names.push_back( "C_Value_Numeric_String" );
   names.push_back( "C_Value_Right_Part" );
   names.push_back( "C_Value_String" );

   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;
}

void Meta_Specification::get_sql_column_values(
 vector< string >& values, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   values.push_back( sql_quote( to_string( Actions( ) ) ) );
   values.push_back( to_string( Can_Access_Below_Model( ) ) );
   values.push_back( sql_quote( to_string( Child_Rel_Child_Class( ) ) ) );
   values.push_back( sql_quote( to_string( Child_Rel_Field_Key( ) ) ) );
   values.push_back( sql_quote( to_string( Child_Relationship( ) ) ) );
   values.push_back( sql_quote( to_string( Child_Specification_Type( ) ) ) );
   values.push_back( sql_quote( to_string( Class( ) ) ) );
   values.push_back( sql_quote( to_string( Comments( ) ) ) );
   values.push_back( sql_quote( to_string( Enum( ) ) ) );
   values.push_back( sql_quote( to_string( Enum_Item( ) ) ) );
   values.push_back( sql_quote( to_string( Enum_Item_2( ) ) ) );
   values.push_back( sql_quote( to_string( Enum_Item_3( ) ) ) );
   values.push_back( sql_quote( to_string( Enum_Item_4( ) ) ) );
   values.push_back( sql_quote( to_string( Enum_Item_5( ) ) ) );
   values.push_back( sql_quote( to_string( Field( ) ) ) );
   values.push_back( sql_quote( to_string( Field_Class( ) ) ) );
   values.push_back( sql_quote( to_string( Field_Pairs( ) ) ) );
   values.push_back( sql_quote( to_string( Field_Values( ) ) ) );
   values.push_back( sql_quote( to_string( Fields( ) ) ) );
   values.push_back( sql_quote( to_string( Id( ) ) ) );
   values.push_back( to_string( Is_Required_For_UI_Gen( ) ) );
   values.push_back( sql_quote( to_string( Model( ) ) ) );
   values.push_back( sql_quote( to_string( Modifier( ) ) ) );
   values.push_back( sql_quote( to_string( Name( ) ) ) );
   values.push_back( sql_quote( to_string( Options( ) ) ) );
   values.push_back( sql_quote( to_string( Order( ) ) ) );
   values.push_back( sql_quote( to_string( Other_Class( ) ) ) );
   values.push_back( sql_quote( to_string( Other_Class_Field( ) ) ) );
   values.push_back( sql_quote( to_string( Other_Field( ) ) ) );
   values.push_back( sql_quote( to_string( Other_Field_2( ) ) ) );
   values.push_back( sql_quote( to_string( Other_Modifier( ) ) ) );
   values.push_back( sql_quote( to_string( Other_Modifier_2( ) ) ) );
   values.push_back( sql_quote( to_string( Other_Permission( ) ) ) );
   values.push_back( sql_quote( to_string( Other_Permission_2( ) ) ) );
   values.push_back( sql_quote( to_string( Other_Procedure( ) ) ) );
   values.push_back( sql_quote( to_string( Other_Procedure_2( ) ) ) );
   values.push_back( sql_quote( to_string( Other_Source_Child( ) ) ) );
   values.push_back( sql_quote( to_string( Other_Source_Child_2( ) ) ) );
   values.push_back( sql_quote( to_string( Parent_Specification( ) ) ) );
   values.push_back( sql_quote( to_string( Permission( ) ) ) );
   values.push_back( sql_quote( to_string( Procedure( ) ) ) );
   values.push_back( sql_quote( to_string( Procedure_Arg( ) ) ) );
   values.push_back( sql_quote( to_string( Procedure_Arg_2( ) ) ) );
   values.push_back( sql_quote( to_string( Procedure_Arg_3( ) ) ) );
   values.push_back( to_string( Protect_Child_Rel( ) ) );
   values.push_back( to_string( Protect_Procedure( ) ) );
   values.push_back( to_string( Protect_Source_Parent( ) ) );
   values.push_back( sql_quote( to_string( Restrict_Values( ) ) ) );
   values.push_back( sql_quote( to_string( Source_Child( ) ) ) );
   values.push_back( sql_quote( to_string( Source_Child_Class( ) ) ) );
   values.push_back( sql_quote( to_string( Source_Class( ) ) ) );
   values.push_back( sql_quote( to_string( Source_Field( ) ) ) );
   values.push_back( sql_quote( to_string( Source_Field_Class( ) ) ) );
   values.push_back( sql_quote( to_string( Source_Grandchild( ) ) ) );
   values.push_back( sql_quote( to_string( Source_Parent( ) ) ) );
   values.push_back( sql_quote( to_string( Source_Parent_Class( ) ) ) );
   values.push_back( sql_quote( to_string( Specification_Type( ) ) ) );
   values.push_back( sql_quote( to_string( Test_Child( ) ) ) );
   values.push_back( sql_quote( to_string( Test_Field( ) ) ) );
   values.push_back( sql_quote( to_string( Test_Field_Class( ) ) ) );
   values.push_back( sql_quote( to_string( Test_Parent( ) ) ) );
   values.push_back( sql_quote( to_string( Test_Parent_Class( ) ) ) );
   values.push_back( sql_quote( to_string( Test_Value( ) ) ) );
   values.push_back( sql_quote( to_string( Test_Value_Label( ) ) ) );
   values.push_back( to_string( Use_Source_Parent( ) ) );
   values.push_back( to_string( Use_Test_Parent_Child( ) ) );
   values.push_back( sql_quote( to_string( Value( ) ) ) );
   values.push_back( sql_quote( to_string( Value_Label( ) ) ) );
   values.push_back( sql_quote( to_string( Value_Left_Part( ) ) ) );
   values.push_back( sql_quote( to_string( Value_Literal( ) ) ) );
   values.push_back( sql_quote( to_string( Value_Numeric_String( ) ) ) );
   values.push_back( sql_quote( to_string( Value_Right_Part( ) ) ) );
   values.push_back( sql_quote( to_string( Value_String( ) ) ) );

   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;
}

void Meta_Specification::get_required_field_names(
 set< string >& names, bool use_transients, set< string >* p_dependents ) const
{
   set< string > local_dependents;
   set< string >& dependents( p_dependents ? *p_dependents : local_dependents );

   get_always_required_field_names( names, use_transients, dependents );

   // [(start field_from_other_field)] 600166
   if( needs_field_value( "Source_Parent_Class", dependents ) )
   {
      dependents.insert( "Source_Parent" );

      if( ( use_transients && is_field_transient( e_field_id_Source_Parent ) )
       || ( !use_transients && !is_field_transient( e_field_id_Source_Parent ) ) )
         names.insert( "Source_Parent" );
   }
   // [(finish field_from_other_field)] 600166

   // [(start field_from_other_field)] 600173
   if( needs_field_value( "Test_Parent_Class", dependents ) )
   {
      dependents.insert( "Test_Parent" );

      if( ( use_transients && is_field_transient( e_field_id_Test_Parent ) )
       || ( !use_transients && !is_field_transient( e_field_id_Test_Parent ) ) )
         names.insert( "Test_Parent" );
   }
   // [(finish field_from_other_field)] 600173

   // [(start field_from_other_field)] 600178
   if( needs_field_value( "Child_Rel_Field_Key", dependents ) )
   {
      dependents.insert( "Child_Relationship" );

      if( ( use_transients && is_field_transient( e_field_id_Child_Relationship ) )
       || ( !use_transients && !is_field_transient( e_field_id_Child_Relationship ) ) )
         names.insert( "Child_Relationship" );
   }
   // [(finish field_from_other_field)] 600178

   // [(start field_from_other_field)] 600179
   if( needs_field_value( "Child_Rel_Child_Class", dependents ) )
   {
      dependents.insert( "Child_Relationship" );

      if( ( use_transients && is_field_transient( e_field_id_Child_Relationship ) )
       || ( !use_transients && !is_field_transient( e_field_id_Child_Relationship ) ) )
         names.insert( "Child_Relationship" );
   }
   // [(finish field_from_other_field)] 600179

   // [(start field_from_other_field)] 600182
   if( needs_field_value( "Is_Required_For_UI_Gen", dependents ) )
   {
      dependents.insert( "Specification_Type" );

      if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
       || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
         names.insert( "Specification_Type" );
   }
   // [(finish field_from_other_field)] 600182

   // [(start field_from_other_field)] 600182a
   if( needs_field_value( "Can_Access_Below_Model", dependents ) )
   {
      dependents.insert( "Specification_Type" );

      if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
       || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
         names.insert( "Specification_Type" );
   }
   // [(finish field_from_other_field)] 600182a

   // [(start field_from_other_field)] 600183
   if( needs_field_value( "Field", dependents ) )
   {
      dependents.insert( "Source_Field" );

      if( ( use_transients && is_field_transient( e_field_id_Source_Field ) )
       || ( !use_transients && !is_field_transient( e_field_id_Source_Field ) ) )
         names.insert( "Source_Field" );

      dependents.insert( "Specification_Type" );

      if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
       || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
         names.insert( "Specification_Type" );
   }
   // [(finish field_from_other_field)] 600183

   // [(start field_from_other_field)] 600184
   if( needs_field_value( "Field", dependents ) )
   {
      dependents.insert( "Source_Child" );

      if( ( use_transients && is_field_transient( e_field_id_Source_Child ) )
       || ( !use_transients && !is_field_transient( e_field_id_Source_Child ) ) )
         names.insert( "Source_Child" );

      dependents.insert( "Specification_Type" );

      if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
       || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
         names.insert( "Specification_Type" );
   }
   // [(finish field_from_other_field)] 600184

   // [(start field_from_other_field)] 600185
   if( needs_field_value( "Value_Literal", dependents ) )
   {
      dependents.insert( "Value" );

      if( ( use_transients && is_field_transient( e_field_id_Value ) )
       || ( !use_transients && !is_field_transient( e_field_id_Value ) ) )
         names.insert( "Value" );
   }
   // [(finish field_from_other_field)] 600185

   // [(start field_from_other_field)] 600186
   if( needs_field_value( "Value_Literal", dependents ) )
   {
      dependents.insert( "Value" );

      if( ( use_transients && is_field_transient( e_field_id_Value ) )
       || ( !use_transients && !is_field_transient( e_field_id_Value ) ) )
         names.insert( "Value" );

      dependents.insert( "Field" );

      if( ( use_transients && is_field_transient( e_field_id_Field ) )
       || ( !use_transients && !is_field_transient( e_field_id_Field ) ) )
         names.insert( "Field" );
   }
   // [(finish field_from_other_field)] 600186

   // [(start field_from_other_field)] 600187
   if( needs_field_value( "Value_String", dependents ) )
   {
      dependents.insert( "Value" );

      if( ( use_transients && is_field_transient( e_field_id_Value ) )
       || ( !use_transients && !is_field_transient( e_field_id_Value ) ) )
         names.insert( "Value" );
   }
   // [(finish field_from_other_field)] 600187

   // [(start field_from_other_field)] 600188
   if( needs_field_value( "Value_Numeric_String", dependents ) )
   {
      dependents.insert( "Value" );

      if( ( use_transients && is_field_transient( e_field_id_Value ) )
       || ( !use_transients && !is_field_transient( e_field_id_Value ) ) )
         names.insert( "Value" );
   }
   // [(finish field_from_other_field)] 600188

   // [(start field_from_other_field)] 600189
   if( needs_field_value( "Value_Left_Part", dependents ) )
   {
      dependents.insert( "Value" );

      if( ( use_transients && is_field_transient( e_field_id_Value ) )
       || ( !use_transients && !is_field_transient( e_field_id_Value ) ) )
         names.insert( "Value" );
   }
   // [(finish field_from_other_field)] 600189

   // [(start field_from_other_field)] 600190
   if( needs_field_value( "Value_Right_Part", dependents ) )
   {
      dependents.insert( "Value" );

      if( ( use_transients && is_field_transient( e_field_id_Value ) )
       || ( !use_transients && !is_field_transient( e_field_id_Value ) ) )
         names.insert( "Value" );
   }
   // [(finish field_from_other_field)] 600190

   // [(start field_from_other_field)] 600190a
   if( needs_field_value( "Value_Label", dependents ) )
   {
      dependents.insert( "Value" );

      if( ( use_transients && is_field_transient( e_field_id_Value ) )
       || ( !use_transients && !is_field_transient( e_field_id_Value ) ) )
         names.insert( "Value" );
   }
   // [(finish field_from_other_field)] 600190a

   // [(start field_from_other_field)] 600190b
   if( needs_field_value( "Test_Value_Label", dependents ) )
   {
      dependents.insert( "Test_Value" );

      if( ( use_transients && is_field_transient( e_field_id_Test_Value ) )
       || ( !use_transients && !is_field_transient( e_field_id_Test_Value ) ) )
         names.insert( "Test_Value" );
   }
   // [(finish field_from_other_field)] 600190b

   // [(start field_from_other_field)] 600191
   if( needs_field_value( "Enum", dependents ) )
   {
      dependents.insert( "Field" );

      if( ( use_transients && is_field_transient( e_field_id_Field ) )
       || ( !use_transients && !is_field_transient( e_field_id_Field ) ) )
         names.insert( "Field" );

      dependents.insert( "Specification_Type" );

      if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
       || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
         names.insert( "Specification_Type" );
   }
   // [(finish field_from_other_field)] 600191

   // [(start field_from_other_field)] 600191a
   if( needs_field_value( "Enum", dependents ) )
   {
      dependents.insert( "Source_Field" );

      if( ( use_transients && is_field_transient( e_field_id_Source_Field ) )
       || ( !use_transients && !is_field_transient( e_field_id_Source_Field ) ) )
         names.insert( "Source_Field" );

      dependents.insert( "Specification_Type" );

      if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
       || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
         names.insert( "Specification_Type" );
   }
   // [(finish field_from_other_field)] 600191a

   // [(start field_from_other_field)] 600191b
   if( needs_field_value( "Enum", dependents ) )
   {
      dependents.insert( "Source_Field" );

      if( ( use_transients && is_field_transient( e_field_id_Source_Field ) )
       || ( !use_transients && !is_field_transient( e_field_id_Source_Field ) ) )
         names.insert( "Source_Field" );

      dependents.insert( "Specification_Type" );

      if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
       || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
         names.insert( "Specification_Type" );
   }
   // [(finish field_from_other_field)] 600191b

   // [(start field_from_other_field)] 600191c
   if( needs_field_value( "Enum", dependents ) )
   {
      dependents.insert( "Source_Child" );

      if( ( use_transients && is_field_transient( e_field_id_Source_Child ) )
       || ( !use_transients && !is_field_transient( e_field_id_Source_Child ) ) )
         names.insert( "Source_Child" );

      dependents.insert( "Specification_Type" );

      if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
       || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
         names.insert( "Specification_Type" );
   }
   // [(finish field_from_other_field)] 600191c

   // [(start field_from_other_field)] 600192
   if( needs_field_value( "Field_Class", dependents ) )
   {
      dependents.insert( "Field" );

      if( ( use_transients && is_field_transient( e_field_id_Field ) )
       || ( !use_transients && !is_field_transient( e_field_id_Field ) ) )
         names.insert( "Field" );
   }
   // [(finish field_from_other_field)] 600192

   // [(start field_from_other_field)] 600193
   if( needs_field_value( "Test_Field_Class", dependents ) )
   {
      dependents.insert( "Test_Field" );

      if( ( use_transients && is_field_transient( e_field_id_Test_Field ) )
       || ( !use_transients && !is_field_transient( e_field_id_Test_Field ) ) )
         names.insert( "Test_Field" );
   }
   // [(finish field_from_other_field)] 600193

   // [(start field_from_other_field)] 600194
   if( needs_field_value( "Source_Child_Class", dependents ) )
   {
      dependents.insert( "Source_Child" );

      if( ( use_transients && is_field_transient( e_field_id_Source_Child ) )
       || ( !use_transients && !is_field_transient( e_field_id_Source_Child ) ) )
         names.insert( "Source_Child" );
   }
   // [(finish field_from_other_field)] 600194

   // [(start field_from_search_replace)] 600204
   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Id" );

      if( ( use_transients && is_field_transient( e_field_id_Id ) )
       || ( !use_transients && !is_field_transient( e_field_id_Id ) ) )
         names.insert( "Id" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Model" );

      if( ( use_transients && is_field_transient( e_field_id_Model ) )
       || ( !use_transients && !is_field_transient( e_field_id_Model ) ) )
         names.insert( "Model" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Class" );

      if( ( use_transients && is_field_transient( e_field_id_Class ) )
       || ( !use_transients && !is_field_transient( e_field_id_Class ) ) )
         names.insert( "Class" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Field" );

      if( ( use_transients && is_field_transient( e_field_id_Field ) )
       || ( !use_transients && !is_field_transient( e_field_id_Field ) ) )
         names.insert( "Field" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Other_Field" );

      if( ( use_transients && is_field_transient( e_field_id_Other_Field ) )
       || ( !use_transients && !is_field_transient( e_field_id_Other_Field ) ) )
         names.insert( "Other_Field" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Other_Field_2" );

      if( ( use_transients && is_field_transient( e_field_id_Other_Field_2 ) )
       || ( !use_transients && !is_field_transient( e_field_id_Other_Field_2 ) ) )
         names.insert( "Other_Field_2" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Modifier" );

      if( ( use_transients && is_field_transient( e_field_id_Modifier ) )
       || ( !use_transients && !is_field_transient( e_field_id_Modifier ) ) )
         names.insert( "Modifier" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Other_Modifier" );

      if( ( use_transients && is_field_transient( e_field_id_Other_Modifier ) )
       || ( !use_transients && !is_field_transient( e_field_id_Other_Modifier ) ) )
         names.insert( "Other_Modifier" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Other_Modifier_2" );

      if( ( use_transients && is_field_transient( e_field_id_Other_Modifier_2 ) )
       || ( !use_transients && !is_field_transient( e_field_id_Other_Modifier_2 ) ) )
         names.insert( "Other_Modifier_2" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Other_Class" );

      if( ( use_transients && is_field_transient( e_field_id_Other_Class ) )
       || ( !use_transients && !is_field_transient( e_field_id_Other_Class ) ) )
         names.insert( "Other_Class" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Other_Class_Field" );

      if( ( use_transients && is_field_transient( e_field_id_Other_Class_Field ) )
       || ( !use_transients && !is_field_transient( e_field_id_Other_Class_Field ) ) )
         names.insert( "Other_Class_Field" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Other_Class_Field" );

      if( ( use_transients && is_field_transient( e_field_id_Other_Class_Field ) )
       || ( !use_transients && !is_field_transient( e_field_id_Other_Class_Field ) ) )
         names.insert( "Other_Class_Field" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Source_Class" );

      if( ( use_transients && is_field_transient( e_field_id_Source_Class ) )
       || ( !use_transients && !is_field_transient( e_field_id_Source_Class ) ) )
         names.insert( "Source_Class" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Source_Parent" );

      if( ( use_transients && is_field_transient( e_field_id_Source_Parent ) )
       || ( !use_transients && !is_field_transient( e_field_id_Source_Parent ) ) )
         names.insert( "Source_Parent" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Source_Parent_Class" );

      if( ( use_transients && is_field_transient( e_field_id_Source_Parent_Class ) )
       || ( !use_transients && !is_field_transient( e_field_id_Source_Parent_Class ) ) )
         names.insert( "Source_Parent_Class" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Source_Child" );

      if( ( use_transients && is_field_transient( e_field_id_Source_Child ) )
       || ( !use_transients && !is_field_transient( e_field_id_Source_Child ) ) )
         names.insert( "Source_Child" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Other_Source_Child" );

      if( ( use_transients && is_field_transient( e_field_id_Other_Source_Child ) )
       || ( !use_transients && !is_field_transient( e_field_id_Other_Source_Child ) ) )
         names.insert( "Other_Source_Child" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Other_Source_Child_2" );

      if( ( use_transients && is_field_transient( e_field_id_Other_Source_Child_2 ) )
       || ( !use_transients && !is_field_transient( e_field_id_Other_Source_Child_2 ) ) )
         names.insert( "Other_Source_Child_2" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Source_Field" );

      if( ( use_transients && is_field_transient( e_field_id_Source_Field ) )
       || ( !use_transients && !is_field_transient( e_field_id_Source_Field ) ) )
         names.insert( "Source_Field" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Source_Grandchild" );

      if( ( use_transients && is_field_transient( e_field_id_Source_Grandchild ) )
       || ( !use_transients && !is_field_transient( e_field_id_Source_Grandchild ) ) )
         names.insert( "Source_Grandchild" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Test_Parent" );

      if( ( use_transients && is_field_transient( e_field_id_Test_Parent ) )
       || ( !use_transients && !is_field_transient( e_field_id_Test_Parent ) ) )
         names.insert( "Test_Parent" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Test_Child" );

      if( ( use_transients && is_field_transient( e_field_id_Test_Child ) )
       || ( !use_transients && !is_field_transient( e_field_id_Test_Child ) ) )
         names.insert( "Test_Child" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Test_Field" );

      if( ( use_transients && is_field_transient( e_field_id_Test_Field ) )
       || ( !use_transients && !is_field_transient( e_field_id_Test_Field ) ) )
         names.insert( "Test_Field" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Test_Value" );

      if( ( use_transients && is_field_transient( e_field_id_Test_Value ) )
       || ( !use_transients && !is_field_transient( e_field_id_Test_Value ) ) )
         names.insert( "Test_Value" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Test_Value_Label" );

      if( ( use_transients && is_field_transient( e_field_id_Test_Value_Label ) )
       || ( !use_transients && !is_field_transient( e_field_id_Test_Value_Label ) ) )
         names.insert( "Test_Value_Label" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Permission" );

      if( ( use_transients && is_field_transient( e_field_id_Permission ) )
       || ( !use_transients && !is_field_transient( e_field_id_Permission ) ) )
         names.insert( "Permission" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Other_Permission" );

      if( ( use_transients && is_field_transient( e_field_id_Other_Permission ) )
       || ( !use_transients && !is_field_transient( e_field_id_Other_Permission ) ) )
         names.insert( "Other_Permission" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Other_Permission_2" );

      if( ( use_transients && is_field_transient( e_field_id_Other_Permission_2 ) )
       || ( !use_transients && !is_field_transient( e_field_id_Other_Permission_2 ) ) )
         names.insert( "Other_Permission_2" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Procedure" );

      if( ( use_transients && is_field_transient( e_field_id_Procedure ) )
       || ( !use_transients && !is_field_transient( e_field_id_Procedure ) ) )
         names.insert( "Procedure" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Procedure_Arg" );

      if( ( use_transients && is_field_transient( e_field_id_Procedure_Arg ) )
       || ( !use_transients && !is_field_transient( e_field_id_Procedure_Arg ) ) )
         names.insert( "Procedure_Arg" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Other_Procedure" );

      if( ( use_transients && is_field_transient( e_field_id_Other_Procedure ) )
       || ( !use_transients && !is_field_transient( e_field_id_Other_Procedure ) ) )
         names.insert( "Other_Procedure" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Other_Procedure_2" );

      if( ( use_transients && is_field_transient( e_field_id_Other_Procedure_2 ) )
       || ( !use_transients && !is_field_transient( e_field_id_Other_Procedure_2 ) ) )
         names.insert( "Other_Procedure_2" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Child_Relationship" );

      if( ( use_transients && is_field_transient( e_field_id_Child_Relationship ) )
       || ( !use_transients && !is_field_transient( e_field_id_Child_Relationship ) ) )
         names.insert( "Child_Relationship" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Child_Relationship" );

      if( ( use_transients && is_field_transient( e_field_id_Child_Relationship ) )
       || ( !use_transients && !is_field_transient( e_field_id_Child_Relationship ) ) )
         names.insert( "Child_Relationship" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Child_Relationship" );

      if( ( use_transients && is_field_transient( e_field_id_Child_Relationship ) )
       || ( !use_transients && !is_field_transient( e_field_id_Child_Relationship ) ) )
         names.insert( "Child_Relationship" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Enum" );

      if( ( use_transients && is_field_transient( e_field_id_Enum ) )
       || ( !use_transients && !is_field_transient( e_field_id_Enum ) ) )
         names.insert( "Enum" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Enum_Item" );

      if( ( use_transients && is_field_transient( e_field_id_Enum_Item ) )
       || ( !use_transients && !is_field_transient( e_field_id_Enum_Item ) ) )
         names.insert( "Enum_Item" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Value" );

      if( ( use_transients && is_field_transient( e_field_id_Value ) )
       || ( !use_transients && !is_field_transient( e_field_id_Value ) ) )
         names.insert( "Value" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Value_Label" );

      if( ( use_transients && is_field_transient( e_field_id_Value_Label ) )
       || ( !use_transients && !is_field_transient( e_field_id_Value_Label ) ) )
         names.insert( "Value_Label" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Value_Numeric_String" );

      if( ( use_transients && is_field_transient( e_field_id_Value_Numeric_String ) )
       || ( !use_transients && !is_field_transient( e_field_id_Value_Numeric_String ) ) )
         names.insert( "Value_Numeric_String" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Field_Values" );

      if( ( use_transients && is_field_transient( e_field_id_Field_Values ) )
       || ( !use_transients && !is_field_transient( e_field_id_Field_Values ) ) )
         names.insert( "Field_Values" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Fields" );

      if( ( use_transients && is_field_transient( e_field_id_Fields ) )
       || ( !use_transients && !is_field_transient( e_field_id_Fields ) ) )
         names.insert( "Fields" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Field_Pairs" );

      if( ( use_transients && is_field_transient( e_field_id_Field_Pairs ) )
       || ( !use_transients && !is_field_transient( e_field_id_Field_Pairs ) ) )
         names.insert( "Field_Pairs" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Specification_Type" );

      if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
       || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
         names.insert( "Specification_Type" );
   }
   // [(finish field_from_search_replace)] 600204

   // [(start field_dependency)] 600205
   if( needs_field_value( "All_Vars", dependents ) )
   {
      dependents.insert( "Vars" );

      if( ( use_transients && is_field_transient( e_field_id_Vars ) )
       || ( !use_transients && !is_field_transient( e_field_id_Vars ) ) )
         names.insert( "Vars" );
   }
   // [(finish field_dependency)] 600205

   // [(start field_dependency)] 600206
   if( needs_field_value( "All_Strings", dependents ) )
   {
      dependents.insert( "Strings" );

      if( ( use_transients && is_field_transient( e_field_id_Strings ) )
       || ( !use_transients && !is_field_transient( e_field_id_Strings ) ) )
         names.insert( "Strings" );
   }
   // [(finish field_dependency)] 600206

   // [(start field_from_search_replace)] 600207
   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Id" );

      if( ( use_transients && is_field_transient( e_field_id_Id ) )
       || ( !use_transients && !is_field_transient( e_field_id_Id ) ) )
         names.insert( "Id" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Model" );

      if( ( use_transients && is_field_transient( e_field_id_Model ) )
       || ( !use_transients && !is_field_transient( e_field_id_Model ) ) )
         names.insert( "Model" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Model" );

      if( ( use_transients && is_field_transient( e_field_id_Model ) )
       || ( !use_transients && !is_field_transient( e_field_id_Model ) ) )
         names.insert( "Model" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Class" );

      if( ( use_transients && is_field_transient( e_field_id_Class ) )
       || ( !use_transients && !is_field_transient( e_field_id_Class ) ) )
         names.insert( "Class" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Class" );

      if( ( use_transients && is_field_transient( e_field_id_Class ) )
       || ( !use_transients && !is_field_transient( e_field_id_Class ) ) )
         names.insert( "Class" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Other_Class" );

      if( ( use_transients && is_field_transient( e_field_id_Other_Class ) )
       || ( !use_transients && !is_field_transient( e_field_id_Other_Class ) ) )
         names.insert( "Other_Class" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Other_Class" );

      if( ( use_transients && is_field_transient( e_field_id_Other_Class ) )
       || ( !use_transients && !is_field_transient( e_field_id_Other_Class ) ) )
         names.insert( "Other_Class" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Field" );

      if( ( use_transients && is_field_transient( e_field_id_Field ) )
       || ( !use_transients && !is_field_transient( e_field_id_Field ) ) )
         names.insert( "Field" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Field_Class" );

      if( ( use_transients && is_field_transient( e_field_id_Field_Class ) )
       || ( !use_transients && !is_field_transient( e_field_id_Field_Class ) ) )
         names.insert( "Field_Class" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Field" );

      if( ( use_transients && is_field_transient( e_field_id_Field ) )
       || ( !use_transients && !is_field_transient( e_field_id_Field ) ) )
         names.insert( "Field" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Field" );

      if( ( use_transients && is_field_transient( e_field_id_Field ) )
       || ( !use_transients && !is_field_transient( e_field_id_Field ) ) )
         names.insert( "Field" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Field" );

      if( ( use_transients && is_field_transient( e_field_id_Field ) )
       || ( !use_transients && !is_field_transient( e_field_id_Field ) ) )
         names.insert( "Field" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Field" );

      if( ( use_transients && is_field_transient( e_field_id_Field ) )
       || ( !use_transients && !is_field_transient( e_field_id_Field ) ) )
         names.insert( "Field" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Field" );

      if( ( use_transients && is_field_transient( e_field_id_Field ) )
       || ( !use_transients && !is_field_transient( e_field_id_Field ) ) )
         names.insert( "Field" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Field" );

      if( ( use_transients && is_field_transient( e_field_id_Field ) )
       || ( !use_transients && !is_field_transient( e_field_id_Field ) ) )
         names.insert( "Field" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Field" );

      if( ( use_transients && is_field_transient( e_field_id_Field ) )
       || ( !use_transients && !is_field_transient( e_field_id_Field ) ) )
         names.insert( "Field" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Other_Field" );

      if( ( use_transients && is_field_transient( e_field_id_Other_Field ) )
       || ( !use_transients && !is_field_transient( e_field_id_Other_Field ) ) )
         names.insert( "Other_Field" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Other_Field" );

      if( ( use_transients && is_field_transient( e_field_id_Other_Field ) )
       || ( !use_transients && !is_field_transient( e_field_id_Other_Field ) ) )
         names.insert( "Other_Field" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Other_Field" );

      if( ( use_transients && is_field_transient( e_field_id_Other_Field ) )
       || ( !use_transients && !is_field_transient( e_field_id_Other_Field ) ) )
         names.insert( "Other_Field" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Other_Field_2" );

      if( ( use_transients && is_field_transient( e_field_id_Other_Field_2 ) )
       || ( !use_transients && !is_field_transient( e_field_id_Other_Field_2 ) ) )
         names.insert( "Other_Field_2" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Other_Field_2" );

      if( ( use_transients && is_field_transient( e_field_id_Other_Field_2 ) )
       || ( !use_transients && !is_field_transient( e_field_id_Other_Field_2 ) ) )
         names.insert( "Other_Field_2" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Other_Class_Field" );

      if( ( use_transients && is_field_transient( e_field_id_Other_Class_Field ) )
       || ( !use_transients && !is_field_transient( e_field_id_Other_Class_Field ) ) )
         names.insert( "Other_Class_Field" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Other_Class_Field" );

      if( ( use_transients && is_field_transient( e_field_id_Other_Class_Field ) )
       || ( !use_transients && !is_field_transient( e_field_id_Other_Class_Field ) ) )
         names.insert( "Other_Class_Field" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Modifier" );

      if( ( use_transients && is_field_transient( e_field_id_Modifier ) )
       || ( !use_transients && !is_field_transient( e_field_id_Modifier ) ) )
         names.insert( "Modifier" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Other_Modifier" );

      if( ( use_transients && is_field_transient( e_field_id_Other_Modifier ) )
       || ( !use_transients && !is_field_transient( e_field_id_Other_Modifier ) ) )
         names.insert( "Other_Modifier" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Other_Modifier_2" );

      if( ( use_transients && is_field_transient( e_field_id_Other_Modifier_2 ) )
       || ( !use_transients && !is_field_transient( e_field_id_Other_Modifier_2 ) ) )
         names.insert( "Other_Modifier_2" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Source_Parent_Class" );

      if( ( use_transients && is_field_transient( e_field_id_Source_Parent_Class ) )
       || ( !use_transients && !is_field_transient( e_field_id_Source_Parent_Class ) ) )
         names.insert( "Source_Parent_Class" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Source_Parent" );

      if( ( use_transients && is_field_transient( e_field_id_Source_Parent ) )
       || ( !use_transients && !is_field_transient( e_field_id_Source_Parent ) ) )
         names.insert( "Source_Parent" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Source_Parent" );

      if( ( use_transients && is_field_transient( e_field_id_Source_Parent ) )
       || ( !use_transients && !is_field_transient( e_field_id_Source_Parent ) ) )
         names.insert( "Source_Parent" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Source_Child_Class" );

      if( ( use_transients && is_field_transient( e_field_id_Source_Child_Class ) )
       || ( !use_transients && !is_field_transient( e_field_id_Source_Child_Class ) ) )
         names.insert( "Source_Child_Class" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Source_Child" );

      if( ( use_transients && is_field_transient( e_field_id_Source_Child ) )
       || ( !use_transients && !is_field_transient( e_field_id_Source_Child ) ) )
         names.insert( "Source_Child" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Source_Child" );

      if( ( use_transients && is_field_transient( e_field_id_Source_Child ) )
       || ( !use_transients && !is_field_transient( e_field_id_Source_Child ) ) )
         names.insert( "Source_Child" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Other_Source_Child" );

      if( ( use_transients && is_field_transient( e_field_id_Other_Source_Child ) )
       || ( !use_transients && !is_field_transient( e_field_id_Other_Source_Child ) ) )
         names.insert( "Other_Source_Child" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Other_Source_Child" );

      if( ( use_transients && is_field_transient( e_field_id_Other_Source_Child ) )
       || ( !use_transients && !is_field_transient( e_field_id_Other_Source_Child ) ) )
         names.insert( "Other_Source_Child" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Other_Source_Child_2" );

      if( ( use_transients && is_field_transient( e_field_id_Other_Source_Child_2 ) )
       || ( !use_transients && !is_field_transient( e_field_id_Other_Source_Child_2 ) ) )
         names.insert( "Other_Source_Child_2" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Other_Source_Child_2" );

      if( ( use_transients && is_field_transient( e_field_id_Other_Source_Child_2 ) )
       || ( !use_transients && !is_field_transient( e_field_id_Other_Source_Child_2 ) ) )
         names.insert( "Other_Source_Child_2" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Source_Field" );

      if( ( use_transients && is_field_transient( e_field_id_Source_Field ) )
       || ( !use_transients && !is_field_transient( e_field_id_Source_Field ) ) )
         names.insert( "Source_Field" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Source_Field" );

      if( ( use_transients && is_field_transient( e_field_id_Source_Field ) )
       || ( !use_transients && !is_field_transient( e_field_id_Source_Field ) ) )
         names.insert( "Source_Field" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Source_Grandchild" );

      if( ( use_transients && is_field_transient( e_field_id_Source_Grandchild ) )
       || ( !use_transients && !is_field_transient( e_field_id_Source_Grandchild ) ) )
         names.insert( "Source_Grandchild" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Source_Field" );

      if( ( use_transients && is_field_transient( e_field_id_Source_Field ) )
       || ( !use_transients && !is_field_transient( e_field_id_Source_Field ) ) )
         names.insert( "Source_Field" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Source_Field" );

      if( ( use_transients && is_field_transient( e_field_id_Source_Field ) )
       || ( !use_transients && !is_field_transient( e_field_id_Source_Field ) ) )
         names.insert( "Source_Field" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Source_Child" );

      if( ( use_transients && is_field_transient( e_field_id_Source_Child ) )
       || ( !use_transients && !is_field_transient( e_field_id_Source_Child ) ) )
         names.insert( "Source_Child" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Test_Parent" );

      if( ( use_transients && is_field_transient( e_field_id_Test_Parent ) )
       || ( !use_transients && !is_field_transient( e_field_id_Test_Parent ) ) )
         names.insert( "Test_Parent" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Test_Child" );

      if( ( use_transients && is_field_transient( e_field_id_Test_Child ) )
       || ( !use_transients && !is_field_transient( e_field_id_Test_Child ) ) )
         names.insert( "Test_Child" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Test_Child" );

      if( ( use_transients && is_field_transient( e_field_id_Test_Child ) )
       || ( !use_transients && !is_field_transient( e_field_id_Test_Child ) ) )
         names.insert( "Test_Child" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Test_Field" );

      if( ( use_transients && is_field_transient( e_field_id_Test_Field ) )
       || ( !use_transients && !is_field_transient( e_field_id_Test_Field ) ) )
         names.insert( "Test_Field" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Test_Field" );

      if( ( use_transients && is_field_transient( e_field_id_Test_Field ) )
       || ( !use_transients && !is_field_transient( e_field_id_Test_Field ) ) )
         names.insert( "Test_Field" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Test_Field" );

      if( ( use_transients && is_field_transient( e_field_id_Test_Field ) )
       || ( !use_transients && !is_field_transient( e_field_id_Test_Field ) ) )
         names.insert( "Test_Field" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Test_Field_Class" );

      if( ( use_transients && is_field_transient( e_field_id_Test_Field_Class ) )
       || ( !use_transients && !is_field_transient( e_field_id_Test_Field_Class ) ) )
         names.insert( "Test_Field_Class" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Test_Value" );

      if( ( use_transients && is_field_transient( e_field_id_Test_Value ) )
       || ( !use_transients && !is_field_transient( e_field_id_Test_Value ) ) )
         names.insert( "Test_Value" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Permission" );

      if( ( use_transients && is_field_transient( e_field_id_Permission ) )
       || ( !use_transients && !is_field_transient( e_field_id_Permission ) ) )
         names.insert( "Permission" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Permission" );

      if( ( use_transients && is_field_transient( e_field_id_Permission ) )
       || ( !use_transients && !is_field_transient( e_field_id_Permission ) ) )
         names.insert( "Permission" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Other_Permission" );

      if( ( use_transients && is_field_transient( e_field_id_Other_Permission ) )
       || ( !use_transients && !is_field_transient( e_field_id_Other_Permission ) ) )
         names.insert( "Other_Permission" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Other_Permission" );

      if( ( use_transients && is_field_transient( e_field_id_Other_Permission ) )
       || ( !use_transients && !is_field_transient( e_field_id_Other_Permission ) ) )
         names.insert( "Other_Permission" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Other_Permission_2" );

      if( ( use_transients && is_field_transient( e_field_id_Other_Permission_2 ) )
       || ( !use_transients && !is_field_transient( e_field_id_Other_Permission_2 ) ) )
         names.insert( "Other_Permission_2" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Other_Permission_2" );

      if( ( use_transients && is_field_transient( e_field_id_Other_Permission_2 ) )
       || ( !use_transients && !is_field_transient( e_field_id_Other_Permission_2 ) ) )
         names.insert( "Other_Permission_2" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Procedure" );

      if( ( use_transients && is_field_transient( e_field_id_Procedure ) )
       || ( !use_transients && !is_field_transient( e_field_id_Procedure ) ) )
         names.insert( "Procedure" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Procedure" );

      if( ( use_transients && is_field_transient( e_field_id_Procedure ) )
       || ( !use_transients && !is_field_transient( e_field_id_Procedure ) ) )
         names.insert( "Procedure" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Procedure_Arg" );

      if( ( use_transients && is_field_transient( e_field_id_Procedure_Arg ) )
       || ( !use_transients && !is_field_transient( e_field_id_Procedure_Arg ) ) )
         names.insert( "Procedure_Arg" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Procedure_Arg_2" );

      if( ( use_transients && is_field_transient( e_field_id_Procedure_Arg_2 ) )
       || ( !use_transients && !is_field_transient( e_field_id_Procedure_Arg_2 ) ) )
         names.insert( "Procedure_Arg_2" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Procedure_Arg_3" );

      if( ( use_transients && is_field_transient( e_field_id_Procedure_Arg_3 ) )
       || ( !use_transients && !is_field_transient( e_field_id_Procedure_Arg_3 ) ) )
         names.insert( "Procedure_Arg_3" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Other_Procedure" );

      if( ( use_transients && is_field_transient( e_field_id_Other_Procedure ) )
       || ( !use_transients && !is_field_transient( e_field_id_Other_Procedure ) ) )
         names.insert( "Other_Procedure" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Other_Procedure" );

      if( ( use_transients && is_field_transient( e_field_id_Other_Procedure ) )
       || ( !use_transients && !is_field_transient( e_field_id_Other_Procedure ) ) )
         names.insert( "Other_Procedure" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Other_Procedure_2" );

      if( ( use_transients && is_field_transient( e_field_id_Other_Procedure_2 ) )
       || ( !use_transients && !is_field_transient( e_field_id_Other_Procedure_2 ) ) )
         names.insert( "Other_Procedure_2" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Other_Procedure_2" );

      if( ( use_transients && is_field_transient( e_field_id_Other_Procedure_2 ) )
       || ( !use_transients && !is_field_transient( e_field_id_Other_Procedure_2 ) ) )
         names.insert( "Other_Procedure_2" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Child_Relationship" );

      if( ( use_transients && is_field_transient( e_field_id_Child_Relationship ) )
       || ( !use_transients && !is_field_transient( e_field_id_Child_Relationship ) ) )
         names.insert( "Child_Relationship" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Child_Relationship" );

      if( ( use_transients && is_field_transient( e_field_id_Child_Relationship ) )
       || ( !use_transients && !is_field_transient( e_field_id_Child_Relationship ) ) )
         names.insert( "Child_Relationship" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Child_Relationship" );

      if( ( use_transients && is_field_transient( e_field_id_Child_Relationship ) )
       || ( !use_transients && !is_field_transient( e_field_id_Child_Relationship ) ) )
         names.insert( "Child_Relationship" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Child_Relationship" );

      if( ( use_transients && is_field_transient( e_field_id_Child_Relationship ) )
       || ( !use_transients && !is_field_transient( e_field_id_Child_Relationship ) ) )
         names.insert( "Child_Relationship" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Field" );

      if( ( use_transients && is_field_transient( e_field_id_Field ) )
       || ( !use_transients && !is_field_transient( e_field_id_Field ) ) )
         names.insert( "Field" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Enum" );

      if( ( use_transients && is_field_transient( e_field_id_Enum ) )
       || ( !use_transients && !is_field_transient( e_field_id_Enum ) ) )
         names.insert( "Enum" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Enum_Item" );

      if( ( use_transients && is_field_transient( e_field_id_Enum_Item ) )
       || ( !use_transients && !is_field_transient( e_field_id_Enum_Item ) ) )
         names.insert( "Enum_Item" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Enum_Item" );

      if( ( use_transients && is_field_transient( e_field_id_Enum_Item ) )
       || ( !use_transients && !is_field_transient( e_field_id_Enum_Item ) ) )
         names.insert( "Enum_Item" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Enum_Item_2" );

      if( ( use_transients && is_field_transient( e_field_id_Enum_Item_2 ) )
       || ( !use_transients && !is_field_transient( e_field_id_Enum_Item_2 ) ) )
         names.insert( "Enum_Item_2" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Enum_Item_2" );

      if( ( use_transients && is_field_transient( e_field_id_Enum_Item_2 ) )
       || ( !use_transients && !is_field_transient( e_field_id_Enum_Item_2 ) ) )
         names.insert( "Enum_Item_2" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Enum_Item_3" );

      if( ( use_transients && is_field_transient( e_field_id_Enum_Item_3 ) )
       || ( !use_transients && !is_field_transient( e_field_id_Enum_Item_3 ) ) )
         names.insert( "Enum_Item_3" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Enum_Item_3" );

      if( ( use_transients && is_field_transient( e_field_id_Enum_Item_3 ) )
       || ( !use_transients && !is_field_transient( e_field_id_Enum_Item_3 ) ) )
         names.insert( "Enum_Item_3" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Enum_Item_4" );

      if( ( use_transients && is_field_transient( e_field_id_Enum_Item_4 ) )
       || ( !use_transients && !is_field_transient( e_field_id_Enum_Item_4 ) ) )
         names.insert( "Enum_Item_4" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Enum_Item_4" );

      if( ( use_transients && is_field_transient( e_field_id_Enum_Item_4 ) )
       || ( !use_transients && !is_field_transient( e_field_id_Enum_Item_4 ) ) )
         names.insert( "Enum_Item_4" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Enum_Item_5" );

      if( ( use_transients && is_field_transient( e_field_id_Enum_Item_5 ) )
       || ( !use_transients && !is_field_transient( e_field_id_Enum_Item_5 ) ) )
         names.insert( "Enum_Item_5" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Enum_Item_5" );

      if( ( use_transients && is_field_transient( e_field_id_Enum_Item_5 ) )
       || ( !use_transients && !is_field_transient( e_field_id_Enum_Item_5 ) ) )
         names.insert( "Enum_Item_5" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Value" );

      if( ( use_transients && is_field_transient( e_field_id_Value ) )
       || ( !use_transients && !is_field_transient( e_field_id_Value ) ) )
         names.insert( "Value" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Value_Literal" );

      if( ( use_transients && is_field_transient( e_field_id_Value_Literal ) )
       || ( !use_transients && !is_field_transient( e_field_id_Value_Literal ) ) )
         names.insert( "Value_Literal" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Value_String" );

      if( ( use_transients && is_field_transient( e_field_id_Value_String ) )
       || ( !use_transients && !is_field_transient( e_field_id_Value_String ) ) )
         names.insert( "Value_String" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Value_Left_Part" );

      if( ( use_transients && is_field_transient( e_field_id_Value_Left_Part ) )
       || ( !use_transients && !is_field_transient( e_field_id_Value_Left_Part ) ) )
         names.insert( "Value_Left_Part" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Value_Right_Part" );

      if( ( use_transients && is_field_transient( e_field_id_Value_Right_Part ) )
       || ( !use_transients && !is_field_transient( e_field_id_Value_Right_Part ) ) )
         names.insert( "Value_Right_Part" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Field_Values" );

      if( ( use_transients && is_field_transient( e_field_id_Field_Values ) )
       || ( !use_transients && !is_field_transient( e_field_id_Field_Values ) ) )
         names.insert( "Field_Values" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Fields" );

      if( ( use_transients && is_field_transient( e_field_id_Fields ) )
       || ( !use_transients && !is_field_transient( e_field_id_Fields ) ) )
         names.insert( "Fields" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Field_Pairs" );

      if( ( use_transients && is_field_transient( e_field_id_Field_Pairs ) )
       || ( !use_transients && !is_field_transient( e_field_id_Field_Pairs ) ) )
         names.insert( "Field_Pairs" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Options" );

      if( ( use_transients && is_field_transient( e_field_id_Options ) )
       || ( !use_transients && !is_field_transient( e_field_id_Options ) ) )
         names.insert( "Options" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Specification_Type" );

      if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
       || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
         names.insert( "Specification_Type" );
   }
   // [(finish field_from_search_replace)] 600207

   // [(start field_from_search_replace)] 600208
   if( needs_field_value( "Strings", dependents ) )
   {
      dependents.insert( "Model" );

      if( ( use_transients && is_field_transient( e_field_id_Model ) )
       || ( !use_transients && !is_field_transient( e_field_id_Model ) ) )
         names.insert( "Model" );
   }

   if( needs_field_value( "Strings", dependents ) )
   {
      dependents.insert( "Class" );

      if( ( use_transients && is_field_transient( e_field_id_Class ) )
       || ( !use_transients && !is_field_transient( e_field_id_Class ) ) )
         names.insert( "Class" );
   }

   if( needs_field_value( "Strings", dependents ) )
   {
      dependents.insert( "Field" );

      if( ( use_transients && is_field_transient( e_field_id_Field ) )
       || ( !use_transients && !is_field_transient( e_field_id_Field ) ) )
         names.insert( "Field" );
   }

   if( needs_field_value( "Strings", dependents ) )
   {
      dependents.insert( "Field_Class" );

      if( ( use_transients && is_field_transient( e_field_id_Field_Class ) )
       || ( !use_transients && !is_field_transient( e_field_id_Field_Class ) ) )
         names.insert( "Field_Class" );
   }

   if( needs_field_value( "Strings", dependents ) )
   {
      dependents.insert( "Modifier" );

      if( ( use_transients && is_field_transient( e_field_id_Modifier ) )
       || ( !use_transients && !is_field_transient( e_field_id_Modifier ) ) )
         names.insert( "Modifier" );
   }

   if( needs_field_value( "Strings", dependents ) )
   {
      dependents.insert( "Source_Parent" );

      if( ( use_transients && is_field_transient( e_field_id_Source_Parent ) )
       || ( !use_transients && !is_field_transient( e_field_id_Source_Parent ) ) )
         names.insert( "Source_Parent" );
   }

   if( needs_field_value( "Strings", dependents ) )
   {
      dependents.insert( "Source_Child" );

      if( ( use_transients && is_field_transient( e_field_id_Source_Child ) )
       || ( !use_transients && !is_field_transient( e_field_id_Source_Child ) ) )
         names.insert( "Source_Child" );
   }

   if( needs_field_value( "Strings", dependents ) )
   {
      dependents.insert( "Source_Field" );

      if( ( use_transients && is_field_transient( e_field_id_Source_Field ) )
       || ( !use_transients && !is_field_transient( e_field_id_Source_Field ) ) )
         names.insert( "Source_Field" );
   }

   if( needs_field_value( "Strings", dependents ) )
   {
      dependents.insert( "Test_Parent" );

      if( ( use_transients && is_field_transient( e_field_id_Test_Parent ) )
       || ( !use_transients && !is_field_transient( e_field_id_Test_Parent ) ) )
         names.insert( "Test_Parent" );
   }

   if( needs_field_value( "Strings", dependents ) )
   {
      dependents.insert( "Test_Child" );

      if( ( use_transients && is_field_transient( e_field_id_Test_Child ) )
       || ( !use_transients && !is_field_transient( e_field_id_Test_Child ) ) )
         names.insert( "Test_Child" );
   }

   if( needs_field_value( "Strings", dependents ) )
   {
      dependents.insert( "Test_Field" );

      if( ( use_transients && is_field_transient( e_field_id_Test_Field ) )
       || ( !use_transients && !is_field_transient( e_field_id_Test_Field ) ) )
         names.insert( "Test_Field" );
   }

   if( needs_field_value( "Strings", dependents ) )
   {
      dependents.insert( "Test_Field_Class" );

      if( ( use_transients && is_field_transient( e_field_id_Test_Field_Class ) )
       || ( !use_transients && !is_field_transient( e_field_id_Test_Field_Class ) ) )
         names.insert( "Test_Field_Class" );
   }

   if( needs_field_value( "Strings", dependents ) )
   {
      dependents.insert( "Test_Value" );

      if( ( use_transients && is_field_transient( e_field_id_Test_Value ) )
       || ( !use_transients && !is_field_transient( e_field_id_Test_Value ) ) )
         names.insert( "Test_Value" );
   }

   if( needs_field_value( "Strings", dependents ) )
   {
      dependents.insert( "Procedure" );

      if( ( use_transients && is_field_transient( e_field_id_Procedure ) )
       || ( !use_transients && !is_field_transient( e_field_id_Procedure ) ) )
         names.insert( "Procedure" );
   }

   if( needs_field_value( "Strings", dependents ) )
   {
      dependents.insert( "Child_Relationship" );

      if( ( use_transients && is_field_transient( e_field_id_Child_Relationship ) )
       || ( !use_transients && !is_field_transient( e_field_id_Child_Relationship ) ) )
         names.insert( "Child_Relationship" );
   }

   if( needs_field_value( "Strings", dependents ) )
   {
      dependents.insert( "Enum" );

      if( ( use_transients && is_field_transient( e_field_id_Enum ) )
       || ( !use_transients && !is_field_transient( e_field_id_Enum ) ) )
         names.insert( "Enum" );
   }

   if( needs_field_value( "Strings", dependents ) )
   {
      dependents.insert( "Enum_Item" );

      if( ( use_transients && is_field_transient( e_field_id_Enum_Item ) )
       || ( !use_transients && !is_field_transient( e_field_id_Enum_Item ) ) )
         names.insert( "Enum_Item" );
   }

   if( needs_field_value( "Strings", dependents ) )
   {
      dependents.insert( "Enum_Item" );

      if( ( use_transients && is_field_transient( e_field_id_Enum_Item ) )
       || ( !use_transients && !is_field_transient( e_field_id_Enum_Item ) ) )
         names.insert( "Enum_Item" );
   }

   if( needs_field_value( "Strings", dependents ) )
   {
      dependents.insert( "Value" );

      if( ( use_transients && is_field_transient( e_field_id_Value ) )
       || ( !use_transients && !is_field_transient( e_field_id_Value ) ) )
         names.insert( "Value" );
   }

   if( needs_field_value( "Strings", dependents ) )
   {
      dependents.insert( "Value_Left_Part" );

      if( ( use_transients && is_field_transient( e_field_id_Value_Left_Part ) )
       || ( !use_transients && !is_field_transient( e_field_id_Value_Left_Part ) ) )
         names.insert( "Value_Left_Part" );
   }

   if( needs_field_value( "Strings", dependents ) )
   {
      dependents.insert( "Value_Right_Part" );

      if( ( use_transients && is_field_transient( e_field_id_Value_Right_Part ) )
       || ( !use_transients && !is_field_transient( e_field_id_Value_Right_Part ) ) )
         names.insert( "Value_Right_Part" );
   }

   if( needs_field_value( "Strings", dependents ) )
   {
      dependents.insert( "Specification_Type" );

      if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
       || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
         names.insert( "Specification_Type" );
   }
   // [(finish field_from_search_replace)] 600208

   // [(start field_from_other_field)] 610192
   if( needs_field_value( "Source_Field_Class", dependents ) )
   {
      dependents.insert( "Source_Field" );

      if( ( use_transients && is_field_transient( e_field_id_Source_Field ) )
       || ( !use_transients && !is_field_transient( e_field_id_Source_Field ) ) )
         names.insert( "Source_Field" );
   }
   // [(finish field_from_other_field)] 610192

   // [<start get_required_field_names>]
   // [<finish get_required_field_names>]
}

void Meta_Specification::get_always_required_field_names(
 set< string >& names, bool use_transients, set< string >& dependents ) const
{
   ( void )names;
   ( void )dependents;
   ( void )use_transients;

   // [(start modifier_field_value)] 600101
   dependents.insert( "Specification_Type" ); // (for Is_Comment modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 600101

   // [(start modifier_field_value)] 600101a
   dependents.insert( "Specification_Type" ); // (for Hide_Other_Class modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 600101a

   // [(start modifier_field_value)] 600101b
   dependents.insert( "Specification_Type" ); // (for Hide_Field modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 600101b

   // [(start modifier_field_value)] 600102a
   dependents.insert( "Specification_Type" ); // (for Hide_Other_Field modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 600102a

   // [(start modifier_field_value)] 600102c
   dependents.insert( "Specification_Type" ); // (for Hide_Other_Class_Field modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 600102c

   // [(start modifier_field_value)] 600103
   dependents.insert( "Specification_Type" ); // (for Hide_Procedure modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 600103

   // [(start modifier_field_value)] 600104
   dependents.insert( "Specification_Type" ); // (for Hide_Procedure_Arg modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 600104

   // [(start modifier_field_value)] 600104a
   dependents.insert( "Specification_Type" ); // (for Hide_Procedure_Arg_2 modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 600104a

   // [(start modifier_field_value)] 600104b
   dependents.insert( "Specification_Type" ); // (for Hide_Procedure_Arg_3 modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 600104b

   // [(start modifier_field_value)] 600104c
   dependents.insert( "Specification_Type" ); // (for Hide_Other_Procedure modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 600104c

   // [(start modifier_field_value)] 600104d
   dependents.insert( "Specification_Type" ); // (for Hide_Other_Procedure_2 modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 600104d

   // [(start modifier_field_value)] 600105
   dependents.insert( "Specification_Type" ); // (for Hide_Modifier modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 600105

   // [(start modifier_field_value)] 600105a
   dependents.insert( "Specification_Type" ); // (for Hide_Other_Modifier modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 600105a

   // [(start modifier_field_value)] 600105b
   dependents.insert( "Specification_Type" ); // (for Hide_Other_Modifier_2 modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 600105b

   // [(start modifier_field_value)] 600106
   dependents.insert( "Specification_Type" ); // (for Hide_Enum modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 600106

   // [(start modifier_field_value)] 600107
   dependents.insert( "Specification_Type" ); // (for Hide_Enum_Item modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 600107

   // [(start modifier_field_value)] 600107a
   dependents.insert( "Specification_Type" ); // (for Hide_Enum_Item_2 modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 600107a

   // [(start modifier_field_value)] 600107b
   dependents.insert( "Specification_Type" ); // (for Hide_Enum_Item_3 modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 600107b

   // [(start modifier_field_value)] 600107c
   dependents.insert( "Specification_Type" ); // (for Hide_Enum_Item_4 modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 600107c

   // [(start modifier_field_value)] 600107d
   dependents.insert( "Specification_Type" ); // (for Hide_Enum_Item_5 modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 600107d

   // [(start modifier_field_value)] 600108
   dependents.insert( "Specification_Type" ); // (for Hide_Value modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 600108

   // [(start modifier_field_value)] 600109
   dependents.insert( "Specification_Type" ); // (for Hide_Source_Parent modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 600109

   // [(start modifier_field_value)] 600110
   dependents.insert( "Specification_Type" ); // (for Hide_Source_Field modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 600110

   // [(start modifier_field_value)] 600111
   dependents.insert( "Specification_Type" ); // (for Hide_Test_Field modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 600111

   // [(start modifier_field_value)] 600112
   dependents.insert( "Specification_Type" ); // (for Hide_Test_Value modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 600112

   // [(start modifier_field_value)] 600113
   dependents.insert( "Specification_Type" ); // (for Hide_Options modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 600113

   // [(start modifier_field_value)] 600114
   dependents.insert( "Specification_Type" ); // (for Hide_Source_Child modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 600114

   // [(start modifier_field_value)] 600115
   dependents.insert( "Specification_Type" ); // (for Hide_Use_Source_Parent modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 600115

   // [(start modifier_field_value)] 600116
   dependents.insert( "Use_Source_Parent" ); // (for Hide_Source_Parent modifier)

   if( ( use_transients && is_field_transient( e_field_id_Use_Source_Parent ) )
    || ( !use_transients && !is_field_transient( e_field_id_Use_Source_Parent ) ) )
      names.insert( "Use_Source_Parent" );

   dependents.insert( "Specification_Type" ); // (for Hide_Source_Parent modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 600116

   // [(start modifier_field_value)] 600117
   dependents.insert( "Use_Source_Parent" ); // (for Hide_Source_Child modifier)

   if( ( use_transients && is_field_transient( e_field_id_Use_Source_Parent ) )
    || ( !use_transients && !is_field_transient( e_field_id_Use_Source_Parent ) ) )
      names.insert( "Use_Source_Parent" );

   dependents.insert( "Specification_Type" ); // (for Hide_Source_Child modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 600117

   // [(start modifier_field_value)] 600118
   dependents.insert( "Use_Source_Parent" ); // (for Hide_Source_Field modifier)

   if( ( use_transients && is_field_transient( e_field_id_Use_Source_Parent ) )
    || ( !use_transients && !is_field_transient( e_field_id_Use_Source_Parent ) ) )
      names.insert( "Use_Source_Parent" );

   dependents.insert( "Specification_Type" ); // (for Hide_Source_Field modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 600118

   // [(start modifier_field_value)] 600119
   dependents.insert( "Specification_Type" ); // (for Hide_Permission modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 600119

   // [(start modifier_field_value)] 600119a
   dependents.insert( "Specification_Type" ); // (for Hide_Other_Permission modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 600119a

   // [(start modifier_field_value)] 600119b
   dependents.insert( "Specification_Type" ); // (for Hide_Other_Permission_2 modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 600119b

   // [(start modifier_field_value)] 600120
   dependents.insert( "Protect_Source_Parent" ); // (for Protect_Source_Parent modifier)

   if( ( use_transients && is_field_transient( e_field_id_Protect_Source_Parent ) )
    || ( !use_transients && !is_field_transient( e_field_id_Protect_Source_Parent ) ) )
      names.insert( "Protect_Source_Parent" );
   // [(finish modifier_field_value)] 600120

   // [(start modifier_field_value)] 600121
   dependents.insert( "Specification_Type" ); // (for Protect_Source_Parent modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 600121

   // [(start modifier_field_value)] 600122
   dependents.insert( "Specification_Type" ); // (for Hide_Child_Relationship modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 600122

   // [(start modifier_field_value)] 600123
   dependents.insert( "Specification_Type" ); // (for Protect_Class modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 600123

   // [(start modifier_field_value)] 600124
   dependents.insert( "Specification_Type" ); // (for Hide_Use_Test_Parent_Child modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 600124

   // [(start modifier_field_value)] 600125
   dependents.insert( "Use_Test_Parent_Child" ); // (for Hide_Test_Parent modifier)

   if( ( use_transients && is_field_transient( e_field_id_Use_Test_Parent_Child ) )
    || ( !use_transients && !is_field_transient( e_field_id_Use_Test_Parent_Child ) ) )
      names.insert( "Use_Test_Parent_Child" );

   dependents.insert( "Specification_Type" ); // (for Hide_Test_Parent modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 600125

   // [(start modifier_field_value)] 600126
   dependents.insert( "Use_Test_Parent_Child" ); // (for Hide_Test_Child modifier)

   if( ( use_transients && is_field_transient( e_field_id_Use_Test_Parent_Child ) )
    || ( !use_transients && !is_field_transient( e_field_id_Use_Test_Parent_Child ) ) )
      names.insert( "Use_Test_Parent_Child" );

   dependents.insert( "Specification_Type" ); // (for Hide_Test_Child modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 600126

   // [(start modifier_field_value)] 600127
   dependents.insert( "Use_Test_Parent_Child" ); // (for Hide_Test_Field modifier)

   if( ( use_transients && is_field_transient( e_field_id_Use_Test_Parent_Child ) )
    || ( !use_transients && !is_field_transient( e_field_id_Use_Test_Parent_Child ) ) )
      names.insert( "Use_Test_Parent_Child" );

   dependents.insert( "Specification_Type" ); // (for Hide_Test_Field modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 600127

   // [(start modifier_field_value)] 600128
   dependents.insert( "Protect_Child_Rel" ); // (for Protect_Child_Relationship modifier)

   if( ( use_transients && is_field_transient( e_field_id_Protect_Child_Rel ) )
    || ( !use_transients && !is_field_transient( e_field_id_Protect_Child_Rel ) ) )
      names.insert( "Protect_Child_Rel" );
   // [(finish modifier_field_value)] 600128

   // [(start move_up_and_down)] 600169
   dependents.insert( "Order" );

   if( ( use_transients && is_field_transient( e_field_id_Order ) )
    || ( !use_transients && !is_field_transient( e_field_id_Order ) ) )
      names.insert( "Order" );
   // [(finish move_up_and_down)] 600169

   // [(start destroy_restrict)] 600180
   dependents.insert( "Specification_Type" );

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish destroy_restrict)] 600180

   // [(start modifier_field_value)] 610102b
   dependents.insert( "Specification_Type" ); // (for Hide_Other_Field_2 modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 610102b

   // [(start modifier_field_value)] 610114
   dependents.insert( "Specification_Type" ); // (for Hide_Other_Source_Child modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 610114

   // [(start modifier_field_value)] 610117
   dependents.insert( "Use_Source_Parent" ); // (for Hide_Other_Source_Child modifier)

   if( ( use_transients && is_field_transient( e_field_id_Use_Source_Parent ) )
    || ( !use_transients && !is_field_transient( e_field_id_Use_Source_Parent ) ) )
      names.insert( "Use_Source_Parent" );

   dependents.insert( "Specification_Type" ); // (for Hide_Other_Source_Child modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 610117

   // [(start modifier_field_value)] 610128
   dependents.insert( "Specification_Type" ); // (for Protect_Child_Relationship modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 610128

   // [(start modifier_field_value)] 610181
   dependents.insert( "Protect_Procedure" ); // (for Protect_Procedure modifier)

   if( ( use_transients && is_field_transient( e_field_id_Protect_Procedure ) )
    || ( !use_transients && !is_field_transient( e_field_id_Protect_Procedure ) ) )
      names.insert( "Protect_Procedure" );
   // [(finish modifier_field_value)] 610181

   // [(start modifier_field_value)] 620114
   dependents.insert( "Specification_Type" ); // (for Hide_Other_Source_Child_2 modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 620114

   // [(start modifier_field_value)] 620117
   dependents.insert( "Use_Source_Parent" ); // (for Hide_Other_Source_Child_2 modifier)

   if( ( use_transients && is_field_transient( e_field_id_Use_Source_Parent ) )
    || ( !use_transients && !is_field_transient( e_field_id_Use_Source_Parent ) ) )
      names.insert( "Use_Source_Parent" );

   dependents.insert( "Specification_Type" ); // (for Hide_Other_Source_Child_2 modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 620117

   // [(start modifier_field_value)] 620128
   dependents.insert( "Specification_Type" ); // (for Protect_Class modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 620128

   // [(start modifier_field_value)] 620146
   dependents.insert( "Specification_Type" ); // (for Hide_Source_Class modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 620146

   // [(start modifier_field_value)] 620181
   dependents.insert( "Specification_Type" ); // (for Protect_Procedure modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 620181

   // [(start update_parent_cascade)] 620194
   dependents.insert( "Parent_Specification" );

   if( ( use_transients && is_field_transient( e_field_id_Parent_Specification ) )
    || ( !use_transients && !is_field_transient( e_field_id_Parent_Specification ) ) )
      names.insert( "Parent_Specification" );
   // [(finish update_parent_cascade)] 620194

   // [(start modifier_field_value)] 630114
   dependents.insert( "Specification_Type" ); // (for Hide_Source_Grandchild modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 630114

   // [(start modifier_field_value)] 630117
   dependents.insert( "Use_Source_Parent" ); // (for Hide_Source_Grandchild modifier)

   if( ( use_transients && is_field_transient( e_field_id_Use_Source_Parent ) )
    || ( !use_transients && !is_field_transient( e_field_id_Use_Source_Parent ) ) )
      names.insert( "Use_Source_Parent" );

   dependents.insert( "Specification_Type" ); // (for Hide_Source_Grandchild modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 630117

   // [(start modifier_field_value)] 630128
   dependents.insert( "Specification_Type" ); // (for Protect_Enum modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 630128

   // [(start modifier_field_value)] 630128a
   dependents.insert( "Specification_Type" ); // (for Protect_Enum modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 630128a

   // [(start modifier_field_value)] 630128b
   dependents.insert( "Specification_Type" ); // (for Protect_Enum modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 630128b

   // [(start modifier_field_value)] 630129
   dependents.insert( "Specification_Type" ); // (for Protect_Other_Class modifier)

   if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)] 630129

   // [(start update_parent_cascade)] 630194
   dependents.insert( "Parent_Specification" );

   if( ( use_transients && is_field_transient( e_field_id_Parent_Specification ) )
    || ( !use_transients && !is_field_transient( e_field_id_Parent_Specification ) ) )
      names.insert( "Parent_Specification" );
   // [(finish update_parent_cascade)] 630194

   // [(start update_parent_cascade)] 640194
   dependents.insert( "Parent_Specification" );

   if( ( use_transients && is_field_transient( e_field_id_Parent_Specification ) )
    || ( !use_transients && !is_field_transient( e_field_id_Parent_Specification ) ) )
      names.insert( "Parent_Specification" );
   // [(finish update_parent_cascade)] 640194

   // [(start update_parent_cascade)] 650194
   dependents.insert( "Parent_Specification" );

   if( ( use_transients && is_field_transient( e_field_id_Parent_Specification ) )
    || ( !use_transients && !is_field_transient( e_field_id_Parent_Specification ) ) )
      names.insert( "Parent_Specification" );
   // [(finish update_parent_cascade)] 650194

   // [(start update_parent_cascade)] 660194
   dependents.insert( "Parent_Specification" );

   if( ( use_transients && is_field_transient( e_field_id_Parent_Specification ) )
    || ( !use_transients && !is_field_transient( e_field_id_Parent_Specification ) ) )
      names.insert( "Parent_Specification" );
   // [(finish update_parent_cascade)] 660194

   // [(start update_parent_cascade)] 670194
   dependents.insert( "Parent_Specification" );

   if( ( use_transients && is_field_transient( e_field_id_Parent_Specification ) )
    || ( !use_transients && !is_field_transient( e_field_id_Parent_Specification ) ) )
      names.insert( "Parent_Specification" );
   // [(finish update_parent_cascade)] 670194

   // [(start update_parent_cascade)] 680194
   dependents.insert( "Parent_Specification" );

   if( ( use_transients && is_field_transient( e_field_id_Parent_Specification ) )
    || ( !use_transients && !is_field_transient( e_field_id_Parent_Specification ) ) )
      names.insert( "Parent_Specification" );
   // [(finish update_parent_cascade)] 680194

   // [(start update_parent_cascade)] 690194
   dependents.insert( "Parent_Specification" );

   if( ( use_transients && is_field_transient( e_field_id_Parent_Specification ) )
    || ( !use_transients && !is_field_transient( e_field_id_Parent_Specification ) ) )
      names.insert( "Parent_Specification" );
   // [(finish update_parent_cascade)] 690194

   // [<start get_always_required_field_names>]
   // [<finish get_always_required_field_names>]
}

void Meta_Specification::get_transient_replacement_field_names( const string& name, vector< string >& names ) const
{
   ( void )name;
   ( void )names;

   // [<start get_transient_replacement_field_names>]
   // [<finish get_transient_replacement_field_names>]
}

void Meta_Specification::do_generate_sql( generate_sql_type type,
 vector< string >& sql_stmts, set< string >& tx_key_info, vector< string >* p_sql_undo_stmts ) const
{
   generate_sql( static_class_name( ), type, sql_stmts, tx_key_info, p_sql_undo_stmts );
}

const char* Meta_Specification::static_resolved_module_id( )
{
   return static_module_id( );
}

const char* Meta_Specification::static_resolved_module_name( )
{
   return static_module_name( );
}

const char* Meta_Specification::static_lock_class_id( )
{
   return "115100";
}

const char* Meta_Specification::static_check_class_name( )
{
   return "Specification";
}

const char* Meta_Specification::static_persistence_extra( )
{
   return "";
}

bool Meta_Specification::static_has_derivations( )
{
   return !g_derivations.empty( );
}

void Meta_Specification::static_get_class_info( class_info_container& class_info )
{
   class_info.push_back( "100.115100" );
}

void Meta_Specification::static_get_field_info( field_info_container& all_field_info )
{
   all_field_info.push_back( field_info( "115108", "Actions", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "115124", "All_Strings", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "115123", "All_Vars", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "115133", "Can_Access_Below_Model", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "301491", "Child_Rel_Child_Class", "Meta_Class", false, "", "" ) );
   all_field_info.push_back( field_info( "115121", "Child_Rel_Field_Key", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "301475", "Child_Relationship", "Meta_Relationship", false, "", "" ) );
   all_field_info.push_back( field_info( "301406", "Child_Specification_Type", "Meta_Specification_Type", false, "", "" ) );
   all_field_info.push_back( field_info( "301420", "Class", "Meta_Class", true, "", "" ) );
   all_field_info.push_back( field_info( "115112", "Comments", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "301460", "Enum", "Meta_Enum", false, "", "" ) );
   all_field_info.push_back( field_info( "301470", "Enum_Item", "Meta_Enum_Item", false, "", "" ) );
   all_field_info.push_back( field_info( "301471", "Enum_Item_2", "Meta_Enum_Item", false, "", "" ) );
   all_field_info.push_back( field_info( "301472", "Enum_Item_3", "Meta_Enum_Item", false, "", "" ) );
   all_field_info.push_back( field_info( "301473", "Enum_Item_4", "Meta_Enum_Item", false, "", "" ) );
   all_field_info.push_back( field_info( "301474", "Enum_Item_5", "Meta_Enum_Item", false, "", "" ) );
   all_field_info.push_back( field_info( "301430", "Field", "Meta_Field", false, "", "" ) );
   all_field_info.push_back( field_info( "301492", "Field_Class", "Meta_Class", false, "", "" ) );
   all_field_info.push_back( field_info( "115120", "Field_Pairs", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "115113", "Field_Values", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "115126", "Fields", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "115117", "Id", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "115128", "Is_Required_For_UI_Gen", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "301400", "Model", "Meta_Model", true, "", "" ) );
   all_field_info.push_back( field_info( "301440", "Modifier", "Meta_Modifier", false, "", "" ) );
   all_field_info.push_back( field_info( "115101", "Name", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "115104", "Options", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "115105", "Order", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "301421", "Other_Class", "Meta_Class", false, "", "" ) );
   all_field_info.push_back( field_info( "301437", "Other_Class_Field", "Meta_Field", false, "", "" ) );
   all_field_info.push_back( field_info( "301435", "Other_Field", "Meta_Field", false, "", "" ) );
   all_field_info.push_back( field_info( "301436", "Other_Field_2", "Meta_Field", false, "", "" ) );
   all_field_info.push_back( field_info( "301441", "Other_Modifier", "Meta_Modifier", false, "", "" ) );
   all_field_info.push_back( field_info( "301442", "Other_Modifier_2", "Meta_Modifier", false, "", "" ) );
   all_field_info.push_back( field_info( "301331", "Other_Permission", "Meta_Permission", false, "", "" ) );
   all_field_info.push_back( field_info( "301332", "Other_Permission_2", "Meta_Permission", false, "", "" ) );
   all_field_info.push_back( field_info( "301451", "Other_Procedure", "Meta_Procedure", false, "", "" ) );
   all_field_info.push_back( field_info( "301452", "Other_Procedure_2", "Meta_Procedure", false, "", "" ) );
   all_field_info.push_back( field_info( "301496", "Other_Source_Child", "Meta_Field", false, "", "" ) );
   all_field_info.push_back( field_info( "301497", "Other_Source_Child_2", "Meta_Field", false, "", "" ) );
   all_field_info.push_back( field_info( "301405", "Parent_Specification", "Meta_Specification", false, "", "" ) );
   all_field_info.push_back( field_info( "301499", "Permission", "Meta_Permission", false, "", "" ) );
   all_field_info.push_back( field_info( "301450", "Procedure", "Meta_Procedure", false, "", "" ) );
   all_field_info.push_back( field_info( "301498", "Procedure_Arg", "Meta_Procedure_Arg", false, "", "" ) );
   all_field_info.push_back( field_info( "301477", "Procedure_Arg_2", "Meta_Procedure_Arg", false, "", "" ) );
   all_field_info.push_back( field_info( "301478", "Procedure_Arg_3", "Meta_Procedure_Arg", false, "", "" ) );
   all_field_info.push_back( field_info( "115122", "Protect_Child_Rel", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "115115", "Protect_Procedure", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "115111", "Protect_Source_Parent", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "115118", "Restrict_Values", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "301495", "Source_Child", "Meta_Field", false, "", "" ) );
   all_field_info.push_back( field_info( "301489", "Source_Child_Class", "Meta_Class", false, "", "" ) );
   all_field_info.push_back( field_info( "301494", "Source_Class", "Meta_Class", false, "", "" ) );
   all_field_info.push_back( field_info( "301481", "Source_Field", "Meta_Field", false, "", "" ) );
   all_field_info.push_back( field_info( "301482", "Source_Field_Class", "Meta_Class", false, "", "" ) );
   all_field_info.push_back( field_info( "301476", "Source_Grandchild", "Meta_Field", false, "", "" ) );
   all_field_info.push_back( field_info( "301480", "Source_Parent", "Meta_Field", false, "", "" ) );
   all_field_info.push_back( field_info( "301490", "Source_Parent_Class", "Meta_Class", false, "", "" ) );
   all_field_info.push_back( field_info( "301410", "Specification_Type", "Meta_Specification_Type", true, "", "" ) );
   all_field_info.push_back( field_info( "115119", "Strings", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "301488", "Test_Child", "Meta_Field", false, "", "" ) );
   all_field_info.push_back( field_info( "301485", "Test_Field", "Meta_Field", false, "", "" ) );
   all_field_info.push_back( field_info( "301493", "Test_Field_Class", "Meta_Class", false, "", "" ) );
   all_field_info.push_back( field_info( "301486", "Test_Parent", "Meta_Field", false, "", "" ) );
   all_field_info.push_back( field_info( "301487", "Test_Parent_Class", "Meta_Class", false, "", "" ) );
   all_field_info.push_back( field_info( "115103", "Test_Value", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "115132", "Test_Value_Label", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "115107", "Use_Source_Parent", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "115110", "Use_Test_Parent_Child", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "115102", "Value", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "115131", "Value_Label", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "115129", "Value_Left_Part", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "115116", "Value_Literal", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "115127", "Value_Numeric_String", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "115130", "Value_Right_Part", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "115125", "Value_String", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "115114", "Vars", "string", false, "", "" ) );
}

void Meta_Specification::static_get_foreign_key_info( foreign_key_info_container& foreign_key_info )
{
   ( void )foreign_key_info;

   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Child_Rel_Child_Class, make_pair( "Meta.115100", "Meta_Class" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Child_Relationship, make_pair( "Meta.115100", "Meta_Relationship" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Child_Specification_Type, make_pair( "Meta.115100", "Meta_Specification_Type" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Class, make_pair( "Meta.115100", "Meta_Class" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Enum, make_pair( "Meta.115100", "Meta_Enum" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Enum_Item, make_pair( "Meta.115100", "Meta_Enum_Item" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Enum_Item_2, make_pair( "Meta.115100", "Meta_Enum_Item" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Enum_Item_3, make_pair( "Meta.115100", "Meta_Enum_Item" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Enum_Item_4, make_pair( "Meta.115100", "Meta_Enum_Item" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Enum_Item_5, make_pair( "Meta.115100", "Meta_Enum_Item" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Field, make_pair( "Meta.115100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Field_Class, make_pair( "Meta.115100", "Meta_Class" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Model, make_pair( "Meta.115100", "Meta_Model" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Modifier, make_pair( "Meta.115100", "Meta_Modifier" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Other_Class, make_pair( "Meta.115100", "Meta_Class" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Other_Class_Field, make_pair( "Meta.115100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Other_Field, make_pair( "Meta.115100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Other_Field_2, make_pair( "Meta.115100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Other_Modifier, make_pair( "Meta.115100", "Meta_Modifier" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Other_Modifier_2, make_pair( "Meta.115100", "Meta_Modifier" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Other_Permission, make_pair( "Meta.115100", "Meta_Permission" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Other_Permission_2, make_pair( "Meta.115100", "Meta_Permission" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Other_Procedure, make_pair( "Meta.115100", "Meta_Procedure" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Other_Procedure_2, make_pair( "Meta.115100", "Meta_Procedure" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Other_Source_Child, make_pair( "Meta.115100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Other_Source_Child_2, make_pair( "Meta.115100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Parent_Specification, make_pair( "Meta.115100", "Meta_Specification" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Permission, make_pair( "Meta.115100", "Meta_Permission" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Procedure, make_pair( "Meta.115100", "Meta_Procedure" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Procedure_Arg, make_pair( "Meta.115100", "Meta_Procedure_Arg" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Procedure_Arg_2, make_pair( "Meta.115100", "Meta_Procedure_Arg" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Procedure_Arg_3, make_pair( "Meta.115100", "Meta_Procedure_Arg" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Source_Child, make_pair( "Meta.115100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Source_Child_Class, make_pair( "Meta.115100", "Meta_Class" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Source_Class, make_pair( "Meta.115100", "Meta_Class" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Source_Field, make_pair( "Meta.115100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Source_Field_Class, make_pair( "Meta.115100", "Meta_Class" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Source_Grandchild, make_pair( "Meta.115100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Source_Parent, make_pair( "Meta.115100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Source_Parent_Class, make_pair( "Meta.115100", "Meta_Class" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Specification_Type, make_pair( "Meta.115100", "Meta_Specification_Type" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Test_Child, make_pair( "Meta.115100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Test_Field, make_pair( "Meta.115100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Test_Field_Class, make_pair( "Meta.115100", "Meta_Class" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Test_Parent, make_pair( "Meta.115100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Test_Parent_Class, make_pair( "Meta.115100", "Meta_Class" ) ) );
}

int Meta_Specification::static_get_num_fields( bool* p_done, const string* p_class_name )
{
   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;

   return c_num_fields;
}

bool Meta_Specification::static_is_field_encrypted( field_id id )
{
   return is_encrypted_field( static_get_field_id( id ) );
}

bool Meta_Specification::static_is_field_transient( field_id id )
{
   return is_transient_field( static_get_field_id( id ) );
}

const char* Meta_Specification::static_get_field_id( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "115108";
      break;

      case 2:
      p_id = "115124";
      break;

      case 3:
      p_id = "115123";
      break;

      case 4:
      p_id = "115133";
      break;

      case 5:
      p_id = "301491";
      break;

      case 6:
      p_id = "115121";
      break;

      case 7:
      p_id = "301475";
      break;

      case 8:
      p_id = "301406";
      break;

      case 9:
      p_id = "301420";
      break;

      case 10:
      p_id = "115112";
      break;

      case 11:
      p_id = "301460";
      break;

      case 12:
      p_id = "301470";
      break;

      case 13:
      p_id = "301471";
      break;

      case 14:
      p_id = "301472";
      break;

      case 15:
      p_id = "301473";
      break;

      case 16:
      p_id = "301474";
      break;

      case 17:
      p_id = "301430";
      break;

      case 18:
      p_id = "301492";
      break;

      case 19:
      p_id = "115120";
      break;

      case 20:
      p_id = "115113";
      break;

      case 21:
      p_id = "115126";
      break;

      case 22:
      p_id = "115117";
      break;

      case 23:
      p_id = "115128";
      break;

      case 24:
      p_id = "301400";
      break;

      case 25:
      p_id = "301440";
      break;

      case 26:
      p_id = "115101";
      break;

      case 27:
      p_id = "115104";
      break;

      case 28:
      p_id = "115105";
      break;

      case 29:
      p_id = "301421";
      break;

      case 30:
      p_id = "301437";
      break;

      case 31:
      p_id = "301435";
      break;

      case 32:
      p_id = "301436";
      break;

      case 33:
      p_id = "301441";
      break;

      case 34:
      p_id = "301442";
      break;

      case 35:
      p_id = "301331";
      break;

      case 36:
      p_id = "301332";
      break;

      case 37:
      p_id = "301451";
      break;

      case 38:
      p_id = "301452";
      break;

      case 39:
      p_id = "301496";
      break;

      case 40:
      p_id = "301497";
      break;

      case 41:
      p_id = "301405";
      break;

      case 42:
      p_id = "301499";
      break;

      case 43:
      p_id = "301450";
      break;

      case 44:
      p_id = "301498";
      break;

      case 45:
      p_id = "301477";
      break;

      case 46:
      p_id = "301478";
      break;

      case 47:
      p_id = "115122";
      break;

      case 48:
      p_id = "115115";
      break;

      case 49:
      p_id = "115111";
      break;

      case 50:
      p_id = "115118";
      break;

      case 51:
      p_id = "301495";
      break;

      case 52:
      p_id = "301489";
      break;

      case 53:
      p_id = "301494";
      break;

      case 54:
      p_id = "301481";
      break;

      case 55:
      p_id = "301482";
      break;

      case 56:
      p_id = "301476";
      break;

      case 57:
      p_id = "301480";
      break;

      case 58:
      p_id = "301490";
      break;

      case 59:
      p_id = "301410";
      break;

      case 60:
      p_id = "115119";
      break;

      case 61:
      p_id = "301488";
      break;

      case 62:
      p_id = "301485";
      break;

      case 63:
      p_id = "301493";
      break;

      case 64:
      p_id = "301486";
      break;

      case 65:
      p_id = "301487";
      break;

      case 66:
      p_id = "115103";
      break;

      case 67:
      p_id = "115132";
      break;

      case 68:
      p_id = "115107";
      break;

      case 69:
      p_id = "115110";
      break;

      case 70:
      p_id = "115102";
      break;

      case 71:
      p_id = "115131";
      break;

      case 72:
      p_id = "115129";
      break;

      case 73:
      p_id = "115116";
      break;

      case 74:
      p_id = "115127";
      break;

      case 75:
      p_id = "115130";
      break;

      case 76:
      p_id = "115125";
      break;

      case 77:
      p_id = "115114";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Specification" );

   return p_id;
}

const char* Meta_Specification::static_get_field_name( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "Actions";
      break;

      case 2:
      p_id = "All_Strings";
      break;

      case 3:
      p_id = "All_Vars";
      break;

      case 4:
      p_id = "Can_Access_Below_Model";
      break;

      case 5:
      p_id = "Child_Rel_Child_Class";
      break;

      case 6:
      p_id = "Child_Rel_Field_Key";
      break;

      case 7:
      p_id = "Child_Relationship";
      break;

      case 8:
      p_id = "Child_Specification_Type";
      break;

      case 9:
      p_id = "Class";
      break;

      case 10:
      p_id = "Comments";
      break;

      case 11:
      p_id = "Enum";
      break;

      case 12:
      p_id = "Enum_Item";
      break;

      case 13:
      p_id = "Enum_Item_2";
      break;

      case 14:
      p_id = "Enum_Item_3";
      break;

      case 15:
      p_id = "Enum_Item_4";
      break;

      case 16:
      p_id = "Enum_Item_5";
      break;

      case 17:
      p_id = "Field";
      break;

      case 18:
      p_id = "Field_Class";
      break;

      case 19:
      p_id = "Field_Pairs";
      break;

      case 20:
      p_id = "Field_Values";
      break;

      case 21:
      p_id = "Fields";
      break;

      case 22:
      p_id = "Id";
      break;

      case 23:
      p_id = "Is_Required_For_UI_Gen";
      break;

      case 24:
      p_id = "Model";
      break;

      case 25:
      p_id = "Modifier";
      break;

      case 26:
      p_id = "Name";
      break;

      case 27:
      p_id = "Options";
      break;

      case 28:
      p_id = "Order";
      break;

      case 29:
      p_id = "Other_Class";
      break;

      case 30:
      p_id = "Other_Class_Field";
      break;

      case 31:
      p_id = "Other_Field";
      break;

      case 32:
      p_id = "Other_Field_2";
      break;

      case 33:
      p_id = "Other_Modifier";
      break;

      case 34:
      p_id = "Other_Modifier_2";
      break;

      case 35:
      p_id = "Other_Permission";
      break;

      case 36:
      p_id = "Other_Permission_2";
      break;

      case 37:
      p_id = "Other_Procedure";
      break;

      case 38:
      p_id = "Other_Procedure_2";
      break;

      case 39:
      p_id = "Other_Source_Child";
      break;

      case 40:
      p_id = "Other_Source_Child_2";
      break;

      case 41:
      p_id = "Parent_Specification";
      break;

      case 42:
      p_id = "Permission";
      break;

      case 43:
      p_id = "Procedure";
      break;

      case 44:
      p_id = "Procedure_Arg";
      break;

      case 45:
      p_id = "Procedure_Arg_2";
      break;

      case 46:
      p_id = "Procedure_Arg_3";
      break;

      case 47:
      p_id = "Protect_Child_Rel";
      break;

      case 48:
      p_id = "Protect_Procedure";
      break;

      case 49:
      p_id = "Protect_Source_Parent";
      break;

      case 50:
      p_id = "Restrict_Values";
      break;

      case 51:
      p_id = "Source_Child";
      break;

      case 52:
      p_id = "Source_Child_Class";
      break;

      case 53:
      p_id = "Source_Class";
      break;

      case 54:
      p_id = "Source_Field";
      break;

      case 55:
      p_id = "Source_Field_Class";
      break;

      case 56:
      p_id = "Source_Grandchild";
      break;

      case 57:
      p_id = "Source_Parent";
      break;

      case 58:
      p_id = "Source_Parent_Class";
      break;

      case 59:
      p_id = "Specification_Type";
      break;

      case 60:
      p_id = "Strings";
      break;

      case 61:
      p_id = "Test_Child";
      break;

      case 62:
      p_id = "Test_Field";
      break;

      case 63:
      p_id = "Test_Field_Class";
      break;

      case 64:
      p_id = "Test_Parent";
      break;

      case 65:
      p_id = "Test_Parent_Class";
      break;

      case 66:
      p_id = "Test_Value";
      break;

      case 67:
      p_id = "Test_Value_Label";
      break;

      case 68:
      p_id = "Use_Source_Parent";
      break;

      case 69:
      p_id = "Use_Test_Parent_Child";
      break;

      case 70:
      p_id = "Value";
      break;

      case 71:
      p_id = "Value_Label";
      break;

      case 72:
      p_id = "Value_Left_Part";
      break;

      case 73:
      p_id = "Value_Literal";
      break;

      case 74:
      p_id = "Value_Numeric_String";
      break;

      case 75:
      p_id = "Value_Right_Part";
      break;

      case 76:
      p_id = "Value_String";
      break;

      case 77:
      p_id = "Vars";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Specification" );

   return p_id;
}

int Meta_Specification::static_get_field_num( const string& field )
{
   int rc = 0;

   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for static_get_field_num( )" );
   else if( field == c_field_id_Actions || field == c_field_name_Actions )
      rc += 1;
   else if( field == c_field_id_All_Strings || field == c_field_name_All_Strings )
      rc += 2;
   else if( field == c_field_id_All_Vars || field == c_field_name_All_Vars )
      rc += 3;
   else if( field == c_field_id_Can_Access_Below_Model || field == c_field_name_Can_Access_Below_Model )
      rc += 4;
   else if( field == c_field_id_Child_Rel_Child_Class || field == c_field_name_Child_Rel_Child_Class )
      rc += 5;
   else if( field == c_field_id_Child_Rel_Field_Key || field == c_field_name_Child_Rel_Field_Key )
      rc += 6;
   else if( field == c_field_id_Child_Relationship || field == c_field_name_Child_Relationship )
      rc += 7;
   else if( field == c_field_id_Child_Specification_Type || field == c_field_name_Child_Specification_Type )
      rc += 8;
   else if( field == c_field_id_Class || field == c_field_name_Class )
      rc += 9;
   else if( field == c_field_id_Comments || field == c_field_name_Comments )
      rc += 10;
   else if( field == c_field_id_Enum || field == c_field_name_Enum )
      rc += 11;
   else if( field == c_field_id_Enum_Item || field == c_field_name_Enum_Item )
      rc += 12;
   else if( field == c_field_id_Enum_Item_2 || field == c_field_name_Enum_Item_2 )
      rc += 13;
   else if( field == c_field_id_Enum_Item_3 || field == c_field_name_Enum_Item_3 )
      rc += 14;
   else if( field == c_field_id_Enum_Item_4 || field == c_field_name_Enum_Item_4 )
      rc += 15;
   else if( field == c_field_id_Enum_Item_5 || field == c_field_name_Enum_Item_5 )
      rc += 16;
   else if( field == c_field_id_Field || field == c_field_name_Field )
      rc += 17;
   else if( field == c_field_id_Field_Class || field == c_field_name_Field_Class )
      rc += 18;
   else if( field == c_field_id_Field_Pairs || field == c_field_name_Field_Pairs )
      rc += 19;
   else if( field == c_field_id_Field_Values || field == c_field_name_Field_Values )
      rc += 20;
   else if( field == c_field_id_Fields || field == c_field_name_Fields )
      rc += 21;
   else if( field == c_field_id_Id || field == c_field_name_Id )
      rc += 22;
   else if( field == c_field_id_Is_Required_For_UI_Gen || field == c_field_name_Is_Required_For_UI_Gen )
      rc += 23;
   else if( field == c_field_id_Model || field == c_field_name_Model )
      rc += 24;
   else if( field == c_field_id_Modifier || field == c_field_name_Modifier )
      rc += 25;
   else if( field == c_field_id_Name || field == c_field_name_Name )
      rc += 26;
   else if( field == c_field_id_Options || field == c_field_name_Options )
      rc += 27;
   else if( field == c_field_id_Order || field == c_field_name_Order )
      rc += 28;
   else if( field == c_field_id_Other_Class || field == c_field_name_Other_Class )
      rc += 29;
   else if( field == c_field_id_Other_Class_Field || field == c_field_name_Other_Class_Field )
      rc += 30;
   else if( field == c_field_id_Other_Field || field == c_field_name_Other_Field )
      rc += 31;
   else if( field == c_field_id_Other_Field_2 || field == c_field_name_Other_Field_2 )
      rc += 32;
   else if( field == c_field_id_Other_Modifier || field == c_field_name_Other_Modifier )
      rc += 33;
   else if( field == c_field_id_Other_Modifier_2 || field == c_field_name_Other_Modifier_2 )
      rc += 34;
   else if( field == c_field_id_Other_Permission || field == c_field_name_Other_Permission )
      rc += 35;
   else if( field == c_field_id_Other_Permission_2 || field == c_field_name_Other_Permission_2 )
      rc += 36;
   else if( field == c_field_id_Other_Procedure || field == c_field_name_Other_Procedure )
      rc += 37;
   else if( field == c_field_id_Other_Procedure_2 || field == c_field_name_Other_Procedure_2 )
      rc += 38;
   else if( field == c_field_id_Other_Source_Child || field == c_field_name_Other_Source_Child )
      rc += 39;
   else if( field == c_field_id_Other_Source_Child_2 || field == c_field_name_Other_Source_Child_2 )
      rc += 40;
   else if( field == c_field_id_Parent_Specification || field == c_field_name_Parent_Specification )
      rc += 41;
   else if( field == c_field_id_Permission || field == c_field_name_Permission )
      rc += 42;
   else if( field == c_field_id_Procedure || field == c_field_name_Procedure )
      rc += 43;
   else if( field == c_field_id_Procedure_Arg || field == c_field_name_Procedure_Arg )
      rc += 44;
   else if( field == c_field_id_Procedure_Arg_2 || field == c_field_name_Procedure_Arg_2 )
      rc += 45;
   else if( field == c_field_id_Procedure_Arg_3 || field == c_field_name_Procedure_Arg_3 )
      rc += 46;
   else if( field == c_field_id_Protect_Child_Rel || field == c_field_name_Protect_Child_Rel )
      rc += 47;
   else if( field == c_field_id_Protect_Procedure || field == c_field_name_Protect_Procedure )
      rc += 48;
   else if( field == c_field_id_Protect_Source_Parent || field == c_field_name_Protect_Source_Parent )
      rc += 49;
   else if( field == c_field_id_Restrict_Values || field == c_field_name_Restrict_Values )
      rc += 50;
   else if( field == c_field_id_Source_Child || field == c_field_name_Source_Child )
      rc += 51;
   else if( field == c_field_id_Source_Child_Class || field == c_field_name_Source_Child_Class )
      rc += 52;
   else if( field == c_field_id_Source_Class || field == c_field_name_Source_Class )
      rc += 53;
   else if( field == c_field_id_Source_Field || field == c_field_name_Source_Field )
      rc += 54;
   else if( field == c_field_id_Source_Field_Class || field == c_field_name_Source_Field_Class )
      rc += 55;
   else if( field == c_field_id_Source_Grandchild || field == c_field_name_Source_Grandchild )
      rc += 56;
   else if( field == c_field_id_Source_Parent || field == c_field_name_Source_Parent )
      rc += 57;
   else if( field == c_field_id_Source_Parent_Class || field == c_field_name_Source_Parent_Class )
      rc += 58;
   else if( field == c_field_id_Specification_Type || field == c_field_name_Specification_Type )
      rc += 59;
   else if( field == c_field_id_Strings || field == c_field_name_Strings )
      rc += 60;
   else if( field == c_field_id_Test_Child || field == c_field_name_Test_Child )
      rc += 61;
   else if( field == c_field_id_Test_Field || field == c_field_name_Test_Field )
      rc += 62;
   else if( field == c_field_id_Test_Field_Class || field == c_field_name_Test_Field_Class )
      rc += 63;
   else if( field == c_field_id_Test_Parent || field == c_field_name_Test_Parent )
      rc += 64;
   else if( field == c_field_id_Test_Parent_Class || field == c_field_name_Test_Parent_Class )
      rc += 65;
   else if( field == c_field_id_Test_Value || field == c_field_name_Test_Value )
      rc += 66;
   else if( field == c_field_id_Test_Value_Label || field == c_field_name_Test_Value_Label )
      rc += 67;
   else if( field == c_field_id_Use_Source_Parent || field == c_field_name_Use_Source_Parent )
      rc += 68;
   else if( field == c_field_id_Use_Test_Parent_Child || field == c_field_name_Use_Test_Parent_Child )
      rc += 69;
   else if( field == c_field_id_Value || field == c_field_name_Value )
      rc += 70;
   else if( field == c_field_id_Value_Label || field == c_field_name_Value_Label )
      rc += 71;
   else if( field == c_field_id_Value_Left_Part || field == c_field_name_Value_Left_Part )
      rc += 72;
   else if( field == c_field_id_Value_Literal || field == c_field_name_Value_Literal )
      rc += 73;
   else if( field == c_field_id_Value_Numeric_String || field == c_field_name_Value_Numeric_String )
      rc += 74;
   else if( field == c_field_id_Value_Right_Part || field == c_field_name_Value_Right_Part )
      rc += 75;
   else if( field == c_field_id_Value_String || field == c_field_name_Value_String )
      rc += 76;
   else if( field == c_field_id_Vars || field == c_field_name_Vars )
      rc += 77;

   return rc - 1;
}

procedure_info_container& Meta_Specification::static_get_procedure_info( )
{
   static bool initialised = false;
   static procedure_info_container procedures;

   if( !initialised )
   {
      initialised = true;
      procedures.insert( make_pair( "115439", procedure_info( "Add_Arg_1" ) ) );
      procedures.insert( make_pair( "115440", procedure_info( "Add_Arg_2" ) ) );
      procedures.insert( make_pair( "115432", procedure_info( "Add_Child_Info" ) ) );
      procedures.insert( make_pair( "115447", procedure_info( "Add_Destination_Info" ) ) );
      procedures.insert( make_pair( "115443", procedure_info( "Add_Extra_Info" ) ) );
      procedures.insert( make_pair( "115430", procedure_info( "Add_Field" ) ) );
      procedures.insert( make_pair( "115446", procedure_info( "Add_Field_Info" ) ) );
      procedures.insert( make_pair( "115431", procedure_info( "Add_Field_Pair" ) ) );
      procedures.insert( make_pair( "115438", procedure_info( "Add_Grandchild_Info" ) ) );
      procedures.insert( make_pair( "115441", procedure_info( "Add_Info" ) ) );
      procedures.insert( make_pair( "115444", procedure_info( "Add_Next" ) ) );
      procedures.insert( make_pair( "115442", procedure_info( "Add_Opt_Info" ) ) );
      procedures.insert( make_pair( "115445", procedure_info( "Add_Other_Info" ) ) );
      procedures.insert( make_pair( "115433", procedure_info( "Add_Secondary" ) ) );
      procedures.insert( make_pair( "115437", procedure_info( "Add_Source_Info" ) ) );
      procedures.insert( make_pair( "115434", procedure_info( "Add_Stats_1" ) ) );
      procedures.insert( make_pair( "115435", procedure_info( "Add_Stats_2" ) ) );
      procedures.insert( make_pair( "115436", procedure_info( "Add_Stats_Info" ) ) );
      procedures.insert( make_pair( "115510", procedure_info( "Generate_All_Strings" ) ) );
      procedures.insert( make_pair( "115500", procedure_info( "Generate_All_Vars" ) ) );
      procedures.insert( make_pair( "115420", procedure_info( "Move_Down" ) ) );
      procedures.insert( make_pair( "115410", procedure_info( "Move_Up" ) ) );
   }

   return procedures;
}

string Meta_Specification::static_get_sql_columns( )
{
   string sql_columns;

   sql_columns += 
    "C_Key_ VARCHAR(75),"
    "C_Ver_ SMALLINT UNSIGNED NOT NULL,"
    "C_Rev_ BIGINT UNSIGNED NOT NULL,"
    "C_Sec_ BIGINT UNSIGNED NOT NULL,"
    "C_Typ_ VARCHAR(16) NOT NULL,"
    "C_Actions VARCHAR(200) NOT NULL,"
    "C_Can_Access_Below_Model INTEGER NOT NULL,"
    "C_Child_Rel_Child_Class VARCHAR(75) NOT NULL,"
    "C_Child_Rel_Field_Key VARCHAR(200) NOT NULL,"
    "C_Child_Relationship VARCHAR(75) NOT NULL,"
    "C_Child_Specification_Type VARCHAR(75) NOT NULL,"
    "C_Class VARCHAR(75) NOT NULL,"
    "C_Comments VARCHAR(200) NOT NULL,"
    "C_Enum VARCHAR(75) NOT NULL,"
    "C_Enum_Item VARCHAR(75) NOT NULL,"
    "C_Enum_Item_2 VARCHAR(75) NOT NULL,"
    "C_Enum_Item_3 VARCHAR(75) NOT NULL,"
    "C_Enum_Item_4 VARCHAR(75) NOT NULL,"
    "C_Enum_Item_5 VARCHAR(75) NOT NULL,"
    "C_Field VARCHAR(75) NOT NULL,"
    "C_Field_Class VARCHAR(75) NOT NULL,"
    "C_Field_Pairs TEXT NOT NULL,"
    "C_Field_Values TEXT NOT NULL,"
    "C_Fields TEXT NOT NULL,"
    "C_Id VARCHAR(200) NOT NULL,"
    "C_Is_Required_For_UI_Gen INTEGER NOT NULL,"
    "C_Model VARCHAR(75) NOT NULL,"
    "C_Modifier VARCHAR(75) NOT NULL,"
    "C_Name VARCHAR(200) NOT NULL,"
    "C_Options VARCHAR(256) NOT NULL,"
    "C_Order VARCHAR(200) NOT NULL,"
    "C_Other_Class VARCHAR(256) NOT NULL,"
    "C_Other_Class_Field VARCHAR(256) NOT NULL,"
    "C_Other_Field VARCHAR(256) NOT NULL,"
    "C_Other_Field_2 VARCHAR(256) NOT NULL,"
    "C_Other_Modifier VARCHAR(256) NOT NULL,"
    "C_Other_Modifier_2 VARCHAR(256) NOT NULL,"
    "C_Other_Permission VARCHAR(256) NOT NULL,"
    "C_Other_Permission_2 VARCHAR(256) NOT NULL,"
    "C_Other_Procedure VARCHAR(256) NOT NULL,"
    "C_Other_Procedure_2 VARCHAR(256) NOT NULL,"
    "C_Other_Source_Child VARCHAR(256) NOT NULL,"
    "C_Other_Source_Child_2 VARCHAR(256) NOT NULL,"
    "C_Parent_Specification VARCHAR(75) NOT NULL,"
    "C_Permission VARCHAR(256) NOT NULL,"
    "C_Procedure VARCHAR(256) NOT NULL,"
    "C_Procedure_Arg VARCHAR(256) NOT NULL,"
    "C_Procedure_Arg_2 VARCHAR(256) NOT NULL,"
    "C_Procedure_Arg_3 VARCHAR(256) NOT NULL,"
    "C_Protect_Child_Rel INTEGER NOT NULL,"
    "C_Protect_Procedure INTEGER NOT NULL,"
    "C_Protect_Source_Parent INTEGER NOT NULL,"
    "C_Restrict_Values VARCHAR(256) NOT NULL,"
    "C_Source_Child VARCHAR(256) NOT NULL,"
    "C_Source_Child_Class VARCHAR(256) NOT NULL,"
    "C_Source_Class VARCHAR(256) NOT NULL,"
    "C_Source_Field VARCHAR(256) NOT NULL,"
    "C_Source_Field_Class VARCHAR(256) NOT NULL,"
    "C_Source_Grandchild VARCHAR(256) NOT NULL,"
    "C_Source_Parent VARCHAR(256) NOT NULL,"
    "C_Source_Parent_Class VARCHAR(256) NOT NULL,"
    "C_Specification_Type VARCHAR(75) NOT NULL,"
    "C_Test_Child VARCHAR(256) NOT NULL,"
    "C_Test_Field VARCHAR(256) NOT NULL,"
    "C_Test_Field_Class VARCHAR(256) NOT NULL,"
    "C_Test_Parent VARCHAR(256) NOT NULL,"
    "C_Test_Parent_Class VARCHAR(256) NOT NULL,"
    "C_Test_Value VARCHAR(256) NOT NULL,"
    "C_Test_Value_Label VARCHAR(256) NOT NULL,"
    "C_Use_Source_Parent INTEGER NOT NULL,"
    "C_Use_Test_Parent_Child INTEGER NOT NULL,"
    "C_Value VARCHAR(256) NOT NULL,"
    "C_Value_Label VARCHAR(256) NOT NULL,"
    "C_Value_Left_Part VARCHAR(256) NOT NULL,"
    "C_Value_Literal VARCHAR(256) NOT NULL,"
    "C_Value_Numeric_String VARCHAR(256) NOT NULL,"
    "C_Value_Right_Part VARCHAR(256) NOT NULL,"
    "C_Value_String VARCHAR(256) NOT NULL,"
    "PRIMARY KEY(C_Key_)";

   return sql_columns;
}

void Meta_Specification::static_get_text_search_fields( vector< string >& fields )
{
   ( void )fields;
}

void Meta_Specification::static_get_all_enum_pairs( vector< pair< string, string > >& pairs )
{
   ( void )pairs;
}

void Meta_Specification::static_get_all_index_pairs( vector< pair< string, string > >& pairs )
{
   pairs.push_back( make_pair( "Class,Order,@pk", "string,string,string" ) );
   pairs.push_back( make_pair( "Class,Parent_Specification,Order", "string,string,string" ) );
   pairs.push_back( make_pair( "Class,Specification_Type,@pk", "string,string,string" ) );
   pairs.push_back( make_pair( "Field,@pk", "string,string" ) );
   pairs.push_back( make_pair( "Model,Id", "string,string" ) );
   pairs.push_back( make_pair( "Model,Order", "string,string" ) );
   pairs.push_back( make_pair( "Model,Parent_Specification,Can_Access_Below_Model,Order", "string,string,bool,string" ) );
   pairs.push_back( make_pair( "Model,Parent_Specification,Id", "string,string,string" ) );
   pairs.push_back( make_pair( "Model,Parent_Specification,Name", "string,string,string" ) );
   pairs.push_back( make_pair( "Model,Parent_Specification,Order", "string,string,string" ) );
   pairs.push_back( make_pair( "Parent_Specification,@pk", "string,string" ) );
   pairs.push_back( make_pair( "Parent_Specification,Order", "string,string" ) );
}

void Meta_Specification::static_get_all_unique_indexes( vector< string >& unique_indexes )
{
   unique_indexes.push_back( "Class,Parent_Specification,Order" );
   unique_indexes.push_back( "Model,Id" );
   unique_indexes.push_back( "Model,Order" );
   unique_indexes.push_back( "Model,Parent_Specification,Can_Access_Below_Model,Order" );
   unique_indexes.push_back( "Model,Parent_Specification,Id" );
   unique_indexes.push_back( "Model,Parent_Specification,Name" );
   unique_indexes.push_back( "Model,Parent_Specification,Order" );
   unique_indexes.push_back( "Parent_Specification,Order" );
}

void Meta_Specification::static_get_sql_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_Class,C_Order,C_Key_" );
   indexes.push_back( "C_Class,C_Parent_Specification,C_Order" );
   indexes.push_back( "C_Class,C_Specification_Type,C_Key_" );
   indexes.push_back( "C_Field,C_Key_" );
   indexes.push_back( "C_Model,C_Id" );
   indexes.push_back( "C_Model,C_Order" );
   indexes.push_back( "C_Model,C_Parent_Specification,C_Can_Access_Below_Model,C_Order" );
   indexes.push_back( "C_Model,C_Parent_Specification,C_Id" );
   indexes.push_back( "C_Model,C_Parent_Specification,C_Name" );
   indexes.push_back( "C_Model,C_Parent_Specification,C_Order" );
   indexes.push_back( "C_Parent_Specification,C_Key_" );
   indexes.push_back( "C_Parent_Specification,C_Order" );
}

void Meta_Specification::static_get_sql_unique_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_Class,C_Parent_Specification,C_Order" );
   indexes.push_back( "C_Model,C_Id" );
   indexes.push_back( "C_Model,C_Order" );
   indexes.push_back( "C_Model,C_Parent_Specification,C_Can_Access_Below_Model,C_Order" );
   indexes.push_back( "C_Model,C_Parent_Specification,C_Id" );
   indexes.push_back( "C_Model,C_Parent_Specification,C_Name" );
   indexes.push_back( "C_Model,C_Parent_Specification,C_Order" );
   indexes.push_back( "C_Parent_Specification,C_Order" );
}

void Meta_Specification::static_insert_derivation( const string& module_and_class_id )
{
   g_derivations.insert( module_and_class_id );
}

void Meta_Specification::static_remove_derivation( const string& module_and_class_id )
{
   if( g_derivations.count( module_and_class_id ) )
      g_derivations.erase( module_and_class_id );
}

void Meta_Specification::static_insert_external_alias( const string& module_and_class_id, Meta_Specification* p_instance )
{
   g_external_aliases.insert( external_aliases_value_type( module_and_class_id, p_instance ) );
}

void Meta_Specification::static_remove_external_alias( const string& module_and_class_id )
{
   if( g_external_aliases.count( module_and_class_id ) )
   {
      delete g_external_aliases[ module_and_class_id ];
      g_external_aliases.erase( module_and_class_id );
   }
}

void Meta_Specification::static_class_init( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for init" );

   g_state_names_variable = get_special_var_name( e_special_var_state_names );

   // [<start static_class_init>]
   // [<finish static_class_init>]
}

void Meta_Specification::static_class_term( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for term" );

   // [<start static_class_term>]
   // [<finish static_class_term>]
}
