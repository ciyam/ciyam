// Copyright (c) 2008-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2014 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef META_LIST_FIELD_TYPE_H
#  define META_LIST_FIELD_TYPE_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#  endif

#  include "class_base.h"

#  ifdef MODULE_META_IMPL
#     define META_LIST_FIELD_TYPE_DECL_SPEC DYNAMIC_EXPORT
#  else
#     define META_LIST_FIELD_TYPE_DECL_SPEC DYNAMIC_IMPORT
#  endif

class Meta_List_Field;

class META_LIST_FIELD_TYPE_DECL_SPEC Meta_List_Field_Type : public class_base
{
   friend class Meta_List_Field;

   public:
   typedef Meta_List_Field_Type class_type;

   enum field_id
   {
      e_field_id_none = 0,
      e_field_id_Allow_Child_Rel_Select_Specifics = 1,
      e_field_id_Allow_Exclude_In_Use_FK = 2,
      e_field_id_Allow_Link_Empty_Restriction = 3,
      e_field_id_Allow_Link_Permission = 4,
      e_field_id_Allow_Link_Specifics = 5,
      e_field_id_Allow_Link_Type = 6,
      e_field_id_Allow_Procedure = 7,
      e_field_id_Allow_Restriction_Field = 8,
      e_field_id_Allow_Restriction_Spec = 9,
      e_field_id_Allow_Restriction_Value = 10,
      e_field_id_Allow_Search_Option_Limit = 11,
      e_field_id_Allow_Select_Specifics = 12,
      e_field_id_Allow_View_Parent_Extra = 13,
      e_field_id_Is_Restrict_Search = 14,
      e_field_id_List_Field_Name = 15,
      e_field_id_Name = 16,
      e_field_id_Needs_Restriction_Field = 17,
      e_field_id_Needs_Source = 18,
      e_field_id_Needs_Switch_Type = 19,
      e_field_id_Non_Simple_Field = 20,
      e_field_id_Trivial_Field_Only = 21
   };

   Meta_List_Field_Type( );
   ~Meta_List_Field_Type( );

   bool Allow_Child_Rel_Select_Specifics( ) const;
   void Allow_Child_Rel_Select_Specifics( bool Allow_Child_Rel_Select_Specifics );

   bool Allow_Exclude_In_Use_FK( ) const;
   void Allow_Exclude_In_Use_FK( bool Allow_Exclude_In_Use_FK );

   bool Allow_Link_Empty_Restriction( ) const;
   void Allow_Link_Empty_Restriction( bool Allow_Link_Empty_Restriction );

   bool Allow_Link_Permission( ) const;
   void Allow_Link_Permission( bool Allow_Link_Permission );

   bool Allow_Link_Specifics( ) const;
   void Allow_Link_Specifics( bool Allow_Link_Specifics );

   bool Allow_Link_Type( ) const;
   void Allow_Link_Type( bool Allow_Link_Type );

   bool Allow_Procedure( ) const;
   void Allow_Procedure( bool Allow_Procedure );

   bool Allow_Restriction_Field( ) const;
   void Allow_Restriction_Field( bool Allow_Restriction_Field );

   bool Allow_Restriction_Spec( ) const;
   void Allow_Restriction_Spec( bool Allow_Restriction_Spec );

   bool Allow_Restriction_Value( ) const;
   void Allow_Restriction_Value( bool Allow_Restriction_Value );

   bool Allow_Search_Option_Limit( ) const;
   void Allow_Search_Option_Limit( bool Allow_Search_Option_Limit );

   bool Allow_Select_Specifics( ) const;
   void Allow_Select_Specifics( bool Allow_Select_Specifics );

   bool Allow_View_Parent_Extra( ) const;
   void Allow_View_Parent_Extra( bool Allow_View_Parent_Extra );

   bool Is_Restrict_Search( ) const;
   void Is_Restrict_Search( bool Is_Restrict_Search );

   const std::string& List_Field_Name( ) const;
   void List_Field_Name( const std::string& List_Field_Name );

   const std::string& Name( ) const;
   void Name( const std::string& Name );

   bool Needs_Restriction_Field( ) const;
   void Needs_Restriction_Field( bool Needs_Restriction_Field );

   bool Needs_Source( ) const;
   void Needs_Source( bool Needs_Source );

   bool Needs_Switch_Type( ) const;
   void Needs_Switch_Type( bool Needs_Switch_Type );

   bool Non_Simple_Field( ) const;
   void Non_Simple_Field( bool Non_Simple_Field );

   bool Trivial_Field_Only( ) const;
   void Trivial_Field_Only( bool Trivial_Field_Only );

   Meta_List_Field& child_List_Field_Type( );
   const Meta_List_Field& child_List_Field_Type( ) const;

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

   bool is_file_field_name( const std::string& name ) const;
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

   void do_generate_sql( generate_sql_type type,
    std::vector< std::string >& sql_stmts, std::set< std::string >& tx_key_info ) const;

   static void static_class_init( const char* p_module_name );
   static void static_class_term( const char* p_module_name );

   static const char* static_class_id( ) { return "123100"; }
   static const char* static_class_name( ) { return "List_Field_Type"; }
   static const char* static_plural_name( ) { return "List_Field_Types"; }

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

   static void static_insert_external_alias( const std::string& module_and_class_id, Meta_List_Field_Type* p_instance );
   static void static_remove_external_alias( const std::string& module_and_class_id );

   private:
   Meta_List_Field_Type( const Meta_List_Field_Type& );
   Meta_List_Field_Type& operator =( const Meta_List_Field_Type& );

   struct impl;
   friend struct impl;

   impl* p_impl;

   protected:
   const char* get_field_id( const std::string& name, bool* p_sql_numeric = 0, std::string* p_type_name = 0 ) const;
   const char* get_field_name( const std::string& id, bool* p_sql_numeric = 0, std::string* p_type_name = 0 ) const;

   void get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const;

   virtual void setup_graph_parent( Meta_List_Field& o, const std::string& foreign_key_field );

   size_t get_total_child_relationships( ) const;
   void set_total_child_relationships( size_t new_total_child_relationships ) const;

   size_t get_num_foreign_key_children( bool is_internal = false ) const;

   class_base* get_next_foreign_key_child( size_t child_num,
    std::string& next_child_field, cascade_op op, bool is_internal = false );

   void add_extra_fixed_info( std::vector< std::pair< std::string, std::string > >& fixed_info ) const;
   void add_extra_paging_info( std::vector< std::pair< std::string, std::string > >& paging_info ) const;
};

inline std::string to_string( const Meta_List_Field_Type& c ) { return c.get_key( ); }

inline bool is_null( const Meta_List_Field_Type& c ) { return c.get_key( ).empty( ); }

inline bool check_equal( const Meta_List_Field_Type& c, const char* p ) { return c.get_key( ) == p; }
inline bool check_not_equal( const Meta_List_Field_Type& c, const char* p ) { return !( c.get_key( ) == p ); }

#endif

