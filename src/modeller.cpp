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
#  include <cstring>
#  include <cstdlib>
#  include <set>
#  include <string>
#  include <sstream>
#  include <fstream>
#  include <iostream>
#  include <algorithm>
#  include <stdexcept>
#endif

#include "model.h"
#include "common.h"
#include "macros.h"
#include "sql_db.h"
#include "utilities.h"
#include "date_time.h"
#include "ciyam_core.h"
#include "console_commands.h"

using namespace std;

#include "modeller.cmh"

const char* const c_app_title = "modeller";
const char* const c_app_version = "0.1";

const char* const c_true = "true";
const char* const c_false = "false";

const char* const c_cpp_ext = ".cpp";
const char* const c_txt_ext = ".txt";
const char* const c_make_ext = ".make";
const char* const c_spec_ext = ".spec";
const char* const c_xrep_variables_ext = ".vars.xrep";

const char* const c_all_simple_field_types[ ] =
{
   "bool",
   "int"
};

const char* const c_all_complex_field_types[ ] =
{
   "date",
   "datetime",
   "mtime",
   "numeric",
   "string",
   "time"
};

const char* const c_non_string_field_types[ ] =
{
   "bool",
   "int",
   "numeric"
};

const char* const c_all_nullable_field_types[ ] =
{
   "bool",
   "date",
   "datetime",
   "file",
   "string",
   "time"
};

const char* const c_all_commandable_field_types[ ] =
{
   "datetime",
   "file",
   "numeric"
};

const char* const c_all_customised_raw_field_types[ ] =
{
   "date_time",
   "mtime",
   "numeric",
   "tbool",
   "tdate",
   "tdatetime",
   "ttime",
   "udate"
};

inline bool compare( const char* p_s1, const char* p_s2 ) { return strcmp( p_s1, p_s2 ) < 0; }

inline bool is_simple_type( const string& type )
{
   return binary_search( c_all_simple_field_types,
    c_all_simple_field_types + ARRAY_SIZE( c_all_simple_field_types ), type.c_str( ), compare );
}

inline bool is_complex_type( const string& type )
{
   return binary_search( c_all_complex_field_types,
    c_all_complex_field_types + ARRAY_SIZE( c_all_complex_field_types ), type.c_str( ), compare );
}

inline bool is_string_type( const string& type )
{
   return !binary_search( c_non_string_field_types,
    c_non_string_field_types + ARRAY_SIZE( c_non_string_field_types ), type.c_str( ), compare );
}

inline bool is_nullable_type( const string& type )
{
   return binary_search( c_all_nullable_field_types,
    c_all_nullable_field_types + ARRAY_SIZE( c_all_nullable_field_types ), type.c_str( ), compare );
}

inline bool is_commandable_type( const string& type )
{
   return binary_search( c_all_commandable_field_types,
    c_all_commandable_field_types + ARRAY_SIZE( c_all_commandable_field_types ), type.c_str( ), compare );
}

inline bool is_customised_raw_type( const string& raw_type )
{
   return binary_search( c_all_customised_raw_field_types,
    c_all_customised_raw_field_types + ARRAY_SIZE( c_all_customised_raw_field_types ), raw_type.c_str( ), compare );
}

string get_sql_type( const string& type, bool is_mandatory,
 bool is_foreign_key, bool* p_is_sql_numeric = 0, bool is_large_text = false, bool is_large_table = false )
{
   string sql_type;
   bool is_sql_numeric = false;

   if( type == "date" )
      sql_type = "CHAR(10)";
   else if( type == "time" )
      sql_type = "CHAR(8)";
   else if( type == "datetime" )
      sql_type = "CHAR(23)";
   else if( type == "int" || type == "bool" )
   {
      sql_type = "INTEGER";
      is_sql_numeric = true;
   }
   else if( type == "numeric" || type == "currency" )
   {
      sql_type = "NUMERIC(28\\\\,8)";
      is_sql_numeric = true;
   }
   else
   {
      if( is_large_text )
         sql_type = "TEXT";
      else if( is_foreign_key && !is_large_table )
         sql_type = "VARCHAR(" + to_string( c_max_key_length ) + ")";
      else if( !is_large_table )
         sql_type = "VARCHAR(200)";
      else
         sql_type = "VARCHAR(256)";
   }

   // NOTE: As '' is used rather than NULL by the server (and the server does not support NULL for numeric
   // types nor permit it as a seperate state to empty string for text types) NOT NULL is always used here.
   ( void )is_mandatory;
   sql_type += " NOT NULL";

   if( p_is_sql_numeric )
      *p_is_sql_numeric = is_sql_numeric;

   return sql_type;
}

string get_raw_type( const string& type, bool is_mandatory )
{
   string raw_type( type );

   if( type == "bool" )
   {
      if( !is_mandatory )
         raw_type = "tbool";
   }
   else if( type == "date" )
   {
      if( is_mandatory )
         raw_type = "udate";
      else
         raw_type = "tdate";
   }
   else if( type == "datetime" )
   {
      if( is_mandatory )
         raw_type = "date_time";
      else
         raw_type = "tdatetime";
   }
   else if( type == "time" )
   {
      if( is_mandatory )
         raw_type = "mtime";
      else
         raw_type = "ttime";
   }

   return raw_type;
}

string unique_id( )
{
   ostringstream osstr;
   mtime mt( mtime::local( ) );
   osstr << ( milliseconds )mt;
   msleep( 7 );
   return osstr.str( );
}

void replace_char_occurences( string& str, char old_char, char new_char, char esc_char = '\0' )
{
   string new_str;

   bool had_escape = false;
   for( string::size_type i = 0; i < str.size( ); i++ )
   {
      if( had_escape )
      {
         had_escape = false;
         new_str += str[ i ];
      }
      else if( esc_char && str[ i ] == esc_char )
         had_escape = true;
      else if( str[ i ] == old_char )
         new_str += new_char;
      else
         new_str += str[ i ];
   }

   str = new_str;
}

string string_list( const set< string >& strings, const char* p_separator = " " )
{
   string str;
   for( set< string >::const_iterator ci = strings.begin( ); ci != strings.end( ); ++ci )
   {
      if( !str.empty( ) )
         str += p_separator;
      str += *ci;
   }

   return str;
}

string string_list( const vector< string >& strings, const char* p_separator = " " )
{
   string str;
   for( vector< string >::size_type i = 0; i < strings.size( ); i++ )
   {
      if( i > 0 )
         str += p_separator;
      str += strings[ i ];
   }

   return str;
}

int g_rc;

model g_model;

bool g_application_title_called = false;

string application_title( app_info_request request )
{
   g_application_title_called = true;

   if( request == e_app_info_request_title )
      return string( c_app_title );
   else if( request == e_app_info_request_version )
      return string( c_app_version );
   else if( request == e_app_info_request_title_and_version )
   {
      string title( c_app_title );
      title += " v";
      title += string( c_app_version );

      return title;
   }
   else
   {
      ostringstream osstr;
      osstr << "unknown app_info_request: " << request;
      throw runtime_error( osstr.str( ) );
   }
}

class modeller_command_functor;

class modeller_command_handler : public console_command_handler
{
   friend class modeller_command_functor;

   public:
   modeller_command_handler( )
   {
   }
};

class modeller_command_functor : public command_functor
{
   public:
   modeller_command_functor( modeller_command_handler& modeller_handler )
    : command_functor( modeller_handler ),
    modeller_handler( modeller_handler )
   {
   }

   void operator ( )( const string& command, const parameter_info& parameters );

   modeller_command_handler& modeller_handler;
};

void modeller_command_functor::operator ( )( const string& command, const parameter_info& parameters )
{
   try
   {
      if( command == c_cmd_modeller_exit )
         handler.set_finished( );
      else if( command == c_cmd_modeller_class_add )
      {
         string id( get_parm_val( parameters, c_cmd_modeller_class_add_id ) );
         string class_name( get_parm_val( parameters, c_cmd_modeller_class_add_class_name ) );
         string plural_name( get_parm_val( parameters, c_cmd_modeller_class_add_plural_name ) );
         string fixed_key_val( get_parm_val( parameters, c_cmd_modeller_class_add_fixed_key_val ) );
         string extra( get_parm_val( parameters, c_cmd_modeller_class_add_extra ) );
         string base_class_name( get_parm_val( parameters, c_cmd_modeller_class_add_base_class_name ) );

         if( id.empty( ) )
            id = unique_id( );

         g_model.class_add( id, class_name, plural_name, fixed_key_val, extra, base_class_name );
      }
      else if( command == c_cmd_modeller_class_list )
      {
         g_model.class_list( cout );
      }
      else if( command == c_cmd_modeller_class_remove )
      {
         string class_name( get_parm_val( parameters, c_cmd_modeller_class_remove_class_name ) );

         g_model.class_remove( class_name );
      }
      else if( command == c_cmd_modeller_class_rename )
      {
         string old_name( get_parm_val( parameters, c_cmd_modeller_class_rename_old_class_name ) );
         string new_name( get_parm_val( parameters, c_cmd_modeller_class_rename_new_class_name ) );

         g_model.class_rename( old_name, new_name );
      }
      else if( command == c_cmd_modeller_field_add )
      {
         string id( get_parm_val( parameters, c_cmd_modeller_field_add_id ) );
         string class_name( get_parm_val( parameters, c_cmd_modeller_field_add_class_name ) );
         string field_name( get_parm_val( parameters, c_cmd_modeller_field_add_field_name ) );
         string type_name( get_parm_val( parameters, c_cmd_modeller_field_add_type_name ) );
         bool is_mandatory( has_parm_val( parameters, c_cmd_modeller_field_add_mand ) );
         string enum_name( get_parm_val( parameters, c_cmd_modeller_field_add_enum ) );
         string enum_filter( get_parm_val( parameters, c_cmd_modeller_field_add_enum_filter ) );
         string extra( get_parm_val( parameters, c_cmd_modeller_field_add_extra ) );
         string default_value( get_parm_val( parameters, c_cmd_modeller_field_add_default ) );
         bool is_transient( has_parm_val( parameters, c_cmd_modeller_field_add_transient ) );
         bool for_text_search( has_parm_val( parameters, c_cmd_modeller_field_add_text_search ) );

         if( !field_name.empty( ) && field_name[ field_name.length( ) - 1 ] == '_' )
            throw runtime_error( "cannot use trailing underscores in field names" );

         if( is_complex_type( field_name ) )
            throw runtime_error( "cannot use type name '" + field_name + "' as a field name" );

         vector< user_defined_type_data > all_udt_data;
         g_model.get_user_defined_type_data( all_udt_data, true );

         map< string, string > user_defined_types;
         for( size_t i = 0; i < all_udt_data.size( ); i++ )
            user_defined_types.insert( make_pair( all_udt_data[ i ].name, all_udt_data[ i ].type ) );

         string actual_type( type_name );
         while( user_defined_types.count( actual_type ) )
            actual_type = user_defined_types[ actual_type ];

         if( !is_simple_type( actual_type ) && !is_complex_type( actual_type ) )
            throw runtime_error( "unknown field type '" + type_name + "'" );

         if( !is_mandatory && !is_nullable_type( actual_type ) )
            throw runtime_error( "type '" + type_name + "' fields are not permitted to be optional" );

         if( id.empty( ) )
            id = unique_id( );

         g_model.field_add( id, class_name, field_name, type_name,
          is_mandatory, is_transient, for_text_search, enum_name, enum_filter, extra, default_value );
      }
      else if( command == c_cmd_modeller_field_list )
      {
         string class_name( get_parm_val( parameters, c_cmd_modeller_field_list_class_name ) );

         g_model.field_list( class_name, cout );
      }
      else if( command == c_cmd_modeller_field_remove )
      {
         string class_name( get_parm_val( parameters, c_cmd_modeller_field_remove_class_name ) );
         string field_name( get_parm_val( parameters, c_cmd_modeller_field_remove_field_name ) );

         g_model.field_remove( class_name, field_name );
      }
      else if( command == c_cmd_modeller_field_rename )
      {
         string class_name( get_parm_val( parameters, c_cmd_modeller_field_rename_class_name ) );
         string old_field_name( get_parm_val( parameters, c_cmd_modeller_field_rename_old_field_name ) );
         string new_field_name( get_parm_val( parameters, c_cmd_modeller_field_rename_new_field_name ) );

         g_model.field_rename( class_name, old_field_name, new_field_name );
      }
      else if( command == c_cmd_modeller_index_add )
      {
         string id( get_parm_val( parameters, c_cmd_modeller_index_add_id ) );
         string class_name( get_parm_val( parameters, c_cmd_modeller_index_add_class_name ) );
         string field_list( get_parm_val( parameters, c_cmd_modeller_index_add_field_list ) );
         bool is_unique( has_parm_val( parameters, c_cmd_modeller_index_add_unique ) );

         vector< index_data > all_index_data;
         g_model.get_index_data( class_name, all_index_data );

         if( all_index_data.size( ) >= c_max_indexes_per_class )
            throw runtime_error( "number of indexes per class must not exceed " + to_string( c_max_indexes_per_class ) );

         vector< string > fields;
         if( !field_list.empty( ) )
            split( field_list, fields );

         if( fields.size( ) > c_max_fields_per_index )
            throw runtime_error( "number of fields in an index must not exceed " + to_string( c_max_fields_per_index ) );

         if( id.empty( ) )
            id = unique_id( );

         g_model.index_add( id, class_name, field_list, is_unique );
      }
      else if( command == c_cmd_modeller_index_list )
      {
         string class_name( get_parm_val( parameters, c_cmd_modeller_index_list_class_name ) );

         g_model.index_list( class_name, cout );
      }
      else if( command == c_cmd_modeller_index_remove )
      {
         string class_name( get_parm_val( parameters, c_cmd_modeller_index_remove_class_name ) );
         string field_list( get_parm_val( parameters, c_cmd_modeller_index_remove_field_list ) );

         g_model.index_remove( class_name, field_list );
      }
      else if( command == c_cmd_modeller_modifier_add )
      {
         string id( get_parm_val( parameters, c_cmd_modeller_modifier_add_id ) );
         string class_name( get_parm_val( parameters, c_cmd_modeller_modifier_add_class_name ) );
         string modifier_name( get_parm_val( parameters, c_cmd_modeller_modifier_add_modifier_name ) );
         string field_info( get_parm_val( parameters, c_cmd_modeller_modifier_add_field_info ) );

         if( id.empty( ) )
            id = unique_id( );

         g_model.modifier_add( id, class_name, modifier_name, field_info );
      }
      else if( command == c_cmd_modeller_modifier_list )
      {
         string class_name( get_parm_val( parameters, c_cmd_modeller_modifier_list_class_name ) );

         g_model.modifier_list( class_name, cout );
      }
      else if( command == c_cmd_modeller_modifier_remove )
      {
         string class_name( get_parm_val( parameters, c_cmd_modeller_modifier_remove_class_name ) );
         string modifier_name( get_parm_val( parameters, c_cmd_modeller_modifier_remove_modifier_name ) );

         g_model.modifier_remove( class_name, modifier_name );
      }
      else if( command == c_cmd_modeller_modifier_rename )
      {
         string class_name( get_parm_val( parameters, c_cmd_modeller_modifier_rename_class_name ) );
         string old_modifier_name( get_parm_val( parameters, c_cmd_modeller_modifier_rename_old_modifier_name ) );
         string new_modifier_name( get_parm_val( parameters, c_cmd_modeller_modifier_rename_new_modifier_name ) );

         g_model.modifier_rename( class_name, old_modifier_name, new_modifier_name );
      }
      else if( command == c_cmd_modeller_procedure_add )
      {
         string id( get_parm_val( parameters, c_cmd_modeller_procedure_add_id ) );
         string class_name( get_parm_val( parameters, c_cmd_modeller_procedure_add_class_name ) );
         string procedure_name( get_parm_val( parameters, c_cmd_modeller_procedure_add_procedure_name ) );

         if( id.empty( ) )
            id = unique_id( );

         g_model.procedure_add( id, class_name, procedure_name );
      }
      else if( command == c_cmd_modeller_procedure_list )
      {
         string class_name( get_parm_val( parameters, c_cmd_modeller_procedure_list_class_name ) );

         g_model.procedure_list( class_name, cout );
      }
      else if( command == c_cmd_modeller_procedure_remove )
      {
         string class_name( get_parm_val( parameters, c_cmd_modeller_procedure_remove_class_name ) );
         string procedure_name( get_parm_val( parameters, c_cmd_modeller_procedure_remove_procedure_name ) );

         g_model.procedure_remove( class_name, procedure_name );
      }
      else if( command == c_cmd_modeller_procedure_rename )
      {
         string class_name( get_parm_val( parameters, c_cmd_modeller_procedure_rename_class_name ) );
         string old_procedure_name( get_parm_val( parameters, c_cmd_modeller_procedure_rename_old_procedure_name ) );
         string new_procedure_name( get_parm_val( parameters, c_cmd_modeller_procedure_rename_new_procedure_name ) );

         g_model.procedure_rename( class_name, old_procedure_name, new_procedure_name );
      }
      else if( command == c_cmd_modeller_procedure_arg_add )
      {
         string id( get_parm_val( parameters, c_cmd_modeller_procedure_arg_add_id ) );
         string class_name( get_parm_val( parameters, c_cmd_modeller_procedure_arg_add_class_name ) );
         string procedure_name( get_parm_val( parameters, c_cmd_modeller_procedure_arg_add_procedure_name ) );
         string arg_name( get_parm_val( parameters, c_cmd_modeller_procedure_arg_add_arg_name ) );
         string arg_type( get_parm_val( parameters, c_cmd_modeller_procedure_arg_add_arg_type ) );
         bool is_output( has_parm_val( parameters, c_cmd_modeller_procedure_arg_add_out ) );

         if( id.empty( ) )
            id = unique_id( );

         g_model.procedure_arg_add( id, class_name, procedure_name, arg_name, arg_type, is_output );
      }
      else if( command == c_cmd_modeller_procedure_arg_list )
      {
         string class_name( get_parm_val( parameters, c_cmd_modeller_procedure_arg_list_class_name ) );
         string procedure_name( get_parm_val( parameters, c_cmd_modeller_procedure_arg_list_procedure_name ) );

         g_model.procedure_arg_list( class_name, procedure_name, cout );
      }
      else if( command == c_cmd_modeller_procedure_arg_remove )
      {
         string class_name( get_parm_val( parameters, c_cmd_modeller_procedure_arg_remove_class_name ) );
         string procedure_name( get_parm_val( parameters, c_cmd_modeller_procedure_arg_remove_procedure_name ) );
         string arg_name( get_parm_val( parameters, c_cmd_modeller_procedure_arg_remove_arg_name ) );

         g_model.procedure_arg_remove( class_name, procedure_name, arg_name );
      }
      else if( command == c_cmd_modeller_procedure_arg_rename )
      {
         string class_name( get_parm_val( parameters, c_cmd_modeller_procedure_arg_rename_class_name ) );
         string procedure_name( get_parm_val( parameters, c_cmd_modeller_procedure_arg_rename_procedure_name ) );
         string old_arg_name( get_parm_val( parameters, c_cmd_modeller_procedure_arg_rename_old_arg_name ) );
         string new_arg_name( get_parm_val( parameters, c_cmd_modeller_procedure_arg_rename_new_arg_name ) );

         g_model.procedure_arg_rename( class_name, procedure_name, old_arg_name, new_arg_name );
      }
      else if( command == c_cmd_modeller_relationship_add )
      {
         string id( get_parm_val( parameters, c_cmd_modeller_relationship_add_id ) );
         string fid( get_parm_val( parameters, c_cmd_modeller_relationship_add_fid ) );
         string child_class_name( get_parm_val( parameters, c_cmd_modeller_relationship_add_child_class_name ) );
         string child_field_name( get_parm_val( parameters, c_cmd_modeller_relationship_add_child_field_name ) );
         string parent_class_name( get_parm_val( parameters, c_cmd_modeller_relationship_add_parent_class_name ) );
         bool is_mandatory( has_parm_val( parameters, c_cmd_modeller_relationship_add_mand ) );
         bool is_restrict( has_parm_val( parameters, c_cmd_modeller_relationship_add_restrict ) );
         bool is_unlink( has_parm_val( parameters, c_cmd_modeller_relationship_add_unlink ) );
         string extra( get_parm_val( parameters, c_cmd_modeller_relationship_add_extra ) );
         bool is_transient( has_parm_val( parameters, c_cmd_modeller_relationship_add_transient ) );

         cascade_operation cascade_op( e_cascade_operation_destroy );
         if( is_unlink )
            cascade_op = e_cascade_operation_unlink;
         else if( is_restrict )
            cascade_op = e_cascade_operation_restrict;

         if( id.empty( ) )
            id = unique_id( );

         if( fid.empty( ) )
            fid = unique_id( );

         g_model.relationship_add( id,
          fid, child_class_name, child_field_name, parent_class_name, is_mandatory, is_transient, cascade_op, extra );
      }
      else if( command == c_cmd_modeller_relationship_list )
      {
         g_model.relationship_list( cout );
      }
      else if( command == c_cmd_modeller_relationship_list_parents )
      {
         string child_class_name(
          get_parm_val( parameters, c_cmd_modeller_relationship_list_parents_child_class_name ) );

         g_model.relationship_list_parents( child_class_name, cout );
      }
      else if( command == c_cmd_modeller_relationship_list_children )
      {
         string parent_class_name(
          get_parm_val( parameters, c_cmd_modeller_relationship_list_children_parent_class_name ) );

         g_model.relationship_list_children( parent_class_name, cout );
      }
      else if( command == c_cmd_modeller_relationship_remove )
      {
         string relationship_name( get_parm_val( parameters, c_cmd_modeller_relationship_remove_relationship_name ) );

         g_model.relationship_remove( relationship_name );
      }
      else if( command == c_cmd_modeller_relationship_rename )
      {
         string old_relationship_name( get_parm_val( parameters, c_cmd_modeller_relationship_rename_old_relationship_name ) );
         string new_relationship_name( get_parm_val( parameters, c_cmd_modeller_relationship_rename_new_relationship_name ) );

         g_model.relationship_rename( old_relationship_name, new_relationship_name );
      }
      else if( command == c_cmd_modeller_specification_add )
      {
         string id( get_parm_val( parameters, c_cmd_modeller_specification_add_id ) );
         string specification_name( get_parm_val( parameters, c_cmd_modeller_specification_add_specification_name ) );
         string specification_type( get_parm_val( parameters, c_cmd_modeller_specification_add_specification_type ) );
         string specification_args( get_parm_val( parameters, c_cmd_modeller_specification_add_specification_args ) );

         if( id.empty( ) )
            id = unique_id( );

         g_model.specification_add( id, specification_name, specification_type, specification_args );
      }
      else if( command == c_cmd_modeller_specification_remove )
      {
         string specification_name( get_parm_val( parameters, c_cmd_modeller_specification_remove_specification_name ) );

         g_model.specification_remove( specification_name );
      }
      else if( command == c_cmd_modeller_specification_list )
      {
         g_model.specification_list( cout );
      }
      else if( command == c_cmd_modeller_specification_rename )
      {
         string old_specification_name(
          get_parm_val( parameters, c_cmd_modeller_specification_rename_old_specification_name ) );

         string new_specification_name(
          get_parm_val( parameters, c_cmd_modeller_specification_rename_new_specification_name ) );

         g_model.specification_rename( old_specification_name, new_specification_name );
      }
      else if( command == c_cmd_modeller_user_defined_enum_add )
      {
         string id( get_parm_val( parameters, c_cmd_modeller_user_defined_enum_add_id ) );
         string ude_name( get_parm_val( parameters, c_cmd_modeller_user_defined_enum_add_ude_name ) );
         string type_name( get_parm_val( parameters, c_cmd_modeller_user_defined_enum_add_type_name ) );
         string key_values( get_parm_val( parameters, c_cmd_modeller_user_defined_enum_add_key_values ) );

         if( id.empty( ) )
            id = unique_id( );

         if( is_string_type( type_name ) )
            throw runtime_error( "string type enums are not currently supported" );

         g_model.user_defined_enum_add( id, ude_name, type_name, key_values );
      }
      else if( command == c_cmd_modeller_user_defined_enum_list )
      {
         g_model.user_defined_enum_list( cout );
      }
      else if( command == c_cmd_modeller_user_defined_enum_remove )
      {
         string ude_name( get_parm_val( parameters, c_cmd_modeller_user_defined_enum_remove_ude_name ) );

         g_model.user_defined_enum_remove( ude_name );
      }
      else if( command == c_cmd_modeller_user_defined_enum_rename )
      {
         string old_ude_name( get_parm_val( parameters, c_cmd_modeller_user_defined_enum_rename_old_ude_name ) );
         string new_ude_name( get_parm_val( parameters, c_cmd_modeller_user_defined_enum_rename_new_ude_name ) );

         g_model.user_defined_enum_rename( old_ude_name, new_ude_name );
      }
      else if( command == c_cmd_modeller_user_defined_type_add )
      {
         string id( get_parm_val( parameters, c_cmd_modeller_user_defined_type_add_id ) );
         string udt_name( get_parm_val( parameters, c_cmd_modeller_user_defined_type_add_udt_name ) );
         string type_name( get_parm_val( parameters, c_cmd_modeller_user_defined_type_add_type_name ) );
         string type_domain_info( get_parm_val( parameters, c_cmd_modeller_user_defined_type_add_type_domain_info ) );

         if( id.empty( ) )
            id = unique_id( );

         g_model.user_defined_type_add( id, udt_name, type_name, type_domain_info );
      }
      else if( command == c_cmd_modeller_user_defined_type_list )
      {
         g_model.user_defined_type_list( cout );
      }
      else if( command == c_cmd_modeller_user_defined_type_remove )
      {
         string udt_name( get_parm_val( parameters, c_cmd_modeller_user_defined_type_remove_udt_name ) );

         g_model.user_defined_type_remove( udt_name );
      }
      else if( command == c_cmd_modeller_user_defined_type_rename )
      {
         string old_udt_name( get_parm_val( parameters, c_cmd_modeller_user_defined_type_rename_old_udt_name ) );
         string new_udt_name( get_parm_val( parameters, c_cmd_modeller_user_defined_type_rename_new_udt_name ) );

         g_model.user_defined_type_rename( old_udt_name, new_udt_name );
      }
      else if( command == c_cmd_modeller_model_info )
      {
         bool has_new_id( has_parm_val( parameters, c_cmd_modeller_model_info_new_id ) );
         bool has_new_name( has_parm_val( parameters, c_cmd_modeller_model_info_new_name ) );
         bool has_new_perm( has_parm_val( parameters, c_cmd_modeller_model_info_new_perm ) );
         bool has_new_version( has_parm_val( parameters, c_cmd_modeller_model_info_new_version ) );
         bool has_new_created( has_parm_val( parameters, c_cmd_modeller_model_info_new_created ) );

         if( has_new_id )
            g_model.set_id( get_parm_val( parameters, c_cmd_modeller_model_info_new_id ) );

         if( has_new_name )
            g_model.set_name( get_parm_val( parameters, c_cmd_modeller_model_info_new_name ) );

         if( has_new_perm )
            g_model.set_perm( get_parm_val( parameters, c_cmd_modeller_model_info_new_perm ) );

         if( has_new_version )
            g_model.set_version( get_parm_val( parameters, c_cmd_modeller_model_info_new_version ) );

         if( has_new_created )
            g_model.set_year_created(
             atoi( get_parm_val( parameters, c_cmd_modeller_model_info_new_created ).c_str( ) ) );

         cout
          << "id: " << g_model.get_id( ) << '\n'
          << "name: " << g_model.get_name( ) << '\n'
          << "perm: " << g_model.get_perm( ) << '\n'
          << "version: " << g_model.get_version( ) << '\n'
          << "created: " << g_model.get_year_created( ) << '\n';
      }
      else if( command == c_cmd_modeller_model_check )
      {
         g_model.check( );
         cout << "okay\n";
      }
      else if( command == c_cmd_modeller_model_load )
      {
         string file_name( get_parm_val( parameters, c_cmd_modeller_model_load_name ) );

         file_name += get_model_file_ext( );

         g_model.load( file_name );
      }
      else if( command == c_cmd_modeller_model_save )
      {
         string file_name( g_model.get_name( ) );

         if( file_name.empty( ) )
            throw runtime_error( "a model name must be set before the model can be saved" );

         file_name += get_model_file_ext( );

         g_model.save( file_name );
      }
      else if( command == c_cmd_modeller_generate )
      {
         string command( get_parm_val( parameters, c_cmd_modeller_generate_command ) );
         string class_name( get_parm_val( parameters, c_cmd_modeller_generate_class_name ) );

         g_model.check( );

         bool has_output_any = false;

         vector< class_data > all_class_data;

         g_model.get_class_data( all_class_data );

         vector< user_defined_enum_data > mod_ude_data;
         vector< user_defined_enum_data > all_ude_data;

         g_model.get_user_defined_enum_data( mod_ude_data );
         g_model.get_user_defined_enum_data( all_ude_data, true );

         vector< user_defined_type_data > all_udt_data;

         g_model.get_user_defined_type_data( all_udt_data, true );

         string spec_file_name( g_model.get_name( ) );

         spec_file_name += c_spec_ext;
         spec_file_name += c_xrep_variables_ext;

         string vars_file_name( g_model.get_name( ) );

         vars_file_name += c_xrep_variables_ext;

         string string_file_name( g_model.get_name( ) );

         string_file_name += c_txt_ext;

         ofstream specf;
         ofstream varsf;
         ofstream stringf;

         if( class_name.empty( ) )
         {
            specf.open( spec_file_name.c_str( ), ios::out );

            if( !specf )
               throw runtime_error( "unable to open file '" + spec_file_name + "' for output" );

            specf << "`{`$model_id`=`'" << g_model.get_id( ) << "`'`}\n";
            specf << "`{`$model_name`=`'" << g_model.get_name( ) << "`'`}\n";
            specf << "`{`$model_perm`=`'" << g_model.get_perm( ) << "`'`}\n";
            specf << "`{`$model_title`=`'" << g_model.get_title( ) << "`'`}\n";
            specf << "`{`$model_version`=`'" << g_model.get_version( ) << "`'`}\n";
            specf << "`{`$year_created`=`'" << to_string( g_model.get_year_created( ) ) << "`'`}\n";
            specf << "`{`}\n";
         }

         if( class_name.empty( ) )
         {
            varsf.open( vars_file_name.c_str( ), ios::out );

            if( !varsf )
               throw runtime_error( "unable to open file '" + vars_file_name + "' for output" );

            varsf << "`{`$model_name`=`'" << g_model.get_name( ) << "`'`}\n";
            varsf << "`{`$model_title`=`'" << g_model.get_title( ) << "`'`}\n";
            varsf << "`{`}\n";
         }

         if( class_name.empty( ) )
         {
            string make_file_name( g_model.get_name( ) );

            make_file_name += c_make_ext;
            make_file_name += c_xrep_variables_ext;

            ofstream makef( make_file_name.c_str( ) );

            if( !makef )
               throw runtime_error( "unable to open file '" + make_file_name + "' for output" );

            stringf.open( string_file_name.c_str( ), ios::out );

            if( !stringf )
               throw runtime_error( "unable to open file '" + string_file_name + "' for output" );

            string title( g_model.get_title( ) );
            replace_char_occurences( title, '_', ' ' );

            stringf << "title \"" << title << "\"\n";
            stringf << "home_title \"" << title << "\"\n";
            stringf << "version \"" << g_model.get_version( ) << "\"\n";

            stringf << "field_key \"Key\"\n";
            stringf << "field_row \"#\"\n";

            vector< string > all_enums;

            for( vector< user_defined_enum_data >::size_type i = 0; i < mod_ude_data.size( ); i++ )
               all_enums.push_back( mod_ude_data[ i ].id + "," + mod_ude_data[ i ].name );

            if( !all_enums.empty( ) )
            {
               specf << "`{`$all_enums`=`'" << string_list( all_enums ) << "`'`}\n";
               specf << "`{`}\n";

               for( vector< user_defined_enum_data >::size_type i = 0; i < mod_ude_data.size( ); i++ )
               {
                  string key_values( mod_ude_data[ i ].key_values );
                  replace_char_occurences( key_values, ',', ' ' );

                  specf << "`{`$enum_" << mod_ude_data[ i ].name << "`=`'" << unescaped( key_values ) << "`'`}\n";

                  string name( "enum_" );
                  name += lower( mod_ude_data[ i ].name );

                  string name_string( mod_ude_data[ i ].name );
                  replace_char_occurences( name_string, '_', ' ' );

                  stringf << name << " \"" << name_string << "\"\n";

                  vector< string > key_data_pairs;
                  split( key_values, key_data_pairs, ' ' );

                  for( size_t j = 0; j < key_data_pairs.size( ); j++ )
                  {
                     string::size_type pos = key_data_pairs[ j ].find( '=' );
                     string label = key_data_pairs[ j ].substr( pos + 1 );

                     pos = label.find( '=' );
                     if( pos != string::npos )
                        label.erase( pos );

                     string label_string( label );
                     replace_char_occurences( label_string, '_', ' ', '\\' );

                     stringf << name << '_' << lower( unescape( label ) ) << " \"" << label_string << "\"\n";
                  }
               }

               specf << "`{`}\n";
            }

            specf << "`{`$all_classes`=`'";

            vector< string > externals;

            externals.push_back( "ciyam_base" );

            g_model.get_external_module_names( externals );

            makef << "`{`$" << g_model.get_name( ) << "_dylk`=`'" << string_list( externals ) << "`'`}\n";

            makef << "`{`$" << g_model.get_name( ) << "_cpps`=`'" << g_model.get_name( ) << ".cpp";
            if( !all_class_data.empty( ) )
               makef << "\\\n";
            else
               makef << "`'`}\n";

            for( vector< class_data >::size_type i = 0; i < all_class_data.size( ); i++ )
            {
               makef << ' ' << g_model.get_name( ) << '_' << all_class_data[ i ].name << ".cpp";

               if( i > 0 )
                  specf << ' ';
               specf << all_class_data[ i ].name;

               if( i == all_class_data.size( ) - 1 )
                  makef << "`'`}\n";
               else
                  makef << "\\\n";

               string name( lower( all_class_data[ i ].name ) );

               string name_string( all_class_data[ i ].name );
               replace_char_occurences( name_string, '_', ' ' );

               stringf << "class_" << name << " \"" << name_string << "\"\n";

               string plural_string( all_class_data[ i ].plural );
               replace_char_occurences( plural_string, '_', ' ' );

               stringf << "plural_" << name << " \"" << plural_string << "\"\n";

               vector< field_data > all_field_data;
               g_model.get_field_data( all_class_data[ i ].name, all_field_data );
               for( vector< field_data >::size_type j = 0; j < all_field_data.size( ); j++ )
               {
                  string field( all_field_data[ j ].name );
                  string field_string( field );
                  replace_char_occurences( field_string, '_', ' ' );

                  stringf << "field_" << name << '_' << lower( field ) << " \"" << field_string << "\"\n";
               }

               vector< procedure_data > all_procedure_data;
               g_model.get_procedure_data( all_class_data[ i ].name, all_procedure_data );
               for( vector< procedure_data >::size_type j = 0; j < all_procedure_data.size( ); j++ )
               {
                  string procedure( lower( all_procedure_data[ j ].name ) );

                  string procedure_string( all_procedure_data[ j ].name );
                  replace_char_occurences( procedure_string, '_', ' ' );

                  stringf << "procedure_" << name << '_' << procedure << " \"" << procedure_string << "\"\n";
                  stringf << "procedure_" << all_procedure_data[ j ].id << " \"" << procedure_string << "\"\n";
               }

               vector< specification_data > all_spec_data;
               g_model.get_specification_data( all_class_data[ i ].name, all_spec_data );
               for( vector< specification_data >::size_type j = 0; j < all_spec_data.size( ); j++ )
               {
                  if( !all_spec_data[ j ].strings.empty( ) )
                  {
                     string id( all_spec_data[ j ].id );

                     for( size_t k = 0; k < all_spec_data[ j ].strings.size( ); k++ )
                     {
                        string specification_string( all_spec_data[ j ].strings[ k ].second );
                        replace_char_occurences( specification_string, '_', ' ' );

                        stringf << id << "_"
                         << all_spec_data[ j ].strings[ k ].first << " \"" << specification_string << "\"\n";
                     }
                  }
               }
            }

            specf << "`'`}\n";

            // NOTE: Remove "ciyam_base" which was inserted at the start of the vector above.
            externals.erase( externals.begin( ) );

            if( !externals.empty( ) )
               specf << "`{`$all_externals`=`'" << string_list( externals ) << "`'`}\n";

            makef << "`{`$" << g_model.get_name( ) << "_cmss`=`'" << g_model.get_name( ) << ".cms";
            if( !all_class_data.empty( ) )
               makef << "\\\n";
            else
               makef << "`'`}\n";

            for( vector< class_data >::size_type i = 0; i < all_class_data.size( ); i++ )
            {
               makef << ' ' << g_model.get_name( ) << '_' << all_class_data[ i ].name << ".cms";

               if( i == all_class_data.size( ) - 1 )
                  makef << "`'`}\n";
               else
                  makef << "\\\n";
            }

            makef.flush( );

            if( !makef.good( ) )
               throw runtime_error( "unexpected error flushing output file '" + make_file_name + "'" );

            if( !modeller_handler.has_option_quiet( ) )
               cout << "generated '" << make_file_name << "'" << endl;

            if( !modeller_handler.has_option_quiet( ) )
               cout << "generated '" << spec_file_name << "'" << endl;
         }

         for( vector< class_data >::size_type i = 0; i < all_class_data.size( ); i++ )
         {
            string next_class_id( all_class_data[ i ].id );
            string next_class_name( all_class_data[ i ].name );
            string next_class_extra( all_class_data[ i ].extra );
            string next_plural_name( all_class_data[ i ].plural );
            string next_fixed_key_val( all_class_data[ i ].fixed_key_val );
            string next_base_class_name( all_class_data[ i ].base_class_name );

            string group_field, order_field;

            string class_file_name( g_model.get_name( ) );

            class_file_name += '_';
            class_file_name += next_class_name;
            class_file_name += c_cpp_ext;

            if( class_name.empty( ) || class_name == next_class_name )
            {
               get_specifications_type gstype( e_get_specifications_type_non_interface );
               if( class_name.empty( ) )
                  gstype = e_get_specifications_type_interface_and_facets_only;

               vector< specification_data > all_spec_data;
               g_model.get_specification_data( next_class_name, all_spec_data, gstype );

               if( class_name.empty( ) )
               {
                  for( size_t i = 0; i < all_spec_data.size( ); i++ )
                  {
                     if( !all_spec_data[ i ].data_pairs.empty( ) )
                     {
                        specf << "`{`}\n";
                        for( size_t j = 0; j < all_spec_data[ i ].data_pairs.size( ); j++ )
                        {
                           specf
                            << "`{`$specification_" << all_spec_data[ i ].name
                            << '_' << all_spec_data[ i ].data_pairs[ j ].first
                            << "`=`'" << all_spec_data[ i ].data_pairs[ j ].second << "`'`}\n";
                        }
                     }
                  }
               }

               if( !command.empty( ) )
               {
                  ostringstream osstr;

                  osstr << command << ' ' << g_model.get_name( ) << ' ' << next_class_name;

                  if( system( osstr.str( ).c_str( ) ) != 0 )
                     throw runtime_error( "unexpected system failure" );
                  else if( !modeller_handler.has_option_quiet( ) )
                     cout << "executed: " << osstr.str( ) << endl;

                  continue;
               }

               string file_name( g_model.get_name( ) );

               file_name += '_';
               file_name += next_class_name;
               file_name += c_xrep_variables_ext;

               ofstream outf( file_name.c_str( ) );

               if( !outf )
                  throw runtime_error( "unable to open file '" + file_name + "' for output" );

               has_output_any = true;

               outf << "`{`$class_id`=`'" << next_class_id << "`'`}\n";
               outf << "`{`$class_name`=`'" << next_class_name << "`'`}\n";
               outf << "`{`$plural_name`=`'" << next_plural_name << "`'`}\n";
               outf << "`{`$class_version`=`'1`'`}\n";

               string::size_type pos = next_class_extra.find( "local" );

               if( pos == 0 )
               {
                  outf << "`{`$persistence_type`=`'1`'`}\n";

                  pos = next_class_extra.find( ':' );

                  if( pos != string::npos )
                     outf << "`{`$persistence_extra`=`'" << next_class_extra.substr( pos + 1 ) << "`'`}\n";
               }

               pos = next_class_extra.find( "global" );

               if( pos == 0 )
               {
                  outf << "`{`$persistence_type`=`'2`'`}\n";

                  pos = next_class_extra.find( ':' );

                  if( pos != string::npos )
                     outf << "`{`$persistence_extra`=`'" << next_class_extra.substr( pos + 1 ) << "`'`}\n";
               }

               pos = next_class_extra.find( "sysvar" );

               if( pos == 0 )
               {
                  outf << "`{`$persistence_type`=`'3`'`}\n";

                  pos = next_class_extra.find( ':' );

                  if( pos != string::npos )
                     outf << "`{`$persistence_extra`=`'" << next_class_extra.substr( pos + 1 ) << "`'`}\n";
               }

               outf << "`{`$module_id`=`'" << g_model.get_id( ) << "`'`}\n";
               outf << "`{`$module_name`=`'" << g_model.get_name( ) << "`'`}\n";
               outf << "`{`$year_created`=`'" << to_string( g_model.get_year_created( ) ) << "`'`}\n";
               outf << "`{`}\n";

               vector< pair< string, pair< string, string > > > base_field_info;

               vector< field_data > all_field_data_with_base;

               g_model.get_field_data( next_class_name, all_field_data_with_base, e_get_field_type_any, true );

               vector< field_data > all_field_data;

               g_model.get_field_data( next_class_name, all_field_data );

               map< string, string > all_field_ids_and_names;

               for( int j = 0; j < all_field_data.size( ); j++ )
                  all_field_ids_and_names.insert( make_pair( all_field_data[ j ].id, all_field_data[ j ].name ) );

               for( int j = 0; j < all_field_data_with_base.size( ); j++ )
               {
                  if( !all_field_ids_and_names.count( all_field_data_with_base[ j ].id ) )
                     base_field_info.push_back( make_pair( all_field_data_with_base[ j ].id,
                      make_pair( all_field_data_with_base[ j ].name, all_field_data_with_base[ j ].class_name ) ) );
               }

               vector< field_data > parent_field_data;

               g_model.get_field_data( next_class_name, parent_field_data, e_get_field_type_relationships );

               map< string, string > field_names_and_raw_types;

               vector< string > enum_fields;
               vector< string > basic_fields;
               vector< string > parent_fields;
               vector< string > complex_fields;
               vector< string > encrypted_fields;
               vector< string > transient_fields;
               vector< string > commandable_fields;
               vector< string > sql_numeric_fields;
               vector< string > text_search_fields;

               vector< string > domain_fields;
               vector< string > domain_type_names;

               vector< string > mandatory_normal_fields;
               vector< string > mandatory_parent_fields;

               vector< string > child_types;
               vector< string > parent_types;
               vector< string > customised_types;

               set< string > all_base_related_classes;

               g_model.get_all_base_related_classes( next_class_name, all_base_related_classes );

               map< string, string > user_defined_enum_types;

               for( vector< user_defined_enum_data >::size_type i = 0; i < all_ude_data.size( ); i++ )
                  user_defined_enum_types.insert( make_pair( all_ude_data[ i ].name, all_ude_data[ i ].type ) );

               vector< string > user_defined_enums;

               for( vector< field_data >::size_type i = 0; i < parent_field_data.size( ); i++ )
               {
                  string parent_field_name( parent_field_data[ i ].name );
                  string parent_field_type( parent_field_data[ i ].type );

                  parent_fields.push_back( parent_field_name );

                  if( parent_field_data[ i ].is_mandatory )
                     mandatory_parent_fields.push_back( parent_field_name );

                  if( find( parent_types.begin( ), parent_types.end( ), parent_field_type ) == parent_types.end( ) )
                     parent_types.push_back( parent_field_type );
               }

               // NOTE: If no fields are present then it's a assumed that the class is an external alias.
               if( all_field_data.empty( ) )
                  outf << "`{`$is_alias`=`'true`'`}\n";
               else
                  outf << "`{`$is_alias`=`'false`'`}\n";

               string sql_columns;

               if( !all_field_data.empty( ) )
               {
                  sql_columns += "C_Key_ VARCHAR(" + to_string( c_max_key_length ) + "),\n";
                  sql_columns += "C_Ver_ SMALLINT UNSIGNED NOT NULL,\n";
                  sql_columns += "C_Rev_ BIGINT UNSIGNED NOT NULL,\n";
                  sql_columns += "C_Sec_ BIGINT UNSIGNED NOT NULL,\n";
                  sql_columns += "C_Typ_ VARCHAR(16) NOT NULL";

                  if( !next_base_class_name.empty( ) )
                  {
                     vector< field_data > base_field_data;
                     g_model.get_field_data( next_base_class_name, base_field_data );

                     for( vector< field_data >::size_type i = 0; i < base_field_data.size( ); i++ )
                     {
                        if( base_field_data[ i ].is_transient )
                           continue;

                        string field_name( base_field_data[ i ].name );

                        if( sql_columns.empty( ) )
                           sql_columns += "C_";
                        else
                           sql_columns += ",\nC_";

                        bool is_large_text = false;

                        if( base_field_data[ i ].extra.find( "text" ) != string::npos
                         || base_field_data[ i ].extra.find( "notes" ) != string::npos
                         || base_field_data[ i ].extra.find( "content" ) != string::npos
                         || base_field_data[ i ].extra.find( "user_perms" ) != string::npos )
                           is_large_text = true;

                        // KLUDGE: Due to limits in different versions of MariaDB row sizes
                        // can exceed the maxiumum permitted so use VARCHAR(256) where it's
                        // possible for tables with a large number of columns.
                        bool is_large_table = false;

                        if( ( i > 25 )
                         && ( field_name.find( "Name" ) != 0 )
                         && ( field_name.find( "View" ) != 0 )
                         && ( field_name.find( "Model" ) != 0 )
                         && ( field_name.find( "Order" ) != 0 )
                         && ( field_name.find( "Parent" ) != 0 )
                         && ( field_name.find( "Package" ) != 0 )
                         && ( field_name.find( "Workgroup" ) != 0 )
                         && ( field_name.find( "Specification" ) != 0 ) )
                           is_large_table = true;

                        bool is_sql_numeric;

                        sql_columns += field_name;

                        sql_columns += " " + get_sql_type(
                         base_field_data[ i ].sys_type, base_field_data[ i ].is_mandatory,
                         base_field_data[ i ].is_foreign_key, &is_sql_numeric, is_large_text, is_large_table );
                     }
                  }
               }

               for( vector< field_data >::size_type i = 0; i < all_field_data.size( ); i++ )
               {
                  if( i == 0 )
                     outf << "`{`$all_fields`=`'\\\n";

                  string field_id( all_field_data[ i ].id );
                  string field_name( all_field_data[ i ].name );
                  string field_type( all_field_data[ i ].type );

                  if( all_field_data[ i ].extra.find( "group" ) != string::npos )
                     group_field = field_name;

                  if( all_field_data[ i ].extra.find( "order" ) != string::npos )
                     order_field = field_name;

                  string enum_name( all_field_data[ i ].enum_name );

                  if( all_field_data[ i ].extra.find( "encrypted" ) != string::npos )
                     encrypted_fields.push_back( field_id + "," + field_name );

                  if( all_field_data[ i ].is_transient )
                     transient_fields.push_back( field_id + "," + field_name );
                  else
                  {
                     if( sql_columns.empty( ) )
                        sql_columns += "C_";
                     else
                        sql_columns += ",\nC_";

                     bool is_large_text = false;

                     if( all_field_data[ i ].extra.find( "text" ) != string::npos
                      || all_field_data[ i ].extra.find( "notes" ) != string::npos
                      || all_field_data[ i ].extra.find( "content" ) != string::npos
                      || all_field_data[ i ].extra.find( "user_perms" ) != string::npos )
                        is_large_text = true;

                     // KLUDGE: (see above)
                     bool is_large_table = false;

                     if( ( i > 25 )
                      && ( field_name.find( "Name" ) != 0 )
                      && ( field_name.find( "View" ) != 0 )
                      && ( field_name.find( "Model" ) != 0 )
                      && ( field_name.find( "Order" ) != 0 )
                      && ( field_name.find( "Parent" ) != 0 )
                      && ( field_name.find( "Package" ) != 0 )
                      && ( field_name.find( "Workgroup" ) != 0 )
                      && ( field_name.find( "Specification" ) != 0 ) )
                        is_large_table = true;

                     bool is_sql_numeric;

                     sql_columns += field_name;

                     sql_columns += " " + get_sql_type(
                      all_field_data[ i ].sys_type, all_field_data[ i ].is_mandatory,
                      all_field_data[ i ].is_foreign_key, &is_sql_numeric, is_large_text, is_large_table );

                     if( is_sql_numeric )
                        sql_numeric_fields.push_back( field_name );

                     if( all_field_data[ i ].for_text_search )
                        text_search_fields.push_back( field_name );
                  }

                  if( !enum_name.empty( ) )
                  {
                     enum_fields.push_back( field_name + ',' + enum_name );

                     if( find( user_defined_enums.begin( ), user_defined_enums.end( ), enum_name ) == user_defined_enums.end( ) )
                        user_defined_enums.push_back( enum_name );
                  }

                  bool is_domain_aggregate = false;
                  for( vector< user_defined_type_data >::size_type j = 0; j < all_udt_data.size( ); j++ )
                  {
                     if( field_type == all_udt_data[ j ].id )
                     {
                        domain_fields.push_back( field_name );

                        string domain_type( all_udt_data[ j ].domain_type );

                        string next_type( all_udt_data[ j ].type );

                        while( true )
                        {
                           bool found = false;

                           // FUTURE: This could be done more efficiently by using a map.
                           for( vector< user_defined_type_data >::size_type k = 0; k < all_udt_data.size( ); k++ )
                           {
                              if( next_type == all_udt_data[ k ].name )
                              {
                                 found = true;
                                 is_domain_aggregate = true;

                                 next_type = all_udt_data[ k ].type;

                                 domain_type += ",\n ";
                                 domain_type += all_udt_data[ k ].domain_type;

                                 break;
                              }
                           }

                           if( !found )
                           {
                              field_type = next_type;

                              break;
                           }
                        }

                        if( is_domain_aggregate )
                           domain_type = "aggregate_domain< " + field_type + ",\n " + domain_type + " >";

                        domain_type_names.push_back( domain_type );

                        break;
                     }
                  }

                  string raw_field_type( get_raw_type( field_type, all_field_data[ i ].is_mandatory ) );

                  field_names_and_raw_types.insert( make_pair( field_name, raw_field_type ) );

                  if( find( parent_fields.begin( ), parent_fields.end( ), field_name ) == parent_fields.end( ) )
                  {
                     if( is_complex_type( field_type ) )
                     {
                        complex_fields.push_back( field_name );

                        if( all_field_data[ i ].is_mandatory && is_nullable_type( field_type ) )
                           mandatory_normal_fields.push_back( field_name );
                     }
                     else
                        basic_fields.push_back( field_name );

                     if( is_commandable_type( field_type ) )
                     {
                        string field_name_and_type( field_name );

                        field_name_and_type += ',';
                        field_name_and_type += raw_field_type;

                        commandable_fields.push_back( field_name_and_type );
                     }

                     if( is_customised_raw_type( raw_field_type ) )
                     {
                        if( find( customised_types.begin( ), customised_types.end( ), raw_field_type ) == customised_types.end( ) )
                           customised_types.push_back( raw_field_type );
                     }
                  }

                  outf << field_name << ',' << raw_field_type << ',' << all_field_data[ i ].id;

                  if( i == all_field_data.size( ) - 1 )
                     outf << "`'`}";
                  else
                     outf << " \\";

                  outf << "\n";
               }

               if( !sql_columns.empty( ) )
                  sql_columns += ",\nPRIMARY KEY(C_Key_)";

               for( vector< field_data >::size_type i = 0; i < all_field_data.size( ); i++ )
               {
                  if( !all_field_data[ i ].default_value.empty( ) )
                  {
                     string default_value( all_field_data[ i ].default_value );

                     if( is_string_type( all_field_data[ i ].sys_type ) )
                        default_value = '"' + default_value + '"';

                     outf << "`{`$field_default_" << all_field_data[ i ].name << "`=`'" << default_value << "`'`}\n";
                  }
               }

               if( !group_field.empty( ) )
                  outf << "`{`$group_field`=`'" << group_field << "`'`}\n";

               if( !order_field.empty( ) )
                  outf << "`{`$order_field`=`'" << order_field << "`'`}\n";

               if( !basic_fields.empty( ) )
                  outf << "`{`$basic_fields`=`'" << string_list( basic_fields ) << "`'`}\n";

               if( !parent_fields.empty( ) )
                  outf << "`{`$parent_fields`=`'" << string_list( parent_fields ) << "`'`}\n";

               if( !encrypted_fields.empty( ) )
                  outf << "`{`$encrypted_fields`=`'" << string_list( encrypted_fields ) << "`'`}\n";

               if( !transient_fields.empty( ) )
                  outf << "`{`$transient_fields`=`'" << string_list( transient_fields ) << "`'`}\n";

               if( !complex_fields.empty( ) )
                  outf << "`{`$complex_fields`=`'" << string_list( complex_fields ) << "`'`}\n";

               if( !sql_numeric_fields.empty( ) )
                  outf << "`{`$sql_numeric_fields`=`'" << string_list( sql_numeric_fields ) << "`'`}\n";

               if( !text_search_fields.empty( ) )
                  outf << "`{`$text_search_fields`=`'" << string_list( text_search_fields ) << "`'`}\n";

               if( !domain_fields.empty( ) )
               {
                  outf << "`{`}\n";

                  outf << "`{`$domain_fields`=`'" << string_list( domain_fields ) << "`'`}\n";

                  for( size_t i = 0; i < domain_fields.size( ); i++ )
                     outf << "`{`$domain_type_" << domain_fields[ i ] << "`=`'" << domain_type_names[ i ] << "`'`}\n";
               }

               if( !commandable_fields.empty( ) )
               {
                  outf << "`{`}\n";
                  outf << "`{`$commandable_fields`=`'" << string_list( commandable_fields ) << "`'`}\n";
               }

               if( !mandatory_normal_fields.empty( ) || !mandatory_parent_fields.empty( ) )
               {
                  outf << "`{`}\n";

                  if( !mandatory_normal_fields.empty( ) )
                     outf << "`{`$mandatory_normal_fields`=`'" << string_list( mandatory_normal_fields ) << "`'`}\n";

                  if( !mandatory_parent_fields.empty( ) )
                     outf << "`{`$mandatory_parent_fields`=`'" << string_list( mandatory_parent_fields ) << "`'`}\n";
               }

               if( !base_field_info.empty( ) )
               {
                  outf << "`{`}\n";

                  outf << "`{`$base_fields`=`'\\";

                  for( size_t i = 0; i < base_field_info.size( ); i++ )
                  {
                     if( i > 0 )
                        outf << " \\";
                     outf << "\n" << base_field_info[ i ].second.first
                      << "," << base_field_info[ i ].first << "," << base_field_info[ i ].second.second;
                  }

                  outf << "`'`}\n";
               }

               if( !all_base_related_classes.empty( ) )
               {
                  outf << "`{`}\n";
                  outf << "`{`$base_related_types`=`'" << string_list( all_base_related_classes ) << "`'`}\n";
               }

               vector< child_relationship_data > child_rel_info;

               g_model.get_child_relationship_data( next_class_name, child_rel_info );

               if( !child_rel_info.empty( ) )
               {
                  outf << "`{`}\n";
                  outf << "`{`$child_info`=`'\\\n";

                  for( vector< child_relationship_data >::size_type i = 0; i < child_rel_info.size( ); i++ )
                  {
                     string relationship_name( child_rel_info[ i ].name );

                     string child_class_name( child_rel_info[ i ].child_class_name );

                     string child_full_entity_name( g_model.get_name( ) );
                     child_full_entity_name += '_';
                     child_full_entity_name += child_class_name;

                     if( find( child_types.begin( ), child_types.end( ), child_full_entity_name ) == child_types.end( ) )
                        child_types.push_back( child_full_entity_name );

                     string cascade_op_name( child_rel_info[ i ].cascade_op_name );

                     string rel_without_child;
                     string child_rel_suffix( g_model.determine_child_rel_suffix(
                      child_rel_info[ i ].decorated_name, &rel_without_child ) );

                     // NOTE: The "rel_without_child" should be the child's foreign key field to the
                     // parent so its identity is looked up here (if not found then raise an error).
                     vector< field_data > all_child_field_data;
                     g_model.get_field_data( child_class_name, all_child_field_data );

                     string child_fk_field_id;
                     for( size_t j = 0; j < all_child_field_data.size( ); j++ )
                     {
                        if( all_child_field_data[ j ].name == rel_without_child )
                           child_fk_field_id = all_child_field_data[ j ].id;
                     }

                     if( child_fk_field_id.empty( ) )
                        throw runtime_error( "unable to find field '" + rel_without_child + "' in class '" + child_class_name + "'" );

                     outf << "child_" << child_rel_suffix << ','
                      << child_full_entity_name << ',' << rel_without_child << ',' << child_fk_field_id << ',' << cascade_op_name;

                     if( i == child_rel_info.size( ) - 1 )
                        outf << "`'`}";
                     else
                        outf << " \\";

                     outf << "\n";
                  }
               }

               if( !child_types.empty( ) || !parent_types.empty( ) || !customised_types.empty( ) )
               {
                  outf << "`{`}\n";

                  if( !child_types.empty( ) )
                     outf << "`{`$child_types`=`'" << string_list( child_types ) << "`'`}\n";

                  if( !parent_types.empty( ) )
                     outf << "`{`$parent_types`=`'" << string_list( parent_types ) << "`'`}\n";

                  if( !customised_types.empty( ) )
                     outf << "`{`$customised_types`=`'" << string_list( customised_types ) << "`'`}\n";
               }

               if( !user_defined_enums.empty( ) )
               {
                  string all_enums;
                  for( size_t i = 0; i < user_defined_enums.size( ); i++ )
                  {
                     if( !all_enums.empty( ) )
                        all_enums += " ";

                     all_enums += user_defined_enums[ i ];
                     all_enums += ',' + user_defined_enum_types[ user_defined_enums[ i ] ];
                     if( is_string_type( user_defined_enum_types[ user_defined_enums[ i ] ] ) )
                        all_enums += ",strlit";
                     else
                        all_enums += ",numlit";
                  }

                  outf << "`{`}\n";
                  outf << "`{`$all_enums`=`'" << all_enums << "`'`}\n";
                  outf << "`{`$enum_fields`=`'" << string_list( enum_fields ) << "`'`}\n";

                  set< string > enum_set( user_defined_enums.begin( ), user_defined_enums.end( ) );

                  outf << "`{`}\n";
                  for( size_t i = 0; i < all_ude_data.size( ); i++ )
                  {
                     string key_values( all_ude_data[ i ].key_values );
                     replace_char_occurences( key_values, ',', ' ' );

                     if( enum_set.count( all_ude_data[ i ].name ) )
                        outf << "`{`$enum_" << all_ude_data[ i ].name << "`=`'" << unescaped( key_values ) << "`'`}\n";
                  }
               }

               if( !next_fixed_key_val.empty( ) )
               {
                  outf << "`{`}\n";
                  outf << "`{`$fixed_key`=`'" << next_fixed_key_val << "`'`}\n";
               }

               if( !next_base_class_name.empty( ) )
               {
                  outf << "`{`}\n";

                  string::size_type pos = next_base_class_name.find( ':' );
                  if( pos == string::npos )
                     outf << "`{`$parent_class`=`'" << g_model.get_name( ) << '_' << next_base_class_name << "`'`}\n";
                  else
                     outf << "`{`$parent_class`=`'"
                      << next_base_class_name.substr( 0, pos ) << '_' << next_base_class_name.substr( pos + 1 ) << "`'`}\n";
               }

               outf << "`{`}\n";
               outf << "`{`$sql_columns`=`'\\\n" << sql_columns << "`'`}\n";

               vector< string > procedure_info;
               vector< string > procedure_names;

               vector< index_data > all_index_data;
               g_model.get_index_data( next_class_name, all_index_data );

               sort( all_index_data.begin( ), all_index_data.end( ) );

               string all_indexes;
               string all_sql_indexes;
               string all_unique_indexes;
               string all_sql_unique_indexes;

               for( size_t i = 0; i < all_index_data.size( ); i++ )
               {
                  string field_types;
                  string field_names( all_index_data[ i ].name );

                  vector< string > fields;
                  split( field_names, fields );

                  if( !all_sql_indexes.empty( ) )
                     all_sql_indexes += " \\\n";

                  if( all_index_data[ i ].is_unique && !all_sql_unique_indexes.empty( ) )
                     all_sql_unique_indexes += ' ';

                  for( size_t j = 0; j < fields.size( ); j++ )
                  {
                     if( j > 0 )
                     {
                        field_types += ',';
                        all_sql_indexes += ',';

                        if( all_index_data[ i ].is_unique )
                           all_sql_unique_indexes += ',';
                     }

                     all_sql_indexes += "C_" + fields[ j ];

                     if( all_index_data[ i ].is_unique )
                        all_sql_unique_indexes += "C_" + fields[ j ];

                     // NOTE: Change the type of an index field to "string" if it's a parent field.
                     if( find( parent_fields.begin( ), parent_fields.end( ), fields[ j ] ) == parent_fields.end( ) )
                        field_types += field_names_and_raw_types[ fields[ j ] ];
                     else
                        field_types += "string";
                  }

                  // NOTE: For non-unique indexes add a special "@pk" field of type "string".
                  if( !all_index_data[ i ].is_unique )
                  {
                     field_names += ",@pk";
                     field_types += ",string";
                     all_sql_indexes += ",C_Key_";
                  }

                  if( !all_indexes.empty( ) )
                     all_indexes += ' ';
                  all_indexes += field_names + ';' + field_types;

                  if( all_index_data[ i ].is_unique )
                  {
                     if( !all_unique_indexes.empty( ) )
                        all_unique_indexes += ' ';
                     all_unique_indexes += field_names;
                  }
               }

               if( !all_indexes.empty( ) )
               {
                  outf << "`{`}\n";
                  outf << "`{`$all_indexes`=`'" << all_indexes << "`'`}\n";
                  outf << "`{`$all_unique_indexes`=`'" << all_unique_indexes << "`'`}\n";

                  outf << "`{`$all_sql_indexes`=`'\\\n" << all_sql_indexes << "`'`}\n";
                  outf << "`{`$all_sql_unique_indexes`=`'" << all_sql_unique_indexes << "`'`}\n";
               }

               vector< modifier_data > all_modifier_data;
               g_model.get_modifier_data( next_class_name, all_modifier_data );

               if( !all_modifier_data.empty( ) )
               {
                  outf << "`{`}\n";
                  outf << "`{`$all_modifiers`=`'";
                  for( size_t i = 0; i < all_modifier_data.size( ); i++ )
                  {
                     if( i > 0 )
                        outf << ' ';
                     outf << all_modifier_data[ i ].name << ',' << all_modifier_data[ i ].value;
                  }
                  outf << "`'`}\n";
               }

               vector< procedure_data > all_procedure_data;
               g_model.get_procedure_data( next_class_name, all_procedure_data );

               for( size_t i = 0; i < all_procedure_data.size( ); i++ )
               {
                  procedure_names.push_back( all_procedure_data[ i ].name );
                  procedure_info.push_back( all_procedure_data[ i ].id + ',' + all_procedure_data[ i ].name );
               }

               if( !procedure_names.empty( ) )
               {
                  outf << "`{`}\n";
                  outf << "`{`$all_procedures`=`'" << string_list( procedure_names ) << "`'`}\n";
                  outf << "`{`$all_procedure_info`=`'" << string_list( procedure_info ) << "`'`}\n";

                  for( vector< string >::size_type i = 0; i < procedure_names.size( ); i++ )
                  {
                     string next_procedure_name( procedure_names[ i ] );
                     vector< procedure_arg_data > all_procedure_arg_data;

                     g_model.get_procedure_arg_data( next_class_name, next_procedure_name, all_procedure_arg_data );

                     string all_input_args, all_output_args;
                     string all_simple_inputs, all_complex_inputs;

                     outf << "`{`}\n";
                     outf << "`{`$" << next_procedure_name << "_all_args`=`'";
                     for( vector< procedure_arg_data >::size_type j = 0; j < all_procedure_arg_data.size( ); j++ )
                     {
                        if( j > 0 )
                           outf << ' ';

                        string arg_name( all_procedure_arg_data[ j ].name );
                        string arg_type( all_procedure_arg_data[ j ].type );

                        string raw_type( get_raw_type( arg_type, true ) );

                        string arg_name_and_type( arg_name );
                        arg_name_and_type += ',';
                        arg_name_and_type += raw_type;

                        outf << arg_name_and_type;

                        if( !all_procedure_arg_data[ j ].is_output )
                        {
                           if( !all_input_args.empty( ) )
                              all_input_args += ' ';
                           all_input_args += arg_name_and_type;

                           if( !is_complex_type( arg_type ) )
                           {
                              if( !all_simple_inputs.empty( ) )
                                 all_simple_inputs += ' ';
                              all_simple_inputs += arg_name;
                           }
                           else
                           {
                              if( !all_complex_inputs.empty( ) )
                                 all_complex_inputs += ' ';
                              all_complex_inputs += arg_name;
                           }
                        }
                        else
                        {
                           if( !all_output_args.empty( ) )
                              all_output_args += ' ';
                           all_output_args += arg_name_and_type;
                        }
                     }
                     outf << "`'`}\n";

                     outf << "`{`$" << next_procedure_name << "_all_inputs`=`'" << all_input_args << "`'`}\n";
                     outf << "`{`$" << next_procedure_name << "_all_outputs`=`'" << all_output_args << "`'`}\n";

                     if( !all_procedure_arg_data.empty( ) )
                     {
                        outf << "`{`$" << next_procedure_name << "_all_simple_inputs`=`'" << all_simple_inputs << "`'`}\n";
                        outf << "`{`$" << next_procedure_name << "_all_complex_inputs`=`'" << all_complex_inputs << "`'`}\n";
                     }
                  }
               }

               vector< string > spec_types;
               vector< string > spec_details;

               if( !all_spec_data.empty( ) )
               {
                  for( size_t i = 0; i < all_spec_data.size( ); i++ )
                  {
                     string name( all_spec_data[ i ].name );
                     string type( all_spec_data[ i ].type );

                     string str( name );
                     str += ',';
                     str += type;

                     spec_details.push_back( str );

                     if( find( spec_types.begin( ), spec_types.end( ), type ) == spec_types.end( ) )
                        spec_types.push_back( type );
                  }

                  outf << "`{`}\n";
                  outf << "`{`$all_specifications`=`'" << string_list( spec_details ) << "`'`}\n";
                  outf << "`{`$all_specification_types`=`'" << string_list( spec_types ) << "`'`}\n";

                  for( size_t i = 0; i < all_spec_data.size( ); i++ )
                  {
                     if( !all_spec_data[ i ].data_pairs.empty( ) )
                     {
                        outf << "`{`}\n";
                        for( size_t j = 0; j < all_spec_data[ i ].data_pairs.size( ); j++ )
                        {
                           outf
                            << "`{`$specification_" << all_spec_data[ i ].name
                            << '_' << all_spec_data[ i ].data_pairs[ j ].first
                            << "`=`'" << all_spec_data[ i ].data_pairs[ j ].second << "`'`}\n";
                        }
                     }
                  }
               }

               outf.flush( );

               if( !outf.good( ) )
                  throw runtime_error( "unexpected error flushing output file '" + file_name + "'" );

               if( !modeller_handler.has_option_quiet( ) )
                  cout << "generated '" << file_name << "'" << endl;
            }
         }

         specf.flush( );

         if( !specf.good( ) )
            throw runtime_error( "unexpected error flushing output file '" + spec_file_name + "'" );

         varsf.flush( );

         if( !varsf.good( ) )
            throw runtime_error( "unexpected error flushing output file '" + vars_file_name + "'" );

         stringf.flush( );

         if( !stringf.good( ) )
            throw runtime_error( "unexpected error flushing output file '" + string_file_name + "'" );

         if( !has_output_any && !class_name.empty( ) )
            throw runtime_error( "unknown class name '" + class_name + "'" );
      }
      else
         cout << "warning: unhandled command '" << command << "'" << endl;
   }
   catch( exception& x )
   {
      g_rc = 1;
      cerr << "error: " << x.what( ) << endl;
   }
}

command_functor* modeller_command_functor_factory( const string& /*name*/, command_handler& handler )
{
   return new modeller_command_functor( dynamic_cast< modeller_command_handler& >( handler ) );
}

int main( int argc, char* argv[ ] )
{
   modeller_command_handler cmd_handler;

   try
   {
      // NOTE: Use block scope for startup command processor object...
      {
         startup_command_processor processor( cmd_handler, application_title, 0, argc, argv );

         processor.process_commands( );
      }

      if( !cmd_handler.has_option_quiet( ) )
         cout << application_title( e_app_info_request_title_and_version ) << endl;

      cmd_handler.add_commands( 0,
       modeller_command_functor_factory, ARRAY_PTR_AND_SIZE( modeller_command_definitions ) );

      console_command_processor processor( cmd_handler );
      processor.process_commands( );
   }
   catch( exception& x )
   {
      cerr << "error: " << x.what( ) << endl;
      return 1;
   }
   catch( ... )
   {
      cerr << "error: unexpected exception occurred" << endl;
      return 2;
   }

   return g_rc;
}

