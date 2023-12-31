// Copyright (c) 2012-2024 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef META_MODEL_H
#  define META_MODEL_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#  endif

#  include "class_base.h"

#  ifdef MODULE_META_IMPL
#     define META_MODEL_DECL_SPEC DYNAMIC_EXPORT
#  else
#     define META_MODEL_DECL_SPEC DYNAMIC_IMPORT
#  endif

class Meta_Class;
class Meta_List;
class Meta_Module;
class Meta_Package_Option;
class Meta_Package;
class Meta_Relationship;
class Meta_Specification;
class Meta_View;
class Meta_Permission;
class Meta_Workgroup;

class META_MODEL_DECL_SPEC Meta_Model : public class_base
{
   friend class Meta_Class;
   friend class Meta_List;
   friend class Meta_Module;
   friend class Meta_Package_Option;
   friend class Meta_Package;
   friend class Meta_Relationship;
   friend class Meta_Specification;
   friend class Meta_View;
   friend class Meta_Permission;
   friend class Meta_Workgroup;

   public:
   typedef Meta_Model class_type;

   enum field_id
   {
      e_field_id_none = 0,
      e_field_id_Actions = 1,
      e_field_id_Add_Packages = 2,
      e_field_id_Allow_Anonymous_Access = 3,
      e_field_id_Commands_File = 4,
      e_field_id_Created = 5,
      e_field_id_Home_Title = 6,
      e_field_id_Id = 7,
      e_field_id_Name = 8,
      e_field_id_Next_Class_Id = 9,
      e_field_id_Next_List_Id = 10,
      e_field_id_Next_Specification_Id = 11,
      e_field_id_Next_View_Id = 12,
      e_field_id_Permission = 13,
      e_field_id_Source_File = 14,
      e_field_id_Status = 15,
      e_field_id_Type = 16,
      e_field_id_Use_Package_Demo_Data = 17,
      e_field_id_Version = 18,
      e_field_id_Workgroup = 19,
      e_field_id_Year_Created = 20
   };

   Meta_Model( );
   ~Meta_Model( );

   const std::string& Actions( ) const;
   void Actions( const std::string& Actions );

   int Add_Packages( ) const;
   void Add_Packages( int Add_Packages );

   bool Allow_Anonymous_Access( ) const;
   void Allow_Anonymous_Access( bool Allow_Anonymous_Access );

   const std::string& Commands_File( ) const;
   void Commands_File( const std::string& Commands_File );

   bool Created( ) const;
   void Created( bool Created );

   const std::string& Home_Title( ) const;
   void Home_Title( const std::string& Home_Title );

   const std::string& Id( ) const;
   void Id( const std::string& Id );

   const std::string& Name( ) const;
   void Name( const std::string& Name );

   const std::string& Next_Class_Id( ) const;
   void Next_Class_Id( const std::string& Next_Class_Id );

   const std::string& Next_List_Id( ) const;
   void Next_List_Id( const std::string& Next_List_Id );

   const std::string& Next_Specification_Id( ) const;
   void Next_Specification_Id( const std::string& Next_Specification_Id );

   const std::string& Next_View_Id( ) const;
   void Next_View_Id( const std::string& Next_View_Id );

   const std::string& Source_File( ) const;
   void Source_File( const std::string& Source_File );

   const std::string& Status( ) const;
   void Status( const std::string& Status );

   bool Type( ) const;
   void Type( bool Type );

   bool Use_Package_Demo_Data( ) const;
   void Use_Package_Demo_Data( bool Use_Package_Demo_Data );

   const std::string& Version( ) const;
   void Version( const std::string& Version );

   int Year_Created( ) const;
   void Year_Created( int Year_Created );

   Meta_Permission& Permission( );
   const Meta_Permission& Permission( ) const;
   void Permission( const std::string& key );

   Meta_Workgroup& Workgroup( );
   const Meta_Workgroup& Workgroup( ) const;
   void Workgroup( const std::string& key );

   Meta_Class& child_Class_Source( );
   const Meta_Class& child_Class_Source( ) const;

   Meta_Class& child_Class( );
   const Meta_Class& child_Class( ) const;

   Meta_List& child_List( );
   const Meta_List& child_List( ) const;

   Meta_Module& child_Module( );
   const Meta_Module& child_Module( ) const;

   Meta_Package_Option& child_Package_Option( );
   const Meta_Package_Option& child_Package_Option( ) const;

   Meta_Package& child_Package( );
   const Meta_Package& child_Package( ) const;

   Meta_Relationship& child_Relationship( );
   const Meta_Relationship& child_Relationship( ) const;

   Meta_Specification& child_Specification( );
   const Meta_Specification& child_Specification( ) const;

   Meta_View& child_View( );
   const Meta_View& child_View( ) const;

   virtual void Create_Module( );

   virtual void Generate( );

   virtual void Generate_File_Links( );

   virtual void Get_Acyclic_Class_List( std::string& Acyclic_Class_Keys, std::string& Acyclic_Class_Names, bool Check_Initial_Data );

   virtual void Remove_All_Packages( );

   virtual void Remove_Module( );

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

   static const char* static_class_id( ) { return "105100"; }
   static const char* static_class_name( ) { return "Model"; }
   static const char* static_plural_name( ) { return "Models"; }

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

   static void static_get_all_enum_pairs( std::vector< std::pair< std::string, std::string > >& pairs );

   static void static_get_text_search_fields( std::vector< std::string >& fields );

   static std::string static_get_sql_columns( );

   static void static_get_sql_indexes( std::vector< std::string >& indexes );
   static void static_get_sql_unique_indexes( std::vector< std::string >& indexes );

   static void static_insert_derivation( const std::string& module_and_class_id );
   static void static_remove_derivation( const std::string& module_and_class_id );

   static void static_insert_external_alias( const std::string& module_and_class_id, Meta_Model* p_instance );
   static void static_remove_external_alias( const std::string& module_and_class_id );

   private:
   Meta_Model( const Meta_Model& );
   Meta_Model& operator =( const Meta_Model& );

   struct impl;
   friend struct impl;

   impl* p_impl;

   protected:
   const char* get_field_id( const std::string& name, bool* p_sql_numeric = 0, std::string* p_type_name = 0 ) const;
   const char* get_field_name( const std::string& id, bool* p_sql_numeric = 0, std::string* p_type_name = 0 ) const;

   void get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const;

   virtual void setup_foreign_key( Meta_Permission& o, const std::string& value );
   virtual void setup_foreign_key( Meta_Workgroup& o, const std::string& value );

   virtual void setup_graph_parent( Meta_Class& o, const std::string& foreign_key_field );
   virtual void setup_graph_parent( Meta_List& o, const std::string& foreign_key_field );
   virtual void setup_graph_parent( Meta_Module& o, const std::string& foreign_key_field );
   virtual void setup_graph_parent( Meta_Package_Option& o, const std::string& foreign_key_field );
   virtual void setup_graph_parent( Meta_Package& o, const std::string& foreign_key_field );
   virtual void setup_graph_parent( Meta_Relationship& o, const std::string& foreign_key_field );
   virtual void setup_graph_parent( Meta_Specification& o, const std::string& foreign_key_field );
   virtual void setup_graph_parent( Meta_View& o, const std::string& foreign_key_field );

   virtual void setup_graph_parent( Meta_Permission& o,
    const std::string& foreign_key_field, const std::string& init_value );

   virtual void setup_graph_parent( Meta_Workgroup& o,
    const std::string& foreign_key_field, const std::string& init_value );

   size_t get_total_child_relationships( ) const;
   void set_total_child_relationships( size_t new_total_child_relationships ) const;

   size_t get_num_foreign_key_children( bool is_internal = false ) const;

   class_base* get_next_foreign_key_child( size_t child_num,
    std::string& next_child_field, cascade_op op, bool is_internal = false );

   void add_extra_fixed_info( std::vector< std::pair< std::string, std::string > >& fixed_info ) const;
   void add_extra_paging_info( std::vector< std::pair< std::string, std::string > >& paging_info ) const;
};

inline std::string to_string( const Meta_Model& c ) { return c.get_key( ); }

inline bool is_null( const Meta_Model& c ) { return c.get_key( ).empty( ); }

inline bool check_equal( const Meta_Model& c, const char* p ) { return c.get_key( ) == p; }
inline bool check_not_equal( const Meta_Model& c, const char* p ) { return !( c.get_key( ) == p ); }

#endif
