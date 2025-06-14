// Copyright (c) 2004-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

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

#  include "common.h"
#  include "ptypes.h"
#  include "utilities.h"
#  include "ciyam_core.h"

#  ifdef CIYAM_BASE_IMPL
#     define CIYAM_BASE_DECL_SPEC DYNAMIC_EXPORT
#  else
#     define CIYAM_BASE_DECL_SPEC DYNAMIC_IMPORT
#  endif

#  define MODULE_TRACE( x ) get_obj( ).trace( x )

class ods;
class numeric;
class date_time;
class class_base;

struct sql_data;

const char* const c_key_field = "@key";

const char* const c_group_field = "@group";
const char* const c_level_field = "@level";
const char* const c_order_field = "@order";
const char* const c_owner_field = "@owner";

const char* const c_dtm_now = "@now";
const char* const c_date_today = "@today";
const char* const c_date_tomorrow = "@tomorrow";

const char* const c_object_variable_skip_fk_handling = "@skip_fk_handling";

struct class_cascade;
struct class_pointer_base;
struct class_base_accessor;

enum cascade_op
{
   e_cascade_op_none,
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

   field_info( const std::string& id,
    const std::string& name, const std::string& type_name, bool mandatory,
    const char* p_scope = 0, const char* p_change = 0, bool is_owner_fk = false,
    bool is_encrypted = false, bool is_transient = false, bool is_attachment = false )
    :
    id( id ),
    name( name ),
    type_name( type_name ),
    mandatory( mandatory ),
    is_owner_fk( is_owner_fk ),
    is_encrypted( is_encrypted ),
    is_transient( is_transient ),
    is_attachment( is_attachment )
   {
      if( p_scope )
         scope = std::string( p_scope );

      if( p_change )
         change = std::string( p_change );
   }

   std::string id;
   std::string name;
   std::string type_name;

   std::string scope;
   std::string change;

   bool mandatory;
   bool is_owner_fk;
   bool is_encrypted;
   bool is_transient;
   bool is_attachment;
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

struct procedure_info
{
   procedure_info( ) { }

   procedure_info( const std::string& name, const char* p_access_info = 0 )
    :
    name( name )
   {
      if( p_access_info )
         access_info = std::string( p_access_info );
   }

   std::string name;
   std::string access_info;
};

typedef std::map< std::string, procedure_info > procedure_info_container;
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

struct CIYAM_BASE_DECL_SPEC class_cascade
{
   class_cascade( class_base& cb );
   ~class_cascade( );

   struct impl;
   impl* p_impl;
};

class CIYAM_BASE_DECL_SPEC class_base
{
   friend struct class_cascade;
   friend struct class_cascade::impl;

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
   void prepare( bool for_create, bool call_to_store = true, bool starting_iteration = false );

   bool is_valid( bool is_internal = false, std::set< std::string >* p_fields_set = 0 );
   bool has_changed( ) const;

   void link_to_graph_parent( );
   void unlink_from_graph_parent( );

   void begin_review( begin_review_rc* p_rc = 0 ) { begin_review( get_key( ), p_rc ); }
   void begin_review( const std::string& key, begin_review_rc* p_rc = 0 );
   void finish_review( );

   inline void perform_fetch( perform_fetch_rc* p_rc = 0, bool force = false, bool sys_only_fields = false )
   {
      perform_fetch( get_key( ), p_rc, force, sys_only_fields );
   }

   void perform_fetch( const std::string& key_info, perform_fetch_rc* p_rc = 0, bool force = false, bool sys_only_fields = false );

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

   bool get_is_for_peer( ) const;

   std::string get_peer_identity( ) const;

   void set_is_for_peer( const std::string& identity );

   void copy_all_field_values( const class_base& src );
   void copy_original_field_values( const class_base& src );

   bool filtered( ) const;

   bool has_transient_filter_fields( ) const;

   std::string get_validation_errors( validation_errors_type type = e_validation_errors_type_all );

   inline uint16_t get_version( ) const { return version; }
   inline uint64_t get_revision( ) const { return revision; }
   inline uint64_t get_security( ) const { return security; }

   std::string get_version_info( ) const;

   const std::string get_current_identity( ) const;
   inline const std::string& get_original_identity( ) const { return original_identity; }

   inline op_type get_op( ) const { return op; }

   inline bool get_is_in_op( ) const { return op != e_op_type_none; }

   inline bool get_is_creating( ) const { return op == e_op_type_create; }
   inline bool get_is_updating( ) const { return op == e_op_type_update; }
   inline bool get_is_reviewing( ) const { return op == e_op_type_review; }
   inline bool get_is_destroying( ) const { return op == e_op_type_destroy; }

   inline bool get_is_editing( ) const { return op == e_op_type_create || op == e_op_type_update; }

   inline bool get_is_transforming( ) const
   {
      return op == e_op_type_create || op == e_op_type_update || op == e_op_type_destroy;
   }

   inline bool get_is_minimal_update( ) const { return op == e_op_type_update && utype == e_update_type_minimal; }

   inline bool get_is_fetching( ) const { return is_fetching; }
   inline bool get_is_executing( ) const { return is_executing; }
   inline bool get_is_preparing( ) const { return is_preparing; }
   inline bool get_is_iterating( ) const { return in_forwards_iteration || in_backwards_iteration; }

   inline bool get_is_starting_iteration( ) const { return iteration_starting; }

   inline bool get_is_in_forwards_iteration( ) const { return in_forwards_iteration; }
   inline bool get_is_in_backwards_iteration( ) const { return in_backwards_iteration; }

   const std::string& get_key( ) const;
   inline operator const std::string& ( ) const { return get_key( ); }

   inline const std::string& get_clone_key( ) const { return clone_key; }
   inline void clear_clone_key( ) { clone_key.erase( ); }

   virtual const std::string& get_fixed_key( ) const;

   std::string get_parent_key( ) const;

   std::string get_attached_file_path( const std::string& file_name ) const;
   inline std::string get_attached_file_path( ) const { return get_attached_file_path( "" ); }

   inline bool get_is_singular( ) const { return is_singular; }

   inline bool get_is_after_store( ) const { return is_after_store; }

   inline bool get_is_being_cascaded( ) const { return is_being_cascaded; }
   inline bool get_is_dynamic_enabled( ) const { return is_dynamic_enabled; }

   inline class_base* get_owning_instance( ) { return p_owning_instance ? p_owning_instance : this; }
   inline class_base* get_dynamic_instance( ) const { return p_dynamic_instance; }

   int get_graph_depth( ) const;

   class_base* get_graph_root( );
   const class_base* get_graph_root( ) const;

   inline class_base* get_graph_parent( ) { return p_graph_parent; }
   inline const class_base* get_graph_parent( ) const { return p_graph_parent; }

   inline const std::string& get_graph_parent_fk_field( ) const { return graph_parent_fk_field; }

   virtual std::string get_raw_variable( const std::string& name ) const;

   bool has_variable( const std::string& name ) const;

   std::string get_variable( const std::string& name_or_expr ) const;
   void set_variable( const std::string& name, const std::string& value );

   bool has_field_changed( int field ) const;

   bool needs_field_value( const std::string& field_name ) const;
   bool needs_field_value( const std::string& field_name, const std::set< std::string >& names ) const;

   std::string get_original_field_value( int field ) const;

   inline std::string get_original_field_value( const std::string& field ) const
   {
      return get_original_field_value( get_field_num( field ) );
   }

   enum field_label_type
   {
      e_field_label_type_name,
      e_field_label_type_full_id,
      e_field_label_type_short_id
   };

   enum field_include_type
   {
      e_field_include_type_normal,
      e_field_include_type_modified,
      e_field_include_type_exhaustive
   };

   std::string get_fields_and_values(
    field_label_type label_type = e_field_label_type_full_id,
    field_include_type include_type = e_field_include_type_normal,
    const std::vector< std::string >* p_initial_field_values = 0 ) const;

   virtual std::string get_field_value( int field ) const = 0;
   virtual void set_field_value( int field, const std::string& value ) = 0;

   virtual void set_field_default( int field ) = 0;
   virtual void set_field_default( const std::string& field ) = 0;

   virtual bool has_field_changed( const std::string& field ) const = 0;

   virtual bool is_field_default( int field ) const = 0;
   virtual bool is_field_default( const std::string& field ) const = 0;

   virtual bool is_field_encrypted( int field ) const = 0;
   virtual bool is_field_transient( int field ) const = 0;

   virtual std::string get_field_id( int field ) const = 0;
   virtual std::string get_field_name( int field ) const = 0;

   std::string get_short_field_id( int field ) const;

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

   virtual int get_class_type( ) const = 0;
   virtual int get_persistence_type( ) const = 0;

   virtual std::string get_persistence_extra( ) const = 0;

   virtual std::string get_create_instance_info( ) const = 0;
   virtual std::string get_update_instance_info( ) const = 0;
   virtual std::string get_destroy_instance_info( ) const = 0;

   virtual std::string get_execute_procedure_info( const std::string& procedure_id ) const = 0;

   virtual bool get_is_alias( ) const = 0;
   virtual void get_alias_base_info( std::pair< std::string, std::string >& alias_base_info ) const = 0;

   virtual void get_base_class_info( std::vector< std::pair< std::string, std::string > >& base_class_info ) const = 0;

   virtual void clear_foreign_key( const std::string& fk_field_name ) = 0;

   virtual class_base& get_or_create_graph_child( const std::string& context ) = 0;

   virtual void get_class_info( class_info_container& class_info ) const = 0;
   virtual void get_field_info( field_info_container& field_info ) const = 0;
   virtual void get_foreign_key_info( foreign_key_info_container& foreign_key_info ) const = 0;

   virtual int get_num_fields( bool* p_done = 0, const std::string* p_class_name = 0 ) const = 0;

   virtual std::string& get_group_field_name( ) const = 0;
   virtual std::string& get_level_field_name( ) const = 0;
   virtual std::string& get_order_field_name( ) const = 0;
   virtual std::string& get_owner_field_name( ) const = 0;

   virtual bool is_file_field( const std::string& id_or_name ) const = 0;
   virtual void get_file_field_names( std::vector< std::string >& file_field_names ) const = 0;

   virtual std::string get_field_uom_symbol( const std::string& id_or_name ) const = 0;
   virtual std::string get_field_display_name( const std::string& id_or_name ) const = 0;

   void get_alternative_key_field_info( std::vector< key_field_info_container >& all_key_field_info ) const;

   virtual const procedure_info_container& get_procedure_info( ) const = 0;

   virtual void get_text_search_fields( std::vector< std::string >& fields ) const = 0;

   virtual void get_all_enum_pairs( std::vector< std::pair< std::string, std::string > >& pairs ) const = 0;

   virtual void get_all_index_pairs( std::vector< std::pair< std::string, std::string > >& pairs ) const = 0;
   virtual void get_all_unique_indexes( std::vector< std::string >& unique_indexes ) const = 0;

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
    std::vector< std::string >& sql_stmts, std::set< std::string >& tx_key_info,
    std::vector< std::string >* p_sql_undo_stmts = 0 ) const = 0;

   void generate_sql( const std::string& class_name, generate_sql_type type,
    std::vector< std::string >& sql_stmts, std::set< std::string >& tx_key_info,
    std::vector< std::string >* p_sql_undo_stmts = 0 ) const;

   std::string generate_sql_insert( const std::string& class_name, std::string* p_undo_stmt = 0 ) const;
   std::string generate_sql_update( const std::string& class_name, std::string* p_undo_stmt = 0 ) const;
   std::string generate_sql_delete( const std::string& class_name, std::string* p_undo_stmt = 0 ) const;

   void set_class_pointer_base( class_pointer_base* p_cpb );

   static ods* ods_instance( );
   static void ods_instance( ods* p_ods );

   private:
   uint16_t version;
   uint64_t revision;
   uint64_t security;

   uint64_t original_revision;
   uint64_t original_security;

   std::string original_identity;
   std::vector< std::string > original_values;

   op_type op;
   update_type utype;

   std::string key;
   std::string clone_key;

   std::string lazy_fetch_key;

   uint16_t last_lazy_fetch_ver;
   uint64_t last_lazy_fetch_rev;
   uint64_t last_lazy_fetch_sec;

   std::string last_lazy_fetch_key;
   std::string last_lazy_fetch_identity;
   std::vector< std::string > last_lazy_fetch_field_values;

   size_t index_num;
   size_t lock_handle;
   size_t xlock_handle;

   std::string ver_exp;

   sql_data* p_sql_data;

   std::vector< int > field_nums;

   std::vector< int > select_columns;
   std::map< int, int > select_fields;

   std::set< std::string > filters;
   std::set< std::string > fetch_field_names;

   std::deque< std::vector< std::string > > row_cache;

   std::map< std::string, std::string > transient_filter_field_values;

   class_base* p_graph_parent;
   std::string graph_parent_fk_field;

   std::map< std::string, char > search_replace_separators;
   std::set< std::string > search_replace_has_opt_prefixing;

   bool force_fetch;

   bool in_op_begin;

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

   bool get_sql_stmts( std::vector< std::string >& sql_stmts,
    std::set< std::string >& tx_key_info, std::vector< std::string >* p_sql_undo_stmts = 0 );

   bool has_skipped_empty_update( );

   void finish( );
   void cancel( );

   void clean_up( );

   void fetch( std::string& sql, bool check_only, bool use_lazy_key = false );
   void destroy( );

   virtual void clear( ) = 0;
   
   virtual void validate( uint64_t state, bool is_internal ) = 0;
   virtual void validate_set_fields( std::set< std::string >& fields_set ) = 0;

   void after_fetch_from_db( );
   void cache_original_values( );

   virtual void after_fetch( ) = 0;
   virtual void finalise_fetch( bool skip_set_original = false ) = 0;

   void perform_after_fetch( bool is_minimal = false, bool is_for_prepare = false );

   virtual void at_create( ) = 0;
   virtual void post_init( ) = 0;

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

   inline std::string get_lazy_fetch_key( ) const { return lazy_fetch_key; }

   class_base* fetch_dynamic_instance( std::string& sql, bool check_only );

   void set_op( op_type new_op, bool is_new_key );

   void set_is_executing( bool executing );

   void set_is_after_store( bool after_store );

   inline void set_iteration_starting( bool starting ) { iteration_starting = starting; }
   void set_is_in_iteration( bool is_in_iter, bool is_forwards = true );

   inline void set_is_dynamic_enabled( bool enabled ) { is_dynamic_enabled = enabled; }

   inline size_t get_index_num( ) const { return index_num; }
   inline void set_index_num( size_t new_index_num ) { index_num = new_index_num; }

   inline const std::string& get_ver_exp( ) const { return ver_exp; }
   inline void set_ver_exp( const std::string& new_ver_exp ) { ver_exp = new_ver_exp; }

   inline bool get_in_op_begin( ) const { return in_op_begin; }
   inline void set_in_op_begin( bool new_in_op_begin ) { in_op_begin = new_in_op_begin; }

   inline size_t get_lock_handle( ) const { return lock_handle; }
   inline void set_lock_handle( size_t new_lock_handle ) { lock_handle = new_lock_handle; }

   inline size_t get_xlock_handle( ) const { return xlock_handle; }
   inline void set_xlock_handle( size_t new_xlock_handle ) { xlock_handle = new_xlock_handle; }

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

   inline bool has_filter( const std::string& id_or_name ) const
   {
      return filters.count( "*" ) > 0 || filters.count( id_or_name ) > 0;
   }

   void add_search_replacement( const std::string& field, const std::string& search, const std::string& replace );

   inline void set_search_replace_separator( const std::string& field, char separator )
   {
      search_replace_separators.insert( make_pair( field, separator ) );
   }

   inline void set_search_replace_has_opt_prefixing( const std::string& field )
   {
      search_replace_has_opt_prefixing.insert( field );
   }

   void fetch_updated_instance( bool force = false );

   void add_required_transients( std::set< std::string >& required_transients );

   void set_new_original_values( );

   void set_key( const std::string& new_key, bool skip_fk_handling = false );
   inline void set_clone_key( const std::string& new_clone_key ) { clone_key = new_clone_key; }

   inline void set_version( uint16_t new_version ) { version = new_version; }
   inline void set_revision( uint64_t new_revision ) { revision = new_revision; }
   inline void set_security( uint64_t new_security ) { security = new_security; }

   inline void set_original_revision( uint64_t new_revision ) { original_revision = new_revision; }
   inline void set_original_identity( const std::string& new_original_identity ) { original_identity = new_original_identity; }

   void set_graph_parent( class_base* p_parent, const std::string& fk_field, bool is_fk = false );

   void trace( const std::string& s ) const;
};

#  ifdef __BORLANDC__
std::string construct_class_identity( const class_base& cb );
#  else
inline std::string construct_class_identity( const class_base& cb )
{
   std::string identity( cb.get_module_id( ) );
   identity += ':';
   identity += cb.get_class_id( );
   return identity;
}
#  endif

struct variable_getter
{
   virtual std::string get_value( const std::string& name ) const = 0;
};

struct class_variable_getter : variable_getter
{
   class_variable_getter( const class_base& cb ) : cb( cb ) { }

   std::string get_value( const std::string& name ) const { return cb.get_raw_variable( name ); }

   const class_base& cb;
};

struct variable_expression
{
   variable_expression( const std::string& expr, const variable_getter& getter )
    :
    getter( getter ),
    invert_1( false ),
    invert_2( false ),
    binary_type( '\0' ),
    is_binary_expression( false ),
    is_logical_expression( false )
   {
      std::string::size_type pos = expr.find_first_of( "&|" );

      if( pos != std::string::npos )
      {
         binary_type = expr[ pos ];

         is_binary_expression = is_logical_expression = true;

         var1 = expr.substr( 0, pos );
         var2 = expr.substr( pos + 1 );
      }
      else
         var1 = expr;

      if( !var1.empty( ) && ( ( var1[ 0 ] == '!' ) || ( var1[ 0 ] == '?' ) ) )
      {
         if( var1[ 0 ] == '!' )
            invert_1 = true;

         var1.erase( 0, 1 );
         is_logical_expression = true;
      }

      if( !var2.empty( ) && ( ( var2[ 0 ] == '!' ) || ( var2[ 0 ] == '?' ) ) )
      {
         if( var2[ 0 ] == '!' )
            invert_2 = true;

         var2.erase( 0, 1 );
         is_logical_expression = true;
      }
   }

   std::string get_value( )
   {
      if( !is_logical_expression )
         return getter.get_value( var1 );
      else if( !is_binary_expression )
      {
         if( invert_1 )
            return getter.get_value( var1 ).empty( ) ? "1" : "";
         else
            return !getter.get_value( var1 ).empty( ) ? "1" : "";
      }
      else
      {
         bool result = false;

         if( invert_1 )
            result = getter.get_value( var1 ).empty( );
         else
            result = !getter.get_value( var1 ).empty( );

         if( binary_type == '&' )
         {
            if( invert_2 )
               result = ( result && getter.get_value( var2 ).empty( ) );
            else
               result = ( result && !getter.get_value( var2 ).empty( ) );
         }
         else
         {
            if( invert_2 )
               result = ( result || getter.get_value( var2 ).empty( ) );
            else
               result = ( result || !getter.get_value( var2 ).empty( ) );
         }

         return ( result ? "1" : "" );
      }
   }

   std::string var1;
   std::string var2;

   bool invert_1;
   bool invert_2;

   char binary_type;

   bool is_binary_expression;
   bool is_logical_expression;

   const variable_getter& getter;
};

struct temporary_object_variable
{
   temporary_object_variable( class_base& cb, const std::string& name, const std::string& value )
    :
    cb( cb ),
    name( name )
   {
      original_value = cb.get_raw_variable( name );
      cb.set_variable( name, value );
   }

   ~temporary_object_variable( )
   {
      cb.set_variable( name, original_value );
   }

   class_base& cb;

   std::string name;
   std::string original_value;
};

struct CIYAM_BASE_DECL_SPEC unique_items_object_variable
{
   unique_items_object_variable( class_base& cb, const std::string& name );
   ~unique_items_object_variable( );

   void check_unique( );

   bool is_unique( ) const;

   struct impl;
   impl* p_impl;
};

struct CIYAM_BASE_DECL_SPEC procedure_progress
{
   procedure_progress( size_t seconds = 10 );
   ~procedure_progress( );

   void check_progress( size_t amount );

   struct impl;
   impl* p_impl;
};

struct class_base_accessor
{
   class_base_accessor( class_base& cb ) : cb( cb ) { }

   class_base& get_obj( ) { return cb; }

   bool get_sql_stmts( std::vector< std::string >& sql_stmts,
    std::set< std::string >& tx_key_info, std::vector< std::string >* p_sql_undo_stmts = 0 )
   {
      return cb.get_sql_stmts( sql_stmts, tx_key_info, p_sql_undo_stmts );
   }

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

   void set_is_executing( bool is_executing ) { cb.set_is_executing( is_executing ); }

   void set_is_after_store( bool is_after_store ) { cb.set_is_after_store( is_after_store ); }

   void set_is_dynamic_enabled( bool enabled ) { cb.set_is_dynamic_enabled( enabled ); }

   void set_iteration_starting( bool starting ) { cb.set_iteration_starting( starting ); }
   void set_is_in_iteration( bool is_in_iter, bool is_forwards = true ) { cb.set_is_in_iteration( is_in_iter, is_forwards ); }

   void set_key( const std::string& new_key, bool skip_fk_handling = false ) { cb.set_key( new_key, skip_fk_handling ); }
   void set_clone_key( const std::string& new_clone_key ) { cb.set_clone_key( new_clone_key ); }

   void set_version( uint16_t new_version ) { cb.set_version( new_version ); }
   void set_revision( uint64_t new_revision ) { cb.set_revision( new_revision ); }
   void set_security( uint64_t new_security ) { cb.set_security( new_security ); }

   void set_original_revision( uint64_t new_revision ) { cb.set_original_revision( new_revision ); }
   void set_original_identity( const std::string& new_original_identity ) { cb.set_original_identity( new_original_identity ); }

   size_t get_index_num( ) const { return cb.get_index_num( ); }
   void set_index_num( size_t new_index_num ) { cb.set_index_num( new_index_num ); }

   const std::string& get_ver_exp( ) const { return cb.get_ver_exp( ); }
   void set_ver_exp( const std::string& new_ver_exp ) { cb.set_ver_exp( new_ver_exp ); }

   sql_data*& p_sql_data( ) { return cb.p_sql_data; }

   std::vector< int >& field_nums( ) { return cb.field_nums; }

   std::vector< int >& select_columns( ) { return cb.select_columns; }
   std::map< int, int >& select_fields( ) { return cb.select_fields; }

   std::set< std::string >& filters( ) { return cb.filters; }

   std::set< std::string >& fetch_field_names( ) { return cb.fetch_field_names; }

   std::deque< std::vector< std::string > >& row_cache( ) { return cb.row_cache; }

   std::map< std::string, std::string >& transient_filter_field_values( ) { return cb.transient_filter_field_values; }

   bool get_in_op_begin( ) const { return cb.get_in_op_begin( ); }
   void set_in_op_begin( bool new_in_op_begin ) { cb.set_in_op_begin( new_in_op_begin ); }

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

class CIYAM_BASE_DECL_SPEC class_base_filter
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

void CIYAM_BASE_DECL_SPEC throw_bad_class_pointer_init( class_base& cb );

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
   typedef void ( class_pointer< CBD >::*bool_type )( ) const;
   void this_type_does_not_support_comparisons( ) const { }

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

   operator bool_type( ) const
   {
      if( !p_ct )
         return 0;
      else
         return &class_pointer< CBD >::this_type_does_not_support_comparisons;
   }

   template< typename T > bool operator ==( const T& rhs ) const
   {
      this_type_does_not_support_comparisons( );
      return false;
   }

   template< typename T > bool operator !=( const T& rhs ) const
   {
      this_type_does_not_support_comparisons( );
      return false;
   }

   typename CBD::class_type& operator *( ) const { return *dynamic_cast< CBD* >( p_ct->get_dynamic_instance( ) ); }
   typename CBD::class_type* operator ->( ) const { return dynamic_cast< CBD* >( p_ct->get_dynamic_instance( ) ); }

   private:
   std::auto_ptr< CBD > ap_cbd;
   typename CBD::class_type* p_ct;
};

std::string CIYAM_BASE_DECL_SPEC sql_quote( const std::string& s );

bool CIYAM_BASE_DECL_SPEC is_valid_int( const std::string& s );
bool CIYAM_BASE_DECL_SPEC is_valid_bool( const std::string& s );
bool CIYAM_BASE_DECL_SPEC is_valid_date( const std::string& s );
bool CIYAM_BASE_DECL_SPEC is_valid_time( const std::string& s );
bool CIYAM_BASE_DECL_SPEC is_valid_numeric( const std::string& s );
bool CIYAM_BASE_DECL_SPEC is_valid_date_time( const std::string& s );

bool CIYAM_BASE_DECL_SPEC is_valid_value( const std::string& s, primitive p,
 unsigned int max_size = 0, const char* p_min_value = 0, const char* p_max_value = 0 );

bool CIYAM_BASE_DECL_SPEC is_valid_file_name( const std::string& name, bool allow_directory_path = true );

inline std::string to_string( const std::string& s ) { return s; }

template< typename T > inline std::string to_rep_string( const T& t ) { return to_string( t ); }

inline std::string to_string( const class_base& cb ) { return cb.get_key( ); }

inline bool operator ==( const class_base& cb, const std::string& s ) { return s == to_string( cb ); }
inline bool operator !=( const class_base& cb, const std::string& s ) { return s != to_string( cb ); }

void CIYAM_BASE_DECL_SPEC from_string( class_base& cb, const std::string& s );

std::string CIYAM_BASE_DECL_SPEC incremented_key_val( const std::string& s, const numeric& amt_to_add );

std::string CIYAM_BASE_DECL_SPEC construct_key_from_int( const std::string& prefix, int num, int num_digits = 5 );

template< typename T > inline int to_integer( const T& t ) { return t; }

inline bool check_equal( const bool b, const int i ) { return b == ( bool )i; }
inline bool check_not_equal( const bool b, const int i ) { return !( b == ( bool )i ); }

template< typename T, typename V > inline bool check_equal( const T& t, const V& v ) { return t == v; }
template< typename T, typename V > inline bool check_not_equal( const T& t, const V& v ) { return !( t == v ); }

inline void lazy_fetch( class_base* p_cb ) { p_cb->perform_lazy_fetch( ); }

void CIYAM_BASE_DECL_SPEC wait( unsigned long ms );

std::string CIYAM_BASE_DECL_SPEC get_uuid( );

uint32_t CIYAM_BASE_DECL_SPEC get_random( );

std::string CIYAM_BASE_DECL_SPEC get_random_hash( );

int64_t CIYAM_BASE_DECL_SPEC get_unix_time( bool use_dtm = true );

std::string CIYAM_BASE_DECL_SPEC get_soundex( const std::string& str, bool skip_prefix_specials = true );

std::string CIYAM_BASE_DECL_SPEC get_notifier_files_viewed( const std::string& watch_root );

std::string CIYAM_BASE_DECL_SPEC extract_unviewed_file_list(
 const std::string& file_list, const std::string& viewed_list, bool file_list_includes_sizes = true );

std::string CIYAM_BASE_DECL_SPEC get_ext( const std::string& filename );
std::string CIYAM_BASE_DECL_SPEC get_path( const std::string& filename );

bool CIYAM_BASE_DECL_SPEC exists_file( const std::string& filename, bool check_link_target = true );
bool CIYAM_BASE_DECL_SPEC exists_files( const std::string& filenames, char sep = ',', bool check_link_target = true );

void CIYAM_BASE_DECL_SPEC remove_file( const std::string& filename );
void CIYAM_BASE_DECL_SPEC remove_files( const std::string& filenames, char sep = ',' );

void CIYAM_BASE_DECL_SPEC rename_file( const std::string& oldname, const std::string& newname );

int64_t CIYAM_BASE_DECL_SPEC size_file( const std::string& filename );

std::string CIYAM_BASE_DECL_SPEC size_file_info( const std::string& filename );

int64_t CIYAM_BASE_DECL_SPEC last_mod_time( const std::string& filename );

void CIYAM_BASE_DECL_SPEC copy_file( const std::string& source, const std::string& destination );

void CIYAM_BASE_DECL_SPEC append_file( const std::string& source, const std::string& destination );

void CIYAM_BASE_DECL_SPEC copy_files(
 const std::string& source_files, const std::string& destination_files, char sep = ',' );

std::string CIYAM_BASE_DECL_SPEC load_file( const std::string& filename, bool is_optional = false );

void CIYAM_BASE_DECL_SPEC save_file( const std::string& filename, const std::string& data );

void CIYAM_BASE_DECL_SPEC touch_file( const std::string& filename, bool only_if_exists = false );

void CIYAM_BASE_DECL_SPEC read_file_lines( const std::string& filename, std::set< std::string >& lines );
void CIYAM_BASE_DECL_SPEC read_file_lines( const std::string& filename, std::vector< std::string >& lines );

void CIYAM_BASE_DECL_SPEC link_file( const std::string& source, const std::string& name );

std::string CIYAM_BASE_DECL_SPEC copy_class_file(
 const std::string& src_path, const std::string& dest_class_id, const std::string& dest_file_name,
 bool copy_only_if_missing = false, bool return_full_path = false, const std::string* p_dest_directory = 0 );

inline void copy_field_or_file_and_field( class_base& dest, const std::string& dest_key,
 const std::string& dest_field_name, const class_base& src, const std::string& src_field_name, bool copy_only_if_missing = false )
{
   if( !dest.is_file_field( dest_field_name ) )
      dest.set_field_value( dest.get_field_num( dest_field_name ),
       src.get_field_value( src.get_field_num( src_field_name ) ) );
   else
      dest.set_field_value(
       dest.get_field_num( dest_field_name ), copy_class_file( src.get_field_value(
       src.get_field_num( src_field_name ) ), dest.get_class_id( ), dest_key, copy_only_if_missing ) );
}

void CIYAM_BASE_DECL_SPEC copy_class_files( const class_base& src, class_base& dest );

void CIYAM_BASE_DECL_SPEC copy_class_files_for_clone(
 const std::vector< std::pair< std::string, std::string > >& file_field_name_and_values, class_base& dest );

std::string CIYAM_BASE_DECL_SPEC get_app_dir( );
std::string CIYAM_BASE_DECL_SPEC get_app_file( const std::string& module_name );

bool CIYAM_BASE_DECL_SPEC has_web_file( const std::string& file_name );

void CIYAM_BASE_DECL_SPEC touch_web_file( const char* p_file_name = 0, bool only_if_exists = false );

inline void touch_web_file( const std::string& file_name, bool only_if_exists = false )
{
   touch_web_file( file_name.c_str( ), only_if_exists );
}

void CIYAM_BASE_DECL_SPEC remove_web_file( const char* p_file_name = 0 );

inline void remove_web_file( const std::string& file_name )
{
   remove_web_file( file_name.c_str( ) );
}

std::string CIYAM_BASE_DECL_SPEC get_attached_file_dir( );

std::string CIYAM_BASE_DECL_SPEC get_attached_file_path(
 const std::string& module_id, const std::string& class_id );

std::string CIYAM_BASE_DECL_SPEC get_attached_file_path(
 const std::string& module_id, const std::string& class_id, const std::string& file_name );

std::string CIYAM_BASE_DECL_SPEC genesis_block_hash( const std::string& identity );

bool CIYAM_BASE_DECL_SPEC has_files_area_tag( const std::string& tag, file_type type = e_file_type_any );

bool CIYAM_BASE_DECL_SPEC has_files_area_file( const std::string& hash, bool include_archives = true );

void CIYAM_BASE_DECL_SPEC remove_files_area_tag( const std::string& tag );

void CIYAM_BASE_DECL_SPEC delete_files_area_files_for_pat( const std::string& pat );

std::string CIYAM_BASE_DECL_SPEC get_files_area_hash_for_tag( const std::string& tag );

bool CIYAM_BASE_DECL_SPEC has_files_area_archive( const std::string& archive );

void CIYAM_BASE_DECL_SPEC remove_files_area_archive(
 const std::string& archive, bool destroy = false, bool remove_directory = false );

std::string CIYAM_BASE_DECL_SPEC expand_lf_to_cr_lf( const std::string& input );

void CIYAM_BASE_DECL_SPEC delete_directory_tree( const std::string& path );

void CIYAM_BASE_DECL_SPEC create_directories_for_file_name( const std::string& file_name );
std::string CIYAM_BASE_DECL_SPEC get_directory_for_file_name( const std::string& file_name );

void CIYAM_BASE_DECL_SPEC add_user( const std::string* p_user_id = 0 );
std::string CIYAM_BASE_DECL_SPEC generate_password( const std::string& user_id, bool include_prefix = true );

void CIYAM_BASE_DECL_SPEC remove_gpg_key( const std::string& gpg_key_id, bool ignore_error = false );

void CIYAM_BASE_DECL_SPEC locate_gpg_key( const std::string& email, std::string& gpg_key_id, std::string& gpg_fingerprint );

void CIYAM_BASE_DECL_SPEC install_gpg_key( const std::string& key_file,
 const std::string& email, std::string& gpg_key_id, std::string& gpg_fingerprint, std::string* p_new_email = 0 );

std::string CIYAM_BASE_DECL_SPEC after_string( const std::string& s, const std::string& after );

std::string CIYAM_BASE_DECL_SPEC reversed_string( const std::string& s );

std::string CIYAM_BASE_DECL_SPEC replaced_suffix(
 const std::string& s, const std::string& old_suffix, const std::string& new_suffix );

std::string CIYAM_BASE_DECL_SPEC trim_whitespace( const std::string& s );
std::string CIYAM_BASE_DECL_SPEC trim_whitespace_and_quotes( const std::string& s );

std::string CIYAM_BASE_DECL_SPEC truncate_string( const std::string& s, int max_length, const char* p_overflow_suffix = 0 );

std::string CIYAM_BASE_DECL_SPEC join_string( const std::set< std::string >& c, char sep = ',' );
std::string CIYAM_BASE_DECL_SPEC join_string( const std::deque< std::string >& c, char sep = ',' );
std::string CIYAM_BASE_DECL_SPEC join_string( const std::vector< std::string >& c, char sep = ',' );

size_t CIYAM_BASE_DECL_SPEC split_count( const std::string& s, char sep = ',' );

void CIYAM_BASE_DECL_SPEC split_string( const std::string& s, std::set< std::string >& c, char sep = ',', bool unescape = true );
void CIYAM_BASE_DECL_SPEC split_string( const std::string& s, std::deque< std::string >& c, char sep = ',', bool unescape = true );
void CIYAM_BASE_DECL_SPEC split_string( const std::string& s, std::vector< std::string >& c, char sep = ',', bool unescape = true );

void CIYAM_BASE_DECL_SPEC split_string( const std::string& s, std::set< std::string >& c, const std::string& sep );
void CIYAM_BASE_DECL_SPEC split_string( const std::string& s, std::deque< std::string >& c, const std::string& sep );
void CIYAM_BASE_DECL_SPEC split_string( const std::string& s, std::vector< std::string >& c, const std::string& sep );

std::string CIYAM_BASE_DECL_SPEC search_replace( const std::string& s, const std::string& search, const std::string& replace );
std::string CIYAM_BASE_DECL_SPEC search_replace( const std::string& s,
 const std::string& search1, const std::string& replace1, const std::string& search2, const std::string& replace2 );

std::string CIYAM_BASE_DECL_SPEC escaped_string( const std::string& s,
 const char* p_chars = 0, char esc = '\\', const char* p_specials = 0 );

std::string CIYAM_BASE_DECL_SPEC quoted_literal( const std::string& s, char esc = '\\', bool add_quotes = true );

inline std::string unquoted_literal( const std::string& s, char esc = '\\' ) { return quoted_literal( s, esc, false ); }

std::string CIYAM_BASE_DECL_SPEC replace_leading_cols_with_ws( const std::string& s, const std::string& sep, size_t num_spaces );

std::string CIYAM_BASE_DECL_SPEC insert_or_remove_list_item(
 const std::string& item, const std::string& list, bool items_are_keys = false );

inline std::string insert_or_remove_list_key( const std::string& key, const std::string& list )
{
   return insert_or_remove_list_item( key, list, true );
}

std::string CIYAM_BASE_DECL_SPEC decode_hex( const std::string& s );
std::string CIYAM_BASE_DECL_SPEC encode_hex( const std::string& s );

std::string CIYAM_BASE_DECL_SPEC decode_if_base64( const std::string& s );
std::string CIYAM_BASE_DECL_SPEC encode_to_base64( const std::string& x );

std::string CIYAM_BASE_DECL_SPEC check_with_regex( const std::string& r, const std::string& s, bool* p_rc = 0 );

std::string CIYAM_BASE_DECL_SPEC hash_sha1( const std::string& s, int num_chars = 0 );

inline std::string CIYAM_BASE_DECL_SPEC hash_sha1(
 const std::string& s1, const std::string& s2, int num_chars = 0 )
{
   return hash_sha1( s1 + s2, num_chars );
}

std::string CIYAM_BASE_DECL_SPEC hash_sha256( const std::string& s, int num_chars = 0 );

inline std::string CIYAM_BASE_DECL_SPEC hash_sha256(
 const std::string& s1, const std::string& s2, int num_chars = 0 )
{
   return hash_sha256( s1 + s2, num_chars );
}

std::string CIYAM_BASE_DECL_SPEC decrypt( const std::string& s );
std::string CIYAM_BASE_DECL_SPEC encrypt( const std::string& s );

std::string CIYAM_BASE_DECL_SPEC decrypt( const std::string& pw, const std::string& s );
std::string CIYAM_BASE_DECL_SPEC encrypt( const std::string& pw, const std::string& s );

std::string CIYAM_BASE_DECL_SPEC shared_decrypt( const std::string& pk, const std::string& s );
std::string CIYAM_BASE_DECL_SPEC shared_encrypt( const std::string& pk, const std::string& s );

inline std::string hashed_decrypt( const std::string& s, bool use_sha256 = false )
{
   if( !use_sha256 )
      return hash_sha1( decrypt( s ) );
   else
      return hash_sha256( decrypt( s ) );
}

std::string CIYAM_BASE_DECL_SPEC shared_secret(
 const std::string& identity_for_peer, const std::string& encrypted_identity );

bool CIYAM_BASE_DECL_SPEC is_own_identity( const std::string& identity );

std::string CIYAM_BASE_DECL_SPEC private_identity( const std::string& s );

std::string CIYAM_BASE_DECL_SPEC masked_identity_key( const std::string& s );
std::string CIYAM_BASE_DECL_SPEC unmasked_identity_key( const std::string& s );

std::string CIYAM_BASE_DECL_SPEC totp_pin( const std::string& secret );
std::string CIYAM_BASE_DECL_SPEC totp_secret( const std::string& unique );

std::string CIYAM_BASE_DECL_SPEC file_extension( const std::string& str );

std::string CIYAM_BASE_DECL_SPEC valid_utf8_filename( const std::string& str );
std::string CIYAM_BASE_DECL_SPEC valid_non_utf8_filename( const std::string& str );

std::string CIYAM_BASE_DECL_SPEC unix_to_locktime( const numeric& unix_time );
std::string CIYAM_BASE_DECL_SPEC unix_to_datetime( const numeric& unix_time );

numeric CIYAM_BASE_DECL_SPEC datetime_to_unix( const date_time& dtm );

std::string CIYAM_BASE_DECL_SPEC seconds_from_now(
 const std::string& unix_time_str, bool append_secs_char = true );

std::string CIYAM_BASE_DECL_SPEC formatted_int( int n, const std::string& mask );
std::string CIYAM_BASE_DECL_SPEC formatted_numeric( const numeric& n, const std::string& mask );

std::string CIYAM_BASE_DECL_SPEC numeric_name( const std::string& s, bool show_plus_if_no_sign = false );

std::string CIYAM_BASE_DECL_SPEC value_label( const std::string& s );
std::string CIYAM_BASE_DECL_SPEC value_leftpart( const std::string& s );
std::string CIYAM_BASE_DECL_SPEC value_rightpart( const std::string& s );

std::string CIYAM_BASE_DECL_SPEC increment_numbers( const std::string& s );

std::string CIYAM_BASE_DECL_SPEC auto_int_increment( const std::string& current );

struct daylight_info
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
   std::string name;

   float utc_offset;

   std::string tz_info;
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

   std::vector< daylight_info > daylight_changes;
};

void CIYAM_BASE_DECL_SPEC generate_timezones_sio( const std::vector< timezone_info >& timezones );

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
   std::string tz_info;
   std::string description;
   std::string daylight_abbr;
   daylight_savings_info daylight_savings;
};

typedef std::map< std::string, timezone_data > timezone_container;

typedef timezone_container::iterator timezone_iterator;
typedef timezone_container::const_iterator timezone_const_iterator;
typedef timezone_container::value_type timezone_value_type;

void CIYAM_BASE_DECL_SPEC setup_time_zones( );

std::string CIYAM_BASE_DECL_SPEC list_time_zones( );

std::string CIYAM_BASE_DECL_SPEC get_tz_desc( const std::string& tz_name );
void CIYAM_BASE_DECL_SPEC get_tz_info( const date_time& dt, std::string& tz_name, float& offset );

date_time CIYAM_BASE_DECL_SPEC utc_to_local( const date_time& dt );
date_time CIYAM_BASE_DECL_SPEC utc_to_local( const date_time& dt, std::string& tz_name );
date_time CIYAM_BASE_DECL_SPEC utc_to_local( const date_time& dt, const std::string& tz_name );

date_time CIYAM_BASE_DECL_SPEC local_to_utc( const date_time& dt, const std::string& tz_name );

bool CIYAM_BASE_DECL_SPEC schedulable_month_and_day( int month, int day );

void CIYAM_BASE_DECL_SPEC add_class_map( const std::string& class_id,
 const std::string& map_id, const std::string& file_name, bool in_reverse = false );

bool CIYAM_BASE_DECL_SPEC has_class_map( const std::string& class_id, const std::string& map_id );

void CIYAM_BASE_DECL_SPEC remove_class_map( const std::string& class_id, const std::string& map_id );

std::string CIYAM_BASE_DECL_SPEC get_class_map_value(
 const std::string& class_id, const std::string& map_id, const std::string& key );

bool CIYAM_BASE_DECL_SPEC is_ntfy_email( const std::string& recipient );

std::string CIYAM_BASE_DECL_SPEC ntfy_topic( const std::string& user_key );

void CIYAM_BASE_DECL_SPEC send_ntfy_message(
 const std::string& user_key, const std::string& message, bool throw_on_error = false );

struct user_account
{
   std::string sender;
   std::string username;
   std::string password;
};

void CIYAM_BASE_DECL_SPEC send_email_message( const std::string& recipient,
 const std::string& subject, const std::string& message,
 const std::string& html_source, const std::vector< std::string >* p_extra_headers = 0,
 const std::vector< std::string >* p_file_names = 0, const std::string* p_tz_name = 0,
 const std::vector< std::string >* p_image_names = 0, const std::string* p_image_path_prefix = 0 );

void CIYAM_BASE_DECL_SPEC send_email_message( const std::vector< std::string >& recipients,
 const std::string& subject, const std::string& message, const std::string& html_source,
 const std::vector< std::string >* p_extra_headers = 0, const std::vector< std::string >* p_file_names = 0,
 const std::string* p_tz_name = 0, const std::vector< std::string >* p_image_names = 0,
 const std::string* p_image_path_prefix = 0 );

void CIYAM_BASE_DECL_SPEC send_email_message( const user_account& account,
 const std::vector< std::string >& recipients, const std::string& subject, const std::string& message,
 const std::string& html_source, const std::vector< std::string >* p_extra_headers = 0,
 const std::vector< std::string >* p_file_names = 0, const std::string* p_tz_name = 0,
 const std::vector< std::string >* p_image_names = 0, const std::string* p_image_path_prefix = 0 );

std::string CIYAM_BASE_DECL_SPEC generate_hashcash( const std::string& recipient );
bool CIYAM_BASE_DECL_SPEC has_valid_hashcash( const std::string& value );

std::string CIYAM_BASE_DECL_SPEC check_email_headers(
 const std::vector< std::string >& headers, bool create_script_output = false );

void CIYAM_BASE_DECL_SPEC fetch_email_messages(
 const std::string& file_name_prefix, bool skip_scripts = false );

void CIYAM_BASE_DECL_SPEC fetch_email_messages(
 std::vector< std::pair< bool, std::string > >& messages, bool skip_scripts = false );

void CIYAM_BASE_DECL_SPEC fetch_email_messages( const user_account& account,
 std::vector< std::pair< bool, std::string > >& messages, bool skip_scripts = false );

std::string CIYAM_BASE_DECL_SPEC decode_email_header( const std::string& header );

void CIYAM_BASE_DECL_SPEC decode_mime_message( const std::string& mime, std::string& message,
 std::string& html_message, std::vector< std::pair< std::string, std::string > >& attachments );

void CIYAM_BASE_DECL_SPEC parse_email_address(
 const std::string& address, std::string& name, std::string& email );

void CIYAM_BASE_DECL_SPEC save_attachment(
 const std::string& encoding, const std::string& data, const std::string& output_file );

std::string CIYAM_BASE_DECL_SPEC remove_html_scripts( const std::string& html );

std::string CIYAM_BASE_DECL_SPEC convert_html_to_text( const std::string& html );

std::string CIYAM_BASE_DECL_SPEC create_html_embedded_image( const std::string& source_file );

std::string CIYAM_BASE_DECL_SPEC crypto_digest( const std::string& data,
 bool use_sha512 = false, bool decode_hex_data = false, size_t extra_rounds = 0, const std::string* p_update = 0 );

std::string CIYAM_BASE_DECL_SPEC crypto_checksum( const std::string& hashes );

std::string CIYAM_BASE_DECL_SPEC local_backup_checksum( const std::string& extra );
std::string CIYAM_BASE_DECL_SPEC local_shared_checksum( const std::string& extra );
std::string CIYAM_BASE_DECL_SPEC local_combined_checksum( const std::string& extra );
std::string CIYAM_BASE_DECL_SPEC local_peer_hub_checksum( const std::string& extra );

bool CIYAM_BASE_DECL_SPEC any_session_backup_blockchains( );

std::string CIYAM_BASE_DECL_SPEC local_backup_blockchain_status( );

uint64_t CIYAM_BASE_DECL_SPEC crypto_amount( const std::string& amount );

std::string CIYAM_BASE_DECL_SPEC crypto_sign(
 const std::string& secret, const std::string& message, bool decode_hex_message = false );

std::string CIYAM_BASE_DECL_SPEC crypto_public(
 const std::string& privkey, bool is_wif = false, bool use_base64 = true, bool compressed = true );

std::string CIYAM_BASE_DECL_SPEC crypto_secret( const std::string& privkey, bool is_wif = false );

void CIYAM_BASE_DECL_SPEC crypto_verify(
 const std::string& pubkey, const std::string& address, bool* p_rc = 0 );

void CIYAM_BASE_DECL_SPEC crypto_verify( const std::string& pubkey,
 const std::string& message, const std::string& signature, bool decode_hex_message = false );

std::string CIYAM_BASE_DECL_SPEC crypto_pubkey_for_sid( const std::string& suffix, std::string* p_priv_key = 0 );

std::string CIYAM_BASE_DECL_SPEC crypto_secret_for_sid( const std::string& suffix, const std::string& other_pubkey );

std::string CIYAM_BASE_DECL_SPEC crypto_lamport( const std::string& filename,
 const std::string& mnenomics_or_hex_seed, bool is_sign = false, bool is_verify = false, const char* p_extra = 0 );

std::string CIYAM_BASE_DECL_SPEC crypto_address_hash( const std::string& address );

std::string CIYAM_BASE_DECL_SPEC crypto_p2sh_address( const std::string& ext_key, const std::string& hex_script );

std::string CIYAM_BASE_DECL_SPEC create_address_key_pair(
 const std::string& ext_key,
 std::string& pub_key, std::string& priv_key, bool use_base64 = false, bool compressed = true );

std::string CIYAM_BASE_DECL_SPEC create_address_key_pair(
 const std::string& ext_key, std::string& pub_key, std::string& priv_key,
 const std::string& priv_info, bool is_secret = true, bool use_base64 = false, bool compressed = true );

void CIYAM_BASE_DECL_SPEC get_mnemonics_or_hex_seed( std::string& s, const std::string& mnemonics_or_hex_seed );

inline std::string get_mnemonics_or_hex_seed( const std::string& mnemonics_or_hex_seed )
{
   std::string s;
   get_mnemonics_or_hex_seed( s, mnemonics_or_hex_seed );

   return s;
}

void CIYAM_BASE_DECL_SPEC use_peerchain( const std::string& identity, bool no_delay = false );
void CIYAM_BASE_DECL_SPEC disuse_peerchain( const std::string& identity, bool no_delay = false );
void CIYAM_BASE_DECL_SPEC connect_peerchain( const std::string& connect_info, bool no_delay = false );
void CIYAM_BASE_DECL_SPEC disconnect_peerchain( const std::string& identity, bool no_delay = false );

bool CIYAM_BASE_DECL_SPEC is_synchronising_peerchain( const std::string& identity );

bool CIYAM_BASE_DECL_SPEC active_external_service( const std::string& ext_key );

std::string CIYAM_BASE_DECL_SPEC get_external_extra( const std::string& ext_key, const std::string& extra );

void CIYAM_BASE_DECL_SPEC get_external_balance( const std::string& ext_key, numeric& balance );

bool CIYAM_BASE_DECL_SPEC can_create_address( const std::string& ext_key );

std::string CIYAM_BASE_DECL_SPEC create_new_address(
 const std::string& ext_key, const std::string& label, bool ignore_errors = false );

std::string CIYAM_BASE_DECL_SPEC send_funds_to_address(
 const std::string& ext_key, const std::string& address, const numeric& amount );

void CIYAM_BASE_DECL_SPEC import_address(
 const std::string& ext_key, const std::string& address, const std::string& label );

void CIYAM_BASE_DECL_SPEC load_address_information( const std::string& ext_key, const std::string& file_name );

struct address_info
{
   address_info( )
    :
    amount( 0 ),
    amount_d( 0.0 )
   {
   }

   address_info( const std::string& addr, const std::string& label, double amount_d )
    :
    addr( addr ),
    label( label ),
    amount_d( amount_d )
   {
      amount = 0;
   }

   address_info( const std::string& addr, const std::string& label, uint64_t amount )
    :
    addr( addr ),
    label( label ),
    amount( amount )
   {
      amount_d = 0.0;
   }

   std::string addr;
   std::string label;

   double amount_d;
   uint64_t amount;
};

void CIYAM_BASE_DECL_SPEC parse_address_information(
 const std::string& file_name, std::vector< address_info >& addresses );

struct utxo_info
{
   utxo_info( )
    :
    vout( 0 ),
    amount( 0 ),
    spendable( true ),
    confirmations( 0 )
   {
   }

   void clear( )
   {
      tx_id.erase( );
      tx_id_rev.erase( );

      vout = 0;

      script.erase( );
      account.erase( );

      address.erase( );

      amount = 0;
      confirmations = 0;
   }

   std::string tx_id;
   std::string tx_id_rev;

   unsigned int vout;

   std::string script;
   std::string account;

   std::string secret;
   std::string address;

   uint64_t amount;

   bool spendable;
   unsigned int confirmations;
};

void CIYAM_BASE_DECL_SPEC load_utxo_information(
 const std::string& ext_key, const std::string& source_addresses, const std::string& file_name );

void CIYAM_BASE_DECL_SPEC parse_utxo_information(
 const std::string& file_name, std::vector< utxo_info >& utxos );

uint64_t CIYAM_BASE_DECL_SPEC determine_utxo_balance( const std::string& file_name );

std::string CIYAM_BASE_DECL_SPEC convert_hash160_to_address( const std::string& ext_key, const std::string& hash160 );

std::string CIYAM_BASE_DECL_SPEC construct_raw_transaction(
 const std::string& ext_key, bool change_type_is_automatic,
 const std::string& source_addresses, const std::string& destination_addresses,
 std::string& changes_address, uint64_t amount, quote_style qs, uint64_t& fee,
 std::string& sign_tx_template, const std::string& file_name );

std::string CIYAM_BASE_DECL_SPEC construct_p2sh_redeem_transaction(
 const std::string& txid, unsigned int index, const std::string& redeem_script,
 const std::string& extras, const std::string& to_address, uint64_t amount, const std::string& key,
 bool is_wif_format = false, uint32_t lock_time = 0 );

enum p2sh_redeem_extra_info_count
{
   e_p2sh_redeem_extra_info_count_acct_secret = 4
};

enum p2sh_redeem_extra_info_offset
{
   e_p2sh_redeem_extra_info_offset_acct_secret = 2
};

std::string CIYAM_BASE_DECL_SPEC retreive_p2sh_redeem_extra_info(
 const std::string& ext_key, const std::string& check_address,
 p2sh_redeem_extra_info_count count = e_p2sh_redeem_extra_info_count_acct_secret,
 p2sh_redeem_extra_info_offset offset = e_p2sh_redeem_extra_info_offset_acct_secret );

std::string CIYAM_BASE_DECL_SPEC create_or_sign_raw_transaction(
 const std::string& ext_key, const std::string& raw_tx_cmd, bool throw_on_error = true,
 bool* p_is_complete = 0, std::vector< utxo_info >* p_utxos = 0, std::vector< address_info >* p_outputs = 0 );

bool CIYAM_BASE_DECL_SPEC raw_transaction_was_signed( const std::string& tx_info, std::string& raw_tx );

std::string CIYAM_BASE_DECL_SPEC send_raw_transaction( const std::string& ext_key, const std::string& tx );

void CIYAM_BASE_DECL_SPEC execute_command( numeric& n, const std::string& cmd_and_args, std::string& retval );
void CIYAM_BASE_DECL_SPEC execute_command( date_time& n, const std::string& cmd_and_args, std::string& retval );

void CIYAM_BASE_DECL_SPEC meta_relationship_child_name( std::string& name,
 const std::string& child_name, const std::string& parent_name, const std::string& separator );

const int c_sql_std_char_size = 100;
const int c_sql_large_char_size = 256;
const int c_sql_small_char_size = 30;

enum sql_char_type
{
   e_sql_char_type_std,
   e_sql_char_type_large,
   e_sql_char_type_small,
   e_sql_char_type_security,
   e_sql_char_type_very_large,
   e_sql_char_type_foreign_key
};

std::string CIYAM_BASE_DECL_SPEC meta_sql_type(
 const std::string& field_type, bool is_mandatory, sql_char_type char_type );

std::string CIYAM_BASE_DECL_SPEC meta_field_uom( int uom );

int CIYAM_BASE_DECL_SPEC meta_field_type_primitive( const std::string& type );

std::string CIYAM_BASE_DECL_SPEC meta_field_type_name( int primitive,
 bool mandatory, const std::string& parent_class_name, const std::string& model_name, bool* p_is_customised = 0 );

std::string CIYAM_BASE_DECL_SPEC meta_field_domain_type( const std::string& enum_id,
 int primitive, int max_size, const std::string& min_value, const std::string& max_value,
 int numeric_digits, int numeric_decimals, int string_domain, int date_precision, int time_precision,
 bool show_plus_sign, int zero_padding, int fraction_limit, std::string& mask, std::string* p_tmask = 0 );

std::string CIYAM_BASE_DECL_SPEC meta_field_extras( int uom,
 const std::string& uom_name, int extra, bool encrypted, bool transient,
 int max_size, const std::string& enum_id, const std::string& enum_filter_id, int primitive,
 const std::string& min_value, const std::string& max_value, int numeric_digits, int numeric_decimals,
 int string_domain, int date_precision, int time_precision, bool show_plus_sign, int zero_padding,
 int int_type, int numeric_type, bool is_child_or_grandchild = false );

std::string CIYAM_BASE_DECL_SPEC meta_procedure_arg_type( int primitive );

class mutex;

mutex& CIYAM_BASE_DECL_SPEC get_mutex_for_class_base( );

#endif

