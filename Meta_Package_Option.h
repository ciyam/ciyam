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

#ifndef META_PACKAGE_OPTION_H
#  define META_PACKAGE_OPTION_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#  endif

#  include "class_base.h"

#  ifdef MODULE_META_IMPL
#     define META_PACKAGE_OPTION_DECL_SPEC DYNAMIC_EXPORT
#  else
#     define META_PACKAGE_OPTION_DECL_SPEC DYNAMIC_IMPORT
#  endif

class Meta_Model;
class Meta_Package;

class META_PACKAGE_OPTION_DECL_SPEC Meta_Package_Option : public class_base
{
   friend class Meta_Model;
   friend class Meta_Package;

   public:
   typedef Meta_Package_Option class_type;

   enum field_id
   {
      e_field_id_none = 0,
      e_field_id_Date = 1,
      e_field_id_Datetime = 2,
      e_field_id_Id = 3,
      e_field_id_Installed = 4,
      e_field_id_Integer = 5,
      e_field_id_Is_Other_Package = 6,
      e_field_id_Model = 7,
      e_field_id_Name = 8,
      e_field_id_Numeric = 9,
      e_field_id_Other_Package = 10,
      e_field_id_Other_Package_Required = 11,
      e_field_id_Other_Package_Type = 12,
      e_field_id_Package = 13,
      e_field_id_Primitive = 14,
      e_field_id_String = 15,
      e_field_id_Time = 16,
      e_field_id_Use_Option = 17,
      e_field_id_Value = 18
   };

   Meta_Package_Option( );
   ~Meta_Package_Option( );

   const std::string& Date( ) const;
   void Date( const std::string& Date );

   const std::string& Datetime( ) const;
   void Datetime( const std::string& Datetime );

   const std::string& Id( ) const;
   void Id( const std::string& Id );

   bool Installed( ) const;
   void Installed( bool Installed );

   int Integer( ) const;
   void Integer( int Integer );

   bool Is_Other_Package( ) const;
   void Is_Other_Package( bool Is_Other_Package );

   const std::string& Name( ) const;
   void Name( const std::string& Name );

   const std::string& Numeric( ) const;
   void Numeric( const std::string& Numeric );

   bool Other_Package_Required( ) const;
   void Other_Package_Required( bool Other_Package_Required );

   const std::string& Other_Package_Type( ) const;
   void Other_Package_Type( const std::string& Other_Package_Type );

   int Primitive( ) const;
   void Primitive( int Primitive );

   const std::string& String( ) const;
   void String( const std::string& String );

   const std::string& Time( ) const;
   void Time( const std::string& Time );

   bool Use_Option( ) const;
   void Use_Option( bool Use_Option );

   const std::string& Value( ) const;
   void Value( const std::string& Value );

   Meta_Model& Model( );
   const Meta_Model& Model( ) const;
   void Model( const std::string& key );

   Meta_Package& Other_Package( );
   const Meta_Package& Other_Package( ) const;
   void Other_Package( const std::string& key );

   Meta_Package& Package( );
   const Meta_Package& Package( ) const;
   void Package( const std::string& key );

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
   void do_post_init( );

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

   static const char* static_class_id( ) { return "137100"; }
   static const char* static_class_name( ) { return "Package_Option"; }
   static const char* static_plural_name( ) { return "Package_Options"; }

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

   static void static_insert_external_alias( const std::string& module_and_class_id, Meta_Package_Option* p_instance );
   static void static_remove_external_alias( const std::string& module_and_class_id );

   private:
   Meta_Package_Option( const Meta_Package_Option& );
   Meta_Package_Option& operator =( const Meta_Package_Option& );

   struct impl;
   friend struct impl;

   impl* p_impl;

   protected:
   const char* get_field_id( const std::string& name, bool* p_sql_numeric = 0, std::string* p_type_name = 0 ) const;
   const char* get_field_name( const std::string& id, bool* p_sql_numeric = 0, std::string* p_type_name = 0 ) const;

   void get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const;

   virtual void setup_foreign_key( Meta_Model& o, const std::string& value );
   virtual void setup_foreign_key( Meta_Package& o, const std::string& value );

   virtual void setup_graph_parent( Meta_Model& o,
    const std::string& foreign_key_field, const std::string& init_value );

   virtual void setup_graph_parent( Meta_Package& o,
    const std::string& foreign_key_field, const std::string& init_value );

   size_t get_total_child_relationships( ) const;
   void set_total_child_relationships( size_t new_total_child_relationships ) const;

   size_t get_num_foreign_key_children( bool is_internal = false ) const;

   class_base* get_next_foreign_key_child( size_t child_num,
    std::string& next_child_field, cascade_op op, bool is_internal = false );

   void add_extra_fixed_info( std::vector< std::pair< std::string, std::string > >& fixed_info ) const;
   void add_extra_paging_info( std::vector< std::pair< std::string, std::string > >& paging_info ) const;
};

inline std::string to_string( const Meta_Package_Option& c ) { return c.get_key( ); }

inline bool is_null( const Meta_Package_Option& c ) { return c.get_key( ).empty( ); }

inline bool check_equal( const Meta_Package_Option& c, const char* p ) { return c.get_key( ) == p; }
inline bool check_not_equal( const Meta_Package_Option& c, const char* p ) { return !( c.get_key( ) == p ); }

#endif

