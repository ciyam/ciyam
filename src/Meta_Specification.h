// Copyright (c) 2012-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef META_SPECIFICATION_H
#  define META_SPECIFICATION_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#  endif

#  include "class_base.h"


class Meta_List_Field;
class Meta_View_Field;
class Meta_Class;
class Meta_Relationship;
class Meta_Specification_Type;
class Meta_Enum;
class Meta_Enum_Item;
class Meta_Field;
class Meta_Model;
class Meta_Modifier;
class Meta_Permission;
class Meta_Procedure;
class Meta_Specification;
class Meta_Procedure_Arg;

class Meta_Specification : public class_base
{
   friend class Meta_List_Field;
   friend class Meta_View_Field;
   friend class Meta_Class;
   friend class Meta_Relationship;
   friend class Meta_Specification_Type;
   friend class Meta_Enum;
   friend class Meta_Enum_Item;
   friend class Meta_Field;
   friend class Meta_Model;
   friend class Meta_Modifier;
   friend class Meta_Permission;
   friend class Meta_Procedure;
   friend class Meta_Procedure_Arg;

   public:
   typedef Meta_Specification class_type;

   enum field_id
   {
      e_field_id_none = 0,
      e_field_id_Actions = 1,
      e_field_id_All_Strings = 2,
      e_field_id_All_Vars = 3,
      e_field_id_Can_Access_Below_Model = 4,
      e_field_id_Child_Rel_Child_Class = 5,
      e_field_id_Child_Rel_Field_Key = 6,
      e_field_id_Child_Relationship = 7,
      e_field_id_Child_Specification_Type = 8,
      e_field_id_Class = 9,
      e_field_id_Comments = 10,
      e_field_id_Enum = 11,
      e_field_id_Enum_Item = 12,
      e_field_id_Enum_Item_2 = 13,
      e_field_id_Enum_Item_3 = 14,
      e_field_id_Enum_Item_4 = 15,
      e_field_id_Enum_Item_5 = 16,
      e_field_id_Field = 17,
      e_field_id_Field_Class = 18,
      e_field_id_Field_Pairs = 19,
      e_field_id_Field_Values = 20,
      e_field_id_Fields = 21,
      e_field_id_Id = 22,
      e_field_id_Is_Required_For_UI_Gen = 23,
      e_field_id_Model = 24,
      e_field_id_Modifier = 25,
      e_field_id_Name = 26,
      e_field_id_Options = 27,
      e_field_id_Order = 28,
      e_field_id_Other_Class = 29,
      e_field_id_Other_Class_Field = 30,
      e_field_id_Other_Field = 31,
      e_field_id_Other_Field_2 = 32,
      e_field_id_Other_Modifier = 33,
      e_field_id_Other_Modifier_2 = 34,
      e_field_id_Other_Permission = 35,
      e_field_id_Other_Permission_2 = 36,
      e_field_id_Other_Procedure = 37,
      e_field_id_Other_Procedure_2 = 38,
      e_field_id_Other_Source_Child = 39,
      e_field_id_Other_Source_Child_2 = 40,
      e_field_id_Parent_Specification = 41,
      e_field_id_Permission = 42,
      e_field_id_Procedure = 43,
      e_field_id_Procedure_Arg = 44,
      e_field_id_Procedure_Arg_2 = 45,
      e_field_id_Procedure_Arg_3 = 46,
      e_field_id_Protect_Child_Rel = 47,
      e_field_id_Protect_Procedure = 48,
      e_field_id_Protect_Source_Parent = 49,
      e_field_id_Restrict_Values = 50,
      e_field_id_Source_Child = 51,
      e_field_id_Source_Child_Class = 52,
      e_field_id_Source_Class = 53,
      e_field_id_Source_Field = 54,
      e_field_id_Source_Field_Class = 55,
      e_field_id_Source_Grandchild = 56,
      e_field_id_Source_Parent = 57,
      e_field_id_Source_Parent_Class = 58,
      e_field_id_Specification_Type = 59,
      e_field_id_Strings = 60,
      e_field_id_Test_Child = 61,
      e_field_id_Test_Field = 62,
      e_field_id_Test_Field_Class = 63,
      e_field_id_Test_Parent = 64,
      e_field_id_Test_Parent_Class = 65,
      e_field_id_Test_Value = 66,
      e_field_id_Test_Value_Label = 67,
      e_field_id_Use_Source_Parent = 68,
      e_field_id_Use_Test_Parent_Child = 69,
      e_field_id_Value = 70,
      e_field_id_Value_Label = 71,
      e_field_id_Value_Left_Part = 72,
      e_field_id_Value_Literal = 73,
      e_field_id_Value_Numeric_String = 74,
      e_field_id_Value_Right_Part = 75,
      e_field_id_Value_String = 76,
      e_field_id_Vars = 77
   };

   Meta_Specification( );
   ~Meta_Specification( );

   const std::string& Actions( ) const;
   void Actions( const std::string& Actions );

   const std::string& All_Strings( ) const;
   void All_Strings( const std::string& All_Strings );

   const std::string& All_Vars( ) const;
   void All_Vars( const std::string& All_Vars );

   bool Can_Access_Below_Model( ) const;
   void Can_Access_Below_Model( bool Can_Access_Below_Model );

   const std::string& Child_Rel_Field_Key( ) const;
   void Child_Rel_Field_Key( const std::string& Child_Rel_Field_Key );

   const std::string& Comments( ) const;
   void Comments( const std::string& Comments );

   const std::string& Field_Pairs( ) const;
   void Field_Pairs( const std::string& Field_Pairs );

   const std::string& Field_Values( ) const;
   void Field_Values( const std::string& Field_Values );

   const std::string& Fields( ) const;
   void Fields( const std::string& Fields );

   const std::string& Id( ) const;
   void Id( const std::string& Id );

   bool Is_Required_For_UI_Gen( ) const;
   void Is_Required_For_UI_Gen( bool Is_Required_For_UI_Gen );

   const std::string& Name( ) const;
   void Name( const std::string& Name );

   const std::string& Options( ) const;
   void Options( const std::string& Options );

   const std::string& Order( ) const;
   void Order( const std::string& Order );

   bool Protect_Child_Rel( ) const;
   void Protect_Child_Rel( bool Protect_Child_Rel );

   bool Protect_Procedure( ) const;
   void Protect_Procedure( bool Protect_Procedure );

   bool Protect_Source_Parent( ) const;
   void Protect_Source_Parent( bool Protect_Source_Parent );

   const std::string& Restrict_Values( ) const;
   void Restrict_Values( const std::string& Restrict_Values );

   const std::string& Strings( ) const;
   void Strings( const std::string& Strings );

   const std::string& Test_Value( ) const;
   void Test_Value( const std::string& Test_Value );

   const std::string& Test_Value_Label( ) const;
   void Test_Value_Label( const std::string& Test_Value_Label );

   bool Use_Source_Parent( ) const;
   void Use_Source_Parent( bool Use_Source_Parent );

   bool Use_Test_Parent_Child( ) const;
   void Use_Test_Parent_Child( bool Use_Test_Parent_Child );

   const std::string& Value( ) const;
   void Value( const std::string& Value );

   const std::string& Value_Label( ) const;
   void Value_Label( const std::string& Value_Label );

   const std::string& Value_Left_Part( ) const;
   void Value_Left_Part( const std::string& Value_Left_Part );

   const std::string& Value_Literal( ) const;
   void Value_Literal( const std::string& Value_Literal );

   const std::string& Value_Numeric_String( ) const;
   void Value_Numeric_String( const std::string& Value_Numeric_String );

   const std::string& Value_Right_Part( ) const;
   void Value_Right_Part( const std::string& Value_Right_Part );

   const std::string& Value_String( ) const;
   void Value_String( const std::string& Value_String );

   const std::string& Vars( ) const;
   void Vars( const std::string& Vars );

   Meta_Class& Child_Rel_Child_Class( );
   const Meta_Class& Child_Rel_Child_Class( ) const;
   void Child_Rel_Child_Class( const std::string& key );

   Meta_Relationship& Child_Relationship( );
   const Meta_Relationship& Child_Relationship( ) const;
   void Child_Relationship( const std::string& key );

   Meta_Specification_Type& Child_Specification_Type( );
   const Meta_Specification_Type& Child_Specification_Type( ) const;
   void Child_Specification_Type( const std::string& key );

   Meta_Class& Class( );
   const Meta_Class& Class( ) const;
   void Class( const std::string& key );

   Meta_Enum& Enum( );
   const Meta_Enum& Enum( ) const;
   void Enum( const std::string& key );

   Meta_Enum_Item& Enum_Item( );
   const Meta_Enum_Item& Enum_Item( ) const;
   void Enum_Item( const std::string& key );

   Meta_Enum_Item& Enum_Item_2( );
   const Meta_Enum_Item& Enum_Item_2( ) const;
   void Enum_Item_2( const std::string& key );

   Meta_Enum_Item& Enum_Item_3( );
   const Meta_Enum_Item& Enum_Item_3( ) const;
   void Enum_Item_3( const std::string& key );

   Meta_Enum_Item& Enum_Item_4( );
   const Meta_Enum_Item& Enum_Item_4( ) const;
   void Enum_Item_4( const std::string& key );

   Meta_Enum_Item& Enum_Item_5( );
   const Meta_Enum_Item& Enum_Item_5( ) const;
   void Enum_Item_5( const std::string& key );

   Meta_Field& Field( );
   const Meta_Field& Field( ) const;
   void Field( const std::string& key );

   Meta_Class& Field_Class( );
   const Meta_Class& Field_Class( ) const;
   void Field_Class( const std::string& key );

   Meta_Model& Model( );
   const Meta_Model& Model( ) const;
   void Model( const std::string& key );

   Meta_Modifier& Modifier( );
   const Meta_Modifier& Modifier( ) const;
   void Modifier( const std::string& key );

   Meta_Class& Other_Class( );
   const Meta_Class& Other_Class( ) const;
   void Other_Class( const std::string& key );

   Meta_Field& Other_Class_Field( );
   const Meta_Field& Other_Class_Field( ) const;
   void Other_Class_Field( const std::string& key );

   Meta_Field& Other_Field( );
   const Meta_Field& Other_Field( ) const;
   void Other_Field( const std::string& key );

   Meta_Field& Other_Field_2( );
   const Meta_Field& Other_Field_2( ) const;
   void Other_Field_2( const std::string& key );

   Meta_Modifier& Other_Modifier( );
   const Meta_Modifier& Other_Modifier( ) const;
   void Other_Modifier( const std::string& key );

   Meta_Modifier& Other_Modifier_2( );
   const Meta_Modifier& Other_Modifier_2( ) const;
   void Other_Modifier_2( const std::string& key );

   Meta_Permission& Other_Permission( );
   const Meta_Permission& Other_Permission( ) const;
   void Other_Permission( const std::string& key );

   Meta_Permission& Other_Permission_2( );
   const Meta_Permission& Other_Permission_2( ) const;
   void Other_Permission_2( const std::string& key );

   Meta_Procedure& Other_Procedure( );
   const Meta_Procedure& Other_Procedure( ) const;
   void Other_Procedure( const std::string& key );

   Meta_Procedure& Other_Procedure_2( );
   const Meta_Procedure& Other_Procedure_2( ) const;
   void Other_Procedure_2( const std::string& key );

   Meta_Field& Other_Source_Child( );
   const Meta_Field& Other_Source_Child( ) const;
   void Other_Source_Child( const std::string& key );

   Meta_Field& Other_Source_Child_2( );
   const Meta_Field& Other_Source_Child_2( ) const;
   void Other_Source_Child_2( const std::string& key );

   Meta_Specification& Parent_Specification( );
   const Meta_Specification& Parent_Specification( ) const;
   void Parent_Specification( const std::string& key );

   Meta_Permission& Permission( );
   const Meta_Permission& Permission( ) const;
   void Permission( const std::string& key );

   Meta_Procedure& Procedure( );
   const Meta_Procedure& Procedure( ) const;
   void Procedure( const std::string& key );

   Meta_Procedure_Arg& Procedure_Arg( );
   const Meta_Procedure_Arg& Procedure_Arg( ) const;
   void Procedure_Arg( const std::string& key );

   Meta_Procedure_Arg& Procedure_Arg_2( );
   const Meta_Procedure_Arg& Procedure_Arg_2( ) const;
   void Procedure_Arg_2( const std::string& key );

   Meta_Procedure_Arg& Procedure_Arg_3( );
   const Meta_Procedure_Arg& Procedure_Arg_3( ) const;
   void Procedure_Arg_3( const std::string& key );

   Meta_Field& Source_Child( );
   const Meta_Field& Source_Child( ) const;
   void Source_Child( const std::string& key );

   Meta_Class& Source_Child_Class( );
   const Meta_Class& Source_Child_Class( ) const;
   void Source_Child_Class( const std::string& key );

   Meta_Class& Source_Class( );
   const Meta_Class& Source_Class( ) const;
   void Source_Class( const std::string& key );

   Meta_Field& Source_Field( );
   const Meta_Field& Source_Field( ) const;
   void Source_Field( const std::string& key );

   Meta_Class& Source_Field_Class( );
   const Meta_Class& Source_Field_Class( ) const;
   void Source_Field_Class( const std::string& key );

   Meta_Field& Source_Grandchild( );
   const Meta_Field& Source_Grandchild( ) const;
   void Source_Grandchild( const std::string& key );

   Meta_Field& Source_Parent( );
   const Meta_Field& Source_Parent( ) const;
   void Source_Parent( const std::string& key );

   Meta_Class& Source_Parent_Class( );
   const Meta_Class& Source_Parent_Class( ) const;
   void Source_Parent_Class( const std::string& key );

   Meta_Specification_Type& Specification_Type( );
   const Meta_Specification_Type& Specification_Type( ) const;
   void Specification_Type( const std::string& key );

   Meta_Field& Test_Child( );
   const Meta_Field& Test_Child( ) const;
   void Test_Child( const std::string& key );

   Meta_Field& Test_Field( );
   const Meta_Field& Test_Field( ) const;
   void Test_Field( const std::string& key );

   Meta_Class& Test_Field_Class( );
   const Meta_Class& Test_Field_Class( ) const;
   void Test_Field_Class( const std::string& key );

   Meta_Field& Test_Parent( );
   const Meta_Field& Test_Parent( ) const;
   void Test_Parent( const std::string& key );

   Meta_Class& Test_Parent_Class( );
   const Meta_Class& Test_Parent_Class( ) const;
   void Test_Parent_Class( const std::string& key );

   Meta_Specification& child_Specification_Parent( );
   const Meta_Specification& child_Specification_Parent( ) const;

   Meta_List_Field& child_List_Field_Restriction_Spec( );
   const Meta_List_Field& child_List_Field_Restriction_Spec( ) const;

   Meta_View_Field& child_View_Field_Restriction_Spec( );
   const Meta_View_Field& child_View_Field_Restriction_Spec( ) const;

   virtual void Add_Arg_1( );

   virtual void Add_Arg_2( );

   virtual void Add_Child_Info( );

   virtual void Add_Destination_Info( );

   virtual void Add_Extra_Info( );

   virtual void Add_Field( );

   virtual void Add_Field_Info( );

   virtual void Add_Field_Pair( );

   virtual void Add_Grandchild_Info( );

   virtual void Add_Info( );

   virtual void Add_Next( );

   virtual void Add_Opt_Info( );

   virtual void Add_Other_Info( );

   virtual void Add_Secondary( );

   virtual void Add_Source_Info( );

   virtual void Add_Stats_1( );

   virtual void Add_Stats_2( );

   virtual void Add_Stats_Info( );

   virtual void Generate_All_Strings( std::string& All_Strings, const std::string& Parent_Prefix );

   virtual void Generate_All_Vars( std::string& All_Vars, const std::string& Parent_Prefix );

   virtual void Move_Down( const std::string& Restrict_Fields, const std::string& Restrict_Values );

   virtual void Move_Up( const std::string& Restrict_Fields, const std::string& Restrict_Values );

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

   static const char* static_class_id( ) { return "115100"; }
   static const char* static_class_name( ) { return "Specification"; }
   static const char* static_plural_name( ) { return "Specifications"; }

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

   static void static_insert_external_alias( const std::string& module_and_class_id, Meta_Specification* p_instance );
   static void static_remove_external_alias( const std::string& module_and_class_id );

   private:
   Meta_Specification( const Meta_Specification& );
   Meta_Specification& operator =( const Meta_Specification& );

   struct impl;
   friend struct impl;

   impl* p_impl;

   protected:
   const char* get_field_id( const std::string& name, bool* p_sql_numeric = 0, std::string* p_type_name = 0 ) const;
   const char* get_field_name( const std::string& id, bool* p_sql_numeric = 0, std::string* p_type_name = 0 ) const;

   void get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const;

   virtual void setup_foreign_key( Meta_Class& o, const std::string& value );
   virtual void setup_foreign_key( Meta_Relationship& o, const std::string& value );
   virtual void setup_foreign_key( Meta_Specification_Type& o, const std::string& value );
   virtual void setup_foreign_key( Meta_Enum& o, const std::string& value );
   virtual void setup_foreign_key( Meta_Enum_Item& o, const std::string& value );
   virtual void setup_foreign_key( Meta_Field& o, const std::string& value );
   virtual void setup_foreign_key( Meta_Model& o, const std::string& value );
   virtual void setup_foreign_key( Meta_Modifier& o, const std::string& value );
   virtual void setup_foreign_key( Meta_Permission& o, const std::string& value );
   virtual void setup_foreign_key( Meta_Procedure& o, const std::string& value );
   virtual void setup_foreign_key( Meta_Specification& o, const std::string& value );
   virtual void setup_foreign_key( Meta_Procedure_Arg& o, const std::string& value );

   virtual void setup_graph_parent( Meta_Specification& o, const std::string& foreign_key_field );
   virtual void setup_graph_parent( Meta_List_Field& o, const std::string& foreign_key_field );
   virtual void setup_graph_parent( Meta_View_Field& o, const std::string& foreign_key_field );

   virtual void setup_graph_parent( Meta_Class& o,
    const std::string& foreign_key_field, const std::string& init_value );

   virtual void setup_graph_parent( Meta_Relationship& o,
    const std::string& foreign_key_field, const std::string& init_value );

   virtual void setup_graph_parent( Meta_Specification_Type& o,
    const std::string& foreign_key_field, const std::string& init_value );

   virtual void setup_graph_parent( Meta_Enum& o,
    const std::string& foreign_key_field, const std::string& init_value );

   virtual void setup_graph_parent( Meta_Enum_Item& o,
    const std::string& foreign_key_field, const std::string& init_value );

   virtual void setup_graph_parent( Meta_Field& o,
    const std::string& foreign_key_field, const std::string& init_value );

   virtual void setup_graph_parent( Meta_Model& o,
    const std::string& foreign_key_field, const std::string& init_value );

   virtual void setup_graph_parent( Meta_Modifier& o,
    const std::string& foreign_key_field, const std::string& init_value );

   virtual void setup_graph_parent( Meta_Permission& o,
    const std::string& foreign_key_field, const std::string& init_value );

   virtual void setup_graph_parent( Meta_Procedure& o,
    const std::string& foreign_key_field, const std::string& init_value );

   virtual void setup_graph_parent( Meta_Specification& o,
    const std::string& foreign_key_field, const std::string& init_value );

   virtual void setup_graph_parent( Meta_Procedure_Arg& o,
    const std::string& foreign_key_field, const std::string& init_value );

   size_t get_total_child_relationships( ) const;
   void set_total_child_relationships( size_t new_total_child_relationships ) const;

   size_t get_num_foreign_key_children( bool is_internal = false ) const;

   class_base* get_next_foreign_key_child( size_t child_num,
    std::string& next_child_field, cascade_op op, bool is_internal = false );

   void add_extra_fixed_info( std::vector< std::pair< std::string, std::string > >& fixed_info ) const;
   void add_extra_paging_info( std::vector< std::pair< std::string, std::string > >& paging_info ) const;
};

inline std::string to_string( const Meta_Specification& c ) { return c.get_key( ); }

inline bool is_null( const Meta_Specification& c ) { return c.get_key( ).empty( ); }

inline bool check_equal( const Meta_Specification& c, const char* p ) { return c.get_key( ) == p; }
inline bool check_not_equal( const Meta_Specification& c, const char* p ) { return !( c.get_key( ) == p ); }

#endif
