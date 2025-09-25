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

#include "Meta_Specification_Field_Action.h"

#include "Meta_Enum.h"
#include "Meta_Enum_Item.h"
#include "Meta_List_Field.h"
#include "Meta_Model.h"
#include "Meta_Modifier.h"
#include "Meta_Permission.h"
#include "Meta_Procedure.h"
#include "Meta_Procedure_Arg.h"
#include "Meta_Relationship.h"
#include "Meta_Specification.h"
#include "Meta_Specification_Type.h"
#include "Meta_View_Field.h"
#include "Meta_Class.h"
#include "Meta_Field.h"

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

template< > inline string to_string( const Meta_Class& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_Field& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

inline void from_string( Meta_Class& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_Field& c, const string& s )
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

#include "Meta_Specification_Field_Action.cmh"

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

const char* const c_field_id_Access_Restriction = "128103";
const char* const c_field_id_Clone_Key = "128101";
const char* const c_field_id_Create_Type = "128102";
const char* const c_field_id_New_Record_Class = "302300";
const char* const c_field_id_New_Record_FK_Field = "302310";
const char* const c_field_id_New_Record_FK_Value = "128105";
const char* const c_field_id_Type = "128104";

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

const char* const c_field_name_Access_Restriction = "Access_Restriction";
const char* const c_field_name_Clone_Key = "Clone_Key";
const char* const c_field_name_Create_Type = "Create_Type";
const char* const c_field_name_New_Record_Class = "New_Record_Class";
const char* const c_field_name_New_Record_FK_Field = "New_Record_FK_Field";
const char* const c_field_name_New_Record_FK_Value = "New_Record_FK_Value";
const char* const c_field_name_Type = "Type";

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

const char* const c_field_display_name_Access_Restriction = "field_specification_field_action_access_restriction";
const char* const c_field_display_name_Clone_Key = "field_specification_field_action_clone_key";
const char* const c_field_display_name_Create_Type = "field_specification_field_action_create_type";
const char* const c_field_display_name_New_Record_Class = "field_specification_field_action_new_record_class";
const char* const c_field_display_name_New_Record_FK_Field = "field_specification_field_action_new_record_fk_field";
const char* const c_field_display_name_New_Record_FK_Value = "field_specification_field_action_new_record_fk_value";
const char* const c_field_display_name_Type = "field_specification_field_action_type";

const int c_num_fields = 7;

const char* const c_all_sorted_field_ids[ ] =
{
   "128101",
   "128102",
   "128103",
   "128104",
   "128105",
   "302300",
   "302310"
};

const char* const c_all_sorted_field_names[ ] =
{
   "Access_Restriction",
   "Clone_Key",
   "Create_Type",
   "New_Record_Class",
   "New_Record_FK_Field",
   "New_Record_FK_Value",
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

const int c_num_transient_fields = 0;

bool is_transient_field( const string& ) { static bool false_value( false ); return false_value; }

const char* const c_procedure_id_Generate_All_Vars = "128500";

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
const uint64_t c_modifier_Hide_Record_Create_Info = UINT64_C( 0x40000000000000 );

domain_string_max_size< 100 > g_Clone_Key_domain;
domain_string_max_size< 100 > g_New_Record_FK_Value_domain;

string g_group_field_name;
string g_level_field_name;
string g_order_field_name;
string g_owner_field_name;

string g_state_names_variable;

set< string > g_derivations;

set< string > g_file_field_ids;
set< string > g_file_field_names;

typedef map< string, Meta_Specification_Field_Action* > external_aliases_container;
typedef external_aliases_container::const_iterator external_aliases_const_iterator;
typedef external_aliases_container::value_type external_aliases_value_type;

typedef map< size_t, Meta_Specification_Field_Action* > external_aliases_lookup_container;
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

int g_default_Access_Restriction = int( 0 );
string g_default_Clone_Key = string( );
int g_default_Create_Type = int( 0 );
string g_default_New_Record_Class = string( );
string g_default_New_Record_FK_Field = string( );
string g_default_New_Record_FK_Value = string( );
int g_default_Type = int( 0 );

set< int > g_field_action_create_access_restriction_enum;
set< int > g_field_action_create_type_enum;
set< int > g_field_action_type_enum;

const int c_enum_field_action_create_access_restriction_none( 0 );
const int c_enum_field_action_create_access_restriction_owner( 1 );
const int c_enum_field_action_create_access_restriction_admin( 2 );
const int c_enum_field_action_create_access_restriction_non_admin( 3 );

string get_enum_string_field_action_create_access_restriction( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for field_action_create_access_restriction" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_field_action_create_access_restriction_none";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_field_action_create_access_restriction_owner";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_field_action_create_access_restriction_admin";
   else if( to_string( val ) == to_string( "3" ) )
      string_name = "enum_field_action_create_access_restriction_non_admin";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for field_action_create_access_restriction" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_field_action_create_type_standard( 0 );
const int c_enum_field_action_create_type_confirmation( 1 );
const int c_enum_field_action_create_type_edit_and_save( 2 );

string get_enum_string_field_action_create_type( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for field_action_create_type" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_field_action_create_type_standard";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_field_action_create_type_confirmation";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_field_action_create_type_edit_and_save";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for field_action_create_type" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_field_action_type_create_record( 0 );
const int c_enum_field_action_type_procedure_call( 1 );

string get_enum_string_field_action_type( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for field_action_type" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_field_action_type_create_record";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_field_action_type_procedure_call";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for field_action_type" );

   return get_module_string( lower( string_name ) );
}

// [<start anonymous>]
// [<finish anonymous>]

}

registration< Meta_Specification_Field_Action > Specification_Field_Action_registration( get_class_registry( ), "128100" );

class Meta_Specification_Field_Action_command_functor;

class Meta_Specification_Field_Action_command_handler : public command_handler
{
   friend class Meta_Specification_Field_Action_command_functor;

   public:
   Meta_Specification_Field_Action_command_handler( ) : p_Meta_Specification_Field_Action( 0 ) { }

   void set_Meta_Specification_Field_Action( Meta_Specification_Field_Action* p_new_Meta_Specification_Field_Action ) { p_Meta_Specification_Field_Action = p_new_Meta_Specification_Field_Action; }

   void handle_unknown_command( const string& command, const string& cmd_and_args )
   {
      ( void )command;

      retval = p_Meta_Specification_Field_Action->Meta_Specification::execute( cmd_and_args );
   }

   void handle_invalid_command( const command_parser& parser, const string& cmd_and_args )
   {
      throw runtime_error( "invalid command usage '" + cmd_and_args + "'" );
   }

   private:
   Meta_Specification_Field_Action* p_Meta_Specification_Field_Action;

   protected:
   string retval;
   string cmd_and_args;
};

class Meta_Specification_Field_Action_command_functor : public command_functor
{
   public:
   Meta_Specification_Field_Action_command_functor( Meta_Specification_Field_Action_command_handler& handler )
    : command_functor( handler ),
    cmd_handler( handler )
   {
   }

   void operator ( )( const string& command, const parameter_info& parameters );

   private:
   Meta_Specification_Field_Action_command_handler& cmd_handler;
};

command_functor* Meta_Specification_Field_Action_command_functor_factory( const string& /*name*/, command_handler& handler )
{
   return new Meta_Specification_Field_Action_command_functor( dynamic_cast< Meta_Specification_Field_Action_command_handler& >( handler ) );
}

void Meta_Specification_Field_Action_command_functor::operator ( )( const string& command, const parameter_info& parameters )
{
   if( command == c_cmd_Meta_Specification_Field_Action_key )
   {
      bool want_fixed( has_parm_val( parameters, c_cmd_Meta_Specification_Field_Action_key_fixed ) );

      if( !want_fixed )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Field_Action->get_key( );
      else
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Field_Action->get_fixed_key( );
   }
   else if( command == c_cmd_Meta_Specification_Field_Action_ver )
   {
      string ver_rev( to_string( cmd_handler.p_Meta_Specification_Field_Action->get_version( ) ) );
      ver_rev += "." + to_string( cmd_handler.p_Meta_Specification_Field_Action->get_revision( ) );

      cmd_handler.retval = ver_rev;
   }
   else if( command == c_cmd_Meta_Specification_Field_Action_get )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Specification_Field_Action_get_field_name ) );

      bool handled = false;

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for getter call" );

      if( !handled && ( ( field_name == c_field_id_Access_Restriction ) || ( field_name == c_field_name_Access_Restriction ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Specification_Field_Action->Access_Restriction( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Clone_Key ) || ( field_name == c_field_name_Clone_Key ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Specification_Field_Action->Clone_Key( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Create_Type ) || ( field_name == c_field_name_Create_Type ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Specification_Field_Action->Create_Type( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_New_Record_Class ) || ( field_name == c_field_name_New_Record_Class ) ) )
      {
         handled = true;

         string_getter< Meta_Class >( cmd_handler.p_Meta_Specification_Field_Action->New_Record_Class( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_New_Record_FK_Field ) || ( field_name == c_field_name_New_Record_FK_Field ) ) )
      {
         handled = true;

         string_getter< Meta_Field >( cmd_handler.p_Meta_Specification_Field_Action->New_Record_FK_Field( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_New_Record_FK_Value ) || ( field_name == c_field_name_New_Record_FK_Value ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Specification_Field_Action->New_Record_FK_Value( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Type ) || ( field_name == c_field_name_Type ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Specification_Field_Action->Type( ), cmd_handler.retval );
      }

      if( !handled )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Field_Action->Meta_Specification::execute( cmd_handler.cmd_and_args );
   }
   else if( command == c_cmd_Meta_Specification_Field_Action_set )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Specification_Field_Action_set_field_name ) );
      string field_value( get_parm_val( parameters, c_cmd_Meta_Specification_Field_Action_set_field_value ) );

      bool handled = false;

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for setter call" );

      if( !handled && ( ( field_name == c_field_id_Access_Restriction ) || ( field_name == c_field_name_Access_Restriction ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Field_Action, int >(
          *cmd_handler.p_Meta_Specification_Field_Action, &Meta_Specification_Field_Action::Access_Restriction, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Clone_Key ) || ( field_name == c_field_name_Clone_Key ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Field_Action, string >(
          *cmd_handler.p_Meta_Specification_Field_Action, &Meta_Specification_Field_Action::Clone_Key, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Create_Type ) || ( field_name == c_field_name_Create_Type ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Field_Action, int >(
          *cmd_handler.p_Meta_Specification_Field_Action, &Meta_Specification_Field_Action::Create_Type, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_New_Record_Class ) || ( field_name == c_field_name_New_Record_Class ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Field_Action, Meta_Class >(
          *cmd_handler.p_Meta_Specification_Field_Action, &Meta_Specification_Field_Action::New_Record_Class, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_New_Record_FK_Field ) || ( field_name == c_field_name_New_Record_FK_Field ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Field_Action, Meta_Field >(
          *cmd_handler.p_Meta_Specification_Field_Action, &Meta_Specification_Field_Action::New_Record_FK_Field, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_New_Record_FK_Value ) || ( field_name == c_field_name_New_Record_FK_Value ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Field_Action, string >(
          *cmd_handler.p_Meta_Specification_Field_Action, &Meta_Specification_Field_Action::New_Record_FK_Value, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Type ) || ( field_name == c_field_name_Type ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Field_Action, int >(
          *cmd_handler.p_Meta_Specification_Field_Action, &Meta_Specification_Field_Action::Type, field_value );
      }

      if( !handled )
         cmd_handler.p_Meta_Specification_Field_Action->Meta_Specification::execute( cmd_handler.cmd_and_args );

      cmd_handler.retval = c_okay;
   }
   else if( command == c_cmd_Meta_Specification_Field_Action_cmd )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Specification_Field_Action_cmd_field_name ) );
      string cmd_and_args( get_parm_val( parameters, c_cmd_Meta_Specification_Field_Action_cmd_cmd_and_args ) );

      cmd_handler.retval.erase( );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for command call" );
      else if( ( field_name == c_field_id_New_Record_Class ) || ( field_name == c_field_name_New_Record_Class ) )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Field_Action->New_Record_Class( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_New_Record_FK_Field ) || ( field_name == c_field_name_New_Record_FK_Field ) )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Field_Action->New_Record_FK_Field( ).execute( cmd_and_args );
      else
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Field_Action->Meta_Specification::execute( cmd_handler.cmd_and_args );
   }
   else if( command == c_cmd_Meta_Specification_Field_Action_Generate_All_Vars )
   {
      string Parent_Prefix( get_parm_val_from_string< string >( parameters, c_cmd_Meta_Specification_Field_Action_Generate_All_Vars_Parent_Prefix ) );
      string All_Vars;

      cmd_handler.p_Meta_Specification_Field_Action->Generate_All_Vars( All_Vars, Parent_Prefix );

      cmd_handler.retval.erase( );

      append_value( cmd_handler.retval, All_Vars );
   }
}

struct Meta_Specification_Field_Action::impl : public Meta_Specification_Field_Action_command_handler
{
   impl( Meta_Specification_Field_Action& o )
    :
    cp_obj( &o ),
    total_child_relationships( 0 )
   {
      p_obj = &o;
      set_Meta_Specification_Field_Action( &o );

      add_commands( 0, Meta_Specification_Field_Action_command_functor_factory,
       ARRAY_PTR_AND_SIZE( Meta_Specification_Field_Action_command_definitions ) );
   }

   Meta_Specification_Field_Action& get_obj( ) const
   {
      return *cp_obj;
   }

   int impl_Access_Restriction( ) const { return lazy_fetch( p_obj ), v_Access_Restriction; }
   void impl_Access_Restriction( int Access_Restriction ) { v_Access_Restriction = Access_Restriction; }

   const string& impl_Clone_Key( ) const { return lazy_fetch( p_obj ), v_Clone_Key; }
   void impl_Clone_Key( const string& Clone_Key ) { sanity_check( Clone_Key ); v_Clone_Key = Clone_Key; }

   int impl_Create_Type( ) const { return lazy_fetch( p_obj ), v_Create_Type; }
   void impl_Create_Type( int Create_Type ) { v_Create_Type = Create_Type; }

   const string& impl_New_Record_FK_Value( ) const { return lazy_fetch( p_obj ), v_New_Record_FK_Value; }
   void impl_New_Record_FK_Value( const string& New_Record_FK_Value ) { sanity_check( New_Record_FK_Value ); v_New_Record_FK_Value = New_Record_FK_Value; }

   int impl_Type( ) const { return lazy_fetch( p_obj ), v_Type; }
   void impl_Type( int Type ) { v_Type = Type; }

   Meta_Class& impl_New_Record_Class( )
   {
      if( !cp_New_Record_Class )
      {
         cp_New_Record_Class.init( );

         p_obj->setup_graph_parent( *cp_New_Record_Class, c_field_id_New_Record_Class, v_New_Record_Class );
      }
      return *cp_New_Record_Class;
   }

   const Meta_Class& impl_New_Record_Class( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_New_Record_Class )
      {
         cp_New_Record_Class.init( );

         p_obj->setup_graph_parent( *cp_New_Record_Class, c_field_id_New_Record_Class, v_New_Record_Class );
      }
      return *cp_New_Record_Class;
   }

   void impl_New_Record_Class( const string& key )
   {
      class_base_accessor cba( impl_New_Record_Class( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_New_Record_FK_Field( )
   {
      if( !cp_New_Record_FK_Field )
      {
         cp_New_Record_FK_Field.init( );

         p_obj->setup_graph_parent( *cp_New_Record_FK_Field, c_field_id_New_Record_FK_Field, v_New_Record_FK_Field );
      }
      return *cp_New_Record_FK_Field;
   }

   const Meta_Field& impl_New_Record_FK_Field( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_New_Record_FK_Field )
      {
         cp_New_Record_FK_Field.init( );

         p_obj->setup_graph_parent( *cp_New_Record_FK_Field, c_field_id_New_Record_FK_Field, v_New_Record_FK_Field );
      }
      return *cp_New_Record_FK_Field;
   }

   void impl_New_Record_FK_Field( const string& key )
   {
      class_base_accessor cba( impl_New_Record_FK_Field( ) );
      cba.set_key( key );
   }

   void impl_Generate_All_Vars( string& All_Vars, const string& Parent_Prefix );

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

   Meta_Specification_Field_Action* p_obj;
   class_pointer< Meta_Specification_Field_Action > cp_obj;

   mutable set< string > required_transients;

   // [<start members>]
   // [<finish members>]

   size_t total_child_relationships;

   int v_Access_Restriction;
   string v_Clone_Key;
   int v_Create_Type;
   string v_New_Record_FK_Value;
   int v_Type;

   string v_New_Record_Class;
   mutable class_pointer< Meta_Class > cp_New_Record_Class;

   string v_New_Record_FK_Field;
   mutable class_pointer< Meta_Field > cp_New_Record_FK_Field;
};

void Meta_Specification_Field_Action::impl::impl_Generate_All_Vars( string& All_Vars, const string& Parent_Prefix )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start Generate_All_Vars_impl>]
//nyi
   // NOTE: If a restore is in progress then do nothing.
   if( storage_locked_for_admin( ) )
      return;

   if( get_obj( ).get_key( ).empty( ) )
      throw runtime_error( "unexpected call to Meta_Specification_Field_Action_Create::Generate_All_Vars" );

   string action( "value=\"" );

   switch( get_obj( ).Create_Type( ) )
   {
      case 0: //i.e. standard
      break;

      case 1: //i.e. confirm
      action += "?";
      break;

      case 2: //i.e. edit_and_save
      action += "*";
      break;
   }

   switch( get_obj( ).Access_Restriction( ) )
   {
      case 0: //i.e. none
      break;

      case 1: //i.e. owner
      action += "#";
      break;

      case 2: //i.e. admin
      action += "@";
      break;

      case 3: //i.e. non_admin
      action += "~";
      break;
   }

   action += get_obj( ).Procedure( ).Id( );

   if( get_obj( ).Type( ) == 0 ) // i.e. create_record
   {
      action += "$" + get_obj( ).New_Record_Class( ).Id( );
      action += "." + get_obj( ).New_Record_FK_Field( ).Id( );

      if( !get_obj( ).New_Record_FK_Value( ).empty( ) )
         action += "=" + get_obj( ).New_Record_FK_Value( );

      if( !get_obj( ).Clone_Key( ).empty( ) )
         action += "%" + get_obj( ).Clone_Key( );
   }

   action += "\"";

   All_Vars = action;

   get_obj( ).Meta_Specification::Generate_All_Vars( All_Vars, Parent_Prefix );
   // [<finish Generate_All_Vars_impl>]
}

string Meta_Specification_Field_Action::impl::get_field_value( int field ) const
{
   string retval;

   switch( field )
   {
      case 0:
      retval = to_string( impl_Access_Restriction( ) );
      break;

      case 1:
      retval = to_string( impl_Clone_Key( ) );
      break;

      case 2:
      retval = to_string( impl_Create_Type( ) );
      break;

      case 3:
      retval = to_string( impl_New_Record_Class( ) );
      break;

      case 4:
      retval = to_string( impl_New_Record_FK_Field( ) );
      break;

      case 5:
      retval = to_string( impl_New_Record_FK_Value( ) );
      break;

      case 6:
      retval = to_string( impl_Type( ) );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in get field value" );
   }

   return retval;
}

void Meta_Specification_Field_Action::impl::set_field_value( int field, const string& value )
{
   switch( field )
   {
      case 0:
      func_string_setter< Meta_Specification_Field_Action::impl, int >( *this, &Meta_Specification_Field_Action::impl::impl_Access_Restriction, value );
      break;

      case 1:
      func_string_setter< Meta_Specification_Field_Action::impl, string >( *this, &Meta_Specification_Field_Action::impl::impl_Clone_Key, value );
      break;

      case 2:
      func_string_setter< Meta_Specification_Field_Action::impl, int >( *this, &Meta_Specification_Field_Action::impl::impl_Create_Type, value );
      break;

      case 3:
      func_string_setter< Meta_Specification_Field_Action::impl, Meta_Class >( *this, &Meta_Specification_Field_Action::impl::impl_New_Record_Class, value );
      break;

      case 4:
      func_string_setter< Meta_Specification_Field_Action::impl, Meta_Field >( *this, &Meta_Specification_Field_Action::impl::impl_New_Record_FK_Field, value );
      break;

      case 5:
      func_string_setter< Meta_Specification_Field_Action::impl, string >( *this, &Meta_Specification_Field_Action::impl::impl_New_Record_FK_Value, value );
      break;

      case 6:
      func_string_setter< Meta_Specification_Field_Action::impl, int >( *this, &Meta_Specification_Field_Action::impl::impl_Type, value );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in set field value" );
   }
}

void Meta_Specification_Field_Action::impl::set_field_default( int field )
{
   switch( field )
   {
      case 0:
      impl_Access_Restriction( g_default_Access_Restriction );
      break;

      case 1:
      impl_Clone_Key( g_default_Clone_Key );
      break;

      case 2:
      impl_Create_Type( g_default_Create_Type );
      break;

      case 3:
      impl_New_Record_Class( g_default_New_Record_Class );
      break;

      case 4:
      impl_New_Record_FK_Field( g_default_New_Record_FK_Field );
      break;

      case 5:
      impl_New_Record_FK_Value( g_default_New_Record_FK_Value );
      break;

      case 6:
      impl_Type( g_default_Type );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in set field default" );
   }
}

bool Meta_Specification_Field_Action::impl::is_field_default( int field ) const
{
   bool retval = false;

   switch( field )
   {
      case 0:
      retval = ( v_Access_Restriction == g_default_Access_Restriction );
      break;

      case 1:
      retval = ( v_Clone_Key == g_default_Clone_Key );
      break;

      case 2:
      retval = ( v_Create_Type == g_default_Create_Type );
      break;

      case 3:
      retval = ( v_New_Record_Class == g_default_New_Record_Class );
      break;

      case 4:
      retval = ( v_New_Record_FK_Field == g_default_New_Record_FK_Field );
      break;

      case 5:
      retval = ( v_New_Record_FK_Value == g_default_New_Record_FK_Value );
      break;

      case 6:
      retval = ( v_Type == g_default_Type );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in is_field_default" );
   }

   return retval;
}

uint64_t Meta_Specification_Field_Action::impl::get_state( ) const
{
   uint64_t state = 0;

   // [(start modifier_field_value)] 600265
   if( get_obj( ).Type( ) == 1 ) // i.e. procedure_call
      state |= c_modifier_Hide_Record_Create_Info;
   // [(finish modifier_field_value)] 600265

   // [<start get_state>]
   // [<finish get_state>]

   return state;
}

string Meta_Specification_Field_Action::impl::get_state_names( ) const
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
   if( state & c_modifier_Hide_Record_Create_Info )
      state_names += "|" + string( "Hide_Record_Create_Info" );

   return state_names.empty( ) ? state_names : state_names.substr( 1 );
}

const string& Meta_Specification_Field_Action::impl::execute( const string& cmd_and_args )
{
   this->cmd_and_args = cmd_and_args;
   execute_command( cmd_and_args );
   return retval;
}

void Meta_Specification_Field_Action::impl::clear_foreign_key( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( ( field == c_field_id_New_Record_Class ) || ( field == c_field_name_New_Record_Class ) )
      impl_New_Record_Class( "" );
   else if( ( field == c_field_id_New_Record_FK_Field ) || ( field == c_field_name_New_Record_FK_Field ) )
      impl_New_Record_FK_Field( "" );
   else
      p_obj->parent_class_type::clear_foreign_key( field );
}

void Meta_Specification_Field_Action::impl::set_foreign_key_value( const string& field, const string& value )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for value: " + value );
   else if( ( field == c_field_id_New_Record_Class ) || ( field == c_field_name_New_Record_Class ) )
      v_New_Record_Class = value;
   else if( ( field == c_field_id_New_Record_FK_Field ) || ( field == c_field_name_New_Record_FK_Field ) )
      v_New_Record_FK_Field = value;
   else
      p_obj->parent_class_type::set_foreign_key_value( field, value );
}

const string& Meta_Specification_Field_Action::impl::get_foreign_key_value( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( ( field == c_field_id_New_Record_Class ) || ( field == c_field_name_New_Record_Class ) )
      return v_New_Record_Class;
   else if( ( field == c_field_id_New_Record_FK_Field ) || ( field == c_field_name_New_Record_FK_Field ) )
      return v_New_Record_FK_Field;
   else
      return p_obj->parent_class_type::get_foreign_key_value( field );
}

void Meta_Specification_Field_Action::impl::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_New_Record_Class, v_New_Record_Class ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_New_Record_FK_Field, v_New_Record_FK_Field ) );
}

void Meta_Specification_Field_Action::impl::add_extra_fixed_info( vector< pair< string, string > >& fixed_info ) const
{
   ( void )fixed_info;

   // [<start add_extra_fixed_info>]
   // [<finish add_extra_fixed_info>]
}

void Meta_Specification_Field_Action::impl::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   ( void )paging_info;

   // [<start add_extra_paging_info>]
   // [<finish add_extra_paging_info>]
}

void Meta_Specification_Field_Action::impl::clear( )
{
   v_Access_Restriction = g_default_Access_Restriction;
   v_Clone_Key = g_default_Clone_Key;
   v_Create_Type = g_default_Create_Type;
   v_New_Record_FK_Value = g_default_New_Record_FK_Value;
   v_Type = g_default_Type;

   v_New_Record_Class = string( );
   if( cp_New_Record_Class )
      p_obj->setup_foreign_key( *cp_New_Record_Class, v_New_Record_Class );

   v_New_Record_FK_Field = string( );
   if( cp_New_Record_FK_Field )
      p_obj->setup_foreign_key( *cp_New_Record_FK_Field, v_New_Record_FK_Field );
}

bool Meta_Specification_Field_Action::impl::value_will_be_provided( const string& field_name )
{
   ( void )field_name;

   // [<start value_will_be_provided>]
   // [<finish value_will_be_provided>]

   return false;
}

void Meta_Specification_Field_Action::impl::validate(
 uint64_t state, bool is_internal, validation_error_container* p_validation_errors )
{
   ( void )state;
   ( void )is_internal;

   if( !p_validation_errors )
      throw runtime_error( "unexpected null validation_errors container" );

   string error_message;
   validate_formatter vf;

   if( !is_null( v_Clone_Key )
    && ( v_Clone_Key != g_default_Clone_Key
    || !value_will_be_provided( c_field_name_Clone_Key ) )
    && !g_Clone_Key_domain.is_valid( v_Clone_Key, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Clone_Key,
       get_module_string( c_field_display_name_Clone_Key ) + " " + error_message ) );

   if( !is_null( v_New_Record_FK_Value )
    && ( v_New_Record_FK_Value != g_default_New_Record_FK_Value
    || !value_will_be_provided( c_field_name_New_Record_FK_Value ) )
    && !g_New_Record_FK_Value_domain.is_valid( v_New_Record_FK_Value, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_New_Record_FK_Value,
       get_module_string( c_field_display_name_New_Record_FK_Value ) + " " + error_message ) );

   if( !g_field_action_create_access_restriction_enum.count( v_Access_Restriction ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Access_Restriction,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_field_has_invalid_value_field, get_module_string( c_field_display_name_Access_Restriction ) ) ) ) );

   if( !g_field_action_create_type_enum.count( v_Create_Type ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Create_Type,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_field_has_invalid_value_field, get_module_string( c_field_display_name_Create_Type ) ) ) ) );

   if( !g_field_action_type_enum.count( v_Type ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Type,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_field_has_invalid_value_field, get_module_string( c_field_display_name_Type ) ) ) ) );

   // [(start check_cond_non_null)] 600270
   if( get_obj( ).Type( ) == 0 && get_obj( ).Specification_Type( ).Allow_Procedure( ) && is_null( get_obj( ).New_Record_Class( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_New_Record_Class, get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_field_must_not_be_empty_field, get_module_string( c_field_display_name_New_Record_Class ) ) ) ) );
   // [(finish check_cond_non_null)] 600270

   // [(start check_cond_non_null)] 600271
   if( get_obj( ).Type( ) == 0 && get_obj( ).Specification_Type( ).Allow_Procedure( ) && is_null( get_obj( ).New_Record_FK_Field( ) ) )
      p_validation_errors->insert( construct_validation_error( vf.num,
       c_field_name_New_Record_FK_Field, get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_field_must_not_be_empty_field, get_module_string( c_field_display_name_New_Record_FK_Field ) ) ) ) );
   // [(finish check_cond_non_null)] 600271

   // [<start validate>]
   // [<finish validate>]
}

void Meta_Specification_Field_Action::impl::validate_set_fields(
 set< string >& fields_set, validation_error_container* p_validation_errors )
{
   ( void )fields_set;

   if( !p_validation_errors )
      throw runtime_error( "unexpected null validation_errors container" );

   string error_message;
   validate_formatter vf;

   if( !is_null( v_Clone_Key )
    && ( fields_set.count( c_field_id_Clone_Key ) || fields_set.count( c_field_name_Clone_Key ) )
    && !g_Clone_Key_domain.is_valid( v_Clone_Key, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Clone_Key,
       get_module_string( c_field_display_name_Clone_Key ) + " " + error_message ) );

   if( !is_null( v_New_Record_FK_Value )
    && ( fields_set.count( c_field_id_New_Record_FK_Value ) || fields_set.count( c_field_name_New_Record_FK_Value ) )
    && !g_New_Record_FK_Value_domain.is_valid( v_New_Record_FK_Value, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_New_Record_FK_Value,
       get_module_string( c_field_display_name_New_Record_FK_Value ) + " " + error_message ) );
}

void Meta_Specification_Field_Action::impl::after_fetch( )
{
   if( !get_obj( ).get_is_iterating( ) || get_obj( ).get_is_starting_iteration( ) )
      get_required_transients( );

   if( cp_New_Record_Class )
      p_obj->setup_foreign_key( *cp_New_Record_Class, v_New_Record_Class );

   if( cp_New_Record_FK_Field )
      p_obj->setup_foreign_key( *cp_New_Record_FK_Field, v_New_Record_FK_Field );

   post_init( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_fetch>]
   // [<finish after_fetch>]
}

void Meta_Specification_Field_Action::impl::finalise_fetch( bool skip_set_original )
{
   if( !skip_set_original && !get_obj( ).get_key( ).empty( ) )
      get_obj( ).set_new_original_values( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start finalise_fetch>]
   // [<finish finalise_fetch>]
}

void Meta_Specification_Field_Action::impl::at_create( )
{
   // [<start at_create>]
   // [<finish at_create>]
}

void Meta_Specification_Field_Action::impl::post_init( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start post_init>]
   // [<finish post_init>]
}

void Meta_Specification_Field_Action::impl::to_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   if( !get_obj( ).get_is_preparing( ) )
      post_init( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start field_from_search_replace)] 600260
   if( get_obj( ).get_is_editing( )
    && !get_obj( ).get_is_for_peer( ) )
   {
      string str( get_obj( ).Specification_Type( ).Specification_Name( ) );

      get_obj( ).Name( str );

      get_obj( ).add_search_replacement( "Name", "{newclass}", to_rep_string( get_obj( ).New_Record_Class( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Name", "{newfield}", to_rep_string( get_obj( ).New_Record_FK_Field( ).Name( ) ) );

      get_obj( ).set_search_replace_separator( "Name", '_' );

      get_obj( ).set_search_replace_has_opt_prefixing( "Name" );
   }
   // [(finish field_from_search_replace)] 600260

   // [<start to_store>]
   // [<finish to_store>]
}

void Meta_Specification_Field_Action::impl::for_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start for_store>]
   // [<finish for_store>]
}

void Meta_Specification_Field_Action::impl::after_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_store>]
   // [<finish after_store>]
}

bool Meta_Specification_Field_Action::impl::can_destroy( bool is_internal )
{
   uint64_t state = p_obj->get_state( );

   bool retval = ( is_internal || !( state & c_state_undeletable ) );

   // [<start can_destroy>]
   // [<finish can_destroy>]

   return retval;
}

void Meta_Specification_Field_Action::impl::for_destroy( bool is_internal )
{
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start for_destroy>]
   // [<finish for_destroy>]
}

void Meta_Specification_Field_Action::impl::after_destroy( bool is_internal )
{
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_destroy>]
   // [<finish after_destroy>]
}

void Meta_Specification_Field_Action::impl::set_default_values( )
{
   clear( );
}

bool Meta_Specification_Field_Action::impl::is_filtered( ) const
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start is_filtered>]
   // [<finish is_filtered>]

   return false;
}

void Meta_Specification_Field_Action::impl::get_required_transients( ) const
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

Meta_Specification_Field_Action::Meta_Specification_Field_Action( )
{
   set_version( c_version );

   p_impl = new impl( *this );
}

Meta_Specification_Field_Action::~Meta_Specification_Field_Action( )
{
   delete p_impl;
}

int Meta_Specification_Field_Action::Access_Restriction( ) const
{
   return p_impl->impl_Access_Restriction( );
}

void Meta_Specification_Field_Action::Access_Restriction( int Access_Restriction )
{
   p_impl->impl_Access_Restriction( Access_Restriction );
}

const string& Meta_Specification_Field_Action::Clone_Key( ) const
{
   return p_impl->impl_Clone_Key( );
}

void Meta_Specification_Field_Action::Clone_Key( const string& Clone_Key )
{
   p_impl->impl_Clone_Key( Clone_Key );
}

int Meta_Specification_Field_Action::Create_Type( ) const
{
   return p_impl->impl_Create_Type( );
}

void Meta_Specification_Field_Action::Create_Type( int Create_Type )
{
   p_impl->impl_Create_Type( Create_Type );
}

const string& Meta_Specification_Field_Action::New_Record_FK_Value( ) const
{
   return p_impl->impl_New_Record_FK_Value( );
}

void Meta_Specification_Field_Action::New_Record_FK_Value( const string& New_Record_FK_Value )
{
   p_impl->impl_New_Record_FK_Value( New_Record_FK_Value );
}

int Meta_Specification_Field_Action::Type( ) const
{
   return p_impl->impl_Type( );
}

void Meta_Specification_Field_Action::Type( int Type )
{
   p_impl->impl_Type( Type );
}

Meta_Class& Meta_Specification_Field_Action::New_Record_Class( )
{
   return p_impl->impl_New_Record_Class( );
}

const Meta_Class& Meta_Specification_Field_Action::New_Record_Class( ) const
{
   return p_impl->impl_New_Record_Class( );
}

void Meta_Specification_Field_Action::New_Record_Class( const string& key )
{
   p_impl->impl_New_Record_Class( key );
}

Meta_Field& Meta_Specification_Field_Action::New_Record_FK_Field( )
{
   return p_impl->impl_New_Record_FK_Field( );
}

const Meta_Field& Meta_Specification_Field_Action::New_Record_FK_Field( ) const
{
   return p_impl->impl_New_Record_FK_Field( );
}

void Meta_Specification_Field_Action::New_Record_FK_Field( const string& key )
{
   p_impl->impl_New_Record_FK_Field( key );
}

void Meta_Specification_Field_Action::Generate_All_Vars( string& All_Vars, const string& Parent_Prefix )
{
   p_impl->impl_Generate_All_Vars( All_Vars, Parent_Prefix );
}

string Meta_Specification_Field_Action::get_field_value( int field ) const
{
   int num_parent_fields( parent_class_type::get_num_fields( ) );

   if( field < num_parent_fields )
      return parent_class_type::get_field_value( field );
   else
      return p_impl->get_field_value( field - num_parent_fields );
}

void Meta_Specification_Field_Action::set_field_value( int field, const string& value )
{
   int num_parent_fields( parent_class_type::get_num_fields( ) );

   if( field < num_parent_fields )
      parent_class_type::set_field_value( field, value );
   else
      p_impl->set_field_value( field - num_parent_fields, value );
}

void Meta_Specification_Field_Action::set_field_default( int field )
{
   int num_parent_fields( parent_class_type::get_num_fields( ) );

   if( field < num_parent_fields )
      return parent_class_type::set_field_default( field );
   else
      return set_field_default( ( field_id )( field - num_parent_fields + 1 ) );
}

void Meta_Specification_Field_Action::set_field_default( field_id id )
{
   p_impl->set_field_default( ( int )id - 1 );
}

void Meta_Specification_Field_Action::set_field_default( const string& field )
{
   p_impl->set_field_default( get_field_num( field ) );
}

bool Meta_Specification_Field_Action::is_field_default( int field ) const
{
   int num_parent_fields( parent_class_type::get_num_fields( ) );

   if( field < num_parent_fields )
      return parent_class_type::is_field_default( field );
   else
      return is_field_default( ( field_id )( field - num_parent_fields + 1 ) );
}

bool Meta_Specification_Field_Action::is_field_default( field_id id ) const
{
   return p_impl->is_field_default( ( int )id - 1 );
}

bool Meta_Specification_Field_Action::is_field_default( const string& field ) const
{
   return p_impl->is_field_default( get_field_num( field ) );
}

bool Meta_Specification_Field_Action::is_field_encrypted( int field ) const
{
   int num_parent_fields( parent_class_type::get_num_fields( ) );

   if( field < num_parent_fields )
      return parent_class_type::is_field_encrypted( field );
   else
      return static_is_field_encrypted( ( field_id )( field - num_parent_fields + 1 ) );
}

bool Meta_Specification_Field_Action::is_field_transient( int field ) const
{
   int num_parent_fields( parent_class_type::get_num_fields( ) );

   if( field < num_parent_fields )
      return parent_class_type::is_field_transient( field );
   else
      return static_is_field_transient( ( field_id )( field - num_parent_fields + 1 ) );
}

string Meta_Specification_Field_Action::get_field_id( int field ) const
{
   int num_parent_fields( parent_class_type::get_num_fields( ) );

   if( field < num_parent_fields )
      return parent_class_type::get_field_id( field );
   else
      return static_get_field_id( ( field_id )( field - num_parent_fields + 1 ) );
}

string Meta_Specification_Field_Action::get_field_name( int field ) const
{
   int num_parent_fields( parent_class_type::get_num_fields( ) );

   if( field < num_parent_fields )
      return parent_class_type::get_field_name( field );
   else
      return static_get_field_name( ( field_id )( field - num_parent_fields + 1 ) );
}

int Meta_Specification_Field_Action::get_field_num( const string& field ) const
{
   int rc = static_get_field_num( field );

   if( rc < 0 )
      throw runtime_error( "unknown field name/id '" + field + "' in get_field_num( )" );

   return rc;
}

bool Meta_Specification_Field_Action::has_field_changed( const string& field ) const
{
   return class_base::has_field_changed( get_field_num( field ) );
}

uint64_t Meta_Specification_Field_Action::get_state( ) const
{
   uint64_t state = 0;

   state |= parent_class_type::get_state( );
   state |= p_impl->get_state( );

   return state;
}

const string& Meta_Specification_Field_Action::execute( const string& cmd_and_args )
{
   return p_impl->execute( cmd_and_args );
}

void Meta_Specification_Field_Action::clear( )
{
   parent_class_type::clear( );
   p_impl->clear( );
}

void Meta_Specification_Field_Action::validate( uint64_t state, bool is_internal )
{
   parent_class_type::validate( state, is_internal );
   p_impl->validate( state, is_internal, &validation_errors );
}

void Meta_Specification_Field_Action::validate_set_fields( set< string >& fields_set )
{
   parent_class_type::validate_set_fields( fields_set );
   p_impl->validate_set_fields( fields_set, &validation_errors );
}

void Meta_Specification_Field_Action::after_fetch( )
{
   parent_class_type::after_fetch( );
   p_impl->after_fetch( );
}

void Meta_Specification_Field_Action::finalise_fetch( bool skip_set_original )
{
   parent_class_type::finalise_fetch( skip_set_original );
   p_impl->finalise_fetch( skip_set_original );
}

void Meta_Specification_Field_Action::at_create( )
{
   parent_class_type::at_create( );
   p_impl->at_create( );
}

void Meta_Specification_Field_Action::post_init( )
{
   parent_class_type::post_init( );
   p_impl->post_init( );
}

void Meta_Specification_Field_Action::to_store( bool is_create, bool is_internal )
{
   parent_class_type::to_store( is_create, is_internal );
   p_impl->to_store( is_create, is_internal );
}

void Meta_Specification_Field_Action::for_store( bool is_create, bool is_internal )
{
   parent_class_type::for_store( is_create, is_internal );
   p_impl->for_store( is_create, is_internal );
}

void Meta_Specification_Field_Action::after_store( bool is_create, bool is_internal )
{
   parent_class_type::after_store( is_create, is_internal );
   p_impl->after_store( is_create, is_internal );
}

bool Meta_Specification_Field_Action::can_destroy( bool is_internal )
{
   if( !parent_class_type::can_destroy( is_internal ) )
      return false;
   return p_impl->can_destroy( is_internal );
}

void Meta_Specification_Field_Action::for_destroy( bool is_internal )
{
   p_impl->for_destroy( is_internal );
   parent_class_type::for_destroy( is_internal );
}

void Meta_Specification_Field_Action::after_destroy( bool is_internal )
{
   p_impl->after_destroy( is_internal );
   parent_class_type::after_destroy( is_internal );
}

void Meta_Specification_Field_Action::set_default_values( )
{
   parent_class_type::set_default_values( );
   p_impl->set_default_values( );
}

bool Meta_Specification_Field_Action::is_filtered( ) const
{
   if( parent_class_type::is_filtered( ) )
      return true;

   return p_impl->is_filtered( );
}

const char* Meta_Specification_Field_Action::get_field_id(
 const string& name, bool* p_sql_numeric, string* p_type_name ) const
{
   const char* p_id( parent_class_type::get_field_id( name, p_sql_numeric, p_type_name ) );
   if( p_id )
      return p_id;

   if( name.empty( ) )
      throw runtime_error( "unexpected empty field name for get_field_id" );
   else if( name == c_field_name_Access_Restriction )
   {
      p_id = c_field_id_Access_Restriction;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Clone_Key )
   {
      p_id = c_field_id_Clone_Key;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Create_Type )
   {
      p_id = c_field_id_Create_Type;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_New_Record_Class )
   {
      p_id = c_field_id_New_Record_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_New_Record_FK_Field )
   {
      p_id = c_field_id_New_Record_FK_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_New_Record_FK_Value )
   {
      p_id = c_field_id_New_Record_FK_Value;

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

const char* Meta_Specification_Field_Action::get_field_name(
 const string& id, bool* p_sql_numeric, string* p_type_name ) const
{
   const char* p_name( parent_class_type::get_field_name( id, p_sql_numeric, p_type_name ) );

   if( p_name )
      return p_name;

   if( id.empty( ) )
      throw runtime_error( "unexpected empty field id for get_field_name" );
   else if( id == c_field_id_Access_Restriction )
   {
      p_name = c_field_name_Access_Restriction;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Clone_Key )
   {
      p_name = c_field_name_Clone_Key;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Create_Type )
   {
      p_name = c_field_name_Create_Type;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_New_Record_Class )
   {
      p_name = c_field_name_New_Record_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_New_Record_FK_Field )
   {
      p_name = c_field_name_New_Record_FK_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_New_Record_FK_Value )
   {
      p_name = c_field_name_New_Record_FK_Value;

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

string& Meta_Specification_Field_Action::get_group_field_name( ) const
{
   return parent_class_type::get_group_field_name( );
}

string& Meta_Specification_Field_Action::get_level_field_name( ) const
{
   return parent_class_type::get_level_field_name( );
}

string& Meta_Specification_Field_Action::get_order_field_name( ) const
{
   return parent_class_type::get_order_field_name( );
}

string& Meta_Specification_Field_Action::get_owner_field_name( ) const
{
   return parent_class_type::get_owner_field_name( );
}

bool Meta_Specification_Field_Action::is_file_field( const string& id_or_name ) const
{
   return ( parent_class_type::is_file_field( id_or_name )
    || g_file_field_ids.count( id_or_name ) || g_file_field_names.count( id_or_name ) );
}

void Meta_Specification_Field_Action::get_file_field_names( vector< string >& file_field_names ) const
{
   parent_class_type::get_file_field_names( file_field_names );

   for( set< string >::const_iterator ci = g_file_field_names.begin( ); ci != g_file_field_names.end( ); ++ci )
      file_field_names.push_back( *ci );
}

string Meta_Specification_Field_Action::get_field_uom_symbol( const string& id_or_name ) const
{
   string uom_symbol( parent_class_type::get_field_uom_symbol( id_or_name ) );
   if( !uom_symbol.empty( ) )
      return uom_symbol;

   string name;
   pair< string, string > next;

   if( id_or_name.empty( ) )
      throw runtime_error( "unexpected empty field id_or_name for get_field_uom_symbol" );
   if( ( id_or_name == c_field_id_Access_Restriction ) || ( id_or_name == c_field_name_Access_Restriction ) )
   {
      name = string( c_field_display_name_Access_Restriction );
      get_module_string( c_field_display_name_Access_Restriction, &next );
   }
   if( ( id_or_name == c_field_id_Clone_Key ) || ( id_or_name == c_field_name_Clone_Key ) )
   {
      name = string( c_field_display_name_Clone_Key );
      get_module_string( c_field_display_name_Clone_Key, &next );
   }
   if( ( id_or_name == c_field_id_Create_Type ) || ( id_or_name == c_field_name_Create_Type ) )
   {
      name = string( c_field_display_name_Create_Type );
      get_module_string( c_field_display_name_Create_Type, &next );
   }
   if( ( id_or_name == c_field_id_New_Record_Class ) || ( id_or_name == c_field_name_New_Record_Class ) )
   {
      name = string( c_field_display_name_New_Record_Class );
      get_module_string( c_field_display_name_New_Record_Class, &next );
   }
   if( ( id_or_name == c_field_id_New_Record_FK_Field ) || ( id_or_name == c_field_name_New_Record_FK_Field ) )
   {
      name = string( c_field_display_name_New_Record_FK_Field );
      get_module_string( c_field_display_name_New_Record_FK_Field, &next );
   }
   if( ( id_or_name == c_field_id_New_Record_FK_Value ) || ( id_or_name == c_field_name_New_Record_FK_Value ) )
   {
      name = string( c_field_display_name_New_Record_FK_Value );
      get_module_string( c_field_display_name_New_Record_FK_Value, &next );
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

string Meta_Specification_Field_Action::get_field_display_name( const string& id_or_name ) const
{
   string display_name( parent_class_type::get_field_display_name( id_or_name ) );
   if( !display_name.empty( ) )
      return display_name;

   if( id_or_name.empty( ) )
      throw runtime_error( "unexpected empty field id_or_name for get_field_display_name" );
   if( ( id_or_name == c_field_id_Access_Restriction ) || ( id_or_name == c_field_name_Access_Restriction ) )
      display_name = get_module_string( c_field_display_name_Access_Restriction );
   if( ( id_or_name == c_field_id_Clone_Key ) || ( id_or_name == c_field_name_Clone_Key ) )
      display_name = get_module_string( c_field_display_name_Clone_Key );
   if( ( id_or_name == c_field_id_Create_Type ) || ( id_or_name == c_field_name_Create_Type ) )
      display_name = get_module_string( c_field_display_name_Create_Type );
   if( ( id_or_name == c_field_id_New_Record_Class ) || ( id_or_name == c_field_name_New_Record_Class ) )
      display_name = get_module_string( c_field_display_name_New_Record_Class );
   if( ( id_or_name == c_field_id_New_Record_FK_Field ) || ( id_or_name == c_field_name_New_Record_FK_Field ) )
      display_name = get_module_string( c_field_display_name_New_Record_FK_Field );
   if( ( id_or_name == c_field_id_New_Record_FK_Value ) || ( id_or_name == c_field_name_New_Record_FK_Value ) )
      display_name = get_module_string( c_field_display_name_New_Record_FK_Value );
   if( ( id_or_name == c_field_id_Type ) || ( id_or_name == c_field_name_Type ) )
      display_name = get_module_string( c_field_display_name_Type );

   return display_name;
}

void Meta_Specification_Field_Action::clear_foreign_key( const string& field )
{
   if( has_field( field ) )
      p_impl->clear_foreign_key( field );
   else
      parent_class_type::clear_foreign_key( field );
}

void Meta_Specification_Field_Action::set_foreign_key_value( const string& field, const string& value )
{
   if( has_field( field ) )
      p_impl->set_foreign_key_value( field, value );
   else
      parent_class_type::set_foreign_key_value( field, value );
}

const string& Meta_Specification_Field_Action::get_foreign_key_value( const string& field )
{
   if( has_field( field ) )
      return p_impl->get_foreign_key_value( field );
   else
      return parent_class_type::get_foreign_key_value( field );
}

void Meta_Specification_Field_Action::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   parent_class_type::get_foreign_key_values( foreign_key_values );
   p_impl->get_foreign_key_values( foreign_key_values );
}

void Meta_Specification_Field_Action::setup_foreign_key( Meta_Class& o, const string& value )
{
   static_cast< Meta_Class& >( o ).set_key( value );
}

void Meta_Specification_Field_Action::setup_foreign_key( Meta_Field& o, const string& value )
{
   static_cast< Meta_Field& >( o ).set_key( value );
}

void Meta_Specification_Field_Action::setup_graph_parent(
 Meta_Class& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Class& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Class& >( o ).set_key( init_value );
}

void Meta_Specification_Field_Action::setup_graph_parent(
 Meta_Field& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Field& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Field& >( o ).set_key( init_value );
}

size_t Meta_Specification_Field_Action::get_total_child_relationships( ) const
{
   return p_impl->total_child_relationships;
}

void Meta_Specification_Field_Action::set_total_child_relationships( size_t new_total_child_relationships ) const
{
   p_impl->total_child_relationships = new_total_child_relationships;
}

size_t Meta_Specification_Field_Action::get_num_foreign_key_children( bool is_internal ) const
{
   size_t rc;
   if( is_internal )
      rc = 0;
   else
      rc = Meta_Specification::get_num_foreign_key_children( );

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

class_base* Meta_Specification_Field_Action::get_next_foreign_key_child(
 size_t child_num, string& next_child_field, cascade_op op, bool is_internal )
{
   class_base* p_class_base = 0;

   p_class_base = Meta_Specification::get_next_foreign_key_child( child_num, next_child_field, op );

   return p_class_base;
}

void Meta_Specification_Field_Action::add_extra_fixed_info( vector< pair< string, string > >& fixed_info ) const
{
   parent_class_type::add_extra_fixed_info( fixed_info );
   p_impl->add_extra_fixed_info( fixed_info );
}

void Meta_Specification_Field_Action::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   parent_class_type::add_extra_paging_info( paging_info );
   p_impl->add_extra_paging_info( paging_info );
}

string Meta_Specification_Field_Action::get_class_id( ) const
{
   return static_class_id( );
}

string Meta_Specification_Field_Action::get_class_name( ) const
{
   return static_class_name( );
}

string Meta_Specification_Field_Action::get_plural_name( ) const
{
   return static_plural_name( );
}

string Meta_Specification_Field_Action::get_module_id( ) const
{
   return static_module_id( );
}

string Meta_Specification_Field_Action::get_module_name( ) const
{
   return static_module_name( );
}

string Meta_Specification_Field_Action::get_display_name( bool plural ) const
{
   string key( plural ? "plural_" : "class_" );

   key += "specification_field_action";

   return get_module_string( key );
}

string Meta_Specification_Field_Action::get_raw_variable( const std::string& name ) const
{
   if( name == g_state_names_variable )
      return p_impl->get_state_names( );
   else
      return class_base::get_raw_variable( name );
}

string Meta_Specification_Field_Action::get_create_instance_info( ) const
{
   return "";
}

string Meta_Specification_Field_Action::get_update_instance_info( ) const
{
   return "";
}

string Meta_Specification_Field_Action::get_destroy_instance_info( ) const
{
   return "";
}

string Meta_Specification_Field_Action::get_execute_procedure_info( const string& procedure_id ) const
{
   string retval;

   if( procedure_id.empty( ) )
      throw runtime_error( "unexpected empty procedure_id for get_execute_procedure_info" );
   else if( procedure_id == "128500" ) // i.e. Generate_All_Vars
      retval = "";

   return retval;
}

bool Meta_Specification_Field_Action::get_is_alias( ) const
{
   return false;
}

void Meta_Specification_Field_Action::get_alias_base_info( pair< string, string >& alias_base_info ) const
{
   ( void )alias_base_info;
}

void Meta_Specification_Field_Action::get_base_class_info( vector< pair< string, string > >& base_class_info ) const
{
   Meta_Specification::get_base_class_info( base_class_info );

   base_class_info.push_back( make_pair( Meta_Specification::get_class_id( ), Meta_Specification::get_class_name( ) ) );
}

class_base& Meta_Specification_Field_Action::get_or_create_graph_child( const string& context )
{
   class_base* p_class_base( 0 );

   string::size_type pos = context.find( '.' );
   string sub_context( context.substr( 0, pos ) );

   if( sub_context.empty( ) )
      throw runtime_error( "unexpected empty sub-context" );
   else if( ( sub_context == c_field_id_New_Record_Class ) || ( sub_context == c_field_name_New_Record_Class ) )
      p_class_base = &New_Record_Class( );
   else if( ( sub_context == c_field_id_New_Record_FK_Field ) || ( sub_context == c_field_name_New_Record_FK_Field ) )
      p_class_base = &New_Record_FK_Field( );
   else
      return parent_class_type::get_or_create_graph_child( context );

   if( !p_class_base )
      throw runtime_error( "unknown sub-context '" + sub_context + "'" );

   if( pos != string::npos )
      p_class_base = &p_class_base->get_or_create_graph_child( context.substr( pos + 1 ) );

   return *p_class_base;
}

void Meta_Specification_Field_Action::get_sql_column_names(
 vector< string >& names, bool* p_done, const string* p_class_name ) const
{
   parent_class_type::get_sql_column_names( names, p_done, p_class_name );
   if( p_done && *p_done )
      return;

   names.push_back( "C_Access_Restriction" );
   names.push_back( "C_Clone_Key" );
   names.push_back( "C_Create_Type" );
   names.push_back( "C_New_Record_Class" );
   names.push_back( "C_New_Record_FK_Field" );
   names.push_back( "C_New_Record_FK_Value" );
   names.push_back( "C_Type" );

   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;
}

void Meta_Specification_Field_Action::get_sql_column_values(
 vector< string >& values, bool* p_done, const string* p_class_name ) const
{
   parent_class_type::get_sql_column_values( values, p_done, p_class_name );
   if( p_done && *p_done )
      return;

   values.push_back( to_string( Access_Restriction( ) ) );
   values.push_back( sql_quote( to_string( Clone_Key( ) ) ) );
   values.push_back( to_string( Create_Type( ) ) );
   values.push_back( sql_quote( to_string( New_Record_Class( ) ) ) );
   values.push_back( sql_quote( to_string( New_Record_FK_Field( ) ) ) );
   values.push_back( sql_quote( to_string( New_Record_FK_Value( ) ) ) );
   values.push_back( to_string( Type( ) ) );

   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;
}

void Meta_Specification_Field_Action::get_required_field_names(
 set< string >& names, bool use_transients, set< string >* p_dependents ) const
{
   set< string > local_dependents;

   parent_class_type::get_required_field_names( names,
    use_transients, p_dependents ? p_dependents : &local_dependents );

   set< string >& dependents( p_dependents ? *p_dependents : local_dependents );

   get_always_required_field_names( names, use_transients, dependents );

   // [(start field_from_search_replace)] 600260
   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "New_Record_Class" );

      if( ( use_transients && is_field_transient( e_field_id_New_Record_Class ) )
       || ( !use_transients && !is_field_transient( e_field_id_New_Record_Class ) ) )
         names.insert( "New_Record_Class" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "New_Record_FK_Field" );

      if( ( use_transients && is_field_transient( e_field_id_New_Record_FK_Field ) )
       || ( !use_transients && !is_field_transient( e_field_id_New_Record_FK_Field ) ) )
         names.insert( "New_Record_FK_Field" );
   }

   if( needs_field_value( "Name", dependents ) )
   {
      dependents.insert( "Specification_Type" );

      if( ( use_transients && is_field_transient( e_field_id_Specification_Type ) )
       || ( !use_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
         names.insert( "Specification_Type" );
   }
   // [(finish field_from_search_replace)] 600260

   // [<start get_required_field_names>]
//nyi
   if( needs_field_value( "Vars" )
    && ( use_transients && is_transient_field( "Type" ) )
    || ( !use_transients && !is_transient_field( "Type" ) ) )
      names.insert( "Type" ); // (for Generate_All_Vars procedure)

   if( needs_field_value( "Vars" )
    && ( use_transients && is_transient_field( "Clone_Key" ) )
    || ( !use_transients && !is_transient_field( "Clone_Key" ) ) )
      names.insert( "Clone_Key" ); // (for Generate_All_Vars procedure)

   if( needs_field_value( "Vars" )
    && ( use_transients && is_transient_field( "Access_Restriction" ) )
    || ( !use_transients && !is_transient_field( "Access_Restriction" ) ) )
      names.insert( "Access_Restriction" ); // (for Generate_All_Vars procedure)

   if( needs_field_value( "Vars" )
    && ( use_transients && is_transient_field( "Procedure" ) )
    || ( !use_transients && !is_transient_field( "Procedure" ) ) )
      names.insert( "Procedure" ); // (for Generate_All_Vars procedure)

   if( needs_field_value( "Vars" )
    && ( use_transients && is_transient_field( "New_Record_Class" ) )
    || ( !use_transients && !is_transient_field( "New_Record_Class" ) ) )
      names.insert( "New_Record_Class" ); // (for Generate_All_Vars procedure)

   if( needs_field_value( "Vars" )
    && ( use_transients && is_transient_field( "New_Record_FK_Field" ) )
    || ( !use_transients && !is_transient_field( "New_Record_FK_Field" ) ) )
      names.insert( "New_Record_FK_Field" ); // (for Generate_All_Vars procedure)
   // [<finish get_required_field_names>]
}

void Meta_Specification_Field_Action::get_always_required_field_names(
 set< string >& names, bool use_transients, set< string >& dependents ) const
{
   parent_class_type::get_always_required_field_names( names, use_transients, dependents );

   // [(start modifier_field_value)] 600265
   dependents.insert( "Type" ); // (for Hide_Record_Create_Info modifier)

   if( ( use_transients && is_field_transient( e_field_id_Type ) )
    || ( !use_transients && !is_field_transient( e_field_id_Type ) ) )
      names.insert( "Type" );
   // [(finish modifier_field_value)] 600265

   // [<start get_always_required_field_names>]
   // [<finish get_always_required_field_names>]
}

void Meta_Specification_Field_Action::get_transient_replacement_field_names( const string& name, vector< string >& names ) const
{
   parent_class_type::get_transient_replacement_field_names( name, names );

   // [<start get_transient_replacement_field_names>]
   // [<finish get_transient_replacement_field_names>]
}

void Meta_Specification_Field_Action::do_generate_sql( generate_sql_type type,
 vector< string >& sql_stmts, set< string >& tx_key_info, vector< string >* p_sql_undo_stmts ) const
{
   parent_class_type::do_generate_sql( type, sql_stmts, tx_key_info, p_sql_undo_stmts );
   generate_sql( static_class_name( ), type, sql_stmts, tx_key_info, p_sql_undo_stmts );
}

const char* Meta_Specification_Field_Action::static_resolved_module_id( )
{
   return static_module_id( );
}

const char* Meta_Specification_Field_Action::static_resolved_module_name( )
{
   return static_module_name( );
}

const char* Meta_Specification_Field_Action::static_lock_class_id( )
{
   return parent_class_type::static_lock_class_id( );
}

const char* Meta_Specification_Field_Action::static_check_class_name( )
{
   return parent_class_type::static_check_class_name( );
}

const char* Meta_Specification_Field_Action::static_persistence_extra( )
{
   return "";
}

bool Meta_Specification_Field_Action::static_has_derivations( )
{
   return !g_derivations.empty( );
}

void Meta_Specification_Field_Action::static_get_class_info( class_info_container& class_info )
{
   parent_class_type::static_get_class_info( class_info );
   class_info.push_back( "100.128100" );
}

void Meta_Specification_Field_Action::static_get_field_info( field_info_container& all_field_info )
{
   parent_class_type::static_get_field_info( all_field_info );
   all_field_info.push_back( field_info( "128103", "Access_Restriction", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "128101", "Clone_Key", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "128102", "Create_Type", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "302300", "New_Record_Class", "Meta_Class", false, "", "" ) );
   all_field_info.push_back( field_info( "302310", "New_Record_FK_Field", "Meta_Field", false, "", "" ) );
   all_field_info.push_back( field_info( "128105", "New_Record_FK_Value", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "128104", "Type", "int", false, "", "" ) );
}

void Meta_Specification_Field_Action::static_get_foreign_key_info( foreign_key_info_container& foreign_key_info )
{
   ( void )foreign_key_info;

   parent_class_type::static_get_foreign_key_info( foreign_key_info );

   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_New_Record_Class, make_pair( "Meta.128100", "Meta_Class" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_New_Record_FK_Field, make_pair( "Meta.128100", "Meta_Field" ) ) );
}

int Meta_Specification_Field_Action::static_get_num_fields( bool* p_done, const string* p_class_name )
{
   int num_fields( parent_class_type::static_get_num_fields( p_done, p_class_name ) );

   if( p_done && *p_done )
      return num_fields;

   num_fields += c_num_fields;

   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;

   return num_fields;
}

bool Meta_Specification_Field_Action::static_is_field_encrypted( field_id id )
{
   return is_encrypted_field( static_get_field_id( id ) );
}

bool Meta_Specification_Field_Action::static_is_field_transient( field_id id )
{
   return is_transient_field( static_get_field_id( id ) );
}

const char* Meta_Specification_Field_Action::static_get_field_id( field_id id )
{
   const char* p_id = 0;

   int num_parent_fields( parent_class_type::static_get_num_fields( ) );

   if( ( int )id < num_parent_fields )
      p_id = parent_class_type::static_get_field_id( ( parent_class_type::field_id )id );

   switch( id )
   {
      case 1:
      p_id = "128103";
      break;

      case 2:
      p_id = "128101";
      break;

      case 3:
      p_id = "128102";
      break;

      case 4:
      p_id = "302300";
      break;

      case 5:
      p_id = "302310";
      break;

      case 6:
      p_id = "128105";
      break;

      case 7:
      p_id = "128104";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Specification_Field_Action" );

   return p_id;
}

const char* Meta_Specification_Field_Action::static_get_field_name( field_id id )
{
   const char* p_id = 0;

   int num_parent_fields( parent_class_type::static_get_num_fields( ) );

   if( ( int )id < num_parent_fields )
      p_id = parent_class_type::static_get_field_id( ( parent_class_type::field_id )id );

   switch( id )
   {
      case 1:
      p_id = "Access_Restriction";
      break;

      case 2:
      p_id = "Clone_Key";
      break;

      case 3:
      p_id = "Create_Type";
      break;

      case 4:
      p_id = "New_Record_Class";
      break;

      case 5:
      p_id = "New_Record_FK_Field";
      break;

      case 6:
      p_id = "New_Record_FK_Value";
      break;

      case 7:
      p_id = "Type";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Specification_Field_Action" );

   return p_id;
}

int Meta_Specification_Field_Action::static_get_field_num( const string& field )
{
   int rc = parent_class_type::static_get_field_num( field );
   if( rc >= 0 )
      return rc;

   rc = parent_class_type::static_get_num_fields( );

   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for static_get_field_num( )" );
   else if( field == c_field_id_Access_Restriction || field == c_field_name_Access_Restriction )
      rc += 1;
   else if( field == c_field_id_Clone_Key || field == c_field_name_Clone_Key )
      rc += 2;
   else if( field == c_field_id_Create_Type || field == c_field_name_Create_Type )
      rc += 3;
   else if( field == c_field_id_New_Record_Class || field == c_field_name_New_Record_Class )
      rc += 4;
   else if( field == c_field_id_New_Record_FK_Field || field == c_field_name_New_Record_FK_Field )
      rc += 5;
   else if( field == c_field_id_New_Record_FK_Value || field == c_field_name_New_Record_FK_Value )
      rc += 6;
   else if( field == c_field_id_Type || field == c_field_name_Type )
      rc += 7;

   return rc - 1;
}

procedure_info_container& Meta_Specification_Field_Action::static_get_procedure_info( )
{
   static bool initialised = false;
   static procedure_info_container& procedures( parent_class_type::static_get_procedure_info( ) );

   if( !initialised )
   {
      initialised = true;
      procedures.insert( make_pair( "128500", procedure_info( "Generate_All_Vars" ) ) );
   }

   return procedures;
}

string Meta_Specification_Field_Action::static_get_sql_columns( )
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
    "C_Access_Restriction INTEGER NOT NULL,"
    "C_Clone_Key VARCHAR(200) NOT NULL,"
    "C_Create_Type INTEGER NOT NULL,"
    "C_New_Record_Class VARCHAR(75) NOT NULL,"
    "C_New_Record_FK_Field VARCHAR(75) NOT NULL,"
    "C_New_Record_FK_Value VARCHAR(200) NOT NULL,"
    "C_Type INTEGER NOT NULL,"
    "PRIMARY KEY(C_Key_)";

   return sql_columns;
}

void Meta_Specification_Field_Action::static_get_text_search_fields( vector< string >& fields )
{
   parent_class_type::static_get_text_search_fields( fields );
}

void Meta_Specification_Field_Action::static_get_all_enum_pairs( vector< pair< string, string > >& pairs )
{
   parent_class_type::static_get_all_enum_pairs( pairs );
   pairs.push_back( make_pair( "enum_field_action_create_access_restriction_0", get_enum_string_field_action_create_access_restriction( 0 ) ) );
   pairs.push_back( make_pair( "enum_field_action_create_access_restriction_1", get_enum_string_field_action_create_access_restriction( 1 ) ) );
   pairs.push_back( make_pair( "enum_field_action_create_access_restriction_2", get_enum_string_field_action_create_access_restriction( 2 ) ) );
   pairs.push_back( make_pair( "enum_field_action_create_access_restriction_3", get_enum_string_field_action_create_access_restriction( 3 ) ) );

   pairs.push_back( make_pair( "enum_field_action_create_type_0", get_enum_string_field_action_create_type( 0 ) ) );
   pairs.push_back( make_pair( "enum_field_action_create_type_1", get_enum_string_field_action_create_type( 1 ) ) );
   pairs.push_back( make_pair( "enum_field_action_create_type_2", get_enum_string_field_action_create_type( 2 ) ) );

   pairs.push_back( make_pair( "enum_field_action_type_0", get_enum_string_field_action_type( 0 ) ) );
   pairs.push_back( make_pair( "enum_field_action_type_1", get_enum_string_field_action_type( 1 ) ) );
}

void Meta_Specification_Field_Action::static_get_all_index_pairs( vector< pair< string, string > >& pairs )
{
   parent_class_type::static_get_all_index_pairs( pairs );
}

void Meta_Specification_Field_Action::static_get_all_unique_indexes( vector< string >& unique_indexes )
{
   parent_class_type::static_get_all_unique_indexes( unique_indexes );
}

void Meta_Specification_Field_Action::static_get_sql_indexes( vector< string >& indexes )
{
   parent_class_type::static_get_sql_indexes( indexes );
}

void Meta_Specification_Field_Action::static_get_sql_unique_indexes( vector< string >& indexes )
{
   parent_class_type::static_get_sql_unique_indexes( indexes );
}

void Meta_Specification_Field_Action::static_insert_derivation( const string& module_and_class_id )
{
   g_derivations.insert( module_and_class_id );
}

void Meta_Specification_Field_Action::static_remove_derivation( const string& module_and_class_id )
{
   if( g_derivations.count( module_and_class_id ) )
      g_derivations.erase( module_and_class_id );
}

void Meta_Specification_Field_Action::static_insert_external_alias( const string& module_and_class_id, Meta_Specification_Field_Action* p_instance )
{
   g_external_aliases.insert( external_aliases_value_type( module_and_class_id, p_instance ) );
}

void Meta_Specification_Field_Action::static_remove_external_alias( const string& module_and_class_id )
{
   if( g_external_aliases.count( module_and_class_id ) )
   {
      delete g_external_aliases[ module_and_class_id ];
      g_external_aliases.erase( module_and_class_id );
   }
}

void Meta_Specification_Field_Action::static_class_init( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for init" );

   g_state_names_variable = get_special_var_name( e_special_var_state_names );

   Meta_Specification::static_insert_derivation( "Meta_Specification_Field_Action" );

   g_field_action_create_access_restriction_enum.insert( 0 );
   g_field_action_create_access_restriction_enum.insert( 1 );
   g_field_action_create_access_restriction_enum.insert( 2 );
   g_field_action_create_access_restriction_enum.insert( 3 );

   g_field_action_create_type_enum.insert( 0 );
   g_field_action_create_type_enum.insert( 1 );
   g_field_action_create_type_enum.insert( 2 );

   g_field_action_type_enum.insert( 0 );
   g_field_action_type_enum.insert( 1 );

   // [<start static_class_init>]
   // [<finish static_class_init>]
}

void Meta_Specification_Field_Action::static_class_term( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for term" );

   Meta_Specification::static_remove_derivation( "Meta_Specification_Field_Action" );

   // [<start static_class_term>]
   // [<finish static_class_term>]
}
