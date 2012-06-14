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

#ifndef META_SPECIFICATION_TYPE_H
#  define META_SPECIFICATION_TYPE_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#  endif

#  include "class_base.h"

#  ifdef MODULE_META_IMPL
#     define META_SPECIFICATION_TYPE_DECL_SPEC DYNAMIC_EXPORT
#  else
#     define META_SPECIFICATION_TYPE_DECL_SPEC DYNAMIC_IMPORT
#  endif

class Meta_Specification;
class Meta_Specification_Type;

class META_SPECIFICATION_TYPE_DECL_SPEC Meta_Specification_Type : public class_base
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
      e_field_id_Allow_Other_Field = 10,
      e_field_id_Allow_Other_Field_2 = 11,
      e_field_id_Allow_Permission = 12,
      e_field_id_Allow_Procedure = 13,
      e_field_id_Allow_Source_Child = 14,
      e_field_id_Allow_Source_Child_2 = 15,
      e_field_id_Allow_Source_Class = 16,
      e_field_id_Allow_Source_Field = 17,
      e_field_id_Allow_Source_Grandchild = 18,
      e_field_id_Allow_Source_Parent = 19,
      e_field_id_Allow_Test_Field = 20,
      e_field_id_Allow_Test_Value = 21,
      e_field_id_Allow_Value = 22,
      e_field_id_Child_Prefix = 23,
      e_field_id_Child_Rel_Grandparent_Match = 24,
      e_field_id_Child_Rel_Parent_Match = 25,
      e_field_id_Child_Relationship_Class_Match = 26,
      e_field_id_Child_Specification_Type = 27,
      e_field_id_Default_Child_Vars = 28,
      e_field_id_Field_type = 29,
      e_field_id_Has_Next_Specification_Info = 30,
      e_field_id_Is_Child_Only = 31,
      e_field_id_Is_Required_For_UI_Gen = 32,
      e_field_id_Is_System = 33,
      e_field_id_Name = 34,
      e_field_id_Needs_Child_Relationship = 35,
      e_field_id_Needs_Class = 36,
      e_field_id_Needs_Enum = 37,
      e_field_id_Needs_Enum_Item = 38,
      e_field_id_Needs_Enum_Item_2 = 39,
      e_field_id_Needs_Enum_Item_3 = 40,
      e_field_id_Needs_Enum_Item_4 = 41,
      e_field_id_Needs_Enum_Item_5 = 42,
      e_field_id_Needs_Field = 43,
      e_field_id_Needs_Modifier = 44,
      e_field_id_Needs_Other_Class = 45,
      e_field_id_Needs_Other_Field = 46,
      e_field_id_Needs_Other_Field_2 = 47,
      e_field_id_Needs_Other_Procedure = 48,
      e_field_id_Needs_Other_Procedure_2 = 49,
      e_field_id_Needs_Permission = 50,
      e_field_id_Needs_Procedure = 51,
      e_field_id_Needs_Procedure_Arg = 52,
      e_field_id_Needs_Procedure_Arg_2 = 53,
      e_field_id_Needs_Procedure_Arg_3 = 54,
      e_field_id_Needs_Source_Child = 55,
      e_field_id_Needs_Source_Child_2 = 56,
      e_field_id_Needs_Source_Field = 57,
      e_field_id_Needs_Source_Field_Or_Child = 58,
      e_field_id_Needs_Source_Parent = 59,
      e_field_id_Needs_Test_Field = 60,
      e_field_id_Needs_Test_Value = 61,
      e_field_id_Needs_Value = 62,
      e_field_id_Next_Child_Specification_Type = 63,
      e_field_id_Next_Protect_Child_Rel = 64,
      e_field_id_Next_Protect_Procedure = 65,
      e_field_id_Next_Protect_Source_Parent = 66,
      e_field_id_Next_Specification_Actions = 67,
      e_field_id_Notes = 68,
      e_field_id_Parent_Specification_Type = 69,
      e_field_id_Procedure_Arg_primitive = 70,
      e_field_id_Procedure_Arg_type = 71,
      e_field_id_Protect_Child_Rel_From_Update = 72,
      e_field_id_Protect_Class_From_Edit = 73,
      e_field_id_Protect_Class_From_Update = 74,
      e_field_id_Protect_Procedure_From_Edit = 75,
      e_field_id_Protect_Source_Parent = 76,
      e_field_id_Set_Field_To_Source_Field_Or_Child = 77,
      e_field_id_Source_Field_Needs_Test_Field = 78,
      e_field_id_Source_Field_Type_Match = 79,
      e_field_id_Source_Parent_type = 80,
      e_field_id_Source_type = 81,
      e_field_id_Specification_Actions = 82,
      e_field_id_Specification_Name = 83,
      e_field_id_Specification_Object = 84,
      e_field_id_Specification_Strings = 85,
      e_field_id_Specification_Vars = 86,
      e_field_id_Test_Field_Type_Match = 87,
      e_field_id_Test_Field_type = 88,
      e_field_id_Use_Class_As_Source_Parent_Class = 89,
      e_field_id_Use_Field_Enum = 90,
      e_field_id_Use_Parent_Child_Rel_As_Source_Parent = 91,
      e_field_id_Use_Parent_Child_Rel_For_Class = 92,
      e_field_id_Use_Parent_Class = 93,
      e_field_id_Use_Parent_Field_For_Class = 94,
      e_field_id_Use_Parent_Other_Class_For_Class = 95,
      e_field_id_Use_Parent_Procedure = 96,
      e_field_id_Use_Parent_Source_Class = 97,
      e_field_id_Use_Parent_Source_Field_For_Class = 98,
      e_field_id_Use_Parent_Source_Parent = 99,
      e_field_id_Use_Parent_Source_Parent_For_Class = 100,
      e_field_id_View_Id = 101
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

   bool Allow_Other_Field( ) const;
   void Allow_Other_Field( bool Allow_Other_Field );

   bool Allow_Other_Field_2( ) const;
   void Allow_Other_Field_2( bool Allow_Other_Field_2 );

   bool Allow_Permission( ) const;
   void Allow_Permission( bool Allow_Permission );

   bool Allow_Procedure( ) const;
   void Allow_Procedure( bool Allow_Procedure );

   bool Allow_Source_Child( ) const;
   void Allow_Source_Child( bool Allow_Source_Child );

   bool Allow_Source_Child_2( ) const;
   void Allow_Source_Child_2( bool Allow_Source_Child_2 );

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

   bool Needs_Other_Field( ) const;
   void Needs_Other_Field( bool Needs_Other_Field );

   bool Needs_Other_Field_2( ) const;
   void Needs_Other_Field_2( bool Needs_Other_Field_2 );

   bool Needs_Other_Procedure( ) const;
   void Needs_Other_Procedure( bool Needs_Other_Procedure );

   bool Needs_Other_Procedure_2( ) const;
   void Needs_Other_Procedure_2( bool Needs_Other_Procedure_2 );

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

   bool Needs_Source_Child_2( ) const;
   void Needs_Source_Child_2( bool Needs_Source_Child_2 );

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

   bool Use_Parent_Class( ) const;
   void Use_Parent_Class( bool Use_Parent_Class );

   bool Use_Parent_Field_For_Class( ) const;
   void Use_Parent_Field_For_Class( bool Use_Parent_Field_For_Class );

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

   bool has_field_changed( const std::string& field ) const;

   bool is_field_transient( int field ) const;
   bool is_field_transient( field_id id ) const { return static_is_field_transient( id ); }

   std::string get_field_name( int field ) const;

   int get_field_num( const std::string& field ) const;

   uint64_t get_state( ) const;

   const std::string& execute( const std::string& cmd_and_args );

   void clear( );

   void validate( unsigned state, bool is_internal );
   void validate_set_fields( std::set< std::string >& fields_set );

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

   const char* class_id( ) const;
   const char* class_name( ) const;
   const char* plural_name( ) const;

   const char* module_id( ) const;
   const char* module_name( ) const;

   const char* resolved_module_id( ) const { return static_resolved_module_id( ); }
   const char* resolved_module_name( ) const { return static_resolved_module_name( ); }

   const char* lock_class_id( ) const { return static_lock_class_id( ); }
   const char* check_class_name( ) const { return static_check_class_name( ); }

   std::string get_display_name( bool plural = false ) const;

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

   const procedure_info_container& get_procedure_info( ) const { return static_get_procedure_info( ); }

   void get_text_search_fields( std::vector< std::string >& fields ) const { return static_get_text_search_fields( fields ); }

   void get_all_enum_pairs( std::vector< std::pair< std::string, std::string > >& pairs ) const { return static_get_all_enum_pairs( pairs ); }

   std::string get_sql_columns( ) const { return static_get_sql_columns( ); }

   void get_sql_indexes( std::vector< std::string >& indexes ) const { static_get_sql_indexes( indexes ); }
   void get_sql_unique_indexes( std::vector< std::string >& indexes ) const { static_get_sql_unique_indexes( indexes ); }

   void get_sql_column_names( std::vector< std::string >& names, bool* p_done, const std::string* p_class_name ) const;
   void get_sql_column_values( std::vector< std::string >& values, bool* p_done, const std::string* p_class_name ) const;

   void get_required_field_names( std::set< std::string >& names,
    bool required_transients, std::set< std::string >* p_dependents = 0 ) const;

   void get_always_required_field_names( std::set< std::string >& names,
    bool required_transients, std::set< std::string >& dependents ) const;

   void get_transient_replacement_field_names( const std::string& name, std::vector< std::string >& names ) const;

   void do_generate_sql( generate_sql_type type, std::vector< std::string >& sql_stmts ) const;

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

   static bool static_has_derivations( );

   static void static_get_class_info( class_info_container& class_info );
   static void static_get_field_info( field_info_container& all_field_info );
   static void static_get_foreign_key_info( foreign_key_info_container& foreign_key_info );

   static int static_get_num_fields( bool* p_done = 0, const std::string* p_class_name = 0 );

   static bool static_is_field_transient( field_id id );

   static const char* static_get_field_id( field_id id );
   static const char* static_get_field_name( field_id id );

   static int static_get_field_num( const std::string& field );

   static procedure_info_container& static_get_procedure_info( );

   static void static_get_all_enum_pairs( std::vector< std::pair< std::string, std::string > >& pairs );

   static void static_get_text_search_fields( std::vector< std::string >& fields );

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

   std::string get_field_display_name( const std::string& id ) const;

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
};

inline std::string to_string( const Meta_Specification_Type& c ) { return c.get_key( ); }

inline bool is_null( const Meta_Specification_Type& c ) { return c.get_key( ).empty( ); }

inline bool check_equal( const Meta_Specification_Type& c, const char* p ) { return c.get_key( ) == p; }
inline bool check_not_equal( const Meta_Specification_Type& c, const char* p ) { return !( c.get_key( ) == p ); }

#endif

