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

#include "Meta_Specification_Type.h"

#include "Meta_Specification.h"
#include "Meta_Specification_Type.h"

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

template< > inline string to_string( const Meta_Specification_Type& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

inline void from_string( Meta_Specification_Type& c, const string& s )
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

#include "Meta_Specification_Type.cmh"

const int32_t c_version = 1;

const char* const c_field_id_Allow_Child_Relationship = "114191";
const char* const c_field_id_Allow_Enum = "114198";
const char* const c_field_id_Allow_Enum_Item = "114199";
const char* const c_field_id_Allow_FK_Source_Field_Choice = "114123";
const char* const c_field_id_Allow_FK_Test_Field_Choice = "114142";
const char* const c_field_id_Allow_Field = "114124";
const char* const c_field_id_Allow_Modifier = "114197";
const char* const c_field_id_Allow_Options = "114117";
const char* const c_field_id_Allow_Other_Class = "114192";
const char* const c_field_id_Allow_Other_Class_Field = "114212";
const char* const c_field_id_Allow_Other_Field = "114174";
const char* const c_field_id_Allow_Other_Field_2 = "114183";
const char* const c_field_id_Allow_Other_Modifier = "114216";
const char* const c_field_id_Allow_Other_Modifier_2 = "114217";
const char* const c_field_id_Allow_Other_Procedure = "114208";
const char* const c_field_id_Allow_Other_Procedure_2 = "114209";
const char* const c_field_id_Allow_Other_Source_Child = "114176";
const char* const c_field_id_Allow_Other_Source_Child_2 = "114205";
const char* const c_field_id_Allow_Permission = "114136";
const char* const c_field_id_Allow_Procedure = "114171";
const char* const c_field_id_Allow_Procedure_Arg = "114204";
const char* const c_field_id_Allow_Source_Child = "114121";
const char* const c_field_id_Allow_Source_Class = "114172";
const char* const c_field_id_Allow_Source_Field = "114128";
const char* const c_field_id_Allow_Source_Grandchild = "114178";
const char* const c_field_id_Allow_Source_Parent = "114125";
const char* const c_field_id_Allow_Test_Field = "114126";
const char* const c_field_id_Allow_Test_Value = "114115";
const char* const c_field_id_Allow_Value = "114135";
const char* const c_field_id_Can_Access_Below_Model = "114218";
const char* const c_field_id_Child_Prefix = "114164";
const char* const c_field_id_Child_Rel_Grandparent_Match = "114157";
const char* const c_field_id_Child_Rel_Parent_Match = "114156";
const char* const c_field_id_Child_Relationship_Class_Match = "114155";
const char* const c_field_id_Child_Specification_Type = "301710";
const char* const c_field_id_Default_Child_Vars = "114165";
const char* const c_field_id_Field_type = "114111";
const char* const c_field_id_Has_Next_Specification_Info = "114149";
const char* const c_field_id_Is_Child_Only = "114151";
const char* const c_field_id_Is_Required_For_UI_Gen = "114163";
const char* const c_field_id_Is_System = "114147";
const char* const c_field_id_Name = "114101";
const char* const c_field_id_Needs_Child_Relationship = "114138";
const char* const c_field_id_Needs_Class = "114104";
const char* const c_field_id_Needs_Enum = "114108";
const char* const c_field_id_Needs_Enum_Item = "114109";
const char* const c_field_id_Needs_Enum_Item_2 = "114182";
const char* const c_field_id_Needs_Enum_Item_3 = "114185";
const char* const c_field_id_Needs_Enum_Item_4 = "114186";
const char* const c_field_id_Needs_Enum_Item_5 = "114189";
const char* const c_field_id_Needs_Field = "114105";
const char* const c_field_id_Needs_Modifier = "114107";
const char* const c_field_id_Needs_Other_Class = "114193";
const char* const c_field_id_Needs_Other_Class_Field = "114213";
const char* const c_field_id_Needs_Other_Field = "114175";
const char* const c_field_id_Needs_Other_Field_2 = "114184";
const char* const c_field_id_Needs_Other_Modifier = "114202";
const char* const c_field_id_Needs_Other_Modifier_2 = "114203";
const char* const c_field_id_Needs_Other_Permission = "114200";
const char* const c_field_id_Needs_Other_Permission_2 = "114201";
const char* const c_field_id_Needs_Other_Procedure = "114187";
const char* const c_field_id_Needs_Other_Procedure_2 = "114188";
const char* const c_field_id_Needs_Other_Source_Child = "114177";
const char* const c_field_id_Needs_Other_Source_Child_2 = "114206";
const char* const c_field_id_Needs_Permission = "114180";
const char* const c_field_id_Needs_Procedure = "114106";
const char* const c_field_id_Needs_Procedure_Arg = "114132";
const char* const c_field_id_Needs_Procedure_Arg_2 = "114194";
const char* const c_field_id_Needs_Procedure_Arg_3 = "114195";
const char* const c_field_id_Needs_Source_Child = "114122";
const char* const c_field_id_Needs_Source_Field = "114113";
const char* const c_field_id_Needs_Source_Field_Or_Child = "114154";
const char* const c_field_id_Needs_Source_Parent = "114112";
const char* const c_field_id_Needs_Test_Field = "114114";
const char* const c_field_id_Needs_Test_Value = "114116";
const char* const c_field_id_Needs_Value = "114110";
const char* const c_field_id_Next_Child_Specification_Type = "301720";
const char* const c_field_id_Next_Protect_Child_Rel = "114169";
const char* const c_field_id_Next_Protect_Procedure = "114161";
const char* const c_field_id_Next_Protect_Source_Parent = "114153";
const char* const c_field_id_Next_Specification_Actions = "114150";
const char* const c_field_id_Notes = "114120";
const char* const c_field_id_Parent_Specification_Type = "301700";
const char* const c_field_id_Procedure_Arg_primitive = "114133";
const char* const c_field_id_Procedure_Arg_type = "114134";
const char* const c_field_id_Protect_Child_Rel_From_Update = "114145";
const char* const c_field_id_Protect_Class_From_Edit = "114146";
const char* const c_field_id_Protect_Class_From_Update = "114141";
const char* const c_field_id_Protect_Other_Class_From_Edit = "114214";
const char* const c_field_id_Protect_Procedure_From_Edit = "114160";
const char* const c_field_id_Protect_Source_Parent = "114137";
const char* const c_field_id_Set_Field_To_Source_Field_Or_Child = "114179";
const char* const c_field_id_Source_Field_Needs_Test_Field = "114129";
const char* const c_field_id_Source_Field_Type_Match = "114118";
const char* const c_field_id_Source_Parent_type = "114119";
const char* const c_field_id_Source_type = "114130";
const char* const c_field_id_Specification_Actions = "114140";
const char* const c_field_id_Specification_Name = "114102";
const char* const c_field_id_Specification_Object = "114103";
const char* const c_field_id_Specification_Strings = "114166";
const char* const c_field_id_Specification_Vars = "114158";
const char* const c_field_id_Test_Field_Type_Match = "114127";
const char* const c_field_id_Test_Field_type = "114131";
const char* const c_field_id_Use_Class_As_Source_Parent_Class = "114181";
const char* const c_field_id_Use_Field_Enum = "114162";
const char* const c_field_id_Use_Parent_Child_Rel_As_Source_Parent = "114168";
const char* const c_field_id_Use_Parent_Child_Rel_For_Class = "114148";
const char* const c_field_id_Use_Parent_Child_Rel_For_Other_Class = "114210";
const char* const c_field_id_Use_Parent_Class = "114139";
const char* const c_field_id_Use_Parent_Field_For_Class = "114167";
const char* const c_field_id_Use_Parent_Other_Class = "114211";
const char* const c_field_id_Use_Parent_Other_Class_For_Class = "114196";
const char* const c_field_id_Use_Parent_Procedure = "114159";
const char* const c_field_id_Use_Parent_Source_Class = "114173";
const char* const c_field_id_Use_Parent_Source_Field_For_Class = "114190";
const char* const c_field_id_Use_Parent_Source_Parent = "114144";
const char* const c_field_id_Use_Parent_Source_Parent_For_Class = "114152";
const char* const c_field_id_Use_Source_Field_Enum = "114207";
const char* const c_field_id_Use_Source_Field_Or_Child_Enum = "114215";
const char* const c_field_id_View_Id = "114170";

const char* const c_field_name_Allow_Child_Relationship = "Allow_Child_Relationship";
const char* const c_field_name_Allow_Enum = "Allow_Enum";
const char* const c_field_name_Allow_Enum_Item = "Allow_Enum_Item";
const char* const c_field_name_Allow_FK_Source_Field_Choice = "Allow_FK_Source_Field_Choice";
const char* const c_field_name_Allow_FK_Test_Field_Choice = "Allow_FK_Test_Field_Choice";
const char* const c_field_name_Allow_Field = "Allow_Field";
const char* const c_field_name_Allow_Modifier = "Allow_Modifier";
const char* const c_field_name_Allow_Options = "Allow_Options";
const char* const c_field_name_Allow_Other_Class = "Allow_Other_Class";
const char* const c_field_name_Allow_Other_Class_Field = "Allow_Other_Class_Field";
const char* const c_field_name_Allow_Other_Field = "Allow_Other_Field";
const char* const c_field_name_Allow_Other_Field_2 = "Allow_Other_Field_2";
const char* const c_field_name_Allow_Other_Modifier = "Allow_Other_Modifier";
const char* const c_field_name_Allow_Other_Modifier_2 = "Allow_Other_Modifier_2";
const char* const c_field_name_Allow_Other_Procedure = "Allow_Other_Procedure";
const char* const c_field_name_Allow_Other_Procedure_2 = "Allow_Other_Procedure_2";
const char* const c_field_name_Allow_Other_Source_Child = "Allow_Other_Source_Child";
const char* const c_field_name_Allow_Other_Source_Child_2 = "Allow_Other_Source_Child_2";
const char* const c_field_name_Allow_Permission = "Allow_Permission";
const char* const c_field_name_Allow_Procedure = "Allow_Procedure";
const char* const c_field_name_Allow_Procedure_Arg = "Allow_Procedure_Arg";
const char* const c_field_name_Allow_Source_Child = "Allow_Source_Child";
const char* const c_field_name_Allow_Source_Class = "Allow_Source_Class";
const char* const c_field_name_Allow_Source_Field = "Allow_Source_Field";
const char* const c_field_name_Allow_Source_Grandchild = "Allow_Source_Grandchild";
const char* const c_field_name_Allow_Source_Parent = "Allow_Source_Parent";
const char* const c_field_name_Allow_Test_Field = "Allow_Test_Field";
const char* const c_field_name_Allow_Test_Value = "Allow_Test_Value";
const char* const c_field_name_Allow_Value = "Allow_Value";
const char* const c_field_name_Can_Access_Below_Model = "Can_Access_Below_Model";
const char* const c_field_name_Child_Prefix = "Child_Prefix";
const char* const c_field_name_Child_Rel_Grandparent_Match = "Child_Rel_Grandparent_Match";
const char* const c_field_name_Child_Rel_Parent_Match = "Child_Rel_Parent_Match";
const char* const c_field_name_Child_Relationship_Class_Match = "Child_Relationship_Class_Match";
const char* const c_field_name_Child_Specification_Type = "Child_Specification_Type";
const char* const c_field_name_Default_Child_Vars = "Default_Child_Vars";
const char* const c_field_name_Field_type = "Field_type";
const char* const c_field_name_Has_Next_Specification_Info = "Has_Next_Specification_Info";
const char* const c_field_name_Is_Child_Only = "Is_Child_Only";
const char* const c_field_name_Is_Required_For_UI_Gen = "Is_Required_For_UI_Gen";
const char* const c_field_name_Is_System = "Is_System";
const char* const c_field_name_Name = "Name";
const char* const c_field_name_Needs_Child_Relationship = "Needs_Child_Relationship";
const char* const c_field_name_Needs_Class = "Needs_Class";
const char* const c_field_name_Needs_Enum = "Needs_Enum";
const char* const c_field_name_Needs_Enum_Item = "Needs_Enum_Item";
const char* const c_field_name_Needs_Enum_Item_2 = "Needs_Enum_Item_2";
const char* const c_field_name_Needs_Enum_Item_3 = "Needs_Enum_Item_3";
const char* const c_field_name_Needs_Enum_Item_4 = "Needs_Enum_Item_4";
const char* const c_field_name_Needs_Enum_Item_5 = "Needs_Enum_Item_5";
const char* const c_field_name_Needs_Field = "Needs_Field";
const char* const c_field_name_Needs_Modifier = "Needs_Modifier";
const char* const c_field_name_Needs_Other_Class = "Needs_Other_Class";
const char* const c_field_name_Needs_Other_Class_Field = "Needs_Other_Class_Field";
const char* const c_field_name_Needs_Other_Field = "Needs_Other_Field";
const char* const c_field_name_Needs_Other_Field_2 = "Needs_Other_Field_2";
const char* const c_field_name_Needs_Other_Modifier = "Needs_Other_Modifier";
const char* const c_field_name_Needs_Other_Modifier_2 = "Needs_Other_Modifier_2";
const char* const c_field_name_Needs_Other_Permission = "Needs_Other_Permission";
const char* const c_field_name_Needs_Other_Permission_2 = "Needs_Other_Permission_2";
const char* const c_field_name_Needs_Other_Procedure = "Needs_Other_Procedure";
const char* const c_field_name_Needs_Other_Procedure_2 = "Needs_Other_Procedure_2";
const char* const c_field_name_Needs_Other_Source_Child = "Needs_Other_Source_Child";
const char* const c_field_name_Needs_Other_Source_Child_2 = "Needs_Other_Source_Child_2";
const char* const c_field_name_Needs_Permission = "Needs_Permission";
const char* const c_field_name_Needs_Procedure = "Needs_Procedure";
const char* const c_field_name_Needs_Procedure_Arg = "Needs_Procedure_Arg";
const char* const c_field_name_Needs_Procedure_Arg_2 = "Needs_Procedure_Arg_2";
const char* const c_field_name_Needs_Procedure_Arg_3 = "Needs_Procedure_Arg_3";
const char* const c_field_name_Needs_Source_Child = "Needs_Source_Child";
const char* const c_field_name_Needs_Source_Field = "Needs_Source_Field";
const char* const c_field_name_Needs_Source_Field_Or_Child = "Needs_Source_Field_Or_Child";
const char* const c_field_name_Needs_Source_Parent = "Needs_Source_Parent";
const char* const c_field_name_Needs_Test_Field = "Needs_Test_Field";
const char* const c_field_name_Needs_Test_Value = "Needs_Test_Value";
const char* const c_field_name_Needs_Value = "Needs_Value";
const char* const c_field_name_Next_Child_Specification_Type = "Next_Child_Specification_Type";
const char* const c_field_name_Next_Protect_Child_Rel = "Next_Protect_Child_Rel";
const char* const c_field_name_Next_Protect_Procedure = "Next_Protect_Procedure";
const char* const c_field_name_Next_Protect_Source_Parent = "Next_Protect_Source_Parent";
const char* const c_field_name_Next_Specification_Actions = "Next_Specification_Actions";
const char* const c_field_name_Notes = "Notes";
const char* const c_field_name_Parent_Specification_Type = "Parent_Specification_Type";
const char* const c_field_name_Procedure_Arg_primitive = "Procedure_Arg_primitive";
const char* const c_field_name_Procedure_Arg_type = "Procedure_Arg_type";
const char* const c_field_name_Protect_Child_Rel_From_Update = "Protect_Child_Rel_From_Update";
const char* const c_field_name_Protect_Class_From_Edit = "Protect_Class_From_Edit";
const char* const c_field_name_Protect_Class_From_Update = "Protect_Class_From_Update";
const char* const c_field_name_Protect_Other_Class_From_Edit = "Protect_Other_Class_From_Edit";
const char* const c_field_name_Protect_Procedure_From_Edit = "Protect_Procedure_From_Edit";
const char* const c_field_name_Protect_Source_Parent = "Protect_Source_Parent";
const char* const c_field_name_Set_Field_To_Source_Field_Or_Child = "Set_Field_To_Source_Field_Or_Child";
const char* const c_field_name_Source_Field_Needs_Test_Field = "Source_Field_Needs_Test_Field";
const char* const c_field_name_Source_Field_Type_Match = "Source_Field_Type_Match";
const char* const c_field_name_Source_Parent_type = "Source_Parent_type";
const char* const c_field_name_Source_type = "Source_type";
const char* const c_field_name_Specification_Actions = "Specification_Actions";
const char* const c_field_name_Specification_Name = "Specification_Name";
const char* const c_field_name_Specification_Object = "Specification_Object";
const char* const c_field_name_Specification_Strings = "Specification_Strings";
const char* const c_field_name_Specification_Vars = "Specification_Vars";
const char* const c_field_name_Test_Field_Type_Match = "Test_Field_Type_Match";
const char* const c_field_name_Test_Field_type = "Test_Field_type";
const char* const c_field_name_Use_Class_As_Source_Parent_Class = "Use_Class_As_Source_Parent_Class";
const char* const c_field_name_Use_Field_Enum = "Use_Field_Enum";
const char* const c_field_name_Use_Parent_Child_Rel_As_Source_Parent = "Use_Parent_Child_Rel_As_Source_Parent";
const char* const c_field_name_Use_Parent_Child_Rel_For_Class = "Use_Parent_Child_Rel_For_Class";
const char* const c_field_name_Use_Parent_Child_Rel_For_Other_Class = "Use_Parent_Child_Rel_For_Other_Class";
const char* const c_field_name_Use_Parent_Class = "Use_Parent_Class";
const char* const c_field_name_Use_Parent_Field_For_Class = "Use_Parent_Field_For_Class";
const char* const c_field_name_Use_Parent_Other_Class = "Use_Parent_Other_Class";
const char* const c_field_name_Use_Parent_Other_Class_For_Class = "Use_Parent_Other_Class_For_Class";
const char* const c_field_name_Use_Parent_Procedure = "Use_Parent_Procedure";
const char* const c_field_name_Use_Parent_Source_Class = "Use_Parent_Source_Class";
const char* const c_field_name_Use_Parent_Source_Field_For_Class = "Use_Parent_Source_Field_For_Class";
const char* const c_field_name_Use_Parent_Source_Parent = "Use_Parent_Source_Parent";
const char* const c_field_name_Use_Parent_Source_Parent_For_Class = "Use_Parent_Source_Parent_For_Class";
const char* const c_field_name_Use_Source_Field_Enum = "Use_Source_Field_Enum";
const char* const c_field_name_Use_Source_Field_Or_Child_Enum = "Use_Source_Field_Or_Child_Enum";
const char* const c_field_name_View_Id = "View_Id";

const char* const c_field_display_name_Allow_Child_Relationship = "field_specification_type_allow_child_relationship";
const char* const c_field_display_name_Allow_Enum = "field_specification_type_allow_enum";
const char* const c_field_display_name_Allow_Enum_Item = "field_specification_type_allow_enum_item";
const char* const c_field_display_name_Allow_FK_Source_Field_Choice = "field_specification_type_allow_fk_source_field_choice";
const char* const c_field_display_name_Allow_FK_Test_Field_Choice = "field_specification_type_allow_fk_test_field_choice";
const char* const c_field_display_name_Allow_Field = "field_specification_type_allow_field";
const char* const c_field_display_name_Allow_Modifier = "field_specification_type_allow_modifier";
const char* const c_field_display_name_Allow_Options = "field_specification_type_allow_options";
const char* const c_field_display_name_Allow_Other_Class = "field_specification_type_allow_other_class";
const char* const c_field_display_name_Allow_Other_Class_Field = "field_specification_type_allow_other_class_field";
const char* const c_field_display_name_Allow_Other_Field = "field_specification_type_allow_other_field";
const char* const c_field_display_name_Allow_Other_Field_2 = "field_specification_type_allow_other_field_2";
const char* const c_field_display_name_Allow_Other_Modifier = "field_specification_type_allow_other_modifier";
const char* const c_field_display_name_Allow_Other_Modifier_2 = "field_specification_type_allow_other_modifier_2";
const char* const c_field_display_name_Allow_Other_Procedure = "field_specification_type_allow_other_procedure";
const char* const c_field_display_name_Allow_Other_Procedure_2 = "field_specification_type_allow_other_procedure_2";
const char* const c_field_display_name_Allow_Other_Source_Child = "field_specification_type_allow_other_source_child";
const char* const c_field_display_name_Allow_Other_Source_Child_2 = "field_specification_type_allow_other_source_child_2";
const char* const c_field_display_name_Allow_Permission = "field_specification_type_allow_permission";
const char* const c_field_display_name_Allow_Procedure = "field_specification_type_allow_procedure";
const char* const c_field_display_name_Allow_Procedure_Arg = "field_specification_type_allow_procedure_arg";
const char* const c_field_display_name_Allow_Source_Child = "field_specification_type_allow_source_child";
const char* const c_field_display_name_Allow_Source_Class = "field_specification_type_allow_source_class";
const char* const c_field_display_name_Allow_Source_Field = "field_specification_type_allow_source_field";
const char* const c_field_display_name_Allow_Source_Grandchild = "field_specification_type_allow_source_grandchild";
const char* const c_field_display_name_Allow_Source_Parent = "field_specification_type_allow_source_parent";
const char* const c_field_display_name_Allow_Test_Field = "field_specification_type_allow_test_field";
const char* const c_field_display_name_Allow_Test_Value = "field_specification_type_allow_test_value";
const char* const c_field_display_name_Allow_Value = "field_specification_type_allow_value";
const char* const c_field_display_name_Can_Access_Below_Model = "field_specification_type_can_access_below_model";
const char* const c_field_display_name_Child_Prefix = "field_specification_type_child_prefix";
const char* const c_field_display_name_Child_Rel_Grandparent_Match = "field_specification_type_child_rel_grandparent_match";
const char* const c_field_display_name_Child_Rel_Parent_Match = "field_specification_type_child_rel_parent_match";
const char* const c_field_display_name_Child_Relationship_Class_Match = "field_specification_type_child_relationship_class_match";
const char* const c_field_display_name_Child_Specification_Type = "field_specification_type_child_specification_type";
const char* const c_field_display_name_Default_Child_Vars = "field_specification_type_default_child_vars";
const char* const c_field_display_name_Field_type = "field_specification_type_field_type";
const char* const c_field_display_name_Has_Next_Specification_Info = "field_specification_type_has_next_specification_info";
const char* const c_field_display_name_Is_Child_Only = "field_specification_type_is_child_only";
const char* const c_field_display_name_Is_Required_For_UI_Gen = "field_specification_type_is_required_for_ui_gen";
const char* const c_field_display_name_Is_System = "field_specification_type_is_system";
const char* const c_field_display_name_Name = "field_specification_type_name";
const char* const c_field_display_name_Needs_Child_Relationship = "field_specification_type_needs_child_relationship";
const char* const c_field_display_name_Needs_Class = "field_specification_type_needs_class";
const char* const c_field_display_name_Needs_Enum = "field_specification_type_needs_enum";
const char* const c_field_display_name_Needs_Enum_Item = "field_specification_type_needs_enum_item";
const char* const c_field_display_name_Needs_Enum_Item_2 = "field_specification_type_needs_enum_item_2";
const char* const c_field_display_name_Needs_Enum_Item_3 = "field_specification_type_needs_enum_item_3";
const char* const c_field_display_name_Needs_Enum_Item_4 = "field_specification_type_needs_enum_item_4";
const char* const c_field_display_name_Needs_Enum_Item_5 = "field_specification_type_needs_enum_item_5";
const char* const c_field_display_name_Needs_Field = "field_specification_type_needs_field";
const char* const c_field_display_name_Needs_Modifier = "field_specification_type_needs_modifier";
const char* const c_field_display_name_Needs_Other_Class = "field_specification_type_needs_other_class";
const char* const c_field_display_name_Needs_Other_Class_Field = "field_specification_type_needs_other_class_field";
const char* const c_field_display_name_Needs_Other_Field = "field_specification_type_needs_other_field";
const char* const c_field_display_name_Needs_Other_Field_2 = "field_specification_type_needs_other_field_2";
const char* const c_field_display_name_Needs_Other_Modifier = "field_specification_type_needs_other_modifier";
const char* const c_field_display_name_Needs_Other_Modifier_2 = "field_specification_type_needs_other_modifier_2";
const char* const c_field_display_name_Needs_Other_Permission = "field_specification_type_needs_other_permission";
const char* const c_field_display_name_Needs_Other_Permission_2 = "field_specification_type_needs_other_permission_2";
const char* const c_field_display_name_Needs_Other_Procedure = "field_specification_type_needs_other_procedure";
const char* const c_field_display_name_Needs_Other_Procedure_2 = "field_specification_type_needs_other_procedure_2";
const char* const c_field_display_name_Needs_Other_Source_Child = "field_specification_type_needs_other_source_child";
const char* const c_field_display_name_Needs_Other_Source_Child_2 = "field_specification_type_needs_other_source_child_2";
const char* const c_field_display_name_Needs_Permission = "field_specification_type_needs_permission";
const char* const c_field_display_name_Needs_Procedure = "field_specification_type_needs_procedure";
const char* const c_field_display_name_Needs_Procedure_Arg = "field_specification_type_needs_procedure_arg";
const char* const c_field_display_name_Needs_Procedure_Arg_2 = "field_specification_type_needs_procedure_arg_2";
const char* const c_field_display_name_Needs_Procedure_Arg_3 = "field_specification_type_needs_procedure_arg_3";
const char* const c_field_display_name_Needs_Source_Child = "field_specification_type_needs_source_child";
const char* const c_field_display_name_Needs_Source_Field = "field_specification_type_needs_source_field";
const char* const c_field_display_name_Needs_Source_Field_Or_Child = "field_specification_type_needs_source_field_or_child";
const char* const c_field_display_name_Needs_Source_Parent = "field_specification_type_needs_source_parent";
const char* const c_field_display_name_Needs_Test_Field = "field_specification_type_needs_test_field";
const char* const c_field_display_name_Needs_Test_Value = "field_specification_type_needs_test_value";
const char* const c_field_display_name_Needs_Value = "field_specification_type_needs_value";
const char* const c_field_display_name_Next_Child_Specification_Type = "field_specification_type_next_child_specification_type";
const char* const c_field_display_name_Next_Protect_Child_Rel = "field_specification_type_next_protect_child_rel";
const char* const c_field_display_name_Next_Protect_Procedure = "field_specification_type_next_protect_procedure";
const char* const c_field_display_name_Next_Protect_Source_Parent = "field_specification_type_next_protect_source_parent";
const char* const c_field_display_name_Next_Specification_Actions = "field_specification_type_next_specification_actions";
const char* const c_field_display_name_Notes = "field_specification_type_notes";
const char* const c_field_display_name_Parent_Specification_Type = "field_specification_type_parent_specification_type";
const char* const c_field_display_name_Procedure_Arg_primitive = "field_specification_type_procedure_arg_primitive";
const char* const c_field_display_name_Procedure_Arg_type = "field_specification_type_procedure_arg_type";
const char* const c_field_display_name_Protect_Child_Rel_From_Update = "field_specification_type_protect_child_rel_from_update";
const char* const c_field_display_name_Protect_Class_From_Edit = "field_specification_type_protect_class_from_edit";
const char* const c_field_display_name_Protect_Class_From_Update = "field_specification_type_protect_class_from_update";
const char* const c_field_display_name_Protect_Other_Class_From_Edit = "field_specification_type_protect_other_class_from_edit";
const char* const c_field_display_name_Protect_Procedure_From_Edit = "field_specification_type_protect_procedure_from_edit";
const char* const c_field_display_name_Protect_Source_Parent = "field_specification_type_protect_source_parent";
const char* const c_field_display_name_Set_Field_To_Source_Field_Or_Child = "field_specification_type_set_field_to_source_field_or_child";
const char* const c_field_display_name_Source_Field_Needs_Test_Field = "field_specification_type_source_field_needs_test_field";
const char* const c_field_display_name_Source_Field_Type_Match = "field_specification_type_source_field_type_match";
const char* const c_field_display_name_Source_Parent_type = "field_specification_type_source_parent_type";
const char* const c_field_display_name_Source_type = "field_specification_type_source_type";
const char* const c_field_display_name_Specification_Actions = "field_specification_type_specification_actions";
const char* const c_field_display_name_Specification_Name = "field_specification_type_specification_name";
const char* const c_field_display_name_Specification_Object = "field_specification_type_specification_object";
const char* const c_field_display_name_Specification_Strings = "field_specification_type_specification_strings";
const char* const c_field_display_name_Specification_Vars = "field_specification_type_specification_vars";
const char* const c_field_display_name_Test_Field_Type_Match = "field_specification_type_test_field_type_match";
const char* const c_field_display_name_Test_Field_type = "field_specification_type_test_field_type";
const char* const c_field_display_name_Use_Class_As_Source_Parent_Class = "field_specification_type_use_class_as_source_parent_class";
const char* const c_field_display_name_Use_Field_Enum = "field_specification_type_use_field_enum";
const char* const c_field_display_name_Use_Parent_Child_Rel_As_Source_Parent = "field_specification_type_use_parent_child_rel_as_source_parent";
const char* const c_field_display_name_Use_Parent_Child_Rel_For_Class = "field_specification_type_use_parent_child_rel_for_class";
const char* const c_field_display_name_Use_Parent_Child_Rel_For_Other_Class = "field_specification_type_use_parent_child_rel_for_other_class";
const char* const c_field_display_name_Use_Parent_Class = "field_specification_type_use_parent_class";
const char* const c_field_display_name_Use_Parent_Field_For_Class = "field_specification_type_use_parent_field_for_class";
const char* const c_field_display_name_Use_Parent_Other_Class = "field_specification_type_use_parent_other_class";
const char* const c_field_display_name_Use_Parent_Other_Class_For_Class = "field_specification_type_use_parent_other_class_for_class";
const char* const c_field_display_name_Use_Parent_Procedure = "field_specification_type_use_parent_procedure";
const char* const c_field_display_name_Use_Parent_Source_Class = "field_specification_type_use_parent_source_class";
const char* const c_field_display_name_Use_Parent_Source_Field_For_Class = "field_specification_type_use_parent_source_field_for_class";
const char* const c_field_display_name_Use_Parent_Source_Parent = "field_specification_type_use_parent_source_parent";
const char* const c_field_display_name_Use_Parent_Source_Parent_For_Class = "field_specification_type_use_parent_source_parent_for_class";
const char* const c_field_display_name_Use_Source_Field_Enum = "field_specification_type_use_source_field_enum";
const char* const c_field_display_name_Use_Source_Field_Or_Child_Enum = "field_specification_type_use_source_field_or_child_enum";
const char* const c_field_display_name_View_Id = "field_specification_type_view_id";

const int c_num_fields = 120;

const char* const c_all_sorted_field_ids[ ] =
{
   "114101",
   "114102",
   "114103",
   "114104",
   "114105",
   "114106",
   "114107",
   "114108",
   "114109",
   "114110",
   "114111",
   "114112",
   "114113",
   "114114",
   "114115",
   "114116",
   "114117",
   "114118",
   "114119",
   "114120",
   "114121",
   "114122",
   "114123",
   "114124",
   "114125",
   "114126",
   "114127",
   "114128",
   "114129",
   "114130",
   "114131",
   "114132",
   "114133",
   "114134",
   "114135",
   "114136",
   "114137",
   "114138",
   "114139",
   "114140",
   "114141",
   "114142",
   "114144",
   "114145",
   "114146",
   "114147",
   "114148",
   "114149",
   "114150",
   "114151",
   "114152",
   "114153",
   "114154",
   "114155",
   "114156",
   "114157",
   "114158",
   "114159",
   "114160",
   "114161",
   "114162",
   "114163",
   "114164",
   "114165",
   "114166",
   "114167",
   "114168",
   "114169",
   "114170",
   "114171",
   "114172",
   "114173",
   "114174",
   "114175",
   "114176",
   "114177",
   "114178",
   "114179",
   "114180",
   "114181",
   "114182",
   "114183",
   "114184",
   "114185",
   "114186",
   "114187",
   "114188",
   "114189",
   "114190",
   "114191",
   "114192",
   "114193",
   "114194",
   "114195",
   "114196",
   "114197",
   "114198",
   "114199",
   "114200",
   "114201",
   "114202",
   "114203",
   "114204",
   "114205",
   "114206",
   "114207",
   "114208",
   "114209",
   "114210",
   "114211",
   "114212",
   "114213",
   "114214",
   "114215",
   "114216",
   "114217",
   "114218",
   "301700",
   "301710",
   "301720"
};

const char* const c_all_sorted_field_names[ ] =
{
   "Allow_Child_Relationship",
   "Allow_Enum",
   "Allow_Enum_Item",
   "Allow_FK_Source_Field_Choice",
   "Allow_FK_Test_Field_Choice",
   "Allow_Field",
   "Allow_Modifier",
   "Allow_Options",
   "Allow_Other_Class",
   "Allow_Other_Class_Field",
   "Allow_Other_Field",
   "Allow_Other_Field_2",
   "Allow_Other_Modifier",
   "Allow_Other_Modifier_2",
   "Allow_Other_Procedure",
   "Allow_Other_Procedure_2",
   "Allow_Other_Source_Child",
   "Allow_Other_Source_Child_2",
   "Allow_Permission",
   "Allow_Procedure",
   "Allow_Procedure_Arg",
   "Allow_Source_Child",
   "Allow_Source_Class",
   "Allow_Source_Field",
   "Allow_Source_Grandchild",
   "Allow_Source_Parent",
   "Allow_Test_Field",
   "Allow_Test_Value",
   "Allow_Value",
   "Can_Access_Below_Model",
   "Child_Prefix",
   "Child_Rel_Grandparent_Match",
   "Child_Rel_Parent_Match",
   "Child_Relationship_Class_Match",
   "Child_Specification_Type",
   "Default_Child_Vars",
   "Field_type",
   "Has_Next_Specification_Info",
   "Is_Child_Only",
   "Is_Required_For_UI_Gen",
   "Is_System",
   "Name",
   "Needs_Child_Relationship",
   "Needs_Class",
   "Needs_Enum",
   "Needs_Enum_Item",
   "Needs_Enum_Item_2",
   "Needs_Enum_Item_3",
   "Needs_Enum_Item_4",
   "Needs_Enum_Item_5",
   "Needs_Field",
   "Needs_Modifier",
   "Needs_Other_Class",
   "Needs_Other_Class_Field",
   "Needs_Other_Field",
   "Needs_Other_Field_2",
   "Needs_Other_Modifier",
   "Needs_Other_Modifier_2",
   "Needs_Other_Permission",
   "Needs_Other_Permission_2",
   "Needs_Other_Procedure",
   "Needs_Other_Procedure_2",
   "Needs_Other_Source_Child",
   "Needs_Other_Source_Child_2",
   "Needs_Permission",
   "Needs_Procedure",
   "Needs_Procedure_Arg",
   "Needs_Procedure_Arg_2",
   "Needs_Procedure_Arg_3",
   "Needs_Source_Child",
   "Needs_Source_Field",
   "Needs_Source_Field_Or_Child",
   "Needs_Source_Parent",
   "Needs_Test_Field",
   "Needs_Test_Value",
   "Needs_Value",
   "Next_Child_Specification_Type",
   "Next_Protect_Child_Rel",
   "Next_Protect_Procedure",
   "Next_Protect_Source_Parent",
   "Next_Specification_Actions",
   "Notes",
   "Parent_Specification_Type",
   "Procedure_Arg_primitive",
   "Procedure_Arg_type",
   "Protect_Child_Rel_From_Update",
   "Protect_Class_From_Edit",
   "Protect_Class_From_Update",
   "Protect_Other_Class_From_Edit",
   "Protect_Procedure_From_Edit",
   "Protect_Source_Parent",
   "Set_Field_To_Source_Field_Or_Child",
   "Source_Field_Needs_Test_Field",
   "Source_Field_Type_Match",
   "Source_Parent_type",
   "Source_type",
   "Specification_Actions",
   "Specification_Name",
   "Specification_Object",
   "Specification_Strings",
   "Specification_Vars",
   "Test_Field_Type_Match",
   "Test_Field_type",
   "Use_Class_As_Source_Parent_Class",
   "Use_Field_Enum",
   "Use_Parent_Child_Rel_As_Source_Parent",
   "Use_Parent_Child_Rel_For_Class",
   "Use_Parent_Child_Rel_For_Other_Class",
   "Use_Parent_Class",
   "Use_Parent_Field_For_Class",
   "Use_Parent_Other_Class",
   "Use_Parent_Other_Class_For_Class",
   "Use_Parent_Procedure",
   "Use_Parent_Source_Class",
   "Use_Parent_Source_Field_For_Class",
   "Use_Parent_Source_Parent",
   "Use_Parent_Source_Parent_For_Class",
   "Use_Source_Field_Enum",
   "Use_Source_Field_Or_Child_Enum",
   "View_Id"
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

const char* const c_procedure_id_Is_Valid_Field_Type = "114410";

domain_string_max_size< 30 > g_Child_Prefix_domain;
domain_string_max_size< 100 > g_Name_domain;
domain_string_max_size< 200 > g_Specification_Name_domain;
domain_string_max_size< 100 > g_Specification_Object_domain;
domain_string_max_size< 10 > g_View_Id_domain;

string g_group_field_name;
string g_level_field_name;
string g_order_field_name;
string g_owner_field_name;

string g_state_names_variable;

set< string > g_derivations;

set< string > g_file_field_ids;
set< string > g_file_field_names;

typedef map< string, Meta_Specification_Type* > external_aliases_container;
typedef external_aliases_container::const_iterator external_aliases_const_iterator;
typedef external_aliases_container::value_type external_aliases_value_type;

typedef map< size_t, Meta_Specification_Type* > external_aliases_lookup_container;
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

bool g_default_Allow_Child_Relationship = bool( 0 );
bool g_default_Allow_Enum = bool( 0 );
bool g_default_Allow_Enum_Item = bool( 0 );
bool g_default_Allow_FK_Source_Field_Choice = bool( 0 );
bool g_default_Allow_FK_Test_Field_Choice = bool( 0 );
bool g_default_Allow_Field = bool( 0 );
bool g_default_Allow_Modifier = bool( 0 );
bool g_default_Allow_Options = bool( 0 );
bool g_default_Allow_Other_Class = bool( 0 );
bool g_default_Allow_Other_Class_Field = bool( 0 );
bool g_default_Allow_Other_Field = bool( 0 );
bool g_default_Allow_Other_Field_2 = bool( 0 );
bool g_default_Allow_Other_Modifier = bool( 0 );
bool g_default_Allow_Other_Modifier_2 = bool( 0 );
bool g_default_Allow_Other_Procedure = bool( 0 );
bool g_default_Allow_Other_Procedure_2 = bool( 0 );
bool g_default_Allow_Other_Source_Child = bool( 0 );
bool g_default_Allow_Other_Source_Child_2 = bool( 0 );
bool g_default_Allow_Permission = bool( 0 );
bool g_default_Allow_Procedure = bool( 0 );
bool g_default_Allow_Procedure_Arg = bool( 0 );
bool g_default_Allow_Source_Child = bool( 0 );
bool g_default_Allow_Source_Class = bool( 0 );
bool g_default_Allow_Source_Field = bool( 0 );
bool g_default_Allow_Source_Grandchild = bool( 0 );
bool g_default_Allow_Source_Parent = bool( 0 );
bool g_default_Allow_Test_Field = bool( 0 );
bool g_default_Allow_Test_Value = bool( 0 );
bool g_default_Allow_Value = bool( 0 );
bool g_default_Can_Access_Below_Model = bool( 0 );
string g_default_Child_Prefix = string( );
bool g_default_Child_Rel_Grandparent_Match = bool( 0 );
bool g_default_Child_Rel_Parent_Match = bool( 0 );
bool g_default_Child_Relationship_Class_Match = bool( 0 );
string g_default_Child_Specification_Type = string( );
string g_default_Default_Child_Vars = string( );
int g_default_Field_type = int( 99 );
bool g_default_Has_Next_Specification_Info = bool( 0 );
bool g_default_Is_Child_Only = bool( 0 );
bool g_default_Is_Required_For_UI_Gen = bool( 0 );
bool g_default_Is_System = bool( 0 );
string g_default_Name = string( );
bool g_default_Needs_Child_Relationship = bool( 0 );
bool g_default_Needs_Class = bool( 0 );
bool g_default_Needs_Enum = bool( 0 );
bool g_default_Needs_Enum_Item = bool( 0 );
bool g_default_Needs_Enum_Item_2 = bool( 0 );
bool g_default_Needs_Enum_Item_3 = bool( 0 );
bool g_default_Needs_Enum_Item_4 = bool( 0 );
bool g_default_Needs_Enum_Item_5 = bool( 0 );
bool g_default_Needs_Field = bool( 0 );
bool g_default_Needs_Modifier = bool( 0 );
bool g_default_Needs_Other_Class = bool( 0 );
bool g_default_Needs_Other_Class_Field = bool( 0 );
bool g_default_Needs_Other_Field = bool( 0 );
bool g_default_Needs_Other_Field_2 = bool( 0 );
bool g_default_Needs_Other_Modifier = bool( 0 );
bool g_default_Needs_Other_Modifier_2 = bool( 0 );
bool g_default_Needs_Other_Permission = bool( 0 );
bool g_default_Needs_Other_Permission_2 = bool( 0 );
bool g_default_Needs_Other_Procedure = bool( 0 );
bool g_default_Needs_Other_Procedure_2 = bool( 0 );
bool g_default_Needs_Other_Source_Child = bool( 0 );
bool g_default_Needs_Other_Source_Child_2 = bool( 0 );
bool g_default_Needs_Permission = bool( 0 );
bool g_default_Needs_Procedure = bool( 0 );
bool g_default_Needs_Procedure_Arg = bool( 0 );
bool g_default_Needs_Procedure_Arg_2 = bool( 0 );
bool g_default_Needs_Procedure_Arg_3 = bool( 0 );
bool g_default_Needs_Source_Child = bool( 0 );
bool g_default_Needs_Source_Field = bool( 0 );
bool g_default_Needs_Source_Field_Or_Child = bool( 0 );
bool g_default_Needs_Source_Parent = bool( 0 );
bool g_default_Needs_Test_Field = bool( 0 );
bool g_default_Needs_Test_Value = bool( 0 );
bool g_default_Needs_Value = bool( 0 );
string g_default_Next_Child_Specification_Type = string( );
bool g_default_Next_Protect_Child_Rel = bool( 0 );
bool g_default_Next_Protect_Procedure = bool( 0 );
bool g_default_Next_Protect_Source_Parent = bool( 0 );
string g_default_Next_Specification_Actions = string( );
string g_default_Notes = string( );
string g_default_Parent_Specification_Type = string( );
int g_default_Procedure_Arg_primitive = int( 99 );
int g_default_Procedure_Arg_type = int( 99 );
bool g_default_Protect_Child_Rel_From_Update = bool( 0 );
bool g_default_Protect_Class_From_Edit = bool( 0 );
bool g_default_Protect_Class_From_Update = bool( 1 );
bool g_default_Protect_Other_Class_From_Edit = bool( 0 );
bool g_default_Protect_Procedure_From_Edit = bool( 0 );
bool g_default_Protect_Source_Parent = bool( 0 );
bool g_default_Set_Field_To_Source_Field_Or_Child = bool( 0 );
bool g_default_Source_Field_Needs_Test_Field = bool( 0 );
bool g_default_Source_Field_Type_Match = bool( 0 );
int g_default_Source_Parent_type = int( 50 );
int g_default_Source_type = int( 99 );
string g_default_Specification_Actions = string( );
string g_default_Specification_Name = string( );
string g_default_Specification_Object = string( );
string g_default_Specification_Strings = string( );
string g_default_Specification_Vars = string( );
bool g_default_Test_Field_Type_Match = bool( 0 );
int g_default_Test_Field_type = int( 99 );
bool g_default_Use_Class_As_Source_Parent_Class = bool( 0 );
bool g_default_Use_Field_Enum = bool( 0 );
bool g_default_Use_Parent_Child_Rel_As_Source_Parent = bool( 0 );
bool g_default_Use_Parent_Child_Rel_For_Class = bool( 0 );
bool g_default_Use_Parent_Child_Rel_For_Other_Class = bool( 0 );
bool g_default_Use_Parent_Class = bool( 0 );
bool g_default_Use_Parent_Field_For_Class = bool( 0 );
bool g_default_Use_Parent_Other_Class = bool( 0 );
bool g_default_Use_Parent_Other_Class_For_Class = bool( 0 );
bool g_default_Use_Parent_Procedure = bool( 0 );
bool g_default_Use_Parent_Source_Class = bool( 0 );
bool g_default_Use_Parent_Source_Field_For_Class = bool( 0 );
bool g_default_Use_Parent_Source_Parent = bool( 0 );
bool g_default_Use_Parent_Source_Parent_For_Class = bool( 0 );
bool g_default_Use_Source_Field_Enum = bool( 0 );
bool g_default_Use_Source_Field_Or_Child_Enum = bool( 0 );
string g_default_View_Id = string( );

set< int > g_field_type_enum;
set< int > g_procedure_arg_primitive_enum;
set< int > g_procedure_arg_type_enum;
set< int > g_source_parent_type_enum;
set< int > g_source_type_enum;
set< int > g_test_field_type_enum;

const int c_enum_field_type_string( 0 );
const int c_enum_field_type_datetime( 1 );
const int c_enum_field_type_date( 2 );
const int c_enum_field_type_time( 3 );
const int c_enum_field_type_numeric( 4 );
const int c_enum_field_type_int( 5 );
const int c_enum_field_type_bool( 6 );
const int c_enum_field_type_Any_Text_Type( 20 );
const int c_enum_field_type_Any_Scalar_Type( 30 );
const int c_enum_field_type_Any_Math_Type( 31 );
const int c_enum_field_type_Any_Non_Text_Type( 39 );
const int c_enum_field_type_Date_Or_Time( 40 );
const int c_enum_field_type_Calendar_Type( 41 );
const int c_enum_field_type_Any_Date_Or_Time_Type( 45 );
const int c_enum_field_type_Any_Primitive( 49 );
const int c_enum_field_type_Any_Relationship( 50 );
const int c_enum_field_type_Self_Relationship( 51 );
const int c_enum_field_type_Non_Self_Relationship( 52 );
const int c_enum_field_type_Any_Type( 99 );

string get_enum_string_field_type( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for field_type" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_field_type_string";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_field_type_datetime";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_field_type_date";
   else if( to_string( val ) == to_string( "3" ) )
      string_name = "enum_field_type_time";
   else if( to_string( val ) == to_string( "4" ) )
      string_name = "enum_field_type_numeric";
   else if( to_string( val ) == to_string( "5" ) )
      string_name = "enum_field_type_int";
   else if( to_string( val ) == to_string( "6" ) )
      string_name = "enum_field_type_bool";
   else if( to_string( val ) == to_string( "20" ) )
      string_name = "enum_field_type_Any_Text_Type";
   else if( to_string( val ) == to_string( "30" ) )
      string_name = "enum_field_type_Any_Scalar_Type";
   else if( to_string( val ) == to_string( "31" ) )
      string_name = "enum_field_type_Any_Math_Type";
   else if( to_string( val ) == to_string( "39" ) )
      string_name = "enum_field_type_Any_Non_Text_Type";
   else if( to_string( val ) == to_string( "40" ) )
      string_name = "enum_field_type_Date_Or_Time";
   else if( to_string( val ) == to_string( "41" ) )
      string_name = "enum_field_type_Calendar_Type";
   else if( to_string( val ) == to_string( "45" ) )
      string_name = "enum_field_type_Any_Date_Or_Time_Type";
   else if( to_string( val ) == to_string( "49" ) )
      string_name = "enum_field_type_Any_Primitive";
   else if( to_string( val ) == to_string( "50" ) )
      string_name = "enum_field_type_Any_Relationship";
   else if( to_string( val ) == to_string( "51" ) )
      string_name = "enum_field_type_Self_Relationship";
   else if( to_string( val ) == to_string( "52" ) )
      string_name = "enum_field_type_Non_Self_Relationship";
   else if( to_string( val ) == to_string( "99" ) )
      string_name = "enum_field_type_Any_Type";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for field_type" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_procedure_arg_primitive_string( 0 );
const int c_enum_procedure_arg_primitive_datetime( 1 );
const int c_enum_procedure_arg_primitive_date( 2 );
const int c_enum_procedure_arg_primitive_time( 3 );
const int c_enum_procedure_arg_primitive_numeric( 4 );
const int c_enum_procedure_arg_primitive_int( 5 );
const int c_enum_procedure_arg_primitive_bool( 6 );
const int c_enum_procedure_arg_primitive_Any_Primitive( 99 );

string get_enum_string_procedure_arg_primitive( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for procedure_arg_primitive" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_procedure_arg_primitive_string";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_procedure_arg_primitive_datetime";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_procedure_arg_primitive_date";
   else if( to_string( val ) == to_string( "3" ) )
      string_name = "enum_procedure_arg_primitive_time";
   else if( to_string( val ) == to_string( "4" ) )
      string_name = "enum_procedure_arg_primitive_numeric";
   else if( to_string( val ) == to_string( "5" ) )
      string_name = "enum_procedure_arg_primitive_int";
   else if( to_string( val ) == to_string( "6" ) )
      string_name = "enum_procedure_arg_primitive_bool";
   else if( to_string( val ) == to_string( "99" ) )
      string_name = "enum_procedure_arg_primitive_Any_Primitive";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for procedure_arg_primitive" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_procedure_arg_type_in( 0 );
const int c_enum_procedure_arg_type_out( 1 );
const int c_enum_procedure_arg_type_Any_Type( 99 );

string get_enum_string_procedure_arg_type( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for procedure_arg_type" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_procedure_arg_type_in";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_procedure_arg_type_out";
   else if( to_string( val ) == to_string( "99" ) )
      string_name = "enum_procedure_arg_type_Any_Type";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for procedure_arg_type" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_source_parent_type_Any_Relationship( 50 );
const int c_enum_source_parent_type_Self_Relationship( 51 );
const int c_enum_source_parent_type_Non_Self_Relationship( 52 );

string get_enum_string_source_parent_type( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for source_parent_type" );
   else if( to_string( val ) == to_string( "50" ) )
      string_name = "enum_source_parent_type_Any_Relationship";
   else if( to_string( val ) == to_string( "51" ) )
      string_name = "enum_source_parent_type_Self_Relationship";
   else if( to_string( val ) == to_string( "52" ) )
      string_name = "enum_source_parent_type_Non_Self_Relationship";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for source_parent_type" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_source_type_string( 0 );
const int c_enum_source_type_datetime( 1 );
const int c_enum_source_type_date( 2 );
const int c_enum_source_type_time( 3 );
const int c_enum_source_type_numeric( 4 );
const int c_enum_source_type_int( 5 );
const int c_enum_source_type_bool( 6 );
const int c_enum_source_type_Any_Text_Type( 20 );
const int c_enum_source_type_Any_Conditional( 35 );
const int c_enum_source_type_Any_Type( 99 );

string get_enum_string_source_type( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for source_type" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_source_type_string";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_source_type_datetime";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_source_type_date";
   else if( to_string( val ) == to_string( "3" ) )
      string_name = "enum_source_type_time";
   else if( to_string( val ) == to_string( "4" ) )
      string_name = "enum_source_type_numeric";
   else if( to_string( val ) == to_string( "5" ) )
      string_name = "enum_source_type_int";
   else if( to_string( val ) == to_string( "6" ) )
      string_name = "enum_source_type_bool";
   else if( to_string( val ) == to_string( "20" ) )
      string_name = "enum_source_type_Any_Text_Type";
   else if( to_string( val ) == to_string( "35" ) )
      string_name = "enum_source_type_Any_Conditional";
   else if( to_string( val ) == to_string( "99" ) )
      string_name = "enum_source_type_Any_Type";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for source_type" );

   return get_module_string( lower( string_name ) );
}

const int c_enum_test_field_type_string( 0 );
const int c_enum_test_field_type_datetime( 1 );
const int c_enum_test_field_type_date( 2 );
const int c_enum_test_field_type_time( 3 );
const int c_enum_test_field_type_numeric( 4 );
const int c_enum_test_field_type_int( 5 );
const int c_enum_test_field_type_bool( 6 );
const int c_enum_test_field_type_Any_Text_Type( 20 );
const int c_enum_test_field_type_Any_Scalar_Type( 30 );
const int c_enum_test_field_type_Any_Conditional( 35 );
const int c_enum_test_field_type_Any_Value_Or_Conditional( 38 );
const int c_enum_test_field_type_Any_Type( 99 );

string get_enum_string_test_field_type( int val )
{
   string string_name;

   if( to_string( val ) == "" )
      throw runtime_error( "unexpected empty enum value for test_field_type" );
   else if( to_string( val ) == to_string( "0" ) )
      string_name = "enum_test_field_type_string";
   else if( to_string( val ) == to_string( "1" ) )
      string_name = "enum_test_field_type_datetime";
   else if( to_string( val ) == to_string( "2" ) )
      string_name = "enum_test_field_type_date";
   else if( to_string( val ) == to_string( "3" ) )
      string_name = "enum_test_field_type_time";
   else if( to_string( val ) == to_string( "4" ) )
      string_name = "enum_test_field_type_numeric";
   else if( to_string( val ) == to_string( "5" ) )
      string_name = "enum_test_field_type_int";
   else if( to_string( val ) == to_string( "6" ) )
      string_name = "enum_test_field_type_bool";
   else if( to_string( val ) == to_string( "20" ) )
      string_name = "enum_test_field_type_Any_Text_Type";
   else if( to_string( val ) == to_string( "30" ) )
      string_name = "enum_test_field_type_Any_Scalar_Type";
   else if( to_string( val ) == to_string( "35" ) )
      string_name = "enum_test_field_type_Any_Conditional";
   else if( to_string( val ) == to_string( "38" ) )
      string_name = "enum_test_field_type_Any_Value_Or_Conditional";
   else if( to_string( val ) == to_string( "99" ) )
      string_name = "enum_test_field_type_Any_Type";
   else
      throw runtime_error( "unexpected enum value '" + to_string( val ) + "' for test_field_type" );

   return get_module_string( lower( string_name ) );
}

// [<start anonymous>]
// [<finish anonymous>]

}

registration< Meta_Specification_Type > Specification_Type_registration( get_class_registry( ), "114100" );

class Meta_Specification_Type_command_functor;

class Meta_Specification_Type_command_handler : public command_handler
{
   friend class Meta_Specification_Type_command_functor;

   public:
   Meta_Specification_Type_command_handler( ) : p_Meta_Specification_Type( 0 ) { }

   void set_Meta_Specification_Type( Meta_Specification_Type* p_new_Meta_Specification_Type ) { p_Meta_Specification_Type = p_new_Meta_Specification_Type; }

   void handle_unknown_command( const string& command, const string& cmd_and_args )
   {
      throw runtime_error( "unknown command '" + command + "'" );
   }

   void handle_invalid_command( const command_parser& parser, const string& cmd_and_args )
   {
      throw runtime_error( "invalid command usage '" + cmd_and_args + "'" );
   }

   private:
   Meta_Specification_Type* p_Meta_Specification_Type;

   protected:
   string retval;
};

class Meta_Specification_Type_command_functor : public command_functor
{
   public:
   Meta_Specification_Type_command_functor( Meta_Specification_Type_command_handler& handler )
    : command_functor( handler ),
    cmd_handler( handler )
   {
   }

   void operator ( )( const string& command, const parameter_info& parameters );

   private:
   Meta_Specification_Type_command_handler& cmd_handler;
};

command_functor* Meta_Specification_Type_command_functor_factory( const string& /*name*/, command_handler& handler )
{
   return new Meta_Specification_Type_command_functor( dynamic_cast< Meta_Specification_Type_command_handler& >( handler ) );
}

void Meta_Specification_Type_command_functor::operator ( )( const string& command, const parameter_info& parameters )
{
   if( command == c_cmd_Meta_Specification_Type_key )
   {
      bool want_fixed( has_parm_val( parameters, c_cmd_Meta_Specification_Type_key_fixed ) );

      if( !want_fixed )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Type->get_key( );
      else
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Type->get_fixed_key( );
   }
   else if( command == c_cmd_Meta_Specification_Type_ver )
   {
      string ver_rev( to_string( cmd_handler.p_Meta_Specification_Type->get_version( ) ) );
      ver_rev += "." + to_string( cmd_handler.p_Meta_Specification_Type->get_revision( ) );

      cmd_handler.retval = ver_rev;
   }
   else if( command == c_cmd_Meta_Specification_Type_get )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Specification_Type_get_field_name ) );

      bool handled = false;

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for getter call" );

      if( !handled && ( ( field_name == c_field_id_Allow_Child_Relationship ) || ( field_name == c_field_name_Allow_Child_Relationship ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Allow_Child_Relationship( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Enum ) || ( field_name == c_field_name_Allow_Enum ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Allow_Enum( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Enum_Item ) || ( field_name == c_field_name_Allow_Enum_Item ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Allow_Enum_Item( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_FK_Source_Field_Choice ) || ( field_name == c_field_name_Allow_FK_Source_Field_Choice ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Allow_FK_Source_Field_Choice( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_FK_Test_Field_Choice ) || ( field_name == c_field_name_Allow_FK_Test_Field_Choice ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Allow_FK_Test_Field_Choice( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Field ) || ( field_name == c_field_name_Allow_Field ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Allow_Field( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Modifier ) || ( field_name == c_field_name_Allow_Modifier ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Allow_Modifier( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Options ) || ( field_name == c_field_name_Allow_Options ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Allow_Options( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Other_Class ) || ( field_name == c_field_name_Allow_Other_Class ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Allow_Other_Class( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Other_Class_Field ) || ( field_name == c_field_name_Allow_Other_Class_Field ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Allow_Other_Class_Field( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Other_Field ) || ( field_name == c_field_name_Allow_Other_Field ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Allow_Other_Field( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Other_Field_2 ) || ( field_name == c_field_name_Allow_Other_Field_2 ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Allow_Other_Field_2( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Other_Modifier ) || ( field_name == c_field_name_Allow_Other_Modifier ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Allow_Other_Modifier( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Other_Modifier_2 ) || ( field_name == c_field_name_Allow_Other_Modifier_2 ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Allow_Other_Modifier_2( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Other_Procedure ) || ( field_name == c_field_name_Allow_Other_Procedure ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Allow_Other_Procedure( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Other_Procedure_2 ) || ( field_name == c_field_name_Allow_Other_Procedure_2 ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Allow_Other_Procedure_2( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Other_Source_Child ) || ( field_name == c_field_name_Allow_Other_Source_Child ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Allow_Other_Source_Child( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Other_Source_Child_2 ) || ( field_name == c_field_name_Allow_Other_Source_Child_2 ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Allow_Other_Source_Child_2( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Permission ) || ( field_name == c_field_name_Allow_Permission ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Allow_Permission( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Procedure ) || ( field_name == c_field_name_Allow_Procedure ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Allow_Procedure( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Procedure_Arg ) || ( field_name == c_field_name_Allow_Procedure_Arg ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Allow_Procedure_Arg( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Source_Child ) || ( field_name == c_field_name_Allow_Source_Child ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Allow_Source_Child( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Source_Class ) || ( field_name == c_field_name_Allow_Source_Class ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Allow_Source_Class( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Source_Field ) || ( field_name == c_field_name_Allow_Source_Field ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Allow_Source_Field( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Source_Grandchild ) || ( field_name == c_field_name_Allow_Source_Grandchild ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Allow_Source_Grandchild( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Source_Parent ) || ( field_name == c_field_name_Allow_Source_Parent ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Allow_Source_Parent( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Test_Field ) || ( field_name == c_field_name_Allow_Test_Field ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Allow_Test_Field( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Test_Value ) || ( field_name == c_field_name_Allow_Test_Value ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Allow_Test_Value( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Value ) || ( field_name == c_field_name_Allow_Value ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Allow_Value( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Can_Access_Below_Model ) || ( field_name == c_field_name_Can_Access_Below_Model ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Can_Access_Below_Model( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Child_Prefix ) || ( field_name == c_field_name_Child_Prefix ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Specification_Type->Child_Prefix( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Child_Rel_Grandparent_Match ) || ( field_name == c_field_name_Child_Rel_Grandparent_Match ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Child_Rel_Grandparent_Match( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Child_Rel_Parent_Match ) || ( field_name == c_field_name_Child_Rel_Parent_Match ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Child_Rel_Parent_Match( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Child_Relationship_Class_Match ) || ( field_name == c_field_name_Child_Relationship_Class_Match ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Child_Relationship_Class_Match( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Child_Specification_Type ) || ( field_name == c_field_name_Child_Specification_Type ) ) )
      {
         handled = true;

         string_getter< Meta_Specification_Type >( cmd_handler.p_Meta_Specification_Type->Child_Specification_Type( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Default_Child_Vars ) || ( field_name == c_field_name_Default_Child_Vars ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Specification_Type->Default_Child_Vars( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Field_type ) || ( field_name == c_field_name_Field_type ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Specification_Type->Field_type( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Has_Next_Specification_Info ) || ( field_name == c_field_name_Has_Next_Specification_Info ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Has_Next_Specification_Info( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Is_Child_Only ) || ( field_name == c_field_name_Is_Child_Only ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Is_Child_Only( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Is_Required_For_UI_Gen ) || ( field_name == c_field_name_Is_Required_For_UI_Gen ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Is_Required_For_UI_Gen( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Is_System ) || ( field_name == c_field_name_Is_System ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Is_System( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Name ) || ( field_name == c_field_name_Name ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Specification_Type->Name( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Child_Relationship ) || ( field_name == c_field_name_Needs_Child_Relationship ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Needs_Child_Relationship( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Class ) || ( field_name == c_field_name_Needs_Class ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Needs_Class( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Enum ) || ( field_name == c_field_name_Needs_Enum ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Needs_Enum( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Enum_Item ) || ( field_name == c_field_name_Needs_Enum_Item ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Needs_Enum_Item( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Enum_Item_2 ) || ( field_name == c_field_name_Needs_Enum_Item_2 ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Needs_Enum_Item_2( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Enum_Item_3 ) || ( field_name == c_field_name_Needs_Enum_Item_3 ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Needs_Enum_Item_3( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Enum_Item_4 ) || ( field_name == c_field_name_Needs_Enum_Item_4 ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Needs_Enum_Item_4( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Enum_Item_5 ) || ( field_name == c_field_name_Needs_Enum_Item_5 ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Needs_Enum_Item_5( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Field ) || ( field_name == c_field_name_Needs_Field ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Needs_Field( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Modifier ) || ( field_name == c_field_name_Needs_Modifier ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Needs_Modifier( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Other_Class ) || ( field_name == c_field_name_Needs_Other_Class ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Needs_Other_Class( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Other_Class_Field ) || ( field_name == c_field_name_Needs_Other_Class_Field ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Needs_Other_Class_Field( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Other_Field ) || ( field_name == c_field_name_Needs_Other_Field ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Needs_Other_Field( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Other_Field_2 ) || ( field_name == c_field_name_Needs_Other_Field_2 ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Needs_Other_Field_2( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Other_Modifier ) || ( field_name == c_field_name_Needs_Other_Modifier ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Needs_Other_Modifier( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Other_Modifier_2 ) || ( field_name == c_field_name_Needs_Other_Modifier_2 ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Needs_Other_Modifier_2( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Other_Permission ) || ( field_name == c_field_name_Needs_Other_Permission ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Needs_Other_Permission( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Other_Permission_2 ) || ( field_name == c_field_name_Needs_Other_Permission_2 ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Needs_Other_Permission_2( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Other_Procedure ) || ( field_name == c_field_name_Needs_Other_Procedure ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Needs_Other_Procedure( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Other_Procedure_2 ) || ( field_name == c_field_name_Needs_Other_Procedure_2 ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Needs_Other_Procedure_2( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Other_Source_Child ) || ( field_name == c_field_name_Needs_Other_Source_Child ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Needs_Other_Source_Child( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Other_Source_Child_2 ) || ( field_name == c_field_name_Needs_Other_Source_Child_2 ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Needs_Other_Source_Child_2( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Permission ) || ( field_name == c_field_name_Needs_Permission ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Needs_Permission( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Procedure ) || ( field_name == c_field_name_Needs_Procedure ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Needs_Procedure( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Procedure_Arg ) || ( field_name == c_field_name_Needs_Procedure_Arg ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Needs_Procedure_Arg( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Procedure_Arg_2 ) || ( field_name == c_field_name_Needs_Procedure_Arg_2 ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Needs_Procedure_Arg_2( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Procedure_Arg_3 ) || ( field_name == c_field_name_Needs_Procedure_Arg_3 ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Needs_Procedure_Arg_3( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Source_Child ) || ( field_name == c_field_name_Needs_Source_Child ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Needs_Source_Child( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Source_Field ) || ( field_name == c_field_name_Needs_Source_Field ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Needs_Source_Field( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Source_Field_Or_Child ) || ( field_name == c_field_name_Needs_Source_Field_Or_Child ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Needs_Source_Field_Or_Child( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Source_Parent ) || ( field_name == c_field_name_Needs_Source_Parent ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Needs_Source_Parent( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Test_Field ) || ( field_name == c_field_name_Needs_Test_Field ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Needs_Test_Field( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Test_Value ) || ( field_name == c_field_name_Needs_Test_Value ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Needs_Test_Value( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Value ) || ( field_name == c_field_name_Needs_Value ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Needs_Value( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Next_Child_Specification_Type ) || ( field_name == c_field_name_Next_Child_Specification_Type ) ) )
      {
         handled = true;

         string_getter< Meta_Specification_Type >( cmd_handler.p_Meta_Specification_Type->Next_Child_Specification_Type( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Next_Protect_Child_Rel ) || ( field_name == c_field_name_Next_Protect_Child_Rel ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Next_Protect_Child_Rel( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Next_Protect_Procedure ) || ( field_name == c_field_name_Next_Protect_Procedure ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Next_Protect_Procedure( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Next_Protect_Source_Parent ) || ( field_name == c_field_name_Next_Protect_Source_Parent ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Next_Protect_Source_Parent( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Next_Specification_Actions ) || ( field_name == c_field_name_Next_Specification_Actions ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Specification_Type->Next_Specification_Actions( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Notes ) || ( field_name == c_field_name_Notes ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Specification_Type->Notes( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Parent_Specification_Type ) || ( field_name == c_field_name_Parent_Specification_Type ) ) )
      {
         handled = true;

         string_getter< Meta_Specification_Type >( cmd_handler.p_Meta_Specification_Type->Parent_Specification_Type( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Procedure_Arg_primitive ) || ( field_name == c_field_name_Procedure_Arg_primitive ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Specification_Type->Procedure_Arg_primitive( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Procedure_Arg_type ) || ( field_name == c_field_name_Procedure_Arg_type ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Specification_Type->Procedure_Arg_type( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Protect_Child_Rel_From_Update ) || ( field_name == c_field_name_Protect_Child_Rel_From_Update ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Protect_Child_Rel_From_Update( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Protect_Class_From_Edit ) || ( field_name == c_field_name_Protect_Class_From_Edit ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Protect_Class_From_Edit( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Protect_Class_From_Update ) || ( field_name == c_field_name_Protect_Class_From_Update ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Protect_Class_From_Update( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Protect_Other_Class_From_Edit ) || ( field_name == c_field_name_Protect_Other_Class_From_Edit ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Protect_Other_Class_From_Edit( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Protect_Procedure_From_Edit ) || ( field_name == c_field_name_Protect_Procedure_From_Edit ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Protect_Procedure_From_Edit( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Protect_Source_Parent ) || ( field_name == c_field_name_Protect_Source_Parent ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Protect_Source_Parent( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Set_Field_To_Source_Field_Or_Child ) || ( field_name == c_field_name_Set_Field_To_Source_Field_Or_Child ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Set_Field_To_Source_Field_Or_Child( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Field_Needs_Test_Field ) || ( field_name == c_field_name_Source_Field_Needs_Test_Field ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Source_Field_Needs_Test_Field( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Field_Type_Match ) || ( field_name == c_field_name_Source_Field_Type_Match ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Source_Field_Type_Match( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Parent_type ) || ( field_name == c_field_name_Source_Parent_type ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Specification_Type->Source_Parent_type( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Source_type ) || ( field_name == c_field_name_Source_type ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Specification_Type->Source_type( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Specification_Actions ) || ( field_name == c_field_name_Specification_Actions ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Specification_Type->Specification_Actions( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Specification_Name ) || ( field_name == c_field_name_Specification_Name ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Specification_Type->Specification_Name( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Specification_Object ) || ( field_name == c_field_name_Specification_Object ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Specification_Type->Specification_Object( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Specification_Strings ) || ( field_name == c_field_name_Specification_Strings ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Specification_Type->Specification_Strings( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Specification_Vars ) || ( field_name == c_field_name_Specification_Vars ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Specification_Type->Specification_Vars( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Test_Field_Type_Match ) || ( field_name == c_field_name_Test_Field_Type_Match ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Test_Field_Type_Match( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Test_Field_type ) || ( field_name == c_field_name_Test_Field_type ) ) )
      {
         handled = true;

         string_getter< int >( cmd_handler.p_Meta_Specification_Type->Test_Field_type( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Class_As_Source_Parent_Class ) || ( field_name == c_field_name_Use_Class_As_Source_Parent_Class ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Use_Class_As_Source_Parent_Class( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Field_Enum ) || ( field_name == c_field_name_Use_Field_Enum ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Use_Field_Enum( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Parent_Child_Rel_As_Source_Parent ) || ( field_name == c_field_name_Use_Parent_Child_Rel_As_Source_Parent ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Use_Parent_Child_Rel_As_Source_Parent( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Parent_Child_Rel_For_Class ) || ( field_name == c_field_name_Use_Parent_Child_Rel_For_Class ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Use_Parent_Child_Rel_For_Class( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Parent_Child_Rel_For_Other_Class ) || ( field_name == c_field_name_Use_Parent_Child_Rel_For_Other_Class ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Use_Parent_Child_Rel_For_Other_Class( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Parent_Class ) || ( field_name == c_field_name_Use_Parent_Class ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Use_Parent_Class( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Parent_Field_For_Class ) || ( field_name == c_field_name_Use_Parent_Field_For_Class ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Use_Parent_Field_For_Class( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Parent_Other_Class ) || ( field_name == c_field_name_Use_Parent_Other_Class ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Use_Parent_Other_Class( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Parent_Other_Class_For_Class ) || ( field_name == c_field_name_Use_Parent_Other_Class_For_Class ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Use_Parent_Other_Class_For_Class( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Parent_Procedure ) || ( field_name == c_field_name_Use_Parent_Procedure ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Use_Parent_Procedure( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Parent_Source_Class ) || ( field_name == c_field_name_Use_Parent_Source_Class ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Use_Parent_Source_Class( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Parent_Source_Field_For_Class ) || ( field_name == c_field_name_Use_Parent_Source_Field_For_Class ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Use_Parent_Source_Field_For_Class( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Parent_Source_Parent ) || ( field_name == c_field_name_Use_Parent_Source_Parent ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Use_Parent_Source_Parent( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Parent_Source_Parent_For_Class ) || ( field_name == c_field_name_Use_Parent_Source_Parent_For_Class ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Use_Parent_Source_Parent_For_Class( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Source_Field_Enum ) || ( field_name == c_field_name_Use_Source_Field_Enum ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Use_Source_Field_Enum( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Source_Field_Or_Child_Enum ) || ( field_name == c_field_name_Use_Source_Field_Or_Child_Enum ) ) )
      {
         handled = true;

         string_getter< bool >( cmd_handler.p_Meta_Specification_Type->Use_Source_Field_Or_Child_Enum( ), cmd_handler.retval );
      }

      if( !handled && ( ( field_name == c_field_id_View_Id ) || ( field_name == c_field_name_View_Id ) ) )
      {
         handled = true;

         string_getter< string >( cmd_handler.p_Meta_Specification_Type->View_Id( ), cmd_handler.retval );
      }

      if( !handled )
         throw runtime_error( "unknown field name '" + field_name + "' for getter call" );
   }
   else if( command == c_cmd_Meta_Specification_Type_set )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Specification_Type_set_field_name ) );
      string field_value( get_parm_val( parameters, c_cmd_Meta_Specification_Type_set_field_value ) );

      bool handled = false;

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for setter call" );

      if( !handled && ( ( field_name == c_field_id_Allow_Child_Relationship ) || ( field_name == c_field_name_Allow_Child_Relationship ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Allow_Child_Relationship, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Enum ) || ( field_name == c_field_name_Allow_Enum ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Allow_Enum, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Enum_Item ) || ( field_name == c_field_name_Allow_Enum_Item ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Allow_Enum_Item, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_FK_Source_Field_Choice ) || ( field_name == c_field_name_Allow_FK_Source_Field_Choice ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Allow_FK_Source_Field_Choice, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_FK_Test_Field_Choice ) || ( field_name == c_field_name_Allow_FK_Test_Field_Choice ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Allow_FK_Test_Field_Choice, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Field ) || ( field_name == c_field_name_Allow_Field ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Allow_Field, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Modifier ) || ( field_name == c_field_name_Allow_Modifier ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Allow_Modifier, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Options ) || ( field_name == c_field_name_Allow_Options ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Allow_Options, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Other_Class ) || ( field_name == c_field_name_Allow_Other_Class ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Allow_Other_Class, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Other_Class_Field ) || ( field_name == c_field_name_Allow_Other_Class_Field ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Allow_Other_Class_Field, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Other_Field ) || ( field_name == c_field_name_Allow_Other_Field ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Allow_Other_Field, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Other_Field_2 ) || ( field_name == c_field_name_Allow_Other_Field_2 ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Allow_Other_Field_2, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Other_Modifier ) || ( field_name == c_field_name_Allow_Other_Modifier ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Allow_Other_Modifier, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Other_Modifier_2 ) || ( field_name == c_field_name_Allow_Other_Modifier_2 ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Allow_Other_Modifier_2, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Other_Procedure ) || ( field_name == c_field_name_Allow_Other_Procedure ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Allow_Other_Procedure, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Other_Procedure_2 ) || ( field_name == c_field_name_Allow_Other_Procedure_2 ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Allow_Other_Procedure_2, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Other_Source_Child ) || ( field_name == c_field_name_Allow_Other_Source_Child ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Allow_Other_Source_Child, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Other_Source_Child_2 ) || ( field_name == c_field_name_Allow_Other_Source_Child_2 ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Allow_Other_Source_Child_2, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Permission ) || ( field_name == c_field_name_Allow_Permission ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Allow_Permission, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Procedure ) || ( field_name == c_field_name_Allow_Procedure ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Allow_Procedure, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Procedure_Arg ) || ( field_name == c_field_name_Allow_Procedure_Arg ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Allow_Procedure_Arg, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Source_Child ) || ( field_name == c_field_name_Allow_Source_Child ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Allow_Source_Child, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Source_Class ) || ( field_name == c_field_name_Allow_Source_Class ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Allow_Source_Class, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Source_Field ) || ( field_name == c_field_name_Allow_Source_Field ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Allow_Source_Field, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Source_Grandchild ) || ( field_name == c_field_name_Allow_Source_Grandchild ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Allow_Source_Grandchild, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Source_Parent ) || ( field_name == c_field_name_Allow_Source_Parent ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Allow_Source_Parent, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Test_Field ) || ( field_name == c_field_name_Allow_Test_Field ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Allow_Test_Field, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Test_Value ) || ( field_name == c_field_name_Allow_Test_Value ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Allow_Test_Value, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Allow_Value ) || ( field_name == c_field_name_Allow_Value ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Allow_Value, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Can_Access_Below_Model ) || ( field_name == c_field_name_Can_Access_Below_Model ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Can_Access_Below_Model, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Child_Prefix ) || ( field_name == c_field_name_Child_Prefix ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, string >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Child_Prefix, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Child_Rel_Grandparent_Match ) || ( field_name == c_field_name_Child_Rel_Grandparent_Match ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Child_Rel_Grandparent_Match, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Child_Rel_Parent_Match ) || ( field_name == c_field_name_Child_Rel_Parent_Match ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Child_Rel_Parent_Match, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Child_Relationship_Class_Match ) || ( field_name == c_field_name_Child_Relationship_Class_Match ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Child_Relationship_Class_Match, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Child_Specification_Type ) || ( field_name == c_field_name_Child_Specification_Type ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, Meta_Specification_Type >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Child_Specification_Type, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Default_Child_Vars ) || ( field_name == c_field_name_Default_Child_Vars ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, string >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Default_Child_Vars, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Field_type ) || ( field_name == c_field_name_Field_type ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, int >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Field_type, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Has_Next_Specification_Info ) || ( field_name == c_field_name_Has_Next_Specification_Info ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Has_Next_Specification_Info, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Is_Child_Only ) || ( field_name == c_field_name_Is_Child_Only ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Is_Child_Only, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Is_Required_For_UI_Gen ) || ( field_name == c_field_name_Is_Required_For_UI_Gen ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Is_Required_For_UI_Gen, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Is_System ) || ( field_name == c_field_name_Is_System ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Is_System, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Name ) || ( field_name == c_field_name_Name ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, string >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Name, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Child_Relationship ) || ( field_name == c_field_name_Needs_Child_Relationship ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Needs_Child_Relationship, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Class ) || ( field_name == c_field_name_Needs_Class ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Needs_Class, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Enum ) || ( field_name == c_field_name_Needs_Enum ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Needs_Enum, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Enum_Item ) || ( field_name == c_field_name_Needs_Enum_Item ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Needs_Enum_Item, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Enum_Item_2 ) || ( field_name == c_field_name_Needs_Enum_Item_2 ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Needs_Enum_Item_2, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Enum_Item_3 ) || ( field_name == c_field_name_Needs_Enum_Item_3 ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Needs_Enum_Item_3, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Enum_Item_4 ) || ( field_name == c_field_name_Needs_Enum_Item_4 ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Needs_Enum_Item_4, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Enum_Item_5 ) || ( field_name == c_field_name_Needs_Enum_Item_5 ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Needs_Enum_Item_5, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Field ) || ( field_name == c_field_name_Needs_Field ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Needs_Field, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Modifier ) || ( field_name == c_field_name_Needs_Modifier ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Needs_Modifier, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Other_Class ) || ( field_name == c_field_name_Needs_Other_Class ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Needs_Other_Class, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Other_Class_Field ) || ( field_name == c_field_name_Needs_Other_Class_Field ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Needs_Other_Class_Field, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Other_Field ) || ( field_name == c_field_name_Needs_Other_Field ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Needs_Other_Field, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Other_Field_2 ) || ( field_name == c_field_name_Needs_Other_Field_2 ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Needs_Other_Field_2, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Other_Modifier ) || ( field_name == c_field_name_Needs_Other_Modifier ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Needs_Other_Modifier, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Other_Modifier_2 ) || ( field_name == c_field_name_Needs_Other_Modifier_2 ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Needs_Other_Modifier_2, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Other_Permission ) || ( field_name == c_field_name_Needs_Other_Permission ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Needs_Other_Permission, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Other_Permission_2 ) || ( field_name == c_field_name_Needs_Other_Permission_2 ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Needs_Other_Permission_2, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Other_Procedure ) || ( field_name == c_field_name_Needs_Other_Procedure ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Needs_Other_Procedure, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Other_Procedure_2 ) || ( field_name == c_field_name_Needs_Other_Procedure_2 ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Needs_Other_Procedure_2, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Other_Source_Child ) || ( field_name == c_field_name_Needs_Other_Source_Child ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Needs_Other_Source_Child, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Other_Source_Child_2 ) || ( field_name == c_field_name_Needs_Other_Source_Child_2 ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Needs_Other_Source_Child_2, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Permission ) || ( field_name == c_field_name_Needs_Permission ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Needs_Permission, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Procedure ) || ( field_name == c_field_name_Needs_Procedure ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Needs_Procedure, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Procedure_Arg ) || ( field_name == c_field_name_Needs_Procedure_Arg ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Needs_Procedure_Arg, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Procedure_Arg_2 ) || ( field_name == c_field_name_Needs_Procedure_Arg_2 ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Needs_Procedure_Arg_2, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Procedure_Arg_3 ) || ( field_name == c_field_name_Needs_Procedure_Arg_3 ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Needs_Procedure_Arg_3, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Source_Child ) || ( field_name == c_field_name_Needs_Source_Child ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Needs_Source_Child, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Source_Field ) || ( field_name == c_field_name_Needs_Source_Field ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Needs_Source_Field, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Source_Field_Or_Child ) || ( field_name == c_field_name_Needs_Source_Field_Or_Child ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Needs_Source_Field_Or_Child, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Source_Parent ) || ( field_name == c_field_name_Needs_Source_Parent ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Needs_Source_Parent, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Test_Field ) || ( field_name == c_field_name_Needs_Test_Field ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Needs_Test_Field, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Test_Value ) || ( field_name == c_field_name_Needs_Test_Value ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Needs_Test_Value, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Needs_Value ) || ( field_name == c_field_name_Needs_Value ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Needs_Value, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Next_Child_Specification_Type ) || ( field_name == c_field_name_Next_Child_Specification_Type ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, Meta_Specification_Type >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Next_Child_Specification_Type, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Next_Protect_Child_Rel ) || ( field_name == c_field_name_Next_Protect_Child_Rel ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Next_Protect_Child_Rel, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Next_Protect_Procedure ) || ( field_name == c_field_name_Next_Protect_Procedure ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Next_Protect_Procedure, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Next_Protect_Source_Parent ) || ( field_name == c_field_name_Next_Protect_Source_Parent ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Next_Protect_Source_Parent, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Next_Specification_Actions ) || ( field_name == c_field_name_Next_Specification_Actions ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, string >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Next_Specification_Actions, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Notes ) || ( field_name == c_field_name_Notes ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, string >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Notes, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Parent_Specification_Type ) || ( field_name == c_field_name_Parent_Specification_Type ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, Meta_Specification_Type >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Parent_Specification_Type, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Procedure_Arg_primitive ) || ( field_name == c_field_name_Procedure_Arg_primitive ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, int >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Procedure_Arg_primitive, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Procedure_Arg_type ) || ( field_name == c_field_name_Procedure_Arg_type ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, int >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Procedure_Arg_type, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Protect_Child_Rel_From_Update ) || ( field_name == c_field_name_Protect_Child_Rel_From_Update ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Protect_Child_Rel_From_Update, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Protect_Class_From_Edit ) || ( field_name == c_field_name_Protect_Class_From_Edit ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Protect_Class_From_Edit, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Protect_Class_From_Update ) || ( field_name == c_field_name_Protect_Class_From_Update ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Protect_Class_From_Update, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Protect_Other_Class_From_Edit ) || ( field_name == c_field_name_Protect_Other_Class_From_Edit ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Protect_Other_Class_From_Edit, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Protect_Procedure_From_Edit ) || ( field_name == c_field_name_Protect_Procedure_From_Edit ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Protect_Procedure_From_Edit, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Protect_Source_Parent ) || ( field_name == c_field_name_Protect_Source_Parent ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Protect_Source_Parent, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Set_Field_To_Source_Field_Or_Child ) || ( field_name == c_field_name_Set_Field_To_Source_Field_Or_Child ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Set_Field_To_Source_Field_Or_Child, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Field_Needs_Test_Field ) || ( field_name == c_field_name_Source_Field_Needs_Test_Field ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Source_Field_Needs_Test_Field, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Field_Type_Match ) || ( field_name == c_field_name_Source_Field_Type_Match ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Source_Field_Type_Match, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Source_Parent_type ) || ( field_name == c_field_name_Source_Parent_type ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, int >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Source_Parent_type, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Source_type ) || ( field_name == c_field_name_Source_type ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, int >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Source_type, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Specification_Actions ) || ( field_name == c_field_name_Specification_Actions ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, string >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Specification_Actions, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Specification_Name ) || ( field_name == c_field_name_Specification_Name ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, string >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Specification_Name, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Specification_Object ) || ( field_name == c_field_name_Specification_Object ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, string >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Specification_Object, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Specification_Strings ) || ( field_name == c_field_name_Specification_Strings ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, string >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Specification_Strings, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Specification_Vars ) || ( field_name == c_field_name_Specification_Vars ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, string >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Specification_Vars, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Test_Field_Type_Match ) || ( field_name == c_field_name_Test_Field_Type_Match ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Test_Field_Type_Match, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Test_Field_type ) || ( field_name == c_field_name_Test_Field_type ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, int >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Test_Field_type, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Class_As_Source_Parent_Class ) || ( field_name == c_field_name_Use_Class_As_Source_Parent_Class ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Use_Class_As_Source_Parent_Class, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Field_Enum ) || ( field_name == c_field_name_Use_Field_Enum ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Use_Field_Enum, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Parent_Child_Rel_As_Source_Parent ) || ( field_name == c_field_name_Use_Parent_Child_Rel_As_Source_Parent ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Use_Parent_Child_Rel_As_Source_Parent, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Parent_Child_Rel_For_Class ) || ( field_name == c_field_name_Use_Parent_Child_Rel_For_Class ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Use_Parent_Child_Rel_For_Class, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Parent_Child_Rel_For_Other_Class ) || ( field_name == c_field_name_Use_Parent_Child_Rel_For_Other_Class ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Use_Parent_Child_Rel_For_Other_Class, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Parent_Class ) || ( field_name == c_field_name_Use_Parent_Class ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Use_Parent_Class, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Parent_Field_For_Class ) || ( field_name == c_field_name_Use_Parent_Field_For_Class ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Use_Parent_Field_For_Class, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Parent_Other_Class ) || ( field_name == c_field_name_Use_Parent_Other_Class ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Use_Parent_Other_Class, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Parent_Other_Class_For_Class ) || ( field_name == c_field_name_Use_Parent_Other_Class_For_Class ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Use_Parent_Other_Class_For_Class, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Parent_Procedure ) || ( field_name == c_field_name_Use_Parent_Procedure ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Use_Parent_Procedure, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Parent_Source_Class ) || ( field_name == c_field_name_Use_Parent_Source_Class ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Use_Parent_Source_Class, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Parent_Source_Field_For_Class ) || ( field_name == c_field_name_Use_Parent_Source_Field_For_Class ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Use_Parent_Source_Field_For_Class, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Parent_Source_Parent ) || ( field_name == c_field_name_Use_Parent_Source_Parent ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Use_Parent_Source_Parent, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Parent_Source_Parent_For_Class ) || ( field_name == c_field_name_Use_Parent_Source_Parent_For_Class ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Use_Parent_Source_Parent_For_Class, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Source_Field_Enum ) || ( field_name == c_field_name_Use_Source_Field_Enum ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Use_Source_Field_Enum, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_Use_Source_Field_Or_Child_Enum ) || ( field_name == c_field_name_Use_Source_Field_Or_Child_Enum ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, bool >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::Use_Source_Field_Or_Child_Enum, field_value );
      }

      if( !handled && ( ( field_name == c_field_id_View_Id ) || ( field_name == c_field_name_View_Id ) ) )
      {
         handled = true;

         func_string_setter< Meta_Specification_Type, string >(
          *cmd_handler.p_Meta_Specification_Type, &Meta_Specification_Type::View_Id, field_value );
      }

      if( !handled )
         throw runtime_error( "unknown field name '" + field_name + "' for setter call" );

      cmd_handler.retval = c_okay;
   }
   else if( command == c_cmd_Meta_Specification_Type_cmd )
   {
      string field_name( get_parm_val( parameters, c_cmd_Meta_Specification_Type_cmd_field_name ) );
      string cmd_and_args( get_parm_val( parameters, c_cmd_Meta_Specification_Type_cmd_cmd_and_args ) );

      cmd_handler.retval.erase( );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for command call" );
      else if( ( field_name == c_field_id_Child_Specification_Type ) || ( field_name == c_field_name_Child_Specification_Type ) )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Type->Child_Specification_Type( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Next_Child_Specification_Type ) || ( field_name == c_field_name_Next_Child_Specification_Type ) )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Type->Next_Child_Specification_Type( ).execute( cmd_and_args );
      else if( ( field_name == c_field_id_Parent_Specification_Type ) || ( field_name == c_field_name_Parent_Specification_Type ) )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Type->Parent_Specification_Type( ).execute( cmd_and_args );
      else
         throw runtime_error( "unknown field name '" + field_name + "' for command call" );
   }
   else if( command == c_cmd_Meta_Specification_Type_Is_Valid_Field_Type )
   {
      string Class( get_parm_val_from_string< string >( parameters, c_cmd_Meta_Specification_Type_Is_Valid_Field_Type_Class ) );
      string Parent_Class( get_parm_val_from_string< string >( parameters, c_cmd_Meta_Specification_Type_Is_Valid_Field_Type_Parent_Class ) );
      int Primitive( get_parm_val_from_string< int >( parameters, c_cmd_Meta_Specification_Type_Is_Valid_Field_Type_Primitive ) );
      int Procedure_Arg_Primitive( get_parm_val_from_string< int >( parameters, c_cmd_Meta_Specification_Type_Is_Valid_Field_Type_Procedure_Arg_Primitive ) );
      int Procedure_Arg_Type( get_parm_val_from_string< int >( parameters, c_cmd_Meta_Specification_Type_Is_Valid_Field_Type_Procedure_Arg_Type ) );
      string Source_Parent_Class( get_parm_val_from_string< string >( parameters, c_cmd_Meta_Specification_Type_Is_Valid_Field_Type_Source_Parent_Class ) );
      int Source_Primitive( get_parm_val_from_string< int >( parameters, c_cmd_Meta_Specification_Type_Is_Valid_Field_Type_Source_Primitive ) );
      int Test_Primitive( get_parm_val_from_string< int >( parameters, c_cmd_Meta_Specification_Type_Is_Valid_Field_Type_Test_Primitive ) );
      string Test_Value( get_parm_val_from_string< string >( parameters, c_cmd_Meta_Specification_Type_Is_Valid_Field_Type_Test_Value ) );
      string Error;

      cmd_handler.p_Meta_Specification_Type->Is_Valid_Field_Type( Class, Error, Parent_Class, Primitive, Procedure_Arg_Primitive, Procedure_Arg_Type, Source_Parent_Class, Source_Primitive, Test_Primitive, Test_Value );

      cmd_handler.retval.erase( );

      append_value( cmd_handler.retval, Error );
   }
}

struct Meta_Specification_Type::impl : public Meta_Specification_Type_command_handler
{
   impl( Meta_Specification_Type& o )
    :
    cp_obj( &o ),
    total_child_relationships( 0 )
   {
      p_obj = &o;
      set_Meta_Specification_Type( &o );

      add_commands( 0, Meta_Specification_Type_command_functor_factory,
       ARRAY_PTR_AND_SIZE( Meta_Specification_Type_command_definitions ) );
   }

   Meta_Specification_Type& get_obj( ) const
   {
      return *cp_obj;
   }

   bool impl_Allow_Child_Relationship( ) const { return lazy_fetch( p_obj ), v_Allow_Child_Relationship; }
   void impl_Allow_Child_Relationship( bool Allow_Child_Relationship ) { v_Allow_Child_Relationship = Allow_Child_Relationship; }

   bool impl_Allow_Enum( ) const { return lazy_fetch( p_obj ), v_Allow_Enum; }
   void impl_Allow_Enum( bool Allow_Enum ) { v_Allow_Enum = Allow_Enum; }

   bool impl_Allow_Enum_Item( ) const { return lazy_fetch( p_obj ), v_Allow_Enum_Item; }
   void impl_Allow_Enum_Item( bool Allow_Enum_Item ) { v_Allow_Enum_Item = Allow_Enum_Item; }

   bool impl_Allow_FK_Source_Field_Choice( ) const { return lazy_fetch( p_obj ), v_Allow_FK_Source_Field_Choice; }
   void impl_Allow_FK_Source_Field_Choice( bool Allow_FK_Source_Field_Choice ) { v_Allow_FK_Source_Field_Choice = Allow_FK_Source_Field_Choice; }

   bool impl_Allow_FK_Test_Field_Choice( ) const { return lazy_fetch( p_obj ), v_Allow_FK_Test_Field_Choice; }
   void impl_Allow_FK_Test_Field_Choice( bool Allow_FK_Test_Field_Choice ) { v_Allow_FK_Test_Field_Choice = Allow_FK_Test_Field_Choice; }

   bool impl_Allow_Field( ) const { return lazy_fetch( p_obj ), v_Allow_Field; }
   void impl_Allow_Field( bool Allow_Field ) { v_Allow_Field = Allow_Field; }

   bool impl_Allow_Modifier( ) const { return lazy_fetch( p_obj ), v_Allow_Modifier; }
   void impl_Allow_Modifier( bool Allow_Modifier ) { v_Allow_Modifier = Allow_Modifier; }

   bool impl_Allow_Options( ) const { return lazy_fetch( p_obj ), v_Allow_Options; }
   void impl_Allow_Options( bool Allow_Options ) { v_Allow_Options = Allow_Options; }

   bool impl_Allow_Other_Class( ) const { return lazy_fetch( p_obj ), v_Allow_Other_Class; }
   void impl_Allow_Other_Class( bool Allow_Other_Class ) { v_Allow_Other_Class = Allow_Other_Class; }

   bool impl_Allow_Other_Class_Field( ) const { return lazy_fetch( p_obj ), v_Allow_Other_Class_Field; }
   void impl_Allow_Other_Class_Field( bool Allow_Other_Class_Field ) { v_Allow_Other_Class_Field = Allow_Other_Class_Field; }

   bool impl_Allow_Other_Field( ) const { return lazy_fetch( p_obj ), v_Allow_Other_Field; }
   void impl_Allow_Other_Field( bool Allow_Other_Field ) { v_Allow_Other_Field = Allow_Other_Field; }

   bool impl_Allow_Other_Field_2( ) const { return lazy_fetch( p_obj ), v_Allow_Other_Field_2; }
   void impl_Allow_Other_Field_2( bool Allow_Other_Field_2 ) { v_Allow_Other_Field_2 = Allow_Other_Field_2; }

   bool impl_Allow_Other_Modifier( ) const { return lazy_fetch( p_obj ), v_Allow_Other_Modifier; }
   void impl_Allow_Other_Modifier( bool Allow_Other_Modifier ) { v_Allow_Other_Modifier = Allow_Other_Modifier; }

   bool impl_Allow_Other_Modifier_2( ) const { return lazy_fetch( p_obj ), v_Allow_Other_Modifier_2; }
   void impl_Allow_Other_Modifier_2( bool Allow_Other_Modifier_2 ) { v_Allow_Other_Modifier_2 = Allow_Other_Modifier_2; }

   bool impl_Allow_Other_Procedure( ) const { return lazy_fetch( p_obj ), v_Allow_Other_Procedure; }
   void impl_Allow_Other_Procedure( bool Allow_Other_Procedure ) { v_Allow_Other_Procedure = Allow_Other_Procedure; }

   bool impl_Allow_Other_Procedure_2( ) const { return lazy_fetch( p_obj ), v_Allow_Other_Procedure_2; }
   void impl_Allow_Other_Procedure_2( bool Allow_Other_Procedure_2 ) { v_Allow_Other_Procedure_2 = Allow_Other_Procedure_2; }

   bool impl_Allow_Other_Source_Child( ) const { return lazy_fetch( p_obj ), v_Allow_Other_Source_Child; }
   void impl_Allow_Other_Source_Child( bool Allow_Other_Source_Child ) { v_Allow_Other_Source_Child = Allow_Other_Source_Child; }

   bool impl_Allow_Other_Source_Child_2( ) const { return lazy_fetch( p_obj ), v_Allow_Other_Source_Child_2; }
   void impl_Allow_Other_Source_Child_2( bool Allow_Other_Source_Child_2 ) { v_Allow_Other_Source_Child_2 = Allow_Other_Source_Child_2; }

   bool impl_Allow_Permission( ) const { return lazy_fetch( p_obj ), v_Allow_Permission; }
   void impl_Allow_Permission( bool Allow_Permission ) { v_Allow_Permission = Allow_Permission; }

   bool impl_Allow_Procedure( ) const { return lazy_fetch( p_obj ), v_Allow_Procedure; }
   void impl_Allow_Procedure( bool Allow_Procedure ) { v_Allow_Procedure = Allow_Procedure; }

   bool impl_Allow_Procedure_Arg( ) const { return lazy_fetch( p_obj ), v_Allow_Procedure_Arg; }
   void impl_Allow_Procedure_Arg( bool Allow_Procedure_Arg ) { v_Allow_Procedure_Arg = Allow_Procedure_Arg; }

   bool impl_Allow_Source_Child( ) const { return lazy_fetch( p_obj ), v_Allow_Source_Child; }
   void impl_Allow_Source_Child( bool Allow_Source_Child ) { v_Allow_Source_Child = Allow_Source_Child; }

   bool impl_Allow_Source_Class( ) const { return lazy_fetch( p_obj ), v_Allow_Source_Class; }
   void impl_Allow_Source_Class( bool Allow_Source_Class ) { v_Allow_Source_Class = Allow_Source_Class; }

   bool impl_Allow_Source_Field( ) const { return lazy_fetch( p_obj ), v_Allow_Source_Field; }
   void impl_Allow_Source_Field( bool Allow_Source_Field ) { v_Allow_Source_Field = Allow_Source_Field; }

   bool impl_Allow_Source_Grandchild( ) const { return lazy_fetch( p_obj ), v_Allow_Source_Grandchild; }
   void impl_Allow_Source_Grandchild( bool Allow_Source_Grandchild ) { v_Allow_Source_Grandchild = Allow_Source_Grandchild; }

   bool impl_Allow_Source_Parent( ) const { return lazy_fetch( p_obj ), v_Allow_Source_Parent; }
   void impl_Allow_Source_Parent( bool Allow_Source_Parent ) { v_Allow_Source_Parent = Allow_Source_Parent; }

   bool impl_Allow_Test_Field( ) const { return lazy_fetch( p_obj ), v_Allow_Test_Field; }
   void impl_Allow_Test_Field( bool Allow_Test_Field ) { v_Allow_Test_Field = Allow_Test_Field; }

   bool impl_Allow_Test_Value( ) const { return lazy_fetch( p_obj ), v_Allow_Test_Value; }
   void impl_Allow_Test_Value( bool Allow_Test_Value ) { v_Allow_Test_Value = Allow_Test_Value; }

   bool impl_Allow_Value( ) const { return lazy_fetch( p_obj ), v_Allow_Value; }
   void impl_Allow_Value( bool Allow_Value ) { v_Allow_Value = Allow_Value; }

   bool impl_Can_Access_Below_Model( ) const { return lazy_fetch( p_obj ), v_Can_Access_Below_Model; }
   void impl_Can_Access_Below_Model( bool Can_Access_Below_Model ) { v_Can_Access_Below_Model = Can_Access_Below_Model; }

   const string& impl_Child_Prefix( ) const { return lazy_fetch( p_obj ), v_Child_Prefix; }
   void impl_Child_Prefix( const string& Child_Prefix ) { sanity_check( Child_Prefix ); v_Child_Prefix = Child_Prefix; }

   bool impl_Child_Rel_Grandparent_Match( ) const { return lazy_fetch( p_obj ), v_Child_Rel_Grandparent_Match; }
   void impl_Child_Rel_Grandparent_Match( bool Child_Rel_Grandparent_Match ) { v_Child_Rel_Grandparent_Match = Child_Rel_Grandparent_Match; }

   bool impl_Child_Rel_Parent_Match( ) const { return lazy_fetch( p_obj ), v_Child_Rel_Parent_Match; }
   void impl_Child_Rel_Parent_Match( bool Child_Rel_Parent_Match ) { v_Child_Rel_Parent_Match = Child_Rel_Parent_Match; }

   bool impl_Child_Relationship_Class_Match( ) const { return lazy_fetch( p_obj ), v_Child_Relationship_Class_Match; }
   void impl_Child_Relationship_Class_Match( bool Child_Relationship_Class_Match ) { v_Child_Relationship_Class_Match = Child_Relationship_Class_Match; }

   const string& impl_Default_Child_Vars( ) const { return lazy_fetch( p_obj ), v_Default_Child_Vars; }
   void impl_Default_Child_Vars( const string& Default_Child_Vars ) { sanity_check( Default_Child_Vars ); v_Default_Child_Vars = Default_Child_Vars; }

   int impl_Field_type( ) const { return lazy_fetch( p_obj ), v_Field_type; }
   void impl_Field_type( int Field_type ) { v_Field_type = Field_type; }

   bool impl_Has_Next_Specification_Info( ) const { return lazy_fetch( p_obj ), v_Has_Next_Specification_Info; }
   void impl_Has_Next_Specification_Info( bool Has_Next_Specification_Info ) { v_Has_Next_Specification_Info = Has_Next_Specification_Info; }

   bool impl_Is_Child_Only( ) const { return lazy_fetch( p_obj ), v_Is_Child_Only; }
   void impl_Is_Child_Only( bool Is_Child_Only ) { v_Is_Child_Only = Is_Child_Only; }

   bool impl_Is_Required_For_UI_Gen( ) const { return lazy_fetch( p_obj ), v_Is_Required_For_UI_Gen; }
   void impl_Is_Required_For_UI_Gen( bool Is_Required_For_UI_Gen ) { v_Is_Required_For_UI_Gen = Is_Required_For_UI_Gen; }

   bool impl_Is_System( ) const { return lazy_fetch( p_obj ), v_Is_System; }
   void impl_Is_System( bool Is_System ) { v_Is_System = Is_System; }

   const string& impl_Name( ) const { return lazy_fetch( p_obj ), v_Name; }
   void impl_Name( const string& Name ) { sanity_check( Name ); v_Name = Name; }

   bool impl_Needs_Child_Relationship( ) const { return lazy_fetch( p_obj ), v_Needs_Child_Relationship; }
   void impl_Needs_Child_Relationship( bool Needs_Child_Relationship ) { v_Needs_Child_Relationship = Needs_Child_Relationship; }

   bool impl_Needs_Class( ) const { return lazy_fetch( p_obj ), v_Needs_Class; }
   void impl_Needs_Class( bool Needs_Class ) { v_Needs_Class = Needs_Class; }

   bool impl_Needs_Enum( ) const { return lazy_fetch( p_obj ), v_Needs_Enum; }
   void impl_Needs_Enum( bool Needs_Enum ) { v_Needs_Enum = Needs_Enum; }

   bool impl_Needs_Enum_Item( ) const { return lazy_fetch( p_obj ), v_Needs_Enum_Item; }
   void impl_Needs_Enum_Item( bool Needs_Enum_Item ) { v_Needs_Enum_Item = Needs_Enum_Item; }

   bool impl_Needs_Enum_Item_2( ) const { return lazy_fetch( p_obj ), v_Needs_Enum_Item_2; }
   void impl_Needs_Enum_Item_2( bool Needs_Enum_Item_2 ) { v_Needs_Enum_Item_2 = Needs_Enum_Item_2; }

   bool impl_Needs_Enum_Item_3( ) const { return lazy_fetch( p_obj ), v_Needs_Enum_Item_3; }
   void impl_Needs_Enum_Item_3( bool Needs_Enum_Item_3 ) { v_Needs_Enum_Item_3 = Needs_Enum_Item_3; }

   bool impl_Needs_Enum_Item_4( ) const { return lazy_fetch( p_obj ), v_Needs_Enum_Item_4; }
   void impl_Needs_Enum_Item_4( bool Needs_Enum_Item_4 ) { v_Needs_Enum_Item_4 = Needs_Enum_Item_4; }

   bool impl_Needs_Enum_Item_5( ) const { return lazy_fetch( p_obj ), v_Needs_Enum_Item_5; }
   void impl_Needs_Enum_Item_5( bool Needs_Enum_Item_5 ) { v_Needs_Enum_Item_5 = Needs_Enum_Item_5; }

   bool impl_Needs_Field( ) const { return lazy_fetch( p_obj ), v_Needs_Field; }
   void impl_Needs_Field( bool Needs_Field ) { v_Needs_Field = Needs_Field; }

   bool impl_Needs_Modifier( ) const { return lazy_fetch( p_obj ), v_Needs_Modifier; }
   void impl_Needs_Modifier( bool Needs_Modifier ) { v_Needs_Modifier = Needs_Modifier; }

   bool impl_Needs_Other_Class( ) const { return lazy_fetch( p_obj ), v_Needs_Other_Class; }
   void impl_Needs_Other_Class( bool Needs_Other_Class ) { v_Needs_Other_Class = Needs_Other_Class; }

   bool impl_Needs_Other_Class_Field( ) const { return lazy_fetch( p_obj ), v_Needs_Other_Class_Field; }
   void impl_Needs_Other_Class_Field( bool Needs_Other_Class_Field ) { v_Needs_Other_Class_Field = Needs_Other_Class_Field; }

   bool impl_Needs_Other_Field( ) const { return lazy_fetch( p_obj ), v_Needs_Other_Field; }
   void impl_Needs_Other_Field( bool Needs_Other_Field ) { v_Needs_Other_Field = Needs_Other_Field; }

   bool impl_Needs_Other_Field_2( ) const { return lazy_fetch( p_obj ), v_Needs_Other_Field_2; }
   void impl_Needs_Other_Field_2( bool Needs_Other_Field_2 ) { v_Needs_Other_Field_2 = Needs_Other_Field_2; }

   bool impl_Needs_Other_Modifier( ) const { return lazy_fetch( p_obj ), v_Needs_Other_Modifier; }
   void impl_Needs_Other_Modifier( bool Needs_Other_Modifier ) { v_Needs_Other_Modifier = Needs_Other_Modifier; }

   bool impl_Needs_Other_Modifier_2( ) const { return lazy_fetch( p_obj ), v_Needs_Other_Modifier_2; }
   void impl_Needs_Other_Modifier_2( bool Needs_Other_Modifier_2 ) { v_Needs_Other_Modifier_2 = Needs_Other_Modifier_2; }

   bool impl_Needs_Other_Permission( ) const { return lazy_fetch( p_obj ), v_Needs_Other_Permission; }
   void impl_Needs_Other_Permission( bool Needs_Other_Permission ) { v_Needs_Other_Permission = Needs_Other_Permission; }

   bool impl_Needs_Other_Permission_2( ) const { return lazy_fetch( p_obj ), v_Needs_Other_Permission_2; }
   void impl_Needs_Other_Permission_2( bool Needs_Other_Permission_2 ) { v_Needs_Other_Permission_2 = Needs_Other_Permission_2; }

   bool impl_Needs_Other_Procedure( ) const { return lazy_fetch( p_obj ), v_Needs_Other_Procedure; }
   void impl_Needs_Other_Procedure( bool Needs_Other_Procedure ) { v_Needs_Other_Procedure = Needs_Other_Procedure; }

   bool impl_Needs_Other_Procedure_2( ) const { return lazy_fetch( p_obj ), v_Needs_Other_Procedure_2; }
   void impl_Needs_Other_Procedure_2( bool Needs_Other_Procedure_2 ) { v_Needs_Other_Procedure_2 = Needs_Other_Procedure_2; }

   bool impl_Needs_Other_Source_Child( ) const { return lazy_fetch( p_obj ), v_Needs_Other_Source_Child; }
   void impl_Needs_Other_Source_Child( bool Needs_Other_Source_Child ) { v_Needs_Other_Source_Child = Needs_Other_Source_Child; }

   bool impl_Needs_Other_Source_Child_2( ) const { return lazy_fetch( p_obj ), v_Needs_Other_Source_Child_2; }
   void impl_Needs_Other_Source_Child_2( bool Needs_Other_Source_Child_2 ) { v_Needs_Other_Source_Child_2 = Needs_Other_Source_Child_2; }

   bool impl_Needs_Permission( ) const { return lazy_fetch( p_obj ), v_Needs_Permission; }
   void impl_Needs_Permission( bool Needs_Permission ) { v_Needs_Permission = Needs_Permission; }

   bool impl_Needs_Procedure( ) const { return lazy_fetch( p_obj ), v_Needs_Procedure; }
   void impl_Needs_Procedure( bool Needs_Procedure ) { v_Needs_Procedure = Needs_Procedure; }

   bool impl_Needs_Procedure_Arg( ) const { return lazy_fetch( p_obj ), v_Needs_Procedure_Arg; }
   void impl_Needs_Procedure_Arg( bool Needs_Procedure_Arg ) { v_Needs_Procedure_Arg = Needs_Procedure_Arg; }

   bool impl_Needs_Procedure_Arg_2( ) const { return lazy_fetch( p_obj ), v_Needs_Procedure_Arg_2; }
   void impl_Needs_Procedure_Arg_2( bool Needs_Procedure_Arg_2 ) { v_Needs_Procedure_Arg_2 = Needs_Procedure_Arg_2; }

   bool impl_Needs_Procedure_Arg_3( ) const { return lazy_fetch( p_obj ), v_Needs_Procedure_Arg_3; }
   void impl_Needs_Procedure_Arg_3( bool Needs_Procedure_Arg_3 ) { v_Needs_Procedure_Arg_3 = Needs_Procedure_Arg_3; }

   bool impl_Needs_Source_Child( ) const { return lazy_fetch( p_obj ), v_Needs_Source_Child; }
   void impl_Needs_Source_Child( bool Needs_Source_Child ) { v_Needs_Source_Child = Needs_Source_Child; }

   bool impl_Needs_Source_Field( ) const { return lazy_fetch( p_obj ), v_Needs_Source_Field; }
   void impl_Needs_Source_Field( bool Needs_Source_Field ) { v_Needs_Source_Field = Needs_Source_Field; }

   bool impl_Needs_Source_Field_Or_Child( ) const { return lazy_fetch( p_obj ), v_Needs_Source_Field_Or_Child; }
   void impl_Needs_Source_Field_Or_Child( bool Needs_Source_Field_Or_Child ) { v_Needs_Source_Field_Or_Child = Needs_Source_Field_Or_Child; }

   bool impl_Needs_Source_Parent( ) const { return lazy_fetch( p_obj ), v_Needs_Source_Parent; }
   void impl_Needs_Source_Parent( bool Needs_Source_Parent ) { v_Needs_Source_Parent = Needs_Source_Parent; }

   bool impl_Needs_Test_Field( ) const { return lazy_fetch( p_obj ), v_Needs_Test_Field; }
   void impl_Needs_Test_Field( bool Needs_Test_Field ) { v_Needs_Test_Field = Needs_Test_Field; }

   bool impl_Needs_Test_Value( ) const { return lazy_fetch( p_obj ), v_Needs_Test_Value; }
   void impl_Needs_Test_Value( bool Needs_Test_Value ) { v_Needs_Test_Value = Needs_Test_Value; }

   bool impl_Needs_Value( ) const { return lazy_fetch( p_obj ), v_Needs_Value; }
   void impl_Needs_Value( bool Needs_Value ) { v_Needs_Value = Needs_Value; }

   bool impl_Next_Protect_Child_Rel( ) const { return lazy_fetch( p_obj ), v_Next_Protect_Child_Rel; }
   void impl_Next_Protect_Child_Rel( bool Next_Protect_Child_Rel ) { v_Next_Protect_Child_Rel = Next_Protect_Child_Rel; }

   bool impl_Next_Protect_Procedure( ) const { return lazy_fetch( p_obj ), v_Next_Protect_Procedure; }
   void impl_Next_Protect_Procedure( bool Next_Protect_Procedure ) { v_Next_Protect_Procedure = Next_Protect_Procedure; }

   bool impl_Next_Protect_Source_Parent( ) const { return lazy_fetch( p_obj ), v_Next_Protect_Source_Parent; }
   void impl_Next_Protect_Source_Parent( bool Next_Protect_Source_Parent ) { v_Next_Protect_Source_Parent = Next_Protect_Source_Parent; }

   const string& impl_Next_Specification_Actions( ) const { return lazy_fetch( p_obj ), v_Next_Specification_Actions; }
   void impl_Next_Specification_Actions( const string& Next_Specification_Actions ) { sanity_check( Next_Specification_Actions ); v_Next_Specification_Actions = Next_Specification_Actions; }

   const string& impl_Notes( ) const { return lazy_fetch( p_obj ), v_Notes; }
   void impl_Notes( const string& Notes ) { sanity_check( Notes ); v_Notes = Notes; }

   int impl_Procedure_Arg_primitive( ) const { return lazy_fetch( p_obj ), v_Procedure_Arg_primitive; }
   void impl_Procedure_Arg_primitive( int Procedure_Arg_primitive ) { v_Procedure_Arg_primitive = Procedure_Arg_primitive; }

   int impl_Procedure_Arg_type( ) const { return lazy_fetch( p_obj ), v_Procedure_Arg_type; }
   void impl_Procedure_Arg_type( int Procedure_Arg_type ) { v_Procedure_Arg_type = Procedure_Arg_type; }

   bool impl_Protect_Child_Rel_From_Update( ) const { return lazy_fetch( p_obj ), v_Protect_Child_Rel_From_Update; }
   void impl_Protect_Child_Rel_From_Update( bool Protect_Child_Rel_From_Update ) { v_Protect_Child_Rel_From_Update = Protect_Child_Rel_From_Update; }

   bool impl_Protect_Class_From_Edit( ) const { return lazy_fetch( p_obj ), v_Protect_Class_From_Edit; }
   void impl_Protect_Class_From_Edit( bool Protect_Class_From_Edit ) { v_Protect_Class_From_Edit = Protect_Class_From_Edit; }

   bool impl_Protect_Class_From_Update( ) const { return lazy_fetch( p_obj ), v_Protect_Class_From_Update; }
   void impl_Protect_Class_From_Update( bool Protect_Class_From_Update ) { v_Protect_Class_From_Update = Protect_Class_From_Update; }

   bool impl_Protect_Other_Class_From_Edit( ) const { return lazy_fetch( p_obj ), v_Protect_Other_Class_From_Edit; }
   void impl_Protect_Other_Class_From_Edit( bool Protect_Other_Class_From_Edit ) { v_Protect_Other_Class_From_Edit = Protect_Other_Class_From_Edit; }

   bool impl_Protect_Procedure_From_Edit( ) const { return lazy_fetch( p_obj ), v_Protect_Procedure_From_Edit; }
   void impl_Protect_Procedure_From_Edit( bool Protect_Procedure_From_Edit ) { v_Protect_Procedure_From_Edit = Protect_Procedure_From_Edit; }

   bool impl_Protect_Source_Parent( ) const { return lazy_fetch( p_obj ), v_Protect_Source_Parent; }
   void impl_Protect_Source_Parent( bool Protect_Source_Parent ) { v_Protect_Source_Parent = Protect_Source_Parent; }

   bool impl_Set_Field_To_Source_Field_Or_Child( ) const { return lazy_fetch( p_obj ), v_Set_Field_To_Source_Field_Or_Child; }
   void impl_Set_Field_To_Source_Field_Or_Child( bool Set_Field_To_Source_Field_Or_Child ) { v_Set_Field_To_Source_Field_Or_Child = Set_Field_To_Source_Field_Or_Child; }

   bool impl_Source_Field_Needs_Test_Field( ) const { return lazy_fetch( p_obj ), v_Source_Field_Needs_Test_Field; }
   void impl_Source_Field_Needs_Test_Field( bool Source_Field_Needs_Test_Field ) { v_Source_Field_Needs_Test_Field = Source_Field_Needs_Test_Field; }

   bool impl_Source_Field_Type_Match( ) const { return lazy_fetch( p_obj ), v_Source_Field_Type_Match; }
   void impl_Source_Field_Type_Match( bool Source_Field_Type_Match ) { v_Source_Field_Type_Match = Source_Field_Type_Match; }

   int impl_Source_Parent_type( ) const { return lazy_fetch( p_obj ), v_Source_Parent_type; }
   void impl_Source_Parent_type( int Source_Parent_type ) { v_Source_Parent_type = Source_Parent_type; }

   int impl_Source_type( ) const { return lazy_fetch( p_obj ), v_Source_type; }
   void impl_Source_type( int Source_type ) { v_Source_type = Source_type; }

   const string& impl_Specification_Actions( ) const { return lazy_fetch( p_obj ), v_Specification_Actions; }
   void impl_Specification_Actions( const string& Specification_Actions ) { sanity_check( Specification_Actions ); v_Specification_Actions = Specification_Actions; }

   const string& impl_Specification_Name( ) const { return lazy_fetch( p_obj ), v_Specification_Name; }
   void impl_Specification_Name( const string& Specification_Name ) { sanity_check( Specification_Name ); v_Specification_Name = Specification_Name; }

   const string& impl_Specification_Object( ) const { return lazy_fetch( p_obj ), v_Specification_Object; }
   void impl_Specification_Object( const string& Specification_Object ) { sanity_check( Specification_Object ); v_Specification_Object = Specification_Object; }

   const string& impl_Specification_Strings( ) const { return lazy_fetch( p_obj ), v_Specification_Strings; }
   void impl_Specification_Strings( const string& Specification_Strings ) { sanity_check( Specification_Strings ); v_Specification_Strings = Specification_Strings; }

   const string& impl_Specification_Vars( ) const { return lazy_fetch( p_obj ), v_Specification_Vars; }
   void impl_Specification_Vars( const string& Specification_Vars ) { sanity_check( Specification_Vars ); v_Specification_Vars = Specification_Vars; }

   bool impl_Test_Field_Type_Match( ) const { return lazy_fetch( p_obj ), v_Test_Field_Type_Match; }
   void impl_Test_Field_Type_Match( bool Test_Field_Type_Match ) { v_Test_Field_Type_Match = Test_Field_Type_Match; }

   int impl_Test_Field_type( ) const { return lazy_fetch( p_obj ), v_Test_Field_type; }
   void impl_Test_Field_type( int Test_Field_type ) { v_Test_Field_type = Test_Field_type; }

   bool impl_Use_Class_As_Source_Parent_Class( ) const { return lazy_fetch( p_obj ), v_Use_Class_As_Source_Parent_Class; }
   void impl_Use_Class_As_Source_Parent_Class( bool Use_Class_As_Source_Parent_Class ) { v_Use_Class_As_Source_Parent_Class = Use_Class_As_Source_Parent_Class; }

   bool impl_Use_Field_Enum( ) const { return lazy_fetch( p_obj ), v_Use_Field_Enum; }
   void impl_Use_Field_Enum( bool Use_Field_Enum ) { v_Use_Field_Enum = Use_Field_Enum; }

   bool impl_Use_Parent_Child_Rel_As_Source_Parent( ) const { return lazy_fetch( p_obj ), v_Use_Parent_Child_Rel_As_Source_Parent; }
   void impl_Use_Parent_Child_Rel_As_Source_Parent( bool Use_Parent_Child_Rel_As_Source_Parent ) { v_Use_Parent_Child_Rel_As_Source_Parent = Use_Parent_Child_Rel_As_Source_Parent; }

   bool impl_Use_Parent_Child_Rel_For_Class( ) const { return lazy_fetch( p_obj ), v_Use_Parent_Child_Rel_For_Class; }
   void impl_Use_Parent_Child_Rel_For_Class( bool Use_Parent_Child_Rel_For_Class ) { v_Use_Parent_Child_Rel_For_Class = Use_Parent_Child_Rel_For_Class; }

   bool impl_Use_Parent_Child_Rel_For_Other_Class( ) const { return lazy_fetch( p_obj ), v_Use_Parent_Child_Rel_For_Other_Class; }
   void impl_Use_Parent_Child_Rel_For_Other_Class( bool Use_Parent_Child_Rel_For_Other_Class ) { v_Use_Parent_Child_Rel_For_Other_Class = Use_Parent_Child_Rel_For_Other_Class; }

   bool impl_Use_Parent_Class( ) const { return lazy_fetch( p_obj ), v_Use_Parent_Class; }
   void impl_Use_Parent_Class( bool Use_Parent_Class ) { v_Use_Parent_Class = Use_Parent_Class; }

   bool impl_Use_Parent_Field_For_Class( ) const { return lazy_fetch( p_obj ), v_Use_Parent_Field_For_Class; }
   void impl_Use_Parent_Field_For_Class( bool Use_Parent_Field_For_Class ) { v_Use_Parent_Field_For_Class = Use_Parent_Field_For_Class; }

   bool impl_Use_Parent_Other_Class( ) const { return lazy_fetch( p_obj ), v_Use_Parent_Other_Class; }
   void impl_Use_Parent_Other_Class( bool Use_Parent_Other_Class ) { v_Use_Parent_Other_Class = Use_Parent_Other_Class; }

   bool impl_Use_Parent_Other_Class_For_Class( ) const { return lazy_fetch( p_obj ), v_Use_Parent_Other_Class_For_Class; }
   void impl_Use_Parent_Other_Class_For_Class( bool Use_Parent_Other_Class_For_Class ) { v_Use_Parent_Other_Class_For_Class = Use_Parent_Other_Class_For_Class; }

   bool impl_Use_Parent_Procedure( ) const { return lazy_fetch( p_obj ), v_Use_Parent_Procedure; }
   void impl_Use_Parent_Procedure( bool Use_Parent_Procedure ) { v_Use_Parent_Procedure = Use_Parent_Procedure; }

   bool impl_Use_Parent_Source_Class( ) const { return lazy_fetch( p_obj ), v_Use_Parent_Source_Class; }
   void impl_Use_Parent_Source_Class( bool Use_Parent_Source_Class ) { v_Use_Parent_Source_Class = Use_Parent_Source_Class; }

   bool impl_Use_Parent_Source_Field_For_Class( ) const { return lazy_fetch( p_obj ), v_Use_Parent_Source_Field_For_Class; }
   void impl_Use_Parent_Source_Field_For_Class( bool Use_Parent_Source_Field_For_Class ) { v_Use_Parent_Source_Field_For_Class = Use_Parent_Source_Field_For_Class; }

   bool impl_Use_Parent_Source_Parent( ) const { return lazy_fetch( p_obj ), v_Use_Parent_Source_Parent; }
   void impl_Use_Parent_Source_Parent( bool Use_Parent_Source_Parent ) { v_Use_Parent_Source_Parent = Use_Parent_Source_Parent; }

   bool impl_Use_Parent_Source_Parent_For_Class( ) const { return lazy_fetch( p_obj ), v_Use_Parent_Source_Parent_For_Class; }
   void impl_Use_Parent_Source_Parent_For_Class( bool Use_Parent_Source_Parent_For_Class ) { v_Use_Parent_Source_Parent_For_Class = Use_Parent_Source_Parent_For_Class; }

   bool impl_Use_Source_Field_Enum( ) const { return lazy_fetch( p_obj ), v_Use_Source_Field_Enum; }
   void impl_Use_Source_Field_Enum( bool Use_Source_Field_Enum ) { v_Use_Source_Field_Enum = Use_Source_Field_Enum; }

   bool impl_Use_Source_Field_Or_Child_Enum( ) const { return lazy_fetch( p_obj ), v_Use_Source_Field_Or_Child_Enum; }
   void impl_Use_Source_Field_Or_Child_Enum( bool Use_Source_Field_Or_Child_Enum ) { v_Use_Source_Field_Or_Child_Enum = Use_Source_Field_Or_Child_Enum; }

   const string& impl_View_Id( ) const { return lazy_fetch( p_obj ), v_View_Id; }
   void impl_View_Id( const string& View_Id ) { sanity_check( View_Id ); v_View_Id = View_Id; }

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

   Meta_Specification_Type& impl_Next_Child_Specification_Type( )
   {
      if( !cp_Next_Child_Specification_Type )
      {
         cp_Next_Child_Specification_Type.init( );

         p_obj->setup_graph_parent( *cp_Next_Child_Specification_Type, c_field_id_Next_Child_Specification_Type, v_Next_Child_Specification_Type );
      }
      return *cp_Next_Child_Specification_Type;
   }

   const Meta_Specification_Type& impl_Next_Child_Specification_Type( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Next_Child_Specification_Type )
      {
         cp_Next_Child_Specification_Type.init( );

         p_obj->setup_graph_parent( *cp_Next_Child_Specification_Type, c_field_id_Next_Child_Specification_Type, v_Next_Child_Specification_Type );
      }
      return *cp_Next_Child_Specification_Type;
   }

   void impl_Next_Child_Specification_Type( const string& key )
   {
      class_base_accessor cba( impl_Next_Child_Specification_Type( ) );
      cba.set_key( key );
   }

   Meta_Specification_Type& impl_Parent_Specification_Type( )
   {
      if( !cp_Parent_Specification_Type )
      {
         cp_Parent_Specification_Type.init( );

         p_obj->setup_graph_parent( *cp_Parent_Specification_Type, c_field_id_Parent_Specification_Type, v_Parent_Specification_Type );
      }
      return *cp_Parent_Specification_Type;
   }

   const Meta_Specification_Type& impl_Parent_Specification_Type( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Parent_Specification_Type )
      {
         cp_Parent_Specification_Type.init( );

         p_obj->setup_graph_parent( *cp_Parent_Specification_Type, c_field_id_Parent_Specification_Type, v_Parent_Specification_Type );
      }
      return *cp_Parent_Specification_Type;
   }

   void impl_Parent_Specification_Type( const string& key )
   {
      class_base_accessor cba( impl_Parent_Specification_Type( ) );
      cba.set_key( key );
   }

   Meta_Specification_Type& impl_child_Specification_Type_Child( )
   {
      if( !cp_child_Specification_Type_Child )
      {
         cp_child_Specification_Type_Child.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Type_Child, "301710" );
      }
      return *cp_child_Specification_Type_Child;
   }

   const Meta_Specification_Type& impl_child_Specification_Type_Child( ) const
   {
      if( !cp_child_Specification_Type_Child )
      {
         cp_child_Specification_Type_Child.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Type_Child, "301710" );
      }
      return *cp_child_Specification_Type_Child;
   }

   Meta_Specification& impl_child_Specification_Child( )
   {
      if( !cp_child_Specification_Child )
      {
         cp_child_Specification_Child.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Child, "301406" );
      }
      return *cp_child_Specification_Child;
   }

   const Meta_Specification& impl_child_Specification_Child( ) const
   {
      if( !cp_child_Specification_Child )
      {
         cp_child_Specification_Child.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Child, "301406" );
      }
      return *cp_child_Specification_Child;
   }

   Meta_Specification_Type& impl_child_Specification_Type_Next_Child( )
   {
      if( !cp_child_Specification_Type_Next_Child )
      {
         cp_child_Specification_Type_Next_Child.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Type_Next_Child, "301720" );
      }
      return *cp_child_Specification_Type_Next_Child;
   }

   const Meta_Specification_Type& impl_child_Specification_Type_Next_Child( ) const
   {
      if( !cp_child_Specification_Type_Next_Child )
      {
         cp_child_Specification_Type_Next_Child.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Type_Next_Child, "301720" );
      }
      return *cp_child_Specification_Type_Next_Child;
   }

   Meta_Specification_Type& impl_child_Specification_Type_Parent( )
   {
      if( !cp_child_Specification_Type_Parent )
      {
         cp_child_Specification_Type_Parent.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Type_Parent, "301700" );
      }
      return *cp_child_Specification_Type_Parent;
   }

   const Meta_Specification_Type& impl_child_Specification_Type_Parent( ) const
   {
      if( !cp_child_Specification_Type_Parent )
      {
         cp_child_Specification_Type_Parent.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Type_Parent, "301700" );
      }
      return *cp_child_Specification_Type_Parent;
   }

   Meta_Specification& impl_child_Specification( )
   {
      if( !cp_child_Specification )
      {
         cp_child_Specification.init( );

         p_obj->setup_graph_parent( *cp_child_Specification, "301410" );
      }
      return *cp_child_Specification;
   }

   const Meta_Specification& impl_child_Specification( ) const
   {
      if( !cp_child_Specification )
      {
         cp_child_Specification.init( );

         p_obj->setup_graph_parent( *cp_child_Specification, "301410" );
      }
      return *cp_child_Specification;
   }

   void impl_Is_Valid_Field_Type( const string& Class,
    string& Error, const string& Parent_Class, int Primitive, int Procedure_Arg_Primitive,
    int Procedure_Arg_Type, const string& Source_Parent_Class, int Source_Primitive, int Test_Primitive, const string& Test_Value );

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

   Meta_Specification_Type* p_obj;
   class_pointer< Meta_Specification_Type > cp_obj;

   mutable set< string > required_transients;

   // [<start members>]
   // [<finish members>]

   size_t total_child_relationships;

   bool v_Allow_Child_Relationship;
   bool v_Allow_Enum;
   bool v_Allow_Enum_Item;
   bool v_Allow_FK_Source_Field_Choice;
   bool v_Allow_FK_Test_Field_Choice;
   bool v_Allow_Field;
   bool v_Allow_Modifier;
   bool v_Allow_Options;
   bool v_Allow_Other_Class;
   bool v_Allow_Other_Class_Field;
   bool v_Allow_Other_Field;
   bool v_Allow_Other_Field_2;
   bool v_Allow_Other_Modifier;
   bool v_Allow_Other_Modifier_2;
   bool v_Allow_Other_Procedure;
   bool v_Allow_Other_Procedure_2;
   bool v_Allow_Other_Source_Child;
   bool v_Allow_Other_Source_Child_2;
   bool v_Allow_Permission;
   bool v_Allow_Procedure;
   bool v_Allow_Procedure_Arg;
   bool v_Allow_Source_Child;
   bool v_Allow_Source_Class;
   bool v_Allow_Source_Field;
   bool v_Allow_Source_Grandchild;
   bool v_Allow_Source_Parent;
   bool v_Allow_Test_Field;
   bool v_Allow_Test_Value;
   bool v_Allow_Value;
   bool v_Can_Access_Below_Model;
   string v_Child_Prefix;
   bool v_Child_Rel_Grandparent_Match;
   bool v_Child_Rel_Parent_Match;
   bool v_Child_Relationship_Class_Match;
   string v_Default_Child_Vars;
   int v_Field_type;
   bool v_Has_Next_Specification_Info;
   bool v_Is_Child_Only;
   bool v_Is_Required_For_UI_Gen;
   bool v_Is_System;
   string v_Name;
   bool v_Needs_Child_Relationship;
   bool v_Needs_Class;
   bool v_Needs_Enum;
   bool v_Needs_Enum_Item;
   bool v_Needs_Enum_Item_2;
   bool v_Needs_Enum_Item_3;
   bool v_Needs_Enum_Item_4;
   bool v_Needs_Enum_Item_5;
   bool v_Needs_Field;
   bool v_Needs_Modifier;
   bool v_Needs_Other_Class;
   bool v_Needs_Other_Class_Field;
   bool v_Needs_Other_Field;
   bool v_Needs_Other_Field_2;
   bool v_Needs_Other_Modifier;
   bool v_Needs_Other_Modifier_2;
   bool v_Needs_Other_Permission;
   bool v_Needs_Other_Permission_2;
   bool v_Needs_Other_Procedure;
   bool v_Needs_Other_Procedure_2;
   bool v_Needs_Other_Source_Child;
   bool v_Needs_Other_Source_Child_2;
   bool v_Needs_Permission;
   bool v_Needs_Procedure;
   bool v_Needs_Procedure_Arg;
   bool v_Needs_Procedure_Arg_2;
   bool v_Needs_Procedure_Arg_3;
   bool v_Needs_Source_Child;
   bool v_Needs_Source_Field;
   bool v_Needs_Source_Field_Or_Child;
   bool v_Needs_Source_Parent;
   bool v_Needs_Test_Field;
   bool v_Needs_Test_Value;
   bool v_Needs_Value;
   bool v_Next_Protect_Child_Rel;
   bool v_Next_Protect_Procedure;
   bool v_Next_Protect_Source_Parent;
   string v_Next_Specification_Actions;
   string v_Notes;
   int v_Procedure_Arg_primitive;
   int v_Procedure_Arg_type;
   bool v_Protect_Child_Rel_From_Update;
   bool v_Protect_Class_From_Edit;
   bool v_Protect_Class_From_Update;
   bool v_Protect_Other_Class_From_Edit;
   bool v_Protect_Procedure_From_Edit;
   bool v_Protect_Source_Parent;
   bool v_Set_Field_To_Source_Field_Or_Child;
   bool v_Source_Field_Needs_Test_Field;
   bool v_Source_Field_Type_Match;
   int v_Source_Parent_type;
   int v_Source_type;
   string v_Specification_Actions;
   string v_Specification_Name;
   string v_Specification_Object;
   string v_Specification_Strings;
   string v_Specification_Vars;
   bool v_Test_Field_Type_Match;
   int v_Test_Field_type;
   bool v_Use_Class_As_Source_Parent_Class;
   bool v_Use_Field_Enum;
   bool v_Use_Parent_Child_Rel_As_Source_Parent;
   bool v_Use_Parent_Child_Rel_For_Class;
   bool v_Use_Parent_Child_Rel_For_Other_Class;
   bool v_Use_Parent_Class;
   bool v_Use_Parent_Field_For_Class;
   bool v_Use_Parent_Other_Class;
   bool v_Use_Parent_Other_Class_For_Class;
   bool v_Use_Parent_Procedure;
   bool v_Use_Parent_Source_Class;
   bool v_Use_Parent_Source_Field_For_Class;
   bool v_Use_Parent_Source_Parent;
   bool v_Use_Parent_Source_Parent_For_Class;
   bool v_Use_Source_Field_Enum;
   bool v_Use_Source_Field_Or_Child_Enum;
   string v_View_Id;

   string v_Child_Specification_Type;
   mutable class_pointer< Meta_Specification_Type > cp_Child_Specification_Type;

   string v_Next_Child_Specification_Type;
   mutable class_pointer< Meta_Specification_Type > cp_Next_Child_Specification_Type;

   string v_Parent_Specification_Type;
   mutable class_pointer< Meta_Specification_Type > cp_Parent_Specification_Type;

   mutable class_pointer< Meta_Specification_Type > cp_child_Specification_Type_Child;
   mutable class_pointer< Meta_Specification > cp_child_Specification_Child;
   mutable class_pointer< Meta_Specification_Type > cp_child_Specification_Type_Next_Child;
   mutable class_pointer< Meta_Specification_Type > cp_child_Specification_Type_Parent;
   mutable class_pointer< Meta_Specification > cp_child_Specification;
};

void Meta_Specification_Type::impl::impl_Is_Valid_Field_Type( const string& Class,
 string& Error, const string& Parent_Class, int Primitive, int Procedure_Arg_Primitive,
 int Procedure_Arg_Type, const string& Source_Parent_Class, int Source_Primitive, int Test_Primitive, const string& Test_Value )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start meta_trait_val_proc)] 600211
   if( get_obj( ).Field_type( ) != c_enum_field_type_Any_Type )
   {
      if( get_obj( ).Field_type( ) == c_enum_field_type_Any_Primitive )
      {
         if( !Parent_Class.empty( ) )
            Error = GMS( "field_specification_type_field_type" ) + " must be a primitive"; // FUTURE: Should be in module_strings...
      }
      else if( get_obj( ).Field_type( ) == c_enum_field_type_Any_Relationship )
      {
         if( Parent_Class.empty( ) )
            Error = GMS( "field_specification_type_field_type" ) + " must be a relationship"; // FUTURE: Should be in module_strings...
      }
      else if( get_obj( ).Field_type( ) == c_enum_field_type_Self_Relationship )
      {
         if( Parent_Class.empty( ) || Parent_Class != Class )
            Error = GMS( "field_specification_type_field_type" ) + " must be a self-relationship"; // FUTURE: Should be in module_strings...
      }
      else if( get_obj( ).Field_type( ) == c_enum_field_type_Non_Self_Relationship )
      {
         if( Parent_Class.empty( ) || Parent_Class == Class )
            Error = GMS( "field_specification_type_field_type" ) + " must be a (non-self) relationship"; // FUTURE: Should be in module_strings...
      }
      else if( get_obj( ).Field_type( ) == c_enum_field_type_Any_Text_Type )
      {
         if( Primitive == c_enum_field_type_bool
          || Primitive == c_enum_field_type_int || Primitive == c_enum_field_type_numeric )
            Error = GMS( "field_specification_type_field_type" ) + " must be a text type"; // FUTURE: Should be in module_strings...
      }
      else if( get_obj( ).Field_type( ) == c_enum_field_type_Any_Scalar_Type )
      {
         bool in_error = true;
         if( Parent_Class.empty( ) )
         {
            if( Primitive == c_enum_field_type_datetime
             || Primitive == c_enum_field_type_date || Primitive == c_enum_field_type_time
             || Primitive == c_enum_field_type_numeric || Primitive == c_enum_field_type_int || Primitive == c_enum_field_type_bool )
               in_error = false;
         }

         if( in_error )
            Error = GMS( "field_specification_type_field_type" ) + " must be a scalar type"; // FUTURE: Should be in module_strings...
      }
      else if( get_obj( ).Field_type( ) == c_enum_field_type_Any_Math_Type )
      {
         bool in_error = true;
         if( Parent_Class.empty( ) )
         {
            if( Primitive == c_enum_field_type_numeric
             || Primitive == c_enum_field_type_int || Primitive == c_enum_field_type_bool )
               in_error = false;
         }

         if( in_error )
            Error = GMS( "field_specification_type_field_type" ) + " must be a math type"; // FUTURE: Should be in module_strings...
      }
      else if( get_obj( ).Field_type( ) == c_enum_field_type_Calendar_Type )
      {
         bool in_error = true;
         if( Parent_Class.empty( ) )
         {
            if( Primitive == c_enum_field_type_datetime || Primitive == c_enum_field_type_date )
               in_error = false;
         }

         if( in_error )
            Error = GMS( "field_specification_type_field_type" ) + " must be a calendar type"; // FUTURE: Should be in module_strings...
      }
      else if( get_obj( ).Field_type( ) < c_enum_field_type_Any_Text_Type ) // i.e. all primitives
      {
         if( !Parent_Class.empty( )
          || ( Primitive >= 0 && Primitive != get_obj( ).Field_type( ) ) )
         {
            string primitive_name;

            if( get_obj( ).Field_type( ) == c_enum_field_type_string )
               primitive_name = GMS( get_enum_string_field_type( c_enum_field_type_string ) );
            else if( get_obj( ).Field_type( ) == c_enum_field_type_datetime )
               primitive_name = GMS( get_enum_string_field_type( c_enum_field_type_datetime ) );
            else if( get_obj( ).Field_type( ) == c_enum_field_type_date )
               primitive_name = GMS( get_enum_string_field_type( c_enum_field_type_date ) );
            else if( get_obj( ).Field_type( ) == c_enum_field_type_time )
               primitive_name = GMS( get_enum_string_field_type( c_enum_field_type_time ) );
            else if( get_obj( ).Field_type( ) == c_enum_field_type_numeric )
               primitive_name = GMS( get_enum_string_field_type( c_enum_field_type_numeric ) );
            else if( get_obj( ).Field_type( ) == c_enum_field_type_int )
               primitive_name = GMS( get_enum_string_field_type( c_enum_field_type_int ) );
            else if( get_obj( ).Field_type( ) == c_enum_field_type_bool )
               primitive_name = GMS( get_enum_string_field_type( c_enum_field_type_bool ) );

            Error = GMS( "field_specification_type_field_type" ) + " must be a " + primitive_name + " primitive"; // FUTURE: Should be in module_strings...
         }
      }
      else
         throw runtime_error( "unexpected Field_type value " + to_string( get_obj( ).Field_type( ) ) + " in Is_Valid_Field_Type" );
   }
   // [(finish meta_trait_val_proc)] 600211

   // [(start meta_trait_val_proc)] 600212
   if( Source_Primitive >= 0 && get_obj( ).Source_type( ) != c_enum_source_type_Any_Type )
   {
      if( get_obj( ).Source_type( ) == c_enum_source_type_Any_Text_Type )
      {
         if( Source_Primitive >= c_enum_source_type_numeric )
            Error = GMS( "field_specification_type_source_type" ) + " must be a text type"; // FUTURE: Should be in module_strings...
      }
      else if( get_obj( ).Source_type( ) == c_enum_source_type_Any_Conditional )
      {
         if( Source_Primitive < c_enum_source_type_int || Source_Primitive > c_enum_source_type_bool )
            Error = GMS( "field_specification_type_source_type" ) + " must be a conditional type"; // FUTURE: Should be in module_strings...
      }
      else if( get_obj( ).Source_type( ) < c_enum_source_type_Any_Text_Type ) // i.e. all primitives
      {
         if( Source_Primitive != get_obj( ).Source_type( ) )
         {
            string primitive_name;

            if( get_obj( ).Source_type( ) == c_enum_source_type_string )
               primitive_name = GMS( get_enum_string_field_type( c_enum_source_type_string ) );
            else if( get_obj( ).Source_type( ) == c_enum_source_type_datetime )
               primitive_name = GMS( get_enum_string_field_type( c_enum_source_type_datetime ) );
            else if( get_obj( ).Source_type( ) == c_enum_source_type_date )
               primitive_name = GMS( get_enum_string_field_type( c_enum_source_type_date ) );
            else if( get_obj( ).Source_type( ) == c_enum_source_type_time )
               primitive_name = GMS( get_enum_string_field_type( c_enum_source_type_time ) );
            else if( get_obj( ).Source_type( ) == c_enum_source_type_numeric )
               primitive_name = GMS( get_enum_string_field_type( c_enum_source_type_numeric ) );
            else if( get_obj( ).Source_type( ) == c_enum_source_type_int )
               primitive_name = GMS( get_enum_string_field_type( c_enum_source_type_int ) );
            else if( get_obj( ).Source_type( ) == c_enum_source_type_bool )
               primitive_name = GMS( get_enum_string_field_type( c_enum_source_type_bool ) );

            Error = GMS( "field_specification_type_source_type" ) + " must be a " + primitive_name + " primitive"; // FUTURE: Should be in module_strings...
         }
      }
      else
         throw runtime_error( "unexpected Source_type value " + to_string( get_obj( ).Source_type( ) ) + " in Is_Valid_Field_Type" );
   }
   // [(finish meta_trait_val_proc)] 600212

   // [(start meta_trait_val_proc)] 600220
   if( Test_Primitive >= 0 && get_obj( ).Test_Field_type( ) != c_enum_test_field_type_Any_Type )
   {
      if( get_obj( ).Test_Field_type( ) == c_enum_test_field_type_Any_Text_Type )
      {
         if( Test_Primitive >= c_enum_test_field_type_numeric )
            Error = GMS( "field_specification_type_test_field_type" ) + " must be a text type"; // FUTURE: Should be in module_strings...
      }
      else if( get_obj( ).Test_Field_type( ) == c_enum_test_field_type_Any_Conditional )
      {
         if( Test_Primitive < c_enum_test_field_type_int || Test_Primitive > c_enum_test_field_type_bool )
            Error = GMS( "field_specification_type_test_field_type" ) + " must be a conditional type"; // FUTURE: Should be in module_strings...
      }
      else if( get_obj( ).Test_Field_type( ) == c_enum_test_field_type_Any_Value_Or_Conditional )
      {
         if( Test_Value.empty( )
          && ( Test_Primitive < c_enum_test_field_type_int || Test_Primitive > c_enum_test_field_type_bool ) )
            Error = GMS( "field_specification_type_test_field_type" ) + " must be a conditional type if no value provided"; // FUTURE: Should be in module_strings...
      }
      else if( get_obj( ).Test_Field_type( ) == c_enum_test_field_type_Any_Scalar_Type )
      {
         bool in_error = true;

         if( Test_Primitive == c_enum_test_field_type_datetime
          || Test_Primitive == c_enum_test_field_type_date || Test_Primitive == c_enum_test_field_type_time
          || Test_Primitive == c_enum_test_field_type_numeric || Test_Primitive == c_enum_test_field_type_int || Test_Primitive == c_enum_test_field_type_bool )
            in_error = false;

         if( in_error )
            Error = GMS( "field_specification_type_test_field_type" ) + " must be a scalar type"; // FUTURE: Should be in module_strings...
      }
      else if( get_obj( ).Test_Field_type( ) < c_enum_test_field_type_Any_Text_Type ) // i.e. all primitives
      {
         if( Test_Primitive != get_obj( ).Test_Field_type( ) )
         {
            string primitive_name;

            if( get_obj( ).Test_Field_type( ) == c_enum_test_field_type_string )
               primitive_name = GMS( get_enum_string_field_type( c_enum_test_field_type_string ) );
            else if( get_obj( ).Test_Field_type( ) == c_enum_test_field_type_datetime )
               primitive_name = GMS( get_enum_string_field_type( c_enum_test_field_type_datetime ) );
            else if( get_obj( ).Test_Field_type( ) == c_enum_test_field_type_date )
               primitive_name = GMS( get_enum_string_field_type( c_enum_test_field_type_date ) );
            else if( get_obj( ).Test_Field_type( ) == c_enum_test_field_type_time )
               primitive_name = GMS( get_enum_string_field_type( c_enum_test_field_type_time ) );
            else if( get_obj( ).Test_Field_type( ) == c_enum_test_field_type_numeric )
               primitive_name = GMS( get_enum_string_field_type( c_enum_test_field_type_numeric ) );
            else if( get_obj( ).Test_Field_type( ) == c_enum_test_field_type_int )
               primitive_name = GMS( get_enum_string_field_type( c_enum_test_field_type_int ) );
            else if( get_obj( ).Test_Field_type( ) == c_enum_test_field_type_bool )
               primitive_name = GMS( get_enum_string_field_type( c_enum_test_field_type_bool ) );

            Error = GMS( "field_specification_type_test_field_type" ) + " must be a " + primitive_name + " primitive"; // FUTURE: Should be in module_strings...
         }
      }
      else
         throw runtime_error( "unexpected Test_Field_type value " + to_string( get_obj( ).Test_Field_type( ) ) + " in Is_Valid_Field_Type" );
   }
   // [(finish meta_trait_val_proc)] 600220

   // [(start meta_trait_val_proc)] 600230
   if( get_obj( ).Source_Parent_type( ) != c_enum_source_parent_type_Any_Relationship )
   {
      if( get_obj( ).Source_Parent_type( ) == c_enum_source_parent_type_Self_Relationship )
      {
         if( !Source_Parent_Class.empty( ) && Source_Parent_Class != Class )
            Error = GMS( "field_specification_type_source_parent_type" ) + " must be a self-relationship"; // FUTURE: Should be in module_strings...
      }
      else if( get_obj( ).Source_Parent_type( ) == c_enum_source_parent_type_Non_Self_Relationship )
      {
         if( !Source_Parent_Class.empty( ) && Source_Parent_Class == Class )
            Error = GMS( "field_specification_type_source_parent_type" ) + " must be a (non-self) relationship"; // FUTURE: Should be in module_strings...
      }
      else
         throw runtime_error( "unexpected Source_Parent_type value " + to_string( get_obj( ).Source_Parent_type( ) ) + " in Is_Valid_Field_Type" );
   }
   // [(finish meta_trait_val_proc)] 600230

   // [(start meta_trait_val_proc)] 600240
   if( get_obj( ).Procedure_Arg_primitive( ) != c_enum_procedure_arg_primitive_Any_Primitive )
   {
      if( Procedure_Arg_Primitive != get_obj( ).Procedure_Arg_primitive( ) )
      {
         string primitive_name;

         if( get_obj( ).Procedure_Arg_primitive( ) == c_enum_procedure_arg_primitive_string )
            primitive_name = GMS( get_enum_string_field_type( c_enum_procedure_arg_primitive_string ) );
         else if( get_obj( ).Procedure_Arg_primitive( ) == c_enum_procedure_arg_primitive_datetime )
            primitive_name = GMS( get_enum_string_field_type( c_enum_procedure_arg_primitive_datetime ) );
         else if( get_obj( ).Procedure_Arg_primitive( ) == c_enum_procedure_arg_primitive_date )
            primitive_name = GMS( get_enum_string_field_type( c_enum_procedure_arg_primitive_date ) );
         else if( get_obj( ).Procedure_Arg_primitive( ) == c_enum_procedure_arg_primitive_time )
            primitive_name = GMS( get_enum_string_field_type( c_enum_procedure_arg_primitive_time ) );
         else if( get_obj( ).Procedure_Arg_primitive( ) == c_enum_procedure_arg_primitive_numeric )
            primitive_name = GMS( get_enum_string_field_type( c_enum_procedure_arg_primitive_numeric ) );
         else if( get_obj( ).Procedure_Arg_primitive( ) == c_enum_procedure_arg_primitive_int )
            primitive_name = GMS( get_enum_string_field_type( c_enum_procedure_arg_primitive_int ) );
         else if( get_obj( ).Procedure_Arg_primitive( ) == c_enum_procedure_arg_primitive_bool )
            primitive_name = GMS( get_enum_string_field_type( c_enum_procedure_arg_primitive_bool ) );

         Error = GMS( "field_specification_type_procedure_arg_primitive" ) + " must be a " + primitive_name + " primitive"; // FUTURE: Should be in module_strings...
      }
   }
   // [(finish meta_trait_val_proc)] 600240

   // [(start meta_trait_val_proc)] 600250
   if( get_obj( ).Procedure_Arg_type( ) != c_enum_procedure_arg_type_Any_Type )
   {
      if( Procedure_Arg_Type != get_obj( ).Procedure_Arg_type( ) )
      {
         string type_name;

         if( get_obj( ).Procedure_Arg_type( ) == c_enum_procedure_arg_type_in )
            type_name = GMS( get_enum_string_field_type( c_enum_procedure_arg_type_in ) );
         else if( get_obj( ).Procedure_Arg_type( ) == c_enum_procedure_arg_type_out )
            type_name = GMS( get_enum_string_field_type( c_enum_procedure_arg_type_out ) );

         Error = GMS( "field_specification_type_procedure_arg_type" ) + " must be type " + type_name; // FUTURE: Should be in module_strings...
      }
   }
   // [(finish meta_trait_val_proc)] 600250

   // [<start Is_Valid_Field_Type_impl>]
   // [<finish Is_Valid_Field_Type_impl>]
}

string Meta_Specification_Type::impl::get_field_value( int field ) const
{
   string retval;

   switch( field )
   {
      case 0:
      retval = to_string( impl_Allow_Child_Relationship( ) );
      break;

      case 1:
      retval = to_string( impl_Allow_Enum( ) );
      break;

      case 2:
      retval = to_string( impl_Allow_Enum_Item( ) );
      break;

      case 3:
      retval = to_string( impl_Allow_FK_Source_Field_Choice( ) );
      break;

      case 4:
      retval = to_string( impl_Allow_FK_Test_Field_Choice( ) );
      break;

      case 5:
      retval = to_string( impl_Allow_Field( ) );
      break;

      case 6:
      retval = to_string( impl_Allow_Modifier( ) );
      break;

      case 7:
      retval = to_string( impl_Allow_Options( ) );
      break;

      case 8:
      retval = to_string( impl_Allow_Other_Class( ) );
      break;

      case 9:
      retval = to_string( impl_Allow_Other_Class_Field( ) );
      break;

      case 10:
      retval = to_string( impl_Allow_Other_Field( ) );
      break;

      case 11:
      retval = to_string( impl_Allow_Other_Field_2( ) );
      break;

      case 12:
      retval = to_string( impl_Allow_Other_Modifier( ) );
      break;

      case 13:
      retval = to_string( impl_Allow_Other_Modifier_2( ) );
      break;

      case 14:
      retval = to_string( impl_Allow_Other_Procedure( ) );
      break;

      case 15:
      retval = to_string( impl_Allow_Other_Procedure_2( ) );
      break;

      case 16:
      retval = to_string( impl_Allow_Other_Source_Child( ) );
      break;

      case 17:
      retval = to_string( impl_Allow_Other_Source_Child_2( ) );
      break;

      case 18:
      retval = to_string( impl_Allow_Permission( ) );
      break;

      case 19:
      retval = to_string( impl_Allow_Procedure( ) );
      break;

      case 20:
      retval = to_string( impl_Allow_Procedure_Arg( ) );
      break;

      case 21:
      retval = to_string( impl_Allow_Source_Child( ) );
      break;

      case 22:
      retval = to_string( impl_Allow_Source_Class( ) );
      break;

      case 23:
      retval = to_string( impl_Allow_Source_Field( ) );
      break;

      case 24:
      retval = to_string( impl_Allow_Source_Grandchild( ) );
      break;

      case 25:
      retval = to_string( impl_Allow_Source_Parent( ) );
      break;

      case 26:
      retval = to_string( impl_Allow_Test_Field( ) );
      break;

      case 27:
      retval = to_string( impl_Allow_Test_Value( ) );
      break;

      case 28:
      retval = to_string( impl_Allow_Value( ) );
      break;

      case 29:
      retval = to_string( impl_Can_Access_Below_Model( ) );
      break;

      case 30:
      retval = to_string( impl_Child_Prefix( ) );
      break;

      case 31:
      retval = to_string( impl_Child_Rel_Grandparent_Match( ) );
      break;

      case 32:
      retval = to_string( impl_Child_Rel_Parent_Match( ) );
      break;

      case 33:
      retval = to_string( impl_Child_Relationship_Class_Match( ) );
      break;

      case 34:
      retval = to_string( impl_Child_Specification_Type( ) );
      break;

      case 35:
      retval = to_string( impl_Default_Child_Vars( ) );
      break;

      case 36:
      retval = to_string( impl_Field_type( ) );
      break;

      case 37:
      retval = to_string( impl_Has_Next_Specification_Info( ) );
      break;

      case 38:
      retval = to_string( impl_Is_Child_Only( ) );
      break;

      case 39:
      retval = to_string( impl_Is_Required_For_UI_Gen( ) );
      break;

      case 40:
      retval = to_string( impl_Is_System( ) );
      break;

      case 41:
      retval = to_string( impl_Name( ) );
      break;

      case 42:
      retval = to_string( impl_Needs_Child_Relationship( ) );
      break;

      case 43:
      retval = to_string( impl_Needs_Class( ) );
      break;

      case 44:
      retval = to_string( impl_Needs_Enum( ) );
      break;

      case 45:
      retval = to_string( impl_Needs_Enum_Item( ) );
      break;

      case 46:
      retval = to_string( impl_Needs_Enum_Item_2( ) );
      break;

      case 47:
      retval = to_string( impl_Needs_Enum_Item_3( ) );
      break;

      case 48:
      retval = to_string( impl_Needs_Enum_Item_4( ) );
      break;

      case 49:
      retval = to_string( impl_Needs_Enum_Item_5( ) );
      break;

      case 50:
      retval = to_string( impl_Needs_Field( ) );
      break;

      case 51:
      retval = to_string( impl_Needs_Modifier( ) );
      break;

      case 52:
      retval = to_string( impl_Needs_Other_Class( ) );
      break;

      case 53:
      retval = to_string( impl_Needs_Other_Class_Field( ) );
      break;

      case 54:
      retval = to_string( impl_Needs_Other_Field( ) );
      break;

      case 55:
      retval = to_string( impl_Needs_Other_Field_2( ) );
      break;

      case 56:
      retval = to_string( impl_Needs_Other_Modifier( ) );
      break;

      case 57:
      retval = to_string( impl_Needs_Other_Modifier_2( ) );
      break;

      case 58:
      retval = to_string( impl_Needs_Other_Permission( ) );
      break;

      case 59:
      retval = to_string( impl_Needs_Other_Permission_2( ) );
      break;

      case 60:
      retval = to_string( impl_Needs_Other_Procedure( ) );
      break;

      case 61:
      retval = to_string( impl_Needs_Other_Procedure_2( ) );
      break;

      case 62:
      retval = to_string( impl_Needs_Other_Source_Child( ) );
      break;

      case 63:
      retval = to_string( impl_Needs_Other_Source_Child_2( ) );
      break;

      case 64:
      retval = to_string( impl_Needs_Permission( ) );
      break;

      case 65:
      retval = to_string( impl_Needs_Procedure( ) );
      break;

      case 66:
      retval = to_string( impl_Needs_Procedure_Arg( ) );
      break;

      case 67:
      retval = to_string( impl_Needs_Procedure_Arg_2( ) );
      break;

      case 68:
      retval = to_string( impl_Needs_Procedure_Arg_3( ) );
      break;

      case 69:
      retval = to_string( impl_Needs_Source_Child( ) );
      break;

      case 70:
      retval = to_string( impl_Needs_Source_Field( ) );
      break;

      case 71:
      retval = to_string( impl_Needs_Source_Field_Or_Child( ) );
      break;

      case 72:
      retval = to_string( impl_Needs_Source_Parent( ) );
      break;

      case 73:
      retval = to_string( impl_Needs_Test_Field( ) );
      break;

      case 74:
      retval = to_string( impl_Needs_Test_Value( ) );
      break;

      case 75:
      retval = to_string( impl_Needs_Value( ) );
      break;

      case 76:
      retval = to_string( impl_Next_Child_Specification_Type( ) );
      break;

      case 77:
      retval = to_string( impl_Next_Protect_Child_Rel( ) );
      break;

      case 78:
      retval = to_string( impl_Next_Protect_Procedure( ) );
      break;

      case 79:
      retval = to_string( impl_Next_Protect_Source_Parent( ) );
      break;

      case 80:
      retval = to_string( impl_Next_Specification_Actions( ) );
      break;

      case 81:
      retval = to_string( impl_Notes( ) );
      break;

      case 82:
      retval = to_string( impl_Parent_Specification_Type( ) );
      break;

      case 83:
      retval = to_string( impl_Procedure_Arg_primitive( ) );
      break;

      case 84:
      retval = to_string( impl_Procedure_Arg_type( ) );
      break;

      case 85:
      retval = to_string( impl_Protect_Child_Rel_From_Update( ) );
      break;

      case 86:
      retval = to_string( impl_Protect_Class_From_Edit( ) );
      break;

      case 87:
      retval = to_string( impl_Protect_Class_From_Update( ) );
      break;

      case 88:
      retval = to_string( impl_Protect_Other_Class_From_Edit( ) );
      break;

      case 89:
      retval = to_string( impl_Protect_Procedure_From_Edit( ) );
      break;

      case 90:
      retval = to_string( impl_Protect_Source_Parent( ) );
      break;

      case 91:
      retval = to_string( impl_Set_Field_To_Source_Field_Or_Child( ) );
      break;

      case 92:
      retval = to_string( impl_Source_Field_Needs_Test_Field( ) );
      break;

      case 93:
      retval = to_string( impl_Source_Field_Type_Match( ) );
      break;

      case 94:
      retval = to_string( impl_Source_Parent_type( ) );
      break;

      case 95:
      retval = to_string( impl_Source_type( ) );
      break;

      case 96:
      retval = to_string( impl_Specification_Actions( ) );
      break;

      case 97:
      retval = to_string( impl_Specification_Name( ) );
      break;

      case 98:
      retval = to_string( impl_Specification_Object( ) );
      break;

      case 99:
      retval = to_string( impl_Specification_Strings( ) );
      break;

      case 100:
      retval = to_string( impl_Specification_Vars( ) );
      break;

      case 101:
      retval = to_string( impl_Test_Field_Type_Match( ) );
      break;

      case 102:
      retval = to_string( impl_Test_Field_type( ) );
      break;

      case 103:
      retval = to_string( impl_Use_Class_As_Source_Parent_Class( ) );
      break;

      case 104:
      retval = to_string( impl_Use_Field_Enum( ) );
      break;

      case 105:
      retval = to_string( impl_Use_Parent_Child_Rel_As_Source_Parent( ) );
      break;

      case 106:
      retval = to_string( impl_Use_Parent_Child_Rel_For_Class( ) );
      break;

      case 107:
      retval = to_string( impl_Use_Parent_Child_Rel_For_Other_Class( ) );
      break;

      case 108:
      retval = to_string( impl_Use_Parent_Class( ) );
      break;

      case 109:
      retval = to_string( impl_Use_Parent_Field_For_Class( ) );
      break;

      case 110:
      retval = to_string( impl_Use_Parent_Other_Class( ) );
      break;

      case 111:
      retval = to_string( impl_Use_Parent_Other_Class_For_Class( ) );
      break;

      case 112:
      retval = to_string( impl_Use_Parent_Procedure( ) );
      break;

      case 113:
      retval = to_string( impl_Use_Parent_Source_Class( ) );
      break;

      case 114:
      retval = to_string( impl_Use_Parent_Source_Field_For_Class( ) );
      break;

      case 115:
      retval = to_string( impl_Use_Parent_Source_Parent( ) );
      break;

      case 116:
      retval = to_string( impl_Use_Parent_Source_Parent_For_Class( ) );
      break;

      case 117:
      retval = to_string( impl_Use_Source_Field_Enum( ) );
      break;

      case 118:
      retval = to_string( impl_Use_Source_Field_Or_Child_Enum( ) );
      break;

      case 119:
      retval = to_string( impl_View_Id( ) );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in get field value" );
   }

   return retval;
}

void Meta_Specification_Type::impl::set_field_value( int field, const string& value )
{
   switch( field )
   {
      case 0:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Allow_Child_Relationship, value );
      break;

      case 1:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Allow_Enum, value );
      break;

      case 2:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Allow_Enum_Item, value );
      break;

      case 3:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Allow_FK_Source_Field_Choice, value );
      break;

      case 4:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Allow_FK_Test_Field_Choice, value );
      break;

      case 5:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Allow_Field, value );
      break;

      case 6:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Allow_Modifier, value );
      break;

      case 7:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Allow_Options, value );
      break;

      case 8:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Allow_Other_Class, value );
      break;

      case 9:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Allow_Other_Class_Field, value );
      break;

      case 10:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Allow_Other_Field, value );
      break;

      case 11:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Allow_Other_Field_2, value );
      break;

      case 12:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Allow_Other_Modifier, value );
      break;

      case 13:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Allow_Other_Modifier_2, value );
      break;

      case 14:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Allow_Other_Procedure, value );
      break;

      case 15:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Allow_Other_Procedure_2, value );
      break;

      case 16:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Allow_Other_Source_Child, value );
      break;

      case 17:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Allow_Other_Source_Child_2, value );
      break;

      case 18:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Allow_Permission, value );
      break;

      case 19:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Allow_Procedure, value );
      break;

      case 20:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Allow_Procedure_Arg, value );
      break;

      case 21:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Allow_Source_Child, value );
      break;

      case 22:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Allow_Source_Class, value );
      break;

      case 23:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Allow_Source_Field, value );
      break;

      case 24:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Allow_Source_Grandchild, value );
      break;

      case 25:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Allow_Source_Parent, value );
      break;

      case 26:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Allow_Test_Field, value );
      break;

      case 27:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Allow_Test_Value, value );
      break;

      case 28:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Allow_Value, value );
      break;

      case 29:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Can_Access_Below_Model, value );
      break;

      case 30:
      func_string_setter< Meta_Specification_Type::impl, string >( *this, &Meta_Specification_Type::impl::impl_Child_Prefix, value );
      break;

      case 31:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Child_Rel_Grandparent_Match, value );
      break;

      case 32:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Child_Rel_Parent_Match, value );
      break;

      case 33:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Child_Relationship_Class_Match, value );
      break;

      case 34:
      func_string_setter< Meta_Specification_Type::impl, Meta_Specification_Type >( *this, &Meta_Specification_Type::impl::impl_Child_Specification_Type, value );
      break;

      case 35:
      func_string_setter< Meta_Specification_Type::impl, string >( *this, &Meta_Specification_Type::impl::impl_Default_Child_Vars, value );
      break;

      case 36:
      func_string_setter< Meta_Specification_Type::impl, int >( *this, &Meta_Specification_Type::impl::impl_Field_type, value );
      break;

      case 37:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Has_Next_Specification_Info, value );
      break;

      case 38:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Is_Child_Only, value );
      break;

      case 39:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Is_Required_For_UI_Gen, value );
      break;

      case 40:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Is_System, value );
      break;

      case 41:
      func_string_setter< Meta_Specification_Type::impl, string >( *this, &Meta_Specification_Type::impl::impl_Name, value );
      break;

      case 42:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Needs_Child_Relationship, value );
      break;

      case 43:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Needs_Class, value );
      break;

      case 44:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Needs_Enum, value );
      break;

      case 45:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Needs_Enum_Item, value );
      break;

      case 46:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Needs_Enum_Item_2, value );
      break;

      case 47:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Needs_Enum_Item_3, value );
      break;

      case 48:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Needs_Enum_Item_4, value );
      break;

      case 49:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Needs_Enum_Item_5, value );
      break;

      case 50:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Needs_Field, value );
      break;

      case 51:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Needs_Modifier, value );
      break;

      case 52:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Needs_Other_Class, value );
      break;

      case 53:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Needs_Other_Class_Field, value );
      break;

      case 54:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Needs_Other_Field, value );
      break;

      case 55:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Needs_Other_Field_2, value );
      break;

      case 56:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Needs_Other_Modifier, value );
      break;

      case 57:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Needs_Other_Modifier_2, value );
      break;

      case 58:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Needs_Other_Permission, value );
      break;

      case 59:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Needs_Other_Permission_2, value );
      break;

      case 60:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Needs_Other_Procedure, value );
      break;

      case 61:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Needs_Other_Procedure_2, value );
      break;

      case 62:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Needs_Other_Source_Child, value );
      break;

      case 63:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Needs_Other_Source_Child_2, value );
      break;

      case 64:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Needs_Permission, value );
      break;

      case 65:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Needs_Procedure, value );
      break;

      case 66:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Needs_Procedure_Arg, value );
      break;

      case 67:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Needs_Procedure_Arg_2, value );
      break;

      case 68:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Needs_Procedure_Arg_3, value );
      break;

      case 69:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Needs_Source_Child, value );
      break;

      case 70:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Needs_Source_Field, value );
      break;

      case 71:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Needs_Source_Field_Or_Child, value );
      break;

      case 72:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Needs_Source_Parent, value );
      break;

      case 73:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Needs_Test_Field, value );
      break;

      case 74:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Needs_Test_Value, value );
      break;

      case 75:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Needs_Value, value );
      break;

      case 76:
      func_string_setter< Meta_Specification_Type::impl, Meta_Specification_Type >( *this, &Meta_Specification_Type::impl::impl_Next_Child_Specification_Type, value );
      break;

      case 77:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Next_Protect_Child_Rel, value );
      break;

      case 78:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Next_Protect_Procedure, value );
      break;

      case 79:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Next_Protect_Source_Parent, value );
      break;

      case 80:
      func_string_setter< Meta_Specification_Type::impl, string >( *this, &Meta_Specification_Type::impl::impl_Next_Specification_Actions, value );
      break;

      case 81:
      func_string_setter< Meta_Specification_Type::impl, string >( *this, &Meta_Specification_Type::impl::impl_Notes, value );
      break;

      case 82:
      func_string_setter< Meta_Specification_Type::impl, Meta_Specification_Type >( *this, &Meta_Specification_Type::impl::impl_Parent_Specification_Type, value );
      break;

      case 83:
      func_string_setter< Meta_Specification_Type::impl, int >( *this, &Meta_Specification_Type::impl::impl_Procedure_Arg_primitive, value );
      break;

      case 84:
      func_string_setter< Meta_Specification_Type::impl, int >( *this, &Meta_Specification_Type::impl::impl_Procedure_Arg_type, value );
      break;

      case 85:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Protect_Child_Rel_From_Update, value );
      break;

      case 86:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Protect_Class_From_Edit, value );
      break;

      case 87:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Protect_Class_From_Update, value );
      break;

      case 88:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Protect_Other_Class_From_Edit, value );
      break;

      case 89:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Protect_Procedure_From_Edit, value );
      break;

      case 90:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Protect_Source_Parent, value );
      break;

      case 91:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Set_Field_To_Source_Field_Or_Child, value );
      break;

      case 92:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Source_Field_Needs_Test_Field, value );
      break;

      case 93:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Source_Field_Type_Match, value );
      break;

      case 94:
      func_string_setter< Meta_Specification_Type::impl, int >( *this, &Meta_Specification_Type::impl::impl_Source_Parent_type, value );
      break;

      case 95:
      func_string_setter< Meta_Specification_Type::impl, int >( *this, &Meta_Specification_Type::impl::impl_Source_type, value );
      break;

      case 96:
      func_string_setter< Meta_Specification_Type::impl, string >( *this, &Meta_Specification_Type::impl::impl_Specification_Actions, value );
      break;

      case 97:
      func_string_setter< Meta_Specification_Type::impl, string >( *this, &Meta_Specification_Type::impl::impl_Specification_Name, value );
      break;

      case 98:
      func_string_setter< Meta_Specification_Type::impl, string >( *this, &Meta_Specification_Type::impl::impl_Specification_Object, value );
      break;

      case 99:
      func_string_setter< Meta_Specification_Type::impl, string >( *this, &Meta_Specification_Type::impl::impl_Specification_Strings, value );
      break;

      case 100:
      func_string_setter< Meta_Specification_Type::impl, string >( *this, &Meta_Specification_Type::impl::impl_Specification_Vars, value );
      break;

      case 101:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Test_Field_Type_Match, value );
      break;

      case 102:
      func_string_setter< Meta_Specification_Type::impl, int >( *this, &Meta_Specification_Type::impl::impl_Test_Field_type, value );
      break;

      case 103:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Use_Class_As_Source_Parent_Class, value );
      break;

      case 104:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Use_Field_Enum, value );
      break;

      case 105:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Use_Parent_Child_Rel_As_Source_Parent, value );
      break;

      case 106:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Use_Parent_Child_Rel_For_Class, value );
      break;

      case 107:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Use_Parent_Child_Rel_For_Other_Class, value );
      break;

      case 108:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Use_Parent_Class, value );
      break;

      case 109:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Use_Parent_Field_For_Class, value );
      break;

      case 110:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Use_Parent_Other_Class, value );
      break;

      case 111:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Use_Parent_Other_Class_For_Class, value );
      break;

      case 112:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Use_Parent_Procedure, value );
      break;

      case 113:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Use_Parent_Source_Class, value );
      break;

      case 114:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Use_Parent_Source_Field_For_Class, value );
      break;

      case 115:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Use_Parent_Source_Parent, value );
      break;

      case 116:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Use_Parent_Source_Parent_For_Class, value );
      break;

      case 117:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Use_Source_Field_Enum, value );
      break;

      case 118:
      func_string_setter< Meta_Specification_Type::impl, bool >( *this, &Meta_Specification_Type::impl::impl_Use_Source_Field_Or_Child_Enum, value );
      break;

      case 119:
      func_string_setter< Meta_Specification_Type::impl, string >( *this, &Meta_Specification_Type::impl::impl_View_Id, value );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in set field value" );
   }
}

void Meta_Specification_Type::impl::set_field_default( int field )
{
   switch( field )
   {
      case 0:
      impl_Allow_Child_Relationship( g_default_Allow_Child_Relationship );
      break;

      case 1:
      impl_Allow_Enum( g_default_Allow_Enum );
      break;

      case 2:
      impl_Allow_Enum_Item( g_default_Allow_Enum_Item );
      break;

      case 3:
      impl_Allow_FK_Source_Field_Choice( g_default_Allow_FK_Source_Field_Choice );
      break;

      case 4:
      impl_Allow_FK_Test_Field_Choice( g_default_Allow_FK_Test_Field_Choice );
      break;

      case 5:
      impl_Allow_Field( g_default_Allow_Field );
      break;

      case 6:
      impl_Allow_Modifier( g_default_Allow_Modifier );
      break;

      case 7:
      impl_Allow_Options( g_default_Allow_Options );
      break;

      case 8:
      impl_Allow_Other_Class( g_default_Allow_Other_Class );
      break;

      case 9:
      impl_Allow_Other_Class_Field( g_default_Allow_Other_Class_Field );
      break;

      case 10:
      impl_Allow_Other_Field( g_default_Allow_Other_Field );
      break;

      case 11:
      impl_Allow_Other_Field_2( g_default_Allow_Other_Field_2 );
      break;

      case 12:
      impl_Allow_Other_Modifier( g_default_Allow_Other_Modifier );
      break;

      case 13:
      impl_Allow_Other_Modifier_2( g_default_Allow_Other_Modifier_2 );
      break;

      case 14:
      impl_Allow_Other_Procedure( g_default_Allow_Other_Procedure );
      break;

      case 15:
      impl_Allow_Other_Procedure_2( g_default_Allow_Other_Procedure_2 );
      break;

      case 16:
      impl_Allow_Other_Source_Child( g_default_Allow_Other_Source_Child );
      break;

      case 17:
      impl_Allow_Other_Source_Child_2( g_default_Allow_Other_Source_Child_2 );
      break;

      case 18:
      impl_Allow_Permission( g_default_Allow_Permission );
      break;

      case 19:
      impl_Allow_Procedure( g_default_Allow_Procedure );
      break;

      case 20:
      impl_Allow_Procedure_Arg( g_default_Allow_Procedure_Arg );
      break;

      case 21:
      impl_Allow_Source_Child( g_default_Allow_Source_Child );
      break;

      case 22:
      impl_Allow_Source_Class( g_default_Allow_Source_Class );
      break;

      case 23:
      impl_Allow_Source_Field( g_default_Allow_Source_Field );
      break;

      case 24:
      impl_Allow_Source_Grandchild( g_default_Allow_Source_Grandchild );
      break;

      case 25:
      impl_Allow_Source_Parent( g_default_Allow_Source_Parent );
      break;

      case 26:
      impl_Allow_Test_Field( g_default_Allow_Test_Field );
      break;

      case 27:
      impl_Allow_Test_Value( g_default_Allow_Test_Value );
      break;

      case 28:
      impl_Allow_Value( g_default_Allow_Value );
      break;

      case 29:
      impl_Can_Access_Below_Model( g_default_Can_Access_Below_Model );
      break;

      case 30:
      impl_Child_Prefix( g_default_Child_Prefix );
      break;

      case 31:
      impl_Child_Rel_Grandparent_Match( g_default_Child_Rel_Grandparent_Match );
      break;

      case 32:
      impl_Child_Rel_Parent_Match( g_default_Child_Rel_Parent_Match );
      break;

      case 33:
      impl_Child_Relationship_Class_Match( g_default_Child_Relationship_Class_Match );
      break;

      case 34:
      impl_Child_Specification_Type( g_default_Child_Specification_Type );
      break;

      case 35:
      impl_Default_Child_Vars( g_default_Default_Child_Vars );
      break;

      case 36:
      impl_Field_type( g_default_Field_type );
      break;

      case 37:
      impl_Has_Next_Specification_Info( g_default_Has_Next_Specification_Info );
      break;

      case 38:
      impl_Is_Child_Only( g_default_Is_Child_Only );
      break;

      case 39:
      impl_Is_Required_For_UI_Gen( g_default_Is_Required_For_UI_Gen );
      break;

      case 40:
      impl_Is_System( g_default_Is_System );
      break;

      case 41:
      impl_Name( g_default_Name );
      break;

      case 42:
      impl_Needs_Child_Relationship( g_default_Needs_Child_Relationship );
      break;

      case 43:
      impl_Needs_Class( g_default_Needs_Class );
      break;

      case 44:
      impl_Needs_Enum( g_default_Needs_Enum );
      break;

      case 45:
      impl_Needs_Enum_Item( g_default_Needs_Enum_Item );
      break;

      case 46:
      impl_Needs_Enum_Item_2( g_default_Needs_Enum_Item_2 );
      break;

      case 47:
      impl_Needs_Enum_Item_3( g_default_Needs_Enum_Item_3 );
      break;

      case 48:
      impl_Needs_Enum_Item_4( g_default_Needs_Enum_Item_4 );
      break;

      case 49:
      impl_Needs_Enum_Item_5( g_default_Needs_Enum_Item_5 );
      break;

      case 50:
      impl_Needs_Field( g_default_Needs_Field );
      break;

      case 51:
      impl_Needs_Modifier( g_default_Needs_Modifier );
      break;

      case 52:
      impl_Needs_Other_Class( g_default_Needs_Other_Class );
      break;

      case 53:
      impl_Needs_Other_Class_Field( g_default_Needs_Other_Class_Field );
      break;

      case 54:
      impl_Needs_Other_Field( g_default_Needs_Other_Field );
      break;

      case 55:
      impl_Needs_Other_Field_2( g_default_Needs_Other_Field_2 );
      break;

      case 56:
      impl_Needs_Other_Modifier( g_default_Needs_Other_Modifier );
      break;

      case 57:
      impl_Needs_Other_Modifier_2( g_default_Needs_Other_Modifier_2 );
      break;

      case 58:
      impl_Needs_Other_Permission( g_default_Needs_Other_Permission );
      break;

      case 59:
      impl_Needs_Other_Permission_2( g_default_Needs_Other_Permission_2 );
      break;

      case 60:
      impl_Needs_Other_Procedure( g_default_Needs_Other_Procedure );
      break;

      case 61:
      impl_Needs_Other_Procedure_2( g_default_Needs_Other_Procedure_2 );
      break;

      case 62:
      impl_Needs_Other_Source_Child( g_default_Needs_Other_Source_Child );
      break;

      case 63:
      impl_Needs_Other_Source_Child_2( g_default_Needs_Other_Source_Child_2 );
      break;

      case 64:
      impl_Needs_Permission( g_default_Needs_Permission );
      break;

      case 65:
      impl_Needs_Procedure( g_default_Needs_Procedure );
      break;

      case 66:
      impl_Needs_Procedure_Arg( g_default_Needs_Procedure_Arg );
      break;

      case 67:
      impl_Needs_Procedure_Arg_2( g_default_Needs_Procedure_Arg_2 );
      break;

      case 68:
      impl_Needs_Procedure_Arg_3( g_default_Needs_Procedure_Arg_3 );
      break;

      case 69:
      impl_Needs_Source_Child( g_default_Needs_Source_Child );
      break;

      case 70:
      impl_Needs_Source_Field( g_default_Needs_Source_Field );
      break;

      case 71:
      impl_Needs_Source_Field_Or_Child( g_default_Needs_Source_Field_Or_Child );
      break;

      case 72:
      impl_Needs_Source_Parent( g_default_Needs_Source_Parent );
      break;

      case 73:
      impl_Needs_Test_Field( g_default_Needs_Test_Field );
      break;

      case 74:
      impl_Needs_Test_Value( g_default_Needs_Test_Value );
      break;

      case 75:
      impl_Needs_Value( g_default_Needs_Value );
      break;

      case 76:
      impl_Next_Child_Specification_Type( g_default_Next_Child_Specification_Type );
      break;

      case 77:
      impl_Next_Protect_Child_Rel( g_default_Next_Protect_Child_Rel );
      break;

      case 78:
      impl_Next_Protect_Procedure( g_default_Next_Protect_Procedure );
      break;

      case 79:
      impl_Next_Protect_Source_Parent( g_default_Next_Protect_Source_Parent );
      break;

      case 80:
      impl_Next_Specification_Actions( g_default_Next_Specification_Actions );
      break;

      case 81:
      impl_Notes( g_default_Notes );
      break;

      case 82:
      impl_Parent_Specification_Type( g_default_Parent_Specification_Type );
      break;

      case 83:
      impl_Procedure_Arg_primitive( g_default_Procedure_Arg_primitive );
      break;

      case 84:
      impl_Procedure_Arg_type( g_default_Procedure_Arg_type );
      break;

      case 85:
      impl_Protect_Child_Rel_From_Update( g_default_Protect_Child_Rel_From_Update );
      break;

      case 86:
      impl_Protect_Class_From_Edit( g_default_Protect_Class_From_Edit );
      break;

      case 87:
      impl_Protect_Class_From_Update( g_default_Protect_Class_From_Update );
      break;

      case 88:
      impl_Protect_Other_Class_From_Edit( g_default_Protect_Other_Class_From_Edit );
      break;

      case 89:
      impl_Protect_Procedure_From_Edit( g_default_Protect_Procedure_From_Edit );
      break;

      case 90:
      impl_Protect_Source_Parent( g_default_Protect_Source_Parent );
      break;

      case 91:
      impl_Set_Field_To_Source_Field_Or_Child( g_default_Set_Field_To_Source_Field_Or_Child );
      break;

      case 92:
      impl_Source_Field_Needs_Test_Field( g_default_Source_Field_Needs_Test_Field );
      break;

      case 93:
      impl_Source_Field_Type_Match( g_default_Source_Field_Type_Match );
      break;

      case 94:
      impl_Source_Parent_type( g_default_Source_Parent_type );
      break;

      case 95:
      impl_Source_type( g_default_Source_type );
      break;

      case 96:
      impl_Specification_Actions( g_default_Specification_Actions );
      break;

      case 97:
      impl_Specification_Name( g_default_Specification_Name );
      break;

      case 98:
      impl_Specification_Object( g_default_Specification_Object );
      break;

      case 99:
      impl_Specification_Strings( g_default_Specification_Strings );
      break;

      case 100:
      impl_Specification_Vars( g_default_Specification_Vars );
      break;

      case 101:
      impl_Test_Field_Type_Match( g_default_Test_Field_Type_Match );
      break;

      case 102:
      impl_Test_Field_type( g_default_Test_Field_type );
      break;

      case 103:
      impl_Use_Class_As_Source_Parent_Class( g_default_Use_Class_As_Source_Parent_Class );
      break;

      case 104:
      impl_Use_Field_Enum( g_default_Use_Field_Enum );
      break;

      case 105:
      impl_Use_Parent_Child_Rel_As_Source_Parent( g_default_Use_Parent_Child_Rel_As_Source_Parent );
      break;

      case 106:
      impl_Use_Parent_Child_Rel_For_Class( g_default_Use_Parent_Child_Rel_For_Class );
      break;

      case 107:
      impl_Use_Parent_Child_Rel_For_Other_Class( g_default_Use_Parent_Child_Rel_For_Other_Class );
      break;

      case 108:
      impl_Use_Parent_Class( g_default_Use_Parent_Class );
      break;

      case 109:
      impl_Use_Parent_Field_For_Class( g_default_Use_Parent_Field_For_Class );
      break;

      case 110:
      impl_Use_Parent_Other_Class( g_default_Use_Parent_Other_Class );
      break;

      case 111:
      impl_Use_Parent_Other_Class_For_Class( g_default_Use_Parent_Other_Class_For_Class );
      break;

      case 112:
      impl_Use_Parent_Procedure( g_default_Use_Parent_Procedure );
      break;

      case 113:
      impl_Use_Parent_Source_Class( g_default_Use_Parent_Source_Class );
      break;

      case 114:
      impl_Use_Parent_Source_Field_For_Class( g_default_Use_Parent_Source_Field_For_Class );
      break;

      case 115:
      impl_Use_Parent_Source_Parent( g_default_Use_Parent_Source_Parent );
      break;

      case 116:
      impl_Use_Parent_Source_Parent_For_Class( g_default_Use_Parent_Source_Parent_For_Class );
      break;

      case 117:
      impl_Use_Source_Field_Enum( g_default_Use_Source_Field_Enum );
      break;

      case 118:
      impl_Use_Source_Field_Or_Child_Enum( g_default_Use_Source_Field_Or_Child_Enum );
      break;

      case 119:
      impl_View_Id( g_default_View_Id );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in set field default" );
   }
}

bool Meta_Specification_Type::impl::is_field_default( int field ) const
{
   bool retval = false;

   switch( field )
   {
      case 0:
      retval = ( v_Allow_Child_Relationship == g_default_Allow_Child_Relationship );
      break;

      case 1:
      retval = ( v_Allow_Enum == g_default_Allow_Enum );
      break;

      case 2:
      retval = ( v_Allow_Enum_Item == g_default_Allow_Enum_Item );
      break;

      case 3:
      retval = ( v_Allow_FK_Source_Field_Choice == g_default_Allow_FK_Source_Field_Choice );
      break;

      case 4:
      retval = ( v_Allow_FK_Test_Field_Choice == g_default_Allow_FK_Test_Field_Choice );
      break;

      case 5:
      retval = ( v_Allow_Field == g_default_Allow_Field );
      break;

      case 6:
      retval = ( v_Allow_Modifier == g_default_Allow_Modifier );
      break;

      case 7:
      retval = ( v_Allow_Options == g_default_Allow_Options );
      break;

      case 8:
      retval = ( v_Allow_Other_Class == g_default_Allow_Other_Class );
      break;

      case 9:
      retval = ( v_Allow_Other_Class_Field == g_default_Allow_Other_Class_Field );
      break;

      case 10:
      retval = ( v_Allow_Other_Field == g_default_Allow_Other_Field );
      break;

      case 11:
      retval = ( v_Allow_Other_Field_2 == g_default_Allow_Other_Field_2 );
      break;

      case 12:
      retval = ( v_Allow_Other_Modifier == g_default_Allow_Other_Modifier );
      break;

      case 13:
      retval = ( v_Allow_Other_Modifier_2 == g_default_Allow_Other_Modifier_2 );
      break;

      case 14:
      retval = ( v_Allow_Other_Procedure == g_default_Allow_Other_Procedure );
      break;

      case 15:
      retval = ( v_Allow_Other_Procedure_2 == g_default_Allow_Other_Procedure_2 );
      break;

      case 16:
      retval = ( v_Allow_Other_Source_Child == g_default_Allow_Other_Source_Child );
      break;

      case 17:
      retval = ( v_Allow_Other_Source_Child_2 == g_default_Allow_Other_Source_Child_2 );
      break;

      case 18:
      retval = ( v_Allow_Permission == g_default_Allow_Permission );
      break;

      case 19:
      retval = ( v_Allow_Procedure == g_default_Allow_Procedure );
      break;

      case 20:
      retval = ( v_Allow_Procedure_Arg == g_default_Allow_Procedure_Arg );
      break;

      case 21:
      retval = ( v_Allow_Source_Child == g_default_Allow_Source_Child );
      break;

      case 22:
      retval = ( v_Allow_Source_Class == g_default_Allow_Source_Class );
      break;

      case 23:
      retval = ( v_Allow_Source_Field == g_default_Allow_Source_Field );
      break;

      case 24:
      retval = ( v_Allow_Source_Grandchild == g_default_Allow_Source_Grandchild );
      break;

      case 25:
      retval = ( v_Allow_Source_Parent == g_default_Allow_Source_Parent );
      break;

      case 26:
      retval = ( v_Allow_Test_Field == g_default_Allow_Test_Field );
      break;

      case 27:
      retval = ( v_Allow_Test_Value == g_default_Allow_Test_Value );
      break;

      case 28:
      retval = ( v_Allow_Value == g_default_Allow_Value );
      break;

      case 29:
      retval = ( v_Can_Access_Below_Model == g_default_Can_Access_Below_Model );
      break;

      case 30:
      retval = ( v_Child_Prefix == g_default_Child_Prefix );
      break;

      case 31:
      retval = ( v_Child_Rel_Grandparent_Match == g_default_Child_Rel_Grandparent_Match );
      break;

      case 32:
      retval = ( v_Child_Rel_Parent_Match == g_default_Child_Rel_Parent_Match );
      break;

      case 33:
      retval = ( v_Child_Relationship_Class_Match == g_default_Child_Relationship_Class_Match );
      break;

      case 34:
      retval = ( v_Child_Specification_Type == g_default_Child_Specification_Type );
      break;

      case 35:
      retval = ( v_Default_Child_Vars == g_default_Default_Child_Vars );
      break;

      case 36:
      retval = ( v_Field_type == g_default_Field_type );
      break;

      case 37:
      retval = ( v_Has_Next_Specification_Info == g_default_Has_Next_Specification_Info );
      break;

      case 38:
      retval = ( v_Is_Child_Only == g_default_Is_Child_Only );
      break;

      case 39:
      retval = ( v_Is_Required_For_UI_Gen == g_default_Is_Required_For_UI_Gen );
      break;

      case 40:
      retval = ( v_Is_System == g_default_Is_System );
      break;

      case 41:
      retval = ( v_Name == g_default_Name );
      break;

      case 42:
      retval = ( v_Needs_Child_Relationship == g_default_Needs_Child_Relationship );
      break;

      case 43:
      retval = ( v_Needs_Class == g_default_Needs_Class );
      break;

      case 44:
      retval = ( v_Needs_Enum == g_default_Needs_Enum );
      break;

      case 45:
      retval = ( v_Needs_Enum_Item == g_default_Needs_Enum_Item );
      break;

      case 46:
      retval = ( v_Needs_Enum_Item_2 == g_default_Needs_Enum_Item_2 );
      break;

      case 47:
      retval = ( v_Needs_Enum_Item_3 == g_default_Needs_Enum_Item_3 );
      break;

      case 48:
      retval = ( v_Needs_Enum_Item_4 == g_default_Needs_Enum_Item_4 );
      break;

      case 49:
      retval = ( v_Needs_Enum_Item_5 == g_default_Needs_Enum_Item_5 );
      break;

      case 50:
      retval = ( v_Needs_Field == g_default_Needs_Field );
      break;

      case 51:
      retval = ( v_Needs_Modifier == g_default_Needs_Modifier );
      break;

      case 52:
      retval = ( v_Needs_Other_Class == g_default_Needs_Other_Class );
      break;

      case 53:
      retval = ( v_Needs_Other_Class_Field == g_default_Needs_Other_Class_Field );
      break;

      case 54:
      retval = ( v_Needs_Other_Field == g_default_Needs_Other_Field );
      break;

      case 55:
      retval = ( v_Needs_Other_Field_2 == g_default_Needs_Other_Field_2 );
      break;

      case 56:
      retval = ( v_Needs_Other_Modifier == g_default_Needs_Other_Modifier );
      break;

      case 57:
      retval = ( v_Needs_Other_Modifier_2 == g_default_Needs_Other_Modifier_2 );
      break;

      case 58:
      retval = ( v_Needs_Other_Permission == g_default_Needs_Other_Permission );
      break;

      case 59:
      retval = ( v_Needs_Other_Permission_2 == g_default_Needs_Other_Permission_2 );
      break;

      case 60:
      retval = ( v_Needs_Other_Procedure == g_default_Needs_Other_Procedure );
      break;

      case 61:
      retval = ( v_Needs_Other_Procedure_2 == g_default_Needs_Other_Procedure_2 );
      break;

      case 62:
      retval = ( v_Needs_Other_Source_Child == g_default_Needs_Other_Source_Child );
      break;

      case 63:
      retval = ( v_Needs_Other_Source_Child_2 == g_default_Needs_Other_Source_Child_2 );
      break;

      case 64:
      retval = ( v_Needs_Permission == g_default_Needs_Permission );
      break;

      case 65:
      retval = ( v_Needs_Procedure == g_default_Needs_Procedure );
      break;

      case 66:
      retval = ( v_Needs_Procedure_Arg == g_default_Needs_Procedure_Arg );
      break;

      case 67:
      retval = ( v_Needs_Procedure_Arg_2 == g_default_Needs_Procedure_Arg_2 );
      break;

      case 68:
      retval = ( v_Needs_Procedure_Arg_3 == g_default_Needs_Procedure_Arg_3 );
      break;

      case 69:
      retval = ( v_Needs_Source_Child == g_default_Needs_Source_Child );
      break;

      case 70:
      retval = ( v_Needs_Source_Field == g_default_Needs_Source_Field );
      break;

      case 71:
      retval = ( v_Needs_Source_Field_Or_Child == g_default_Needs_Source_Field_Or_Child );
      break;

      case 72:
      retval = ( v_Needs_Source_Parent == g_default_Needs_Source_Parent );
      break;

      case 73:
      retval = ( v_Needs_Test_Field == g_default_Needs_Test_Field );
      break;

      case 74:
      retval = ( v_Needs_Test_Value == g_default_Needs_Test_Value );
      break;

      case 75:
      retval = ( v_Needs_Value == g_default_Needs_Value );
      break;

      case 76:
      retval = ( v_Next_Child_Specification_Type == g_default_Next_Child_Specification_Type );
      break;

      case 77:
      retval = ( v_Next_Protect_Child_Rel == g_default_Next_Protect_Child_Rel );
      break;

      case 78:
      retval = ( v_Next_Protect_Procedure == g_default_Next_Protect_Procedure );
      break;

      case 79:
      retval = ( v_Next_Protect_Source_Parent == g_default_Next_Protect_Source_Parent );
      break;

      case 80:
      retval = ( v_Next_Specification_Actions == g_default_Next_Specification_Actions );
      break;

      case 81:
      retval = ( v_Notes == g_default_Notes );
      break;

      case 82:
      retval = ( v_Parent_Specification_Type == g_default_Parent_Specification_Type );
      break;

      case 83:
      retval = ( v_Procedure_Arg_primitive == g_default_Procedure_Arg_primitive );
      break;

      case 84:
      retval = ( v_Procedure_Arg_type == g_default_Procedure_Arg_type );
      break;

      case 85:
      retval = ( v_Protect_Child_Rel_From_Update == g_default_Protect_Child_Rel_From_Update );
      break;

      case 86:
      retval = ( v_Protect_Class_From_Edit == g_default_Protect_Class_From_Edit );
      break;

      case 87:
      retval = ( v_Protect_Class_From_Update == g_default_Protect_Class_From_Update );
      break;

      case 88:
      retval = ( v_Protect_Other_Class_From_Edit == g_default_Protect_Other_Class_From_Edit );
      break;

      case 89:
      retval = ( v_Protect_Procedure_From_Edit == g_default_Protect_Procedure_From_Edit );
      break;

      case 90:
      retval = ( v_Protect_Source_Parent == g_default_Protect_Source_Parent );
      break;

      case 91:
      retval = ( v_Set_Field_To_Source_Field_Or_Child == g_default_Set_Field_To_Source_Field_Or_Child );
      break;

      case 92:
      retval = ( v_Source_Field_Needs_Test_Field == g_default_Source_Field_Needs_Test_Field );
      break;

      case 93:
      retval = ( v_Source_Field_Type_Match == g_default_Source_Field_Type_Match );
      break;

      case 94:
      retval = ( v_Source_Parent_type == g_default_Source_Parent_type );
      break;

      case 95:
      retval = ( v_Source_type == g_default_Source_type );
      break;

      case 96:
      retval = ( v_Specification_Actions == g_default_Specification_Actions );
      break;

      case 97:
      retval = ( v_Specification_Name == g_default_Specification_Name );
      break;

      case 98:
      retval = ( v_Specification_Object == g_default_Specification_Object );
      break;

      case 99:
      retval = ( v_Specification_Strings == g_default_Specification_Strings );
      break;

      case 100:
      retval = ( v_Specification_Vars == g_default_Specification_Vars );
      break;

      case 101:
      retval = ( v_Test_Field_Type_Match == g_default_Test_Field_Type_Match );
      break;

      case 102:
      retval = ( v_Test_Field_type == g_default_Test_Field_type );
      break;

      case 103:
      retval = ( v_Use_Class_As_Source_Parent_Class == g_default_Use_Class_As_Source_Parent_Class );
      break;

      case 104:
      retval = ( v_Use_Field_Enum == g_default_Use_Field_Enum );
      break;

      case 105:
      retval = ( v_Use_Parent_Child_Rel_As_Source_Parent == g_default_Use_Parent_Child_Rel_As_Source_Parent );
      break;

      case 106:
      retval = ( v_Use_Parent_Child_Rel_For_Class == g_default_Use_Parent_Child_Rel_For_Class );
      break;

      case 107:
      retval = ( v_Use_Parent_Child_Rel_For_Other_Class == g_default_Use_Parent_Child_Rel_For_Other_Class );
      break;

      case 108:
      retval = ( v_Use_Parent_Class == g_default_Use_Parent_Class );
      break;

      case 109:
      retval = ( v_Use_Parent_Field_For_Class == g_default_Use_Parent_Field_For_Class );
      break;

      case 110:
      retval = ( v_Use_Parent_Other_Class == g_default_Use_Parent_Other_Class );
      break;

      case 111:
      retval = ( v_Use_Parent_Other_Class_For_Class == g_default_Use_Parent_Other_Class_For_Class );
      break;

      case 112:
      retval = ( v_Use_Parent_Procedure == g_default_Use_Parent_Procedure );
      break;

      case 113:
      retval = ( v_Use_Parent_Source_Class == g_default_Use_Parent_Source_Class );
      break;

      case 114:
      retval = ( v_Use_Parent_Source_Field_For_Class == g_default_Use_Parent_Source_Field_For_Class );
      break;

      case 115:
      retval = ( v_Use_Parent_Source_Parent == g_default_Use_Parent_Source_Parent );
      break;

      case 116:
      retval = ( v_Use_Parent_Source_Parent_For_Class == g_default_Use_Parent_Source_Parent_For_Class );
      break;

      case 117:
      retval = ( v_Use_Source_Field_Enum == g_default_Use_Source_Field_Enum );
      break;

      case 118:
      retval = ( v_Use_Source_Field_Or_Child_Enum == g_default_Use_Source_Field_Or_Child_Enum );
      break;

      case 119:
      retval = ( v_View_Id == g_default_View_Id );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range in is_field_default" );
   }

   return retval;
}

uint64_t Meta_Specification_Type::impl::get_state( ) const
{
   uint64_t state = 0;

   // [<start get_state>]
   // [<finish get_state>]

   return state;
}

string Meta_Specification_Type::impl::get_state_names( ) const
{
   string state_names;
   uint64_t state = get_state( );

   return state_names.empty( ) ? state_names : state_names.substr( 1 );
}

const string& Meta_Specification_Type::impl::execute( const string& cmd_and_args )
{
   execute_command( cmd_and_args );
   return retval;
}

void Meta_Specification_Type::impl::clear_foreign_key( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( ( field == c_field_id_Child_Specification_Type ) || ( field == c_field_name_Child_Specification_Type ) )
      impl_Child_Specification_Type( "" );
   else if( ( field == c_field_id_Next_Child_Specification_Type ) || ( field == c_field_name_Next_Child_Specification_Type ) )
      impl_Next_Child_Specification_Type( "" );
   else if( ( field == c_field_id_Parent_Specification_Type ) || ( field == c_field_name_Parent_Specification_Type ) )
      impl_Parent_Specification_Type( "" );
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Specification_Type::impl::set_foreign_key_value( const string& field, const string& value )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for value: " + value );
   else if( ( field == c_field_id_Child_Specification_Type ) || ( field == c_field_name_Child_Specification_Type ) )
      v_Child_Specification_Type = value;
   else if( ( field == c_field_id_Next_Child_Specification_Type ) || ( field == c_field_name_Next_Child_Specification_Type ) )
      v_Next_Child_Specification_Type = value;
   else if( ( field == c_field_id_Parent_Specification_Type ) || ( field == c_field_name_Parent_Specification_Type ) )
      v_Parent_Specification_Type = value;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

const string& Meta_Specification_Type::impl::get_foreign_key_value( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( ( field == c_field_id_Child_Specification_Type ) || ( field == c_field_name_Child_Specification_Type ) )
      return v_Child_Specification_Type;
   else if( ( field == c_field_id_Next_Child_Specification_Type ) || ( field == c_field_name_Next_Child_Specification_Type ) )
      return v_Next_Child_Specification_Type;
   else if( ( field == c_field_id_Parent_Specification_Type ) || ( field == c_field_name_Parent_Specification_Type ) )
      return v_Parent_Specification_Type;
   else
      throw runtime_error( "unknown foreign key field '" + field + "'" );
}

void Meta_Specification_Type::impl::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Child_Specification_Type, v_Child_Specification_Type ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Next_Child_Specification_Type, v_Next_Child_Specification_Type ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Parent_Specification_Type, v_Parent_Specification_Type ) );
}

void Meta_Specification_Type::impl::add_extra_fixed_info( vector< pair< string, string > >& fixed_info ) const
{
   ( void )fixed_info;

   // [<start add_extra_fixed_info>]
   // [<finish add_extra_fixed_info>]
}

void Meta_Specification_Type::impl::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   ( void )paging_info;

   // [<start add_extra_paging_info>]
   // [<finish add_extra_paging_info>]
}

void Meta_Specification_Type::impl::clear( )
{
   v_Allow_Child_Relationship = g_default_Allow_Child_Relationship;
   v_Allow_Enum = g_default_Allow_Enum;
   v_Allow_Enum_Item = g_default_Allow_Enum_Item;
   v_Allow_FK_Source_Field_Choice = g_default_Allow_FK_Source_Field_Choice;
   v_Allow_FK_Test_Field_Choice = g_default_Allow_FK_Test_Field_Choice;
   v_Allow_Field = g_default_Allow_Field;
   v_Allow_Modifier = g_default_Allow_Modifier;
   v_Allow_Options = g_default_Allow_Options;
   v_Allow_Other_Class = g_default_Allow_Other_Class;
   v_Allow_Other_Class_Field = g_default_Allow_Other_Class_Field;
   v_Allow_Other_Field = g_default_Allow_Other_Field;
   v_Allow_Other_Field_2 = g_default_Allow_Other_Field_2;
   v_Allow_Other_Modifier = g_default_Allow_Other_Modifier;
   v_Allow_Other_Modifier_2 = g_default_Allow_Other_Modifier_2;
   v_Allow_Other_Procedure = g_default_Allow_Other_Procedure;
   v_Allow_Other_Procedure_2 = g_default_Allow_Other_Procedure_2;
   v_Allow_Other_Source_Child = g_default_Allow_Other_Source_Child;
   v_Allow_Other_Source_Child_2 = g_default_Allow_Other_Source_Child_2;
   v_Allow_Permission = g_default_Allow_Permission;
   v_Allow_Procedure = g_default_Allow_Procedure;
   v_Allow_Procedure_Arg = g_default_Allow_Procedure_Arg;
   v_Allow_Source_Child = g_default_Allow_Source_Child;
   v_Allow_Source_Class = g_default_Allow_Source_Class;
   v_Allow_Source_Field = g_default_Allow_Source_Field;
   v_Allow_Source_Grandchild = g_default_Allow_Source_Grandchild;
   v_Allow_Source_Parent = g_default_Allow_Source_Parent;
   v_Allow_Test_Field = g_default_Allow_Test_Field;
   v_Allow_Test_Value = g_default_Allow_Test_Value;
   v_Allow_Value = g_default_Allow_Value;
   v_Can_Access_Below_Model = g_default_Can_Access_Below_Model;
   v_Child_Prefix = g_default_Child_Prefix;
   v_Child_Rel_Grandparent_Match = g_default_Child_Rel_Grandparent_Match;
   v_Child_Rel_Parent_Match = g_default_Child_Rel_Parent_Match;
   v_Child_Relationship_Class_Match = g_default_Child_Relationship_Class_Match;
   v_Default_Child_Vars = g_default_Default_Child_Vars;
   v_Field_type = g_default_Field_type;
   v_Has_Next_Specification_Info = g_default_Has_Next_Specification_Info;
   v_Is_Child_Only = g_default_Is_Child_Only;
   v_Is_Required_For_UI_Gen = g_default_Is_Required_For_UI_Gen;
   v_Is_System = g_default_Is_System;
   v_Name = g_default_Name;
   v_Needs_Child_Relationship = g_default_Needs_Child_Relationship;
   v_Needs_Class = g_default_Needs_Class;
   v_Needs_Enum = g_default_Needs_Enum;
   v_Needs_Enum_Item = g_default_Needs_Enum_Item;
   v_Needs_Enum_Item_2 = g_default_Needs_Enum_Item_2;
   v_Needs_Enum_Item_3 = g_default_Needs_Enum_Item_3;
   v_Needs_Enum_Item_4 = g_default_Needs_Enum_Item_4;
   v_Needs_Enum_Item_5 = g_default_Needs_Enum_Item_5;
   v_Needs_Field = g_default_Needs_Field;
   v_Needs_Modifier = g_default_Needs_Modifier;
   v_Needs_Other_Class = g_default_Needs_Other_Class;
   v_Needs_Other_Class_Field = g_default_Needs_Other_Class_Field;
   v_Needs_Other_Field = g_default_Needs_Other_Field;
   v_Needs_Other_Field_2 = g_default_Needs_Other_Field_2;
   v_Needs_Other_Modifier = g_default_Needs_Other_Modifier;
   v_Needs_Other_Modifier_2 = g_default_Needs_Other_Modifier_2;
   v_Needs_Other_Permission = g_default_Needs_Other_Permission;
   v_Needs_Other_Permission_2 = g_default_Needs_Other_Permission_2;
   v_Needs_Other_Procedure = g_default_Needs_Other_Procedure;
   v_Needs_Other_Procedure_2 = g_default_Needs_Other_Procedure_2;
   v_Needs_Other_Source_Child = g_default_Needs_Other_Source_Child;
   v_Needs_Other_Source_Child_2 = g_default_Needs_Other_Source_Child_2;
   v_Needs_Permission = g_default_Needs_Permission;
   v_Needs_Procedure = g_default_Needs_Procedure;
   v_Needs_Procedure_Arg = g_default_Needs_Procedure_Arg;
   v_Needs_Procedure_Arg_2 = g_default_Needs_Procedure_Arg_2;
   v_Needs_Procedure_Arg_3 = g_default_Needs_Procedure_Arg_3;
   v_Needs_Source_Child = g_default_Needs_Source_Child;
   v_Needs_Source_Field = g_default_Needs_Source_Field;
   v_Needs_Source_Field_Or_Child = g_default_Needs_Source_Field_Or_Child;
   v_Needs_Source_Parent = g_default_Needs_Source_Parent;
   v_Needs_Test_Field = g_default_Needs_Test_Field;
   v_Needs_Test_Value = g_default_Needs_Test_Value;
   v_Needs_Value = g_default_Needs_Value;
   v_Next_Protect_Child_Rel = g_default_Next_Protect_Child_Rel;
   v_Next_Protect_Procedure = g_default_Next_Protect_Procedure;
   v_Next_Protect_Source_Parent = g_default_Next_Protect_Source_Parent;
   v_Next_Specification_Actions = g_default_Next_Specification_Actions;
   v_Notes = g_default_Notes;
   v_Procedure_Arg_primitive = g_default_Procedure_Arg_primitive;
   v_Procedure_Arg_type = g_default_Procedure_Arg_type;
   v_Protect_Child_Rel_From_Update = g_default_Protect_Child_Rel_From_Update;
   v_Protect_Class_From_Edit = g_default_Protect_Class_From_Edit;
   v_Protect_Class_From_Update = g_default_Protect_Class_From_Update;
   v_Protect_Other_Class_From_Edit = g_default_Protect_Other_Class_From_Edit;
   v_Protect_Procedure_From_Edit = g_default_Protect_Procedure_From_Edit;
   v_Protect_Source_Parent = g_default_Protect_Source_Parent;
   v_Set_Field_To_Source_Field_Or_Child = g_default_Set_Field_To_Source_Field_Or_Child;
   v_Source_Field_Needs_Test_Field = g_default_Source_Field_Needs_Test_Field;
   v_Source_Field_Type_Match = g_default_Source_Field_Type_Match;
   v_Source_Parent_type = g_default_Source_Parent_type;
   v_Source_type = g_default_Source_type;
   v_Specification_Actions = g_default_Specification_Actions;
   v_Specification_Name = g_default_Specification_Name;
   v_Specification_Object = g_default_Specification_Object;
   v_Specification_Strings = g_default_Specification_Strings;
   v_Specification_Vars = g_default_Specification_Vars;
   v_Test_Field_Type_Match = g_default_Test_Field_Type_Match;
   v_Test_Field_type = g_default_Test_Field_type;
   v_Use_Class_As_Source_Parent_Class = g_default_Use_Class_As_Source_Parent_Class;
   v_Use_Field_Enum = g_default_Use_Field_Enum;
   v_Use_Parent_Child_Rel_As_Source_Parent = g_default_Use_Parent_Child_Rel_As_Source_Parent;
   v_Use_Parent_Child_Rel_For_Class = g_default_Use_Parent_Child_Rel_For_Class;
   v_Use_Parent_Child_Rel_For_Other_Class = g_default_Use_Parent_Child_Rel_For_Other_Class;
   v_Use_Parent_Class = g_default_Use_Parent_Class;
   v_Use_Parent_Field_For_Class = g_default_Use_Parent_Field_For_Class;
   v_Use_Parent_Other_Class = g_default_Use_Parent_Other_Class;
   v_Use_Parent_Other_Class_For_Class = g_default_Use_Parent_Other_Class_For_Class;
   v_Use_Parent_Procedure = g_default_Use_Parent_Procedure;
   v_Use_Parent_Source_Class = g_default_Use_Parent_Source_Class;
   v_Use_Parent_Source_Field_For_Class = g_default_Use_Parent_Source_Field_For_Class;
   v_Use_Parent_Source_Parent = g_default_Use_Parent_Source_Parent;
   v_Use_Parent_Source_Parent_For_Class = g_default_Use_Parent_Source_Parent_For_Class;
   v_Use_Source_Field_Enum = g_default_Use_Source_Field_Enum;
   v_Use_Source_Field_Or_Child_Enum = g_default_Use_Source_Field_Or_Child_Enum;
   v_View_Id = g_default_View_Id;

   v_Child_Specification_Type = string( );
   if( cp_Child_Specification_Type )
      p_obj->setup_foreign_key( *cp_Child_Specification_Type, v_Child_Specification_Type );

   v_Next_Child_Specification_Type = string( );
   if( cp_Next_Child_Specification_Type )
      p_obj->setup_foreign_key( *cp_Next_Child_Specification_Type, v_Next_Child_Specification_Type );

   v_Parent_Specification_Type = string( );
   if( cp_Parent_Specification_Type )
      p_obj->setup_foreign_key( *cp_Parent_Specification_Type, v_Parent_Specification_Type );
}

bool Meta_Specification_Type::impl::value_will_be_provided( const string& field_name )
{
   ( void )field_name;

   // [<start value_will_be_provided>]
   // [<finish value_will_be_provided>]

   return false;
}

void Meta_Specification_Type::impl::validate(
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

   if( is_null( v_Specification_Name ) && !value_will_be_provided( c_field_name_Specification_Name ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Specification_Name,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_field_must_not_be_empty_field, get_module_string( c_field_display_name_Specification_Name ) ) ) ) );

   if( is_null( v_Specification_Object ) && !value_will_be_provided( c_field_name_Specification_Object ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Specification_Object,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_field_must_not_be_empty_field, get_module_string( c_field_display_name_Specification_Object ) ) ) ) );

   if( !is_null( v_Child_Prefix )
    && ( v_Child_Prefix != g_default_Child_Prefix
    || !value_will_be_provided( c_field_name_Child_Prefix ) )
    && !g_Child_Prefix_domain.is_valid( v_Child_Prefix, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Child_Prefix,
       get_module_string( c_field_display_name_Child_Prefix ) + " " + error_message ) );

   if( !is_null( v_Name )
    && ( v_Name != g_default_Name
    || !value_will_be_provided( c_field_name_Name ) )
    && !g_Name_domain.is_valid( v_Name, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Name,
       get_module_string( c_field_display_name_Name ) + " " + error_message ) );

   if( !is_null( v_Specification_Name )
    && ( v_Specification_Name != g_default_Specification_Name
    || !value_will_be_provided( c_field_name_Specification_Name ) )
    && !g_Specification_Name_domain.is_valid( v_Specification_Name, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Specification_Name,
       get_module_string( c_field_display_name_Specification_Name ) + " " + error_message ) );

   if( !is_null( v_Specification_Object )
    && ( v_Specification_Object != g_default_Specification_Object
    || !value_will_be_provided( c_field_name_Specification_Object ) )
    && !g_Specification_Object_domain.is_valid( v_Specification_Object, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Specification_Object,
       get_module_string( c_field_display_name_Specification_Object ) + " " + error_message ) );

   if( !is_null( v_View_Id )
    && ( v_View_Id != g_default_View_Id
    || !value_will_be_provided( c_field_name_View_Id ) )
    && !g_View_Id_domain.is_valid( v_View_Id, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_View_Id,
       get_module_string( c_field_display_name_View_Id ) + " " + error_message ) );

   if( !g_field_type_enum.count( v_Field_type ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Field_type,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_field_has_invalid_value_field, get_module_string( c_field_display_name_Field_type ) ) ) ) );

   if( !g_procedure_arg_primitive_enum.count( v_Procedure_Arg_primitive ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Procedure_Arg_primitive,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_field_has_invalid_value_field, get_module_string( c_field_display_name_Procedure_Arg_primitive ) ) ) ) );

   if( !g_procedure_arg_type_enum.count( v_Procedure_Arg_type ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Procedure_Arg_type,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_field_has_invalid_value_field, get_module_string( c_field_display_name_Procedure_Arg_type ) ) ) ) );

   if( !g_source_parent_type_enum.count( v_Source_Parent_type ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Source_Parent_type,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_field_has_invalid_value_field, get_module_string( c_field_display_name_Source_Parent_type ) ) ) ) );

   if( !g_source_type_enum.count( v_Source_type ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Source_type,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_field_has_invalid_value_field, get_module_string( c_field_display_name_Source_type ) ) ) ) );

   if( !g_test_field_type_enum.count( v_Test_Field_type ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Test_Field_type,
       get_string_message( GS( c_str_field_has_invalid_value ), make_pair(
       c_str_field_has_invalid_value_field, get_module_string( c_field_display_name_Test_Field_type ) ) ) ) );

   // [<start validate>]
   // [<finish validate>]
}

void Meta_Specification_Type::impl::validate_set_fields(
 set< string >& fields_set, validation_error_container* p_validation_errors )
{
   ( void )fields_set;

   if( !p_validation_errors )
      throw runtime_error( "unexpected null validation_errors container" );

   string error_message;
   validate_formatter vf;

   if( !is_null( v_Child_Prefix )
    && ( fields_set.count( c_field_id_Child_Prefix ) || fields_set.count( c_field_name_Child_Prefix ) )
    && !g_Child_Prefix_domain.is_valid( v_Child_Prefix, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Child_Prefix,
       get_module_string( c_field_display_name_Child_Prefix ) + " " + error_message ) );

   if( !is_null( v_Name )
    && ( fields_set.count( c_field_id_Name ) || fields_set.count( c_field_name_Name ) )
    && !g_Name_domain.is_valid( v_Name, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Name,
       get_module_string( c_field_display_name_Name ) + " " + error_message ) );

   if( !is_null( v_Specification_Name )
    && ( fields_set.count( c_field_id_Specification_Name ) || fields_set.count( c_field_name_Specification_Name ) )
    && !g_Specification_Name_domain.is_valid( v_Specification_Name, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Specification_Name,
       get_module_string( c_field_display_name_Specification_Name ) + " " + error_message ) );

   if( !is_null( v_Specification_Object )
    && ( fields_set.count( c_field_id_Specification_Object ) || fields_set.count( c_field_name_Specification_Object ) )
    && !g_Specification_Object_domain.is_valid( v_Specification_Object, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_Specification_Object,
       get_module_string( c_field_display_name_Specification_Object ) + " " + error_message ) );

   if( !is_null( v_View_Id )
    && ( fields_set.count( c_field_id_View_Id ) || fields_set.count( c_field_name_View_Id ) )
    && !g_View_Id_domain.is_valid( v_View_Id, error_message = "" ) )
      p_validation_errors->insert( construct_validation_error( vf.num, c_field_name_View_Id,
       get_module_string( c_field_display_name_View_Id ) + " " + error_message ) );
}

void Meta_Specification_Type::impl::after_fetch( )
{
   if( !get_obj( ).get_is_iterating( ) || get_obj( ).get_is_starting_iteration( ) )
      get_required_transients( );

   if( cp_Child_Specification_Type )
      p_obj->setup_foreign_key( *cp_Child_Specification_Type, v_Child_Specification_Type );

   if( cp_Next_Child_Specification_Type )
      p_obj->setup_foreign_key( *cp_Next_Child_Specification_Type, v_Next_Child_Specification_Type );

   if( cp_Parent_Specification_Type )
      p_obj->setup_foreign_key( *cp_Parent_Specification_Type, v_Parent_Specification_Type );

   post_init( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_fetch>]
   // [<finish after_fetch>]
}

void Meta_Specification_Type::impl::completed_fetch( bool skip_set_original )
{
   if( !skip_set_original && !get_obj( ).get_key( ).empty( ) )
      get_obj( ).set_new_original_values( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start completed_fetch>]
   // [<finish completed_fetch>]
}

void Meta_Specification_Type::impl::at_create( )
{
   // [<start at_create>]
   // [<finish at_create>]
}

void Meta_Specification_Type::impl::post_init( )
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start post_init>]
   // [<finish post_init>]
}

void Meta_Specification_Type::impl::to_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   if( !get_obj( ).get_is_preparing( ) )
      post_init( );

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start default_to_field)] 600251
   if( ( is_create || get_obj( ).get_key( ).empty( ) )
    && get_obj( ).get_clone_key( ).empty( )
    && ( get_obj( ).Allow_Procedure_Arg( ) == g_default_Allow_Procedure_Arg )
    && get_obj( ).Needs_Procedure_Arg( ) == true )
      get_obj( ).Allow_Procedure_Arg( get_obj( ).Needs_Procedure_Arg( ) );
   // [(finish default_to_field)] 600251

   // [(start default_to_field)] 600252
   if( ( is_create || get_obj( ).get_key( ).empty( ) )
    && get_obj( ).get_clone_key( ).empty( )
    && ( get_obj( ).Allow_Other_Procedure( ) == g_default_Allow_Other_Procedure )
    && get_obj( ).Needs_Other_Procedure( ) == true )
      get_obj( ).Allow_Other_Procedure( get_obj( ).Needs_Other_Procedure( ) );
   // [(finish default_to_field)] 600252

   // [(start default_to_field)] 600253
   if( ( is_create || get_obj( ).get_key( ).empty( ) )
    && get_obj( ).get_clone_key( ).empty( )
    && ( get_obj( ).Allow_Other_Procedure_2( ) == g_default_Allow_Other_Procedure_2 )
    && get_obj( ).Needs_Other_Procedure_2( ) == true )
      get_obj( ).Allow_Other_Procedure_2( get_obj( ).Needs_Other_Procedure_2( ) );
   // [(finish default_to_field)] 600253

   // [<start to_store>]
   // [<finish to_store>]
}

void Meta_Specification_Type::impl::for_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start for_store>]
   // [<finish for_store>]
}

void Meta_Specification_Type::impl::after_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_store>]
   // [<finish after_store>]
}

bool Meta_Specification_Type::impl::can_destroy( bool is_internal )
{
   uint64_t state = p_obj->get_state( );

   bool retval = ( is_internal || !( state & c_state_undeletable ) );

   // [<start can_destroy>]
   // [<finish can_destroy>]

   return retval;
}

void Meta_Specification_Type::impl::for_destroy( bool is_internal )
{
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start for_destroy>]
   // [<finish for_destroy>]
}

void Meta_Specification_Type::impl::after_destroy( bool is_internal )
{
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start after_destroy>]
   // [<finish after_destroy>]
}

void Meta_Specification_Type::impl::set_default_values( )
{
   clear( );
}

bool Meta_Specification_Type::impl::is_filtered( ) const
{
   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [<start is_filtered>]
   // [<finish is_filtered>]

   return false;
}

void Meta_Specification_Type::impl::get_required_transients( ) const
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

Meta_Specification_Type::Meta_Specification_Type( )
{
   set_version( c_version );

   p_impl = new impl( *this );
}

Meta_Specification_Type::~Meta_Specification_Type( )
{
   cleanup( );
   delete p_impl;
}

bool Meta_Specification_Type::Allow_Child_Relationship( ) const
{
   return p_impl->impl_Allow_Child_Relationship( );
}

void Meta_Specification_Type::Allow_Child_Relationship( bool Allow_Child_Relationship )
{
   p_impl->impl_Allow_Child_Relationship( Allow_Child_Relationship );
}

bool Meta_Specification_Type::Allow_Enum( ) const
{
   return p_impl->impl_Allow_Enum( );
}

void Meta_Specification_Type::Allow_Enum( bool Allow_Enum )
{
   p_impl->impl_Allow_Enum( Allow_Enum );
}

bool Meta_Specification_Type::Allow_Enum_Item( ) const
{
   return p_impl->impl_Allow_Enum_Item( );
}

void Meta_Specification_Type::Allow_Enum_Item( bool Allow_Enum_Item )
{
   p_impl->impl_Allow_Enum_Item( Allow_Enum_Item );
}

bool Meta_Specification_Type::Allow_FK_Source_Field_Choice( ) const
{
   return p_impl->impl_Allow_FK_Source_Field_Choice( );
}

void Meta_Specification_Type::Allow_FK_Source_Field_Choice( bool Allow_FK_Source_Field_Choice )
{
   p_impl->impl_Allow_FK_Source_Field_Choice( Allow_FK_Source_Field_Choice );
}

bool Meta_Specification_Type::Allow_FK_Test_Field_Choice( ) const
{
   return p_impl->impl_Allow_FK_Test_Field_Choice( );
}

void Meta_Specification_Type::Allow_FK_Test_Field_Choice( bool Allow_FK_Test_Field_Choice )
{
   p_impl->impl_Allow_FK_Test_Field_Choice( Allow_FK_Test_Field_Choice );
}

bool Meta_Specification_Type::Allow_Field( ) const
{
   return p_impl->impl_Allow_Field( );
}

void Meta_Specification_Type::Allow_Field( bool Allow_Field )
{
   p_impl->impl_Allow_Field( Allow_Field );
}

bool Meta_Specification_Type::Allow_Modifier( ) const
{
   return p_impl->impl_Allow_Modifier( );
}

void Meta_Specification_Type::Allow_Modifier( bool Allow_Modifier )
{
   p_impl->impl_Allow_Modifier( Allow_Modifier );
}

bool Meta_Specification_Type::Allow_Options( ) const
{
   return p_impl->impl_Allow_Options( );
}

void Meta_Specification_Type::Allow_Options( bool Allow_Options )
{
   p_impl->impl_Allow_Options( Allow_Options );
}

bool Meta_Specification_Type::Allow_Other_Class( ) const
{
   return p_impl->impl_Allow_Other_Class( );
}

void Meta_Specification_Type::Allow_Other_Class( bool Allow_Other_Class )
{
   p_impl->impl_Allow_Other_Class( Allow_Other_Class );
}

bool Meta_Specification_Type::Allow_Other_Class_Field( ) const
{
   return p_impl->impl_Allow_Other_Class_Field( );
}

void Meta_Specification_Type::Allow_Other_Class_Field( bool Allow_Other_Class_Field )
{
   p_impl->impl_Allow_Other_Class_Field( Allow_Other_Class_Field );
}

bool Meta_Specification_Type::Allow_Other_Field( ) const
{
   return p_impl->impl_Allow_Other_Field( );
}

void Meta_Specification_Type::Allow_Other_Field( bool Allow_Other_Field )
{
   p_impl->impl_Allow_Other_Field( Allow_Other_Field );
}

bool Meta_Specification_Type::Allow_Other_Field_2( ) const
{
   return p_impl->impl_Allow_Other_Field_2( );
}

void Meta_Specification_Type::Allow_Other_Field_2( bool Allow_Other_Field_2 )
{
   p_impl->impl_Allow_Other_Field_2( Allow_Other_Field_2 );
}

bool Meta_Specification_Type::Allow_Other_Modifier( ) const
{
   return p_impl->impl_Allow_Other_Modifier( );
}

void Meta_Specification_Type::Allow_Other_Modifier( bool Allow_Other_Modifier )
{
   p_impl->impl_Allow_Other_Modifier( Allow_Other_Modifier );
}

bool Meta_Specification_Type::Allow_Other_Modifier_2( ) const
{
   return p_impl->impl_Allow_Other_Modifier_2( );
}

void Meta_Specification_Type::Allow_Other_Modifier_2( bool Allow_Other_Modifier_2 )
{
   p_impl->impl_Allow_Other_Modifier_2( Allow_Other_Modifier_2 );
}

bool Meta_Specification_Type::Allow_Other_Procedure( ) const
{
   return p_impl->impl_Allow_Other_Procedure( );
}

void Meta_Specification_Type::Allow_Other_Procedure( bool Allow_Other_Procedure )
{
   p_impl->impl_Allow_Other_Procedure( Allow_Other_Procedure );
}

bool Meta_Specification_Type::Allow_Other_Procedure_2( ) const
{
   return p_impl->impl_Allow_Other_Procedure_2( );
}

void Meta_Specification_Type::Allow_Other_Procedure_2( bool Allow_Other_Procedure_2 )
{
   p_impl->impl_Allow_Other_Procedure_2( Allow_Other_Procedure_2 );
}

bool Meta_Specification_Type::Allow_Other_Source_Child( ) const
{
   return p_impl->impl_Allow_Other_Source_Child( );
}

void Meta_Specification_Type::Allow_Other_Source_Child( bool Allow_Other_Source_Child )
{
   p_impl->impl_Allow_Other_Source_Child( Allow_Other_Source_Child );
}

bool Meta_Specification_Type::Allow_Other_Source_Child_2( ) const
{
   return p_impl->impl_Allow_Other_Source_Child_2( );
}

void Meta_Specification_Type::Allow_Other_Source_Child_2( bool Allow_Other_Source_Child_2 )
{
   p_impl->impl_Allow_Other_Source_Child_2( Allow_Other_Source_Child_2 );
}

bool Meta_Specification_Type::Allow_Permission( ) const
{
   return p_impl->impl_Allow_Permission( );
}

void Meta_Specification_Type::Allow_Permission( bool Allow_Permission )
{
   p_impl->impl_Allow_Permission( Allow_Permission );
}

bool Meta_Specification_Type::Allow_Procedure( ) const
{
   return p_impl->impl_Allow_Procedure( );
}

void Meta_Specification_Type::Allow_Procedure( bool Allow_Procedure )
{
   p_impl->impl_Allow_Procedure( Allow_Procedure );
}

bool Meta_Specification_Type::Allow_Procedure_Arg( ) const
{
   return p_impl->impl_Allow_Procedure_Arg( );
}

void Meta_Specification_Type::Allow_Procedure_Arg( bool Allow_Procedure_Arg )
{
   p_impl->impl_Allow_Procedure_Arg( Allow_Procedure_Arg );
}

bool Meta_Specification_Type::Allow_Source_Child( ) const
{
   return p_impl->impl_Allow_Source_Child( );
}

void Meta_Specification_Type::Allow_Source_Child( bool Allow_Source_Child )
{
   p_impl->impl_Allow_Source_Child( Allow_Source_Child );
}

bool Meta_Specification_Type::Allow_Source_Class( ) const
{
   return p_impl->impl_Allow_Source_Class( );
}

void Meta_Specification_Type::Allow_Source_Class( bool Allow_Source_Class )
{
   p_impl->impl_Allow_Source_Class( Allow_Source_Class );
}

bool Meta_Specification_Type::Allow_Source_Field( ) const
{
   return p_impl->impl_Allow_Source_Field( );
}

void Meta_Specification_Type::Allow_Source_Field( bool Allow_Source_Field )
{
   p_impl->impl_Allow_Source_Field( Allow_Source_Field );
}

bool Meta_Specification_Type::Allow_Source_Grandchild( ) const
{
   return p_impl->impl_Allow_Source_Grandchild( );
}

void Meta_Specification_Type::Allow_Source_Grandchild( bool Allow_Source_Grandchild )
{
   p_impl->impl_Allow_Source_Grandchild( Allow_Source_Grandchild );
}

bool Meta_Specification_Type::Allow_Source_Parent( ) const
{
   return p_impl->impl_Allow_Source_Parent( );
}

void Meta_Specification_Type::Allow_Source_Parent( bool Allow_Source_Parent )
{
   p_impl->impl_Allow_Source_Parent( Allow_Source_Parent );
}

bool Meta_Specification_Type::Allow_Test_Field( ) const
{
   return p_impl->impl_Allow_Test_Field( );
}

void Meta_Specification_Type::Allow_Test_Field( bool Allow_Test_Field )
{
   p_impl->impl_Allow_Test_Field( Allow_Test_Field );
}

bool Meta_Specification_Type::Allow_Test_Value( ) const
{
   return p_impl->impl_Allow_Test_Value( );
}

void Meta_Specification_Type::Allow_Test_Value( bool Allow_Test_Value )
{
   p_impl->impl_Allow_Test_Value( Allow_Test_Value );
}

bool Meta_Specification_Type::Allow_Value( ) const
{
   return p_impl->impl_Allow_Value( );
}

void Meta_Specification_Type::Allow_Value( bool Allow_Value )
{
   p_impl->impl_Allow_Value( Allow_Value );
}

bool Meta_Specification_Type::Can_Access_Below_Model( ) const
{
   return p_impl->impl_Can_Access_Below_Model( );
}

void Meta_Specification_Type::Can_Access_Below_Model( bool Can_Access_Below_Model )
{
   p_impl->impl_Can_Access_Below_Model( Can_Access_Below_Model );
}

const string& Meta_Specification_Type::Child_Prefix( ) const
{
   return p_impl->impl_Child_Prefix( );
}

void Meta_Specification_Type::Child_Prefix( const string& Child_Prefix )
{
   p_impl->impl_Child_Prefix( Child_Prefix );
}

bool Meta_Specification_Type::Child_Rel_Grandparent_Match( ) const
{
   return p_impl->impl_Child_Rel_Grandparent_Match( );
}

void Meta_Specification_Type::Child_Rel_Grandparent_Match( bool Child_Rel_Grandparent_Match )
{
   p_impl->impl_Child_Rel_Grandparent_Match( Child_Rel_Grandparent_Match );
}

bool Meta_Specification_Type::Child_Rel_Parent_Match( ) const
{
   return p_impl->impl_Child_Rel_Parent_Match( );
}

void Meta_Specification_Type::Child_Rel_Parent_Match( bool Child_Rel_Parent_Match )
{
   p_impl->impl_Child_Rel_Parent_Match( Child_Rel_Parent_Match );
}

bool Meta_Specification_Type::Child_Relationship_Class_Match( ) const
{
   return p_impl->impl_Child_Relationship_Class_Match( );
}

void Meta_Specification_Type::Child_Relationship_Class_Match( bool Child_Relationship_Class_Match )
{
   p_impl->impl_Child_Relationship_Class_Match( Child_Relationship_Class_Match );
}

const string& Meta_Specification_Type::Default_Child_Vars( ) const
{
   return p_impl->impl_Default_Child_Vars( );
}

void Meta_Specification_Type::Default_Child_Vars( const string& Default_Child_Vars )
{
   p_impl->impl_Default_Child_Vars( Default_Child_Vars );
}

int Meta_Specification_Type::Field_type( ) const
{
   return p_impl->impl_Field_type( );
}

void Meta_Specification_Type::Field_type( int Field_type )
{
   p_impl->impl_Field_type( Field_type );
}

bool Meta_Specification_Type::Has_Next_Specification_Info( ) const
{
   return p_impl->impl_Has_Next_Specification_Info( );
}

void Meta_Specification_Type::Has_Next_Specification_Info( bool Has_Next_Specification_Info )
{
   p_impl->impl_Has_Next_Specification_Info( Has_Next_Specification_Info );
}

bool Meta_Specification_Type::Is_Child_Only( ) const
{
   return p_impl->impl_Is_Child_Only( );
}

void Meta_Specification_Type::Is_Child_Only( bool Is_Child_Only )
{
   p_impl->impl_Is_Child_Only( Is_Child_Only );
}

bool Meta_Specification_Type::Is_Required_For_UI_Gen( ) const
{
   return p_impl->impl_Is_Required_For_UI_Gen( );
}

void Meta_Specification_Type::Is_Required_For_UI_Gen( bool Is_Required_For_UI_Gen )
{
   p_impl->impl_Is_Required_For_UI_Gen( Is_Required_For_UI_Gen );
}

bool Meta_Specification_Type::Is_System( ) const
{
   return p_impl->impl_Is_System( );
}

void Meta_Specification_Type::Is_System( bool Is_System )
{
   p_impl->impl_Is_System( Is_System );
}

const string& Meta_Specification_Type::Name( ) const
{
   return p_impl->impl_Name( );
}

void Meta_Specification_Type::Name( const string& Name )
{
   p_impl->impl_Name( Name );
}

bool Meta_Specification_Type::Needs_Child_Relationship( ) const
{
   return p_impl->impl_Needs_Child_Relationship( );
}

void Meta_Specification_Type::Needs_Child_Relationship( bool Needs_Child_Relationship )
{
   p_impl->impl_Needs_Child_Relationship( Needs_Child_Relationship );
}

bool Meta_Specification_Type::Needs_Class( ) const
{
   return p_impl->impl_Needs_Class( );
}

void Meta_Specification_Type::Needs_Class( bool Needs_Class )
{
   p_impl->impl_Needs_Class( Needs_Class );
}

bool Meta_Specification_Type::Needs_Enum( ) const
{
   return p_impl->impl_Needs_Enum( );
}

void Meta_Specification_Type::Needs_Enum( bool Needs_Enum )
{
   p_impl->impl_Needs_Enum( Needs_Enum );
}

bool Meta_Specification_Type::Needs_Enum_Item( ) const
{
   return p_impl->impl_Needs_Enum_Item( );
}

void Meta_Specification_Type::Needs_Enum_Item( bool Needs_Enum_Item )
{
   p_impl->impl_Needs_Enum_Item( Needs_Enum_Item );
}

bool Meta_Specification_Type::Needs_Enum_Item_2( ) const
{
   return p_impl->impl_Needs_Enum_Item_2( );
}

void Meta_Specification_Type::Needs_Enum_Item_2( bool Needs_Enum_Item_2 )
{
   p_impl->impl_Needs_Enum_Item_2( Needs_Enum_Item_2 );
}

bool Meta_Specification_Type::Needs_Enum_Item_3( ) const
{
   return p_impl->impl_Needs_Enum_Item_3( );
}

void Meta_Specification_Type::Needs_Enum_Item_3( bool Needs_Enum_Item_3 )
{
   p_impl->impl_Needs_Enum_Item_3( Needs_Enum_Item_3 );
}

bool Meta_Specification_Type::Needs_Enum_Item_4( ) const
{
   return p_impl->impl_Needs_Enum_Item_4( );
}

void Meta_Specification_Type::Needs_Enum_Item_4( bool Needs_Enum_Item_4 )
{
   p_impl->impl_Needs_Enum_Item_4( Needs_Enum_Item_4 );
}

bool Meta_Specification_Type::Needs_Enum_Item_5( ) const
{
   return p_impl->impl_Needs_Enum_Item_5( );
}

void Meta_Specification_Type::Needs_Enum_Item_5( bool Needs_Enum_Item_5 )
{
   p_impl->impl_Needs_Enum_Item_5( Needs_Enum_Item_5 );
}

bool Meta_Specification_Type::Needs_Field( ) const
{
   return p_impl->impl_Needs_Field( );
}

void Meta_Specification_Type::Needs_Field( bool Needs_Field )
{
   p_impl->impl_Needs_Field( Needs_Field );
}

bool Meta_Specification_Type::Needs_Modifier( ) const
{
   return p_impl->impl_Needs_Modifier( );
}

void Meta_Specification_Type::Needs_Modifier( bool Needs_Modifier )
{
   p_impl->impl_Needs_Modifier( Needs_Modifier );
}

bool Meta_Specification_Type::Needs_Other_Class( ) const
{
   return p_impl->impl_Needs_Other_Class( );
}

void Meta_Specification_Type::Needs_Other_Class( bool Needs_Other_Class )
{
   p_impl->impl_Needs_Other_Class( Needs_Other_Class );
}

bool Meta_Specification_Type::Needs_Other_Class_Field( ) const
{
   return p_impl->impl_Needs_Other_Class_Field( );
}

void Meta_Specification_Type::Needs_Other_Class_Field( bool Needs_Other_Class_Field )
{
   p_impl->impl_Needs_Other_Class_Field( Needs_Other_Class_Field );
}

bool Meta_Specification_Type::Needs_Other_Field( ) const
{
   return p_impl->impl_Needs_Other_Field( );
}

void Meta_Specification_Type::Needs_Other_Field( bool Needs_Other_Field )
{
   p_impl->impl_Needs_Other_Field( Needs_Other_Field );
}

bool Meta_Specification_Type::Needs_Other_Field_2( ) const
{
   return p_impl->impl_Needs_Other_Field_2( );
}

void Meta_Specification_Type::Needs_Other_Field_2( bool Needs_Other_Field_2 )
{
   p_impl->impl_Needs_Other_Field_2( Needs_Other_Field_2 );
}

bool Meta_Specification_Type::Needs_Other_Modifier( ) const
{
   return p_impl->impl_Needs_Other_Modifier( );
}

void Meta_Specification_Type::Needs_Other_Modifier( bool Needs_Other_Modifier )
{
   p_impl->impl_Needs_Other_Modifier( Needs_Other_Modifier );
}

bool Meta_Specification_Type::Needs_Other_Modifier_2( ) const
{
   return p_impl->impl_Needs_Other_Modifier_2( );
}

void Meta_Specification_Type::Needs_Other_Modifier_2( bool Needs_Other_Modifier_2 )
{
   p_impl->impl_Needs_Other_Modifier_2( Needs_Other_Modifier_2 );
}

bool Meta_Specification_Type::Needs_Other_Permission( ) const
{
   return p_impl->impl_Needs_Other_Permission( );
}

void Meta_Specification_Type::Needs_Other_Permission( bool Needs_Other_Permission )
{
   p_impl->impl_Needs_Other_Permission( Needs_Other_Permission );
}

bool Meta_Specification_Type::Needs_Other_Permission_2( ) const
{
   return p_impl->impl_Needs_Other_Permission_2( );
}

void Meta_Specification_Type::Needs_Other_Permission_2( bool Needs_Other_Permission_2 )
{
   p_impl->impl_Needs_Other_Permission_2( Needs_Other_Permission_2 );
}

bool Meta_Specification_Type::Needs_Other_Procedure( ) const
{
   return p_impl->impl_Needs_Other_Procedure( );
}

void Meta_Specification_Type::Needs_Other_Procedure( bool Needs_Other_Procedure )
{
   p_impl->impl_Needs_Other_Procedure( Needs_Other_Procedure );
}

bool Meta_Specification_Type::Needs_Other_Procedure_2( ) const
{
   return p_impl->impl_Needs_Other_Procedure_2( );
}

void Meta_Specification_Type::Needs_Other_Procedure_2( bool Needs_Other_Procedure_2 )
{
   p_impl->impl_Needs_Other_Procedure_2( Needs_Other_Procedure_2 );
}

bool Meta_Specification_Type::Needs_Other_Source_Child( ) const
{
   return p_impl->impl_Needs_Other_Source_Child( );
}

void Meta_Specification_Type::Needs_Other_Source_Child( bool Needs_Other_Source_Child )
{
   p_impl->impl_Needs_Other_Source_Child( Needs_Other_Source_Child );
}

bool Meta_Specification_Type::Needs_Other_Source_Child_2( ) const
{
   return p_impl->impl_Needs_Other_Source_Child_2( );
}

void Meta_Specification_Type::Needs_Other_Source_Child_2( bool Needs_Other_Source_Child_2 )
{
   p_impl->impl_Needs_Other_Source_Child_2( Needs_Other_Source_Child_2 );
}

bool Meta_Specification_Type::Needs_Permission( ) const
{
   return p_impl->impl_Needs_Permission( );
}

void Meta_Specification_Type::Needs_Permission( bool Needs_Permission )
{
   p_impl->impl_Needs_Permission( Needs_Permission );
}

bool Meta_Specification_Type::Needs_Procedure( ) const
{
   return p_impl->impl_Needs_Procedure( );
}

void Meta_Specification_Type::Needs_Procedure( bool Needs_Procedure )
{
   p_impl->impl_Needs_Procedure( Needs_Procedure );
}

bool Meta_Specification_Type::Needs_Procedure_Arg( ) const
{
   return p_impl->impl_Needs_Procedure_Arg( );
}

void Meta_Specification_Type::Needs_Procedure_Arg( bool Needs_Procedure_Arg )
{
   p_impl->impl_Needs_Procedure_Arg( Needs_Procedure_Arg );
}

bool Meta_Specification_Type::Needs_Procedure_Arg_2( ) const
{
   return p_impl->impl_Needs_Procedure_Arg_2( );
}

void Meta_Specification_Type::Needs_Procedure_Arg_2( bool Needs_Procedure_Arg_2 )
{
   p_impl->impl_Needs_Procedure_Arg_2( Needs_Procedure_Arg_2 );
}

bool Meta_Specification_Type::Needs_Procedure_Arg_3( ) const
{
   return p_impl->impl_Needs_Procedure_Arg_3( );
}

void Meta_Specification_Type::Needs_Procedure_Arg_3( bool Needs_Procedure_Arg_3 )
{
   p_impl->impl_Needs_Procedure_Arg_3( Needs_Procedure_Arg_3 );
}

bool Meta_Specification_Type::Needs_Source_Child( ) const
{
   return p_impl->impl_Needs_Source_Child( );
}

void Meta_Specification_Type::Needs_Source_Child( bool Needs_Source_Child )
{
   p_impl->impl_Needs_Source_Child( Needs_Source_Child );
}

bool Meta_Specification_Type::Needs_Source_Field( ) const
{
   return p_impl->impl_Needs_Source_Field( );
}

void Meta_Specification_Type::Needs_Source_Field( bool Needs_Source_Field )
{
   p_impl->impl_Needs_Source_Field( Needs_Source_Field );
}

bool Meta_Specification_Type::Needs_Source_Field_Or_Child( ) const
{
   return p_impl->impl_Needs_Source_Field_Or_Child( );
}

void Meta_Specification_Type::Needs_Source_Field_Or_Child( bool Needs_Source_Field_Or_Child )
{
   p_impl->impl_Needs_Source_Field_Or_Child( Needs_Source_Field_Or_Child );
}

bool Meta_Specification_Type::Needs_Source_Parent( ) const
{
   return p_impl->impl_Needs_Source_Parent( );
}

void Meta_Specification_Type::Needs_Source_Parent( bool Needs_Source_Parent )
{
   p_impl->impl_Needs_Source_Parent( Needs_Source_Parent );
}

bool Meta_Specification_Type::Needs_Test_Field( ) const
{
   return p_impl->impl_Needs_Test_Field( );
}

void Meta_Specification_Type::Needs_Test_Field( bool Needs_Test_Field )
{
   p_impl->impl_Needs_Test_Field( Needs_Test_Field );
}

bool Meta_Specification_Type::Needs_Test_Value( ) const
{
   return p_impl->impl_Needs_Test_Value( );
}

void Meta_Specification_Type::Needs_Test_Value( bool Needs_Test_Value )
{
   p_impl->impl_Needs_Test_Value( Needs_Test_Value );
}

bool Meta_Specification_Type::Needs_Value( ) const
{
   return p_impl->impl_Needs_Value( );
}

void Meta_Specification_Type::Needs_Value( bool Needs_Value )
{
   p_impl->impl_Needs_Value( Needs_Value );
}

bool Meta_Specification_Type::Next_Protect_Child_Rel( ) const
{
   return p_impl->impl_Next_Protect_Child_Rel( );
}

void Meta_Specification_Type::Next_Protect_Child_Rel( bool Next_Protect_Child_Rel )
{
   p_impl->impl_Next_Protect_Child_Rel( Next_Protect_Child_Rel );
}

bool Meta_Specification_Type::Next_Protect_Procedure( ) const
{
   return p_impl->impl_Next_Protect_Procedure( );
}

void Meta_Specification_Type::Next_Protect_Procedure( bool Next_Protect_Procedure )
{
   p_impl->impl_Next_Protect_Procedure( Next_Protect_Procedure );
}

bool Meta_Specification_Type::Next_Protect_Source_Parent( ) const
{
   return p_impl->impl_Next_Protect_Source_Parent( );
}

void Meta_Specification_Type::Next_Protect_Source_Parent( bool Next_Protect_Source_Parent )
{
   p_impl->impl_Next_Protect_Source_Parent( Next_Protect_Source_Parent );
}

const string& Meta_Specification_Type::Next_Specification_Actions( ) const
{
   return p_impl->impl_Next_Specification_Actions( );
}

void Meta_Specification_Type::Next_Specification_Actions( const string& Next_Specification_Actions )
{
   p_impl->impl_Next_Specification_Actions( Next_Specification_Actions );
}

const string& Meta_Specification_Type::Notes( ) const
{
   return p_impl->impl_Notes( );
}

void Meta_Specification_Type::Notes( const string& Notes )
{
   p_impl->impl_Notes( Notes );
}

int Meta_Specification_Type::Procedure_Arg_primitive( ) const
{
   return p_impl->impl_Procedure_Arg_primitive( );
}

void Meta_Specification_Type::Procedure_Arg_primitive( int Procedure_Arg_primitive )
{
   p_impl->impl_Procedure_Arg_primitive( Procedure_Arg_primitive );
}

int Meta_Specification_Type::Procedure_Arg_type( ) const
{
   return p_impl->impl_Procedure_Arg_type( );
}

void Meta_Specification_Type::Procedure_Arg_type( int Procedure_Arg_type )
{
   p_impl->impl_Procedure_Arg_type( Procedure_Arg_type );
}

bool Meta_Specification_Type::Protect_Child_Rel_From_Update( ) const
{
   return p_impl->impl_Protect_Child_Rel_From_Update( );
}

void Meta_Specification_Type::Protect_Child_Rel_From_Update( bool Protect_Child_Rel_From_Update )
{
   p_impl->impl_Protect_Child_Rel_From_Update( Protect_Child_Rel_From_Update );
}

bool Meta_Specification_Type::Protect_Class_From_Edit( ) const
{
   return p_impl->impl_Protect_Class_From_Edit( );
}

void Meta_Specification_Type::Protect_Class_From_Edit( bool Protect_Class_From_Edit )
{
   p_impl->impl_Protect_Class_From_Edit( Protect_Class_From_Edit );
}

bool Meta_Specification_Type::Protect_Class_From_Update( ) const
{
   return p_impl->impl_Protect_Class_From_Update( );
}

void Meta_Specification_Type::Protect_Class_From_Update( bool Protect_Class_From_Update )
{
   p_impl->impl_Protect_Class_From_Update( Protect_Class_From_Update );
}

bool Meta_Specification_Type::Protect_Other_Class_From_Edit( ) const
{
   return p_impl->impl_Protect_Other_Class_From_Edit( );
}

void Meta_Specification_Type::Protect_Other_Class_From_Edit( bool Protect_Other_Class_From_Edit )
{
   p_impl->impl_Protect_Other_Class_From_Edit( Protect_Other_Class_From_Edit );
}

bool Meta_Specification_Type::Protect_Procedure_From_Edit( ) const
{
   return p_impl->impl_Protect_Procedure_From_Edit( );
}

void Meta_Specification_Type::Protect_Procedure_From_Edit( bool Protect_Procedure_From_Edit )
{
   p_impl->impl_Protect_Procedure_From_Edit( Protect_Procedure_From_Edit );
}

bool Meta_Specification_Type::Protect_Source_Parent( ) const
{
   return p_impl->impl_Protect_Source_Parent( );
}

void Meta_Specification_Type::Protect_Source_Parent( bool Protect_Source_Parent )
{
   p_impl->impl_Protect_Source_Parent( Protect_Source_Parent );
}

bool Meta_Specification_Type::Set_Field_To_Source_Field_Or_Child( ) const
{
   return p_impl->impl_Set_Field_To_Source_Field_Or_Child( );
}

void Meta_Specification_Type::Set_Field_To_Source_Field_Or_Child( bool Set_Field_To_Source_Field_Or_Child )
{
   p_impl->impl_Set_Field_To_Source_Field_Or_Child( Set_Field_To_Source_Field_Or_Child );
}

bool Meta_Specification_Type::Source_Field_Needs_Test_Field( ) const
{
   return p_impl->impl_Source_Field_Needs_Test_Field( );
}

void Meta_Specification_Type::Source_Field_Needs_Test_Field( bool Source_Field_Needs_Test_Field )
{
   p_impl->impl_Source_Field_Needs_Test_Field( Source_Field_Needs_Test_Field );
}

bool Meta_Specification_Type::Source_Field_Type_Match( ) const
{
   return p_impl->impl_Source_Field_Type_Match( );
}

void Meta_Specification_Type::Source_Field_Type_Match( bool Source_Field_Type_Match )
{
   p_impl->impl_Source_Field_Type_Match( Source_Field_Type_Match );
}

int Meta_Specification_Type::Source_Parent_type( ) const
{
   return p_impl->impl_Source_Parent_type( );
}

void Meta_Specification_Type::Source_Parent_type( int Source_Parent_type )
{
   p_impl->impl_Source_Parent_type( Source_Parent_type );
}

int Meta_Specification_Type::Source_type( ) const
{
   return p_impl->impl_Source_type( );
}

void Meta_Specification_Type::Source_type( int Source_type )
{
   p_impl->impl_Source_type( Source_type );
}

const string& Meta_Specification_Type::Specification_Actions( ) const
{
   return p_impl->impl_Specification_Actions( );
}

void Meta_Specification_Type::Specification_Actions( const string& Specification_Actions )
{
   p_impl->impl_Specification_Actions( Specification_Actions );
}

const string& Meta_Specification_Type::Specification_Name( ) const
{
   return p_impl->impl_Specification_Name( );
}

void Meta_Specification_Type::Specification_Name( const string& Specification_Name )
{
   p_impl->impl_Specification_Name( Specification_Name );
}

const string& Meta_Specification_Type::Specification_Object( ) const
{
   return p_impl->impl_Specification_Object( );
}

void Meta_Specification_Type::Specification_Object( const string& Specification_Object )
{
   p_impl->impl_Specification_Object( Specification_Object );
}

const string& Meta_Specification_Type::Specification_Strings( ) const
{
   return p_impl->impl_Specification_Strings( );
}

void Meta_Specification_Type::Specification_Strings( const string& Specification_Strings )
{
   p_impl->impl_Specification_Strings( Specification_Strings );
}

const string& Meta_Specification_Type::Specification_Vars( ) const
{
   return p_impl->impl_Specification_Vars( );
}

void Meta_Specification_Type::Specification_Vars( const string& Specification_Vars )
{
   p_impl->impl_Specification_Vars( Specification_Vars );
}

bool Meta_Specification_Type::Test_Field_Type_Match( ) const
{
   return p_impl->impl_Test_Field_Type_Match( );
}

void Meta_Specification_Type::Test_Field_Type_Match( bool Test_Field_Type_Match )
{
   p_impl->impl_Test_Field_Type_Match( Test_Field_Type_Match );
}

int Meta_Specification_Type::Test_Field_type( ) const
{
   return p_impl->impl_Test_Field_type( );
}

void Meta_Specification_Type::Test_Field_type( int Test_Field_type )
{
   p_impl->impl_Test_Field_type( Test_Field_type );
}

bool Meta_Specification_Type::Use_Class_As_Source_Parent_Class( ) const
{
   return p_impl->impl_Use_Class_As_Source_Parent_Class( );
}

void Meta_Specification_Type::Use_Class_As_Source_Parent_Class( bool Use_Class_As_Source_Parent_Class )
{
   p_impl->impl_Use_Class_As_Source_Parent_Class( Use_Class_As_Source_Parent_Class );
}

bool Meta_Specification_Type::Use_Field_Enum( ) const
{
   return p_impl->impl_Use_Field_Enum( );
}

void Meta_Specification_Type::Use_Field_Enum( bool Use_Field_Enum )
{
   p_impl->impl_Use_Field_Enum( Use_Field_Enum );
}

bool Meta_Specification_Type::Use_Parent_Child_Rel_As_Source_Parent( ) const
{
   return p_impl->impl_Use_Parent_Child_Rel_As_Source_Parent( );
}

void Meta_Specification_Type::Use_Parent_Child_Rel_As_Source_Parent( bool Use_Parent_Child_Rel_As_Source_Parent )
{
   p_impl->impl_Use_Parent_Child_Rel_As_Source_Parent( Use_Parent_Child_Rel_As_Source_Parent );
}

bool Meta_Specification_Type::Use_Parent_Child_Rel_For_Class( ) const
{
   return p_impl->impl_Use_Parent_Child_Rel_For_Class( );
}

void Meta_Specification_Type::Use_Parent_Child_Rel_For_Class( bool Use_Parent_Child_Rel_For_Class )
{
   p_impl->impl_Use_Parent_Child_Rel_For_Class( Use_Parent_Child_Rel_For_Class );
}

bool Meta_Specification_Type::Use_Parent_Child_Rel_For_Other_Class( ) const
{
   return p_impl->impl_Use_Parent_Child_Rel_For_Other_Class( );
}

void Meta_Specification_Type::Use_Parent_Child_Rel_For_Other_Class( bool Use_Parent_Child_Rel_For_Other_Class )
{
   p_impl->impl_Use_Parent_Child_Rel_For_Other_Class( Use_Parent_Child_Rel_For_Other_Class );
}

bool Meta_Specification_Type::Use_Parent_Class( ) const
{
   return p_impl->impl_Use_Parent_Class( );
}

void Meta_Specification_Type::Use_Parent_Class( bool Use_Parent_Class )
{
   p_impl->impl_Use_Parent_Class( Use_Parent_Class );
}

bool Meta_Specification_Type::Use_Parent_Field_For_Class( ) const
{
   return p_impl->impl_Use_Parent_Field_For_Class( );
}

void Meta_Specification_Type::Use_Parent_Field_For_Class( bool Use_Parent_Field_For_Class )
{
   p_impl->impl_Use_Parent_Field_For_Class( Use_Parent_Field_For_Class );
}

bool Meta_Specification_Type::Use_Parent_Other_Class( ) const
{
   return p_impl->impl_Use_Parent_Other_Class( );
}

void Meta_Specification_Type::Use_Parent_Other_Class( bool Use_Parent_Other_Class )
{
   p_impl->impl_Use_Parent_Other_Class( Use_Parent_Other_Class );
}

bool Meta_Specification_Type::Use_Parent_Other_Class_For_Class( ) const
{
   return p_impl->impl_Use_Parent_Other_Class_For_Class( );
}

void Meta_Specification_Type::Use_Parent_Other_Class_For_Class( bool Use_Parent_Other_Class_For_Class )
{
   p_impl->impl_Use_Parent_Other_Class_For_Class( Use_Parent_Other_Class_For_Class );
}

bool Meta_Specification_Type::Use_Parent_Procedure( ) const
{
   return p_impl->impl_Use_Parent_Procedure( );
}

void Meta_Specification_Type::Use_Parent_Procedure( bool Use_Parent_Procedure )
{
   p_impl->impl_Use_Parent_Procedure( Use_Parent_Procedure );
}

bool Meta_Specification_Type::Use_Parent_Source_Class( ) const
{
   return p_impl->impl_Use_Parent_Source_Class( );
}

void Meta_Specification_Type::Use_Parent_Source_Class( bool Use_Parent_Source_Class )
{
   p_impl->impl_Use_Parent_Source_Class( Use_Parent_Source_Class );
}

bool Meta_Specification_Type::Use_Parent_Source_Field_For_Class( ) const
{
   return p_impl->impl_Use_Parent_Source_Field_For_Class( );
}

void Meta_Specification_Type::Use_Parent_Source_Field_For_Class( bool Use_Parent_Source_Field_For_Class )
{
   p_impl->impl_Use_Parent_Source_Field_For_Class( Use_Parent_Source_Field_For_Class );
}

bool Meta_Specification_Type::Use_Parent_Source_Parent( ) const
{
   return p_impl->impl_Use_Parent_Source_Parent( );
}

void Meta_Specification_Type::Use_Parent_Source_Parent( bool Use_Parent_Source_Parent )
{
   p_impl->impl_Use_Parent_Source_Parent( Use_Parent_Source_Parent );
}

bool Meta_Specification_Type::Use_Parent_Source_Parent_For_Class( ) const
{
   return p_impl->impl_Use_Parent_Source_Parent_For_Class( );
}

void Meta_Specification_Type::Use_Parent_Source_Parent_For_Class( bool Use_Parent_Source_Parent_For_Class )
{
   p_impl->impl_Use_Parent_Source_Parent_For_Class( Use_Parent_Source_Parent_For_Class );
}

bool Meta_Specification_Type::Use_Source_Field_Enum( ) const
{
   return p_impl->impl_Use_Source_Field_Enum( );
}

void Meta_Specification_Type::Use_Source_Field_Enum( bool Use_Source_Field_Enum )
{
   p_impl->impl_Use_Source_Field_Enum( Use_Source_Field_Enum );
}

bool Meta_Specification_Type::Use_Source_Field_Or_Child_Enum( ) const
{
   return p_impl->impl_Use_Source_Field_Or_Child_Enum( );
}

void Meta_Specification_Type::Use_Source_Field_Or_Child_Enum( bool Use_Source_Field_Or_Child_Enum )
{
   p_impl->impl_Use_Source_Field_Or_Child_Enum( Use_Source_Field_Or_Child_Enum );
}

const string& Meta_Specification_Type::View_Id( ) const
{
   return p_impl->impl_View_Id( );
}

void Meta_Specification_Type::View_Id( const string& View_Id )
{
   p_impl->impl_View_Id( View_Id );
}

Meta_Specification_Type& Meta_Specification_Type::Child_Specification_Type( )
{
   return p_impl->impl_Child_Specification_Type( );
}

const Meta_Specification_Type& Meta_Specification_Type::Child_Specification_Type( ) const
{
   return p_impl->impl_Child_Specification_Type( );
}

void Meta_Specification_Type::Child_Specification_Type( const string& key )
{
   p_impl->impl_Child_Specification_Type( key );
}

Meta_Specification_Type& Meta_Specification_Type::Next_Child_Specification_Type( )
{
   return p_impl->impl_Next_Child_Specification_Type( );
}

const Meta_Specification_Type& Meta_Specification_Type::Next_Child_Specification_Type( ) const
{
   return p_impl->impl_Next_Child_Specification_Type( );
}

void Meta_Specification_Type::Next_Child_Specification_Type( const string& key )
{
   p_impl->impl_Next_Child_Specification_Type( key );
}

Meta_Specification_Type& Meta_Specification_Type::Parent_Specification_Type( )
{
   return p_impl->impl_Parent_Specification_Type( );
}

const Meta_Specification_Type& Meta_Specification_Type::Parent_Specification_Type( ) const
{
   return p_impl->impl_Parent_Specification_Type( );
}

void Meta_Specification_Type::Parent_Specification_Type( const string& key )
{
   p_impl->impl_Parent_Specification_Type( key );
}

Meta_Specification_Type& Meta_Specification_Type::child_Specification_Type_Child( )
{
   return p_impl->impl_child_Specification_Type_Child( );
}

const Meta_Specification_Type& Meta_Specification_Type::child_Specification_Type_Child( ) const
{
   return p_impl->impl_child_Specification_Type_Child( );
}

Meta_Specification& Meta_Specification_Type::child_Specification_Child( )
{
   return p_impl->impl_child_Specification_Child( );
}

const Meta_Specification& Meta_Specification_Type::child_Specification_Child( ) const
{
   return p_impl->impl_child_Specification_Child( );
}

Meta_Specification_Type& Meta_Specification_Type::child_Specification_Type_Next_Child( )
{
   return p_impl->impl_child_Specification_Type_Next_Child( );
}

const Meta_Specification_Type& Meta_Specification_Type::child_Specification_Type_Next_Child( ) const
{
   return p_impl->impl_child_Specification_Type_Next_Child( );
}

Meta_Specification_Type& Meta_Specification_Type::child_Specification_Type_Parent( )
{
   return p_impl->impl_child_Specification_Type_Parent( );
}

const Meta_Specification_Type& Meta_Specification_Type::child_Specification_Type_Parent( ) const
{
   return p_impl->impl_child_Specification_Type_Parent( );
}

Meta_Specification& Meta_Specification_Type::child_Specification( )
{
   return p_impl->impl_child_Specification( );
}

const Meta_Specification& Meta_Specification_Type::child_Specification( ) const
{
   return p_impl->impl_child_Specification( );
}

void Meta_Specification_Type::Is_Valid_Field_Type( const string& Class,
 string& Error, const string& Parent_Class, int Primitive, int Procedure_Arg_Primitive,
 int Procedure_Arg_Type, const string& Source_Parent_Class, int Source_Primitive, int Test_Primitive, const string& Test_Value )
{
   p_impl->impl_Is_Valid_Field_Type( Class, Error, Parent_Class, Primitive, Procedure_Arg_Primitive, Procedure_Arg_Type, Source_Parent_Class, Source_Primitive, Test_Primitive, Test_Value );
}

string Meta_Specification_Type::get_field_value( int field ) const
{
   return p_impl->get_field_value( field );
}

void Meta_Specification_Type::set_field_value( int field, const string& value )
{
   p_impl->set_field_value( field, value );
}

void Meta_Specification_Type::set_field_default( int field )
{
   return set_field_default( ( field_id )( field + 1 ) );
}

void Meta_Specification_Type::set_field_default( field_id id )
{
   p_impl->set_field_default( ( int )id - 1 );
}

void Meta_Specification_Type::set_field_default( const string& field )
{
   p_impl->set_field_default( get_field_num( field ) );
}

bool Meta_Specification_Type::is_field_default( int field ) const
{
   return is_field_default( ( field_id )( field + 1 ) );
}

bool Meta_Specification_Type::is_field_default( field_id id ) const
{
   return p_impl->is_field_default( ( int )id - 1 );
}

bool Meta_Specification_Type::is_field_default( const string& field ) const
{
   return p_impl->is_field_default( get_field_num( field ) );
}

bool Meta_Specification_Type::is_field_encrypted( int field ) const
{
   return static_is_field_encrypted( ( field_id )( field + 1 ) );
}

bool Meta_Specification_Type::is_field_transient( int field ) const
{
   return static_is_field_transient( ( field_id )( field + 1 ) );
}

string Meta_Specification_Type::get_field_id( int field ) const
{
   return static_get_field_id( ( field_id )( field + 1 ) );
}

string Meta_Specification_Type::get_field_name( int field ) const
{
   return static_get_field_name( ( field_id )( field + 1 ) );
}

int Meta_Specification_Type::get_field_num( const string& field ) const
{
   int rc = static_get_field_num( field );

   if( rc < 0 )
      throw runtime_error( "unknown field name/id '" + field + "' in get_field_num( )" );

   return rc;
}

bool Meta_Specification_Type::has_field_changed( const string& field ) const
{
   return class_base::has_field_changed( get_field_num( field ) );
}

uint64_t Meta_Specification_Type::get_state( ) const
{
   uint64_t state = 0;

   state |= p_impl->get_state( );

   return state;
}

const string& Meta_Specification_Type::execute( const string& cmd_and_args )
{
   return p_impl->execute( cmd_and_args );
}

void Meta_Specification_Type::clear( )
{
   p_impl->clear( );
}

void Meta_Specification_Type::validate( uint64_t state, bool is_internal )
{
   p_impl->validate( state, is_internal, &validation_errors );
}

void Meta_Specification_Type::validate_set_fields( set< string >& fields_set )
{
   p_impl->validate_set_fields( fields_set, &validation_errors );
}

void Meta_Specification_Type::after_fetch( )
{
   p_impl->after_fetch( );
}

void Meta_Specification_Type::completed_fetch( bool skip_set_original )
{
   p_impl->completed_fetch( skip_set_original );
}

void Meta_Specification_Type::at_create( )
{
   p_impl->at_create( );
}

void Meta_Specification_Type::post_init( )
{
   p_impl->post_init( );
}

void Meta_Specification_Type::to_store( bool is_create, bool is_internal )
{
   p_impl->to_store( is_create, is_internal );
}

void Meta_Specification_Type::for_store( bool is_create, bool is_internal )
{
   p_impl->for_store( is_create, is_internal );
}

void Meta_Specification_Type::after_store( bool is_create, bool is_internal )
{
   p_impl->after_store( is_create, is_internal );
}

bool Meta_Specification_Type::can_destroy( bool is_internal )
{
   return p_impl->can_destroy( is_internal );
}

void Meta_Specification_Type::for_destroy( bool is_internal )
{
   p_impl->for_destroy( is_internal );
}

void Meta_Specification_Type::after_destroy( bool is_internal )
{
   p_impl->after_destroy( is_internal );
}

void Meta_Specification_Type::set_default_values( )
{
   p_impl->set_default_values( );
}

bool Meta_Specification_Type::is_filtered( ) const
{
   return p_impl->is_filtered( );
}

const char* Meta_Specification_Type::get_field_id(
 const string& name, bool* p_sql_numeric, string* p_type_name ) const
{
   const char* p_id( 0 );

   if( name.empty( ) )
      throw runtime_error( "unexpected empty field name for get_field_id" );
   else if( name == c_field_name_Allow_Child_Relationship )
   {
      p_id = c_field_id_Allow_Child_Relationship;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Allow_Enum )
   {
      p_id = c_field_id_Allow_Enum;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Allow_Enum_Item )
   {
      p_id = c_field_id_Allow_Enum_Item;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Allow_FK_Source_Field_Choice )
   {
      p_id = c_field_id_Allow_FK_Source_Field_Choice;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Allow_FK_Test_Field_Choice )
   {
      p_id = c_field_id_Allow_FK_Test_Field_Choice;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Allow_Field )
   {
      p_id = c_field_id_Allow_Field;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Allow_Modifier )
   {
      p_id = c_field_id_Allow_Modifier;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Allow_Options )
   {
      p_id = c_field_id_Allow_Options;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Allow_Other_Class )
   {
      p_id = c_field_id_Allow_Other_Class;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Allow_Other_Class_Field )
   {
      p_id = c_field_id_Allow_Other_Class_Field;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Allow_Other_Field )
   {
      p_id = c_field_id_Allow_Other_Field;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Allow_Other_Field_2 )
   {
      p_id = c_field_id_Allow_Other_Field_2;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Allow_Other_Modifier )
   {
      p_id = c_field_id_Allow_Other_Modifier;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Allow_Other_Modifier_2 )
   {
      p_id = c_field_id_Allow_Other_Modifier_2;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Allow_Other_Procedure )
   {
      p_id = c_field_id_Allow_Other_Procedure;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Allow_Other_Procedure_2 )
   {
      p_id = c_field_id_Allow_Other_Procedure_2;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Allow_Other_Source_Child )
   {
      p_id = c_field_id_Allow_Other_Source_Child;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Allow_Other_Source_Child_2 )
   {
      p_id = c_field_id_Allow_Other_Source_Child_2;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Allow_Permission )
   {
      p_id = c_field_id_Allow_Permission;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Allow_Procedure )
   {
      p_id = c_field_id_Allow_Procedure;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Allow_Procedure_Arg )
   {
      p_id = c_field_id_Allow_Procedure_Arg;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Allow_Source_Child )
   {
      p_id = c_field_id_Allow_Source_Child;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Allow_Source_Class )
   {
      p_id = c_field_id_Allow_Source_Class;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Allow_Source_Field )
   {
      p_id = c_field_id_Allow_Source_Field;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Allow_Source_Grandchild )
   {
      p_id = c_field_id_Allow_Source_Grandchild;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Allow_Source_Parent )
   {
      p_id = c_field_id_Allow_Source_Parent;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Allow_Test_Field )
   {
      p_id = c_field_id_Allow_Test_Field;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Allow_Test_Value )
   {
      p_id = c_field_id_Allow_Test_Value;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Allow_Value )
   {
      p_id = c_field_id_Allow_Value;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Can_Access_Below_Model )
   {
      p_id = c_field_id_Can_Access_Below_Model;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Child_Prefix )
   {
      p_id = c_field_id_Child_Prefix;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Child_Rel_Grandparent_Match )
   {
      p_id = c_field_id_Child_Rel_Grandparent_Match;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Child_Rel_Parent_Match )
   {
      p_id = c_field_id_Child_Rel_Parent_Match;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Child_Relationship_Class_Match )
   {
      p_id = c_field_id_Child_Relationship_Class_Match;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Child_Specification_Type )
   {
      p_id = c_field_id_Child_Specification_Type;

      if( p_type_name )
         *p_type_name = "Meta_Specification_Type";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Default_Child_Vars )
   {
      p_id = c_field_id_Default_Child_Vars;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Field_type )
   {
      p_id = c_field_id_Field_type;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Has_Next_Specification_Info )
   {
      p_id = c_field_id_Has_Next_Specification_Info;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Is_Child_Only )
   {
      p_id = c_field_id_Is_Child_Only;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Is_Required_For_UI_Gen )
   {
      p_id = c_field_id_Is_Required_For_UI_Gen;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Is_System )
   {
      p_id = c_field_id_Is_System;

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
   else if( name == c_field_name_Needs_Child_Relationship )
   {
      p_id = c_field_id_Needs_Child_Relationship;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Needs_Class )
   {
      p_id = c_field_id_Needs_Class;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Needs_Enum )
   {
      p_id = c_field_id_Needs_Enum;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Needs_Enum_Item )
   {
      p_id = c_field_id_Needs_Enum_Item;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Needs_Enum_Item_2 )
   {
      p_id = c_field_id_Needs_Enum_Item_2;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Needs_Enum_Item_3 )
   {
      p_id = c_field_id_Needs_Enum_Item_3;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Needs_Enum_Item_4 )
   {
      p_id = c_field_id_Needs_Enum_Item_4;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Needs_Enum_Item_5 )
   {
      p_id = c_field_id_Needs_Enum_Item_5;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Needs_Field )
   {
      p_id = c_field_id_Needs_Field;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Needs_Modifier )
   {
      p_id = c_field_id_Needs_Modifier;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Needs_Other_Class )
   {
      p_id = c_field_id_Needs_Other_Class;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Needs_Other_Class_Field )
   {
      p_id = c_field_id_Needs_Other_Class_Field;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Needs_Other_Field )
   {
      p_id = c_field_id_Needs_Other_Field;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Needs_Other_Field_2 )
   {
      p_id = c_field_id_Needs_Other_Field_2;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Needs_Other_Modifier )
   {
      p_id = c_field_id_Needs_Other_Modifier;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Needs_Other_Modifier_2 )
   {
      p_id = c_field_id_Needs_Other_Modifier_2;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Needs_Other_Permission )
   {
      p_id = c_field_id_Needs_Other_Permission;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Needs_Other_Permission_2 )
   {
      p_id = c_field_id_Needs_Other_Permission_2;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Needs_Other_Procedure )
   {
      p_id = c_field_id_Needs_Other_Procedure;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Needs_Other_Procedure_2 )
   {
      p_id = c_field_id_Needs_Other_Procedure_2;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Needs_Other_Source_Child )
   {
      p_id = c_field_id_Needs_Other_Source_Child;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Needs_Other_Source_Child_2 )
   {
      p_id = c_field_id_Needs_Other_Source_Child_2;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Needs_Permission )
   {
      p_id = c_field_id_Needs_Permission;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Needs_Procedure )
   {
      p_id = c_field_id_Needs_Procedure;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Needs_Procedure_Arg )
   {
      p_id = c_field_id_Needs_Procedure_Arg;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Needs_Procedure_Arg_2 )
   {
      p_id = c_field_id_Needs_Procedure_Arg_2;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Needs_Procedure_Arg_3 )
   {
      p_id = c_field_id_Needs_Procedure_Arg_3;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Needs_Source_Child )
   {
      p_id = c_field_id_Needs_Source_Child;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Needs_Source_Field )
   {
      p_id = c_field_id_Needs_Source_Field;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Needs_Source_Field_Or_Child )
   {
      p_id = c_field_id_Needs_Source_Field_Or_Child;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Needs_Source_Parent )
   {
      p_id = c_field_id_Needs_Source_Parent;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Needs_Test_Field )
   {
      p_id = c_field_id_Needs_Test_Field;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Needs_Test_Value )
   {
      p_id = c_field_id_Needs_Test_Value;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Needs_Value )
   {
      p_id = c_field_id_Needs_Value;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Next_Child_Specification_Type )
   {
      p_id = c_field_id_Next_Child_Specification_Type;

      if( p_type_name )
         *p_type_name = "Meta_Specification_Type";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Next_Protect_Child_Rel )
   {
      p_id = c_field_id_Next_Protect_Child_Rel;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Next_Protect_Procedure )
   {
      p_id = c_field_id_Next_Protect_Procedure;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Next_Protect_Source_Parent )
   {
      p_id = c_field_id_Next_Protect_Source_Parent;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Next_Specification_Actions )
   {
      p_id = c_field_id_Next_Specification_Actions;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Notes )
   {
      p_id = c_field_id_Notes;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Parent_Specification_Type )
   {
      p_id = c_field_id_Parent_Specification_Type;

      if( p_type_name )
         *p_type_name = "Meta_Specification_Type";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Procedure_Arg_primitive )
   {
      p_id = c_field_id_Procedure_Arg_primitive;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Procedure_Arg_type )
   {
      p_id = c_field_id_Procedure_Arg_type;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Protect_Child_Rel_From_Update )
   {
      p_id = c_field_id_Protect_Child_Rel_From_Update;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Protect_Class_From_Edit )
   {
      p_id = c_field_id_Protect_Class_From_Edit;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Protect_Class_From_Update )
   {
      p_id = c_field_id_Protect_Class_From_Update;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Protect_Other_Class_From_Edit )
   {
      p_id = c_field_id_Protect_Other_Class_From_Edit;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Protect_Procedure_From_Edit )
   {
      p_id = c_field_id_Protect_Procedure_From_Edit;

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
   else if( name == c_field_name_Set_Field_To_Source_Field_Or_Child )
   {
      p_id = c_field_id_Set_Field_To_Source_Field_Or_Child;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Source_Field_Needs_Test_Field )
   {
      p_id = c_field_id_Source_Field_Needs_Test_Field;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Source_Field_Type_Match )
   {
      p_id = c_field_id_Source_Field_Type_Match;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Source_Parent_type )
   {
      p_id = c_field_id_Source_Parent_type;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Source_type )
   {
      p_id = c_field_id_Source_type;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Specification_Actions )
   {
      p_id = c_field_id_Specification_Actions;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Specification_Name )
   {
      p_id = c_field_id_Specification_Name;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Specification_Object )
   {
      p_id = c_field_id_Specification_Object;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Specification_Strings )
   {
      p_id = c_field_id_Specification_Strings;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   if( name == c_field_name_Specification_Vars )
   {
      p_id = c_field_id_Specification_Vars;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Test_Field_Type_Match )
   {
      p_id = c_field_id_Test_Field_Type_Match;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Test_Field_type )
   {
      p_id = c_field_id_Test_Field_type;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Use_Class_As_Source_Parent_Class )
   {
      p_id = c_field_id_Use_Class_As_Source_Parent_Class;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Use_Field_Enum )
   {
      p_id = c_field_id_Use_Field_Enum;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Use_Parent_Child_Rel_As_Source_Parent )
   {
      p_id = c_field_id_Use_Parent_Child_Rel_As_Source_Parent;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Use_Parent_Child_Rel_For_Class )
   {
      p_id = c_field_id_Use_Parent_Child_Rel_For_Class;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Use_Parent_Child_Rel_For_Other_Class )
   {
      p_id = c_field_id_Use_Parent_Child_Rel_For_Other_Class;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Use_Parent_Class )
   {
      p_id = c_field_id_Use_Parent_Class;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Use_Parent_Field_For_Class )
   {
      p_id = c_field_id_Use_Parent_Field_For_Class;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Use_Parent_Other_Class )
   {
      p_id = c_field_id_Use_Parent_Other_Class;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Use_Parent_Other_Class_For_Class )
   {
      p_id = c_field_id_Use_Parent_Other_Class_For_Class;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Use_Parent_Procedure )
   {
      p_id = c_field_id_Use_Parent_Procedure;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Use_Parent_Source_Class )
   {
      p_id = c_field_id_Use_Parent_Source_Class;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Use_Parent_Source_Field_For_Class )
   {
      p_id = c_field_id_Use_Parent_Source_Field_For_Class;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Use_Parent_Source_Parent )
   {
      p_id = c_field_id_Use_Parent_Source_Parent;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Use_Parent_Source_Parent_For_Class )
   {
      p_id = c_field_id_Use_Parent_Source_Parent_For_Class;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Use_Source_Field_Enum )
   {
      p_id = c_field_id_Use_Source_Field_Enum;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_Use_Source_Field_Or_Child_Enum )
   {
      p_id = c_field_id_Use_Source_Field_Or_Child_Enum;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( name == c_field_name_View_Id )
   {
      p_id = c_field_id_View_Id;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }

   return p_id;
}

const char* Meta_Specification_Type::get_field_name(
 const string& id, bool* p_sql_numeric, string* p_type_name ) const
{
   const char* p_name( 0 );

   if( id.empty( ) )
      throw runtime_error( "unexpected empty field id for get_field_name" );
   else if( id == c_field_id_Allow_Child_Relationship )
   {
      p_name = c_field_name_Allow_Child_Relationship;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Allow_Enum )
   {
      p_name = c_field_name_Allow_Enum;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Allow_Enum_Item )
   {
      p_name = c_field_name_Allow_Enum_Item;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Allow_FK_Source_Field_Choice )
   {
      p_name = c_field_name_Allow_FK_Source_Field_Choice;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Allow_FK_Test_Field_Choice )
   {
      p_name = c_field_name_Allow_FK_Test_Field_Choice;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Allow_Field )
   {
      p_name = c_field_name_Allow_Field;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Allow_Modifier )
   {
      p_name = c_field_name_Allow_Modifier;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Allow_Options )
   {
      p_name = c_field_name_Allow_Options;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Allow_Other_Class )
   {
      p_name = c_field_name_Allow_Other_Class;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Allow_Other_Class_Field )
   {
      p_name = c_field_name_Allow_Other_Class_Field;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Allow_Other_Field )
   {
      p_name = c_field_name_Allow_Other_Field;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Allow_Other_Field_2 )
   {
      p_name = c_field_name_Allow_Other_Field_2;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Allow_Other_Modifier )
   {
      p_name = c_field_name_Allow_Other_Modifier;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Allow_Other_Modifier_2 )
   {
      p_name = c_field_name_Allow_Other_Modifier_2;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Allow_Other_Procedure )
   {
      p_name = c_field_name_Allow_Other_Procedure;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Allow_Other_Procedure_2 )
   {
      p_name = c_field_name_Allow_Other_Procedure_2;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Allow_Other_Source_Child )
   {
      p_name = c_field_name_Allow_Other_Source_Child;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Allow_Other_Source_Child_2 )
   {
      p_name = c_field_name_Allow_Other_Source_Child_2;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Allow_Permission )
   {
      p_name = c_field_name_Allow_Permission;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Allow_Procedure )
   {
      p_name = c_field_name_Allow_Procedure;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Allow_Procedure_Arg )
   {
      p_name = c_field_name_Allow_Procedure_Arg;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Allow_Source_Child )
   {
      p_name = c_field_name_Allow_Source_Child;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Allow_Source_Class )
   {
      p_name = c_field_name_Allow_Source_Class;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Allow_Source_Field )
   {
      p_name = c_field_name_Allow_Source_Field;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Allow_Source_Grandchild )
   {
      p_name = c_field_name_Allow_Source_Grandchild;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Allow_Source_Parent )
   {
      p_name = c_field_name_Allow_Source_Parent;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Allow_Test_Field )
   {
      p_name = c_field_name_Allow_Test_Field;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Allow_Test_Value )
   {
      p_name = c_field_name_Allow_Test_Value;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Allow_Value )
   {
      p_name = c_field_name_Allow_Value;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Can_Access_Below_Model )
   {
      p_name = c_field_name_Can_Access_Below_Model;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Child_Prefix )
   {
      p_name = c_field_name_Child_Prefix;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Child_Rel_Grandparent_Match )
   {
      p_name = c_field_name_Child_Rel_Grandparent_Match;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Child_Rel_Parent_Match )
   {
      p_name = c_field_name_Child_Rel_Parent_Match;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Child_Relationship_Class_Match )
   {
      p_name = c_field_name_Child_Relationship_Class_Match;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Child_Specification_Type )
   {
      p_name = c_field_name_Child_Specification_Type;

      if( p_type_name )
         *p_type_name = "Meta_Specification_Type";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Default_Child_Vars )
   {
      p_name = c_field_name_Default_Child_Vars;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Field_type )
   {
      p_name = c_field_name_Field_type;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Has_Next_Specification_Info )
   {
      p_name = c_field_name_Has_Next_Specification_Info;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Is_Child_Only )
   {
      p_name = c_field_name_Is_Child_Only;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Is_Required_For_UI_Gen )
   {
      p_name = c_field_name_Is_Required_For_UI_Gen;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Is_System )
   {
      p_name = c_field_name_Is_System;

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
   else if( id == c_field_id_Needs_Child_Relationship )
   {
      p_name = c_field_name_Needs_Child_Relationship;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Needs_Class )
   {
      p_name = c_field_name_Needs_Class;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Needs_Enum )
   {
      p_name = c_field_name_Needs_Enum;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Needs_Enum_Item )
   {
      p_name = c_field_name_Needs_Enum_Item;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Needs_Enum_Item_2 )
   {
      p_name = c_field_name_Needs_Enum_Item_2;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Needs_Enum_Item_3 )
   {
      p_name = c_field_name_Needs_Enum_Item_3;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Needs_Enum_Item_4 )
   {
      p_name = c_field_name_Needs_Enum_Item_4;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Needs_Enum_Item_5 )
   {
      p_name = c_field_name_Needs_Enum_Item_5;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Needs_Field )
   {
      p_name = c_field_name_Needs_Field;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Needs_Modifier )
   {
      p_name = c_field_name_Needs_Modifier;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Needs_Other_Class )
   {
      p_name = c_field_name_Needs_Other_Class;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Needs_Other_Class_Field )
   {
      p_name = c_field_name_Needs_Other_Class_Field;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Needs_Other_Field )
   {
      p_name = c_field_name_Needs_Other_Field;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Needs_Other_Field_2 )
   {
      p_name = c_field_name_Needs_Other_Field_2;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Needs_Other_Modifier )
   {
      p_name = c_field_name_Needs_Other_Modifier;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Needs_Other_Modifier_2 )
   {
      p_name = c_field_name_Needs_Other_Modifier_2;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Needs_Other_Permission )
   {
      p_name = c_field_name_Needs_Other_Permission;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Needs_Other_Permission_2 )
   {
      p_name = c_field_name_Needs_Other_Permission_2;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Needs_Other_Procedure )
   {
      p_name = c_field_name_Needs_Other_Procedure;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Needs_Other_Procedure_2 )
   {
      p_name = c_field_name_Needs_Other_Procedure_2;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Needs_Other_Source_Child )
   {
      p_name = c_field_name_Needs_Other_Source_Child;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Needs_Other_Source_Child_2 )
   {
      p_name = c_field_name_Needs_Other_Source_Child_2;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Needs_Permission )
   {
      p_name = c_field_name_Needs_Permission;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Needs_Procedure )
   {
      p_name = c_field_name_Needs_Procedure;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Needs_Procedure_Arg )
   {
      p_name = c_field_name_Needs_Procedure_Arg;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Needs_Procedure_Arg_2 )
   {
      p_name = c_field_name_Needs_Procedure_Arg_2;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Needs_Procedure_Arg_3 )
   {
      p_name = c_field_name_Needs_Procedure_Arg_3;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Needs_Source_Child )
   {
      p_name = c_field_name_Needs_Source_Child;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Needs_Source_Field )
   {
      p_name = c_field_name_Needs_Source_Field;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Needs_Source_Field_Or_Child )
   {
      p_name = c_field_name_Needs_Source_Field_Or_Child;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Needs_Source_Parent )
   {
      p_name = c_field_name_Needs_Source_Parent;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Needs_Test_Field )
   {
      p_name = c_field_name_Needs_Test_Field;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Needs_Test_Value )
   {
      p_name = c_field_name_Needs_Test_Value;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Needs_Value )
   {
      p_name = c_field_name_Needs_Value;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Next_Child_Specification_Type )
   {
      p_name = c_field_name_Next_Child_Specification_Type;

      if( p_type_name )
         *p_type_name = "Meta_Specification_Type";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Next_Protect_Child_Rel )
   {
      p_name = c_field_name_Next_Protect_Child_Rel;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Next_Protect_Procedure )
   {
      p_name = c_field_name_Next_Protect_Procedure;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Next_Protect_Source_Parent )
   {
      p_name = c_field_name_Next_Protect_Source_Parent;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Next_Specification_Actions )
   {
      p_name = c_field_name_Next_Specification_Actions;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Notes )
   {
      p_name = c_field_name_Notes;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Parent_Specification_Type )
   {
      p_name = c_field_name_Parent_Specification_Type;

      if( p_type_name )
         *p_type_name = "Meta_Specification_Type";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Procedure_Arg_primitive )
   {
      p_name = c_field_name_Procedure_Arg_primitive;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Procedure_Arg_type )
   {
      p_name = c_field_name_Procedure_Arg_type;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Protect_Child_Rel_From_Update )
   {
      p_name = c_field_name_Protect_Child_Rel_From_Update;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Protect_Class_From_Edit )
   {
      p_name = c_field_name_Protect_Class_From_Edit;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Protect_Class_From_Update )
   {
      p_name = c_field_name_Protect_Class_From_Update;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Protect_Other_Class_From_Edit )
   {
      p_name = c_field_name_Protect_Other_Class_From_Edit;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Protect_Procedure_From_Edit )
   {
      p_name = c_field_name_Protect_Procedure_From_Edit;

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
   else if( id == c_field_id_Set_Field_To_Source_Field_Or_Child )
   {
      p_name = c_field_name_Set_Field_To_Source_Field_Or_Child;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Source_Field_Needs_Test_Field )
   {
      p_name = c_field_name_Source_Field_Needs_Test_Field;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Source_Field_Type_Match )
   {
      p_name = c_field_name_Source_Field_Type_Match;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Source_Parent_type )
   {
      p_name = c_field_name_Source_Parent_type;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Source_type )
   {
      p_name = c_field_name_Source_type;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Specification_Actions )
   {
      p_name = c_field_name_Specification_Actions;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Specification_Name )
   {
      p_name = c_field_name_Specification_Name;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Specification_Object )
   {
      p_name = c_field_name_Specification_Object;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Specification_Strings )
   {
      p_name = c_field_name_Specification_Strings;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   if( id == c_field_id_Specification_Vars )
   {
      p_name = c_field_name_Specification_Vars;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Test_Field_Type_Match )
   {
      p_name = c_field_name_Test_Field_Type_Match;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Test_Field_type )
   {
      p_name = c_field_name_Test_Field_type;

      if( p_type_name )
         *p_type_name = "int";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Use_Class_As_Source_Parent_Class )
   {
      p_name = c_field_name_Use_Class_As_Source_Parent_Class;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Use_Field_Enum )
   {
      p_name = c_field_name_Use_Field_Enum;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Use_Parent_Child_Rel_As_Source_Parent )
   {
      p_name = c_field_name_Use_Parent_Child_Rel_As_Source_Parent;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Use_Parent_Child_Rel_For_Class )
   {
      p_name = c_field_name_Use_Parent_Child_Rel_For_Class;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Use_Parent_Child_Rel_For_Other_Class )
   {
      p_name = c_field_name_Use_Parent_Child_Rel_For_Other_Class;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Use_Parent_Class )
   {
      p_name = c_field_name_Use_Parent_Class;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Use_Parent_Field_For_Class )
   {
      p_name = c_field_name_Use_Parent_Field_For_Class;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Use_Parent_Other_Class )
   {
      p_name = c_field_name_Use_Parent_Other_Class;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Use_Parent_Other_Class_For_Class )
   {
      p_name = c_field_name_Use_Parent_Other_Class_For_Class;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Use_Parent_Procedure )
   {
      p_name = c_field_name_Use_Parent_Procedure;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Use_Parent_Source_Class )
   {
      p_name = c_field_name_Use_Parent_Source_Class;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Use_Parent_Source_Field_For_Class )
   {
      p_name = c_field_name_Use_Parent_Source_Field_For_Class;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Use_Parent_Source_Parent )
   {
      p_name = c_field_name_Use_Parent_Source_Parent;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Use_Parent_Source_Parent_For_Class )
   {
      p_name = c_field_name_Use_Parent_Source_Parent_For_Class;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Use_Source_Field_Enum )
   {
      p_name = c_field_name_Use_Source_Field_Enum;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_Use_Source_Field_Or_Child_Enum )
   {
      p_name = c_field_name_Use_Source_Field_Or_Child_Enum;

      if( p_type_name )
         *p_type_name = "bool";

      if( p_sql_numeric )
         *p_sql_numeric = true;
   }
   else if( id == c_field_id_View_Id )
   {
      p_name = c_field_name_View_Id;

      if( p_type_name )
         *p_type_name = "string";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }

   return p_name;
}

string& Meta_Specification_Type::get_group_field_name( ) const
{
   return g_group_field_name;
}

string& Meta_Specification_Type::get_level_field_name( ) const
{
   return g_level_field_name;
}

string& Meta_Specification_Type::get_order_field_name( ) const
{
   return g_order_field_name;
}

string& Meta_Specification_Type::get_owner_field_name( ) const
{
   return g_owner_field_name;
}

bool Meta_Specification_Type::is_file_field( const string& id_or_name ) const
{
   return g_file_field_ids.count( id_or_name ) || g_file_field_names.count( id_or_name );
}

void Meta_Specification_Type::get_file_field_names( vector< string >& file_field_names ) const
{
   for( set< string >::const_iterator ci = g_file_field_names.begin( ); ci != g_file_field_names.end( ); ++ci )
      file_field_names.push_back( *ci );
}

string Meta_Specification_Type::get_field_uom_symbol( const string& id_or_name ) const
{
   string uom_symbol;

   string name;
   pair< string, string > next;

   if( id_or_name.empty( ) )
      throw runtime_error( "unexpected empty field id_or_name for get_field_uom_symbol" );
   if( ( id_or_name == c_field_id_Allow_Child_Relationship ) || ( id_or_name == c_field_name_Allow_Child_Relationship ) )
   {
      name = string( c_field_display_name_Allow_Child_Relationship );
      get_module_string( c_field_display_name_Allow_Child_Relationship, &next );
   }
   if( ( id_or_name == c_field_id_Allow_Enum ) || ( id_or_name == c_field_name_Allow_Enum ) )
   {
      name = string( c_field_display_name_Allow_Enum );
      get_module_string( c_field_display_name_Allow_Enum, &next );
   }
   if( ( id_or_name == c_field_id_Allow_Enum_Item ) || ( id_or_name == c_field_name_Allow_Enum_Item ) )
   {
      name = string( c_field_display_name_Allow_Enum_Item );
      get_module_string( c_field_display_name_Allow_Enum_Item, &next );
   }
   if( ( id_or_name == c_field_id_Allow_FK_Source_Field_Choice ) || ( id_or_name == c_field_name_Allow_FK_Source_Field_Choice ) )
   {
      name = string( c_field_display_name_Allow_FK_Source_Field_Choice );
      get_module_string( c_field_display_name_Allow_FK_Source_Field_Choice, &next );
   }
   if( ( id_or_name == c_field_id_Allow_FK_Test_Field_Choice ) || ( id_or_name == c_field_name_Allow_FK_Test_Field_Choice ) )
   {
      name = string( c_field_display_name_Allow_FK_Test_Field_Choice );
      get_module_string( c_field_display_name_Allow_FK_Test_Field_Choice, &next );
   }
   if( ( id_or_name == c_field_id_Allow_Field ) || ( id_or_name == c_field_name_Allow_Field ) )
   {
      name = string( c_field_display_name_Allow_Field );
      get_module_string( c_field_display_name_Allow_Field, &next );
   }
   if( ( id_or_name == c_field_id_Allow_Modifier ) || ( id_or_name == c_field_name_Allow_Modifier ) )
   {
      name = string( c_field_display_name_Allow_Modifier );
      get_module_string( c_field_display_name_Allow_Modifier, &next );
   }
   if( ( id_or_name == c_field_id_Allow_Options ) || ( id_or_name == c_field_name_Allow_Options ) )
   {
      name = string( c_field_display_name_Allow_Options );
      get_module_string( c_field_display_name_Allow_Options, &next );
   }
   if( ( id_or_name == c_field_id_Allow_Other_Class ) || ( id_or_name == c_field_name_Allow_Other_Class ) )
   {
      name = string( c_field_display_name_Allow_Other_Class );
      get_module_string( c_field_display_name_Allow_Other_Class, &next );
   }
   if( ( id_or_name == c_field_id_Allow_Other_Class_Field ) || ( id_or_name == c_field_name_Allow_Other_Class_Field ) )
   {
      name = string( c_field_display_name_Allow_Other_Class_Field );
      get_module_string( c_field_display_name_Allow_Other_Class_Field, &next );
   }
   if( ( id_or_name == c_field_id_Allow_Other_Field ) || ( id_or_name == c_field_name_Allow_Other_Field ) )
   {
      name = string( c_field_display_name_Allow_Other_Field );
      get_module_string( c_field_display_name_Allow_Other_Field, &next );
   }
   if( ( id_or_name == c_field_id_Allow_Other_Field_2 ) || ( id_or_name == c_field_name_Allow_Other_Field_2 ) )
   {
      name = string( c_field_display_name_Allow_Other_Field_2 );
      get_module_string( c_field_display_name_Allow_Other_Field_2, &next );
   }
   if( ( id_or_name == c_field_id_Allow_Other_Modifier ) || ( id_or_name == c_field_name_Allow_Other_Modifier ) )
   {
      name = string( c_field_display_name_Allow_Other_Modifier );
      get_module_string( c_field_display_name_Allow_Other_Modifier, &next );
   }
   if( ( id_or_name == c_field_id_Allow_Other_Modifier_2 ) || ( id_or_name == c_field_name_Allow_Other_Modifier_2 ) )
   {
      name = string( c_field_display_name_Allow_Other_Modifier_2 );
      get_module_string( c_field_display_name_Allow_Other_Modifier_2, &next );
   }
   if( ( id_or_name == c_field_id_Allow_Other_Procedure ) || ( id_or_name == c_field_name_Allow_Other_Procedure ) )
   {
      name = string( c_field_display_name_Allow_Other_Procedure );
      get_module_string( c_field_display_name_Allow_Other_Procedure, &next );
   }
   if( ( id_or_name == c_field_id_Allow_Other_Procedure_2 ) || ( id_or_name == c_field_name_Allow_Other_Procedure_2 ) )
   {
      name = string( c_field_display_name_Allow_Other_Procedure_2 );
      get_module_string( c_field_display_name_Allow_Other_Procedure_2, &next );
   }
   if( ( id_or_name == c_field_id_Allow_Other_Source_Child ) || ( id_or_name == c_field_name_Allow_Other_Source_Child ) )
   {
      name = string( c_field_display_name_Allow_Other_Source_Child );
      get_module_string( c_field_display_name_Allow_Other_Source_Child, &next );
   }
   if( ( id_or_name == c_field_id_Allow_Other_Source_Child_2 ) || ( id_or_name == c_field_name_Allow_Other_Source_Child_2 ) )
   {
      name = string( c_field_display_name_Allow_Other_Source_Child_2 );
      get_module_string( c_field_display_name_Allow_Other_Source_Child_2, &next );
   }
   if( ( id_or_name == c_field_id_Allow_Permission ) || ( id_or_name == c_field_name_Allow_Permission ) )
   {
      name = string( c_field_display_name_Allow_Permission );
      get_module_string( c_field_display_name_Allow_Permission, &next );
   }
   if( ( id_or_name == c_field_id_Allow_Procedure ) || ( id_or_name == c_field_name_Allow_Procedure ) )
   {
      name = string( c_field_display_name_Allow_Procedure );
      get_module_string( c_field_display_name_Allow_Procedure, &next );
   }
   if( ( id_or_name == c_field_id_Allow_Procedure_Arg ) || ( id_or_name == c_field_name_Allow_Procedure_Arg ) )
   {
      name = string( c_field_display_name_Allow_Procedure_Arg );
      get_module_string( c_field_display_name_Allow_Procedure_Arg, &next );
   }
   if( ( id_or_name == c_field_id_Allow_Source_Child ) || ( id_or_name == c_field_name_Allow_Source_Child ) )
   {
      name = string( c_field_display_name_Allow_Source_Child );
      get_module_string( c_field_display_name_Allow_Source_Child, &next );
   }
   if( ( id_or_name == c_field_id_Allow_Source_Class ) || ( id_or_name == c_field_name_Allow_Source_Class ) )
   {
      name = string( c_field_display_name_Allow_Source_Class );
      get_module_string( c_field_display_name_Allow_Source_Class, &next );
   }
   if( ( id_or_name == c_field_id_Allow_Source_Field ) || ( id_or_name == c_field_name_Allow_Source_Field ) )
   {
      name = string( c_field_display_name_Allow_Source_Field );
      get_module_string( c_field_display_name_Allow_Source_Field, &next );
   }
   if( ( id_or_name == c_field_id_Allow_Source_Grandchild ) || ( id_or_name == c_field_name_Allow_Source_Grandchild ) )
   {
      name = string( c_field_display_name_Allow_Source_Grandchild );
      get_module_string( c_field_display_name_Allow_Source_Grandchild, &next );
   }
   if( ( id_or_name == c_field_id_Allow_Source_Parent ) || ( id_or_name == c_field_name_Allow_Source_Parent ) )
   {
      name = string( c_field_display_name_Allow_Source_Parent );
      get_module_string( c_field_display_name_Allow_Source_Parent, &next );
   }
   if( ( id_or_name == c_field_id_Allow_Test_Field ) || ( id_or_name == c_field_name_Allow_Test_Field ) )
   {
      name = string( c_field_display_name_Allow_Test_Field );
      get_module_string( c_field_display_name_Allow_Test_Field, &next );
   }
   if( ( id_or_name == c_field_id_Allow_Test_Value ) || ( id_or_name == c_field_name_Allow_Test_Value ) )
   {
      name = string( c_field_display_name_Allow_Test_Value );
      get_module_string( c_field_display_name_Allow_Test_Value, &next );
   }
   if( ( id_or_name == c_field_id_Allow_Value ) || ( id_or_name == c_field_name_Allow_Value ) )
   {
      name = string( c_field_display_name_Allow_Value );
      get_module_string( c_field_display_name_Allow_Value, &next );
   }
   if( ( id_or_name == c_field_id_Can_Access_Below_Model ) || ( id_or_name == c_field_name_Can_Access_Below_Model ) )
   {
      name = string( c_field_display_name_Can_Access_Below_Model );
      get_module_string( c_field_display_name_Can_Access_Below_Model, &next );
   }
   if( ( id_or_name == c_field_id_Child_Prefix ) || ( id_or_name == c_field_name_Child_Prefix ) )
   {
      name = string( c_field_display_name_Child_Prefix );
      get_module_string( c_field_display_name_Child_Prefix, &next );
   }
   if( ( id_or_name == c_field_id_Child_Rel_Grandparent_Match ) || ( id_or_name == c_field_name_Child_Rel_Grandparent_Match ) )
   {
      name = string( c_field_display_name_Child_Rel_Grandparent_Match );
      get_module_string( c_field_display_name_Child_Rel_Grandparent_Match, &next );
   }
   if( ( id_or_name == c_field_id_Child_Rel_Parent_Match ) || ( id_or_name == c_field_name_Child_Rel_Parent_Match ) )
   {
      name = string( c_field_display_name_Child_Rel_Parent_Match );
      get_module_string( c_field_display_name_Child_Rel_Parent_Match, &next );
   }
   if( ( id_or_name == c_field_id_Child_Relationship_Class_Match ) || ( id_or_name == c_field_name_Child_Relationship_Class_Match ) )
   {
      name = string( c_field_display_name_Child_Relationship_Class_Match );
      get_module_string( c_field_display_name_Child_Relationship_Class_Match, &next );
   }
   if( ( id_or_name == c_field_id_Child_Specification_Type ) || ( id_or_name == c_field_name_Child_Specification_Type ) )
   {
      name = string( c_field_display_name_Child_Specification_Type );
      get_module_string( c_field_display_name_Child_Specification_Type, &next );
   }
   if( ( id_or_name == c_field_id_Default_Child_Vars ) || ( id_or_name == c_field_name_Default_Child_Vars ) )
   {
      name = string( c_field_display_name_Default_Child_Vars );
      get_module_string( c_field_display_name_Default_Child_Vars, &next );
   }
   if( ( id_or_name == c_field_id_Field_type ) || ( id_or_name == c_field_name_Field_type ) )
   {
      name = string( c_field_display_name_Field_type );
      get_module_string( c_field_display_name_Field_type, &next );
   }
   if( ( id_or_name == c_field_id_Has_Next_Specification_Info ) || ( id_or_name == c_field_name_Has_Next_Specification_Info ) )
   {
      name = string( c_field_display_name_Has_Next_Specification_Info );
      get_module_string( c_field_display_name_Has_Next_Specification_Info, &next );
   }
   if( ( id_or_name == c_field_id_Is_Child_Only ) || ( id_or_name == c_field_name_Is_Child_Only ) )
   {
      name = string( c_field_display_name_Is_Child_Only );
      get_module_string( c_field_display_name_Is_Child_Only, &next );
   }
   if( ( id_or_name == c_field_id_Is_Required_For_UI_Gen ) || ( id_or_name == c_field_name_Is_Required_For_UI_Gen ) )
   {
      name = string( c_field_display_name_Is_Required_For_UI_Gen );
      get_module_string( c_field_display_name_Is_Required_For_UI_Gen, &next );
   }
   if( ( id_or_name == c_field_id_Is_System ) || ( id_or_name == c_field_name_Is_System ) )
   {
      name = string( c_field_display_name_Is_System );
      get_module_string( c_field_display_name_Is_System, &next );
   }
   if( ( id_or_name == c_field_id_Name ) || ( id_or_name == c_field_name_Name ) )
   {
      name = string( c_field_display_name_Name );
      get_module_string( c_field_display_name_Name, &next );
   }
   if( ( id_or_name == c_field_id_Needs_Child_Relationship ) || ( id_or_name == c_field_name_Needs_Child_Relationship ) )
   {
      name = string( c_field_display_name_Needs_Child_Relationship );
      get_module_string( c_field_display_name_Needs_Child_Relationship, &next );
   }
   if( ( id_or_name == c_field_id_Needs_Class ) || ( id_or_name == c_field_name_Needs_Class ) )
   {
      name = string( c_field_display_name_Needs_Class );
      get_module_string( c_field_display_name_Needs_Class, &next );
   }
   if( ( id_or_name == c_field_id_Needs_Enum ) || ( id_or_name == c_field_name_Needs_Enum ) )
   {
      name = string( c_field_display_name_Needs_Enum );
      get_module_string( c_field_display_name_Needs_Enum, &next );
   }
   if( ( id_or_name == c_field_id_Needs_Enum_Item ) || ( id_or_name == c_field_name_Needs_Enum_Item ) )
   {
      name = string( c_field_display_name_Needs_Enum_Item );
      get_module_string( c_field_display_name_Needs_Enum_Item, &next );
   }
   if( ( id_or_name == c_field_id_Needs_Enum_Item_2 ) || ( id_or_name == c_field_name_Needs_Enum_Item_2 ) )
   {
      name = string( c_field_display_name_Needs_Enum_Item_2 );
      get_module_string( c_field_display_name_Needs_Enum_Item_2, &next );
   }
   if( ( id_or_name == c_field_id_Needs_Enum_Item_3 ) || ( id_or_name == c_field_name_Needs_Enum_Item_3 ) )
   {
      name = string( c_field_display_name_Needs_Enum_Item_3 );
      get_module_string( c_field_display_name_Needs_Enum_Item_3, &next );
   }
   if( ( id_or_name == c_field_id_Needs_Enum_Item_4 ) || ( id_or_name == c_field_name_Needs_Enum_Item_4 ) )
   {
      name = string( c_field_display_name_Needs_Enum_Item_4 );
      get_module_string( c_field_display_name_Needs_Enum_Item_4, &next );
   }
   if( ( id_or_name == c_field_id_Needs_Enum_Item_5 ) || ( id_or_name == c_field_name_Needs_Enum_Item_5 ) )
   {
      name = string( c_field_display_name_Needs_Enum_Item_5 );
      get_module_string( c_field_display_name_Needs_Enum_Item_5, &next );
   }
   if( ( id_or_name == c_field_id_Needs_Field ) || ( id_or_name == c_field_name_Needs_Field ) )
   {
      name = string( c_field_display_name_Needs_Field );
      get_module_string( c_field_display_name_Needs_Field, &next );
   }
   if( ( id_or_name == c_field_id_Needs_Modifier ) || ( id_or_name == c_field_name_Needs_Modifier ) )
   {
      name = string( c_field_display_name_Needs_Modifier );
      get_module_string( c_field_display_name_Needs_Modifier, &next );
   }
   if( ( id_or_name == c_field_id_Needs_Other_Class ) || ( id_or_name == c_field_name_Needs_Other_Class ) )
   {
      name = string( c_field_display_name_Needs_Other_Class );
      get_module_string( c_field_display_name_Needs_Other_Class, &next );
   }
   if( ( id_or_name == c_field_id_Needs_Other_Class_Field ) || ( id_or_name == c_field_name_Needs_Other_Class_Field ) )
   {
      name = string( c_field_display_name_Needs_Other_Class_Field );
      get_module_string( c_field_display_name_Needs_Other_Class_Field, &next );
   }
   if( ( id_or_name == c_field_id_Needs_Other_Field ) || ( id_or_name == c_field_name_Needs_Other_Field ) )
   {
      name = string( c_field_display_name_Needs_Other_Field );
      get_module_string( c_field_display_name_Needs_Other_Field, &next );
   }
   if( ( id_or_name == c_field_id_Needs_Other_Field_2 ) || ( id_or_name == c_field_name_Needs_Other_Field_2 ) )
   {
      name = string( c_field_display_name_Needs_Other_Field_2 );
      get_module_string( c_field_display_name_Needs_Other_Field_2, &next );
   }
   if( ( id_or_name == c_field_id_Needs_Other_Modifier ) || ( id_or_name == c_field_name_Needs_Other_Modifier ) )
   {
      name = string( c_field_display_name_Needs_Other_Modifier );
      get_module_string( c_field_display_name_Needs_Other_Modifier, &next );
   }
   if( ( id_or_name == c_field_id_Needs_Other_Modifier_2 ) || ( id_or_name == c_field_name_Needs_Other_Modifier_2 ) )
   {
      name = string( c_field_display_name_Needs_Other_Modifier_2 );
      get_module_string( c_field_display_name_Needs_Other_Modifier_2, &next );
   }
   if( ( id_or_name == c_field_id_Needs_Other_Permission ) || ( id_or_name == c_field_name_Needs_Other_Permission ) )
   {
      name = string( c_field_display_name_Needs_Other_Permission );
      get_module_string( c_field_display_name_Needs_Other_Permission, &next );
   }
   if( ( id_or_name == c_field_id_Needs_Other_Permission_2 ) || ( id_or_name == c_field_name_Needs_Other_Permission_2 ) )
   {
      name = string( c_field_display_name_Needs_Other_Permission_2 );
      get_module_string( c_field_display_name_Needs_Other_Permission_2, &next );
   }
   if( ( id_or_name == c_field_id_Needs_Other_Procedure ) || ( id_or_name == c_field_name_Needs_Other_Procedure ) )
   {
      name = string( c_field_display_name_Needs_Other_Procedure );
      get_module_string( c_field_display_name_Needs_Other_Procedure, &next );
   }
   if( ( id_or_name == c_field_id_Needs_Other_Procedure_2 ) || ( id_or_name == c_field_name_Needs_Other_Procedure_2 ) )
   {
      name = string( c_field_display_name_Needs_Other_Procedure_2 );
      get_module_string( c_field_display_name_Needs_Other_Procedure_2, &next );
   }
   if( ( id_or_name == c_field_id_Needs_Other_Source_Child ) || ( id_or_name == c_field_name_Needs_Other_Source_Child ) )
   {
      name = string( c_field_display_name_Needs_Other_Source_Child );
      get_module_string( c_field_display_name_Needs_Other_Source_Child, &next );
   }
   if( ( id_or_name == c_field_id_Needs_Other_Source_Child_2 ) || ( id_or_name == c_field_name_Needs_Other_Source_Child_2 ) )
   {
      name = string( c_field_display_name_Needs_Other_Source_Child_2 );
      get_module_string( c_field_display_name_Needs_Other_Source_Child_2, &next );
   }
   if( ( id_or_name == c_field_id_Needs_Permission ) || ( id_or_name == c_field_name_Needs_Permission ) )
   {
      name = string( c_field_display_name_Needs_Permission );
      get_module_string( c_field_display_name_Needs_Permission, &next );
   }
   if( ( id_or_name == c_field_id_Needs_Procedure ) || ( id_or_name == c_field_name_Needs_Procedure ) )
   {
      name = string( c_field_display_name_Needs_Procedure );
      get_module_string( c_field_display_name_Needs_Procedure, &next );
   }
   if( ( id_or_name == c_field_id_Needs_Procedure_Arg ) || ( id_or_name == c_field_name_Needs_Procedure_Arg ) )
   {
      name = string( c_field_display_name_Needs_Procedure_Arg );
      get_module_string( c_field_display_name_Needs_Procedure_Arg, &next );
   }
   if( ( id_or_name == c_field_id_Needs_Procedure_Arg_2 ) || ( id_or_name == c_field_name_Needs_Procedure_Arg_2 ) )
   {
      name = string( c_field_display_name_Needs_Procedure_Arg_2 );
      get_module_string( c_field_display_name_Needs_Procedure_Arg_2, &next );
   }
   if( ( id_or_name == c_field_id_Needs_Procedure_Arg_3 ) || ( id_or_name == c_field_name_Needs_Procedure_Arg_3 ) )
   {
      name = string( c_field_display_name_Needs_Procedure_Arg_3 );
      get_module_string( c_field_display_name_Needs_Procedure_Arg_3, &next );
   }
   if( ( id_or_name == c_field_id_Needs_Source_Child ) || ( id_or_name == c_field_name_Needs_Source_Child ) )
   {
      name = string( c_field_display_name_Needs_Source_Child );
      get_module_string( c_field_display_name_Needs_Source_Child, &next );
   }
   if( ( id_or_name == c_field_id_Needs_Source_Field ) || ( id_or_name == c_field_name_Needs_Source_Field ) )
   {
      name = string( c_field_display_name_Needs_Source_Field );
      get_module_string( c_field_display_name_Needs_Source_Field, &next );
   }
   if( ( id_or_name == c_field_id_Needs_Source_Field_Or_Child ) || ( id_or_name == c_field_name_Needs_Source_Field_Or_Child ) )
   {
      name = string( c_field_display_name_Needs_Source_Field_Or_Child );
      get_module_string( c_field_display_name_Needs_Source_Field_Or_Child, &next );
   }
   if( ( id_or_name == c_field_id_Needs_Source_Parent ) || ( id_or_name == c_field_name_Needs_Source_Parent ) )
   {
      name = string( c_field_display_name_Needs_Source_Parent );
      get_module_string( c_field_display_name_Needs_Source_Parent, &next );
   }
   if( ( id_or_name == c_field_id_Needs_Test_Field ) || ( id_or_name == c_field_name_Needs_Test_Field ) )
   {
      name = string( c_field_display_name_Needs_Test_Field );
      get_module_string( c_field_display_name_Needs_Test_Field, &next );
   }
   if( ( id_or_name == c_field_id_Needs_Test_Value ) || ( id_or_name == c_field_name_Needs_Test_Value ) )
   {
      name = string( c_field_display_name_Needs_Test_Value );
      get_module_string( c_field_display_name_Needs_Test_Value, &next );
   }
   if( ( id_or_name == c_field_id_Needs_Value ) || ( id_or_name == c_field_name_Needs_Value ) )
   {
      name = string( c_field_display_name_Needs_Value );
      get_module_string( c_field_display_name_Needs_Value, &next );
   }
   if( ( id_or_name == c_field_id_Next_Child_Specification_Type ) || ( id_or_name == c_field_name_Next_Child_Specification_Type ) )
   {
      name = string( c_field_display_name_Next_Child_Specification_Type );
      get_module_string( c_field_display_name_Next_Child_Specification_Type, &next );
   }
   if( ( id_or_name == c_field_id_Next_Protect_Child_Rel ) || ( id_or_name == c_field_name_Next_Protect_Child_Rel ) )
   {
      name = string( c_field_display_name_Next_Protect_Child_Rel );
      get_module_string( c_field_display_name_Next_Protect_Child_Rel, &next );
   }
   if( ( id_or_name == c_field_id_Next_Protect_Procedure ) || ( id_or_name == c_field_name_Next_Protect_Procedure ) )
   {
      name = string( c_field_display_name_Next_Protect_Procedure );
      get_module_string( c_field_display_name_Next_Protect_Procedure, &next );
   }
   if( ( id_or_name == c_field_id_Next_Protect_Source_Parent ) || ( id_or_name == c_field_name_Next_Protect_Source_Parent ) )
   {
      name = string( c_field_display_name_Next_Protect_Source_Parent );
      get_module_string( c_field_display_name_Next_Protect_Source_Parent, &next );
   }
   if( ( id_or_name == c_field_id_Next_Specification_Actions ) || ( id_or_name == c_field_name_Next_Specification_Actions ) )
   {
      name = string( c_field_display_name_Next_Specification_Actions );
      get_module_string( c_field_display_name_Next_Specification_Actions, &next );
   }
   if( ( id_or_name == c_field_id_Notes ) || ( id_or_name == c_field_name_Notes ) )
   {
      name = string( c_field_display_name_Notes );
      get_module_string( c_field_display_name_Notes, &next );
   }
   if( ( id_or_name == c_field_id_Parent_Specification_Type ) || ( id_or_name == c_field_name_Parent_Specification_Type ) )
   {
      name = string( c_field_display_name_Parent_Specification_Type );
      get_module_string( c_field_display_name_Parent_Specification_Type, &next );
   }
   if( ( id_or_name == c_field_id_Procedure_Arg_primitive ) || ( id_or_name == c_field_name_Procedure_Arg_primitive ) )
   {
      name = string( c_field_display_name_Procedure_Arg_primitive );
      get_module_string( c_field_display_name_Procedure_Arg_primitive, &next );
   }
   if( ( id_or_name == c_field_id_Procedure_Arg_type ) || ( id_or_name == c_field_name_Procedure_Arg_type ) )
   {
      name = string( c_field_display_name_Procedure_Arg_type );
      get_module_string( c_field_display_name_Procedure_Arg_type, &next );
   }
   if( ( id_or_name == c_field_id_Protect_Child_Rel_From_Update ) || ( id_or_name == c_field_name_Protect_Child_Rel_From_Update ) )
   {
      name = string( c_field_display_name_Protect_Child_Rel_From_Update );
      get_module_string( c_field_display_name_Protect_Child_Rel_From_Update, &next );
   }
   if( ( id_or_name == c_field_id_Protect_Class_From_Edit ) || ( id_or_name == c_field_name_Protect_Class_From_Edit ) )
   {
      name = string( c_field_display_name_Protect_Class_From_Edit );
      get_module_string( c_field_display_name_Protect_Class_From_Edit, &next );
   }
   if( ( id_or_name == c_field_id_Protect_Class_From_Update ) || ( id_or_name == c_field_name_Protect_Class_From_Update ) )
   {
      name = string( c_field_display_name_Protect_Class_From_Update );
      get_module_string( c_field_display_name_Protect_Class_From_Update, &next );
   }
   if( ( id_or_name == c_field_id_Protect_Other_Class_From_Edit ) || ( id_or_name == c_field_name_Protect_Other_Class_From_Edit ) )
   {
      name = string( c_field_display_name_Protect_Other_Class_From_Edit );
      get_module_string( c_field_display_name_Protect_Other_Class_From_Edit, &next );
   }
   if( ( id_or_name == c_field_id_Protect_Procedure_From_Edit ) || ( id_or_name == c_field_name_Protect_Procedure_From_Edit ) )
   {
      name = string( c_field_display_name_Protect_Procedure_From_Edit );
      get_module_string( c_field_display_name_Protect_Procedure_From_Edit, &next );
   }
   if( ( id_or_name == c_field_id_Protect_Source_Parent ) || ( id_or_name == c_field_name_Protect_Source_Parent ) )
   {
      name = string( c_field_display_name_Protect_Source_Parent );
      get_module_string( c_field_display_name_Protect_Source_Parent, &next );
   }
   if( ( id_or_name == c_field_id_Set_Field_To_Source_Field_Or_Child ) || ( id_or_name == c_field_name_Set_Field_To_Source_Field_Or_Child ) )
   {
      name = string( c_field_display_name_Set_Field_To_Source_Field_Or_Child );
      get_module_string( c_field_display_name_Set_Field_To_Source_Field_Or_Child, &next );
   }
   if( ( id_or_name == c_field_id_Source_Field_Needs_Test_Field ) || ( id_or_name == c_field_name_Source_Field_Needs_Test_Field ) )
   {
      name = string( c_field_display_name_Source_Field_Needs_Test_Field );
      get_module_string( c_field_display_name_Source_Field_Needs_Test_Field, &next );
   }
   if( ( id_or_name == c_field_id_Source_Field_Type_Match ) || ( id_or_name == c_field_name_Source_Field_Type_Match ) )
   {
      name = string( c_field_display_name_Source_Field_Type_Match );
      get_module_string( c_field_display_name_Source_Field_Type_Match, &next );
   }
   if( ( id_or_name == c_field_id_Source_Parent_type ) || ( id_or_name == c_field_name_Source_Parent_type ) )
   {
      name = string( c_field_display_name_Source_Parent_type );
      get_module_string( c_field_display_name_Source_Parent_type, &next );
   }
   if( ( id_or_name == c_field_id_Source_type ) || ( id_or_name == c_field_name_Source_type ) )
   {
      name = string( c_field_display_name_Source_type );
      get_module_string( c_field_display_name_Source_type, &next );
   }
   if( ( id_or_name == c_field_id_Specification_Actions ) || ( id_or_name == c_field_name_Specification_Actions ) )
   {
      name = string( c_field_display_name_Specification_Actions );
      get_module_string( c_field_display_name_Specification_Actions, &next );
   }
   if( ( id_or_name == c_field_id_Specification_Name ) || ( id_or_name == c_field_name_Specification_Name ) )
   {
      name = string( c_field_display_name_Specification_Name );
      get_module_string( c_field_display_name_Specification_Name, &next );
   }
   if( ( id_or_name == c_field_id_Specification_Object ) || ( id_or_name == c_field_name_Specification_Object ) )
   {
      name = string( c_field_display_name_Specification_Object );
      get_module_string( c_field_display_name_Specification_Object, &next );
   }
   if( ( id_or_name == c_field_id_Specification_Strings ) || ( id_or_name == c_field_name_Specification_Strings ) )
   {
      name = string( c_field_display_name_Specification_Strings );
      get_module_string( c_field_display_name_Specification_Strings, &next );
   }
   if( ( id_or_name == c_field_id_Specification_Vars ) || ( id_or_name == c_field_name_Specification_Vars ) )
   {
      name = string( c_field_display_name_Specification_Vars );
      get_module_string( c_field_display_name_Specification_Vars, &next );
   }
   if( ( id_or_name == c_field_id_Test_Field_Type_Match ) || ( id_or_name == c_field_name_Test_Field_Type_Match ) )
   {
      name = string( c_field_display_name_Test_Field_Type_Match );
      get_module_string( c_field_display_name_Test_Field_Type_Match, &next );
   }
   if( ( id_or_name == c_field_id_Test_Field_type ) || ( id_or_name == c_field_name_Test_Field_type ) )
   {
      name = string( c_field_display_name_Test_Field_type );
      get_module_string( c_field_display_name_Test_Field_type, &next );
   }
   if( ( id_or_name == c_field_id_Use_Class_As_Source_Parent_Class ) || ( id_or_name == c_field_name_Use_Class_As_Source_Parent_Class ) )
   {
      name = string( c_field_display_name_Use_Class_As_Source_Parent_Class );
      get_module_string( c_field_display_name_Use_Class_As_Source_Parent_Class, &next );
   }
   if( ( id_or_name == c_field_id_Use_Field_Enum ) || ( id_or_name == c_field_name_Use_Field_Enum ) )
   {
      name = string( c_field_display_name_Use_Field_Enum );
      get_module_string( c_field_display_name_Use_Field_Enum, &next );
   }
   if( ( id_or_name == c_field_id_Use_Parent_Child_Rel_As_Source_Parent ) || ( id_or_name == c_field_name_Use_Parent_Child_Rel_As_Source_Parent ) )
   {
      name = string( c_field_display_name_Use_Parent_Child_Rel_As_Source_Parent );
      get_module_string( c_field_display_name_Use_Parent_Child_Rel_As_Source_Parent, &next );
   }
   if( ( id_or_name == c_field_id_Use_Parent_Child_Rel_For_Class ) || ( id_or_name == c_field_name_Use_Parent_Child_Rel_For_Class ) )
   {
      name = string( c_field_display_name_Use_Parent_Child_Rel_For_Class );
      get_module_string( c_field_display_name_Use_Parent_Child_Rel_For_Class, &next );
   }
   if( ( id_or_name == c_field_id_Use_Parent_Child_Rel_For_Other_Class ) || ( id_or_name == c_field_name_Use_Parent_Child_Rel_For_Other_Class ) )
   {
      name = string( c_field_display_name_Use_Parent_Child_Rel_For_Other_Class );
      get_module_string( c_field_display_name_Use_Parent_Child_Rel_For_Other_Class, &next );
   }
   if( ( id_or_name == c_field_id_Use_Parent_Class ) || ( id_or_name == c_field_name_Use_Parent_Class ) )
   {
      name = string( c_field_display_name_Use_Parent_Class );
      get_module_string( c_field_display_name_Use_Parent_Class, &next );
   }
   if( ( id_or_name == c_field_id_Use_Parent_Field_For_Class ) || ( id_or_name == c_field_name_Use_Parent_Field_For_Class ) )
   {
      name = string( c_field_display_name_Use_Parent_Field_For_Class );
      get_module_string( c_field_display_name_Use_Parent_Field_For_Class, &next );
   }
   if( ( id_or_name == c_field_id_Use_Parent_Other_Class ) || ( id_or_name == c_field_name_Use_Parent_Other_Class ) )
   {
      name = string( c_field_display_name_Use_Parent_Other_Class );
      get_module_string( c_field_display_name_Use_Parent_Other_Class, &next );
   }
   if( ( id_or_name == c_field_id_Use_Parent_Other_Class_For_Class ) || ( id_or_name == c_field_name_Use_Parent_Other_Class_For_Class ) )
   {
      name = string( c_field_display_name_Use_Parent_Other_Class_For_Class );
      get_module_string( c_field_display_name_Use_Parent_Other_Class_For_Class, &next );
   }
   if( ( id_or_name == c_field_id_Use_Parent_Procedure ) || ( id_or_name == c_field_name_Use_Parent_Procedure ) )
   {
      name = string( c_field_display_name_Use_Parent_Procedure );
      get_module_string( c_field_display_name_Use_Parent_Procedure, &next );
   }
   if( ( id_or_name == c_field_id_Use_Parent_Source_Class ) || ( id_or_name == c_field_name_Use_Parent_Source_Class ) )
   {
      name = string( c_field_display_name_Use_Parent_Source_Class );
      get_module_string( c_field_display_name_Use_Parent_Source_Class, &next );
   }
   if( ( id_or_name == c_field_id_Use_Parent_Source_Field_For_Class ) || ( id_or_name == c_field_name_Use_Parent_Source_Field_For_Class ) )
   {
      name = string( c_field_display_name_Use_Parent_Source_Field_For_Class );
      get_module_string( c_field_display_name_Use_Parent_Source_Field_For_Class, &next );
   }
   if( ( id_or_name == c_field_id_Use_Parent_Source_Parent ) || ( id_or_name == c_field_name_Use_Parent_Source_Parent ) )
   {
      name = string( c_field_display_name_Use_Parent_Source_Parent );
      get_module_string( c_field_display_name_Use_Parent_Source_Parent, &next );
   }
   if( ( id_or_name == c_field_id_Use_Parent_Source_Parent_For_Class ) || ( id_or_name == c_field_name_Use_Parent_Source_Parent_For_Class ) )
   {
      name = string( c_field_display_name_Use_Parent_Source_Parent_For_Class );
      get_module_string( c_field_display_name_Use_Parent_Source_Parent_For_Class, &next );
   }
   if( ( id_or_name == c_field_id_Use_Source_Field_Enum ) || ( id_or_name == c_field_name_Use_Source_Field_Enum ) )
   {
      name = string( c_field_display_name_Use_Source_Field_Enum );
      get_module_string( c_field_display_name_Use_Source_Field_Enum, &next );
   }
   if( ( id_or_name == c_field_id_Use_Source_Field_Or_Child_Enum ) || ( id_or_name == c_field_name_Use_Source_Field_Or_Child_Enum ) )
   {
      name = string( c_field_display_name_Use_Source_Field_Or_Child_Enum );
      get_module_string( c_field_display_name_Use_Source_Field_Or_Child_Enum, &next );
   }
   if( ( id_or_name == c_field_id_View_Id ) || ( id_or_name == c_field_name_View_Id ) )
   {
      name = string( c_field_display_name_View_Id );
      get_module_string( c_field_display_name_View_Id, &next );
   }

   // NOTE: It is being assumed here that the customised UOM symbol for a field (if it
   // has one) will be in the module string that immediately follows that of its name.
   if( next.first.find( name + "_(" ) == 0 )
      uom_symbol = next.second;

   return uom_symbol;
}

string Meta_Specification_Type::get_field_display_name( const string& id_or_name ) const
{
   string display_name;

   if( id_or_name.empty( ) )
      throw runtime_error( "unexpected empty field id_or_name for get_field_display_name" );
   if( ( id_or_name == c_field_id_Allow_Child_Relationship ) || ( id_or_name == c_field_name_Allow_Child_Relationship ) )
      display_name = get_module_string( c_field_display_name_Allow_Child_Relationship );
   if( ( id_or_name == c_field_id_Allow_Enum ) || ( id_or_name == c_field_name_Allow_Enum ) )
      display_name = get_module_string( c_field_display_name_Allow_Enum );
   if( ( id_or_name == c_field_id_Allow_Enum_Item ) || ( id_or_name == c_field_name_Allow_Enum_Item ) )
      display_name = get_module_string( c_field_display_name_Allow_Enum_Item );
   if( ( id_or_name == c_field_id_Allow_FK_Source_Field_Choice ) || ( id_or_name == c_field_name_Allow_FK_Source_Field_Choice ) )
      display_name = get_module_string( c_field_display_name_Allow_FK_Source_Field_Choice );
   if( ( id_or_name == c_field_id_Allow_FK_Test_Field_Choice ) || ( id_or_name == c_field_name_Allow_FK_Test_Field_Choice ) )
      display_name = get_module_string( c_field_display_name_Allow_FK_Test_Field_Choice );
   if( ( id_or_name == c_field_id_Allow_Field ) || ( id_or_name == c_field_name_Allow_Field ) )
      display_name = get_module_string( c_field_display_name_Allow_Field );
   if( ( id_or_name == c_field_id_Allow_Modifier ) || ( id_or_name == c_field_name_Allow_Modifier ) )
      display_name = get_module_string( c_field_display_name_Allow_Modifier );
   if( ( id_or_name == c_field_id_Allow_Options ) || ( id_or_name == c_field_name_Allow_Options ) )
      display_name = get_module_string( c_field_display_name_Allow_Options );
   if( ( id_or_name == c_field_id_Allow_Other_Class ) || ( id_or_name == c_field_name_Allow_Other_Class ) )
      display_name = get_module_string( c_field_display_name_Allow_Other_Class );
   if( ( id_or_name == c_field_id_Allow_Other_Class_Field ) || ( id_or_name == c_field_name_Allow_Other_Class_Field ) )
      display_name = get_module_string( c_field_display_name_Allow_Other_Class_Field );
   if( ( id_or_name == c_field_id_Allow_Other_Field ) || ( id_or_name == c_field_name_Allow_Other_Field ) )
      display_name = get_module_string( c_field_display_name_Allow_Other_Field );
   if( ( id_or_name == c_field_id_Allow_Other_Field_2 ) || ( id_or_name == c_field_name_Allow_Other_Field_2 ) )
      display_name = get_module_string( c_field_display_name_Allow_Other_Field_2 );
   if( ( id_or_name == c_field_id_Allow_Other_Modifier ) || ( id_or_name == c_field_name_Allow_Other_Modifier ) )
      display_name = get_module_string( c_field_display_name_Allow_Other_Modifier );
   if( ( id_or_name == c_field_id_Allow_Other_Modifier_2 ) || ( id_or_name == c_field_name_Allow_Other_Modifier_2 ) )
      display_name = get_module_string( c_field_display_name_Allow_Other_Modifier_2 );
   if( ( id_or_name == c_field_id_Allow_Other_Procedure ) || ( id_or_name == c_field_name_Allow_Other_Procedure ) )
      display_name = get_module_string( c_field_display_name_Allow_Other_Procedure );
   if( ( id_or_name == c_field_id_Allow_Other_Procedure_2 ) || ( id_or_name == c_field_name_Allow_Other_Procedure_2 ) )
      display_name = get_module_string( c_field_display_name_Allow_Other_Procedure_2 );
   if( ( id_or_name == c_field_id_Allow_Other_Source_Child ) || ( id_or_name == c_field_name_Allow_Other_Source_Child ) )
      display_name = get_module_string( c_field_display_name_Allow_Other_Source_Child );
   if( ( id_or_name == c_field_id_Allow_Other_Source_Child_2 ) || ( id_or_name == c_field_name_Allow_Other_Source_Child_2 ) )
      display_name = get_module_string( c_field_display_name_Allow_Other_Source_Child_2 );
   if( ( id_or_name == c_field_id_Allow_Permission ) || ( id_or_name == c_field_name_Allow_Permission ) )
      display_name = get_module_string( c_field_display_name_Allow_Permission );
   if( ( id_or_name == c_field_id_Allow_Procedure ) || ( id_or_name == c_field_name_Allow_Procedure ) )
      display_name = get_module_string( c_field_display_name_Allow_Procedure );
   if( ( id_or_name == c_field_id_Allow_Procedure_Arg ) || ( id_or_name == c_field_name_Allow_Procedure_Arg ) )
      display_name = get_module_string( c_field_display_name_Allow_Procedure_Arg );
   if( ( id_or_name == c_field_id_Allow_Source_Child ) || ( id_or_name == c_field_name_Allow_Source_Child ) )
      display_name = get_module_string( c_field_display_name_Allow_Source_Child );
   if( ( id_or_name == c_field_id_Allow_Source_Class ) || ( id_or_name == c_field_name_Allow_Source_Class ) )
      display_name = get_module_string( c_field_display_name_Allow_Source_Class );
   if( ( id_or_name == c_field_id_Allow_Source_Field ) || ( id_or_name == c_field_name_Allow_Source_Field ) )
      display_name = get_module_string( c_field_display_name_Allow_Source_Field );
   if( ( id_or_name == c_field_id_Allow_Source_Grandchild ) || ( id_or_name == c_field_name_Allow_Source_Grandchild ) )
      display_name = get_module_string( c_field_display_name_Allow_Source_Grandchild );
   if( ( id_or_name == c_field_id_Allow_Source_Parent ) || ( id_or_name == c_field_name_Allow_Source_Parent ) )
      display_name = get_module_string( c_field_display_name_Allow_Source_Parent );
   if( ( id_or_name == c_field_id_Allow_Test_Field ) || ( id_or_name == c_field_name_Allow_Test_Field ) )
      display_name = get_module_string( c_field_display_name_Allow_Test_Field );
   if( ( id_or_name == c_field_id_Allow_Test_Value ) || ( id_or_name == c_field_name_Allow_Test_Value ) )
      display_name = get_module_string( c_field_display_name_Allow_Test_Value );
   if( ( id_or_name == c_field_id_Allow_Value ) || ( id_or_name == c_field_name_Allow_Value ) )
      display_name = get_module_string( c_field_display_name_Allow_Value );
   if( ( id_or_name == c_field_id_Can_Access_Below_Model ) || ( id_or_name == c_field_name_Can_Access_Below_Model ) )
      display_name = get_module_string( c_field_display_name_Can_Access_Below_Model );
   if( ( id_or_name == c_field_id_Child_Prefix ) || ( id_or_name == c_field_name_Child_Prefix ) )
      display_name = get_module_string( c_field_display_name_Child_Prefix );
   if( ( id_or_name == c_field_id_Child_Rel_Grandparent_Match ) || ( id_or_name == c_field_name_Child_Rel_Grandparent_Match ) )
      display_name = get_module_string( c_field_display_name_Child_Rel_Grandparent_Match );
   if( ( id_or_name == c_field_id_Child_Rel_Parent_Match ) || ( id_or_name == c_field_name_Child_Rel_Parent_Match ) )
      display_name = get_module_string( c_field_display_name_Child_Rel_Parent_Match );
   if( ( id_or_name == c_field_id_Child_Relationship_Class_Match ) || ( id_or_name == c_field_name_Child_Relationship_Class_Match ) )
      display_name = get_module_string( c_field_display_name_Child_Relationship_Class_Match );
   if( ( id_or_name == c_field_id_Child_Specification_Type ) || ( id_or_name == c_field_name_Child_Specification_Type ) )
      display_name = get_module_string( c_field_display_name_Child_Specification_Type );
   if( ( id_or_name == c_field_id_Default_Child_Vars ) || ( id_or_name == c_field_name_Default_Child_Vars ) )
      display_name = get_module_string( c_field_display_name_Default_Child_Vars );
   if( ( id_or_name == c_field_id_Field_type ) || ( id_or_name == c_field_name_Field_type ) )
      display_name = get_module_string( c_field_display_name_Field_type );
   if( ( id_or_name == c_field_id_Has_Next_Specification_Info ) || ( id_or_name == c_field_name_Has_Next_Specification_Info ) )
      display_name = get_module_string( c_field_display_name_Has_Next_Specification_Info );
   if( ( id_or_name == c_field_id_Is_Child_Only ) || ( id_or_name == c_field_name_Is_Child_Only ) )
      display_name = get_module_string( c_field_display_name_Is_Child_Only );
   if( ( id_or_name == c_field_id_Is_Required_For_UI_Gen ) || ( id_or_name == c_field_name_Is_Required_For_UI_Gen ) )
      display_name = get_module_string( c_field_display_name_Is_Required_For_UI_Gen );
   if( ( id_or_name == c_field_id_Is_System ) || ( id_or_name == c_field_name_Is_System ) )
      display_name = get_module_string( c_field_display_name_Is_System );
   if( ( id_or_name == c_field_id_Name ) || ( id_or_name == c_field_name_Name ) )
      display_name = get_module_string( c_field_display_name_Name );
   if( ( id_or_name == c_field_id_Needs_Child_Relationship ) || ( id_or_name == c_field_name_Needs_Child_Relationship ) )
      display_name = get_module_string( c_field_display_name_Needs_Child_Relationship );
   if( ( id_or_name == c_field_id_Needs_Class ) || ( id_or_name == c_field_name_Needs_Class ) )
      display_name = get_module_string( c_field_display_name_Needs_Class );
   if( ( id_or_name == c_field_id_Needs_Enum ) || ( id_or_name == c_field_name_Needs_Enum ) )
      display_name = get_module_string( c_field_display_name_Needs_Enum );
   if( ( id_or_name == c_field_id_Needs_Enum_Item ) || ( id_or_name == c_field_name_Needs_Enum_Item ) )
      display_name = get_module_string( c_field_display_name_Needs_Enum_Item );
   if( ( id_or_name == c_field_id_Needs_Enum_Item_2 ) || ( id_or_name == c_field_name_Needs_Enum_Item_2 ) )
      display_name = get_module_string( c_field_display_name_Needs_Enum_Item_2 );
   if( ( id_or_name == c_field_id_Needs_Enum_Item_3 ) || ( id_or_name == c_field_name_Needs_Enum_Item_3 ) )
      display_name = get_module_string( c_field_display_name_Needs_Enum_Item_3 );
   if( ( id_or_name == c_field_id_Needs_Enum_Item_4 ) || ( id_or_name == c_field_name_Needs_Enum_Item_4 ) )
      display_name = get_module_string( c_field_display_name_Needs_Enum_Item_4 );
   if( ( id_or_name == c_field_id_Needs_Enum_Item_5 ) || ( id_or_name == c_field_name_Needs_Enum_Item_5 ) )
      display_name = get_module_string( c_field_display_name_Needs_Enum_Item_5 );
   if( ( id_or_name == c_field_id_Needs_Field ) || ( id_or_name == c_field_name_Needs_Field ) )
      display_name = get_module_string( c_field_display_name_Needs_Field );
   if( ( id_or_name == c_field_id_Needs_Modifier ) || ( id_or_name == c_field_name_Needs_Modifier ) )
      display_name = get_module_string( c_field_display_name_Needs_Modifier );
   if( ( id_or_name == c_field_id_Needs_Other_Class ) || ( id_or_name == c_field_name_Needs_Other_Class ) )
      display_name = get_module_string( c_field_display_name_Needs_Other_Class );
   if( ( id_or_name == c_field_id_Needs_Other_Class_Field ) || ( id_or_name == c_field_name_Needs_Other_Class_Field ) )
      display_name = get_module_string( c_field_display_name_Needs_Other_Class_Field );
   if( ( id_or_name == c_field_id_Needs_Other_Field ) || ( id_or_name == c_field_name_Needs_Other_Field ) )
      display_name = get_module_string( c_field_display_name_Needs_Other_Field );
   if( ( id_or_name == c_field_id_Needs_Other_Field_2 ) || ( id_or_name == c_field_name_Needs_Other_Field_2 ) )
      display_name = get_module_string( c_field_display_name_Needs_Other_Field_2 );
   if( ( id_or_name == c_field_id_Needs_Other_Modifier ) || ( id_or_name == c_field_name_Needs_Other_Modifier ) )
      display_name = get_module_string( c_field_display_name_Needs_Other_Modifier );
   if( ( id_or_name == c_field_id_Needs_Other_Modifier_2 ) || ( id_or_name == c_field_name_Needs_Other_Modifier_2 ) )
      display_name = get_module_string( c_field_display_name_Needs_Other_Modifier_2 );
   if( ( id_or_name == c_field_id_Needs_Other_Permission ) || ( id_or_name == c_field_name_Needs_Other_Permission ) )
      display_name = get_module_string( c_field_display_name_Needs_Other_Permission );
   if( ( id_or_name == c_field_id_Needs_Other_Permission_2 ) || ( id_or_name == c_field_name_Needs_Other_Permission_2 ) )
      display_name = get_module_string( c_field_display_name_Needs_Other_Permission_2 );
   if( ( id_or_name == c_field_id_Needs_Other_Procedure ) || ( id_or_name == c_field_name_Needs_Other_Procedure ) )
      display_name = get_module_string( c_field_display_name_Needs_Other_Procedure );
   if( ( id_or_name == c_field_id_Needs_Other_Procedure_2 ) || ( id_or_name == c_field_name_Needs_Other_Procedure_2 ) )
      display_name = get_module_string( c_field_display_name_Needs_Other_Procedure_2 );
   if( ( id_or_name == c_field_id_Needs_Other_Source_Child ) || ( id_or_name == c_field_name_Needs_Other_Source_Child ) )
      display_name = get_module_string( c_field_display_name_Needs_Other_Source_Child );
   if( ( id_or_name == c_field_id_Needs_Other_Source_Child_2 ) || ( id_or_name == c_field_name_Needs_Other_Source_Child_2 ) )
      display_name = get_module_string( c_field_display_name_Needs_Other_Source_Child_2 );
   if( ( id_or_name == c_field_id_Needs_Permission ) || ( id_or_name == c_field_name_Needs_Permission ) )
      display_name = get_module_string( c_field_display_name_Needs_Permission );
   if( ( id_or_name == c_field_id_Needs_Procedure ) || ( id_or_name == c_field_name_Needs_Procedure ) )
      display_name = get_module_string( c_field_display_name_Needs_Procedure );
   if( ( id_or_name == c_field_id_Needs_Procedure_Arg ) || ( id_or_name == c_field_name_Needs_Procedure_Arg ) )
      display_name = get_module_string( c_field_display_name_Needs_Procedure_Arg );
   if( ( id_or_name == c_field_id_Needs_Procedure_Arg_2 ) || ( id_or_name == c_field_name_Needs_Procedure_Arg_2 ) )
      display_name = get_module_string( c_field_display_name_Needs_Procedure_Arg_2 );
   if( ( id_or_name == c_field_id_Needs_Procedure_Arg_3 ) || ( id_or_name == c_field_name_Needs_Procedure_Arg_3 ) )
      display_name = get_module_string( c_field_display_name_Needs_Procedure_Arg_3 );
   if( ( id_or_name == c_field_id_Needs_Source_Child ) || ( id_or_name == c_field_name_Needs_Source_Child ) )
      display_name = get_module_string( c_field_display_name_Needs_Source_Child );
   if( ( id_or_name == c_field_id_Needs_Source_Field ) || ( id_or_name == c_field_name_Needs_Source_Field ) )
      display_name = get_module_string( c_field_display_name_Needs_Source_Field );
   if( ( id_or_name == c_field_id_Needs_Source_Field_Or_Child ) || ( id_or_name == c_field_name_Needs_Source_Field_Or_Child ) )
      display_name = get_module_string( c_field_display_name_Needs_Source_Field_Or_Child );
   if( ( id_or_name == c_field_id_Needs_Source_Parent ) || ( id_or_name == c_field_name_Needs_Source_Parent ) )
      display_name = get_module_string( c_field_display_name_Needs_Source_Parent );
   if( ( id_or_name == c_field_id_Needs_Test_Field ) || ( id_or_name == c_field_name_Needs_Test_Field ) )
      display_name = get_module_string( c_field_display_name_Needs_Test_Field );
   if( ( id_or_name == c_field_id_Needs_Test_Value ) || ( id_or_name == c_field_name_Needs_Test_Value ) )
      display_name = get_module_string( c_field_display_name_Needs_Test_Value );
   if( ( id_or_name == c_field_id_Needs_Value ) || ( id_or_name == c_field_name_Needs_Value ) )
      display_name = get_module_string( c_field_display_name_Needs_Value );
   if( ( id_or_name == c_field_id_Next_Child_Specification_Type ) || ( id_or_name == c_field_name_Next_Child_Specification_Type ) )
      display_name = get_module_string( c_field_display_name_Next_Child_Specification_Type );
   if( ( id_or_name == c_field_id_Next_Protect_Child_Rel ) || ( id_or_name == c_field_name_Next_Protect_Child_Rel ) )
      display_name = get_module_string( c_field_display_name_Next_Protect_Child_Rel );
   if( ( id_or_name == c_field_id_Next_Protect_Procedure ) || ( id_or_name == c_field_name_Next_Protect_Procedure ) )
      display_name = get_module_string( c_field_display_name_Next_Protect_Procedure );
   if( ( id_or_name == c_field_id_Next_Protect_Source_Parent ) || ( id_or_name == c_field_name_Next_Protect_Source_Parent ) )
      display_name = get_module_string( c_field_display_name_Next_Protect_Source_Parent );
   if( ( id_or_name == c_field_id_Next_Specification_Actions ) || ( id_or_name == c_field_name_Next_Specification_Actions ) )
      display_name = get_module_string( c_field_display_name_Next_Specification_Actions );
   if( ( id_or_name == c_field_id_Notes ) || ( id_or_name == c_field_name_Notes ) )
      display_name = get_module_string( c_field_display_name_Notes );
   if( ( id_or_name == c_field_id_Parent_Specification_Type ) || ( id_or_name == c_field_name_Parent_Specification_Type ) )
      display_name = get_module_string( c_field_display_name_Parent_Specification_Type );
   if( ( id_or_name == c_field_id_Procedure_Arg_primitive ) || ( id_or_name == c_field_name_Procedure_Arg_primitive ) )
      display_name = get_module_string( c_field_display_name_Procedure_Arg_primitive );
   if( ( id_or_name == c_field_id_Procedure_Arg_type ) || ( id_or_name == c_field_name_Procedure_Arg_type ) )
      display_name = get_module_string( c_field_display_name_Procedure_Arg_type );
   if( ( id_or_name == c_field_id_Protect_Child_Rel_From_Update ) || ( id_or_name == c_field_name_Protect_Child_Rel_From_Update ) )
      display_name = get_module_string( c_field_display_name_Protect_Child_Rel_From_Update );
   if( ( id_or_name == c_field_id_Protect_Class_From_Edit ) || ( id_or_name == c_field_name_Protect_Class_From_Edit ) )
      display_name = get_module_string( c_field_display_name_Protect_Class_From_Edit );
   if( ( id_or_name == c_field_id_Protect_Class_From_Update ) || ( id_or_name == c_field_name_Protect_Class_From_Update ) )
      display_name = get_module_string( c_field_display_name_Protect_Class_From_Update );
   if( ( id_or_name == c_field_id_Protect_Other_Class_From_Edit ) || ( id_or_name == c_field_name_Protect_Other_Class_From_Edit ) )
      display_name = get_module_string( c_field_display_name_Protect_Other_Class_From_Edit );
   if( ( id_or_name == c_field_id_Protect_Procedure_From_Edit ) || ( id_or_name == c_field_name_Protect_Procedure_From_Edit ) )
      display_name = get_module_string( c_field_display_name_Protect_Procedure_From_Edit );
   if( ( id_or_name == c_field_id_Protect_Source_Parent ) || ( id_or_name == c_field_name_Protect_Source_Parent ) )
      display_name = get_module_string( c_field_display_name_Protect_Source_Parent );
   if( ( id_or_name == c_field_id_Set_Field_To_Source_Field_Or_Child ) || ( id_or_name == c_field_name_Set_Field_To_Source_Field_Or_Child ) )
      display_name = get_module_string( c_field_display_name_Set_Field_To_Source_Field_Or_Child );
   if( ( id_or_name == c_field_id_Source_Field_Needs_Test_Field ) || ( id_or_name == c_field_name_Source_Field_Needs_Test_Field ) )
      display_name = get_module_string( c_field_display_name_Source_Field_Needs_Test_Field );
   if( ( id_or_name == c_field_id_Source_Field_Type_Match ) || ( id_or_name == c_field_name_Source_Field_Type_Match ) )
      display_name = get_module_string( c_field_display_name_Source_Field_Type_Match );
   if( ( id_or_name == c_field_id_Source_Parent_type ) || ( id_or_name == c_field_name_Source_Parent_type ) )
      display_name = get_module_string( c_field_display_name_Source_Parent_type );
   if( ( id_or_name == c_field_id_Source_type ) || ( id_or_name == c_field_name_Source_type ) )
      display_name = get_module_string( c_field_display_name_Source_type );
   if( ( id_or_name == c_field_id_Specification_Actions ) || ( id_or_name == c_field_name_Specification_Actions ) )
      display_name = get_module_string( c_field_display_name_Specification_Actions );
   if( ( id_or_name == c_field_id_Specification_Name ) || ( id_or_name == c_field_name_Specification_Name ) )
      display_name = get_module_string( c_field_display_name_Specification_Name );
   if( ( id_or_name == c_field_id_Specification_Object ) || ( id_or_name == c_field_name_Specification_Object ) )
      display_name = get_module_string( c_field_display_name_Specification_Object );
   if( ( id_or_name == c_field_id_Specification_Strings ) || ( id_or_name == c_field_name_Specification_Strings ) )
      display_name = get_module_string( c_field_display_name_Specification_Strings );
   if( ( id_or_name == c_field_id_Specification_Vars ) || ( id_or_name == c_field_name_Specification_Vars ) )
      display_name = get_module_string( c_field_display_name_Specification_Vars );
   if( ( id_or_name == c_field_id_Test_Field_Type_Match ) || ( id_or_name == c_field_name_Test_Field_Type_Match ) )
      display_name = get_module_string( c_field_display_name_Test_Field_Type_Match );
   if( ( id_or_name == c_field_id_Test_Field_type ) || ( id_or_name == c_field_name_Test_Field_type ) )
      display_name = get_module_string( c_field_display_name_Test_Field_type );
   if( ( id_or_name == c_field_id_Use_Class_As_Source_Parent_Class ) || ( id_or_name == c_field_name_Use_Class_As_Source_Parent_Class ) )
      display_name = get_module_string( c_field_display_name_Use_Class_As_Source_Parent_Class );
   if( ( id_or_name == c_field_id_Use_Field_Enum ) || ( id_or_name == c_field_name_Use_Field_Enum ) )
      display_name = get_module_string( c_field_display_name_Use_Field_Enum );
   if( ( id_or_name == c_field_id_Use_Parent_Child_Rel_As_Source_Parent ) || ( id_or_name == c_field_name_Use_Parent_Child_Rel_As_Source_Parent ) )
      display_name = get_module_string( c_field_display_name_Use_Parent_Child_Rel_As_Source_Parent );
   if( ( id_or_name == c_field_id_Use_Parent_Child_Rel_For_Class ) || ( id_or_name == c_field_name_Use_Parent_Child_Rel_For_Class ) )
      display_name = get_module_string( c_field_display_name_Use_Parent_Child_Rel_For_Class );
   if( ( id_or_name == c_field_id_Use_Parent_Child_Rel_For_Other_Class ) || ( id_or_name == c_field_name_Use_Parent_Child_Rel_For_Other_Class ) )
      display_name = get_module_string( c_field_display_name_Use_Parent_Child_Rel_For_Other_Class );
   if( ( id_or_name == c_field_id_Use_Parent_Class ) || ( id_or_name == c_field_name_Use_Parent_Class ) )
      display_name = get_module_string( c_field_display_name_Use_Parent_Class );
   if( ( id_or_name == c_field_id_Use_Parent_Field_For_Class ) || ( id_or_name == c_field_name_Use_Parent_Field_For_Class ) )
      display_name = get_module_string( c_field_display_name_Use_Parent_Field_For_Class );
   if( ( id_or_name == c_field_id_Use_Parent_Other_Class ) || ( id_or_name == c_field_name_Use_Parent_Other_Class ) )
      display_name = get_module_string( c_field_display_name_Use_Parent_Other_Class );
   if( ( id_or_name == c_field_id_Use_Parent_Other_Class_For_Class ) || ( id_or_name == c_field_name_Use_Parent_Other_Class_For_Class ) )
      display_name = get_module_string( c_field_display_name_Use_Parent_Other_Class_For_Class );
   if( ( id_or_name == c_field_id_Use_Parent_Procedure ) || ( id_or_name == c_field_name_Use_Parent_Procedure ) )
      display_name = get_module_string( c_field_display_name_Use_Parent_Procedure );
   if( ( id_or_name == c_field_id_Use_Parent_Source_Class ) || ( id_or_name == c_field_name_Use_Parent_Source_Class ) )
      display_name = get_module_string( c_field_display_name_Use_Parent_Source_Class );
   if( ( id_or_name == c_field_id_Use_Parent_Source_Field_For_Class ) || ( id_or_name == c_field_name_Use_Parent_Source_Field_For_Class ) )
      display_name = get_module_string( c_field_display_name_Use_Parent_Source_Field_For_Class );
   if( ( id_or_name == c_field_id_Use_Parent_Source_Parent ) || ( id_or_name == c_field_name_Use_Parent_Source_Parent ) )
      display_name = get_module_string( c_field_display_name_Use_Parent_Source_Parent );
   if( ( id_or_name == c_field_id_Use_Parent_Source_Parent_For_Class ) || ( id_or_name == c_field_name_Use_Parent_Source_Parent_For_Class ) )
      display_name = get_module_string( c_field_display_name_Use_Parent_Source_Parent_For_Class );
   if( ( id_or_name == c_field_id_Use_Source_Field_Enum ) || ( id_or_name == c_field_name_Use_Source_Field_Enum ) )
      display_name = get_module_string( c_field_display_name_Use_Source_Field_Enum );
   if( ( id_or_name == c_field_id_Use_Source_Field_Or_Child_Enum ) || ( id_or_name == c_field_name_Use_Source_Field_Or_Child_Enum ) )
      display_name = get_module_string( c_field_display_name_Use_Source_Field_Or_Child_Enum );
   if( ( id_or_name == c_field_id_View_Id ) || ( id_or_name == c_field_name_View_Id ) )
      display_name = get_module_string( c_field_display_name_View_Id );

   return display_name;
}

void Meta_Specification_Type::clear_foreign_key( const string& field )
{
   p_impl->clear_foreign_key( field );
}

void Meta_Specification_Type::set_foreign_key_value( const string& field, const string& value )
{
   p_impl->set_foreign_key_value( field, value );
}

const string& Meta_Specification_Type::get_foreign_key_value( const string& field )
{
   return p_impl->get_foreign_key_value( field );
}

void Meta_Specification_Type::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   p_impl->get_foreign_key_values( foreign_key_values );
}

void Meta_Specification_Type::setup_foreign_key( Meta_Specification_Type& o, const string& value )
{
   static_cast< Meta_Specification_Type& >( o ).set_key( value );
}

void Meta_Specification_Type::setup_graph_parent( Meta_Specification_Type& o, const string& foreign_key_field )
{
   static_cast< Meta_Specification_Type& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Specification_Type::setup_graph_parent( Meta_Specification& o, const string& foreign_key_field )
{
   static_cast< Meta_Specification& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Specification_Type::setup_graph_parent(
 Meta_Specification_Type& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Specification_Type& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Specification_Type& >( o ).set_key( init_value );
}

size_t Meta_Specification_Type::get_total_child_relationships( ) const
{
   return p_impl->total_child_relationships;
}

void Meta_Specification_Type::set_total_child_relationships( size_t new_total_child_relationships ) const
{
   p_impl->total_child_relationships = new_total_child_relationships;
}

size_t Meta_Specification_Type::get_num_foreign_key_children( bool is_internal ) const
{
   size_t rc = 5;

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

class_base* Meta_Specification_Type::get_next_foreign_key_child(
 size_t child_num, string& next_child_field, cascade_op op, bool is_internal )
{
   class_base* p_class_base = 0;

   if( child_num >= 5 )
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
            next_child_field = "301710";
            p_class_base = &child_Specification_Type_Child( );
         }
         break;

         case 1:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "301406";
            p_class_base = &child_Specification_Child( );
         }
         break;

         case 2:
         if( op == e_cascade_op_restrict )
         {
            next_child_field = "301720";
            p_class_base = &child_Specification_Type_Next_Child( );
         }
         break;

         case 3:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "301700";
            p_class_base = &child_Specification_Type_Parent( );
         }
         break;

         case 4:
         if( op == e_cascade_op_destroy )
         {
            next_child_field = "301410";
            p_class_base = &child_Specification( );
         }
         break;
      }
   }

   return p_class_base;
}

void Meta_Specification_Type::add_extra_fixed_info( vector< pair< string, string > >& fixed_info ) const
{
   p_impl->add_extra_fixed_info( fixed_info );
}

void Meta_Specification_Type::add_extra_paging_info( vector< pair< string, string > >& paging_info ) const
{
   p_impl->add_extra_paging_info( paging_info );
}

string Meta_Specification_Type::get_class_id( ) const
{
   return static_class_id( );
}

string Meta_Specification_Type::get_class_name( ) const
{
   return static_class_name( );
}

string Meta_Specification_Type::get_plural_name( ) const
{
   return static_plural_name( );
}

string Meta_Specification_Type::get_module_id( ) const
{
   return static_module_id( );
}

string Meta_Specification_Type::get_module_name( ) const
{
   return static_module_name( );
}

string Meta_Specification_Type::get_display_name( bool plural ) const
{
   string key( plural ? "plural_" : "class_" );

   key += "specification_type";

   return get_module_string( key );
}

string Meta_Specification_Type::get_raw_variable( const std::string& name ) const
{
   if( name == g_state_names_variable )
      return p_impl->get_state_names( );
   else
      return class_base::get_raw_variable( name );
}

string Meta_Specification_Type::get_create_instance_info( ) const
{
   return "";
}

string Meta_Specification_Type::get_update_instance_info( ) const
{
   return "";
}

string Meta_Specification_Type::get_destroy_instance_info( ) const
{
   return "";
}

string Meta_Specification_Type::get_execute_procedure_info( const string& procedure_id ) const
{
   string retval;

   if( procedure_id.empty( ) )
      throw runtime_error( "unexpected empty procedure_id for get_execute_procedure_info" );
   else if( procedure_id == "114410" ) // i.e. Is_Valid_Field_Type
      retval = "";

   return retval;
}

bool Meta_Specification_Type::get_is_alias( ) const
{
   return false;
}

void Meta_Specification_Type::get_alias_base_info( pair< string, string >& alias_base_info ) const
{
   ( void )alias_base_info;
}

void Meta_Specification_Type::get_base_class_info( vector< pair< string, string > >& base_class_info ) const
{
   ( void )base_class_info;
}

class_base& Meta_Specification_Type::get_or_create_graph_child( const string& context )
{
   class_base* p_class_base( 0 );

   string::size_type pos = context.find( '.' );
   string sub_context( context.substr( 0, pos ) );

   if( sub_context.empty( ) )
      throw runtime_error( "unexpected empty sub-context" );
   else if( ( sub_context == "_301710" ) || ( sub_context == "child_Specification_Type_Child" ) )
      p_class_base = &child_Specification_Type_Child( );
   else if( ( sub_context == "_301406" ) || ( sub_context == "child_Specification_Child" ) )
      p_class_base = &child_Specification_Child( );
   else if( ( sub_context == "_301720" ) || ( sub_context == "child_Specification_Type_Next_Child" ) )
      p_class_base = &child_Specification_Type_Next_Child( );
   else if( ( sub_context == "_301700" ) || ( sub_context == "child_Specification_Type_Parent" ) )
      p_class_base = &child_Specification_Type_Parent( );
   else if( ( sub_context == "_301410" ) || ( sub_context == "child_Specification" ) )
      p_class_base = &child_Specification( );
   else if( ( sub_context == c_field_id_Child_Specification_Type ) || ( sub_context == c_field_name_Child_Specification_Type ) )
      p_class_base = &Child_Specification_Type( );
   else if( ( sub_context == c_field_id_Next_Child_Specification_Type ) || ( sub_context == c_field_name_Next_Child_Specification_Type ) )
      p_class_base = &Next_Child_Specification_Type( );
   else if( ( sub_context == c_field_id_Parent_Specification_Type ) || ( sub_context == c_field_name_Parent_Specification_Type ) )
      p_class_base = &Parent_Specification_Type( );

   if( !p_class_base )
      throw runtime_error( "unknown sub-context '" + sub_context + "'" );

   if( pos != string::npos )
      p_class_base = &p_class_base->get_or_create_graph_child( context.substr( pos + 1 ) );

   return *p_class_base;
}

void Meta_Specification_Type::get_sql_column_names(
 vector< string >& names, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   names.push_back( "C_Allow_Child_Relationship" );
   names.push_back( "C_Allow_Enum" );
   names.push_back( "C_Allow_Enum_Item" );
   names.push_back( "C_Allow_FK_Source_Field_Choice" );
   names.push_back( "C_Allow_FK_Test_Field_Choice" );
   names.push_back( "C_Allow_Field" );
   names.push_back( "C_Allow_Modifier" );
   names.push_back( "C_Allow_Options" );
   names.push_back( "C_Allow_Other_Class" );
   names.push_back( "C_Allow_Other_Class_Field" );
   names.push_back( "C_Allow_Other_Field" );
   names.push_back( "C_Allow_Other_Field_2" );
   names.push_back( "C_Allow_Other_Modifier" );
   names.push_back( "C_Allow_Other_Modifier_2" );
   names.push_back( "C_Allow_Other_Procedure" );
   names.push_back( "C_Allow_Other_Procedure_2" );
   names.push_back( "C_Allow_Other_Source_Child" );
   names.push_back( "C_Allow_Other_Source_Child_2" );
   names.push_back( "C_Allow_Permission" );
   names.push_back( "C_Allow_Procedure" );
   names.push_back( "C_Allow_Procedure_Arg" );
   names.push_back( "C_Allow_Source_Child" );
   names.push_back( "C_Allow_Source_Class" );
   names.push_back( "C_Allow_Source_Field" );
   names.push_back( "C_Allow_Source_Grandchild" );
   names.push_back( "C_Allow_Source_Parent" );
   names.push_back( "C_Allow_Test_Field" );
   names.push_back( "C_Allow_Test_Value" );
   names.push_back( "C_Allow_Value" );
   names.push_back( "C_Can_Access_Below_Model" );
   names.push_back( "C_Child_Prefix" );
   names.push_back( "C_Child_Rel_Grandparent_Match" );
   names.push_back( "C_Child_Rel_Parent_Match" );
   names.push_back( "C_Child_Relationship_Class_Match" );
   names.push_back( "C_Child_Specification_Type" );
   names.push_back( "C_Default_Child_Vars" );
   names.push_back( "C_Field_type" );
   names.push_back( "C_Has_Next_Specification_Info" );
   names.push_back( "C_Is_Child_Only" );
   names.push_back( "C_Is_Required_For_UI_Gen" );
   names.push_back( "C_Is_System" );
   names.push_back( "C_Name" );
   names.push_back( "C_Needs_Child_Relationship" );
   names.push_back( "C_Needs_Class" );
   names.push_back( "C_Needs_Enum" );
   names.push_back( "C_Needs_Enum_Item" );
   names.push_back( "C_Needs_Enum_Item_2" );
   names.push_back( "C_Needs_Enum_Item_3" );
   names.push_back( "C_Needs_Enum_Item_4" );
   names.push_back( "C_Needs_Enum_Item_5" );
   names.push_back( "C_Needs_Field" );
   names.push_back( "C_Needs_Modifier" );
   names.push_back( "C_Needs_Other_Class" );
   names.push_back( "C_Needs_Other_Class_Field" );
   names.push_back( "C_Needs_Other_Field" );
   names.push_back( "C_Needs_Other_Field_2" );
   names.push_back( "C_Needs_Other_Modifier" );
   names.push_back( "C_Needs_Other_Modifier_2" );
   names.push_back( "C_Needs_Other_Permission" );
   names.push_back( "C_Needs_Other_Permission_2" );
   names.push_back( "C_Needs_Other_Procedure" );
   names.push_back( "C_Needs_Other_Procedure_2" );
   names.push_back( "C_Needs_Other_Source_Child" );
   names.push_back( "C_Needs_Other_Source_Child_2" );
   names.push_back( "C_Needs_Permission" );
   names.push_back( "C_Needs_Procedure" );
   names.push_back( "C_Needs_Procedure_Arg" );
   names.push_back( "C_Needs_Procedure_Arg_2" );
   names.push_back( "C_Needs_Procedure_Arg_3" );
   names.push_back( "C_Needs_Source_Child" );
   names.push_back( "C_Needs_Source_Field" );
   names.push_back( "C_Needs_Source_Field_Or_Child" );
   names.push_back( "C_Needs_Source_Parent" );
   names.push_back( "C_Needs_Test_Field" );
   names.push_back( "C_Needs_Test_Value" );
   names.push_back( "C_Needs_Value" );
   names.push_back( "C_Next_Child_Specification_Type" );
   names.push_back( "C_Next_Protect_Child_Rel" );
   names.push_back( "C_Next_Protect_Procedure" );
   names.push_back( "C_Next_Protect_Source_Parent" );
   names.push_back( "C_Next_Specification_Actions" );
   names.push_back( "C_Notes" );
   names.push_back( "C_Parent_Specification_Type" );
   names.push_back( "C_Procedure_Arg_primitive" );
   names.push_back( "C_Procedure_Arg_type" );
   names.push_back( "C_Protect_Child_Rel_From_Update" );
   names.push_back( "C_Protect_Class_From_Edit" );
   names.push_back( "C_Protect_Class_From_Update" );
   names.push_back( "C_Protect_Other_Class_From_Edit" );
   names.push_back( "C_Protect_Procedure_From_Edit" );
   names.push_back( "C_Protect_Source_Parent" );
   names.push_back( "C_Set_Field_To_Source_Field_Or_Child" );
   names.push_back( "C_Source_Field_Needs_Test_Field" );
   names.push_back( "C_Source_Field_Type_Match" );
   names.push_back( "C_Source_Parent_type" );
   names.push_back( "C_Source_type" );
   names.push_back( "C_Specification_Actions" );
   names.push_back( "C_Specification_Name" );
   names.push_back( "C_Specification_Object" );
   names.push_back( "C_Specification_Strings" );
   names.push_back( "C_Specification_Vars" );
   names.push_back( "C_Test_Field_Type_Match" );
   names.push_back( "C_Test_Field_type" );
   names.push_back( "C_Use_Class_As_Source_Parent_Class" );
   names.push_back( "C_Use_Field_Enum" );
   names.push_back( "C_Use_Parent_Child_Rel_As_Source_Parent" );
   names.push_back( "C_Use_Parent_Child_Rel_For_Class" );
   names.push_back( "C_Use_Parent_Child_Rel_For_Other_Class" );
   names.push_back( "C_Use_Parent_Class" );
   names.push_back( "C_Use_Parent_Field_For_Class" );
   names.push_back( "C_Use_Parent_Other_Class" );
   names.push_back( "C_Use_Parent_Other_Class_For_Class" );
   names.push_back( "C_Use_Parent_Procedure" );
   names.push_back( "C_Use_Parent_Source_Class" );
   names.push_back( "C_Use_Parent_Source_Field_For_Class" );
   names.push_back( "C_Use_Parent_Source_Parent" );
   names.push_back( "C_Use_Parent_Source_Parent_For_Class" );
   names.push_back( "C_Use_Source_Field_Enum" );
   names.push_back( "C_Use_Source_Field_Or_Child_Enum" );
   names.push_back( "C_View_Id" );

   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;
}

void Meta_Specification_Type::get_sql_column_values(
 vector< string >& values, bool* p_done, const string* p_class_name ) const
{
   if( p_done && *p_done )
      return;

   values.push_back( to_string( Allow_Child_Relationship( ) ) );
   values.push_back( to_string( Allow_Enum( ) ) );
   values.push_back( to_string( Allow_Enum_Item( ) ) );
   values.push_back( to_string( Allow_FK_Source_Field_Choice( ) ) );
   values.push_back( to_string( Allow_FK_Test_Field_Choice( ) ) );
   values.push_back( to_string( Allow_Field( ) ) );
   values.push_back( to_string( Allow_Modifier( ) ) );
   values.push_back( to_string( Allow_Options( ) ) );
   values.push_back( to_string( Allow_Other_Class( ) ) );
   values.push_back( to_string( Allow_Other_Class_Field( ) ) );
   values.push_back( to_string( Allow_Other_Field( ) ) );
   values.push_back( to_string( Allow_Other_Field_2( ) ) );
   values.push_back( to_string( Allow_Other_Modifier( ) ) );
   values.push_back( to_string( Allow_Other_Modifier_2( ) ) );
   values.push_back( to_string( Allow_Other_Procedure( ) ) );
   values.push_back( to_string( Allow_Other_Procedure_2( ) ) );
   values.push_back( to_string( Allow_Other_Source_Child( ) ) );
   values.push_back( to_string( Allow_Other_Source_Child_2( ) ) );
   values.push_back( to_string( Allow_Permission( ) ) );
   values.push_back( to_string( Allow_Procedure( ) ) );
   values.push_back( to_string( Allow_Procedure_Arg( ) ) );
   values.push_back( to_string( Allow_Source_Child( ) ) );
   values.push_back( to_string( Allow_Source_Class( ) ) );
   values.push_back( to_string( Allow_Source_Field( ) ) );
   values.push_back( to_string( Allow_Source_Grandchild( ) ) );
   values.push_back( to_string( Allow_Source_Parent( ) ) );
   values.push_back( to_string( Allow_Test_Field( ) ) );
   values.push_back( to_string( Allow_Test_Value( ) ) );
   values.push_back( to_string( Allow_Value( ) ) );
   values.push_back( to_string( Can_Access_Below_Model( ) ) );
   values.push_back( sql_quote( to_string( Child_Prefix( ) ) ) );
   values.push_back( to_string( Child_Rel_Grandparent_Match( ) ) );
   values.push_back( to_string( Child_Rel_Parent_Match( ) ) );
   values.push_back( to_string( Child_Relationship_Class_Match( ) ) );
   values.push_back( sql_quote( to_string( Child_Specification_Type( ) ) ) );
   values.push_back( sql_quote( to_string( Default_Child_Vars( ) ) ) );
   values.push_back( to_string( Field_type( ) ) );
   values.push_back( to_string( Has_Next_Specification_Info( ) ) );
   values.push_back( to_string( Is_Child_Only( ) ) );
   values.push_back( to_string( Is_Required_For_UI_Gen( ) ) );
   values.push_back( to_string( Is_System( ) ) );
   values.push_back( sql_quote( to_string( Name( ) ) ) );
   values.push_back( to_string( Needs_Child_Relationship( ) ) );
   values.push_back( to_string( Needs_Class( ) ) );
   values.push_back( to_string( Needs_Enum( ) ) );
   values.push_back( to_string( Needs_Enum_Item( ) ) );
   values.push_back( to_string( Needs_Enum_Item_2( ) ) );
   values.push_back( to_string( Needs_Enum_Item_3( ) ) );
   values.push_back( to_string( Needs_Enum_Item_4( ) ) );
   values.push_back( to_string( Needs_Enum_Item_5( ) ) );
   values.push_back( to_string( Needs_Field( ) ) );
   values.push_back( to_string( Needs_Modifier( ) ) );
   values.push_back( to_string( Needs_Other_Class( ) ) );
   values.push_back( to_string( Needs_Other_Class_Field( ) ) );
   values.push_back( to_string( Needs_Other_Field( ) ) );
   values.push_back( to_string( Needs_Other_Field_2( ) ) );
   values.push_back( to_string( Needs_Other_Modifier( ) ) );
   values.push_back( to_string( Needs_Other_Modifier_2( ) ) );
   values.push_back( to_string( Needs_Other_Permission( ) ) );
   values.push_back( to_string( Needs_Other_Permission_2( ) ) );
   values.push_back( to_string( Needs_Other_Procedure( ) ) );
   values.push_back( to_string( Needs_Other_Procedure_2( ) ) );
   values.push_back( to_string( Needs_Other_Source_Child( ) ) );
   values.push_back( to_string( Needs_Other_Source_Child_2( ) ) );
   values.push_back( to_string( Needs_Permission( ) ) );
   values.push_back( to_string( Needs_Procedure( ) ) );
   values.push_back( to_string( Needs_Procedure_Arg( ) ) );
   values.push_back( to_string( Needs_Procedure_Arg_2( ) ) );
   values.push_back( to_string( Needs_Procedure_Arg_3( ) ) );
   values.push_back( to_string( Needs_Source_Child( ) ) );
   values.push_back( to_string( Needs_Source_Field( ) ) );
   values.push_back( to_string( Needs_Source_Field_Or_Child( ) ) );
   values.push_back( to_string( Needs_Source_Parent( ) ) );
   values.push_back( to_string( Needs_Test_Field( ) ) );
   values.push_back( to_string( Needs_Test_Value( ) ) );
   values.push_back( to_string( Needs_Value( ) ) );
   values.push_back( sql_quote( to_string( Next_Child_Specification_Type( ) ) ) );
   values.push_back( to_string( Next_Protect_Child_Rel( ) ) );
   values.push_back( to_string( Next_Protect_Procedure( ) ) );
   values.push_back( to_string( Next_Protect_Source_Parent( ) ) );
   values.push_back( sql_quote( to_string( Next_Specification_Actions( ) ) ) );
   values.push_back( sql_quote( to_string( Notes( ) ) ) );
   values.push_back( sql_quote( to_string( Parent_Specification_Type( ) ) ) );
   values.push_back( to_string( Procedure_Arg_primitive( ) ) );
   values.push_back( to_string( Procedure_Arg_type( ) ) );
   values.push_back( to_string( Protect_Child_Rel_From_Update( ) ) );
   values.push_back( to_string( Protect_Class_From_Edit( ) ) );
   values.push_back( to_string( Protect_Class_From_Update( ) ) );
   values.push_back( to_string( Protect_Other_Class_From_Edit( ) ) );
   values.push_back( to_string( Protect_Procedure_From_Edit( ) ) );
   values.push_back( to_string( Protect_Source_Parent( ) ) );
   values.push_back( to_string( Set_Field_To_Source_Field_Or_Child( ) ) );
   values.push_back( to_string( Source_Field_Needs_Test_Field( ) ) );
   values.push_back( to_string( Source_Field_Type_Match( ) ) );
   values.push_back( to_string( Source_Parent_type( ) ) );
   values.push_back( to_string( Source_type( ) ) );
   values.push_back( sql_quote( to_string( Specification_Actions( ) ) ) );
   values.push_back( sql_quote( to_string( Specification_Name( ) ) ) );
   values.push_back( sql_quote( to_string( Specification_Object( ) ) ) );
   values.push_back( sql_quote( to_string( Specification_Strings( ) ) ) );
   values.push_back( sql_quote( to_string( Specification_Vars( ) ) ) );
   values.push_back( to_string( Test_Field_Type_Match( ) ) );
   values.push_back( to_string( Test_Field_type( ) ) );
   values.push_back( to_string( Use_Class_As_Source_Parent_Class( ) ) );
   values.push_back( to_string( Use_Field_Enum( ) ) );
   values.push_back( to_string( Use_Parent_Child_Rel_As_Source_Parent( ) ) );
   values.push_back( to_string( Use_Parent_Child_Rel_For_Class( ) ) );
   values.push_back( to_string( Use_Parent_Child_Rel_For_Other_Class( ) ) );
   values.push_back( to_string( Use_Parent_Class( ) ) );
   values.push_back( to_string( Use_Parent_Field_For_Class( ) ) );
   values.push_back( to_string( Use_Parent_Other_Class( ) ) );
   values.push_back( to_string( Use_Parent_Other_Class_For_Class( ) ) );
   values.push_back( to_string( Use_Parent_Procedure( ) ) );
   values.push_back( to_string( Use_Parent_Source_Class( ) ) );
   values.push_back( to_string( Use_Parent_Source_Field_For_Class( ) ) );
   values.push_back( to_string( Use_Parent_Source_Parent( ) ) );
   values.push_back( to_string( Use_Parent_Source_Parent_For_Class( ) ) );
   values.push_back( to_string( Use_Source_Field_Enum( ) ) );
   values.push_back( to_string( Use_Source_Field_Or_Child_Enum( ) ) );
   values.push_back( sql_quote( to_string( View_Id( ) ) ) );

   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;
}

void Meta_Specification_Type::get_required_field_names(
 set< string >& names, bool use_transients, set< string >* p_dependents ) const
{
   set< string > local_dependents;
   set< string >& dependents( p_dependents ? *p_dependents : local_dependents );

   get_always_required_field_names( names, use_transients, dependents );

   // [<start get_required_field_names>]
   // [<finish get_required_field_names>]
}

void Meta_Specification_Type::get_always_required_field_names(
 set< string >& names, bool use_transients, set< string >& dependents ) const
{
   ( void )names;
   ( void )dependents;
   ( void )use_transients;

   // [<start get_always_required_field_names>]
   // [<finish get_always_required_field_names>]
}

void Meta_Specification_Type::get_transient_replacement_field_names( const string& name, vector< string >& names ) const
{
   ( void )name;
   ( void )names;

   // [<start get_transient_replacement_field_names>]
   // [<finish get_transient_replacement_field_names>]
}

void Meta_Specification_Type::do_generate_sql( generate_sql_type type,
 vector< string >& sql_stmts, set< string >& tx_key_info, vector< string >* p_sql_undo_stmts ) const
{
   generate_sql( static_class_name( ), type, sql_stmts, tx_key_info, p_sql_undo_stmts );
}

const char* Meta_Specification_Type::static_resolved_module_id( )
{
   return static_module_id( );
}

const char* Meta_Specification_Type::static_resolved_module_name( )
{
   return static_module_name( );
}

const char* Meta_Specification_Type::static_lock_class_id( )
{
   return "114100";
}

const char* Meta_Specification_Type::static_check_class_name( )
{
   return "Specification_Type";
}

const char* Meta_Specification_Type::static_persistence_extra( )
{
   return "";
}

bool Meta_Specification_Type::static_has_derivations( )
{
   return !g_derivations.empty( );
}

void Meta_Specification_Type::static_get_class_info( class_info_container& class_info )
{
   class_info.push_back( "100.114100" );
}

void Meta_Specification_Type::static_get_field_info( field_info_container& all_field_info )
{
   all_field_info.push_back( field_info( "114191", "Allow_Child_Relationship", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114198", "Allow_Enum", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114199", "Allow_Enum_Item", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114123", "Allow_FK_Source_Field_Choice", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114142", "Allow_FK_Test_Field_Choice", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114124", "Allow_Field", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114197", "Allow_Modifier", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114117", "Allow_Options", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114192", "Allow_Other_Class", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114212", "Allow_Other_Class_Field", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114174", "Allow_Other_Field", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114183", "Allow_Other_Field_2", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114216", "Allow_Other_Modifier", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114217", "Allow_Other_Modifier_2", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114208", "Allow_Other_Procedure", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114209", "Allow_Other_Procedure_2", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114176", "Allow_Other_Source_Child", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114205", "Allow_Other_Source_Child_2", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114136", "Allow_Permission", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114171", "Allow_Procedure", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114204", "Allow_Procedure_Arg", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114121", "Allow_Source_Child", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114172", "Allow_Source_Class", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114128", "Allow_Source_Field", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114178", "Allow_Source_Grandchild", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114125", "Allow_Source_Parent", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114126", "Allow_Test_Field", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114115", "Allow_Test_Value", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114135", "Allow_Value", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114218", "Can_Access_Below_Model", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114164", "Child_Prefix", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "114157", "Child_Rel_Grandparent_Match", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114156", "Child_Rel_Parent_Match", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114155", "Child_Relationship_Class_Match", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "301710", "Child_Specification_Type", "Meta_Specification_Type", false, "", "" ) );
   all_field_info.push_back( field_info( "114165", "Default_Child_Vars", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "114111", "Field_type", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "114149", "Has_Next_Specification_Info", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114151", "Is_Child_Only", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114163", "Is_Required_For_UI_Gen", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114147", "Is_System", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114101", "Name", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "114138", "Needs_Child_Relationship", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114104", "Needs_Class", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114108", "Needs_Enum", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114109", "Needs_Enum_Item", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114182", "Needs_Enum_Item_2", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114185", "Needs_Enum_Item_3", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114186", "Needs_Enum_Item_4", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114189", "Needs_Enum_Item_5", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114105", "Needs_Field", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114107", "Needs_Modifier", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114193", "Needs_Other_Class", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114213", "Needs_Other_Class_Field", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114175", "Needs_Other_Field", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114184", "Needs_Other_Field_2", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114202", "Needs_Other_Modifier", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114203", "Needs_Other_Modifier_2", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114200", "Needs_Other_Permission", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114201", "Needs_Other_Permission_2", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114187", "Needs_Other_Procedure", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114188", "Needs_Other_Procedure_2", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114177", "Needs_Other_Source_Child", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114206", "Needs_Other_Source_Child_2", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114180", "Needs_Permission", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114106", "Needs_Procedure", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114132", "Needs_Procedure_Arg", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114194", "Needs_Procedure_Arg_2", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114195", "Needs_Procedure_Arg_3", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114122", "Needs_Source_Child", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114113", "Needs_Source_Field", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114154", "Needs_Source_Field_Or_Child", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114112", "Needs_Source_Parent", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114114", "Needs_Test_Field", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114116", "Needs_Test_Value", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114110", "Needs_Value", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "301720", "Next_Child_Specification_Type", "Meta_Specification_Type", false, "", "" ) );
   all_field_info.push_back( field_info( "114169", "Next_Protect_Child_Rel", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114161", "Next_Protect_Procedure", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114153", "Next_Protect_Source_Parent", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114150", "Next_Specification_Actions", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "114120", "Notes", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "301700", "Parent_Specification_Type", "Meta_Specification_Type", false, "", "" ) );
   all_field_info.push_back( field_info( "114133", "Procedure_Arg_primitive", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "114134", "Procedure_Arg_type", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "114145", "Protect_Child_Rel_From_Update", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114146", "Protect_Class_From_Edit", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114141", "Protect_Class_From_Update", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114214", "Protect_Other_Class_From_Edit", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114160", "Protect_Procedure_From_Edit", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114137", "Protect_Source_Parent", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114179", "Set_Field_To_Source_Field_Or_Child", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114129", "Source_Field_Needs_Test_Field", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114118", "Source_Field_Type_Match", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114119", "Source_Parent_type", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "114130", "Source_type", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "114140", "Specification_Actions", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "114102", "Specification_Name", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "114103", "Specification_Object", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "114166", "Specification_Strings", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "114158", "Specification_Vars", "string", false, "", "" ) );
   all_field_info.push_back( field_info( "114127", "Test_Field_Type_Match", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114131", "Test_Field_type", "int", false, "", "" ) );
   all_field_info.push_back( field_info( "114181", "Use_Class_As_Source_Parent_Class", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114162", "Use_Field_Enum", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114168", "Use_Parent_Child_Rel_As_Source_Parent", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114148", "Use_Parent_Child_Rel_For_Class", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114210", "Use_Parent_Child_Rel_For_Other_Class", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114139", "Use_Parent_Class", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114167", "Use_Parent_Field_For_Class", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114211", "Use_Parent_Other_Class", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114196", "Use_Parent_Other_Class_For_Class", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114159", "Use_Parent_Procedure", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114173", "Use_Parent_Source_Class", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114190", "Use_Parent_Source_Field_For_Class", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114144", "Use_Parent_Source_Parent", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114152", "Use_Parent_Source_Parent_For_Class", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114207", "Use_Source_Field_Enum", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114215", "Use_Source_Field_Or_Child_Enum", "bool", false, "", "" ) );
   all_field_info.push_back( field_info( "114170", "View_Id", "string", false, "", "" ) );
}

void Meta_Specification_Type::static_get_foreign_key_info( foreign_key_info_container& foreign_key_info )
{
   ( void )foreign_key_info;

   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Child_Specification_Type, make_pair( "Meta.114100", "Meta_Specification_Type" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Next_Child_Specification_Type, make_pair( "Meta.114100", "Meta_Specification_Type" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Parent_Specification_Type, make_pair( "Meta.114100", "Meta_Specification_Type" ) ) );
}

int Meta_Specification_Type::static_get_num_fields( bool* p_done, const string* p_class_name )
{
   if( p_done && p_class_name && ( *p_class_name == static_class_name( ) ) )
      *p_done = true;

   return c_num_fields;
}

bool Meta_Specification_Type::static_is_field_encrypted( field_id id )
{
   return is_encrypted_field( static_get_field_id( id ) );
}

bool Meta_Specification_Type::static_is_field_transient( field_id id )
{
   return is_transient_field( static_get_field_id( id ) );
}

const char* Meta_Specification_Type::static_get_field_id( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "114191";
      break;

      case 2:
      p_id = "114198";
      break;

      case 3:
      p_id = "114199";
      break;

      case 4:
      p_id = "114123";
      break;

      case 5:
      p_id = "114142";
      break;

      case 6:
      p_id = "114124";
      break;

      case 7:
      p_id = "114197";
      break;

      case 8:
      p_id = "114117";
      break;

      case 9:
      p_id = "114192";
      break;

      case 10:
      p_id = "114212";
      break;

      case 11:
      p_id = "114174";
      break;

      case 12:
      p_id = "114183";
      break;

      case 13:
      p_id = "114216";
      break;

      case 14:
      p_id = "114217";
      break;

      case 15:
      p_id = "114208";
      break;

      case 16:
      p_id = "114209";
      break;

      case 17:
      p_id = "114176";
      break;

      case 18:
      p_id = "114205";
      break;

      case 19:
      p_id = "114136";
      break;

      case 20:
      p_id = "114171";
      break;

      case 21:
      p_id = "114204";
      break;

      case 22:
      p_id = "114121";
      break;

      case 23:
      p_id = "114172";
      break;

      case 24:
      p_id = "114128";
      break;

      case 25:
      p_id = "114178";
      break;

      case 26:
      p_id = "114125";
      break;

      case 27:
      p_id = "114126";
      break;

      case 28:
      p_id = "114115";
      break;

      case 29:
      p_id = "114135";
      break;

      case 30:
      p_id = "114218";
      break;

      case 31:
      p_id = "114164";
      break;

      case 32:
      p_id = "114157";
      break;

      case 33:
      p_id = "114156";
      break;

      case 34:
      p_id = "114155";
      break;

      case 35:
      p_id = "301710";
      break;

      case 36:
      p_id = "114165";
      break;

      case 37:
      p_id = "114111";
      break;

      case 38:
      p_id = "114149";
      break;

      case 39:
      p_id = "114151";
      break;

      case 40:
      p_id = "114163";
      break;

      case 41:
      p_id = "114147";
      break;

      case 42:
      p_id = "114101";
      break;

      case 43:
      p_id = "114138";
      break;

      case 44:
      p_id = "114104";
      break;

      case 45:
      p_id = "114108";
      break;

      case 46:
      p_id = "114109";
      break;

      case 47:
      p_id = "114182";
      break;

      case 48:
      p_id = "114185";
      break;

      case 49:
      p_id = "114186";
      break;

      case 50:
      p_id = "114189";
      break;

      case 51:
      p_id = "114105";
      break;

      case 52:
      p_id = "114107";
      break;

      case 53:
      p_id = "114193";
      break;

      case 54:
      p_id = "114213";
      break;

      case 55:
      p_id = "114175";
      break;

      case 56:
      p_id = "114184";
      break;

      case 57:
      p_id = "114202";
      break;

      case 58:
      p_id = "114203";
      break;

      case 59:
      p_id = "114200";
      break;

      case 60:
      p_id = "114201";
      break;

      case 61:
      p_id = "114187";
      break;

      case 62:
      p_id = "114188";
      break;

      case 63:
      p_id = "114177";
      break;

      case 64:
      p_id = "114206";
      break;

      case 65:
      p_id = "114180";
      break;

      case 66:
      p_id = "114106";
      break;

      case 67:
      p_id = "114132";
      break;

      case 68:
      p_id = "114194";
      break;

      case 69:
      p_id = "114195";
      break;

      case 70:
      p_id = "114122";
      break;

      case 71:
      p_id = "114113";
      break;

      case 72:
      p_id = "114154";
      break;

      case 73:
      p_id = "114112";
      break;

      case 74:
      p_id = "114114";
      break;

      case 75:
      p_id = "114116";
      break;

      case 76:
      p_id = "114110";
      break;

      case 77:
      p_id = "301720";
      break;

      case 78:
      p_id = "114169";
      break;

      case 79:
      p_id = "114161";
      break;

      case 80:
      p_id = "114153";
      break;

      case 81:
      p_id = "114150";
      break;

      case 82:
      p_id = "114120";
      break;

      case 83:
      p_id = "301700";
      break;

      case 84:
      p_id = "114133";
      break;

      case 85:
      p_id = "114134";
      break;

      case 86:
      p_id = "114145";
      break;

      case 87:
      p_id = "114146";
      break;

      case 88:
      p_id = "114141";
      break;

      case 89:
      p_id = "114214";
      break;

      case 90:
      p_id = "114160";
      break;

      case 91:
      p_id = "114137";
      break;

      case 92:
      p_id = "114179";
      break;

      case 93:
      p_id = "114129";
      break;

      case 94:
      p_id = "114118";
      break;

      case 95:
      p_id = "114119";
      break;

      case 96:
      p_id = "114130";
      break;

      case 97:
      p_id = "114140";
      break;

      case 98:
      p_id = "114102";
      break;

      case 99:
      p_id = "114103";
      break;

      case 100:
      p_id = "114166";
      break;

      case 101:
      p_id = "114158";
      break;

      case 102:
      p_id = "114127";
      break;

      case 103:
      p_id = "114131";
      break;

      case 104:
      p_id = "114181";
      break;

      case 105:
      p_id = "114162";
      break;

      case 106:
      p_id = "114168";
      break;

      case 107:
      p_id = "114148";
      break;

      case 108:
      p_id = "114210";
      break;

      case 109:
      p_id = "114139";
      break;

      case 110:
      p_id = "114167";
      break;

      case 111:
      p_id = "114211";
      break;

      case 112:
      p_id = "114196";
      break;

      case 113:
      p_id = "114159";
      break;

      case 114:
      p_id = "114173";
      break;

      case 115:
      p_id = "114190";
      break;

      case 116:
      p_id = "114144";
      break;

      case 117:
      p_id = "114152";
      break;

      case 118:
      p_id = "114207";
      break;

      case 119:
      p_id = "114215";
      break;

      case 120:
      p_id = "114170";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Specification_Type" );

   return p_id;
}

const char* Meta_Specification_Type::static_get_field_name( field_id id )
{
   const char* p_id = 0;

   switch( id )
   {
      case 1:
      p_id = "Allow_Child_Relationship";
      break;

      case 2:
      p_id = "Allow_Enum";
      break;

      case 3:
      p_id = "Allow_Enum_Item";
      break;

      case 4:
      p_id = "Allow_FK_Source_Field_Choice";
      break;

      case 5:
      p_id = "Allow_FK_Test_Field_Choice";
      break;

      case 6:
      p_id = "Allow_Field";
      break;

      case 7:
      p_id = "Allow_Modifier";
      break;

      case 8:
      p_id = "Allow_Options";
      break;

      case 9:
      p_id = "Allow_Other_Class";
      break;

      case 10:
      p_id = "Allow_Other_Class_Field";
      break;

      case 11:
      p_id = "Allow_Other_Field";
      break;

      case 12:
      p_id = "Allow_Other_Field_2";
      break;

      case 13:
      p_id = "Allow_Other_Modifier";
      break;

      case 14:
      p_id = "Allow_Other_Modifier_2";
      break;

      case 15:
      p_id = "Allow_Other_Procedure";
      break;

      case 16:
      p_id = "Allow_Other_Procedure_2";
      break;

      case 17:
      p_id = "Allow_Other_Source_Child";
      break;

      case 18:
      p_id = "Allow_Other_Source_Child_2";
      break;

      case 19:
      p_id = "Allow_Permission";
      break;

      case 20:
      p_id = "Allow_Procedure";
      break;

      case 21:
      p_id = "Allow_Procedure_Arg";
      break;

      case 22:
      p_id = "Allow_Source_Child";
      break;

      case 23:
      p_id = "Allow_Source_Class";
      break;

      case 24:
      p_id = "Allow_Source_Field";
      break;

      case 25:
      p_id = "Allow_Source_Grandchild";
      break;

      case 26:
      p_id = "Allow_Source_Parent";
      break;

      case 27:
      p_id = "Allow_Test_Field";
      break;

      case 28:
      p_id = "Allow_Test_Value";
      break;

      case 29:
      p_id = "Allow_Value";
      break;

      case 30:
      p_id = "Can_Access_Below_Model";
      break;

      case 31:
      p_id = "Child_Prefix";
      break;

      case 32:
      p_id = "Child_Rel_Grandparent_Match";
      break;

      case 33:
      p_id = "Child_Rel_Parent_Match";
      break;

      case 34:
      p_id = "Child_Relationship_Class_Match";
      break;

      case 35:
      p_id = "Child_Specification_Type";
      break;

      case 36:
      p_id = "Default_Child_Vars";
      break;

      case 37:
      p_id = "Field_type";
      break;

      case 38:
      p_id = "Has_Next_Specification_Info";
      break;

      case 39:
      p_id = "Is_Child_Only";
      break;

      case 40:
      p_id = "Is_Required_For_UI_Gen";
      break;

      case 41:
      p_id = "Is_System";
      break;

      case 42:
      p_id = "Name";
      break;

      case 43:
      p_id = "Needs_Child_Relationship";
      break;

      case 44:
      p_id = "Needs_Class";
      break;

      case 45:
      p_id = "Needs_Enum";
      break;

      case 46:
      p_id = "Needs_Enum_Item";
      break;

      case 47:
      p_id = "Needs_Enum_Item_2";
      break;

      case 48:
      p_id = "Needs_Enum_Item_3";
      break;

      case 49:
      p_id = "Needs_Enum_Item_4";
      break;

      case 50:
      p_id = "Needs_Enum_Item_5";
      break;

      case 51:
      p_id = "Needs_Field";
      break;

      case 52:
      p_id = "Needs_Modifier";
      break;

      case 53:
      p_id = "Needs_Other_Class";
      break;

      case 54:
      p_id = "Needs_Other_Class_Field";
      break;

      case 55:
      p_id = "Needs_Other_Field";
      break;

      case 56:
      p_id = "Needs_Other_Field_2";
      break;

      case 57:
      p_id = "Needs_Other_Modifier";
      break;

      case 58:
      p_id = "Needs_Other_Modifier_2";
      break;

      case 59:
      p_id = "Needs_Other_Permission";
      break;

      case 60:
      p_id = "Needs_Other_Permission_2";
      break;

      case 61:
      p_id = "Needs_Other_Procedure";
      break;

      case 62:
      p_id = "Needs_Other_Procedure_2";
      break;

      case 63:
      p_id = "Needs_Other_Source_Child";
      break;

      case 64:
      p_id = "Needs_Other_Source_Child_2";
      break;

      case 65:
      p_id = "Needs_Permission";
      break;

      case 66:
      p_id = "Needs_Procedure";
      break;

      case 67:
      p_id = "Needs_Procedure_Arg";
      break;

      case 68:
      p_id = "Needs_Procedure_Arg_2";
      break;

      case 69:
      p_id = "Needs_Procedure_Arg_3";
      break;

      case 70:
      p_id = "Needs_Source_Child";
      break;

      case 71:
      p_id = "Needs_Source_Field";
      break;

      case 72:
      p_id = "Needs_Source_Field_Or_Child";
      break;

      case 73:
      p_id = "Needs_Source_Parent";
      break;

      case 74:
      p_id = "Needs_Test_Field";
      break;

      case 75:
      p_id = "Needs_Test_Value";
      break;

      case 76:
      p_id = "Needs_Value";
      break;

      case 77:
      p_id = "Next_Child_Specification_Type";
      break;

      case 78:
      p_id = "Next_Protect_Child_Rel";
      break;

      case 79:
      p_id = "Next_Protect_Procedure";
      break;

      case 80:
      p_id = "Next_Protect_Source_Parent";
      break;

      case 81:
      p_id = "Next_Specification_Actions";
      break;

      case 82:
      p_id = "Notes";
      break;

      case 83:
      p_id = "Parent_Specification_Type";
      break;

      case 84:
      p_id = "Procedure_Arg_primitive";
      break;

      case 85:
      p_id = "Procedure_Arg_type";
      break;

      case 86:
      p_id = "Protect_Child_Rel_From_Update";
      break;

      case 87:
      p_id = "Protect_Class_From_Edit";
      break;

      case 88:
      p_id = "Protect_Class_From_Update";
      break;

      case 89:
      p_id = "Protect_Other_Class_From_Edit";
      break;

      case 90:
      p_id = "Protect_Procedure_From_Edit";
      break;

      case 91:
      p_id = "Protect_Source_Parent";
      break;

      case 92:
      p_id = "Set_Field_To_Source_Field_Or_Child";
      break;

      case 93:
      p_id = "Source_Field_Needs_Test_Field";
      break;

      case 94:
      p_id = "Source_Field_Type_Match";
      break;

      case 95:
      p_id = "Source_Parent_type";
      break;

      case 96:
      p_id = "Source_type";
      break;

      case 97:
      p_id = "Specification_Actions";
      break;

      case 98:
      p_id = "Specification_Name";
      break;

      case 99:
      p_id = "Specification_Object";
      break;

      case 100:
      p_id = "Specification_Strings";
      break;

      case 101:
      p_id = "Specification_Vars";
      break;

      case 102:
      p_id = "Test_Field_Type_Match";
      break;

      case 103:
      p_id = "Test_Field_type";
      break;

      case 104:
      p_id = "Use_Class_As_Source_Parent_Class";
      break;

      case 105:
      p_id = "Use_Field_Enum";
      break;

      case 106:
      p_id = "Use_Parent_Child_Rel_As_Source_Parent";
      break;

      case 107:
      p_id = "Use_Parent_Child_Rel_For_Class";
      break;

      case 108:
      p_id = "Use_Parent_Child_Rel_For_Other_Class";
      break;

      case 109:
      p_id = "Use_Parent_Class";
      break;

      case 110:
      p_id = "Use_Parent_Field_For_Class";
      break;

      case 111:
      p_id = "Use_Parent_Other_Class";
      break;

      case 112:
      p_id = "Use_Parent_Other_Class_For_Class";
      break;

      case 113:
      p_id = "Use_Parent_Procedure";
      break;

      case 114:
      p_id = "Use_Parent_Source_Class";
      break;

      case 115:
      p_id = "Use_Parent_Source_Field_For_Class";
      break;

      case 116:
      p_id = "Use_Parent_Source_Parent";
      break;

      case 117:
      p_id = "Use_Parent_Source_Parent_For_Class";
      break;

      case 118:
      p_id = "Use_Source_Field_Enum";
      break;

      case 119:
      p_id = "Use_Source_Field_Or_Child_Enum";
      break;

      case 120:
      p_id = "View_Id";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Specification_Type" );

   return p_id;
}

int Meta_Specification_Type::static_get_field_num( const string& field )
{
   int rc = 0;

   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for static_get_field_num( )" );
   else if( ( field == c_field_id_Allow_Child_Relationship ) || ( field == c_field_name_Allow_Child_Relationship ) )
      rc += 1;
   else if( ( field == c_field_id_Allow_Enum ) || ( field == c_field_name_Allow_Enum ) )
      rc += 2;
   else if( ( field == c_field_id_Allow_Enum_Item ) || ( field == c_field_name_Allow_Enum_Item ) )
      rc += 3;
   else if( ( field == c_field_id_Allow_FK_Source_Field_Choice ) || ( field == c_field_name_Allow_FK_Source_Field_Choice ) )
      rc += 4;
   else if( ( field == c_field_id_Allow_FK_Test_Field_Choice ) || ( field == c_field_name_Allow_FK_Test_Field_Choice ) )
      rc += 5;
   else if( ( field == c_field_id_Allow_Field ) || ( field == c_field_name_Allow_Field ) )
      rc += 6;
   else if( ( field == c_field_id_Allow_Modifier ) || ( field == c_field_name_Allow_Modifier ) )
      rc += 7;
   else if( ( field == c_field_id_Allow_Options ) || ( field == c_field_name_Allow_Options ) )
      rc += 8;
   else if( ( field == c_field_id_Allow_Other_Class ) || ( field == c_field_name_Allow_Other_Class ) )
      rc += 9;
   else if( ( field == c_field_id_Allow_Other_Class_Field ) || ( field == c_field_name_Allow_Other_Class_Field ) )
      rc += 10;
   else if( ( field == c_field_id_Allow_Other_Field ) || ( field == c_field_name_Allow_Other_Field ) )
      rc += 11;
   else if( ( field == c_field_id_Allow_Other_Field_2 ) || ( field == c_field_name_Allow_Other_Field_2 ) )
      rc += 12;
   else if( ( field == c_field_id_Allow_Other_Modifier ) || ( field == c_field_name_Allow_Other_Modifier ) )
      rc += 13;
   else if( ( field == c_field_id_Allow_Other_Modifier_2 ) || ( field == c_field_name_Allow_Other_Modifier_2 ) )
      rc += 14;
   else if( ( field == c_field_id_Allow_Other_Procedure ) || ( field == c_field_name_Allow_Other_Procedure ) )
      rc += 15;
   else if( ( field == c_field_id_Allow_Other_Procedure_2 ) || ( field == c_field_name_Allow_Other_Procedure_2 ) )
      rc += 16;
   else if( ( field == c_field_id_Allow_Other_Source_Child ) || ( field == c_field_name_Allow_Other_Source_Child ) )
      rc += 17;
   else if( ( field == c_field_id_Allow_Other_Source_Child_2 ) || ( field == c_field_name_Allow_Other_Source_Child_2 ) )
      rc += 18;
   else if( ( field == c_field_id_Allow_Permission ) || ( field == c_field_name_Allow_Permission ) )
      rc += 19;
   else if( ( field == c_field_id_Allow_Procedure ) || ( field == c_field_name_Allow_Procedure ) )
      rc += 20;
   else if( ( field == c_field_id_Allow_Procedure_Arg ) || ( field == c_field_name_Allow_Procedure_Arg ) )
      rc += 21;
   else if( ( field == c_field_id_Allow_Source_Child ) || ( field == c_field_name_Allow_Source_Child ) )
      rc += 22;
   else if( ( field == c_field_id_Allow_Source_Class ) || ( field == c_field_name_Allow_Source_Class ) )
      rc += 23;
   else if( ( field == c_field_id_Allow_Source_Field ) || ( field == c_field_name_Allow_Source_Field ) )
      rc += 24;
   else if( ( field == c_field_id_Allow_Source_Grandchild ) || ( field == c_field_name_Allow_Source_Grandchild ) )
      rc += 25;
   else if( ( field == c_field_id_Allow_Source_Parent ) || ( field == c_field_name_Allow_Source_Parent ) )
      rc += 26;
   else if( ( field == c_field_id_Allow_Test_Field ) || ( field == c_field_name_Allow_Test_Field ) )
      rc += 27;
   else if( ( field == c_field_id_Allow_Test_Value ) || ( field == c_field_name_Allow_Test_Value ) )
      rc += 28;
   else if( ( field == c_field_id_Allow_Value ) || ( field == c_field_name_Allow_Value ) )
      rc += 29;
   else if( ( field == c_field_id_Can_Access_Below_Model ) || ( field == c_field_name_Can_Access_Below_Model ) )
      rc += 30;
   else if( ( field == c_field_id_Child_Prefix ) || ( field == c_field_name_Child_Prefix ) )
      rc += 31;
   else if( ( field == c_field_id_Child_Rel_Grandparent_Match ) || ( field == c_field_name_Child_Rel_Grandparent_Match ) )
      rc += 32;
   else if( ( field == c_field_id_Child_Rel_Parent_Match ) || ( field == c_field_name_Child_Rel_Parent_Match ) )
      rc += 33;
   else if( ( field == c_field_id_Child_Relationship_Class_Match ) || ( field == c_field_name_Child_Relationship_Class_Match ) )
      rc += 34;
   else if( ( field == c_field_id_Child_Specification_Type ) || ( field == c_field_name_Child_Specification_Type ) )
      rc += 35;
   else if( ( field == c_field_id_Default_Child_Vars ) || ( field == c_field_name_Default_Child_Vars ) )
      rc += 36;
   else if( ( field == c_field_id_Field_type ) || ( field == c_field_name_Field_type ) )
      rc += 37;
   else if( ( field == c_field_id_Has_Next_Specification_Info ) || ( field == c_field_name_Has_Next_Specification_Info ) )
      rc += 38;
   else if( ( field == c_field_id_Is_Child_Only ) || ( field == c_field_name_Is_Child_Only ) )
      rc += 39;
   else if( ( field == c_field_id_Is_Required_For_UI_Gen ) || ( field == c_field_name_Is_Required_For_UI_Gen ) )
      rc += 40;
   else if( ( field == c_field_id_Is_System ) || ( field == c_field_name_Is_System ) )
      rc += 41;
   else if( ( field == c_field_id_Name ) || ( field == c_field_name_Name ) )
      rc += 42;
   else if( ( field == c_field_id_Needs_Child_Relationship ) || ( field == c_field_name_Needs_Child_Relationship ) )
      rc += 43;
   else if( ( field == c_field_id_Needs_Class ) || ( field == c_field_name_Needs_Class ) )
      rc += 44;
   else if( ( field == c_field_id_Needs_Enum ) || ( field == c_field_name_Needs_Enum ) )
      rc += 45;
   else if( ( field == c_field_id_Needs_Enum_Item ) || ( field == c_field_name_Needs_Enum_Item ) )
      rc += 46;
   else if( ( field == c_field_id_Needs_Enum_Item_2 ) || ( field == c_field_name_Needs_Enum_Item_2 ) )
      rc += 47;
   else if( ( field == c_field_id_Needs_Enum_Item_3 ) || ( field == c_field_name_Needs_Enum_Item_3 ) )
      rc += 48;
   else if( ( field == c_field_id_Needs_Enum_Item_4 ) || ( field == c_field_name_Needs_Enum_Item_4 ) )
      rc += 49;
   else if( ( field == c_field_id_Needs_Enum_Item_5 ) || ( field == c_field_name_Needs_Enum_Item_5 ) )
      rc += 50;
   else if( ( field == c_field_id_Needs_Field ) || ( field == c_field_name_Needs_Field ) )
      rc += 51;
   else if( ( field == c_field_id_Needs_Modifier ) || ( field == c_field_name_Needs_Modifier ) )
      rc += 52;
   else if( ( field == c_field_id_Needs_Other_Class ) || ( field == c_field_name_Needs_Other_Class ) )
      rc += 53;
   else if( ( field == c_field_id_Needs_Other_Class_Field ) || ( field == c_field_name_Needs_Other_Class_Field ) )
      rc += 54;
   else if( ( field == c_field_id_Needs_Other_Field ) || ( field == c_field_name_Needs_Other_Field ) )
      rc += 55;
   else if( ( field == c_field_id_Needs_Other_Field_2 ) || ( field == c_field_name_Needs_Other_Field_2 ) )
      rc += 56;
   else if( ( field == c_field_id_Needs_Other_Modifier ) || ( field == c_field_name_Needs_Other_Modifier ) )
      rc += 57;
   else if( ( field == c_field_id_Needs_Other_Modifier_2 ) || ( field == c_field_name_Needs_Other_Modifier_2 ) )
      rc += 58;
   else if( ( field == c_field_id_Needs_Other_Permission ) || ( field == c_field_name_Needs_Other_Permission ) )
      rc += 59;
   else if( ( field == c_field_id_Needs_Other_Permission_2 ) || ( field == c_field_name_Needs_Other_Permission_2 ) )
      rc += 60;
   else if( ( field == c_field_id_Needs_Other_Procedure ) || ( field == c_field_name_Needs_Other_Procedure ) )
      rc += 61;
   else if( ( field == c_field_id_Needs_Other_Procedure_2 ) || ( field == c_field_name_Needs_Other_Procedure_2 ) )
      rc += 62;
   else if( ( field == c_field_id_Needs_Other_Source_Child ) || ( field == c_field_name_Needs_Other_Source_Child ) )
      rc += 63;
   else if( ( field == c_field_id_Needs_Other_Source_Child_2 ) || ( field == c_field_name_Needs_Other_Source_Child_2 ) )
      rc += 64;
   else if( ( field == c_field_id_Needs_Permission ) || ( field == c_field_name_Needs_Permission ) )
      rc += 65;
   else if( ( field == c_field_id_Needs_Procedure ) || ( field == c_field_name_Needs_Procedure ) )
      rc += 66;
   else if( ( field == c_field_id_Needs_Procedure_Arg ) || ( field == c_field_name_Needs_Procedure_Arg ) )
      rc += 67;
   else if( ( field == c_field_id_Needs_Procedure_Arg_2 ) || ( field == c_field_name_Needs_Procedure_Arg_2 ) )
      rc += 68;
   else if( ( field == c_field_id_Needs_Procedure_Arg_3 ) || ( field == c_field_name_Needs_Procedure_Arg_3 ) )
      rc += 69;
   else if( ( field == c_field_id_Needs_Source_Child ) || ( field == c_field_name_Needs_Source_Child ) )
      rc += 70;
   else if( ( field == c_field_id_Needs_Source_Field ) || ( field == c_field_name_Needs_Source_Field ) )
      rc += 71;
   else if( ( field == c_field_id_Needs_Source_Field_Or_Child ) || ( field == c_field_name_Needs_Source_Field_Or_Child ) )
      rc += 72;
   else if( ( field == c_field_id_Needs_Source_Parent ) || ( field == c_field_name_Needs_Source_Parent ) )
      rc += 73;
   else if( ( field == c_field_id_Needs_Test_Field ) || ( field == c_field_name_Needs_Test_Field ) )
      rc += 74;
   else if( ( field == c_field_id_Needs_Test_Value ) || ( field == c_field_name_Needs_Test_Value ) )
      rc += 75;
   else if( ( field == c_field_id_Needs_Value ) || ( field == c_field_name_Needs_Value ) )
      rc += 76;
   else if( ( field == c_field_id_Next_Child_Specification_Type ) || ( field == c_field_name_Next_Child_Specification_Type ) )
      rc += 77;
   else if( ( field == c_field_id_Next_Protect_Child_Rel ) || ( field == c_field_name_Next_Protect_Child_Rel ) )
      rc += 78;
   else if( ( field == c_field_id_Next_Protect_Procedure ) || ( field == c_field_name_Next_Protect_Procedure ) )
      rc += 79;
   else if( ( field == c_field_id_Next_Protect_Source_Parent ) || ( field == c_field_name_Next_Protect_Source_Parent ) )
      rc += 80;
   else if( ( field == c_field_id_Next_Specification_Actions ) || ( field == c_field_name_Next_Specification_Actions ) )
      rc += 81;
   else if( ( field == c_field_id_Notes ) || ( field == c_field_name_Notes ) )
      rc += 82;
   else if( ( field == c_field_id_Parent_Specification_Type ) || ( field == c_field_name_Parent_Specification_Type ) )
      rc += 83;
   else if( ( field == c_field_id_Procedure_Arg_primitive ) || ( field == c_field_name_Procedure_Arg_primitive ) )
      rc += 84;
   else if( ( field == c_field_id_Procedure_Arg_type ) || ( field == c_field_name_Procedure_Arg_type ) )
      rc += 85;
   else if( ( field == c_field_id_Protect_Child_Rel_From_Update ) || ( field == c_field_name_Protect_Child_Rel_From_Update ) )
      rc += 86;
   else if( ( field == c_field_id_Protect_Class_From_Edit ) || ( field == c_field_name_Protect_Class_From_Edit ) )
      rc += 87;
   else if( ( field == c_field_id_Protect_Class_From_Update ) || ( field == c_field_name_Protect_Class_From_Update ) )
      rc += 88;
   else if( ( field == c_field_id_Protect_Other_Class_From_Edit ) || ( field == c_field_name_Protect_Other_Class_From_Edit ) )
      rc += 89;
   else if( ( field == c_field_id_Protect_Procedure_From_Edit ) || ( field == c_field_name_Protect_Procedure_From_Edit ) )
      rc += 90;
   else if( ( field == c_field_id_Protect_Source_Parent ) || ( field == c_field_name_Protect_Source_Parent ) )
      rc += 91;
   else if( ( field == c_field_id_Set_Field_To_Source_Field_Or_Child ) || ( field == c_field_name_Set_Field_To_Source_Field_Or_Child ) )
      rc += 92;
   else if( ( field == c_field_id_Source_Field_Needs_Test_Field ) || ( field == c_field_name_Source_Field_Needs_Test_Field ) )
      rc += 93;
   else if( ( field == c_field_id_Source_Field_Type_Match ) || ( field == c_field_name_Source_Field_Type_Match ) )
      rc += 94;
   else if( ( field == c_field_id_Source_Parent_type ) || ( field == c_field_name_Source_Parent_type ) )
      rc += 95;
   else if( ( field == c_field_id_Source_type ) || ( field == c_field_name_Source_type ) )
      rc += 96;
   else if( ( field == c_field_id_Specification_Actions ) || ( field == c_field_name_Specification_Actions ) )
      rc += 97;
   else if( ( field == c_field_id_Specification_Name ) || ( field == c_field_name_Specification_Name ) )
      rc += 98;
   else if( ( field == c_field_id_Specification_Object ) || ( field == c_field_name_Specification_Object ) )
      rc += 99;
   else if( ( field == c_field_id_Specification_Strings ) || ( field == c_field_name_Specification_Strings ) )
      rc += 100;
   if( ( field == c_field_id_Specification_Vars ) || ( field == c_field_name_Specification_Vars ) )
      rc += 101;
   else if( ( field == c_field_id_Test_Field_Type_Match ) || ( field == c_field_name_Test_Field_Type_Match ) )
      rc += 102;
   else if( ( field == c_field_id_Test_Field_type ) || ( field == c_field_name_Test_Field_type ) )
      rc += 103;
   else if( ( field == c_field_id_Use_Class_As_Source_Parent_Class ) || ( field == c_field_name_Use_Class_As_Source_Parent_Class ) )
      rc += 104;
   else if( ( field == c_field_id_Use_Field_Enum ) || ( field == c_field_name_Use_Field_Enum ) )
      rc += 105;
   else if( ( field == c_field_id_Use_Parent_Child_Rel_As_Source_Parent ) || ( field == c_field_name_Use_Parent_Child_Rel_As_Source_Parent ) )
      rc += 106;
   else if( ( field == c_field_id_Use_Parent_Child_Rel_For_Class ) || ( field == c_field_name_Use_Parent_Child_Rel_For_Class ) )
      rc += 107;
   else if( ( field == c_field_id_Use_Parent_Child_Rel_For_Other_Class ) || ( field == c_field_name_Use_Parent_Child_Rel_For_Other_Class ) )
      rc += 108;
   else if( ( field == c_field_id_Use_Parent_Class ) || ( field == c_field_name_Use_Parent_Class ) )
      rc += 109;
   else if( ( field == c_field_id_Use_Parent_Field_For_Class ) || ( field == c_field_name_Use_Parent_Field_For_Class ) )
      rc += 110;
   else if( ( field == c_field_id_Use_Parent_Other_Class ) || ( field == c_field_name_Use_Parent_Other_Class ) )
      rc += 111;
   else if( ( field == c_field_id_Use_Parent_Other_Class_For_Class ) || ( field == c_field_name_Use_Parent_Other_Class_For_Class ) )
      rc += 112;
   else if( ( field == c_field_id_Use_Parent_Procedure ) || ( field == c_field_name_Use_Parent_Procedure ) )
      rc += 113;
   else if( ( field == c_field_id_Use_Parent_Source_Class ) || ( field == c_field_name_Use_Parent_Source_Class ) )
      rc += 114;
   else if( ( field == c_field_id_Use_Parent_Source_Field_For_Class ) || ( field == c_field_name_Use_Parent_Source_Field_For_Class ) )
      rc += 115;
   else if( ( field == c_field_id_Use_Parent_Source_Parent ) || ( field == c_field_name_Use_Parent_Source_Parent ) )
      rc += 116;
   else if( ( field == c_field_id_Use_Parent_Source_Parent_For_Class ) || ( field == c_field_name_Use_Parent_Source_Parent_For_Class ) )
      rc += 117;
   else if( ( field == c_field_id_Use_Source_Field_Enum ) || ( field == c_field_name_Use_Source_Field_Enum ) )
      rc += 118;
   else if( ( field == c_field_id_Use_Source_Field_Or_Child_Enum ) || ( field == c_field_name_Use_Source_Field_Or_Child_Enum ) )
      rc += 119;
   else if( ( field == c_field_id_View_Id ) || ( field == c_field_name_View_Id ) )
      rc += 120;

   return rc - 1;
}

procedure_info_container& Meta_Specification_Type::static_get_procedure_info( )
{
   static bool initialised = false;
   static procedure_info_container procedures;

   if( !initialised )
   {
      initialised = true;
      procedures.insert( make_pair( "114410", procedure_info( "Is_Valid_Field_Type" ) ) );
   }

   return procedures;
}

string Meta_Specification_Type::static_get_sql_columns( )
{
   string sql_columns;

   sql_columns += 
    "C_Key_ VARCHAR(75),"
    "C_Ver_ SMALLINT UNSIGNED NOT NULL,"
    "C_Rev_ BIGINT UNSIGNED NOT NULL,"
    "C_Sec_ BIGINT UNSIGNED NOT NULL,"
    "C_Typ_ VARCHAR(16) NOT NULL,"
    "C_Allow_Child_Relationship INTEGER NOT NULL,"
    "C_Allow_Enum INTEGER NOT NULL,"
    "C_Allow_Enum_Item INTEGER NOT NULL,"
    "C_Allow_FK_Source_Field_Choice INTEGER NOT NULL,"
    "C_Allow_FK_Test_Field_Choice INTEGER NOT NULL,"
    "C_Allow_Field INTEGER NOT NULL,"
    "C_Allow_Modifier INTEGER NOT NULL,"
    "C_Allow_Options INTEGER NOT NULL,"
    "C_Allow_Other_Class INTEGER NOT NULL,"
    "C_Allow_Other_Class_Field INTEGER NOT NULL,"
    "C_Allow_Other_Field INTEGER NOT NULL,"
    "C_Allow_Other_Field_2 INTEGER NOT NULL,"
    "C_Allow_Other_Modifier INTEGER NOT NULL,"
    "C_Allow_Other_Modifier_2 INTEGER NOT NULL,"
    "C_Allow_Other_Procedure INTEGER NOT NULL,"
    "C_Allow_Other_Procedure_2 INTEGER NOT NULL,"
    "C_Allow_Other_Source_Child INTEGER NOT NULL,"
    "C_Allow_Other_Source_Child_2 INTEGER NOT NULL,"
    "C_Allow_Permission INTEGER NOT NULL,"
    "C_Allow_Procedure INTEGER NOT NULL,"
    "C_Allow_Procedure_Arg INTEGER NOT NULL,"
    "C_Allow_Source_Child INTEGER NOT NULL,"
    "C_Allow_Source_Class INTEGER NOT NULL,"
    "C_Allow_Source_Field INTEGER NOT NULL,"
    "C_Allow_Source_Grandchild INTEGER NOT NULL,"
    "C_Allow_Source_Parent INTEGER NOT NULL,"
    "C_Allow_Test_Field INTEGER NOT NULL,"
    "C_Allow_Test_Value INTEGER NOT NULL,"
    "C_Allow_Value INTEGER NOT NULL,"
    "C_Can_Access_Below_Model INTEGER NOT NULL,"
    "C_Child_Prefix VARCHAR(256) NOT NULL,"
    "C_Child_Rel_Grandparent_Match INTEGER NOT NULL,"
    "C_Child_Rel_Parent_Match INTEGER NOT NULL,"
    "C_Child_Relationship_Class_Match INTEGER NOT NULL,"
    "C_Child_Specification_Type VARCHAR(256) NOT NULL,"
    "C_Default_Child_Vars TEXT NOT NULL,"
    "C_Field_type INTEGER NOT NULL,"
    "C_Has_Next_Specification_Info INTEGER NOT NULL,"
    "C_Is_Child_Only INTEGER NOT NULL,"
    "C_Is_Required_For_UI_Gen INTEGER NOT NULL,"
    "C_Is_System INTEGER NOT NULL,"
    "C_Name VARCHAR(200) NOT NULL,"
    "C_Needs_Child_Relationship INTEGER NOT NULL,"
    "C_Needs_Class INTEGER NOT NULL,"
    "C_Needs_Enum INTEGER NOT NULL,"
    "C_Needs_Enum_Item INTEGER NOT NULL,"
    "C_Needs_Enum_Item_2 INTEGER NOT NULL,"
    "C_Needs_Enum_Item_3 INTEGER NOT NULL,"
    "C_Needs_Enum_Item_4 INTEGER NOT NULL,"
    "C_Needs_Enum_Item_5 INTEGER NOT NULL,"
    "C_Needs_Field INTEGER NOT NULL,"
    "C_Needs_Modifier INTEGER NOT NULL,"
    "C_Needs_Other_Class INTEGER NOT NULL,"
    "C_Needs_Other_Class_Field INTEGER NOT NULL,"
    "C_Needs_Other_Field INTEGER NOT NULL,"
    "C_Needs_Other_Field_2 INTEGER NOT NULL,"
    "C_Needs_Other_Modifier INTEGER NOT NULL,"
    "C_Needs_Other_Modifier_2 INTEGER NOT NULL,"
    "C_Needs_Other_Permission INTEGER NOT NULL,"
    "C_Needs_Other_Permission_2 INTEGER NOT NULL,"
    "C_Needs_Other_Procedure INTEGER NOT NULL,"
    "C_Needs_Other_Procedure_2 INTEGER NOT NULL,"
    "C_Needs_Other_Source_Child INTEGER NOT NULL,"
    "C_Needs_Other_Source_Child_2 INTEGER NOT NULL,"
    "C_Needs_Permission INTEGER NOT NULL,"
    "C_Needs_Procedure INTEGER NOT NULL,"
    "C_Needs_Procedure_Arg INTEGER NOT NULL,"
    "C_Needs_Procedure_Arg_2 INTEGER NOT NULL,"
    "C_Needs_Procedure_Arg_3 INTEGER NOT NULL,"
    "C_Needs_Source_Child INTEGER NOT NULL,"
    "C_Needs_Source_Field INTEGER NOT NULL,"
    "C_Needs_Source_Field_Or_Child INTEGER NOT NULL,"
    "C_Needs_Source_Parent INTEGER NOT NULL,"
    "C_Needs_Test_Field INTEGER NOT NULL,"
    "C_Needs_Test_Value INTEGER NOT NULL,"
    "C_Needs_Value INTEGER NOT NULL,"
    "C_Next_Child_Specification_Type VARCHAR(256) NOT NULL,"
    "C_Next_Protect_Child_Rel INTEGER NOT NULL,"
    "C_Next_Protect_Procedure INTEGER NOT NULL,"
    "C_Next_Protect_Source_Parent INTEGER NOT NULL,"
    "C_Next_Specification_Actions VARCHAR(256) NOT NULL,"
    "C_Notes TEXT NOT NULL,"
    "C_Parent_Specification_Type VARCHAR(75) NOT NULL,"
    "C_Procedure_Arg_primitive INTEGER NOT NULL,"
    "C_Procedure_Arg_type INTEGER NOT NULL,"
    "C_Protect_Child_Rel_From_Update INTEGER NOT NULL,"
    "C_Protect_Class_From_Edit INTEGER NOT NULL,"
    "C_Protect_Class_From_Update INTEGER NOT NULL,"
    "C_Protect_Other_Class_From_Edit INTEGER NOT NULL,"
    "C_Protect_Procedure_From_Edit INTEGER NOT NULL,"
    "C_Protect_Source_Parent INTEGER NOT NULL,"
    "C_Set_Field_To_Source_Field_Or_Child INTEGER NOT NULL,"
    "C_Source_Field_Needs_Test_Field INTEGER NOT NULL,"
    "C_Source_Field_Type_Match INTEGER NOT NULL,"
    "C_Source_Parent_type INTEGER NOT NULL,"
    "C_Source_type INTEGER NOT NULL,"
    "C_Specification_Actions VARCHAR(200) NOT NULL,"
    "C_Specification_Name VARCHAR(200) NOT NULL,"
    "C_Specification_Object VARCHAR(200) NOT NULL,"
    "C_Specification_Strings TEXT NOT NULL,"
    "C_Specification_Vars TEXT NOT NULL,"
    "C_Test_Field_Type_Match INTEGER NOT NULL,"
    "C_Test_Field_type INTEGER NOT NULL,"
    "C_Use_Class_As_Source_Parent_Class INTEGER NOT NULL,"
    "C_Use_Field_Enum INTEGER NOT NULL,"
    "C_Use_Parent_Child_Rel_As_Source_Parent INTEGER NOT NULL,"
    "C_Use_Parent_Child_Rel_For_Class INTEGER NOT NULL,"
    "C_Use_Parent_Child_Rel_For_Other_Class INTEGER NOT NULL,"
    "C_Use_Parent_Class INTEGER NOT NULL,"
    "C_Use_Parent_Field_For_Class INTEGER NOT NULL,"
    "C_Use_Parent_Other_Class INTEGER NOT NULL,"
    "C_Use_Parent_Other_Class_For_Class INTEGER NOT NULL,"
    "C_Use_Parent_Procedure INTEGER NOT NULL,"
    "C_Use_Parent_Source_Class INTEGER NOT NULL,"
    "C_Use_Parent_Source_Field_For_Class INTEGER NOT NULL,"
    "C_Use_Parent_Source_Parent INTEGER NOT NULL,"
    "C_Use_Parent_Source_Parent_For_Class INTEGER NOT NULL,"
    "C_Use_Source_Field_Enum INTEGER NOT NULL,"
    "C_Use_Source_Field_Or_Child_Enum INTEGER NOT NULL,"
    "C_View_Id VARCHAR(200) NOT NULL,"
    "PRIMARY KEY(C_Key_)";

   return sql_columns;
}

void Meta_Specification_Type::static_get_text_search_fields( vector< string >& fields )
{
   ( void )fields;
}

void Meta_Specification_Type::static_get_all_enum_pairs( vector< pair< string, string > >& pairs )
{
   pairs.push_back( make_pair( "enum_field_type_0", get_enum_string_field_type( 0 ) ) );
   pairs.push_back( make_pair( "enum_field_type_1", get_enum_string_field_type( 1 ) ) );
   pairs.push_back( make_pair( "enum_field_type_2", get_enum_string_field_type( 2 ) ) );
   pairs.push_back( make_pair( "enum_field_type_3", get_enum_string_field_type( 3 ) ) );
   pairs.push_back( make_pair( "enum_field_type_4", get_enum_string_field_type( 4 ) ) );
   pairs.push_back( make_pair( "enum_field_type_5", get_enum_string_field_type( 5 ) ) );
   pairs.push_back( make_pair( "enum_field_type_6", get_enum_string_field_type( 6 ) ) );
   pairs.push_back( make_pair( "enum_field_type_20", get_enum_string_field_type( 20 ) ) );
   pairs.push_back( make_pair( "enum_field_type_30", get_enum_string_field_type( 30 ) ) );
   pairs.push_back( make_pair( "enum_field_type_31", get_enum_string_field_type( 31 ) ) );
   pairs.push_back( make_pair( "enum_field_type_39", get_enum_string_field_type( 39 ) ) );
   pairs.push_back( make_pair( "enum_field_type_40", get_enum_string_field_type( 40 ) ) );
   pairs.push_back( make_pair( "enum_field_type_41", get_enum_string_field_type( 41 ) ) );
   pairs.push_back( make_pair( "enum_field_type_45", get_enum_string_field_type( 45 ) ) );
   pairs.push_back( make_pair( "enum_field_type_49", get_enum_string_field_type( 49 ) ) );
   pairs.push_back( make_pair( "enum_field_type_50", get_enum_string_field_type( 50 ) ) );
   pairs.push_back( make_pair( "enum_field_type_51", get_enum_string_field_type( 51 ) ) );
   pairs.push_back( make_pair( "enum_field_type_52", get_enum_string_field_type( 52 ) ) );
   pairs.push_back( make_pair( "enum_field_type_99", get_enum_string_field_type( 99 ) ) );

   pairs.push_back( make_pair( "enum_procedure_arg_primitive_0", get_enum_string_procedure_arg_primitive( 0 ) ) );
   pairs.push_back( make_pair( "enum_procedure_arg_primitive_1", get_enum_string_procedure_arg_primitive( 1 ) ) );
   pairs.push_back( make_pair( "enum_procedure_arg_primitive_2", get_enum_string_procedure_arg_primitive( 2 ) ) );
   pairs.push_back( make_pair( "enum_procedure_arg_primitive_3", get_enum_string_procedure_arg_primitive( 3 ) ) );
   pairs.push_back( make_pair( "enum_procedure_arg_primitive_4", get_enum_string_procedure_arg_primitive( 4 ) ) );
   pairs.push_back( make_pair( "enum_procedure_arg_primitive_5", get_enum_string_procedure_arg_primitive( 5 ) ) );
   pairs.push_back( make_pair( "enum_procedure_arg_primitive_6", get_enum_string_procedure_arg_primitive( 6 ) ) );
   pairs.push_back( make_pair( "enum_procedure_arg_primitive_99", get_enum_string_procedure_arg_primitive( 99 ) ) );

   pairs.push_back( make_pair( "enum_procedure_arg_type_0", get_enum_string_procedure_arg_type( 0 ) ) );
   pairs.push_back( make_pair( "enum_procedure_arg_type_1", get_enum_string_procedure_arg_type( 1 ) ) );
   pairs.push_back( make_pair( "enum_procedure_arg_type_99", get_enum_string_procedure_arg_type( 99 ) ) );

   pairs.push_back( make_pair( "enum_source_parent_type_50", get_enum_string_source_parent_type( 50 ) ) );
   pairs.push_back( make_pair( "enum_source_parent_type_51", get_enum_string_source_parent_type( 51 ) ) );
   pairs.push_back( make_pair( "enum_source_parent_type_52", get_enum_string_source_parent_type( 52 ) ) );

   pairs.push_back( make_pair( "enum_source_type_0", get_enum_string_source_type( 0 ) ) );
   pairs.push_back( make_pair( "enum_source_type_1", get_enum_string_source_type( 1 ) ) );
   pairs.push_back( make_pair( "enum_source_type_2", get_enum_string_source_type( 2 ) ) );
   pairs.push_back( make_pair( "enum_source_type_3", get_enum_string_source_type( 3 ) ) );
   pairs.push_back( make_pair( "enum_source_type_4", get_enum_string_source_type( 4 ) ) );
   pairs.push_back( make_pair( "enum_source_type_5", get_enum_string_source_type( 5 ) ) );
   pairs.push_back( make_pair( "enum_source_type_6", get_enum_string_source_type( 6 ) ) );
   pairs.push_back( make_pair( "enum_source_type_20", get_enum_string_source_type( 20 ) ) );
   pairs.push_back( make_pair( "enum_source_type_35", get_enum_string_source_type( 35 ) ) );
   pairs.push_back( make_pair( "enum_source_type_99", get_enum_string_source_type( 99 ) ) );

   pairs.push_back( make_pair( "enum_test_field_type_0", get_enum_string_test_field_type( 0 ) ) );
   pairs.push_back( make_pair( "enum_test_field_type_1", get_enum_string_test_field_type( 1 ) ) );
   pairs.push_back( make_pair( "enum_test_field_type_2", get_enum_string_test_field_type( 2 ) ) );
   pairs.push_back( make_pair( "enum_test_field_type_3", get_enum_string_test_field_type( 3 ) ) );
   pairs.push_back( make_pair( "enum_test_field_type_4", get_enum_string_test_field_type( 4 ) ) );
   pairs.push_back( make_pair( "enum_test_field_type_5", get_enum_string_test_field_type( 5 ) ) );
   pairs.push_back( make_pair( "enum_test_field_type_6", get_enum_string_test_field_type( 6 ) ) );
   pairs.push_back( make_pair( "enum_test_field_type_20", get_enum_string_test_field_type( 20 ) ) );
   pairs.push_back( make_pair( "enum_test_field_type_30", get_enum_string_test_field_type( 30 ) ) );
   pairs.push_back( make_pair( "enum_test_field_type_35", get_enum_string_test_field_type( 35 ) ) );
   pairs.push_back( make_pair( "enum_test_field_type_38", get_enum_string_test_field_type( 38 ) ) );
   pairs.push_back( make_pair( "enum_test_field_type_99", get_enum_string_test_field_type( 99 ) ) );
}

void Meta_Specification_Type::static_get_all_index_pairs( vector< pair< string, string > >& pairs )
{
   pairs.push_back( make_pair( "Is_Child_Only,Is_System,Parent_Specification_Type,Can_Access_Below_Model,Name", "bool,bool,string,bool,string" ) );
   pairs.push_back( make_pair( "Is_Child_Only,Is_System,Parent_Specification_Type,Name", "bool,bool,string,string" ) );
   pairs.push_back( make_pair( "Is_Child_Only,Parent_Specification_Type,Can_Access_Below_Model,Name", "bool,string,bool,string" ) );
   pairs.push_back( make_pair( "Is_Child_Only,Parent_Specification_Type,Name", "bool,string,string" ) );
   pairs.push_back( make_pair( "Parent_Specification_Type,Is_Child_Only,Name", "string,bool,string" ) );
   pairs.push_back( make_pair( "Parent_Specification_Type,Name", "string,string" ) );
}

void Meta_Specification_Type::static_get_all_unique_indexes( vector< string >& unique_indexes )
{
   unique_indexes.push_back( "Is_Child_Only,Is_System,Parent_Specification_Type,Can_Access_Below_Model,Name" );
   unique_indexes.push_back( "Is_Child_Only,Is_System,Parent_Specification_Type,Name" );
   unique_indexes.push_back( "Is_Child_Only,Parent_Specification_Type,Can_Access_Below_Model,Name" );
   unique_indexes.push_back( "Is_Child_Only,Parent_Specification_Type,Name" );
   unique_indexes.push_back( "Parent_Specification_Type,Is_Child_Only,Name" );
   unique_indexes.push_back( "Parent_Specification_Type,Name" );
}

void Meta_Specification_Type::static_get_sql_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_Is_Child_Only,C_Is_System,C_Parent_Specification_Type,C_Can_Access_Below_Model,C_Name" );
   indexes.push_back( "C_Is_Child_Only,C_Is_System,C_Parent_Specification_Type,C_Name" );
   indexes.push_back( "C_Is_Child_Only,C_Parent_Specification_Type,C_Can_Access_Below_Model,C_Name" );
   indexes.push_back( "C_Is_Child_Only,C_Parent_Specification_Type,C_Name" );
   indexes.push_back( "C_Parent_Specification_Type,C_Is_Child_Only,C_Name" );
   indexes.push_back( "C_Parent_Specification_Type,C_Name" );
}

void Meta_Specification_Type::static_get_sql_unique_indexes( vector< string >& indexes )
{
   indexes.push_back( "C_Is_Child_Only,C_Is_System,C_Parent_Specification_Type,C_Can_Access_Below_Model,C_Name" );
   indexes.push_back( "C_Is_Child_Only,C_Is_System,C_Parent_Specification_Type,C_Name" );
   indexes.push_back( "C_Is_Child_Only,C_Parent_Specification_Type,C_Can_Access_Below_Model,C_Name" );
   indexes.push_back( "C_Is_Child_Only,C_Parent_Specification_Type,C_Name" );
   indexes.push_back( "C_Parent_Specification_Type,C_Is_Child_Only,C_Name" );
   indexes.push_back( "C_Parent_Specification_Type,C_Name" );
}

void Meta_Specification_Type::static_insert_derivation( const string& module_and_class_id )
{
   g_derivations.insert( module_and_class_id );
}

void Meta_Specification_Type::static_remove_derivation( const string& module_and_class_id )
{
   if( g_derivations.count( module_and_class_id ) )
      g_derivations.erase( module_and_class_id );
}

void Meta_Specification_Type::static_insert_external_alias( const string& module_and_class_id, Meta_Specification_Type* p_instance )
{
   g_external_aliases.insert( external_aliases_value_type( module_and_class_id, p_instance ) );
}

void Meta_Specification_Type::static_remove_external_alias( const string& module_and_class_id )
{
   if( g_external_aliases.count( module_and_class_id ) )
   {
      delete g_external_aliases[ module_and_class_id ];
      g_external_aliases.erase( module_and_class_id );
   }
}

void Meta_Specification_Type::static_class_init( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for init" );

   g_state_names_variable = get_special_var_name( e_special_var_state_names );

   g_field_type_enum.insert( 0 );
   g_field_type_enum.insert( 1 );
   g_field_type_enum.insert( 2 );
   g_field_type_enum.insert( 3 );
   g_field_type_enum.insert( 4 );
   g_field_type_enum.insert( 5 );
   g_field_type_enum.insert( 6 );
   g_field_type_enum.insert( 20 );
   g_field_type_enum.insert( 30 );
   g_field_type_enum.insert( 31 );
   g_field_type_enum.insert( 39 );
   g_field_type_enum.insert( 40 );
   g_field_type_enum.insert( 41 );
   g_field_type_enum.insert( 45 );
   g_field_type_enum.insert( 49 );
   g_field_type_enum.insert( 50 );
   g_field_type_enum.insert( 51 );
   g_field_type_enum.insert( 52 );
   g_field_type_enum.insert( 99 );

   g_procedure_arg_primitive_enum.insert( 0 );
   g_procedure_arg_primitive_enum.insert( 1 );
   g_procedure_arg_primitive_enum.insert( 2 );
   g_procedure_arg_primitive_enum.insert( 3 );
   g_procedure_arg_primitive_enum.insert( 4 );
   g_procedure_arg_primitive_enum.insert( 5 );
   g_procedure_arg_primitive_enum.insert( 6 );
   g_procedure_arg_primitive_enum.insert( 99 );

   g_procedure_arg_type_enum.insert( 0 );
   g_procedure_arg_type_enum.insert( 1 );
   g_procedure_arg_type_enum.insert( 99 );

   g_source_parent_type_enum.insert( 50 );
   g_source_parent_type_enum.insert( 51 );
   g_source_parent_type_enum.insert( 52 );

   g_source_type_enum.insert( 0 );
   g_source_type_enum.insert( 1 );
   g_source_type_enum.insert( 2 );
   g_source_type_enum.insert( 3 );
   g_source_type_enum.insert( 4 );
   g_source_type_enum.insert( 5 );
   g_source_type_enum.insert( 6 );
   g_source_type_enum.insert( 20 );
   g_source_type_enum.insert( 35 );
   g_source_type_enum.insert( 99 );

   g_test_field_type_enum.insert( 0 );
   g_test_field_type_enum.insert( 1 );
   g_test_field_type_enum.insert( 2 );
   g_test_field_type_enum.insert( 3 );
   g_test_field_type_enum.insert( 4 );
   g_test_field_type_enum.insert( 5 );
   g_test_field_type_enum.insert( 6 );
   g_test_field_type_enum.insert( 20 );
   g_test_field_type_enum.insert( 30 );
   g_test_field_type_enum.insert( 35 );
   g_test_field_type_enum.insert( 38 );
   g_test_field_type_enum.insert( 99 );

   // [<start static_class_init>]
   // [<finish static_class_init>]
}

void Meta_Specification_Type::static_class_term( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for term" );

   // [<start static_class_term>]
   // [<finish static_class_term>]
}
