// Copyright (c) 2006-2012 CIYAM Pty. Ltd. ACN 093 704 539
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
#endif

#include "model_specifications.h"

#include "sio.h"
#include "model.h"
#include "utilities.h"

using namespace std;

namespace
{

// KLUDGE: Same constants are in "model.cpp"...
const char* const c_field_extra_order = "order";
const char* const c_class_extra_ordered = "ordered";

const char* const c_field_extra_force_opt = "force_opt";
const char* const c_field_extra_force_mand = "force_mand";

const char* const c_field_extra_permission = "permission";

const char* const c_true = "true";
const char* const c_false = "false";

const char* const c_key_title = "key";
const char* const c_key_field = "@key";
const char* const c_tab_field = "@tab";

const char* const c_row_title = "row";
const char* const c_row_field = "@row";

const char* const c_tab_prefix = "tab";
const char* const c_field_prefix = "field";

const char* const c_arg_perm_prefix = "-perm=";
const char* const c_arg_group_prefix = "-group=";
const char* const c_arg_other_prefix = "-other=";
const char* const c_arg_active_prefix = "-active=";
const char* const c_arg_parent_prefix = "-parent=";
const char* const c_arg_select_prefix = "-select=";

const char* const c_attribute_name = "name";
const char* const c_attribute_type = "type";
const char* const c_attribute_extra = "extra";
const char* const c_attribute_actions = "actions";
const char* const c_attribute_command = "command";
const char* const c_attribute_class_id = "class_id";
const char* const c_attribute_ordering = "ordering";
const char* const c_attribute_arguments = "arguments";
const char* const c_attribute_active_id = "active_id";
const char* const c_attribute_select_id = "select_id";
const char* const c_attribute_gfield_id = "gfield_id";
const char* const c_attribute_ofield_id = "ofield_id";
const char* const c_attribute_pclass_id = "pclass_id";
const char* const c_attribute_pfield_id = "pfield_id";
const char* const c_attribute_uclass_id = "uclass_id";
const char* const c_attribute_ufield_id = "ufield_id";
const char* const c_attribute_field_info = "field_info";
const char* const c_attribute_permission = "permission";
const char* const c_attribute_bclass_info = "bclass_info";
const char* const c_attribute_filter_info = "filter_info";
const char* const c_attribute_parent_info = "parent_info";
const char* const c_attribute_select_perm = "select_perm";
const char* const c_attribute_uid_field_id = "uid_field_id";
const char* const c_attribute_url_field_id = "url_field_id";
const char* const c_attribute_pwd_field_id = "pwd_field_id";
const char* const c_attribute_new_class_id = "new_class_id";
const char* const c_attribute_new_field_id = "new_field_id";
const char* const c_attribute_new_ext_info = "new_ext_info";
const char* const c_attribute_dest_enum_id = "dest_enum_id";
const char* const c_attribute_dest_field_id = "dest_field_id";
const char* const c_attribute_hash_field_id = "hash_field_id";
const char* const c_attribute_name_field_id = "name_field_id";
const char* const c_attribute_perm_field_id = "perm_field_id";
const char* const c_attribute_restrict_info = "restrict_info";
const char* const c_attribute_order_field_id = "order_field_id";
const char* const c_attribute_parent_field_id = "parent_field_id";
const char* const c_attribute_dest_field_value = "dest_field_value";
const char* const c_attribute_select_ofield_id = "select_ofield_id";
const char* const c_attribute_select_pclass_id = "select_pclass_id";
const char* const c_attribute_select_pfield_id = "select_pfield_id";
const char* const c_attribute_select_pcfield_id = "select_pcfield_id";
const char* const c_attribute_checksum_field_id = "checksum_field_id";

const char* const c_data_cid = "cid";
const char* const c_data_pwd = "pwd";
const char* const c_data_uid = "uid";
const char* const c_data_url = "url";
const char* const c_data_acts = "acts";
const char* const c_data_hash = "hash";
const char* const c_data_mods = "mods";
const char* const c_data_name = "name";
const char* const c_data_perm = "perm";
const char* const c_data_type = "type";
const char* const c_data_binfo = "binfo";
const char* const c_data_class = "class";
const char* const c_data_extra = "extra";
const char* const c_data_field = "field";
const char* const c_data_fnums = "fnums";
const char* const c_data_ftype = "ftype";
const char* const c_data_group = "group";
const char* const c_data_fmand = "fmand";
const char* const c_data_fmods = "fmods";
const char* const c_data_fname = "fname";
const char* const c_data_order = "order";
const char* const c_data_prcls = "prcls";
const char* const c_data_prcnm = "prcnm";
const char* const c_data_prfld = "prfld";
const char* const c_data_prfnm = "prfnm";
const char* const c_data_pdfld = "pdfld";
const char* const c_data_prxtr = "prxtr";
const char* const c_data_pridx = "pridx";
const char* const c_data_other = "other";
const char* const c_data_active = "active";
const char* const c_data_column = "column";
const char* const c_data_clname = "clname";
const char* const c_data_cltype = "cltype";
const char* const c_data_clxtra = "clxtra";
const char* const c_data_clmods = "clmods";
const char* const c_data_cluniq = "cluniq";
const char* const c_data_cindex = "cindex";
const char* const c_data_dfenum = "dfenum";
const char* const c_data_dfield = "dfield";
const char* const c_data_dextra = "dextra";
const char* const c_data_dvalue = "dvalue";
const char* const c_data_jfield = "jfield";
const char* const c_data_ofield = "ofield";
const char* const c_data_parent = "parent";
const char* const c_data_pclass = "pclass";
const char* const c_data_pclsnm = "pclsnm";
const char* const c_data_pextra = "pextra";
const char* const c_data_pfield = "pfield";
const char* const c_data_pfldnm = "pfldnm";
const char* const c_data_nclass = "nclass";
const char* const c_data_nfield = "nfield";
const char* const c_data_nindex = "nindex";
const char* const c_data_nextra = "nextra";
const char* const c_data_rfield = "rfield";
const char* const c_data_rfldnm = "rfldnm";
const char* const c_data_rftype = "rftype";
const char* const c_data_rfxtra = "rfxtra";
const char* const c_data_rfmand = "rfmand";
const char* const c_data_sfield = "sfield";
const char* const c_data_sclsnm = "sclsnm";
const char* const c_data_slperm = "slperm";
const char* const c_data_uclass = "uclass";
const char* const c_data_uclsnm = "uclsnm";
const char* const c_data_ufield = "ufield";
const char* const c_data_ufldnm = "ufldnm";
const char* const c_data_command = "command";
const char* const c_data_filters = "filters";
const char* const c_data_pcfield = "pcfield";
const char* const c_data_rfldops = "rfldops";
const char* const c_data_checksum = "checksum";
const char* const c_data_pindexed = "pindexed";
const char* const c_data_arguments = "arguments";
const char* const c_data_mandatory = "mandatory";
const char* const c_data_operations = "operations";
const char* const c_data_permission = "permission";

bool is_non_string_type( const string& field_type )
{
   bool retval = false;

   // FUTURE: These types should not listed both here and in the "modeller".
   if( field_type == "int" || field_type == "bool" || field_type == "numeric" )
      retval = true;

   return retval;
}

string get_id_for_enum_name( model& m, const string& enum_name )
{
   string enum_id;

   vector< user_defined_enum_data > all_ude_data;
   m.get_user_defined_enum_data( all_ude_data, true );

   for( size_t i = 0; i < all_ude_data.size( ); i++ )
   {
      if( all_ude_data[ i ].name == enum_name )
      {
         enum_id = all_ude_data[ i ].id;
         break;
      }
   }

   return enum_id;
}

string get_class_id_for_name( model& m,
 const string& class_name, bool* p_is_external_alias = 0, string* p_base_class_name = 0 )
{
   class_data cls_data;
   m.get_class_data( class_name, cls_data );

   if( p_is_external_alias )
      *p_is_external_alias = cls_data.is_external_alias;

   if( p_base_class_name )
      *p_base_class_name = cls_data.base_class_name;

   return cls_data.id;
}

string get_class_name_for_id( const vector< class_data >& all_class_data, const string& class_id )
{
   string class_name;
   for( size_t i = 0; i < all_class_data.size( ); i++ )
   {
      if( all_class_data[ i ].id == class_id )
      {
         class_name = all_class_data[ i ].name;
         break;
      }
   }

   return class_name;
}

string get_class_name_for_id( model& m, const string& class_id )
{
   vector< class_data > all_class_data;
   m.get_class_data( all_class_data );

   return get_class_name_for_id( all_class_data, class_id );
}

string get_expanded_name( model& m, const string& class_id, const string& name )
{
   string str( name );

   vector< class_data > all_class_data;
   m.get_class_data( all_class_data );

   string class_name, plural_name;
   for( size_t i = 0; i < all_class_data.size( ); i++ )
   {
      if( all_class_data[ i ].id == class_id )
      {
         class_name = all_class_data[ i ].name;
         plural_name = all_class_data[ i ].plural;
         break;
      }
   }

   while( true )
   {
      string::size_type pos = str.find_first_of( "?*" );
      if( pos == string::npos )
         break;

      if( str[ pos ] == '?' )
         str.replace( pos, 1, class_name );
      else if( str[ pos ] == '*' )
         str.replace( pos, 1, plural_name );
   }

   return str;
}

string get_field_id_for_name( const vector< field_data >& all_field_data,
 const string& field_name, string* p_field_type = 0, bool check_decorated_name = false )
{
   string field_id;
   for( size_t i = 0; i < all_field_data.size( ); i++ )
   {
      if( ( !check_decorated_name && field_name == all_field_data[ i ].name )
       || ( check_decorated_name && field_name == all_field_data[ i ].decorated_name ) )
      {
         field_id = all_field_data[ i ].id;
         if( p_field_type )
            *p_field_type = all_field_data[ i ].type;
         break;
      }
   }

   return field_id;
}

string get_field_id_for_name( model& m, const string& class_name,
 const string& field_name, string* p_field_type = 0, bool check_decorated_name = false )
{
   vector< field_data > all_field_data;
   m.get_field_data( class_name, all_field_data );

   return get_field_id_for_name( all_field_data, field_name, p_field_type, check_decorated_name );
}

string get_field_name_for_id( const vector< field_data >& all_field_data, const string& field_id, string* p_field_type = 0 )
{
   string field_name;
   for( size_t i = 0; i < all_field_data.size( ); i++ )
   {
      if( field_id == all_field_data[ i ].id )
      {
         field_name = all_field_data[ i ].name;
         if( p_field_type )
            *p_field_type = all_field_data[ i ].type;
         break;
      }
   }

   return field_name;
}

string get_field_name_for_id( model& m, const string& class_name,
 const string& field_id, string* p_field_type = 0, bool include_base_fields = false )
{
   vector< field_data > all_field_data;
   m.get_field_data( class_name, all_field_data, e_get_field_type_any, include_base_fields );

   return get_field_name_for_id( all_field_data, field_id, p_field_type );
}

string get_class_name_from_field_type( model& m, const string& field_type )
{
   vector< string > module_names;
   module_names.push_back( m.get_name( ) );
   m.get_external_module_names( module_names );

   string module_name;
   for( size_t i = 0; i < module_names.size( ); i++ )
   {
      string::size_type pos = field_type.find( module_names[ i ] );
      if( pos != string::npos && field_type.length( ) >= module_names[ i ].length( ) + 2 )
      {
         module_name = module_names[ i ];
         break;
      }
   }

   if( module_name.empty( ) )
      throw runtime_error( "unexpected field_type '" + field_type + "'" );

   return field_type.substr( module_name.length( ) + 1 );
}

string get_class_name_from_field_type( model& m,
 const string& class_name, const string& field_name, const string& field_type )
{
   vector< string > module_names;
   module_names.push_back( m.get_name( ) );
   m.get_external_module_names( module_names );

   string module_name;
   for( size_t i = 0; i < module_names.size( ); i++ )
   {
      string::size_type pos = field_type.find( module_names[ i ] );
      if( pos != string::npos && field_type.length( ) >= module_names[ i ].length( ) + 2 )
      {
         module_name = module_names[ i ];
         break;
      }
   }

   if( module_name.empty( ) )
      throw runtime_error( "field '" + field_name + "' is not a foreign key in class '" + class_name + "'" );

   return field_type.substr( module_name.length( ) + 1 );
}

string get_class_name_from_field_type( const string& model_name,
 const string& class_name, const string& field_name, const string& field_type )
{
   string::size_type pos = field_type.find( model_name );
   if( pos == string::npos || field_type.length( ) < model_name.length( ) + 2 )
      throw runtime_error( "field '" + field_name + "' is not a foreign key in class '" + class_name + "'" );

   return field_type.substr( model_name.length( ) + 1 );
}

string get_procedure_id_for_name( model& m, const string& class_name, const string& procedure_name )
{
   string procedure_id;

   vector< procedure_data > all_procedure_data;
   m.get_procedure_data( class_name, all_procedure_data );

   for( size_t i = 0; i < all_procedure_data.size( ); i++ )
   {
      if( all_procedure_data[ i ].name == procedure_name )
      {
         procedure_id = all_procedure_data[ i ].id;
         break;
      }
   }

   return procedure_id;
}

void split_extra_from_field( string& field, string& extra, const vector< field_data >& all_field_data, bool is_name )
{
   string excludes;

   string::size_type pos = field.find( '!' );
   if( pos != string::npos )
   {
      string extras( field.substr( pos + 1 ) );
      field.erase( pos );

      pos = extras.find( '-' );
      if( pos != string::npos )
      {
         excludes = extras.substr( pos );
         extras.erase( pos );
      }

      vector< string > all_extras;
      split( extras, all_extras, '+' );

      for( size_t i = 0; i < all_extras.size( ); i++ )
      {
         if( i > 0 )
            extra += '+';

         string next( all_extras[ i ] );

         string::size_type xpos = next.find( '=' );
         if( xpos != string::npos )
         {
            string s, str( next.substr( 0, xpos ) );
            if( is_name )
               s = get_field_id_for_name( all_field_data, str, 0, true );
            else
               s = get_field_name_for_id( all_field_data, str );

            if( s.empty( ) )
               throw runtime_error( "unknown field/id '" + str + "' in extra foreign key restriction" );

            extra += s;
            extra += next.substr( xpos );
         }
         else
         {
            xpos = next.find( ':' );
            if( xpos != string::npos )
            {
               if( next.substr( 0, xpos ) == "@view" )
                  extra += next;
               else
               {
                  string s, str( next.substr( xpos + 1 ) );
                  if( is_name )
                     s = get_field_id_for_name( all_field_data, str, 0, true );
                  else
                     s = get_field_name_for_id( all_field_data, str );

                  if( s.empty( ) )
                     throw runtime_error( "unknown field/id '" + str + "' in extra foreign key restriction" );

                  extra += next.substr( 0, xpos );
                  extra += ':';
                  extra += s;
               }
            }
            else if( !next.empty( ) )
            {
               if( next[ 0 ] != '@' )
               {
                  if( is_name )
                     next = get_field_id_for_name( all_field_data, next, 0, true );
                  else
                     next = get_field_name_for_id( all_field_data, next );

                  if( next.empty( ) )
                     throw runtime_error( "unknown field/id '" + next + "' in extra foreign key restriction" );
               }

               extra += next;
            }
         }
      }

      extra += excludes;
   }
}

}

DECLARE_CLASS_FACTORY( std::string, specification );

struct list_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_non_model; }

   bool has_specification_data_for_class( const string& id ) const { return id == class_id; }

   bool is_interface_specification( ) const { return true; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   void get_specification_strings( model& m, vector< pair< string, string > >& strings ) const;

   static string static_class_name( );

   string name;
   string type;
   string extra;
   string actions;
   string class_id;
   string ordering;
   string bclass_info;

   string pclass_id;
   string pfield_id;

   string uclass_id;
   string ufield_id;

   string new_class_id;
   string new_field_id;
   string new_ext_info;
   string dest_enum_id;
   string dest_field_id;
   string dest_field_value;

   vector< string > field_info;
   vector< string > filter_info;
   vector< string > parent_info;
   vector< string > restrict_info;
};

void list_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) < 4 )
      throw runtime_error( "list specification requires 'list name', 'list type', 'class name' and 'columns fields'" );

   string arg_list_name( args[ 0 ] );
   string arg_type_name( args[ 1 ] );
   string arg_class_name( args[ 2 ] );
   string arg_column_fields( args[ 3 ] );

   size_t first_extra_arg = 4;

   string user_class_name;
   string user_field_name;
   string::size_type pos = arg_class_name.find( ',' );
   if( pos != string::npos )
   {
      user_class_name = arg_class_name.substr( pos + 1 );
      arg_class_name.erase( pos );

      pos = user_class_name.find( ':' );
      if( pos != string::npos )
      {
         user_field_name = user_class_name.substr( pos + 1 );
         user_class_name.erase( pos );
      }
   }

   bool is_external_alias;
   class_id = get_class_id_for_name( m, arg_class_name, &is_external_alias, &bclass_info );

   if( !user_class_name.empty( ) )
   {
      uclass_id = get_class_id_for_name( m, user_class_name );

      if( user_field_name.empty( ) )
         user_field_name = "{" + user_class_name + "}";
   }

   if( !user_field_name.empty( ) )
   {
      ufield_id = get_field_id_for_name( m, arg_class_name, user_field_name, 0, true );

      if( ufield_id.empty( ) )
         throw runtime_error( "field name '" + user_field_name + "' not found in class '" + arg_class_name + "'" );
   }

   name = arg_list_name;
   type = arg_type_name;

   pos = type.find( ':' );
   if( pos != string::npos )
   {
      extra = type.substr( pos + 1 );
      type.erase( pos );
   }

   vector< field_data > all_field_data;
   m.get_field_data( arg_class_name, all_field_data, e_get_field_type_any, true );

   vector< string > spec_field_names;
   if( !arg_column_fields.empty( ) )
      split( arg_column_fields, spec_field_names );

   vector< string > field_ids;

   class_data cls_data;
   m.get_class_data( arg_class_name, cls_data, true );

   set< string > extras;
   if( !extra.empty( ) )
      split( extra, extras );

   bool needs_order_field = false;

   set< string > class_extras;
   split( cls_data.extra, class_extras );

   if( class_extras.count( c_class_extra_ordered ) )
      needs_order_field = true;

   string permission_field_id;
   string ordered_order_field_id;

   for( size_t i = 0; i < all_field_data.size( ); i++ )
   {
      if( !all_field_data[ i ].extra.empty( ) )
      {
         set< string > extras;
         split( all_field_data[ i ].extra, extras );

         if( extras.count( c_field_extra_permission ) )
            permission_field_id = all_field_data[ i ].id;

         if( extras.count( c_field_extra_order ) )
            ordered_order_field_id = all_field_data[ i ].id;
      }
   }

   if( !needs_order_field || extras.count( "sort_no_limit" ) || extras.count( "explicit_ordering" ) )
   {
      needs_order_field = false;
      ordered_order_field_id.erase( );
   }

   for( size_t i = 0; i < spec_field_names.size( ); i++ )
   {
      string id;
      bool is_external;

      if( spec_field_names[ i ] == c_key_field )
         id = c_key_field;
      else if( spec_field_names[ i ] == c_row_field )
         id = c_row_field;
      else
      {
         string type, extra;

         string::size_type pos = spec_field_names[ i ].find( ';' );
         if( pos != string::npos )
         {
            extra = spec_field_names[ i ].substr( pos + 1 );
            spec_field_names[ i ].erase( pos );
         }

         pos = spec_field_names[ i ].find( ':' );
         string name( spec_field_names[ i ].substr( 0, pos ) );

         for( size_t j = 0; j < all_field_data.size( ); j++ )
         {
            if( all_field_data[ j ].decorated_name == name )
            {
               id = all_field_data[ j ].id;
               type = all_field_data[ j ].type;
               is_external = all_field_data[ j ].is_external_field;

               // NOTE: If the class has been identified as "ordered" and the first
               // field was not the "order" field then add this field automatically.
               if( i == 0 && needs_order_field && all_field_data[ j ].id != ordered_order_field_id )
               {
                  if( !is_external )
                     field_ids.push_back( ordered_order_field_id );
                  field_info.push_back( ordered_order_field_id + ";hidden" );
               }

               if( !all_field_data[ j ].enum_name.empty( ) )
               {
                  if( !extra.empty( ) )
                     extra += '+';
                  extra += "enum=" + get_id_for_enum_name( m, all_field_data[ j ].enum_name );
               }

               break;
            }
         }

         if( id.empty( ) )
            throw runtime_error( "field name '"
             + spec_field_names[ i ] + "' not found in class '" + arg_class_name + "'" );

         if( !is_external )
            field_ids.push_back( id );

         string last_name( name );
         string last_type( type );
         string next_name( spec_field_names[ i ].substr( pos + 1 ) );
         while( pos != string::npos )
         {
            pos = next_name.find( ':' );
            string foreign_field_name( next_name.substr( 0, pos ) );

            string foreign_class_name( get_class_name_from_field_type( m, arg_class_name, last_name, last_type ) );

            vector< field_data > all_foreign_fields;
            m.get_field_data( foreign_class_name, all_foreign_fields, e_get_field_type_any, true );

            string foreign_field_id;
            for( size_t j = 0; j < all_foreign_fields.size( ); j++ )
            {
               if( all_foreign_fields[ j ].decorated_name == foreign_field_name )
               {
                  foreign_field_id = all_foreign_fields[ j ].id;

                  // NOTE: Field extra's are applicable to the plain field (not to intermdiate fk's).
                  if( pos == string::npos )
                  {
                     if( !extra.empty( ) )
                        extra += '+';
                     extra += all_foreign_fields[ j ].extra;
                  }

                  last_name = all_foreign_fields[ j ].name;
                  last_type = all_foreign_fields[ j ].type;

                  break;
               }
            }

            if( foreign_field_id.empty( ) )
               throw runtime_error( "foreign field '" + foreign_field_name + "' not found in class '" + foreign_class_name + "'" );

            id += ":" + foreign_field_id;

            if( pos != string::npos )
               next_name.erase( 0, pos + 1 );
         }

         if( !extra.empty( ) )
            id += ";" + extra;
      }

      field_info.push_back( id );
   }

   if( !permission_field_id.empty( ) )
   {
      if( !is_external_alias )
         field_ids.push_back( permission_field_id );
      field_info.push_back( permission_field_id + ";hidden" );
   }

   string reverse_opt( "-rev" );
   if( args.size( ) > first_extra_arg && args[ first_extra_arg ] == reverse_opt )
   {
      ordering = "reverse";
      ++first_extra_arg;
   }
   else
      ordering = "forward";

   string filter_prefix( "-f=" );
   if( args.size( ) > first_extra_arg
    && args[ first_extra_arg ].size( ) > filter_prefix.length( )
    && args[ first_extra_arg ].substr( 0, filter_prefix.length( ) ) == filter_prefix )
   {
      vector< string > filter_names;
      split( args[ first_extra_arg++ ].substr( filter_prefix.length( ) ), filter_names );

      vector< specification_data > all_spec_data;
      m.get_specification_data( arg_class_name, all_spec_data );

      for( size_t i = 0; i < filter_names.size( ); i++ )
      {
         string next_spec_id;

         if( filter_names[ i ] == "uid" )
            next_spec_id = "uid";
         else
         {
            for( size_t j = 0; j < all_spec_data.size( ); j++ )
            {
               if( all_spec_data[ j ].decorated_name == filter_names[ i ] )
               {
                  if( all_spec_data[ j ].type != "filter" )
                     throw runtime_error( "specification '" + filter_names[ i ] + "' is not a filter" );

                  next_spec_id = all_spec_data[ j ].id;
                  break;
               }
            }
         }

         if( next_spec_id.empty( ) )
            throw runtime_error( "unknown specification '" + filter_names[ i ] + "' for class '" + arg_class_name + "'" );

         filter_info.push_back( next_spec_id );
      }
   }

   string newrec_prefix( "-n=" );
   if( args.size( ) > first_extra_arg
    && args[ first_extra_arg ].size( ) > newrec_prefix.length( )
    && args[ first_extra_arg ].substr( 0, newrec_prefix.length( ) ) == newrec_prefix )
   {
      string new_info( args[ first_extra_arg++ ].substr( newrec_prefix.length( ) ) );

      string new_class_name, new_field_name, dest_field_name;
      string::size_type pos = new_info.find( ',' );

      if( pos == string::npos )
      {
         pos = new_info.find( '=' );

         if( pos == string::npos )
            dest_field_name = new_info;
         else
         {
            dest_field_name = new_info.substr( 0, pos );
            dest_field_value = new_info.substr( pos + 1 );
         }

         new_info.erase( );
      }
      else
      {
         dest_field_name = new_info.substr( pos + 1 );
         new_info.erase( pos );
      }

      string id, type, enum_name;
      for( size_t i = 0; i < all_field_data.size( ); i++ )
      {
         if( all_field_data[ i ].decorated_name == dest_field_name )
         {
            id = all_field_data[ i ].id;
            type = all_field_data[ i ].type;
            enum_name = all_field_data[ i ].enum_name;
            break;
         }
      }

      if( id.empty( ) )
         throw runtime_error( "unknown field '" + dest_field_name + "' for class '" + arg_class_name + "'" );

      if( new_info.empty( ) && enum_name.empty( ) && dest_field_value.empty( ) )
         throw runtime_error( "field '" + dest_field_name + "' for class '" + arg_class_name
          + "' has not been given a fixed value and does not use an enumerated type" );

      dest_field_id = id;

      if( new_info.empty( ) )
      {
         if( dest_field_value.empty( ) )
         {
            dest_enum_id = get_id_for_enum_name( m, enum_name );

            if( dest_enum_id.empty( ) )
               throw runtime_error( "unexpected enum '" + enum_name + "' not found" );
         }
      }
      else
      {
         string dest_field_class( get_class_name_from_field_type( m, arg_class_name, dest_field_name, type ) );

         pos = new_info.find( ':' );

         if( pos == string::npos )
            throw runtime_error( "unexpected format '" + new_info + "' for new record class:field" );

         new_class_name = new_info.substr( 0, pos );
         new_field_name = new_info.substr( pos + 1 );

         if( new_class_name != dest_field_class )
            throw runtime_error( "new record class '" + new_class_name + "' does not match dest field class '" + dest_field_class + "'" );

         class_data ncls_data;
         m.get_class_data( new_class_name, ncls_data );

         vector< field_data > new_field_data;
         m.get_field_data( new_class_name, new_field_data );

         split_extra_from_field( new_field_name, new_ext_info, new_field_data, true );

         new_class_id = ncls_data.id;
         new_field_id = get_field_id_for_name( m, new_class_name, new_field_name );

         if( new_field_id.empty( ) )
            throw runtime_error( "unknown field '" + new_field_name + "' for class '" + new_class_name + "'" );
      }
   }

   string actions_prefix( "-act=" );
   if( args.size( ) > first_extra_arg
    && args[ first_extra_arg ].size( ) > actions_prefix.length( )
    && args[ first_extra_arg ].substr( 0, actions_prefix.length( ) ) == actions_prefix )
   {
      actions = args[ first_extra_arg++ ].substr( actions_prefix.length( ) );
   }

   pos = type.find( '=' );
   string type_without_perm( type.substr( 0, pos ) );

   // NOTE: For a "child" list a field name is required to identify the applicable parent.
   if( type_without_perm == "child" || type_without_perm == "group" || type_without_perm == "nongroup"
    || type_without_perm == "user" || type_without_perm == "nonuser" || type_without_perm == "user_child" )
   {
      if( args.size( ) < first_extra_arg + 1 )
         throw runtime_error( "child list specifications require a 'parent field'" );

      if( type == "user_child" && ( uclass_id.empty( ) || ufield_id.empty( ) ) )
         throw runtime_error( "must specify user class name and field for user_child type" );

      string pfield( args[ first_extra_arg++ ] );

      string id, type;
      for( size_t i = 0; i < all_field_data.size( ); i++ )
      {
         if( all_field_data[ i ].decorated_name == pfield )
         {
            id = all_field_data[ i ].id;
            type = all_field_data[ i ].type;
            break;
         }
      }

      if( id.empty( ) )
         throw runtime_error( "unknown field '" + pfield + "' for class '" + arg_class_name + "'" );

      string pclass( get_class_name_from_field_type( m, arg_class_name, pfield, type ) );

      pfield_id = id;

      class_data pcls_data;
      m.get_class_data( pclass, pcls_data );

      pclass_id = pcls_data.id;
   }
   else if( type_without_perm != "admin" && type_without_perm != "home" && type_without_perm != "standard" )
      throw runtime_error( "unexpected list type '" + type_without_perm
       + "' (expecting 'child', 'group', 'nongroup', 'user', 'nonuser', 'user_child', 'admin', 'home' or 'standard')" );

   vector< string > join_field_ids;
   vector< string > parent_class_ids;
   vector< string > parent_field_ids;
   vector< string > restrict_field_ids;
   for( size_t argnum = first_extra_arg; argnum < args.size( ); argnum++ )
   {
      string next( args[ argnum ] );
      string::size_type pos = next.find( ',' );
      if( pos == string::npos )
         throw runtime_error( "invalid format for parent info '" + args[ argnum ] + "'" );

      string field( next.substr( 0, pos ) );
      next.erase( 0, pos + 1 );

      string pfield;
      pos = next.find( ',' );
      if( pos != string::npos )
      {
         pfield = next.substr( 0, pos );
         next.erase( 0, pos + 1 );
      }

      string operations( next );

      string id, pinfo, ftype;
      bool is_external_field;
      for( size_t i = 0; i < all_field_data.size( ); i++ )
      {
         if( all_field_data[ i ].decorated_name == field )
         {
            id = all_field_data[ i ].id;
            ftype = all_field_data[ i ].type;
            is_external_field = all_field_data[ i ].is_external_field;
            break;
         }
      }

      if( id.empty( ) )
         throw runtime_error( "unknown field '" + field + "' for class '" + arg_class_name + "'" );

      if( !pfield.empty( ) )
      {
         string pclass( get_class_name_from_field_type( m, arg_class_name, field, ftype ) );

         pinfo = id;

         if( !is_external_field )
            join_field_ids.push_back( id );

         class_data pcls_data;
         m.get_class_data( pclass, pcls_data );

         pinfo += "," + pcls_data.id;
         parent_class_ids.push_back( pcls_data.id );

         vector< field_data > parent_field_data;
         m.get_field_data( pclass, parent_field_data, e_get_field_type_any, true );

         bool parent_needs_order_field = false;
         string parent_ordered_order_field_id;
         if( !pcls_data.extra.empty( ) )
         {
            set< string > extras;
            split( pcls_data.extra, extras );

            if( extras.count( c_class_extra_ordered ) )
               parent_needs_order_field = true;

            for( size_t i = 0; i < parent_field_data.size( ); i++ )
            {
               if( !parent_field_data[ i ].extra.empty( ) )
               {
                  set< string > extras;
                  split( parent_field_data[ i ].extra, extras );

                  if( extras.count( c_field_extra_order ) )
                  {
                     parent_ordered_order_field_id = parent_field_data[ i ].id;
                     break;
                  }
               }
            }
         }

         string pextra;
         split_extra_from_field( pfield, pextra, parent_field_data, true );

         id.erase( );
         if( pfield == c_key_field )
            id = c_key_field;
         else
         {
            for( size_t i = 0; i < parent_field_data.size( ); i++ )
            {
               if( parent_field_data[ i ].decorated_name == pfield )
               {
                  id = parent_field_data[ i ].id;
                  break;
               }
            }

            if( id.empty( ) )
               throw runtime_error( "unknown field '" + pfield + "' for class '" + pclass + "'" );

            if( !pcls_data.is_external_alias )
               parent_field_ids.push_back( id );
         }

         pos = operations.find( ':' );
         string operation_type( operations.substr( 0, pos ) );

         pinfo += "," + id;
         if( !pextra.empty( ) )
            pinfo += "!" + pextra;
         else if( parent_needs_order_field && operation_type != "restricted" )
            pinfo += "!" + parent_ordered_order_field_id;

         pinfo += "," + operations;

         parent_info.push_back( pinfo );
      }
      else
      {
         if( !is_external_field )
            restrict_field_ids.push_back( id );

         string rinfo( id + "," + operations );
         restrict_info.push_back( rinfo );
      }
   }

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );

   if( !uclass_id.empty( ) )
      details.push_back( specification_detail( uclass_id, "uclass", e_model_element_type_class ) );

   if( !new_class_id.empty( ) )
      details.push_back( specification_detail( new_class_id, "new_class", e_model_element_type_class ) );

   for( size_t i = 0; i < parent_class_ids.size( ); i++ )
      details.push_back( specification_detail( parent_class_ids[ i ], "pclass", e_model_element_type_class ) );

   for( size_t i = 0; i < field_ids.size( ); i++ )
      details.push_back( specification_detail( field_ids[ i ], "field", e_model_element_type_field ) );

   for( size_t i = 0; i < join_field_ids.size( ); i++ )
      details.push_back( specification_detail( join_field_ids[ i ], "jfield", e_model_element_type_field ) );

   for( size_t i = 0; i < parent_field_ids.size( ); i++ )
      details.push_back( specification_detail( parent_field_ids[ i ], "pfield", e_model_element_type_field ) );

   for( size_t i = 0; i < restrict_field_ids.size( ); i++ )
      details.push_back( specification_detail( restrict_field_ids[ i ], "rfield", e_model_element_type_field ) );

   if( !new_field_id.empty( ) )
      details.push_back( specification_detail( new_field_id, "new_field", e_model_element_type_field ) );

   if( !dest_field_id.empty( ) )
      details.push_back( specification_detail( dest_field_id, "dest_field", e_model_element_type_field ) );
}

void list_specification::read_data( sio_reader& reader )
{
   name = reader.read_attribute( c_attribute_name );
   type = reader.read_attribute( c_attribute_type );
   extra = reader.read_opt_attribute( c_attribute_extra );
   actions = reader.read_opt_attribute( c_attribute_actions );

   class_id = reader.read_attribute( c_attribute_class_id );
   ordering = reader.read_attribute( c_attribute_ordering );

   bclass_info = reader.read_opt_attribute( c_attribute_bclass_info );

   pclass_id = reader.read_opt_attribute( c_attribute_pclass_id );
   pfield_id = reader.read_opt_attribute( c_attribute_pfield_id );

   uclass_id = reader.read_opt_attribute( c_attribute_uclass_id );
   ufield_id = reader.read_opt_attribute( c_attribute_ufield_id );

   new_class_id = reader.read_opt_attribute( c_attribute_new_class_id );
   new_field_id = reader.read_opt_attribute( c_attribute_new_field_id );
   new_ext_info = reader.read_opt_attribute( c_attribute_new_ext_info );
   dest_enum_id = reader.read_opt_attribute( c_attribute_dest_enum_id );
   dest_field_id = reader.read_opt_attribute( c_attribute_dest_field_id );
   dest_field_value = reader.read_opt_attribute( c_attribute_dest_field_value );

   string next;
   while( reader.has_read_attribute( c_attribute_field_info, next ) )
      field_info.push_back( next );

   while( reader.has_read_attribute( c_attribute_filter_info, next ) )
      filter_info.push_back( next );

   while( reader.has_read_attribute( c_attribute_parent_info, next ) )
      parent_info.push_back( next );

   while( reader.has_read_attribute( c_attribute_restrict_info, next ) )
      restrict_info.push_back( next );
}

void list_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_name, name );
   writer.write_attribute( c_attribute_type, type );
   writer.write_opt_attribute( c_attribute_extra, extra );
   writer.write_opt_attribute( c_attribute_actions, actions );

   writer.write_attribute( c_attribute_class_id, class_id );
   writer.write_attribute( c_attribute_ordering, ordering );

   writer.write_opt_attribute( c_attribute_bclass_info, bclass_info );

   writer.write_opt_attribute( c_attribute_pclass_id, pclass_id );
   writer.write_opt_attribute( c_attribute_pfield_id, pfield_id );

   writer.write_opt_attribute( c_attribute_uclass_id, uclass_id );
   writer.write_opt_attribute( c_attribute_ufield_id, ufield_id );

   writer.write_opt_attribute( c_attribute_new_class_id, new_class_id );
   writer.write_opt_attribute( c_attribute_new_field_id, new_field_id );
   writer.write_opt_attribute( c_attribute_new_ext_info, new_ext_info );
   writer.write_opt_attribute( c_attribute_dest_enum_id, dest_enum_id );
   writer.write_opt_attribute( c_attribute_dest_field_id, dest_field_id );
   writer.write_opt_attribute( c_attribute_dest_field_value, dest_field_value );

   for( size_t i = 0; i < field_info.size( ); i++ )
      writer.write_attribute( c_attribute_field_info, field_info[ i ] );

   for( size_t i = 0; i < filter_info.size( ); i++ )
      writer.write_attribute( c_attribute_filter_info, filter_info[ i ] );

   for( size_t i = 0; i < parent_info.size( ); i++ )
      writer.write_attribute( c_attribute_parent_info, parent_info[ i ] );

   for( size_t i = 0; i < restrict_info.size( ); i++ )
      writer.write_attribute( c_attribute_restrict_info, restrict_info[ i ] );
}

void list_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   vector< class_data > all_class_data;
   m.get_class_data( all_class_data, true );

   string class_name = get_class_name_for_id( all_class_data, class_id );
   spec_data.data_pairs.push_back( make_pair( c_data_cid, class_id ) );

   vector< field_data > all_field_data;
   m.get_field_data( class_name, all_field_data, e_get_field_type_any, true );

   vector< index_data > all_index_data;
   m.get_index_data( class_name, all_index_data, true );

   spec_data.data_pairs.push_back( make_pair( c_data_name, get_expanded_name( m, class_id, name ) ) );
   spec_data.data_pairs.push_back( make_pair( c_data_type, type ) );
   spec_data.data_pairs.push_back( make_pair( c_data_acts, actions ) );

   string modifiers;
   class_data cls_data;
   m.get_class_data( class_name, cls_data, true );

   vector< string > all_modifiers;
   split( cls_data.modifiers, all_modifiers );
   for( size_t i = 0; i < all_modifiers.size( ); i++ )
   {
      string::size_type pos = all_modifiers[ i ].find( ';' );
      if( pos == string::npos || all_modifiers[ i ].substr( pos + 1 ) == "list" )
      {
         if( !modifiers.empty( ) )
            modifiers += ',';
         modifiers += all_modifiers[ i ].substr( 0, pos );
      }
   }
   spec_data.data_pairs.push_back( make_pair( c_data_mods, modifiers ) );

   spec_data.data_pairs.push_back( make_pair( c_data_extra, extra ) );
   spec_data.data_pairs.push_back( make_pair( c_data_order, ordering ) );
   spec_data.data_pairs.push_back( make_pair( c_data_binfo, bclass_info ) );

   string parent_field_name;

   if( !pclass_id.empty( ) )
   {
      string pclass_name = get_class_name_for_id( all_class_data, pclass_id );
      string pfield_name = get_field_name_for_id( m, class_name, pfield_id );

      parent_field_name = pfield_name;

      spec_data.data_pairs.push_back( make_pair( c_data_pclass, pclass_id ) );
      spec_data.data_pairs.push_back( make_pair( c_data_pclsnm, pclass_name ) );
      spec_data.data_pairs.push_back( make_pair( c_data_pfield, pfield_id ) );
      spec_data.data_pairs.push_back( make_pair( c_data_pfldnm, pfield_name ) );
   }

   if( !uclass_id.empty( ) )
   {
      string uclass_name = get_class_name_for_id( all_class_data, uclass_id );
      string ufield_name = get_field_name_for_id( m, class_name, ufield_id );

      spec_data.data_pairs.push_back( make_pair( c_data_uclass, uclass_id ) );
      spec_data.data_pairs.push_back( make_pair( c_data_uclsnm, uclass_name ) );
      spec_data.data_pairs.push_back( make_pair( c_data_ufield, ufield_id ) );
      spec_data.data_pairs.push_back( make_pair( c_data_ufldnm, ufield_name ) );
   }

   if( !new_class_id.empty( ) )
   {
      spec_data.data_pairs.push_back( make_pair( c_data_nclass, new_class_id ) );
      spec_data.data_pairs.push_back( make_pair( c_data_nfield, new_field_id ) );

      string new_class_name = get_class_name_for_id( all_class_data, new_class_id );
      string new_field_name = get_field_name_for_id( m, new_class_name, new_field_id );

      vector< index_data > new_class_index_data;
      m.get_index_data( new_class_name, new_class_index_data, true );

      bool is_indexed = false;
      for( size_t i = 0; i < new_class_index_data.size( ); i++ )
      {
         vector< string > fields;
         split( new_class_index_data[ i ].name, fields );

         if( fields[ 0 ] == new_field_name )
         {
            is_indexed = true;
            break;
         }
      }

      spec_data.data_pairs.push_back( make_pair( c_data_nindex, is_indexed ? c_true : c_false ) );
      spec_data.data_pairs.push_back( make_pair( c_data_nextra, new_ext_info ) );
      spec_data.data_pairs.push_back( make_pair( c_data_dfield, dest_field_id ) );
   }
   else if( !dest_enum_id.empty( ) )
   {
      spec_data.data_pairs.push_back( make_pair( c_data_dfenum, dest_enum_id ) );
      spec_data.data_pairs.push_back( make_pair( c_data_dfield, dest_field_id ) );
   }
   else if( !dest_field_value.empty( ) )
   {
      spec_data.data_pairs.push_back( make_pair( c_data_dfield, dest_field_id ) );
      spec_data.data_pairs.push_back( make_pair( c_data_dvalue, dest_field_value ) );
   }

   if( filter_info.size( ) )
   {
      string filters;

      for( size_t i = 0; i < filter_info.size( ); i++ )
      {
         if( !filters.empty( ) )
            filters += ",";
         filters += filter_info[ i ];
      }

      spec_data.data_pairs.push_back( make_pair( string( c_data_filters ), filters ) );
   }

   vector< string > restricted_pfields;

   for( size_t i = 0; i < parent_info.size( ); i++ )
   {
      string pnum;
      pnum += ( '0' + ( i % 10 ) );

      string next( parent_info[ i ] );

      string::size_type pos = next.find( ',' );
      string field( next.substr( 0, pos ) );

      string field_name;
      bool is_mandatory;
      for( size_t j = 0; j < all_field_data.size( ); j++ )
      {
         if( all_field_data[ j ].id == field )
         {
            field_name = all_field_data[ j ].name;
            is_mandatory = all_field_data[ j ].is_mandatory;
            break;
         }
      }

      next.erase( 0, pos + 1 );
      pos = next.find( ',' );
      string pclass( next.substr( 0, pos ) );

      string pclass_name = get_class_name_for_id( all_class_data, pclass );

      next.erase( 0, pos + 1 );
      pos = next.find( ',' );
      string pfield( next.substr( 0, pos ) );

      next.erase( 0, pos + 1 );
      pos = next.find( ',' );
      string operations( next.substr( 0, pos ) );

      vector< field_data > parent_field_data;
      m.get_field_data( pclass_name, parent_field_data, e_get_field_type_any, true );

      string pfield_id, pfield_name, pfield_extra;

      pos = pfield.find( '!' );
      if( pos != string::npos )
      {
         pfield_extra = pfield.substr( pos + 1 );
         pfield.erase( pos );
      }

      if( pfield == c_key_field )
         pfield_id = c_key_field;
      else
      {
         for( size_t j = 0; j < parent_field_data.size( ); j++ )
         {
            if( parent_field_data[ j ].id == pfield )
            {
               pfield_id = parent_field_data[ j ].id;
               pfield_name = parent_field_data[ j ].name;
               break;
            }
         }
      }

      bool is_indexed = false;
      if( pfield != c_key_field && pfield_extra.empty( ) )
      {
         vector< index_data > all_index_data;
         m.get_index_data( pclass_name, all_index_data, true );

         for( size_t j = 0; j < all_index_data.size( ); j++ )
         {
            vector< string > fields;
            split( all_index_data[ j ].name, fields );

            if( fields[ 0 ] == pfield_name )
            {
               is_indexed = true;
               break;
            }
         }
      }

      pos = operations.find( ':' );
      string op_name( operations.substr( 0, pos ) );

      if( op_name == "restricted" )
         restricted_pfields.push_back( field_name );

      spec_data.data_pairs.push_back( make_pair( string( c_data_parent ) + pnum, field_name ) );
      spec_data.data_pairs.push_back( make_pair( string( c_data_jfield ) + pnum, field ) );
      spec_data.data_pairs.push_back( make_pair( string( c_data_dextra ) + pnum, "" ) );
      spec_data.data_pairs.push_back( make_pair( string( c_data_pclass ) + pnum, pclass ) );
      spec_data.data_pairs.push_back( make_pair( string( c_data_pclsnm ) + pnum, pclass_name ) );
      spec_data.data_pairs.push_back( make_pair( string( c_data_pfield ) + pnum, pfield_id ) );
      spec_data.data_pairs.push_back( make_pair( string( c_data_pextra ) + pnum, pfield_extra ) );
      spec_data.data_pairs.push_back( make_pair( string( c_data_pindexed ) + pnum, is_indexed ? c_true : c_false ) );
      spec_data.data_pairs.push_back( make_pair( string( c_data_mandatory ) + pnum, is_mandatory ? c_true : c_false ) );
      spec_data.data_pairs.push_back( make_pair( string( c_data_operations ) + pnum, operations ) );
   }

   for( size_t i = 0; i < restrict_info.size( ); i++ )
   {
      string rnum;
      rnum += ( '0' + ( i % 10 ) );

      string next( restrict_info[ i ] );
      string::size_type pos = next.find( ',' );

      string field_id( next.substr( 0, pos ) );

      bool is_mandatory;
      string field_name, field_type, field_extra;
      for( size_t j = 0; j < all_field_data.size( ); j++ )
      {
         if( all_field_data[ j ].id == field_id )
         {
            field_name = all_field_data[ j ].name;
            field_type = all_field_data[ j ].sys_type;
            field_extra = all_field_data[ j ].extra;
            is_mandatory = all_field_data[ j ].is_mandatory;

            if( !all_field_data[ j ].enum_name.empty( ) )
            {
               if( !field_extra.empty( ) )
                  field_extra += '+';
               field_extra += "enum=" + get_id_for_enum_name( m, all_field_data[ j ].enum_name );
            }
            break;
         }
      }

      next.erase( 0, pos + 1 );
      string operations( next );

      spec_data.data_pairs.push_back( make_pair( string( c_data_rfldnm ) + rnum, field_name ) );
      spec_data.data_pairs.push_back( make_pair( string( c_data_rfield ) + rnum, field_id ) );
      spec_data.data_pairs.push_back( make_pair( string( c_data_rftype ) + rnum, field_type ) );
      spec_data.data_pairs.push_back( make_pair( string( c_data_rfmand ) + rnum, is_mandatory ? c_true : c_false ) );
      spec_data.data_pairs.push_back( make_pair( string( c_data_rfxtra ) + rnum, field_extra ) );
      spec_data.data_pairs.push_back( make_pair( string( c_data_rfldops ) + rnum, operations ) );
   }

   for( size_t i = 0; i < field_info.size( ); i++ )
   {
      string fnum;
      fnum += ( '0' + ( i / 10 ) );
      fnum += ( '0' + ( i % 10 ) );

      string field, fname, ftype, fxtra, fmods, stype, extra, pfield_id;

      bool is_foreign_key = false;

      string id( field_info[ i ] );

      string::size_type pos = id.find( ';' );

      if( pos != string::npos )
      {
         extra = id.substr( pos + 1 );
         id.erase( pos );
      }

      pos = id.find( ':' );
      if( pos != string::npos )
      {
         pfield_id = id.substr( pos + 1 );
         id.erase( pos );

         // NOTE: Change colon separators to dot separators.
         for( size_t j = 0; j < pfield_id.size( ); j++ )
         {
            if( pfield_id[ j ] == ':' )
               pfield_id[ j ] = '.';
         }
      }

      if( id == c_key_field )
      {
         field = c_key_field;
         fname = c_key_title;
      }
      else if( id == c_row_field )
      {
         field = c_row_field;
         fname = c_row_title;
      }
      else
      {
         for( size_t j = 0; j < all_field_data.size( ); j++ )
         {
            if( all_field_data[ j ].id == id )
            {
               field = all_field_data[ j ].name;
               fname = all_field_data[ j ].name;
               ftype = all_field_data[ j ].type;
               fxtra = all_field_data[ j ].extra;
               fmods = all_field_data[ j ].modifiers;
               stype = all_field_data[ j ].sys_type;
               is_foreign_key = all_field_data[ j ].is_foreign_key;

               if( !fxtra.empty( ) )
               {
                  if( !extra.empty( ) )
                     extra += "+";
                  extra += fxtra;
               }

               break;
            }
         }
      }

      // NOTE: Only include "modifiers" that are applicable to "lists" here (i.e. exclude
      // any modifiers that have been labelled as being relevant to "views" only).
      if( !fmods.empty( ) )
      {
         string new_fmods;

         vector< string > all_mods;
         split( fmods, all_mods );

         for( size_t i = 0; i < all_mods.size( ); i++ )
         {
            string next( all_mods[ i ] );
            string::size_type pos = next.find( ';' );

            if( pos != string::npos && next.substr( pos + 1 ) != "list" )
               continue;

            if( !new_fmods.empty( ) )
               new_fmods += ",";
            new_fmods += all_mods[ i ].substr( 0, pos );
         }

         fmods = new_fmods;
      }

      bool is_nested_foreign_key_field = false;
      string foreign_class_name, last_foreign_class_name, foreign_class_id, foreign_field_name;

      // NOTE: For nested foreign keys append (with dots) the interim fk's and set the display name
      // and parent class identifier to the last foreign key field (prior to the actual display field).
      if( is_foreign_key )
      {
         foreign_class_name = get_class_name_from_field_type( m, ftype );

         foreign_class_id = get_class_id_for_name( m, foreign_class_name );
         string::size_type pos = pfield_id.find( '.' );
         while( pos != string::npos )
         {
            is_nested_foreign_key_field = true;

            string next_type;
            fname = get_field_name_for_id( m, foreign_class_name, pfield_id.substr( 0, pos ), &next_type );

            last_foreign_class_name = foreign_class_name;

            foreign_class_name = get_class_name_from_field_type( m, next_type );
            foreign_class_id = get_class_id_for_name( m, foreign_class_name );

            id += "." + pfield_id.substr( 0, pos );

            pfield_id.erase( 0, pos + 1 );
            pos = pfield_id.find( '.' );
         }

         foreign_field_name = get_field_name_for_id( m, foreign_class_name, pfield_id, 0, true );
      }

      string clname( fname );
      if( fname != c_key_title && fname != c_row_title )
      {
         clname = class_name;
         if( is_nested_foreign_key_field )
            clname = last_foreign_class_name;

         clname += '_' + fname;
      }

      spec_data.data_pairs.push_back( make_pair( string( c_data_column ) + fnum, id ) );
      spec_data.data_pairs.push_back( make_pair( string( c_data_clname ) + fnum, clname ) );
      spec_data.data_pairs.push_back( make_pair( string( c_data_cltype ) + fnum, stype ) );
      spec_data.data_pairs.push_back( make_pair( string( c_data_clxtra ) + fnum, extra ) );
      spec_data.data_pairs.push_back( make_pair( string( c_data_clmods ) + fnum, fmods ) );

      bool is_unique = false;
      bool is_indexed = false;
      if( field == c_key_field )
         is_indexed = true;
      else
      {
         for( size_t j = 0; j < all_index_data.size( ); j++ )
         {
            vector< string > fields;
            split( all_index_data[ j ].name, fields );

            size_t start = 0;
            if( !parent_field_name.empty( ) )
            {
               if( fields[ 0 ] != parent_field_name )
                  continue;
               ++start;
            }

            size_t finish = start + restricted_pfields.size( );

            // NOTE: In order for virtual list scrolling to work correctly an
            // index with the exact number of matching fields must be present.
            if( fields.size( ) != finish + 1 )
               continue;

            bool okay = true;
            for( size_t k = start; k < finish; k++ )
            {
               if( fields[ k ] != restricted_pfields[ k - start ] )
               {
                  okay = false;
                  break;
               }
            }

            if( !okay )
               continue;

            if( fields[ finish ] == field )
            {
               is_unique = all_index_data[ j ].is_unique;
               is_indexed = true;
               break;
            }
         }
      }

      spec_data.data_pairs.push_back( make_pair( string( c_data_cluniq ) + fnum, is_unique ? c_true : c_false ) );
      spec_data.data_pairs.push_back( make_pair( string( c_data_cindex ) + fnum, is_indexed ? c_true : c_false ) );

      if( is_foreign_key )
      {
         spec_data.data_pairs.push_back( make_pair( string( c_data_pclass ) + fnum, foreign_class_id ) );
         spec_data.data_pairs.push_back( make_pair( string( c_data_pclsnm ) + fnum, foreign_class_name ) );
         spec_data.data_pairs.push_back( make_pair( string( c_data_pfldnm ) + fnum, foreign_field_name ) );
      }

      if( !pfield_id.empty( ) )
         spec_data.data_pairs.push_back( make_pair( string( c_data_pfield ) + fnum, pfield_id ) );
   }
}

void list_specification::get_specification_strings( model& m, vector< pair< string, string > >& strings ) const
{
   strings.push_back( make_pair( "name", get_expanded_name( m, class_id, name ) ) );
}

string list_specification::static_class_name( ) { return "list"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, list_specification, static_class_name );

struct view_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_non_model; }

   bool has_specification_data_for_class( const string& id ) const { return id == class_id; }

   bool is_interface_specification( ) const { return true; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   void get_specification_strings( model& m, vector< pair< string, string > >& strings ) const;

   static string static_class_name( );

   string name;
   string type;
   string extra;
   string class_id;
   string bclass_info;
   vector< string > field_info;
};

void view_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) != 4 )
      throw runtime_error( "view specification requires 'view name', 'view type', 'class name' and 'field names'" );

   string arg_view_name( args[ 0 ] );
   string arg_type_name( args[ 1 ] );
   string arg_class_name( args[ 2 ] );
   string arg_field_names( args[ 3 ] );

   bool is_external_alias;
   class_id = get_class_id_for_name( m, arg_class_name, &is_external_alias, &bclass_info );

   name = arg_view_name;
   type = arg_type_name;

   string::size_type pos = type.find( ':' );
   if( pos != string::npos )
   {
      extra = type.substr( pos + 1 );
      type.erase( pos );
   }

   pos = type.find( '=' );
   string vtype( type.substr( 0, pos ) );

   if( vtype != "admin" && vtype != "print" && vtype != "standard" && vtype != "admin_print" )
      throw runtime_error( "unknown view type '" + vtype + "'" );

   vector< field_data > all_field_data;
   m.get_field_data( arg_class_name, all_field_data, e_get_field_type_any, true );

   vector< string > spec_field_names;
   if( !arg_field_names.empty( ) )
      split( arg_field_names, spec_field_names );

   vector< string > field_ids;

   for( size_t i = 0; i < spec_field_names.size( ); i++ )
   {
      string id;
      bool is_external;

      if( spec_field_names[ i ] == c_key_field
       || ( spec_field_names[ i ].length( ) > strlen( c_tab_field )
       && spec_field_names[ i ].substr( 0, strlen( c_tab_field ) ) == string( c_tab_field ) ) )
         id = spec_field_names[ i ];
      else
      {
         string extra, spec_field_name( spec_field_names[ i ] );

         string::size_type pos = spec_field_name.find( ';' );
         if( pos != string::npos )
         {
            extra = spec_field_name.substr( pos + 1 );
            spec_field_name.erase( pos );
         }

         string foreign_field_name;
         pos = spec_field_name.find( ':' );
         if( pos != string::npos )
         {
            foreign_field_name = spec_field_name.substr( pos + 1 );
            spec_field_name.erase( pos );
         }

         string type;
         string name( spec_field_name );

         for( size_t j = 0; j < all_field_data.size( ); j++ )
         {
            if( all_field_data[ j ].decorated_name == name )
            {
               id = all_field_data[ j ].id;
               type = all_field_data[ j ].type;
               is_external = all_field_data[ j ].is_external_field;

               if( !all_field_data[ j ].enum_name.empty( ) )
               {
                  if( !extra.empty( ) )
                     extra += '+';
                  extra += "enum=" + get_id_for_enum_name( m, all_field_data[ j ].enum_name );
               }

               if( !all_field_data[ j ].enum_filter.empty( ) )
               {
                  if( !extra.empty( ) )
                     extra += '+';
                  extra += "enum_filter=" + get_id_for_enum_name( m, all_field_data[ j ].enum_filter );
               }

               break;
            }
         }

         if( id.empty( ) )
            throw runtime_error( "field name '" + name + "' not found in class '" + arg_class_name + "'" );

         if( !is_external )
            field_ids.push_back( id );

         if( !foreign_field_name.empty( ) )
         {
            string foreign_class_name( get_class_name_from_field_type( m, arg_class_name, name, type ) );

            vector< field_data > all_foreign_fields;
            m.get_field_data( foreign_class_name, all_foreign_fields, e_get_field_type_any, true );

            class_data cls_data;
            m.get_class_data( foreign_class_name, cls_data );

            bool needs_order_field = false;
            string ordered_order_field_id;
            if( this->type != "print" && !cls_data.extra.empty( ) )
            {
               set< string > extras;
               split( cls_data.extra, extras );

               if( extras.count( c_class_extra_ordered ) )
                  needs_order_field = true;

               for( size_t j = 0; j < all_foreign_fields.size( ); j++ )
               {
                  if( !all_foreign_fields[ j ].extra.empty( ) )
                  {
                     set< string > extras;
                     split( all_foreign_fields[ j ].extra, extras );

                     if( extras.count( c_field_extra_order ) )
                     {
                        ordered_order_field_id = all_foreign_fields[ j ].id;
                        break;
                     }
                  }
               }
            }

            for( size_t j = 0; j < all_foreign_fields.size( ); j++ )
            {
               if( all_foreign_fields[ j ].name == foreign_field_name )
               {
                  if( !all_foreign_fields[ j ].extra.empty( ) )
                  {
                     if( !extra.empty( ) )
                        extra += '+';
                     extra += all_foreign_fields[ j ].extra;
                  }
               }
            }

            string foreign_field_extra;
            split_extra_from_field( foreign_field_name, foreign_field_extra, all_foreign_fields, true );

            string foreign_field2_name;
            string::size_type pos = foreign_field_name.find( '+' );
            if( pos != string::npos )
            {
               if( !foreign_field_extra.empty( ) )
                  throw runtime_error( "secondary parent field not permitted when extra parent info present" );

               foreign_field2_name = foreign_field_name.substr( pos + 1 );
               foreign_field_name.erase( pos );
            }

            string foreign_field_id, foreign_field2_id;
            for( size_t j = 0; j < all_foreign_fields.size( ); j++ )
            {
               if( all_foreign_fields[ j ].decorated_name == foreign_field_name )
               {
                  foreign_field_id = all_foreign_fields[ j ].id;
                  if( foreign_field2_name.empty( ) )
                     break;
               }

               if( all_foreign_fields[ j ].decorated_name == foreign_field2_name )
                  foreign_field2_id = all_foreign_fields[ j ].id;
            }

            if( foreign_field2_name == c_key_field )
               foreign_field2_id = c_key_field;

            if( foreign_field_id.empty( ) )
               throw runtime_error( "foreign field '" + foreign_field_name + "' not found in class '" + foreign_class_name + "'" );

            if( !foreign_field2_name.empty( ) && foreign_field2_id.empty( ) )
               throw runtime_error( "foreign field '" + foreign_field2_name + "' not found in class '" + foreign_class_name + "'" );

            id += ":" + foreign_field_id;

            if( !foreign_field2_id.empty( ) )
               id += "+" + foreign_field2_id;

            if( !foreign_field_extra.empty( ) )
               id += "!" + foreign_field_extra;
            else if( needs_order_field )
               id += "!" + ordered_order_field_id;
         }

         if( !extra.empty( ) )
            id += ";" + extra;
      }

      field_info.push_back( id );
   }

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );

   for( size_t i = 0; i < field_ids.size( ); i++ )
      details.push_back( specification_detail( field_ids[ i ], "field", e_model_element_type_field ) );
}

void view_specification::read_data( sio_reader& reader )
{
   name = reader.read_attribute( c_attribute_name );
   type = reader.read_attribute( c_attribute_type );
   extra = reader.read_opt_attribute( c_attribute_extra );

   class_id = reader.read_attribute( c_attribute_class_id );
   bclass_info = reader.read_opt_attribute( c_attribute_bclass_info );

   string next;
   while( reader.has_read_attribute( c_attribute_field_info, next ) )
      field_info.push_back( next );
}

void view_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_name, name );
   writer.write_attribute( c_attribute_type, type );
   writer.write_opt_attribute( c_attribute_extra, extra );

   writer.write_attribute( c_attribute_class_id, class_id );
   writer.write_opt_attribute( c_attribute_bclass_info, bclass_info );

   for( size_t i = 0; i < field_info.size( ); i++ )
      writer.write_attribute( c_attribute_field_info, field_info[ i ] );
}

void view_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   string class_name = get_class_name_for_id( m, class_id );
   spec_data.data_pairs.push_back( make_pair( c_data_cid, class_id ) );

   vector< field_data > all_field_data;
   m.get_field_data( class_name, all_field_data, e_get_field_type_any, true );

   spec_data.data_pairs.push_back( make_pair( c_data_name, get_expanded_name( m, class_id, name ) ) );
   spec_data.data_pairs.push_back( make_pair( c_data_type, type ) );
   spec_data.data_pairs.push_back( make_pair( c_data_extra, extra ) );

   string modifiers;
   class_data cls_data;
   m.get_class_data( class_name, cls_data );

   vector< string > all_modifiers;
   split( cls_data.modifiers, all_modifiers );
   for( size_t i = 0; i < all_modifiers.size( ); i++ )
   {
      string::size_type pos = all_modifiers[ i ].find( ';' );
      if( pos == string::npos || all_modifiers[ i ].substr( pos + 1 ) == "view" )
      {
         if( !modifiers.empty( ) )
            modifiers += ',';
         modifiers += all_modifiers[ i ].substr( 0, pos );
      }
   }
   spec_data.data_pairs.push_back( make_pair( c_data_mods, modifiers ) );

   spec_data.data_pairs.push_back( make_pair( c_data_binfo, bclass_info ) );

   string fnums;
   for( size_t i = 0; i < field_info.size( ); i++ )
   {
      string fnum;
      fnum += ( '0' + ( i / 100 ) );
      fnum += ( '0' + ( i % 100 / 10 ) );
      fnum += ( '0' + ( i % 10 ) );

      if( i > 0 )
         fnums += ' ';
      fnums += fnum;
   }
   spec_data.data_pairs.push_back( make_pair( c_data_fnums, fnums ) );

   for( size_t i = 0; i < field_info.size( ); i++ )
   {
      string fnum;
      fnum += ( '0' + ( i / 100 ) );
      fnum += ( '0' + ( i % 100 / 10 ) );
      fnum += ( '0' + ( i % 10 ) );

      string extra, pfield, pfield_info;

      string id( field_info[ i ] );
      string fname;

      string::size_type pos = id.find( ';' );
      if( pos != string::npos )
      {
         extra = id.substr( pos + 1 );
         id.erase( pos );
      }

      pos = id.find( ':' );
      if( pos != string::npos )
      {
         pfield_info = id.substr( pos + 1 );
         id.erase( pos );
      }

      string ftype, fxtra, stype, fmods;
      string fname_key( c_field_prefix );

      bool is_key_field = false;
      bool is_tab_field = false;
      bool is_mandatory = false;
      bool is_foreign_key = false;

      if( id == c_key_field )
      {
         is_key_field = true;
         is_mandatory = true;
         fname = c_key_title;
      }
      else if( id == c_tab_field )
      {
         is_tab_field = true;
         fname_key = c_tab_prefix;

         fname = pfield_info;
         pfield_info.erase( );
      }
      else
      {
         for( size_t j = 0; j < all_field_data.size( ); j++ )
         {
            if( all_field_data[ j ].id == id )
            {
               fname = all_field_data[ j ].name;
               ftype = all_field_data[ j ].type;
               fxtra = all_field_data[ j ].extra;
               stype = all_field_data[ j ].sys_type;
               fmods = all_field_data[ j ].modifiers;
               is_mandatory = all_field_data[ j ].is_mandatory;
               is_foreign_key = all_field_data[ j ].is_foreign_key;

               fname_key += "_" + lower( all_field_data[ j ].class_name );

               if( !fxtra.empty( ) )
               {
                  if( !extra.empty( ) )
                     extra += "+";
                  extra += fxtra;
               }

               break;
            }
         }
      }

      fname_key += "_" + lower( fname );

      // NOTE: Only include "modifiers" that are applicable to "views" here (i.e. exclude
      // any modifiers that have been labelled as being relevant to "lists" only).
      if( !fmods.empty( ) )
      {
         string new_fmods;

         vector< string > all_mods;
         split( fmods, all_mods );

         for( size_t i = 0; i < all_mods.size( ); i++ )
         {
            string next( all_mods[ i ] );
            string::size_type pos = next.find( ';' );

            if( pos != string::npos && next.substr( pos + 1 ) != "view" )
               continue;

            if( !new_fmods.empty( ) )
               new_fmods += ",";
            new_fmods += all_mods[ i ].substr( 0, pos );
         }

         fmods = new_fmods;
      }

      set< string > extras;
      split( extra, extras, '+' );

      if( extras.count( c_field_extra_force_opt ) )
         is_mandatory = false;
      else if( extras.count( c_field_extra_force_mand ) )
         is_mandatory = true;

      spec_data.data_pairs.push_back( make_pair( string( c_data_field ) + fnum, id ) );
      spec_data.data_pairs.push_back( make_pair( string( c_data_ftype ) + fnum, stype ) );
      spec_data.data_pairs.push_back( make_pair( string( c_data_extra ) + fnum, extra ) );
      spec_data.data_pairs.push_back( make_pair( string( c_data_fmods ) + fnum, fmods ) );
      spec_data.data_pairs.push_back( make_pair( string( c_data_fname ) + fnum, fname_key ) );
      spec_data.data_pairs.push_back( make_pair( string( c_data_fmand ) + fnum, is_mandatory ? c_true : c_false ) );

      string parent_class_name;
      if( is_foreign_key )
      {
         parent_class_name = get_class_name_from_field_type( m, ftype );
         spec_data.data_pairs.push_back(
          make_pair( string( c_data_prcls ) + fnum, get_class_id_for_name( m, parent_class_name ) ) );
      }

      string pextra;
      if( !pfield_info.empty( ) )
      {
         bool has_secondary_field = false;
         string pdname, pfield_id( pfield_info ), index_field_id( pfield_info );

         vector< field_data > all_parent_fields;
         m.get_field_data( parent_class_name, all_parent_fields, e_get_field_type_any, true );

         bool is_indexed = false;
         // NOTE: If extra parent info has been provided then the indexed status will be
         // left false as it is assumed that the extra parent info will contain ordering.
         string::size_type pos = pfield_info.find( '!' );
         if( pos != string::npos )
         {
            pextra = pfield_info.substr( pos + 1 );
            pfield_info.erase( pos );
            pfield_id = pfield_info;

            for( size_t j = 0; j < all_parent_fields.size( ); j++ )
            {
               if( all_parent_fields[ j ].id == pfield_id )
               {
                  pfield = all_parent_fields[ j ].name;
                  break;
               }
            }
         }
         else
         {
            // NOTE: If a secondary field has been provided then the indexed
            // status will be determined according to this field.
            pos = pfield_info.find( '+' );
            if( pos != string::npos )
            {
               has_secondary_field = true;

               pfield_id.erase( pos );
               index_field_id.erase( 0, pos + 1 );
            }

            if( index_field_id == c_key_field )
               is_indexed = true;
            else
            {
               string index_field_name( pfield );

               for( size_t j = 0; j < all_parent_fields.size( ); j++ )
               {
                  if( all_parent_fields[ j ].id == pfield_id )
                     pfield = all_parent_fields[ j ].name;

                  if( all_parent_fields[ j ].id == index_field_id )
                  {
                     index_field_name = all_parent_fields[ j ].name;
                     if( has_secondary_field )
                        pdname = parent_class_name + '_' + index_field_name;
                  }
               }

               vector< index_data > all_index_data;
               m.get_index_data( parent_class_name, all_index_data, true );

               for( size_t j = 0; j < all_index_data.size( ); j++ )
               {
                  vector< string > fields;
                  split( all_index_data[ j ].name, fields );

                  if( fields[ 0 ] == index_field_name )
                  {
                     is_indexed = true;
                     break;
                  }
               }
            }
         }

         spec_data.data_pairs.push_back( make_pair( string( c_data_prcnm ) + fnum, parent_class_name ) );
         spec_data.data_pairs.push_back( make_pair( string( c_data_prfld ) + fnum, pfield_info ) );
         spec_data.data_pairs.push_back( make_pair( string( c_data_prfnm ) + fnum, pfield ) );
         if( !pdname.empty( ) )
            spec_data.data_pairs.push_back( make_pair( string( c_data_pdfld ) + fnum, pdname ) );
         spec_data.data_pairs.push_back( make_pair( string( c_data_prxtr ) + fnum, pextra ) );
         spec_data.data_pairs.push_back( make_pair( string( c_data_pridx ) + fnum, is_indexed ? c_true : c_false ) );
      }
   }
}

void view_specification::get_specification_strings( model& m, vector< pair< string, string > >& strings ) const
{
   strings.push_back( make_pair( "name", get_expanded_name( m, class_id, name ) ) );

   for( size_t i = 0; i < field_info.size( ); i++ )
   {
      if( field_info[ i ].length( ) > strlen( c_tab_field )
       && field_info[ i ].substr( 0, strlen( c_tab_field ) ) == string( c_tab_field ) )
      {
         size_t pos = field_info[ i ].find( ':' );
         if( pos != string::npos )
            strings.push_back( make_pair( string( c_tab_prefix )
             + "_" + lower( field_info[ i ].substr( pos + 1 ) ), field_info[ i ].substr( pos + 1 ) ) );
      }
   }
}

string view_specification::static_class_name( ) { return "view"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, view_specification, static_class_name );

struct user_info_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_non_model; }

   bool is_interface_specification( ) const { return true; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string class_id;
   string uid_field_id;
   string pwd_field_id;
   string hash_field_id;
   string perm_field_id;
   string group_field_id;
   string other_field_id;
   string parent_field_id;
   string active_field_id;

   string select_field_id;
   string select_permission;
   string select_order_field_id;
   string select_parent_class_id;
   string select_parent_field_id;
   string select_parent_cfield_id;
};

void user_info_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) < 4 )
      throw runtime_error( "user_info specification requires 'user class', 'uid field', 'pwd field' and 'user_hash field'" );

   string arg_class_name( args[ 0 ] );
   string arg_uid_field_name( args[ 1 ] );
   string arg_pwd_field_name( args[ 2 ] );
   string arg_hash_field_name( args[ 3 ] );

   string arg_select_info;
   string arg_perm_field_name, arg_group_field_name, arg_other_field_name, arg_parent_field_name, arg_active_field_name;

   for( size_t arg = 4; arg < args.size( ); arg++ )
   {
      string next_arg( args[ arg ] );

      if( next_arg.find( c_arg_perm_prefix ) == 0 )
         arg_perm_field_name = next_arg.substr( strlen( c_arg_perm_prefix ) );
      else if( next_arg.find( c_arg_group_prefix ) == 0 )
         arg_group_field_name = next_arg.substr( strlen( c_arg_group_prefix ) );
      else if( next_arg.find( c_arg_other_prefix ) == 0 )
         arg_other_field_name = next_arg.substr( strlen( c_arg_other_prefix ) );
      else if( next_arg.find( c_arg_active_prefix ) == 0 )
         arg_active_field_name = next_arg.substr( strlen( c_arg_active_prefix ) );
      else if( next_arg.find( c_arg_parent_prefix ) == 0 )
         arg_parent_field_name = next_arg.substr( strlen( c_arg_parent_prefix ) );
      else if( next_arg.find( c_arg_select_prefix ) == 0 )
         arg_select_info = next_arg.substr( strlen( c_arg_select_prefix ) );
      else
         throw runtime_error( "unexpected extra argument '" + next_arg + "' for user_info specification" );
   }

   string select_field_name, select_order_field_name, select_parent_field_name, select_parent_cfield_name;
   if( !arg_select_info.empty( ) )
   {
      string::size_type pos = arg_select_info.find( ':' );
      if( pos == string::npos )
         throw runtime_error( "unexpected format '" + arg_select_info + "' for select info argument" );

      select_field_name = arg_select_info.substr( 0, pos );
      select_parent_field_name = arg_select_info.substr( pos + 1 );

      if( select_field_name[ 0 ] == '~' )
      {
         select_field_name.erase( 0, 1 );
         select_parent_cfield_name = arg_class_name;
      }

      pos = select_parent_field_name.find( '#' );
      if( pos != string::npos )
      {
         select_permission = select_parent_field_name.substr( pos + 1 );
         select_parent_field_name.erase( pos );
      }

      pos = select_parent_field_name.find( '!' );
      if( pos != string::npos )
      {
         select_order_field_name = select_parent_field_name.substr( pos + 1 );
         select_parent_field_name.erase( pos );
      }
   }

   bool is_external_alias;
   string base_class_name;
   class_id = get_class_id_for_name( m, arg_class_name, &is_external_alias, &base_class_name );

   vector< field_data > all_field_data;
   m.get_field_data( arg_class_name, all_field_data, e_get_field_type_any, true );

   uid_field_id = get_field_id_for_name( all_field_data, arg_uid_field_name );

   if( uid_field_id.empty( ) )
      throw runtime_error( "class '" + arg_class_name + "' does not contain the field '" + arg_uid_field_name + "'" );

   pwd_field_id = get_field_id_for_name( all_field_data, arg_pwd_field_name );

   if( pwd_field_id.empty( ) )
      throw runtime_error( "class '" + arg_class_name + "' does not contain the field '" + arg_pwd_field_name + "'" );

   hash_field_id = get_field_id_for_name( all_field_data, arg_hash_field_name );

   if( hash_field_id.empty( ) )
      throw runtime_error( "class '" + arg_class_name + "' does not contain the field '" + arg_hash_field_name + "'" );

   if( !arg_perm_field_name.empty( ) )
   {
      perm_field_id = get_field_id_for_name( all_field_data, arg_perm_field_name, 0, true );

      if( perm_field_id.empty( ) )
         throw runtime_error( "class '" + arg_class_name + "' does not contain the field '" + arg_perm_field_name + "'" );
   }

   if( !arg_group_field_name.empty( ) )
   {
      group_field_id = get_field_id_for_name( all_field_data, arg_group_field_name, 0, true );

      if( group_field_id.empty( ) )
         throw runtime_error( "class '" + arg_class_name + "' does not contain the field '" + arg_group_field_name + "'" );
   }

   if( !arg_other_field_name.empty( ) )
   {
      other_field_id = get_field_id_for_name( all_field_data, arg_other_field_name, 0, true );

      if( other_field_id.empty( ) )
         throw runtime_error( "class '" + arg_class_name + "' does not contain the field '" + arg_other_field_name + "'" );
   }

   if( !arg_active_field_name.empty( ) )
   {
      active_field_id = get_field_id_for_name( all_field_data, arg_active_field_name );

      if( active_field_id.empty( ) )
         throw runtime_error( "class '" + arg_class_name + "' does not contain the field '" + arg_active_field_name + "'" );
   }

   if( !arg_parent_field_name.empty( ) )
   {
      string parent_type_name;
      parent_field_id = get_field_id_for_name( all_field_data, arg_parent_field_name, &parent_type_name, true );

      if( parent_field_id.empty( ) )
         throw runtime_error( "class '" + arg_class_name + "' does not contain the field '" + arg_parent_field_name + "'" );

      string parent_class_name( get_class_name_from_field_type( m, arg_class_name, arg_parent_field_name, parent_type_name ) );

      if( parent_class_name != arg_class_name )
         throw runtime_error( "field '" + arg_parent_field_name + "' is not a self relationship for class '" + arg_class_name + "'" );
   }

   if( !select_field_name.empty( ) )
   {
      string select_type_name;
      select_field_id = get_field_id_for_name( all_field_data, select_field_name, &select_type_name, true );

      if( select_field_id.empty( ) )
         throw runtime_error( "class '" + arg_class_name + "' does not contain the field '" + select_field_name + "'" );

      string select_class_name( get_class_name_from_field_type(
       m, arg_class_name, select_field_name, select_type_name ) );

      class_data select_class_data;
      m.get_class_data( select_class_name, select_class_data );

      select_parent_class_id = get_class_id_for_name( m, select_class_name );

      vector< field_data > all_parent_field_data;
      m.get_field_data( select_class_name, all_parent_field_data, e_get_field_type_any, true );

      select_parent_field_id = get_field_id_for_name( all_parent_field_data, select_parent_field_name );

      if( select_parent_field_id.empty( ) )
         throw runtime_error( "field name '"
          + select_parent_field_name + "' not found in class '" + select_class_name + "'" );

      if( !select_parent_cfield_name.empty( ) )
      {
         select_parent_cfield_id = get_field_id_for_name( all_parent_field_data, select_parent_cfield_name );

         if( select_parent_cfield_id.empty( ) )
            throw runtime_error( "field name '"
             + select_parent_cfield_name + "' not found in class '" + select_class_name + "'" );
      }

      bool select_needs_order_field = false;
      string select_ordered_order_field_id;
      if( !select_class_data.extra.empty( ) )
      {
         set< string > extras;
         split( select_class_data.extra, extras );

         if( extras.count( c_class_extra_ordered ) )
            select_needs_order_field = true;

         for( size_t i = 0; i < all_parent_field_data.size( ); i++ )
         {
            if( !all_parent_field_data[ i ].extra.empty( ) )
            {
               set< string > extras;
               split( all_parent_field_data[ i ].extra, extras );

               if( extras.count( c_field_extra_order ) )
               {
                  select_ordered_order_field_id = all_parent_field_data[ i ].id;
                  break;
               }
            }
         }
      }

      if( !select_order_field_name.empty( ) )
      {
         select_order_field_id = get_field_id_for_name( all_parent_field_data, select_order_field_name );

         if( select_order_field_id.empty( ) )
            throw runtime_error( "field name '"
             + select_order_field_name + "' not found in class '" + select_class_name + "'" );
      }
      else if( select_needs_order_field )
         select_order_field_id = select_ordered_order_field_id;
   }

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );

   if( !is_external_alias )
   {
      details.push_back( specification_detail( uid_field_id, "uid", e_model_element_type_field ) );
      details.push_back( specification_detail( pwd_field_id, "pwd", e_model_element_type_field ) );
      details.push_back( specification_detail( hash_field_id, "hash", e_model_element_type_field ) );

      if( !perm_field_id.empty( ) )
         details.push_back( specification_detail( perm_field_id, "perm", e_model_element_type_field ) );

      if( !group_field_id.empty( ) )
         details.push_back( specification_detail( group_field_id, "group", e_model_element_type_field ) );

      if( !other_field_id.empty( ) )
         details.push_back( specification_detail( other_field_id, "other", e_model_element_type_field ) );

      if( !parent_field_id.empty( ) )
         details.push_back( specification_detail( parent_field_id, "parent", e_model_element_type_field ) );

      if( !active_field_id.empty( ) )
         details.push_back( specification_detail( active_field_id, "active", e_model_element_type_field ) );
   }
}

void user_info_specification::read_data( sio_reader& reader )
{
   class_id = reader.read_attribute( c_attribute_class_id );
   uid_field_id = reader.read_attribute( c_attribute_uid_field_id );
   pwd_field_id = reader.read_attribute( c_attribute_pwd_field_id );
   hash_field_id = reader.read_attribute( c_attribute_hash_field_id );
   perm_field_id = reader.read_opt_attribute( c_attribute_perm_field_id );
   group_field_id = reader.read_opt_attribute( c_attribute_gfield_id );
   other_field_id = reader.read_opt_attribute( c_attribute_ofield_id );
   parent_field_id = reader.read_opt_attribute( c_attribute_pfield_id );
   active_field_id = reader.read_opt_attribute( c_attribute_active_id );

   select_field_id = reader.read_opt_attribute( c_attribute_select_id );
   select_permission = reader.read_opt_attribute( c_attribute_select_perm );
   select_order_field_id = reader.read_opt_attribute( c_attribute_select_ofield_id );
   select_parent_class_id = reader.read_opt_attribute( c_attribute_select_pclass_id );
   select_parent_field_id = reader.read_opt_attribute( c_attribute_select_pfield_id );
   select_parent_cfield_id = reader.read_opt_attribute( c_attribute_select_pcfield_id );
}

void user_info_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, class_id );
   writer.write_attribute( c_attribute_uid_field_id, uid_field_id );
   writer.write_attribute( c_attribute_pwd_field_id, pwd_field_id );
   writer.write_attribute( c_attribute_hash_field_id, hash_field_id );
   writer.write_opt_attribute( c_attribute_perm_field_id, perm_field_id );
   writer.write_opt_attribute( c_attribute_gfield_id, group_field_id );
   writer.write_opt_attribute( c_attribute_ofield_id, other_field_id );
   writer.write_opt_attribute( c_attribute_pfield_id, parent_field_id );
   writer.write_opt_attribute( c_attribute_active_id, active_field_id );

   writer.write_opt_attribute( c_attribute_select_id, select_field_id );
   writer.write_opt_attribute( c_attribute_select_perm, select_permission );
   writer.write_opt_attribute( c_attribute_select_ofield_id, select_order_field_id );
   writer.write_opt_attribute( c_attribute_select_pclass_id, select_parent_class_id );
   writer.write_opt_attribute( c_attribute_select_pfield_id, select_parent_field_id );
   writer.write_opt_attribute( c_attribute_select_pcfield_id, select_parent_cfield_id );
}

void user_info_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   string class_name = get_class_name_for_id( m, class_id );
   spec_data.data_pairs.push_back( make_pair( c_data_cid, class_id ) );
   spec_data.data_pairs.push_back( make_pair( c_data_class, class_name ) );

   spec_data.data_pairs.push_back( make_pair( c_data_uid, uid_field_id ) );
   spec_data.data_pairs.push_back( make_pair( c_data_pwd, pwd_field_id ) );
   spec_data.data_pairs.push_back( make_pair( c_data_hash, hash_field_id ) );

   spec_data.data_pairs.push_back( make_pair( c_data_perm, perm_field_id ) );

   spec_data.data_pairs.push_back( make_pair( c_data_group, group_field_id ) );
   spec_data.data_pairs.push_back( make_pair( c_data_other, other_field_id ) );
   spec_data.data_pairs.push_back( make_pair( c_data_parent, parent_field_id ) );
   spec_data.data_pairs.push_back( make_pair( c_data_active, active_field_id ) );

   spec_data.data_pairs.push_back( make_pair( c_data_sfield, select_field_id ) );
   spec_data.data_pairs.push_back( make_pair( c_data_slperm, select_permission ) );
   spec_data.data_pairs.push_back( make_pair( c_data_ofield, select_order_field_id ) );
   spec_data.data_pairs.push_back( make_pair( c_data_pfield, select_parent_field_id ) );
   spec_data.data_pairs.push_back( make_pair( c_data_pcfield, select_parent_cfield_id ) );

   string select_parent_class_name;
   if( !select_parent_class_id.empty( ) )
      select_parent_class_name = get_class_name_for_id( m, select_parent_class_id );
   spec_data.data_pairs.push_back( make_pair( c_data_sclsnm, select_parent_class_name ) );

   spec_data.data_pairs.push_back( make_pair( "ostrict", "" ) );
   spec_data.data_pairs.push_back( make_pair( "koverride", "" ) );
   spec_data.data_pairs.push_back( make_pair( "soverride", "" ) );
}

string user_info_specification::static_class_name( ) { return "user_info"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, user_info_specification, static_class_name );

struct user_key_gen_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_model; }

   bool is_facet_specification( ) const { return true; }

   static string static_class_name( );
};

void user_key_gen_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) < 3 )
      throw runtime_error( "key_generator specification requires 'class', 'user class' and 'key gen procedure'" );

   string arg_class_name( args[ 0 ] );
   string arg_user_class_name( args[ 1 ] );
   string arg_key_gen_procedure_name( args[ 2 ] );

   string class_id = get_class_id_for_name( m, arg_class_name );
   string user_class_id = get_class_id_for_name( m, arg_user_class_name );
   string key_gen_procedure_id = get_procedure_id_for_name( m, arg_user_class_name, arg_key_gen_procedure_name );

   if( key_gen_procedure_id.empty( ) )
      throw runtime_error( "unknown procedure '" + arg_key_gen_procedure_name + "' for class '" + arg_user_class_name + "'" );

   details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );
   details.push_back( specification_detail( user_class_id, "user_class", e_model_element_type_class ) );
   details.push_back( specification_detail( key_gen_procedure_id, "key_gen_procedure", e_model_element_type_procedure ) );
}

string user_key_gen_specification::static_class_name( ) { return "user_key_gen"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, user_key_gen_specification, static_class_name );

struct qlink_info_specification : specification
{
   void add( model& m, const vector< string >& args, vector< specification_detail >& details );

   specification_data_source get_specification_data_source( ) const { return e_specification_data_source_non_model; }

   bool is_interface_specification( ) const { return true; }

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   void add_specification_data( model& m, specification_data& spec_data ) const;

   static string static_class_name( );

   string class_id;
   string url_field_id;
   string name_field_id;
   string order_field_id;
   string parent_field_id;
   string checksum_field_id;

   string permission;
};

void qlink_info_specification::add( model& m, const vector< string >& args, vector< specification_detail >& details )
{
   if( args.size( ) < 6 )
      throw runtime_error( "user_info specification requires 'class', 'url', 'name', 'order', 'parent' and 'checksum'" );

   string arg_class_name( args[ 0 ] );
   string arg_url_field_name( args[ 1 ] );
   string arg_name_field_name( args[ 2 ] );
   string arg_order_field_name( args[ 3 ] );
   string arg_parent_field_name( args[ 4 ] );
   string arg_checksum_field_name( args[ 5 ] );

   if( args.size( ) > 6 )
      permission = args[ 6 ];

   bool is_external_alias;
   string base_class_name;
   class_id = get_class_id_for_name( m, arg_class_name, &is_external_alias, &base_class_name );

   url_field_id = get_field_id_for_name( m, arg_class_name, arg_url_field_name );

   if( url_field_id.empty( ) )
      throw runtime_error( "class '" + arg_class_name + "' does not contain the field '" + arg_url_field_name + "'" );

   name_field_id = get_field_id_for_name( m, arg_class_name, arg_name_field_name );

   if( name_field_id.empty( ) )
      throw runtime_error( "class '" + arg_class_name + "' does not contain the field '" + arg_name_field_name + "'" );

   order_field_id = get_field_id_for_name( m, arg_class_name, arg_order_field_name );

   if( order_field_id.empty( ) )
      throw runtime_error( "class '" + arg_class_name + "' does not contain the field '" + arg_order_field_name + "'" );

   parent_field_id = get_field_id_for_name( m, arg_class_name, arg_parent_field_name, 0, true );

   if( parent_field_id.empty( ) )
      throw runtime_error( "class '" + arg_class_name + "' does not contain the field '" + arg_parent_field_name + "'" );

   checksum_field_id = get_field_id_for_name( m, arg_class_name, arg_checksum_field_name );

   if( checksum_field_id.empty( ) )
      throw runtime_error( "class '" + arg_class_name + "' does not contain the field '" + arg_checksum_field_name + "'" );

   if( !is_external_alias )
   {
      details.push_back( specification_detail( class_id, "class", e_model_element_type_class ) );

      details.push_back( specification_detail( url_field_id, "url", e_model_element_type_field ) );
      details.push_back( specification_detail( name_field_id, "name", e_model_element_type_field ) );
      details.push_back( specification_detail( order_field_id, "order", e_model_element_type_field ) );
      details.push_back( specification_detail( parent_field_id, "parent", e_model_element_type_field ) );
      details.push_back( specification_detail( checksum_field_id, "checksum", e_model_element_type_field ) );
   }
}

void qlink_info_specification::read_data( sio_reader& reader )
{
   class_id = reader.read_attribute( c_attribute_class_id );

   url_field_id = reader.read_attribute( c_attribute_url_field_id );
   name_field_id = reader.read_attribute( c_attribute_name_field_id );
   order_field_id = reader.read_attribute( c_attribute_order_field_id );
   parent_field_id = reader.read_attribute( c_attribute_parent_field_id );
   checksum_field_id = reader.read_attribute( c_attribute_checksum_field_id );

   permission = reader.read_opt_attribute( c_attribute_permission );
}

void qlink_info_specification::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_class_id, class_id );

   writer.write_attribute( c_attribute_url_field_id, url_field_id );
   writer.write_attribute( c_attribute_name_field_id, name_field_id );
   writer.write_attribute( c_attribute_order_field_id, order_field_id );
   writer.write_attribute( c_attribute_parent_field_id, parent_field_id );
   writer.write_attribute( c_attribute_checksum_field_id, checksum_field_id );

   writer.write_opt_attribute( c_attribute_permission, permission );
}

void qlink_info_specification::add_specification_data( model& m, specification_data& spec_data ) const
{
   spec_data.data_pairs.push_back( make_pair( c_data_cid, class_id ) );

   spec_data.data_pairs.push_back( make_pair( c_data_url, url_field_id ) );
   spec_data.data_pairs.push_back( make_pair( c_data_name, name_field_id ) );
   spec_data.data_pairs.push_back( make_pair( c_data_order, order_field_id ) );
   spec_data.data_pairs.push_back( make_pair( c_data_parent, parent_field_id ) );
   spec_data.data_pairs.push_back( make_pair( c_data_checksum, checksum_field_id ) );

   spec_data.data_pairs.push_back( make_pair( c_data_perm, permission ) );
}

string qlink_info_specification::static_class_name( ) { return "qlink_info"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, specification, qlink_info_specification, static_class_name );

