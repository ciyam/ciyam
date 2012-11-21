// Copyright (c) 2004
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

#ifndef CLASS_BASE_H
#  define CLASS_BASE_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <set>
#     include <map>
#     include <deque>
#     include <vector>
#     include <memory>
#     include <string>
#     include <utility>
#  endif

#  include "ptypes.h"
#  include "utilities.h"
#  include "ciyam_common.h"

#  ifdef CIYAM_BASE_IMPL
#     define CLASS_BASE_DECL_SPEC DYNAMIC_EXPORT
#  else
#     define CLASS_BASE_DECL_SPEC DYNAMIC_IMPORT
#  endif

#  define MODULE_TRACE( x ) get_obj( ).trace( x )

class ods;
class numeric;
class date_time;
class class_base;
class sql_dataset;

const char* const c_key_field = "@key";

const char* const c_dtm_now = "@now";
const char* const c_date_today = "@today";
const char* const c_date_tomorrow = "@tomorrow";

const char* const c_object_variable_skip_fk_handling = "@skip_fk_handling";

struct class_cascade;
struct class_pointer_base;
struct class_base_accessor;

enum cascade_op
{
   e_cascade_op_unlink,
   e_cascade_op_destroy,
   e_cascade_op_restrict
};

enum generate_sql_type
{
   e_generate_sql_type_insert,
   e_generate_sql_type_update,
   e_generate_sql_type_delete
};

typedef std::vector< std::string > class_info_container;
typedef class_info_container::iterator class_info_iterator;
typedef class_info_container::const_iterator class_info_const_iterator;
typedef class_info_container::value_type class_info_value_type;

struct field_info
{
   field_info( ) { }

   field_info( const std::string& id, const std::string& name, const std::string& type_name, bool mandatory )
    :
    id( id ),
    name( name ),
    type_name( type_name ),
    mandatory( mandatory )
   {
   }

   std::string id;
   std::string name;
   std::string type_name;

   bool mandatory;
};

typedef std::vector< field_info > field_info_container;

typedef field_info_container::const_iterator field_info_const_iterator;

struct key_field_info
{
   key_field_info( ) { }

   key_field_info( const std::string& id, const std::string& name )
    :
    id( id ),
    name( name )
   {
   }

   std::string id;
   std::string name;
};

typedef std::vector< key_field_info > key_field_info_container;

typedef key_field_info_container::const_iterator key_field_info_const_iterator;

typedef std::map< std::string, std::string > procedure_info_container;
typedef procedure_info_container::iterator procedure_info_iterator;
typedef procedure_info_container::const_iterator procedure_info_const_iterator;
typedef procedure_info_container::value_type procedure_info_value_type;

typedef std::map< std::string, std::string > foreign_key_data_container;
typedef foreign_key_data_container::iterator foreign_key_data_iterator;
typedef foreign_key_data_container::const_iterator foreign_key_data_const_iterator;
typedef foreign_key_data_container::value_type foreign_key_data_value_type;

typedef std::map< std::string, std::pair< std::string, std::string > > foreign_key_info_container;
typedef foreign_key_info_container::iterator foreign_key_info_iterator;
typedef foreign_key_info_container::const_iterator foreign_key_info_const_iterator;
typedef foreign_key_info_container::value_type foreign_key_info_value_type;

typedef std::multimap< std::string, std::string > validation_error_container;
typedef validation_error_container::iterator validation_error_iterator;
typedef validation_error_container::const_iterator validation_error_const_iterator;
typedef validation_error_container::value_type validation_error_value_type;

typedef std::map< std::string, std::vector< std::pair< std::string, std::string > > > search_replace_container;
typedef search_replace_container::iterator search_replace_iterator;
typedef search_replace_container::const_iterator search_replace_const_iterator;
typedef search_replace_container::value_type search_replace_value_type;

struct CLASS_BASE_DECL_SPEC class_cascade
{
   class_cascade( class_base& cb );
   ~class_cascade( );

   struct impl;
   impl* p_impl;
};

struct CLASS_BASE_DECL_SPEC class_after_store
{
   class_after_store( class_base& cb );
   ~class_after_store( );

   struct impl;
   impl* p_impl;
};

class CLASS_BASE_DECL_SPEC class_base
{
   friend struct class_cascade;
   friend struct class_after_store;

   friend struct class_cascade::impl;
   friend struct class_after_store::impl;

   friend struct class_base_accessor;

   public:
   enum op_type
   {
      e_op_type_none,
      e_op_type_review,
      e_op_type_create,
      e_op_type_update,
      e_op_type_destroy
   };

   enum update_type
   {
      e_update_type_normal,
      e_update_type_minimal
   };

   enum validation_errors_type
   {
      e_validation_errors_type_all,
      e_validation_errors_type_first_only
   };

   class_base( );

   bool class_has_derivations( ) const;
   void set_dynamic_if_class_has_derivations( );

   virtual ~class_base( );

   void op_create( op_create_rc* p_rc = 0, bool is_internal = true ) { op_create( get_key( ), p_rc, is_internal ); }
   void op_create( const std::string& key, op_create_rc* p_rc = 0, bool is_internal = true );

   void op_update( op_update_rc* p_rc = 0, bool is_internal = true ) { op_update( get_key( ), "", p_rc, is_internal ); }
   void op_update( const std::string& key, op_update_rc* p_rc = 0, bool is_internal = true ) { op_update( key, "", p_rc, is_internal ); }
   void op_update( const std::string& key, const std::string& fields, op_update_rc* p_rc = 0, bool is_internal = true );

   void op_destroy( op_destroy_rc* p_rc = 0, bool is_internal = true ) { op_destroy( get_key( ), p_rc, is_internal ); }
   void op_destroy( const std::string& key, op_destroy_rc* p_rc = 0, bool is_internal = true );

   void op_apply( op_apply_rc* p_rc = 0, bool is_internal = true, std::set< std::string >* p_fields_set = 0 );
   void op_cancel( bool is_internal = true );

   void init( bool for_create );
   void prepare( bool for_create );

   bool is_valid( bool is_internal, std::set< std::string >* p_fields_set = 0 );
   bool has_changed( ) const;

   void link_to_graph_parent( );
   void unlink_from_graph_parent( );

   void begin_review( begin_review_rc* p_rc = 0 ) { begin_review( get_key( ), p_rc ); }
   void begin_review( const std::string& key, begin_review_rc* p_rc = 0 );
   void finish_review( );

   void perform_fetch( perform_fetch_rc* p_rc = 0 ) { perform_fetch( get_key( ), p_rc ); }
   void perform_fetch( const std::string& key_info, perform_fetch_rc* p_rc = 0 );

   void perform_lazy_fetch( );
   void ensure_original_fetch( );

   bool iterate_forwards( bool inclusive = true,
    int row_limit = 0, sql_optimisation optimisation = e_sql_optimisation_none );

   bool iterate_forwards( const std::string& key_info, bool inclusive = true,
    int row_limit = 0, sql_optimisation optimisation = e_sql_optimisation_none );

   bool iterate_forwards( const std::string& key_info, const std::string& fields,
    bool inclusive = true, int row_limit = 0, sql_optimisation optimisation = e_sql_optimisation_none );

   bool iterate_backwards( bool inclusive = true,
    int row_limit = 0, sql_optimisation optimisation = e_sql_optimisation_none );

   bool iterate_backwards( const std::string& key_info, bool inclusive = true,
    int row_limit = 0, sql_optimisation optimisation = e_sql_optimisation_none );

   bool iterate_backwards( const std::string& key_info, const std::string& fields,
    bool inclusive = true, int row_limit = 0, sql_optimisation optimisation = e_sql_optimisation_none );

   bool iterate_next( );
   void iterate_stop( );

   void set_instance( const std::string& key );

   void copy_all_field_values( const class_base& src );
   void copy_original_field_values( const class_base& src );

   bool filtered( ) const { return is_filtered( ); }

   std::string get_validation_errors( validation_errors_type type = e_validation_errors_type_all );

   uint32_t get_version( ) const { return version; }
   uint32_t get_revision( ) const { return revision; }

   std::string get_version_info( ) const;

   const std::string get_current_identity( ) const;
   const std::string& get_original_identity( ) const { return original_identity; }

   op_type get_op( ) const { return op; }

   bool get_is_in_op( ) const { return op != e_op_type_none; }

   bool get_is_editing( ) const { return op == e_op_type_create || op == e_op_type_update; }
   bool get_is_creating( ) const { return op == e_op_type_create; }
   bool get_is_updating( ) const { return op == e_op_type_update; }
   bool get_is_destroying( ) const { return op == e_op_type_destroy; }

   bool get_is_minimal_update( ) const { return utype == e_update_type_minimal; }

   bool get_is_fetching( ) const { return is_fetching; }
   bool get_is_executing( ) const { return is_executing; }
   bool get_is_preparing( ) const { return is_preparing; }
   bool get_is_iterating( ) const { return in_forwards_iteration || in_backwards_iteration; }

   bool get_is_starting_iteration( ) const { return iteration_starting; }

   bool get_is_in_forwards_iteration( ) const { return in_forwards_iteration; }
   bool get_is_in_backwards_iteration( ) const { return in_backwards_iteration; }

   const std::string& get_key( ) const;
   operator const std::string& ( ) const { return get_key( ); }

   const std::string& get_clone_key( ) const { return clone_key; }
   void clear_clone_key( ) { clone_key.erase( ); }

   virtual const std::string& get_fixed_key( ) const;

   std::string get_parent_key( ) const;

   std::string get_attached_file_path( const std::string& file_name ) const;

   bool get_is_singular( ) const { return is_singular; }

   bool get_is_after_store( ) const { return is_after_store; }

   bool get_is_being_cascaded( ) const { return is_being_cascaded; }
   bool get_is_dynamic_enabled( ) const { return is_dynamic_enabled; }

   class_base* get_owning_instance( ) { return p_owning_instance ? p_owning_instance : this; }
   class_base* get_dynamic_instance( ) const { return p_dynamic_instance; }

   int get_graph_depth( ) const;

   class_base* get_graph_root( );

   class_base* get_graph_parent( ) { return p_graph_parent; }
   const class_base* get_graph_parent( ) const { return p_graph_parent; }

   const std::string& get_graph_parent_fk_field( ) const { return graph_parent_fk_field; }

   std::string get_variable( const std::string& vname ) const;
   void set_variable( const std::string& vname, const std::string& value );

   bool has_field_changed( int field ) const;

   bool needs_field_value( const std::string& field_name ) const;
   bool needs_field_value( const std::string& field_name, const std::set< std::string >& names ) const;

   std::string get_original_field_value( int field ) const;
   inline std::string get_original_field_value( const std::string& field ) const
   {
      return get_original_field_value( get_field_num( field ) );
   }

   virtual std::string get_field_value( int field ) const = 0;
   virtual void set_field_value( int field, const std::string& value ) = 0;

   virtual bool has_field_changed( const std::string& field ) const = 0;

   virtual bool is_field_transient( int field ) const = 0;
   virtual std::string get_field_name( int field ) const = 0;

   virtual int get_field_num( const std::string& field ) const = 0;

   virtual uint64_t get_state( ) const = 0;

   virtual const std::string& execute( const std::string& cmd_and_args ) = 0;

   virtual std::string get_class_id( ) const = 0;
   virtual std::string get_class_name( ) const = 0;
   virtual std::string get_plural_name( ) const = 0;

   virtual std::string get_module_id( ) const = 0;
   virtual std::string get_module_name( ) const = 0;

   virtual std::string get_resolved_module_id( ) const = 0;
   virtual std::string get_resolved_module_name( ) const = 0;

   virtual std::string get_lock_class_id( ) const = 0;
   virtual std::string get_check_class_name( ) const = 0;

   virtual std::string get_display_name( bool plural = false ) const = 0;

   virtual bool get_is_alias( ) const = 0;
   virtual void get_alias_base_info( std::pair< std::string, std::string >& alias_base_info ) const = 0;

   virtual void get_base_class_info( std::vector< std::pair< std::string, std::string > >& base_class_info ) const = 0;

   virtual void clear_foreign_key( const std::string& fk_field_name ) = 0;

   virtual class_base& get_or_create_graph_child( const std::string& context ) = 0;

   virtual void get_class_info( class_info_container& class_info ) const = 0;
   virtual void get_field_info( field_info_container& field_info ) const = 0;
   virtual void get_foreign_key_info( foreign_key_info_container& foreign_key_info ) const = 0;

   virtual int get_num_fields( bool* p_done = 0, const std::string* p_class_name = 0 ) const = 0;

   virtual std::string& get_order_field_name( ) const = 0;

   virtual bool is_file_field_name( const std::string& name ) const = 0;
   virtual void get_file_field_names( std::vector< std::string >& file_field_names ) const = 0;

   virtual std::string get_field_display_name( const std::string& id_or_name ) const = 0;

   void get_alternative_key_field_info( std::vector< key_field_info_container >& all_key_field_info ) const;

   virtual const procedure_info_container& get_procedure_info( ) const = 0;

   virtual void get_text_search_fields( std::vector< std::string >& fields ) const = 0;

   virtual void get_all_enum_pairs( std::vector< std::pair< std::string, std::string > >& pairs ) const = 0;

   virtual std::string get_sql_columns( ) const = 0;

   virtual void get_sql_indexes( std::vector< std::string >& indexes ) const = 0;
   virtual void get_sql_unique_indexes( std::vector< std::string >& indexes ) const = 0;

   virtual void get_sql_column_names(
    std::vector< std::string >& names, bool* p_done = 0, const std::string* p_class_name = 0 ) const = 0;
   virtual void get_sql_column_values(
    std::vector< std::string >& values, bool* p_done = 0, const std::string* p_class_name = 0 ) const = 0;

   virtual void get_required_field_names( std::set< std::string >& names,
    bool use_transients = false, std::set< std::string >* p_dependents = 0 ) const = 0;

   virtual void get_always_required_field_names( std::set< std::string >& names,
    bool use_transients, std::set< std::string >& dependents ) const = 0;

   virtual void get_transient_replacement_field_names(
    const std::string& name, std::vector< std::string >& names ) const = 0;

   int get_max_index_depth( const std::vector< std::string >& field_names ) const;

   virtual void do_generate_sql( generate_sql_type type,
    std::vector< std::string >& sql_stmts, std::set< std::string >& tx_key_info ) const = 0;

   void generate_sql( const std::string& class_name,
    generate_sql_type type, std::vector< std::string >& sql_stmts, std::set< std::string >& tx_key_info ) const;

   std::string generate_sql_insert( const std::string& class_name ) const;
   std::string generate_sql_update( const std::string& class_name ) const;
   std::string generate_sql_delete( const std::string& class_name ) const;

   void set_class_pointer_base( class_pointer_base* p_cpb );

   static ods* ods_instance( );
   static void ods_instance( ods* p_ods );

   private:
   uint32_t version;
   uint32_t revision;
   std::string original_identity;
   std::vector< std::string > original_values;

   op_type op;
   update_type utype;

   std::string key;
   std::string clone_key;

   std::string lazy_fetch_key;

   uint32_t last_lazy_fetch_ver;
   uint32_t last_lazy_fetch_rev;
   std::string last_lazy_fetch_key;
   std::string last_lazy_fetch_identity;
   std::vector< std::string > last_lazy_fetch_field_values;

   size_t index_num;
   size_t lock_handle;
   size_t xlock_handle;

   std::string ver_exp;

   sql_dataset* p_sql_dataset;

   std::vector< int > select_columns;
   std::map< int, int > select_fields;

   std::set< std::string > filters;
   std::set< std::string > fetch_field_names;

   std::deque< std::vector< std::string > > row_cache;

   class_base* p_graph_parent;
   std::string graph_parent_fk_field;

   std::map< std::string, char > search_replace_separators;
   std::set< std::string > search_replace_has_opt_prefixing;

   bool is_singular;
   bool is_fetching;
   bool is_executing;
   bool is_preparing;

   bool is_after_store;

   bool is_being_cascaded;
   bool is_dynamic_enabled;

   bool iteration_starting;
   bool in_forwards_iteration;
   bool in_backwards_iteration;

   struct impl;
   impl* p_impl;

   class_base( const class_base& );
   class_base& operator =( const class_base& );

   class_base* p_owning_instance;
   class_base* p_dynamic_instance;

   class_pointer_base* p_class_pointer_base;

   bool get_sql_stmts( std::vector< std::string >& sql_stmts, std::set< std::string >& tx_key_info );

   bool has_skipped_empty_update( );

   void finish( );
   void cancel( );

   void clean_up( );

   void fetch( std::string& sql, bool check_only, bool use_lazy_key = false );
   void destroy( );

   virtual void clear( ) = 0;
   
   virtual void validate( unsigned state, bool is_internal ) = 0;
   virtual void validate_set_fields( std::set< std::string >& fields_set ) = 0;

   void after_fetch_from_db( );

   virtual void after_fetch( ) = 0;
   virtual void finalise_fetch( ) = 0;
   void perform_after_fetch( bool is_minimal = false, bool is_for_prepare = false );

   virtual void at_create( ) = 0;
   virtual void do_post_init( ) = 0;

   virtual void to_store( bool is_create, bool is_internal ) = 0;
   void perform_to_store( bool is_create, bool is_internal );

   virtual void for_store( bool is_create, bool is_internal ) = 0;
   virtual void after_store( bool is_create, bool is_internal ) = 0;

   virtual bool can_destroy( bool is_internal ) = 0;
   virtual void for_destroy( bool is_internal ) = 0;
   virtual void after_destroy( bool is_internal ) = 0;

   virtual void set_default_values( ) = 0;

   virtual bool is_filtered( ) const = 0;

   void cleanup_dynamic_instance( );
   void destroy_dynamic_instance( );
   void construct_dynamic_instance( );

   std::string get_lazy_fetch_key( ) const { return lazy_fetch_key; }

   class_base* fetch_dynamic_instance( std::string& sql, bool check_only );

   void set_op( op_type new_op, bool is_new_key );

   void set_iteration_starting( bool starting ) { iteration_starting = starting; }
   void set_is_in_iteration( bool is_in_iter, bool is_forwards = true );

   void set_is_dynamic_enabled( bool enabled ) { is_dynamic_enabled = enabled; }

   size_t get_index_num( ) const { return index_num; }
   void set_index_num( size_t new_index_num ) { index_num = new_index_num; }

   const std::string& get_ver_exp( ) const { return ver_exp; }
   void set_ver_exp( const std::string& new_ver_exp ) { ver_exp = new_ver_exp; }

   size_t get_lock_handle( ) const { return lock_handle; }
   void set_lock_handle( size_t new_lock_handle ) { lock_handle = new_lock_handle; }

   size_t get_xlock_handle( ) const { return xlock_handle; }
   void set_xlock_handle( size_t new_xlock_handle ) { xlock_handle = new_xlock_handle; }

   virtual const char* get_field_id( const std::string& name,
    bool* p_sql_numeric = 0, std::string* p_type_name = 0 ) const = 0;

   virtual const char* get_field_name( const std::string& id,
    bool* p_sql_numeric = 0, std::string* p_type_name = 0 ) const = 0;

   virtual void set_foreign_key_value( const std::string& field, const std::string& value ) = 0;

   virtual const std::string& get_foreign_key_value( const std::string& field ) = 0;

   virtual void get_foreign_key_values( foreign_key_data_container& foreign_key_values ) const = 0;

   virtual size_t get_num_foreign_key_children( bool is_internal = false ) const = 0;

   virtual class_base* get_next_foreign_key_child(
    size_t child_num, std::string& next_child_field, cascade_op op, bool is_internal = false ) = 0;

   virtual void add_extra_fixed_info( std::vector< std::pair< std::string, std::string > >& fixed_info ) const = 0;
   virtual void add_extra_paging_info( std::vector< std::pair< std::string, std::string > >& paging_info ) const = 0;

   void add_validation_error( const std::string& source, const std::string& error_message );

   void perform_field_search_replacements( );

   protected:
   validation_error_container validation_errors;

   void cleanup( );

   bool has_filter( const std::string& id_or_name ) const { return filters.count( "*" ) > 0 || filters.count( id_or_name ) > 0; }

   void add_search_replacement( const std::string& field, const std::string& search, const std::string& replace );

   void set_search_replace_separator( const std::string& field, char separator )
   {
      search_replace_separators.insert( make_pair( field, separator ) );
   }

   void set_search_replace_has_opt_prefixing( const std::string& field )
   {
      search_replace_has_opt_prefixing.insert( field );
   }

   void fetch_updated_instance( );

   void set_key( const std::string& new_key, bool skip_fk_handling = false );
   void set_clone_key( const std::string& new_clone_key ) { clone_key = new_clone_key; }

   void set_version( uint32_t new_version ) { version = new_version; }
   void set_revision( uint32_t new_revision ) { revision = new_revision; }

   void set_original_identity( const std::string& new_original_identity ) { original_identity = new_original_identity; }

   void set_graph_parent( class_base* p_parent, const std::string& fk_field, bool is_fk = false );

   void trace( const std::string& s ) const;
};

#  ifdef __BORLANDC__
std::string construct_class_identity( const class_base& cb );
#else
inline std::string construct_class_identity( const class_base& cb )
{
   std::string identity( cb.get_module_id( ) );
   identity += ':';
   identity += cb.get_class_id( );
   return identity;
}
#endif

struct class_base_accessor
{
   class_base_accessor( class_base& cb ) : cb( cb ) { }

   bool get_sql_stmts( std::vector< std::string >& sql_stmts, std::set< std::string >& tx_key_info ) { return cb.get_sql_stmts( sql_stmts, tx_key_info ); }

   bool has_skipped_empty_update( ) { return cb.has_skipped_empty_update( ); }

   void finish( ) { cb.finish( ); }
   void cancel( ) { cb.cancel( ); }

   void fetch( std::string& sql, bool check_only, bool use_lazy_key = false ) { cb.fetch( sql, check_only, use_lazy_key ); }
   void destroy( ) { cb.destroy( ); }

   void clear( ) { cb.clear( ); }

   void after_fetch_from_db( ) { cb.after_fetch_from_db( ); }
   void perform_after_fetch( bool is_minimal = false ) { cb.perform_after_fetch( is_minimal ); }

   void at_create( ) { cb.at_create( ); }

   void perform_to_store( bool is_create, bool is_internal ) { cb.perform_to_store( is_create, is_internal ); }

   void for_store( bool is_create, bool is_internal ) { cb.for_store( is_create, is_internal ); }
   void after_store( bool is_create, bool is_internal ) { cb.after_store( is_create, is_internal ); }

   bool can_destroy( bool is_internal ) { return cb.can_destroy( is_internal ); }
   void for_destroy( bool is_internal ) { cb.for_destroy( is_internal ); }
   void after_destroy( bool is_internal ) { cb.after_destroy( is_internal ); }

   void set_default_values( ) { cb.set_default_values( ); }

   void destroy_dynamic_instance( ) { cb.destroy_dynamic_instance( ); }
   void construct_dynamic_instance( ) { cb.construct_dynamic_instance( ); }

   class_base* fetch_dynamic_instance( std::string& sql, bool check_only ) { return cb.fetch_dynamic_instance( sql, check_only ); }

   std::string get_lazy_fetch_key( ) const { return cb.get_lazy_fetch_key( ); }

   void set_op( class_base::op_type new_op, bool is_new_key ) { cb.set_op( new_op, is_new_key ); }

   void set_is_dynamic_enabled( bool enabled ) { cb.set_is_dynamic_enabled( enabled ); }

   void set_is_executing( bool is_executing ) { cb.is_executing = is_executing; }

   void set_iteration_starting( bool starting ) { cb.set_iteration_starting( starting ); }
   void set_is_in_iteration( bool is_in_iter, bool is_forwards = true ) { cb.set_is_in_iteration( is_in_iter, is_forwards ); }

   void set_key( const std::string& new_key, bool skip_fk_handling = false ) { cb.set_key( new_key, skip_fk_handling ); }
   void set_clone_key( const std::string& new_clone_key ) { cb.set_clone_key( new_clone_key ); }

   void set_version( uint32_t new_version ) { cb.set_version( new_version ); }
   void set_revision( uint32_t new_revision ) { cb.set_revision( new_revision ); }

   void set_original_identity( const std::string& new_original_identity ) { cb.set_original_identity( new_original_identity ); }

   size_t get_index_num( ) const { return cb.get_index_num( ); }
   void set_index_num( size_t new_index_num ) { cb.set_index_num( new_index_num ); }

   const std::string& get_ver_exp( ) const { return cb.get_ver_exp( ); }
   void set_ver_exp( const std::string& new_ver_exp ) { cb.set_ver_exp( new_ver_exp ); }

   sql_dataset*& p_sql_dataset( ) { return cb.p_sql_dataset; }

   std::vector< int >& select_columns( ) { return cb.select_columns; }
   std::map< int, int >& select_fields( ) { return cb.select_fields; }

   std::set< std::string >& filters( ) { return cb.filters; }

   std::set< std::string >& fetch_field_names( ) { return cb.fetch_field_names; }

   std::deque< std::vector< std::string > >& row_cache( ) { return cb.row_cache; }

   size_t get_lock_handle( ) const { return cb.get_lock_handle( ); }
   void set_lock_handle( size_t new_lock_handle ) { cb.set_lock_handle( new_lock_handle ); }

   size_t get_xlock_handle( ) const { return cb.get_xlock_handle( ); }
   void set_xlock_handle( size_t new_xlock_handle ) { cb.set_xlock_handle( new_xlock_handle ); }

   const char* get_field_id( const std::string& name,
    bool* p_sql_numeric = 0, std::string* p_type_name = 0 ) { return cb.get_field_id( name, p_sql_numeric, p_type_name ); }

   const char* get_field_name( const std::string& id,
    bool* p_sql_numeric = 0, std::string* p_type_name = 0 ) { return cb.get_field_name( id, p_sql_numeric, p_type_name ); }

   std::string get_field_display_name( const std::string& id ) { return cb.get_field_display_name( id ); }

   size_t get_num_foreign_key_children( ) const { return cb.get_num_foreign_key_children( ); }

   class_base* get_next_foreign_key_child( size_t child_num, std::string& next_child_field, cascade_op op )
   {
      return cb.get_next_foreign_key_child( child_num, next_child_field, op );
   }

   void add_extra_fixed_info( std::vector< std::pair< std::string, std::string > >& fixed_info ) const
   {
      cb.add_extra_fixed_info( fixed_info );
   }

   void add_extra_paging_info( std::vector< std::pair< std::string, std::string > >& paging_info ) const
   {
      cb.add_extra_paging_info( paging_info );
   }

   void add_validation_error( const std::string& source, const std::string& error_message )
   {
      cb.add_validation_error( source, error_message );
   }

   class_base& cb;
};

class CLASS_BASE_DECL_SPEC class_base_filter
{
   public:
   class_base_filter( class_base& cb, const std::string& filter_ids );
   ~class_base_filter( );

   private:
   class_base_accessor cba;
   std::set< std::string > filters;
};

enum create_instance
{
   e_create_instance
};

void CLASS_BASE_DECL_SPEC throw_bad_class_pointer_init( class_base& cb );

struct class_pointer_base
{
   class_pointer_base( ) { }

   virtual void instance_destroyed( ) = 0;

   private:
   class_pointer_base( const class_pointer_base& );
   class_pointer_base& operator =( const class_pointer_base& );
};

template< typename CBD > class class_pointer : class_pointer_base
{
   public:
   class_pointer( )
    :
    p_ct( 0 )
   {
   }

   class_pointer( create_instance /*dummy*/ )
   {
      init( );
   }

   class_pointer( typename CBD::class_type* p )
   {
      init( p );
   }

   void init( typename CBD::class_type* p = 0 )
   {
      if( p )
      {
         p_ct = p;

         if( p != p->get_owning_instance( ) )
            p->set_class_pointer_base( this );
      }
      else
      {
         ap_cbd.reset( new CBD );

         p_ct = ap_cbd.get( );
         p_ct->set_dynamic_if_class_has_derivations( );
      }
   }

   ~class_pointer( )
   {
      p_ct = 0;
   }

   void instance_destroyed( )
   {
      p_ct = 0;
   }

   operator const void*( ) const { return p_ct; }

   typename CBD::class_type& operator *( ) const { return *dynamic_cast< CBD* >( p_ct->get_dynamic_instance( ) ); }
   typename CBD::class_type* operator ->( ) const { return dynamic_cast< CBD* >( p_ct->get_dynamic_instance( ) ); }

   private:
   std::auto_ptr< CBD > ap_cbd;
   typename CBD::class_type* p_ct;
};

std::string CLASS_BASE_DECL_SPEC sql_quote( const std::string& s );

bool CLASS_BASE_DECL_SPEC is_valid_int( const std::string& s );
bool CLASS_BASE_DECL_SPEC is_valid_bool( const std::string& s );
bool CLASS_BASE_DECL_SPEC is_valid_date( const std::string& s );
bool CLASS_BASE_DECL_SPEC is_valid_time( const std::string& s );
bool CLASS_BASE_DECL_SPEC is_valid_numeric( const std::string& s );
bool CLASS_BASE_DECL_SPEC is_valid_date_time( const std::string& s );

bool CLASS_BASE_DECL_SPEC is_valid_value( const std::string& s, primitive p,
 unsigned int max_size = 0, const char* p_min_value = 0, const char* p_max_value = 0 );

inline std::string to_string( const std::string& s ) { return s; }

template< typename T > inline std::string to_rep_string( const T& t ) { return to_string( t ); }

inline std::string to_string( const class_base& cb ) { return cb.get_key( ); }

inline bool operator ==( const class_base& cb, const std::string& s ) { return s == to_string( cb ); }
inline bool operator !=( const class_base& cb, const std::string& s ) { return s != to_string( cb ); }

void CLASS_BASE_DECL_SPEC from_string( class_base& cb, const std::string& s );

std::string CLASS_BASE_DECL_SPEC int_to_comparable_string( int i, bool prefix_with_sign = true, int max_digits = 0 );

std::string CLASS_BASE_DECL_SPEC construct_key_from_int( const std::string& prefix, int num );

template< typename T > inline int to_integer( const T& t ) { return t; }

inline bool check_equal( const bool b, const int i ) { return b == ( bool )i; }
inline bool check_not_equal( const bool b, const int i ) { return !( b == ( bool )i ); }

template< typename T, typename V > inline bool check_equal( const T& t, const V& v ) { return t == v; }
template< typename T, typename V > inline bool check_not_equal( const T& t, const V& v ) { return !( t == v ); }

inline void lazy_fetch( class_base* p_cb ) { p_cb->perform_lazy_fetch( ); }

void CLASS_BASE_DECL_SPEC wait( unsigned long ms );

std::string CLASS_BASE_DECL_SPEC get_uuid( );

std::string CLASS_BASE_DECL_SPEC get_ext( const std::string& filename );
std::string CLASS_BASE_DECL_SPEC get_path( const std::string& filename );

bool CLASS_BASE_DECL_SPEC exists_file( const std::string& filename, bool check_link_target = true );
void CLASS_BASE_DECL_SPEC remove_file( const std::string& filename );
void CLASS_BASE_DECL_SPEC rename_file( const std::string& oldname, const std::string& newname );

int64_t CLASS_BASE_DECL_SPEC size_file( const std::string& filename );
int64_t CLASS_BASE_DECL_SPEC last_mod_time( const std::string& filename );

void CLASS_BASE_DECL_SPEC copy_file( const std::string& source, const std::string& destination );
std::string CLASS_BASE_DECL_SPEC load_file( const std::string& filename, bool is_optional = false );

void CLASS_BASE_DECL_SPEC read_file_lines( const std::string& filename, std::set< std::string >& lines );
void CLASS_BASE_DECL_SPEC read_file_lines( const std::string& filename, std::vector< std::string >& lines );

void CLASS_BASE_DECL_SPEC link_file( const std::string& source, const std::string& name );

std::string CLASS_BASE_DECL_SPEC copy_class_file( const std::string& src_path,
 const std::string& dest_class_id, const std::string& dest_file_name, bool return_name_only = false, bool return_full_path = false );

inline void copy_field_or_file_and_field( class_base& dest, const std::string& dest_key,
 const std::string& dest_field_name, const class_base& src, const std::string& src_field_name, bool return_name_only )
{
   if( dest.is_file_field_name( dest_field_name ) )
      dest.set_field_value(
       dest.get_field_num( dest_field_name ), copy_class_file( src.get_field_value(
       src.get_field_num( src_field_name ) ), dest.get_class_id( ), dest_key, return_name_only ) );
   else
      dest.set_field_value( dest.get_field_num( dest_field_name ),
       src.get_field_value( src.get_field_num( src_field_name ) ) );
}

void CLASS_BASE_DECL_SPEC copy_class_files( const class_base& src, class_base& dest );

void CLASS_BASE_DECL_SPEC copy_class_files_for_clone(
 const std::vector< std::pair< std::string, std::string > >& file_field_name_and_values, class_base& dest );

std::string CLASS_BASE_DECL_SPEC get_app_dir( );
std::string CLASS_BASE_DECL_SPEC get_app_file( const std::string& module_name );

std::string CLASS_BASE_DECL_SPEC get_attached_file_path( const std::string& module_id, const std::string& class_id );

std::string CLASS_BASE_DECL_SPEC expand_lf_to_cr_lf( const std::string& input );

void CLASS_BASE_DECL_SPEC delete_directory_tree( const std::string& path );

void CLASS_BASE_DECL_SPEC create_directories_for_file_name( const std::string& file_name, bool allow_all_rwx = false );
std::string CLASS_BASE_DECL_SPEC get_directory_for_file_name( const std::string& file_name );

size_t CLASS_BASE_DECL_SPEC split_count( const std::string& s, char sep = ',' );

void CLASS_BASE_DECL_SPEC split_string( const std::string& s, std::set< std::string >& c, char sep = ',' );
void CLASS_BASE_DECL_SPEC split_string( const std::string& s, std::deque< std::string >& c, char sep = ',' );
void CLASS_BASE_DECL_SPEC split_string( const std::string& s, std::vector< std::string >& c, char sep = ',' );

void CLASS_BASE_DECL_SPEC split_string( const std::string& s, std::set< std::string >& c, const std::string& sep );
void CLASS_BASE_DECL_SPEC split_string( const std::string& s, std::deque< std::string >& c, const std::string& sep );
void CLASS_BASE_DECL_SPEC split_string( const std::string& s, std::vector< std::string >& c, const std::string& sep );

std::string CLASS_BASE_DECL_SPEC search_replace( const std::string& s, const std::string& search, const std::string& replace );
std::string CLASS_BASE_DECL_SPEC search_replace( const std::string& s,
 const std::string& search1, const std::string& replace1, const std::string& search2, const std::string& replace2 );

std::string CLASS_BASE_DECL_SPEC escaped_string( const std::string& s, const char* p_chars = 0, char esc = '\\' );

std::string CLASS_BASE_DECL_SPEC quoted_literal( const std::string& s, char esc = '\\', bool add_quotes = true );

inline std::string unquoted_literal( const std::string& s, char esc = '\\' ) { return quoted_literal( s, esc, false ); }

void CLASS_BASE_DECL_SPEC check_with_regex( const std::string& r, const std::string& s );

std::string CLASS_BASE_DECL_SPEC hash_sha1( const std::string& s );
std::string CLASS_BASE_DECL_SPEC hash_sha256( const std::string& s );

std::string CLASS_BASE_DECL_SPEC valid_utf8_filename( const std::string& str );
std::string CLASS_BASE_DECL_SPEC valid_non_utf8_filename( const std::string& str );

std::string CLASS_BASE_DECL_SPEC formatted_int( int n, const std::string& mask );
std::string CLASS_BASE_DECL_SPEC formatted_numeric( const numeric& n, const std::string& mask );

std::string CLASS_BASE_DECL_SPEC numeric_name( const std::string& s, bool show_plus_if_no_sign = false );

std::string CLASS_BASE_DECL_SPEC value_label( const std::string& s );
std::string CLASS_BASE_DECL_SPEC value_leftpart( const std::string& s );
std::string CLASS_BASE_DECL_SPEC value_rightpart( const std::string& s );

std::string CLASS_BASE_DECL_SPEC auto_int_increment( const std::string& current );

struct historical_daylight_info
{
   int year_start;
   int year_finish;

   std::string description;

   int start_month;
   int start_occurrence;
   int start_day_of_week;

   std::string start_time;

   int finish_month;
   int finish_occurrence;
   int finish_day_of_week;

   std::string finish_time;

   float utc_offset;
};

struct timezone_info
{
   timezone_info( )
    :
    daylight_year_start( 0 ),
    daylight_year_finish( 0 ),
    daylight_start_month( 0 ),
    daylight_start_occurrence( 0 ),
    daylight_start_day_of_week( 0 ),
    daylight_finish_month( 0 ),
    daylight_finish_occurrence( 0 ),
    daylight_finish_day_of_week( 0 ),
    daylight_utc_offset( 0 )
   {
   }

   std::string abbr;

   float utc_offset;
   std::string description;

   std::string daylight_abbr;

   int daylight_year_start;
   int daylight_year_finish;

   int daylight_start_month;
   int daylight_start_occurrence;
   int daylight_start_day_of_week;

   std::string daylight_start_time;

   int daylight_finish_month;
   int daylight_finish_occurrence;
   int daylight_finish_day_of_week;

   std::string daylight_finish_time;

   float daylight_utc_offset;

   std::vector< historical_daylight_info > historical_daylights;
};

void CLASS_BASE_DECL_SPEC generate_timezones_sio( const std::vector< timezone_info >& timezones );

struct daylight_bias_info
{
   daylight_bias_info( ) { }

   daylight_bias_info( int bias, std::string begin, std::string finish )
    :
    bias( bias ),
    begin( begin ),
    finish( finish )
   {
   }

   int bias;
   std::string begin;
   std::string finish;
};

typedef std::map< int, daylight_bias_info > years_info_container;

typedef years_info_container::iterator years_info_iterator;
typedef years_info_container::const_iterator years_info_const_iterator;
typedef years_info_container::value_type years_info_value_type;

struct daylight_savings_info
{
   years_info_container years_info;
};

struct timezone_data
{
   int utc_offset;
   std::string daylight_abbr;
   daylight_savings_info daylight_savings;
};

typedef std::map< std::string, timezone_data > timezone_container;

void setup_timezones( );

void CLASS_BASE_DECL_SPEC get_tz_info( const date_time& dt, std::string& tz_abbr, float& offset );

date_time CLASS_BASE_DECL_SPEC utc_to_local( const date_time& dt, const std::string& tz_abbr );
date_time CLASS_BASE_DECL_SPEC local_to_utc( const date_time& dt, const std::string& tz_abbr );

bool CLASS_BASE_DECL_SPEC schedulable_month_and_day( int month, int day );

void CLASS_BASE_DECL_SPEC add_class_map( const std::string& class_id,
 const std::string& map_id, const std::string& file_name, bool in_reverse = false );

void CLASS_BASE_DECL_SPEC remove_class_map( const std::string& class_id, const std::string& map_id );

std::string CLASS_BASE_DECL_SPEC get_class_map_value(
 const std::string& class_id, const std::string& map_id, const std::string& key );

struct user_account
{
   std::string sender;
   std::string username;
   std::string password;
};

void CLASS_BASE_DECL_SPEC send_email_message( const std::string& recipient,
 const std::string& subject, const std::string& message,
 const std::string& html_source, const std::vector< std::string >* p_extra_headers = 0,
 const std::vector< std::string >* p_file_names = 0, const std::string* p_tz_abbr = 0,
 const std::vector< std::string >* p_image_names = 0, const std::string* p_image_path_prefix = 0 );

void CLASS_BASE_DECL_SPEC send_email_message( const std::vector< std::string >& recipients,
 const std::string& subject, const std::string& message, const std::string& html_source,
 const std::vector< std::string >* p_extra_headers = 0, const std::vector< std::string >* p_file_names = 0,
 const std::string* p_tz_abbr = 0, const std::vector< std::string >* p_image_names = 0,
 const std::string* p_image_path_prefix = 0 );

void CLASS_BASE_DECL_SPEC send_email_message( const user_account& account,
 const std::vector< std::string >& recipients, const std::string& subject, const std::string& message,
 const std::string& html_source, const std::vector< std::string >* p_extra_headers = 0,
 const std::vector< std::string >* p_file_names = 0, const std::string* p_tz_abbr = 0,
 const std::vector< std::string >* p_image_names = 0, const std::string* p_image_path_prefix = 0 );

std::string CLASS_BASE_DECL_SPEC generate_hashcash( const std::string& recipient );
bool CLASS_BASE_DECL_SPEC has_valid_hashcash( const std::string& value );

std::string CLASS_BASE_DECL_SPEC check_email_headers(
 const std::vector< std::string >& headers, bool create_script_output = false );

void CLASS_BASE_DECL_SPEC fetch_email_messages( const std::string& file_name_prefix, bool skip_scripts = false );

void CLASS_BASE_DECL_SPEC fetch_email_messages(
 std::vector< std::pair< bool, std::string > >& messages, bool skip_scripts = false );

void CLASS_BASE_DECL_SPEC fetch_email_messages( const user_account& account,
 std::vector< std::pair< bool, std::string > >& messages, bool skip_scripts = false );

std::string CLASS_BASE_DECL_SPEC decode_email_header( const std::string& header );

void CLASS_BASE_DECL_SPEC decode_mime_message( const std::string& mime, std::string& message,
 std::string& html_message, std::vector< std::pair< std::string, std::string > >& attachments );

void CLASS_BASE_DECL_SPEC parse_email_address( const std::string& address, std::string& name, std::string& email );

void CLASS_BASE_DECL_SPEC save_attachment(
 const std::string& encoding, const std::string& data, const std::string& output_file );

std::string CLASS_BASE_DECL_SPEC remove_html_scripts( const std::string& html );

std::string CLASS_BASE_DECL_SPEC convert_html_to_text( const std::string& html );

std::string CLASS_BASE_DECL_SPEC create_html_embedded_image( const std::string& source_file );

void CLASS_BASE_DECL_SPEC execute_command( numeric& n, const std::string& cmd_and_args, std::string& retval );
void CLASS_BASE_DECL_SPEC execute_command( date_time& n, const std::string& cmd_and_args, std::string& retval );

void CLASS_BASE_DECL_SPEC meta_relationship_child_name( std::string& name,
 const std::string& child_name, const std::string& parent_name, const std::string& separator );

const int c_sql_std_char_size = 128;
const int c_sql_small_char_size = 32;

enum sql_char_type
{
   e_sql_char_type_std,
   e_sql_char_type_small,
   e_sql_char_type_large,
   e_sql_char_type_security,
   e_sql_char_type_foreign_key,
};

std::string CLASS_BASE_DECL_SPEC meta_sql_type(
 const std::string& field_type, bool is_mandatory, sql_char_type char_type );

std::string CLASS_BASE_DECL_SPEC meta_field_uom( int uom );

int CLASS_BASE_DECL_SPEC meta_field_type_primitive( const std::string& type );

std::string CLASS_BASE_DECL_SPEC meta_field_type_name( int primitive,
 bool mandatory, const std::string& parent_class_name, const std::string& model_name, bool* p_is_customised = 0 );

std::string CLASS_BASE_DECL_SPEC meta_field_domain_type( const std::string& enum_id,
 int primitive, int max_size, const std::string& min_value, const std::string& max_value,
 int numeric_digits, int numeric_decimals, int string_domain, int date_precision, int time_precision,
 bool show_plus_sign, int zero_padding, int fraction_limit, std::string& mask, std::string* p_tmask = 0 );

std::string CLASS_BASE_DECL_SPEC meta_field_extras( int uom, int extra, bool transient, int max_size,
 const std::string& enum_id, int primitive, const std::string& min_value, const std::string& max_value,
 int numeric_digits, int numeric_decimals, int string_domain, int date_precision, int time_precision,
 bool show_plus_sign, int zero_padding, int int_type, int numeric_type );

std::string CLASS_BASE_DECL_SPEC meta_procedure_arg_type( int primitive );

#endif

