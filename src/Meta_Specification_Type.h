// Copyright (c) 2012-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef META_SPECIFICATION_TYPE_H
#  define META_SPECIFICATION_TYPE_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#  endif

#  include "class_base.h"


class Meta_Specification;
class Meta_Specification_Type;

class Meta_Specification_Type : public class_base
{
   friend class Meta_Specification;

   public:
   typedef Meta_Specification_Type class_type;

   enum field_id
   {
      e_field_id_none = 0,
      e_field_id_Allow_Child_Relationship = 1,
      e_field_id_Allow_Enum = 2,
      e_field_id_Allow_Enum_Item = 3,
      e_field_id_Allow_FK_Source_Field_Choice = 4,
      e_field_id_Allow_FK_Test_Field_Choice = 5,
      e_field_id_Allow_Field = 6,
      e_field_id_Allow_Modifier = 7,
      e_field_id_Allow_Options = 8,
      e_field_id_Allow_Other_Class = 9,
      e_field_id_Allow_Other_Class_Field = 10,
      e_field_id_Allow_Other_Field = 11,
      e_field_id_Allow_Other_Field_2 = 12,
      e_field_id_Allow_Other_Modifier = 13,
      e_field_id_Allow_Other_Modifier_2 = 14,
      e_field_id_Allow_Other_Procedure = 15,
      e_field_id_Allow_Other_Procedure_2 = 16,
      e_field_id_Allow_Other_Source_Child = 17,
      e_field_id_Allow_Other_Source_Child_2 = 18,
      e_field_id_Allow_Permission = 19,
      e_field_id_Allow_Procedure = 20,
      e_field_id_Allow_Procedure_Arg = 21,
      e_field_id_Allow_Source_Child = 22,
      e_field_id_Allow_Source_Class = 23,
      e_field_id_Allow_Source_Field = 24,
      e_field_id_Allow_Source_Grandchild = 25,
      e_field_id_Allow_Source_Parent = 26,
      e_field_id_Allow_Test_Field = 27,
      e_field_id_Allow_Test_Value = 28,
      e_field_id_Allow_Value = 29,
      e_field_id_Can_Access_Below_Model = 30,
      e_field_id_Child_Prefix = 31,
      e_field_id_Child_Rel_Grandparent_Match = 32,
      e_field_id_Child_Rel_Parent_Match = 33,
      e_field_id_Child_Relationship_Class_Match = 34,
      e_field_id_Child_Specification_Type = 35,
      e_field_id_Default_Child_Vars = 36,
      e_field_id_Field_type = 37,
      e_field_id_Has_Next_Specification_Info = 38,
      e_field_id_Is_Child_Only = 39,
      e_field_id_Is_Required_For_UI_Gen = 40,
      e_field_id_Is_System = 41,
      e_field_id_Name = 42,
      e_field_id_Needs_Child_Relationship = 43,
      e_field_id_Needs_Class = 44,
      e_field_id_Needs_Enum = 45,
      e_field_id_Needs_Enum_Item = 46,
      e_field_id_Needs_Enum_Item_2 = 47,
      e_field_id_Needs_Enum_Item_3 = 48,
      e_field_id_Needs_Enum_Item_4 = 49,
      e_field_id_Needs_Enum_Item_5 = 50,
      e_field_id_Needs_Field = 51,
      e_field_id_Needs_Modifier = 52,
      e_field_id_Needs_Other_Class = 53,
      e_field_id_Needs_Other_Class_Field = 54,
      e_field_id_Needs_Other_Field = 55,
      e_field_id_Needs_Other_Field_2 = 56,
      e_field_id_Needs_Other_Modifier = 57,
      e_field_id_Needs_Other_Modifier_2 = 58,
      e_field_id_Needs_Other_Permission = 59,
      e_field_id_Needs_Other_Permission_2 = 60,
      e_field_id_Needs_Other_Procedure = 61,
      e_field_id_Needs_Other_Procedure_2 = 62,
      e_field_id_Needs_Other_Source_Child = 63,
      e_field_id_Needs_Other_Source_Child_2 = 64,
      e_field_id_Needs_Permission = 65,
      e_field_id_Needs_Procedure = 66,
      e_field_id_Needs_Procedure_Arg = 67,
      e_field_id_Needs_Procedure_Arg_2 = 68,
      e_field_id_Needs_Procedure_Arg_3 = 69,
      e_field_id_Needs_Source_Child = 70,
      e_field_id_Needs_Source_Field = 71,
      e_field_id_Needs_Source_Field_Or_Child = 72,
      e_field_id_Needs_Source_Parent = 73,
      e_field_id_Needs_Test_Field = 74,
      e_field_id_Needs_Test_Value = 75,
      e_field_id_Needs_Value = 76,
      e_field_id_Next_Child_Specification_Type = 77,
      e_field_id_Next_Protect_Child_Rel = 78,
      e_field_id_Next_Protect_Procedure = 79,
      e_field_id_Next_Protect_Source_Parent = 80,
      e_field_id_Next_Specification_Actions = 81,
      e_field_id_Notes = 82,
      e_field_id_Parent_Specification_Type = 83,
      e_field_id_Procedure_Arg_primitive = 84,
      e_field_id_Procedure_Arg_type = 85,
      e_field_id_Protect_Child_Rel_From_Update = 86,
      e_field_id_Protect_Class_From_Edit = 87,
      e_field_id_Protect_Class_From_Update = 88,
      e_field_id_Protect_Other_Class_From_Edit = 89,
      e_field_id_Protect_Procedure_From_Edit = 90,
      e_field_id_Protect_Source_Parent = 91,
      e_field_id_Set_Field_To_Source_Field_Or_Child = 92,
      e_field_id_Source_Field_Needs_Test_Field = 93,
      e_field_id_Source_Field_Type_Match = 94,
      e_field_id_Source_Parent_type = 95,
      e_field_id_Source_type = 96,
      e_field_id_Specification_Actions = 97,
      e_field_id_Specification_Name = 98,
      e_field_id_Specification_Object = 99,
      e_field_id_Specification_Strings = 100,
      e_field_id_Specification_Vars = 101,
      e_field_id_Test_Field_Type_Match = 102,
      e_field_id_Test_Field_type = 103,
      e_field_id_Use_Class_As_Source_Parent_Class = 104,
      e_field_id_Use_Field_Enum = 105,
      e_field_id_Use_Parent_Child_Rel_As_Source_Parent = 106,
      e_field_id_Use_Parent_Child_Rel_For_Class = 107,
      e_field_id_Use_Parent_Child_Rel_For_Other_Class = 108,
      e_field_id_Use_Parent_Class = 109,
      e_field_id_Use_Parent_Field_For_Class = 110,
      e_field_id_Use_Parent_Other_Class = 111,
      e_field_id_Use_Parent_Other_Class_For_Class = 112,
      e_field_id_Use_Parent_Procedure = 113,
      e_field_id_Use_Parent_Source_Class = 114,
      e_field_id_Use_Parent_Source_Field_For_Class = 115,
      e_field_id_Use_Parent_Source_Parent = 116,
      e_field_id_Use_Parent_Source_Parent_For_Class = 117,
      e_field_id_Use_Source_Field_Enum = 118,
      e_field_id_Use_Source_Field_Or_Child_Enum = 119,
      e_field_id_View_Id = 120
   };

   Meta_Specification_Type( );
   ~Meta_Specification_Type( );

   bool Allow_Child_Relationship( ) const;
   void Allow_Child_Relationship( bool Allow_Child_Relationship );

   bool Allow_Enum( ) const;
   void Allow_Enum( bool Allow_Enum );

   bool Allow_Enum_Item( ) const;
   void Allow_Enum_Item( bool Allow_Enum_Item );

   bool Allow_FK_Source_Field_Choice( ) const;
   void Allow_FK_Source_Field_Choice( bool Allow_FK_Source_Field_Choice );

   bool Allow_FK_Test_Field_Choice( ) const;
   void Allow_FK_Test_Field_Choice( bool Allow_FK_Test_Field_Choice );

   bool Allow_Field( ) const;
   void Allow_Field( bool Allow_Field );

   bool Allow_Modifier( ) const;
   void Allow_Modifier( bool Allow_Modifier );

   bool Allow_Options( ) const;
   void Allow_Options( bool Allow_Options );

   bool Allow_Other_Class( ) const;
   void Allow_Other_Class( bool Allow_Other_Class );

   bool Allow_Other_Class_Field( ) const;
   void Allow_Other_Class_Field( bool Allow_Other_Class_Field );

   bool Allow_Other_Field( ) const;
   void Allow_Other_Field( bool Allow_Other_Field );

   bool Allow_Other_Field_2( ) const;
   void Allow_Other_Field_2( bool Allow_Other_Field_2 );

   bool Allow_Other_Modifier( ) const;
   void Allow_Other_Modifier( bool Allow_Other_Modifier );

   bool Allow_Other_Modifier_2( ) const;
   void Allow_Other_Modifier_2( bool Allow_Other_Modifier_2 );

   bool Allow_Other_Procedure( ) const;
   void Allow_Other_Procedure( bool Allow_Other_Procedure );

   bool Allow_Other_Procedure_2( ) const;
   void Allow_Other_Procedure_2( bool Allow_Other_Procedure_2 );

   bool Allow_Other_Source_Child( ) const;
   void Allow_Other_Source_Child( bool Allow_Other_Source_Child );

   bool Allow_Other_Source_Child_2( ) const;
   void Allow_Other_Source_Child_2( bool Allow_Other_Source_Child_2 );

   bool Allow_Permission( ) const;
   void Allow_Permission( bool Allow_Permission );

   bool Allow_Procedure( ) const;
   void Allow_Procedure( bool Allow_Procedure );

   bool Allow_Procedure_Arg( ) const;
   void Allow_Procedure_Arg( bool Allow_Procedure_Arg );

   bool Allow_Source_Child( ) const;
   void Allow_Source_Child( bool Allow_Source_Child );

   bool Allow_Source_Class( ) const;
   void Allow_Source_Class( bool Allow_Source_Class );

   bool Allow_Source_Field( ) const;
   void Allow_Source_Field( bool Allow_Source_Field );

   bool Allow_Source_Grandchild( ) const;
   void Allow_Source_Grandchild( bool Allow_Source_Grandchild );

   bool Allow_Source_Parent( ) const;
   void Allow_Source_Parent( bool Allow_Source_Parent );

   bool Allow_Test_Field( ) const;
   void Allow_Test_Field( bool Allow_Test_Field );

   bool Allow_Test_Value( ) const;
   void Allow_Test_Value( bool Allow_Test_Value );

   bool Allow_Value( ) const;
   void Allow_Value( bool Allow_Value );

   bool Can_Access_Below_Model( ) const;
   void Can_Access_Below_Model( bool Can_Access_Below_Model );

   const std::string& Child_Prefix( ) const;
   void Child_Prefix( const std::string& Child_Prefix );

   bool Child_Rel_Grandparent_Match( ) const;
   void Child_Rel_Grandparent_Match( bool Child_Rel_Grandparent_Match );

   bool Child_Rel_Parent_Match( ) const;
   void Child_Rel_Parent_Match( bool Child_Rel_Parent_Match );

   bool Child_Relationship_Class_Match( ) const;
   void Child_Relationship_Class_Match( bool Child_Relationship_Class_Match );

   const std::string& Default_Child_Vars( ) const;
   void Default_Child_Vars( const std::string& Default_Child_Vars );

   int Field_type( ) const;
   void Field_type( int Field_type );

   bool Has_Next_Specification_Info( ) const;
   void Has_Next_Specification_Info( bool Has_Next_Specification_Info );

   bool Is_Child_Only( ) const;
   void Is_Child_Only( bool Is_Child_Only );

   bool Is_Required_For_UI_Gen( ) const;
   void Is_Required_For_UI_Gen( bool Is_Required_For_UI_Gen );

   bool Is_System( ) const;
   void Is_System( bool Is_System );

   const std::string& Name( ) const;
   void Name( const std::string& Name );

   bool Needs_Child_Relationship( ) const;
   void Needs_Child_Relationship( bool Needs_Child_Relationship );

   bool Needs_Class( ) const;
   void Needs_Class( bool Needs_Class );

   bool Needs_Enum( ) const;
   void Needs_Enum( bool Needs_Enum );

   bool Needs_Enum_Item( ) const;
   void Needs_Enum_Item( bool Needs_Enum_Item );

   bool Needs_Enum_Item_2( ) const;
   void Needs_Enum_Item_2( bool Needs_Enum_Item_2 );

   bool Needs_Enum_Item_3( ) const;
   void Needs_Enum_Item_3( bool Needs_Enum_Item_3 );

   bool Needs_Enum_Item_4( ) const;
   void Needs_Enum_Item_4( bool Needs_Enum_Item_4 );

   bool Needs_Enum_Item_5( ) const;
   void Needs_Enum_Item_5( bool Needs_Enum_Item_5 );

   bool Needs_Field( ) const;
   void Needs_Field( bool Needs_Field );

   bool Needs_Modifier( ) const;
   void Needs_Modifier( bool Needs_Modifier );

   bool Needs_Other_Class( ) const;
   void Needs_Other_Class( bool Needs_Other_Class );

   bool Needs_Other_Class_Field( ) const;
   void Needs_Other_Class_Field( bool Needs_Other_Class_Field );

   bool Needs_Other_Field( ) const;
   void Needs_Other_Field( bool Needs_Other_Field );

   bool Needs_Other_Field_2( ) const;
   void Needs_Other_Field_2( bool Needs_Other_Field_2 );

   bool Needs_Other_Modifier( ) const;
   void Needs_Other_Modifier( bool Needs_Other_Modifier );

   bool Needs_Other_Modifier_2( ) const;
   void Needs_Other_Modifier_2( bool Needs_Other_Modifier_2 );

   bool Needs_Other_Permission( ) const;
   void Needs_Other_Permission( bool Needs_Other_Permission );

   bool Needs_Other_Permission_2( ) const;
   void Needs_Other_Permission_2( bool Needs_Other_Permission_2 );

   bool Needs_Other_Procedure( ) const;
   void Needs_Other_Procedure( bool Needs_Other_Procedure );

   bool Needs_Other_Procedure_2( ) const;
   void Needs_Other_Procedure_2( bool Needs_Other_Procedure_2 );

   bool Needs_Other_Source_Child( ) const;
   void Needs_Other_Source_Child( bool Needs_Other_Source_Child );

   bool Needs_Other_Source_Child_2( ) const;
   void Needs_Other_Source_Child_2( bool Needs_Other_Source_Child_2 );

   bool Needs_Permission( ) const;
   void Needs_Permission( bool Needs_Permission );

   bool Needs_Procedure( ) const;
   void Needs_Procedure( bool Needs_Procedure );

   bool Needs_Procedure_Arg( ) const;
   void Needs_Procedure_Arg( bool Needs_Procedure_Arg );

   bool Needs_Procedure_Arg_2( ) const;
   void Needs_Procedure_Arg_2( bool Needs_Procedure_Arg_2 );

   bool Needs_Procedure_Arg_3( ) const;
   void Needs_Procedure_Arg_3( bool Needs_Procedure_Arg_3 );

   bool Needs_Source_Child( ) const;
   void Needs_Source_Child( bool Needs_Source_Child );

   bool Needs_Source_Field( ) const;
   void Needs_Source_Field( bool Needs_Source_Field );

   bool Needs_Source_Field_Or_Child( ) const;
   void Needs_Source_Field_Or_Child( bool Needs_Source_Field_Or_Child );

   bool Needs_Source_Parent( ) const;
   void Needs_Source_Parent( bool Needs_Source_Parent );

   bool Needs_Test_Field( ) const;
   void Needs_Test_Field( bool Needs_Test_Field );

   bool Needs_Test_Value( ) const;
   void Needs_Test_Value( bool Needs_Test_Value );

   bool Needs_Value( ) const;
   void Needs_Value( bool Needs_Value );

   bool Next_Protect_Child_Rel( ) const;
   void Next_Protect_Child_Rel( bool Next_Protect_Child_Rel );

   bool Next_Protect_Procedure( ) const;
   void Next_Protect_Procedure( bool Next_Protect_Procedure );

   bool Next_Protect_Source_Parent( ) const;
   void Next_Protect_Source_Parent( bool Next_Protect_Source_Parent );

   const std::string& Next_Specification_Actions( ) const;
   void Next_Specification_Actions( const std::string& Next_Specification_Actions );

   const std::string& Notes( ) const;
   void Notes( const std::string& Notes );

   int Procedure_Arg_primitive( ) const;
   void Procedure_Arg_primitive( int Procedure_Arg_primitive );

   int Procedure_Arg_type( ) const;
   void Procedure_Arg_type( int Procedure_Arg_type );

   bool Protect_Child_Rel_From_Update( ) const;
   void Protect_Child_Rel_From_Update( bool Protect_Child_Rel_From_Update );

   bool Protect_Class_From_Edit( ) const;
   void Protect_Class_From_Edit( bool Protect_Class_From_Edit );

   bool Protect_Class_From_Update( ) const;
   void Protect_Class_From_Update( bool Protect_Class_From_Update );

   bool Protect_Other_Class_From_Edit( ) const;
   void Protect_Other_Class_From_Edit( bool Protect_Other_Class_From_Edit );

   bool Protect_Procedure_From_Edit( ) const;
   void Protect_Procedure_From_Edit( bool Protect_Procedure_From_Edit );

   bool Protect_Source_Parent( ) const;
   void Protect_Source_Parent( bool Protect_Source_Parent );

   bool Set_Field_To_Source_Field_Or_Child( ) const;
   void Set_Field_To_Source_Field_Or_Child( bool Set_Field_To_Source_Field_Or_Child );

   bool Source_Field_Needs_Test_Field( ) const;
   void Source_Field_Needs_Test_Field( bool Source_Field_Needs_Test_Field );

   bool Source_Field_Type_Match( ) const;
   void Source_Field_Type_Match( bool Source_Field_Type_Match );

   int Source_Parent_type( ) const;
   void Source_Parent_type( int Source_Parent_type );

   int Source_type( ) const;
   void Source_type( int Source_type );

   const std::string& Specification_Actions( ) const;
   void Specification_Actions( const std::string& Specification_Actions );

   const std::string& Specification_Name( ) const;
   void Specification_Name( const std::string& Specification_Name );

   const std::string& Specification_Object( ) const;
   void Specification_Object( const std::string& Specification_Object );

   const std::string& Specification_Strings( ) const;
   void Specification_Strings( const std::string& Specification_Strings );

   const std::string& Specification_Vars( ) const;
   void Specification_Vars( const std::string& Specification_Vars );

   bool Test_Field_Type_Match( ) const;
   void Test_Field_Type_Match( bool Test_Field_Type_Match );

   int Test_Field_type( ) const;
   void Test_Field_type( int Test_Field_type );

   bool Use_Class_As_Source_Parent_Class( ) const;
   void Use_Class_As_Source_Parent_Class( bool Use_Class_As_Source_Parent_Class );

   bool Use_Field_Enum( ) const;
   void Use_Field_Enum( bool Use_Field_Enum );

   bool Use_Parent_Child_Rel_As_Source_Parent( ) const;
   void Use_Parent_Child_Rel_As_Source_Parent( bool Use_Parent_Child_Rel_As_Source_Parent );

   bool Use_Parent_Child_Rel_For_Class( ) const;
   void Use_Parent_Child_Rel_For_Class( bool Use_Parent_Child_Rel_For_Class );

   bool Use_Parent_Child_Rel_For_Other_Class( ) const;
   void Use_Parent_Child_Rel_For_Other_Class( bool Use_Parent_Child_Rel_For_Other_Class );

   bool Use_Parent_Class( ) const;
   void Use_Parent_Class( bool Use_Parent_Class );

   bool Use_Parent_Field_For_Class( ) const;
   void Use_Parent_Field_For_Class( bool Use_Parent_Field_For_Class );

   bool Use_Parent_Other_Class( ) const;
   void Use_Parent_Other_Class( bool Use_Parent_Other_Class );

   bool Use_Parent_Other_Class_For_Class( ) const;
   void Use_Parent_Other_Class_For_Class( bool Use_Parent_Other_Class_For_Class );

   bool Use_Parent_Procedure( ) const;
   void Use_Parent_Procedure( bool Use_Parent_Procedure );

   bool Use_Parent_Source_Class( ) const;
   void Use_Parent_Source_Class( bool Use_Parent_Source_Class );

   bool Use_Parent_Source_Field_For_Class( ) const;
   void Use_Parent_Source_Field_For_Class( bool Use_Parent_Source_Field_For_Class );

   bool Use_Parent_Source_Parent( ) const;
   void Use_Parent_Source_Parent( bool Use_Parent_Source_Parent );

   bool Use_Parent_Source_Parent_For_Class( ) const;
   void Use_Parent_Source_Parent_For_Class( bool Use_Parent_Source_Parent_For_Class );

   bool Use_Source_Field_Enum( ) const;
   void Use_Source_Field_Enum( bool Use_Source_Field_Enum );

   bool Use_Source_Field_Or_Child_Enum( ) const;
   void Use_Source_Field_Or_Child_Enum( bool Use_Source_Field_Or_Child_Enum );

   const std::string& View_Id( ) const;
   void View_Id( const std::string& View_Id );

   Meta_Specification_Type& Child_Specification_Type( );
   const Meta_Specification_Type& Child_Specification_Type( ) const;
   void Child_Specification_Type( const std::string& key );

   Meta_Specification_Type& Next_Child_Specification_Type( );
   const Meta_Specification_Type& Next_Child_Specification_Type( ) const;
   void Next_Child_Specification_Type( const std::string& key );

   Meta_Specification_Type& Parent_Specification_Type( );
   const Meta_Specification_Type& Parent_Specification_Type( ) const;
   void Parent_Specification_Type( const std::string& key );

   Meta_Specification_Type& child_Specification_Type_Child( );
   const Meta_Specification_Type& child_Specification_Type_Child( ) const;

   Meta_Specification& child_Specification_Child( );
   const Meta_Specification& child_Specification_Child( ) const;

   Meta_Specification_Type& child_Specification_Type_Next_Child( );
   const Meta_Specification_Type& child_Specification_Type_Next_Child( ) const;

   Meta_Specification_Type& child_Specification_Type_Parent( );
   const Meta_Specification_Type& child_Specification_Type_Parent( ) const;

   Meta_Specification& child_Specification( );
   const Meta_Specification& child_Specification( ) const;

   virtual void Is_Valid_Field_Type( const std::string& Class, std::string& Error, const std::string& Parent_Class, int Primitive, int Procedure_Arg_Primitive, int Procedure_Arg_Type, const std::string& Source_Parent_Class, int Source_Primitive, int Test_Primitive, const std::string& Test_Value );

   std::string get_field_value( int field ) const;
   void set_field_value( int field, const std::string& value );

   void set_field_default( int field );
   void set_field_default( field_id id );
   void set_field_default( const std::string& field );

   bool is_field_default( int field ) const;
   bool is_field_default( field_id id ) const;
   bool is_field_default( const std::string& field ) const;

   bool has_field_changed( const std::string& field ) const;

   bool is_field_encrypted( int field ) const;
   bool is_field_encrypted( field_id id ) const { return static_is_field_encrypted( id ); }

   bool is_field_transient( int field ) const;
   bool is_field_transient( field_id id ) const { return static_is_field_transient( id ); }

   std::string get_field_id( int field ) const;
   std::string get_field_name( int field ) const;

   int get_field_num( const std::string& field ) const;

   uint64_t get_state( ) const;

   const std::string& execute( const std::string& cmd_and_args );

   void clear( );

   void validate( uint64_t state, bool is_internal );
   void validate_set_fields( std::set< std::string >& fields_set );

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

   std::string get_class_id( ) const;
   std::string get_class_name( ) const;
   std::string get_plural_name( ) const;

   std::string get_module_id( ) const;
   std::string get_module_name( ) const;

   std::string get_resolved_module_id( ) const { return static_resolved_module_id( ); }
   std::string get_resolved_module_name( ) const { return static_resolved_module_name( ); }

   std::string get_lock_class_id( ) const { return static_lock_class_id( ); }
   std::string get_check_class_name( ) const { return static_check_class_name( ); }

   std::string get_display_name( bool plural = false ) const;

   int get_class_type( ) const { return static_class_type( ); }
   int get_persistence_type( ) const { return static_persistence_type( ); }

   std::string get_persistence_extra( ) const { return static_persistence_extra( ); }

   std::string get_raw_variable( const std::string& name ) const;

   std::string get_create_instance_info( ) const;
   std::string get_update_instance_info( ) const;
   std::string get_destroy_instance_info( ) const;

   std::string get_execute_procedure_info( const std::string& procedure_id ) const;

   bool get_is_alias( ) const;
   void get_alias_base_info( std::pair< std::string, std::string >& alias_base_info ) const;

   void get_base_class_info( std::vector< std::pair< std::string, std::string > >& base_class_info ) const;

   class_base& get_or_create_graph_child( const std::string& context );

   void clear_foreign_key( const std::string& field );
   void set_foreign_key_value( const std::string& field, const std::string& value );

   const std::string& get_foreign_key_value( const std::string& field );

   void get_class_info( class_info_container& class_info ) const { static_get_class_info( class_info ); }
   void get_field_info( field_info_container& field_info ) const { return static_get_field_info( field_info ); }
   void get_foreign_key_info( foreign_key_info_container& foreign_key_info ) const { static_get_foreign_key_info( foreign_key_info ); }

   int get_num_fields( bool* p_done = 0, const std::string* p_class_name = 0 ) const { return static_get_num_fields( p_done, p_class_name ); }

   std::string& get_group_field_name( ) const;
   std::string& get_level_field_name( ) const;
   std::string& get_order_field_name( ) const;
   std::string& get_owner_field_name( ) const;

   bool is_file_field( const std::string& id_or_name ) const;
   void get_file_field_names( std::vector< std::string >& file_field_names ) const;

   std::string get_field_uom_symbol( const std::string& id_or_name ) const;
   std::string get_field_display_name( const std::string& id_or_name ) const;

   const procedure_info_container& get_procedure_info( ) const { return static_get_procedure_info( ); }

   void get_text_search_fields( std::vector< std::string >& fields ) const { return static_get_text_search_fields( fields ); }

   void get_all_enum_pairs( std::vector< std::pair< std::string, std::string > >& pairs ) const { return static_get_all_enum_pairs( pairs ); }

   void get_all_index_pairs( std::vector< std::pair< std::string, std::string > >& pairs ) const { return static_get_all_index_pairs( pairs ); }
   void get_all_unique_indexes( std::vector< std::string >& unique_indexes ) const { return static_get_all_unique_indexes( unique_indexes ); }

   std::string get_sql_columns( ) const { return static_get_sql_columns( ); }

   void get_sql_indexes( std::vector< std::string >& indexes ) const { static_get_sql_indexes( indexes ); }
   void get_sql_unique_indexes( std::vector< std::string >& indexes ) const { static_get_sql_unique_indexes( indexes ); }

   void get_sql_column_names( std::vector< std::string >& names, bool* p_done, const std::string* p_class_name ) const;
   void get_sql_column_values( std::vector< std::string >& values, bool* p_done, const std::string* p_class_name ) const;

   void get_required_field_names( std::set< std::string >& names,
    bool use_transients, std::set< std::string >* p_dependents = 0 ) const;

   void get_always_required_field_names( std::set< std::string >& names,
    bool use_transients, std::set< std::string >& dependents ) const;

   void get_transient_replacement_field_names( const std::string& name, std::vector< std::string >& names ) const;

   void do_generate_sql( generate_sql_type type, std::vector< std::string >& sql_stmts,
    std::set< std::string >& tx_key_info, std::vector< std::string >* p_sql_undo_stmts = 0 ) const;

   static void static_class_init( const char* p_module_name );
   static void static_class_term( const char* p_module_name );

   static const char* static_class_id( ) { return "114100"; }
   static const char* static_class_name( ) { return "Specification_Type"; }
   static const char* static_plural_name( ) { return "Specification_Types"; }

   static const char* static_module_id( ) { return "100"; }
   static const char* static_module_name( ) { return "Meta"; }

   static const char* static_resolved_module_id( );
   static const char* static_resolved_module_name( );

   static const char* static_lock_class_id( );
   static const char* static_check_class_name( );

   static const char* static_persistence_extra( );

   static int static_class_type( ) { return 0; }
   static int static_persistence_type( ) { return 0; }

   static bool static_has_derivations( );

   static void static_get_class_info( class_info_container& class_info );
   static void static_get_field_info( field_info_container& all_field_info );
   static void static_get_foreign_key_info( foreign_key_info_container& foreign_key_info );

   static int static_get_num_fields( bool* p_done = 0, const std::string* p_class_name = 0 );

   static bool static_is_field_encrypted( field_id id );
   static bool static_is_field_transient( field_id id );

   static const char* static_get_field_id( field_id id );
   static const char* static_get_field_name( field_id id );

   static int static_get_field_num( const std::string& field );

   static procedure_info_container& static_get_procedure_info( );

   static void static_get_text_search_fields( std::vector< std::string >& fields );

   static void static_get_all_enum_pairs( std::vector< std::pair< std::string, std::string > >& pairs );

   static void static_get_all_index_pairs( std::vector< std::pair< std::string, std::string > >& pairs );
   static void static_get_all_unique_indexes( std::vector< std::string >& unique_indexes );

   static std::string static_get_sql_columns( );

   static void static_get_sql_indexes( std::vector< std::string >& indexes );
   static void static_get_sql_unique_indexes( std::vector< std::string >& indexes );

   static void static_insert_derivation( const std::string& module_and_class_id );
   static void static_remove_derivation( const std::string& module_and_class_id );

   static void static_insert_external_alias( const std::string& module_and_class_id, Meta_Specification_Type* p_instance );
   static void static_remove_external_alias( const std::string& module_and_class_id );

   private:
   Meta_Specification_Type( const Meta_Specification_Type& );
   Meta_Specification_Type& operator =( const Meta_Specification_Type& );

   struct impl;
   friend struct impl;

   impl* p_impl;

   protected:
   const char* get_field_id( const std::string& name, bool* p_sql_numeric = 0, std::string* p_type_name = 0 ) const;
   const char* get_field_name( const std::string& id, bool* p_sql_numeric = 0, std::string* p_type_name = 0 ) const;

   void get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const;

   virtual void setup_foreign_key( Meta_Specification_Type& o, const std::string& value );

   virtual void setup_graph_parent( Meta_Specification_Type& o, const std::string& foreign_key_field );
   virtual void setup_graph_parent( Meta_Specification& o, const std::string& foreign_key_field );

   virtual void setup_graph_parent( Meta_Specification_Type& o,
    const std::string& foreign_key_field, const std::string& init_value );

   size_t get_total_child_relationships( ) const;
   void set_total_child_relationships( size_t new_total_child_relationships ) const;

   size_t get_num_foreign_key_children( bool is_internal = false ) const;

   class_base* get_next_foreign_key_child( size_t child_num,
    std::string& next_child_field, cascade_op op, bool is_internal = false );

   void add_extra_fixed_info( std::vector< std::pair< std::string, std::string > >& fixed_info ) const;
   void add_extra_paging_info( std::vector< std::pair< std::string, std::string > >& paging_info ) const;
};

inline std::string to_string( const Meta_Specification_Type& c ) { return c.get_key( ); }

inline bool is_null( const Meta_Specification_Type& c ) { return c.get_key( ).empty( ); }

inline bool check_equal( const Meta_Specification_Type& c, const char* p ) { return c.get_key( ) == p; }
inline bool check_not_equal( const Meta_Specification_Type& c, const char* p ) { return !( c.get_key( ) == p ); }

#endif
