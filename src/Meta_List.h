// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef META_LIST_H
#  define META_LIST_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#  endif

#  include "class_base.h"


class Meta_List_Field;
class Meta_Package_Option;
class Meta_Modifier;
class Meta_Permission;
class Meta_Class;
class Meta_Model;
class Meta_Field;
class Meta_List_Type;

class Meta_List : public class_base
{
   friend class Meta_List_Field;
   friend class Meta_Package_Option;
   friend class Meta_Modifier;
   friend class Meta_Permission;
   friend class Meta_Class;
   friend class Meta_Model;
   friend class Meta_Field;
   friend class Meta_List_Type;

   public:
   typedef Meta_List class_type;

   enum field_id
   {
      e_field_id_none = 0,
      e_field_id_Access_Parent_Modifier = 1,
      e_field_id_Access_Permission = 2,
      e_field_id_Access_Restriction = 3,
      e_field_id_Allow_Anonymous_Access = 4,
      e_field_id_Allow_Quick_Link = 5,
      e_field_id_Allow_Text_Search = 6,
      e_field_id_Class = 7,
      e_field_id_Create_Only_If_Default_Other = 8,
      e_field_id_Create_Parent_Modifier = 9,
      e_field_id_Create_Permission = 10,
      e_field_id_Create_Restriction = 11,
      e_field_id_Delete_Direction = 12,
      e_field_id_Destroy_Only_If_Default_Other = 13,
      e_field_id_Destroy_Parent_Modifier = 14,
      e_field_id_Destroy_Permission = 15,
      e_field_id_Destroy_Restriction = 16,
      e_field_id_Direction = 17,
      e_field_id_Display_Only_If_Default_Other = 18,
      e_field_id_Display_Row_Limit = 19,
      e_field_id_Display_Security_Level = 20,
      e_field_id_Display_Sub_Totals = 21,
      e_field_id_Display_Totals = 22,
      e_field_id_File_Links_Always_As_Single = 23,
      e_field_id_Id = 24,
      e_field_id_Ignore_Implicit_Ordering = 25,
      e_field_id_Ignore_State_For_Display = 26,
      e_field_id_Ignore_Unactionable_Records = 27,
      e_field_id_Ignore_Uneditable_Parent = 28,
      e_field_id_Ignore_Username_Filter = 29,
      e_field_id_Is_Admin = 30,
      e_field_id_Is_Child = 31,
      e_field_id_Is_Home = 32,
      e_field_id_Is_Not_Anonymous = 33,
      e_field_id_Is_Variation = 34,
      e_field_id_Limit_Scroll_And_New = 35,
      e_field_id_Model = 36,
      e_field_id_Multiline_Truncate_For_Print = 37,
      e_field_id_Name = 38,
      e_field_id_Number_Multiple_Pages = 39,
      e_field_id_PDF_Font_Type = 40,
      e_field_id_PDF_List_Type = 41,
      e_field_id_Parent_Class = 42,
      e_field_id_Parent_Field = 43,
      e_field_id_Print_Restriction = 44,
      e_field_id_Print_Without_Highlight = 45,
      e_field_id_Search_Option_Limit = 46,
      e_field_id_Sort_Rows_In_UI = 47,
      e_field_id_Style = 48,
      e_field_id_Text_Match_Highlight = 49,
      e_field_id_Title = 50,
      e_field_id_Type = 51,
      e_field_id_Variation_Name = 52
   };

   Meta_List( );
   ~Meta_List( );

   int Access_Restriction( ) const;
   void Access_Restriction( int Access_Restriction );

   bool Allow_Anonymous_Access( ) const;
   void Allow_Anonymous_Access( bool Allow_Anonymous_Access );

   bool Allow_Quick_Link( ) const;
   void Allow_Quick_Link( bool Allow_Quick_Link );

   bool Allow_Text_Search( ) const;
   void Allow_Text_Search( bool Allow_Text_Search );

   bool Create_Only_If_Default_Other( ) const;
   void Create_Only_If_Default_Other( bool Create_Only_If_Default_Other );

   int Create_Restriction( ) const;
   void Create_Restriction( int Create_Restriction );

   int Delete_Direction( ) const;
   void Delete_Direction( int Delete_Direction );

   bool Destroy_Only_If_Default_Other( ) const;
   void Destroy_Only_If_Default_Other( bool Destroy_Only_If_Default_Other );

   int Destroy_Restriction( ) const;
   void Destroy_Restriction( int Destroy_Restriction );

   int Direction( ) const;
   void Direction( int Direction );

   bool Display_Only_If_Default_Other( ) const;
   void Display_Only_If_Default_Other( bool Display_Only_If_Default_Other );

   int Display_Row_Limit( ) const;
   void Display_Row_Limit( int Display_Row_Limit );

   bool Display_Security_Level( ) const;
   void Display_Security_Level( bool Display_Security_Level );

   bool Display_Sub_Totals( ) const;
   void Display_Sub_Totals( bool Display_Sub_Totals );

   bool Display_Totals( ) const;
   void Display_Totals( bool Display_Totals );

   bool File_Links_Always_As_Single( ) const;
   void File_Links_Always_As_Single( bool File_Links_Always_As_Single );

   const std::string& Id( ) const;
   void Id( const std::string& Id );

   bool Ignore_Implicit_Ordering( ) const;
   void Ignore_Implicit_Ordering( bool Ignore_Implicit_Ordering );

   bool Ignore_State_For_Display( ) const;
   void Ignore_State_For_Display( bool Ignore_State_For_Display );

   bool Ignore_Unactionable_Records( ) const;
   void Ignore_Unactionable_Records( bool Ignore_Unactionable_Records );

   bool Ignore_Uneditable_Parent( ) const;
   void Ignore_Uneditable_Parent( bool Ignore_Uneditable_Parent );

   bool Ignore_Username_Filter( ) const;
   void Ignore_Username_Filter( bool Ignore_Username_Filter );

   bool Is_Admin( ) const;
   void Is_Admin( bool Is_Admin );

   bool Is_Child( ) const;
   void Is_Child( bool Is_Child );

   bool Is_Home( ) const;
   void Is_Home( bool Is_Home );

   bool Is_Not_Anonymous( ) const;
   void Is_Not_Anonymous( bool Is_Not_Anonymous );

   bool Is_Variation( ) const;
   void Is_Variation( bool Is_Variation );

   bool Limit_Scroll_And_New( ) const;
   void Limit_Scroll_And_New( bool Limit_Scroll_And_New );

   bool Multiline_Truncate_For_Print( ) const;
   void Multiline_Truncate_For_Print( bool Multiline_Truncate_For_Print );

   const std::string& Name( ) const;
   void Name( const std::string& Name );

   bool Number_Multiple_Pages( ) const;
   void Number_Multiple_Pages( bool Number_Multiple_Pages );

   int PDF_Font_Type( ) const;
   void PDF_Font_Type( int PDF_Font_Type );

   int PDF_List_Type( ) const;
   void PDF_List_Type( int PDF_List_Type );

   int Print_Restriction( ) const;
   void Print_Restriction( int Print_Restriction );

   bool Print_Without_Highlight( ) const;
   void Print_Without_Highlight( bool Print_Without_Highlight );

   int Search_Option_Limit( ) const;
   void Search_Option_Limit( int Search_Option_Limit );

   bool Sort_Rows_In_UI( ) const;
   void Sort_Rows_In_UI( bool Sort_Rows_In_UI );

   int Style( ) const;
   void Style( int Style );

   int Text_Match_Highlight( ) const;
   void Text_Match_Highlight( int Text_Match_Highlight );

   const std::string& Title( ) const;
   void Title( const std::string& Title );

   const std::string& Variation_Name( ) const;
   void Variation_Name( const std::string& Variation_Name );

   Meta_Modifier& Access_Parent_Modifier( );
   const Meta_Modifier& Access_Parent_Modifier( ) const;
   void Access_Parent_Modifier( const std::string& key );

   Meta_Permission& Access_Permission( );
   const Meta_Permission& Access_Permission( ) const;
   void Access_Permission( const std::string& key );

   Meta_Class& Class( );
   const Meta_Class& Class( ) const;
   void Class( const std::string& key );

   Meta_Modifier& Create_Parent_Modifier( );
   const Meta_Modifier& Create_Parent_Modifier( ) const;
   void Create_Parent_Modifier( const std::string& key );

   Meta_Permission& Create_Permission( );
   const Meta_Permission& Create_Permission( ) const;
   void Create_Permission( const std::string& key );

   Meta_Modifier& Destroy_Parent_Modifier( );
   const Meta_Modifier& Destroy_Parent_Modifier( ) const;
   void Destroy_Parent_Modifier( const std::string& key );

   Meta_Permission& Destroy_Permission( );
   const Meta_Permission& Destroy_Permission( ) const;
   void Destroy_Permission( const std::string& key );

   Meta_Model& Model( );
   const Meta_Model& Model( ) const;
   void Model( const std::string& key );

   Meta_Class& Parent_Class( );
   const Meta_Class& Parent_Class( ) const;
   void Parent_Class( const std::string& key );

   Meta_Field& Parent_Field( );
   const Meta_Field& Parent_Field( ) const;
   void Parent_Field( const std::string& key );

   Meta_List_Type& Type( );
   const Meta_List_Type& Type( ) const;
   void Type( const std::string& key );

   Meta_Class& child_Class_Created( );
   const Meta_Class& child_Class_Created( ) const;

   Meta_List_Field& child_List_Field( );
   const Meta_List_Field& child_List_Field( ) const;

   Meta_Package_Option& child_Package_Option( );
   const Meta_Package_Option& child_Package_Option( ) const;

   virtual void Generate_PDF_List( int Variation_Num );

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

   static const char* static_class_id( ) { return "122100"; }
   static const char* static_class_name( ) { return "List"; }
   static const char* static_plural_name( ) { return "Lists"; }

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

   static void static_insert_external_alias( const std::string& module_and_class_id, Meta_List* p_instance );
   static void static_remove_external_alias( const std::string& module_and_class_id );

   private:
   Meta_List( const Meta_List& );
   Meta_List& operator =( const Meta_List& );

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
   virtual void setup_foreign_key( Meta_Model& o, const std::string& value );
   virtual void setup_foreign_key( Meta_Field& o, const std::string& value );
   virtual void setup_foreign_key( Meta_List_Type& o, const std::string& value );

   virtual void setup_graph_parent( Meta_Class& o, const std::string& foreign_key_field );
   virtual void setup_graph_parent( Meta_List_Field& o, const std::string& foreign_key_field );
   virtual void setup_graph_parent( Meta_Package_Option& o, const std::string& foreign_key_field );

   virtual void setup_graph_parent( Meta_Modifier& o,
    const std::string& foreign_key_field, const std::string& init_value );

   virtual void setup_graph_parent( Meta_Permission& o,
    const std::string& foreign_key_field, const std::string& init_value );

   virtual void setup_graph_parent( Meta_Class& o,
    const std::string& foreign_key_field, const std::string& init_value );

   virtual void setup_graph_parent( Meta_Model& o,
    const std::string& foreign_key_field, const std::string& init_value );

   virtual void setup_graph_parent( Meta_Field& o,
    const std::string& foreign_key_field, const std::string& init_value );

   virtual void setup_graph_parent( Meta_List_Type& o,
    const std::string& foreign_key_field, const std::string& init_value );

   size_t get_total_child_relationships( ) const;
   void set_total_child_relationships( size_t new_total_child_relationships ) const;

   size_t get_num_foreign_key_children( bool is_internal = false ) const;

   class_base* get_next_foreign_key_child( size_t child_num,
    std::string& next_child_field, cascade_op op, bool is_internal = false );

   void add_extra_fixed_info( std::vector< std::pair< std::string, std::string > >& fixed_info ) const;
   void add_extra_paging_info( std::vector< std::pair< std::string, std::string > >& paging_info ) const;
};

inline std::string to_string( const Meta_List& c ) { return c.get_key( ); }

inline bool is_null( const Meta_List& c ) { return c.get_key( ).empty( ); }

inline bool check_equal( const Meta_List& c, const char* p ) { return c.get_key( ) == p; }
inline bool check_not_equal( const Meta_List& c, const char* p ) { return !( c.get_key( ) == p ); }

#endif
