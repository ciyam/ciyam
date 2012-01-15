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

#include "Meta_Specification_Content_Page.h"

#include "Meta_Enum.h"
#include "Meta_Enum_Item.h"
#include "Meta_List_Field.h"
#include "Meta_Model.h"
#include "Meta_Modifier.h"
#include "Meta_Permission.h"
#include "Meta_Specification.h"
#include "Meta_Specification_Copy_Child_Links.h"
#include "Meta_Specification_Type.h"
#include "Meta_View_Field.h"
#include "Meta_Field.h"
#include "Meta_Relationship.h"
#include "Meta_Procedure_Arg.h"
#include "Meta_Procedure.h"
#include "Meta_Class.h"

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

template< > inline string to_string( const Meta_Field& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_Relationship& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_Procedure_Arg& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_Procedure& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_Class& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

inline void from_string( Meta_Field& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_Relationship& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_Procedure_Arg& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_Procedure& c, const string& s )
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

#include "Meta_Specification_Content_Page.cmh"

const int32_t c_version = 1;

const char* const c_okay = "okay";

const char* const c_field_id_Actions = "115108";
const char* const c_field_id_All_Strings = "115124";
const char* const c_field_id_All_Vars = "115123";
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
const char* const c_field_id_Other_Field = "301435";
const char* const c_field_id_Other_Field_2 = "301436";
const char* const c_field_id_Other_Procedure = "301451";
const char* const c_field_id_Other_Procedure_2 = "301452";
const char* const c_field_id_Parent_Specification = "301405";
const char* const c_field_id_Parent_Specification_Copy_Child_Links = "302550";
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
const char* const c_field_id_Source_Child_2 = "301496";
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
const char* const c_field_id_Use_Source_Parent = "115107";
const char* const c_field_id_Use_Test_Parent_Child = "115110";
const char* const c_field_id_Value = "115102";
const char* const c_field_id_Value_Left_Part = "115129";
const char* const c_field_id_Value_Literal = "115116";
const char* const c_field_id_Value_Numeric_String = "115127";
const char* const c_field_id_Value_Right_Part = "115130";
const char* const c_field_id_Value_String = "115125";
const char* const c_field_id_Vars = "115114";

const char* const c_field_id_Actions_Field = "302629";
const char* const c_field_id_Child_Self_Relationship = "302620";
const char* const c_field_id_Clone_Permitted_Field = "302607";
const char* const c_field_id_Code_Field = "302610";
const char* const c_field_id_Content_Type = "302605";
const char* const c_field_id_Create_Copy_Output_Arg = "302644";
const char* const c_field_id_Create_Copy_Procedure = "302628";
const char* const c_field_id_File_Name_Field = "302612";
const char* const c_field_id_File_Name_Gen_Field = "302608";
const char* const c_field_id_Frag_Code_Field = "302640";
const char* const c_field_id_Frag_Content_Field = "302642";
const char* const c_field_id_Frag_Group_Field = "302641";
const char* const c_field_id_Fragment_Class = "302602";
const char* const c_field_id_Generate_Children_Field = "302619";
const char* const c_field_id_Generate_Output_Arg = "302626";
const char* const c_field_id_Generate_Procedure = "302625";
const char* const c_field_id_Group_Base_Path_Field = "302647";
const char* const c_field_id_Group_Class = "302603";
const char* const c_field_id_Group_Extra_Field = "302646";
const char* const c_field_id_Group_Field = "302609";
const char* const c_field_id_Group_Name_Field = "302645";
const char* const c_field_id_Hyperlink_Title_Field = "302618";
const char* const c_field_id_Link_Class = "302604";
const char* const c_field_id_Link_Content_Field = "302617";
const char* const c_field_id_Link_Group_Field = "302622";
const char* const c_field_id_Link_Name_Field = "302621";
const char* const c_field_id_Link_Page_Link_Child = "302632";
const char* const c_field_id_Meta_Content_Field = "302616";
const char* const c_field_id_Next_Child_Num_Field = "302643";
const char* const c_field_id_Order_Field = "302630";
const char* const c_field_id_Owned_Links_Field = "302623";
const char* const c_field_id_Page_Field = "302631";
const char* const c_field_id_Page_File_Class = "302601";
const char* const c_field_id_Page_File_Field = "302635";
const char* const c_field_id_Page_File_Name_Field = "302636";
const char* const c_field_id_Page_File_Name_Gen_Field = "302606";
const char* const c_field_id_Page_Link_Class = "302600";
const char* const c_field_id_Page_Page_Link_Child = "302627";
const char* const c_field_id_Parent_Page_Field = "302614";
const char* const c_field_id_Sub_Title_Field = "302613";
const char* const c_field_id_Text_Content_Field = "302615";
const char* const c_field_id_Title_Field = "302611";
const char* const c_field_id_Variable_Class = "302637";
const char* const c_field_id_Variable_Name = "302638";
const char* const c_field_id_Variable_Value = "302639";

const char* const c_field_name_Actions = "Actions";
const char* const c_field_name_All_Strings = "All_Strings";
const char* const c_field_name_All_Vars = "All_Vars";
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
const char* const c_field_name_Other_Field = "Other_Field";
const char* const c_field_name_Other_Field_2 = "Other_Field_2";
const char* const c_field_name_Other_Procedure = "Other_Procedure";
const char* const c_field_name_Other_Procedure_2 = "Other_Procedure_2";
const char* const c_field_name_Parent_Specification = "Parent_Specification";
const char* const c_field_name_Parent_Specification_Copy_Child_Links = "Parent_Specification_Copy_Child_Links";
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
const char* const c_field_name_Source_Child_2 = "Source_Child_2";
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
const char* const c_field_name_Use_Source_Parent = "Use_Source_Parent";
const char* const c_field_name_Use_Test_Parent_Child = "Use_Test_Parent_Child";
const char* const c_field_name_Value = "Value";
const char* const c_field_name_Value_Left_Part = "Value_Left_Part";
const char* const c_field_name_Value_Literal = "Value_Literal";
const char* const c_field_name_Value_Numeric_String = "Value_Numeric_String";
const char* const c_field_name_Value_Right_Part = "Value_Right_Part";
const char* const c_field_name_Value_String = "Value_String";
const char* const c_field_name_Vars = "Vars";

const char* const c_field_name_Actions_Field = "Actions_Field";
const char* const c_field_name_Child_Self_Relationship = "Child_Self_Relationship";
const char* const c_field_name_Clone_Permitted_Field = "Clone_Permitted_Field";
const char* const c_field_name_Code_Field = "Code_Field";
const char* const c_field_name_Content_Type = "Content_Type";
const char* const c_field_name_Create_Copy_Output_Arg = "Create_Copy_Output_Arg";
const char* const c_field_name_Create_Copy_Procedure = "Create_Copy_Procedure";
const char* const c_field_name_File_Name_Field = "File_Name_Field";
const char* const c_field_name_File_Name_Gen_Field = "File_Name_Gen_Field";
const char* const c_field_name_Frag_Code_Field = "Frag_Code_Field";
const char* const c_field_name_Frag_Content_Field = "Frag_Content_Field";
const char* const c_field_name_Frag_Group_Field = "Frag_Group_Field";
const char* const c_field_name_Fragment_Class = "Fragment_Class";
const char* const c_field_name_Generate_Children_Field = "Generate_Children_Field";
const char* const c_field_name_Generate_Output_Arg = "Generate_Output_Arg";
const char* const c_field_name_Generate_Procedure = "Generate_Procedure";
const char* const c_field_name_Group_Base_Path_Field = "Group_Base_Path_Field";
const char* const c_field_name_Group_Class = "Group_Class";
const char* const c_field_name_Group_Extra_Field = "Group_Extra_Field";
const char* const c_field_name_Group_Field = "Group_Field";
const char* const c_field_name_Group_Name_Field = "Group_Name_Field";
const char* const c_field_name_Hyperlink_Title_Field = "Hyperlink_Title_Field";
const char* const c_field_name_Link_Class = "Link_Class";
const char* const c_field_name_Link_Content_Field = "Link_Content_Field";
const char* const c_field_name_Link_Group_Field = "Link_Group_Field";
const char* const c_field_name_Link_Name_Field = "Link_Name_Field";
const char* const c_field_name_Link_Page_Link_Child = "Link_Page_Link_Child";
const char* const c_field_name_Meta_Content_Field = "Meta_Content_Field";
const char* const c_field_name_Next_Child_Num_Field = "Next_Child_Num_Field";
const char* const c_field_name_Order_Field = "Order_Field";
const char* const c_field_name_Owned_Links_Field = "Owned_Links_Field";
const char* const c_field_name_Page_Field = "Page_Field";
const char* const c_field_name_Page_File_Class = "Page_File_Class";
const char* const c_field_name_Page_File_Field = "Page_File_Field";
const char* const c_field_name_Page_File_Name_Field = "Page_File_Name_Field";
const char* const c_field_name_Page_File_Name_Gen_Field = "Page_File_Name_Gen_Field";
const char* const c_field_name_Page_Link_Class = "Page_Link_Class";
const char* const c_field_name_Page_Page_Link_Child = "Page_Page_Link_Child";
const char* const c_field_name_Parent_Page_Field = "Parent_Page_Field";
const char* const c_field_name_Sub_Title_Field = "Sub_Title_Field";
const char* const c_field_name_Text_Content_Field = "Text_Content_Field";
const char* const c_field_name_Title_Field = "Title_Field";
const char* const c_field_name_Variable_Class = "Variable_Class";
const char* const c_field_name_Variable_Name = "Variable_Name";
const char* const c_field_name_Variable_Value = "Variable_Value";

const char* const c_field_display_name_Actions = "field_specification_actions";
const char* const c_field_display_name_All_Strings = "field_specification_all_strings";
const char* const c_field_display_name_All_Vars = "field_specification_all_vars";
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
const char* const c_field_display_name_Other_Field = "field_specification_other_field";
const char* const c_field_display_name_Other_Field_2 = "field_specification_other_field_2";
const char* const c_field_display_name_Other_Procedure = "field_specification_other_procedure";
const char* const c_field_display_name_Other_Procedure_2 = "field_specification_other_procedure_2";
const char* const c_field_display_name_Parent_Specification = "field_specification_parent_specification";
const char* const c_field_display_name_Parent_Specification_Copy_Child_Links = "field_specification_parent_specification_copy_child_links";
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
const char* const c_field_display_name_Source_Child_2 = "field_specification_source_child_2";
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
const char* const c_field_display_name_Use_Source_Parent = "field_specification_use_source_parent";
const char* const c_field_display_name_Use_Test_Parent_Child = "field_specification_use_test_parent_child";
const char* const c_field_display_name_Value = "field_specification_value";
const char* const c_field_display_name_Value_Left_Part = "field_specification_value_left_part";
const char* const c_field_display_name_Value_Literal = "field_specification_value_literal";
const char* const c_field_display_name_Value_Numeric_String = "field_specification_value_numeric_string";
const char* const c_field_display_name_Value_Right_Part = "field_specification_value_right_part";
const char* const c_field_display_name_Value_String = "field_specification_value_string";
const char* const c_field_display_name_Vars = "field_specification_vars";

const char* const c_field_display_name_Actions_Field = "field_specification_content_page_actions_field";
const char* const c_field_display_name_Child_Self_Relationship = "field_specification_content_page_child_self_relationship";
const char* const c_field_display_name_Clone_Permitted_Field = "field_specification_content_page_clone_permitted_field";
const char* const c_field_display_name_Code_Field = "field_specification_content_page_code_field";
const char* const c_field_display_name_Content_Type = "field_specification_content_page_content_type";
const char* const c_field_display_name_Create_Copy_Output_Arg = "field_specification_content_page_create_copy_output_arg";
const char* const c_field_display_name_Create_Copy_Procedure = "field_specification_content_page_create_copy_procedure";
const char* const c_field_display_name_File_Name_Field = "field_specification_content_page_file_name_field";
const char* const c_field_display_name_File_Name_Gen_Field = "field_specification_content_page_file_name_gen_field";
const char* const c_field_display_name_Frag_Code_Field = "field_specification_content_page_frag_code_field";
const char* const c_field_display_name_Frag_Content_Field = "field_specification_content_page_frag_content_field";
const char* const c_field_display_name_Frag_Group_Field = "field_specification_content_page_frag_group_field";
const char* const c_field_display_name_Fragment_Class = "field_specification_content_page_fragment_class";
const char* const c_field_display_name_Generate_Children_Field = "field_specification_content_page_generate_children_field";
const char* const c_field_display_name_Generate_Output_Arg = "field_specification_content_page_generate_output_arg";
const char* const c_field_display_name_Generate_Procedure = "field_specification_content_page_generate_procedure";
const char* const c_field_display_name_Group_Base_Path_Field = "field_specification_content_page_group_base_path_field";
const char* const c_field_display_name_Group_Class = "field_specification_content_page_group_class";
const char* const c_field_display_name_Group_Extra_Field = "field_specification_content_page_group_extra_field";
const char* const c_field_display_name_Group_Field = "field_specification_content_page_group_field";
const char* const c_field_display_name_Group_Name_Field = "field_specification_content_page_group_name_field";
const char* const c_field_display_name_Hyperlink_Title_Field = "field_specification_content_page_hyperlink_title_field";
const char* const c_field_display_name_Link_Class = "field_specification_content_page_link_class";
const char* const c_field_display_name_Link_Content_Field = "field_specification_content_page_link_content_field";
const char* const c_field_display_name_Link_Group_Field = "field_specification_content_page_link_group_field";
const char* const c_field_display_name_Link_Name_Field = "field_specification_content_page_link_name_field";
const char* const c_field_display_name_Link_Page_Link_Child = "field_specification_content_page_link_page_link_child";
const char* const c_field_display_name_Meta_Content_Field = "field_specification_content_page_meta_content_field";
const char* const c_field_display_name_Next_Child_Num_Field = "field_specification_content_page_next_child_num_field";
const char* const c_field_display_name_Order_Field = "field_specification_content_page_order_field";
const char* const c_field_display_name_Owned_Links_Field = "field_specification_content_page_owned_links_field";
const char* const c_field_display_name_Page_Field = "field_specification_content_page_page_field";
const char* const c_field_display_name_Page_File_Class = "field_specification_content_page_page_file_class";
const char* const c_field_display_name_Page_File_Field = "field_specification_content_page_page_file_field";
const char* const c_field_display_name_Page_File_Name_Field = "field_specification_content_page_page_file_name_field";
const char* const c_field_display_name_Page_File_Name_Gen_Field = "field_specification_content_page_page_file_name_gen_field";
const char* const c_field_display_name_Page_Link_Class = "field_specification_content_page_page_link_class";
const char* const c_field_display_name_Page_Page_Link_Child = "field_specification_content_page_page_page_link_child";
const char* const c_field_display_name_Parent_Page_Field = "field_specification_content_page_parent_page_field";
const char* const c_field_display_name_Sub_Title_Field = "field_specification_content_page_sub_title_field";
const char* const c_field_display_name_Text_Content_Field = "field_specification_content_page_text_content_field";
const char* const c_field_display_name_Title_Field = "field_specification_content_page_title_field";
const char* const c_field_display_name_Variable_Class = "field_specification_content_page_variable_class";
const char* const c_field_display_name_Variable_Name = "field_specification_content_page_variable_name";
const char* const c_field_display_name_Variable_Value = "field_specification_content_page_variable_value";

const int c_num_fields = 45;

const char* const c_all_sorted_field_ids[ ] =
{
   "302600",
   "302601",
   "302602",
   "302603",
   "302604",
   "302605",
   "302606",
   "302607",
   "302608",
   "302609",
   "302610",
   "302611",
   "302612",
   "302613",
   "302614",
   "302615",
   "302616",
   "302617",
   "302618",
   "302619",
   "302620",
   "302621",
   "302622",
   "302623",
   "302625",
   "302626",
   "302627",
   "302628",
   "302629",
   "302630",
   "302631",
   "302632",
   "302635",
   "302636",
   "302637",
   "302638",
   "302639",
   "302640",
   "302641",
   "302642",
   "302643",
   "302644",
   "302645",
   "302646",
   "302647"
};

const char* const c_all_sorted_field_names[ ] =
{
   "Actions_Field",
   "Child_Self_Relationship",
   "Clone_Permitted_Field",
   "Code_Field",
   "Content_Type",
   "Create_Copy_Output_Arg",
   "Create_Copy_Procedure",
   "File_Name_Field",
   "File_Name_Gen_Field",
   "Frag_Code_Field",
   "Frag_Content_Field",
   "Frag_Group_Field",
   "Fragment_Class",
   "Generate_Children_Field",
   "Generate_Output_Arg",
   "Generate_Procedure",
   "Group_Base_Path_Field",
   "Group_Class",
   "Group_Extra_Field",
   "Group_Field",
   "Group_Name_Field",
   "Hyperlink_Title_Field",
   "Link_Class",
   "Link_Content_Field",
   "Link_Group_Field",
   "Link_Name_Field",
   "Link_Page_Link_Child",
   "Meta_Content_Field",
   "Next_Child_Num_Field",
   "Order_Field",
   "Owned_Links_Field",
   "Page_Field",
   "Page_File_Class",
   "Page_File_Field",
   "Page_File_Name_Field",
   "Page_File_Name_Gen_Field",
   "Page_Link_Class",
   "Page_Page_Link_Child",
   "Parent_Page_Field",
   "Sub_Title_Field",
   "Text_Content_Field",
   "Title_Field",
   "Variable_Class",
   "Variable_Name",
   "Variable_Value"
};

inline bool compare( const char* p_s1, const char* p_s2 ) { return strcmp( p_s1, p_s2 ) < 0; }

inline bool has_field( const string& field )
{
   return binary_search( c_all_sorted_field_ids, c_all_sorted_field_ids + c_num_fields, field.c_str( ), compare )
    || binary_search( c_all_sorted_field_names, c_all_sorted_field_names + c_num_fields, field.c_str( ), compare );
}

const int c_num_transient_fields = 0;

bool is_transient_field( const string& ) { static bool false_value( false ); return false_value; }

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
const uint64_t c_modifier_Hide_Other_Field = UINT64_C( 0x80000 );
const uint64_t c_modifier_Hide_Other_Field_2 = UINT64_C( 0x100000 );
const uint64_t c_modifier_Hide_Other_Procedure = UINT64_C( 0x200000 );
const uint64_t c_modifier_Hide_Other_Procedure_2 = UINT64_C( 0x400000 );
const uint64_t c_modifier_Hide_Permission = UINT64_C( 0x800000 );
const uint64_t c_modifier_Hide_Procedure = UINT64_C( 0x1000000 );
const uint64_t c_modifier_Hide_Procedure_Arg = UINT64_C( 0x2000000 );
const uint64_t c_modifier_Hide_Procedure_Arg_2 = UINT64_C( 0x4000000 );
const uint64_t c_modifier_Hide_Procedure_Arg_3 = UINT64_C( 0x8000000 );
const uint64_t c_modifier_Hide_Source_Child = UINT64_C( 0x10000000 );
const uint64_t c_modifier_Hide_Source_Child_2 = UINT64_C( 0x20000000 );
const uint64_t c_modifier_Hide_Source_Class = UINT64_C( 0x40000000 );
const uint64_t c_modifier_Hide_Source_Field = UINT64_C( 0x80000000 );
const uint64_t c_modifier_Hide_Source_Grandchild = UINT64_C( 0x100000000 );
const uint64_t c_modifier_Hide_Source_Parent = UINT64_C( 0x200000000 );
const uint64_t c_modifier_Hide_Test_Child = UINT64_C( 0x400000000 );
const uint64_t c_modifier_Hide_Test_Field = UINT64_C( 0x800000000 );
const uint64_t c_modifier_Hide_Test_Parent = UINT64_C( 0x1000000000 );
const uint64_t c_modifier_Hide_Test_Value = UINT64_C( 0x2000000000 );
const uint64_t c_modifier_Hide_Use_Source_Parent = UINT64_C( 0x4000000000 );
const uint64_t c_modifier_Hide_Use_Test_Parent_Child = UINT64_C( 0x8000000000 );
const uint64_t c_modifier_Hide_Value = UINT64_C( 0x10000000000 );
const uint64_t c_modifier_Protect_Child_Relationship = UINT64_C( 0x20000000000 );
const uint64_t c_modifier_Protect_Class = UINT64_C( 0x40000000000 );
const uint64_t c_modifier_Protect_Enum = UINT64_C( 0x80000000000 );
const uint64_t c_modifier_Protect_Procedure = UINT64_C( 0x100000000000 );
const uint64_t c_modifier_Protect_Source_Parent = UINT64_C( 0x200000000000 );

set< string > g_derivations;

typedef map< string, Meta_Specification_Content_Page* > external_aliases_container;
typedef external_aliases_container::const_iterator external_aliases_const_iterator;
typedef external_aliases_container::value_type external_aliases_value_type;

typedef map< size_t, Meta_Specification_Content_Page* > external_aliases_lookup_container;
typedef external_aliases_lookup_container::const_iterator external_aliases_lookup_const_iterator;

external_aliases_container g_external_aliases;
external_aliases_lookup_container g_external_aliases_lookup;

// [<start anonymous>]
// [<finish anonymous>]

}

registration< Meta_Specification_Content_Page > Specification_Content_Page_registration( get_class_registry( ), "131100" );

class Meta_Specification_Content_Page_command_functor;

class Meta_Specification_Content_Page_command_handler : public command_handler
{
   friend class Meta_Specification_Content_Page_command_functor;

   public:
   Meta_Specification_Content_Page_command_handler( ) : p_Meta_Specification_Content_Page( 0 ) { }

   void set_Meta_Specification_Content_Page( Meta_Specification_Content_Page* p_new_Meta_Specification_Content_Page ) { p_Meta_Specification_Content_Page = p_new_Meta_Specification_Content_Page; }

   void handle_unknown_command( const string& command )
   {
      ( void )command;

      retval = p_Meta_Specification_Content_Page->Meta_Specification::execute( get_cmd_and_args( ) );
   }

   void handle_invalid_command( const command_parser& parser, const string& cmd_and_args )
   {
      throw runtime_error( "invalid command usage '" + cmd_and_args + "'" );
   }

   private:
   Meta_Specification_Content_Page* p_Meta_Specification_Content_Page;

   protected:
   string retval;
   string cmd_and_args;
};

class Meta_Specification_Content_Page_command_functor : public command_functor
{
   public:
   Meta_Specification_Content_Page_command_functor( Meta_Specification_Content_Page_command_handler& handler )
    : command_functor( handler ),
    cmd_handler( handler )
   {
   }

   void operator ( )( const string& command, const parameter_info& parameters );

   private:
   Meta_Specification_Content_Page_command_handler& cmd_handler;
};

command_functor* Meta_Specification_Content_Page_command_functor_factory( const string& /*name*/, command_handler& handler )
{
   return new Meta_Specification_Content_Page_command_functor( dynamic_cast< Meta_Specification_Content_Page_command_handler& >( handler ) );
}

void Meta_Specification_Content_Page_command_functor::operator ( )( const string& command, const parameter_info& parameters )
{
   if( command == c_cmd_Meta_Specification_Content_Page_key )
   {
      bool want_fixed( has_parm_val( parameters, c_cmd_parm_Meta_Specification_Content_Page_key_fixed ) );

      if( !want_fixed )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Content_Page->get_key( );
      else
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Content_Page->get_fixed_key( );
   }
   else if( command == c_cmd_Meta_Specification_Content_Page_ver )
   {
      string ver_rev( to_string( cmd_handler.p_Meta_Specification_Content_Page->get_version( ) ) );
      ver_rev += "." + to_string( cmd_handler.p_Meta_Specification_Content_Page->get_revision( ) );

      cmd_handler.retval = ver_rev;
   }
   else if( command == c_cmd_Meta_Specification_Content_Page_get )
   {
      string field_name( get_parm_val( parameters, c_cmd_parm_Meta_Specification_Content_Page_get_field_name ) );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for getter call" );
      else if( field_name == c_field_id_Actions_Field || field_name == c_field_name_Actions_Field )
         string_getter< Meta_Field >( cmd_handler.p_Meta_Specification_Content_Page->Actions_Field( ), cmd_handler.retval );
      else if( field_name == c_field_id_Child_Self_Relationship || field_name == c_field_name_Child_Self_Relationship )
         string_getter< Meta_Relationship >( cmd_handler.p_Meta_Specification_Content_Page->Child_Self_Relationship( ), cmd_handler.retval );
      else if( field_name == c_field_id_Clone_Permitted_Field || field_name == c_field_name_Clone_Permitted_Field )
         string_getter< Meta_Field >( cmd_handler.p_Meta_Specification_Content_Page->Clone_Permitted_Field( ), cmd_handler.retval );
      else if( field_name == c_field_id_Code_Field || field_name == c_field_name_Code_Field )
         string_getter< Meta_Field >( cmd_handler.p_Meta_Specification_Content_Page->Code_Field( ), cmd_handler.retval );
      else if( field_name == c_field_id_Content_Type || field_name == c_field_name_Content_Type )
         string_getter< Meta_Field >( cmd_handler.p_Meta_Specification_Content_Page->Content_Type( ), cmd_handler.retval );
      else if( field_name == c_field_id_Create_Copy_Output_Arg || field_name == c_field_name_Create_Copy_Output_Arg )
         string_getter< Meta_Procedure_Arg >( cmd_handler.p_Meta_Specification_Content_Page->Create_Copy_Output_Arg( ), cmd_handler.retval );
      else if( field_name == c_field_id_Create_Copy_Procedure || field_name == c_field_name_Create_Copy_Procedure )
         string_getter< Meta_Procedure >( cmd_handler.p_Meta_Specification_Content_Page->Create_Copy_Procedure( ), cmd_handler.retval );
      else if( field_name == c_field_id_File_Name_Field || field_name == c_field_name_File_Name_Field )
         string_getter< Meta_Field >( cmd_handler.p_Meta_Specification_Content_Page->File_Name_Field( ), cmd_handler.retval );
      else if( field_name == c_field_id_File_Name_Gen_Field || field_name == c_field_name_File_Name_Gen_Field )
         string_getter< Meta_Field >( cmd_handler.p_Meta_Specification_Content_Page->File_Name_Gen_Field( ), cmd_handler.retval );
      else if( field_name == c_field_id_Frag_Code_Field || field_name == c_field_name_Frag_Code_Field )
         string_getter< Meta_Field >( cmd_handler.p_Meta_Specification_Content_Page->Frag_Code_Field( ), cmd_handler.retval );
      else if( field_name == c_field_id_Frag_Content_Field || field_name == c_field_name_Frag_Content_Field )
         string_getter< Meta_Field >( cmd_handler.p_Meta_Specification_Content_Page->Frag_Content_Field( ), cmd_handler.retval );
      else if( field_name == c_field_id_Frag_Group_Field || field_name == c_field_name_Frag_Group_Field )
         string_getter< Meta_Field >( cmd_handler.p_Meta_Specification_Content_Page->Frag_Group_Field( ), cmd_handler.retval );
      else if( field_name == c_field_id_Fragment_Class || field_name == c_field_name_Fragment_Class )
         string_getter< Meta_Class >( cmd_handler.p_Meta_Specification_Content_Page->Fragment_Class( ), cmd_handler.retval );
      else if( field_name == c_field_id_Generate_Children_Field || field_name == c_field_name_Generate_Children_Field )
         string_getter< Meta_Field >( cmd_handler.p_Meta_Specification_Content_Page->Generate_Children_Field( ), cmd_handler.retval );
      else if( field_name == c_field_id_Generate_Output_Arg || field_name == c_field_name_Generate_Output_Arg )
         string_getter< Meta_Procedure_Arg >( cmd_handler.p_Meta_Specification_Content_Page->Generate_Output_Arg( ), cmd_handler.retval );
      else if( field_name == c_field_id_Generate_Procedure || field_name == c_field_name_Generate_Procedure )
         string_getter< Meta_Procedure >( cmd_handler.p_Meta_Specification_Content_Page->Generate_Procedure( ), cmd_handler.retval );
      else if( field_name == c_field_id_Group_Base_Path_Field || field_name == c_field_name_Group_Base_Path_Field )
         string_getter< Meta_Field >( cmd_handler.p_Meta_Specification_Content_Page->Group_Base_Path_Field( ), cmd_handler.retval );
      else if( field_name == c_field_id_Group_Class || field_name == c_field_name_Group_Class )
         string_getter< Meta_Class >( cmd_handler.p_Meta_Specification_Content_Page->Group_Class( ), cmd_handler.retval );
      else if( field_name == c_field_id_Group_Extra_Field || field_name == c_field_name_Group_Extra_Field )
         string_getter< Meta_Field >( cmd_handler.p_Meta_Specification_Content_Page->Group_Extra_Field( ), cmd_handler.retval );
      else if( field_name == c_field_id_Group_Field || field_name == c_field_name_Group_Field )
         string_getter< Meta_Field >( cmd_handler.p_Meta_Specification_Content_Page->Group_Field( ), cmd_handler.retval );
      else if( field_name == c_field_id_Group_Name_Field || field_name == c_field_name_Group_Name_Field )
         string_getter< Meta_Field >( cmd_handler.p_Meta_Specification_Content_Page->Group_Name_Field( ), cmd_handler.retval );
      else if( field_name == c_field_id_Hyperlink_Title_Field || field_name == c_field_name_Hyperlink_Title_Field )
         string_getter< Meta_Field >( cmd_handler.p_Meta_Specification_Content_Page->Hyperlink_Title_Field( ), cmd_handler.retval );
      else if( field_name == c_field_id_Link_Class || field_name == c_field_name_Link_Class )
         string_getter< Meta_Class >( cmd_handler.p_Meta_Specification_Content_Page->Link_Class( ), cmd_handler.retval );
      else if( field_name == c_field_id_Link_Content_Field || field_name == c_field_name_Link_Content_Field )
         string_getter< Meta_Field >( cmd_handler.p_Meta_Specification_Content_Page->Link_Content_Field( ), cmd_handler.retval );
      else if( field_name == c_field_id_Link_Group_Field || field_name == c_field_name_Link_Group_Field )
         string_getter< Meta_Field >( cmd_handler.p_Meta_Specification_Content_Page->Link_Group_Field( ), cmd_handler.retval );
      else if( field_name == c_field_id_Link_Name_Field || field_name == c_field_name_Link_Name_Field )
         string_getter< Meta_Field >( cmd_handler.p_Meta_Specification_Content_Page->Link_Name_Field( ), cmd_handler.retval );
      else if( field_name == c_field_id_Link_Page_Link_Child || field_name == c_field_name_Link_Page_Link_Child )
         string_getter< Meta_Relationship >( cmd_handler.p_Meta_Specification_Content_Page->Link_Page_Link_Child( ), cmd_handler.retval );
      else if( field_name == c_field_id_Meta_Content_Field || field_name == c_field_name_Meta_Content_Field )
         string_getter< Meta_Field >( cmd_handler.p_Meta_Specification_Content_Page->Meta_Content_Field( ), cmd_handler.retval );
      else if( field_name == c_field_id_Next_Child_Num_Field || field_name == c_field_name_Next_Child_Num_Field )
         string_getter< Meta_Field >( cmd_handler.p_Meta_Specification_Content_Page->Next_Child_Num_Field( ), cmd_handler.retval );
      else if( field_name == c_field_id_Order_Field || field_name == c_field_name_Order_Field )
         string_getter< Meta_Field >( cmd_handler.p_Meta_Specification_Content_Page->Order_Field( ), cmd_handler.retval );
      else if( field_name == c_field_id_Owned_Links_Field || field_name == c_field_name_Owned_Links_Field )
         string_getter< Meta_Field >( cmd_handler.p_Meta_Specification_Content_Page->Owned_Links_Field( ), cmd_handler.retval );
      else if( field_name == c_field_id_Page_Field || field_name == c_field_name_Page_Field )
         string_getter< Meta_Field >( cmd_handler.p_Meta_Specification_Content_Page->Page_Field( ), cmd_handler.retval );
      else if( field_name == c_field_id_Page_File_Class || field_name == c_field_name_Page_File_Class )
         string_getter< Meta_Class >( cmd_handler.p_Meta_Specification_Content_Page->Page_File_Class( ), cmd_handler.retval );
      else if( field_name == c_field_id_Page_File_Field || field_name == c_field_name_Page_File_Field )
         string_getter< Meta_Field >( cmd_handler.p_Meta_Specification_Content_Page->Page_File_Field( ), cmd_handler.retval );
      else if( field_name == c_field_id_Page_File_Name_Field || field_name == c_field_name_Page_File_Name_Field )
         string_getter< Meta_Field >( cmd_handler.p_Meta_Specification_Content_Page->Page_File_Name_Field( ), cmd_handler.retval );
      else if( field_name == c_field_id_Page_File_Name_Gen_Field || field_name == c_field_name_Page_File_Name_Gen_Field )
         string_getter< Meta_Field >( cmd_handler.p_Meta_Specification_Content_Page->Page_File_Name_Gen_Field( ), cmd_handler.retval );
      else if( field_name == c_field_id_Page_Link_Class || field_name == c_field_name_Page_Link_Class )
         string_getter< Meta_Class >( cmd_handler.p_Meta_Specification_Content_Page->Page_Link_Class( ), cmd_handler.retval );
      else if( field_name == c_field_id_Page_Page_Link_Child || field_name == c_field_name_Page_Page_Link_Child )
         string_getter< Meta_Relationship >( cmd_handler.p_Meta_Specification_Content_Page->Page_Page_Link_Child( ), cmd_handler.retval );
      else if( field_name == c_field_id_Parent_Page_Field || field_name == c_field_name_Parent_Page_Field )
         string_getter< Meta_Field >( cmd_handler.p_Meta_Specification_Content_Page->Parent_Page_Field( ), cmd_handler.retval );
      else if( field_name == c_field_id_Sub_Title_Field || field_name == c_field_name_Sub_Title_Field )
         string_getter< Meta_Field >( cmd_handler.p_Meta_Specification_Content_Page->Sub_Title_Field( ), cmd_handler.retval );
      else if( field_name == c_field_id_Text_Content_Field || field_name == c_field_name_Text_Content_Field )
         string_getter< Meta_Field >( cmd_handler.p_Meta_Specification_Content_Page->Text_Content_Field( ), cmd_handler.retval );
      else if( field_name == c_field_id_Title_Field || field_name == c_field_name_Title_Field )
         string_getter< Meta_Field >( cmd_handler.p_Meta_Specification_Content_Page->Title_Field( ), cmd_handler.retval );
      else if( field_name == c_field_id_Variable_Class || field_name == c_field_name_Variable_Class )
         string_getter< Meta_Class >( cmd_handler.p_Meta_Specification_Content_Page->Variable_Class( ), cmd_handler.retval );
      else if( field_name == c_field_id_Variable_Name || field_name == c_field_name_Variable_Name )
         string_getter< Meta_Field >( cmd_handler.p_Meta_Specification_Content_Page->Variable_Name( ), cmd_handler.retval );
      else if( field_name == c_field_id_Variable_Value || field_name == c_field_name_Variable_Value )
         string_getter< Meta_Field >( cmd_handler.p_Meta_Specification_Content_Page->Variable_Value( ), cmd_handler.retval );
      else
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Content_Page->Meta_Specification::execute( cmd_handler.cmd_and_args );
   }
   else if( command == c_cmd_Meta_Specification_Content_Page_set )
   {
      string field_name( get_parm_val( parameters, c_cmd_parm_Meta_Specification_Content_Page_set_field_name ) );
      string field_value( get_parm_val( parameters, c_cmd_parm_Meta_Specification_Content_Page_set_field_value ) );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for setter call" );
      else if( field_name == c_field_id_Actions_Field || field_name == c_field_name_Actions_Field )
         func_string_setter< Meta_Specification_Content_Page, Meta_Field >(
          *cmd_handler.p_Meta_Specification_Content_Page, &Meta_Specification_Content_Page::Actions_Field, field_value );
      else if( field_name == c_field_id_Child_Self_Relationship || field_name == c_field_name_Child_Self_Relationship )
         func_string_setter< Meta_Specification_Content_Page, Meta_Relationship >(
          *cmd_handler.p_Meta_Specification_Content_Page, &Meta_Specification_Content_Page::Child_Self_Relationship, field_value );
      else if( field_name == c_field_id_Clone_Permitted_Field || field_name == c_field_name_Clone_Permitted_Field )
         func_string_setter< Meta_Specification_Content_Page, Meta_Field >(
          *cmd_handler.p_Meta_Specification_Content_Page, &Meta_Specification_Content_Page::Clone_Permitted_Field, field_value );
      else if( field_name == c_field_id_Code_Field || field_name == c_field_name_Code_Field )
         func_string_setter< Meta_Specification_Content_Page, Meta_Field >(
          *cmd_handler.p_Meta_Specification_Content_Page, &Meta_Specification_Content_Page::Code_Field, field_value );
      else if( field_name == c_field_id_Content_Type || field_name == c_field_name_Content_Type )
         func_string_setter< Meta_Specification_Content_Page, Meta_Field >(
          *cmd_handler.p_Meta_Specification_Content_Page, &Meta_Specification_Content_Page::Content_Type, field_value );
      else if( field_name == c_field_id_Create_Copy_Output_Arg || field_name == c_field_name_Create_Copy_Output_Arg )
         func_string_setter< Meta_Specification_Content_Page, Meta_Procedure_Arg >(
          *cmd_handler.p_Meta_Specification_Content_Page, &Meta_Specification_Content_Page::Create_Copy_Output_Arg, field_value );
      else if( field_name == c_field_id_Create_Copy_Procedure || field_name == c_field_name_Create_Copy_Procedure )
         func_string_setter< Meta_Specification_Content_Page, Meta_Procedure >(
          *cmd_handler.p_Meta_Specification_Content_Page, &Meta_Specification_Content_Page::Create_Copy_Procedure, field_value );
      else if( field_name == c_field_id_File_Name_Field || field_name == c_field_name_File_Name_Field )
         func_string_setter< Meta_Specification_Content_Page, Meta_Field >(
          *cmd_handler.p_Meta_Specification_Content_Page, &Meta_Specification_Content_Page::File_Name_Field, field_value );
      else if( field_name == c_field_id_File_Name_Gen_Field || field_name == c_field_name_File_Name_Gen_Field )
         func_string_setter< Meta_Specification_Content_Page, Meta_Field >(
          *cmd_handler.p_Meta_Specification_Content_Page, &Meta_Specification_Content_Page::File_Name_Gen_Field, field_value );
      else if( field_name == c_field_id_Frag_Code_Field || field_name == c_field_name_Frag_Code_Field )
         func_string_setter< Meta_Specification_Content_Page, Meta_Field >(
          *cmd_handler.p_Meta_Specification_Content_Page, &Meta_Specification_Content_Page::Frag_Code_Field, field_value );
      else if( field_name == c_field_id_Frag_Content_Field || field_name == c_field_name_Frag_Content_Field )
         func_string_setter< Meta_Specification_Content_Page, Meta_Field >(
          *cmd_handler.p_Meta_Specification_Content_Page, &Meta_Specification_Content_Page::Frag_Content_Field, field_value );
      else if( field_name == c_field_id_Frag_Group_Field || field_name == c_field_name_Frag_Group_Field )
         func_string_setter< Meta_Specification_Content_Page, Meta_Field >(
          *cmd_handler.p_Meta_Specification_Content_Page, &Meta_Specification_Content_Page::Frag_Group_Field, field_value );
      else if( field_name == c_field_id_Fragment_Class || field_name == c_field_name_Fragment_Class )
         func_string_setter< Meta_Specification_Content_Page, Meta_Class >(
          *cmd_handler.p_Meta_Specification_Content_Page, &Meta_Specification_Content_Page::Fragment_Class, field_value );
      else if( field_name == c_field_id_Generate_Children_Field || field_name == c_field_name_Generate_Children_Field )
         func_string_setter< Meta_Specification_Content_Page, Meta_Field >(
          *cmd_handler.p_Meta_Specification_Content_Page, &Meta_Specification_Content_Page::Generate_Children_Field, field_value );
      else if( field_name == c_field_id_Generate_Output_Arg || field_name == c_field_name_Generate_Output_Arg )
         func_string_setter< Meta_Specification_Content_Page, Meta_Procedure_Arg >(
          *cmd_handler.p_Meta_Specification_Content_Page, &Meta_Specification_Content_Page::Generate_Output_Arg, field_value );
      else if( field_name == c_field_id_Generate_Procedure || field_name == c_field_name_Generate_Procedure )
         func_string_setter< Meta_Specification_Content_Page, Meta_Procedure >(
          *cmd_handler.p_Meta_Specification_Content_Page, &Meta_Specification_Content_Page::Generate_Procedure, field_value );
      else if( field_name == c_field_id_Group_Base_Path_Field || field_name == c_field_name_Group_Base_Path_Field )
         func_string_setter< Meta_Specification_Content_Page, Meta_Field >(
          *cmd_handler.p_Meta_Specification_Content_Page, &Meta_Specification_Content_Page::Group_Base_Path_Field, field_value );
      else if( field_name == c_field_id_Group_Class || field_name == c_field_name_Group_Class )
         func_string_setter< Meta_Specification_Content_Page, Meta_Class >(
          *cmd_handler.p_Meta_Specification_Content_Page, &Meta_Specification_Content_Page::Group_Class, field_value );
      else if( field_name == c_field_id_Group_Extra_Field || field_name == c_field_name_Group_Extra_Field )
         func_string_setter< Meta_Specification_Content_Page, Meta_Field >(
          *cmd_handler.p_Meta_Specification_Content_Page, &Meta_Specification_Content_Page::Group_Extra_Field, field_value );
      else if( field_name == c_field_id_Group_Field || field_name == c_field_name_Group_Field )
         func_string_setter< Meta_Specification_Content_Page, Meta_Field >(
          *cmd_handler.p_Meta_Specification_Content_Page, &Meta_Specification_Content_Page::Group_Field, field_value );
      else if( field_name == c_field_id_Group_Name_Field || field_name == c_field_name_Group_Name_Field )
         func_string_setter< Meta_Specification_Content_Page, Meta_Field >(
          *cmd_handler.p_Meta_Specification_Content_Page, &Meta_Specification_Content_Page::Group_Name_Field, field_value );
      else if( field_name == c_field_id_Hyperlink_Title_Field || field_name == c_field_name_Hyperlink_Title_Field )
         func_string_setter< Meta_Specification_Content_Page, Meta_Field >(
          *cmd_handler.p_Meta_Specification_Content_Page, &Meta_Specification_Content_Page::Hyperlink_Title_Field, field_value );
      else if( field_name == c_field_id_Link_Class || field_name == c_field_name_Link_Class )
         func_string_setter< Meta_Specification_Content_Page, Meta_Class >(
          *cmd_handler.p_Meta_Specification_Content_Page, &Meta_Specification_Content_Page::Link_Class, field_value );
      else if( field_name == c_field_id_Link_Content_Field || field_name == c_field_name_Link_Content_Field )
         func_string_setter< Meta_Specification_Content_Page, Meta_Field >(
          *cmd_handler.p_Meta_Specification_Content_Page, &Meta_Specification_Content_Page::Link_Content_Field, field_value );
      else if( field_name == c_field_id_Link_Group_Field || field_name == c_field_name_Link_Group_Field )
         func_string_setter< Meta_Specification_Content_Page, Meta_Field >(
          *cmd_handler.p_Meta_Specification_Content_Page, &Meta_Specification_Content_Page::Link_Group_Field, field_value );
      else if( field_name == c_field_id_Link_Name_Field || field_name == c_field_name_Link_Name_Field )
         func_string_setter< Meta_Specification_Content_Page, Meta_Field >(
          *cmd_handler.p_Meta_Specification_Content_Page, &Meta_Specification_Content_Page::Link_Name_Field, field_value );
      else if( field_name == c_field_id_Link_Page_Link_Child || field_name == c_field_name_Link_Page_Link_Child )
         func_string_setter< Meta_Specification_Content_Page, Meta_Relationship >(
          *cmd_handler.p_Meta_Specification_Content_Page, &Meta_Specification_Content_Page::Link_Page_Link_Child, field_value );
      else if( field_name == c_field_id_Meta_Content_Field || field_name == c_field_name_Meta_Content_Field )
         func_string_setter< Meta_Specification_Content_Page, Meta_Field >(
          *cmd_handler.p_Meta_Specification_Content_Page, &Meta_Specification_Content_Page::Meta_Content_Field, field_value );
      else if( field_name == c_field_id_Next_Child_Num_Field || field_name == c_field_name_Next_Child_Num_Field )
         func_string_setter< Meta_Specification_Content_Page, Meta_Field >(
          *cmd_handler.p_Meta_Specification_Content_Page, &Meta_Specification_Content_Page::Next_Child_Num_Field, field_value );
      else if( field_name == c_field_id_Order_Field || field_name == c_field_name_Order_Field )
         func_string_setter< Meta_Specification_Content_Page, Meta_Field >(
          *cmd_handler.p_Meta_Specification_Content_Page, &Meta_Specification_Content_Page::Order_Field, field_value );
      else if( field_name == c_field_id_Owned_Links_Field || field_name == c_field_name_Owned_Links_Field )
         func_string_setter< Meta_Specification_Content_Page, Meta_Field >(
          *cmd_handler.p_Meta_Specification_Content_Page, &Meta_Specification_Content_Page::Owned_Links_Field, field_value );
      else if( field_name == c_field_id_Page_Field || field_name == c_field_name_Page_Field )
         func_string_setter< Meta_Specification_Content_Page, Meta_Field >(
          *cmd_handler.p_Meta_Specification_Content_Page, &Meta_Specification_Content_Page::Page_Field, field_value );
      else if( field_name == c_field_id_Page_File_Class || field_name == c_field_name_Page_File_Class )
         func_string_setter< Meta_Specification_Content_Page, Meta_Class >(
          *cmd_handler.p_Meta_Specification_Content_Page, &Meta_Specification_Content_Page::Page_File_Class, field_value );
      else if( field_name == c_field_id_Page_File_Field || field_name == c_field_name_Page_File_Field )
         func_string_setter< Meta_Specification_Content_Page, Meta_Field >(
          *cmd_handler.p_Meta_Specification_Content_Page, &Meta_Specification_Content_Page::Page_File_Field, field_value );
      else if( field_name == c_field_id_Page_File_Name_Field || field_name == c_field_name_Page_File_Name_Field )
         func_string_setter< Meta_Specification_Content_Page, Meta_Field >(
          *cmd_handler.p_Meta_Specification_Content_Page, &Meta_Specification_Content_Page::Page_File_Name_Field, field_value );
      else if( field_name == c_field_id_Page_File_Name_Gen_Field || field_name == c_field_name_Page_File_Name_Gen_Field )
         func_string_setter< Meta_Specification_Content_Page, Meta_Field >(
          *cmd_handler.p_Meta_Specification_Content_Page, &Meta_Specification_Content_Page::Page_File_Name_Gen_Field, field_value );
      else if( field_name == c_field_id_Page_Link_Class || field_name == c_field_name_Page_Link_Class )
         func_string_setter< Meta_Specification_Content_Page, Meta_Class >(
          *cmd_handler.p_Meta_Specification_Content_Page, &Meta_Specification_Content_Page::Page_Link_Class, field_value );
      else if( field_name == c_field_id_Page_Page_Link_Child || field_name == c_field_name_Page_Page_Link_Child )
         func_string_setter< Meta_Specification_Content_Page, Meta_Relationship >(
          *cmd_handler.p_Meta_Specification_Content_Page, &Meta_Specification_Content_Page::Page_Page_Link_Child, field_value );
      else if( field_name == c_field_id_Parent_Page_Field || field_name == c_field_name_Parent_Page_Field )
         func_string_setter< Meta_Specification_Content_Page, Meta_Field >(
          *cmd_handler.p_Meta_Specification_Content_Page, &Meta_Specification_Content_Page::Parent_Page_Field, field_value );
      else if( field_name == c_field_id_Sub_Title_Field || field_name == c_field_name_Sub_Title_Field )
         func_string_setter< Meta_Specification_Content_Page, Meta_Field >(
          *cmd_handler.p_Meta_Specification_Content_Page, &Meta_Specification_Content_Page::Sub_Title_Field, field_value );
      else if( field_name == c_field_id_Text_Content_Field || field_name == c_field_name_Text_Content_Field )
         func_string_setter< Meta_Specification_Content_Page, Meta_Field >(
          *cmd_handler.p_Meta_Specification_Content_Page, &Meta_Specification_Content_Page::Text_Content_Field, field_value );
      else if( field_name == c_field_id_Title_Field || field_name == c_field_name_Title_Field )
         func_string_setter< Meta_Specification_Content_Page, Meta_Field >(
          *cmd_handler.p_Meta_Specification_Content_Page, &Meta_Specification_Content_Page::Title_Field, field_value );
      else if( field_name == c_field_id_Variable_Class || field_name == c_field_name_Variable_Class )
         func_string_setter< Meta_Specification_Content_Page, Meta_Class >(
          *cmd_handler.p_Meta_Specification_Content_Page, &Meta_Specification_Content_Page::Variable_Class, field_value );
      else if( field_name == c_field_id_Variable_Name || field_name == c_field_name_Variable_Name )
         func_string_setter< Meta_Specification_Content_Page, Meta_Field >(
          *cmd_handler.p_Meta_Specification_Content_Page, &Meta_Specification_Content_Page::Variable_Name, field_value );
      else if( field_name == c_field_id_Variable_Value || field_name == c_field_name_Variable_Value )
         func_string_setter< Meta_Specification_Content_Page, Meta_Field >(
          *cmd_handler.p_Meta_Specification_Content_Page, &Meta_Specification_Content_Page::Variable_Value, field_value );
      else
         cmd_handler.p_Meta_Specification_Content_Page->Meta_Specification::execute( cmd_handler.cmd_and_args );

      cmd_handler.retval = c_okay;
   }
   else if( command == c_cmd_Meta_Specification_Content_Page_cmd )
   {
      string field_name( get_parm_val( parameters, c_cmd_parm_Meta_Specification_Content_Page_cmd_field_name ) );
      string cmd_and_args( get_parm_val( parameters, c_cmd_parm_Meta_Specification_Content_Page_cmd_cmd_and_args ) );

      cmd_handler.retval.erase( );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for command call" );
      else if( field_name == c_field_id_Actions_Field || field_name == c_field_name_Actions_Field )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Content_Page->Actions_Field( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Child_Self_Relationship || field_name == c_field_name_Child_Self_Relationship )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Content_Page->Child_Self_Relationship( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Clone_Permitted_Field || field_name == c_field_name_Clone_Permitted_Field )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Content_Page->Clone_Permitted_Field( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Code_Field || field_name == c_field_name_Code_Field )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Content_Page->Code_Field( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Content_Type || field_name == c_field_name_Content_Type )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Content_Page->Content_Type( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Create_Copy_Output_Arg || field_name == c_field_name_Create_Copy_Output_Arg )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Content_Page->Create_Copy_Output_Arg( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Create_Copy_Procedure || field_name == c_field_name_Create_Copy_Procedure )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Content_Page->Create_Copy_Procedure( ).execute( cmd_and_args );
      else if( field_name == c_field_id_File_Name_Field || field_name == c_field_name_File_Name_Field )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Content_Page->File_Name_Field( ).execute( cmd_and_args );
      else if( field_name == c_field_id_File_Name_Gen_Field || field_name == c_field_name_File_Name_Gen_Field )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Content_Page->File_Name_Gen_Field( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Frag_Code_Field || field_name == c_field_name_Frag_Code_Field )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Content_Page->Frag_Code_Field( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Frag_Content_Field || field_name == c_field_name_Frag_Content_Field )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Content_Page->Frag_Content_Field( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Frag_Group_Field || field_name == c_field_name_Frag_Group_Field )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Content_Page->Frag_Group_Field( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Fragment_Class || field_name == c_field_name_Fragment_Class )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Content_Page->Fragment_Class( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Generate_Children_Field || field_name == c_field_name_Generate_Children_Field )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Content_Page->Generate_Children_Field( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Generate_Output_Arg || field_name == c_field_name_Generate_Output_Arg )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Content_Page->Generate_Output_Arg( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Generate_Procedure || field_name == c_field_name_Generate_Procedure )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Content_Page->Generate_Procedure( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Group_Base_Path_Field || field_name == c_field_name_Group_Base_Path_Field )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Content_Page->Group_Base_Path_Field( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Group_Class || field_name == c_field_name_Group_Class )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Content_Page->Group_Class( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Group_Extra_Field || field_name == c_field_name_Group_Extra_Field )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Content_Page->Group_Extra_Field( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Group_Field || field_name == c_field_name_Group_Field )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Content_Page->Group_Field( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Group_Name_Field || field_name == c_field_name_Group_Name_Field )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Content_Page->Group_Name_Field( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Hyperlink_Title_Field || field_name == c_field_name_Hyperlink_Title_Field )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Content_Page->Hyperlink_Title_Field( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Link_Class || field_name == c_field_name_Link_Class )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Content_Page->Link_Class( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Link_Content_Field || field_name == c_field_name_Link_Content_Field )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Content_Page->Link_Content_Field( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Link_Group_Field || field_name == c_field_name_Link_Group_Field )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Content_Page->Link_Group_Field( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Link_Name_Field || field_name == c_field_name_Link_Name_Field )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Content_Page->Link_Name_Field( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Link_Page_Link_Child || field_name == c_field_name_Link_Page_Link_Child )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Content_Page->Link_Page_Link_Child( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Meta_Content_Field || field_name == c_field_name_Meta_Content_Field )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Content_Page->Meta_Content_Field( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Next_Child_Num_Field || field_name == c_field_name_Next_Child_Num_Field )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Content_Page->Next_Child_Num_Field( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Order_Field || field_name == c_field_name_Order_Field )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Content_Page->Order_Field( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Owned_Links_Field || field_name == c_field_name_Owned_Links_Field )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Content_Page->Owned_Links_Field( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Page_Field || field_name == c_field_name_Page_Field )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Content_Page->Page_Field( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Page_File_Class || field_name == c_field_name_Page_File_Class )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Content_Page->Page_File_Class( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Page_File_Field || field_name == c_field_name_Page_File_Field )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Content_Page->Page_File_Field( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Page_File_Name_Field || field_name == c_field_name_Page_File_Name_Field )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Content_Page->Page_File_Name_Field( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Page_File_Name_Gen_Field || field_name == c_field_name_Page_File_Name_Gen_Field )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Content_Page->Page_File_Name_Gen_Field( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Page_Link_Class || field_name == c_field_name_Page_Link_Class )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Content_Page->Page_Link_Class( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Page_Page_Link_Child || field_name == c_field_name_Page_Page_Link_Child )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Content_Page->Page_Page_Link_Child( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Parent_Page_Field || field_name == c_field_name_Parent_Page_Field )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Content_Page->Parent_Page_Field( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Sub_Title_Field || field_name == c_field_name_Sub_Title_Field )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Content_Page->Sub_Title_Field( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Text_Content_Field || field_name == c_field_name_Text_Content_Field )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Content_Page->Text_Content_Field( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Title_Field || field_name == c_field_name_Title_Field )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Content_Page->Title_Field( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Variable_Class || field_name == c_field_name_Variable_Class )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Content_Page->Variable_Class( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Variable_Name || field_name == c_field_name_Variable_Name )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Content_Page->Variable_Name( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Variable_Value || field_name == c_field_name_Variable_Value )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Content_Page->Variable_Value( ).execute( cmd_and_args );
      else
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Content_Page->Meta_Specification::execute( cmd_handler.cmd_and_args );
   }
}

struct Meta_Specification_Content_Page::impl : public Meta_Specification_Content_Page_command_handler
{
   impl( Meta_Specification_Content_Page& o )
    :
    cp_obj( &o ),
    total_child_relationships( 0 )
   {
      p_obj = &o;
      set_Meta_Specification_Content_Page( &o );

      add_commands( 0, Meta_Specification_Content_Page_command_functor_factory,
       ARRAY_PTR_AND_SIZE( Meta_Specification_Content_Page_command_definitions ) );
   }

   Meta_Specification_Content_Page& get_obj( ) const
   {
      return *cp_obj;
   }

   Meta_Field& impl_Actions_Field( )
   {
      if( !cp_Actions_Field )
      {
         cp_Actions_Field.init( );

         p_obj->setup_graph_parent( *cp_Actions_Field, c_field_id_Actions_Field, v_Actions_Field );
      }
      return *cp_Actions_Field;
   }

   const Meta_Field& impl_Actions_Field( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Actions_Field )
      {
         cp_Actions_Field.init( );

         p_obj->setup_graph_parent( *cp_Actions_Field, c_field_id_Actions_Field, v_Actions_Field );
      }
      return *cp_Actions_Field;
   }

   void impl_Actions_Field( const string& key )
   {
      class_base_accessor cba( impl_Actions_Field( ) );
      cba.set_key( key );
   }

   Meta_Relationship& impl_Child_Self_Relationship( )
   {
      if( !cp_Child_Self_Relationship )
      {
         cp_Child_Self_Relationship.init( );

         p_obj->setup_graph_parent( *cp_Child_Self_Relationship, c_field_id_Child_Self_Relationship, v_Child_Self_Relationship );
      }
      return *cp_Child_Self_Relationship;
   }

   const Meta_Relationship& impl_Child_Self_Relationship( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Child_Self_Relationship )
      {
         cp_Child_Self_Relationship.init( );

         p_obj->setup_graph_parent( *cp_Child_Self_Relationship, c_field_id_Child_Self_Relationship, v_Child_Self_Relationship );
      }
      return *cp_Child_Self_Relationship;
   }

   void impl_Child_Self_Relationship( const string& key )
   {
      class_base_accessor cba( impl_Child_Self_Relationship( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Clone_Permitted_Field( )
   {
      if( !cp_Clone_Permitted_Field )
      {
         cp_Clone_Permitted_Field.init( );

         p_obj->setup_graph_parent( *cp_Clone_Permitted_Field, c_field_id_Clone_Permitted_Field, v_Clone_Permitted_Field );
      }
      return *cp_Clone_Permitted_Field;
   }

   const Meta_Field& impl_Clone_Permitted_Field( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Clone_Permitted_Field )
      {
         cp_Clone_Permitted_Field.init( );

         p_obj->setup_graph_parent( *cp_Clone_Permitted_Field, c_field_id_Clone_Permitted_Field, v_Clone_Permitted_Field );
      }
      return *cp_Clone_Permitted_Field;
   }

   void impl_Clone_Permitted_Field( const string& key )
   {
      class_base_accessor cba( impl_Clone_Permitted_Field( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Code_Field( )
   {
      if( !cp_Code_Field )
      {
         cp_Code_Field.init( );

         p_obj->setup_graph_parent( *cp_Code_Field, c_field_id_Code_Field, v_Code_Field );
      }
      return *cp_Code_Field;
   }

   const Meta_Field& impl_Code_Field( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Code_Field )
      {
         cp_Code_Field.init( );

         p_obj->setup_graph_parent( *cp_Code_Field, c_field_id_Code_Field, v_Code_Field );
      }
      return *cp_Code_Field;
   }

   void impl_Code_Field( const string& key )
   {
      class_base_accessor cba( impl_Code_Field( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Content_Type( )
   {
      if( !cp_Content_Type )
      {
         cp_Content_Type.init( );

         p_obj->setup_graph_parent( *cp_Content_Type, c_field_id_Content_Type, v_Content_Type );
      }
      return *cp_Content_Type;
   }

   const Meta_Field& impl_Content_Type( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Content_Type )
      {
         cp_Content_Type.init( );

         p_obj->setup_graph_parent( *cp_Content_Type, c_field_id_Content_Type, v_Content_Type );
      }
      return *cp_Content_Type;
   }

   void impl_Content_Type( const string& key )
   {
      class_base_accessor cba( impl_Content_Type( ) );
      cba.set_key( key );
   }

   Meta_Procedure_Arg& impl_Create_Copy_Output_Arg( )
   {
      if( !cp_Create_Copy_Output_Arg )
      {
         cp_Create_Copy_Output_Arg.init( );

         p_obj->setup_graph_parent( *cp_Create_Copy_Output_Arg, c_field_id_Create_Copy_Output_Arg, v_Create_Copy_Output_Arg );
      }
      return *cp_Create_Copy_Output_Arg;
   }

   const Meta_Procedure_Arg& impl_Create_Copy_Output_Arg( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Create_Copy_Output_Arg )
      {
         cp_Create_Copy_Output_Arg.init( );

         p_obj->setup_graph_parent( *cp_Create_Copy_Output_Arg, c_field_id_Create_Copy_Output_Arg, v_Create_Copy_Output_Arg );
      }
      return *cp_Create_Copy_Output_Arg;
   }

   void impl_Create_Copy_Output_Arg( const string& key )
   {
      class_base_accessor cba( impl_Create_Copy_Output_Arg( ) );
      cba.set_key( key );
   }

   Meta_Procedure& impl_Create_Copy_Procedure( )
   {
      if( !cp_Create_Copy_Procedure )
      {
         cp_Create_Copy_Procedure.init( );

         p_obj->setup_graph_parent( *cp_Create_Copy_Procedure, c_field_id_Create_Copy_Procedure, v_Create_Copy_Procedure );
      }
      return *cp_Create_Copy_Procedure;
   }

   const Meta_Procedure& impl_Create_Copy_Procedure( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Create_Copy_Procedure )
      {
         cp_Create_Copy_Procedure.init( );

         p_obj->setup_graph_parent( *cp_Create_Copy_Procedure, c_field_id_Create_Copy_Procedure, v_Create_Copy_Procedure );
      }
      return *cp_Create_Copy_Procedure;
   }

   void impl_Create_Copy_Procedure( const string& key )
   {
      class_base_accessor cba( impl_Create_Copy_Procedure( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_File_Name_Field( )
   {
      if( !cp_File_Name_Field )
      {
         cp_File_Name_Field.init( );

         p_obj->setup_graph_parent( *cp_File_Name_Field, c_field_id_File_Name_Field, v_File_Name_Field );
      }
      return *cp_File_Name_Field;
   }

   const Meta_Field& impl_File_Name_Field( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_File_Name_Field )
      {
         cp_File_Name_Field.init( );

         p_obj->setup_graph_parent( *cp_File_Name_Field, c_field_id_File_Name_Field, v_File_Name_Field );
      }
      return *cp_File_Name_Field;
   }

   void impl_File_Name_Field( const string& key )
   {
      class_base_accessor cba( impl_File_Name_Field( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_File_Name_Gen_Field( )
   {
      if( !cp_File_Name_Gen_Field )
      {
         cp_File_Name_Gen_Field.init( );

         p_obj->setup_graph_parent( *cp_File_Name_Gen_Field, c_field_id_File_Name_Gen_Field, v_File_Name_Gen_Field );
      }
      return *cp_File_Name_Gen_Field;
   }

   const Meta_Field& impl_File_Name_Gen_Field( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_File_Name_Gen_Field )
      {
         cp_File_Name_Gen_Field.init( );

         p_obj->setup_graph_parent( *cp_File_Name_Gen_Field, c_field_id_File_Name_Gen_Field, v_File_Name_Gen_Field );
      }
      return *cp_File_Name_Gen_Field;
   }

   void impl_File_Name_Gen_Field( const string& key )
   {
      class_base_accessor cba( impl_File_Name_Gen_Field( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Frag_Code_Field( )
   {
      if( !cp_Frag_Code_Field )
      {
         cp_Frag_Code_Field.init( );

         p_obj->setup_graph_parent( *cp_Frag_Code_Field, c_field_id_Frag_Code_Field, v_Frag_Code_Field );
      }
      return *cp_Frag_Code_Field;
   }

   const Meta_Field& impl_Frag_Code_Field( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Frag_Code_Field )
      {
         cp_Frag_Code_Field.init( );

         p_obj->setup_graph_parent( *cp_Frag_Code_Field, c_field_id_Frag_Code_Field, v_Frag_Code_Field );
      }
      return *cp_Frag_Code_Field;
   }

   void impl_Frag_Code_Field( const string& key )
   {
      class_base_accessor cba( impl_Frag_Code_Field( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Frag_Content_Field( )
   {
      if( !cp_Frag_Content_Field )
      {
         cp_Frag_Content_Field.init( );

         p_obj->setup_graph_parent( *cp_Frag_Content_Field, c_field_id_Frag_Content_Field, v_Frag_Content_Field );
      }
      return *cp_Frag_Content_Field;
   }

   const Meta_Field& impl_Frag_Content_Field( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Frag_Content_Field )
      {
         cp_Frag_Content_Field.init( );

         p_obj->setup_graph_parent( *cp_Frag_Content_Field, c_field_id_Frag_Content_Field, v_Frag_Content_Field );
      }
      return *cp_Frag_Content_Field;
   }

   void impl_Frag_Content_Field( const string& key )
   {
      class_base_accessor cba( impl_Frag_Content_Field( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Frag_Group_Field( )
   {
      if( !cp_Frag_Group_Field )
      {
         cp_Frag_Group_Field.init( );

         p_obj->setup_graph_parent( *cp_Frag_Group_Field, c_field_id_Frag_Group_Field, v_Frag_Group_Field );
      }
      return *cp_Frag_Group_Field;
   }

   const Meta_Field& impl_Frag_Group_Field( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Frag_Group_Field )
      {
         cp_Frag_Group_Field.init( );

         p_obj->setup_graph_parent( *cp_Frag_Group_Field, c_field_id_Frag_Group_Field, v_Frag_Group_Field );
      }
      return *cp_Frag_Group_Field;
   }

   void impl_Frag_Group_Field( const string& key )
   {
      class_base_accessor cba( impl_Frag_Group_Field( ) );
      cba.set_key( key );
   }

   Meta_Class& impl_Fragment_Class( )
   {
      if( !cp_Fragment_Class )
      {
         cp_Fragment_Class.init( );

         p_obj->setup_graph_parent( *cp_Fragment_Class, c_field_id_Fragment_Class, v_Fragment_Class );
      }
      return *cp_Fragment_Class;
   }

   const Meta_Class& impl_Fragment_Class( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Fragment_Class )
      {
         cp_Fragment_Class.init( );

         p_obj->setup_graph_parent( *cp_Fragment_Class, c_field_id_Fragment_Class, v_Fragment_Class );
      }
      return *cp_Fragment_Class;
   }

   void impl_Fragment_Class( const string& key )
   {
      class_base_accessor cba( impl_Fragment_Class( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Generate_Children_Field( )
   {
      if( !cp_Generate_Children_Field )
      {
         cp_Generate_Children_Field.init( );

         p_obj->setup_graph_parent( *cp_Generate_Children_Field, c_field_id_Generate_Children_Field, v_Generate_Children_Field );
      }
      return *cp_Generate_Children_Field;
   }

   const Meta_Field& impl_Generate_Children_Field( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Generate_Children_Field )
      {
         cp_Generate_Children_Field.init( );

         p_obj->setup_graph_parent( *cp_Generate_Children_Field, c_field_id_Generate_Children_Field, v_Generate_Children_Field );
      }
      return *cp_Generate_Children_Field;
   }

   void impl_Generate_Children_Field( const string& key )
   {
      class_base_accessor cba( impl_Generate_Children_Field( ) );
      cba.set_key( key );
   }

   Meta_Procedure_Arg& impl_Generate_Output_Arg( )
   {
      if( !cp_Generate_Output_Arg )
      {
         cp_Generate_Output_Arg.init( );

         p_obj->setup_graph_parent( *cp_Generate_Output_Arg, c_field_id_Generate_Output_Arg, v_Generate_Output_Arg );
      }
      return *cp_Generate_Output_Arg;
   }

   const Meta_Procedure_Arg& impl_Generate_Output_Arg( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Generate_Output_Arg )
      {
         cp_Generate_Output_Arg.init( );

         p_obj->setup_graph_parent( *cp_Generate_Output_Arg, c_field_id_Generate_Output_Arg, v_Generate_Output_Arg );
      }
      return *cp_Generate_Output_Arg;
   }

   void impl_Generate_Output_Arg( const string& key )
   {
      class_base_accessor cba( impl_Generate_Output_Arg( ) );
      cba.set_key( key );
   }

   Meta_Procedure& impl_Generate_Procedure( )
   {
      if( !cp_Generate_Procedure )
      {
         cp_Generate_Procedure.init( );

         p_obj->setup_graph_parent( *cp_Generate_Procedure, c_field_id_Generate_Procedure, v_Generate_Procedure );
      }
      return *cp_Generate_Procedure;
   }

   const Meta_Procedure& impl_Generate_Procedure( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Generate_Procedure )
      {
         cp_Generate_Procedure.init( );

         p_obj->setup_graph_parent( *cp_Generate_Procedure, c_field_id_Generate_Procedure, v_Generate_Procedure );
      }
      return *cp_Generate_Procedure;
   }

   void impl_Generate_Procedure( const string& key )
   {
      class_base_accessor cba( impl_Generate_Procedure( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Group_Base_Path_Field( )
   {
      if( !cp_Group_Base_Path_Field )
      {
         cp_Group_Base_Path_Field.init( );

         p_obj->setup_graph_parent( *cp_Group_Base_Path_Field, c_field_id_Group_Base_Path_Field, v_Group_Base_Path_Field );
      }
      return *cp_Group_Base_Path_Field;
   }

   const Meta_Field& impl_Group_Base_Path_Field( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Group_Base_Path_Field )
      {
         cp_Group_Base_Path_Field.init( );

         p_obj->setup_graph_parent( *cp_Group_Base_Path_Field, c_field_id_Group_Base_Path_Field, v_Group_Base_Path_Field );
      }
      return *cp_Group_Base_Path_Field;
   }

   void impl_Group_Base_Path_Field( const string& key )
   {
      class_base_accessor cba( impl_Group_Base_Path_Field( ) );
      cba.set_key( key );
   }

   Meta_Class& impl_Group_Class( )
   {
      if( !cp_Group_Class )
      {
         cp_Group_Class.init( );

         p_obj->setup_graph_parent( *cp_Group_Class, c_field_id_Group_Class, v_Group_Class );
      }
      return *cp_Group_Class;
   }

   const Meta_Class& impl_Group_Class( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Group_Class )
      {
         cp_Group_Class.init( );

         p_obj->setup_graph_parent( *cp_Group_Class, c_field_id_Group_Class, v_Group_Class );
      }
      return *cp_Group_Class;
   }

   void impl_Group_Class( const string& key )
   {
      class_base_accessor cba( impl_Group_Class( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Group_Extra_Field( )
   {
      if( !cp_Group_Extra_Field )
      {
         cp_Group_Extra_Field.init( );

         p_obj->setup_graph_parent( *cp_Group_Extra_Field, c_field_id_Group_Extra_Field, v_Group_Extra_Field );
      }
      return *cp_Group_Extra_Field;
   }

   const Meta_Field& impl_Group_Extra_Field( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Group_Extra_Field )
      {
         cp_Group_Extra_Field.init( );

         p_obj->setup_graph_parent( *cp_Group_Extra_Field, c_field_id_Group_Extra_Field, v_Group_Extra_Field );
      }
      return *cp_Group_Extra_Field;
   }

   void impl_Group_Extra_Field( const string& key )
   {
      class_base_accessor cba( impl_Group_Extra_Field( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Group_Field( )
   {
      if( !cp_Group_Field )
      {
         cp_Group_Field.init( );

         p_obj->setup_graph_parent( *cp_Group_Field, c_field_id_Group_Field, v_Group_Field );
      }
      return *cp_Group_Field;
   }

   const Meta_Field& impl_Group_Field( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Group_Field )
      {
         cp_Group_Field.init( );

         p_obj->setup_graph_parent( *cp_Group_Field, c_field_id_Group_Field, v_Group_Field );
      }
      return *cp_Group_Field;
   }

   void impl_Group_Field( const string& key )
   {
      class_base_accessor cba( impl_Group_Field( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Group_Name_Field( )
   {
      if( !cp_Group_Name_Field )
      {
         cp_Group_Name_Field.init( );

         p_obj->setup_graph_parent( *cp_Group_Name_Field, c_field_id_Group_Name_Field, v_Group_Name_Field );
      }
      return *cp_Group_Name_Field;
   }

   const Meta_Field& impl_Group_Name_Field( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Group_Name_Field )
      {
         cp_Group_Name_Field.init( );

         p_obj->setup_graph_parent( *cp_Group_Name_Field, c_field_id_Group_Name_Field, v_Group_Name_Field );
      }
      return *cp_Group_Name_Field;
   }

   void impl_Group_Name_Field( const string& key )
   {
      class_base_accessor cba( impl_Group_Name_Field( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Hyperlink_Title_Field( )
   {
      if( !cp_Hyperlink_Title_Field )
      {
         cp_Hyperlink_Title_Field.init( );

         p_obj->setup_graph_parent( *cp_Hyperlink_Title_Field, c_field_id_Hyperlink_Title_Field, v_Hyperlink_Title_Field );
      }
      return *cp_Hyperlink_Title_Field;
   }

   const Meta_Field& impl_Hyperlink_Title_Field( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Hyperlink_Title_Field )
      {
         cp_Hyperlink_Title_Field.init( );

         p_obj->setup_graph_parent( *cp_Hyperlink_Title_Field, c_field_id_Hyperlink_Title_Field, v_Hyperlink_Title_Field );
      }
      return *cp_Hyperlink_Title_Field;
   }

   void impl_Hyperlink_Title_Field( const string& key )
   {
      class_base_accessor cba( impl_Hyperlink_Title_Field( ) );
      cba.set_key( key );
   }

   Meta_Class& impl_Link_Class( )
   {
      if( !cp_Link_Class )
      {
         cp_Link_Class.init( );

         p_obj->setup_graph_parent( *cp_Link_Class, c_field_id_Link_Class, v_Link_Class );
      }
      return *cp_Link_Class;
   }

   const Meta_Class& impl_Link_Class( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Link_Class )
      {
         cp_Link_Class.init( );

         p_obj->setup_graph_parent( *cp_Link_Class, c_field_id_Link_Class, v_Link_Class );
      }
      return *cp_Link_Class;
   }

   void impl_Link_Class( const string& key )
   {
      class_base_accessor cba( impl_Link_Class( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Link_Content_Field( )
   {
      if( !cp_Link_Content_Field )
      {
         cp_Link_Content_Field.init( );

         p_obj->setup_graph_parent( *cp_Link_Content_Field, c_field_id_Link_Content_Field, v_Link_Content_Field );
      }
      return *cp_Link_Content_Field;
   }

   const Meta_Field& impl_Link_Content_Field( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Link_Content_Field )
      {
         cp_Link_Content_Field.init( );

         p_obj->setup_graph_parent( *cp_Link_Content_Field, c_field_id_Link_Content_Field, v_Link_Content_Field );
      }
      return *cp_Link_Content_Field;
   }

   void impl_Link_Content_Field( const string& key )
   {
      class_base_accessor cba( impl_Link_Content_Field( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Link_Group_Field( )
   {
      if( !cp_Link_Group_Field )
      {
         cp_Link_Group_Field.init( );

         p_obj->setup_graph_parent( *cp_Link_Group_Field, c_field_id_Link_Group_Field, v_Link_Group_Field );
      }
      return *cp_Link_Group_Field;
   }

   const Meta_Field& impl_Link_Group_Field( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Link_Group_Field )
      {
         cp_Link_Group_Field.init( );

         p_obj->setup_graph_parent( *cp_Link_Group_Field, c_field_id_Link_Group_Field, v_Link_Group_Field );
      }
      return *cp_Link_Group_Field;
   }

   void impl_Link_Group_Field( const string& key )
   {
      class_base_accessor cba( impl_Link_Group_Field( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Link_Name_Field( )
   {
      if( !cp_Link_Name_Field )
      {
         cp_Link_Name_Field.init( );

         p_obj->setup_graph_parent( *cp_Link_Name_Field, c_field_id_Link_Name_Field, v_Link_Name_Field );
      }
      return *cp_Link_Name_Field;
   }

   const Meta_Field& impl_Link_Name_Field( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Link_Name_Field )
      {
         cp_Link_Name_Field.init( );

         p_obj->setup_graph_parent( *cp_Link_Name_Field, c_field_id_Link_Name_Field, v_Link_Name_Field );
      }
      return *cp_Link_Name_Field;
   }

   void impl_Link_Name_Field( const string& key )
   {
      class_base_accessor cba( impl_Link_Name_Field( ) );
      cba.set_key( key );
   }

   Meta_Relationship& impl_Link_Page_Link_Child( )
   {
      if( !cp_Link_Page_Link_Child )
      {
         cp_Link_Page_Link_Child.init( );

         p_obj->setup_graph_parent( *cp_Link_Page_Link_Child, c_field_id_Link_Page_Link_Child, v_Link_Page_Link_Child );
      }
      return *cp_Link_Page_Link_Child;
   }

   const Meta_Relationship& impl_Link_Page_Link_Child( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Link_Page_Link_Child )
      {
         cp_Link_Page_Link_Child.init( );

         p_obj->setup_graph_parent( *cp_Link_Page_Link_Child, c_field_id_Link_Page_Link_Child, v_Link_Page_Link_Child );
      }
      return *cp_Link_Page_Link_Child;
   }

   void impl_Link_Page_Link_Child( const string& key )
   {
      class_base_accessor cba( impl_Link_Page_Link_Child( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Meta_Content_Field( )
   {
      if( !cp_Meta_Content_Field )
      {
         cp_Meta_Content_Field.init( );

         p_obj->setup_graph_parent( *cp_Meta_Content_Field, c_field_id_Meta_Content_Field, v_Meta_Content_Field );
      }
      return *cp_Meta_Content_Field;
   }

   const Meta_Field& impl_Meta_Content_Field( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Meta_Content_Field )
      {
         cp_Meta_Content_Field.init( );

         p_obj->setup_graph_parent( *cp_Meta_Content_Field, c_field_id_Meta_Content_Field, v_Meta_Content_Field );
      }
      return *cp_Meta_Content_Field;
   }

   void impl_Meta_Content_Field( const string& key )
   {
      class_base_accessor cba( impl_Meta_Content_Field( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Next_Child_Num_Field( )
   {
      if( !cp_Next_Child_Num_Field )
      {
         cp_Next_Child_Num_Field.init( );

         p_obj->setup_graph_parent( *cp_Next_Child_Num_Field, c_field_id_Next_Child_Num_Field, v_Next_Child_Num_Field );
      }
      return *cp_Next_Child_Num_Field;
   }

   const Meta_Field& impl_Next_Child_Num_Field( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Next_Child_Num_Field )
      {
         cp_Next_Child_Num_Field.init( );

         p_obj->setup_graph_parent( *cp_Next_Child_Num_Field, c_field_id_Next_Child_Num_Field, v_Next_Child_Num_Field );
      }
      return *cp_Next_Child_Num_Field;
   }

   void impl_Next_Child_Num_Field( const string& key )
   {
      class_base_accessor cba( impl_Next_Child_Num_Field( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Order_Field( )
   {
      if( !cp_Order_Field )
      {
         cp_Order_Field.init( );

         p_obj->setup_graph_parent( *cp_Order_Field, c_field_id_Order_Field, v_Order_Field );
      }
      return *cp_Order_Field;
   }

   const Meta_Field& impl_Order_Field( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Order_Field )
      {
         cp_Order_Field.init( );

         p_obj->setup_graph_parent( *cp_Order_Field, c_field_id_Order_Field, v_Order_Field );
      }
      return *cp_Order_Field;
   }

   void impl_Order_Field( const string& key )
   {
      class_base_accessor cba( impl_Order_Field( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Owned_Links_Field( )
   {
      if( !cp_Owned_Links_Field )
      {
         cp_Owned_Links_Field.init( );

         p_obj->setup_graph_parent( *cp_Owned_Links_Field, c_field_id_Owned_Links_Field, v_Owned_Links_Field );
      }
      return *cp_Owned_Links_Field;
   }

   const Meta_Field& impl_Owned_Links_Field( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Owned_Links_Field )
      {
         cp_Owned_Links_Field.init( );

         p_obj->setup_graph_parent( *cp_Owned_Links_Field, c_field_id_Owned_Links_Field, v_Owned_Links_Field );
      }
      return *cp_Owned_Links_Field;
   }

   void impl_Owned_Links_Field( const string& key )
   {
      class_base_accessor cba( impl_Owned_Links_Field( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Page_Field( )
   {
      if( !cp_Page_Field )
      {
         cp_Page_Field.init( );

         p_obj->setup_graph_parent( *cp_Page_Field, c_field_id_Page_Field, v_Page_Field );
      }
      return *cp_Page_Field;
   }

   const Meta_Field& impl_Page_Field( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Page_Field )
      {
         cp_Page_Field.init( );

         p_obj->setup_graph_parent( *cp_Page_Field, c_field_id_Page_Field, v_Page_Field );
      }
      return *cp_Page_Field;
   }

   void impl_Page_Field( const string& key )
   {
      class_base_accessor cba( impl_Page_Field( ) );
      cba.set_key( key );
   }

   Meta_Class& impl_Page_File_Class( )
   {
      if( !cp_Page_File_Class )
      {
         cp_Page_File_Class.init( );

         p_obj->setup_graph_parent( *cp_Page_File_Class, c_field_id_Page_File_Class, v_Page_File_Class );
      }
      return *cp_Page_File_Class;
   }

   const Meta_Class& impl_Page_File_Class( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Page_File_Class )
      {
         cp_Page_File_Class.init( );

         p_obj->setup_graph_parent( *cp_Page_File_Class, c_field_id_Page_File_Class, v_Page_File_Class );
      }
      return *cp_Page_File_Class;
   }

   void impl_Page_File_Class( const string& key )
   {
      class_base_accessor cba( impl_Page_File_Class( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Page_File_Field( )
   {
      if( !cp_Page_File_Field )
      {
         cp_Page_File_Field.init( );

         p_obj->setup_graph_parent( *cp_Page_File_Field, c_field_id_Page_File_Field, v_Page_File_Field );
      }
      return *cp_Page_File_Field;
   }

   const Meta_Field& impl_Page_File_Field( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Page_File_Field )
      {
         cp_Page_File_Field.init( );

         p_obj->setup_graph_parent( *cp_Page_File_Field, c_field_id_Page_File_Field, v_Page_File_Field );
      }
      return *cp_Page_File_Field;
   }

   void impl_Page_File_Field( const string& key )
   {
      class_base_accessor cba( impl_Page_File_Field( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Page_File_Name_Field( )
   {
      if( !cp_Page_File_Name_Field )
      {
         cp_Page_File_Name_Field.init( );

         p_obj->setup_graph_parent( *cp_Page_File_Name_Field, c_field_id_Page_File_Name_Field, v_Page_File_Name_Field );
      }
      return *cp_Page_File_Name_Field;
   }

   const Meta_Field& impl_Page_File_Name_Field( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Page_File_Name_Field )
      {
         cp_Page_File_Name_Field.init( );

         p_obj->setup_graph_parent( *cp_Page_File_Name_Field, c_field_id_Page_File_Name_Field, v_Page_File_Name_Field );
      }
      return *cp_Page_File_Name_Field;
   }

   void impl_Page_File_Name_Field( const string& key )
   {
      class_base_accessor cba( impl_Page_File_Name_Field( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Page_File_Name_Gen_Field( )
   {
      if( !cp_Page_File_Name_Gen_Field )
      {
         cp_Page_File_Name_Gen_Field.init( );

         p_obj->setup_graph_parent( *cp_Page_File_Name_Gen_Field, c_field_id_Page_File_Name_Gen_Field, v_Page_File_Name_Gen_Field );
      }
      return *cp_Page_File_Name_Gen_Field;
   }

   const Meta_Field& impl_Page_File_Name_Gen_Field( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Page_File_Name_Gen_Field )
      {
         cp_Page_File_Name_Gen_Field.init( );

         p_obj->setup_graph_parent( *cp_Page_File_Name_Gen_Field, c_field_id_Page_File_Name_Gen_Field, v_Page_File_Name_Gen_Field );
      }
      return *cp_Page_File_Name_Gen_Field;
   }

   void impl_Page_File_Name_Gen_Field( const string& key )
   {
      class_base_accessor cba( impl_Page_File_Name_Gen_Field( ) );
      cba.set_key( key );
   }

   Meta_Class& impl_Page_Link_Class( )
   {
      if( !cp_Page_Link_Class )
      {
         cp_Page_Link_Class.init( );

         p_obj->setup_graph_parent( *cp_Page_Link_Class, c_field_id_Page_Link_Class, v_Page_Link_Class );
      }
      return *cp_Page_Link_Class;
   }

   const Meta_Class& impl_Page_Link_Class( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Page_Link_Class )
      {
         cp_Page_Link_Class.init( );

         p_obj->setup_graph_parent( *cp_Page_Link_Class, c_field_id_Page_Link_Class, v_Page_Link_Class );
      }
      return *cp_Page_Link_Class;
   }

   void impl_Page_Link_Class( const string& key )
   {
      class_base_accessor cba( impl_Page_Link_Class( ) );
      cba.set_key( key );
   }

   Meta_Relationship& impl_Page_Page_Link_Child( )
   {
      if( !cp_Page_Page_Link_Child )
      {
         cp_Page_Page_Link_Child.init( );

         p_obj->setup_graph_parent( *cp_Page_Page_Link_Child, c_field_id_Page_Page_Link_Child, v_Page_Page_Link_Child );
      }
      return *cp_Page_Page_Link_Child;
   }

   const Meta_Relationship& impl_Page_Page_Link_Child( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Page_Page_Link_Child )
      {
         cp_Page_Page_Link_Child.init( );

         p_obj->setup_graph_parent( *cp_Page_Page_Link_Child, c_field_id_Page_Page_Link_Child, v_Page_Page_Link_Child );
      }
      return *cp_Page_Page_Link_Child;
   }

   void impl_Page_Page_Link_Child( const string& key )
   {
      class_base_accessor cba( impl_Page_Page_Link_Child( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Parent_Page_Field( )
   {
      if( !cp_Parent_Page_Field )
      {
         cp_Parent_Page_Field.init( );

         p_obj->setup_graph_parent( *cp_Parent_Page_Field, c_field_id_Parent_Page_Field, v_Parent_Page_Field );
      }
      return *cp_Parent_Page_Field;
   }

   const Meta_Field& impl_Parent_Page_Field( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Parent_Page_Field )
      {
         cp_Parent_Page_Field.init( );

         p_obj->setup_graph_parent( *cp_Parent_Page_Field, c_field_id_Parent_Page_Field, v_Parent_Page_Field );
      }
      return *cp_Parent_Page_Field;
   }

   void impl_Parent_Page_Field( const string& key )
   {
      class_base_accessor cba( impl_Parent_Page_Field( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Sub_Title_Field( )
   {
      if( !cp_Sub_Title_Field )
      {
         cp_Sub_Title_Field.init( );

         p_obj->setup_graph_parent( *cp_Sub_Title_Field, c_field_id_Sub_Title_Field, v_Sub_Title_Field );
      }
      return *cp_Sub_Title_Field;
   }

   const Meta_Field& impl_Sub_Title_Field( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Sub_Title_Field )
      {
         cp_Sub_Title_Field.init( );

         p_obj->setup_graph_parent( *cp_Sub_Title_Field, c_field_id_Sub_Title_Field, v_Sub_Title_Field );
      }
      return *cp_Sub_Title_Field;
   }

   void impl_Sub_Title_Field( const string& key )
   {
      class_base_accessor cba( impl_Sub_Title_Field( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Text_Content_Field( )
   {
      if( !cp_Text_Content_Field )
      {
         cp_Text_Content_Field.init( );

         p_obj->setup_graph_parent( *cp_Text_Content_Field, c_field_id_Text_Content_Field, v_Text_Content_Field );
      }
      return *cp_Text_Content_Field;
   }

   const Meta_Field& impl_Text_Content_Field( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Text_Content_Field )
      {
         cp_Text_Content_Field.init( );

         p_obj->setup_graph_parent( *cp_Text_Content_Field, c_field_id_Text_Content_Field, v_Text_Content_Field );
      }
      return *cp_Text_Content_Field;
   }

   void impl_Text_Content_Field( const string& key )
   {
      class_base_accessor cba( impl_Text_Content_Field( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Title_Field( )
   {
      if( !cp_Title_Field )
      {
         cp_Title_Field.init( );

         p_obj->setup_graph_parent( *cp_Title_Field, c_field_id_Title_Field, v_Title_Field );
      }
      return *cp_Title_Field;
   }

   const Meta_Field& impl_Title_Field( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Title_Field )
      {
         cp_Title_Field.init( );

         p_obj->setup_graph_parent( *cp_Title_Field, c_field_id_Title_Field, v_Title_Field );
      }
      return *cp_Title_Field;
   }

   void impl_Title_Field( const string& key )
   {
      class_base_accessor cba( impl_Title_Field( ) );
      cba.set_key( key );
   }

   Meta_Class& impl_Variable_Class( )
   {
      if( !cp_Variable_Class )
      {
         cp_Variable_Class.init( );

         p_obj->setup_graph_parent( *cp_Variable_Class, c_field_id_Variable_Class, v_Variable_Class );
      }
      return *cp_Variable_Class;
   }

   const Meta_Class& impl_Variable_Class( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Variable_Class )
      {
         cp_Variable_Class.init( );

         p_obj->setup_graph_parent( *cp_Variable_Class, c_field_id_Variable_Class, v_Variable_Class );
      }
      return *cp_Variable_Class;
   }

   void impl_Variable_Class( const string& key )
   {
      class_base_accessor cba( impl_Variable_Class( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Variable_Name( )
   {
      if( !cp_Variable_Name )
      {
         cp_Variable_Name.init( );

         p_obj->setup_graph_parent( *cp_Variable_Name, c_field_id_Variable_Name, v_Variable_Name );
      }
      return *cp_Variable_Name;
   }

   const Meta_Field& impl_Variable_Name( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Variable_Name )
      {
         cp_Variable_Name.init( );

         p_obj->setup_graph_parent( *cp_Variable_Name, c_field_id_Variable_Name, v_Variable_Name );
      }
      return *cp_Variable_Name;
   }

   void impl_Variable_Name( const string& key )
   {
      class_base_accessor cba( impl_Variable_Name( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Variable_Value( )
   {
      if( !cp_Variable_Value )
      {
         cp_Variable_Value.init( );

         p_obj->setup_graph_parent( *cp_Variable_Value, c_field_id_Variable_Value, v_Variable_Value );
      }
      return *cp_Variable_Value;
   }

   const Meta_Field& impl_Variable_Value( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Variable_Value )
      {
         cp_Variable_Value.init( );

         p_obj->setup_graph_parent( *cp_Variable_Value, c_field_id_Variable_Value, v_Variable_Value );
      }
      return *cp_Variable_Value;
   }

   void impl_Variable_Value( const string& key )
   {
      class_base_accessor cba( impl_Variable_Value( ) );
      cba.set_key( key );
   }

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

   bool is_filtered( const set< string >& filters ) const;

   Meta_Specification_Content_Page* p_obj;
   class_pointer< Meta_Specification_Content_Page > cp_obj;

   // [<start members>]
   // [<finish members>]

   size_t total_child_relationships;

   string v_Actions_Field;
   mutable class_pointer< Meta_Field > cp_Actions_Field;

   string v_Child_Self_Relationship;
   mutable class_pointer< Meta_Relationship > cp_Child_Self_Relationship;

   string v_Clone_Permitted_Field;
   mutable class_pointer< Meta_Field > cp_Clone_Permitted_Field;

   string v_Code_Field;
   mutable class_pointer< Meta_Field > cp_Code_Field;

   string v_Content_Type;
   mutable class_pointer< Meta_Field > cp_Content_Type;

   string v_Create_Copy_Output_Arg;
   mutable class_pointer< Meta_Procedure_Arg > cp_Create_Copy_Output_Arg;

   string v_Create_Copy_Procedure;
   mutable class_pointer< Meta_Procedure > cp_Create_Copy_Procedure;

   string v_File_Name_Field;
   mutable class_pointer< Meta_Field > cp_File_Name_Field;

   string v_File_Name_Gen_Field;
   mutable class_pointer< Meta_Field > cp_File_Name_Gen_Field;

   string v_Frag_Code_Field;
   mutable class_pointer< Meta_Field > cp_Frag_Code_Field;

   string v_Frag_Content_Field;
   mutable class_pointer< Meta_Field > cp_Frag_Content_Field;

   string v_Frag_Group_Field;
   mutable class_pointer< Meta_Field > cp_Frag_Group_Field;

   string v_Fragment_Class;
   mutable class_pointer< Meta_Class > cp_Fragment_Class;

   string v_Generate_Children_Field;
   mutable class_pointer< Meta_Field > cp_Generate_Children_Field;

   string v_Generate_Output_Arg;
   mutable class_pointer< Meta_Procedure_Arg > cp_Generate_Output_Arg;

   string v_Generate_Procedure;
   mutable class_pointer< Meta_Procedure > cp_Generate_Procedure;

   string v_Group_Base_Path_Field;
   mutable class_pointer< Meta_Field > cp_Group_Base_Path_Field;

   string v_Group_Class;
   mutable class_pointer< Meta_Class > cp_Group_Class;

   string v_Group_Extra_Field;
   mutable class_pointer< Meta_Field > cp_Group_Extra_Field;

   string v_Group_Field;
   mutable class_pointer< Meta_Field > cp_Group_Field;

   string v_Group_Name_Field;
   mutable class_pointer< Meta_Field > cp_Group_Name_Field;

   string v_Hyperlink_Title_Field;
   mutable class_pointer< Meta_Field > cp_Hyperlink_Title_Field;

   string v_Link_Class;
   mutable class_pointer< Meta_Class > cp_Link_Class;

   string v_Link_Content_Field;
   mutable class_pointer< Meta_Field > cp_Link_Content_Field;

   string v_Link_Group_Field;
   mutable class_pointer< Meta_Field > cp_Link_Group_Field;

   string v_Link_Name_Field;
   mutable class_pointer< Meta_Field > cp_Link_Name_Field;

   string v_Link_Page_Link_Child;
   mutable class_pointer< Meta_Relationship > cp_Link_Page_Link_Child;

   string v_Meta_Content_Field;
   mutable class_pointer< Meta_Field > cp_Meta_Content_Field;

   string v_Next_Child_Num_Field;
   mutable class_pointer< Meta_Field > cp_Next_Child_Num_Field;

   string v_Order_Field;
   mutable class_pointer< Meta_Field > cp_Order_Field;

   string v_Owned_Links_Field;
   mutable class_pointer< Meta_Field > cp_Owned_Links_Field;

   string v_Page_Field;
   mutable class_pointer< Meta_Field > cp_Page_Field;

   string v_Page_File_Class;
   mutable class_pointer< Meta_Class > cp_Page_File_Class;

   string v_Page_File_Field;
   mutable class_pointer< Meta_Field > cp_Page_File_Field;

   string v_Page_File_Name_Field;
   mutable class_pointer< Meta_Field > cp_Page_File_Name_Field;

   string v_Page_File_Name_Gen_Field;
   mutable class_pointer< Meta_Field > cp_Page_File_Name_Gen_Field;

   string v_Page_Link_Class;
   mutable class_pointer< Meta_Class > cp_Page_Link_Class;

   string v_Page_Page_Link_Child;
   mutable class_pointer< Meta_Relationship > cp_Page_Page_Link_Child;

   string v_Parent_Page_Field;
   mutable class_pointer< Meta_Field > cp_Parent_Page_Field;

   string v_Sub_Title_Field;
   mutable class_pointer< Meta_Field > cp_Sub_Title_Field;

   string v_Text_Content_Field;
   mutable class_pointer< Meta_Field > cp_Text_Content_Field;

   string v_Title_Field;
   mutable class_pointer< Meta_Field > cp_Title_Field;

   string v_Variable_Class;
   mutable class_pointer< Meta_Class > cp_Variable_Class;

   string v_Variable_Name;
   mutable class_pointer< Meta_Field > cp_Variable_Name;

   string v_Variable_Value;
   mutable class_pointer< Meta_Field > cp_Variable_Value;
};

string Meta_Specification_Content_Page::impl::get_field_value( int field ) const
{
   string retval;

   switch( field )
   {
      case 0:
      retval = to_string( impl_Actions_Field( ) );
      break;

      case 1:
      retval = to_string( impl_Child_Self_Relationship( ) );
      break;

      case 2:
      retval = to_string( impl_Clone_Permitted_Field( ) );
      break;

      case 3:
      retval = to_string( impl_Code_Field( ) );
      break;

      case 4:
      retval = to_string( impl_Content_Type( ) );
      break;

      case 5:
      retval = to_string( impl_Create_Copy_Output_Arg( ) );
      break;

      case 6:
      retval = to_string( impl_Create_Copy_Procedure( ) );
      break;

      case 7:
      retval = to_string( impl_File_Name_Field( ) );
      break;

      case 8:
      retval = to_string( impl_File_Name_Gen_Field( ) );
      break;

      case 9:
      retval = to_string( impl_Frag_Code_Field( ) );
      break;

      case 10:
      retval = to_string( impl_Frag_Content_Field( ) );
      break;

      case 11:
      retval = to_string( impl_Frag_Group_Field( ) );
      break;

      case 12:
      retval = to_string( impl_Fragment_Class( ) );
      break;

      case 13:
      retval = to_string( impl_Generate_Children_Field( ) );
      break;

      case 14:
      retval = to_string( impl_Generate_Output_Arg( ) );
      break;

      case 15:
      retval = to_string( impl_Generate_Procedure( ) );
      break;

      case 16:
      retval = to_string( impl_Group_Base_Path_Field( ) );
      break;

      case 17:
      retval = to_string( impl_Group_Class( ) );
      break;

      case 18:
      retval = to_string( impl_Group_Extra_Field( ) );
      break;

      case 19:
      retval = to_string( impl_Group_Field( ) );
      break;

      case 20:
      retval = to_string( impl_Group_Name_Field( ) );
      break;

      case 21:
      retval = to_string( impl_Hyperlink_Title_Field( ) );
      break;

      case 22:
      retval = to_string( impl_Link_Class( ) );
      break;

      case 23:
      retval = to_string( impl_Link_Content_Field( ) );
      break;

      case 24:
      retval = to_string( impl_Link_Group_Field( ) );
      break;

      case 25:
      retval = to_string( impl_Link_Name_Field( ) );
      break;

      case 26:
      retval = to_string( impl_Link_Page_Link_Child( ) );
      break;

      case 27:
      retval = to_string( impl_Meta_Content_Field( ) );
      break;

      case 28:
      retval = to_string( impl_Next_Child_Num_Field( ) );
      break;

      case 29:
      retval = to_string( impl_Order_Field( ) );
      break;

      case 30:
      retval = to_string( impl_Owned_Links_Field( ) );
      break;

      case 31:
      retval = to_string( impl_Page_Field( ) );
      break;

      case 32:
      retval = to_string( impl_Page_File_Class( ) );
      break;

      case 33:
      retval = to_string( impl_Page_File_Field( ) );
      break;

      case 34:
      retval = to_string( impl_Page_File_Name_Field( ) );
      break;

      case 35:
      retval = to_string( impl_Page_File_Name_Gen_Field( ) );
      break;

      case 36:
      retval = to_string( impl_Page_Link_Class( ) );
      break;

      case 37:
      retval = to_string( impl_Page_Page_Link_Child( ) );
      break;

      case 38:
      retval = to_string( impl_Parent_Page_Field( ) );
      break;

      case 39:
      retval = to_string( impl_Sub_Title_Field( ) );
      break;

      case 40:
      retval = to_string( impl_Text_Content_Field( ) );
      break;

      case 41:
      retval = to_string( impl_Title_Field( ) );
      break;

      case 42:
      retval = to_string( impl_Variable_Class( ) );
      break;

      case 43:
      retval = to_string( impl_Variable_Name( ) );
      break;

      case 44:
      retval = to_string( impl_Variable_Value( ) );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range" );
   }

   return retval;
}

void Meta_Specification_Content_Page::impl::set_field_value( int field, const string& value )
{
   switch( field )
   {
      case 0:
      func_string_setter< Meta_Specification_Content_Page::impl, Meta_Field >( *this, &Meta_Specification_Content_Page::impl::impl_Actions_Field, value );
      break;

      case 1:
      func_string_setter< Meta_Specification_Content_Page::impl, Meta_Relationship >( *this, &Meta_Specification_Content_Page::impl::impl_Child_Self_Relationship, value );
      break;

      case 2:
      func_string_setter< Meta_Specification_Content_Page::impl, Meta_Field >( *this, &Meta_Specification_Content_Page::impl::impl_Clone_Permitted_Field, value );
      break;

      case 3:
      func_string_setter< Meta_Specification_Content_Page::impl, Meta_Field >( *this, &Meta_Specification_Content_Page::impl::impl_Code_Field, value );
      break;

      case 4:
      func_string_setter< Meta_Specification_Content_Page::impl, Meta_Field >( *this, &Meta_Specification_Content_Page::impl::impl_Content_Type, value );
      break;

      case 5:
      func_string_setter< Meta_Specification_Content_Page::impl, Meta_Procedure_Arg >( *this, &Meta_Specification_Content_Page::impl::impl_Create_Copy_Output_Arg, value );
      break;

      case 6:
      func_string_setter< Meta_Specification_Content_Page::impl, Meta_Procedure >( *this, &Meta_Specification_Content_Page::impl::impl_Create_Copy_Procedure, value );
      break;

      case 7:
      func_string_setter< Meta_Specification_Content_Page::impl, Meta_Field >( *this, &Meta_Specification_Content_Page::impl::impl_File_Name_Field, value );
      break;

      case 8:
      func_string_setter< Meta_Specification_Content_Page::impl, Meta_Field >( *this, &Meta_Specification_Content_Page::impl::impl_File_Name_Gen_Field, value );
      break;

      case 9:
      func_string_setter< Meta_Specification_Content_Page::impl, Meta_Field >( *this, &Meta_Specification_Content_Page::impl::impl_Frag_Code_Field, value );
      break;

      case 10:
      func_string_setter< Meta_Specification_Content_Page::impl, Meta_Field >( *this, &Meta_Specification_Content_Page::impl::impl_Frag_Content_Field, value );
      break;

      case 11:
      func_string_setter< Meta_Specification_Content_Page::impl, Meta_Field >( *this, &Meta_Specification_Content_Page::impl::impl_Frag_Group_Field, value );
      break;

      case 12:
      func_string_setter< Meta_Specification_Content_Page::impl, Meta_Class >( *this, &Meta_Specification_Content_Page::impl::impl_Fragment_Class, value );
      break;

      case 13:
      func_string_setter< Meta_Specification_Content_Page::impl, Meta_Field >( *this, &Meta_Specification_Content_Page::impl::impl_Generate_Children_Field, value );
      break;

      case 14:
      func_string_setter< Meta_Specification_Content_Page::impl, Meta_Procedure_Arg >( *this, &Meta_Specification_Content_Page::impl::impl_Generate_Output_Arg, value );
      break;

      case 15:
      func_string_setter< Meta_Specification_Content_Page::impl, Meta_Procedure >( *this, &Meta_Specification_Content_Page::impl::impl_Generate_Procedure, value );
      break;

      case 16:
      func_string_setter< Meta_Specification_Content_Page::impl, Meta_Field >( *this, &Meta_Specification_Content_Page::impl::impl_Group_Base_Path_Field, value );
      break;

      case 17:
      func_string_setter< Meta_Specification_Content_Page::impl, Meta_Class >( *this, &Meta_Specification_Content_Page::impl::impl_Group_Class, value );
      break;

      case 18:
      func_string_setter< Meta_Specification_Content_Page::impl, Meta_Field >( *this, &Meta_Specification_Content_Page::impl::impl_Group_Extra_Field, value );
      break;

      case 19:
      func_string_setter< Meta_Specification_Content_Page::impl, Meta_Field >( *this, &Meta_Specification_Content_Page::impl::impl_Group_Field, value );
      break;

      case 20:
      func_string_setter< Meta_Specification_Content_Page::impl, Meta_Field >( *this, &Meta_Specification_Content_Page::impl::impl_Group_Name_Field, value );
      break;

      case 21:
      func_string_setter< Meta_Specification_Content_Page::impl, Meta_Field >( *this, &Meta_Specification_Content_Page::impl::impl_Hyperlink_Title_Field, value );
      break;

      case 22:
      func_string_setter< Meta_Specification_Content_Page::impl, Meta_Class >( *this, &Meta_Specification_Content_Page::impl::impl_Link_Class, value );
      break;

      case 23:
      func_string_setter< Meta_Specification_Content_Page::impl, Meta_Field >( *this, &Meta_Specification_Content_Page::impl::impl_Link_Content_Field, value );
      break;

      case 24:
      func_string_setter< Meta_Specification_Content_Page::impl, Meta_Field >( *this, &Meta_Specification_Content_Page::impl::impl_Link_Group_Field, value );
      break;

      case 25:
      func_string_setter< Meta_Specification_Content_Page::impl, Meta_Field >( *this, &Meta_Specification_Content_Page::impl::impl_Link_Name_Field, value );
      break;

      case 26:
      func_string_setter< Meta_Specification_Content_Page::impl, Meta_Relationship >( *this, &Meta_Specification_Content_Page::impl::impl_Link_Page_Link_Child, value );
      break;

      case 27:
      func_string_setter< Meta_Specification_Content_Page::impl, Meta_Field >( *this, &Meta_Specification_Content_Page::impl::impl_Meta_Content_Field, value );
      break;

      case 28:
      func_string_setter< Meta_Specification_Content_Page::impl, Meta_Field >( *this, &Meta_Specification_Content_Page::impl::impl_Next_Child_Num_Field, value );
      break;

      case 29:
      func_string_setter< Meta_Specification_Content_Page::impl, Meta_Field >( *this, &Meta_Specification_Content_Page::impl::impl_Order_Field, value );
      break;

      case 30:
      func_string_setter< Meta_Specification_Content_Page::impl, Meta_Field >( *this, &Meta_Specification_Content_Page::impl::impl_Owned_Links_Field, value );
      break;

      case 31:
      func_string_setter< Meta_Specification_Content_Page::impl, Meta_Field >( *this, &Meta_Specification_Content_Page::impl::impl_Page_Field, value );
      break;

      case 32:
      func_string_setter< Meta_Specification_Content_Page::impl, Meta_Class >( *this, &Meta_Specification_Content_Page::impl::impl_Page_File_Class, value );
      break;

      case 33:
      func_string_setter< Meta_Specification_Content_Page::impl, Meta_Field >( *this, &Meta_Specification_Content_Page::impl::impl_Page_File_Field, value );
      break;

      case 34:
      func_string_setter< Meta_Specification_Content_Page::impl, Meta_Field >( *this, &Meta_Specification_Content_Page::impl::impl_Page_File_Name_Field, value );
      break;

      case 35:
      func_string_setter< Meta_Specification_Content_Page::impl, Meta_Field >( *this, &Meta_Specification_Content_Page::impl::impl_Page_File_Name_Gen_Field, value );
      break;

      case 36:
      func_string_setter< Meta_Specification_Content_Page::impl, Meta_Class >( *this, &Meta_Specification_Content_Page::impl::impl_Page_Link_Class, value );
      break;

      case 37:
      func_string_setter< Meta_Specification_Content_Page::impl, Meta_Relationship >( *this, &Meta_Specification_Content_Page::impl::impl_Page_Page_Link_Child, value );
      break;

      case 38:
      func_string_setter< Meta_Specification_Content_Page::impl, Meta_Field >( *this, &Meta_Specification_Content_Page::impl::impl_Parent_Page_Field, value );
      break;

      case 39:
      func_string_setter< Meta_Specification_Content_Page::impl, Meta_Field >( *this, &Meta_Specification_Content_Page::impl::impl_Sub_Title_Field, value );
      break;

      case 40:
      func_string_setter< Meta_Specification_Content_Page::impl, Meta_Field >( *this, &Meta_Specification_Content_Page::impl::impl_Text_Content_Field, value );
      break;

      case 41:
      func_string_setter< Meta_Specification_Content_Page::impl, Meta_Field >( *this, &Meta_Specification_Content_Page::impl::impl_Title_Field, value );
      break;

      case 42:
      func_string_setter< Meta_Specification_Content_Page::impl, Meta_Class >( *this, &Meta_Specification_Content_Page::impl::impl_Variable_Class, value );
      break;

      case 43:
      func_string_setter< Meta_Specification_Content_Page::impl, Meta_Field >( *this, &Meta_Specification_Content_Page::impl::impl_Variable_Name, value );
      break;

      case 44:
      func_string_setter< Meta_Specification_Content_Page::impl, Meta_Field >( *this, &Meta_Specification_Content_Page::impl::impl_Variable_Value, value );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range" );
   }
}

uint64_t Meta_Specification_Content_Page::impl::get_state( ) const
{
   uint64_t state = 0;

   // [<start get_state>]
   // [<finish get_state>]

   return state;
}

const string& Meta_Specification_Content_Page::impl::execute( const string& cmd_and_args )
{
   this->cmd_and_args = cmd_and_args;
   execute_command( cmd_and_args );
   return retval;
}

void Meta_Specification_Content_Page::impl::clear_foreign_key( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( field == c_field_id_Actions_Field || field == c_field_name_Actions_Field )
      impl_Actions_Field( "" );
   else if( field == c_field_id_Child_Self_Relationship || field == c_field_name_Child_Self_Relationship )
      impl_Child_Self_Relationship( "" );
   else if( field == c_field_id_Clone_Permitted_Field || field == c_field_name_Clone_Permitted_Field )
      impl_Clone_Permitted_Field( "" );
   else if( field == c_field_id_Code_Field || field == c_field_name_Code_Field )
      impl_Code_Field( "" );
   else if( field == c_field_id_Content_Type || field == c_field_name_Content_Type )
      impl_Content_Type( "" );
   else if( field == c_field_id_Create_Copy_Output_Arg || field == c_field_name_Create_Copy_Output_Arg )
      impl_Create_Copy_Output_Arg( "" );
   else if( field == c_field_id_Create_Copy_Procedure || field == c_field_name_Create_Copy_Procedure )
      impl_Create_Copy_Procedure( "" );
   else if( field == c_field_id_File_Name_Field || field == c_field_name_File_Name_Field )
      impl_File_Name_Field( "" );
   else if( field == c_field_id_File_Name_Gen_Field || field == c_field_name_File_Name_Gen_Field )
      impl_File_Name_Gen_Field( "" );
   else if( field == c_field_id_Frag_Code_Field || field == c_field_name_Frag_Code_Field )
      impl_Frag_Code_Field( "" );
   else if( field == c_field_id_Frag_Content_Field || field == c_field_name_Frag_Content_Field )
      impl_Frag_Content_Field( "" );
   else if( field == c_field_id_Frag_Group_Field || field == c_field_name_Frag_Group_Field )
      impl_Frag_Group_Field( "" );
   else if( field == c_field_id_Fragment_Class || field == c_field_name_Fragment_Class )
      impl_Fragment_Class( "" );
   else if( field == c_field_id_Generate_Children_Field || field == c_field_name_Generate_Children_Field )
      impl_Generate_Children_Field( "" );
   else if( field == c_field_id_Generate_Output_Arg || field == c_field_name_Generate_Output_Arg )
      impl_Generate_Output_Arg( "" );
   else if( field == c_field_id_Generate_Procedure || field == c_field_name_Generate_Procedure )
      impl_Generate_Procedure( "" );
   else if( field == c_field_id_Group_Base_Path_Field || field == c_field_name_Group_Base_Path_Field )
      impl_Group_Base_Path_Field( "" );
   else if( field == c_field_id_Group_Class || field == c_field_name_Group_Class )
      impl_Group_Class( "" );
   else if( field == c_field_id_Group_Extra_Field || field == c_field_name_Group_Extra_Field )
      impl_Group_Extra_Field( "" );
   else if( field == c_field_id_Group_Field || field == c_field_name_Group_Field )
      impl_Group_Field( "" );
   else if( field == c_field_id_Group_Name_Field || field == c_field_name_Group_Name_Field )
      impl_Group_Name_Field( "" );
   else if( field == c_field_id_Hyperlink_Title_Field || field == c_field_name_Hyperlink_Title_Field )
      impl_Hyperlink_Title_Field( "" );
   else if( field == c_field_id_Link_Class || field == c_field_name_Link_Class )
      impl_Link_Class( "" );
   else if( field == c_field_id_Link_Content_Field || field == c_field_name_Link_Content_Field )
      impl_Link_Content_Field( "" );
   else if( field == c_field_id_Link_Group_Field || field == c_field_name_Link_Group_Field )
      impl_Link_Group_Field( "" );
   else if( field == c_field_id_Link_Name_Field || field == c_field_name_Link_Name_Field )
      impl_Link_Name_Field( "" );
   else if( field == c_field_id_Link_Page_Link_Child || field == c_field_name_Link_Page_Link_Child )
      impl_Link_Page_Link_Child( "" );
   else if( field == c_field_id_Meta_Content_Field || field == c_field_name_Meta_Content_Field )
      impl_Meta_Content_Field( "" );
   else if( field == c_field_id_Next_Child_Num_Field || field == c_field_name_Next_Child_Num_Field )
      impl_Next_Child_Num_Field( "" );
   else if( field == c_field_id_Order_Field || field == c_field_name_Order_Field )
      impl_Order_Field( "" );
   else if( field == c_field_id_Owned_Links_Field || field == c_field_name_Owned_Links_Field )
      impl_Owned_Links_Field( "" );
   else if( field == c_field_id_Page_Field || field == c_field_name_Page_Field )
      impl_Page_Field( "" );
   else if( field == c_field_id_Page_File_Class || field == c_field_name_Page_File_Class )
      impl_Page_File_Class( "" );
   else if( field == c_field_id_Page_File_Field || field == c_field_name_Page_File_Field )
      impl_Page_File_Field( "" );
   else if( field == c_field_id_Page_File_Name_Field || field == c_field_name_Page_File_Name_Field )
      impl_Page_File_Name_Field( "" );
   else if( field == c_field_id_Page_File_Name_Gen_Field || field == c_field_name_Page_File_Name_Gen_Field )
      impl_Page_File_Name_Gen_Field( "" );
   else if( field == c_field_id_Page_Link_Class || field == c_field_name_Page_Link_Class )
      impl_Page_Link_Class( "" );
   else if( field == c_field_id_Page_Page_Link_Child || field == c_field_name_Page_Page_Link_Child )
      impl_Page_Page_Link_Child( "" );
   else if( field == c_field_id_Parent_Page_Field || field == c_field_name_Parent_Page_Field )
      impl_Parent_Page_Field( "" );
   else if( field == c_field_id_Sub_Title_Field || field == c_field_name_Sub_Title_Field )
      impl_Sub_Title_Field( "" );
   else if( field == c_field_id_Text_Content_Field || field == c_field_name_Text_Content_Field )
      impl_Text_Content_Field( "" );
   else if( field == c_field_id_Title_Field || field == c_field_name_Title_Field )
      impl_Title_Field( "" );
   else if( field == c_field_id_Variable_Class || field == c_field_name_Variable_Class )
      impl_Variable_Class( "" );
   else if( field == c_field_id_Variable_Name || field == c_field_name_Variable_Name )
      impl_Variable_Name( "" );
   else if( field == c_field_id_Variable_Value || field == c_field_name_Variable_Value )
      impl_Variable_Value( "" );
   else
      p_obj->parent_class_type::clear_foreign_key( field );
}

void Meta_Specification_Content_Page::impl::set_foreign_key_value( const string& field, const string& value )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for value: " + value );
   else if( field == c_field_id_Actions_Field || field == c_field_name_Actions_Field )
      v_Actions_Field = value;
   else if( field == c_field_id_Child_Self_Relationship || field == c_field_name_Child_Self_Relationship )
      v_Child_Self_Relationship = value;
   else if( field == c_field_id_Clone_Permitted_Field || field == c_field_name_Clone_Permitted_Field )
      v_Clone_Permitted_Field = value;
   else if( field == c_field_id_Code_Field || field == c_field_name_Code_Field )
      v_Code_Field = value;
   else if( field == c_field_id_Content_Type || field == c_field_name_Content_Type )
      v_Content_Type = value;
   else if( field == c_field_id_Create_Copy_Output_Arg || field == c_field_name_Create_Copy_Output_Arg )
      v_Create_Copy_Output_Arg = value;
   else if( field == c_field_id_Create_Copy_Procedure || field == c_field_name_Create_Copy_Procedure )
      v_Create_Copy_Procedure = value;
   else if( field == c_field_id_File_Name_Field || field == c_field_name_File_Name_Field )
      v_File_Name_Field = value;
   else if( field == c_field_id_File_Name_Gen_Field || field == c_field_name_File_Name_Gen_Field )
      v_File_Name_Gen_Field = value;
   else if( field == c_field_id_Frag_Code_Field || field == c_field_name_Frag_Code_Field )
      v_Frag_Code_Field = value;
   else if( field == c_field_id_Frag_Content_Field || field == c_field_name_Frag_Content_Field )
      v_Frag_Content_Field = value;
   else if( field == c_field_id_Frag_Group_Field || field == c_field_name_Frag_Group_Field )
      v_Frag_Group_Field = value;
   else if( field == c_field_id_Fragment_Class || field == c_field_name_Fragment_Class )
      v_Fragment_Class = value;
   else if( field == c_field_id_Generate_Children_Field || field == c_field_name_Generate_Children_Field )
      v_Generate_Children_Field = value;
   else if( field == c_field_id_Generate_Output_Arg || field == c_field_name_Generate_Output_Arg )
      v_Generate_Output_Arg = value;
   else if( field == c_field_id_Generate_Procedure || field == c_field_name_Generate_Procedure )
      v_Generate_Procedure = value;
   else if( field == c_field_id_Group_Base_Path_Field || field == c_field_name_Group_Base_Path_Field )
      v_Group_Base_Path_Field = value;
   else if( field == c_field_id_Group_Class || field == c_field_name_Group_Class )
      v_Group_Class = value;
   else if( field == c_field_id_Group_Extra_Field || field == c_field_name_Group_Extra_Field )
      v_Group_Extra_Field = value;
   else if( field == c_field_id_Group_Field || field == c_field_name_Group_Field )
      v_Group_Field = value;
   else if( field == c_field_id_Group_Name_Field || field == c_field_name_Group_Name_Field )
      v_Group_Name_Field = value;
   else if( field == c_field_id_Hyperlink_Title_Field || field == c_field_name_Hyperlink_Title_Field )
      v_Hyperlink_Title_Field = value;
   else if( field == c_field_id_Link_Class || field == c_field_name_Link_Class )
      v_Link_Class = value;
   else if( field == c_field_id_Link_Content_Field || field == c_field_name_Link_Content_Field )
      v_Link_Content_Field = value;
   else if( field == c_field_id_Link_Group_Field || field == c_field_name_Link_Group_Field )
      v_Link_Group_Field = value;
   else if( field == c_field_id_Link_Name_Field || field == c_field_name_Link_Name_Field )
      v_Link_Name_Field = value;
   else if( field == c_field_id_Link_Page_Link_Child || field == c_field_name_Link_Page_Link_Child )
      v_Link_Page_Link_Child = value;
   else if( field == c_field_id_Meta_Content_Field || field == c_field_name_Meta_Content_Field )
      v_Meta_Content_Field = value;
   else if( field == c_field_id_Next_Child_Num_Field || field == c_field_name_Next_Child_Num_Field )
      v_Next_Child_Num_Field = value;
   else if( field == c_field_id_Order_Field || field == c_field_name_Order_Field )
      v_Order_Field = value;
   else if( field == c_field_id_Owned_Links_Field || field == c_field_name_Owned_Links_Field )
      v_Owned_Links_Field = value;
   else if( field == c_field_id_Page_Field || field == c_field_name_Page_Field )
      v_Page_Field = value;
   else if( field == c_field_id_Page_File_Class || field == c_field_name_Page_File_Class )
      v_Page_File_Class = value;
   else if( field == c_field_id_Page_File_Field || field == c_field_name_Page_File_Field )
      v_Page_File_Field = value;
   else if( field == c_field_id_Page_File_Name_Field || field == c_field_name_Page_File_Name_Field )
      v_Page_File_Name_Field = value;
   else if( field == c_field_id_Page_File_Name_Gen_Field || field == c_field_name_Page_File_Name_Gen_Field )
      v_Page_File_Name_Gen_Field = value;
   else if( field == c_field_id_Page_Link_Class || field == c_field_name_Page_Link_Class )
      v_Page_Link_Class = value;
   else if( field == c_field_id_Page_Page_Link_Child || field == c_field_name_Page_Page_Link_Child )
      v_Page_Page_Link_Child = value;
   else if( field == c_field_id_Parent_Page_Field || field == c_field_name_Parent_Page_Field )
      v_Parent_Page_Field = value;
   else if( field == c_field_id_Sub_Title_Field || field == c_field_name_Sub_Title_Field )
      v_Sub_Title_Field = value;
   else if( field == c_field_id_Text_Content_Field || field == c_field_name_Text_Content_Field )
      v_Text_Content_Field = value;
   else if( field == c_field_id_Title_Field || field == c_field_name_Title_Field )
      v_Title_Field = value;
   else if( field == c_field_id_Variable_Class || field == c_field_name_Variable_Class )
      v_Variable_Class = value;
   else if( field == c_field_id_Variable_Name || field == c_field_name_Variable_Name )
      v_Variable_Name = value;
   else if( field == c_field_id_Variable_Value || field == c_field_name_Variable_Value )
      v_Variable_Value = value;
   else
      p_obj->parent_class_type::set_foreign_key_value( field, value );
}

const string& Meta_Specification_Content_Page::impl::get_foreign_key_value( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( field == c_field_id_Actions_Field || field == c_field_name_Actions_Field )
      return v_Actions_Field;
   else if( field == c_field_id_Child_Self_Relationship || field == c_field_name_Child_Self_Relationship )
      return v_Child_Self_Relationship;
   else if( field == c_field_id_Clone_Permitted_Field || field == c_field_name_Clone_Permitted_Field )
      return v_Clone_Permitted_Field;
   else if( field == c_field_id_Code_Field || field == c_field_name_Code_Field )
      return v_Code_Field;
   else if( field == c_field_id_Content_Type || field == c_field_name_Content_Type )
      return v_Content_Type;
   else if( field == c_field_id_Create_Copy_Output_Arg || field == c_field_name_Create_Copy_Output_Arg )
      return v_Create_Copy_Output_Arg;
   else if( field == c_field_id_Create_Copy_Procedure || field == c_field_name_Create_Copy_Procedure )
      return v_Create_Copy_Procedure;
   else if( field == c_field_id_File_Name_Field || field == c_field_name_File_Name_Field )
      return v_File_Name_Field;
   else if( field == c_field_id_File_Name_Gen_Field || field == c_field_name_File_Name_Gen_Field )
      return v_File_Name_Gen_Field;
   else if( field == c_field_id_Frag_Code_Field || field == c_field_name_Frag_Code_Field )
      return v_Frag_Code_Field;
   else if( field == c_field_id_Frag_Content_Field || field == c_field_name_Frag_Content_Field )
      return v_Frag_Content_Field;
   else if( field == c_field_id_Frag_Group_Field || field == c_field_name_Frag_Group_Field )
      return v_Frag_Group_Field;
   else if( field == c_field_id_Fragment_Class || field == c_field_name_Fragment_Class )
      return v_Fragment_Class;
   else if( field == c_field_id_Generate_Children_Field || field == c_field_name_Generate_Children_Field )
      return v_Generate_Children_Field;
   else if( field == c_field_id_Generate_Output_Arg || field == c_field_name_Generate_Output_Arg )
      return v_Generate_Output_Arg;
   else if( field == c_field_id_Generate_Procedure || field == c_field_name_Generate_Procedure )
      return v_Generate_Procedure;
   else if( field == c_field_id_Group_Base_Path_Field || field == c_field_name_Group_Base_Path_Field )
      return v_Group_Base_Path_Field;
   else if( field == c_field_id_Group_Class || field == c_field_name_Group_Class )
      return v_Group_Class;
   else if( field == c_field_id_Group_Extra_Field || field == c_field_name_Group_Extra_Field )
      return v_Group_Extra_Field;
   else if( field == c_field_id_Group_Field || field == c_field_name_Group_Field )
      return v_Group_Field;
   else if( field == c_field_id_Group_Name_Field || field == c_field_name_Group_Name_Field )
      return v_Group_Name_Field;
   else if( field == c_field_id_Hyperlink_Title_Field || field == c_field_name_Hyperlink_Title_Field )
      return v_Hyperlink_Title_Field;
   else if( field == c_field_id_Link_Class || field == c_field_name_Link_Class )
      return v_Link_Class;
   else if( field == c_field_id_Link_Content_Field || field == c_field_name_Link_Content_Field )
      return v_Link_Content_Field;
   else if( field == c_field_id_Link_Group_Field || field == c_field_name_Link_Group_Field )
      return v_Link_Group_Field;
   else if( field == c_field_id_Link_Name_Field || field == c_field_name_Link_Name_Field )
      return v_Link_Name_Field;
   else if( field == c_field_id_Link_Page_Link_Child || field == c_field_name_Link_Page_Link_Child )
      return v_Link_Page_Link_Child;
   else if( field == c_field_id_Meta_Content_Field || field == c_field_name_Meta_Content_Field )
      return v_Meta_Content_Field;
   else if( field == c_field_id_Next_Child_Num_Field || field == c_field_name_Next_Child_Num_Field )
      return v_Next_Child_Num_Field;
   else if( field == c_field_id_Order_Field || field == c_field_name_Order_Field )
      return v_Order_Field;
   else if( field == c_field_id_Owned_Links_Field || field == c_field_name_Owned_Links_Field )
      return v_Owned_Links_Field;
   else if( field == c_field_id_Page_Field || field == c_field_name_Page_Field )
      return v_Page_Field;
   else if( field == c_field_id_Page_File_Class || field == c_field_name_Page_File_Class )
      return v_Page_File_Class;
   else if( field == c_field_id_Page_File_Field || field == c_field_name_Page_File_Field )
      return v_Page_File_Field;
   else if( field == c_field_id_Page_File_Name_Field || field == c_field_name_Page_File_Name_Field )
      return v_Page_File_Name_Field;
   else if( field == c_field_id_Page_File_Name_Gen_Field || field == c_field_name_Page_File_Name_Gen_Field )
      return v_Page_File_Name_Gen_Field;
   else if( field == c_field_id_Page_Link_Class || field == c_field_name_Page_Link_Class )
      return v_Page_Link_Class;
   else if( field == c_field_id_Page_Page_Link_Child || field == c_field_name_Page_Page_Link_Child )
      return v_Page_Page_Link_Child;
   else if( field == c_field_id_Parent_Page_Field || field == c_field_name_Parent_Page_Field )
      return v_Parent_Page_Field;
   else if( field == c_field_id_Sub_Title_Field || field == c_field_name_Sub_Title_Field )
      return v_Sub_Title_Field;
   else if( field == c_field_id_Text_Content_Field || field == c_field_name_Text_Content_Field )
      return v_Text_Content_Field;
   else if( field == c_field_id_Title_Field || field == c_field_name_Title_Field )
      return v_Title_Field;
   else if( field == c_field_id_Variable_Class || field == c_field_name_Variable_Class )
      return v_Variable_Class;
   else if( field == c_field_id_Variable_Name || field == c_field_name_Variable_Name )
      return v_Variable_Name;
   else if( field == c_field_id_Variable_Value || field == c_field_name_Variable_Value )
      return v_Variable_Value;
   else
      return p_obj->parent_class_type::get_foreign_key_value( field );
}

void Meta_Specification_Content_Page::impl::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Actions_Field, v_Actions_Field ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Child_Self_Relationship, v_Child_Self_Relationship ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Clone_Permitted_Field, v_Clone_Permitted_Field ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Code_Field, v_Code_Field ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Content_Type, v_Content_Type ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Create_Copy_Output_Arg, v_Create_Copy_Output_Arg ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Create_Copy_Procedure, v_Create_Copy_Procedure ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_File_Name_Field, v_File_Name_Field ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_File_Name_Gen_Field, v_File_Name_Gen_Field ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Frag_Code_Field, v_Frag_Code_Field ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Frag_Content_Field, v_Frag_Content_Field ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Frag_Group_Field, v_Frag_Group_Field ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Fragment_Class, v_Fragment_Class ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Generate_Children_Field, v_Generate_Children_Field ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Generate_Output_Arg, v_Generate_Output_Arg ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Generate_Procedure, v_Generate_Procedure ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Group_Base_Path_Field, v_Group_Base_Path_Field ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Group_Class, v_Group_Class ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Group_Extra_Field, v_Group_Extra_Field ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Group_Field, v_Group_Field ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Group_Name_Field, v_Group_Name_Field ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Hyperlink_Title_Field, v_Hyperlink_Title_Field ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Link_Class, v_Link_Class ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Link_Content_Field, v_Link_Content_Field ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Link_Group_Field, v_Link_Group_Field ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Link_Name_Field, v_Link_Name_Field ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Link_Page_Link_Child, v_Link_Page_Link_Child ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Meta_Content_Field, v_Meta_Content_Field ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Next_Child_Num_Field, v_Next_Child_Num_Field ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Order_Field, v_Order_Field ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Owned_Links_Field, v_Owned_Links_Field ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Page_Field, v_Page_Field ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Page_File_Class, v_Page_File_Class ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Page_File_Field, v_Page_File_Field ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Page_File_Name_Field, v_Page_File_Name_Field ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Page_File_Name_Gen_Field, v_Page_File_Name_Gen_Field ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Page_Link_Class, v_Page_Link_Class ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Page_Page_Link_Child, v_Page_Page_Link_Child ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Parent_Page_Field, v_Parent_Page_Field ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Sub_Title_Field, v_Sub_Title_Field ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Text_Content_Field, v_Text_Content_Field ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Title_Field, v_Title_Field ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Variable_Class, v_Variable_Class ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Variable_Name, v_Variable_Name ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Variable_Value, v_Variable_Value ) );
}

void Meta_Specification_Content_Page::impl::clear( )
{

   v_Actions_Field = string( );
   if( cp_Actions_Field )
      p_obj->setup_foreign_key( *cp_Actions_Field, v_Actions_Field );

   v_Child_Self_Relationship = string( );
   if( cp_Child_Self_Relationship )
      p_obj->setup_foreign_key( *cp_Child_Self_Relationship, v_Child_Self_Relationship );

   v_Clone_Permitted_Field = string( );
   if( cp_Clone_Permitted_Field )
      p_obj->setup_foreign_key( *cp_Clone_Permitted_Field, v_Clone_Permitted_Field );

   v_Code_Field = string( );
   if( cp_Code_Field )
      p_obj->setup_foreign_key( *cp_Code_Field, v_Code_Field );

   v_Content_Type = string( );
   if( cp_Content_Type )
      p_obj->setup_foreign_key( *cp_Content_Type, v_Content_Type );

   v_Create_Copy_Output_Arg = string( );
   if( cp_Create_Copy_Output_Arg )
      p_obj->setup_foreign_key( *cp_Create_Copy_Output_Arg, v_Create_Copy_Output_Arg );

   v_Create_Copy_Procedure = string( );
   if( cp_Create_Copy_Procedure )
      p_obj->setup_foreign_key( *cp_Create_Copy_Procedure, v_Create_Copy_Procedure );

   v_File_Name_Field = string( );
   if( cp_File_Name_Field )
      p_obj->setup_foreign_key( *cp_File_Name_Field, v_File_Name_Field );

   v_File_Name_Gen_Field = string( );
   if( cp_File_Name_Gen_Field )
      p_obj->setup_foreign_key( *cp_File_Name_Gen_Field, v_File_Name_Gen_Field );

   v_Frag_Code_Field = string( );
   if( cp_Frag_Code_Field )
      p_obj->setup_foreign_key( *cp_Frag_Code_Field, v_Frag_Code_Field );

   v_Frag_Content_Field = string( );
   if( cp_Frag_Content_Field )
      p_obj->setup_foreign_key( *cp_Frag_Content_Field, v_Frag_Content_Field );

   v_Frag_Group_Field = string( );
   if( cp_Frag_Group_Field )
      p_obj->setup_foreign_key( *cp_Frag_Group_Field, v_Frag_Group_Field );

   v_Fragment_Class = string( );
   if( cp_Fragment_Class )
      p_obj->setup_foreign_key( *cp_Fragment_Class, v_Fragment_Class );

   v_Generate_Children_Field = string( );
   if( cp_Generate_Children_Field )
      p_obj->setup_foreign_key( *cp_Generate_Children_Field, v_Generate_Children_Field );

   v_Generate_Output_Arg = string( );
   if( cp_Generate_Output_Arg )
      p_obj->setup_foreign_key( *cp_Generate_Output_Arg, v_Generate_Output_Arg );

   v_Generate_Procedure = string( );
   if( cp_Generate_Procedure )
      p_obj->setup_foreign_key( *cp_Generate_Procedure, v_Generate_Procedure );

   v_Group_Base_Path_Field = string( );
   if( cp_Group_Base_Path_Field )
      p_obj->setup_foreign_key( *cp_Group_Base_Path_Field, v_Group_Base_Path_Field );

   v_Group_Class = string( );
   if( cp_Group_Class )
      p_obj->setup_foreign_key( *cp_Group_Class, v_Group_Class );

   v_Group_Extra_Field = string( );
   if( cp_Group_Extra_Field )
      p_obj->setup_foreign_key( *cp_Group_Extra_Field, v_Group_Extra_Field );

   v_Group_Field = string( );
   if( cp_Group_Field )
      p_obj->setup_foreign_key( *cp_Group_Field, v_Group_Field );

   v_Group_Name_Field = string( );
   if( cp_Group_Name_Field )
      p_obj->setup_foreign_key( *cp_Group_Name_Field, v_Group_Name_Field );

   v_Hyperlink_Title_Field = string( );
   if( cp_Hyperlink_Title_Field )
      p_obj->setup_foreign_key( *cp_Hyperlink_Title_Field, v_Hyperlink_Title_Field );

   v_Link_Class = string( );
   if( cp_Link_Class )
      p_obj->setup_foreign_key( *cp_Link_Class, v_Link_Class );

   v_Link_Content_Field = string( );
   if( cp_Link_Content_Field )
      p_obj->setup_foreign_key( *cp_Link_Content_Field, v_Link_Content_Field );

   v_Link_Group_Field = string( );
   if( cp_Link_Group_Field )
      p_obj->setup_foreign_key( *cp_Link_Group_Field, v_Link_Group_Field );

   v_Link_Name_Field = string( );
   if( cp_Link_Name_Field )
      p_obj->setup_foreign_key( *cp_Link_Name_Field, v_Link_Name_Field );

   v_Link_Page_Link_Child = string( );
   if( cp_Link_Page_Link_Child )
      p_obj->setup_foreign_key( *cp_Link_Page_Link_Child, v_Link_Page_Link_Child );

   v_Meta_Content_Field = string( );
   if( cp_Meta_Content_Field )
      p_obj->setup_foreign_key( *cp_Meta_Content_Field, v_Meta_Content_Field );

   v_Next_Child_Num_Field = string( );
   if( cp_Next_Child_Num_Field )
      p_obj->setup_foreign_key( *cp_Next_Child_Num_Field, v_Next_Child_Num_Field );

   v_Order_Field = string( );
   if( cp_Order_Field )
      p_obj->setup_foreign_key( *cp_Order_Field, v_Order_Field );

   v_Owned_Links_Field = string( );
   if( cp_Owned_Links_Field )
      p_obj->setup_foreign_key( *cp_Owned_Links_Field, v_Owned_Links_Field );

   v_Page_Field = string( );
   if( cp_Page_Field )
      p_obj->setup_foreign_key( *cp_Page_Field, v_Page_Field );

   v_Page_File_Class = string( );
   if( cp_Page_File_Class )
      p_obj->setup_foreign_key( *cp_Page_File_Class, v_Page_File_Class );

   v_Page_File_Field = string( );
   if( cp_Page_File_Field )
      p_obj->setup_foreign_key( *cp_Page_File_Field, v_Page_File_Field );

   v_Page_File_Name_Field = string( );
   if( cp_Page_File_Name_Field )
      p_obj->setup_foreign_key( *cp_Page_File_Name_Field, v_Page_File_Name_Field );

   v_Page_File_Name_Gen_Field = string( );
   if( cp_Page_File_Name_Gen_Field )
      p_obj->setup_foreign_key( *cp_Page_File_Name_Gen_Field, v_Page_File_Name_Gen_Field );

   v_Page_Link_Class = string( );
   if( cp_Page_Link_Class )
      p_obj->setup_foreign_key( *cp_Page_Link_Class, v_Page_Link_Class );

   v_Page_Page_Link_Child = string( );
   if( cp_Page_Page_Link_Child )
      p_obj->setup_foreign_key( *cp_Page_Page_Link_Child, v_Page_Page_Link_Child );

   v_Parent_Page_Field = string( );
   if( cp_Parent_Page_Field )
      p_obj->setup_foreign_key( *cp_Parent_Page_Field, v_Parent_Page_Field );

   v_Sub_Title_Field = string( );
   if( cp_Sub_Title_Field )
      p_obj->setup_foreign_key( *cp_Sub_Title_Field, v_Sub_Title_Field );

   v_Text_Content_Field = string( );
   if( cp_Text_Content_Field )
      p_obj->setup_foreign_key( *cp_Text_Content_Field, v_Text_Content_Field );

   v_Title_Field = string( );
   if( cp_Title_Field )
      p_obj->setup_foreign_key( *cp_Title_Field, v_Title_Field );

   v_Variable_Class = string( );
   if( cp_Variable_Class )
      p_obj->setup_foreign_key( *cp_Variable_Class, v_Variable_Class );

   v_Variable_Name = string( );
   if( cp_Variable_Name )
      p_obj->setup_foreign_key( *cp_Variable_Name, v_Variable_Name );

   v_Variable_Value = string( );
   if( cp_Variable_Value )
      p_obj->setup_foreign_key( *cp_Variable_Value, v_Variable_Value );
}

bool Meta_Specification_Content_Page::impl::value_will_be_provided( const string& field_name )
{
   ( void )field_name;

   // [<start value_will_be_provided>]
   // [<finish value_will_be_provided>]

   return false;
}

void Meta_Specification_Content_Page::impl::validate( unsigned state, bool is_internal, validation_error_container* p_validation_errors )
{
   ( void )state;
   ( void )is_internal;

   if( !p_validation_errors )
      throw runtime_error( "unexpected null validation_errors container" );

   if( v_Child_Self_Relationship.empty( ) && !value_will_be_provided( c_field_name_Child_Self_Relationship ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Child_Self_Relationship,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Child_Self_Relationship ) ) ) ) );

   if( v_Code_Field.empty( ) && !value_will_be_provided( c_field_name_Code_Field ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Code_Field,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Code_Field ) ) ) ) );

   if( v_File_Name_Field.empty( ) && !value_will_be_provided( c_field_name_File_Name_Field ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_File_Name_Field,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_File_Name_Field ) ) ) ) );

   if( v_Frag_Code_Field.empty( ) && !value_will_be_provided( c_field_name_Frag_Code_Field ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Frag_Code_Field,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Frag_Code_Field ) ) ) ) );

   if( v_Frag_Content_Field.empty( ) && !value_will_be_provided( c_field_name_Frag_Content_Field ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Frag_Content_Field,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Frag_Content_Field ) ) ) ) );

   if( v_Frag_Group_Field.empty( ) && !value_will_be_provided( c_field_name_Frag_Group_Field ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Frag_Group_Field,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Frag_Group_Field ) ) ) ) );

   if( v_Fragment_Class.empty( ) && !value_will_be_provided( c_field_name_Fragment_Class ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Fragment_Class,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Fragment_Class ) ) ) ) );

   if( v_Generate_Children_Field.empty( ) && !value_will_be_provided( c_field_name_Generate_Children_Field ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Generate_Children_Field,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Generate_Children_Field ) ) ) ) );

   if( v_Generate_Output_Arg.empty( ) && !value_will_be_provided( c_field_name_Generate_Output_Arg ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Generate_Output_Arg,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Generate_Output_Arg ) ) ) ) );

   if( v_Generate_Procedure.empty( ) && !value_will_be_provided( c_field_name_Generate_Procedure ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Generate_Procedure,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Generate_Procedure ) ) ) ) );

   if( v_Group_Base_Path_Field.empty( ) && !value_will_be_provided( c_field_name_Group_Base_Path_Field ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Group_Base_Path_Field,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Group_Base_Path_Field ) ) ) ) );

   if( v_Group_Class.empty( ) && !value_will_be_provided( c_field_name_Group_Class ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Group_Class,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Group_Class ) ) ) ) );

   if( v_Group_Extra_Field.empty( ) && !value_will_be_provided( c_field_name_Group_Extra_Field ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Group_Extra_Field,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Group_Extra_Field ) ) ) ) );

   if( v_Group_Name_Field.empty( ) && !value_will_be_provided( c_field_name_Group_Name_Field ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Group_Name_Field,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Group_Name_Field ) ) ) ) );

   if( v_Hyperlink_Title_Field.empty( ) && !value_will_be_provided( c_field_name_Hyperlink_Title_Field ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Hyperlink_Title_Field,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Hyperlink_Title_Field ) ) ) ) );

   if( v_Link_Class.empty( ) && !value_will_be_provided( c_field_name_Link_Class ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Link_Class,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Link_Class ) ) ) ) );

   if( v_Link_Content_Field.empty( ) && !value_will_be_provided( c_field_name_Link_Content_Field ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Link_Content_Field,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Link_Content_Field ) ) ) ) );

   if( v_Link_Page_Link_Child.empty( ) && !value_will_be_provided( c_field_name_Link_Page_Link_Child ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Link_Page_Link_Child,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Link_Page_Link_Child ) ) ) ) );

   if( v_Meta_Content_Field.empty( ) && !value_will_be_provided( c_field_name_Meta_Content_Field ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Meta_Content_Field,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Meta_Content_Field ) ) ) ) );

   if( v_Order_Field.empty( ) && !value_will_be_provided( c_field_name_Order_Field ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Order_Field,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Order_Field ) ) ) ) );

   if( v_Page_Field.empty( ) && !value_will_be_provided( c_field_name_Page_Field ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Page_Field,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Page_Field ) ) ) ) );

   if( v_Page_File_Class.empty( ) && !value_will_be_provided( c_field_name_Page_File_Class ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Page_File_Class,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Page_File_Class ) ) ) ) );

   if( v_Page_File_Field.empty( ) && !value_will_be_provided( c_field_name_Page_File_Field ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Page_File_Field,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Page_File_Field ) ) ) ) );

   if( v_Page_File_Name_Field.empty( ) && !value_will_be_provided( c_field_name_Page_File_Name_Field ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Page_File_Name_Field,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Page_File_Name_Field ) ) ) ) );

   if( v_Page_Link_Class.empty( ) && !value_will_be_provided( c_field_name_Page_Link_Class ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Page_Link_Class,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Page_Link_Class ) ) ) ) );

   if( v_Parent_Page_Field.empty( ) && !value_will_be_provided( c_field_name_Parent_Page_Field ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Parent_Page_Field,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Parent_Page_Field ) ) ) ) );

   if( v_Sub_Title_Field.empty( ) && !value_will_be_provided( c_field_name_Sub_Title_Field ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Sub_Title_Field,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Sub_Title_Field ) ) ) ) );

   if( v_Text_Content_Field.empty( ) && !value_will_be_provided( c_field_name_Text_Content_Field ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Text_Content_Field,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Text_Content_Field ) ) ) ) );

   if( v_Title_Field.empty( ) && !value_will_be_provided( c_field_name_Title_Field ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Title_Field,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Title_Field ) ) ) ) );

   // [(start check_parent_fk_match)]
   if( get_obj( ).Parent_Page_Field( ).has_changed( ) && get_obj( ).Class( ).get_key( ) != get_obj( ).Parent_Page_Field( ).Class( ).get_key( ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Class,
       get_string_message( GS( c_str_field_must_match_parents ), make_pair(
       c_str_parm_field_must_match_parents_field, get_module_string( c_field_display_name_Class ) ),
       make_pair( c_str_parm_field_must_match_parents_pfield, get_module_string( c_field_display_name_Parent_Page_Field ) ) ) ) );
   // [(finish check_parent_fk_match)]

   // [<start validate>]
   // [<finish validate>]
}

void Meta_Specification_Content_Page::impl::after_fetch( )
{
   set< string > required_transients;

   p_obj->get_required_field_names( required_transients, true );

   if( cp_Actions_Field )
      p_obj->setup_foreign_key( *cp_Actions_Field, v_Actions_Field );

   if( cp_Child_Self_Relationship )
      p_obj->setup_foreign_key( *cp_Child_Self_Relationship, v_Child_Self_Relationship );

   if( cp_Clone_Permitted_Field )
      p_obj->setup_foreign_key( *cp_Clone_Permitted_Field, v_Clone_Permitted_Field );

   if( cp_Code_Field )
      p_obj->setup_foreign_key( *cp_Code_Field, v_Code_Field );

   if( cp_Content_Type )
      p_obj->setup_foreign_key( *cp_Content_Type, v_Content_Type );

   if( cp_Create_Copy_Output_Arg )
      p_obj->setup_foreign_key( *cp_Create_Copy_Output_Arg, v_Create_Copy_Output_Arg );

   if( cp_Create_Copy_Procedure )
      p_obj->setup_foreign_key( *cp_Create_Copy_Procedure, v_Create_Copy_Procedure );

   if( cp_File_Name_Field )
      p_obj->setup_foreign_key( *cp_File_Name_Field, v_File_Name_Field );

   if( cp_File_Name_Gen_Field )
      p_obj->setup_foreign_key( *cp_File_Name_Gen_Field, v_File_Name_Gen_Field );

   if( cp_Frag_Code_Field )
      p_obj->setup_foreign_key( *cp_Frag_Code_Field, v_Frag_Code_Field );

   if( cp_Frag_Content_Field )
      p_obj->setup_foreign_key( *cp_Frag_Content_Field, v_Frag_Content_Field );

   if( cp_Frag_Group_Field )
      p_obj->setup_foreign_key( *cp_Frag_Group_Field, v_Frag_Group_Field );

   if( cp_Fragment_Class )
      p_obj->setup_foreign_key( *cp_Fragment_Class, v_Fragment_Class );

   if( cp_Generate_Children_Field )
      p_obj->setup_foreign_key( *cp_Generate_Children_Field, v_Generate_Children_Field );

   if( cp_Generate_Output_Arg )
      p_obj->setup_foreign_key( *cp_Generate_Output_Arg, v_Generate_Output_Arg );

   if( cp_Generate_Procedure )
      p_obj->setup_foreign_key( *cp_Generate_Procedure, v_Generate_Procedure );

   if( cp_Group_Base_Path_Field )
      p_obj->setup_foreign_key( *cp_Group_Base_Path_Field, v_Group_Base_Path_Field );

   if( cp_Group_Class )
      p_obj->setup_foreign_key( *cp_Group_Class, v_Group_Class );

   if( cp_Group_Extra_Field )
      p_obj->setup_foreign_key( *cp_Group_Extra_Field, v_Group_Extra_Field );

   if( cp_Group_Field )
      p_obj->setup_foreign_key( *cp_Group_Field, v_Group_Field );

   if( cp_Group_Name_Field )
      p_obj->setup_foreign_key( *cp_Group_Name_Field, v_Group_Name_Field );

   if( cp_Hyperlink_Title_Field )
      p_obj->setup_foreign_key( *cp_Hyperlink_Title_Field, v_Hyperlink_Title_Field );

   if( cp_Link_Class )
      p_obj->setup_foreign_key( *cp_Link_Class, v_Link_Class );

   if( cp_Link_Content_Field )
      p_obj->setup_foreign_key( *cp_Link_Content_Field, v_Link_Content_Field );

   if( cp_Link_Group_Field )
      p_obj->setup_foreign_key( *cp_Link_Group_Field, v_Link_Group_Field );

   if( cp_Link_Name_Field )
      p_obj->setup_foreign_key( *cp_Link_Name_Field, v_Link_Name_Field );

   if( cp_Link_Page_Link_Child )
      p_obj->setup_foreign_key( *cp_Link_Page_Link_Child, v_Link_Page_Link_Child );

   if( cp_Meta_Content_Field )
      p_obj->setup_foreign_key( *cp_Meta_Content_Field, v_Meta_Content_Field );

   if( cp_Next_Child_Num_Field )
      p_obj->setup_foreign_key( *cp_Next_Child_Num_Field, v_Next_Child_Num_Field );

   if( cp_Order_Field )
      p_obj->setup_foreign_key( *cp_Order_Field, v_Order_Field );

   if( cp_Owned_Links_Field )
      p_obj->setup_foreign_key( *cp_Owned_Links_Field, v_Owned_Links_Field );

   if( cp_Page_Field )
      p_obj->setup_foreign_key( *cp_Page_Field, v_Page_Field );

   if( cp_Page_File_Class )
      p_obj->setup_foreign_key( *cp_Page_File_Class, v_Page_File_Class );

   if( cp_Page_File_Field )
      p_obj->setup_foreign_key( *cp_Page_File_Field, v_Page_File_Field );

   if( cp_Page_File_Name_Field )
      p_obj->setup_foreign_key( *cp_Page_File_Name_Field, v_Page_File_Name_Field );

   if( cp_Page_File_Name_Gen_Field )
      p_obj->setup_foreign_key( *cp_Page_File_Name_Gen_Field, v_Page_File_Name_Gen_Field );

   if( cp_Page_Link_Class )
      p_obj->setup_foreign_key( *cp_Page_Link_Class, v_Page_Link_Class );

   if( cp_Page_Page_Link_Child )
      p_obj->setup_foreign_key( *cp_Page_Page_Link_Child, v_Page_Page_Link_Child );

   if( cp_Parent_Page_Field )
      p_obj->setup_foreign_key( *cp_Parent_Page_Field, v_Parent_Page_Field );

   if( cp_Sub_Title_Field )
      p_obj->setup_foreign_key( *cp_Sub_Title_Field, v_Sub_Title_Field );

   if( cp_Text_Content_Field )
      p_obj->setup_foreign_key( *cp_Text_Content_Field, v_Text_Content_Field );

   if( cp_Title_Field )
      p_obj->setup_foreign_key( *cp_Title_Field, v_Title_Field );

   if( cp_Variable_Class )
      p_obj->setup_foreign_key( *cp_Variable_Class, v_Variable_Class );

   if( cp_Variable_Name )
      p_obj->setup_foreign_key( *cp_Variable_Name, v_Variable_Name );

   if( cp_Variable_Value )
      p_obj->setup_foreign_key( *cp_Variable_Value, v_Variable_Value );

   // [(start field_from_search_replace)]
   if( !get_obj( ).get_key( ).empty( )
    && ( get_obj( ).needs_field_value( "Vars" )
    || required_transients.count( "Vars" ) ) )
   {
      string str( get_obj( ).Specification_Type( ).Specification_Vars( ) );

      get_obj( ).Vars( str );

      get_obj( ).add_search_replacement( "Vars", "{pgcode}", to_string( get_obj( ).Code_Field( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{pggroup}", to_string( get_obj( ).Group_Field( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{pgtitle}", to_string( get_obj( ).Title_Field( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{pgfname}", to_string( get_obj( ).File_Name_Field( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{pgfnamegen}", to_string( get_obj( ).File_Name_Gen_Field( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{pgstitle}", to_string( get_obj( ).Sub_Title_Field( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{pgparent}", to_string( get_obj( ).Parent_Page_Field( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{pgctype}", to_string( get_obj( ).Content_Type( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{pgnext}", to_string( get_obj( ).Next_Child_Num_Field( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{pgtext}", to_string( get_obj( ).Text_Content_Field( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{pgmeta}", to_string( get_obj( ).Meta_Content_Field( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{pglink}", to_string( get_obj( ).Link_Content_Field( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{pgactions}", to_string( get_obj( ).Actions_Field( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{pghyper}", to_string( get_obj( ).Hyperlink_Title_Field( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{pgchildgen}", to_string( get_obj( ).Generate_Children_Field( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{pgcanclone}", to_string( get_obj( ).Clone_Permitted_Field( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{pgchild}", to_string( get_obj( ).Child_Self_Relationship( ).Child_Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{pggen}", to_string( get_obj( ).Generate_Procedure( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{pggenout}", to_string( get_obj( ).Generate_Output_Arg( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{pgcopy}", to_string( get_obj( ).Create_Copy_Procedure( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{pgcopyid}", to_string( get_obj( ).Create_Copy_Procedure( ).Id( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{pgcopyout}", to_string( get_obj( ).Create_Copy_Output_Arg( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{plclass}", to_string( get_obj( ).Page_Link_Class( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{plorder}", to_string( get_obj( ).Order_Field( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{plpage}", to_string( get_obj( ).Page_Field( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{pglchild}", to_string( get_obj( ).Page_Page_Link_Child( ).Child_Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{plchild}", to_string( get_obj( ).Link_Page_Link_Child( ).Child_Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{pfclass}", to_string( get_obj( ).Page_File_Class( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{pffile}", to_string( get_obj( ).Page_File_Field( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{pfname}", to_string( get_obj( ).Page_File_Name_Field( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{pfnamegen}", to_string( get_obj( ).Page_File_Name_Gen_Field( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{frclass}", to_string( get_obj( ).Fragment_Class( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{frcode}", to_string( get_obj( ).Frag_Code_Field( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{vclass}", to_string( get_obj( ).Variable_Class( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{vname}", to_string( get_obj( ).Variable_Name( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{vvalue}", to_string( get_obj( ).Variable_Value( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{lclass}", to_string( get_obj( ).Link_Class( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{lname}", to_string( get_obj( ).Link_Name_Field( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{lgroup}", to_string( get_obj( ).Link_Group_Field( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{pgolinks}", to_string( get_obj( ).Owned_Links_Field( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{frgroup}", to_string( get_obj( ).Frag_Group_Field( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{frmeta}", to_string( get_obj( ).Frag_Content_Field( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{gclass}", to_string( get_obj( ).Group_Class( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{gname}", to_string( get_obj( ).Group_Name_Field( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{gextra}", to_string( get_obj( ).Group_Extra_Field( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{gbase}", to_string( get_obj( ).Group_Base_Path_Field( ).Name( ) ) );
   }
   // [(finish field_from_search_replace)]

   // [<start after_fetch>]
   // [<finish after_fetch>]
}

void Meta_Specification_Content_Page::impl::finalise_fetch( )
{
   set< string > required_transients;

   p_obj->get_required_field_names( required_transients, true );

   // [<start finalise_fetch>]
   // [<finish finalise_fetch>]
}

void Meta_Specification_Content_Page::impl::at_create( )
{
   // [<start at_create>]
   // [<finish at_create>]
}

void Meta_Specification_Content_Page::impl::to_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start field_from_other_field)]
   get_obj( ).Link_Class( get_obj( ).Link_Content_Field( ).Parent_Class( ) );
   // [(finish field_from_other_field)]

   // [<start to_store>]
   // [<finish to_store>]
}

void Meta_Specification_Content_Page::impl::for_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   // [<start for_store>]
   // [<finish for_store>]
}

void Meta_Specification_Content_Page::impl::after_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   // [<start after_store>]
   // [<finish after_store>]
}

bool Meta_Specification_Content_Page::impl::can_destroy( bool is_internal )
{
   bool retval = is_internal || !( get_state( ) & c_state_undeletable );

   // [<start can_destroy>]
   // [<finish can_destroy>]

   return retval;
}

void Meta_Specification_Content_Page::impl::for_destroy( bool is_internal )
{
   ( void )is_internal;

   // [<start for_destroy>]
   // [<finish for_destroy>]
}

void Meta_Specification_Content_Page::impl::after_destroy( bool is_internal )
{
   ( void )is_internal;

   // [<start after_destroy>]
   // [<finish after_destroy>]
}

void Meta_Specification_Content_Page::impl::set_default_values( )
{
   clear( );
}

bool Meta_Specification_Content_Page::impl::is_filtered( const set< string >& filters ) const
{
   ( void )filters;

   // [<start is_filtered>]
   // [<finish is_filtered>]

   return false;
}

#undef MODULE_TRACE
#define MODULE_TRACE( x ) trace( x )

Meta_Specification_Content_Page::Meta_Specification_Content_Page( )
{
   set_version( c_version );

   p_impl = new impl( *this );
}

Meta_Specification_Content_Page::~Meta_Specification_Content_Page( )
{
   delete p_impl;
}

Meta_Field& Meta_Specification_Content_Page::Actions_Field( )
{
   return p_impl->impl_Actions_Field( );
}

const Meta_Field& Meta_Specification_Content_Page::Actions_Field( ) const
{
   return p_impl->impl_Actions_Field( );
}

void Meta_Specification_Content_Page::Actions_Field( const string& key )
{
   p_impl->impl_Actions_Field( key );
}

Meta_Relationship& Meta_Specification_Content_Page::Child_Self_Relationship( )
{
   return p_impl->impl_Child_Self_Relationship( );
}

const Meta_Relationship& Meta_Specification_Content_Page::Child_Self_Relationship( ) const
{
   return p_impl->impl_Child_Self_Relationship( );
}

void Meta_Specification_Content_Page::Child_Self_Relationship( const string& key )
{
   p_impl->impl_Child_Self_Relationship( key );
}

Meta_Field& Meta_Specification_Content_Page::Clone_Permitted_Field( )
{
   return p_impl->impl_Clone_Permitted_Field( );
}

const Meta_Field& Meta_Specification_Content_Page::Clone_Permitted_Field( ) const
{
   return p_impl->impl_Clone_Permitted_Field( );
}

void Meta_Specification_Content_Page::Clone_Permitted_Field( const string& key )
{
   p_impl->impl_Clone_Permitted_Field( key );
}

Meta_Field& Meta_Specification_Content_Page::Code_Field( )
{
   return p_impl->impl_Code_Field( );
}

const Meta_Field& Meta_Specification_Content_Page::Code_Field( ) const
{
   return p_impl->impl_Code_Field( );
}

void Meta_Specification_Content_Page::Code_Field( const string& key )
{
   p_impl->impl_Code_Field( key );
}

Meta_Field& Meta_Specification_Content_Page::Content_Type( )
{
   return p_impl->impl_Content_Type( );
}

const Meta_Field& Meta_Specification_Content_Page::Content_Type( ) const
{
   return p_impl->impl_Content_Type( );
}

void Meta_Specification_Content_Page::Content_Type( const string& key )
{
   p_impl->impl_Content_Type( key );
}

Meta_Procedure_Arg& Meta_Specification_Content_Page::Create_Copy_Output_Arg( )
{
   return p_impl->impl_Create_Copy_Output_Arg( );
}

const Meta_Procedure_Arg& Meta_Specification_Content_Page::Create_Copy_Output_Arg( ) const
{
   return p_impl->impl_Create_Copy_Output_Arg( );
}

void Meta_Specification_Content_Page::Create_Copy_Output_Arg( const string& key )
{
   p_impl->impl_Create_Copy_Output_Arg( key );
}

Meta_Procedure& Meta_Specification_Content_Page::Create_Copy_Procedure( )
{
   return p_impl->impl_Create_Copy_Procedure( );
}

const Meta_Procedure& Meta_Specification_Content_Page::Create_Copy_Procedure( ) const
{
   return p_impl->impl_Create_Copy_Procedure( );
}

void Meta_Specification_Content_Page::Create_Copy_Procedure( const string& key )
{
   p_impl->impl_Create_Copy_Procedure( key );
}

Meta_Field& Meta_Specification_Content_Page::File_Name_Field( )
{
   return p_impl->impl_File_Name_Field( );
}

const Meta_Field& Meta_Specification_Content_Page::File_Name_Field( ) const
{
   return p_impl->impl_File_Name_Field( );
}

void Meta_Specification_Content_Page::File_Name_Field( const string& key )
{
   p_impl->impl_File_Name_Field( key );
}

Meta_Field& Meta_Specification_Content_Page::File_Name_Gen_Field( )
{
   return p_impl->impl_File_Name_Gen_Field( );
}

const Meta_Field& Meta_Specification_Content_Page::File_Name_Gen_Field( ) const
{
   return p_impl->impl_File_Name_Gen_Field( );
}

void Meta_Specification_Content_Page::File_Name_Gen_Field( const string& key )
{
   p_impl->impl_File_Name_Gen_Field( key );
}

Meta_Field& Meta_Specification_Content_Page::Frag_Code_Field( )
{
   return p_impl->impl_Frag_Code_Field( );
}

const Meta_Field& Meta_Specification_Content_Page::Frag_Code_Field( ) const
{
   return p_impl->impl_Frag_Code_Field( );
}

void Meta_Specification_Content_Page::Frag_Code_Field( const string& key )
{
   p_impl->impl_Frag_Code_Field( key );
}

Meta_Field& Meta_Specification_Content_Page::Frag_Content_Field( )
{
   return p_impl->impl_Frag_Content_Field( );
}

const Meta_Field& Meta_Specification_Content_Page::Frag_Content_Field( ) const
{
   return p_impl->impl_Frag_Content_Field( );
}

void Meta_Specification_Content_Page::Frag_Content_Field( const string& key )
{
   p_impl->impl_Frag_Content_Field( key );
}

Meta_Field& Meta_Specification_Content_Page::Frag_Group_Field( )
{
   return p_impl->impl_Frag_Group_Field( );
}

const Meta_Field& Meta_Specification_Content_Page::Frag_Group_Field( ) const
{
   return p_impl->impl_Frag_Group_Field( );
}

void Meta_Specification_Content_Page::Frag_Group_Field( const string& key )
{
   p_impl->impl_Frag_Group_Field( key );
}

Meta_Class& Meta_Specification_Content_Page::Fragment_Class( )
{
   return p_impl->impl_Fragment_Class( );
}

const Meta_Class& Meta_Specification_Content_Page::Fragment_Class( ) const
{
   return p_impl->impl_Fragment_Class( );
}

void Meta_Specification_Content_Page::Fragment_Class( const string& key )
{
   p_impl->impl_Fragment_Class( key );
}

Meta_Field& Meta_Specification_Content_Page::Generate_Children_Field( )
{
   return p_impl->impl_Generate_Children_Field( );
}

const Meta_Field& Meta_Specification_Content_Page::Generate_Children_Field( ) const
{
   return p_impl->impl_Generate_Children_Field( );
}

void Meta_Specification_Content_Page::Generate_Children_Field( const string& key )
{
   p_impl->impl_Generate_Children_Field( key );
}

Meta_Procedure_Arg& Meta_Specification_Content_Page::Generate_Output_Arg( )
{
   return p_impl->impl_Generate_Output_Arg( );
}

const Meta_Procedure_Arg& Meta_Specification_Content_Page::Generate_Output_Arg( ) const
{
   return p_impl->impl_Generate_Output_Arg( );
}

void Meta_Specification_Content_Page::Generate_Output_Arg( const string& key )
{
   p_impl->impl_Generate_Output_Arg( key );
}

Meta_Procedure& Meta_Specification_Content_Page::Generate_Procedure( )
{
   return p_impl->impl_Generate_Procedure( );
}

const Meta_Procedure& Meta_Specification_Content_Page::Generate_Procedure( ) const
{
   return p_impl->impl_Generate_Procedure( );
}

void Meta_Specification_Content_Page::Generate_Procedure( const string& key )
{
   p_impl->impl_Generate_Procedure( key );
}

Meta_Field& Meta_Specification_Content_Page::Group_Base_Path_Field( )
{
   return p_impl->impl_Group_Base_Path_Field( );
}

const Meta_Field& Meta_Specification_Content_Page::Group_Base_Path_Field( ) const
{
   return p_impl->impl_Group_Base_Path_Field( );
}

void Meta_Specification_Content_Page::Group_Base_Path_Field( const string& key )
{
   p_impl->impl_Group_Base_Path_Field( key );
}

Meta_Class& Meta_Specification_Content_Page::Group_Class( )
{
   return p_impl->impl_Group_Class( );
}

const Meta_Class& Meta_Specification_Content_Page::Group_Class( ) const
{
   return p_impl->impl_Group_Class( );
}

void Meta_Specification_Content_Page::Group_Class( const string& key )
{
   p_impl->impl_Group_Class( key );
}

Meta_Field& Meta_Specification_Content_Page::Group_Extra_Field( )
{
   return p_impl->impl_Group_Extra_Field( );
}

const Meta_Field& Meta_Specification_Content_Page::Group_Extra_Field( ) const
{
   return p_impl->impl_Group_Extra_Field( );
}

void Meta_Specification_Content_Page::Group_Extra_Field( const string& key )
{
   p_impl->impl_Group_Extra_Field( key );
}

Meta_Field& Meta_Specification_Content_Page::Group_Field( )
{
   return p_impl->impl_Group_Field( );
}

const Meta_Field& Meta_Specification_Content_Page::Group_Field( ) const
{
   return p_impl->impl_Group_Field( );
}

void Meta_Specification_Content_Page::Group_Field( const string& key )
{
   p_impl->impl_Group_Field( key );
}

Meta_Field& Meta_Specification_Content_Page::Group_Name_Field( )
{
   return p_impl->impl_Group_Name_Field( );
}

const Meta_Field& Meta_Specification_Content_Page::Group_Name_Field( ) const
{
   return p_impl->impl_Group_Name_Field( );
}

void Meta_Specification_Content_Page::Group_Name_Field( const string& key )
{
   p_impl->impl_Group_Name_Field( key );
}

Meta_Field& Meta_Specification_Content_Page::Hyperlink_Title_Field( )
{
   return p_impl->impl_Hyperlink_Title_Field( );
}

const Meta_Field& Meta_Specification_Content_Page::Hyperlink_Title_Field( ) const
{
   return p_impl->impl_Hyperlink_Title_Field( );
}

void Meta_Specification_Content_Page::Hyperlink_Title_Field( const string& key )
{
   p_impl->impl_Hyperlink_Title_Field( key );
}

Meta_Class& Meta_Specification_Content_Page::Link_Class( )
{
   return p_impl->impl_Link_Class( );
}

const Meta_Class& Meta_Specification_Content_Page::Link_Class( ) const
{
   return p_impl->impl_Link_Class( );
}

void Meta_Specification_Content_Page::Link_Class( const string& key )
{
   p_impl->impl_Link_Class( key );
}

Meta_Field& Meta_Specification_Content_Page::Link_Content_Field( )
{
   return p_impl->impl_Link_Content_Field( );
}

const Meta_Field& Meta_Specification_Content_Page::Link_Content_Field( ) const
{
   return p_impl->impl_Link_Content_Field( );
}

void Meta_Specification_Content_Page::Link_Content_Field( const string& key )
{
   p_impl->impl_Link_Content_Field( key );
}

Meta_Field& Meta_Specification_Content_Page::Link_Group_Field( )
{
   return p_impl->impl_Link_Group_Field( );
}

const Meta_Field& Meta_Specification_Content_Page::Link_Group_Field( ) const
{
   return p_impl->impl_Link_Group_Field( );
}

void Meta_Specification_Content_Page::Link_Group_Field( const string& key )
{
   p_impl->impl_Link_Group_Field( key );
}

Meta_Field& Meta_Specification_Content_Page::Link_Name_Field( )
{
   return p_impl->impl_Link_Name_Field( );
}

const Meta_Field& Meta_Specification_Content_Page::Link_Name_Field( ) const
{
   return p_impl->impl_Link_Name_Field( );
}

void Meta_Specification_Content_Page::Link_Name_Field( const string& key )
{
   p_impl->impl_Link_Name_Field( key );
}

Meta_Relationship& Meta_Specification_Content_Page::Link_Page_Link_Child( )
{
   return p_impl->impl_Link_Page_Link_Child( );
}

const Meta_Relationship& Meta_Specification_Content_Page::Link_Page_Link_Child( ) const
{
   return p_impl->impl_Link_Page_Link_Child( );
}

void Meta_Specification_Content_Page::Link_Page_Link_Child( const string& key )
{
   p_impl->impl_Link_Page_Link_Child( key );
}

Meta_Field& Meta_Specification_Content_Page::Meta_Content_Field( )
{
   return p_impl->impl_Meta_Content_Field( );
}

const Meta_Field& Meta_Specification_Content_Page::Meta_Content_Field( ) const
{
   return p_impl->impl_Meta_Content_Field( );
}

void Meta_Specification_Content_Page::Meta_Content_Field( const string& key )
{
   p_impl->impl_Meta_Content_Field( key );
}

Meta_Field& Meta_Specification_Content_Page::Next_Child_Num_Field( )
{
   return p_impl->impl_Next_Child_Num_Field( );
}

const Meta_Field& Meta_Specification_Content_Page::Next_Child_Num_Field( ) const
{
   return p_impl->impl_Next_Child_Num_Field( );
}

void Meta_Specification_Content_Page::Next_Child_Num_Field( const string& key )
{
   p_impl->impl_Next_Child_Num_Field( key );
}

Meta_Field& Meta_Specification_Content_Page::Order_Field( )
{
   return p_impl->impl_Order_Field( );
}

const Meta_Field& Meta_Specification_Content_Page::Order_Field( ) const
{
   return p_impl->impl_Order_Field( );
}

void Meta_Specification_Content_Page::Order_Field( const string& key )
{
   p_impl->impl_Order_Field( key );
}

Meta_Field& Meta_Specification_Content_Page::Owned_Links_Field( )
{
   return p_impl->impl_Owned_Links_Field( );
}

const Meta_Field& Meta_Specification_Content_Page::Owned_Links_Field( ) const
{
   return p_impl->impl_Owned_Links_Field( );
}

void Meta_Specification_Content_Page::Owned_Links_Field( const string& key )
{
   p_impl->impl_Owned_Links_Field( key );
}

Meta_Field& Meta_Specification_Content_Page::Page_Field( )
{
   return p_impl->impl_Page_Field( );
}

const Meta_Field& Meta_Specification_Content_Page::Page_Field( ) const
{
   return p_impl->impl_Page_Field( );
}

void Meta_Specification_Content_Page::Page_Field( const string& key )
{
   p_impl->impl_Page_Field( key );
}

Meta_Class& Meta_Specification_Content_Page::Page_File_Class( )
{
   return p_impl->impl_Page_File_Class( );
}

const Meta_Class& Meta_Specification_Content_Page::Page_File_Class( ) const
{
   return p_impl->impl_Page_File_Class( );
}

void Meta_Specification_Content_Page::Page_File_Class( const string& key )
{
   p_impl->impl_Page_File_Class( key );
}

Meta_Field& Meta_Specification_Content_Page::Page_File_Field( )
{
   return p_impl->impl_Page_File_Field( );
}

const Meta_Field& Meta_Specification_Content_Page::Page_File_Field( ) const
{
   return p_impl->impl_Page_File_Field( );
}

void Meta_Specification_Content_Page::Page_File_Field( const string& key )
{
   p_impl->impl_Page_File_Field( key );
}

Meta_Field& Meta_Specification_Content_Page::Page_File_Name_Field( )
{
   return p_impl->impl_Page_File_Name_Field( );
}

const Meta_Field& Meta_Specification_Content_Page::Page_File_Name_Field( ) const
{
   return p_impl->impl_Page_File_Name_Field( );
}

void Meta_Specification_Content_Page::Page_File_Name_Field( const string& key )
{
   p_impl->impl_Page_File_Name_Field( key );
}

Meta_Field& Meta_Specification_Content_Page::Page_File_Name_Gen_Field( )
{
   return p_impl->impl_Page_File_Name_Gen_Field( );
}

const Meta_Field& Meta_Specification_Content_Page::Page_File_Name_Gen_Field( ) const
{
   return p_impl->impl_Page_File_Name_Gen_Field( );
}

void Meta_Specification_Content_Page::Page_File_Name_Gen_Field( const string& key )
{
   p_impl->impl_Page_File_Name_Gen_Field( key );
}

Meta_Class& Meta_Specification_Content_Page::Page_Link_Class( )
{
   return p_impl->impl_Page_Link_Class( );
}

const Meta_Class& Meta_Specification_Content_Page::Page_Link_Class( ) const
{
   return p_impl->impl_Page_Link_Class( );
}

void Meta_Specification_Content_Page::Page_Link_Class( const string& key )
{
   p_impl->impl_Page_Link_Class( key );
}

Meta_Relationship& Meta_Specification_Content_Page::Page_Page_Link_Child( )
{
   return p_impl->impl_Page_Page_Link_Child( );
}

const Meta_Relationship& Meta_Specification_Content_Page::Page_Page_Link_Child( ) const
{
   return p_impl->impl_Page_Page_Link_Child( );
}

void Meta_Specification_Content_Page::Page_Page_Link_Child( const string& key )
{
   p_impl->impl_Page_Page_Link_Child( key );
}

Meta_Field& Meta_Specification_Content_Page::Parent_Page_Field( )
{
   return p_impl->impl_Parent_Page_Field( );
}

const Meta_Field& Meta_Specification_Content_Page::Parent_Page_Field( ) const
{
   return p_impl->impl_Parent_Page_Field( );
}

void Meta_Specification_Content_Page::Parent_Page_Field( const string& key )
{
   p_impl->impl_Parent_Page_Field( key );
}

Meta_Field& Meta_Specification_Content_Page::Sub_Title_Field( )
{
   return p_impl->impl_Sub_Title_Field( );
}

const Meta_Field& Meta_Specification_Content_Page::Sub_Title_Field( ) const
{
   return p_impl->impl_Sub_Title_Field( );
}

void Meta_Specification_Content_Page::Sub_Title_Field( const string& key )
{
   p_impl->impl_Sub_Title_Field( key );
}

Meta_Field& Meta_Specification_Content_Page::Text_Content_Field( )
{
   return p_impl->impl_Text_Content_Field( );
}

const Meta_Field& Meta_Specification_Content_Page::Text_Content_Field( ) const
{
   return p_impl->impl_Text_Content_Field( );
}

void Meta_Specification_Content_Page::Text_Content_Field( const string& key )
{
   p_impl->impl_Text_Content_Field( key );
}

Meta_Field& Meta_Specification_Content_Page::Title_Field( )
{
   return p_impl->impl_Title_Field( );
}

const Meta_Field& Meta_Specification_Content_Page::Title_Field( ) const
{
   return p_impl->impl_Title_Field( );
}

void Meta_Specification_Content_Page::Title_Field( const string& key )
{
   p_impl->impl_Title_Field( key );
}

Meta_Class& Meta_Specification_Content_Page::Variable_Class( )
{
   return p_impl->impl_Variable_Class( );
}

const Meta_Class& Meta_Specification_Content_Page::Variable_Class( ) const
{
   return p_impl->impl_Variable_Class( );
}

void Meta_Specification_Content_Page::Variable_Class( const string& key )
{
   p_impl->impl_Variable_Class( key );
}

Meta_Field& Meta_Specification_Content_Page::Variable_Name( )
{
   return p_impl->impl_Variable_Name( );
}

const Meta_Field& Meta_Specification_Content_Page::Variable_Name( ) const
{
   return p_impl->impl_Variable_Name( );
}

void Meta_Specification_Content_Page::Variable_Name( const string& key )
{
   p_impl->impl_Variable_Name( key );
}

Meta_Field& Meta_Specification_Content_Page::Variable_Value( )
{
   return p_impl->impl_Variable_Value( );
}

const Meta_Field& Meta_Specification_Content_Page::Variable_Value( ) const
{
   return p_impl->impl_Variable_Value( );
}

void Meta_Specification_Content_Page::Variable_Value( const string& key )
{
   p_impl->impl_Variable_Value( key );
}

string Meta_Specification_Content_Page::get_field_value( int field ) const
{
   int num_parent_fields( parent_class_type::get_num_fields( ) );

   if( field < num_parent_fields )
      return parent_class_type::get_field_value( field );
   else
      return p_impl->get_field_value( field - num_parent_fields );
}

void Meta_Specification_Content_Page::set_field_value( int field, const string& value )
{
   int num_parent_fields( parent_class_type::get_num_fields( ) );

   if( field < num_parent_fields )
      parent_class_type::set_field_value( field, value );
   else
      p_impl->set_field_value( field - num_parent_fields, value );
}

bool Meta_Specification_Content_Page::is_field_transient( int field ) const
{
   int num_parent_fields( parent_class_type::get_num_fields( ) );

   if( field < num_parent_fields )
      return parent_class_type::is_field_transient( field );
   else
      return static_is_field_transient( ( field_id )( field - num_parent_fields + 1 ) );
}

string Meta_Specification_Content_Page::get_field_name( int field ) const
{
   int num_parent_fields( parent_class_type::get_num_fields( ) );

   if( field < num_parent_fields )
      return parent_class_type::get_field_name( field );
   else
      return static_get_field_name( ( field_id )( field - num_parent_fields + 1 ) );
}

int Meta_Specification_Content_Page::get_field_num( const string& field ) const
{
   int rc = static_get_field_num( field );

   if( rc < 0 )
      throw runtime_error( "unknown field name/id '" + field + "' in get_field_num( )" );

   return rc;
}

bool Meta_Specification_Content_Page::has_field_changed( const string& field ) const
{
   return class_base::has_field_changed( get_field_num( field ) );
}

uint64_t Meta_Specification_Content_Page::get_state( ) const
{
   uint64_t state = 0;

   state |= parent_class_type::get_state( );
   state |= p_impl->get_state( );

   return state;
}

const string& Meta_Specification_Content_Page::execute( const string& cmd_and_args )
{
   return p_impl->execute( cmd_and_args );
}

void Meta_Specification_Content_Page::clear( )
{
   parent_class_type::clear( );
   p_impl->clear( );
}

void Meta_Specification_Content_Page::validate( unsigned state, bool is_internal )
{
   parent_class_type::validate( state, is_internal );
   p_impl->validate( state, is_internal, &validation_errors );
}

void Meta_Specification_Content_Page::after_fetch( )
{
   parent_class_type::after_fetch( );
   p_impl->after_fetch( );
}

void Meta_Specification_Content_Page::finalise_fetch( )
{
   parent_class_type::finalise_fetch( );
   p_impl->finalise_fetch( );
}

void Meta_Specification_Content_Page::at_create( )
{
   parent_class_type::at_create( );
   p_impl->at_create( );
}

void Meta_Specification_Content_Page::to_store( bool is_create, bool is_internal )
{
   parent_class_type::to_store( is_create, is_internal );
   p_impl->to_store( is_create, is_internal );
}

void Meta_Specification_Content_Page::for_store( bool is_create, bool is_internal )
{
   parent_class_type::for_store( is_create, is_internal );
   p_impl->for_store( is_create, is_internal );
}

void Meta_Specification_Content_Page::after_store( bool is_create, bool is_internal )
{
   parent_class_type::after_store( is_create, is_internal );
   p_impl->after_store( is_create, is_internal );
}

bool Meta_Specification_Content_Page::can_destroy( bool is_internal )
{
   if( !parent_class_type::can_destroy( is_internal ) )
      return false;
   return p_impl->can_destroy( is_internal );
}

void Meta_Specification_Content_Page::for_destroy( bool is_internal )
{
   p_impl->for_destroy( is_internal );
   parent_class_type::for_destroy( is_internal );
}

void Meta_Specification_Content_Page::after_destroy( bool is_internal )
{
   p_impl->after_destroy( is_internal );
   parent_class_type::after_destroy( is_internal );
}

void Meta_Specification_Content_Page::set_default_values( )
{
   parent_class_type::set_default_values( );
   p_impl->set_default_values( );
}

bool Meta_Specification_Content_Page::is_filtered( const set< string >& filters ) const
{
   if( parent_class_type::is_filtered( filters ) )
      return true;
   return p_impl->is_filtered( filters );
}

const char* Meta_Specification_Content_Page::get_field_id(
 const string& name, bool* p_sql_numeric, string* p_type_name ) const
{
   const char* p_id( parent_class_type::get_field_id( name, p_sql_numeric, p_type_name ) );
   if( p_id )
      return p_id;

   if( name.empty( ) )
      throw runtime_error( "unexpected empty field name for get_field_id" );
   else if( name == c_field_name_Actions_Field )
   {
      p_id = c_field_id_Actions_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Child_Self_Relationship )
   {
      p_id = c_field_id_Child_Self_Relationship;

      if( p_type_name )
         *p_type_name = "Meta_Relationship";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Clone_Permitted_Field )
   {
      p_id = c_field_id_Clone_Permitted_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Code_Field )
   {
      p_id = c_field_id_Code_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Content_Type )
   {
      p_id = c_field_id_Content_Type;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Create_Copy_Output_Arg )
   {
      p_id = c_field_id_Create_Copy_Output_Arg;

      if( p_type_name )
         *p_type_name = "Meta_Procedure_Arg";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Create_Copy_Procedure )
   {
      p_id = c_field_id_Create_Copy_Procedure;

      if( p_type_name )
         *p_type_name = "Meta_Procedure";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_File_Name_Field )
   {
      p_id = c_field_id_File_Name_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_File_Name_Gen_Field )
   {
      p_id = c_field_id_File_Name_Gen_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Frag_Code_Field )
   {
      p_id = c_field_id_Frag_Code_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Frag_Content_Field )
   {
      p_id = c_field_id_Frag_Content_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Frag_Group_Field )
   {
      p_id = c_field_id_Frag_Group_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Fragment_Class )
   {
      p_id = c_field_id_Fragment_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Generate_Children_Field )
   {
      p_id = c_field_id_Generate_Children_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Generate_Output_Arg )
   {
      p_id = c_field_id_Generate_Output_Arg;

      if( p_type_name )
         *p_type_name = "Meta_Procedure_Arg";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Generate_Procedure )
   {
      p_id = c_field_id_Generate_Procedure;

      if( p_type_name )
         *p_type_name = "Meta_Procedure";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Group_Base_Path_Field )
   {
      p_id = c_field_id_Group_Base_Path_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Group_Class )
   {
      p_id = c_field_id_Group_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Group_Extra_Field )
   {
      p_id = c_field_id_Group_Extra_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Group_Field )
   {
      p_id = c_field_id_Group_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Group_Name_Field )
   {
      p_id = c_field_id_Group_Name_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Hyperlink_Title_Field )
   {
      p_id = c_field_id_Hyperlink_Title_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Link_Class )
   {
      p_id = c_field_id_Link_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Link_Content_Field )
   {
      p_id = c_field_id_Link_Content_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Link_Group_Field )
   {
      p_id = c_field_id_Link_Group_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Link_Name_Field )
   {
      p_id = c_field_id_Link_Name_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Link_Page_Link_Child )
   {
      p_id = c_field_id_Link_Page_Link_Child;

      if( p_type_name )
         *p_type_name = "Meta_Relationship";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Meta_Content_Field )
   {
      p_id = c_field_id_Meta_Content_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Next_Child_Num_Field )
   {
      p_id = c_field_id_Next_Child_Num_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Order_Field )
   {
      p_id = c_field_id_Order_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Owned_Links_Field )
   {
      p_id = c_field_id_Owned_Links_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Page_Field )
   {
      p_id = c_field_id_Page_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Page_File_Class )
   {
      p_id = c_field_id_Page_File_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Page_File_Field )
   {
      p_id = c_field_id_Page_File_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Page_File_Name_Field )
   {
      p_id = c_field_id_Page_File_Name_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Page_File_Name_Gen_Field )
   {
      p_id = c_field_id_Page_File_Name_Gen_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Page_Link_Class )
   {
      p_id = c_field_id_Page_Link_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Page_Page_Link_Child )
   {
      p_id = c_field_id_Page_Page_Link_Child;

      if( p_type_name )
         *p_type_name = "Meta_Relationship";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Parent_Page_Field )
   {
      p_id = c_field_id_Parent_Page_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Sub_Title_Field )
   {
      p_id = c_field_id_Sub_Title_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Text_Content_Field )
   {
      p_id = c_field_id_Text_Content_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Title_Field )
   {
      p_id = c_field_id_Title_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Variable_Class )
   {
      p_id = c_field_id_Variable_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Variable_Name )
   {
      p_id = c_field_id_Variable_Name;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Variable_Value )
   {
      p_id = c_field_id_Variable_Value;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }

   return p_id;
}

const char* Meta_Specification_Content_Page::get_field_name(
 const string& id, bool* p_sql_numeric, string* p_type_name ) const
{
   const char* p_name( parent_class_type::get_field_name( id, p_sql_numeric, p_type_name ) );

   if( p_name )
      return p_name;

   if( id.empty( ) )
      throw runtime_error( "unexpected empty field id for get_field_name" );
   else if( id == c_field_id_Actions_Field )
   {
      p_name = c_field_name_Actions_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Child_Self_Relationship )
   {
      p_name = c_field_name_Child_Self_Relationship;

      if( p_type_name )
         *p_type_name = "Meta_Relationship";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Clone_Permitted_Field )
   {
      p_name = c_field_name_Clone_Permitted_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Code_Field )
   {
      p_name = c_field_name_Code_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Content_Type )
   {
      p_name = c_field_name_Content_Type;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Create_Copy_Output_Arg )
   {
      p_name = c_field_name_Create_Copy_Output_Arg;

      if( p_type_name )
         *p_type_name = "Meta_Procedure_Arg";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Create_Copy_Procedure )
   {
      p_name = c_field_name_Create_Copy_Procedure;

      if( p_type_name )
         *p_type_name = "Meta_Procedure";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_File_Name_Field )
   {
      p_name = c_field_name_File_Name_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_File_Name_Gen_Field )
   {
      p_name = c_field_name_File_Name_Gen_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Frag_Code_Field )
   {
      p_name = c_field_name_Frag_Code_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Frag_Content_Field )
   {
      p_name = c_field_name_Frag_Content_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Frag_Group_Field )
   {
      p_name = c_field_name_Frag_Group_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Fragment_Class )
   {
      p_name = c_field_name_Fragment_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Generate_Children_Field )
   {
      p_name = c_field_name_Generate_Children_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Generate_Output_Arg )
   {
      p_name = c_field_name_Generate_Output_Arg;

      if( p_type_name )
         *p_type_name = "Meta_Procedure_Arg";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Generate_Procedure )
   {
      p_name = c_field_name_Generate_Procedure;

      if( p_type_name )
         *p_type_name = "Meta_Procedure";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Group_Base_Path_Field )
   {
      p_name = c_field_name_Group_Base_Path_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Group_Class )
   {
      p_name = c_field_name_Group_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Group_Extra_Field )
   {
      p_name = c_field_name_Group_Extra_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Group_Field )
   {
      p_name = c_field_name_Group_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Group_Name_Field )
   {
      p_name = c_field_name_Group_Name_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Hyperlink_Title_Field )
   {
      p_name = c_field_name_Hyperlink_Title_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Link_Class )
   {
      p_name = c_field_name_Link_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Link_Content_Field )
   {
      p_name = c_field_name_Link_Content_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Link_Group_Field )
   {
      p_name = c_field_name_Link_Group_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Link_Name_Field )
   {
      p_name = c_field_name_Link_Name_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Link_Page_Link_Child )
   {
      p_name = c_field_name_Link_Page_Link_Child;

      if( p_type_name )
         *p_type_name = "Meta_Relationship";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Meta_Content_Field )
   {
      p_name = c_field_name_Meta_Content_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Next_Child_Num_Field )
   {
      p_name = c_field_name_Next_Child_Num_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Order_Field )
   {
      p_name = c_field_name_Order_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Owned_Links_Field )
   {
      p_name = c_field_name_Owned_Links_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Page_Field )
   {
      p_name = c_field_name_Page_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Page_File_Class )
   {
      p_name = c_field_name_Page_File_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Page_File_Field )
   {
      p_name = c_field_name_Page_File_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Page_File_Name_Field )
   {
      p_name = c_field_name_Page_File_Name_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Page_File_Name_Gen_Field )
   {
      p_name = c_field_name_Page_File_Name_Gen_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Page_Link_Class )
   {
      p_name = c_field_name_Page_Link_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Page_Page_Link_Child )
   {
      p_name = c_field_name_Page_Page_Link_Child;

      if( p_type_name )
         *p_type_name = "Meta_Relationship";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Parent_Page_Field )
   {
      p_name = c_field_name_Parent_Page_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Sub_Title_Field )
   {
      p_name = c_field_name_Sub_Title_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Text_Content_Field )
   {
      p_name = c_field_name_Text_Content_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Title_Field )
   {
      p_name = c_field_name_Title_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Variable_Class )
   {
      p_name = c_field_name_Variable_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Variable_Name )
   {
      p_name = c_field_name_Variable_Name;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Variable_Value )
   {
      p_name = c_field_name_Variable_Value;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }

   return p_name;
}

string Meta_Specification_Content_Page::get_field_display_name( const string& id ) const
{
   string display_name( parent_class_type::get_field_display_name( id ) );
   if( !display_name.empty( ) )
      return display_name;

   if( id.empty( ) )
      throw runtime_error( "unexpected empty field id for get_field_display_name" );
   else if( id == c_field_id_Actions_Field )
      display_name = get_module_string( c_field_display_name_Actions_Field );
   else if( id == c_field_id_Child_Self_Relationship )
      display_name = get_module_string( c_field_display_name_Child_Self_Relationship );
   else if( id == c_field_id_Clone_Permitted_Field )
      display_name = get_module_string( c_field_display_name_Clone_Permitted_Field );
   else if( id == c_field_id_Code_Field )
      display_name = get_module_string( c_field_display_name_Code_Field );
   else if( id == c_field_id_Content_Type )
      display_name = get_module_string( c_field_display_name_Content_Type );
   else if( id == c_field_id_Create_Copy_Output_Arg )
      display_name = get_module_string( c_field_display_name_Create_Copy_Output_Arg );
   else if( id == c_field_id_Create_Copy_Procedure )
      display_name = get_module_string( c_field_display_name_Create_Copy_Procedure );
   else if( id == c_field_id_File_Name_Field )
      display_name = get_module_string( c_field_display_name_File_Name_Field );
   else if( id == c_field_id_File_Name_Gen_Field )
      display_name = get_module_string( c_field_display_name_File_Name_Gen_Field );
   else if( id == c_field_id_Frag_Code_Field )
      display_name = get_module_string( c_field_display_name_Frag_Code_Field );
   else if( id == c_field_id_Frag_Content_Field )
      display_name = get_module_string( c_field_display_name_Frag_Content_Field );
   else if( id == c_field_id_Frag_Group_Field )
      display_name = get_module_string( c_field_display_name_Frag_Group_Field );
   else if( id == c_field_id_Fragment_Class )
      display_name = get_module_string( c_field_display_name_Fragment_Class );
   else if( id == c_field_id_Generate_Children_Field )
      display_name = get_module_string( c_field_display_name_Generate_Children_Field );
   else if( id == c_field_id_Generate_Output_Arg )
      display_name = get_module_string( c_field_display_name_Generate_Output_Arg );
   else if( id == c_field_id_Generate_Procedure )
      display_name = get_module_string( c_field_display_name_Generate_Procedure );
   else if( id == c_field_id_Group_Base_Path_Field )
      display_name = get_module_string( c_field_display_name_Group_Base_Path_Field );
   else if( id == c_field_id_Group_Class )
      display_name = get_module_string( c_field_display_name_Group_Class );
   else if( id == c_field_id_Group_Extra_Field )
      display_name = get_module_string( c_field_display_name_Group_Extra_Field );
   else if( id == c_field_id_Group_Field )
      display_name = get_module_string( c_field_display_name_Group_Field );
   else if( id == c_field_id_Group_Name_Field )
      display_name = get_module_string( c_field_display_name_Group_Name_Field );
   else if( id == c_field_id_Hyperlink_Title_Field )
      display_name = get_module_string( c_field_display_name_Hyperlink_Title_Field );
   else if( id == c_field_id_Link_Class )
      display_name = get_module_string( c_field_display_name_Link_Class );
   else if( id == c_field_id_Link_Content_Field )
      display_name = get_module_string( c_field_display_name_Link_Content_Field );
   else if( id == c_field_id_Link_Group_Field )
      display_name = get_module_string( c_field_display_name_Link_Group_Field );
   else if( id == c_field_id_Link_Name_Field )
      display_name = get_module_string( c_field_display_name_Link_Name_Field );
   else if( id == c_field_id_Link_Page_Link_Child )
      display_name = get_module_string( c_field_display_name_Link_Page_Link_Child );
   else if( id == c_field_id_Meta_Content_Field )
      display_name = get_module_string( c_field_display_name_Meta_Content_Field );
   else if( id == c_field_id_Next_Child_Num_Field )
      display_name = get_module_string( c_field_display_name_Next_Child_Num_Field );
   else if( id == c_field_id_Order_Field )
      display_name = get_module_string( c_field_display_name_Order_Field );
   else if( id == c_field_id_Owned_Links_Field )
      display_name = get_module_string( c_field_display_name_Owned_Links_Field );
   else if( id == c_field_id_Page_Field )
      display_name = get_module_string( c_field_display_name_Page_Field );
   else if( id == c_field_id_Page_File_Class )
      display_name = get_module_string( c_field_display_name_Page_File_Class );
   else if( id == c_field_id_Page_File_Field )
      display_name = get_module_string( c_field_display_name_Page_File_Field );
   else if( id == c_field_id_Page_File_Name_Field )
      display_name = get_module_string( c_field_display_name_Page_File_Name_Field );
   else if( id == c_field_id_Page_File_Name_Gen_Field )
      display_name = get_module_string( c_field_display_name_Page_File_Name_Gen_Field );
   else if( id == c_field_id_Page_Link_Class )
      display_name = get_module_string( c_field_display_name_Page_Link_Class );
   else if( id == c_field_id_Page_Page_Link_Child )
      display_name = get_module_string( c_field_display_name_Page_Page_Link_Child );
   else if( id == c_field_id_Parent_Page_Field )
      display_name = get_module_string( c_field_display_name_Parent_Page_Field );
   else if( id == c_field_id_Sub_Title_Field )
      display_name = get_module_string( c_field_display_name_Sub_Title_Field );
   else if( id == c_field_id_Text_Content_Field )
      display_name = get_module_string( c_field_display_name_Text_Content_Field );
   else if( id == c_field_id_Title_Field )
      display_name = get_module_string( c_field_display_name_Title_Field );
   else if( id == c_field_id_Variable_Class )
      display_name = get_module_string( c_field_display_name_Variable_Class );
   else if( id == c_field_id_Variable_Name )
      display_name = get_module_string( c_field_display_name_Variable_Name );
   else if( id == c_field_id_Variable_Value )
      display_name = get_module_string( c_field_display_name_Variable_Value );

   return display_name;
}

void Meta_Specification_Content_Page::clear_foreign_key( const string& field )
{
   if( has_field( field ) )
      p_impl->clear_foreign_key( field );
   else
      parent_class_type::clear_foreign_key( field );
}

void Meta_Specification_Content_Page::set_foreign_key_value( const string& field, const string& value )
{
   if( has_field( field ) )
      p_impl->set_foreign_key_value( field, value );
   else
      parent_class_type::set_foreign_key_value( field, value );
}

const string& Meta_Specification_Content_Page::get_foreign_key_value( const string& field )
{
   if( has_field( field ) )
      return p_impl->get_foreign_key_value( field );
   else
      return parent_class_type::get_foreign_key_value( field );
}

void Meta_Specification_Content_Page::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   parent_class_type::get_foreign_key_values( foreign_key_values );
   p_impl->get_foreign_key_values( foreign_key_values );
}

void Meta_Specification_Content_Page::setup_foreign_key( Meta_Field& o, const string& value )
{
   static_cast< Meta_Field& >( o ).set_key( value );
}

void Meta_Specification_Content_Page::setup_foreign_key( Meta_Relationship& o, const string& value )
{
   static_cast< Meta_Relationship& >( o ).set_key( value );
}

void Meta_Specification_Content_Page::setup_foreign_key( Meta_Procedure_Arg& o, const string& value )
{
   static_cast< Meta_Procedure_Arg& >( o ).set_key( value );
}

void Meta_Specification_Content_Page::setup_foreign_key( Meta_Procedure& o, const string& value )
{
   static_cast< Meta_Procedure& >( o ).set_key( value );
}

void Meta_Specification_Content_Page::setup_foreign_key( Meta_Class& o, const string& value )
{
   static_cast< Meta_Class& >( o ).set_key( value );
}

void Meta_Specification_Content_Page::setup_graph_parent(
 Meta_Field& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Field& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Field& >( o ).set_key( init_value );
}

void Meta_Specification_Content_Page::setup_graph_parent(
 Meta_Relationship& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Relationship& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Relationship& >( o ).set_key( init_value );
}

void Meta_Specification_Content_Page::setup_graph_parent(
 Meta_Procedure_Arg& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Procedure_Arg& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Procedure_Arg& >( o ).set_key( init_value );
}

void Meta_Specification_Content_Page::setup_graph_parent(
 Meta_Procedure& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Procedure& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Procedure& >( o ).set_key( init_value );
}

void Meta_Specification_Content_Page::setup_graph_parent(
 Meta_Class& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Class& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Class& >( o ).set_key( init_value );
}

size_t Meta_Specification_Content_Page::get_total_child_relationships( ) const
{
   return p_impl->total_child_relationships;
}

void Meta_Specification_Content_Page::set_total_child_relationships( size_t new_total_child_relationships ) const
{
   p_impl->total_child_relationships = new_total_child_relationships;
}

size_t Meta_Specification_Content_Page::get_num_foreign_key_children( bool is_internal ) const
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

class_base* Meta_Specification_Content_Page::get_next_foreign_key_child(
 size_t child_num, string& next_child_field, cascade_op op, bool is_internal )
{
   class_base* p_class_base = 0;

   p_class_base = Meta_Specification::get_next_foreign_key_child( child_num, next_child_field, op );

   return p_class_base;
}

const char* Meta_Specification_Content_Page::class_id( ) const
{
   return static_class_id( );
}

const char* Meta_Specification_Content_Page::class_name( ) const
{
   return static_class_name( );
}

const char* Meta_Specification_Content_Page::plural_name( ) const
{
   return static_plural_name( );
}

const char* Meta_Specification_Content_Page::module_id( ) const
{
   return static_module_id( );
}

const char* Meta_Specification_Content_Page::module_name( ) const
{
   return static_module_name( );
}

string Meta_Specification_Content_Page::get_display_name( bool plural ) const
{
   string key( plural ? "plural_" : "class_" );
   key += "specification_content_page";

   return get_module_string( key );
}

bool Meta_Specification_Content_Page::get_is_alias( ) const
{
   return false;
}

void Meta_Specification_Content_Page::get_alias_base_info( pair< string, string >& alias_base_info ) const
{
   ( void )alias_base_info;
}

void Meta_Specification_Content_Page::get_base_class_info( vector< pair< string, string > >& base_class_info ) const
{
   Meta_Specification::get_base_class_info( base_class_info );

   base_class_info.push_back( make_pair( Meta_Specification::class_id( ), Meta_Specification::class_name( ) ) );
}

class_base& Meta_Specification_Content_Page::get_or_create_graph_child( const string& context )
{
   class_base* p_class_base( 0 );

   string::size_type pos = context.find( '.' );
   string sub_context( context.substr( 0, pos ) );

   if( sub_context.empty( ) )
      throw runtime_error( "unexpected empty sub-context" );
   else if( sub_context == c_field_id_Actions_Field || sub_context == c_field_name_Actions_Field )
      p_class_base = &Actions_Field( );
   else if( sub_context == c_field_id_Child_Self_Relationship || sub_context == c_field_name_Child_Self_Relationship )
      p_class_base = &Child_Self_Relationship( );
   else if( sub_context == c_field_id_Clone_Permitted_Field || sub_context == c_field_name_Clone_Permitted_Field )
      p_class_base = &Clone_Permitted_Field( );
   else if( sub_context == c_field_id_Code_Field || sub_context == c_field_name_Code_Field )
      p_class_base = &Code_Field( );
   else if( sub_context == c_field_id_Content_Type || sub_context == c_field_name_Content_Type )
      p_class_base = &Content_Type( );
   else if( sub_context == c_field_id_Create_Copy_Output_Arg || sub_context == c_field_name_Create_Copy_Output_Arg )
      p_class_base = &Create_Copy_Output_Arg( );
   else if( sub_context == c_field_id_Create_Copy_Procedure || sub_context == c_field_name_Create_Copy_Procedure )
      p_class_base = &Create_Copy_Procedure( );
   else if( sub_context == c_field_id_File_Name_Field || sub_context == c_field_name_File_Name_Field )
      p_class_base = &File_Name_Field( );
   else if( sub_context == c_field_id_File_Name_Gen_Field || sub_context == c_field_name_File_Name_Gen_Field )
      p_class_base = &File_Name_Gen_Field( );
   else if( sub_context == c_field_id_Frag_Code_Field || sub_context == c_field_name_Frag_Code_Field )
      p_class_base = &Frag_Code_Field( );
   else if( sub_context == c_field_id_Frag_Content_Field || sub_context == c_field_name_Frag_Content_Field )
      p_class_base = &Frag_Content_Field( );
   else if( sub_context == c_field_id_Frag_Group_Field || sub_context == c_field_name_Frag_Group_Field )
      p_class_base = &Frag_Group_Field( );
   else if( sub_context == c_field_id_Fragment_Class || sub_context == c_field_name_Fragment_Class )
      p_class_base = &Fragment_Class( );
   else if( sub_context == c_field_id_Generate_Children_Field || sub_context == c_field_name_Generate_Children_Field )
      p_class_base = &Generate_Children_Field( );
   else if( sub_context == c_field_id_Generate_Output_Arg || sub_context == c_field_name_Generate_Output_Arg )
      p_class_base = &Generate_Output_Arg( );
   else if( sub_context == c_field_id_Generate_Procedure || sub_context == c_field_name_Generate_Procedure )
      p_class_base = &Generate_Procedure( );
   else if( sub_context == c_field_id_Group_Base_Path_Field || sub_context == c_field_name_Group_Base_Path_Field )
      p_class_base = &Group_Base_Path_Field( );
   else if( sub_context == c_field_id_Group_Class || sub_context == c_field_name_Group_Class )
      p_class_base = &Group_Class( );
   else if( sub_context == c_field_id_Group_Extra_Field || sub_context == c_field_name_Group_Extra_Field )
      p_class_base = &Group_Extra_Field( );
   else if( sub_context == c_field_id_Group_Field || sub_context == c_field_name_Group_Field )
      p_class_base = &Group_Field( );
   else if( sub_context == c_field_id_Group_Name_Field || sub_context == c_field_name_Group_Name_Field )
      p_class_base = &Group_Name_Field( );
   else if( sub_context == c_field_id_Hyperlink_Title_Field || sub_context == c_field_name_Hyperlink_Title_Field )
      p_class_base = &Hyperlink_Title_Field( );
   else if( sub_context == c_field_id_Link_Class || sub_context == c_field_name_Link_Class )
      p_class_base = &Link_Class( );
   else if( sub_context == c_field_id_Link_Content_Field || sub_context == c_field_name_Link_Content_Field )
      p_class_base = &Link_Content_Field( );
   else if( sub_context == c_field_id_Link_Group_Field || sub_context == c_field_name_Link_Group_Field )
      p_class_base = &Link_Group_Field( );
   else if( sub_context == c_field_id_Link_Name_Field || sub_context == c_field_name_Link_Name_Field )
      p_class_base = &Link_Name_Field( );
   else if( sub_context == c_field_id_Link_Page_Link_Child || sub_context == c_field_name_Link_Page_Link_Child )
      p_class_base = &Link_Page_Link_Child( );
   else if( sub_context == c_field_id_Meta_Content_Field || sub_context == c_field_name_Meta_Content_Field )
      p_class_base = &Meta_Content_Field( );
   else if( sub_context == c_field_id_Next_Child_Num_Field || sub_context == c_field_name_Next_Child_Num_Field )
      p_class_base = &Next_Child_Num_Field( );
   else if( sub_context == c_field_id_Order_Field || sub_context == c_field_name_Order_Field )
      p_class_base = &Order_Field( );
   else if( sub_context == c_field_id_Owned_Links_Field || sub_context == c_field_name_Owned_Links_Field )
      p_class_base = &Owned_Links_Field( );
   else if( sub_context == c_field_id_Page_Field || sub_context == c_field_name_Page_Field )
      p_class_base = &Page_Field( );
   else if( sub_context == c_field_id_Page_File_Class || sub_context == c_field_name_Page_File_Class )
      p_class_base = &Page_File_Class( );
   else if( sub_context == c_field_id_Page_File_Field || sub_context == c_field_name_Page_File_Field )
      p_class_base = &Page_File_Field( );
   else if( sub_context == c_field_id_Page_File_Name_Field || sub_context == c_field_name_Page_File_Name_Field )
      p_class_base = &Page_File_Name_Field( );
   else if( sub_context == c_field_id_Page_File_Name_Gen_Field || sub_context == c_field_name_Page_File_Name_Gen_Field )
      p_class_base = &Page_File_Name_Gen_Field( );
   else if( sub_context == c_field_id_Page_Link_Class || sub_context == c_field_name_Page_Link_Class )
      p_class_base = &Page_Link_Class( );
   else if( sub_context == c_field_id_Page_Page_Link_Child || sub_context == c_field_name_Page_Page_Link_Child )
      p_class_base = &Page_Page_Link_Child( );
   else if( sub_context == c_field_id_Parent_Page_Field || sub_context == c_field_name_Parent_Page_Field )
      p_class_base = &Parent_Page_Field( );
   else if( sub_context == c_field_id_Sub_Title_Field || sub_context == c_field_name_Sub_Title_Field )
      p_class_base = &Sub_Title_Field( );
   else if( sub_context == c_field_id_Text_Content_Field || sub_context == c_field_name_Text_Content_Field )
      p_class_base = &Text_Content_Field( );
   else if( sub_context == c_field_id_Title_Field || sub_context == c_field_name_Title_Field )
      p_class_base = &Title_Field( );
   else if( sub_context == c_field_id_Variable_Class || sub_context == c_field_name_Variable_Class )
      p_class_base = &Variable_Class( );
   else if( sub_context == c_field_id_Variable_Name || sub_context == c_field_name_Variable_Name )
      p_class_base = &Variable_Name( );
   else if( sub_context == c_field_id_Variable_Value || sub_context == c_field_name_Variable_Value )
      p_class_base = &Variable_Value( );   else
      return parent_class_type::get_or_create_graph_child( context );

   if( !p_class_base )
      throw runtime_error( "unknown sub-context '" + sub_context + "'" );

   if( pos != string::npos )
      p_class_base = &p_class_base->get_or_create_graph_child( context.substr( pos + 1 ) );

   return *p_class_base;
}

void Meta_Specification_Content_Page::get_sql_column_names(
 vector< string >& names, bool* p_done, const string* p_class_name ) const
{
   parent_class_type::get_sql_column_names( names, p_done, p_class_name );

   if( p_done && *p_done )
      return;

   names.push_back( "C_Actions_Field" );
   names.push_back( "C_Child_Self_Relationship" );
   names.push_back( "C_Clone_Permitted_Field" );
   names.push_back( "C_Code_Field" );
   names.push_back( "C_Content_Type" );
   names.push_back( "C_Create_Copy_Output_Arg" );
   names.push_back( "C_Create_Copy_Procedure" );
   names.push_back( "C_File_Name_Field" );
   names.push_back( "C_File_Name_Gen_Field" );
   names.push_back( "C_Frag_Code_Field" );
   names.push_back( "C_Frag_Content_Field" );
   names.push_back( "C_Frag_Group_Field" );
   names.push_back( "C_Fragment_Class" );
   names.push_back( "C_Generate_Children_Field" );
   names.push_back( "C_Generate_Output_Arg" );
   names.push_back( "C_Generate_Procedure" );
   names.push_back( "C_Group_Base_Path_Field" );
   names.push_back( "C_Group_Class" );
   names.push_back( "C_Group_Extra_Field" );
   names.push_back( "C_Group_Field" );
   names.push_back( "C_Group_Name_Field" );
   names.push_back( "C_Hyperlink_Title_Field" );
   names.push_back( "C_Link_Class" );
   names.push_back( "C_Link_Content_Field" );
   names.push_back( "C_Link_Group_Field" );
   names.push_back( "C_Link_Name_Field" );
   names.push_back( "C_Link_Page_Link_Child" );
   names.push_back( "C_Meta_Content_Field" );
   names.push_back( "C_Next_Child_Num_Field" );
   names.push_back( "C_Order_Field" );
   names.push_back( "C_Owned_Links_Field" );
   names.push_back( "C_Page_Field" );
   names.push_back( "C_Page_File_Class" );
   names.push_back( "C_Page_File_Field" );
   names.push_back( "C_Page_File_Name_Field" );
   names.push_back( "C_Page_File_Name_Gen_Field" );
   names.push_back( "C_Page_Link_Class" );
   names.push_back( "C_Page_Page_Link_Child" );
   names.push_back( "C_Parent_Page_Field" );
   names.push_back( "C_Sub_Title_Field" );
   names.push_back( "C_Text_Content_Field" );
   names.push_back( "C_Title_Field" );
   names.push_back( "C_Variable_Class" );
   names.push_back( "C_Variable_Name" );
   names.push_back( "C_Variable_Value" );

   if( p_done && p_class_name && *p_class_name == static_class_name( ) )
      *p_done = true;
}

void Meta_Specification_Content_Page::get_sql_column_values(
 vector< string >& values, bool* p_done, const string* p_class_name ) const
{
   parent_class_type::get_sql_column_values( values, p_done, p_class_name );

   if( p_done && *p_done )
      return;

   values.push_back( sql_quote( to_string( Actions_Field( ) ) ) );
   values.push_back( sql_quote( to_string( Child_Self_Relationship( ) ) ) );
   values.push_back( sql_quote( to_string( Clone_Permitted_Field( ) ) ) );
   values.push_back( sql_quote( to_string( Code_Field( ) ) ) );
   values.push_back( sql_quote( to_string( Content_Type( ) ) ) );
   values.push_back( sql_quote( to_string( Create_Copy_Output_Arg( ) ) ) );
   values.push_back( sql_quote( to_string( Create_Copy_Procedure( ) ) ) );
   values.push_back( sql_quote( to_string( File_Name_Field( ) ) ) );
   values.push_back( sql_quote( to_string( File_Name_Gen_Field( ) ) ) );
   values.push_back( sql_quote( to_string( Frag_Code_Field( ) ) ) );
   values.push_back( sql_quote( to_string( Frag_Content_Field( ) ) ) );
   values.push_back( sql_quote( to_string( Frag_Group_Field( ) ) ) );
   values.push_back( sql_quote( to_string( Fragment_Class( ) ) ) );
   values.push_back( sql_quote( to_string( Generate_Children_Field( ) ) ) );
   values.push_back( sql_quote( to_string( Generate_Output_Arg( ) ) ) );
   values.push_back( sql_quote( to_string( Generate_Procedure( ) ) ) );
   values.push_back( sql_quote( to_string( Group_Base_Path_Field( ) ) ) );
   values.push_back( sql_quote( to_string( Group_Class( ) ) ) );
   values.push_back( sql_quote( to_string( Group_Extra_Field( ) ) ) );
   values.push_back( sql_quote( to_string( Group_Field( ) ) ) );
   values.push_back( sql_quote( to_string( Group_Name_Field( ) ) ) );
   values.push_back( sql_quote( to_string( Hyperlink_Title_Field( ) ) ) );
   values.push_back( sql_quote( to_string( Link_Class( ) ) ) );
   values.push_back( sql_quote( to_string( Link_Content_Field( ) ) ) );
   values.push_back( sql_quote( to_string( Link_Group_Field( ) ) ) );
   values.push_back( sql_quote( to_string( Link_Name_Field( ) ) ) );
   values.push_back( sql_quote( to_string( Link_Page_Link_Child( ) ) ) );
   values.push_back( sql_quote( to_string( Meta_Content_Field( ) ) ) );
   values.push_back( sql_quote( to_string( Next_Child_Num_Field( ) ) ) );
   values.push_back( sql_quote( to_string( Order_Field( ) ) ) );
   values.push_back( sql_quote( to_string( Owned_Links_Field( ) ) ) );
   values.push_back( sql_quote( to_string( Page_Field( ) ) ) );
   values.push_back( sql_quote( to_string( Page_File_Class( ) ) ) );
   values.push_back( sql_quote( to_string( Page_File_Field( ) ) ) );
   values.push_back( sql_quote( to_string( Page_File_Name_Field( ) ) ) );
   values.push_back( sql_quote( to_string( Page_File_Name_Gen_Field( ) ) ) );
   values.push_back( sql_quote( to_string( Page_Link_Class( ) ) ) );
   values.push_back( sql_quote( to_string( Page_Page_Link_Child( ) ) ) );
   values.push_back( sql_quote( to_string( Parent_Page_Field( ) ) ) );
   values.push_back( sql_quote( to_string( Sub_Title_Field( ) ) ) );
   values.push_back( sql_quote( to_string( Text_Content_Field( ) ) ) );
   values.push_back( sql_quote( to_string( Title_Field( ) ) ) );
   values.push_back( sql_quote( to_string( Variable_Class( ) ) ) );
   values.push_back( sql_quote( to_string( Variable_Name( ) ) ) );
   values.push_back( sql_quote( to_string( Variable_Value( ) ) ) );

   if( p_done && p_class_name && *p_class_name == static_class_name( ) )
      *p_done = true;
}

void Meta_Specification_Content_Page::get_required_field_names(
 set< string >& names, bool required_transients, set< string >* p_dependents ) const
{
   set< string > local_dependents;

   parent_class_type::get_required_field_names( names,
    required_transients, p_dependents ? p_dependents : &local_dependents );

   set< string >& dependents( p_dependents ? *p_dependents : local_dependents );

   get_always_required_field_names( names, required_transients, dependents );

   // [(start field_from_search_replace)]
   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Code_Field" );

      if( ( required_transients && is_field_transient( e_field_id_Code_Field ) )
       || ( !required_transients && !is_field_transient( e_field_id_Code_Field ) ) )
         names.insert( "Code_Field" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Group_Field" );

      if( ( required_transients && is_field_transient( e_field_id_Group_Field ) )
       || ( !required_transients && !is_field_transient( e_field_id_Group_Field ) ) )
         names.insert( "Group_Field" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Title_Field" );

      if( ( required_transients && is_field_transient( e_field_id_Title_Field ) )
       || ( !required_transients && !is_field_transient( e_field_id_Title_Field ) ) )
         names.insert( "Title_Field" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "File_Name_Field" );

      if( ( required_transients && is_field_transient( e_field_id_File_Name_Field ) )
       || ( !required_transients && !is_field_transient( e_field_id_File_Name_Field ) ) )
         names.insert( "File_Name_Field" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "File_Name_Gen_Field" );

      if( ( required_transients && is_field_transient( e_field_id_File_Name_Gen_Field ) )
       || ( !required_transients && !is_field_transient( e_field_id_File_Name_Gen_Field ) ) )
         names.insert( "File_Name_Gen_Field" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Sub_Title_Field" );

      if( ( required_transients && is_field_transient( e_field_id_Sub_Title_Field ) )
       || ( !required_transients && !is_field_transient( e_field_id_Sub_Title_Field ) ) )
         names.insert( "Sub_Title_Field" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Parent_Page_Field" );

      if( ( required_transients && is_field_transient( e_field_id_Parent_Page_Field ) )
       || ( !required_transients && !is_field_transient( e_field_id_Parent_Page_Field ) ) )
         names.insert( "Parent_Page_Field" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Content_Type" );

      if( ( required_transients && is_field_transient( e_field_id_Content_Type ) )
       || ( !required_transients && !is_field_transient( e_field_id_Content_Type ) ) )
         names.insert( "Content_Type" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Next_Child_Num_Field" );

      if( ( required_transients && is_field_transient( e_field_id_Next_Child_Num_Field ) )
       || ( !required_transients && !is_field_transient( e_field_id_Next_Child_Num_Field ) ) )
         names.insert( "Next_Child_Num_Field" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Text_Content_Field" );

      if( ( required_transients && is_field_transient( e_field_id_Text_Content_Field ) )
       || ( !required_transients && !is_field_transient( e_field_id_Text_Content_Field ) ) )
         names.insert( "Text_Content_Field" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Meta_Content_Field" );

      if( ( required_transients && is_field_transient( e_field_id_Meta_Content_Field ) )
       || ( !required_transients && !is_field_transient( e_field_id_Meta_Content_Field ) ) )
         names.insert( "Meta_Content_Field" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Link_Content_Field" );

      if( ( required_transients && is_field_transient( e_field_id_Link_Content_Field ) )
       || ( !required_transients && !is_field_transient( e_field_id_Link_Content_Field ) ) )
         names.insert( "Link_Content_Field" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Actions_Field" );

      if( ( required_transients && is_field_transient( e_field_id_Actions_Field ) )
       || ( !required_transients && !is_field_transient( e_field_id_Actions_Field ) ) )
         names.insert( "Actions_Field" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Hyperlink_Title_Field" );

      if( ( required_transients && is_field_transient( e_field_id_Hyperlink_Title_Field ) )
       || ( !required_transients && !is_field_transient( e_field_id_Hyperlink_Title_Field ) ) )
         names.insert( "Hyperlink_Title_Field" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Generate_Children_Field" );

      if( ( required_transients && is_field_transient( e_field_id_Generate_Children_Field ) )
       || ( !required_transients && !is_field_transient( e_field_id_Generate_Children_Field ) ) )
         names.insert( "Generate_Children_Field" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Clone_Permitted_Field" );

      if( ( required_transients && is_field_transient( e_field_id_Clone_Permitted_Field ) )
       || ( !required_transients && !is_field_transient( e_field_id_Clone_Permitted_Field ) ) )
         names.insert( "Clone_Permitted_Field" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Child_Self_Relationship" );

      if( ( required_transients && is_field_transient( e_field_id_Child_Self_Relationship ) )
       || ( !required_transients && !is_field_transient( e_field_id_Child_Self_Relationship ) ) )
         names.insert( "Child_Self_Relationship" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Generate_Procedure" );

      if( ( required_transients && is_field_transient( e_field_id_Generate_Procedure ) )
       || ( !required_transients && !is_field_transient( e_field_id_Generate_Procedure ) ) )
         names.insert( "Generate_Procedure" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Generate_Output_Arg" );

      if( ( required_transients && is_field_transient( e_field_id_Generate_Output_Arg ) )
       || ( !required_transients && !is_field_transient( e_field_id_Generate_Output_Arg ) ) )
         names.insert( "Generate_Output_Arg" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Create_Copy_Procedure" );

      if( ( required_transients && is_field_transient( e_field_id_Create_Copy_Procedure ) )
       || ( !required_transients && !is_field_transient( e_field_id_Create_Copy_Procedure ) ) )
         names.insert( "Create_Copy_Procedure" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Create_Copy_Procedure" );

      if( ( required_transients && is_field_transient( e_field_id_Create_Copy_Procedure ) )
       || ( !required_transients && !is_field_transient( e_field_id_Create_Copy_Procedure ) ) )
         names.insert( "Create_Copy_Procedure" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Create_Copy_Output_Arg" );

      if( ( required_transients && is_field_transient( e_field_id_Create_Copy_Output_Arg ) )
       || ( !required_transients && !is_field_transient( e_field_id_Create_Copy_Output_Arg ) ) )
         names.insert( "Create_Copy_Output_Arg" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Page_Link_Class" );

      if( ( required_transients && is_field_transient( e_field_id_Page_Link_Class ) )
       || ( !required_transients && !is_field_transient( e_field_id_Page_Link_Class ) ) )
         names.insert( "Page_Link_Class" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Order_Field" );

      if( ( required_transients && is_field_transient( e_field_id_Order_Field ) )
       || ( !required_transients && !is_field_transient( e_field_id_Order_Field ) ) )
         names.insert( "Order_Field" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Page_Field" );

      if( ( required_transients && is_field_transient( e_field_id_Page_Field ) )
       || ( !required_transients && !is_field_transient( e_field_id_Page_Field ) ) )
         names.insert( "Page_Field" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Page_Page_Link_Child" );

      if( ( required_transients && is_field_transient( e_field_id_Page_Page_Link_Child ) )
       || ( !required_transients && !is_field_transient( e_field_id_Page_Page_Link_Child ) ) )
         names.insert( "Page_Page_Link_Child" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Link_Page_Link_Child" );

      if( ( required_transients && is_field_transient( e_field_id_Link_Page_Link_Child ) )
       || ( !required_transients && !is_field_transient( e_field_id_Link_Page_Link_Child ) ) )
         names.insert( "Link_Page_Link_Child" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Page_File_Class" );

      if( ( required_transients && is_field_transient( e_field_id_Page_File_Class ) )
       || ( !required_transients && !is_field_transient( e_field_id_Page_File_Class ) ) )
         names.insert( "Page_File_Class" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Page_File_Field" );

      if( ( required_transients && is_field_transient( e_field_id_Page_File_Field ) )
       || ( !required_transients && !is_field_transient( e_field_id_Page_File_Field ) ) )
         names.insert( "Page_File_Field" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Page_File_Name_Field" );

      if( ( required_transients && is_field_transient( e_field_id_Page_File_Name_Field ) )
       || ( !required_transients && !is_field_transient( e_field_id_Page_File_Name_Field ) ) )
         names.insert( "Page_File_Name_Field" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Page_File_Name_Gen_Field" );

      if( ( required_transients && is_field_transient( e_field_id_Page_File_Name_Gen_Field ) )
       || ( !required_transients && !is_field_transient( e_field_id_Page_File_Name_Gen_Field ) ) )
         names.insert( "Page_File_Name_Gen_Field" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Fragment_Class" );

      if( ( required_transients && is_field_transient( e_field_id_Fragment_Class ) )
       || ( !required_transients && !is_field_transient( e_field_id_Fragment_Class ) ) )
         names.insert( "Fragment_Class" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Frag_Code_Field" );

      if( ( required_transients && is_field_transient( e_field_id_Frag_Code_Field ) )
       || ( !required_transients && !is_field_transient( e_field_id_Frag_Code_Field ) ) )
         names.insert( "Frag_Code_Field" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Variable_Class" );

      if( ( required_transients && is_field_transient( e_field_id_Variable_Class ) )
       || ( !required_transients && !is_field_transient( e_field_id_Variable_Class ) ) )
         names.insert( "Variable_Class" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Variable_Name" );

      if( ( required_transients && is_field_transient( e_field_id_Variable_Name ) )
       || ( !required_transients && !is_field_transient( e_field_id_Variable_Name ) ) )
         names.insert( "Variable_Name" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Variable_Value" );

      if( ( required_transients && is_field_transient( e_field_id_Variable_Value ) )
       || ( !required_transients && !is_field_transient( e_field_id_Variable_Value ) ) )
         names.insert( "Variable_Value" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Link_Class" );

      if( ( required_transients && is_field_transient( e_field_id_Link_Class ) )
       || ( !required_transients && !is_field_transient( e_field_id_Link_Class ) ) )
         names.insert( "Link_Class" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Link_Name_Field" );

      if( ( required_transients && is_field_transient( e_field_id_Link_Name_Field ) )
       || ( !required_transients && !is_field_transient( e_field_id_Link_Name_Field ) ) )
         names.insert( "Link_Name_Field" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Link_Group_Field" );

      if( ( required_transients && is_field_transient( e_field_id_Link_Group_Field ) )
       || ( !required_transients && !is_field_transient( e_field_id_Link_Group_Field ) ) )
         names.insert( "Link_Group_Field" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Owned_Links_Field" );

      if( ( required_transients && is_field_transient( e_field_id_Owned_Links_Field ) )
       || ( !required_transients && !is_field_transient( e_field_id_Owned_Links_Field ) ) )
         names.insert( "Owned_Links_Field" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Frag_Group_Field" );

      if( ( required_transients && is_field_transient( e_field_id_Frag_Group_Field ) )
       || ( !required_transients && !is_field_transient( e_field_id_Frag_Group_Field ) ) )
         names.insert( "Frag_Group_Field" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Frag_Content_Field" );

      if( ( required_transients && is_field_transient( e_field_id_Frag_Content_Field ) )
       || ( !required_transients && !is_field_transient( e_field_id_Frag_Content_Field ) ) )
         names.insert( "Frag_Content_Field" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Group_Class" );

      if( ( required_transients && is_field_transient( e_field_id_Group_Class ) )
       || ( !required_transients && !is_field_transient( e_field_id_Group_Class ) ) )
         names.insert( "Group_Class" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Group_Name_Field" );

      if( ( required_transients && is_field_transient( e_field_id_Group_Name_Field ) )
       || ( !required_transients && !is_field_transient( e_field_id_Group_Name_Field ) ) )
         names.insert( "Group_Name_Field" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Group_Extra_Field" );

      if( ( required_transients && is_field_transient( e_field_id_Group_Extra_Field ) )
       || ( !required_transients && !is_field_transient( e_field_id_Group_Extra_Field ) ) )
         names.insert( "Group_Extra_Field" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Group_Base_Path_Field" );

      if( ( required_transients && is_field_transient( e_field_id_Group_Base_Path_Field ) )
       || ( !required_transients && !is_field_transient( e_field_id_Group_Base_Path_Field ) ) )
         names.insert( "Group_Base_Path_Field" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Specification_Type" );

      if( ( required_transients && is_field_transient( e_field_id_Specification_Type ) )
       || ( !required_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
         names.insert( "Specification_Type" );
   }
   // [(finish field_from_search_replace)]

   // [<start get_required_field_names>]
   // [<finish get_required_field_names>]
}

void Meta_Specification_Content_Page::get_always_required_field_names(
 set< string >& names, bool required_transients, set< string >& dependents ) const
{

   parent_class_type::get_always_required_field_names( names, required_transients, dependents );

   // [<start get_always_required_field_names>]
   // [<finish get_always_required_field_names>]
}

void Meta_Specification_Content_Page::get_transient_replacement_field_names( const string& name, vector< string >& names ) const
{

   parent_class_type::get_transient_replacement_field_names( name, names );

   // [<start get_transient_replacement_field_names>]
   // [<finish get_transient_replacement_field_names>]
}

void Meta_Specification_Content_Page::do_generate_sql( generate_sql_type type, vector< string >& sql_stmts ) const
{
   parent_class_type::do_generate_sql( type, sql_stmts );

   generate_sql( static_class_name( ), type, sql_stmts );
}

const char* Meta_Specification_Content_Page::static_resolved_module_id( )
{
   return static_module_id( );
}

const char* Meta_Specification_Content_Page::static_resolved_module_name( )
{
   return static_module_name( );
}

const char* Meta_Specification_Content_Page::static_lock_class_id( )
{
   return parent_class_type::static_lock_class_id( );
}

const char* Meta_Specification_Content_Page::static_check_class_name( )
{
   return parent_class_type::static_check_class_name( );
}

bool Meta_Specification_Content_Page::static_has_derivations( )
{
   return !g_derivations.empty( );
}

void Meta_Specification_Content_Page::static_get_class_info( class_info_container& class_info )
{
   parent_class_type::static_get_class_info( class_info );
   class_info.push_back( "100.131100" );
}

void Meta_Specification_Content_Page::static_get_field_info( field_info_container& all_field_info )
{
   parent_class_type::static_get_field_info( all_field_info );
   all_field_info.push_back( field_info( "302629", "Actions_Field", "Meta_Field", false ) );
   all_field_info.push_back( field_info( "302620", "Child_Self_Relationship", "Meta_Relationship", true ) );
   all_field_info.push_back( field_info( "302607", "Clone_Permitted_Field", "Meta_Field", false ) );
   all_field_info.push_back( field_info( "302610", "Code_Field", "Meta_Field", true ) );
   all_field_info.push_back( field_info( "302605", "Content_Type", "Meta_Field", false ) );
   all_field_info.push_back( field_info( "302644", "Create_Copy_Output_Arg", "Meta_Procedure_Arg", false ) );
   all_field_info.push_back( field_info( "302628", "Create_Copy_Procedure", "Meta_Procedure", false ) );
   all_field_info.push_back( field_info( "302612", "File_Name_Field", "Meta_Field", true ) );
   all_field_info.push_back( field_info( "302608", "File_Name_Gen_Field", "Meta_Field", false ) );
   all_field_info.push_back( field_info( "302640", "Frag_Code_Field", "Meta_Field", true ) );
   all_field_info.push_back( field_info( "302642", "Frag_Content_Field", "Meta_Field", true ) );
   all_field_info.push_back( field_info( "302641", "Frag_Group_Field", "Meta_Field", true ) );
   all_field_info.push_back( field_info( "302602", "Fragment_Class", "Meta_Class", true ) );
   all_field_info.push_back( field_info( "302619", "Generate_Children_Field", "Meta_Field", true ) );
   all_field_info.push_back( field_info( "302626", "Generate_Output_Arg", "Meta_Procedure_Arg", true ) );
   all_field_info.push_back( field_info( "302625", "Generate_Procedure", "Meta_Procedure", true ) );
   all_field_info.push_back( field_info( "302647", "Group_Base_Path_Field", "Meta_Field", true ) );
   all_field_info.push_back( field_info( "302603", "Group_Class", "Meta_Class", true ) );
   all_field_info.push_back( field_info( "302646", "Group_Extra_Field", "Meta_Field", true ) );
   all_field_info.push_back( field_info( "302609", "Group_Field", "Meta_Field", false ) );
   all_field_info.push_back( field_info( "302645", "Group_Name_Field", "Meta_Field", true ) );
   all_field_info.push_back( field_info( "302618", "Hyperlink_Title_Field", "Meta_Field", true ) );
   all_field_info.push_back( field_info( "302604", "Link_Class", "Meta_Class", true ) );
   all_field_info.push_back( field_info( "302617", "Link_Content_Field", "Meta_Field", true ) );
   all_field_info.push_back( field_info( "302622", "Link_Group_Field", "Meta_Field", false ) );
   all_field_info.push_back( field_info( "302621", "Link_Name_Field", "Meta_Field", false ) );
   all_field_info.push_back( field_info( "302632", "Link_Page_Link_Child", "Meta_Relationship", true ) );
   all_field_info.push_back( field_info( "302616", "Meta_Content_Field", "Meta_Field", true ) );
   all_field_info.push_back( field_info( "302643", "Next_Child_Num_Field", "Meta_Field", false ) );
   all_field_info.push_back( field_info( "302630", "Order_Field", "Meta_Field", true ) );
   all_field_info.push_back( field_info( "302623", "Owned_Links_Field", "Meta_Field", false ) );
   all_field_info.push_back( field_info( "302631", "Page_Field", "Meta_Field", true ) );
   all_field_info.push_back( field_info( "302601", "Page_File_Class", "Meta_Class", true ) );
   all_field_info.push_back( field_info( "302635", "Page_File_Field", "Meta_Field", true ) );
   all_field_info.push_back( field_info( "302636", "Page_File_Name_Field", "Meta_Field", true ) );
   all_field_info.push_back( field_info( "302606", "Page_File_Name_Gen_Field", "Meta_Field", false ) );
   all_field_info.push_back( field_info( "302600", "Page_Link_Class", "Meta_Class", true ) );
   all_field_info.push_back( field_info( "302627", "Page_Page_Link_Child", "Meta_Relationship", false ) );
   all_field_info.push_back( field_info( "302614", "Parent_Page_Field", "Meta_Field", true ) );
   all_field_info.push_back( field_info( "302613", "Sub_Title_Field", "Meta_Field", true ) );
   all_field_info.push_back( field_info( "302615", "Text_Content_Field", "Meta_Field", true ) );
   all_field_info.push_back( field_info( "302611", "Title_Field", "Meta_Field", true ) );
   all_field_info.push_back( field_info( "302637", "Variable_Class", "Meta_Class", false ) );
   all_field_info.push_back( field_info( "302638", "Variable_Name", "Meta_Field", false ) );
   all_field_info.push_back( field_info( "302639", "Variable_Value", "Meta_Field", false ) );
}

void Meta_Specification_Content_Page::static_get_foreign_key_info( foreign_key_info_container& foreign_key_info )
{
   ( void )foreign_key_info;

   parent_class_type::static_get_foreign_key_info( foreign_key_info );

   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Actions_Field, make_pair( "Meta.131100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Child_Self_Relationship, make_pair( "Meta.131100", "Meta_Relationship" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Clone_Permitted_Field, make_pair( "Meta.131100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Code_Field, make_pair( "Meta.131100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Content_Type, make_pair( "Meta.131100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Create_Copy_Output_Arg, make_pair( "Meta.131100", "Meta_Procedure_Arg" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Create_Copy_Procedure, make_pair( "Meta.131100", "Meta_Procedure" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_File_Name_Field, make_pair( "Meta.131100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_File_Name_Gen_Field, make_pair( "Meta.131100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Frag_Code_Field, make_pair( "Meta.131100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Frag_Content_Field, make_pair( "Meta.131100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Frag_Group_Field, make_pair( "Meta.131100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Fragment_Class, make_pair( "Meta.131100", "Meta_Class" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Generate_Children_Field, make_pair( "Meta.131100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Generate_Output_Arg, make_pair( "Meta.131100", "Meta_Procedure_Arg" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Generate_Procedure, make_pair( "Meta.131100", "Meta_Procedure" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Group_Base_Path_Field, make_pair( "Meta.131100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Group_Class, make_pair( "Meta.131100", "Meta_Class" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Group_Extra_Field, make_pair( "Meta.131100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Group_Field, make_pair( "Meta.131100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Group_Name_Field, make_pair( "Meta.131100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Hyperlink_Title_Field, make_pair( "Meta.131100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Link_Class, make_pair( "Meta.131100", "Meta_Class" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Link_Content_Field, make_pair( "Meta.131100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Link_Group_Field, make_pair( "Meta.131100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Link_Name_Field, make_pair( "Meta.131100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Link_Page_Link_Child, make_pair( "Meta.131100", "Meta_Relationship" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Meta_Content_Field, make_pair( "Meta.131100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Next_Child_Num_Field, make_pair( "Meta.131100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Order_Field, make_pair( "Meta.131100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Owned_Links_Field, make_pair( "Meta.131100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Page_Field, make_pair( "Meta.131100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Page_File_Class, make_pair( "Meta.131100", "Meta_Class" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Page_File_Field, make_pair( "Meta.131100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Page_File_Name_Field, make_pair( "Meta.131100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Page_File_Name_Gen_Field, make_pair( "Meta.131100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Page_Link_Class, make_pair( "Meta.131100", "Meta_Class" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Page_Page_Link_Child, make_pair( "Meta.131100", "Meta_Relationship" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Parent_Page_Field, make_pair( "Meta.131100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Sub_Title_Field, make_pair( "Meta.131100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Text_Content_Field, make_pair( "Meta.131100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Title_Field, make_pair( "Meta.131100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Variable_Class, make_pair( "Meta.131100", "Meta_Class" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Variable_Name, make_pair( "Meta.131100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Variable_Value, make_pair( "Meta.131100", "Meta_Field" ) ) );
}

int Meta_Specification_Content_Page::static_get_num_fields( bool* p_done, const string* p_class_name )
{
   int num_fields( parent_class_type::static_get_num_fields( p_done, p_class_name ) );

   if( p_done && *p_done )
      return num_fields;

   num_fields += c_num_fields;

   if( p_done && p_class_name && *p_class_name == static_class_name( ) )
      *p_done = true;

   return num_fields;
}

bool Meta_Specification_Content_Page::static_is_field_transient( field_id id )
{
   return is_transient_field( static_get_field_id( id ) );
}

const char* Meta_Specification_Content_Page::static_get_field_id( field_id id )
{
   const char* p_id = 0;

   int num_parent_fields( parent_class_type::static_get_num_fields( ) );

   if( ( int )id < num_parent_fields )
      p_id = parent_class_type::static_get_field_id( ( parent_class_type::field_id )id );

   switch( id )
   {
      case 1:
      p_id = "302629";
      break;

      case 2:
      p_id = "302620";
      break;

      case 3:
      p_id = "302607";
      break;

      case 4:
      p_id = "302610";
      break;

      case 5:
      p_id = "302605";
      break;

      case 6:
      p_id = "302644";
      break;

      case 7:
      p_id = "302628";
      break;

      case 8:
      p_id = "302612";
      break;

      case 9:
      p_id = "302608";
      break;

      case 10:
      p_id = "302640";
      break;

      case 11:
      p_id = "302642";
      break;

      case 12:
      p_id = "302641";
      break;

      case 13:
      p_id = "302602";
      break;

      case 14:
      p_id = "302619";
      break;

      case 15:
      p_id = "302626";
      break;

      case 16:
      p_id = "302625";
      break;

      case 17:
      p_id = "302647";
      break;

      case 18:
      p_id = "302603";
      break;

      case 19:
      p_id = "302646";
      break;

      case 20:
      p_id = "302609";
      break;

      case 21:
      p_id = "302645";
      break;

      case 22:
      p_id = "302618";
      break;

      case 23:
      p_id = "302604";
      break;

      case 24:
      p_id = "302617";
      break;

      case 25:
      p_id = "302622";
      break;

      case 26:
      p_id = "302621";
      break;

      case 27:
      p_id = "302632";
      break;

      case 28:
      p_id = "302616";
      break;

      case 29:
      p_id = "302643";
      break;

      case 30:
      p_id = "302630";
      break;

      case 31:
      p_id = "302623";
      break;

      case 32:
      p_id = "302631";
      break;

      case 33:
      p_id = "302601";
      break;

      case 34:
      p_id = "302635";
      break;

      case 35:
      p_id = "302636";
      break;

      case 36:
      p_id = "302606";
      break;

      case 37:
      p_id = "302600";
      break;

      case 38:
      p_id = "302627";
      break;

      case 39:
      p_id = "302614";
      break;

      case 40:
      p_id = "302613";
      break;

      case 41:
      p_id = "302615";
      break;

      case 42:
      p_id = "302611";
      break;

      case 43:
      p_id = "302637";
      break;

      case 44:
      p_id = "302638";
      break;

      case 45:
      p_id = "302639";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Specification_Content_Page" );

   return p_id;
}

const char* Meta_Specification_Content_Page::static_get_field_name( field_id id )
{
   const char* p_id = 0;

   int num_parent_fields( parent_class_type::static_get_num_fields( ) );

   if( ( int )id < num_parent_fields )
      p_id = parent_class_type::static_get_field_id( ( parent_class_type::field_id )id );

   switch( id )
   {
      case 1:
      p_id = "Actions_Field";
      break;

      case 2:
      p_id = "Child_Self_Relationship";
      break;

      case 3:
      p_id = "Clone_Permitted_Field";
      break;

      case 4:
      p_id = "Code_Field";
      break;

      case 5:
      p_id = "Content_Type";
      break;

      case 6:
      p_id = "Create_Copy_Output_Arg";
      break;

      case 7:
      p_id = "Create_Copy_Procedure";
      break;

      case 8:
      p_id = "File_Name_Field";
      break;

      case 9:
      p_id = "File_Name_Gen_Field";
      break;

      case 10:
      p_id = "Frag_Code_Field";
      break;

      case 11:
      p_id = "Frag_Content_Field";
      break;

      case 12:
      p_id = "Frag_Group_Field";
      break;

      case 13:
      p_id = "Fragment_Class";
      break;

      case 14:
      p_id = "Generate_Children_Field";
      break;

      case 15:
      p_id = "Generate_Output_Arg";
      break;

      case 16:
      p_id = "Generate_Procedure";
      break;

      case 17:
      p_id = "Group_Base_Path_Field";
      break;

      case 18:
      p_id = "Group_Class";
      break;

      case 19:
      p_id = "Group_Extra_Field";
      break;

      case 20:
      p_id = "Group_Field";
      break;

      case 21:
      p_id = "Group_Name_Field";
      break;

      case 22:
      p_id = "Hyperlink_Title_Field";
      break;

      case 23:
      p_id = "Link_Class";
      break;

      case 24:
      p_id = "Link_Content_Field";
      break;

      case 25:
      p_id = "Link_Group_Field";
      break;

      case 26:
      p_id = "Link_Name_Field";
      break;

      case 27:
      p_id = "Link_Page_Link_Child";
      break;

      case 28:
      p_id = "Meta_Content_Field";
      break;

      case 29:
      p_id = "Next_Child_Num_Field";
      break;

      case 30:
      p_id = "Order_Field";
      break;

      case 31:
      p_id = "Owned_Links_Field";
      break;

      case 32:
      p_id = "Page_Field";
      break;

      case 33:
      p_id = "Page_File_Class";
      break;

      case 34:
      p_id = "Page_File_Field";
      break;

      case 35:
      p_id = "Page_File_Name_Field";
      break;

      case 36:
      p_id = "Page_File_Name_Gen_Field";
      break;

      case 37:
      p_id = "Page_Link_Class";
      break;

      case 38:
      p_id = "Page_Page_Link_Child";
      break;

      case 39:
      p_id = "Parent_Page_Field";
      break;

      case 40:
      p_id = "Sub_Title_Field";
      break;

      case 41:
      p_id = "Text_Content_Field";
      break;

      case 42:
      p_id = "Title_Field";
      break;

      case 43:
      p_id = "Variable_Class";
      break;

      case 44:
      p_id = "Variable_Name";
      break;

      case 45:
      p_id = "Variable_Value";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Specification_Content_Page" );

   return p_id;
}

int Meta_Specification_Content_Page::static_get_field_num( const string& field )
{
   int rc = parent_class_type::static_get_field_num( field );
   if( rc >= 0 )
      return rc;

   rc = parent_class_type::static_get_num_fields( );

   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for static_get_field_num( )" );
   else if( field == c_field_id_Actions_Field || field == c_field_name_Actions_Field )
      rc += 1;
   else if( field == c_field_id_Child_Self_Relationship || field == c_field_name_Child_Self_Relationship )
      rc += 2;
   else if( field == c_field_id_Clone_Permitted_Field || field == c_field_name_Clone_Permitted_Field )
      rc += 3;
   else if( field == c_field_id_Code_Field || field == c_field_name_Code_Field )
      rc += 4;
   else if( field == c_field_id_Content_Type || field == c_field_name_Content_Type )
      rc += 5;
   else if( field == c_field_id_Create_Copy_Output_Arg || field == c_field_name_Create_Copy_Output_Arg )
      rc += 6;
   else if( field == c_field_id_Create_Copy_Procedure || field == c_field_name_Create_Copy_Procedure )
      rc += 7;
   else if( field == c_field_id_File_Name_Field || field == c_field_name_File_Name_Field )
      rc += 8;
   else if( field == c_field_id_File_Name_Gen_Field || field == c_field_name_File_Name_Gen_Field )
      rc += 9;
   else if( field == c_field_id_Frag_Code_Field || field == c_field_name_Frag_Code_Field )
      rc += 10;
   else if( field == c_field_id_Frag_Content_Field || field == c_field_name_Frag_Content_Field )
      rc += 11;
   else if( field == c_field_id_Frag_Group_Field || field == c_field_name_Frag_Group_Field )
      rc += 12;
   else if( field == c_field_id_Fragment_Class || field == c_field_name_Fragment_Class )
      rc += 13;
   else if( field == c_field_id_Generate_Children_Field || field == c_field_name_Generate_Children_Field )
      rc += 14;
   else if( field == c_field_id_Generate_Output_Arg || field == c_field_name_Generate_Output_Arg )
      rc += 15;
   else if( field == c_field_id_Generate_Procedure || field == c_field_name_Generate_Procedure )
      rc += 16;
   else if( field == c_field_id_Group_Base_Path_Field || field == c_field_name_Group_Base_Path_Field )
      rc += 17;
   else if( field == c_field_id_Group_Class || field == c_field_name_Group_Class )
      rc += 18;
   else if( field == c_field_id_Group_Extra_Field || field == c_field_name_Group_Extra_Field )
      rc += 19;
   else if( field == c_field_id_Group_Field || field == c_field_name_Group_Field )
      rc += 20;
   else if( field == c_field_id_Group_Name_Field || field == c_field_name_Group_Name_Field )
      rc += 21;
   else if( field == c_field_id_Hyperlink_Title_Field || field == c_field_name_Hyperlink_Title_Field )
      rc += 22;
   else if( field == c_field_id_Link_Class || field == c_field_name_Link_Class )
      rc += 23;
   else if( field == c_field_id_Link_Content_Field || field == c_field_name_Link_Content_Field )
      rc += 24;
   else if( field == c_field_id_Link_Group_Field || field == c_field_name_Link_Group_Field )
      rc += 25;
   else if( field == c_field_id_Link_Name_Field || field == c_field_name_Link_Name_Field )
      rc += 26;
   else if( field == c_field_id_Link_Page_Link_Child || field == c_field_name_Link_Page_Link_Child )
      rc += 27;
   else if( field == c_field_id_Meta_Content_Field || field == c_field_name_Meta_Content_Field )
      rc += 28;
   else if( field == c_field_id_Next_Child_Num_Field || field == c_field_name_Next_Child_Num_Field )
      rc += 29;
   else if( field == c_field_id_Order_Field || field == c_field_name_Order_Field )
      rc += 30;
   else if( field == c_field_id_Owned_Links_Field || field == c_field_name_Owned_Links_Field )
      rc += 31;
   else if( field == c_field_id_Page_Field || field == c_field_name_Page_Field )
      rc += 32;
   else if( field == c_field_id_Page_File_Class || field == c_field_name_Page_File_Class )
      rc += 33;
   else if( field == c_field_id_Page_File_Field || field == c_field_name_Page_File_Field )
      rc += 34;
   else if( field == c_field_id_Page_File_Name_Field || field == c_field_name_Page_File_Name_Field )
      rc += 35;
   else if( field == c_field_id_Page_File_Name_Gen_Field || field == c_field_name_Page_File_Name_Gen_Field )
      rc += 36;
   else if( field == c_field_id_Page_Link_Class || field == c_field_name_Page_Link_Class )
      rc += 37;
   else if( field == c_field_id_Page_Page_Link_Child || field == c_field_name_Page_Page_Link_Child )
      rc += 38;
   else if( field == c_field_id_Parent_Page_Field || field == c_field_name_Parent_Page_Field )
      rc += 39;
   else if( field == c_field_id_Sub_Title_Field || field == c_field_name_Sub_Title_Field )
      rc += 40;
   else if( field == c_field_id_Text_Content_Field || field == c_field_name_Text_Content_Field )
      rc += 41;
   else if( field == c_field_id_Title_Field || field == c_field_name_Title_Field )
      rc += 42;
   else if( field == c_field_id_Variable_Class || field == c_field_name_Variable_Class )
      rc += 43;
   else if( field == c_field_id_Variable_Name || field == c_field_name_Variable_Name )
      rc += 44;
   else if( field == c_field_id_Variable_Value || field == c_field_name_Variable_Value )
      rc += 45;

   return rc - 1;
}

procedure_info_container& Meta_Specification_Content_Page::static_get_procedure_info( )
{
   static procedure_info_container& procedures( parent_class_type::static_get_procedure_info( ) );

   return procedures;
}

string Meta_Specification_Content_Page::static_get_sql_columns( )
{
   string sql_columns;

   sql_columns += 
    "C_Key_ VARCHAR(64),"
    "C_Ver_ INTEGER NOT NULL,"
    "C_Rev_ INTEGER NOT NULL,"
    "C_Typ_ VARCHAR(24) NOT NULL,"
    "C_Actions VARCHAR(128) NOT NULL,"
    "C_Child_Rel_Child_Class VARCHAR(64) NOT NULL,"
    "C_Child_Rel_Field_Key VARCHAR(128) NOT NULL,"
    "C_Child_Relationship VARCHAR(64) NOT NULL,"
    "C_Child_Specification_Type VARCHAR(64) NOT NULL,"
    "C_Class VARCHAR(64) NOT NULL,"
    "C_Comments VARCHAR(128) NOT NULL,"
    "C_Enum VARCHAR(64) NOT NULL,"
    "C_Enum_Item VARCHAR(64) NOT NULL,"
    "C_Enum_Item_2 VARCHAR(64) NOT NULL,"
    "C_Enum_Item_3 VARCHAR(64) NOT NULL,"
    "C_Enum_Item_4 VARCHAR(64) NOT NULL,"
    "C_Enum_Item_5 VARCHAR(64) NOT NULL,"
    "C_Field VARCHAR(64) NOT NULL,"
    "C_Field_Class VARCHAR(64) NOT NULL,"
    "C_Field_Pairs TEXT NOT NULL,"
    "C_Field_Values TEXT NOT NULL,"
    "C_Fields TEXT NOT NULL,"
    "C_Id VARCHAR(128) NOT NULL,"
    "C_Is_Required_For_UI_Gen INTEGER NOT NULL,"
    "C_Model VARCHAR(64) NOT NULL,"
    "C_Modifier VARCHAR(64) NOT NULL,"
    "C_Name VARCHAR(128) NOT NULL,"
    "C_Options VARCHAR(128) NOT NULL,"
    "C_Order VARCHAR(128) NOT NULL,"
    "C_Other_Class VARCHAR(64) NOT NULL,"
    "C_Other_Field VARCHAR(64) NOT NULL,"
    "C_Other_Field_2 VARCHAR(64) NOT NULL,"
    "C_Other_Procedure VARCHAR(64) NOT NULL,"
    "C_Other_Procedure_2 VARCHAR(64) NOT NULL,"
    "C_Parent_Specification VARCHAR(64) NOT NULL,"
    "C_Parent_Specification_Copy_Child_Links VARCHAR(64) NOT NULL,"
    "C_Permission VARCHAR(64) NOT NULL,"
    "C_Procedure VARCHAR(64) NOT NULL,"
    "C_Procedure_Arg VARCHAR(64) NOT NULL,"
    "C_Procedure_Arg_2 VARCHAR(64) NOT NULL,"
    "C_Procedure_Arg_3 VARCHAR(64) NOT NULL,"
    "C_Protect_Child_Rel INTEGER NOT NULL,"
    "C_Protect_Procedure INTEGER NOT NULL,"
    "C_Protect_Source_Parent INTEGER NOT NULL,"
    "C_Restrict_Values VARCHAR(128) NOT NULL,"
    "C_Source_Child VARCHAR(64) NOT NULL,"
    "C_Source_Child_2 VARCHAR(64) NOT NULL,"
    "C_Source_Child_Class VARCHAR(64) NOT NULL,"
    "C_Source_Class VARCHAR(64) NOT NULL,"
    "C_Source_Field VARCHAR(64) NOT NULL,"
    "C_Source_Field_Class VARCHAR(64) NOT NULL,"
    "C_Source_Grandchild VARCHAR(64) NOT NULL,"
    "C_Source_Parent VARCHAR(64) NOT NULL,"
    "C_Source_Parent_Class VARCHAR(64) NOT NULL,"
    "C_Specification_Type VARCHAR(64) NOT NULL,"
    "C_Test_Child VARCHAR(64) NOT NULL,"
    "C_Test_Field VARCHAR(64) NOT NULL,"
    "C_Test_Field_Class VARCHAR(64) NOT NULL,"
    "C_Test_Parent VARCHAR(64) NOT NULL,"
    "C_Test_Parent_Class VARCHAR(64) NOT NULL,"
    "C_Test_Value VARCHAR(128) NOT NULL,"
    "C_Use_Source_Parent INTEGER NOT NULL,"
    "C_Use_Test_Parent_Child INTEGER NOT NULL,"
    "C_Value VARCHAR(128) NOT NULL,"
    "C_Value_Left_Part VARCHAR(128) NOT NULL,"
    "C_Value_Literal VARCHAR(128) NOT NULL,"
    "C_Value_Numeric_String VARCHAR(128) NOT NULL,"
    "C_Value_Right_Part VARCHAR(128) NOT NULL,"
    "C_Value_String VARCHAR(128) NOT NULL,"
    "C_Actions_Field VARCHAR(64) NOT NULL,"
    "C_Child_Self_Relationship VARCHAR(64) NOT NULL,"
    "C_Clone_Permitted_Field VARCHAR(64) NOT NULL,"
    "C_Code_Field VARCHAR(64) NOT NULL,"
    "C_Content_Type VARCHAR(64) NOT NULL,"
    "C_Create_Copy_Output_Arg VARCHAR(64) NOT NULL,"
    "C_Create_Copy_Procedure VARCHAR(64) NOT NULL,"
    "C_File_Name_Field VARCHAR(64) NOT NULL,"
    "C_File_Name_Gen_Field VARCHAR(64) NOT NULL,"
    "C_Frag_Code_Field VARCHAR(64) NOT NULL,"
    "C_Frag_Content_Field VARCHAR(64) NOT NULL,"
    "C_Frag_Group_Field VARCHAR(64) NOT NULL,"
    "C_Fragment_Class VARCHAR(64) NOT NULL,"
    "C_Generate_Children_Field VARCHAR(64) NOT NULL,"
    "C_Generate_Output_Arg VARCHAR(64) NOT NULL,"
    "C_Generate_Procedure VARCHAR(64) NOT NULL,"
    "C_Group_Base_Path_Field VARCHAR(64) NOT NULL,"
    "C_Group_Class VARCHAR(64) NOT NULL,"
    "C_Group_Extra_Field VARCHAR(64) NOT NULL,"
    "C_Group_Field VARCHAR(64) NOT NULL,"
    "C_Group_Name_Field VARCHAR(64) NOT NULL,"
    "C_Hyperlink_Title_Field VARCHAR(64) NOT NULL,"
    "C_Link_Class VARCHAR(64) NOT NULL,"
    "C_Link_Content_Field VARCHAR(64) NOT NULL,"
    "C_Link_Group_Field VARCHAR(64) NOT NULL,"
    "C_Link_Name_Field VARCHAR(64) NOT NULL,"
    "C_Link_Page_Link_Child VARCHAR(64) NOT NULL,"
    "C_Meta_Content_Field VARCHAR(64) NOT NULL,"
    "C_Next_Child_Num_Field VARCHAR(64) NOT NULL,"
    "C_Order_Field VARCHAR(64) NOT NULL,"
    "C_Owned_Links_Field VARCHAR(64) NOT NULL,"
    "C_Page_Field VARCHAR(64) NOT NULL,"
    "C_Page_File_Class VARCHAR(64) NOT NULL,"
    "C_Page_File_Field VARCHAR(64) NOT NULL,"
    "C_Page_File_Name_Field VARCHAR(64) NOT NULL,"
    "C_Page_File_Name_Gen_Field VARCHAR(64) NOT NULL,"
    "C_Page_Link_Class VARCHAR(64) NOT NULL,"
    "C_Page_Page_Link_Child VARCHAR(64) NOT NULL,"
    "C_Parent_Page_Field VARCHAR(64) NOT NULL,"
    "C_Sub_Title_Field VARCHAR(64) NOT NULL,"
    "C_Text_Content_Field VARCHAR(64) NOT NULL,"
    "C_Title_Field VARCHAR(64) NOT NULL,"
    "C_Variable_Class VARCHAR(64) NOT NULL,"
    "C_Variable_Name VARCHAR(64) NOT NULL,"
    "C_Variable_Value VARCHAR(64) NOT NULL,"
    "PRIMARY KEY(C_Key_)";

   return sql_columns;
}

void Meta_Specification_Content_Page::static_get_text_search_fields( vector< string >& fields )
{
   parent_class_type::static_get_text_search_fields( fields );
}

void Meta_Specification_Content_Page::static_get_all_enum_pairs( vector< pair< string, string > >& pairs )
{
   parent_class_type::static_get_all_enum_pairs( pairs );
}

void Meta_Specification_Content_Page::static_get_sql_indexes( vector< string >& indexes )
{
   parent_class_type::static_get_sql_indexes( indexes );
}

void Meta_Specification_Content_Page::static_get_sql_unique_indexes( vector< string >& indexes )
{
   parent_class_type::static_get_sql_unique_indexes( indexes );
}

void Meta_Specification_Content_Page::static_insert_derivation( const string& module_and_class_id )
{
   g_derivations.insert( module_and_class_id );
}

void Meta_Specification_Content_Page::static_remove_derivation( const string& module_and_class_id )
{
   if( g_derivations.count( module_and_class_id ) )
      g_derivations.erase( module_and_class_id );
}

void Meta_Specification_Content_Page::static_insert_external_alias( const string& module_and_class_id, Meta_Specification_Content_Page* p_instance )
{
   g_external_aliases.insert( external_aliases_value_type( module_and_class_id, p_instance ) );
}

void Meta_Specification_Content_Page::static_remove_external_alias( const string& module_and_class_id )
{
   if( g_external_aliases.count( module_and_class_id ) )
   {
      delete g_external_aliases[ module_and_class_id ];
      g_external_aliases.erase( module_and_class_id );
   }
}

void Meta_Specification_Content_Page::static_class_init( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for init" );

   Meta_Specification::static_insert_derivation( "Meta_Specification_Content_Page" );

   // [<start static_class_init>]
   // [<finish static_class_init>]
}

void Meta_Specification_Content_Page::static_class_term( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for term" );

   Meta_Specification::static_remove_derivation( "Meta_Specification_Content_Page" );

   // [<start static_class_term>]
   // [<finish static_class_term>]
}

