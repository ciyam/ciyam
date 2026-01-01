// Copyright (c) 2005-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <set>
#  include <fstream>
#  include <iostream>
#  include <algorithm>
#  include <stdexcept>
#endif

#include "model.h"

#include "sio.h"
#include "ptypes.h"
#include "pointers.h"
#include "date_time.h"
#include "utilities.h"
#include "model_domains.h"
#include "model_specifications.h"

using namespace std;

namespace
{

const uint64_t c_first_user_modifier = UINT64_C( 0x100 );

const char* const c_input = "in";
const char* const c_output = "out";

const char* const c_true = "true";
const char* const c_false = "false";

const char* const c_optional = "optional";
const char* const c_mandatory = "mandatory";

const char* const c_model_file_extension = ".sio";

const char* const c_field_extra_order = "order";

const char* const c_class_extra_ordered = "ordered";

const char* const c_cascade_operation_unlink = "unlink";
const char* const c_cascade_operation_destroy = "destroy";
const char* const c_cascade_operation_restrict = "restrict";

const char* const c_relationship_type = "@class";

const char* const c_section_model = "model";
const char* const c_attribute_model_id = "id";
const char* const c_attribute_model_name = "name";
const char* const c_attribute_model_perm = "perm";
const char* const c_attribute_model_version = "version";
const char* const c_attribute_model_year_created = "year_created";

const char* const c_section_classes = "classes";
const char* const c_section_relationships = "relationships";
const char* const c_section_specifications = "specifications";
const char* const c_section_user_defined_enums = "user_defined_enums";
const char* const c_section_user_defined_types = "user_defined_types";

const char* const c_section_fields = "fields";
const char* const c_section_indexes = "indexes";
const char* const c_section_modifiers = "modifiers";
const char* const c_section_procedures = "procedures";

const char* const c_section_procedure_args = "args";

const char* const c_section_class = "class";
const char* const c_attribute_class_id = "id";
const char* const c_attribute_class_name = "name";
const char* const c_attribute_class_plural = "plural";
const char* const c_attribute_class_version = "version";
const char* const c_attribute_class_parent = "parent";
const char* const c_attribute_class_fixed_key_val = "fixed_key_val";
const char* const c_attribute_class_extra = "extra";
const char* const c_attribute_class_base_class_id = "base_class_id";

const char* const c_section_field = "field";
const char* const c_attribute_field_id = "id";
const char* const c_attribute_field_name = "name";
const char* const c_attribute_field_type = "type";
const char* const c_attribute_field_mandatory = "mandatory";
const char* const c_attribute_field_transient = "transient";
const char* const c_attribute_field_text_search = "text_search";
const char* const c_attribute_field_enum_id = "enum_id";
const char* const c_attribute_field_enum_filter_id = "enum_filter_id";
const char* const c_attribute_field_extra = "extra";
const char* const c_attribute_field_default_value = "default_value";
const char* const c_attribute_field_class_id = "class_id";
const char* const c_attribute_field_relationship_id = "relationship_id";

const char* const c_section_index = "index";
const char* const c_attribute_index_id = "id";
const char* const c_attribute_index_name = "name";
const char* const c_attribute_index_unique = "unique";

const char* const c_section_modifier = "modifier";
const char* const c_attribute_modifier_id = "id";
const char* const c_attribute_modifier_name = "name";

const char* const c_section_domain = "domain";
const char* const c_attribute_domain_name = "name";

const char* const c_section_procedure = "procedure";
const char* const c_attribute_procedure_id = "id";
const char* const c_attribute_procedure_name = "name";

const char* const c_section_procedure_arg = "arg";
const char* const c_attribute_procedure_arg_id = "id";
const char* const c_attribute_procedure_arg_name = "name";
const char* const c_attribute_procedure_arg_type = "type";
const char* const c_attribute_procedure_arg_output = "output";

const char* const c_section_relationship = "relationship";
const char* const c_attribute_relationship_id = "id";
const char* const c_attribute_relationship_name = "name";
const char* const c_attribute_relationship_parent_id = "parent_id";
const char* const c_attribute_relationship_child_id = "child_id";
const char* const c_attribute_relationship_cascade_op = "cascade_op";

const char* const c_section_specification = "specification";
const char* const c_attribute_specification_id = "id";
const char* const c_attribute_specification_name = "name";
const char* const c_attribute_specification_type = "type";

const char* const c_section_specification_data = "data";

const char* const c_section_user_defined_enum = "user_defined_enum";
const char* const c_attribute_user_defined_enum_id = "id";
const char* const c_attribute_user_defined_enum_name = "name";
const char* const c_attribute_user_defined_enum_type = "type";
const char* const c_attribute_user_defined_enum_key_values = "key_values";

const char* const c_section_user_defined_type = "user_defined_type";
const char* const c_attribute_user_defined_type_id = "id";
const char* const c_attribute_user_defined_type_name = "name";
const char* const c_attribute_user_defined_type_type = "type";
const char* const c_attribute_user_defined_type_sys_type = "sys_type";

const char* const c_section_element_indexes = "indexes";

const char* const c_section_element_index = "index";
const char* const c_attribute_element_index_id = "id";
const char* const c_attribute_element_index_num = "num";

const char* const c_section_element_modifiers = "modifiers";

const char* const c_section_element_modifier = "modifier";
const char* const c_attribute_element_modifier_id = "id";
const char* const c_attribute_element_modifier_affect = "affect";

const char* const c_section_element_specifications = "specifications";

const char* const c_section_element_specification = "specification";
const char* const c_attribute_element_specification_id = "id";
const char* const c_attribute_element_specification_item = "item";

typedef map< string, size_t > index_detail_container;
typedef index_detail_container::value_type index_detail_value_type;
typedef index_detail_container::iterator index_detail_iterator;
typedef index_detail_container::const_iterator index_detail_const_iterator;

typedef map< string, string > modifier_detail_container;
typedef modifier_detail_container::value_type modifier_detail_value_type;
typedef modifier_detail_container::iterator modifier_detail_iterator;
typedef modifier_detail_container::const_iterator modifier_detail_const_iterator;

typedef multimap< string, string > specification_detail_container;
typedef specification_detail_container::value_type specification_detail_value_type;
typedef specification_detail_container::iterator specification_detail_iterator;
typedef specification_detail_container::const_iterator specification_detail_const_iterator;

typedef map< string, ref_count_ptr< model > > external_reference_container;
typedef external_reference_container::value_type external_reference_value_type;
typedef external_reference_container::iterator external_reference_iterator;
typedef external_reference_container::const_iterator external_reference_const_iterator;

struct field_info
{
   field_info( ) : is_mandatory( false ) { }

   field_info( const string& name, const string& type, bool is_mandatory,
    bool is_transient, bool for_text_search, const string& enum_id, const string& enum_filter_id,
    const string& extra, const string& default_value, const string& class_id, const string& relationship_id )
    :
    name( name ),
    type( type ),
    is_mandatory( is_mandatory ),
    is_transient( is_transient ),
    for_text_search( for_text_search ),
    enum_id( enum_id ),
    enum_filter_id( enum_filter_id ),
    extra( extra ),
    default_value( default_value ),
    class_id( class_id ),
    relationship_id( relationship_id )
   {
   }

   string name;
   string type;
   bool is_mandatory;
   bool is_transient;

   bool for_text_search;

   string enum_id;
   string enum_filter_id;

   string extra;
   string default_value;

   string class_id;
   string relationship_id;

   index_detail_container index_details;
   modifier_detail_container modifier_details;
   specification_detail_container specification_details;
};

typedef map< string, field_info > field_container;
typedef field_container::value_type field_value_type;
typedef field_container::iterator field_iterator;
typedef field_container::const_iterator field_const_iterator;

typedef map< string, string > field_index_container;
typedef field_index_container::value_type field_index_value_type;
typedef field_index_container::iterator field_index_iterator;
typedef field_index_container::const_iterator field_index_const_iterator;

struct index_info
{
   index_info( ) : is_unique( false ) { }
   index_info( const string& name, bool is_unique ) : name( name ), is_unique( is_unique ) { }

   string name;
   bool is_unique;
};

typedef map< string, index_info > index_container;
typedef index_container::value_type index_value_type;
typedef index_container::iterator index_iterator;
typedef index_container::const_iterator index_const_iterator;

typedef map< string, string > index_index_container;
typedef index_index_container::value_type index_index_value_type;
typedef index_index_container::iterator index_index_iterator;
typedef index_index_container::const_iterator index_index_const_iterator;

struct modifier_info
{
   modifier_info( ) { }
   modifier_info( const string& name ) : name( name ) { }

   string name;
};

typedef map< string, modifier_info > modifier_container;
typedef modifier_container::value_type modifier_value_type;
typedef modifier_container::iterator modifier_iterator;
typedef modifier_container::const_iterator modifier_const_iterator;

typedef map< string, string > modifier_index_container;
typedef modifier_index_container::value_type modifier_index_value_type;
typedef modifier_index_container::iterator modifier_index_iterator;
typedef modifier_index_container::const_iterator modifier_index_const_iterator;

struct arg_info
{
   arg_info( ) : is_output( false ) { }
   arg_info( const string& name, const string& type, bool is_output ) : name( name ), type( type ), is_output( is_output ) { }

   string name;
   string type;
   bool is_output;

   specification_detail_container specification_details;
};

typedef map< string, arg_info > arg_container;
typedef arg_container::value_type arg_value_type;
typedef arg_container::iterator arg_iterator;
typedef arg_container::const_iterator arg_const_iterator;

typedef map< string, string > arg_index_container;
typedef arg_index_container::value_type arg_index_value_type;
typedef arg_index_container::iterator arg_index_iterator;
typedef arg_index_container::const_iterator arg_index_const_iterator;

struct procedure_info
{
   procedure_info( ) { }
   procedure_info( const string& name ) : name( name ) { }

   string name;

   arg_container args;
   arg_index_container args_index;

   specification_detail_container specification_details;
};

typedef map< string, procedure_info > procedure_container;
typedef procedure_container::value_type procedure_value_type;
typedef procedure_container::iterator procedure_iterator;
typedef procedure_container::const_iterator procedure_const_iterator;

typedef map< string, string > procedure_index_container;
typedef procedure_index_container::value_type procedure_index_value_type;
typedef procedure_index_container::iterator procedure_index_iterator;
typedef procedure_index_container::const_iterator procedure_index_const_iterator;

struct class_info
{
   class_info( ) { }

   class_info( const string& name, const string& plural,
    const string& fixed_key_val, const string& extra, const string& base_class_id )
    :
    name( name ),
    plural( plural ),
    fixed_key_val( fixed_key_val ),
    extra( extra ),
    base_class_id( base_class_id )
   {
   }

   bool is_external_alias( ) const { return base_class_id.find( ':' ) != string::npos; }

   string get_external_model( ) const;

   string name;
   string plural;
   string version;

   string fixed_key_val;

   string extra;

   string base_class_id;

   field_container fields;
   field_index_container fields_index;

   index_container indexes;
   index_index_container indexes_index;

   modifier_container modifiers;
   modifier_index_container modifiers_index;
   modifier_detail_container modifier_details;

   procedure_container procedures;
   procedure_index_container procedures_index;

   specification_detail_container specification_details;
};

string class_info::get_external_model( ) const
{
   string external_model;

   string::size_type pos = base_class_id.find( ':' );
   if( pos != string::npos )
      external_model = base_class_id.substr( 0, pos );

   return external_model;
}

typedef map< string, class_info > class_container;
typedef class_container::value_type class_value_type;
typedef class_container::iterator class_iterator;
typedef class_container::const_iterator class_const_iterator;

typedef map< string, string > class_index_container;
typedef class_index_container::value_type class_index_value_type;
typedef class_index_container::iterator class_index_iterator;
typedef class_index_container::const_iterator class_index_const_iterator;

typedef map< string, string > class_parent_container;
typedef class_parent_container::value_type class_parent_value_type;
typedef class_parent_container::iterator class_parent_iterator;
typedef class_parent_container::const_iterator class_parent_const_iterator;

struct relationship_info
{
   relationship_info( ) : cascade_op( e_cascade_operation_restrict ) { }

   relationship_info( const string& name,
    const string& parent_id, const string& child_id, cascade_operation cascade_op )
    :
    name( name ),
    parent_id( parent_id ),
    child_id( child_id ),
    cascade_op( cascade_op )
   {
   }

   string name;
   string parent_id;
   string child_id;
   cascade_operation cascade_op;

   specification_detail_container specification_details;
};

typedef map< string, relationship_info > relationship_container;
typedef relationship_container::value_type relationship_value_type;
typedef relationship_container::iterator relationship_iterator;
typedef relationship_container::const_iterator relationship_const_iterator;

typedef map< string, string > relationship_index_container;
typedef relationship_index_container::value_type relationship_index_value_type;
typedef relationship_index_container::iterator relationship_index_iterator;
typedef relationship_index_container::const_iterator relationship_index_const_iterator;

struct specification_info
{
   specification_info( ) { }

   specification_info( const string& name, const string& type )
    :
    name( name ),
    type( type )
   {
   }

   string name;
   string type;

   ref_count_ptr< specification > rp_spec;
};

typedef map< string, specification_info > specification_container;
typedef specification_container::value_type specification_value_type;
typedef specification_container::iterator specification_iterator;
typedef specification_container::const_iterator specification_const_iterator;

typedef map< string, string > specification_index_container;
typedef specification_index_container::value_type specification_index_value_type;
typedef specification_index_container::iterator specification_index_iterator;
typedef specification_index_container::const_iterator specification_index_const_iterator;

struct user_defined_enum_info
{
   user_defined_enum_info( ) { }

   user_defined_enum_info( const string& name, const string& type, const string& key_values )
    :
    name( name ),
    type( type ),
    key_values( key_values )
   {
   }

   string name;
   string type;
   string key_values;
};

typedef map< string, user_defined_enum_info > user_defined_enum_container;
typedef user_defined_enum_container::value_type user_defined_enum_value_type;
typedef user_defined_enum_container::iterator user_defined_enum_iterator;
typedef user_defined_enum_container::const_iterator user_defined_enum_const_iterator;

typedef map< string, string > user_defined_enum_index_container;
typedef user_defined_enum_index_container::value_type user_defined_enum_index_value_type;
typedef user_defined_enum_index_container::iterator user_defined_enum_index_iterator;
typedef user_defined_enum_index_container::const_iterator user_defined_enum_index_const_iterator;

struct user_defined_type_info
{
   user_defined_type_info( ) { }

   user_defined_type_info( const string& name, const string& type, const string& sys_type, const string& domain_name )
    :
    name( name ),
    type( type ),
    sys_type( sys_type ),
    domain_name( domain_name )
   {
   }

   string name;
   string type;
   string sys_type;
   string domain_name;

   ref_count_ptr< domain > rp_domain;
};

typedef map< string, user_defined_type_info > user_defined_type_container;
typedef user_defined_type_container::value_type user_defined_type_value_type;
typedef user_defined_type_container::iterator user_defined_type_iterator;
typedef user_defined_type_container::const_iterator user_defined_type_const_iterator;

typedef map< string, string > user_defined_type_index_container;
typedef user_defined_type_index_container::value_type user_defined_type_index_value_type;
typedef user_defined_type_index_container::iterator user_defined_type_index_iterator;
typedef user_defined_type_index_container::const_iterator user_defined_type_index_const_iterator;

void read_specification_details( sio_reader& reader, specification_detail_container& specification_details )
{
   reader.start_section( c_section_element_specifications );
   while( reader.has_started_section( c_section_element_specification ) )
   {
      string id, item;

      id = reader.read_attribute( c_attribute_element_specification_id );
      item = reader.read_attribute( c_attribute_element_specification_item );

      specification_details.insert( specification_detail_value_type( id, item ) );

      reader.finish_section( c_section_element_specification );
   }
   reader.finish_section( c_section_element_specifications );
}

void write_specification_details( sio_writer& writer,
 specification_detail_const_iterator sdci, specification_detail_const_iterator end )
{
   writer.start_section( c_section_element_specifications );
   for( ; sdci != end; ++sdci )
   {
      writer.start_section( c_section_element_specification );

      writer.write_attribute( c_attribute_element_specification_id, sdci->first );
      writer.write_attribute( c_attribute_element_specification_item, sdci->second );

      writer.finish_section( c_section_element_specification );
   }
   writer.finish_section( c_section_element_specifications );
}

}

struct model::impl
{
   impl( ) : p_model( 0 ), p_parent_model( 0 ) { year_created = udate::local( ).get_year( ); }

   const string& get_id( ) const { return id; }
   void set_id( const string& new_id ) { id = new_id; }

   const string& get_name( ) const { return name; }
   void set_name( const string& new_name ) { name = new_name; }

   const string& get_perm( ) const { return perm; }
   void set_perm( const string& new_perm ) { perm = new_perm; }

   const string& get_version( ) const { return version; }
   void set_version( const string& new_version ) { version = new_version; }

   int get_year_created( ) const { return year_created; }
   void set_year_created( int new_year_created ) { year_created = new_year_created; }

   string get_title( ) const;

   int get_major_version( ) const
   {
      string::size_type pos = version.find( '.' );
      return atoi( version.substr( 0, pos ).c_str( ) );
   }

   int get_minor_version( ) const
   {
      string::size_type pos = version.find( '.' );
      return pos == string::npos ? 0 : atoi( version.substr( pos + 1 ).c_str( ) );
   }

   void get_class_names( vector< string >& class_names );

   void get_external_module_names( vector< string >& module_names );

   void get_class_data( vector< class_data >& all_class_data, bool include_base_info );
   void get_class_data( const string& class_name, class_data& cls_data, bool include_base_info );

   void get_field_data( const string& class_name,
    vector< field_data >& all_field_data, get_field_type field_type, bool include_base_fields );

   void get_index_data( const string& class_name,
    vector< index_data >& all_index_data, bool include_base_fields );

   void get_modifier_data( const string& class_name, vector< modifier_data >& all_modifier_data );

   void get_procedure_data( const string& class_name,
    vector< procedure_data >& all_procedure_data, bool include_base_procedures );

   void get_procedure_arg_data( const string& class_name,
    const string& procedure_name, vector< procedure_arg_data >& all_procedure_arg_data );

   void get_relationship_data( const string& relationship_name, relationship_data& rel_data );
   void get_child_relationship_data( const string& parent_class_name, vector< child_relationship_data >& child_rel_info );

   void get_specification_data( const string& class_name,
    vector< specification_data >& all_spec_data, get_specifications_type gstype, bool is_external = false );

   void get_user_defined_enum_data(
    vector< user_defined_enum_data >& all_ude_data, bool include_external_enums ) const;

   void get_user_defined_type_data(
    vector< user_defined_type_data >& all_udt_data, bool include_external_types ) const;

   void get_all_base_related_classes( const string& class_name,
    set< string >& all_related_classes, bool is_base_class = false ) const;

   void check( ) const;

   void save( const string& filename );
   void load( const string& filename );

   void clear( );

   void class_add( const string& class_id,
    const string& class_name, const string& plural_name,
    const string& fixed_key_val, const string& extra, const string& base_class_name );

   void class_list( ostream& outs ) const;
   void class_remove( const string& class_name );
   void class_rename( const string& old_class_name, const string& new_class_name );

   void field_add( const string& field_id, const string& class_name,
    const string& field_name, const string& field_type, bool is_mandatory,
    bool is_transient, bool for_text_search, const string& enum_name, const string& enum_filter,
    const string& extra, const string& default_value, const string& field_class_id = "", const string& relationship_id = "" );

   void field_add_impl( const string& field_id, const string& class_id,
    const string& field_name, const string& field_type, bool is_mandatory,
    bool is_transient, bool for_text_search, const string& enum_name, const string& enum_filter,
    const string& extra, const string& default_value, const string& field_class_id = "", const string& relationship_id = "" );

   void field_list( const string& class_name, ostream& outs ) const;
   void field_remove( const string& class_name, const string& field_name, bool internal = false );
   void field_rename( const string& class_name, const string& old_field_name, const string& new_field_name, bool internal = false );

   void index_add( const string& index_id, const string& class_name, const string& field_list, bool is_unique );
   void index_list( const string& class_name, ostream& outs ) const;
   void index_remove( const string& class_name, const string& field_list );

   void modifier_add( const string& modifier_id, const string& class_name, const string& modifier_name, const string& field_info );
   void modifier_list( const string& class_name, ostream& outs ) const;
   void modifier_remove( const string& class_name, const string& modifier_name );
   void modifier_rename( const string& class_name, const string& old_modifier_name, const string& new_modifier_name );

   void procedure_add( const string& procedure_id, const string& class_name, const string& procedure_name );
   void procedure_list( const string& class_name, ostream& outs ) const;
   void procedure_remove( const string& class_name, const string& procedure_name );
   void procedure_rename( const string& class_name, const string& old_procedure_name, const string& new_procedure_name );

   void procedure_arg_add( const string& arg_id, const string& class_name,
    const string& procedure_name, const string& arg_name, const string& arg_type, bool is_output );

   void procedure_arg_list( const string& class_name, const string& procedure_name, ostream& outs ) const;
   void procedure_arg_remove( const string& class_name, const string& procedure_name, const string& arg_name );
   void procedure_arg_rename( const string& class_name,
    const string& procedure_name, const string& old_arg_name, const string& new_arg_name );

   void relationship_add( const string& relationship_id,
    const string& field_id, const string& child_class_name, const string& child_field_name,
    const string& parent_class_name, bool is_mandatory, bool is_transient, cascade_operation cascade_op, const string& extra );

   void relationship_list( ostream& outs ) const;
   void relationship_list_parents( const string& child_class_name, ostream& outs ) const;
   void relationship_list_children( const string& parent_class_name, ostream& outs ) const;
   void relationship_remove( const string& relationship_name );
   void relationship_rename( const string& old_relationship_name, const string& new_relationship_name );

   void specification_add( const string& specification_id,
    const string& specification_name, const string& specification_type, const string& specification_args );

   void specification_list( ostream& outs ) const;
   void specification_remove( const string& specification_name );
   void specification_rename( const string& old_specification_name, const string& new_specification_name );

   void user_defined_enum_add( const string& ude_id,
    const string& ude_name, const string& type_name, const string& key_values );

   void user_defined_enum_list( ostream& outs ) const;
   void user_defined_enum_remove( const string& ude_name );
   void user_defined_enum_rename( const string& old_ude_name, const string& new_ude_name );

   void user_defined_type_add( const string& udt_id,
    const string& udt_name, const string& type_name, const string& type_domain_info );

   void user_defined_type_list( ostream& outs ) const;
   void user_defined_type_remove( const string& udt_name );
   void user_defined_type_rename( const string& old_udt_name, const string& new_udt_name );

   string determine_child_rel_suffix( const string& relationship_name, string* p_rel_without_child = 0 );

   void verify_base_class_does_not_have_field( class_const_iterator cci, const string& field_name, bool is_base = false ) const;
   void verify_derived_class_does_not_have_field( const string& class_id, const string& field_name ) const;

   model* p_model;
   model* p_parent_model;

   string id;
   string name;
   string perm;
   string version;

   year year_created;

   external_reference_container external_references;

   class_container classes;
   class_index_container classes_index;

   relationship_container relationships;
   relationship_index_container relationships_index;

   specification_container specifications;
   specification_index_container specifications_index;

   user_defined_enum_container user_defined_enums;
   user_defined_enum_index_container user_defined_enums_index;

   user_defined_type_container user_defined_types;
   user_defined_type_index_container user_defined_types_index;

   string get_base_class_name( const string& base_class_id, string* p_extra = 0 ) const;

   void setup_fields( vector< field_data >& all_field_data,
    get_field_type field_type, class_const_iterator cci, const map< string, string >& modifier_flags, bool include_base_fields );

   void setup_indexes( vector< index_data >& all_index_data, class_const_iterator cci, bool include_base_fields );

   void setup_procedures(
    vector< procedure_data >& all_procedure_data, class_const_iterator cci, bool include_base_procedures );

   void setup_modifier_flags( uint64_t& flag_value,
    map< string, string >& modifier_flags, class_const_iterator cci,
    bool include_base_modifiers, string* p_modifiers = 0, bool prefix_flag_with_id = false );

   void locate_base_class( const string& base_class_id, external_reference_const_iterator& erci, class_const_iterator& cci ) const;

   string get_undecorated_class_name( const string& decorated_class_name ) const;
   string get_undecorated_index_name( const string& class_name, const string& decorated_index_name, bool undecorate_classes ) const;

   class_iterator fetch_class_info( const string& class_name );
   class_const_iterator fetch_class_info( const string& class_name ) const;

   field_iterator fetch_field_info( class_iterator ci, const string& field_name, field_index_iterator* p_fii = 0 );
   field_const_iterator fetch_field_info( class_const_iterator cci, const string& field_name ) const;
   field_const_iterator fetch_field_info( class_const_iterator cci, relationship_const_iterator rci ) const;

   procedure_iterator fetch_procedure_info(
    const string& class_name, const string& procedure_name, procedure_index_iterator* p_pii = 0 );
   procedure_const_iterator fetch_procedure_info( const string& class_name, const string& procedure_name ) const;

   relationship_iterator fetch_relationship_info( const string& relationship_name, relationship_index_iterator* p_rii = 0 );

   relationship_const_iterator fetch_relationship_info(
    const string& relationship_id, class_const_iterator* p_child, class_const_iterator* p_parent, string* p_name = 0 ) const;
};

string model::impl::get_title( ) const
{
   string str( name );
   if( get_major_version( ) == 0 )
      str += "_Beta";
   else
   {
      str += "_" + to_string( get_major_version( ) );
      str += "." + to_string( get_minor_version( ) );
   }

   return str;
}

void model::impl::get_class_names( vector< string >& class_names )
{
   for( class_index_const_iterator cici = classes_index.begin( ), end = classes_index.end( ); cici != end; ++cici )
      class_names.push_back( cici->first );
}

void model::impl::get_external_module_names( vector< string >& module_names )
{
   for( external_reference_const_iterator
    erci = external_references.begin( ), end = external_references.end( ); erci != end; ++erci )
      module_names.push_back( erci->first );
}

void model::impl::get_class_data( vector< class_data >& all_class_data, bool include_base_info )
{
   for( class_index_const_iterator cici = classes_index.begin( ), end = classes_index.end( ); cici != end; ++cici )
   {
      string id( cici->second );
      class_const_iterator cci = classes.find( id );
      if( cci == classes.end( ) )
         throw runtime_error( "unexpected class_info entry not found for class '" + cici->first + "' (id = " + id + ")" );

      string extra( ( cci->second ).extra );
      string base_class_name( get_base_class_name( ( cci->second ).base_class_id, &extra ) );

      string modifiers;
      map< string, string > modifier_flags;
      uint64_t flag_value( c_first_user_modifier );

      setup_modifier_flags( flag_value, modifier_flags, cci, include_base_info, &modifiers );

      all_class_data.push_back( class_data( cci->first,
       ( cci->second ).name, ( cci->second ).plural, ( cci->second ).is_external_alias( ),
       ( cci->second ).fixed_key_val, extra, modifiers, base_class_name ) );
   }
}

void model::impl::get_class_data( const string& class_name, class_data& cls_data, bool include_base_info )
{
   class_const_iterator cci = fetch_class_info( class_name );

   cls_data.id = cci->first;
   cls_data.name = class_name;
   cls_data.plural = ( cci->second ).plural;
   cls_data.is_external_alias = ( cci->second ).is_external_alias( );
   cls_data.fixed_key_val = ( cci->second ).fixed_key_val;
   cls_data.extra = ( cci->second ).extra;

   string modifiers;
   map< string, string > modifier_flags;
   uint64_t flag_value( c_first_user_modifier );

   setup_modifier_flags( flag_value, modifier_flags, cci, include_base_info, &modifiers );

   cls_data.modifiers = modifiers;
   cls_data.base_class_name = get_base_class_name( ( cci->second ).base_class_id, &cls_data.extra );
}

void model::impl::get_field_data( const string& class_name,
 vector< field_data >& all_field_data, get_field_type field_type, bool include_base_fields )
{
   class_index_const_iterator cici = classes_index.find( class_name );
   if( cici == classes_index.end( ) )
      throw runtime_error( "unknown class '" + class_name + "'" );

   string id( cici->second );
   class_const_iterator cci = classes.find( id );
   if( cci == classes.end( ) )
      throw runtime_error( "unexpected class_info entry not found for class '" + class_name + "' (id = " + id + ")" );

   uint64_t flag_value( c_first_user_modifier );
   map< string, string > modifier_flags;
   setup_modifier_flags( flag_value, modifier_flags, cci, include_base_fields );

   setup_fields( all_field_data, field_type, cci, modifier_flags, include_base_fields );

   // NOTE: As field names may have been "decorated" they now need to be sorted.
   sort( all_field_data.begin( ), all_field_data.end( ) );
}

void model::impl::get_index_data( const string& class_name,
 vector< index_data >& all_index_data, bool include_base_fields )
{
   class_const_iterator cci = fetch_class_info( class_name );

   setup_indexes( all_index_data, cci, include_base_fields );
}

void model::impl::get_modifier_data( const string& class_name, vector< modifier_data >& all_modifier_data )
{
   class_const_iterator cci = fetch_class_info( class_name );

   map< string, string > modifier_flags;
   uint64_t flag_value( c_first_user_modifier );

   setup_modifier_flags( flag_value, modifier_flags, cci, true, 0, true );

   map< uint64_t, modifier_data > sorted_modifiers;

   for( map< string, string >::iterator i = modifier_flags.begin( ); i != modifier_flags.end( ); ++i )
   {
      string id_and_flag( i->second );
      string::size_type pos = id_and_flag.find( ' ' );
      if( pos == string::npos )
         throw runtime_error( "unexpected id_and_flag '" + id_and_flag + "'" );

      sorted_modifiers.insert( make_pair( from_string< uint64_t >( id_and_flag.substr( pos + 1 ) ),
       modifier_data( i->first, id_and_flag.substr( 0, pos ), id_and_flag.substr( pos + 1 ) ) ) );
   }

   for( map< uint64_t, modifier_data >::iterator i = sorted_modifiers.begin( ); i != sorted_modifiers.end( ); ++i )
      all_modifier_data.push_back( i->second );
}

void model::impl::get_procedure_data( const string& class_name,
 vector< procedure_data >& all_procedure_data, bool include_base_procedures )
{
   class_const_iterator cci = fetch_class_info( class_name );

   setup_procedures( all_procedure_data, cci, include_base_procedures );
}

void model::impl::get_procedure_arg_data( const string& class_name,
 const string& procedure_name, vector< procedure_arg_data >& all_procedure_arg_data )
{
   procedure_const_iterator pci = fetch_procedure_info( class_name, procedure_name );

   arg_index_const_iterator aici;
   for( aici = ( pci->second ).args_index.begin( ); aici != ( pci->second ).args_index.end( ); ++aici )
   {
      arg_const_iterator aci = ( pci->second ).args.find( aici->second );
      if( aci == ( pci->second ).args.end( ) )
         throw runtime_error( "unexpected arg_info entry not found for arg '" + aici->first + "' (id = " + aici->second + "'" );

      all_procedure_arg_data.push_back(
       procedure_arg_data( aici->second, aici->first, ( aci->second ).type, ( aci->second ).is_output ) );
   }
}

void model::impl::get_relationship_data( const string& relationship_name, relationship_data& rel_data )
{
   relationship_index_const_iterator rici = relationships_index.find( relationship_name );
   if( rici == relationships_index.end( ) )
      throw runtime_error( "unknown relationship '" + relationship_name + "'" );

   relationship_const_iterator rci = relationships.find( rici->second );
   if( rci == relationships.end( ) )
      throw runtime_error( "unexpected relationship_info entry not found for relationship id '" + rici->second + "'" );

   class_const_iterator child_cci, parent_cci;

   child_cci = classes.find( ( rci->second ).child_id );
   if( child_cci == classes.end( ) )
      throw runtime_error( "unable to find class_info for child class id '" + ( rci->second ).child_id + "'" );

   parent_cci = classes.find( ( rci->second ).parent_id );
   if( parent_cci == classes.end( ) )
      throw runtime_error( "unable to find class_info for parent class id '" + ( rci->second ).parent_id + "'" );

   rel_data.id = rci->first;
   rel_data.name = relationship_name;
   rel_data.child_class_name = ( child_cci->second ).name;
   rel_data.parent_class_name = ( parent_cci->second ).name;
}

void model::impl::get_child_relationship_data(
 const string& parent_class_name, vector< child_relationship_data >& child_rel_info )
{
   class_index_const_iterator cici = classes_index.find( parent_class_name );
   if( cici == classes_index.end( ) )
      throw runtime_error( "unknown class '" + parent_class_name + "'" );

   string parent_id( cici->second );

   relationship_index_const_iterator rici;
   for( rici = relationships_index.begin( ); rici != relationships_index.end( ); ++rici )
   {
      string name( rici->first );
      class_const_iterator child_cci, parent_cci;
      relationship_const_iterator rci = fetch_relationship_info( rici->second, &child_cci, &parent_cci, &name );

      if( ( rci->second ).parent_id == parent_id )
      {
         string child_name( ( child_cci->second ).name );

         child_rel_info.push_back(
          child_relationship_data( rci->first, name,
          rici->first, child_name, get_cascade_op_name( ( rci->second ).cascade_op ) ) );
      }
   }
}

void model::impl::get_specification_data( const string& class_name,
 vector< specification_data >& all_spec_data, get_specifications_type gstype, bool is_external )
{
   class_const_iterator cci = fetch_class_info( class_name );

   if( gstype == e_get_specifications_type_interface_and_facets_only )
   {
      string base_class_id( ( cci->second ).base_class_id );
      if( !base_class_id.empty( ) )
      {
         class_const_iterator cci;
         external_reference_const_iterator erci;
         locate_base_class( base_class_id, erci, cci );

         // NOTE: If an external base class is found then add any "facet" specifications it might contain.
         if( erci != external_references.end( ) )
            erci->second->p_impl->get_specification_data(
             ( cci->second ).name, all_spec_data, gstype, true );
      }
   }

   for( specification_detail_const_iterator
    sdci = ( cci->second ).specification_details.begin( ), end = ( cci->second ).specification_details.end( ); sdci != end; ++sdci )
   {
      specification_const_iterator sci = specifications.find( sdci->first );
      if( sci == specifications.end( ) )
         throw runtime_error( "unexpected specification_info entry not found for specification id '" + sdci->first + "'" );

      if( !( sci->second ).rp_spec->has_specification_data_for_class( cci->first ) )
         continue;

      if( gstype == e_get_specifications_type_interface_and_facets_only
       && !( sci->second ).rp_spec->is_facet_specification( ) && !( sci->second ).rp_spec->is_interface_specification( ) )
         continue;

      if( gstype == e_get_specifications_type_non_interface && ( sci->second ).rp_spec->is_interface_specification( ) )
         continue;

      if( is_external && !( sci->second ).rp_spec->is_facet_specification( ) )
         continue;

      specification_data spec_data;

      spec_data.id = sdci->first;
      spec_data.name = get_undecorated_class_name( ( sci->second ).name );
      spec_data.type = ( sci->second ).type;
      spec_data.decorated_name = ( sci->second ).name;

      spec_data.data_pairs.push_back( make_pair( c_attribute_specification_id, sci->first ) );

      specification_data_source sdc = ( sci->second ).rp_spec->get_specification_data_source( );

      // NOTE: If the specification data source is from the "model" then insert specification items according to the
      // names that had been tied against the fields, procedures and procedure arguments by the specification itself.
      if( sdc == e_specification_data_source_model )
      {
         const class_info& ci( cci->second );

         for( field_const_iterator fci = ci.fields.begin( ), end = ci.fields.end( ); fci != end; ++fci )
         {
            specification_detail_const_iterator fsdci = ( fci->second ).specification_details.lower_bound( sdci->first );
            for( ; fsdci != ( fci->second ).specification_details.end( ) && fsdci->first == sdci->first; ++fsdci )
            {
               string key( fsdci->second );
               spec_data.data_pairs.push_back( make_pair( key, get_undecorated_class_name( ( fci->second ).name ) ) );

               key += "_id";
               spec_data.data_pairs.push_back( make_pair( key, fci->first ) );
            }
         }

         for( procedure_const_iterator pci = ci.procedures.begin( ), end = ci.procedures.end( ); pci != end; ++pci )
         {
            specification_detail_const_iterator psdci = ( pci->second ).specification_details.lower_bound( sdci->first );
            for( ; psdci != ( pci->second ).specification_details.end( ) && psdci->first == sdci->first; ++psdci )
               spec_data.data_pairs.push_back( make_pair( psdci->second, get_undecorated_class_name( ( pci->second ).name ) ) );

            for( arg_const_iterator aci = ( pci->second ).args.begin( ), end = ( pci->second ).args.end( ); aci != end; ++aci )
            {
               specification_detail_const_iterator asdci = ( aci->second ).specification_details.lower_bound( sdci->first );
               for( ; asdci != ( aci->second ).specification_details.end( ) && asdci->first == sdci->first; ++asdci )
                  spec_data.data_pairs.push_back( make_pair( asdci->second, get_undecorated_class_name( ( aci->second ).name ) ) );
            }
         }
      }

      // NOTE: The specification object is now called to add any data and/or strings of its own.
      ( sci->second ).rp_spec->add_specification_data( *p_model, spec_data );
      ( sci->second ).rp_spec->get_specification_strings( *p_model, spec_data.strings );

      all_spec_data.push_back( spec_data );
   }
}

void model::impl::get_user_defined_enum_data(
 vector< user_defined_enum_data >& all_ude_data, bool include_external_enums ) const
{
   if( include_external_enums )
   {
      external_reference_const_iterator erci;

      for( erci = external_references.begin( ); erci != external_references.end( ); ++erci )
         erci->second->p_impl->get_user_defined_enum_data( all_ude_data, false );
   }

   for( user_defined_enum_index_const_iterator
    uici = user_defined_enums_index.begin( ), end = user_defined_enums_index.end( ); uici != end; ++uici )
   {
      user_defined_enum_const_iterator uci = user_defined_enums.find( uici->second );
      if( uci == user_defined_enums.end( ) )
         throw runtime_error(
          "unexpected user_defined_enum_info entry not found for user_defined_enum '" + uici->first + "' (id = " + uici->second + ")" );

      all_ude_data.push_back( user_defined_enum_data( uci->first, ( uci->second ).name, ( uci->second ).type, ( uci->second ).key_values ) );
   }
}

void model::impl::get_user_defined_type_data(
 vector< user_defined_type_data >& all_udt_data, bool include_external_types ) const
{
   if( include_external_types )
   {
      external_reference_const_iterator erci;

      for( erci = external_references.begin( ); erci != external_references.end( ); ++erci )
         erci->second->p_impl->get_user_defined_type_data( all_udt_data, false );
   }

   for( user_defined_type_index_const_iterator
    uici = user_defined_types_index.begin( ), end = user_defined_types_index.end( ); uici != end; ++uici )
   {
      user_defined_type_const_iterator uci = user_defined_types.find( uici->second );
      if( uci == user_defined_types.end( ) )
         throw runtime_error(
          "unexpected user_defined_type_info entry not found for user_defined_type '" + uici->first + "' (id = " + uici->second + ")" );

      string type( ( uci->second ).type );

      string extra;
      user_defined_type_const_iterator udtci( uci );

      string next_type( type );
      // NOTE: Append any "extra" information obtained from (possibly aggregated) user defined types.
      do
      {
         if( ( udtci->second ).rp_domain )
         {
            if( !extra.empty( ) )
               extra += "+";
            extra += ( udtci->second ).rp_domain->get_extra( );
         }

         // NOTE: If is an aggregrate type then change the "type" to the next type's name.
         if( type.empty( ) )
            type = ( udtci->second ).name;
         else if( type[ 0 ] >= '0' && type[ 0 ] <= '9' )
            type.erase( );

         next_type = ( udtci->second ).type;
      } while( ( udtci = user_defined_types.find( next_type ) ) != user_defined_types.end( ) );

      string sys_type( ( uci->second ).sys_type );

      string domain_type;
      if( ( uci->second ).rp_domain )
         domain_type = ( uci->second ).rp_domain->get_type_name( );

      all_udt_data.push_back( user_defined_type_data( uci->first,
       ( uci->second ).name, type, sys_type, extra, ( uci->second ).domain_name, domain_type ) );
   }
}

void model::impl::get_all_base_related_classes(
 const string& class_name, set< string >& all_related_classes, bool is_base_class ) const
{
   class_const_iterator cci = fetch_class_info( class_name );

   string base_class_id( ( cci->second ).base_class_id );

   if( !base_class_id.empty( ) )
   {
      string::size_type pos = base_class_id.find( ':' );
      if( pos == string::npos )
      {
         class_const_iterator cci = classes.find( base_class_id );
         if( cci == classes.end( ) )
            throw runtime_error( "unexpected class_info entry not found for base class id '" + base_class_id + "'" );

         get_all_base_related_classes( cci->second.name, all_related_classes, true );
      }
   }

   if( is_base_class )
   {
      vector< field_data > all_fk_field_data;
      p_model->get_field_data( class_name, all_fk_field_data, e_get_field_type_relationships );

      for( vector< field_data >::size_type i = 0; i < all_fk_field_data.size( ); i++ )
         all_related_classes.insert( all_fk_field_data[ i ].type );

      vector< child_relationship_data > child_rel_info;
      p_model->get_child_relationship_data( class_name, child_rel_info );

      for( vector< child_relationship_data >::size_type i = 0; i < child_rel_info.size( ); i++ )
         all_related_classes.insert( p_model->get_name( ) + "_" + child_rel_info[ i ].child_class_name );
   }
}

void model::impl::check( ) const
{
   if( id.empty( ) )
      throw runtime_error( "model has not been given an id" );

   if( name.empty( ) )
      throw runtime_error( "model has not been given a name" );

   if( version.empty( ) )
      throw runtime_error( "model has not been given a version" );

   class_const_iterator cci;
   for( cci = classes.begin( ); cci != classes.end( ); ++cci )
   {
      if( ( cci->second ).base_class_id.empty( ) && ( cci->second ).fields.empty( ) && ( cci->second ).procedures.empty( ) )
         throw runtime_error( "base class '" + ( cci->second ).name + "' contains no fields or procedures" );

      if( ( cci->second ).base_class_id.empty( ) && !( cci->second ).extra.empty( ) )
      {
         int num_order_fields = 0;

         for( field_index_const_iterator fici =
          ( cci->second ).fields_index.begin( ); fici != ( cci->second ).fields_index.end( ); ++fici )
         {
            field_const_iterator fci = ( cci->second ).fields.find( fici->second );
            if( fci == ( cci->second ).fields.end( ) )
               throw runtime_error(
                "unexpected field_info entry not found for field '" + fici->first + "' (id = " + fici->second + ")" );

            if( !( fci->second ).extra.empty( ) )
            {
               set< string > extra_set;
               split( ( fci->second ).extra, extra_set, '+' );

               if( extra_set.count( c_field_extra_order ) )
                  ++num_order_fields;
            }
         }

         set< string > extra_set;
         split( ( cci->second ).extra, extra_set, '+' );

         if( extra_set.count( c_class_extra_ordered ) )
         {
            if( num_order_fields != 1 )
               throw runtime_error( "class '"
                + ( cci->second ).name + "' is 'ordered' and requires exactly one 'order' field" );
         }
         else
         {
            if( num_order_fields != 0 )
               throw runtime_error( "class '"
                + ( cci->second ).name + "' needs to be 'ordered' as it contains an 'order' field" );
         }
      }
   }
}

void model::impl::save( const string& filename )
{
   ofstream outf( filename.c_str( ) );
   if( !outf )
      throw runtime_error( "unable to open file '" + filename + "' for output" );

   sio_writer writer( outf );

   writer.start_section( c_section_model );
   writer.write_attribute( c_attribute_model_id, id );
   writer.write_attribute( c_attribute_model_name, name );
   writer.write_attribute( c_attribute_model_perm, perm );
   writer.write_attribute( c_attribute_model_version, version );
   writer.write_attribute( c_attribute_model_year_created, to_string( year_created ) );

   writer.start_section( c_section_classes );

   class_index_const_iterator cici;
   for( cici = classes_index.begin( ); cici != classes_index.end( ); ++cici )
   {
      writer.start_section( c_section_class );

      class_const_iterator cci = classes.find( cici->second );
      if( cci == classes.end( ) )
         throw runtime_error( "unexpected class_info entry not found for class id '" + cici->second + "'" );

      writer.write_attribute( c_attribute_class_id, cci->first );
      writer.write_attribute( c_attribute_class_name, ( cci->second ).name );
      writer.write_attribute( c_attribute_class_plural, ( cci->second ).plural );
      writer.write_attribute( c_attribute_class_version, ( cci->second ).version );

      writer.write_opt_attribute( c_attribute_class_fixed_key_val, ( cci->second ).fixed_key_val );
      writer.write_opt_attribute( c_attribute_class_extra, ( cci->second ).extra );

      writer.write_opt_attribute( c_attribute_class_base_class_id, ( cci->second ).base_class_id );

      const class_info& ci( cci->second );
      write_specification_details( writer, ci.specification_details.begin( ), ci.specification_details.end( ) );

      writer.start_section( c_section_fields );
      for( field_index_const_iterator fici =
       ( cci->second ).fields_index.begin( ); fici != ( cci->second ).fields_index.end( ); ++fici )
      {
         field_const_iterator fci = ( cci->second ).fields.find( fici->second );
         if( fci == ( cci->second ).fields.end( ) )
            throw runtime_error(
             "unexpected field_info entry not found for field '" + fici->first + "' (id = " + fici->second + ")" );

         writer.start_section( c_section_field );

         writer.write_attribute( c_attribute_field_id, fci->first );
         writer.write_attribute( c_attribute_field_name, ( fci->second ).name );
         writer.write_attribute( c_attribute_field_type, ( fci->second ).type );

         string mandatory( c_false );
         if( ( fci->second ).is_mandatory )
            mandatory = c_true;
         writer.write_attribute( c_attribute_field_mandatory, mandatory );

         if( ( fci->second ).is_transient )
            writer.write_opt_attribute( c_attribute_field_transient, c_true );

         if( ( fci->second ).for_text_search )
            writer.write_opt_attribute( c_attribute_field_text_search, c_true );

         writer.write_opt_attribute( c_attribute_field_enum_id, ( fci->second ).enum_id );
         writer.write_opt_attribute( c_attribute_field_enum_filter_id, ( fci->second ).enum_filter_id );

         writer.write_opt_attribute( c_attribute_field_extra, ( fci->second ).extra );
         writer.write_opt_attribute( c_attribute_field_default_value, ( fci->second ).default_value );

         writer.write_opt_attribute( c_attribute_field_class_id, ( fci->second ).class_id );
         writer.write_opt_attribute( c_attribute_field_relationship_id, ( fci->second ).relationship_id );

         writer.start_section( c_section_element_indexes );
         for( index_detail_const_iterator idci =
          ( fci->second ).index_details.begin( ); idci != ( fci->second ).index_details.end( ); ++idci )
         {
            writer.start_section( c_section_element_index );
            writer.write_attribute( c_attribute_element_index_id, idci->first );
            writer.write_attribute( c_attribute_element_index_num, to_string( idci->second ) );
            writer.finish_section( c_section_element_index );
         }
         writer.finish_section( c_section_element_indexes );

         writer.start_section( c_section_element_modifiers );
         for( modifier_detail_const_iterator mdci =
          ( fci->second ).modifier_details.begin( ); mdci != ( fci->second ).modifier_details.end( ); ++mdci )
         {
            writer.start_section( c_section_element_modifier );
            writer.write_attribute( c_attribute_element_modifier_id, mdci->first );
            writer.write_attribute( c_attribute_element_modifier_affect, mdci->second );
            writer.finish_section( c_section_element_modifier );
         }
         writer.finish_section( c_section_element_modifiers );

         const field_info& fi( fci->second );
         write_specification_details( writer, fi.specification_details.begin( ), fi.specification_details.end( ) );

         writer.finish_section( c_section_field );
      }
      writer.finish_section( c_section_fields );

      writer.start_section( c_section_indexes );
      for( index_index_const_iterator iici =
       ( cci->second ).indexes_index.begin( ); iici != ( cci->second ).indexes_index.end( ); ++iici )
      {
         index_const_iterator ici = ( cci->second ).indexes.find( iici->second );
         if( ici == ( cci->second ).indexes.end( ) )
            throw runtime_error( "unexpected index_info entry not found for index '" + iici->first + "' (id = " + iici->second + "'" );

         writer.start_section( c_section_index );

         writer.write_attribute( c_attribute_index_id, ici->first );
         writer.write_attribute( c_attribute_index_name, ( ici->second ).name );

         string unique( c_false );
         if( ( ici->second ).is_unique )
            unique = c_true;
         writer.write_attribute( c_attribute_index_unique, unique );

         writer.finish_section( c_section_index );
      }
      writer.finish_section( c_section_indexes );

      writer.start_section( c_section_modifiers );
      for( modifier_index_const_iterator mici =
       ( cci->second ).modifiers_index.begin( ); mici != ( cci->second ).modifiers_index.end( ); ++mici )
      {
         modifier_const_iterator mci = ( cci->second ).modifiers.find( mici->second );
         if( mci == ( cci->second ).modifiers.end( ) )
            throw runtime_error( "unexpected modifier_info entry not found for modifier '" + mici->first + "' (id = " + mici->second + "'" );

         writer.start_section( c_section_modifier );

         writer.write_attribute( c_attribute_modifier_id, mci->first );
         writer.write_attribute( c_attribute_modifier_name, ( mci->second ).name );

         modifier_detail_const_iterator mdci = ( cci->second ).modifier_details.find( mci->first );
         if( mdci != ( cci->second ).modifier_details.end( ) )
            writer.write_opt_attribute( c_attribute_element_modifier_affect, mdci->second );

         writer.finish_section( c_section_modifier );
      }
      writer.finish_section( c_section_modifiers );

      writer.start_section( c_section_procedures );
      for( procedure_index_const_iterator pici =
       ( cci->second ).procedures_index.begin( ); pici != ( cci->second ).procedures_index.end( ); ++pici )
      {
         procedure_const_iterator pci = ( cci->second ).procedures.find( pici->second );
         if( pci == ( cci->second ).procedures.end( ) )
            throw runtime_error(
             "unexpected procedure_info entry not found for procedure '" + pici->first + "' (id = " + pici->second + ")" );

         writer.start_section( c_section_procedure );

         writer.write_attribute( c_attribute_procedure_id, pci->first );
         writer.write_attribute( c_attribute_procedure_name, ( pci->second ).name );

         const procedure_info& pi( pci->second );
         write_specification_details( writer, pi.specification_details.begin( ), pi.specification_details.end( ) );

         writer.start_section( c_section_procedure_args );
         for( arg_index_const_iterator aici = ( pci->second ).args_index.begin( ); aici != ( pci->second ).args_index.end( ); ++aici )
         {
            arg_const_iterator aci = ( pci->second ).args.find( aici->second );
            if( aci == ( pci->second ).args.end( ) )
               throw runtime_error(
                "unexpected arg_info entry not found for arg '" + aici->first + "' (id = " + aici->second + ")" );

            writer.start_section( c_section_procedure_arg );

            writer.write_attribute( c_attribute_procedure_arg_id, aci->first );
            writer.write_attribute( c_attribute_procedure_arg_name, ( aci->second ).name );
            writer.write_attribute( c_attribute_procedure_arg_type, ( aci->second ).type );

            string output( c_false );
            if( ( aci->second ).is_output )
               output = c_true;
            writer.write_attribute( c_attribute_procedure_arg_output, output );

            const arg_info& ai( aci->second );
            write_specification_details( writer, ai.specification_details.begin( ), ai.specification_details.end( ) );

            writer.finish_section( c_section_procedure_arg );
         }
         writer.finish_section( c_section_procedure_args );

         writer.finish_section( c_section_procedure );
      }
      writer.finish_section( c_section_procedures );

      writer.finish_section( c_section_class );
   }

   writer.finish_section( c_section_classes );

   writer.start_section( c_section_relationships );
   for( relationship_index_const_iterator rici = relationships_index.begin( ); rici != relationships_index.end( ); ++rici )
   {
      relationship_const_iterator rci = relationships.find( rici->second );
      if( rci == relationships.end( ) )
         throw runtime_error(
          "unexpected relationship_info entry not found for relationship '" + rici->first + "' (id = " + rici->second + ")" );

      writer.start_section( c_section_relationship );

      writer.write_attribute( c_attribute_relationship_id, rci->first );
      writer.write_attribute( c_attribute_relationship_name, ( rci->second ).name );
      writer.write_attribute( c_attribute_relationship_parent_id, ( rci->second ).parent_id );
      writer.write_attribute( c_attribute_relationship_child_id, ( rci->second ).child_id );
      writer.write_attribute( c_attribute_relationship_cascade_op, get_cascade_op_name( ( rci->second ).cascade_op ) );

      const relationship_info& ri( rci->second );
      write_specification_details( writer, ri.specification_details.begin( ), ri.specification_details.end( ) );

      writer.finish_section( c_section_relationship );
   }
   writer.finish_section( c_section_relationships );

   writer.start_section( c_section_specifications );
   for( specification_index_const_iterator sici = specifications_index.begin( ); sici != specifications_index.end( ); ++sici )
   {
      specification_const_iterator sci = specifications.find( sici->second );
      if( sci == specifications.end( ) )
         throw runtime_error(
          "unexpected specification_info entry not found for specification '" + sici->first + "' (id = " + sici->second + ")" );

      writer.start_section( c_section_specification );

      writer.write_attribute( c_attribute_specification_id, sci->first );
      writer.write_attribute( c_attribute_specification_name, ( sci->second ).name );
      writer.write_attribute( c_attribute_specification_type, ( sci->second ).type );

      writer.start_section( c_section_specification_data );
      ( sci->second ).rp_spec->write_data( writer );
      writer.finish_section( c_section_specification_data );

      writer.finish_section( c_section_specification );
   }
   writer.finish_section( c_section_specifications );

   writer.start_section( c_section_user_defined_enums );
   for( user_defined_enum_index_const_iterator
    uici = user_defined_enums_index.begin( ); uici != user_defined_enums_index.end( ); ++uici )
   {
      user_defined_enum_const_iterator uci = user_defined_enums.find( uici->second );
      if( uci == user_defined_enums.end( ) )
         throw runtime_error(
          "unexpected user_defined_enum_info entry not found for user_defined_enum '" + uici->first + "' (id = " + uici->second + ")" );

      writer.start_section( c_section_user_defined_enum );

      writer.write_attribute( c_attribute_user_defined_enum_id, uci->first );
      writer.write_attribute( c_attribute_user_defined_enum_name, ( uci->second ).name );
      writer.write_attribute( c_attribute_user_defined_enum_type, ( uci->second ).type );
      writer.write_attribute( c_attribute_user_defined_enum_key_values, ( uci->second ).key_values );

      writer.finish_section( c_section_user_defined_enum );
   }
   writer.finish_section( c_section_user_defined_enums );

   writer.start_section( c_section_user_defined_types );
   for( user_defined_type_index_const_iterator
    uici = user_defined_types_index.begin( ); uici != user_defined_types_index.end( ); ++uici )
   {
      user_defined_type_const_iterator uci = user_defined_types.find( uici->second );
      if( uci == user_defined_types.end( ) )
         throw runtime_error(
          "unexpected user_defined_type_info entry not found for user_defined_type '" + uici->first + "' (id = " + uici->second + ")" );

      writer.start_section( c_section_user_defined_type );

      writer.write_attribute( c_attribute_user_defined_type_id, uci->first );
      writer.write_attribute( c_attribute_user_defined_type_name, ( uci->second ).name );
      writer.write_attribute( c_attribute_user_defined_type_type, ( uci->second ).type );
      writer.write_attribute( c_attribute_user_defined_type_sys_type, ( uci->second ).sys_type );

      if( !( uci->second ).domain_name.empty( ) )
      {
         writer.start_section( c_section_domain );

         writer.write_attribute( c_attribute_domain_name, ( uci->second ).domain_name );
         ( uci->second ).rp_domain->write_data( writer );

         writer.finish_section( c_section_domain );
      }

      writer.finish_section( c_section_user_defined_type );
   }
   writer.finish_section( c_section_user_defined_types );

   writer.finish_section( c_section_model );

   writer.finish_sections( );
}

void model::impl::load( const string& filename )
{
   ifstream inpf( filename.c_str( ) );
   if( !inpf )
      throw runtime_error( "unable to open file '" + filename + "' for input" );

   sio_reader reader( inpf );

   string s;

   model::impl old_model( *this );
   try
   {
      clear( );

      reader.start_section( c_section_model );

      id = reader.read_attribute( c_attribute_model_id );
      name = reader.read_attribute( c_attribute_model_name );
      perm = reader.read_attribute( c_attribute_model_perm );
      version = reader.read_attribute( c_attribute_model_version );
      year_created = from_string< year >( reader.read_attribute( c_attribute_model_year_created ) );

      reader.start_section( c_section_classes );
      while( reader.has_started_section( c_section_class ) )
      {
         string id;
         class_info ci;

         id = reader.read_attribute( c_attribute_class_id );
         ci.name = reader.read_attribute( c_attribute_class_name );
         ci.plural = reader.read_attribute( c_attribute_class_plural );
         ci.version = reader.read_attribute( c_attribute_class_version );

         ci.fixed_key_val = reader.read_opt_attribute( c_attribute_class_fixed_key_val );
         ci.extra = reader.read_opt_attribute( c_attribute_class_extra );

         ci.base_class_id = reader.read_opt_attribute( c_attribute_class_base_class_id );

         // NOTE: Load models that are required to resolve external references.
         if( !ci.base_class_id.empty( ) )
         {
            if( ci.is_external_alias( ) )
            {
               string model_name = ci.get_external_model( );
               external_reference_const_iterator erci = external_references.find( model_name );

               if( erci == external_references.end( ) )
               {
                  ref_count_ptr< model > rp_model( new model );

                  rp_model->p_impl->p_parent_model = p_model;
                  rp_model->load( model_name + get_model_file_ext( ) );

                  external_references.insert( external_reference_value_type( model_name, rp_model ) );

                  if( p_parent_model )
                     p_parent_model->p_impl->external_references.insert( external_reference_value_type( model_name, rp_model ) );
               }
            }
         }

         read_specification_details( reader, ci.specification_details );

         reader.start_section( c_section_fields );
         while( reader.has_started_section( c_section_field ) )
         {
            string id;
            field_info fi;

            id = reader.read_attribute( c_attribute_field_id );
            fi.name = reader.read_attribute( c_attribute_field_name );
            fi.type = reader.read_attribute( c_attribute_field_type );

            s = reader.read_attribute( c_attribute_field_mandatory );
            if( s == c_true )
               fi.is_mandatory = true;

            fi.is_transient = false;
            if( reader.read_opt_attribute( c_attribute_field_transient ) == c_true )
               fi.is_transient = true;

            fi.for_text_search = false;
            if( reader.read_opt_attribute( c_attribute_field_text_search ) == c_true )
               fi.for_text_search = true;

            fi.enum_id = reader.read_opt_attribute( c_attribute_field_enum_id );
            fi.enum_filter_id = reader.read_opt_attribute( c_attribute_field_enum_filter_id );

            fi.extra = reader.read_opt_attribute( c_attribute_field_extra );
            fi.default_value = reader.read_opt_attribute( c_attribute_field_default_value );

            fi.class_id = reader.read_opt_attribute( c_attribute_field_class_id );
            fi.relationship_id = reader.read_opt_attribute( c_attribute_field_relationship_id );

            reader.start_section( c_section_element_indexes );
            while( reader.has_started_section( c_section_element_index ) )
            {
               string id;
               size_t num;

               id = reader.read_attribute( c_attribute_element_index_id );
               num = from_string< size_t >( reader.read_attribute( c_attribute_element_index_num ) );

               fi.index_details.insert( index_detail_value_type( id, num ) );

               reader.finish_section( c_section_element_index );
            }
            reader.finish_section( c_section_element_indexes );

            reader.start_section( c_section_element_modifiers );
            while( reader.has_started_section( c_section_element_modifier ) )
            {
               string id;
               string affect;

               id = reader.read_attribute( c_attribute_element_modifier_id );
               affect = reader.read_attribute( c_attribute_element_modifier_affect );

               fi.modifier_details.insert( modifier_detail_value_type( id, affect ) );

               reader.finish_section( c_section_element_modifier );
            }
            reader.finish_section( c_section_element_modifiers );

            read_specification_details( reader, fi.specification_details );

            ci.fields.insert( field_value_type( id, fi ) );
            ci.fields_index.insert( field_index_value_type( fi.name, id ) );

            reader.finish_section( c_section_field );
         }
         reader.finish_section( c_section_fields );

         reader.start_section( c_section_indexes );
         while( reader.has_started_section( c_section_index ) )
         {
            string id;
            index_info ii;

            id = reader.read_attribute( c_attribute_index_id );
            ii.name = reader.read_attribute( c_attribute_index_name );

            s = reader.read_attribute( c_attribute_index_unique );
            if( s == c_true )
               ii.is_unique = true;

            ci.indexes.insert( index_value_type( id, ii ) );
            ci.indexes_index.insert( index_index_value_type( ii.name, id ) );

            reader.finish_section( c_section_index );
         }
         reader.finish_section( c_section_indexes );

         reader.start_section( c_section_modifiers );
         while( reader.has_started_section( c_section_modifier ) )
         {
            string id;
            modifier_info mi;

            id = reader.read_attribute( c_attribute_modifier_id );
            mi.name = reader.read_attribute( c_attribute_modifier_name );

            ci.modifiers.insert( modifier_value_type( id, mi ) );
            ci.modifiers_index.insert( modifier_index_value_type( mi.name, id ) );

            string modifier_affect = reader.read_opt_attribute( c_attribute_element_modifier_affect );
            if( !modifier_affect.empty( ) )
               ci.modifier_details.insert( modifier_detail_value_type( id, modifier_affect ) );

            reader.finish_section( c_section_modifier );
         }
         reader.finish_section( c_section_modifiers );

         reader.start_section( c_section_procedures );
         while( reader.has_started_section( c_section_procedure ) )
         {
            string id;
            procedure_info pi;

            id = reader.read_attribute( c_attribute_procedure_id );
            pi.name = reader.read_attribute( c_attribute_procedure_name );

            read_specification_details( reader, pi.specification_details );

            reader.start_section( c_section_procedure_args );
            while( reader.has_started_section( c_section_procedure_arg ) )
            {
               string id;
               arg_info ai;

               id = reader.read_attribute( c_attribute_procedure_arg_id );
               ai.name = reader.read_attribute( c_attribute_procedure_arg_name );
               ai.type = reader.read_attribute( c_attribute_procedure_arg_type );

               s = reader.read_attribute( c_attribute_procedure_arg_output );
               if( s == c_true )
                  ai.is_output = true;

               read_specification_details( reader, ai.specification_details );

               pi.args.insert( arg_value_type( id, ai ) );
               pi.args_index.insert( arg_index_value_type( ai.name, id ) );

               reader.finish_section( c_section_procedure_arg );
            }
            reader.finish_section( c_section_procedure_args );

            ci.procedures.insert( procedure_value_type( id, pi ) );
            ci.procedures_index.insert( procedure_index_value_type( pi.name, id ) );

            reader.finish_section( c_section_procedure );
         }
         reader.finish_section( c_section_procedures );

         classes.insert( class_value_type( id, ci ) );
         classes_index.insert( class_index_value_type( ci.name, id ) );

         reader.finish_section( c_section_class );
      }
      reader.finish_section( c_section_classes );

      reader.start_section( c_section_relationships );
      while( reader.has_started_section( c_section_relationship ) )
      {
         string id;
         relationship_info ri;

         id = reader.read_attribute( c_attribute_relationship_id );
         ri.name = reader.read_attribute( c_attribute_relationship_name );
         ri.parent_id = reader.read_attribute( c_attribute_relationship_parent_id );
         ri.child_id = reader.read_attribute( c_attribute_relationship_child_id );

         s = reader.read_attribute( c_attribute_relationship_cascade_op );
         ri.cascade_op = get_cascade_op_for_name( s );

         read_specification_details( reader, ri.specification_details );

         relationships.insert( relationship_value_type( id, ri ) );
         relationships_index.insert( relationship_index_value_type( ri.name, id ) );

         reader.finish_section( c_section_relationship );
      }
      reader.finish_section( c_section_relationships );

      reader.start_section( c_section_specifications );
      while( reader.has_started_section( c_section_specification ) )
      {
         string id;
         specification_info si;

         id = reader.read_attribute( c_attribute_specification_id );
         si.name = reader.read_attribute( c_attribute_specification_name );
         si.type = reader.read_attribute( c_attribute_specification_type );

         si.rp_spec = get_specification_factory( ).construct( si.type );

         reader.start_section( c_section_specification_data );
         si.rp_spec->read_data( reader );
         reader.finish_section( c_section_specification_data );

         specifications.insert( specification_value_type( id, si ) );
         specifications_index.insert( specification_index_value_type( si.name, id ) );

         reader.finish_section( c_section_specification );
      }
      reader.finish_section( c_section_specifications );

      reader.start_section( c_section_user_defined_enums );
      while( reader.has_started_section( c_section_user_defined_enum ) )
      {
         string id;
         user_defined_enum_info ui;

         id = reader.read_attribute( c_attribute_user_defined_enum_id );
         ui.name = reader.read_attribute( c_attribute_user_defined_enum_name );
         ui.type = reader.read_attribute( c_attribute_user_defined_enum_type );
         ui.key_values = reader.read_attribute( c_attribute_user_defined_enum_key_values );

         user_defined_enums.insert( user_defined_enum_value_type( id, ui ) );
         user_defined_enums_index.insert( user_defined_enum_index_value_type( ui.name, id ) );

         reader.finish_section( c_section_user_defined_enum );
      }
      reader.finish_section( c_section_user_defined_enums );

      reader.start_section( c_section_user_defined_types );
      while( reader.has_started_section( c_section_user_defined_type ) )
      {
         string id;
         user_defined_type_info ui;

         id = reader.read_attribute( c_attribute_user_defined_type_id );
         ui.name = reader.read_attribute( c_attribute_user_defined_type_name );
         ui.type = reader.read_attribute( c_attribute_user_defined_type_type );
         ui.sys_type = reader.read_attribute( c_attribute_user_defined_type_sys_type );

         if( reader.has_started_section( c_section_domain ) )
         {
            ui.domain_name = reader.read_attribute( c_attribute_domain_name );

            ui.rp_domain = get_domain_factory( ).construct( ui.sys_type + '_' + ui.domain_name );

            if( !ui.rp_domain )
               throw runtime_error( "unexpected failure constructing domain '" + ui.domain_name + "' for type '" + ui.sys_type + "'" );

            ui.rp_domain->read_data( reader );

            reader.finish_section( c_section_domain );
         }

         user_defined_types.insert( user_defined_type_value_type( id, ui ) );
         user_defined_types_index.insert( user_defined_type_index_value_type( ui.name, id ) );

         reader.finish_section( c_section_user_defined_type );
      }
      reader.finish_section( c_section_user_defined_types );

      reader.finish_section( c_section_model );
      reader.verify_finished_sections( );
   }
   catch( ... )
   {
      *this = old_model;
      throw;
   }
}

void model::impl::clear( )
{
   classes.clear( );
   classes_index.clear( );

   relationships.clear( );
   relationships_index.clear( );
}

void model::impl::class_add( const string& class_id,
 const string& class_name, const string& plural_name,
 const string& fixed_key_val, const string& extra, const string& base_class_name )
{
   class_index_const_iterator cici = classes_index.find( class_name );
   if( cici != classes_index.end( ) )
      throw runtime_error( "class '" + class_name + "' already exists" );

   if( classes.count( class_id ) )
      throw runtime_error( "class id " + class_id + " is already in use" );

   string base_class_id;
   string class_extra( extra );

   if( !base_class_name.empty( ) )
   {
      bool found = false;
      string::size_type pos = base_class_name.find( ':' );
      if( pos == string::npos )
      {
         cici = classes_index.find( base_class_name );
         if( cici != classes_index.end( ) )
         {
            found = true;
            base_class_id = cici->second;
         }
      }
      else
      {
         string model_name = base_class_name.substr( 0, pos );
         external_reference_const_iterator erci = external_references.find( model_name );

         if( erci == external_references.end( ) )
         {
            ref_count_ptr< model > rp_model( new model );

            rp_model->p_impl->p_parent_model = p_model;
            rp_model->load( model_name + get_model_file_ext( ) );

            pair< external_reference_iterator, bool > rc;
            rc = external_references.insert( external_reference_value_type( model_name, rp_model ) );
            erci = rc.first;
         }

         cici = erci->second->p_impl->classes_index.find( base_class_name.substr( pos + 1 ) );
         if( cici != erci->second->p_impl->classes_index.end( ) )
         {
            found = true;
            base_class_id = model_name + ":" + cici->second;

            class_const_iterator cci = erci->second->p_impl->classes.find( cici->second );
            if( cci == erci->second->p_impl->classes.end( ) )
               throw runtime_error( "unexpected class_info entry not found for external class '"
                + cici->first + "' (id = " + cici->second + ")" );

            if( !extra.empty( ) )
               throw runtime_error( "cannot specify 'extra' attributes for external alias classes" );

            class_extra = ( cci->second ).extra;
         }
      }

      if( !found )
         throw runtime_error( "unknown base class '" + base_class_name + "'" );
   }

   pair< class_index_iterator, bool > ret = classes_index.insert( class_index_value_type( class_name, class_id ) );
   try
   {
      pair< class_iterator, bool > ret = classes.insert( class_value_type( class_id,
       class_info( class_name, plural_name, fixed_key_val, class_extra, base_class_id ) ) );
   }
   catch( ... )
   {
      classes_index.erase( ret.first );
      throw;
   }
}

void model::impl::class_list( ostream& outs ) const
{
   class_index_const_iterator cici;
   for( cici = classes_index.begin( ); cici != classes_index.end( ); ++cici )
   {
      string id( cici->second );
      class_const_iterator cci = classes.find( id );
      if( cci == classes.end( ) )
         throw runtime_error( "unexpected class_info entry not found for class '" + cici->first + "' (id = " + id + ")" );

      outs << cici->first << " (id = " << id << ")";

      string extra( ( cci->second ).extra );

      string base_class_name( get_base_class_name( ( cci->second ).base_class_id, &extra ) );
      if( !base_class_name.empty( ) )
         outs << " extends " << base_class_name;

      if( !( cci->second ).fixed_key_val.empty( ) )
         outs << " with fixed key: " << ( cci->second ).fixed_key_val;

      if( !extra.empty( ) )
         outs << " extra: " << extra;

      outs << '\n';
   }
}

void model::impl::class_remove( const string& class_name )
{
   class_index_iterator cii = classes_index.find( class_name );
   if( cii == classes_index.end( ) )
      throw runtime_error( "unknown class '" + class_name + "'" );

   string id( cii->second );
   class_iterator ci = classes.find( id );
   if( ci == classes.end( ) )
      throw runtime_error( "unexpected class_info entry not found for class '" + class_name + "' (id = " + id + ")" );

   if( !( ci->second ).specification_details.empty( ) )
      throw runtime_error( "cannot remove class whilst being used by one or more specifications" );

   vector< string > cascade_rels;
   relationship_const_iterator rci;
   for( rci = relationships.begin( ); rci != relationships.end( ); ++rci )
   {
      if( id == ( rci->second ).parent_id || id == ( rci->second ).child_id )
         cascade_rels.push_back( ( rci->second ).name );
   }

   for( vector< string >::size_type i = 0; i < cascade_rels.size( ); i++ )
      relationship_remove( cascade_rels[ i ] );

   classes.erase( ci );
   classes_index.erase( cii );
}

void model::impl::class_rename( const string& old_class_name, const string& new_class_name )
{
   class_index_iterator cii = classes_index.find( old_class_name );
   if( cii == classes_index.end( ) )
      throw runtime_error( "unknown class '" + old_class_name + "'" );

   class_index_const_iterator cici = classes_index.find( new_class_name );
   if( cici != classes_index.end( ) )
      throw runtime_error( "class name '" + new_class_name + "' is already in use" );

   string id( cii->second );
   class_iterator ci = classes.find( id );
   if( ci == classes.end( ) )
      throw runtime_error( "unexpected class_info entry not found for class '" + old_class_name + "' (id = " + id + ")" );

   model::impl old_model( *this );
   try
   {
      ( ci->second ).name = new_class_name;

      classes_index.erase( cii );
      classes_index.insert( class_index_value_type( new_class_name, ci->first ) );

      string old_decorated_name, new_decorated_name;

      old_decorated_name = '{';
      old_decorated_name += old_class_name;
      old_decorated_name += '}';

      new_decorated_name = '{';
      new_decorated_name += new_class_name;
      new_decorated_name += '}';

      vector< string > rel_names;
      for( relationship_index_const_iterator
       rici = relationships_index.begin( ), end = relationships_index.end( ); rici != end; ++rici )
      {
         if( rici->first.find( old_decorated_name ) != string::npos )
            rel_names.push_back( rici->first );
      }

      for( size_t i = 0; i < rel_names.size( ); i++ )
      {
         string old_rel_name( rel_names[ i ] );
         string new_rel_name( old_rel_name );

         while( true )
         {
            string::size_type pos = new_rel_name.find( old_decorated_name );
            if( pos == string::npos )
               break;

            new_rel_name.erase( pos, old_decorated_name.length( ) );
            new_rel_name.insert( pos, new_decorated_name );
         }

         relationship_rename( old_rel_name, new_rel_name );
      }

      vector< string > spec_names;
      for( specification_index_const_iterator
       sici = specifications_index.begin( ), end = specifications_index.end( ); sici != end; ++sici )
      {
         if( sici->first.find( old_decorated_name ) != string::npos )
            spec_names.push_back( sici->first );
      }

      for( size_t i = 0; i < spec_names.size( ); i++ )
      {
         string old_spec_name( spec_names[ i ] );
         string new_spec_name( old_spec_name );

         while( true )
         {
            string::size_type pos = new_spec_name.find( old_decorated_name );
            if( pos == string::npos )
               break;

            new_spec_name.erase( pos, old_decorated_name.length( ) );
            new_spec_name.insert( pos, new_decorated_name );
         }

         specification_rename( old_spec_name, new_spec_name );
      }
   }
   catch( ... )
   {
      *this = old_model;
      throw;
   }
}

void model::impl::field_add( const string& field_id,
 const string& class_name, const string& field_name, const string& field_type,
 bool is_mandatory, bool is_transient, bool for_text_search, const string& enum_name, const string& enum_filter,
 const string& extra, const string& default_value, const string& field_class_id, const string& relationship_id )
{
   class_index_const_iterator cici = classes_index.find( class_name );
   if( cici == classes_index.end( ) )
      throw runtime_error( "unknown class '" + class_name + "'" );

   field_add_impl( field_id, cici->second, field_name,
    field_type, is_mandatory, is_transient, for_text_search,
    enum_name, enum_filter, extra, default_value, field_class_id, relationship_id );
}

void model::impl::field_add_impl( const string& field_id,
 const string& class_id, const string& field_name, const string& field_type,
 bool is_mandatory, bool is_transient, bool for_text_search, const string& enum_name, const string& enum_filter,
 const string& extra, const string& default_value, const string& field_class_id, const string& relationship_id )
{
   class_iterator ci = classes.find( class_id );
   if( ci == classes.end( ) )
      throw runtime_error( "unexpected class_info entry not found for class id '" + class_id + "'" );

   if( ( ci->second ).is_external_alias( ) )
      throw runtime_error( "cannot add field to externally aliased class id '" + class_id + "'" );

   if( ( ci->second ).fields_index.find( field_name ) != ( ci->second ).fields_index.end( ) )
      throw runtime_error( "field '" + field_name + "' already exists for class '" + ( ci->second ).name + "'" );

   if( ( ci->second ).fields.count( field_id ) )
      throw runtime_error( "field id " + field_id + " is already in use for class '" + ( ci->second ).name + "'" );

   verify_base_class_does_not_have_field( ci, field_name );
   verify_derived_class_does_not_have_field( class_id, field_name );

   pair< field_index_iterator, bool > ret
    = ( ci->second ).fields_index.insert( field_index_value_type( field_name, field_id ) );
   try
   {
      string type( field_type );

      // NOTE: A udt can either be locally defined or from an externally referenced module.
      vector< user_defined_type_data > all_udt_data;
      get_user_defined_type_data( all_udt_data, true );

      for( size_t i = 0; i < all_udt_data.size( ); i++ )
      {
         if( type == all_udt_data[ i ].name )
         {
            type = all_udt_data[ i ].id;
            break;
         }
      }

      string enum_id;
      if( !enum_name.empty( ) )
      {
         // NOTE: An enum can either be locally defined or from an externally referenced module.
         vector< user_defined_enum_data > all_ude_data;
         get_user_defined_enum_data( all_ude_data, true );

         for( size_t i = 0; i < all_ude_data.size( ); i++ )
         {
            if( enum_name == all_ude_data[ i ].name )
            {
               if( type != all_ude_data[ i ].type )
                  throw runtime_error( "enum '" + enum_name
                   + "' can only be used for fields of type '" + all_ude_data[ i ].type + "'" );

               enum_id = all_ude_data[ i ].id;
               break;
            }
         }
      }

      string enum_filter_id;
      if( !enum_filter.empty( ) )
      {
         // NOTE: An enum can either be locally defined or from an externally referenced module.
         vector< user_defined_enum_data > all_ude_data;
         get_user_defined_enum_data( all_ude_data, true );

         for( size_t i = 0; i < all_ude_data.size( ); i++ )
         {
            if( enum_filter == all_ude_data[ i ].name )
            {
               enum_filter_id = all_ude_data[ i ].id;
               break;
            }
         }
      }

      field_info fi( field_name, type, is_mandatory, is_transient,
       for_text_search, enum_id, enum_filter_id, extra, default_value, field_class_id, relationship_id );

      ( ci->second ).fields.insert( field_value_type( field_id, fi ) );
   }
   catch( ... )
   {
      ( ci->second ).fields_index.erase( ret.first );
      throw;
   }
}

void model::impl::field_list( const string& class_name, ostream& outs ) const
{
   class_const_iterator cci = fetch_class_info( class_name );

   string base_class_id( ( cci->second ).base_class_id );

   if( !base_class_id.empty( ) )
   {
      string::size_type pos = base_class_id.find( ':' );
      if( pos != string::npos )
      {
         string model_name = base_class_id.substr( 0, pos );
         external_reference_const_iterator erci = external_references.find( model_name );

         if( erci == external_references.end( ) )
            throw runtime_error( "unexpected external reference entry not found for '" + model_name + "'" );

         class_const_iterator cci = erci->second->p_impl->classes.find( base_class_id.substr( pos + 1 ) );
         if( cci == erci->second->p_impl->classes.end( ) )
            throw runtime_error( "unexpected class_info entry not found for class id '"
             + base_class_id.substr( pos + 1 ) + "' in externally referenced model '" + model_name + "'" );

         erci->second->p_impl->field_list( cci->second.name, outs );
      }
      else
      {
         class_const_iterator cci = classes.find( base_class_id );
         if( cci == classes.end( ) )
            throw runtime_error( "unexpected class_info entry not found for base class id '" + base_class_id + "'" );

         field_list( cci->second.name, outs );
      }
   }

   field_index_const_iterator fici;
   for( fici = ( cci->second ).fields_index.begin( ); fici != ( cci->second ).fields_index.end( ); ++fici )
   {
      field_const_iterator fci = ( cci->second ).fields.find( fici->second );
      if( fci == ( cci->second ).fields.end( ) )
         throw runtime_error(
          "unexpected field_info entry not found for field '" + fici->first + "' (id = " + fici->second + "'" );

      string type( ( fci->second ).type );

      if( type == c_relationship_type )
      {
         string class_id( ( fci->second ).class_id );
         class_const_iterator cci = classes.find( class_id );
         if( cci == classes.end( ) )
            throw runtime_error( "unexpected class_info entry not found for class id '" + class_id + "'" );

         type = name;
         type += '_';
         type += ( cci->second ).name;
      }

      string opt_or_mand;
      if( ( fci->second ).is_mandatory )
         opt_or_mand = c_mandatory;
      else
         opt_or_mand = c_optional;

      // NOTE: A udt can either be locally defined or from an externally referenced module.
      vector< user_defined_type_data > all_udt_data;
      get_user_defined_type_data( all_udt_data, true );

      for( size_t i = 0; i < all_udt_data.size( ); i++ )
      {
         if( type == all_udt_data[ i ].id )
         {
            type = all_udt_data[ i ].name;
            break;
         }
      }

      outs << fici->first << " (id = " << fici->second << ") type: " << type << ' ' << opt_or_mand;

      if( ( fci->second ).is_transient )
         outs << " {transient}";

      if( ( fci->second ).for_text_search )
         outs << " {text_search}";

      if( !( fci->second ).default_value.empty( ) )
         outs << " (default: " << ( fci->second ).default_value << ")";

      if( !( fci->second ).extra.empty( ) )
         outs << " (extra: " << ( fci->second ).extra << ")";

      string model_info;
      if( p_parent_model )
         model_info = name + ":";

      outs << " [" << model_info << class_name << "]\n";
   }
}

void model::impl::field_remove( const string& class_name, const string& field_name, bool internal )
{
   class_iterator ci = fetch_class_info( class_name );

   field_index_iterator fii;
   field_iterator fi = fetch_field_info( ci, field_name, &fii );

   if( !internal && !( fi->second ).class_id.empty( ) )
      throw runtime_error( "cannot remove internally owned field '" + field_name + "'" );

   if( !( fi->second ).index_details.empty( ) )
      throw runtime_error( "cannot remove field whilst being used by one or more indexes" );

   if( !( fi->second ).specification_details.empty( ) )
      throw runtime_error( "cannot remove field whilst being used by one or more specifications" );

   ( ci->second ).fields.erase( fi );
   ( ci->second ).fields_index.erase( fii );
}

void model::impl::field_rename( const string& class_name,
 const string& old_field_name, const string& new_field_name, bool internal )
{
   class_iterator ci = fetch_class_info( class_name );

   field_index_iterator fii;
   field_iterator fi = fetch_field_info( ci, old_field_name, &fii );

   if( !internal && !( fi->second ).class_id.empty( ) )
      throw runtime_error( "cannot rename internally owned field '" + old_field_name + "'" );

   verify_base_class_does_not_have_field( ci, new_field_name );
   verify_derived_class_does_not_have_field( ci->first, new_field_name );

   model::impl old_model( *this );
   try
   {
      for( index_detail_iterator idi = ( fi->second ).index_details.begin( ); idi != ( fi->second ).index_details.end( ); ++idi )
      {
         index_iterator ii = ( ci->second ).indexes.find( idi->first );
         if( ii == ( ci->second ).indexes.end( ) )
            throw runtime_error( "unexpected index_info not found for index id " + idi->first );

         string old_name( "[" + old_field_name + "]" );
         string::size_type pos = ( ii->second ).name.find( old_name );
         if( pos == string::npos )
            throw runtime_error( "unexpected '" + old_name + "' not found in index name '" + ( ii->second ).name + "'" );

         string new_name( "[" + new_field_name + "]" );
         ( ii->second ).name.erase( pos, old_name.length( ) );
         ( ii->second ).name.insert( pos, new_name );
      }

      ( fi->second ).name = new_field_name;
      ( ci->second ).fields_index.erase( fii );
      ( ci->second ).fields_index.insert( field_index_value_type( new_field_name, fi->first ) );
   }
   catch( ... )
   {
      *this = old_model;
      throw;
   }
}

void model::impl::index_add( const string& index_id, const string& class_name, const string& field_list, bool is_unique )
{
   class_iterator ci = fetch_class_info( class_name );

   if( field_list.empty( ) )
      throw runtime_error( "index field list must not be empty" );

   if( ( ci->second ).is_external_alias( ) )
      throw runtime_error( "cannot add index to externally aliased class '" + class_name + "'" );

   if( ( ci->second ).indexes.count( index_id ) )
      throw runtime_error( "index id " + index_id + " is already in use for class '" + class_name + "'" );

   vector< string > fields;
   split( field_list, fields );

   set< string > unique_fields;
   for( size_t i = 0; i < fields.size( ); i++ )
      unique_fields.insert( fields[ i ] );

   if( fields.size( ) != unique_fields.size( ) )
      throw runtime_error( "fields cannot be repeated within an index" );

   string index_name;

   model::impl old_model( *this );
   try
   {
      for( size_t i = 0; i < fields.size( ); i++ )
      {
         if( !index_name.empty( ) )
            index_name += ',';

         index_name += '[';
         index_name += fields[ i ];
         index_name += ']';

         field_iterator fi = fetch_field_info( ci, fields[ i ] );
         ( fi->second ).index_details.insert( index_detail_value_type( index_id, i ) );
      }

      index_index_iterator iii = ( ci->second ).indexes_index.find( index_name );
      if( iii != ( ci->second ).indexes_index.end( ) )
         throw runtime_error( "an index for the fields '" + field_list + "' in class '" + class_name + "' already exists" );

      ( ci->second ).indexes_index.insert( index_index_value_type( index_name, index_id ) );
      ( ci->second ).indexes.insert( index_value_type( index_id, index_info( index_name, is_unique ) ) );
   }
   catch( ... )
   {
      *this = old_model;
      throw;
   }
}

void model::impl::index_list( const string& class_name, ostream& outs ) const
{
   class_const_iterator cci = fetch_class_info( class_name );

   for( index_index_const_iterator iici = ( cci->second ).indexes_index.begin( ); iici != ( cci->second ).indexes_index.end( ); ++iici )
   {
      index_const_iterator ici = ( cci->second ).indexes.find( iici->second );
      if( ici == ( cci->second ).indexes.end( ) )
         throw runtime_error( "unexpected index_info entry not found for index '" + iici->first + "' (id = " + iici->second + ")" );

      outs << get_undecorated_index_name( class_name, ( ici->second ).name, false ) << " (id = " << ici->first << ")";
      if( ( ici->second ).is_unique )
         outs << " unique";
      outs << '\n';
   }
}

void model::impl::index_remove( const string& class_name, const string& field_list )
{
   class_iterator ci = fetch_class_info( class_name );

   vector< string > fields;
   split( field_list, fields );

   string index_name;
   for( size_t i = 0; i < fields.size( ); i++ )
   {
      if( !index_name.empty( ) )
         index_name += ',';

      index_name += '[';
      index_name += fields[ i ];
      index_name += ']';
   }

   index_index_iterator iii = ( ci->second ).indexes_index.find( index_name );
   if( iii == ( ci->second ).indexes_index.end( ) )
      throw runtime_error( "an index for the fields '" + field_list + "' does not exist" );

   index_iterator ii = ( ci->second ).indexes.find( iii->second );
   if( ii == ( ci->second ).indexes.end( ) )
      throw runtime_error( "unexpected index_info not found for index '" + index_name + "' (id = " + iii->second + ")" );

   model::impl old_model( *this );
   try
   {
      for( size_t i = 0; i < fields.size( ); i++ )
      {
         field_iterator fi = fetch_field_info( ci, fields[ i ] );
         ( fi->second ).index_details.erase( iii->second );
      }

      ( ci->second ).indexes.erase( ii );
      ( ci->second ).indexes_index.erase( iii );
   }
   catch( ... )
   {
      *this = old_model;
      throw;
   }
}

void model::impl::modifier_add( const string& modifier_id, const string& class_name, const string& modifier_name, const string& field_info )
{
   class_iterator ci = fetch_class_info( class_name );

   if( field_info.empty( ) )
      throw runtime_error( "modifier field info must not be empty" );

   if( ( ci->second ).is_external_alias( ) )
      throw runtime_error( "cannot add modifier to externally aliased class '" + class_name + "'" );

   if( ( ci->second ).modifiers.count( modifier_id ) )
      throw runtime_error( "modifier id " + modifier_id + " is already in use for class '" + class_name + "'" );

   vector< string > fields;
   split( field_info, fields );

   string class_affect;
   vector< pair< string, string > > field_affects;
   for( size_t i = 0; i < fields.size( ); i++ )
   {
      string::size_type pos = fields[ i ].find( ':' );
      if( pos == string::npos )
         throw runtime_error( "unexpected incorrect format '" + fields[ i ] + "' for field:affect info" );

      string field_name( fields[ i ].substr( 0, pos ) );
      string modifier_affect( fields[ i ].substr( pos + 1 ) );

      pos = modifier_affect.find( ';' );
      string affect_type( modifier_affect.substr( 0, pos ) );

      string affect_extra;
      if( pos != string::npos )
         affect_extra = modifier_affect.substr( pos + 1 );

      if( affect_type != "protect" && affect_type != "relegate"
       && affect_type != "lowlight" && affect_type != "lowlight1"
       && affect_type != "highlight" && affect_type != "highlight1"
       && affect_type != "extralight" && affect_type != "extralight1" )
         throw runtime_error( "unexpected modifier affect type '" + affect_type + "'" );

      if( !affect_extra.empty( ) && affect_extra != "list" && affect_extra != "view" )
         throw runtime_error( "unexpected modifier affect extra '" + affect_extra + "'" );

      if( field_name == "@class" )
         class_affect = modifier_affect;
      else
         field_affects.push_back( make_pair( field_name, modifier_affect ) );
   }

   model::impl old_model( *this );
   try
   {
      for( size_t i = 0; i < field_affects.size( ); i++ )
      {
         field_iterator fi = fetch_field_info( ci, field_affects[ i ].first );
         ( fi->second ).modifier_details.insert( modifier_detail_value_type( modifier_id, field_affects[ i ].second ) );
      }

      if( !class_affect.empty( ) )
         ( ci->second ).modifier_details.insert( modifier_detail_value_type( modifier_id, class_affect ) );

      modifier_index_iterator mii = ( ci->second ).modifiers_index.find( modifier_name );
      if( mii != ( ci->second ).modifiers_index.end( ) )
         throw runtime_error( "modifier '" + modifier_name + "' in class '" + class_name + "' already exists" );

      ( ci->second ).modifiers_index.insert( modifier_index_value_type( modifier_name, modifier_id ) );
      ( ci->second ).modifiers.insert( modifier_value_type( modifier_id, modifier_info( modifier_name ) ) );
   }
   catch( ... )
   {
      *this = old_model;
      throw;
   }
}

void model::impl::modifier_list( const string& class_name, ostream& outs ) const
{
   class_const_iterator cci = fetch_class_info( class_name );

   for( modifier_index_const_iterator mici = ( cci->second ).modifiers_index.begin( ); mici != ( cci->second ).modifiers_index.end( ); ++mici )
   {
      outs << mici->first;
      for( field_const_iterator fci = ( cci->second ).fields.begin( ); fci != ( cci->second ).fields.end( ); ++fci )
      {
         if( ( fci->second ).modifier_details.count( mici->second ) )
            outs << ' ' << ( fci->second ).name << '=' << ( fci->second ).modifier_details.find( mici->second )->second;
      }
      outs << '\n';
   }
}

void model::impl::modifier_remove( const string& class_name, const string& modifier_name )
{
   class_iterator ci = fetch_class_info( class_name );

   modifier_index_iterator mii = ( ci->second ).modifiers_index.find( modifier_name );
   if( mii == ( ci->second ).modifiers_index.end( ) )
      throw runtime_error( "modifier '" + modifier_name + "' does not exist for class '" + class_name + "'" );

   modifier_iterator mi = ( ci->second ).modifiers.find( mii->second );
   if( mi == ( ci->second ).modifiers.end( ) )
      throw runtime_error( "unexpected modifier_info not found for modifier '" + modifier_name + "' (id = " + mii->second + ")" );

   model::impl old_model( *this );
   try
   {
      for( field_iterator fi = ( ci->second ).fields.begin( ); fi != ( ci->second ).fields.end( ); ++fi )
      {
         if( ( fi->second ).modifier_details.count( mii->second ) )
            ( fi->second ).modifier_details.erase( mii->second );
      }

      ( ci->second ).modifiers.erase( mi );
      ( ci->second ).modifiers_index.erase( mii );
   }
   catch( ... )
   {
      *this = old_model;
      throw;
   }
}

void model::impl::modifier_rename( const string& class_name, const string& old_modifier_name, const string& new_modifier_name )
{
   class_iterator ci = fetch_class_info( class_name );

   modifier_index_iterator mii = ( ci->second ).modifiers_index.find( old_modifier_name );
   if( mii == ( ci->second ).modifiers_index.end( ) )
      throw runtime_error( "modifier '" + old_modifier_name + "' does not exist for class '" + class_name + "'" );

   modifier_iterator mi = ( ci->second ).modifiers.find( mii->second );
   if( mi == ( ci->second ).modifiers.end( ) )
      throw runtime_error( "unexpected modifier_info not found for modifier '" + old_modifier_name + "' (id = " + mii->second + ")" );

   ( mi->second ).name = new_modifier_name;
   ( ci->second ).modifiers_index.erase( mii );
   ( ci->second ).modifiers_index.insert( modifier_index_value_type( new_modifier_name, mi->first ) );
}

void model::impl::procedure_add( const string& procedure_id, const string& class_name, const string& procedure_name )
{
   class_iterator ci = fetch_class_info( class_name );

   if( ( ci->second ).is_external_alias( ) )
      throw runtime_error( "cannot add procedure to externally aliased class '" + class_name + "'" );

   if( ( ci->second ).procedures_index.find( procedure_name ) != ( ci->second ).procedures_index.end( ) )
      throw runtime_error( "procedure '" + procedure_name + "' already exists for class '" + class_name + "'" );

   if( ( ci->second ).procedures.count( procedure_id ) )
      throw runtime_error( "procedure id " + procedure_id + " is already in use for class '" + class_name + "'" );

   pair< procedure_index_iterator, bool > ret
    = ( ci->second ).procedures_index.insert( procedure_index_value_type( procedure_name, procedure_id ) );
   try
   {
      ( ci->second ).procedures.insert( procedure_value_type( procedure_id, procedure_info( procedure_name ) ) );
   }
   catch( ... )
   {
      ( ci->second ).procedures_index.erase( ret.first );
      throw;
   }
}

void model::impl::procedure_list( const string& class_name, ostream& outs ) const
{
   class_const_iterator cci = fetch_class_info( class_name );

   string base_class_id( ( cci->second ).base_class_id );

   if( !base_class_id.empty( ) )
   {
      string::size_type pos = base_class_id.find( ':' );
      if( pos != string::npos )
      {
         string model_name = base_class_id.substr( 0, pos );
         external_reference_const_iterator erci = external_references.find( model_name );

         if( erci == external_references.end( ) )
            throw runtime_error( "unexpected external reference entry not found for '" + model_name + "'" );

         class_const_iterator cci = erci->second->p_impl->classes.find( base_class_id.substr( pos + 1 ) );
         if( cci == erci->second->p_impl->classes.end( ) )
            throw runtime_error( "unexpected class_info entry not found for class id '"
             + base_class_id.substr( pos + 1 ) + "' in externally referenced model '" + model_name + "'" );

         erci->second->p_impl->procedure_list( cci->second.name, outs );
      }
      else
      {
         class_const_iterator cci = classes.find( base_class_id );
         if( cci == classes.end( ) )
            throw runtime_error( "unexpected class_info entry not found for base class id '" + base_class_id + "'" );

         procedure_list( cci->second.name, outs );
      }
   }

   for( procedure_index_const_iterator
    pici = ( cci->second ).procedures_index.begin( ), end = ( cci->second ).procedures_index.end( ); pici != end; ++pici )
   {
      procedure_const_iterator pci = ( cci->second ).procedures.find( pici->second );
      if( pci == ( cci->second ).procedures.end( ) )
         throw runtime_error(
          "unexpected procedure_info entry not found for procedure '" + pici->first + "' (id = " + pici->second + "'" );

      string model_info;
      if( p_parent_model )
         model_info = name + ":";

      outs << pici->first << " (id = " << pici->second << ") [" << model_info << class_name << "]\n";
   }
}

void model::impl::procedure_remove( const string& class_name, const string& procedure_name )
{
   class_iterator ci = fetch_class_info( class_name );

   procedure_index_iterator pii;
   procedure_iterator pi = fetch_procedure_info( class_name, procedure_name, &pii );

   if( !( pi->second ).specification_details.empty( ) )
      throw runtime_error( "cannot remove procedure whilst being used by one or more specifications" );

   ( ci->second ).procedures.erase( pi );
   ( ci->second ).procedures_index.erase( pii );
}

void model::impl::procedure_rename( const string& class_name, const string& old_procedure_name, const string& new_procedure_name )
{
   class_iterator ci = fetch_class_info( class_name );

   procedure_index_iterator pii;
   procedure_iterator pi = fetch_procedure_info( class_name, old_procedure_name, &pii );

   ( pi->second ).name = new_procedure_name;
   ( ci->second ).procedures_index.erase( pii );
   ( ci->second ).procedures_index.insert( procedure_index_value_type( new_procedure_name, pi->first ) );
}

void model::impl::procedure_arg_add( const string& arg_id, const string& class_name,
 const string& procedure_name, const string& arg_name, const string& arg_type, bool is_output )
{
   procedure_iterator pi = fetch_procedure_info( class_name, procedure_name );

   if( ( pi->second ).args_index.find( arg_name ) != ( pi->second ).args_index.end( ) )
      throw runtime_error( "arg '" + arg_name
       + "' already exists for procedure '" + procedure_name + "' in class '" + class_name + "'" );

   if( ( pi->second ).args.count( arg_id ) )
      throw runtime_error( "arg id " + arg_id + " is already in use for procedure '" + procedure_name + "'" );

   pair< arg_index_iterator, bool > ret
    = ( pi->second ).args_index.insert( arg_index_value_type( arg_name, arg_id ) );
   try
   {
      ( pi->second ).args.insert( arg_value_type( arg_id, arg_info( arg_name, arg_type, is_output ) ) );
   }
   catch( ... )
   {
      ( pi->second ).args_index.erase( ret.first );
      throw;
   }
}

void model::impl::procedure_arg_list( const string& class_name, const string& procedure_name, ostream& outs ) const
{
   procedure_const_iterator pci = fetch_procedure_info( class_name, procedure_name );

   arg_index_const_iterator aici;
   for( aici = ( pci->second ).args_index.begin( ); aici != ( pci->second ).args_index.end( ); ++aici )
   {
      arg_const_iterator aci = ( pci->second ).args.find( aici->second );
      if( aci == ( pci->second ).args.end( ) )
         throw runtime_error( "unexpected arg_info entry not found for arg '" + aici->first + "' (id = " + aici->second + "'" );

      outs << aici->first << " (id = " << aici->second << ") type: "
       << ( aci->second ).type << " (" << ( ( aci->second ).is_output ? c_output : c_input ) << ")\n";
   }
}

void model::impl::procedure_arg_remove( const string& class_name, const string& procedure_name, const string& arg_name )
{
   procedure_iterator pi = fetch_procedure_info( class_name, procedure_name );

   arg_index_iterator aii = ( pi->second ).args_index.find( arg_name );
   if( aii == ( pi->second ).args_index.end( ) )
      throw runtime_error( "unknown arg '" + arg_name + "' for procedure '" + procedure_name + "' in class '" + class_name + "'" );

   string id( aii->second );
   arg_iterator ai = ( pi->second ).args.find( id );
   if( ai == ( pi->second ).args.end( ) )
      throw runtime_error( "unexpected arg_info entry not found for arg '" + arg_name + "' (id = " + id + ")" );

   if( !( ai->second ).specification_details.empty( ) )
      throw runtime_error( "cannot remove procedure arg whilst being used by one or more specifications" );

   ( pi->second ).args.erase( ai );
   ( pi->second ).args_index.erase( aii );
}

void model::impl::procedure_arg_rename( const string& class_name,
 const string& procedure_name, const string& old_arg_name, const string& new_arg_name )
{
   procedure_iterator pi = fetch_procedure_info( class_name, procedure_name );

   arg_index_iterator aii = ( pi->second ).args_index.find( old_arg_name );
   if( aii == ( pi->second ).args_index.end( ) )
      throw runtime_error( "unknown arg '"
       + old_arg_name + "' for procedure '" + procedure_name + "' in class '" + class_name + "'" );

   string id( aii->second );
   arg_iterator ai = ( pi->second ).args.find( id );
   if( ai == ( pi->second ).args.end( ) )
      throw runtime_error( "unexpected arg_info entry not found for arg '" + old_arg_name + "' (id = " + id + ")" );

   ( ai->second ).name = new_arg_name;
   ( pi->second ).args_index.erase( aii );
   ( pi->second ).args_index.insert( arg_index_value_type( new_arg_name, ai->first ) );
}

void model::impl::relationship_add( const string& relationship_id,
 const string& field_id, const string& child_class_name, const string& child_field_name,
 const string& parent_class_name, bool is_mandatory, bool is_transient, cascade_operation cascade_op, const string& extra )
{
   string relationship_name( child_field_name );

   if( child_class_name != parent_class_name )
   {
      relationship_name += '_';
      relationship_name += '{';
      relationship_name += child_class_name;
      relationship_name += '}';
   }

   relationship_index_const_iterator rici = relationships_index.find( relationship_name );
   if( rici != relationships_index.end( ) )
      throw runtime_error( "relationship '" + relationship_name + "' already exists" );

   if( relationships.count( relationship_id ) )
      throw runtime_error( "relationship id " + relationship_id + " is already in use" );

   class_index_const_iterator cici = classes_index.find( parent_class_name );
   if( cici == classes_index.end( ) )
      throw runtime_error( "unknown parent class '" + parent_class_name + "'" );

   string parent_id( cici->second );

   cici = classes_index.find( child_class_name );
   if( cici == classes_index.end( ) )
      throw runtime_error( "unknown child class '" + child_class_name + "'" );

   string child_id( cici->second );

   pair< relationship_index_iterator, bool > ret
    = relationships_index.insert( relationship_index_value_type( relationship_name, relationship_id ) );
   try
   {
      relationships.insert( relationship_value_type( relationship_id,
       relationship_info( relationship_name, parent_id, child_id, cascade_op ) ) );
   }
   catch( ... )
   {
      relationships_index.erase( ret.first );
      throw;
   }

   field_add( field_id, child_class_name, child_field_name, c_relationship_type,
    is_mandatory, is_transient, false, "", "", extra, "", parent_id, relationship_id );
}

void model::impl::relationship_list( ostream& outs ) const
{
   relationship_index_const_iterator rici;
   for( rici = relationships_index.begin( ); rici != relationships_index.end( ); ++rici )
   {
      string name( rici->first );

      class_const_iterator child_cci, parent_cci;
      relationship_const_iterator rci = fetch_relationship_info( rici->second, &child_cci, &parent_cci );

      string child_name( ( child_cci->second ).name );
      string parent_name( ( parent_cci->second ).name );

      field_const_iterator fci = fetch_field_info( child_cci, rci );

      outs << name << " (id = " << rici->second
       << ") parent: " << parent_name << ", child: " << child_name
       << " (" << ( ( fci->second ).is_mandatory ? c_mandatory : c_optional )
       << ") (" << get_cascade_op_name( ( rci->second ).cascade_op ) << ")\n";
   }
}

void model::impl::relationship_list_parents( const string& child_class_name, ostream& outs ) const
{
   class_index_const_iterator cici = classes_index.find( child_class_name );
   if( cici == classes_index.end( ) )
      throw runtime_error( "unknown class '" + child_class_name + "'" );

   string child_id( cici->second );

   relationship_index_const_iterator rici;
   for( rici = relationships_index.begin( ); rici != relationships_index.end( ); ++rici )
   {
      relationship_const_iterator rci = relationships.find( rici->second );
      if( rci == relationships.end( ) )
         throw runtime_error( "unexpected relationship_info entry not found for relationship '"
          + rici->first + "' (id = " + rici->second + ")" );

      if( ( rci->second ).child_id == child_id )
      {
         class_const_iterator child_cci, parent_cci;
         fetch_relationship_info( rici->second, &child_cci, &parent_cci );

         string child_name( ( child_cci->second ).name );
         string parent_name( ( parent_cci->second ).name );

         field_const_iterator fci = fetch_field_info( child_cci, rci );

         outs << rici->first << " (id = " << rici->second
          << ") parent: " << parent_name << ", child: " << child_name
          << " (" << ( ( fci->second ).is_mandatory ? c_mandatory : c_optional )
          << ") (" << get_cascade_op_name( ( rci->second ).cascade_op ) << ")\n";
      }
   }
}

void model::impl::relationship_list_children( const string& parent_class_name, ostream& outs ) const
{
   class_index_const_iterator cici = classes_index.find( parent_class_name );
   if( cici == classes_index.end( ) )
      throw runtime_error( "unknown class '" + parent_class_name + "'" );

   string parent_id( cici->second );

   relationship_index_const_iterator rici;
   for( rici = relationships_index.begin( ); rici != relationships_index.end( ); ++rici )
   {
      relationship_const_iterator rci = relationships.find( rici->second );
      if( rci == relationships.end( ) )
         throw runtime_error(
          "unexpected relationship_info entry not found for relationship '" + rici->first + "' (id = " + rici->second + ")" );

      if( ( rci->second ).parent_id == parent_id )
      {
         class_const_iterator child_cci, parent_cci;
         fetch_relationship_info( rici->second, &child_cci, &parent_cci );

         string child_name( ( child_cci->second ).name );
         string parent_name( ( parent_cci->second ).name );

         field_const_iterator fci = fetch_field_info( child_cci, rci );

         outs << rici->first << " (id = " << rici->second
          << ") parent: " << parent_name << ", child: " << child_name
          << " (" << ( ( fci->second ).is_mandatory ? c_mandatory : c_optional )
          << ") (" << get_cascade_op_name( ( rci->second ).cascade_op ) << ")\n";
      }
   }
}

void model::impl::relationship_remove( const string& relationship_name )
{
   relationship_index_iterator rii;
   relationship_iterator ri = fetch_relationship_info( relationship_name, &rii );

   if( !( ri->second ).specification_details.empty( ) )
      throw runtime_error( "cannot remove relationship whilst being used by one or more specifications" );

   class_const_iterator cci = classes.find( ( ri->second ).child_id );
   if( cci == classes.end( ) )
      throw runtime_error( "unable to find class_info for child class id '" + ( ri->second ).child_id + "'" );

   string relationship_id( ri->first );

   string::size_type rel_without_child_size( relationship_name.length( ) - ( cci->second ).name.length( ) - 1 );

   field_remove( ( cci->second ).name, relationship_name.substr( 0, rel_without_child_size ), true );

   relationships.erase( ri );
   relationships_index.erase( rii );
}

void model::impl::relationship_rename( const string& old_relationship_name, const string& new_relationship_name )
{
   relationship_index_iterator rii;
   relationship_iterator ri = fetch_relationship_info( old_relationship_name, &rii );

   class_const_iterator cci = classes.find( ( ri->second ).child_id );
   if( cci == classes.end( ) )
      throw runtime_error( "unable to find class_info for child class id '" + ( ri->second ).child_id + "'" );

   string relationship_id( ri->first );

   string new_suffix( "_" );
   new_suffix += '{';
   new_suffix += ( cci->second ).name;
   new_suffix += '}';

   string::size_type pos = old_relationship_name.find_last_of( "{" );
   string old_suffix( old_relationship_name.substr( pos == 0 ? 0 : pos - 1 ) );

   if( new_relationship_name.length( ) > new_suffix.length( )
    && new_relationship_name.substr( new_relationship_name.length( ) - new_suffix.length( ) ) != new_suffix )
      throw runtime_error( "new relationship name must end with the suffix '" + new_suffix + "'" );

   string::size_type new_rel_without_child_size;
   string::size_type old_rel_without_child_size;

   if( new_relationship_name.length( ) < new_suffix.length( ) )
      new_rel_without_child_size = string::npos;
   else
      new_rel_without_child_size = new_relationship_name.length( ) - new_suffix.length( );

   // NOTE: The "old suffix" may have actually been the whole name so use <= here.
   if( old_relationship_name.length( ) <= old_suffix.length( ) )
      old_rel_without_child_size = string::npos;
   else
      old_rel_without_child_size = old_relationship_name.length( ) - old_suffix.length( );

   field_rename( ( cci->second ).name,
    old_relationship_name.substr( 0, old_rel_without_child_size ),
    new_relationship_name.substr( 0, new_rel_without_child_size ), true );

   ( ri->second ).name = new_relationship_name;
   relationships_index.erase( rii );
   relationships_index.insert( relationship_index_value_type( new_relationship_name, ri->first ) );
}

void model::impl::specification_add( const string& specification_id,
 const string& specification_name, const string& specification_type, const string& specification_args )
{
   if( specifications_index.find( specification_name ) != specifications_index.end( ) )
      throw runtime_error( "specification '" + specification_name + "' already exists" );

   if( specifications.count( specification_id ) )
      throw runtime_error( "specification id " + specification_id + " is already in use" );

   model::impl old_model( *this );
   try
   {
      specification_info spec_info;
      spec_info.name = specification_name;
      spec_info.type = specification_type;
      spec_info.rp_spec = get_specification_factory( ).construct( specification_type );

      if( !spec_info.rp_spec )
         throw runtime_error( "unknown specification type '" + specification_type + "'" );

      vector< string > args;
      setup_arguments( specification_args, args );

      vector< specification_detail > details;
      spec_info.rp_spec->add( *p_model, args, details );

      for( size_t i = 0; i < details.size( ); i++ )
      {
         if( details[ i ].element_type == e_model_element_type_class )
         {
            class_iterator ci = classes.find( details[ i ].id );
            if( ci == classes.end( ) )
               throw runtime_error( "unexpected class_info entry not found for class id '" + details[ i ].id + "'" );

            ( ci->second ).specification_details.insert( specification_detail_value_type( specification_id, details[ i ].item ) );
         }
         else if( details[ i ].element_type == e_model_element_type_field )
         {
            bool found = false;
            for( class_iterator ci = classes.begin( ), end = classes.end( ); ci != end; ++ci )
            {
               field_iterator fi = ( ci->second ).fields.find( details[ i ].id );
               if( fi != ( ci->second ).fields.end( ) )
               {
                  ( fi->second ).specification_details.insert(
                   specification_detail_value_type( specification_id, details[ i ].item ) );

                  found = true;
                  break;
               }
            }

            if( !found )
               throw runtime_error( "unexpected field_info entry not found for field id '" + details[ i ].id + "'" );
         }
         else if( details[ i ].element_type == e_model_element_type_procedure )
         {
            bool found = false;
            for( class_iterator ci = classes.begin( ), end = classes.end( ); ci != end; ++ci )
            {
               procedure_iterator pi = ( ci->second ).procedures.find( details[ i ].id );
               if( pi != ( ci->second ).procedures.end( ) )
               {
                  ( pi->second ).specification_details.insert(
                   specification_detail_value_type( specification_id, details[ i ].item ) );

                  found = true;
                  break;
               }
            }

            if( !found )
               throw runtime_error( "unexpected procedure_info entry not found for procedure id '" + details[ i ].id + "'" );
         }
         else if( details[ i ].element_type == e_model_element_type_procedure_arg )
         {
            bool found = false;
            for( class_iterator ci = classes.begin( ), end = classes.end( ); ci != end; ++ci )
            {
               for( procedure_iterator pi = ( ci->second ).procedures.begin( ), end = ( ci->second ).procedures.end( ); pi != end; ++pi )
               {
                  arg_iterator ai = ( pi->second ).args.find( details[ i ].id );
                  if( ai != ( pi->second ).args.end( ) )
                  {
                     ( ai->second ).specification_details.insert(
                      specification_detail_value_type( specification_id, details[ i ].item ) );

                     found = true;
                     break;
                  }
               }

               if( found )
                  break;
            }

            if( !found )
               throw runtime_error( "unexpected arg_info entry not found for procedure arg id '" + details[ i ].id + "'" );
         }
         else if( details[ i ].element_type == e_model_element_type_relationship )
         {
            relationship_iterator ri = relationships.find( details[ i ].id );
            if( ri == relationships.end( ) )
               throw runtime_error( "unexpected relationship_info entry not found for relationship id '" + details[ i ].id + "'" );

            ( ri->second ).specification_details.insert( specification_detail_value_type( specification_id, details[ i ].item ) );
         }
         else
            throw runtime_error( "specification element type #" + to_string( details[ i ].element_type ) + " not yet supported" );
      }

      specifications.insert( specification_value_type( specification_id, spec_info ) );
      specifications_index.insert( specification_index_value_type( specification_name, specification_id ) );
   }
   catch( ... )
   {
      *this = old_model;
      throw;
   }
}

void model::impl::specification_list( ostream& outs ) const
{
   specification_index_const_iterator sici;
   for( sici = specifications_index.begin( ); sici != specifications_index.end( ); ++sici )
   {
      string name( sici->first );

      specification_const_iterator sci = specifications.find( sici->second );
      if( sci == specifications.end( ) )
         throw runtime_error( "unexpected specification_info entry not found for specification id '" + sici->second + "'" );

      string type( ( sci->second ).type );

      outs << name << " (id = " << sici->second << ") type: " << type << "\n";
   }
}

void model::impl::specification_remove( const string& specification_name )
{
   specification_index_iterator sii = specifications_index.find( specification_name );
   if( sii == specifications_index.end( ) )
      throw runtime_error( "unknown specification '" + specification_name + "'" );

   specification_iterator si = specifications.find( sii->second );
   if( si == specifications.end( ) )
      throw runtime_error(
       "unexpected specification_info entry not found for specification '" + specification_name + "' (id = " + sii->second + ")" );

   for( class_iterator ci = classes.begin( ), end = classes.end( ); ci != end; ++ci )
   {
      if( ( ci->second ).specification_details.find( si->first ) != ( ci->second ).specification_details.end( ) )
         ( ci->second ).specification_details.erase( si->first );

      for( field_iterator fi = ( ci->second ).fields.begin( ), end = ( ci->second ).fields.end( ); fi != end; ++fi )
      {
         if( ( fi->second ).specification_details.find( si->first ) != ( fi->second ).specification_details.end( ) )
            ( fi->second ).specification_details.erase( si->first );
      }

      for( procedure_iterator pi = ( ci->second ).procedures.begin( ), end = ( ci->second ).procedures.end( ); pi != end; ++pi )
      {
         if( ( pi->second ).specification_details.find( si->first ) != ( pi->second ).specification_details.end( ) )
            ( pi->second ).specification_details.erase( si->first );

         for( arg_iterator ai = ( pi->second ).args.begin( ), end = ( pi->second ).args.end( ); ai != end; ++ai )
         {
            if( ( ai->second ).specification_details.find( si->first ) != ( ai->second ).specification_details.end( ) )
               ( ai->second ).specification_details.erase( si->first );
         }
      }
   }

   for( relationship_iterator ri = relationships.begin( ), end = relationships.end( ); ri != end; ++ri )
   {
      if( ( ri->second ).specification_details.find( si->first ) != ( ri->second ).specification_details.end( ) )
         ( ri->second ).specification_details.erase( si->first );
   }

   specifications.erase( si );
   specifications_index.erase( sii );
}

void model::impl::specification_rename( const string& old_specification_name, const string& new_specification_name )
{
   specification_index_iterator sii = specifications_index.find( old_specification_name );
   if( sii == specifications_index.end( ) )
      throw runtime_error( "unknown specification '" + old_specification_name + "'" );

   specification_index_const_iterator sici = specifications_index.find( new_specification_name );
   if( sici != specifications_index.end( ) )
      throw runtime_error( "specification name '" + new_specification_name + "' is already in use" );

   string id( sii->second );
   specification_iterator si = specifications.find( id );
   if( si == specifications.end( ) )
      throw runtime_error(
       "unexpected specification_info entry not found for specification '" + old_specification_name + "' (id = " + id + ")" );

   ( si->second ).name = new_specification_name;
   specifications_index.erase( sii );
   specifications_index.insert( specification_index_value_type( new_specification_name, si->first ) );
}

void model::impl::user_defined_enum_add( const string& ude_id,
 const string& ude_name, const string& type_name, const string& key_values )
{
   if( user_defined_enums_index.find( ude_name ) != user_defined_enums_index.end( ) )
      throw runtime_error( "user defined enum '" + ude_name + "' already exists" );

   if( user_defined_enums.count( ude_id ) )
      throw runtime_error( "user defined enum id " + ude_id + " is already in use" );

   model::impl old_model( *this );
   try
   {
      user_defined_enum_info udei( ude_name, type_name, key_values );

      user_defined_enums.insert( user_defined_enum_value_type( ude_id, udei ) );
      user_defined_enums_index.insert( user_defined_enum_index_value_type( ude_name, ude_id ) );
   }
   catch( ... )
   {
      *this = old_model;
      throw;
   }
}

void model::impl::user_defined_enum_list( ostream& outs ) const
{
   user_defined_enum_index_const_iterator uici;
   for( uici = user_defined_enums_index.begin( ); uici != user_defined_enums_index.end( ); ++uici )
   {
      string name( uici->first );

      user_defined_enum_const_iterator uci = user_defined_enums.find( uici->second );
      if( uci == user_defined_enums.end( ) )
         throw runtime_error( "unexpected user_defined_enum_info entry not found for user_defined_enum id '" + uici->second + "'" );

      string type( ( uci->second ).type );
      string key_values( ( uci->second ).key_values );

      outs << name << " " << type << " " << key_values << "\n";
   }
}

void model::impl::user_defined_enum_remove( const string& ude_name )
{
   user_defined_enum_index_iterator uii = user_defined_enums_index.find( ude_name );
   if( uii == user_defined_enums_index.end( ) )
      throw runtime_error( "unknown user defined enum '" + ude_name + "'" );

   user_defined_enum_iterator ui = user_defined_enums.find( uii->second );
   if( ui == user_defined_enums.end( ) )
      throw runtime_error(
       "unexpected user_defined_enum_info entry not found for user_defined_enum '" + ude_name + "' (id = " + uii->second + ")" );

   for( class_iterator ci = classes.begin( ), end = classes.end( ); ci != end; ++ci )
   {
      for( field_iterator fi = ( ci->second ).fields.begin( ), end = ( ci->second ).fields.end( ); fi != end; ++fi )
      {
         if( ( fi->second ).enum_id == ui->first )
            throw runtime_error( "cannot remove user defined enum '" + ude_name + "' as it is being used by one or more fields" );
      }
   }

   user_defined_enums.erase( ui );
   user_defined_enums_index.erase( uii );
}

void model::impl::user_defined_enum_rename( const string& old_ude_name, const string& new_ude_name )
{
   user_defined_enum_index_iterator uii = user_defined_enums_index.find( old_ude_name );
   if( uii == user_defined_enums_index.end( ) )
      throw runtime_error( "unknown user defined enum '" + old_ude_name + "'" );

   user_defined_enum_index_const_iterator uici = user_defined_enums_index.find( new_ude_name );
   if( uici != user_defined_enums_index.end( ) )
      throw runtime_error( "user defined enum name '" + new_ude_name + "' is already in use" );

   string id( uii->second );
   user_defined_enum_iterator ui = user_defined_enums.find( id );
   if( ui == user_defined_enums.end( ) )
      throw runtime_error(
       "unexpected user_defined_enum_info entry not found for user_defined_enum '" + old_ude_name + "' (id = " + id + ")" );

   ( ui->second ).name = new_ude_name;
   user_defined_enums_index.erase( uii );
   user_defined_enums_index.insert( user_defined_enum_index_value_type( new_ude_name, ui->first ) );
}

void model::impl::user_defined_type_add( const string& udt_id,
 const string& udt_name, const string& type_name, const string& type_domain_info )
{
   if( user_defined_types_index.find( udt_name ) != user_defined_types_index.end( ) )
      throw runtime_error( "user defined type '" + udt_name + "' already exists" );

   if( user_defined_types.count( udt_id ) )
      throw runtime_error( "user defined type id " + udt_id + " is already in use" );

   model::impl old_model( *this );
   try
   {
      vector< string > args;
      if( !type_domain_info.empty( ) )
         setup_arguments( type_domain_info, args );

      string domain_name;
      if( !args.empty( ) )
      {
         domain_name = args[ 0 ];
         args.erase( args.begin( ) );
      }

      string type( type_name );

      user_defined_type_index_const_iterator uici = user_defined_types_index.find( type );
      if( uici != user_defined_types_index.end( ) )
         type = uici->second;

      string sys_type( type );

      user_defined_type_const_iterator uci;
      while( ( uci = user_defined_types.find( sys_type ) ) != user_defined_types.end( ) )
         sys_type = ( uci->second ).type;

      user_defined_type_info udti( udt_name, type, sys_type, domain_name );

      if( !domain_name.empty( ) )
      {
         string type_prefixed_domain_name( sys_type );
         type_prefixed_domain_name += '_';
         type_prefixed_domain_name += domain_name;

         udti.rp_domain = get_domain_factory( ).construct( type_prefixed_domain_name );

         if( !udti.rp_domain )
            throw runtime_error( "unknown domain '" + domain_name + "' for type '" + sys_type + "'" );

         udti.rp_domain->add( args );
      }

      user_defined_types.insert( user_defined_type_value_type( udt_id, udti ) );
      user_defined_types_index.insert( user_defined_type_index_value_type( udt_name, udt_id ) );
   }
   catch( ... )
   {
      *this = old_model;
      throw;
   }
}

void model::impl::user_defined_type_list( ostream& outs ) const
{
   user_defined_type_index_const_iterator uici;
   for( uici = user_defined_types_index.begin( ); uici != user_defined_types_index.end( ); ++uici )
   {
      string name( uici->first );

      user_defined_type_const_iterator uci = user_defined_types.find( uici->second );
      if( uci == user_defined_types.end( ) )
         throw runtime_error( "unexpected user_defined_type_info entry not found for user_defined_type id '" + uici->second + "'" );

      string type( ( uci->second ).type );
      string domain_info;

      if( ( uci->second ).rp_domain )
         domain_info = ( uci->second ).rp_domain->get_list_display( );

      uci = user_defined_types.find( type );
      if( uci != user_defined_types.end( ) )
         type = ( uci->second ).name;

      outs << name << " " << type;

      if( !domain_info.empty( ) )
         outs << " (" << domain_info << ")";

      outs << "\n";
   }
}

void model::impl::user_defined_type_remove( const string& udt_name )
{
   user_defined_type_index_iterator uii = user_defined_types_index.find( udt_name );
   if( uii == user_defined_types_index.end( ) )
      throw runtime_error( "unknown user defined type '" + udt_name + "'" );

   user_defined_type_iterator ui = user_defined_types.find( uii->second );
   if( ui == user_defined_types.end( ) )
      throw runtime_error(
       "unexpected user_defined_type_info entry not found for user_defined_type '" + udt_name + "' (id = " + uii->second + ")" );

   for( user_defined_type_const_iterator uci = user_defined_types.begin( ), end = user_defined_types.end( ); uci != end; ++uci )
   {
      if( ui->first == ( uci->second ).type )
         throw runtime_error( "cannot remove user defined type '" + udt_name + "' as it is being used by another user defined type" );
   }

   for( class_iterator ci = classes.begin( ), end = classes.end( ); ci != end; ++ci )
   {
      for( field_iterator fi = ( ci->second ).fields.begin( ), end = ( ci->second ).fields.end( ); fi != end; ++fi )
      {
         if( ( fi->second ).type == ui->first )
            throw runtime_error( "cannot remove user defined type '" + udt_name + "' as it is being used by one or more fields" );
      }
   }

   user_defined_types.erase( ui );
   user_defined_types_index.erase( uii );
}

void model::impl::user_defined_type_rename( const string& old_udt_name, const string& new_udt_name )
{
   user_defined_type_index_iterator uii = user_defined_types_index.find( old_udt_name );
   if( uii == user_defined_types_index.end( ) )
      throw runtime_error( "unknown user defined type '" + old_udt_name + "'" );

   user_defined_type_index_const_iterator uici = user_defined_types_index.find( new_udt_name );
   if( uici != user_defined_types_index.end( ) )
      throw runtime_error( "user defined type name '" + new_udt_name + "' is already in use" );

   string id( uii->second );
   user_defined_type_iterator ui = user_defined_types.find( id );
   if( ui == user_defined_types.end( ) )
      throw runtime_error(
       "unexpected user_defined_type_info entry not found for user_defined_type '" + old_udt_name + "' (id = " + id + ")" );

   ( ui->second ).name = new_udt_name;
   user_defined_types_index.erase( uii );
   user_defined_types_index.insert( user_defined_type_index_value_type( new_udt_name, ui->first ) );
}

string model::impl::determine_child_rel_suffix( const string& relationship_name, string* p_rel_without_child )
{
   relationship_data rel_data;
   get_relationship_data( relationship_name, rel_data );

   string child_class_name( rel_data.child_class_name );
   string parent_class_name( rel_data.parent_class_name );

   string undecorated_name( get_undecorated_class_name( relationship_name ) );

   // NOTE: If the child entity name has been used as a suffix then remove it.
   string rel_without_child( undecorated_name );
   if( parent_class_name != child_class_name && undecorated_name.length( ) > child_class_name.length( ) )
   {
      size_t rel_without_child_size( undecorated_name.length( ) - child_class_name.length( ) - 1 );

      size_t pos = undecorated_name.rfind( child_class_name );
      if( pos == rel_without_child_size + 1 )
         rel_without_child = undecorated_name.substr( 0, rel_without_child_size );
   }

   string child_suffix( rel_without_child );

   if( p_rel_without_child )
      *p_rel_without_child = rel_without_child;

   // NOTE: If the parent class name is found immediately before the child class
   // name suffix then remove it to (possibly removing the entire name so far).
   if( rel_without_child.length( ) >= parent_class_name.length( )
    && rel_without_child.substr( rel_without_child.length( ) - parent_class_name.length( ) ) == parent_class_name )
   {
      if( rel_without_child.length( ) == parent_class_name.length( ) )
         child_suffix.erase( );
      else
         child_suffix
          = rel_without_child.substr( 0, rel_without_child.length( ) - parent_class_name.length( ) - 1 );
   }

   if( !child_suffix.empty( ) )
      child_suffix = child_class_name + '_' + child_suffix;
   else
      child_suffix = child_class_name;

   return child_suffix;
}

void model::impl::verify_base_class_does_not_have_field( class_const_iterator cci, const string& field_name, bool is_base ) const
{
   string base_class_id( ( cci->second ).base_class_id );
   if( !base_class_id.empty( ) )
   {
      class_const_iterator cci;
      external_reference_const_iterator erci;
      locate_base_class( base_class_id, erci, cci );

      if( erci != external_references.end( ) )
         throw runtime_error( "unexpected external base class found" );
      else
         verify_base_class_does_not_have_field( cci, field_name, true );
   }

   if( is_base )
   {
      if( ( cci->second ).fields_index.find( field_name ) != ( cci->second ).fields_index.end( ) )
         throw runtime_error( "field '" + field_name + "' already exists in ancestor class '" + ( cci->second ).name + "'" );
   }
}

void model::impl::verify_derived_class_does_not_have_field( const string& class_id, const string& field_name ) const
{
   class_const_iterator cci;
   for( cci = classes.begin( ); cci != classes.end( ); ++cci )
   {
      if( class_id == ( cci->second ).base_class_id )
      {
         if( ( cci->second ).fields_index.find( field_name ) != ( cci->second ).fields_index.end( ) )
            throw runtime_error( "field '" + field_name + "' already exists in descendant class '" + ( cci->second ).name + "'" );

         verify_derived_class_does_not_have_field( cci->first, field_name );

         break;
      }
   }
}

string model::impl::get_base_class_name( const string& base_class_id, string* p_extra ) const
{
   string base_class_name;

   if( !base_class_id.empty( ) )
   {
      string::size_type pos = base_class_id.find( ':' );
      if( pos != string::npos )
      {
         string model_name = base_class_id.substr( 0, pos );
         external_reference_const_iterator erci = external_references.find( model_name );

         if( erci == external_references.end( ) )
            throw runtime_error( "unexpected external reference entry not found for '" + model_name + "'" );

         class_const_iterator cci = erci->second->p_impl->classes.find( base_class_id.substr( pos + 1 ) );
         if( cci == erci->second->p_impl->classes.end( ) )
            throw runtime_error( "unexpected class_info entry not found for class id '"
             + base_class_id.substr( pos + 1 ) + "' in externally referenced model '" + model_name + "'" );

         base_class_name = model_name + ":" + cci->second.name;
         if( p_extra )
            *p_extra = cci->second.extra;
      }
      else
      {
         class_const_iterator cci = classes.find( base_class_id );
         if( cci == classes.end( ) )
            throw runtime_error( "unexpected class_info entry not found for class id '" + base_class_id + "'" );

         base_class_name = cci->second.name;
         if( p_extra )
            *p_extra = cci->second.extra;
      }
   }

   return base_class_name;
}

void model::impl::setup_fields( vector< field_data >& all_field_data, get_field_type field_type,
 class_const_iterator cci, const map< string, string >& modifier_flags, bool include_base_fields )
{
   if( include_base_fields )
   {
      string base_class_id( ( cci->second ).base_class_id );
      if( !base_class_id.empty( ) )
      {
         class_const_iterator cci;
         external_reference_const_iterator erci;
         locate_base_class( base_class_id, erci, cci );

         if( erci != external_references.end( ) )
            erci->second->p_impl->setup_fields( all_field_data, field_type, cci, modifier_flags, include_base_fields );
         else
            setup_fields( all_field_data, field_type, cci, modifier_flags, include_base_fields );
      }
   }

   // NOTE: Both user defined types and user defined enums can either be
   // locally defined or obtained from an externally referenced module.
   vector< user_defined_type_data > all_udt_data;
   vector< user_defined_enum_data > all_ude_data;

   get_user_defined_type_data( all_udt_data, true );
   get_user_defined_enum_data( all_ude_data, true );

   field_index_const_iterator fici;
   for( fici = ( cci->second ).fields_index.begin( ); fici != ( cci->second ).fields_index.end( ); ++fici )
   {
      field_const_iterator fci = ( cci->second ).fields.find( fici->second );
      if( fci == ( cci->second ).fields.end( ) )
         throw runtime_error(
          "unexpected field_info entry not found for field '" + fici->first + "' (id = " + fici->second + "'" );

      string name( ( fci->second ).name );
      string type( ( fci->second ).type );

      string extra( ( fci->second ).extra );

      string sys_type( type );

      for( size_t i = 0; i < all_udt_data.size( ); i++ )
      {
         if( sys_type == all_udt_data[ i ].id )
         {
            sys_type = all_udt_data[ i ].sys_type;

            if( !all_udt_data[ i ].extra.empty( ) )
            {
               if( !extra.empty( ) )
                  extra += "+";
               extra += all_udt_data[ i ].extra;
            }
            break;
         }
      }

      string enum_name, enum_id( ( fci->second ).enum_id );

      for( size_t i = 0; i < all_ude_data.size( ); i++ )
      {
         if( all_ude_data[ i ].id == enum_id )
         {
            enum_name = all_ude_data[ i ].name;
            break;
         }
      }

      string enum_filter, enum_filter_id( ( fci->second ).enum_filter_id );

      for( size_t i = 0; i < all_ude_data.size( ); i++ )
      {
         if( all_ude_data[ i ].id == enum_filter_id )
         {
            enum_filter = all_ude_data[ i ].name;
            break;
         }
      }

      string default_value( ( fci->second ).default_value );

      string decorated_name( name );

      bool is_mandatory( ( fci->second ).is_mandatory );
      bool is_transient( ( fci->second ).is_transient );
      bool is_foreign_key = false;
      bool for_text_search( ( fci->second ).for_text_search );

      if( type == c_relationship_type )
      {
         is_foreign_key = true;

         string class_id( ( fci->second ).class_id );
         class_const_iterator cci = classes.find( class_id );
         if( cci == classes.end( ) )
            throw runtime_error( "unexpected class_info entry not found for class id '" + class_id + "'" );

         string relationship_id( ( fci->second ).relationship_id );

         class_const_iterator child_cci, parent_cci;
         relationship_const_iterator rci = fetch_relationship_info( relationship_id, &child_cci, &parent_cci, &name );

         type = get_name( );
         type += '_';
         type += ( cci->second ).name;
      }

      bool is_external_field( p_parent_model != 0 );

      string modifiers;
      for( modifier_detail_const_iterator mdci = ( fci->second ).modifier_details.begin( ); mdci != ( fci->second ).modifier_details.end( ); ++mdci )
      {
         if( !modifiers.empty( ) )
            modifiers += ",";

         modifiers += modifier_flags.find( mdci->first )->second;
         modifiers += ":" + mdci->second;
      }

      if( field_type == e_get_field_type_any
       || ( field_type == e_get_field_type_relationships && is_foreign_key ) )
         all_field_data.push_back( field_data( fci->first, name,
          type, sys_type, is_mandatory, is_transient, is_foreign_key, for_text_search, is_external_field,
          enum_name, enum_filter, extra, modifiers, default_value, ( cci->second ).name, decorated_name ) );
   }
}

void model::impl::setup_indexes( vector< index_data >& all_index_data, class_const_iterator cci, bool include_base_fields )
{
   if( include_base_fields )
   {
      string base_class_id( ( cci->second ).base_class_id );
      if( !base_class_id.empty( ) )
      {
         class_const_iterator cci;
         external_reference_const_iterator erci;
         locate_base_class( base_class_id, erci, cci );

         if( erci != external_references.end( ) )
            erci->second->p_impl->setup_indexes( all_index_data, cci, include_base_fields );
         else
            setup_indexes( all_index_data, cci, include_base_fields );
      }
   }

   for( index_index_const_iterator iici =
    ( cci->second ).indexes_index.begin( ); iici != ( cci->second ).indexes_index.end( ); ++iici )
   {
      index_const_iterator ici = ( cci->second ).indexes.find( iici->second );
      if( ici == ( cci->second ).indexes.end( ) )
         throw runtime_error( "unexpected index_info not found for index '" + iici->first + "' (id = " + iici->second + ")" );

      all_index_data.push_back( index_data( iici->second,
       get_undecorated_index_name( ( cci->second ).name, ( ici->second ).name, true ), ( ici->second ).is_unique ) );
   }
}

void model::impl::setup_procedures(
 vector< procedure_data >& all_procedure_data, class_const_iterator cci, bool include_base_procedures )
{
   if( include_base_procedures )
   {
      string base_class_id( ( cci->second ).base_class_id );
      if( !base_class_id.empty( ) )
      {
         class_const_iterator cci;
         external_reference_const_iterator erci;
         locate_base_class( base_class_id, erci, cci );

         if( erci != external_references.end( ) )
            erci->second->p_impl->setup_procedures( all_procedure_data, cci, include_base_procedures );
         else
            setup_procedures( all_procedure_data, cci, include_base_procedures );
      }
   }

   for( procedure_index_const_iterator pici =
    ( cci->second ).procedures_index.begin( ); pici != ( cci->second ).procedures_index.end( ); ++pici )
      all_procedure_data.push_back( procedure_data( pici->second, pici->first ) );
}

void model::impl::setup_modifier_flags( uint64_t& flag_value,
 map< string, string >& modifier_flags, class_const_iterator cci,
 bool include_base_modifiers, string* p_modifiers, bool prefix_flag_with_id )
{
   if( include_base_modifiers )
   {
      string base_class_id( ( cci->second ).base_class_id );
      if( !base_class_id.empty( ) )
      {
         class_const_iterator cci;
         external_reference_const_iterator erci;
         locate_base_class( base_class_id, erci, cci );

         if( erci != external_references.end( ) )
            erci->second->p_impl->setup_modifier_flags( flag_value,
             modifier_flags, cci, include_base_modifiers, p_modifiers, prefix_flag_with_id );
         else
            setup_modifier_flags( flag_value, modifier_flags,
             cci, include_base_modifiers, p_modifiers, prefix_flag_with_id );
      }
   }

   for( modifier_index_const_iterator mici =
    ( cci->second ).modifiers_index.begin( ); mici != ( cci->second ).modifiers_index.end( ); ++mici )
   {
      ostringstream osstr;
      osstr << hex << flag_value;

      string flag( osstr.str( ) );
      if( prefix_flag_with_id )
         flag = mici->first + " " + flag;

      modifier_flags.insert( make_pair( mici->second, flag ) );
      flag_value <<= 1;
   }

   if( p_modifiers )
   {
      for( modifier_detail_const_iterator mdci = ( cci->second ).modifier_details.begin( ); mdci != ( cci->second ).modifier_details.end( ); ++mdci )
      {
         if( !p_modifiers->empty( ) )
            *p_modifiers += ",";

         *p_modifiers += modifier_flags.find( mdci->first )->second;
         *p_modifiers += ":" + mdci->second;
      }
   }
}

void model::impl::locate_base_class(
 const string& base_class_id, external_reference_const_iterator& erci, class_const_iterator& cci ) const
{
   erci = external_references.end( );

   size_t pos = base_class_id.find( ':' );
   if( pos != string::npos )
   {
      string model_name = base_class_id.substr( 0, pos );
      erci = external_references.find( model_name );

      if( erci == external_references.end( ) )
         throw runtime_error( "unexpected external reference entry not found for '" + model_name + "'" );

      cci = erci->second->p_impl->classes.find( base_class_id.substr( pos + 1 ) );
      if( cci == erci->second->p_impl->classes.end( ) )
         throw runtime_error( "unexpected class_info entry not found for class id '"
          + base_class_id.substr( pos + 1 ) + "' in externally referenced model '" + model_name + "'" );
   }
   else
   {
      cci = classes.find( base_class_id );
      if( cci == classes.end( ) )
         throw runtime_error( "unexpected class_info entry not found for base class id '" + base_class_id + "'" );
   }
}

string model::impl::get_undecorated_class_name( const string& decorated_class_name ) const
{
   string str( decorated_class_name );
   string::size_type pos, npos;

   while( true )
   {
      pos = str.find( '{' );
      if( pos == string::npos )
         break;

      npos = str.find( '}', pos + 1 );
      if( npos == string::npos )
         throw runtime_error( "unexpected '{' without '}' in name '" + str + "'" );

      string class_name( str.substr( pos + 1, npos - pos - 1 ) );
      if( classes_index.find( class_name ) == classes_index.end( ) )
         throw runtime_error( "unexpected unknown class name '" + class_name + "'" );

      str.erase( pos, npos - pos + 1 );
      str.insert( pos, class_name );
   }

   return str;
}

string model::impl::get_undecorated_index_name(
 const string& class_name, const string& decorated_index_name, bool undecorate_classes ) const
{
   string str( decorated_index_name );
   string::size_type pos, npos;

   class_index_const_iterator cici = classes_index.find( class_name );
   if( cici == classes_index.end( ) )
      throw runtime_error( "unknown class '" + class_name + "'" );

   string id( cici->second );
   class_const_iterator cci = classes.find( id );
   if( cci == classes.end( ) )
      throw runtime_error( "unexpected class_info entry not found for class '" + class_name + "' (id = " + id + ")" );

   while( true )
   {
      pos = str.find( '[' );
      if( pos == string::npos )
         break;

      npos = str.find( ']', pos + 1 );
      if( npos == string::npos )
         throw runtime_error( "unexpected '[' without ']' in name '" + str + "'" );

      string field_name( str.substr( pos + 1, npos - pos - 1 ) );
      if( cci->second.fields_index.find( field_name ) == cci->second.fields_index.end( ) )
         throw runtime_error( "unexpected unknown field name '" + field_name + "' for class '" + class_name + "'" );

      str.erase( pos, npos - pos + 1 );

      if( undecorate_classes )
         field_name = get_undecorated_class_name( field_name );
      str.insert( pos, field_name );
   }

   return str;
}

class_iterator model::impl::fetch_class_info( const string& class_name )
{
   class_index_const_iterator cici = classes_index.find( class_name );
   if( cici == classes_index.end( ) )
      throw runtime_error( "unknown class '" + class_name + "'" );

   string id( cici->second );
   class_iterator ci = classes.find( id );
   if( ci == classes.end( ) )
      throw runtime_error( "unexpected class_info entry not found for class '" + class_name + "' (id = " + id + ")" );

   return ci;
}

class_const_iterator model::impl::fetch_class_info( const string& class_name ) const
{
   class_index_const_iterator cici = classes_index.find( class_name );
   if( cici == classes_index.end( ) )
      throw runtime_error( "unknown class '" + class_name + "'" );

   string id( cici->second );
   class_const_iterator cci = classes.find( id );
   if( cci == classes.end( ) )
      throw runtime_error( "unexpected class_info entry not found for class '" + class_name + "' (id = " + id + ")" );

   return cci;
}

field_iterator model::impl::fetch_field_info( class_iterator ci, const string& field_name, field_index_iterator* p_fii )
{
   field_index_iterator fii = ( ci->second ).fields_index.find( field_name );
   if( fii == ( ci->second ).fields_index.end( ) )
      throw runtime_error( "field name '" + field_name + "' not found in class '" + ( ci->second ).name + "'" );

   field_iterator fi = ( ci->second ).fields.find( fii->second );
   if( fi == ( ci->second ).fields.end( ) )
      throw runtime_error(
       "unexpected field_info entry not found for field '" + fii->first + "' (id = " + fii->second + "'" );

   if( p_fii )
      *p_fii = fii;

   return fi;
}

field_const_iterator model::impl::fetch_field_info( class_const_iterator cci, const string& field_name ) const
{
   field_index_const_iterator fici = ( cci->second ).fields_index.find( field_name );
   if( fici == ( cci->second ).fields_index.end( ) )
      throw runtime_error( "field name '" + field_name + "' not found in class '" + ( cci->second ).name + "'" );

   field_const_iterator fci = ( cci->second ).fields.find( fici->second );
   if( fci == ( cci->second ).fields.end( ) )
      throw runtime_error(
       "unexpected field_info entry not found for field '" + fici->first + "' (id = " + fici->second + "'" );

   return fci;
}

field_const_iterator model::impl::fetch_field_info( class_const_iterator cci, relationship_const_iterator rci ) const
{
   field_const_iterator fci = ( cci->second ).fields.end( );

   for( field_const_iterator i = ( cci->second ).fields.begin( ), end = ( cci->second ).fields.end( ); i != end; ++i )
   {
      if( ( i->second ).relationship_id == rci->first )
      {
         fci = i;
         break;
      }
   }

   return fci;
}

procedure_iterator model::impl::fetch_procedure_info(
 const string& class_name, const string& procedure_name, procedure_index_iterator* p_pii )
{
   class_iterator ci = fetch_class_info( class_name );

   procedure_index_iterator pii = ( ci->second ).procedures_index.find( procedure_name );
   if( pii == ( ci->second ).procedures_index.end( ) )
      throw runtime_error( "unknown procedure '" + procedure_name + "' for class '" + class_name + "'" );

   string id( pii->second );
   procedure_iterator pi = ( ci->second ).procedures.find( id );
   if( pi == ( ci->second ).procedures.end( ) )
      throw runtime_error( "unexpected procedure_info entry not found for procedure '" + procedure_name + "' (id = " + id + ")" );

   if( p_pii )
      *p_pii = pii;

   return pi;
}

procedure_const_iterator model::impl::fetch_procedure_info( const string& class_name, const string& procedure_name ) const
{
   class_const_iterator cci = fetch_class_info( class_name );

   procedure_index_const_iterator pici = ( cci->second ).procedures_index.find( procedure_name );
   if( pici == ( cci->second ).procedures_index.end( ) )
      throw runtime_error( "unknown procedure '" + procedure_name + "' for class '" + class_name + "'" );

   string id( pici->second );
   procedure_const_iterator pci = ( cci->second ).procedures.find( id );
   if( pci == ( cci->second ).procedures.end( ) )
      throw runtime_error( "unexpected procedure_info entry not found for procedure '" + procedure_name + "' (id = " + id + ")" );

   return pci;
}

relationship_iterator model::impl::fetch_relationship_info( const string& relationship_name, relationship_index_iterator* p_rii )
{
   relationship_index_iterator rii = relationships_index.find( relationship_name );
   if( rii == relationships_index.end( ) )
      throw runtime_error( "unknown relationship '" + relationship_name + "'" );

   string relationship_id( rii->second );

   relationship_iterator ri = relationships.find( relationship_id );
   if( ri == relationships.end( ) )
      throw runtime_error( "unexpected relationship_info entry not found for relationship '"
       + relationship_name + "' (id = " + relationship_id + ")" );

   if( p_rii )
      *p_rii = rii;

   return ri;
}

relationship_const_iterator model::impl::fetch_relationship_info(
 const string& relationship_id, class_const_iterator* p_child, class_const_iterator* p_parent, string* p_name ) const
{
   relationship_const_iterator rci = relationships.find( relationship_id );
   if( rci == relationships.end( ) )
      throw runtime_error( "unexpected relationship_info entry not found for relationship id '" + relationship_id + "'" );

   class_const_iterator child_cci, parent_cci;

   child_cci = classes.find( ( rci->second ).child_id );
   if( child_cci == classes.end( ) )
      throw runtime_error( "unable to find class_info for child class id '" + ( rci->second ).child_id + "'" );

   parent_cci = classes.find( ( rci->second ).parent_id );
   if( parent_cci == classes.end( ) )
      throw runtime_error( "unable to find class_info for parent class id '" + ( rci->second ).parent_id + "'" );

   *p_child = child_cci;
   *p_parent = parent_cci;

   if( p_name )
      *p_name = get_undecorated_class_name( *p_name );

   return rci;
}

model::model( )
{
   p_impl = new impl;
   p_impl->p_model = this;
}

model::~model( )
{
   delete p_impl;
}

const string& model::get_id( ) const
{
   return p_impl->get_id( );
}

void model::set_id( const string& new_id )
{
   p_impl->set_id( new_id );
}

const string& model::get_name( ) const
{
   return p_impl->get_name( );
}

void model::set_name( const string& new_name )
{
   p_impl->set_name( new_name );
}

const string& model::get_perm( ) const
{
   return p_impl->get_perm( );
}

void model::set_perm( const string& new_perm )
{
   p_impl->set_perm( new_perm );
}

const string& model::get_version( ) const
{
   return p_impl->get_version( );
}

void model::set_version( const string& new_version )
{
   p_impl->set_version( new_version );
}

int model::get_year_created( ) const
{
   return p_impl->get_year_created( );
}

void model::set_year_created( int new_year_created )
{
   p_impl->set_year_created( new_year_created );
}

string model::get_title( ) const
{
   return p_impl->get_title( );
}

int model::get_major_version( ) const
{
   return p_impl->get_major_version( );
}

int model::get_minor_version( ) const
{
   return p_impl->get_minor_version( );
}

void model::get_class_names( vector< string >& class_names )
{
   p_impl->get_class_names( class_names );
}

void model::get_external_module_names( vector< string >& module_names )
{
   p_impl->get_external_module_names( module_names );
}

void model::get_class_data( vector< class_data >& all_class_data, bool include_base_info )
{
   p_impl->get_class_data( all_class_data, include_base_info );
}

void model::get_class_data( const string& class_name, class_data& cls_data, bool include_base_info )
{
   p_impl->get_class_data( class_name, cls_data, include_base_info );
}

void model::get_field_data( const string& class_name,
 vector< field_data >& all_field_data, get_field_type field_type, bool include_base_fields )
{
   p_impl->get_field_data( class_name, all_field_data, field_type, include_base_fields );
}

void model::get_index_data( const string& class_name,
 vector< index_data >& all_index_data, bool include_base_fields )
{
   p_impl->get_index_data( class_name, all_index_data, include_base_fields );
}

void model::get_modifier_data( const string& class_name, vector< modifier_data >& all_modifier_data )
{
   p_impl->get_modifier_data( class_name, all_modifier_data );
}

void model::get_procedure_data( const string& class_name,
 vector< procedure_data >& all_procedure_data, bool include_base_procedures )
{
   p_impl->get_procedure_data( class_name, all_procedure_data, include_base_procedures );
}

void model::get_procedure_arg_data( const string& class_name,
 const string& procedure_name, vector< procedure_arg_data >& all_procedure_arg_data )
{
   p_impl->get_procedure_arg_data( class_name, procedure_name, all_procedure_arg_data );
}

void model::get_relationship_data( const string& relationship_name, relationship_data& rel_data )
{
   p_impl->get_relationship_data( relationship_name, rel_data );
}

void model::get_child_relationship_data( const string& parent_class_name, vector< child_relationship_data >& child_rel_info )
{
   p_impl->get_child_relationship_data( parent_class_name, child_rel_info );
}

void model::get_specification_data( const string& class_name,
 vector< specification_data >& all_spec_data, get_specifications_type gstype )
{
   p_impl->get_specification_data( class_name, all_spec_data, gstype );
}

void model::get_user_defined_enum_data(
 vector< user_defined_enum_data >& all_ude_data, bool include_external_enums ) const
{
   p_impl->get_user_defined_enum_data( all_ude_data, include_external_enums );
}

void model::get_user_defined_type_data(
 vector< user_defined_type_data >& all_udt_data, bool include_external_types ) const
{
   p_impl->get_user_defined_type_data( all_udt_data, include_external_types );
}

void model::get_all_base_related_classes( const string& class_name, set< string >& all_related_classes ) const
{
   p_impl->get_all_base_related_classes( class_name, all_related_classes );
}

void model::check( ) const
{
   p_impl->check( );
}

void model::save( const string& filename )
{
   p_impl->save( filename );
}

void model::load( const string& filename )
{
   p_impl->load( filename );
}

void model::clear( )
{
   p_impl->clear( );
}

void model::class_add( const string& class_id,
 const string& class_name, const string& plural_name,
 const string& fixed_key_val, const string& extra, const string& base_class_name )
{
   p_impl->class_add( class_id,
    class_name, plural_name, fixed_key_val, extra, base_class_name );
}

void model::class_list( ostream& outs ) const
{
   p_impl->class_list( outs );
}

void model::class_remove( const string& class_name )
{
   p_impl->class_remove( class_name );
}

void model::class_rename( const string& old_class_name, const string& new_class_name )
{
   p_impl->class_rename( old_class_name, new_class_name );
}

void model::field_add( const string& field_id,
 const string& class_name, const string& field_name,
 const string& field_type, bool is_mandatory, bool is_transient, bool for_text_search,
 const string& enum_name, const string& enum_filter, const string& extra, const string& default_value )
{
   p_impl->field_add( field_id, class_name, field_name,
    field_type, is_mandatory, is_transient, for_text_search, enum_name, enum_filter, extra, default_value );
}

void model::field_list( const string& class_name, ostream& outs ) const
{
   p_impl->field_list( class_name, outs );
}

void model::field_remove( const string& class_name, const string& field_name )
{
   p_impl->field_remove( class_name, field_name );
}

void model::field_rename( const string& class_name, const string& old_field_name, const string& new_field_name )
{
   p_impl->field_rename( class_name, old_field_name, new_field_name );
}

void model::index_add( const string& index_id, const string& class_name, const string& field_list, bool is_unique )
{
   p_impl->index_add( index_id, class_name, field_list, is_unique );
}

void model::index_list( const string& class_name, ostream& outs ) const
{
   p_impl->index_list( class_name, outs );
}

void model::index_remove( const string& class_name, const string& field_list )
{
   p_impl->index_remove( class_name, field_list );
}

void model::modifier_add( const string& modifier_id, const string& class_name, const string& modifier_name, const string& field_info )
{
   p_impl->modifier_add( modifier_id, class_name, modifier_name, field_info );
}

void model::modifier_list( const string& class_name, ostream& outs ) const
{
   p_impl->modifier_list( class_name, outs );
}

void model::modifier_remove( const string& class_name, const string& modifier_name )
{
   p_impl->modifier_remove( class_name, modifier_name );
}

void model::modifier_rename( const string& class_name, const string& old_modifier_name, const string& new_modifier_name )
{
   p_impl->modifier_rename( class_name, old_modifier_name, new_modifier_name );
}

void model::procedure_add( const string& procedure_id, const string& class_name, const string& procedure_name )
{
   p_impl->procedure_add( procedure_id, class_name, procedure_name );
}

void model::procedure_list( const string& class_name, ostream& outs ) const
{
   p_impl->procedure_list( class_name, outs );
}

void model::procedure_remove( const string& class_name, const string& procedure_name )
{
   p_impl->procedure_remove( class_name, procedure_name );
}

void model::procedure_rename( const string& class_name, const string& old_procedure_name, const string& new_procedure_name )
{
   p_impl->procedure_rename( class_name, old_procedure_name, new_procedure_name );
}

void model::procedure_arg_add( const string& arg_id, const string& class_name,
 const string& procedure_name, const string& arg_name, const string& arg_type, bool is_output )
{
   p_impl->procedure_arg_add( arg_id, class_name, procedure_name, arg_name, arg_type, is_output );
}

void model::procedure_arg_list( const string& class_name, const string& procedure_name, ostream& outs ) const
{
   p_impl->procedure_arg_list( class_name, procedure_name, outs );
}

void model::procedure_arg_remove( const string& class_name, const string& procedure_name, const string& arg_name )
{
   p_impl->procedure_arg_remove( class_name, procedure_name, arg_name );
}

void model::procedure_arg_rename( const string& class_name,
 const string& procedure_name, const string& old_arg_name, const string& new_arg_name )
{
   p_impl->procedure_arg_rename( class_name, procedure_name, old_arg_name, new_arg_name );
}

void model::relationship_add( const string& relationship_id,
 const string& field_id, const string& child_class_name, const string& child_field_name,
 const string& parent_class_name, bool is_mandatory, bool is_transient, cascade_operation cascade_op, const string& extra )
{
   p_impl->relationship_add( relationship_id, field_id,
    child_class_name, child_field_name, parent_class_name, is_mandatory, is_transient, cascade_op, extra );
}

void model::relationship_list( ostream& outs ) const
{
   p_impl->relationship_list( outs );
}

void model::relationship_list_parents( const string& child_class_name, ostream& outs ) const
{
   p_impl->relationship_list_parents( child_class_name, outs );
}

void model::relationship_list_children( const string& parent_class_name, ostream& outs ) const
{
   p_impl->relationship_list_children( parent_class_name, outs );
}

void model::relationship_remove( const string& relationship_name )
{
   p_impl->relationship_remove( relationship_name );
}

void model::relationship_rename( const string& old_relationship_name, const string& new_relationship_name )
{
   p_impl->relationship_rename( old_relationship_name, new_relationship_name );
}

void model::specification_add( const string& specification_id,
 const string& specification_name, const string& specification_type, const string& specification_args )
{
   p_impl->specification_add( specification_id, specification_name, specification_type, specification_args );
}

void model::specification_list( ostream& outs ) const
{
   p_impl->specification_list( outs );
}

void model::specification_remove( const string& specification_name )
{
   p_impl->specification_remove( specification_name );
}

void model::specification_rename( const string& old_specification_name, const string& new_specification_name )
{
   p_impl->specification_rename( old_specification_name, new_specification_name );
}

void model::user_defined_enum_add( const string& ude_id,
 const string& ude_name, const string& type_name, const string& key_values )
{
   p_impl->user_defined_enum_add( ude_id, ude_name, type_name, key_values );
}

void model::user_defined_enum_list( ostream& outs ) const
{
   p_impl->user_defined_enum_list( outs );
}

void model::user_defined_enum_remove( const string& ude_name )
{
   p_impl->user_defined_enum_remove( ude_name );
}

void model::user_defined_enum_rename( const string& old_ude_name, const string& new_ude_name )
{
   p_impl->user_defined_enum_rename( old_ude_name, new_ude_name );
}

void model::user_defined_type_add( const string& udt_id,
 const string& udt_name, const string& type_name, const string& type_domain_info )
{
   p_impl->user_defined_type_add( udt_id, udt_name, type_name, type_domain_info );
}

void model::user_defined_type_list( ostream& outs ) const
{
   p_impl->user_defined_type_list( outs );
}

void model::user_defined_type_remove( const string& udt_name )
{
   p_impl->user_defined_type_remove( udt_name );
}

void model::user_defined_type_rename( const string& old_udt_name, const string& new_udt_name )
{
   p_impl->user_defined_type_rename( old_udt_name, new_udt_name );
}

string model::determine_child_rel_suffix( const string& relationship_name, string* p_rel_without_child )
{
   return p_impl->determine_child_rel_suffix( relationship_name, p_rel_without_child );
}

string get_model_file_ext( )
{
   return c_model_file_extension;
}

string get_cascade_op_name( cascade_operation cascade_op )
{
   string retval;

   switch( cascade_op )
   {
      case e_cascade_operation_unlink:
      retval = c_cascade_operation_unlink;
      break;

      case e_cascade_operation_destroy:
      retval = c_cascade_operation_destroy;
      break;

      case e_cascade_operation_restrict:
      retval = c_cascade_operation_restrict;
      break;

      default:
      throw runtime_error( "unexpected cascade_op value: " + to_string( ( int )cascade_op ) );
   }

   return retval;
}

cascade_operation get_cascade_op_for_name( const string& op_name )
{
   cascade_operation cascade_op;

   if( op_name == c_cascade_operation_unlink )
      cascade_op = e_cascade_operation_unlink;
   else if( op_name == c_cascade_operation_destroy )
      cascade_op = e_cascade_operation_destroy;
   else if( op_name == c_cascade_operation_restrict )
      cascade_op = e_cascade_operation_restrict;
   else
      throw runtime_error( "unexpected cascade_op name: " + op_name );

   return cascade_op;
}

