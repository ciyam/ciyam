// Copyright (c) 2012-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef META_VIEW_FIELD_H
#  define META_VIEW_FIELD_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#  endif

#  include "class_base.h"


class Meta_Permission;
class Meta_Class;
class Meta_Specification;
class Meta_Field;
class Meta_View_Field_Type;
class Meta_View;

class Meta_View_Field : public class_base
{
   friend class Meta_Permission;
   friend class Meta_Class;
   friend class Meta_Specification;
   friend class Meta_Field;
   friend class Meta_View_Field_Type;
   friend class Meta_View;

   public:
   typedef Meta_View_Field class_type;

   enum field_id
   {
      e_field_id_none = 0,
      e_field_id_Access_Permission = 1,
      e_field_id_Access_Restriction = 2,
      e_field_id_Access_Scope = 3,
      e_field_id_Alignment = 4,
      e_field_id_Allow_Anonymous_Access = 5,
      e_field_id_Change_Permission = 6,
      e_field_id_Change_Restriction = 7,
      e_field_id_Change_Scope = 8,
      e_field_id_Child_List_Extra_Option = 9,
      e_field_id_Class = 10,
      e_field_id_Date_Precision_Option = 11,
      e_field_id_Enum_Finishes_At = 12,
      e_field_id_Enum_Starts_At = 13,
      e_field_id_FK_Trigger_Behaviour = 14,
      e_field_id_FK_Trigger_Option = 15,
      e_field_id_Font_Size = 16,
      e_field_id_Ignore_Manual_Links = 17,
      e_field_id_Label_Source_Child = 18,
      e_field_id_Link_Permission = 19,
      e_field_id_Link_Restriction = 20,
      e_field_id_Mandatory_Option = 21,
      e_field_id_Name = 22,
      e_field_id_New_Source = 23,
      e_field_id_New_Value = 24,
      e_field_id_Order = 25,
      e_field_id_Orientation = 26,
      e_field_id_Restriction_Spec = 27,
      e_field_id_Show_Hide_Start_Point = 28,
      e_field_id_Sort_Manually = 29,
      e_field_id_Source_Child = 30,
      e_field_id_Source_Edit_Child = 31,
      e_field_id_Source_Field = 32,
      e_field_id_Source_Parent = 33,
      e_field_id_Source_Parent_Class = 34,
      e_field_id_Tab_Name = 35,
      e_field_id_Trigger_Behaviour = 36,
      e_field_id_Trigger_For_State = 37,
      e_field_id_Trigger_Option = 38,
      e_field_id_Type = 39,
      e_field_id_Use_Full_Height = 40,
      e_field_id_Use_Full_Width = 41,
      e_field_id_Use_Source_Parent = 42,
      e_field_id_View = 43
   };

   Meta_View_Field( );
   ~Meta_View_Field( );

   int Access_Restriction( ) const;
   void Access_Restriction( int Access_Restriction );

   int Access_Scope( ) const;
   void Access_Scope( int Access_Scope );

   int Alignment( ) const;
   void Alignment( int Alignment );

   bool Allow_Anonymous_Access( ) const;
   void Allow_Anonymous_Access( bool Allow_Anonymous_Access );

   int Change_Restriction( ) const;
   void Change_Restriction( int Change_Restriction );

   int Change_Scope( ) const;
   void Change_Scope( int Change_Scope );

   int Child_List_Extra_Option( ) const;
   void Child_List_Extra_Option( int Child_List_Extra_Option );

   int Date_Precision_Option( ) const;
   void Date_Precision_Option( int Date_Precision_Option );

   int Enum_Finishes_At( ) const;
   void Enum_Finishes_At( int Enum_Finishes_At );

   int Enum_Starts_At( ) const;
   void Enum_Starts_At( int Enum_Starts_At );

   int FK_Trigger_Behaviour( ) const;
   void FK_Trigger_Behaviour( int FK_Trigger_Behaviour );

   int FK_Trigger_Option( ) const;
   void FK_Trigger_Option( int FK_Trigger_Option );

   int Font_Size( ) const;
   void Font_Size( int Font_Size );

   bool Ignore_Manual_Links( ) const;
   void Ignore_Manual_Links( bool Ignore_Manual_Links );

   int Label_Source_Child( ) const;
   void Label_Source_Child( int Label_Source_Child );

   int Link_Restriction( ) const;
   void Link_Restriction( int Link_Restriction );

   int Mandatory_Option( ) const;
   void Mandatory_Option( int Mandatory_Option );

   const std::string& Name( ) const;
   void Name( const std::string& Name );

   int New_Source( ) const;
   void New_Source( int New_Source );

   const std::string& New_Value( ) const;
   void New_Value( const std::string& New_Value );

   const std::string& Order( ) const;
   void Order( const std::string& Order );

   int Orientation( ) const;
   void Orientation( int Orientation );

   bool Show_Hide_Start_Point( ) const;
   void Show_Hide_Start_Point( bool Show_Hide_Start_Point );

   bool Sort_Manually( ) const;
   void Sort_Manually( bool Sort_Manually );

   const std::string& Tab_Name( ) const;
   void Tab_Name( const std::string& Tab_Name );

   int Trigger_Behaviour( ) const;
   void Trigger_Behaviour( int Trigger_Behaviour );

   int Trigger_For_State( ) const;
   void Trigger_For_State( int Trigger_For_State );

   int Trigger_Option( ) const;
   void Trigger_Option( int Trigger_Option );

   bool Use_Full_Height( ) const;
   void Use_Full_Height( bool Use_Full_Height );

   bool Use_Full_Width( ) const;
   void Use_Full_Width( bool Use_Full_Width );

   bool Use_Source_Parent( ) const;
   void Use_Source_Parent( bool Use_Source_Parent );

   Meta_Permission& Access_Permission( );
   const Meta_Permission& Access_Permission( ) const;
   void Access_Permission( const std::string& key );

   Meta_Permission& Change_Permission( );
   const Meta_Permission& Change_Permission( ) const;
   void Change_Permission( const std::string& key );

   Meta_Class& Class( );
   const Meta_Class& Class( ) const;
   void Class( const std::string& key );

   Meta_Permission& Link_Permission( );
   const Meta_Permission& Link_Permission( ) const;
   void Link_Permission( const std::string& key );

   Meta_Specification& Restriction_Spec( );
   const Meta_Specification& Restriction_Spec( ) const;
   void Restriction_Spec( const std::string& key );

   Meta_Field& Source_Child( );
   const Meta_Field& Source_Child( ) const;
   void Source_Child( const std::string& key );

   Meta_Field& Source_Edit_Child( );
   const Meta_Field& Source_Edit_Child( ) const;
   void Source_Edit_Child( const std::string& key );

   Meta_Field& Source_Field( );
   const Meta_Field& Source_Field( ) const;
   void Source_Field( const std::string& key );

   Meta_Field& Source_Parent( );
   const Meta_Field& Source_Parent( ) const;
   void Source_Parent( const std::string& key );

   Meta_Class& Source_Parent_Class( );
   const Meta_Class& Source_Parent_Class( ) const;
   void Source_Parent_Class( const std::string& key );

   Meta_View_Field_Type& Type( );
   const Meta_View_Field_Type& Type( ) const;
   void Type( const std::string& key );

   Meta_View& View( );
   const Meta_View& View( ) const;
   void View( const std::string& key );

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

   static const char* static_class_id( ) { return "120100"; }
   static const char* static_class_name( ) { return "View_Field"; }
   static const char* static_plural_name( ) { return "View_Fields"; }

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

   static void static_insert_external_alias( const std::string& module_and_class_id, Meta_View_Field* p_instance );
   static void static_remove_external_alias( const std::string& module_and_class_id );

   private:
   Meta_View_Field( const Meta_View_Field& );
   Meta_View_Field& operator =( const Meta_View_Field& );

   struct impl;
   friend struct impl;

   impl* p_impl;

   protected:
   const char* get_field_id( const std::string& name, bool* p_sql_numeric = 0, std::string* p_type_name = 0 ) const;
   const char* get_field_name( const std::string& id, bool* p_sql_numeric = 0, std::string* p_type_name = 0 ) const;

   void get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const;

   virtual void setup_foreign_key( Meta_Permission& o, const std::string& value );
   virtual void setup_foreign_key( Meta_Class& o, const std::string& value );
   virtual void setup_foreign_key( Meta_Specification& o, const std::string& value );
   virtual void setup_foreign_key( Meta_Field& o, const std::string& value );
   virtual void setup_foreign_key( Meta_View_Field_Type& o, const std::string& value );
   virtual void setup_foreign_key( Meta_View& o, const std::string& value );

   virtual void setup_graph_parent( Meta_Permission& o,
    const std::string& foreign_key_field, const std::string& init_value );

   virtual void setup_graph_parent( Meta_Class& o,
    const std::string& foreign_key_field, const std::string& init_value );

   virtual void setup_graph_parent( Meta_Specification& o,
    const std::string& foreign_key_field, const std::string& init_value );

   virtual void setup_graph_parent( Meta_Field& o,
    const std::string& foreign_key_field, const std::string& init_value );

   virtual void setup_graph_parent( Meta_View_Field_Type& o,
    const std::string& foreign_key_field, const std::string& init_value );

   virtual void setup_graph_parent( Meta_View& o,
    const std::string& foreign_key_field, const std::string& init_value );

   size_t get_total_child_relationships( ) const;
   void set_total_child_relationships( size_t new_total_child_relationships ) const;

   size_t get_num_foreign_key_children( bool is_internal = false ) const;

   class_base* get_next_foreign_key_child( size_t child_num,
    std::string& next_child_field, cascade_op op, bool is_internal = false );

   void add_extra_fixed_info( std::vector< std::pair< std::string, std::string > >& fixed_info ) const;
   void add_extra_paging_info( std::vector< std::pair< std::string, std::string > >& paging_info ) const;
};

inline std::string to_string( const Meta_View_Field& c ) { return c.get_key( ); }

inline bool is_null( const Meta_View_Field& c ) { return c.get_key( ).empty( ); }

inline bool check_equal( const Meta_View_Field& c, const char* p ) { return c.get_key( ) == p; }
inline bool check_not_equal( const Meta_View_Field& c, const char* p ) { return !( c.get_key( ) == p ); }

#endif
