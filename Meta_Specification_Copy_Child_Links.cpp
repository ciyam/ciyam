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

#include "Meta_Specification_Copy_Child_Links.h"

#include "Meta_Enum.h"
#include "Meta_Enum_Item.h"
#include "Meta_List_Field.h"
#include "Meta_Model.h"
#include "Meta_Modifier.h"
#include "Meta_Permission.h"
#include "Meta_Procedure.h"
#include "Meta_Procedure_Arg.h"
#include "Meta_Specification_Copy_Child_Links.h"
#include "Meta_Specification_Type.h"
#include "Meta_View_Field.h"
#include "Meta_Specification.h"
#include "Meta_Field.h"
#include "Meta_Class.h"
#include "Meta_Relationship.h"

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

template< > inline string to_string( const Meta_Class& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

template< > inline string to_string( const Meta_Relationship& c )
{
   return ::to_string( static_cast< const class_base& >( c ) );
}

inline void from_string( Meta_Field& c, const string& s )
{
   ::from_string( static_cast< class_base& >( c ), s );
}

inline void from_string( Meta_Class& c, const string& s )
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

#include "Meta_Specification_Copy_Child_Links.cmh"

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
const char* const c_field_id_Value_Label = "115131";
const char* const c_field_id_Value_Left_Part = "115129";
const char* const c_field_id_Value_Literal = "115116";
const char* const c_field_id_Value_Numeric_String = "115127";
const char* const c_field_id_Value_Right_Part = "115130";
const char* const c_field_id_Value_String = "115125";
const char* const c_field_id_Vars = "115114";

const char* const c_field_id_Child_File_Field = "302650";
const char* const c_field_id_Source_Child_File_Field = "302653";
const char* const c_field_id_Source_Child_Rel_Child_Class = "302652";
const char* const c_field_id_Source_Child_Relationship = "302651";

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
const char* const c_field_name_Value_Label = "Value_Label";
const char* const c_field_name_Value_Left_Part = "Value_Left_Part";
const char* const c_field_name_Value_Literal = "Value_Literal";
const char* const c_field_name_Value_Numeric_String = "Value_Numeric_String";
const char* const c_field_name_Value_Right_Part = "Value_Right_Part";
const char* const c_field_name_Value_String = "Value_String";
const char* const c_field_name_Vars = "Vars";

const char* const c_field_name_Child_File_Field = "Child_File_Field";
const char* const c_field_name_Source_Child_File_Field = "Source_Child_File_Field";
const char* const c_field_name_Source_Child_Rel_Child_Class = "Source_Child_Rel_Child_Class";
const char* const c_field_name_Source_Child_Relationship = "Source_Child_Relationship";

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
const char* const c_field_display_name_Value_Label = "field_specification_value_label";
const char* const c_field_display_name_Value_Left_Part = "field_specification_value_left_part";
const char* const c_field_display_name_Value_Literal = "field_specification_value_literal";
const char* const c_field_display_name_Value_Numeric_String = "field_specification_value_numeric_string";
const char* const c_field_display_name_Value_Right_Part = "field_specification_value_right_part";
const char* const c_field_display_name_Value_String = "field_specification_value_string";
const char* const c_field_display_name_Vars = "field_specification_vars";

const char* const c_field_display_name_Child_File_Field = "field_specification_copy_child_links_child_file_field";
const char* const c_field_display_name_Source_Child_File_Field = "field_specification_copy_child_links_source_child_file_field";
const char* const c_field_display_name_Source_Child_Rel_Child_Class = "field_specification_copy_child_links_source_child_rel_child_class";
const char* const c_field_display_name_Source_Child_Relationship = "field_specification_copy_child_links_source_child_relationship";

const int c_num_fields = 4;

const char* const c_all_sorted_field_ids[ ] =
{
   "302650",
   "302651",
   "302652",
   "302653"
};

const char* const c_all_sorted_field_names[ ] =
{
   "Child_File_Field",
   "Source_Child_File_Field",
   "Source_Child_Rel_Child_Class",
   "Source_Child_Relationship"
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
const uint64_t c_modifier_Hide_File_Fields = UINT64_C( 0x400000000000 );

set< string > g_derivations;

typedef map< string, Meta_Specification_Copy_Child_Links* > external_aliases_container;
typedef external_aliases_container::const_iterator external_aliases_const_iterator;
typedef external_aliases_container::value_type external_aliases_value_type;

typedef map< size_t, Meta_Specification_Copy_Child_Links* > external_aliases_lookup_container;
typedef external_aliases_lookup_container::const_iterator external_aliases_lookup_const_iterator;

external_aliases_container g_external_aliases;
external_aliases_lookup_container g_external_aliases_lookup;

// [<start anonymous>]
// [<finish anonymous>]

}

registration< Meta_Specification_Copy_Child_Links > Specification_Copy_Child_Links_registration( get_class_registry( ), "132100" );

class Meta_Specification_Copy_Child_Links_command_functor;

class Meta_Specification_Copy_Child_Links_command_handler : public command_handler
{
   friend class Meta_Specification_Copy_Child_Links_command_functor;

   public:
   Meta_Specification_Copy_Child_Links_command_handler( ) : p_Meta_Specification_Copy_Child_Links( 0 ) { }

   void set_Meta_Specification_Copy_Child_Links( Meta_Specification_Copy_Child_Links* p_new_Meta_Specification_Copy_Child_Links ) { p_Meta_Specification_Copy_Child_Links = p_new_Meta_Specification_Copy_Child_Links; }

   void handle_unknown_command( const string& command )
   {
      ( void )command;

      retval = p_Meta_Specification_Copy_Child_Links->Meta_Specification::execute( get_cmd_and_args( ) );
   }

   void handle_invalid_command( const command_parser& parser, const string& cmd_and_args )
   {
      throw runtime_error( "invalid command usage '" + cmd_and_args + "'" );
   }

   private:
   Meta_Specification_Copy_Child_Links* p_Meta_Specification_Copy_Child_Links;

   protected:
   string retval;
   string cmd_and_args;
};

class Meta_Specification_Copy_Child_Links_command_functor : public command_functor
{
   public:
   Meta_Specification_Copy_Child_Links_command_functor( Meta_Specification_Copy_Child_Links_command_handler& handler )
    : command_functor( handler ),
    cmd_handler( handler )
   {
   }

   void operator ( )( const string& command, const parameter_info& parameters );

   private:
   Meta_Specification_Copy_Child_Links_command_handler& cmd_handler;
};

command_functor* Meta_Specification_Copy_Child_Links_command_functor_factory( const string& /*name*/, command_handler& handler )
{
   return new Meta_Specification_Copy_Child_Links_command_functor( dynamic_cast< Meta_Specification_Copy_Child_Links_command_handler& >( handler ) );
}

void Meta_Specification_Copy_Child_Links_command_functor::operator ( )( const string& command, const parameter_info& parameters )
{
   if( command == c_cmd_Meta_Specification_Copy_Child_Links_key )
   {
      bool want_fixed( has_parm_val( parameters, c_cmd_parm_Meta_Specification_Copy_Child_Links_key_fixed ) );

      if( !want_fixed )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Copy_Child_Links->get_key( );
      else
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Copy_Child_Links->get_fixed_key( );
   }
   else if( command == c_cmd_Meta_Specification_Copy_Child_Links_ver )
   {
      string ver_rev( to_string( cmd_handler.p_Meta_Specification_Copy_Child_Links->get_version( ) ) );
      ver_rev += "." + to_string( cmd_handler.p_Meta_Specification_Copy_Child_Links->get_revision( ) );

      cmd_handler.retval = ver_rev;
   }
   else if( command == c_cmd_Meta_Specification_Copy_Child_Links_get )
   {
      string field_name( get_parm_val( parameters, c_cmd_parm_Meta_Specification_Copy_Child_Links_get_field_name ) );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for getter call" );
      else if( field_name == c_field_id_Child_File_Field || field_name == c_field_name_Child_File_Field )
         string_getter< Meta_Field >( cmd_handler.p_Meta_Specification_Copy_Child_Links->Child_File_Field( ), cmd_handler.retval );
      else if( field_name == c_field_id_Source_Child_File_Field || field_name == c_field_name_Source_Child_File_Field )
         string_getter< Meta_Field >( cmd_handler.p_Meta_Specification_Copy_Child_Links->Source_Child_File_Field( ), cmd_handler.retval );
      else if( field_name == c_field_id_Source_Child_Rel_Child_Class || field_name == c_field_name_Source_Child_Rel_Child_Class )
         string_getter< Meta_Class >( cmd_handler.p_Meta_Specification_Copy_Child_Links->Source_Child_Rel_Child_Class( ), cmd_handler.retval );
      else if( field_name == c_field_id_Source_Child_Relationship || field_name == c_field_name_Source_Child_Relationship )
         string_getter< Meta_Relationship >( cmd_handler.p_Meta_Specification_Copy_Child_Links->Source_Child_Relationship( ), cmd_handler.retval );
      else
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Copy_Child_Links->Meta_Specification::execute( cmd_handler.cmd_and_args );
   }
   else if( command == c_cmd_Meta_Specification_Copy_Child_Links_set )
   {
      string field_name( get_parm_val( parameters, c_cmd_parm_Meta_Specification_Copy_Child_Links_set_field_name ) );
      string field_value( get_parm_val( parameters, c_cmd_parm_Meta_Specification_Copy_Child_Links_set_field_value ) );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for setter call" );
      else if( field_name == c_field_id_Child_File_Field || field_name == c_field_name_Child_File_Field )
         func_string_setter< Meta_Specification_Copy_Child_Links, Meta_Field >(
          *cmd_handler.p_Meta_Specification_Copy_Child_Links, &Meta_Specification_Copy_Child_Links::Child_File_Field, field_value );
      else if( field_name == c_field_id_Source_Child_File_Field || field_name == c_field_name_Source_Child_File_Field )
         func_string_setter< Meta_Specification_Copy_Child_Links, Meta_Field >(
          *cmd_handler.p_Meta_Specification_Copy_Child_Links, &Meta_Specification_Copy_Child_Links::Source_Child_File_Field, field_value );
      else if( field_name == c_field_id_Source_Child_Rel_Child_Class || field_name == c_field_name_Source_Child_Rel_Child_Class )
         func_string_setter< Meta_Specification_Copy_Child_Links, Meta_Class >(
          *cmd_handler.p_Meta_Specification_Copy_Child_Links, &Meta_Specification_Copy_Child_Links::Source_Child_Rel_Child_Class, field_value );
      else if( field_name == c_field_id_Source_Child_Relationship || field_name == c_field_name_Source_Child_Relationship )
         func_string_setter< Meta_Specification_Copy_Child_Links, Meta_Relationship >(
          *cmd_handler.p_Meta_Specification_Copy_Child_Links, &Meta_Specification_Copy_Child_Links::Source_Child_Relationship, field_value );
      else
         cmd_handler.p_Meta_Specification_Copy_Child_Links->Meta_Specification::execute( cmd_handler.cmd_and_args );

      cmd_handler.retval = c_okay;
   }
   else if( command == c_cmd_Meta_Specification_Copy_Child_Links_cmd )
   {
      string field_name( get_parm_val( parameters, c_cmd_parm_Meta_Specification_Copy_Child_Links_cmd_field_name ) );
      string cmd_and_args( get_parm_val( parameters, c_cmd_parm_Meta_Specification_Copy_Child_Links_cmd_cmd_and_args ) );

      cmd_handler.retval.erase( );

      if( field_name.empty( ) )
         throw runtime_error( "field name must not be empty for command call" );
      else if( field_name == c_field_id_Child_File_Field || field_name == c_field_name_Child_File_Field )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Copy_Child_Links->Child_File_Field( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Source_Child_File_Field || field_name == c_field_name_Source_Child_File_Field )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Copy_Child_Links->Source_Child_File_Field( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Source_Child_Rel_Child_Class || field_name == c_field_name_Source_Child_Rel_Child_Class )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Copy_Child_Links->Source_Child_Rel_Child_Class( ).execute( cmd_and_args );
      else if( field_name == c_field_id_Source_Child_Relationship || field_name == c_field_name_Source_Child_Relationship )
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Copy_Child_Links->Source_Child_Relationship( ).execute( cmd_and_args );
      else
         cmd_handler.retval = cmd_handler.p_Meta_Specification_Copy_Child_Links->Meta_Specification::execute( cmd_handler.cmd_and_args );
   }
}

struct Meta_Specification_Copy_Child_Links::impl : public Meta_Specification_Copy_Child_Links_command_handler
{
   impl( Meta_Specification_Copy_Child_Links& o )
    :
    cp_obj( &o ),
    total_child_relationships( 0 )
   {
      p_obj = &o;
      set_Meta_Specification_Copy_Child_Links( &o );

      add_commands( 0, Meta_Specification_Copy_Child_Links_command_functor_factory,
       ARRAY_PTR_AND_SIZE( Meta_Specification_Copy_Child_Links_command_definitions ) );
   }

   Meta_Specification_Copy_Child_Links& get_obj( ) const
   {
      return *cp_obj;
   }

   Meta_Field& impl_Child_File_Field( )
   {
      if( !cp_Child_File_Field )
      {
         cp_Child_File_Field.init( );

         p_obj->setup_graph_parent( *cp_Child_File_Field, c_field_id_Child_File_Field, v_Child_File_Field );
      }
      return *cp_Child_File_Field;
   }

   const Meta_Field& impl_Child_File_Field( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Child_File_Field )
      {
         cp_Child_File_Field.init( );

         p_obj->setup_graph_parent( *cp_Child_File_Field, c_field_id_Child_File_Field, v_Child_File_Field );
      }
      return *cp_Child_File_Field;
   }

   void impl_Child_File_Field( const string& key )
   {
      class_base_accessor cba( impl_Child_File_Field( ) );
      cba.set_key( key );
   }

   Meta_Field& impl_Source_Child_File_Field( )
   {
      if( !cp_Source_Child_File_Field )
      {
         cp_Source_Child_File_Field.init( );

         p_obj->setup_graph_parent( *cp_Source_Child_File_Field, c_field_id_Source_Child_File_Field, v_Source_Child_File_Field );
      }
      return *cp_Source_Child_File_Field;
   }

   const Meta_Field& impl_Source_Child_File_Field( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Source_Child_File_Field )
      {
         cp_Source_Child_File_Field.init( );

         p_obj->setup_graph_parent( *cp_Source_Child_File_Field, c_field_id_Source_Child_File_Field, v_Source_Child_File_Field );
      }
      return *cp_Source_Child_File_Field;
   }

   void impl_Source_Child_File_Field( const string& key )
   {
      class_base_accessor cba( impl_Source_Child_File_Field( ) );
      cba.set_key( key );
   }

   Meta_Class& impl_Source_Child_Rel_Child_Class( )
   {
      if( !cp_Source_Child_Rel_Child_Class )
      {
         cp_Source_Child_Rel_Child_Class.init( );

         p_obj->setup_graph_parent( *cp_Source_Child_Rel_Child_Class, c_field_id_Source_Child_Rel_Child_Class, v_Source_Child_Rel_Child_Class );
      }
      return *cp_Source_Child_Rel_Child_Class;
   }

   const Meta_Class& impl_Source_Child_Rel_Child_Class( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Source_Child_Rel_Child_Class )
      {
         cp_Source_Child_Rel_Child_Class.init( );

         p_obj->setup_graph_parent( *cp_Source_Child_Rel_Child_Class, c_field_id_Source_Child_Rel_Child_Class, v_Source_Child_Rel_Child_Class );
      }
      return *cp_Source_Child_Rel_Child_Class;
   }

   void impl_Source_Child_Rel_Child_Class( const string& key )
   {
      class_base_accessor cba( impl_Source_Child_Rel_Child_Class( ) );
      cba.set_key( key );
   }

   Meta_Relationship& impl_Source_Child_Relationship( )
   {
      if( !cp_Source_Child_Relationship )
      {
         cp_Source_Child_Relationship.init( );

         p_obj->setup_graph_parent( *cp_Source_Child_Relationship, c_field_id_Source_Child_Relationship, v_Source_Child_Relationship );
      }
      return *cp_Source_Child_Relationship;
   }

   const Meta_Relationship& impl_Source_Child_Relationship( ) const
   {
      lazy_fetch( p_obj );

      if( !cp_Source_Child_Relationship )
      {
         cp_Source_Child_Relationship.init( );

         p_obj->setup_graph_parent( *cp_Source_Child_Relationship, c_field_id_Source_Child_Relationship, v_Source_Child_Relationship );
      }
      return *cp_Source_Child_Relationship;
   }

   void impl_Source_Child_Relationship( const string& key )
   {
      class_base_accessor cba( impl_Source_Child_Relationship( ) );
      cba.set_key( key );
   }

   Meta_Specification& impl_child_Specification_Parent( )
   {
      if( !cp_child_Specification_Parent )
      {
         cp_child_Specification_Parent.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Parent, "302550" );
      }
      return *cp_child_Specification_Parent;
   }

   const Meta_Specification& impl_child_Specification_Parent( ) const
   {
      if( !cp_child_Specification_Parent )
      {
         cp_child_Specification_Parent.init( );

         p_obj->setup_graph_parent( *cp_child_Specification_Parent, "302550" );
      }
      return *cp_child_Specification_Parent;
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

   bool is_filtered( ) const;

   Meta_Specification_Copy_Child_Links* p_obj;
   class_pointer< Meta_Specification_Copy_Child_Links > cp_obj;

   // [<start members>]
   // [<finish members>]

   size_t total_child_relationships;

   string v_Child_File_Field;
   mutable class_pointer< Meta_Field > cp_Child_File_Field;

   string v_Source_Child_File_Field;
   mutable class_pointer< Meta_Field > cp_Source_Child_File_Field;

   string v_Source_Child_Rel_Child_Class;
   mutable class_pointer< Meta_Class > cp_Source_Child_Rel_Child_Class;

   string v_Source_Child_Relationship;
   mutable class_pointer< Meta_Relationship > cp_Source_Child_Relationship;

   mutable class_pointer< Meta_Specification > cp_child_Specification_Parent;
};

string Meta_Specification_Copy_Child_Links::impl::get_field_value( int field ) const
{
   string retval;

   switch( field )
   {
      case 0:
      retval = to_string( impl_Child_File_Field( ) );
      break;

      case 1:
      retval = to_string( impl_Source_Child_File_Field( ) );
      break;

      case 2:
      retval = to_string( impl_Source_Child_Rel_Child_Class( ) );
      break;

      case 3:
      retval = to_string( impl_Source_Child_Relationship( ) );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range" );
   }

   return retval;
}

void Meta_Specification_Copy_Child_Links::impl::set_field_value( int field, const string& value )
{
   switch( field )
   {
      case 0:
      func_string_setter< Meta_Specification_Copy_Child_Links::impl, Meta_Field >( *this, &Meta_Specification_Copy_Child_Links::impl::impl_Child_File_Field, value );
      break;

      case 1:
      func_string_setter< Meta_Specification_Copy_Child_Links::impl, Meta_Field >( *this, &Meta_Specification_Copy_Child_Links::impl::impl_Source_Child_File_Field, value );
      break;

      case 2:
      func_string_setter< Meta_Specification_Copy_Child_Links::impl, Meta_Class >( *this, &Meta_Specification_Copy_Child_Links::impl::impl_Source_Child_Rel_Child_Class, value );
      break;

      case 3:
      func_string_setter< Meta_Specification_Copy_Child_Links::impl, Meta_Relationship >( *this, &Meta_Specification_Copy_Child_Links::impl::impl_Source_Child_Relationship, value );
      break;

      default:
      throw runtime_error( "field #" + to_string( field ) + " is out of range" );
   }
}

uint64_t Meta_Specification_Copy_Child_Links::impl::get_state( ) const
{
   uint64_t state = 0;

   // [(start modifier_field_value)]
   if( get_obj( ).Specification_Type( ) == "trigger_create_children_for_fk_children" )
      state |= c_modifier_Hide_File_Fields;
   // [(finish modifier_field_value)]

   // [<start get_state>]
   // [<finish get_state>]

   return state;
}

const string& Meta_Specification_Copy_Child_Links::impl::execute( const string& cmd_and_args )
{
   this->cmd_and_args = cmd_and_args;
   execute_command( cmd_and_args );
   return retval;
}

void Meta_Specification_Copy_Child_Links::impl::clear_foreign_key( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( field == c_field_id_Child_File_Field || field == c_field_name_Child_File_Field )
      impl_Child_File_Field( "" );
   else if( field == c_field_id_Source_Child_File_Field || field == c_field_name_Source_Child_File_Field )
      impl_Source_Child_File_Field( "" );
   else if( field == c_field_id_Source_Child_Rel_Child_Class || field == c_field_name_Source_Child_Rel_Child_Class )
      impl_Source_Child_Rel_Child_Class( "" );
   else if( field == c_field_id_Source_Child_Relationship || field == c_field_name_Source_Child_Relationship )
      impl_Source_Child_Relationship( "" );
   else
      p_obj->parent_class_type::clear_foreign_key( field );
}

void Meta_Specification_Copy_Child_Links::impl::set_foreign_key_value( const string& field, const string& value )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for value: " + value );
   else if( field == c_field_id_Child_File_Field || field == c_field_name_Child_File_Field )
      v_Child_File_Field = value;
   else if( field == c_field_id_Source_Child_File_Field || field == c_field_name_Source_Child_File_Field )
      v_Source_Child_File_Field = value;
   else if( field == c_field_id_Source_Child_Rel_Child_Class || field == c_field_name_Source_Child_Rel_Child_Class )
      v_Source_Child_Rel_Child_Class = value;
   else if( field == c_field_id_Source_Child_Relationship || field == c_field_name_Source_Child_Relationship )
      v_Source_Child_Relationship = value;
   else
      p_obj->parent_class_type::set_foreign_key_value( field, value );
}

const string& Meta_Specification_Copy_Child_Links::impl::get_foreign_key_value( const string& field )
{
   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id" );
   else if( field == c_field_id_Child_File_Field || field == c_field_name_Child_File_Field )
      return v_Child_File_Field;
   else if( field == c_field_id_Source_Child_File_Field || field == c_field_name_Source_Child_File_Field )
      return v_Source_Child_File_Field;
   else if( field == c_field_id_Source_Child_Rel_Child_Class || field == c_field_name_Source_Child_Rel_Child_Class )
      return v_Source_Child_Rel_Child_Class;
   else if( field == c_field_id_Source_Child_Relationship || field == c_field_name_Source_Child_Relationship )
      return v_Source_Child_Relationship;
   else
      return p_obj->parent_class_type::get_foreign_key_value( field );
}

void Meta_Specification_Copy_Child_Links::impl::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Child_File_Field, v_Child_File_Field ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Source_Child_File_Field, v_Source_Child_File_Field ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Source_Child_Rel_Child_Class, v_Source_Child_Rel_Child_Class ) );
   foreign_key_values.insert( foreign_key_data_value_type( c_field_id_Source_Child_Relationship, v_Source_Child_Relationship ) );
}

void Meta_Specification_Copy_Child_Links::impl::clear( )
{

   v_Child_File_Field = string( );
   if( cp_Child_File_Field )
      p_obj->setup_foreign_key( *cp_Child_File_Field, v_Child_File_Field );

   v_Source_Child_File_Field = string( );
   if( cp_Source_Child_File_Field )
      p_obj->setup_foreign_key( *cp_Source_Child_File_Field, v_Source_Child_File_Field );

   v_Source_Child_Rel_Child_Class = string( );
   if( cp_Source_Child_Rel_Child_Class )
      p_obj->setup_foreign_key( *cp_Source_Child_Rel_Child_Class, v_Source_Child_Rel_Child_Class );

   v_Source_Child_Relationship = string( );
   if( cp_Source_Child_Relationship )
      p_obj->setup_foreign_key( *cp_Source_Child_Relationship, v_Source_Child_Relationship );
}

bool Meta_Specification_Copy_Child_Links::impl::value_will_be_provided( const string& field_name )
{
   ( void )field_name;

   // [<start value_will_be_provided>]
   // [<finish value_will_be_provided>]

   return false;
}

void Meta_Specification_Copy_Child_Links::impl::validate( unsigned state, bool is_internal, validation_error_container* p_validation_errors )
{
   ( void )state;
   ( void )is_internal;

   if( !p_validation_errors )
      throw runtime_error( "unexpected null validation_errors container" );

   if( v_Source_Child_Rel_Child_Class.empty( ) && !value_will_be_provided( c_field_name_Source_Child_Rel_Child_Class ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Source_Child_Rel_Child_Class,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Source_Child_Rel_Child_Class ) ) ) ) );

   if( v_Source_Child_Relationship.empty( ) && !value_will_be_provided( c_field_name_Source_Child_Relationship ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Source_Child_Relationship,
       get_string_message( GS( c_str_field_must_not_be_empty ), make_pair(
       c_str_parm_field_must_not_be_empty_field, get_module_string( c_field_display_name_Source_Child_Relationship ) ) ) ) );

   // [<start validate>]
//nyi
   if( check_equal( get_obj( ).Specification_Type( ), "trigger_create_children_for_fk_children" )
    && to_string( get_obj( ).Class( ) ) != to_string( get_obj( ).Child_Relationship( ).Child_Class( ) ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Child_Relationship,
      "Class and Child Relationship must match." ) );

   if( check_equal( get_obj( ).Specification_Type( ), "trigger_create_children_for_fk_children" )
    && to_string( get_obj( ).Source_Parent( ).Parent_Class( ) ) != to_string( get_obj( ).Source_Child_Relationship( ).Child_Class( ) ) )
      p_validation_errors->insert( validation_error_value_type( c_field_name_Source_Child_Relationship,
      "Source Parent and Source Child Relationship must match." ) );
   // [<finish validate>]
}

void Meta_Specification_Copy_Child_Links::impl::after_fetch( )
{
   set< string > required_transients;

   p_obj->get_required_field_names( required_transients, true );

   if( cp_Child_File_Field )
      p_obj->setup_foreign_key( *cp_Child_File_Field, v_Child_File_Field );

   if( cp_Source_Child_File_Field )
      p_obj->setup_foreign_key( *cp_Source_Child_File_Field, v_Source_Child_File_Field );

   if( cp_Source_Child_Rel_Child_Class )
      p_obj->setup_foreign_key( *cp_Source_Child_Rel_Child_Class, v_Source_Child_Rel_Child_Class );

   if( cp_Source_Child_Relationship )
      p_obj->setup_foreign_key( *cp_Source_Child_Relationship, v_Source_Child_Relationship );

   // [(start field_from_search_replace)]
   if( !get_obj( ).get_key( ).empty( )
    && ( get_obj( ).needs_field_value( "Vars" )
    || required_transients.count( "Vars" ) ) )
   {
      string str( get_obj( ).Specification_Type( ).Specification_Vars( ) );

      get_obj( ).Vars( str );

      get_obj( ).add_search_replacement( "Vars", "{cffield}", to_string( get_obj( ).Child_File_Field( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{schild}", to_string( get_obj( ).Source_Child_Relationship( ).Child_Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{screlclass}", to_string( get_obj( ).Source_Child_Rel_Child_Class( ).Name( ) ) );
      get_obj( ).add_search_replacement( "Vars", "{scffield}", to_string( get_obj( ).Source_Child_File_Field( ).Name( ) ) );
   }
   // [(finish field_from_search_replace)]

   // [<start after_fetch>]
   // [<finish after_fetch>]
}

void Meta_Specification_Copy_Child_Links::impl::finalise_fetch( )
{
   set< string > required_transients;

   p_obj->get_required_field_names( required_transients, true );

   // [<start finalise_fetch>]
   // [<finish finalise_fetch>]
}

void Meta_Specification_Copy_Child_Links::impl::at_create( )
{
   // [<start at_create>]
   // [<finish at_create>]
}

void Meta_Specification_Copy_Child_Links::impl::to_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   uint64_t state = p_obj->get_state( );
   ( void )state;

   // [(start field_from_other_field)]
   get_obj( ).Source_Child_Rel_Child_Class( get_obj( ).Source_Child_Relationship( ).Child_Class( ) );
   // [(finish field_from_other_field)]

   // [(start field_from_search_replace)]
   if( get_obj( ).get_is_editing( ) )
   {
      string str( get_obj( ).Specification_Type( ).Specification_Name( ) );

      get_obj( ).Name( str );

      get_obj( ).add_search_replacement( "Name", "{schild}", to_string( get_obj( ).Source_Child_Relationship( ).Child_Name( ) ) );
      get_obj( ).add_search_replacement( "Name", "{screlclass}", to_string( get_obj( ).Source_Child_Rel_Child_Class( ).Name( ) ) );

      get_obj( ).set_search_replace_separator( "Name", '_' );

      get_obj( ).set_search_replace_has_opt_prefixing( "Name" );
   }
   // [(finish field_from_search_replace)]

   // [<start to_store>]
   // [<finish to_store>]
}

void Meta_Specification_Copy_Child_Links::impl::for_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   // [<start for_store>]
   // [<finish for_store>]
}

void Meta_Specification_Copy_Child_Links::impl::after_store( bool is_create, bool is_internal )
{
   ( void )is_create;
   ( void )is_internal;

   // [<start after_store>]
//nyi
   if( is_create )
   {
      get_obj( ).child_Specification_Parent( ).op_create( get_obj( ).get_key( ) + "_L" );
      get_obj( ).child_Specification_Parent( ).Parent_Specification( get_obj( ).get_key( ) );
      get_obj( ).child_Specification_Parent( ).Parent_Specification_Copy_Child_Links( get_obj( ).get_key( ) );

      get_obj( ).child_Specification_Parent( ).Class( get_obj( ).Child_Relationship( ).Child_Class( ) );

      if( get_obj( ).Specification_Type( ).get_key( ) == "trigger_create_children_for_fk_children" )
         get_obj( ).child_Specification_Parent( ).Specification_Type( "field_list" );
      else if( get_obj( ).Specification_Type( ).get_key( ) == "trigger_copy_children_from_fk" )
      {
         get_obj( ).child_Specification_Parent( ).Specification_Type( "field_pair2_list" );
         get_obj( ).child_Specification_Parent( ).Source_Class( get_obj( ).Source_Child_Rel_Child_Class( ) );
      }
      else
         throw runtime_error( "unexpected Specification Type '" + get_obj( ).Specification_Type( ).Name( ) + "'" );

      get_obj( ).child_Specification_Parent( ).op_apply( );
   }
   // [<finish after_store>]
}

bool Meta_Specification_Copy_Child_Links::impl::can_destroy( bool is_internal )
{
   bool retval = is_internal || !( get_state( ) & c_state_undeletable );

   // [<start can_destroy>]
   // [<finish can_destroy>]

   return retval;
}

void Meta_Specification_Copy_Child_Links::impl::for_destroy( bool is_internal )
{
   ( void )is_internal;

   // [<start for_destroy>]
   // [<finish for_destroy>]
}

void Meta_Specification_Copy_Child_Links::impl::after_destroy( bool is_internal )
{
   ( void )is_internal;

   // [<start after_destroy>]
   // [<finish after_destroy>]
}

void Meta_Specification_Copy_Child_Links::impl::set_default_values( )
{
   clear( );
}

bool Meta_Specification_Copy_Child_Links::impl::is_filtered( ) const
{
   // [<start is_filtered>]
   // [<finish is_filtered>]

   return false;
}

#undef MODULE_TRACE
#define MODULE_TRACE( x ) trace( x )

Meta_Specification_Copy_Child_Links::Meta_Specification_Copy_Child_Links( )
{
   set_version( c_version );

   p_impl = new impl( *this );
}

Meta_Specification_Copy_Child_Links::~Meta_Specification_Copy_Child_Links( )
{
   delete p_impl;
}

Meta_Field& Meta_Specification_Copy_Child_Links::Child_File_Field( )
{
   return p_impl->impl_Child_File_Field( );
}

const Meta_Field& Meta_Specification_Copy_Child_Links::Child_File_Field( ) const
{
   return p_impl->impl_Child_File_Field( );
}

void Meta_Specification_Copy_Child_Links::Child_File_Field( const string& key )
{
   p_impl->impl_Child_File_Field( key );
}

Meta_Field& Meta_Specification_Copy_Child_Links::Source_Child_File_Field( )
{
   return p_impl->impl_Source_Child_File_Field( );
}

const Meta_Field& Meta_Specification_Copy_Child_Links::Source_Child_File_Field( ) const
{
   return p_impl->impl_Source_Child_File_Field( );
}

void Meta_Specification_Copy_Child_Links::Source_Child_File_Field( const string& key )
{
   p_impl->impl_Source_Child_File_Field( key );
}

Meta_Class& Meta_Specification_Copy_Child_Links::Source_Child_Rel_Child_Class( )
{
   return p_impl->impl_Source_Child_Rel_Child_Class( );
}

const Meta_Class& Meta_Specification_Copy_Child_Links::Source_Child_Rel_Child_Class( ) const
{
   return p_impl->impl_Source_Child_Rel_Child_Class( );
}

void Meta_Specification_Copy_Child_Links::Source_Child_Rel_Child_Class( const string& key )
{
   p_impl->impl_Source_Child_Rel_Child_Class( key );
}

Meta_Relationship& Meta_Specification_Copy_Child_Links::Source_Child_Relationship( )
{
   return p_impl->impl_Source_Child_Relationship( );
}

const Meta_Relationship& Meta_Specification_Copy_Child_Links::Source_Child_Relationship( ) const
{
   return p_impl->impl_Source_Child_Relationship( );
}

void Meta_Specification_Copy_Child_Links::Source_Child_Relationship( const string& key )
{
   p_impl->impl_Source_Child_Relationship( key );
}

Meta_Specification& Meta_Specification_Copy_Child_Links::child_Specification_Parent( )
{
   return p_impl->impl_child_Specification_Parent( );
}

const Meta_Specification& Meta_Specification_Copy_Child_Links::child_Specification_Parent( ) const
{
   return p_impl->impl_child_Specification_Parent( );
}

string Meta_Specification_Copy_Child_Links::get_field_value( int field ) const
{
   int num_parent_fields( parent_class_type::get_num_fields( ) );

   if( field < num_parent_fields )
      return parent_class_type::get_field_value( field );
   else
      return p_impl->get_field_value( field - num_parent_fields );
}

void Meta_Specification_Copy_Child_Links::set_field_value( int field, const string& value )
{
   int num_parent_fields( parent_class_type::get_num_fields( ) );

   if( field < num_parent_fields )
      parent_class_type::set_field_value( field, value );
   else
      p_impl->set_field_value( field - num_parent_fields, value );
}

bool Meta_Specification_Copy_Child_Links::is_field_transient( int field ) const
{
   int num_parent_fields( parent_class_type::get_num_fields( ) );

   if( field < num_parent_fields )
      return parent_class_type::is_field_transient( field );
   else
      return static_is_field_transient( ( field_id )( field - num_parent_fields + 1 ) );
}

string Meta_Specification_Copy_Child_Links::get_field_name( int field ) const
{
   int num_parent_fields( parent_class_type::get_num_fields( ) );

   if( field < num_parent_fields )
      return parent_class_type::get_field_name( field );
   else
      return static_get_field_name( ( field_id )( field - num_parent_fields + 1 ) );
}

int Meta_Specification_Copy_Child_Links::get_field_num( const string& field ) const
{
   int rc = static_get_field_num( field );

   if( rc < 0 )
      throw runtime_error( "unknown field name/id '" + field + "' in get_field_num( )" );

   return rc;
}

bool Meta_Specification_Copy_Child_Links::has_field_changed( const string& field ) const
{
   return class_base::has_field_changed( get_field_num( field ) );
}

uint64_t Meta_Specification_Copy_Child_Links::get_state( ) const
{
   uint64_t state = 0;

   state |= parent_class_type::get_state( );
   state |= p_impl->get_state( );

   return state;
}

const string& Meta_Specification_Copy_Child_Links::execute( const string& cmd_and_args )
{
   return p_impl->execute( cmd_and_args );
}

void Meta_Specification_Copy_Child_Links::clear( )
{
   parent_class_type::clear( );
   p_impl->clear( );
}

void Meta_Specification_Copy_Child_Links::validate( unsigned state, bool is_internal )
{
   parent_class_type::validate( state, is_internal );
   p_impl->validate( state, is_internal, &validation_errors );
}

void Meta_Specification_Copy_Child_Links::after_fetch( )
{
   parent_class_type::after_fetch( );
   p_impl->after_fetch( );
}

void Meta_Specification_Copy_Child_Links::finalise_fetch( )
{
   parent_class_type::finalise_fetch( );
   p_impl->finalise_fetch( );
}

void Meta_Specification_Copy_Child_Links::at_create( )
{
   parent_class_type::at_create( );
   p_impl->at_create( );
}

void Meta_Specification_Copy_Child_Links::to_store( bool is_create, bool is_internal )
{
   parent_class_type::to_store( is_create, is_internal );
   p_impl->to_store( is_create, is_internal );
}

void Meta_Specification_Copy_Child_Links::for_store( bool is_create, bool is_internal )
{
   parent_class_type::for_store( is_create, is_internal );
   p_impl->for_store( is_create, is_internal );
}

void Meta_Specification_Copy_Child_Links::after_store( bool is_create, bool is_internal )
{
   parent_class_type::after_store( is_create, is_internal );
   p_impl->after_store( is_create, is_internal );
}

bool Meta_Specification_Copy_Child_Links::can_destroy( bool is_internal )
{
   if( !parent_class_type::can_destroy( is_internal ) )
      return false;
   return p_impl->can_destroy( is_internal );
}

void Meta_Specification_Copy_Child_Links::for_destroy( bool is_internal )
{
   p_impl->for_destroy( is_internal );
   parent_class_type::for_destroy( is_internal );
}

void Meta_Specification_Copy_Child_Links::after_destroy( bool is_internal )
{
   p_impl->after_destroy( is_internal );
   parent_class_type::after_destroy( is_internal );
}

void Meta_Specification_Copy_Child_Links::set_default_values( )
{
   parent_class_type::set_default_values( );
   p_impl->set_default_values( );
}

bool Meta_Specification_Copy_Child_Links::is_filtered( ) const
{
   if( parent_class_type::is_filtered( ) )
      return true;

   return p_impl->is_filtered( );
}

const char* Meta_Specification_Copy_Child_Links::get_field_id(
 const string& name, bool* p_sql_numeric, string* p_type_name ) const
{
   const char* p_id( parent_class_type::get_field_id( name, p_sql_numeric, p_type_name ) );
   if( p_id )
      return p_id;

   if( name.empty( ) )
      throw runtime_error( "unexpected empty field name for get_field_id" );
   else if( name == c_field_name_Child_File_Field )
   {
      p_id = c_field_id_Child_File_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Source_Child_File_Field )
   {
      p_id = c_field_id_Source_Child_File_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Source_Child_Rel_Child_Class )
   {
      p_id = c_field_id_Source_Child_Rel_Child_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( name == c_field_name_Source_Child_Relationship )
   {
      p_id = c_field_id_Source_Child_Relationship;

      if( p_type_name )
         *p_type_name = "Meta_Relationship";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }

   return p_id;
}

const char* Meta_Specification_Copy_Child_Links::get_field_name(
 const string& id, bool* p_sql_numeric, string* p_type_name ) const
{
   const char* p_name( parent_class_type::get_field_name( id, p_sql_numeric, p_type_name ) );

   if( p_name )
      return p_name;

   if( id.empty( ) )
      throw runtime_error( "unexpected empty field id for get_field_name" );
   else if( id == c_field_id_Child_File_Field )
   {
      p_name = c_field_name_Child_File_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Source_Child_File_Field )
   {
      p_name = c_field_name_Source_Child_File_Field;

      if( p_type_name )
         *p_type_name = "Meta_Field";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Source_Child_Rel_Child_Class )
   {
      p_name = c_field_name_Source_Child_Rel_Child_Class;

      if( p_type_name )
         *p_type_name = "Meta_Class";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }
   else if( id == c_field_id_Source_Child_Relationship )
   {
      p_name = c_field_name_Source_Child_Relationship;

      if( p_type_name )
         *p_type_name = "Meta_Relationship";

      if( p_sql_numeric )
         *p_sql_numeric = false;
   }

   return p_name;
}

string Meta_Specification_Copy_Child_Links::get_field_display_name( const string& id ) const
{
   string display_name( parent_class_type::get_field_display_name( id ) );
   if( !display_name.empty( ) )
      return display_name;

   if( id.empty( ) )
      throw runtime_error( "unexpected empty field id for get_field_display_name" );
   else if( id == c_field_id_Child_File_Field )
      display_name = get_module_string( c_field_display_name_Child_File_Field );
   else if( id == c_field_id_Source_Child_File_Field )
      display_name = get_module_string( c_field_display_name_Source_Child_File_Field );
   else if( id == c_field_id_Source_Child_Rel_Child_Class )
      display_name = get_module_string( c_field_display_name_Source_Child_Rel_Child_Class );
   else if( id == c_field_id_Source_Child_Relationship )
      display_name = get_module_string( c_field_display_name_Source_Child_Relationship );

   return display_name;
}

void Meta_Specification_Copy_Child_Links::clear_foreign_key( const string& field )
{
   if( has_field( field ) )
      p_impl->clear_foreign_key( field );
   else
      parent_class_type::clear_foreign_key( field );
}

void Meta_Specification_Copy_Child_Links::set_foreign_key_value( const string& field, const string& value )
{
   if( has_field( field ) )
      p_impl->set_foreign_key_value( field, value );
   else
      parent_class_type::set_foreign_key_value( field, value );
}

const string& Meta_Specification_Copy_Child_Links::get_foreign_key_value( const string& field )
{
   if( has_field( field ) )
      return p_impl->get_foreign_key_value( field );
   else
      return parent_class_type::get_foreign_key_value( field );
}

void Meta_Specification_Copy_Child_Links::get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const
{
   parent_class_type::get_foreign_key_values( foreign_key_values );
   p_impl->get_foreign_key_values( foreign_key_values );
}

void Meta_Specification_Copy_Child_Links::setup_foreign_key( Meta_Field& o, const string& value )
{
   static_cast< Meta_Field& >( o ).set_key( value );
}

void Meta_Specification_Copy_Child_Links::setup_foreign_key( Meta_Class& o, const string& value )
{
   static_cast< Meta_Class& >( o ).set_key( value );
}

void Meta_Specification_Copy_Child_Links::setup_foreign_key( Meta_Relationship& o, const string& value )
{
   static_cast< Meta_Relationship& >( o ).set_key( value );
}

void Meta_Specification_Copy_Child_Links::setup_graph_parent( Meta_Specification& o, const string& foreign_key_field )
{
   static_cast< Meta_Specification& >( o ).set_graph_parent( this, foreign_key_field );
}

void Meta_Specification_Copy_Child_Links::setup_graph_parent(
 Meta_Field& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Field& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Field& >( o ).set_key( init_value );
}

void Meta_Specification_Copy_Child_Links::setup_graph_parent(
 Meta_Class& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Class& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Class& >( o ).set_key( init_value );
}

void Meta_Specification_Copy_Child_Links::setup_graph_parent(
 Meta_Relationship& o, const string& foreign_key_field, const string& init_value )
{
   static_cast< Meta_Relationship& >( o ).set_graph_parent( this, foreign_key_field, true );
   static_cast< Meta_Relationship& >( o ).set_key( init_value );
}

size_t Meta_Specification_Copy_Child_Links::get_total_child_relationships( ) const
{
   return p_impl->total_child_relationships;
}

void Meta_Specification_Copy_Child_Links::set_total_child_relationships( size_t new_total_child_relationships ) const
{
   p_impl->total_child_relationships = new_total_child_relationships;
}

size_t Meta_Specification_Copy_Child_Links::get_num_foreign_key_children( bool is_internal ) const
{
   size_t rc;
   if( is_internal )
      rc = 1;
   else
      rc = Meta_Specification::get_num_foreign_key_children( ) + 1;

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

class_base* Meta_Specification_Copy_Child_Links::get_next_foreign_key_child(
 size_t child_num, string& next_child_field, cascade_op op, bool is_internal )
{
   class_base* p_class_base = 0;

   size_t num_parent_children = is_internal ? 0 : Meta_Specification::get_total_child_relationships( );

   if( child_num < num_parent_children )
      p_class_base = Meta_Specification::get_next_foreign_key_child( child_num, next_child_field, op );
   else
   {
      if( child_num >= num_parent_children + 1 )
      {
         external_aliases_lookup_const_iterator ealci = g_external_aliases_lookup.lower_bound( child_num );
         if( ealci == g_external_aliases_lookup.end( ) || ealci->first > child_num )
            --ealci;

         p_class_base = ealci->second->get_next_foreign_key_child( child_num - ealci->first, next_child_field, op, true );
      }
      else
      {
         switch( child_num - num_parent_children )
         {
            case 0:
            if( op == e_cascade_op_destroy )
            {
               next_child_field = "302550";
               p_class_base = &child_Specification_Parent( );
            }
            break;
         }
      }
   }

   return p_class_base;
}

const char* Meta_Specification_Copy_Child_Links::class_id( ) const
{
   return static_class_id( );
}

const char* Meta_Specification_Copy_Child_Links::class_name( ) const
{
   return static_class_name( );
}

const char* Meta_Specification_Copy_Child_Links::plural_name( ) const
{
   return static_plural_name( );
}

const char* Meta_Specification_Copy_Child_Links::module_id( ) const
{
   return static_module_id( );
}

const char* Meta_Specification_Copy_Child_Links::module_name( ) const
{
   return static_module_name( );
}

string Meta_Specification_Copy_Child_Links::get_display_name( bool plural ) const
{
   string key( plural ? "plural_" : "class_" );
   key += "specification_copy_child_links";

   return get_module_string( key );
}

bool Meta_Specification_Copy_Child_Links::get_is_alias( ) const
{
   return false;
}

void Meta_Specification_Copy_Child_Links::get_alias_base_info( pair< string, string >& alias_base_info ) const
{
   ( void )alias_base_info;
}

void Meta_Specification_Copy_Child_Links::get_base_class_info( vector< pair< string, string > >& base_class_info ) const
{
   Meta_Specification::get_base_class_info( base_class_info );

   base_class_info.push_back( make_pair( Meta_Specification::class_id( ), Meta_Specification::class_name( ) ) );
}

class_base& Meta_Specification_Copy_Child_Links::get_or_create_graph_child( const string& context )
{
   class_base* p_class_base( 0 );

   string::size_type pos = context.find( '.' );
   string sub_context( context.substr( 0, pos ) );

   if( sub_context.empty( ) )
      throw runtime_error( "unexpected empty sub-context" );
   else if( sub_context == "_302550" || sub_context == "child_Specification_Parent" )
      p_class_base = &child_Specification_Parent( );
   else if( sub_context == c_field_id_Child_File_Field || sub_context == c_field_name_Child_File_Field )
      p_class_base = &Child_File_Field( );
   else if( sub_context == c_field_id_Source_Child_File_Field || sub_context == c_field_name_Source_Child_File_Field )
      p_class_base = &Source_Child_File_Field( );
   else if( sub_context == c_field_id_Source_Child_Rel_Child_Class || sub_context == c_field_name_Source_Child_Rel_Child_Class )
      p_class_base = &Source_Child_Rel_Child_Class( );
   else if( sub_context == c_field_id_Source_Child_Relationship || sub_context == c_field_name_Source_Child_Relationship )
      p_class_base = &Source_Child_Relationship( );
   else
      return parent_class_type::get_or_create_graph_child( context );

   if( !p_class_base )
      throw runtime_error( "unknown sub-context '" + sub_context + "'" );

   if( pos != string::npos )
      p_class_base = &p_class_base->get_or_create_graph_child( context.substr( pos + 1 ) );

   return *p_class_base;
}

void Meta_Specification_Copy_Child_Links::get_sql_column_names(
 vector< string >& names, bool* p_done, const string* p_class_name ) const
{
   parent_class_type::get_sql_column_names( names, p_done, p_class_name );

   if( p_done && *p_done )
      return;

   names.push_back( "C_Child_File_Field" );
   names.push_back( "C_Source_Child_File_Field" );
   names.push_back( "C_Source_Child_Rel_Child_Class" );
   names.push_back( "C_Source_Child_Relationship" );

   if( p_done && p_class_name && *p_class_name == static_class_name( ) )
      *p_done = true;
}

void Meta_Specification_Copy_Child_Links::get_sql_column_values(
 vector< string >& values, bool* p_done, const string* p_class_name ) const
{
   parent_class_type::get_sql_column_values( values, p_done, p_class_name );

   if( p_done && *p_done )
      return;

   values.push_back( sql_quote( to_string( Child_File_Field( ) ) ) );
   values.push_back( sql_quote( to_string( Source_Child_File_Field( ) ) ) );
   values.push_back( sql_quote( to_string( Source_Child_Rel_Child_Class( ) ) ) );
   values.push_back( sql_quote( to_string( Source_Child_Relationship( ) ) ) );

   if( p_done && p_class_name && *p_class_name == static_class_name( ) )
      *p_done = true;
}

void Meta_Specification_Copy_Child_Links::get_required_field_names(
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
      dependents.insert( "Child_File_Field" );

      if( ( required_transients && is_field_transient( e_field_id_Child_File_Field ) )
       || ( !required_transients && !is_field_transient( e_field_id_Child_File_Field ) ) )
         names.insert( "Child_File_Field" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Source_Child_Relationship" );

      if( ( required_transients && is_field_transient( e_field_id_Source_Child_Relationship ) )
       || ( !required_transients && !is_field_transient( e_field_id_Source_Child_Relationship ) ) )
         names.insert( "Source_Child_Relationship" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Source_Child_Rel_Child_Class" );

      if( ( required_transients && is_field_transient( e_field_id_Source_Child_Rel_Child_Class ) )
       || ( !required_transients && !is_field_transient( e_field_id_Source_Child_Rel_Child_Class ) ) )
         names.insert( "Source_Child_Rel_Child_Class" );
   }

   if( needs_field_value( "Vars", dependents ) )
   {
      dependents.insert( "Source_Child_File_Field" );

      if( ( required_transients && is_field_transient( e_field_id_Source_Child_File_Field ) )
       || ( !required_transients && !is_field_transient( e_field_id_Source_Child_File_Field ) ) )
         names.insert( "Source_Child_File_Field" );
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

void Meta_Specification_Copy_Child_Links::get_always_required_field_names(
 set< string >& names, bool required_transients, set< string >& dependents ) const
{

   parent_class_type::get_always_required_field_names( names, required_transients, dependents );

   // [(start modifier_field_value)]
   dependents.insert( "Specification_Type" ); // (for Hide_File_Fields modifier)

   if( ( required_transients && is_field_transient( e_field_id_Specification_Type ) )
    || ( !required_transients && !is_field_transient( e_field_id_Specification_Type ) ) )
      names.insert( "Specification_Type" );
   // [(finish modifier_field_value)]

   // [<start get_always_required_field_names>]
   // [<finish get_always_required_field_names>]
}

void Meta_Specification_Copy_Child_Links::get_transient_replacement_field_names( const string& name, vector< string >& names ) const
{

   parent_class_type::get_transient_replacement_field_names( name, names );

   // [<start get_transient_replacement_field_names>]
   // [<finish get_transient_replacement_field_names>]
}

void Meta_Specification_Copy_Child_Links::do_generate_sql( generate_sql_type type, vector< string >& sql_stmts ) const
{
   parent_class_type::do_generate_sql( type, sql_stmts );

   generate_sql( static_class_name( ), type, sql_stmts );
}

const char* Meta_Specification_Copy_Child_Links::static_resolved_module_id( )
{
   return static_module_id( );
}

const char* Meta_Specification_Copy_Child_Links::static_resolved_module_name( )
{
   return static_module_name( );
}

const char* Meta_Specification_Copy_Child_Links::static_lock_class_id( )
{
   return parent_class_type::static_lock_class_id( );
}

const char* Meta_Specification_Copy_Child_Links::static_check_class_name( )
{
   return parent_class_type::static_check_class_name( );
}

bool Meta_Specification_Copy_Child_Links::static_has_derivations( )
{
   return !g_derivations.empty( );
}

void Meta_Specification_Copy_Child_Links::static_get_class_info( class_info_container& class_info )
{
   parent_class_type::static_get_class_info( class_info );
   class_info.push_back( "100.132100" );
}

void Meta_Specification_Copy_Child_Links::static_get_field_info( field_info_container& all_field_info )
{
   parent_class_type::static_get_field_info( all_field_info );
   all_field_info.push_back( field_info( "302650", "Child_File_Field", "Meta_Field", false ) );
   all_field_info.push_back( field_info( "302653", "Source_Child_File_Field", "Meta_Field", false ) );
   all_field_info.push_back( field_info( "302652", "Source_Child_Rel_Child_Class", "Meta_Class", true ) );
   all_field_info.push_back( field_info( "302651", "Source_Child_Relationship", "Meta_Relationship", true ) );
}

void Meta_Specification_Copy_Child_Links::static_get_foreign_key_info( foreign_key_info_container& foreign_key_info )
{
   ( void )foreign_key_info;

   parent_class_type::static_get_foreign_key_info( foreign_key_info );

   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Child_File_Field, make_pair( "Meta.132100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Source_Child_File_Field, make_pair( "Meta.132100", "Meta_Field" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Source_Child_Rel_Child_Class, make_pair( "Meta.132100", "Meta_Class" ) ) );
   foreign_key_info.insert( foreign_key_info_value_type( c_field_id_Source_Child_Relationship, make_pair( "Meta.132100", "Meta_Relationship" ) ) );
}

int Meta_Specification_Copy_Child_Links::static_get_num_fields( bool* p_done, const string* p_class_name )
{
   int num_fields( parent_class_type::static_get_num_fields( p_done, p_class_name ) );

   if( p_done && *p_done )
      return num_fields;

   num_fields += c_num_fields;

   if( p_done && p_class_name && *p_class_name == static_class_name( ) )
      *p_done = true;

   return num_fields;
}

bool Meta_Specification_Copy_Child_Links::static_is_field_transient( field_id id )
{
   return is_transient_field( static_get_field_id( id ) );
}

const char* Meta_Specification_Copy_Child_Links::static_get_field_id( field_id id )
{
   const char* p_id = 0;

   int num_parent_fields( parent_class_type::static_get_num_fields( ) );

   if( ( int )id < num_parent_fields )
      p_id = parent_class_type::static_get_field_id( ( parent_class_type::field_id )id );

   switch( id )
   {
      case 1:
      p_id = "302650";
      break;

      case 2:
      p_id = "302653";
      break;

      case 3:
      p_id = "302652";
      break;

      case 4:
      p_id = "302651";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Specification_Copy_Child_Links" );

   return p_id;
}

const char* Meta_Specification_Copy_Child_Links::static_get_field_name( field_id id )
{
   const char* p_id = 0;

   int num_parent_fields( parent_class_type::static_get_num_fields( ) );

   if( ( int )id < num_parent_fields )
      p_id = parent_class_type::static_get_field_id( ( parent_class_type::field_id )id );

   switch( id )
   {
      case 1:
      p_id = "Child_File_Field";
      break;

      case 2:
      p_id = "Source_Child_File_Field";
      break;

      case 3:
      p_id = "Source_Child_Rel_Child_Class";
      break;

      case 4:
      p_id = "Source_Child_Relationship";
      break;
   }

   if( !p_id )
      throw runtime_error( "unknown field id #" + to_string( id ) + " for class Specification_Copy_Child_Links" );

   return p_id;
}

int Meta_Specification_Copy_Child_Links::static_get_field_num( const string& field )
{
   int rc = parent_class_type::static_get_field_num( field );
   if( rc >= 0 )
      return rc;

   rc = parent_class_type::static_get_num_fields( );

   if( field.empty( ) )
      throw runtime_error( "unexpected empty field name/id for static_get_field_num( )" );
   else if( field == c_field_id_Child_File_Field || field == c_field_name_Child_File_Field )
      rc += 1;
   else if( field == c_field_id_Source_Child_File_Field || field == c_field_name_Source_Child_File_Field )
      rc += 2;
   else if( field == c_field_id_Source_Child_Rel_Child_Class || field == c_field_name_Source_Child_Rel_Child_Class )
      rc += 3;
   else if( field == c_field_id_Source_Child_Relationship || field == c_field_name_Source_Child_Relationship )
      rc += 4;

   return rc - 1;
}

procedure_info_container& Meta_Specification_Copy_Child_Links::static_get_procedure_info( )
{
   static procedure_info_container& procedures( parent_class_type::static_get_procedure_info( ) );

   return procedures;
}

string Meta_Specification_Copy_Child_Links::static_get_sql_columns( )
{
   string sql_columns;

   sql_columns += 
    "C_Key_ VARCHAR(64),"
    "C_Ver_ INTEGER NOT NULL,"
    "C_Rev_ INTEGER NOT NULL,"
    "C_Typ_ VARCHAR(24) NOT NULL,"
    "C_Actions VARCHAR(200) NOT NULL,"
    "C_Child_Rel_Child_Class VARCHAR(64) NOT NULL,"
    "C_Child_Rel_Field_Key VARCHAR(200) NOT NULL,"
    "C_Child_Relationship VARCHAR(64) NOT NULL,"
    "C_Child_Specification_Type VARCHAR(64) NOT NULL,"
    "C_Class VARCHAR(64) NOT NULL,"
    "C_Comments VARCHAR(200) NOT NULL,"
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
    "C_Id VARCHAR(200) NOT NULL,"
    "C_Is_Required_For_UI_Gen INTEGER NOT NULL,"
    "C_Model VARCHAR(64) NOT NULL,"
    "C_Modifier VARCHAR(64) NOT NULL,"
    "C_Name VARCHAR(200) NOT NULL,"
    "C_Options VARCHAR(200) NOT NULL,"
    "C_Order VARCHAR(200) NOT NULL,"
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
    "C_Restrict_Values VARCHAR(200) NOT NULL,"
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
    "C_Test_Value VARCHAR(200) NOT NULL,"
    "C_Use_Source_Parent INTEGER NOT NULL,"
    "C_Use_Test_Parent_Child INTEGER NOT NULL,"
    "C_Value VARCHAR(200) NOT NULL,"
    "C_Value_Label VARCHAR(200) NOT NULL,"
    "C_Value_Left_Part VARCHAR(200) NOT NULL,"
    "C_Value_Literal VARCHAR(200) NOT NULL,"
    "C_Value_Numeric_String VARCHAR(200) NOT NULL,"
    "C_Value_Right_Part VARCHAR(200) NOT NULL,"
    "C_Value_String VARCHAR(200) NOT NULL,"
    "C_Child_File_Field VARCHAR(64) NOT NULL,"
    "C_Source_Child_File_Field VARCHAR(64) NOT NULL,"
    "C_Source_Child_Rel_Child_Class VARCHAR(64) NOT NULL,"
    "C_Source_Child_Relationship VARCHAR(64) NOT NULL,"
    "PRIMARY KEY(C_Key_)";

   return sql_columns;
}

void Meta_Specification_Copy_Child_Links::static_get_text_search_fields( vector< string >& fields )
{
   parent_class_type::static_get_text_search_fields( fields );
}

void Meta_Specification_Copy_Child_Links::static_get_all_enum_pairs( vector< pair< string, string > >& pairs )
{
   parent_class_type::static_get_all_enum_pairs( pairs );
}

void Meta_Specification_Copy_Child_Links::static_get_sql_indexes( vector< string >& indexes )
{
   parent_class_type::static_get_sql_indexes( indexes );
}

void Meta_Specification_Copy_Child_Links::static_get_sql_unique_indexes( vector< string >& indexes )
{
   parent_class_type::static_get_sql_unique_indexes( indexes );
}

void Meta_Specification_Copy_Child_Links::static_insert_derivation( const string& module_and_class_id )
{
   g_derivations.insert( module_and_class_id );
}

void Meta_Specification_Copy_Child_Links::static_remove_derivation( const string& module_and_class_id )
{
   if( g_derivations.count( module_and_class_id ) )
      g_derivations.erase( module_and_class_id );
}

void Meta_Specification_Copy_Child_Links::static_insert_external_alias( const string& module_and_class_id, Meta_Specification_Copy_Child_Links* p_instance )
{
   g_external_aliases.insert( external_aliases_value_type( module_and_class_id, p_instance ) );
}

void Meta_Specification_Copy_Child_Links::static_remove_external_alias( const string& module_and_class_id )
{
   if( g_external_aliases.count( module_and_class_id ) )
   {
      delete g_external_aliases[ module_and_class_id ];
      g_external_aliases.erase( module_and_class_id );
   }
}

void Meta_Specification_Copy_Child_Links::static_class_init( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for init" );

   Meta_Specification::static_insert_derivation( "Meta_Specification_Copy_Child_Links" );

   // [<start static_class_init>]
   // [<finish static_class_init>]
}

void Meta_Specification_Copy_Child_Links::static_class_term( const char* p_module_name )
{
   if( !p_module_name )
      throw runtime_error( "unexpected null module name pointer for term" );

   Meta_Specification::static_remove_derivation( "Meta_Specification_Copy_Child_Links" );

   // [<start static_class_term>]
   // [<finish static_class_term>]
}

