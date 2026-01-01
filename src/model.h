// Copyright (c) 2005-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef MODEL_H
#  define MODEL_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <map>
#     include <set>
#     include <vector>
#     include <string>
#     include <iosfwd>
#     include <utility>
#  endif

enum get_field_type
{
   e_get_field_type_any,
   e_get_field_type_relationships
};

enum cascade_operation
{
   e_cascade_operation_unlink,
   e_cascade_operation_destroy,
   e_cascade_operation_restrict
};

enum get_specifications_type
{
   e_get_specifications_type_all,
   e_get_specifications_type_non_interface,
   e_get_specifications_type_interface_and_facets_only
};

struct class_data
{
   class_data( ) { }

   class_data( const std::string& id,
    const std::string& name, const std::string& plural,
    bool is_external_alias, const std::string& fixed_key_val,
    const std::string& extra, const std::string& modifiers, const std::string& base_class_name )
    :
    id( id ),
    name( name ),
    plural( plural ),
    is_external_alias( is_external_alias ),
    fixed_key_val( fixed_key_val ),
    extra( extra ),
    modifiers( modifiers ),
    base_class_name( base_class_name )
   {
   }

   std::string id;
   std::string name;
   std::string plural;
   bool is_external_alias;
   std::string fixed_key_val;
   std::string extra;
   std::string modifiers;
   std::string base_class_name;
};

struct field_data
{
   field_data( ) { }

   field_data( const std::string& id,
    const std::string& name, const std::string& type, const std::string& sys_type,
    bool is_mandatory, bool is_transient, bool is_foreign_key, bool for_text_search,
    bool is_external_field, const std::string& enum_name, const std::string& enum_filter,
    const std::string& extra, const std::string& modifiers, const std::string& default_value,
    const std::string& class_name, const std::string& decorated_name )
    :
    id( id ),
    name( name ),
    type( type ),
    sys_type( sys_type ),
    is_transient( is_transient ),
    is_mandatory( is_mandatory ),
    is_foreign_key( is_foreign_key ),
    for_text_search( for_text_search ),
    is_external_field( is_external_field ),
    enum_name( enum_name ),
    enum_filter( enum_filter ),
    extra( extra ),
    modifiers( modifiers ),
    default_value( default_value ),
    class_name( class_name ),
    decorated_name( decorated_name )
   {
   }

   std::string id;
   std::string name;
   std::string type;
   std::string sys_type;

   bool is_mandatory;
   bool is_transient;
   bool is_foreign_key;
   bool for_text_search;
   bool is_external_field;

   std::string enum_name;
   std::string enum_filter;

   std::string extra;
   std::string modifiers;
   std::string default_value;

   std::string class_name;
   std::string decorated_name;
};

inline bool operator <( const field_data& lhs, const field_data& rhs )
{
   return lhs.name < rhs.name;
}

struct index_data
{
   index_data( ) : is_unique( false ) { }
   index_data( const std::string& id, const std::string& name, bool is_unique ) : id( id ), name( name ), is_unique( is_unique ) { }

   std::string id;
   std::string name;

   bool is_unique;
};

inline bool operator <( const index_data& lhs, const index_data& rhs )
{
   return lhs.name < rhs.name;
}

struct modifier_data
{
   modifier_data( ) { }
   modifier_data( const std::string& id, const std::string& name, const std::string& value ) : id( id ), name( name ), value( value ) { }

   std::string id;
   std::string name;
   std::string value;
};

struct procedure_data
{
   procedure_data( ) { }
   procedure_data( const std::string& id, const std::string& name ) : id( id ), name( name ) { }

   std::string id;
   std::string name;
};

struct procedure_arg_data
{
   procedure_arg_data( ) { }

   procedure_arg_data( const std::string& id, const std::string& name, const std::string& type, bool is_output )
    :
    id( id ),
    name( name ),
    type( type ),
    is_output( is_output )
   {
   }

   std::string id;
   std::string name;
   std::string type;
   bool is_output;
};

struct relationship_data
{
   relationship_data( ) { }

   relationship_data( const std::string& id,
    const std::string& name, const std::string& child_class_name, const std::string& parent_class_name )
    :
    id( id ),
    name( name ),
    is_transient( false ),
    child_class_name( child_class_name ),
    parent_class_name( parent_class_name )
   {
   }

   std::string id;
   std::string name;

   bool is_transient;

   std::string child_class_name;
   std::string parent_class_name;
};

struct child_relationship_data
{
   child_relationship_data( ) { }

   child_relationship_data( const std::string& id, const std::string& name,
    const std::string& decorated_name, const std::string& child_class_name, const std::string& cascade_op_name )
    :
    id( id ),
    name( name ),
    decorated_name( decorated_name ),
    child_class_name( child_class_name ),
    cascade_op_name( cascade_op_name )
   {
   }

   std::string id;
   std::string name;
   std::string decorated_name;
   std::string child_class_name;
   std::string cascade_op_name;
};

struct specification_data
{
   specification_data( ) { }
   specification_data( const std::string& name, const std::string& type ) : name( name ), type( type ) { }

   std::string id;
   std::string name;
   std::string type;

   std::string decorated_name;

   std::vector< std::pair< std::string, std::string > > strings;
   std::vector< std::pair< std::string, std::string > > data_pairs;
};

struct user_defined_enum_data
{
   user_defined_enum_data( ) { }

   user_defined_enum_data( const std::string& id, const std::string& name, const std::string& type, const std::string& key_values )
    :
    id( id ),
    name( name ),
    type( type ),
    key_values( key_values )
   {
   }

   std::string id;
   std::string name;
   std::string type;
   std::string key_values;
};

struct user_defined_type_data
{
   user_defined_type_data( ) { }

   user_defined_type_data( const std::string& id,
    const std::string& name, const std::string& type, const std::string& sys_type,
    const std::string& extra, const std::string& domain_name, const std::string& domain_type )
    :
    id( id ),
    name( name ),
    type( type ),
    sys_type( sys_type ),
    extra( extra ),
    domain_name( domain_name ),
    domain_type( domain_type )
   {
   }

   std::string id;
   std::string name;
   std::string type;
   std::string sys_type;

   std::string extra;

   std::string domain_name;
   std::string domain_type;
};

class model
{
   public:
   model( );
   ~model( );

   const std::string& get_id( ) const;
   void set_id( const std::string& new_id );

   const std::string& get_name( ) const;
   void set_name( const std::string& new_name );

   const std::string& get_perm( ) const;
   void set_perm( const std::string& new_perm );

   const std::string& get_version( ) const;
   void set_version( const std::string& new_version );

   int get_year_created( ) const;
   void set_year_created( int new_year_created );

   std::string get_title( ) const;

   int get_major_version( ) const;
   int get_minor_version( ) const;

   void get_class_names( std::vector< std::string >& class_names );

   void get_external_module_names( std::vector< std::string >& module_names );

   void get_class_data( std::vector< class_data >& all_class_data, bool include_base_info = false );
   void get_class_data( const std::string& class_name, class_data& cls_data, bool include_base_info = false );

   void get_field_data( const std::string& class_name,
    std::vector< field_data >& all_field_data, get_field_type field_type = e_get_field_type_any, bool include_base_fields = false );

   void get_index_data( const std::string& class_name,
    std::vector< index_data >& all_index_data, bool include_base_fields = false );

   void get_modifier_data( const std::string& class_name, std::vector< modifier_data >& all_modifier_data );

   void get_procedure_data( const std::string& class_name,
    std::vector< procedure_data >& all_procedure_data, bool include_base_procedures = false );

   void get_procedure_arg_data( const std::string& class_name,
    const std::string& procedure_name, std::vector< procedure_arg_data >& all_procedure_arg_data );

   void get_relationship_data( const std::string& relationship_name, relationship_data& rel_data );
   void get_child_relationship_data( const std::string& parent_class_name, std::vector< child_relationship_data >& child_rel_info );

   void get_specification_data( const std::string& class_name,
    std::vector< specification_data >& all_spec_data, get_specifications_type gstype = e_get_specifications_type_all );

   void get_user_defined_enum_data(
    std::vector< user_defined_enum_data >& all_ude_data, bool include_external_enums = false ) const;

   void get_user_defined_type_data(
    std::vector< user_defined_type_data >& all_udt_data, bool include_external_types = false ) const;

   void get_all_base_related_classes( const std::string& class_name, std::set< std::string >& all_related_classes ) const;

   void check( ) const;

   void save( const std::string& filename );
   void load( const std::string& filename );

   void clear( );

   void class_add( const std::string& id,
    const std::string& class_name, const std::string& plural_name,
    const std::string& fixed_key_val, const std::string& extra, const std::string& base_class_name );

   void class_list( std::ostream& outs ) const;
   void class_remove( const std::string& class_name );
   void class_rename( const std::string& old_class_name, const std::string& new_class_name );

   void field_add( const std::string& field_id, const std::string& class_name,
    const std::string& field_name, const std::string& field_type, bool is_mandatory,
    bool is_transient, bool for_text_search, const std::string& enum_name, const std::string& enum_filter,
    const std::string& extra, const std::string& default_value );

   void field_list( const std::string& class_name, std::ostream& outs ) const;
   void field_remove( const std::string& class_name, const std::string& field_name );
   void field_rename( const std::string& class_name, const std::string& old_field_name, const std::string& new_field_name );

   void index_add( const std::string& index_id, const std::string& class_name, const std::string& field_list, bool is_unique );
   void index_list( const std::string& class_name, std::ostream& outs ) const;
   void index_remove( const std::string& class_name, const std::string& field_list );

   void modifier_add( const std::string& modifier_id,
    const std::string& class_name, const std::string& modifier_name, const std::string& field_info );

   void modifier_list( const std::string& class_name, std::ostream& outs ) const;
   void modifier_remove( const std::string& class_name, const std::string& modifier_name );
   void modifier_rename( const std::string& class_name, const std::string& old_modifier_name, const std::string& new_modifier_name );

   void procedure_add( const std::string& procedure_id, const std::string& class_name, const std::string& procedure_name );
   void procedure_list( const std::string& class_name, std::ostream& outs ) const;
   void procedure_remove( const std::string& class_name, const std::string& procedure_name );
   void procedure_rename( const std::string& class_name, const std::string& old_procedure_name, const std::string& new_procedure_name );

   void procedure_arg_add( const std::string& arg_id, const std::string& class_name,
    const std::string& procedure_name, const std::string& arg_name, const std::string& arg_type, bool is_output );

   void procedure_arg_list( const std::string& class_name, const std::string& procedure_name, std::ostream& outs ) const;
   void procedure_arg_remove( const std::string& class_name, const std::string& procedure_name, const std::string& arg_name );
   void procedure_arg_rename( const std::string& class_name,
    const std::string& procedure_name, const std::string& old_arg_name, const std::string& new_arg_name );

   void relationship_add( const std::string& relationship_id,
    const std::string& field_id, const std::string& child_class_name, const std::string& child_field_name,
    const std::string& parent_class_name, bool is_mandatory, bool is_transient, cascade_operation cascade_op, const std::string& extra );

   void relationship_list( std::ostream& outs ) const;
   void relationship_list_parents( const std::string& child_class_name, std::ostream& outs ) const;
   void relationship_list_children( const std::string& parent_class_name, std::ostream& outs ) const;
   void relationship_remove( const std::string& relationship_name );
   void relationship_rename( const std::string& old_relationship_name, const std::string& new_relationship_name );

   void specification_add( const std::string& specification_id,
    const std::string& specification_name, const std::string& specification_type, const std::string& specification_args );

   void specification_list( std::ostream& outs ) const;
   void specification_remove( const std::string& specification_name );
   void specification_rename( const std::string& old_specification_name, const std::string& new_specification_name );

   void user_defined_enum_add( const std::string& ude_id,
    const std::string& ude_name, const std::string& type_name, const std::string& key_values );

   void user_defined_enum_list( std::ostream& outs ) const;
   void user_defined_enum_remove( const std::string& ude_name );
   void user_defined_enum_rename( const std::string& old_ude_name, const std::string& new_ude_name );

   void user_defined_type_add( const std::string& udt_id,
    const std::string& udt_name, const std::string& type_name, const std::string& type_domain_info );

   void user_defined_type_list( std::ostream& outs ) const;
   void user_defined_type_remove( const std::string& udt_name );
   void user_defined_type_rename( const std::string& old_udt_name, const std::string& new_udt_name );

   std::string determine_child_rel_suffix( const std::string& relationship_name, std::string* p_rel_without_child = 0 );

   private:
   struct impl;
   impl* p_impl;
   friend struct impl;

   model( const model& );
   model& operator =( const model& );
};

std::string get_model_file_ext( );

std::string get_cascade_op_name( cascade_operation cascade_op );
cascade_operation get_cascade_op_for_name( const std::string& op_name );

#endif

