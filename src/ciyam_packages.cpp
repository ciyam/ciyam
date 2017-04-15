// Copyright (c) 2017 CIYAM Developers
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
 const map< string, int >& all_class_ids, const map< string, set< string > >& excludes,
 const map< string, map< string, string > >& tests, const map< string, set< string > >& includes,
 map< string, set< string > >& exported_records, map< string, set< string > >& exported_children,
 deque< pair< string, string > >& next_pass, map< string, set< string > >& will_be_exported,
 map< string, set< string > >& partial_export, const map< string, int >& rounds,
 int current_round, map< int, deque< pair< string, string > > >& future_rounds, time_t& ts, size_t& total )
{
   size_t handle = create_object_instance( module, class_id );
   class_base& cb( get_class_base_from_handle_for_op( handle, "" ) );

   string class_name;
   vector< pair< string, string > > base_class_info;

   vector< field_data > all_field_data;
   get_all_field_data( handle, "", key, all_field_data, &class_id, &class_name, &base_class_info );

   if( !all_class_ids.count( class_id ) )
      throw runtime_error( "unable to find '" + class_id + "' in all_class_ids for export" );

   string exclude_suffix;
   bool skip_record = false;

   set< string > first_output_ids;

   bool has_seen = false;
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
      bool need_to_repeat = false;
      int position = all_class_ids.find( class_id )->second;

      string all_values( key );
      for( size_t i = 0; i < all_field_data.size( ); i++ )
      {
         if( all_field_data[ i ].transient )
            continue;

         string next_value( all_field_data[ i ].value );

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
               export_data( outs, module, all_field_data[ i ].class_id,
                all_field_data[ i ].value, last_class_id, output_fk_children, handler,
                all_class_ids, excludes, tests, includes, exported_records, exported_children,
                next_pass, will_be_exported, partial_export, rounds, current_round, future_rounds, ts, total );
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

                        export_data( outs, module, p_class_base->get_class_id( ),
                         p_class_base->get_key( ), last_class_id, true, handler, all_class_ids,
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

   destroy_object_instance( handle );
}

}

void export_package( const string& module,
 const string& mclass, const string& keys, const string& exclude_info,
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

   bool is_first = true;

   // NOTE: An export "package" contains foreign key and child related records reached
   // from the nominated starting record. In some cases there may be dependencies that
   // require a record to be output twice (the first time with one or more optional fk
   // links set to blank).
   size_t total = 0;
   time_t ts( time( 0 ) );
   while( true )
   {
      export_data( outf, module_name, next_class_id,
       next_key, last_class_id, is_first, get_session_command_handler( ),
       all_class_ids, excludes, tests, includes, exported_records, exported_children,
       next_pass, will_be_exported, partial_export, rounds, current_round, future_rounds, ts, total );

      if( current_round == 0 )
         is_first = false;

      if( next_pass.empty( ) && !future_rounds.empty( ) )
      {
         current_round = future_rounds.begin( )->first;
         next_pass = future_rounds.begin( )->second;

         is_first = true;
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
   bool is_using_blockchain = get_session_is_using_blockchain( );

   map< string, map< string, string > > skip_fields;

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
            svalue = sfield.substr( pos );
            sfield.erase( pos );
         }

         sfield = resolve_field_id( module_id, sclass, sfield, xinfo );

         skip_fields[ sclass ][ sfield ] = svalue;
      }
   }

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
         if( replace_items[ i ].empty( ) || replace_items[ i ][ 0 ] == ';' )
            continue;

         // NOTE: If the replace info starts with an asterisk then search/replacing
         // will be performed seach string itself (this can be useful when compound
         // keys that contain other package keys need to be optional).
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

         if( replace_with[ 0 ] == '?' || replace_with[ 0 ] == '!' )
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
            replace_with = gen_key( "", !is_using_blockchain );
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
      size_t line_num = 1;
      while( reader.has_started_section( c_section_class ) )
      {
         ++line_num;

         string mclass( reader.read_attribute( c_attribute_name ) );
         string field_list( reader.read_attribute( c_attribute_fields ) );

         line_num += 2;

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
               if( last_field != c_ignore_field && sorted_fields[ i ] == last_field )
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
               ++line_num;

               if( time( 0 ) - ts >= 10 )
               {
                  ts = time( 0 );
                  // FUTURE: This message should be handled as a server string message.
                  get_session_command_handler( ).output_progress( "Processed " + to_string( line_num ) + " lines..." );
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
                     throw runtime_error( "unexpected missing key field processing line #" + to_string( line_num ) );

                  bool skip_op = false;

                  if( field_values[ 0 ].empty( ) || field_values[ 0 ] == "!" )
                     skip_op = true;
                  else
                  {
                     for( size_t i = 1; i < fields.size( ); i++ )
                     {
                        for( size_t j = 0; j < base_class_info.size( ); j++ )
                        {
                           string next_cid = base_class_info[ j ].first;

                           if( skip_fields.count( next_cid ) && skip_fields[ next_cid ].count( fields[ i ] ) )
                           {
                              if( !skip_fields[ next_cid ][ fields[ i ] ].empty( )
                               && field_values[ i ] == skip_fields[ next_cid ][ fields[ i ] ].substr( 1 ) )
                                 skip_op = true;
                           }
                        }
                     }
                  }

                  string next_key( field_values[ 0 ] );

                  // NOTE: Allow packages being imported with the "new_only" option to
                  // still update specific records by prefixing their keys with a '!'.
                  if( !next_key.empty( ) && next_key[ 0 ] == '!' )
                  {
                     next_key.erase( 0, 1 );
                     keys_updating.insert( next_key );
                  }

                  bool is_remove_op = false;
                  if( !next_key.empty( ) && next_key[ 0 ] == '~' )
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
                  if( !next_key.empty( ) && next_key[ 0 ] == '@' )
                  {
                     string::size_type pos = next_key.find( '_' );
                     if( pos == string::npos )
                        throw runtime_error( "unexpected key format '" + next_key + "' processing line #" + to_string( line_num ) );

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
                           throw runtime_error( "unexpected key prefix '" + next_key + "' processing line #" + to_string( line_num ) );
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
                        next_log_line = "pc";
                        op_instance_create( handle, "", key_value, false );
                     }
                     else if( new_only && !keys_created.count( key_value ) && !keys_updating.count( key_value ) )
                        // FUTURE: This message should be handled as a server string message.
                        throw runtime_error( "Package key '" + key_value + "' is already in use." );
                     else
                     {
                        is_update = true;
                        next_log_line = "pu";
                        op_instance_update( handle, "", key_value, "", false );
                     }

                     string class_id_to_log( mclass );

                     if( is_using_blockchain && class_id_to_log.find( module_id ) == 0 )
                        class_id_to_log.erase( 0, module_id.length( ) );

                     next_log_line += " " + uid + " " + dtm + " "
                      + module_id + " " + class_id_to_log + " " + key_value + " \"";

                     string log_field_value_pairs;

                     for( size_t i = 1; i < fields.size( ); i++ )
                     {
                        bool skip_field = false;
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

                        if( skip_field || fields[ i ] == c_ignore_field )
                           continue;

                        if( foreign_field_and_class_ids.count( fields[ i ] )
                         && prefixed_class_keys[ foreign_field_and_class_ids[ fields[ i ] ] ].count( field_values[ i ] ) )
                           field_values[ i ] = key_prefix + field_values[ i ];

                        string value;

                        if( is_using_blockchain )
                        {
                           string method_name_and_args( "get " );
                           method_name_and_args += fields[ i ];

                           value = execute_object_command( handle, "", method_name_and_args );
                        }

                        if( !is_using_blockchain || value != field_values[ i ] )
                        {
                           string method_name_and_args( "set " );
                           method_name_and_args += fields[ i ] + " ";
                           method_name_and_args += "\"" + escaped( unescaped( field_values[ i ], "rn\r\n" ), "\"" ) + "\"";

                           if( !log_field_value_pairs.empty( ) )
                              log_field_value_pairs += ",";

                           string field_id_to_log( fields[ i ] );

                           if( is_using_blockchain && field_id_to_log.find( module_id ) == 0 )
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

                        if( original_key != key_value && search_replaces_used.count( original_key ) )
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
            ++line_num;
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

