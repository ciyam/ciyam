// Copyright (c) 2008-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2015 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef META_FIELD_H
#  define META_FIELD_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#  endif

#  include "class_base.h"

#  ifdef MODULE_META_IMPL
#     define META_FIELD_DECL_SPEC DYNAMIC_EXPORT
#  else
#     define META_FIELD_DECL_SPEC DYNAMIC_IMPORT
#  endif

class Meta_List_Field;
class Meta_Index;
class Meta_Specification_Field_Action;
class Meta_Specification;
class Meta_Package_Option;
class Meta_Relationship;
class Meta_List;
class Meta_View_Field;
class Meta_Initial_Record_Value;
class Meta_Modifier_Affect;
class Meta_Permission;
class Meta_Class;
class Meta_Enum;
class Meta_Field;
class Meta_Type;

class numeric;

class META_FIELD_DECL_SPEC Meta_Field : public class_base
{
   friend class Meta_List_Field;
   friend class Meta_Index;
   friend class Meta_Specification_Field_Action;
   friend class Meta_Specification;
   friend class Meta_Package_Option;
   friend class Meta_Relationship;
   friend class Meta_List;
   friend class Meta_View_Field;
   friend class Meta_Initial_Record_Value;
   friend class Meta_Modifier_Affect;
   friend class Meta_Permission;
   friend class Meta_Class;
   friend class Meta_Enum;
   friend class Meta_Type;

   public:
   typedef Meta_Field class_type;

   enum field_id
   {
      e_field_id_none = 0,
      e_field_id_Access_Permission = 1,
      e_field_id_Access_Restriction = 2,
      e_field_id_Access_Scope = 3,
      e_field_id_Change_Permission = 4,
      e_field_id_Change_Restriction = 5,
      e_field_id_Change_Scope = 6,
      e_field_id_Class = 7,
      e_field_id_Create_List_Field = 8,
      e_field_id_Create_View_Field = 9,
      e_field_id_Def_Value = 10,
      e_field_id_Default = 11,
      e_field_id_Dummy_1 = 12,
      e_field_id_Encrypted = 13,
      e_field_id_Enum = 14,
      e_field_id_Enum_Filter = 15,
      e_field_id_Extra = 16,
      e_field_id_Extra_Filter = 17,
      e_field_id_Id = 18,
      e_field_id_Internal = 19,
      e_field_id_Is_Foreign_Key = 20,
      e_field_id_Is_Text_Type = 21,
      e_field_id_Mandatory = 22,
      e_field_id_Name = 23,
      e_field_id_Numeric_Decimals = 24,
      e_field_id_Parent_Class = 25,
      e_field_id_Parent_Class_Name = 26,
      e_field_id_Primitive = 27,
      e_field_id_Source_Field = 28,
      e_field_id_Transient = 29,
      e_field_id_Type = 30,
      e_field_id_UOM = 31,
      e_field_id_UOM_Name = 32,
      e_field_id_UOM_Symbol = 33,
      e_field_id_Use_In_Text_Search = 34
   };

   Meta_Field( );
   ~Meta_Field( );

   int Access_Restriction( ) const;
   void Access_Restriction( int Access_Restriction );

   int Access_Scope( ) const;
   void Access_Scope( int Access_Scope );

   int Change_Restriction( ) const;
   void Change_Restriction( int Change_Restriction );

   int Change_Scope( ) const;
   void Change_Scope( int Change_Scope );

   bool Create_List_Field( ) const;
   void Create_List_Field( bool Create_List_Field );

   bool Create_View_Field( ) const;
   void Create_View_Field( bool Create_View_Field );

   const std::string& Def_Value( ) const;
   void Def_Value( const std::string& Def_Value );

   const std::string& Default( ) const;
   void Default( const std::string& Default );

   bool Dummy_1( ) const;
   void Dummy_1( bool Dummy_1 );

   bool Encrypted( ) const;
   void Encrypted( bool Encrypted );

   int Extra( ) const;
   void Extra( int Extra );

   const std::string& Extra_Filter( ) const;
   void Extra_Filter( const std::string& Extra_Filter );

   const std::string& Id( ) const;
   void Id( const std::string& Id );

   bool Internal( ) const;
   void Internal( bool Internal );

   bool Is_Foreign_Key( ) const;
   void Is_Foreign_Key( bool Is_Foreign_Key );

   bool Is_Text_Type( ) const;
   void Is_Text_Type( bool Is_Text_Type );

   bool Mandatory( ) const;
   void Mandatory( bool Mandatory );

   const std::string& Name( ) const;
   void Name( const std::string& Name );

   const numeric& Numeric_Decimals( ) const;
   void Numeric_Decimals( const numeric& Numeric_Decimals );

   const std::string& Parent_Class_Name( ) const;
   void Parent_Class_Name( const std::string& Parent_Class_Name );

   int Primitive( ) const;
   void Primitive( int Primitive );

   bool Transient( ) const;
   void Transient( bool Transient );

   int UOM( ) const;
   void UOM( int UOM );

   const std::string& UOM_Name( ) const;
   void UOM_Name( const std::string& UOM_Name );

   const std::string& UOM_Symbol( ) const;
   void UOM_Symbol( const std::string& UOM_Symbol );

   bool Use_In_Text_Search( ) const;
   void Use_In_Text_Search( bool Use_In_Text_Search );

   Meta_Permission& Access_Permission( );
   const Meta_Permission& Access_Permission( ) const;
   void Access_Permission( const std::string& key );

   Meta_Permission& Change_Permission( );
   const Meta_Permission& Change_Permission( ) const;
   void Change_Permission( const std::string& key );

   Meta_Class& Class( );
   const Meta_Class& Class( ) const;
   void Class( const std::string& key );

   Meta_Enum& Enum( );
   const Meta_Enum& Enum( ) const;
   void Enum( const std::string& key );

   Meta_Enum& Enum_Filter( );
   const Meta_Enum& Enum_Filter( ) const;
   void Enum_Filter( const std::string& key );

   Meta_Class& Parent_Class( );
   const Meta_Class& Parent_Class( ) const;
   void Parent_Class( const std::string& key );

   Meta_Field& Source_Field( );
   const Meta_Field& Source_Field( ) const;
   void Source_Field( const std::string& key );

   Meta_Type& Type( );
   const Meta_Type& Type( ) const;
   void Type( const std::string& key );

   Meta_List_Field& child_List_Field_Child_Rel_Source_Child( );
   const Meta_List_Field& child_List_Field_Child_Rel_Source_Child( ) const;

   Meta_List_Field& child_List_Field_Child_Rel_Source( );
   const Meta_List_Field& child_List_Field_Child_Rel_Source( ) const;

   Meta_List_Field& child_List_Field_Child_Rel_Source_Parent( );
   const Meta_List_Field& child_List_Field_Child_Rel_Source_Parent( ) const;

   Meta_Index& child_Index_Field_1( );
   const Meta_Index& child_Index_Field_1( ) const;

   Meta_Index& child_Index_Field_2( );
   const Meta_Index& child_Index_Field_2( ) const;

   Meta_Index& child_Index_Field_3( );
   const Meta_Index& child_Index_Field_3( ) const;

   Meta_Index& child_Index_Field_4( );
   const Meta_Index& child_Index_Field_4( ) const;

   Meta_Index& child_Index_Field_5( );
   const Meta_Index& child_Index_Field_5( ) const;

   Meta_Specification_Field_Action& child_Specification_Field_Action_New_Record_FK( );
   const Meta_Specification_Field_Action& child_Specification_Field_Action_New_Record_FK( ) const;

   Meta_Specification& child_Specification_Other_Class( );
   const Meta_Specification& child_Specification_Other_Class( ) const;

   Meta_Package_Option& child_Package_Option_Other_Field_2( );
   const Meta_Package_Option& child_Package_Option_Other_Field_2( ) const;

   Meta_Specification& child_Specification_Other_Field_2( );
   const Meta_Specification& child_Specification_Other_Field_2( ) const;

   Meta_Package_Option& child_Package_Option_Other( );
   const Meta_Package_Option& child_Package_Option_Other( ) const;

   Meta_Specification& child_Specification_Other( );
   const Meta_Specification& child_Specification_Other( ) const;

   Meta_Specification& child_Specification_Other_Source_Child_2( );
   const Meta_Specification& child_Specification_Other_Source_Child_2( ) const;

   Meta_Specification& child_Specification_Other_Source_Child( );
   const Meta_Specification& child_Specification_Other_Source_Child( ) const;

   Meta_Package_Option& child_Package_Option_Other_Source( );
   const Meta_Package_Option& child_Package_Option_Other_Source( ) const;

   Meta_Relationship& child_Relationship_Parent_Field_For_List( );
   const Meta_Relationship& child_Relationship_Parent_Field_For_List( ) const;

   Meta_Relationship& child_Relationship_Parent_Field_For_View( );
   const Meta_Relationship& child_Relationship_Parent_Field_For_View( ) const;

   Meta_List& child_List_Parent( );
   const Meta_List& child_List_Parent( ) const;

   Meta_Class& child_Class_Quick_Link( );
   const Meta_Class& child_Class_Quick_Link( ) const;

   Meta_List_Field& child_List_Field_Restriction( );
   const Meta_List_Field& child_List_Field_Restriction( ) const;

   Meta_List_Field& child_List_Field_Source_Child( );
   const Meta_List_Field& child_List_Field_Source_Child( ) const;

   Meta_Specification& child_Specification_Source_Child( );
   const Meta_Specification& child_Specification_Source_Child( ) const;

   Meta_View_Field& child_View_Field_Source_Child( );
   const Meta_View_Field& child_View_Field_Source_Child( ) const;

   Meta_View_Field& child_View_Field_Source_Edit_Child( );
   const Meta_View_Field& child_View_Field_Source_Edit_Child( ) const;

   Meta_List_Field& child_List_Field_Source( );
   const Meta_List_Field& child_List_Field_Source( ) const;

   Meta_Package_Option& child_Package_Option_Source( );
   const Meta_Package_Option& child_Package_Option_Source( ) const;

   Meta_Specification& child_Specification_Source( );
   const Meta_Specification& child_Specification_Source( ) const;

   Meta_View_Field& child_View_Field_Source( );
   const Meta_View_Field& child_View_Field_Source( ) const;

   Meta_List_Field& child_List_Field_Source_Grandchild( );
   const Meta_List_Field& child_List_Field_Source_Grandchild( ) const;

   Meta_Specification& child_Specification_Source_Grandchild( );
   const Meta_Specification& child_Specification_Source_Grandchild( ) const;

   Meta_List_Field& child_List_Field_Source_Parent( );
   const Meta_List_Field& child_List_Field_Source_Parent( ) const;

   Meta_Specification& child_Specification_Source_Parent( );
   const Meta_Specification& child_Specification_Source_Parent( ) const;

   Meta_View_Field& child_View_Field_Source_Parent( );
   const Meta_View_Field& child_View_Field_Source_Parent( ) const;

   Meta_Field& child_Field_Source( );
   const Meta_Field& child_Field_Source( ) const;

   Meta_Specification& child_Specification_Test_Child( );
   const Meta_Specification& child_Specification_Test_Child( ) const;

   Meta_Specification& child_Specification_Test( );
   const Meta_Specification& child_Specification_Test( ) const;

   Meta_Specification& child_Specification_Test_Parent( );
   const Meta_Specification& child_Specification_Test_Parent( ) const;

   Meta_List_Field& child_List_Field_Use_Type( );
   const Meta_List_Field& child_List_Field_Use_Type( ) const;

   Meta_Initial_Record_Value& child_Initial_Record_Value( );
   const Meta_Initial_Record_Value& child_Initial_Record_Value( ) const;

   Meta_Modifier_Affect& child_Modifier_Affect( );
   const Meta_Modifier_Affect& child_Modifier_Affect( ) const;

   Meta_Package_Option& child_Package_Option( );
   const Meta_Package_Option& child_Package_Option( ) const;

   Meta_Specification& child_Specification( );
   const Meta_Specification& child_Specification( ) const;

   virtual void Get_Text_Type( int& Result );

   std::string get_field_value( int field ) const;
   void set_field_value( int field, const std::string& value );

   bool is_field_default( int field ) const;
   bool is_field_default( field_id id ) const;
   bool is_field_default( const std::string& field ) const;

   bool has_field_changed( const std::string& field ) const;

   bool is_field_encrypted( int field ) const;
   bool is_field_encrypted( field_id id ) const { return static_is_field_encrypted( id ); }

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

   size_t get_class_type( ) const { return static_class_type( ); }

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

   std::string& get_order_field_name( ) const;
   std::string& get_owner_field_name( ) const;

   bool is_file_field( const std::string& id_or_name ) const;
   void get_file_field_names( std::vector< std::string >& file_field_names ) const;

   std::string get_field_uom_symbol( const std::string& id_or_name ) const;
   std::string get_field_display_name( const std::string& id_or_name ) const;

   const procedure_info_container& get_procedure_info( ) const { return static_get_procedure_info( ); }

   void get_text_search_fields( std::vector< std::string >& fields ) const { return static_get_text_search_fields( fields ); }

   void get_all_enum_pairs( std::vector< std::pair< std::string, std::string > >& pairs ) const { return static_get_all_enum_pairs( pairs ); }

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

   static const char* static_class_id( ) { return "107100"; }
   static const char* static_class_name( ) { return "Field"; }
   static const char* static_plural_name( ) { return "Fields"; }

   static const char* static_module_id( ) { return "100"; }
   static const char* static_module_name( ) { return "Meta"; }

   static const char* static_resolved_module_id( );
   static const char* static_resolved_module_name( );

   static const char* static_lock_class_id( );
   static const char* static_check_class_name( );

   static size_t static_class_type( ) { return 0; }

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

   static void static_get_all_enum_pairs( std::vector< std::pair< std::string, std::string > >& pairs );

   static void static_get_text_search_fields( std::vector< std::string >& fields );

   static std::string static_get_sql_columns( );

   static void static_get_sql_indexes( std::vector< std::string >& indexes );
   static void static_get_sql_unique_indexes( std::vector< std::string >& indexes );

   static void static_insert_derivation( const std::string& module_and_class_id );
   static void static_remove_derivation( const std::string& module_and_class_id );

   static void static_insert_external_alias( const std::string& module_and_class_id, Meta_Field* p_instance );
   static void static_remove_external_alias( const std::string& module_and_class_id );

   private:
   Meta_Field( const Meta_Field& );
   Meta_Field& operator =( const Meta_Field& );

   struct impl;
   friend struct impl;

   impl* p_impl;

   protected:
   const char* get_field_id( const std::string& name, bool* p_sql_numeric = 0, std::string* p_type_name = 0 ) const;
   const char* get_field_name( const std::string& id, bool* p_sql_numeric = 0, std::string* p_type_name = 0 ) const;

   void get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const;

   virtual void setup_foreign_key( Meta_Permission& o, const std::string& value );
   virtual void setup_foreign_key( Meta_Class& o, const std::string& value );
   virtual void setup_foreign_key( Meta_Enum& o, const std::string& value );
   virtual void setup_foreign_key( Meta_Field& o, const std::string& value );
   virtual void setup_foreign_key( Meta_Type& o, const std::string& value );

   virtual void setup_graph_parent( Meta_List_Field& o, const std::string& foreign_key_field );
   virtual void setup_graph_parent( Meta_Index& o, const std::string& foreign_key_field );
   virtual void setup_graph_parent( Meta_Specification_Field_Action& o, const std::string& foreign_key_field );
   virtual void setup_graph_parent( Meta_Specification& o, const std::string& foreign_key_field );
   virtual void setup_graph_parent( Meta_Package_Option& o, const std::string& foreign_key_field );
   virtual void setup_graph_parent( Meta_Relationship& o, const std::string& foreign_key_field );
   virtual void setup_graph_parent( Meta_List& o, const std::string& foreign_key_field );
   virtual void setup_graph_parent( Meta_Class& o, const std::string& foreign_key_field );
   virtual void setup_graph_parent( Meta_View_Field& o, const std::string& foreign_key_field );
   virtual void setup_graph_parent( Meta_Field& o, const std::string& foreign_key_field );
   virtual void setup_graph_parent( Meta_Initial_Record_Value& o, const std::string& foreign_key_field );
   virtual void setup_graph_parent( Meta_Modifier_Affect& o, const std::string& foreign_key_field );

   virtual void setup_graph_parent( Meta_Permission& o,
    const std::string& foreign_key_field, const std::string& init_value );

   virtual void setup_graph_parent( Meta_Class& o,
    const std::string& foreign_key_field, const std::string& init_value );

   virtual void setup_graph_parent( Meta_Enum& o,
    const std::string& foreign_key_field, const std::string& init_value );

   virtual void setup_graph_parent( Meta_Field& o,
    const std::string& foreign_key_field, const std::string& init_value );

   virtual void setup_graph_parent( Meta_Type& o,
    const std::string& foreign_key_field, const std::string& init_value );

   size_t get_total_child_relationships( ) const;
   void set_total_child_relationships( size_t new_total_child_relationships ) const;

   size_t get_num_foreign_key_children( bool is_internal = false ) const;

   class_base* get_next_foreign_key_child( size_t child_num,
    std::string& next_child_field, cascade_op op, bool is_internal = false );

   void add_extra_fixed_info( std::vector< std::pair< std::string, std::string > >& fixed_info ) const;
   void add_extra_paging_info( std::vector< std::pair< std::string, std::string > >& paging_info ) const;
};

inline std::string to_string( const Meta_Field& c ) { return c.get_key( ); }

inline bool is_null( const Meta_Field& c ) { return c.get_key( ).empty( ); }

inline bool check_equal( const Meta_Field& c, const char* p ) { return c.get_key( ) == p; }
inline bool check_not_equal( const Meta_Field& c, const char* p ) { return !( c.get_key( ) == p ); }

#endif

