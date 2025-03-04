// Copyright (c) 2017-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <ctime>
#  include <map>
#  include <set>
#  include <deque>
#  include <vector>
#  include <fstream>
#  include <iostream>
#  include <stdexcept>
#endif

#define CIYAM_BASE_IMPL

#include "ciyam_packages.h"

#include "sio.h"
#include "utilities.h"
#include "ciyam_base.h"
#include "class_base.h"
#include "command_handler.h"
#include "module_management.h"

using namespace std;

namespace
{

#include "ciyam_constants.h"

const char* const c_section_class = "class";

const char* const c_attribute_name = "name";
const char* const c_attribute_fields = "fields";
const char* const c_attribute_record = "record";

void export_data( ostream& outs,
 const string& module, string class_id, const string& key,
 string& last_class_id, bool output_children, command_handler& handler,
 const map< string, int >& all_class_ids, const map< string, map< string, string > >& skip_fields,
 const map< string, set< string > >& excludes, const map< string, map< string, string > >& tests,
 const map< string, set< string > >& includes, map< string, set< string > >& exported_records,
 map< string, set< string > >& exported_children, deque< pair< string, string > >& next_pass,
 map< string, set< string > >& will_be_exported, map< string, set< string > >& partial_export,
 const map< string, int >& rounds, int current_round,
 map< int, deque< pair< string, string > > >& future_rounds, time_t& ts, size_t& total )
{
   size_t handle = create_object_instance( module, class_id );

   class_base& cb( get_class_base_from_handle_for_op( handle, "" ) );

   string class_name;

   vector< field_data > all_field_data;
   vector< pair< string, string > > base_class_info;

   get_all_field_data( handle, "", key, all_field_data, &class_id, &class_name, &base_class_info );

   if( !all_class_ids.count( class_id ) )
      throw runtime_error( "unable to find '" + class_id + "' in all_class_ids for export" );

   string exclude_suffix;

   bool has_seen = false;
   bool skip_record = false;

   set< string > first_output_ids;

   if( partial_export[ class_id ].count( key ) )
   {
      has_seen = true;

      partial_export[ class_id ].erase( key );

      for( size_t i = 0; i < base_class_info.size( ); i++ )
         partial_export[ base_class_info[ i ].first ].erase( key );
   }

   if( will_be_exported[ class_id ].count( key ) )
   {
      has_seen = true;

      will_be_exported[ class_id ].erase( key );

      for( size_t i = 0; i < base_class_info.size( ); i++ )
         will_be_exported[ base_class_info[ i ].first ].erase( key );
   }

   if( tests.count( class_id ) )
   {
      if( tests.find( class_id )->second.count( "*" ) )
         skip_record = true;
      else
      {
         for( size_t i = 0; i < all_field_data.size( ); i++ )
         {
            string next_value( all_field_data[ i ].value );

            string test_expr;

            set< string > key_values;

            if( tests.find( class_id )->second.count( c_key_field ) )
            {
               next_value = key;
               test_expr = tests.find( class_id )->second.find( c_key_field )->second;
            }
            else if( tests.find( class_id )->second.count( all_field_data[ i ].id ) )
               test_expr = tests.find( class_id )->second.find( all_field_data[ i ].id )->second;
            else if( tests.find( class_id )->second.count( all_field_data[ i ].name ) )
               test_expr = tests.find( class_id )->second.find( all_field_data[ i ].name )->second;

            if( !test_expr.empty( ) )
            {
               string suffix;

               string::size_type pos = test_expr.find( ';' );

               if( pos != string::npos )
               {
                  suffix = test_expr.substr( pos );
                  test_expr.erase( pos );
               }

               bool is_negative = false;

               if( !test_expr.empty( ) && test_expr[ 0 ] == '!' )
               {
                  is_negative = true;
                  test_expr.erase( 0, 1 );
               }

               if( !test_expr.empty( ) && test_expr[ 0 ] == '=' )
               {
                  test_expr.erase( 0, 1 );

                  set< string > all_tests;
                  split( test_expr, all_tests );

                  if( ( is_negative && all_tests.count( next_value ) )
                   || ( !is_negative && !all_tests.count( next_value ) ) )
                  {
                     if( suffix.empty( ) )
                        skip_record = true;
                     else
                        exclude_suffix = suffix;

                     break;
                  }
               }
               else if( test_expr[ 0 ] == '#' )
                  first_output_ids.insert( all_field_data[ i ].id );
               else
                  throw runtime_error( "unexpected test_expr '" + test_expr + "'" );
            }
         }
      }
   }

   if( !skip_record )
   {
      string all_values( key );

      bool need_to_repeat = false;
      bool has_exported_fk = false;

      int position = all_class_ids.find( class_id )->second;

      for( size_t i = 0; i < all_field_data.size( ); i++ )
      {
         if( all_field_data[ i ].transient )
            continue;

         string next_value( all_field_data[ i ].value );

         if( skip_fields.count( class_id )
          && skip_fields.find( class_id )->second.count( all_field_data[ i ].id ) )
         {
            string value( skip_fields.find( class_id )->second.find( all_field_data[ i ].id )->second );

            // NOTE: The whole record is being skipped (not just the column) for this case.
            if( !value.empty( ) && value == next_value )
            {
               all_values.erase( );
               break;
            }

            continue;
         }

         if( !next_value.empty( ) && !all_field_data[ i ].class_id.empty( )
          && !partial_export[ all_field_data[ i ].class_id ].count( next_value )
          && !exported_records[ all_field_data[ i ].class_id ].count( next_value ) )
         {
            if( !all_class_ids.count( all_field_data[ i ].class_id ) )
               throw runtime_error( "unable to find '" + all_field_data[ i ].class_id
                + "' in all_class_ids for export (for field " + all_field_data[ i ].id + ")" );

            bool output_fk_children = false;

            if( includes.count( class_id )
             && ( includes.find( class_id )->second.count( all_field_data[ i ].id )
             || includes.find( class_id )->second.count( all_field_data[ i ].name ) ) )
               output_fk_children = true;

            if( first_output_ids.count( all_field_data[ i ].id )
             || all_class_ids.find( all_field_data[ i ].class_id )->second < position )
            {
               export_data( outs, module, all_field_data[ i ].class_id, next_value,
                last_class_id, output_fk_children, handler, all_class_ids, skip_fields,
                excludes, tests, includes, exported_records, exported_children, next_pass,
                will_be_exported, partial_export, rounds, current_round, future_rounds, ts, total );

               if( exported_records[ all_field_data[ i ].class_id ].count( next_value ) )
                  has_exported_fk = true;
            }
            else if( !all_field_data[ i ].mandatory )
            {
               // NOTE: If a foreign key cannot be processed first but is not mandatory (as is often
               // the case with self relationships) then the record will be output without the value
               // and the foreign key record will be processed in the next pass. As the full details
               // are missing the record itself will need to be repeated (see after this loop). This
               // approach of allowing partial records to be initially output solves interdependency
               // issues that can occur especially with self relationships.
               if( !will_be_exported[ all_field_data[ i ].class_id ].count( next_value ) )
               {
                  will_be_exported[ all_field_data[ i ].class_id ].insert( next_value );
                  next_pass.push_back( make_pair( all_field_data[ i ].class_id, next_value ) );
               }

               next_value.erase( );
               need_to_repeat = true;
            }
            else
               throw runtime_error( "unable to determine export ordering for "
                + all_field_data[ i ].class_id + " whilst processing " + class_id );
         }

         all_values += ',';
         all_values += escaped( next_value, ",\"", '\\', "rn\r\n" );
      }

      if( !all_values.empty( ) )
      {
         if( need_to_repeat )
         {
            if( !will_be_exported[ class_id ].count( key ) )
            {
               will_be_exported[ class_id ].insert( key );
               for( size_t i = 0; i < base_class_info.size( ); i++ )
                  will_be_exported[ base_class_info[ i ].first ].insert( key );

               next_pass.push_back( make_pair( class_id, key ) );

               partial_export[ class_id ].insert( key );
               for( size_t i = 0; i < base_class_info.size( ); i++ )
                  partial_export[ base_class_info[ i ].first ].insert( key );
            }
         }

         if( !exported_records[ class_id ].count( key ) )
         {
            if( class_id != last_class_id )
            {
               if( !last_class_id.empty( ) )
                  outs << " </class>\n";

               outs << " <class/>\n";
               outs << "  <name>" << class_name << '\n';

               string field_info( "  <fields>" + to_string( c_key_field ) );
               for( size_t i = 0; i < all_field_data.size( ); i++ )
               {
                  if( all_field_data[ i ].transient )
                     continue;

                  if( skip_fields.count( class_id )
                  && skip_fields.find( class_id )->second.count( all_field_data[ i ].id ) )
                     continue;

                  field_info += ',';
                  field_info += all_field_data[ i ].name;
               }

               outs << field_info << '\n';
            }

            outs << "  <record>" << all_values << '\n';

            ++total;
            last_class_id = class_id;

            if( !need_to_repeat )
            {
               exported_records[ class_id ].insert( key );
               for( size_t i = 0; i < base_class_info.size( ); i++ )
                  exported_records[ base_class_info[ i ].first ].insert( key );
            }

            if( time( 0 ) - ts >= 10 )
            {
               ts = time( 0 );
               // FUTURE: This message should be handled as a server string message.
               handler.output_progress( "Processed " + to_string( total ) + " records..." );
            }
         }

         if( output_children && !exported_children[ class_id ].count( key ) )
         {
            class_base_accessor instance_accessor( cb );

            size_t num_children = instance_accessor.get_num_foreign_key_children( );

            for( int pass = 0; pass < 2; ++pass )
            {
               cascade_op next_op;
               if( pass == 0 )
                  next_op = e_cascade_op_restrict;
               else
                  next_op = e_cascade_op_destroy;

               for( size_t i = 0; i < num_children; i++ )
               {
                  string next_child_field;
                  class_base* p_class_base = instance_accessor.get_next_foreign_key_child( i, next_child_field, next_op );

                  string child_class_and_field;

                  if( p_class_base )
                  {
                     child_class_and_field = p_class_base->get_class_id( ) + "#" + next_child_field;

                     if( excludes.count( class_id )
                      && ( excludes.find( class_id )->second.count( "*" + exclude_suffix )
                      || excludes.find( class_id )->second.count( p_class_base->get_class_id( ) + exclude_suffix )
                      || excludes.find( class_id )->second.count( child_class_and_field + exclude_suffix ) ) )
                        continue;

                     string key_info( p_class_base->get_order_field_name( ) );
                     if( !key_info.empty( ) )
                        key_info += ' ';

                     if( ( !key_info.empty( ) && p_class_base->iterate_forwards( key_info ) )
                      || ( key_info.empty( ) && p_class_base->iterate_forwards( true, 0, e_sql_optimisation_unordered ) ) )
                     {
                        do
                        {
                           if( rounds.count( p_class_base->get_class_id( ) )
                            && rounds.find( p_class_base->get_class_id( ) )->second > current_round )
                           {
                              int round( rounds.find( p_class_base->get_class_id( ) )->second );

                              future_rounds[ round ].push_back(
                               make_pair( p_class_base->get_class_id( ), p_class_base->get_key( ) ) );

                              continue;
                           }

                           export_data( outs, module,
                            p_class_base->get_class_id( ), p_class_base->get_key( ),
                            last_class_id, true, handler, all_class_ids, skip_fields,
                            excludes, tests, includes, exported_records, exported_children, next_pass,
                            will_be_exported, partial_export, rounds, current_round, future_rounds, ts, total );

                        } while( p_class_base->iterate_next( ) );
                     }
                  }
               }
            }

            exported_children[ class_id ].insert( key );

            for( size_t i = 0; i < base_class_info.size( ); i++ )
               exported_children[ base_class_info[ i ].first ].insert( key );
         }
      }
   }

   destroy_object_instance( handle );
}

void read_skip_fields( const string& module_id, const string& skip_field_info, map< string, map< string, string > >& skip_fields )
{
   if( !skip_field_info.empty( ) )
   {
      vector< string > skip_field_items;

      // NOTE: The skip info can alternatively be placed in an external list file for convenience.
      // Each skip entry is in the format <class>:<field>[=<value>] where both "class" and "field"
      // can be specified as either names or id's. If specified as <class>:<field> then all values
      // for the name field are ignored (i.e. as though the field was not in each record). For the
      // use <class>:<field>=[<value>] then the record itself will be skipped if the field's value
      // matches that specified.
      if( skip_field_info[ 0 ] != '@' )
         split( skip_field_info, skip_field_items );
      else
         buffer_file_lines( skip_field_info.substr( 1 ), skip_field_items );

      for( size_t i = 0; i < skip_field_items.size( ); i++ )
      {
         string::size_type pos = skip_field_items[ i ].find( ':' );

         if( pos == string::npos )
            throw runtime_error( "invalid skip_field_info item format '" + skip_field_items[ i ] + "'" );

         string sclass( skip_field_items[ i ].substr( 0, pos ) );

         string xinfo( "for skip_field_info '" + skip_field_items[ i ] + "'" );

         sclass = resolve_class_id( module_id, sclass, xinfo );

         string sfield( skip_field_items[ i ].substr( pos + 1 ) );

         string svalue;

         pos = sfield.find( '=' );

         if( pos != string::npos )
         {
            svalue = sfield.substr( pos + 1 );
            sfield.erase( pos );
         }

         sfield = resolve_field_id( module_id, sclass, sfield, xinfo );

         skip_fields[ sclass ][ sfield ] = svalue;
      }
   }
}

void create_new_package_file( const string& module_id, const string& filename,
 set< string >* p_found_keys = 0, set< string >* p_skipped_keys = 0, map< string, map< string, string > >* p_skip_fields = 0 )
{
   ifstream inpf( filename.c_str( ) );

   if( !inpf )
      throw runtime_error( "unable to open file '" + filename + "' for input" );

   vector< string > initial_comments;
   sio_reader reader( inpf, true, &initial_comments );

   string new_filename( filename + ".new" );

   ofstream outf( new_filename.c_str( ) );
   if( !outf )
      throw runtime_error( "unable to open file '" + new_filename + "' for output" );

   sio_writer writer( outf, &initial_comments );

   while( reader.has_started_section( c_section_class ) )
   {
      string comment;

      while( reader.has_read_comment( comment ) )
         writer.write_comment( comment );

      writer.start_section( c_section_class );

      string mclass( reader.read_attribute( c_attribute_name ) );

      writer.write_attribute( c_attribute_name, mclass );

      string field_list( reader.read_attribute( c_attribute_fields ) );

      bool forced_field_list = false;

      if( !field_list.empty( ) && ( field_list[ 0 ] == '!' ) )
      {
         forced_field_list = true;
         field_list.erase( 0, 1 );
      }

      vector< string > fields;
      split( field_list, fields );

      map< string, size_t > field_value_offsets;

      // NOTE: Map each field (apart from the initial @key one) to its
      // offset so its value can be obtained from the list of values.
      for( size_t i = 1; i < fields.size( ); i++ )
         field_value_offsets.insert( make_pair( fields[ i ], i ) );

      mclass = get_class_id_for_id_or_name( module_id, mclass );

      size_t handle = create_object_instance( module_id, mclass, 0, false );

      try
      {
         vector< string > field_names;
         map< string, string > field_skip_values;

         if( forced_field_list )
         {
            field_names = fields;

            if( field_names.size( ) && field_names[ 0 ] == c_key_field )
               field_names.erase( field_names.begin( ) );
         }
         else
         {
            get_all_field_names( handle, "", field_names, false );

            for( size_t i = 0; i < field_names.size( ); i++ )
            {
               string sfield( resolve_field_id( module_id, mclass, field_names[ i ], field_list ) );

               if( p_skip_fields && p_skip_fields->count( mclass ) && ( *p_skip_fields )[ mclass ].count( sfield ) )
               {
                  if( !( *p_skip_fields )[ mclass ][ sfield ].empty( ) )
                     field_skip_values[ field_names[ i ] ] = ( *p_skip_fields )[ mclass ][ sfield ];

                  field_names.erase( field_names.begin( ) + i );
                  --i;
               }
            }
         }

         string output_fields( forced_field_list ? "!" : "" );
         output_fields += fields[ 0 ] + ',' + join( field_names );

         writer.write_attribute( c_attribute_fields, output_fields );

         while( reader.has_read_comment( comment ) )
            writer.write_comment( comment );

         string next_record;

         while( reader.has_read_attribute( c_attribute_record, next_record ) )
         {
            string unescaped_crs_and_lfs;

            // NOTE: Need to ensure that \\r and \\n are not being confused with \r and \n here.
            replace( next_record, "\\\\", "\1" );
            replace( next_record, "\\r", "\r", "\\n", "\n" );
            replace( next_record, "\1", "\\\\" );

            vector< string > field_values;
            split( next_record, field_values );

            if( field_values.size( ) != fields.size( ) )
               throw runtime_error( "found " + to_string( field_values.size( ) )
                  + " field values but was expecting " + to_string( fields.size( ) ) );

            if( fields.size( ) )
            {
               if( fields[ 0 ] != c_key_field )
                  throw runtime_error(
                   "unexpected missing key field processing line #" + to_string( reader.get_last_line_num( ) ) );

               string output_values( field_values[ 0 ] );

               string key_value( output_values );

               if( !key_value.empty( ) && ( key_value[ 0 ] == '!' ) )
                  key_value.erase( 0, 1 );

               if( !key_value.empty( ) && ( key_value[ 0 ] == '@' ) )
               {
                  string::size_type pos = key_value.find( '_' );

                  if( pos != string::npos )
                     key_value.erase( 0, pos + 1 );
               }

               string extra_key_value;

               if( ( key_value.size( ) > 7 )
                && ( key_value[ key_value.length( ) - 2 ] == '_' ) )
               {
                  char last_ch = key_value[ key_value.length( ) - 1 ];

                  if( ( last_ch >= 'a' ) && ( last_ch <= 'z' ) )
                  {
                     extra_key_value = key_value;
                     key_value = key_value.substr( 0, key_value.length( ) - 2 );
                  }
               }

               bool skip_record = false;

               for( size_t i = 0; i < field_names.size( ); i++ )
               {
                  string next_value( get_field_value( handle, "", field_names[ i ] ) );

                  if( field_value_offsets.count( field_names[ i ] ) )
                     next_value = field_values[ field_value_offsets[ field_names[ i ] ] ];

                  if( field_skip_values.count( field_names[ i ] ) && next_value == field_skip_values[ field_names[ i ] ] )
                  {
                     skip_record = true;
                     break;
                  }

                  output_values += ',';
                  output_values += escaped( next_value, ",\"", '\\', "rn\r\n" );
               }

               if( skip_record )
               {
                  if( p_skipped_keys )
                  {
                     p_skipped_keys->insert( key_value );

                     if( !extra_key_value.empty( ) )
                        p_skipped_keys->insert( extra_key_value );
                  }
               }
               else
               {
                  if( p_found_keys )
                  {
                     p_found_keys->insert( key_value );

                     if( !extra_key_value.empty( ) )
                        p_found_keys->insert( extra_key_value );
                  }

                  writer.write_attribute( c_attribute_record, output_values );
               }
            }

            while( reader.has_read_comment( comment ) )
               writer.write_comment( comment );
         }

         while( reader.has_read_comment( comment ) )
            writer.write_comment( comment );

         reader.finish_section( c_section_class );
         writer.finish_section( c_section_class );

         while( reader.has_read_comment( comment ) )
            writer.write_comment( comment );

         destroy_object_instance( handle );
      }
      catch( ... )
      {
         destroy_object_instance( handle );
         throw;
      }
   }

   writer.finish_sections( );
}

}

string get_meta_class_field_list( const string& mclass )
{
   string module_id( loaded_module_id( c_meta_model_name ) );

   string skip_field_info( "@" + string( c_meta_model_name )
    + "_" + string( c_meta_class_name_class ) + ".skips.lst" );

   map< string, map< string, string > > skip_fields;

   read_skip_fields( module_id, skip_field_info, skip_fields );

   size_t handle = create_object_instance( module_id, mclass, 0, false );

   string field_list( "@key" );

   vector< string > field_names;

   get_all_field_names( handle, "", field_names, false );

   map< string, string > field_skip_values;

   for( size_t i = 0; i < field_names.size( ); i++ )
   {
      string sfield( resolve_field_id( module_id, mclass, field_names[ i ], "" ) );

      if( skip_fields.count( mclass ) && skip_fields[ mclass ].count( sfield ) )
      {
         field_names.erase( field_names.begin( ) + i );
         --i;
      }
      else
      {
         if( !field_list.empty( ) )
            field_list += ',';

         field_list += field_names[ i ];
      }
   }

   return field_list;
}

void export_package( const string& module, const string& mclass,
 const string& keys, const string& exclude_info, const string& skip_field_info,
 const string& test_info, const string& include_info, const string& filename )
{
   string last_class_id;

   map< string, int > all_class_ids;
   deque< pair< string, string > > next_pass;

   map< string, set< string > > partial_export;
   map< string, set< string > > will_be_exported;
   map< string, set< string > > exported_records;
   map< string, set< string > > exported_children;

   string module_id( loaded_module_id( module ) );
   string class_id( get_class_id_for_id_or_name( module_id, mclass ) );

   map< string, map< string, string > > skip_fields;

   read_skip_fields( module_id, skip_field_info, skip_fields );

   ofstream outf( filename.c_str( ) );

   if( !outf )
      throw runtime_error( "unable to open file '" + filename + "' for output in export_package" );

   map< string, map< string, string > > tests;

   if( !test_info.empty( ) )
   {
      vector< string > test_items;

      // NOTE: The test info can alternatively be placed in an external list file for convenience.
      // Each test item can be one of the following formats: <class>:*[;label] or <class>:<field>#
      // or <class>:<field>[!]=[value][;label]. Both "class" and "field" (where applicable) can be
      // specified as either names or id's. The first format will simply prevent any record of the
      // matching class to be included in the package. The second format will handle the field (if
      // it is a foreign key) as though it is mandatory (forcing the parent record to be processed
      // before itself). The third format handles a conditional test (only if true will the record
      // be output). If the optional label is used then instead of omitting the record itself that
      // label (including the semi-colon) will be appended to the exclude child class name/id when
      // processing the child exclusions. The special case <field> being @key is permitted so that
      // specific class records can be omitted and for multiple test values use comma separators.
      if( test_info[ 0 ] != '@' )
         split( test_info, test_items );
      else
         buffer_file_lines( test_info.substr( 1 ), test_items );

      for( size_t i = 0; i < test_items.size( ); i++ )
      {
         string::size_type pos = test_items[ i ].find( ':' );

         if( pos == string::npos )
            throw runtime_error( "invalid test_info item format '" + test_items[ i ] + "'" );

         string pclass = test_items[ i ].substr( 0, pos );
         string cfield = test_items[ i ].substr( pos + 1 );

         string xinfo( "for test_info '" + test_items[ i ] + "'" );

         pclass = resolve_class_id( module_id, pclass, xinfo );

         string test_expr;

         if( cfield != "*" )
         {
            string::size_type cpos = cfield.find( "!=" );
            if( cpos == string::npos )
               cpos = cfield.find( "=" );
            if( cpos == string::npos )
               cpos = cfield.find( "#" );

            if( cpos == string::npos )
               throw runtime_error( "invalid test_info item format '" + test_items[ i ] + "'" );

            test_expr = cfield.substr( cpos );
            cfield.erase( cpos );

            if( cfield != c_key_field )
            {
               string xinfo( "for test_info '" + test_items[ i ] + "'" );

               cfield = resolve_field_id( module_id, pclass, cfield, xinfo );
            }
         }

         tests[ pclass ][ cfield ] = test_expr;
      }
   }

   map< string, set< string > > excludes;

   if( !exclude_info.empty( ) )
   {
      vector< string > exclude_items;

      // NOTE: The exclude info can alternatively be placed in an external list file for convenience.
      // Each exclude is in the format <pclass>:<cclass>[#<field_id>][;label] where both "pclass" and
      // "cclass" able to be specified as either names or id's. The optional "field_id" must be an id
      // and will target a specific child context rather than any of the matching child class. If the
      // "label" is added then only if a test item (see above) identified the record will the exclude
      // actually apply.
      if( exclude_info[ 0 ] != '@' )
         split( exclude_info, exclude_items );
      else
         buffer_file_lines( exclude_info.substr( 1 ), exclude_items );

      for( size_t i = 0; i < exclude_items.size( ); i++ )
      {
         string::size_type pos = exclude_items[ i ].find( ':' );

         if( pos == string::npos )
            throw runtime_error( "invalid exclude_info item format '" + exclude_items[ i ] + "'" );

         string pclass = exclude_items[ i ].substr( 0, pos );
         string cclass = exclude_items[ i ].substr( pos + 1 );

         string xinfo( "for exclude_info '" + exclude_items[ i ] + "'" );

         pclass = resolve_class_id( module_id, pclass, xinfo );

         string field_info;

         pos = cclass.find_first_of( "#;" );

         if( pos != string::npos )
         {
            field_info = cclass.substr( pos );
            cclass.erase( pos );
         }

         if( cclass != "*" )
         {
            cclass = resolve_class_id( module_id, cclass, xinfo );

            if( !field_info.empty( ) && field_info[ 0 ] == '#' )
            {
               pos = field_info.find( ';' );

               resolve_field_id( module_id, cclass,
                field_info.substr( 1, pos == string::npos ? pos : pos - 1 ), xinfo );
            }
         }

         excludes[ pclass ].insert( cclass + field_info );
      }
   }

   map< string, int > rounds;
   map< string, set< string > > includes;

   if( !include_info.empty( ) )
   {
      vector< string > include_items;

      // NOTE: The include info can alternatively be placed in an external list file for convenience.
      // Each include is in the format <class>:<field> or <class>#<round_num> where both "class" and
      // "field" can be specified as either names or id's. The second format allows processing of the
      // nominated class to be delayed to a later round (useful if the pre-determined ordering is not
      // what is wanted).
      if( include_info[ 0 ] != '@' )
         split( include_info, include_items );
      else
         buffer_file_lines( include_info.substr( 1 ), include_items );

      for( size_t i = 0; i < include_items.size( ); i++ )
      {
         string::size_type pos = include_items[ i ].find( '#' );

         if( pos != string::npos )
         {
            string pclass = include_items[ i ].substr( 0, pos );

            string xinfo( "for include_info '" + include_items[ i ] + "'" );

            pclass = resolve_class_id( module_id, pclass, xinfo );

            rounds[ pclass ] = atoi( include_items[ i ].substr( pos + 1 ).c_str( ) );

            continue;
         }

         pos = include_items[ i ].find( ':' );

         if( pos == string::npos )
            throw runtime_error( "invalid include_info item format '" + include_items[ i ] + "'" );

         string pclass = include_items[ i ].substr( 0, pos );
         string cfield = include_items[ i ].substr( pos + 1 );

         string xinfo( "for include_info '" + include_items[ i ] + "'" );

         pclass = resolve_class_id( module_id, pclass, xinfo );

         cfield = resolve_field_id( module_id, pclass, cfield, xinfo );

         includes[ pclass ].insert( cfield );
      }
   }

   int current_round = 0;

   map< int, deque< pair< string, string > > > future_rounds;

   string module_name( loaded_module_name( module ) );

   vector< string > classes;

   buffer_file_lines( module_name + ".acyclic.lst", classes );

   for( size_t i = 0; i < classes.size( ); i++ )
      all_class_ids.insert( make_pair( get_class_id_for_id_or_name( module_id, classes[ i ] ), i ) );

   vector< string > all_keys;
   split( keys, all_keys );

   if( all_keys.empty( ) )
      throw runtime_error( "cannot perform package export without at least one key" );

   string next_key( all_keys[ 0 ] );
   string next_class_id( class_id );

   for( size_t i = 1; i < all_keys.size( ); i++ )
      next_pass.push_back( make_pair( next_class_id, all_keys[ i ] ) );

   outf << "<sio/>\n";

   // NOTE: An export "package" contains foreign key and child related records reached
   // from the nominated starting record. In some cases there may be dependencies that
   // require a record to be output twice (the first time with one or more optional fk
   // links set to blank).
   size_t total = 0;

   time_t ts( time( 0 ) );

   while( true )
   {
      export_data( outf, module_name, next_class_id,
       next_key, last_class_id, true, get_session_command_handler( ),
       all_class_ids, skip_fields, excludes, tests, includes, exported_records, exported_children,
       next_pass, will_be_exported, partial_export, rounds, current_round, future_rounds, ts, total );

      if( next_pass.empty( ) && !future_rounds.empty( ) )
      {
         current_round = future_rounds.begin( )->first;

         next_pass = future_rounds.begin( )->second;
         future_rounds.erase( future_rounds.begin( ) );
      }

      if( next_pass.empty( ) )
         break;

      next_class_id = next_pass.front( ).first;
      next_key = next_pass.front( ).second;

      next_pass.pop_front( );
   }

   if( !exported_records.empty( ) )
      outf << " </class>\n";

   outf << "</sio>\n";

   outf.flush( );

   if( !outf.good( ) )
      throw runtime_error( "unexpected bad stream for '" + filename + "' in export_package" );
}

void import_package( const string& module,
 const string& uid, const string& dtm, const string& filename, const string& key_prefix,
 const string& replace_info, const string& skip_field_info, bool new_only, bool for_remove )
{
   string module_id( loaded_module_id( module ) );

   bool using_verbose_logging = get_storage_using_verbose_logging( );

   map< string, map< string, string > > skip_fields;

   read_skip_fields( module_id, skip_field_info, skip_fields );

   ifstream inpf( filename.c_str( ) );

   if( !inpf )
      throw runtime_error( "unable to open file '" + filename + "' for input in import_package" );

   set_uid( uid );
   set_dtm( dtm );

   sio_reader reader( inpf );

   string log_lines;
   string map_file_name;

   bool has_key_list_file = false;

   map< string, string > search_replaces_map;
   vector< pair< string, string > > search_replaces;

   if( !replace_info.empty( ) )
   {
      vector< string > replace_items;

      if( replace_info[ 0 ] != '@' )
         split( replace_info, replace_items );
      else
      {
         has_key_list_file = true;
         map_file_name = replace_info.substr( 1 ) + ".map";

         buffer_file_lines( replace_info.substr( 1 ), replace_items );
      }

      for( size_t i = 0; i < replace_items.size( ); i++ )
      {
         if( replace_items[ i ].empty( ) || ( replace_items[ i ][ 0 ] == ';' ) )
            continue;

         // NOTE: If the replace info starts with an asterisk then search/replacing
         // will be performed for the search string itself (this can be useful when
         // compound keys that contain other package keys need to be optional).
         bool do_replaces_for_find_string = false;

         if( replace_items[ i ][ 0 ] == '*' )
         {
            do_replaces_for_find_string = true;
            replace_items[ i ].erase( 0, 1 );
         }

         string::size_type pos = replace_items[ i ].find( '=' );

         if( pos == string::npos )
            throw runtime_error( "invalid replace_info item format '" + replace_items[ i ] + "'" );

         string find_string( replace_items[ i ].substr( 0, pos ) );
         string replace_with( replace_items[ i ].substr( pos + 1 ) );

         if( ( replace_with[ 0 ] == '?' ) || ( replace_with[ 0 ] == '!' ) )
         {
            // NOTE: Conditional search replacement is available in two ways:
            //
            // <search>=!<check>=[replace] will change <search> to <replace> if <check>'s replace is empty
            // <search>=?<check>=[replace] will change <search> to <replace> if <check>'s replace is non-empty
            //
            // The <check> value is expected to be the key for an already existing search/replacement pair.
            pos = replace_with.find( "=" );

            if( pos == string::npos )
               throw runtime_error( "invalid replace_info item format '" + replace_items[ i ] + "'" );

            bool check_exists = ( replace_with[ 0 ] == '?' );

            string check_for( replace_with.substr( 1, pos - 1 ) );
            replace_with.erase( 0, pos + 1 );

            if( !search_replaces_map.count( check_for ) )
               throw runtime_error( "conditional search/replace check '" + check_for + "' was not found" );

            if( check_exists && search_replaces_map[ check_for ].empty( ) )
               continue;

            if( !check_exists && !search_replaces_map[ check_for ].empty( ) )
               continue;
         }

         if( replace_with == c_key_field )
            replace_with = gen_key( );
         else
         {
            while( search_replaces_map.count( replace_with ) )
               replace_with = search_replaces_map[ replace_with ];
         }

         if( do_replaces_for_find_string )
         {
            for( size_t i = 0; i < search_replaces.size( ); i++ )
            {
               find_string = search_replace( find_string,
                search_replaces[ i ].first, search_replaces[ i ].second );
            }
         }

         bool exists = ( search_replaces_map.count( find_string ) > 0 );

         search_replaces_map[ find_string ] = replace_with;

         if( !exists )
            search_replaces.push_back( make_pair( find_string, replace_with ) );
         else
         {
            for( size_t i = 0; i < search_replaces.size( ); i++ )
            {
               if( search_replaces[ i ].first == find_string )
               {
                  search_replaces[ i ].second = replace_with;
                  break;
               }
            }
         }
      }

      if( !for_remove && has_key_list_file )
      {
         ofstream outf( map_file_name.c_str( ) );

         for( size_t i = 0; i < search_replaces.size( ); i++ )
            outf << search_replaces[ i ].first << "=" << search_replaces[ i ].second << endl;
      }
   }

   time_t ts( time( 0 ) );

   transaction_start( );

   size_t transaction_id = next_transaction_id( ) + 1;

   set< string > keys_updating;
   map< string, string > keys_created;
   map< string, set< string > > prefixed_class_keys;

   vector< string > map_appends;

   try
   {
      while( reader.has_started_section( c_section_class ) )
      {
         string mclass( reader.read_attribute( c_attribute_name ) );
         string field_list( reader.read_attribute( c_attribute_fields ) );

         bool ignore_skips = false;

         if( !field_list.empty( ) && ( field_list[ 0 ] == '!' ) )
         {
            ignore_skips = true;
            field_list.erase( 0, 1 );
         }

         mclass = get_class_id_for_id_or_name( module_id, mclass );

         size_t handle = create_object_instance( module_id, mclass, 0, false );

         try
         {
            vector< string > fields;
            split( field_list, fields );

            // NOTE: Check that field names have not been repeated (apart from "ignore").
            vector< string > sorted_fields( fields.begin( ), fields.end( ) );

            sort( sorted_fields.begin( ), sorted_fields.end( ) );

            string last_field;

            for( size_t i = 0; i < sorted_fields.size( ); i++ )
            {
               if( ( last_field != c_ignore_field ) && ( sorted_fields[ i ] == last_field ) )
                  throw runtime_error( "field name '" + last_field + "' was repeated" );

               last_field = sorted_fields[ i ];
            }

            set_any_field_names_to_ids( handle, "", fields );

            map< string, string > foreign_field_and_class_ids;

            if( !key_prefix.empty( ) )
               get_foreign_field_and_class_ids( handle, "", foreign_field_and_class_ids );

            vector< pair< string, string > > base_class_info;
            get_base_class_info( handle, "", base_class_info );

            if( base_class_info.empty( ) )
               base_class_info.push_back( make_pair( mclass, mclass ) );

            string next_record;

            while( reader.has_read_attribute( c_attribute_record, next_record ) )
            {
               if( time( 0 ) - ts >= 10 )
               {
                  ts = time( 0 );

                  // FUTURE: This message should be handled as a server string message.
                  get_session_command_handler( ).output_progress( "Processed "
                   + to_string( reader.get_last_line_num( ) ) + " lines..." );
               }

               map< string, string > search_replaces_used;

               if( !search_replaces.empty( ) )
               {
                  for( size_t i = 0; i < search_replaces.size( ); i++ )
                  {
                     string original_next_record( next_record );

                     next_record = search_replace( next_record,
                      search_replaces[ i ].first, search_replaces[ i ].second );

                     // NOTE: Remember any search replaces that were used in order to be able to append
                     // entries to the map file (if required) for @<prefix>_<key> usage entries.
                     if( next_record != original_next_record )
                        search_replaces_used[ search_replaces[ i ].second ] = search_replaces[ i ].first;
                  }
               }

               vector< string > field_values;
               split( next_record, field_values, ',', '\\', false );

               if( field_values.size( ) != fields.size( ) )
                  throw runtime_error( "found " + to_string( field_values.size( ) )
                   + " field values but was expecting " + to_string( fields.size( ) ) );

               if( fields.size( ) )
               {
                  if( fields[ 0 ] != c_key_field )
                     throw runtime_error(
                      "unexpected missing key field processing line #"
                      + to_string( reader.get_last_line_num( ) ) );

                  bool skip_op = false;

                  if( field_values[ 0 ].empty( )
                   || ( field_values[ 0 ] == "!" ) || ( field_values[ 0 ] == "~" ) )
                  {
                     skip_op = true;
                     field_values[ 0 ].erase( );
                  }
                  else if( !ignore_skips )
                  {
                     for( size_t i = 1; i < fields.size( ); i++ )
                     {
                        for( size_t j = 0; j < base_class_info.size( ); j++ )
                        {
                           string next_cid = base_class_info[ j ].first;

                           if( skip_fields.count( next_cid ) && skip_fields[ next_cid ].count( fields[ i ] ) )
                           {
                              if( !skip_fields[ next_cid ][ fields[ i ] ].empty( )
                               && field_values[ i ] == skip_fields[ next_cid ][ fields[ i ] ] )
                                 skip_op = true;
                           }
                        }
                     }
                  }

                  string next_key( field_values[ 0 ] );

                  // NOTE: Allow packages being imported with the "new_only" option to
                  // still update specific records by prefixing their keys with a '!'.
                  if( !next_key.empty( ) && ( next_key[ 0 ] == '!' ) )
                  {
                     next_key.erase( 0, 1 );
                     keys_updating.insert( next_key );
                  }

                  bool is_remove_op = false;

                  if( !next_key.empty( ) && ( next_key[ 0 ] == '~' ) )
                  {
                     next_key.erase( 0, 1 );

                     if( !for_remove )
                        skip_op = true;
                     else
                        is_remove_op = true;
                  }

                  string original_key( next_key );

                  // NOTE: Allow a key field to be specified in the following manner: @3_20101010101010101010
                  // where the text between the @ and _ (in the case "3") is used to replace the equal number
                  // of leading characters in the key (so the final key will become 30101010101010101010).
                  if( !next_key.empty( ) && ( next_key[ 0 ] == '@' ) )
                  {
                     string::size_type pos = next_key.find( '_' );

                     if( pos == string::npos )
                        throw runtime_error( "unexpected key format '" + next_key
                         + "' processing line #" + to_string( reader.get_last_line_num( ) ) );

                     original_key.erase( 0, pos + 1 );

                     string prefix_replace;
                     if( pos > 1 )
                        prefix_replace = next_key.substr( 1, pos - 1 );

                     if( prefix_replace.length( ) )
                     {
                        if( next_key.length( ) > pos * 2 )
                        {
                           next_key.erase( 0, pos + 1 + prefix_replace.length( ) );
                           next_key = prefix_replace + next_key;
                        }
                        else if( pos == next_key.length( ) - 1 )
                           skip_op = true;
                        else
                           throw runtime_error( "unexpected key prefix '" + next_key
                            + "' processing line #" + to_string( reader.get_last_line_num( ) ) );
                     }
                  }

                  if( is_remove_op && !for_remove )
                     skip_op = true;
                  else if( for_remove && !is_remove_op )
                     skip_op = true;

                  if( !skip_op )
                  {
                     string next_log_line;

                     string key_value( key_prefix + next_key );

                     instance_fetch_rc rc;

                     // NOTE: As a performance optimisation the instance fetch is skipped if the
                     // "new_only" option is being used (assumes the record was not found unless
                     // previously created or specifically flagged as an update).
                     if( new_only )
                     {
                        if( keys_created.count( key_value ) || keys_updating.count( key_value ) )
                           rc = e_instance_fetch_rc_okay;
                        else
                           rc = e_instance_fetch_rc_not_found;
                     }
                     else
                        instance_fetch( handle, "", key_value, &rc );

                     bool is_update = false;

                     if( rc != e_instance_fetch_rc_okay )
                     {
                        next_log_line = c_cmd_create;
                        op_instance_create( handle, "", key_value, false );
                     }
                     else if( new_only && !keys_created.count( key_value ) && !keys_updating.count( key_value ) )
                        // FUTURE: This message should be handled as a server string message.
                        throw runtime_error( "Package key '" + key_value + "' is already in use." );
                     else
                     {
                        is_update = true;
                        next_log_line = c_cmd_update;
                        op_instance_update( handle, "", key_value, "", false );
                     }

                     string class_id_to_log( mclass );

                     if( !using_verbose_logging && ( class_id_to_log.find( module_id ) == 0 ) )
                        class_id_to_log.erase( 0, module_id.length( ) );

                     next_log_line += " " + uid + " " + dtm + " "
                      + module_id + " " + class_id_to_log + " " + key_value + " \"";

                     string log_field_value_pairs;

                     for( size_t i = 1; i < fields.size( ); i++ )
                     {
                        bool skip_field = false;

                        if( !ignore_skips )
                        {
                           for( size_t j = 0; j < base_class_info.size( ); j++ )
                           {
                              string next_cid = base_class_info[ j ].first;

                              if( skip_fields.count( next_cid ) && skip_fields[ next_cid ].count( fields[ i ] ) )
                              {
                                 if( skip_fields[ next_cid ][ fields[ i ] ].empty( ) )
                                 {
                                    skip_field = true;
                                    break;
                                 }
                              }
                           }
                        }

                        if( skip_field || ( fields[ i ] == c_ignore_field ) )
                           continue;

                        if( foreign_field_and_class_ids.count( fields[ i ] )
                         && prefixed_class_keys[ foreign_field_and_class_ids[ fields[ i ] ] ].count( field_values[ i ] ) )
                           field_values[ i ] = key_prefix + field_values[ i ];

                        string value;

                        if( !using_verbose_logging )
                        {
                           string method_name_and_args( "get " );
                           method_name_and_args += fields[ i ];

                           value = execute_object_command( handle, "", method_name_and_args );
                        }

                        if( using_verbose_logging || value != field_values[ i ] )
                        {
                           string method_name_and_args( "set " );

                           method_name_and_args += fields[ i ] + " ";

                           method_name_and_args += "\"" + escaped( unescaped( field_values[ i ], "rn\r\n" ), "\"" ) + "\"";

                           if( !log_field_value_pairs.empty( ) )
                              log_field_value_pairs += ",";

                           string field_id_to_log( fields[ i ] );

                           if( !using_verbose_logging && field_id_to_log.find( module_id ) == 0 )
                           {
                              field_id_to_log.erase( 0, module_id.length( ) );

                              if( field_id_to_log.find( class_id_to_log ) == 0 )
                                 field_id_to_log.erase( 0, class_id_to_log.length( ) );
                           }

                           log_field_value_pairs += field_id_to_log
                            + "=" + search_replace( field_values[ i ], "\\\\", "\\\\\\\\", ",", "\\\\," );

                           execute_object_command( handle, "", method_name_and_args );
                        }
                     }

                     next_log_line += log_field_value_pairs + "\"";

                     if( !log_lines.empty( ) )
                        log_lines += "\n";

                     log_lines += next_log_line;

                     op_instance_apply( handle, "", false );

                     if( !is_update && !for_remove )
                     {
                        keys_created.insert( make_pair( key_value, mclass ) );

                        if( ( original_key != key_value ) && search_replaces_used.count( original_key ) )
                           map_appends.push_back( search_replaces_used[ original_key ] + "=" + key_value );
                     }
                  }

                  if( !for_remove && !key_prefix.empty( ) )
                  {
                     prefixed_class_keys[ mclass ].insert( next_key );

                     vector< pair< string, string > > base_class_info;

                     get_base_class_info( handle, "", base_class_info );

                     for( size_t i = 0; i < base_class_info.size( ); i++ )
                        prefixed_class_keys[ base_class_info[ i ].first ].insert( next_key );
                  }
               }
            }

            reader.finish_section( c_section_class );

            destroy_object_instance( handle );
         }
         catch( ... )
         {
            destroy_object_instance( handle );
            throw;
         }
      }

      reader.verify_finished_sections( );
   }
   catch( exception& x )
   {
      transaction_rollback( );

      string s( x.what( ) );

      // FUTURE: This message should be handled as a server string message.
      s += " This occurred while processing line #"
       + to_string( reader.get_last_line_num( ) ) + " of '" + filename + "'.";

      throw runtime_error( s );
   }
   catch( ... )
   {
      transaction_rollback( );
      throw;
   }

   transaction_log_command( log_lines );
   transaction_commit( );

   if( has_key_list_file )
   {
      if( !map_appends.empty( ) )
      {
         ofstream outf( map_file_name.c_str( ), ios::out | ios::app );

         for( size_t i = 0; i < map_appends.size( ); i++ )
            outf << map_appends[ i ] << endl;
      }

      string new_file_name( replace_info.substr( 1 ) + ".new" );
      ofstream outf( new_file_name.c_str( ) );

      for( map< string, string >::iterator i = keys_created.begin( ); i != keys_created.end( ); ++i )
         outf << i->second << ':' << i->first << '\n';
   }
}

void update_package( const string& name )
{
   string module_id( loaded_module_id( c_meta_model_name ) );

   set< string > found_keys;
   set< string > missing_keys;
   set< string > skipped_keys;
   set< string > repeated_keys;

   string package_sio_file( name + ".package.sio" );

   if( !file_exists( package_sio_file ) )
      throw runtime_error( "package file '" + package_sio_file + "' not found in update_package" );
   else
   {
      string specs_sio_file( name + ".specs.sio" );

      if( file_exists( specs_sio_file ) )
         create_new_package_file( module_id, specs_sio_file );

      string skip_field_info( "@" + string( c_meta_model_name )
       + "_" + string( c_meta_class_name_class ) + ".skips.lst" );

      map< string, map< string, string > > skip_fields;
      read_skip_fields( module_id, skip_field_info, skip_fields );

      create_new_package_file( module_id, package_sio_file, &found_keys, &skipped_keys, &skip_fields );

      string package_keys_file( name + ".keys.lst" );

      if( !file_exists( package_keys_file ) )
         throw runtime_error( "package keys file '" + package_keys_file + "' not found in update_package" );
      else
      {
         vector< string > lines;
         buffer_file_lines( package_keys_file, lines );

         string new_package_keys_file( package_keys_file + ".new" );
   
         ofstream outf( new_package_keys_file.c_str( ) );

         if( !outf )
            throw runtime_error( "unable to open file '" + new_package_keys_file + "' for output in update_package" );

         set< string > used_keys;

         for( size_t i = 0; i < lines.size( ); i++ )
         {
            string next_line( lines[ i ] );

            bool skip_line = false;

            if( !next_line.empty( ) )
            {
               if( next_line[ 0 ] != ';' )
               {
                  string::size_type pos = next_line.find( '=' );
                  if( pos != string::npos )
                  {
                     string key( next_line.substr( 0, pos ) );
                     string rhs( next_line.substr( pos + 1 ) );

                     if( skipped_keys.count( key ) )
                        skip_line = true;
                     else if( !found_keys.count( key ) )
                        missing_keys.insert( key );
                     else
                     {
                        if( !rhs.empty( ) && rhs[ 0 ] != '?' && rhs[ 0 ] != '!' )
                        {
                           if( used_keys.count( key ) )
                              repeated_keys.insert( key );
                           else
                              used_keys.insert( key );
                        }
                     }
                  }
               }
            }

            if( !skip_line )
               outf << next_line << '\n';
         }
      }
   }

   if( !found_keys.empty( ) )
   {
      string report_file( name + ".report.txt" );

      ofstream outf( report_file.c_str( ) );
      if( !outf )
         throw runtime_error( "unable to open file '" + report_file + "' for output in update_package" );

      outf << "Total Keys: " << found_keys.size( ) << endl;

      if( !missing_keys.empty( ) )
      {
         bool first = true;

         for( set< string >::iterator i = missing_keys.begin( ); i != missing_keys.end( ); ++i )
         {
            string key_value( *i );

            string::size_type pos = key_value.find( "ext_" );

            if( pos != 0 )
               pos = key_value.find( "loc_" );

            if( pos != 0 )
               pos = key_value.find( "opt_" );

            if( pos != 0 )
               pos = key_value.find( "var_" );

            if( pos != 0 )
            {
               if( first )
               {
                  outf << "\nMissing Keys:" << endl;
                  first = false;
               }

               outf << key_value << endl;
            }
         }
      }

      if( !skipped_keys.empty( ) )
      {
         outf << "\nSkipped Keys:" << endl;

         for( set< string >::iterator i = skipped_keys.begin( ); i != skipped_keys.end( ); ++i )
            outf << *i << endl;
      }

      if( !repeated_keys.empty( ) )
      {
         outf << "\nRepeated Keys:" << endl;

         for( set< string >::iterator i = repeated_keys.begin( ); i != repeated_keys.end( ); ++i )
            outf << *i << endl;
      }
   }
}
