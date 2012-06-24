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

#ifndef META_LIST_FIELD_H
#  define META_LIST_FIELD_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#  endif

#  include "class_base.h"

#  ifdef MODULE_META_IMPL
#     define META_LIST_FIELD_DECL_SPEC DYNAMIC_EXPORT
#  else
#     define META_LIST_FIELD_DECL_SPEC DYNAMIC_IMPORT
#  endif

class Meta_Modifier;
class Meta_Permission;
class Meta_Class;
class Meta_Field;
class Meta_Relationship;
class Meta_List;
class Meta_Procedure;
class Meta_Specification;
class Meta_List_Field_Type;

class META_LIST_FIELD_DECL_SPEC Meta_List_Field : public class_base
{
   friend class Meta_Modifier;
   friend class Meta_Permission;
   friend class Meta_Class;
   friend class Meta_Field;
   friend class Meta_Relationship;
   friend class Meta_List;
   friend class Meta_Procedure;
   friend class Meta_Specification;
   friend class Meta_List_Field_Type;

   public:
   typedef Meta_List_Field class_type;

   enum field_id
   {
      e_field_id_none = 0,
      e_field_id_Access_Parent_Modifier = 1,
      e_field_id_Access_Permission = 2,
      e_field_id_Access_Restriction = 3,
      e_field_id_Alignment = 4,
      e_field_id_Child_Rel_Child_Class = 5,
      e_field_id_Child_Rel_Source_Child = 6,
      e_field_id_Child_Rel_Source_Field = 7,
      e_field_id_Child_Rel_Source_Parent = 8,
      e_field_id_Child_Rel_Source_Parent_Class = 9,
      e_field_id_Child_Relationship = 10,
      e_field_id_Class = 11,
      e_field_id_Exact_Match_Only = 12,
      e_field_id_Exclude_In_Use_FK = 13,
      e_field_id_Font_Size = 14,
      e_field_id_Include_Key_Additions = 15,
      e_field_id_Label_Class = 16,
      e_field_id_Label_Source = 17,
      e_field_id_Label_Without_Prefix = 18,
      e_field_id_Link_Permission = 19,
      e_field_id_Link_Restriction = 20,
      e_field_id_Link_Type = 21,
      e_field_id_List = 22,
      e_field_id_Name = 23,
      e_field_id_Non_Instance_Procedure = 24,
      e_field_id_Notes_Truncation = 25,
      e_field_id_Omit_Versions = 26,
      e_field_id_Order = 27,
      e_field_id_Parent_Class = 28,
      e_field_id_Print_Type = 29,
      e_field_id_Procedure = 30,
      e_field_id_Procedure_Args = 31,
      e_field_id_Restriction_Field = 32,
      e_field_id_Restriction_Spec = 33,
      e_field_id_Restriction_Value = 34,
      e_field_id_Retain_Selected_Rows = 35,
      e_field_id_Reverse_Order = 36,
      e_field_id_Search_Option_Limit = 37,
      e_field_id_Select_Key_Exclusions = 38,
      e_field_id_Source_Child = 39,
      e_field_id_Source_Child_Class = 40,
      e_field_id_Source_Field = 41,
      e_field_id_Source_Grandchild = 42,
      e_field_id_Source_Parent = 43,
      e_field_id_Source_Parent_Class = 44,
      e_field_id_Switch_Type = 45,
      e_field_id_Trigger_Option = 46,
      e_field_id_Type = 47,
      e_field_id_Use_Child_Rel_Source_Parent = 48,
      e_field_id_Use_In_Text_Search_Title = 49,
      e_field_id_Use_Source_Parent = 50
   };

   Meta_List_Field( );
   ~Meta_List_Field( );

   int Access_Restriction( ) const;
   void Access_Restriction( int Access_Restriction );

   int Alignment( ) const;
   void Alignment( int Alignment );

   bool Exact_Match_Only( ) const;
   void Exact_Match_Only( bool Exact_Match_Only );

   bool Exclude_In_Use_FK( ) const;
   void Exclude_In_Use_FK( bool Exclude_In_Use_FK );

   int Font_Size( ) const;
   void Font_Size( int Font_Size );

   const std::string& Include_Key_Additions( ) const;
   void Include_Key_Additions( const std::string& Include_Key_Additions );

   int Label_Class( ) const;
   void Label_Class( int Label_Class );

   int Label_Source( ) const;
   void Label_Source( int Label_Source );

   bool Label_Without_Prefix( ) const;
   void Label_Without_Prefix( bool Label_Without_Prefix );

   int Link_Restriction( ) const;
   void Link_Restriction( int Link_Restriction );

   int Link_Type( ) const;
   void Link_Type( int Link_Type );

   const std::string& Name( ) const;
   void Name( const std::string& Name );

   bool Non_Instance_Procedure( ) const;
   void Non_Instance_Procedure( bool Non_Instance_Procedure );

   int Notes_Truncation( ) const;
   void Notes_Truncation( int Notes_Truncation );

   bool Omit_Versions( ) const;
   void Omit_Versions( bool Omit_Versions );

   const std::string& Order( ) const;
   void Order( const std::string& Order );

   int Print_Type( ) const;
   void Print_Type( int Print_Type );

   const std::string& Procedure_Args( ) const;
   void Procedure_Args( const std::string& Procedure_Args );

   const std::string& Restriction_Value( ) const;
   void Restriction_Value( const std::string& Restriction_Value );

   bool Retain_Selected_Rows( ) const;
   void Retain_Selected_Rows( bool Retain_Selected_Rows );

   bool Reverse_Order( ) const;
   void Reverse_Order( bool Reverse_Order );

   int Search_Option_Limit( ) const;
   void Search_Option_Limit( int Search_Option_Limit );

   const std::string& Select_Key_Exclusions( ) const;
   void Select_Key_Exclusions( const std::string& Select_Key_Exclusions );

   int Switch_Type( ) const;
   void Switch_Type( int Switch_Type );

   int Trigger_Option( ) const;
   void Trigger_Option( int Trigger_Option );

   bool Use_Child_Rel_Source_Parent( ) const;
   void Use_Child_Rel_Source_Parent( bool Use_Child_Rel_Source_Parent );

   bool Use_In_Text_Search_Title( ) const;
   void Use_In_Text_Search_Title( bool Use_In_Text_Search_Title );

   bool Use_Source_Parent( ) const;
   void Use_Source_Parent( bool Use_Source_Parent );

   Meta_Modifier& Access_Parent_Modifier( );
   const Meta_Modifier& Access_Parent_Modifier( ) const;
   void Access_Parent_Modifier( const std::string& key );

   Meta_Permission& Access_Permission( );
   const Meta_Permission& Access_Permission( ) const;
   void Access_Permission( const std::string& key );

   Meta_Class& Child_Rel_Child_Class( );
   const Meta_Class& Child_Rel_Child_Class( ) const;
   void Child_Rel_Child_Class( const std::string& key );

   Meta_Field& Child_Rel_Source_Child( );
   const Meta_Field& Child_Rel_Source_Child( ) const;
   void Child_Rel_Source_Child( const std::string& key );

   Meta_Field& Child_Rel_Source_Field( );
   const Meta_Field& Child_Rel_Source_Field( ) const;
   void Child_Rel_Source_Field( const std::string& key );

   Meta_Field& Child_Rel_Source_Parent( );
   const Meta_Field& Child_Rel_Source_Parent( ) const;
   void Child_Rel_Source_Parent( const std::string& key );

   Meta_Class& Child_Rel_Source_Parent_Class( );
   const Meta_Class& Child_Rel_Source_Parent_Class( ) const;
   void Child_Rel_Source_Parent_Class( const std::string& key );

   Meta_Relationship& Child_Relationship( );
   const Meta_Relationship& Child_Relationship( ) const;
   void Child_Relationship( const std::string& key );

   Meta_Class& Class( );
   const Meta_Class& Class( ) const;
   void Class( const std::string& key );

   Meta_Permission& Link_Permission( );
   const Meta_Permission& Link_Permission( ) const;
   void Link_Permission( const std::string& key );

   Meta_List& List( );
   const Meta_List& List( ) const;
   void List( const std::string& key );

   Meta_Class& Parent_Class( );
   const Meta_Class& Parent_Class( ) const;
   void Parent_Class( const std::string& key );

   Meta_Procedure& Procedure( );
   const Meta_Procedure& Procedure( ) const;
   void Procedure( const std::string& key );

   Meta_Field& Restriction_Field( );
   const Meta_Field& Restriction_Field( ) const;
   void Restriction_Field( const std::string& key );

   Meta_Specification& Restriction_Spec( );
   const Meta_Specification& Restriction_Spec( ) const;
   void Restriction_Spec( const std::string& key );

   Meta_Field& Source_Child( );
   const Meta_Field& Source_Child( ) const;
   void Source_Child( const std::string& key );

   Meta_Class& Source_Child_Class( );
   const Meta_Class& Source_Child_Class( ) const;
   void Source_Child_Class( const std::string& key );

   Meta_Field& Source_Field( );
   const Meta_Field& Source_Field( ) const;
   void Source_Field( const std::string& key );

   Meta_Field& Source_Grandchild( );
   const Meta_Field& Source_Grandchild( ) const;
   void Source_Grandchild( const std::string& key );

   Meta_Field& Source_Parent( );
   const Meta_Field& Source_Parent( ) const;
   void Source_Parent( const std::string& key );

   Meta_Class& Source_Parent_Class( );
   const Meta_Class& Source_Parent_Class( ) const;
   void Source_Parent_Class( const std::string& key );

   Meta_List_Field_Type& Type( );
   const Meta_List_Field_Type& Type( ) const;
   void Type( const std::string& key );

   virtual void Move_Down( const std::string& Restrict_Fields, const std::string& Restrict_Values );

   virtual void Move_Up( const std::string& Restrict_Fields, const std::string& Restrict_Values );

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
    bool required_transients, std::set< std::string >* p_dependents = 0 ) const;

   void get_always_required_field_names( std::set< std::string >& names,
    bool required_transients, std::set< std::string >& dependents ) const;

   void get_transient_replacement_field_names( const std::string& name, std::vector< std::string >& names ) const;

   void do_generate_sql( generate_sql_type type, std::vector< std::string >& sql_stmts ) const;

   static void static_class_init( const char* p_module_name );
   static void static_class_term( const char* p_module_name );

   static const char* static_class_id( ) { return "124100"; }
   static const char* static_class_name( ) { return "List_Field"; }
   static const char* static_plural_name( ) { return "List_Fields"; }

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

   static void static_insert_external_alias( const std::string& module_and_class_id, Meta_List_Field* p_instance );
   static void static_remove_external_alias( const std::string& module_and_class_id );

   private:
   Meta_List_Field( const Meta_List_Field& );
   Meta_List_Field& operator =( const Meta_List_Field& );

   struct impl;
   friend struct impl;

   impl* p_impl;

   protected:
   const char* get_field_id( const std::string& name, bool* p_sql_numeric = 0, std::string* p_type_name = 0 ) const;
   const char* get_field_name( const std::string& id, bool* p_sql_numeric = 0, std::string* p_type_name = 0 ) const;

   void get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const;

   virtual void setup_foreign_key( Meta_Modifier& o, const std::string& value );
   virtual void setup_foreign_key( Meta_Permission& o, const std::string& value );
   virtual void setup_foreign_key( Meta_Class& o, const std::string& value );
   virtual void setup_foreign_key( Meta_Field& o, const std::string& value );
   virtual void setup_foreign_key( Meta_Relationship& o, const std::string& value );
   virtual void setup_foreign_key( Meta_List& o, const std::string& value );
   virtual void setup_foreign_key( Meta_Procedure& o, const std::string& value );
   virtual void setup_foreign_key( Meta_Specification& o, const std::string& value );
   virtual void setup_foreign_key( Meta_List_Field_Type& o, const std::string& value );

   virtual void setup_graph_parent( Meta_Modifier& o,
    const std::string& foreign_key_field, const std::string& init_value );

   virtual void setup_graph_parent( Meta_Permission& o,
    const std::string& foreign_key_field, const std::string& init_value );

   virtual void setup_graph_parent( Meta_Class& o,
    const std::string& foreign_key_field, const std::string& init_value );

   virtual void setup_graph_parent( Meta_Field& o,
    const std::string& foreign_key_field, const std::string& init_value );

   virtual void setup_graph_parent( Meta_Relationship& o,
    const std::string& foreign_key_field, const std::string& init_value );

   virtual void setup_graph_parent( Meta_List& o,
    const std::string& foreign_key_field, const std::string& init_value );

   virtual void setup_graph_parent( Meta_Procedure& o,
    const std::string& foreign_key_field, const std::string& init_value );

   virtual void setup_graph_parent( Meta_Specification& o,
    const std::string& foreign_key_field, const std::string& init_value );

   virtual void setup_graph_parent( Meta_List_Field_Type& o,
    const std::string& foreign_key_field, const std::string& init_value );

   size_t get_total_child_relationships( ) const;
   void set_total_child_relationships( size_t new_total_child_relationships ) const;

   size_t get_num_foreign_key_children( bool is_internal = false ) const;

   class_base* get_next_foreign_key_child( size_t child_num,
    std::string& next_child_field, cascade_op op, bool is_internal = false );

   void add_extra_paging_info( std::vector< std::pair< std::string, std::string > >& paging_info ) const;
};

inline std::string to_string( const Meta_List_Field& c ) { return c.get_key( ); }

inline bool is_null( const Meta_List_Field& c ) { return c.get_key( ).empty( ); }

inline bool check_equal( const Meta_List_Field& c, const char* p ) { return c.get_key( ) == p; }
inline bool check_not_equal( const Meta_List_Field& c, const char* p ) { return !( c.get_key( ) == p ); }

#endif

