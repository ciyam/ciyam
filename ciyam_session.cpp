// Copyright (c) 2006
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

#ifdef __BORLANDC__
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <ctime>
#  include <csignal>
#  include <cstdlib>
#  include <set>
#  include <map>
#  include <deque>
#  include <vector>
#  include <string>
#  include <sstream>
#  include <fstream>
#  include <iomanip>
#  include <iostream>
#  include <algorithm>
#  include <stdexcept>
#endif

#include "ciyam_session.h"

#include "sio.h"
#include "format.h"
#include "pdf_gen.h"
#include "threads.h"
#include "utilities.h"
#include "date_time.h"
#include "ciyam_base.h"
#include "class_base.h"
#include "auto_script.h"
#include "ciyam_strings.h"
#include "command_parser.h"
#include "command_handler.h"
#include "module_interface.h"
#include "command_processor.h"
#include "module_management.h"

//#define DEBUG

using namespace std;

extern size_t g_active_sessions;
extern volatile sig_atomic_t g_server_shutdown;

const size_t c_request_timeout = 500;

namespace
{

mutex g_mutex;

#include "ciyam_session.cmh"

const int c_pdf_default_limit = 5000;

const char* const c_protocol_version = "0.1";

const char* const c_response_okay = "(okay)";
const char* const c_response_okay_more = "(okay more)";
const char* const c_response_not_found = "(not found)";

const char* const c_response_error_prefix = "(error) ";
const char* const c_response_message_prefix = "(message) ";

const char* const c_log_transformation_scope_any_change = "any_change";
const char* const c_log_transformation_scope_execute_only = "execute_only";
const char* const c_log_transformation_scope_any_perform_op = "any_perform_op";
const char* const c_log_transformation_scope_create_update_only = "create_update_only";
const char* const c_log_transformation_scope_update_execute_only = "update_execute_only";
const char* const c_log_transformation_scope_create_update_destroy = "create_update_destroy";

const char* const c_log_transformation_op_map_module = "map_module";
const char* const c_log_transformation_op_ignore_field = "ignore_field";
const char* const c_log_transformation_op_map_class_id = "map_class_id";
const char* const c_log_transformation_op_map_field_id = "map_field_id";
const char* const c_log_transformation_op_map_method_id = "map_method_id";
const char* const c_log_transformation_op_no_args_append = "no_args_append";
const char* const c_log_transformation_op_skip_operation = "skip_operation";
const char* const c_log_transformation_op_file_path_remove = "file_path_remove";
const char* const c_log_transformation_op_change_field_value = "change_field_value";
const char* const c_log_transformation_op_map_first_arg_field_ids = "map_first_arg_field_ids";
const char* const c_log_transformation_op_instance_change_field_value = "instance_change_field_value";

inline void issue_error( const string& message )
{
#ifdef DEBUG
   cerr << "session error: " << message << endl;
#else
   TRACE_LOG( TRACE_ANYTHING, string( "session error: " ) + message );
#endif
}

inline void issue_warning( const string& message )
{
#ifdef DEBUG
   cout << "session warning: " << message << endl;
#else
   TRACE_LOG( TRACE_ANYTHING, string( "session warning: " ) + message );
#endif
}

inline void set_dtm_if_now( string& dtm, string& next_command )
{
   if( dtm == c_dtm_now )
   {
      dtm = date_time::standard( ).as_string( );

      string::size_type pos = next_command.find( c_dtm_now );
      if( pos == string::npos )
         throw runtime_error( "unexpected @now not found in next_command '" + next_command + "'" );

      next_command.erase( pos, strlen( c_dtm_now ) );
      next_command.insert( pos, dtm );
   }
}

inline string convert_local_to_utc( const string& local, const string& tz_abbr )
{
   string s;

   if( !local.empty( ) )
   {
      // NOTE: If a date_time string starts with '#' then it is considered as already being UTC.
      if( local[ 0 ] == '#' )
         s = local.substr( 1 );
      else
         s = local_to_utc( date_time( local ), tz_abbr ).as_string( );
   }

   return s;
}

string resolve_module_id( const string& id_or_name, const map< string, string >* p_transformations = 0 )
{
   string module_id( id_or_name );

   if( p_transformations && !p_transformations->empty( ) )
   {
      string ltf_key( c_log_transformation_scope_any_change );
      ltf_key += " " + id_or_name + " " + string( c_log_transformation_op_map_module );

      if( p_transformations->count( ltf_key ) )
         module_id = p_transformations->find( ltf_key )->second;
   }

   module_id = get_module_id_for_id_or_name( module_id );

   return module_id;
}

string resolve_class_id( const string& module, const string& id_or_name,
 const map< string, string >* p_transformations = 0, const string* p_exception_info = 0 )
{
   string class_id( id_or_name );

   if( p_transformations && !p_transformations->empty( ) )
   {
      string ltf_key( c_log_transformation_scope_any_change );
      ltf_key += " " + module + " " + id_or_name + " " + string( c_log_transformation_op_map_class_id );

      if( p_transformations->count( ltf_key ) )
         class_id = p_transformations->find( ltf_key )->second;
   }

   class_id = get_class_id_for_id_or_name( module, class_id );

   if( p_exception_info && class_id == get_class_name_for_id_or_name( module, class_id ) )
      throw runtime_error( "unknown class '" + class_id + "' " + *p_exception_info );

   return class_id;
}

string resolve_field_id(
 const string& module, const string& mclass, const string& id_or_name,
 const map< string, string >* p_transformations = 0, const string* p_exception_info = 0 )
{
   string field_id( id_or_name );

   if( p_transformations && !p_transformations->empty( ) )
   {
      string ltf_key( c_log_transformation_scope_any_change );
      ltf_key += " " + module
       + " " + mclass + " " + string( c_log_transformation_op_map_field_id ) + " " + id_or_name;

      if( p_transformations->count( ltf_key ) )
         field_id = p_transformations->find( ltf_key )->second;
   }

   field_id = get_field_id_for_id_or_name( module, mclass, field_id );

   if( p_exception_info && field_id == get_field_name_for_id_or_name( module, mclass, field_id ) )
      throw runtime_error( "unknown field '" + field_id + "' " + *p_exception_info );

   return field_id;
}

string resolve_method_name( const string& module, const string& mclass,
 const string& id_or_name, const map< string, string >* p_transformations = 0, string* p_method_id = 0 )
{
   string method_name( id_or_name );

   if( p_transformations && !p_transformations->empty( ) )
   {
      string ltf_key( c_log_transformation_scope_update_execute_only );
      ltf_key += " " + module + " " + mclass + " " + string( c_log_transformation_op_map_method_id ) + " " + id_or_name;

      if( p_transformations->count( ltf_key ) )
         method_name = p_transformations->find( ltf_key )->second;
   }

   // NOTE: If a rebuild is in progress then this will allow the caller to extract a method id.
   if( p_method_id )
      *p_method_id = method_name;

   const procedure_info_container& procedure_info( get_procedure_info_for_module_class( module, mclass ) );
   if( procedure_info.count( method_name ) )
      method_name = procedure_info.find( method_name )->second;

   return method_name;
}

struct summary_info
{
   int idx;
   int num;
   int child;
   string field;
   string value;
};

void add_pdf_variables( size_t handle,
 const string& parent_context, const vector< string >& field_list,
 vector< summary_info >& summaries, map< string, string >& pdf_variables,
 const string& tz_abbr, bool is_single_record, size_t row_num, bool add_values = true )
{
   map< string, int > all_contexts;
   for( size_t i = 0; i < field_list.size( ); i++ )
   {
      string next_field( field_list[ i ] );

      string::size_type pos = next_field.find( "." );
      if( pos != string::npos )
         next_field.erase( pos );

      if( all_contexts.count( next_field ) )
         ++all_contexts[ next_field ];
      else
         all_contexts.insert( make_pair( next_field, 1 ) );
   }

   if( is_single_record && !summaries.empty( ) )
      throw runtime_error( "cannot specify summary fields when fetching a single record" );

   map< string, int > all_summaries;
   for( size_t i = 0; i < summaries.size( ); i++ )
      all_summaries.insert( make_pair( summaries[ i ].field, i ) );

   set< string > contexts;

   vector< pair< string, string > > field_value_pairs;

   for( size_t i = 0; i < field_list.size( ); i++ )
   {
      string next_field( field_list[ i ] );
      size_t pos = next_field.find_last_of( "." );

      string field, field_context;
      if( pos != string::npos )
      {
         field = next_field.substr( pos + 1 );
         field_context = next_field.substr( 0, pos );
      }
      else
         field = next_field;

      string name = get_field_name_for_id( handle, parent_context, field, true );
      if( name.empty( ) )
      {
         size_t fpos = next_field.find( "." );

         name = get_field_name_for_id( handle, parent_context, next_field.substr( 0, fpos ), true );

         if( name.empty( ) )
            name = next_field.substr( 0, fpos );
      }

      string context( parent_context );
      if( !field_context.empty( ) )
      {
         if( !context.empty( ) )
            context += ".";
         context += field_context;
      }

      if( pos != string::npos )
      {
         string child_name = get_field_name_for_id( handle, context, field, true );
         if( child_name.empty( ) )
            child_name = next_field.substr( pos + 1 );

         name += "_" + child_name;
      }

      if( add_values )
      {
         string value = execute_object_command( handle, context, "get " + field );

         if( !value.empty( ) && !tz_abbr.empty( ) )
         {
            string type_name = get_field_type_name( handle, context, field );
            if( type_name == "date_time" || type_name == "tdatetime" )
               value = utc_to_local( date_time( value ), tz_abbr ).as_string( );
         }

         if( all_summaries.count( next_field ) )
         {
            int j = all_summaries[ next_field ];
            if( value != summaries[ j ].value )
            {
               summaries[ j ].child = 0;
               summaries[ j ].value = value;

               for( int k = j + 1; k < summaries.size( ); k++ )
                  summaries[ k ].value = string( "\1" );

               string key( "!" );
               for( int k = 0; k <= j; k++ )
                  key += summaries[ k ].value + "\1";

               string sname( "summary" );
               if( all_summaries.size( ) > 1 )
                  sname += to_string( j + 1 );

               // FUTURE: This variable should contain the record's state value.
               pdf_variables.insert( make_pair( key + "_" + sname, "" ) );

               pdf_variables.insert( make_pair( key + "_" + sname + "_" + name, value ) );
            }
         }

         if( !summaries.empty( ) )
            field_value_pairs.push_back( make_pair( name, value ) );
         else
         {
            string vname( name );

            if( !is_single_record )
            {
               ostringstream osstr;
               osstr << ifmt( 5 ) << row_num;

               // FUTURE: This variable should contain the record's state value.
               pdf_variables.insert( make_pair( osstr.str( ) + "_data", "" ) );

               vname = osstr.str( ) + "_data_" + name;
            }

            pdf_variables.insert( make_pair( vname, value ) );
         }
      }

      if( row_num == 0 )
      {
         name = "@" + name;

         string value = get_field_display_name( handle,
          parent_context, field_context.empty( ) ? field : field_context );

         // NOTE: As currently FK's are repeated (without the child in order to ensure
         // that the parent field value is retrieved) it can only be assumed that more
         // than one child for the same parent is being used if the count is > 2.
         if( !field_context.empty( ) && all_contexts[ field_context ] > 2 )
            value += " " + get_field_display_name( handle, context, field );

         pdf_variables.insert( make_pair( name, value ) );

         if( !contexts.count( context ) )
         {
            vector< pair< string, string > > enum_pairs;
            get_all_enum_pairs( handle, context, enum_pairs );

            for( size_t i = 0; i < enum_pairs.size( ); i++ )
               pdf_variables.insert( make_pair( "@" + enum_pairs[ i ].first, enum_pairs[ i ].second ) );
         }
      }

      contexts.insert( context );
   }

   if( !field_value_pairs.empty( ) )
   {
      for( size_t i = 0; i < field_value_pairs.size( ); i++ )
      {
         string key( "!" );
         for( size_t j = 0; j < summaries.size( ); j++ )
            key += summaries[ j ].value + "\1";

         ostringstream osstr;
         osstr << ifmt( 5 ) << summaries[ summaries.size( ) - 1 ].child;

         key += "_" + osstr.str( );

         // FUTURE: This variable should contain the record's state value.
         pdf_variables.insert( make_pair( key + "_data", "" ) );

         pdf_variables.insert( make_pair( key + "_data_" + field_value_pairs[ i ].first, field_value_pairs[ i ].second ) );
      }

      ++summaries[ summaries.size( ) - 1 ].child;
   }
}

void add_final_pdf_variables( const map< string, string >& variables,
 vector< summary_info >& summaries, map< string, string >& final_variables )
{
   for( size_t j = 0; j < summaries.size( ); j++ )
      summaries[ j ].value = string( "\1" );

   for( map< string, string >::const_iterator i = variables.begin( ); i != variables.end( ); ++i )
   {
      string next( i->first );
      if( !next.empty( ) )
      {
         if( next[ 0 ] == '!' )
         {
            next.erase( 0, 1 );

            vector< string > parts;
            split( next, parts, '\1', '\0', false );

            if( parts.size( ) < 1 || parts.size( ) > summaries.size( ) + 1 )
               throw runtime_error( "unexpected pdf variable key format '" + next + "'" );

            bool has_changed = false;
            for( size_t j = 0; j < min( parts.size( ) - 1, summaries.size( ) ); j++ )
            {
               if( has_changed || summaries[ j ].value != parts[ j ] )
               {
                  if( has_changed )
                     summaries[ j ].num = -1;

                  has_changed = true;
                  summaries[ j ].value = parts[ j ];

                  ++summaries[ j ].num;
               }
            }

            for( size_t j = parts.size( ) - 1; j < summaries.size( ); j++ )
               summaries[ j ].num = -1;

            string key;
            for( size_t j = 0; j < parts.size( ) - 1; j++ )
            {
               ostringstream osstr;
               osstr << setw( 5 ) << setfill( '0' ) << summaries[ j ].num;

               key += osstr.str( ) + "_";
            }

            key += parts[ parts.size( ) - 1 ].substr( 1 );

            final_variables.insert( make_pair( key, i->second ) );
         }
         else
            final_variables.insert( make_pair( i->first, i->second ) );
      }
   }
}

void parse_field_values( const string& module,
 const string& class_id, const string& field_values,
 map< string, string >& field_value_items, const map< string, string >* p_transformations = 0 )
{
   vector< string > field_value_pairs;
   raw_split( field_values, field_value_pairs );

   field_value_items.clear( );

   string::size_type pos;
   for( size_t i = 0; i < field_value_pairs.size( ); i++ )
   {
      pos = field_value_pairs[ i ].find( '=' );
      if( pos == string::npos )
         throw runtime_error( "unexpected field=value pair format '" + field_value_pairs[ i ] + "'" );

      string field_id_or_name( field_value_pairs[ i ].substr( 0, pos ) );

      if( p_transformations && !p_transformations->empty( ) )
      {
         string ltf_key( c_log_transformation_scope_any_change );
         ltf_key += " " + module + " " + class_id + " "
          + string( c_log_transformation_op_map_field_id ) + " " + field_id_or_name;

         if( p_transformations->count( ltf_key ) )
            field_id_or_name = p_transformations->find( ltf_key )->second;
      }

      TRACE_LOG( TRACE_FLD_VALS, "field name (start) '" + field_value_pairs[ i ].substr( 0, pos ) + "'" );
      TRACE_LOG( TRACE_FLD_VALS, "field name (trans) '" + field_id_or_name + "'" );
      TRACE_LOG( TRACE_FLD_VALS, "field value (data) '" + field_value_pairs[ i ].substr( pos + 1 ) + "'" );

      field_value_items[ field_id_or_name ] = field_value_pairs[ i ].substr( pos + 1 );
   }

   if( field_value_items.size( ) != field_value_pairs.size( ) )
      throw runtime_error( "invalid repeated field names in '" + field_values + "'" );
}

void perform_field_value_transformations(
 const map< string, string >& transformations,
 const string& ltf_prefix, map< string, string >& field_value_items )
{
   map< string, string >::iterator mi;
   for( mi = field_value_items.begin( ); mi != field_value_items.end( ); ++mi )
   {
      string ltf_key( ltf_prefix );
      ltf_key += " " + mi->first + " " + mi->second;

      if( transformations.count( ltf_key ) )
         mi->second = transformations.find( ltf_key )->second;
   }
}

void output_response_lines( tcp_socket& socket, const string& response )
{
   vector< string > lines;
   split( response, lines, '\n' );
   for( size_t i = 0; i < lines.size( ); i++ )
      socket.write_line( lines[ i ] );
}

struct query_data
{
   query_data( ) : is_indexed( false ) { }

   string field;
   bool is_indexed;
   string min_value;
   string max_value;
   vector< string > or_values;
};

void expand_key_info( bool is_reverse,
 vector< query_data >& all_query_data, string key_template, vector< string >& all_key_info, size_t part = 0 )
{
   if( part < all_query_data.size( ) && all_query_data[ part ].is_indexed )
   {
      if( all_query_data[ part ].or_values.empty( ) )
      {
         string marker( "@" );
         marker += ( '0' + part );
         string::size_type pos = key_template.find( marker );
         if( pos == string::npos )
            throw runtime_error( "unexpected missing marker '" + marker + "' in query key_template '" + key_template + "'" );

         key_template.replace( pos, marker.size( ), is_reverse ? all_query_data[ part ].max_value : all_query_data[ part ].min_value );

         expand_key_info( is_reverse, all_query_data, key_template, all_key_info, part + 1 );
      }
      else
      {
         for( size_t i = 0; i < all_query_data[ part ].or_values.size( ); i++ )
         {
            string marker( "@" );
            marker += ( '0' + part );

            string key_template_copy( key_template );
            string::size_type pos = key_template_copy.find( marker );
            if( pos == string::npos )
               throw runtime_error( "unexpected missing marker '" + marker + "' in query key_template '" + key_template_copy + "'" );

            key_template_copy.replace( pos, marker.size( ), all_query_data[ part ].or_values[ i ] );

            expand_key_info( is_reverse, all_query_data, key_template_copy, all_key_info, part + 1 );
         }
      }
   }
   else
      all_key_info.push_back( key_template );
}

void read_log_transformation_info( const string& file_name, map< string, string >& transformations )
{
   if( file_exists( file_name ) )
   {
      ifstream inpf( file_name.c_str( ) );

      string next_line;
      while( getline( inpf, next_line ) )
      {
         // NOTE: In case the file had been treated as binary during an FTP remove trailing CR.
         if( next_line.size( ) && next_line[ next_line.size( ) - 1 ] == '\r' )
            next_line.erase( next_line.size( ) - 1 );

         if( next_line.empty( ) )
            continue;

         if( next_line[ 0 ] == ';' ) // ignore comments...
            continue;

         size_t pos = next_line.find( ' ' );
         if( pos == string::npos )
            throw runtime_error( "unexpected log transformation info '" + next_line + "'" );

         string trans_scope( next_line.substr( 0, pos ) );
         next_line.erase( 0, pos + 1 );

         if( trans_scope == c_log_transformation_scope_execute_only )
         {
            string key( trans_scope );
            pos = next_line.find( ' ' );
            if( pos == string::npos )
               throw runtime_error( "unexpected execute info '" + next_line + "'" );

            string module( next_line.substr( 0, pos ) );
            next_line.erase( 0, pos + 1 );

            pos = next_line.find( ' ' );
            if( pos == string::npos )
               throw runtime_error( "unexpected execute info '" + next_line + "'" );

            string class_id( next_line.substr( 0, pos ) );
            next_line.erase( 0, pos + 1 );

            pos = next_line.find( ' ' );
            if( pos == string::npos )
               throw runtime_error( "unexpected execute info '" + next_line + "'" );

            string procedure( next_line.substr( 0, pos ) );
            next_line.erase( 0, pos + 1 );

            pos = next_line.find( ' ' );
            string operation( next_line.substr( 0, pos ) );

            if( pos == string::npos )
               next_line.erase( );
            else
               next_line.erase( 0, pos + 1 );

            if( pos == string::npos
             && operation != c_log_transformation_op_map_first_arg_field_ids )
               throw runtime_error( "unexpected execute info '" + next_line + "'" );

            if( operation != c_log_transformation_op_no_args_append
             && operation != c_log_transformation_op_map_first_arg_field_ids )
               throw runtime_error( "unknown execute transformation operation '" + operation + "'" );

            key += " " + module + " " + class_id + " " + procedure + " " + operation;
            transformations.insert( make_pair( key, next_line ) );
         }
         else if( trans_scope == c_log_transformation_scope_any_change
          || trans_scope == c_log_transformation_scope_any_perform_op
          || trans_scope == c_log_transformation_scope_create_update_only
          || trans_scope == c_log_transformation_scope_update_execute_only
          || trans_scope == c_log_transformation_scope_create_update_destroy )
         {
            string key( trans_scope );
            pos = next_line.find( ' ' );
            if( pos == string::npos )
               throw runtime_error( "unexpected transformation info '" + next_line + "'" );

            string module( next_line.substr( 0, pos ) );
            next_line.erase( 0, pos + 1 );

            pos = next_line.find( ' ' );
            if( pos == string::npos )
               throw runtime_error( "unexpected transformation info '" + next_line + "'" );

            string class_id( next_line.substr( 0, pos ) );
            next_line.erase( 0, pos + 1 );

            if( class_id == c_log_transformation_op_map_module )
            {
               key += " " + module + " " + class_id;
               transformations.insert( make_pair( key, next_line ) );
            }
            else if( class_id == c_log_transformation_op_skip_operation )
            {
               key += " " + module + " " + class_id + " " + next_line;
               transformations.insert( make_pair( key, string( ) ) );
            }
            else
            {
               pos = next_line.find( ' ' );
               if( pos == string::npos )
                  throw runtime_error( "unexpected transformation info '" + next_line + "'" );

               string operation( next_line.substr( 0, pos ) );
               next_line.erase( 0, pos + 1 );

               if( operation != c_log_transformation_op_ignore_field
                && operation != c_log_transformation_op_map_class_id
                && operation != c_log_transformation_op_map_field_id
                && operation != c_log_transformation_op_map_method_id
                && operation != c_log_transformation_op_skip_operation
                && operation != c_log_transformation_op_file_path_remove
                && operation != c_log_transformation_op_change_field_value
                && operation != c_log_transformation_op_instance_change_field_value )
                  throw runtime_error( "unknown transformation operation '" + operation + "'" );

               if( operation == c_log_transformation_op_map_field_id
                || operation == c_log_transformation_op_map_method_id )
               {
                  pos = next_line.find( ' ' );
                  if( pos == string::npos )
                     throw runtime_error( "unexpected field/method transformation format '" + next_line + "'" );

                  operation += " " + next_line.substr( 0, pos );
                  next_line.erase( 0, pos + 1 );
               }

               if( operation == c_log_transformation_op_skip_operation )
               {
                  operation += " " + next_line;
                  next_line.erase( );
               }

               bool is_op_instance_change_field = false;
               if( operation == c_log_transformation_op_instance_change_field_value )
               {
                  pos = next_line.find( ' ' );
                  if( pos == string::npos )
                     throw runtime_error( "unexpected instance change field format '" + next_line + "'" );

                  is_op_instance_change_field = true;

                  operation += " " + next_line.substr( 0, pos );
                  next_line.erase( 0, pos + 1 );
               }

               if( is_op_instance_change_field || operation == c_log_transformation_op_change_field_value )
               {
                  pos = next_line.find( '=' );
                  if( pos == string::npos )
                     throw runtime_error( "unexpected field transformation format '" + next_line + "'" );

                  operation += " " + next_line.substr( 0, pos );
                  next_line.erase( 0, pos + 1 );
               }

               key += " " + module + " " + class_id + " " + operation;
               transformations.insert( make_pair( key, next_line ) );
            }
         }
         else
            throw runtime_error( "unknown log transformation scope '" + trans_scope + "'" );
      }

      if( !inpf.eof( ) )
         throw runtime_error( "unexpected error occurred whilst reading '" + file_name + "' for input" );
   }
}

class socket_command_handler : public command_handler
{
   public:
   socket_command_handler( tcp_socket& socket )
    :
    socket( socket ),
    restoring( false )
   {
   }

   size_t get_sess_id( ) const { return sess_id; }
   void set_sess_id( size_t new_sess_id ) { sess_id = new_sess_id; }

   tcp_socket& get_socket( ) { return socket; }

   const string& get_next_command( ) { return next_command; }

   bool is_restoring( ) const { return restoring; }

   map< string, string >& get_transformations( ) { return transformations; }

   void output_progress( const string& message )
   {
      socket.write_line( string( c_response_message_prefix ) + message );
   }

   const string& get_restore_error( ) const { return restore_error; }
   void set_restore_error( const string& new_error ) { restore_error = new_error; }

   auto_ptr< restorable< bool > > set_restoring( ) { return auto_ptr< restorable< bool > >( new restorable< bool >( restoring, true ) ); }

   private:
   string preprocess_command_and_args( const string& cmd_and_args );

   void handle_unknown_command( const string& command )
   {
      socket.write_line( string( c_response_error_prefix ) + "unknown command '" + command + "'" );
   }

   void handle_invalid_command( const command_parser& parser, const string& cmd_and_args )
   {
      socket.write_line( string( c_response_error_prefix ) + "invalid command usage '" + cmd_and_args + "'" );
   }

   void handle_command_response( const string& response, bool is_special );

   size_t sess_id;
   tcp_socket& socket;

   bool restoring;

   string next_command;
   string restore_error;

   map< string, string > transformations;
};

string socket_command_handler::preprocess_command_and_args( const string& cmd_and_args )
{
   string str( cmd_and_args );

   if( !str.empty( ) )
   {
      TRACE_LOG( TRACE_COMMANDS, cmd_and_args );

      if( str[ 0 ] == '?' )
      {
         string wildcard_match_expr;

         string::size_type pos = str.find( ' ' );
         if( pos != string::npos )
            wildcard_match_expr = str.substr( pos + 1 );

         if( get_command_processor( ) )
            get_command_processor( )->output_command_usage( wildcard_match_expr );

         str.erase( );
      }
#ifdef DEBUG
      else if( str == "die" )
      {
         bool* p( 0 );
         *p = true;
      }
      else if( str == "kill" )
      {
         g_server_shutdown = true;
         str.erase( );
      }
#endif
   }

   if( !str.empty( ) )
      next_command = str;

   return str;
}

void socket_command_handler::handle_command_response( const string& response, bool is_special )
{
   if( !response.empty( ) )
   {
      if( is_special && !socket.set_no_delay( ) )
         issue_warning( "socket set_no_delay failure" );
      socket.write_line( response );
   }

   if( !is_special )
   {
      if( !socket.set_no_delay( ) )
         issue_warning( "socket set_no_delay failure" );
      socket.write_line( c_response_okay );
   }
}

class ciyam_session_command_functor : public command_functor
{
   public:
   ciyam_session_command_functor( command_handler& handler )
    : command_functor( handler ),
    socket_handler( dynamic_cast< socket_command_handler& >( handler ) )
   {
   }

   void operator ( )( const string& command, const parameter_info& parameters );

   socket_command_handler& socket_handler;
};

command_functor* ciyam_session_command_functor_factory( const string& /*name*/, command_handler& handler )
{
   return new ciyam_session_command_functor( handler );
}

void ciyam_session_command_functor::operator ( )( const string& command, const parameter_info& parameters )
{
#ifdef DEBUG
   cout << "processing command: " << command << endl;
#endif
   string response;
   bool send_okay_response = true;

   tcp_socket& socket( socket_handler.get_socket( ) );

   if( !socket_handler.is_restoring( ) && !socket.set_delay( ) )
      issue_warning( "socket set_delay failure" );

   srand( time( 0 ) );

   set_dtm( "" );
   set_uid( "" );
   set_tz_abbr( "" );
   set_last_session_cmd( command );

   try
   {
      ostringstream osstr;

      if( command == c_cmd_ciyam_session_license )
         response = get_license( true, true );
      else if( command == c_cmd_ciyam_session_version )
         response = c_protocol_version;
      else if( command == c_cmd_ciyam_session_module_list )
      {
         module_list( osstr );
         output_response_lines( socket, osstr.str( ) );
      }
      else if( command == c_cmd_ciyam_session_module_load )
      {
         string module( get_parm_val( parameters, c_cmd_parm_ciyam_session_module_load_module ) );

         module_load( module, "", handler, true );
      }
      else if( command == c_cmd_ciyam_session_module_unload )
      {
         string module( get_parm_val( parameters, c_cmd_parm_ciyam_session_module_unload_module ) );

         module_unload( module, handler, true );
      }
      else if( command == c_cmd_ciyam_session_module_class_list )
      {
         string module( get_parm_val( parameters, c_cmd_parm_ciyam_session_module_class_list_module ) );

         module_class_list( module, osstr );
         output_response_lines( socket, osstr.str( ) );
      }
      else if( command == c_cmd_ciyam_session_module_strings_list )
      {
         string module( get_parm_val( parameters, c_cmd_parm_ciyam_session_module_strings_list_module ) );

         module_strings_list( module, osstr );
         output_response_lines( socket, osstr.str( ) );
      }
      else if( command == c_cmd_ciyam_session_module_class_field_list )
      {
         string module( get_parm_val( parameters, c_cmd_parm_ciyam_session_module_class_field_list_module ) );
         string class_id_or_name( get_parm_val( parameters, c_cmd_parm_ciyam_session_module_class_field_list_class ) );

         module_class_fields_list( module, class_id_or_name, osstr );
         output_response_lines( socket, osstr.str( ) );
      }
      else if( command == c_cmd_ciyam_session_object_list )
      {
         list_object_instances( osstr );
         output_response_lines( socket, osstr.str( ) );
      }
      else if( command == c_cmd_ciyam_session_object_fetch )
      {
         string handle( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_fetch_handle ) );
         string context( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_fetch_context ) );
         string key( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_fetch_key ) );

         instance_fetch( atoi( handle.c_str( ) ), context, key );
      }
      else if( command == c_cmd_ciyam_session_object_create )
      {
         string module( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_create_module ) );
         string mclass( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_create_mclass ) );
         string handle( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_create_handle ) );
         bool dynamic( has_parm_val( parameters, c_cmd_parm_ciyam_session_object_create_dynamic ) );

         mclass = get_class_id_for_id_or_name( module, mclass );

         response = to_string( create_object_instance( module, mclass, atoi( handle.c_str( ) ), dynamic ) );
      }
      else if( command == c_cmd_ciyam_session_object_destroy )
      {
         string handle( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_destroy_handle ) );

         destroy_object_instance( atoi( handle.c_str( ) ) );
      }
      else if( command == c_cmd_ciyam_session_object_execute )
      {
         string handle( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_execute_handle ) );
         string context( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_execute_context ) );
         string method_name_and_args( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_execute_method_name ) );

         if( has_parm_val( parameters, c_cmd_parm_ciyam_session_object_execute_args ) )
         {
            method_name_and_args += ' ';
            method_name_and_args += get_parm_val( parameters, c_cmd_parm_ciyam_session_object_execute_args );
         }

         response = execute_object_command( atoi( handle.c_str( ) ), context, method_name_and_args );
      }
      else if( command == c_cmd_ciyam_session_object_validate )
      {
         string handle( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_validate_handle ) );
         string context( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_validate_context ) );

         validate_object_instance( atoi( handle.c_str( ) ), context );
      }
      else if( command == c_cmd_ciyam_session_object_variable )
      {
         string handle( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_variable_handle ) );
         string context( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_variable_context ) );
         string var_name( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_variable_var_name ) );
         bool has_new_val( has_parm_val( parameters, c_cmd_parm_ciyam_session_object_variable_new_value ) );
         string new_value( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_variable_new_value ) );

         if( has_new_val )
            instance_set_variable( atoi( handle.c_str( ) ), context, var_name, new_value );
         else
            response = instance_get_variable( atoi( handle.c_str( ) ), context, var_name );
      }
      else if( command == c_cmd_ciyam_session_object_op_create )
      {
         string handle( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_op_create_handle ) );
         string context( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_op_create_context ) );
         string key( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_op_create_key ) );

         op_instance_create( atoi( handle.c_str( ) ), context, key );
      }
      else if( command == c_cmd_ciyam_session_object_op_update )
      {
         string handle( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_op_update_handle ) );
         string context( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_op_update_context ) );
         string key( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_op_update_key ) );
         string ver_info( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_op_update_ver_info ) );

         op_instance_update( atoi( handle.c_str( ) ), context, key, ver_info );
      }
      else if( command == c_cmd_ciyam_session_object_op_destroy )
      {
         string handle( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_op_destroy_handle ) );
         string context( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_op_destroy_context ) );
         string key( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_op_destroy_key ) );
         string ver_info( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_op_destroy_ver_info ) );

         op_instance_destroy( atoi( handle.c_str( ) ), context, key, ver_info );
      }
      else if( command == c_cmd_ciyam_session_object_op_apply )
      {
         string handle( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_op_apply_handle ) );
         string context( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_op_apply_context ) );

         op_instance_apply( atoi( handle.c_str( ) ), context );
      }
      else if( command == c_cmd_ciyam_session_object_op_cancel )
      {
         string handle( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_op_cancel_handle ) );
         string context( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_op_cancel_context ) );

         op_instance_cancel( atoi( handle.c_str( ) ), context );
      }
      else if( command == c_cmd_ciyam_session_object_review_begin )
      {
         string handle( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_review_begin_handle ) );
         string context( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_review_begin_context ) );
         string key( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_review_begin_key ) );

         instance_review_begin( atoi( handle.c_str( ) ), context, key );
      }
      else if( command == c_cmd_ciyam_session_object_review_finish )
      {
         string handle( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_review_finish_handle ) );
         string context( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_review_finish_context ) );

         instance_review_finish( atoi( handle.c_str( ) ), context );
      }
      else if( command == c_cmd_ciyam_session_object_iterate_forwards )
      {
         string handle( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_iterate_forwards_handle ) );
         string context( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_iterate_forwards_context ) );
         string key_info( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_iterate_forwards_key_info ) );

         bool inclusive( has_parm_val( parameters, c_cmd_parm_ciyam_session_object_iterate_forwards_inc ) );

         bool rc = instance_iterate( atoi( handle.c_str( ) ),
          context, key_info, "", "", "", "", e_iter_direction_forwards, inclusive );

         if( !rc )
         {
            send_okay_response = false;
            response = c_response_not_found;
         }
      }
      else if( command == c_cmd_ciyam_session_object_iterate_backwards )
      {
         string handle( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_iterate_backwards_handle ) );
         string context( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_iterate_backwards_context ) );
         string key_info( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_iterate_backwards_key_info ) );

         bool inclusive( has_parm_val( parameters, c_cmd_parm_ciyam_session_object_iterate_backwards_inc ) );

         bool rc = instance_iterate( atoi( handle.c_str( ) ),
          context, key_info, "", "", "", "", e_iter_direction_backwards, inclusive );

         if( !rc )
         {
            send_okay_response = false;
            response = c_response_not_found;
         }
      }
      else if( command == c_cmd_ciyam_session_object_iterate_next )
      {
         string handle( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_iterate_next_handle ) );
         string context( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_iterate_next_context ) );

         bool rc = instance_iterate_next( atoi( handle.c_str( ) ), context );

         if( !rc )
         {
            send_okay_response = false;
            response = c_response_not_found;
         }
      }
      else if( command == c_cmd_ciyam_session_object_iterate_stop )
      {
         string handle( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_iterate_stop_handle ) );
         string context( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_iterate_stop_context ) );

         instance_iterate_stop( atoi( handle.c_str( ) ), context );
      }
      else if( command == c_cmd_ciyam_session_perform_fetch )
      {
         string module( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_fetch_module ) );
         string mclass( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_fetch_mclass ) );
         bool is_reverse( has_parm_val( parameters, c_cmd_parm_ciyam_session_perform_fetch_reverse ) );
         string uid( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_fetch_uid ) );
         string tz_abbr( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_fetch_tz_abbr ) );
         string filters( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_fetch_filters ) );
         string perms( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_fetch_perms ) );
         string security_info( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_fetch_security_info ) );
         string search_text( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_fetch_search_text ) );
         string search_query( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_fetch_search_query ) );
         string key_info( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_fetch_key_info ) );
         string limit( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_fetch_limit ) );
         string set_values( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_fetch_set_values ) );
         string fields( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_fetch_fields ) );
         bool minimal( has_parm_val( parameters, c_cmd_parm_ciyam_session_perform_fetch_minimal ) );
         bool create_pdf( has_parm_val( parameters, c_cmd_parm_ciyam_session_perform_fetch_create_pdf ) );
         string format_file( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_fetch_format_file ) );
         string output_file( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_fetch_output_file ) );
         string title_name( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_fetch_title_name ) );

         if( tz_abbr.empty( ) )
            tz_abbr = get_timezone( );

         string context;
         string::size_type pos = mclass.find( ':' );
         if( pos != string::npos )
         {
            context = mclass.substr( pos + 1 );
            mclass.erase( pos );
         }

         mclass = get_class_id_for_id_or_name( module, mclass );

         string parent_key;
         bool found_parent_key = false;

         if( !context.empty( ) && !key_info.empty( ) )
         {
            pos = key_info.find( ':' );
            if( pos != string::npos )
            {
               parent_key = key_info.substr( 0, pos );
               key_info.erase( 0, pos + 1 );
               found_parent_key = true;
            }
         }

         if( !context.empty( ) != found_parent_key )
            throw runtime_error( "must provide both child context and parent key or neither" );

         map< int, string > field_inserts;

         vector< string > field_list;
         if( !fields.empty( ) )
            split( fields, field_list );

         string normal_fields;
         size_t non_inserts = 0;
         vector< summary_info > summaries;
         for( size_t i = 0; i < field_list.size( ); i++ )
         {
            if( field_list[ i ] == c_key_field )
            {
               field_inserts.insert( make_pair( non_inserts, field_list[ i ] ) );
               field_list.erase( field_list.begin( ) + i );
               --i;
            }
            else
            {
               ++non_inserts;

               if( !normal_fields.empty( ) )
                  normal_fields += ",";
               normal_fields += field_list[ i ];

               if( !field_list[ i ].empty( ) && field_list[ i ][ 0 ] == '!' )
               {
                  field_list[ i ].erase( 0, 1 );

                  summary_info info;

                  info.idx = i;
                  info.num = -1;
                  info.child = 0;
                  info.field = field_list[ i ];
                  info.value = string( "\1" );

                  summaries.push_back( info );
               }
            }
         }

         vector< string > filter_list;
         if( !filters.empty( ) )
            split( filters, filter_list );

         set< string > filter_set( filter_list.begin( ), filter_list.end( ) );

         int num_limit = create_pdf ? c_pdf_default_limit : 0;
         if( !limit.empty( ) )
            num_limit = from_string< int >( limit );

         map< string, string > set_value_items;
         if( !set_values.empty( ) )
            parse_field_values( module, mclass, set_values, set_value_items, &socket_handler.get_transformations( ) );

         // KLUDGE: Assume a dynamic instance is needed if a context has been supplied
         // as there is no simple way to otherwise determine this (maybe it's not even
         // worth worrying about trying to optimise this behaviour).
         size_t handle = create_object_instance( module, mclass, 0,
          !context.empty( ) || get_module_class_has_derivations( module, mclass ) );

         for( map< string, string >::iterator i = set_value_items.begin( ), end = set_value_items.end( ); i != end; ++i )
         {
            // NOTE: If a field to be set starts with @ then it is instead assumed to be a "variable".
            if( !i->first.empty( ) && i->first[ 0 ] == '@' )
               instance_set_variable( handle, context, i->first, i->second );
         }

         try
         {
            set_uid( uid );
            set_tz_abbr( tz_abbr );
            
            set< string > perm_set;
            if( !perms.empty( ) )
               split( perms, perm_set );
            set_perms( perm_set );

            // NOTE: If a space is provided as the key then fetch the default record values or if the
            // first character of the key is a space then clone a default record from another.
            if( !key_info.empty( ) && key_info[ 0 ] == ' ' )
            {
               if( create_pdf )
                  throw runtime_error( "pdf generation is not permitted for default values usage" );

               if( !parent_key.empty( ) )
                  instance_fetch( handle, "", parent_key );

               if( key_info == " " )
                  init_object_instance( handle, context, true );
               else
               {
                  // NOTE: When cloning a record to create a default some fields may need to be set
                  // via the "to_store" trigger so a "prepare" is called immediately after a create
                  // so it can be determined that cloning has occurred. The instance create is then
                  // cancelled so that the "to_store" that occurs after setting fields (via another
                  // prepare call) no longer behaves as though a clone has just occurred.
                  op_instance_create( handle, context, c_clone_key + key_info, false );
                  prepare_object_instance( handle, context, true );
                  op_instance_cancel( handle, context );
               }

               for( map< string, string >::iterator i = set_value_items.begin( ), end = set_value_items.end( ); i != end; ++i )
               {
                  // NOTE: If a field to be set starts with @ then it is instead assumed to be a "variable".
                  if( !i->first.empty( ) && i->first[ 0 ] != '@' )
                  {
                     string method_name_and_args( "set " );
                     method_name_and_args += i->first + " ";
                     method_name_and_args += "\"" + escaped( i->second, "\"", c_nul ) + "\"";

                     execute_object_command( handle, context, method_name_and_args );
                  }
               }

               prepare_object_instance( handle, context, true );

               string output( "[" + instance_key_info( handle, context ) + "]" );
               string field_output( get_field_values( handle, context, field_list, tz_abbr, true ) );

               if( !field_output.empty( ) )
                  output += " " + field_output;

               socket.write_line( output );
            }
            else
            {
               size_t num_found = 0;

               if( !parent_key.empty( ) )
                  instance_fetch( handle, "", parent_key );

               map< string, string > pdf_gen_variables;
               multimap< string, string > summary_sorted_values;

               if( create_pdf )
               {
                  pdf_gen_variables.insert( make_pair( "@page", GS( c_str_page ) ) );

                  pdf_gen_variables.insert( make_pair( "@true", GS( c_str_true ) ) );
                  pdf_gen_variables.insert( make_pair( "@false", GS( c_str_false ) ) );

                  pdf_gen_variables.insert( make_pair( "@footer", GS( c_str_footer ) ) );

                  if( !title_name.empty( ) )
                     pdf_gen_variables.insert( make_pair( "@title", title_name ) );
                  else
                     pdf_gen_variables.insert( make_pair( "@title", get_class_display_name( handle, context ) ) );

                  pdf_gen_variables.insert( make_pair( "@class", get_class_display_name( handle, context ) ) );
                  pdf_gen_variables.insert( make_pair( "@plural", get_class_display_name( handle, context, true ) ) );

                  pdf_gen_variables.insert( make_pair( "@permissions", perms ) );

                  if( num_limit != 1 )
                  {
                     pdf_gen_variables.insert( make_pair( "@total", GS( c_str_total ) ) );
                     pdf_gen_variables.insert( make_pair( "@records", "  " + GS( c_str_records ) ) );
                     pdf_gen_variables.insert( make_pair( "@subtotal", "  " + GS( c_str_subtotal ) ) );
                  }
               }

               // NOTE: Because filtering can exclude records from the DB fetch the limit must be
               // omitted if any filters have been supplied.
               if( instance_iterate( handle, context,
                key_info, normal_fields, search_text, search_query, security_info,
                is_reverse ? e_iter_direction_backwards : e_iter_direction_forwards,
                true, !filter_set.empty( ) ? 0 : num_limit, e_sql_optimisation_none ) )
               {
                  do
                  {
                     if( !filter_set.empty( ) && instance_filtered( handle, context, filter_set ) )
                        continue;

                     for( map< string, string >::iterator i = set_value_items.begin( ), end = set_value_items.end( ); i != end; ++i )
                     {
                        // NOTE: If a field to be set starts with @ then it is instead assumed to be a "variable".
                        if( !i->first.empty( ) && i->first[ 0 ] != '@' )
                        {
                           string method_name_and_args( "set " );
                           method_name_and_args += i->first + " ";
                           method_name_and_args += "\"" + escaped( i->second, "\"", c_nul ) + "\"";

                           execute_object_command( handle, context, method_name_and_args );
                        }
                     }

                     if( !set_value_items.empty( ) )
                        prepare_object_instance( handle, context, false );

                     if( create_pdf )
                        add_pdf_variables( handle, context,
                         field_list, summaries, pdf_gen_variables, tz_abbr, num_limit == 1, num_found );
                     else
                     {
                        string output;
                        vector< string > raw_values;

                        string key_output( "[" + instance_key_info( handle, context ) + "]" );

                        string field_output( get_field_values( handle, context, field_list,
                         tz_abbr, false, false, summaries.empty( ) ? 0 : &raw_values, &field_inserts ) );

                        if( minimal )
                           output = field_output;
                        else
                        {
                           output = key_output;
                           if( !field_output.empty( ) )
                              output += " " + field_output;
                        }

                        if( summaries.empty( ) )
                           socket.write_line( output );
                        else
                        {
                           string prefix;
                           for( size_t i = 0; i < summaries.size( ); i++ )
                           {
                              if( i > 0 )
                                 prefix += "\1";
                              prefix += raw_values[ summaries[ i ].idx ];
                           }

                           summary_sorted_values.insert( make_pair( prefix, output ) );
                        }
                     }

                     if( g_server_shutdown || ( num_limit && ++num_found >= num_limit ) )
                     {
                        instance_iterate_stop( handle, context );
                        break;
                     }
                  } while( instance_iterate_next( handle, context ) );
               }

               if( create_pdf )
               {
                  if( !num_found )
                  {
                     add_pdf_variables( handle, context,
                      field_list, summaries, pdf_gen_variables, tz_abbr, num_limit == 1, 0, false );
                  }

                  if( summaries.empty( ) )
                     generate_pdf_doc( format_file, output_file, pdf_gen_variables );
                  else
                  {
                     map< string, string > pdf_final_variables;
                     add_final_pdf_variables( pdf_gen_variables, summaries, pdf_final_variables );

                     generate_pdf_doc( format_file, output_file, pdf_final_variables );
                  }
               }
               else if( !summaries.empty( ) )
               {
                  for( multimap< string, string >::iterator
                   i = summary_sorted_values.begin( ); i != summary_sorted_values.end( ); ++i )
                     socket.write_line( i->second );
               }
            }

            destroy_object_instance( handle );
         }
         catch( ... )
         {
            destroy_object_instance( handle );
            throw;
         }
      }
      else if( command == c_cmd_ciyam_session_perform_create )
      {
         string next_command( socket_handler.get_next_command( ) );

         string uid( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_create_uid ) );
         string dtm( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_create_dtm ) );
         string module( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_create_module ) );
         string mclass( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_create_mclass ) );
         string tz_abbr( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_create_tz_abbr ) );
         string key( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_create_key ) );
         string field_values( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_create_field_values ) );

         if( tz_abbr.empty( ) )
            tz_abbr = get_timezone( );

         set_dtm_if_now( dtm, next_command );

         // NOTE: If no key was provided then automatically generate a key.
         if( key.empty( ) || key[ 0 ] == ' ' )
         {
            string new_key( gen_key( ) );

            // NOTE: The generated key is inserted into the "next_command" so an actual key value
            // will appear in the transaction log (otherwise log operations could not be restored).
            string::size_type pos = next_command.find( "\"" );
            next_command.insert( pos + 1, new_key );

            if( key.size( ) == 1 )
               next_command.erase( pos + 1 + new_key.length( ), 1 );

            // NOTE: If there is a clone key then need to keep it.
            if( key.size( ) > 1 )
               key = new_key + key;
            else
               key = new_key;
         }

         transaction_log_command( next_command );

         module = resolve_module_id( module, &socket_handler.get_transformations( ) );
         mclass = resolve_class_id( module, mclass, &socket_handler.get_transformations( ) );

         // NOTE: If a record was being cloned from a "dead key" then just treat as a normal
         // create instead (which means if any cloned fields were not provided by the create
         // command itself then the record may not be the same after a restore occurs).
         string::size_type pos = key.find( ' ' );
         if( pos != string::npos )
         {
            string clone_key( key.substr( pos + 1 ) );
            if( clone_key.empty( ) || storage_is_dead_key( mclass, clone_key ) )
               key.erase( pos );
         }

         map< string, string > field_value_items;

         bool skip_create = false;

         string ltf_key( c_log_transformation_scope_any_perform_op );
         ltf_key += " " + module + " " + string( c_log_transformation_op_skip_operation ) + " " + mclass;

         string ltf_uid_dtm_key( ltf_key + " " + uid + " " + dtm );

         if( socket_handler.get_transformations( ).count( ltf_key )
          || socket_handler.get_transformations( ).count( ltf_uid_dtm_key ) )
            skip_create = true;
         else
         {
            ltf_key = string( c_log_transformation_scope_create_update_destroy );
            ltf_key += " " + module + " " + mclass + " " + string( c_log_transformation_op_skip_operation ) + " " + key;

            if( socket_handler.get_transformations( ).count( ltf_key ) )
               skip_create = true;
            else if( !field_values.empty( ) )
            {
               parse_field_values( module, mclass, field_values, field_value_items, &socket_handler.get_transformations( ) );

               ltf_key = string( c_log_transformation_scope_create_update_only );
               ltf_key += " " + module + " " + mclass + " " + string( c_log_transformation_op_ignore_field );

               if( socket_handler.get_transformations( ).count( ltf_key ) )
               {
                  string field_id( socket_handler.get_transformations( )[ ltf_key ] );

                  if( field_value_items.count( field_id ) )
                     field_value_items.erase( field_value_items.find( field_id ) );
               }

               ltf_key = string( c_log_transformation_scope_create_update_only );
               ltf_key += " " + module + " " + mclass + " " + string( c_log_transformation_op_change_field_value );

               if( !socket_handler.get_transformations( ).empty( ) )
                  perform_field_value_transformations( socket_handler.get_transformations( ), ltf_key, field_value_items );

               ltf_key = string( c_log_transformation_scope_create_update_only );
               ltf_key += " " + module + " " + mclass
                + " " + string( c_log_transformation_op_instance_change_field_value ) + " " + key;

               if( !socket_handler.get_transformations( ).empty( ) )
                  perform_field_value_transformations( socket_handler.get_transformations( ), ltf_key, field_value_items );
            }
         }

         if( socket_handler.is_restoring( ) && storage_is_dead_key( mclass, key ) )
            skip_create = true;

         if( !skip_create )
         {
            size_t handle = create_object_instance( module, mclass, 0, false );
            try
            {
               set_uid( uid );
               set_dtm( dtm );
               set_class( mclass );
               set_module( module );
               set_tz_abbr( tz_abbr );

               transaction_start( );

               op_instance_create( handle, "", key, false );

               for( map< string, string >::iterator i = field_value_items.begin( ), end = field_value_items.end( ); i != end; ++i )
               {
                  if( !i->second.empty( ) && !tz_abbr.empty( ) )
                  {
                     string type_name = get_field_type_name( handle, "", i->first );
                     if( type_name == "date_time" || type_name == "tdatetime" )
                        i->second = convert_local_to_utc( i->second, tz_abbr );
                  }

                  string method_name_and_args( "set " );
                  method_name_and_args += i->first + " ";
                  method_name_and_args += "\"" + escaped( i->second, "\"", c_nul ) + "\"";

                  execute_object_command( handle, "", method_name_and_args );
               }

               op_instance_apply( handle, "", false );
               transaction_commit( );

               response = key.substr( 0, key.find( ' ' ) );

               destroy_object_instance( handle );
            }
            catch( ... )
            {
               transaction_rollback( );
               destroy_object_instance( handle );
               throw;
            }
         }
      }
      else if( command == c_cmd_ciyam_session_perform_update )
      {
         string next_command( socket_handler.get_next_command( ) );

         string uid( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_update_uid ) );
         string dtm( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_update_dtm ) );
         string module( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_update_module ) );
         string mclass( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_update_mclass ) );
         string tz_abbr( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_update_tz_abbr ) );
         string key( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_update_key ) );
         string ver_info( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_update_ver_info ) );
         string field_values( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_update_field_values ) );
         string method_id_or_name( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_update_method_name ) );
         string check_values( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_update_check_values ) );

         if( tz_abbr.empty( ) )
            tz_abbr = get_timezone( );

         // NOTE: Ignore version information during storage recovery.
         if( socket_handler.is_restoring( ) )
            ver_info.erase( );

         set_dtm_if_now( dtm, next_command );

         transaction_log_command( next_command );

         module = resolve_module_id( module, &socket_handler.get_transformations( ) );
         mclass = resolve_class_id( module, mclass, &socket_handler.get_transformations( ) );

         map< string, string > field_value_items;
         parse_field_values( module, mclass, field_values, field_value_items, &socket_handler.get_transformations( ) );

         map< string, string > check_value_items;
         if( !check_values.empty( ) )
            parse_field_values( module, mclass, check_values, check_value_items, &socket_handler.get_transformations( ) );

         string ltf_key( c_log_transformation_scope_create_update_only );
         ltf_key += " " + module + " " + mclass + " " + string( c_log_transformation_op_ignore_field );

         if( socket_handler.get_transformations( ).count( ltf_key ) )
         {
            string field_id( socket_handler.get_transformations( )[ ltf_key ] );

            if( field_value_items.count( field_id ) )
               field_value_items.erase( field_value_items.find( field_id ) );

            if( check_value_items.count( field_id ) )
               check_value_items.erase( check_value_items.find( field_id ) );
         }

         ltf_key = string( c_log_transformation_scope_create_update_only );
         ltf_key += " " + module + " " + mclass + " " + string( c_log_transformation_op_file_path_remove );

         if( socket_handler.get_transformations( ).count( ltf_key ) )
         {
            string field_id( socket_handler.get_transformations( )[ ltf_key ] );

            // NOTE: Legacy attached files (of which there can only be one per instance) included
            // the path and did not have the same name as the instance key. For such files rename
            // the physical file (using copy and delete) and modify the field value to now become
            // the key value plus the file extension.
            if( field_value_items.count( field_id ) && !field_value_items[ field_id ].empty( ) )
            {
               string file_name( field_value_items[ field_id ] );
#ifndef _WIN32
               string::size_type pos = file_name.find_last_of( '/' );
#else
               string::size_type pos = file_name.find_last_of( "/\\" );
#endif
               string file_directory;
               if( pos != string::npos )
               {
                  file_directory = file_name.substr( 0, pos );
                  file_name.erase( 0, pos + 1 );
               }

               pos = file_name.find( '.' );
               string file_name_wo_ext( file_name.substr( 0, pos ) );

               if( key != file_name_wo_ext )
               {
                  // KLUDGE: Some files managed to end up with a non-matching filename between
                  // upgrades so try to fix them up by renaming them here.
                  if( file_directory.empty( ) )
                     file_directory = get_attached_file_path( module, mclass );

                  string old_file( file_directory + "/" + file_name );
                  string new_file( file_directory + "/" + key + get_ext( file_name ) );

                  if( file_exists( old_file.c_str( ) ) )
                     rename( old_file.c_str( ), new_file.c_str( ) );

                  file_name = key + get_ext( file_name );
               }

               field_value_items[ field_id ] = file_name;
            }
         }

         ltf_key = string( c_log_transformation_scope_create_update_only );
         ltf_key += " " + module + " " + mclass + " " + string( c_log_transformation_op_change_field_value );

         if( !socket_handler.get_transformations( ).empty( ) )
            perform_field_value_transformations( socket_handler.get_transformations( ), ltf_key, field_value_items );

         ltf_key = string( c_log_transformation_scope_create_update_only );
         ltf_key += " " + module + " " + mclass
          + " " + string( c_log_transformation_op_instance_change_field_value ) + " " + key;

         if( !socket_handler.get_transformations( ).empty( ) )
            perform_field_value_transformations( socket_handler.get_transformations( ), ltf_key, field_value_items );

         bool skip_update = false;

         ltf_key = string( c_log_transformation_scope_any_perform_op );
         ltf_key += " " + module + " " + string( c_log_transformation_op_skip_operation ) + " " + mclass;

         string ltf_uid_dtm_key( ltf_key + " " + uid + " " + dtm );

         if( socket_handler.get_transformations( ).count( ltf_key )
          || socket_handler.get_transformations( ).count( ltf_uid_dtm_key ) )
            skip_update = true;
         else
         {
            ltf_key = string( c_log_transformation_scope_create_update_destroy );
            ltf_key += " " + module + " " + mclass + " " + string( c_log_transformation_op_skip_operation ) + " " + key;

            if( socket_handler.get_transformations( ).count( ltf_key ) )
               skip_update = true;
         }

         if( socket_handler.is_restoring( ) && storage_is_dead_key( mclass, key ) )
            skip_update = true;

         if( !skip_update )
         {
            if( !method_id_or_name.empty( ) )
               transaction_start( );

            size_t handle = create_object_instance( module, mclass,
             0, get_module_class_has_derivations( module, mclass ) );

            try
            {
               set_uid( uid );
               set_dtm( dtm );
               set_class( mclass );
               set_module( module );
               set_tz_abbr( tz_abbr );

               op_instance_update( handle, "", key, ver_info, false );

               for( map< string, string >::iterator i = check_value_items.begin( ), end = check_value_items.end( ); i != end; ++i )
               {
                  string method_name_and_args( "get " );
                  method_name_and_args += i->first;
                  string value = execute_object_command( handle, "", method_name_and_args );

                  if( value != i->second )
                     throw runtime_error( get_string_message( GS( c_str_field_is_incorrect ),
                      make_pair( c_str_parm_field_is_incorrect_field, get_field_name_for_id( handle, "", i->first ) ) ) );
               }

               for( map< string, string >::iterator i = field_value_items.begin( ), end = field_value_items.end( ); i != end; ++i )
               {
                  if( !i->second.empty( ) && !tz_abbr.empty( ) )
                  {
                     string type_name = get_field_type_name( handle, "", i->first );
                     if( type_name == "date_time" || type_name == "tdatetime" )
                        i->second = convert_local_to_utc( i->second, tz_abbr );
                  }

                  string method_name_and_args( "set " );
                  method_name_and_args += i->first + " ";
                  method_name_and_args += "\"" + escaped( i->second, "\"", c_nul ) + "\"";

                  execute_object_command( handle, "", method_name_and_args );
               }

               op_instance_apply( handle, "", false );

               // NOTE: If a method name has also been provided then execute it now (omitting the version).
               if( !method_id_or_name.empty( ) )
               {
                  method_id_or_name = resolve_method_name( module,
                   mclass, method_id_or_name, &socket_handler.get_transformations( ) );

                  response = instance_execute( handle, "", key, method_id_or_name );
                  transaction_commit( );
               }

               destroy_object_instance( handle );
            }
            catch( ... )
            {
               if( !method_id_or_name.empty( ) )
                  transaction_rollback( );

               destroy_object_instance( handle );
               throw;
            }
         }
      }
      else if( command == c_cmd_ciyam_session_perform_destroy )
      {
         string next_command( socket_handler.get_next_command( ) );

         string uid( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_destroy_uid ) );
         string dtm( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_destroy_dtm ) );
         string module( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_destroy_module ) );
         string mclass( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_destroy_mclass ) );
         string tz_abbr( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_destroy_tz_abbr ) );
         bool quiet( has_parm_val( parameters, c_cmd_parm_ciyam_session_perform_destroy_quiet ) );
         string key( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_destroy_key ) );
         string ver_info( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_destroy_ver_info ) );

         if( tz_abbr.empty( ) )
            tz_abbr = get_timezone( );

         // NOTE: Ignore version information during storage recovery.
         if( socket_handler.is_restoring( ) )
            ver_info.erase( );

         set_dtm_if_now( dtm, next_command );

         transaction_log_command( next_command );

         module = resolve_module_id( module, &socket_handler.get_transformations( ) );
         mclass = resolve_class_id( module, mclass, &socket_handler.get_transformations( ) );

         bool skip_destroy = false;

         string ltf_key( c_log_transformation_scope_any_perform_op );
         ltf_key += " " + module + " " + string( c_log_transformation_op_skip_operation ) + " " + mclass;

         string ltf_uid_dtm_key( ltf_key + " " + uid + " " + dtm );

         if( socket_handler.get_transformations( ).count( ltf_key )
          || socket_handler.get_transformations( ).count( ltf_uid_dtm_key ) )
            skip_destroy = true;
         else
         {
            ltf_key = string( c_log_transformation_scope_create_update_destroy );
            ltf_key += " " + module + " " + mclass + " " + string( c_log_transformation_op_skip_operation ) + " " + key;

            if( socket_handler.get_transformations( ).count( ltf_key ) )
               skip_destroy = true;
         }

         if( socket_handler.is_restoring( ) && storage_is_dead_key( mclass, key ) )
            skip_destroy = true;

         if( !skip_destroy )
         {
            size_t handle = create_object_instance( module, mclass,
             0, get_module_class_has_derivations( module, mclass ) );

            try
            {
               set_uid( uid );
               set_dtm( dtm );
               set_class( mclass );
               set_module( module );
               set_tz_abbr( tz_abbr );

               op_destroy_rc rc;
               op_instance_destroy( handle, "", key, ver_info, false, &rc );

               // NOTE: If the "quiet" option is used then will quietly ignore records that are
               // not found. But otherwise if not okay then call without the return code so the
               // exception will be issued (FUTURE - this could be handled more efficiently).
               if( rc != e_op_destroy_rc_okay && ( !quiet || rc != e_op_destroy_rc_not_found ) )
                  op_instance_destroy( handle, "", key, ver_info, false );

               if( rc == e_op_destroy_rc_okay )
                  op_instance_apply( handle, "", false );

               destroy_object_instance( handle );
            }
            catch( ... )
            {
               destroy_object_instance( handle );
               throw;
            }
         }
      }
      else if( command == c_cmd_ciyam_session_perform_execute )
      {
         string next_command( socket_handler.get_next_command( ) );

         string uid( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_execute_uid ) );
         string dtm( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_execute_dtm ) );
         string module( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_execute_module ) );
         string mclass( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_execute_mclass ) );
         string tz_abbr( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_execute_tz_abbr ) );
         string set_values( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_execute_set_values ) );
         string keys( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_execute_keys ) );
         string vers( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_execute_vers ) );
         string method_name_and_args( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_execute_method_name ) );

         if( tz_abbr.empty( ) )
            tz_abbr = get_timezone( );

         // NOTE: Ignore version information during storage recovery.
         if( socket_handler.is_restoring( ) )
            vers.erase( );

         set_dtm_if_now( dtm, next_command );

         bool skip_transaction = false;

         // NOTE: If a method id/name is prefixed by an underbar then this command is deemed to be
         // a "non-transactional" command and will not be logged (nor will a be transaction used).
         if( !method_name_and_args.empty( ) && method_name_and_args[ 0 ] == '_' )
         {
            skip_transaction = true;
            method_name_and_args.erase( 0, 1 );
         }
         else
         {
            // NOTE: If method id/name is prefixed by a minus sign then the command will still be
            // logged but a transaction will not be issued whilst handling the command here.
            if( !method_name_and_args.empty( ) && method_name_and_args[ 0 ] == '-' )
            {
               skip_transaction = true;
               method_name_and_args.erase( 0, 1 );
            }
            transaction_log_command( next_command );
         }

         module = resolve_module_id( module, &socket_handler.get_transformations( ) );
         mclass = resolve_class_id( module, mclass, &socket_handler.get_transformations( ) );

         bool skip_execute = false;

         string ltf_key( c_log_transformation_scope_any_perform_op );
         ltf_key += " " + module + " " + string( c_log_transformation_op_skip_operation ) + " " + mclass;

         string ltf_uid_dtm_key( ltf_key + " " + uid + " " + dtm );

         if( socket_handler.get_transformations( ).count( ltf_key )
          || socket_handler.get_transformations( ).count( ltf_uid_dtm_key ) )
            skip_execute = true;

         if( !skip_execute )
         {
            string method_id;

            method_name_and_args = resolve_method_name(
             module, mclass, method_name_and_args, &socket_handler.get_transformations( ), &method_id );

            map< string, string > set_value_items;
            if( !set_values.empty( ) )
               parse_field_values( module, mclass, set_values, set_value_items, &socket_handler.get_transformations( ) );

            if( has_parm_val( parameters, c_cmd_parm_ciyam_session_perform_execute_args ) )
            {
               string execute_args( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_execute_args ) );

               string ltf_key( c_log_transformation_scope_execute_only );
               ltf_key += " " + module + " " + mclass
                + " " + method_id + " " + string( c_log_transformation_op_map_first_arg_field_ids );

               if( socket_handler.get_transformations( ).count( ltf_key ) )
               {
                  string::size_type pos = execute_args.find( ' ' );

                  if( pos != 0 )
                  {
                     string new_first_arg;

                     vector< string > field_ids;
                     split( execute_args.substr( 0, pos ), field_ids );

                     for( size_t i = 0; i < field_ids.size( ); i++ )
                     {
                        string ltf_key( c_log_transformation_scope_any_change );
                        ltf_key += " " + module + " " + mclass + " "
                         + string( c_log_transformation_op_map_field_id ) + " " + field_ids[ i ];

                        if( !new_first_arg.empty( ) )
                           new_first_arg += ',';

                        if( !socket_handler.get_transformations( ).count( ltf_key ) )
                           new_first_arg += field_ids[ i ];
                        else
                           new_first_arg += socket_handler.get_transformations( ).find( ltf_key )->second;
                     }

                     execute_args = new_first_arg + execute_args.substr( pos );
                  }
               }
               method_name_and_args += ' ' + execute_args;
            }
            else
            {
               string ltf_key( c_log_transformation_scope_execute_only );
               ltf_key += " " + module + " " + mclass
                + " " + method_id + " " + string( c_log_transformation_op_no_args_append );

               if( socket_handler.get_transformations( ).count( ltf_key ) )
               {
                  method_name_and_args += ' ';
                  method_name_and_args += socket_handler.get_transformations( )[ ltf_key ];
               }
            }

            size_t handle = create_object_instance( module, mclass,
             0, get_module_class_has_derivations( module, mclass ) );

            for( map< string, string >::iterator i = set_value_items.begin( ), end = set_value_items.end( ); i != end; ++i )
            {
               // NOTE: If a field to be set starts with @ then it is instead assumed to be a "variable".
               if( !i->first.empty( ) && i->first[ 0 ] == '@' )
                  instance_set_variable( handle, "", i->first, i->second );
            }

            string client_message = instance_get_variable( handle, "", "@message" );
            if( !client_message.empty( ) )
               handler.output_progress( client_message );

            vector< string > all_keys;
            vector< string > all_vers;

            if( !keys.empty( ) )
               split( keys, all_keys );

            if( !vers.empty( ) )
               split( vers, all_vers );

            bool is_first = true;
            size_t num_responses = 0;

            try
            {
               set_uid( uid );
               set_dtm( dtm );
               set_class( mclass );
               set_module( module );
               set_tz_abbr( tz_abbr );

               if( all_vers.size( ) && ( all_keys.size( ) != all_vers.size( ) ) )
                  throw runtime_error( "unexpected # keys ("
                   + to_string( all_keys.size( ) ) + ") not equal to # vers (" + to_string( all_vers.size( ) ) + ")" );

               for( size_t i = 0; i < all_keys.size( ); i++ )
               {
                  string next_key( all_keys[ i ] );
                  string next_ver( all_vers.size( ) ? all_vers[ i ] : "" );

                  if( next_key == " " )
                  {
                     next_key.erase( );
                     next_ver.erase( );

                     if( all_keys.size( ) > 1 )
                        throw runtime_error( "unexpected empty key in multi-key execute" );
                  }
                  else
                  {
                     if( socket_handler.is_restoring( ) && storage_is_dead_key( mclass, next_key ) )
                        continue;
                  }

                  if( is_first )
                  {
                     is_first = false;
                     if( !skip_transaction )
                        transaction_start( );
                  }

                  instance_prepare_execute( handle, "", next_key, next_ver );

                  for( map< string, string >::iterator j = set_value_items.begin( ), end = set_value_items.end( ); j != end; ++j )
                  {
                     // NOTE: If a field to be set starts with @ then it is instead assumed to be a "variable".
                     if( !j->first.empty( ) && j->first[ 0 ] != '@' )
                     {
                        string method_name_and_args( "set " );
                        method_name_and_args += j->first + " ";
                        method_name_and_args += "\"" + escaped( j->second, "\"", c_nul ) + "\"";

                        execute_object_command( handle, "", method_name_and_args );
                     }
                  }

                  string next_response( instance_execute( handle, "", next_key, method_name_and_args ) );

                  string return_response;
                  // NOTE: Cannot have CR/LF pairs in a response (as the client will get confused) so if
                  // these are found in the initial return string then change them to just LF's.
                  for( size_t j = 0; j < next_response.size( ); j++ )
                  {
                     if( j < next_response.size( ) - 1
                      && next_response[ j ] == '\r' && next_response[ j + 1 ] == '\n' )
                     {
                        ++i;
                        return_response += '\n';
                     }
                     else
                        return_response += next_response[ j ];
                  }

                  ++num_responses;
                  if( return_response.empty( ) && i < all_keys.size( ) - 1 )
                     return_response = c_response_okay_more;

                  if( !socket_handler.is_restoring( ) && !return_response.empty( ) )
                     socket.write_line( return_response );
               }

               if( !is_first && !skip_transaction )
                  transaction_commit( );

               destroy_object_instance( handle );
            }
            catch( exception& x )
            {
               if( socket_handler.is_restoring( ) )
                  socket_handler.set_restore_error( x.what( ) );
               else
               {
                  send_okay_response = false;
                  transaction_log_command( "" );
                  response = string( c_response_error_prefix ) + x.what( );

                  // NOTE: As the client is expecting a response for each key provided when an
                  // an exception is thrown fill out any remaining responses with "okay more".
                  for( size_t i = num_responses; i < all_keys.size( ) - 1; i++ )
                     socket.write_line( c_response_okay_more );
               }

               if( !is_first && !skip_transaction )
                  transaction_rollback( );

               destroy_object_instance( handle );
            }
            catch( ... )
            {
               if( socket_handler.is_restoring( ) )
                  socket_handler.set_restore_error( "unexpected unknown exception caught" );
               else
               {
                  send_okay_response = false;
                  transaction_log_command( "" );
                  response = string( c_response_error_prefix ) + "unexpected unknown exception caught";

                  // NOTE: As the client is expecting a response for each key provided when an
                  // an exception is thrown fill out any remaining responses with "okay more".
                  for( size_t i = num_responses; i < all_keys.size( ) - 1; i++ )
                     socket.write_line( c_response_okay_more );
               }

               if( !is_first && !skip_transaction )
                  transaction_rollback( );

               destroy_object_instance( handle );
            }
         }
      }
      else if( command == c_cmd_ciyam_session_perform_bulk_ops )
      {
         string uid( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_bulk_ops_uid ) );
         string dtm( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_bulk_ops_dtm ) );
         string module( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_bulk_ops_module ) );
         string mclass( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_bulk_ops_mclass ) );
         string tz_abbr( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_bulk_ops_tz_abbr ) );
         string filename( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_bulk_ops_filename ) );
         string export_fields( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_bulk_ops_export_fields ) );
         bool destroy_records( has_parm_val( parameters, c_cmd_parm_ciyam_session_perform_bulk_ops_destroy_records ) );
         string search_text( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_bulk_ops_search_text ) );
         string search_query( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_bulk_ops_search_query ) );
         string fixed_field_values( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_bulk_ops_fixed_field_values ) );

         if( tz_abbr.empty( ) )
            tz_abbr = get_timezone( );

         if( dtm == c_dtm_now )
            dtm = date_time::standard( ).as_string( );

         response = exec_bulk_ops( module, uid, dtm, mclass, filename,
          export_fields, tz_abbr, destroy_records, search_text, search_query, fixed_field_values, handler );
      }
      else if( command == c_cmd_ciyam_session_perform_package_export )
      {
         string module( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_package_export_module ) );
         string mclass( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_package_export_mclass ) );
         string keys( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_package_export_keys ) );
         string filename( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_package_export_filename ) );
         string test_info( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_package_export_test_info ) );
         string exclude_info( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_package_export_exclude_info ) );
         string include_info( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_package_export_include_info ) );

         export_package( module, mclass, keys, exclude_info, test_info, include_info, filename );
      }
      else if( command == c_cmd_ciyam_session_perform_package_import )
      {
         string uid( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_package_import_uid ) );
         string dtm( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_package_import_dtm ) );
         string module( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_package_import_module ) );
         string filename( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_package_import_filename ) );
         bool new_only( has_parm_val( parameters, c_cmd_parm_ciyam_session_perform_package_import_new_only ) );
         string key_prefix( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_package_import_key_prefix ) );
         string replace_info( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_package_import_replace_info ) );
         string skip_field_info( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_package_import_skip_field_info ) );

         if( dtm == c_dtm_now )
            dtm = date_time::standard( ).as_string( );

         import_package( module, uid, dtm, filename, key_prefix, replace_info, skip_field_info, new_only );
      }
      else if( command == c_cmd_ciyam_session_session_list )
      {
         bool minimal( has_parm_val( parameters, c_cmd_parm_ciyam_session_session_list_minimal ) );

         list_sessions( osstr, !minimal );
         output_response_lines( socket, osstr.str( ) );
      }
      else if( command == c_cmd_ciyam_session_session_kill )
      {
         bool kill_all( has_parm_val( parameters, c_cmd_parm_ciyam_session_session_kill_all ) );
         string sess_ids( get_parm_val( parameters, c_cmd_parm_ciyam_session_session_kill_sess_ids ) );
         string seconds( get_parm_val( parameters, c_cmd_parm_ciyam_session_session_kill_seconds ) );

         int num_seconds = 0;
         if( !seconds.empty( ) )
            num_seconds = atoi( seconds.c_str( ) );

         if( kill_all )
            condemn_all_other_sessions( num_seconds );
         else if( !sess_ids.empty( ) )
         {
            vector< string > session_ids;
            split( sess_ids, session_ids );

            for( size_t i = 0; i < session_ids.size( ); i++ )
               condemn_session( atoi( session_ids[ i ].c_str( ) ), num_seconds );
         }
      }
      else if( command == c_cmd_ciyam_session_session_lock )
      {
         bool lock_capture( has_parm_val( parameters, c_cmd_parm_ciyam_session_session_lock_capture ) );
         bool lock_release( has_parm_val( parameters, c_cmd_parm_ciyam_session_session_lock_release ) );
         bool lock_all( has_parm_val( parameters, c_cmd_parm_ciyam_session_session_lock_all ) );
         string sess_ids( get_parm_val( parameters, c_cmd_parm_ciyam_session_session_lock_sess_ids ) );

         if( lock_all )
         {
            if( lock_capture )
               capture_all_other_sessions( );
            else
               release_all_other_sessions( );
         }
         else if( !sess_ids.empty( ) )
         {
            vector< string > session_ids;
            split( sess_ids, session_ids );

            for( size_t i = 0; i < session_ids.size( ); i++ )
            {
               if( lock_capture )
                  capture_session( atoi( session_ids[ i ].c_str( ) ) );
               else
                  release_session( atoi( session_ids[ i ].c_str( ) ) );
            }
         }
      }
      else if( command == c_cmd_ciyam_session_session_timeout )
      {
         string seconds( get_parm_val( parameters, c_cmd_parm_ciyam_session_session_timeout_seconds ) );

         int val;
         if( !seconds.empty( ) )
         {
            val = atoi( seconds.c_str( ) );
            set_session_timeout( val );
         }
         else
            val = get_session_timeout( );

         response = to_string( val );
      }
      else if( command == c_cmd_ciyam_session_session_variable )
      {
         string var_name( get_parm_val( parameters, c_cmd_parm_ciyam_session_session_variable_var_name ) );
         bool has_new_val( has_parm_val( parameters, c_cmd_parm_ciyam_session_session_variable_new_value ) );
         string new_value( get_parm_val( parameters, c_cmd_parm_ciyam_session_session_variable_new_value ) );

         if( has_new_val )
            set_session_variable( var_name, new_value );

         response = get_session_variable( var_name );
      }
      else if( command == c_cmd_ciyam_session_storage_info )
      {
         response = "Name: " + storage_name( ) + '\n';
         response += "Prefix: " + storage_module_prefix( ) + '\n';
         response += "Identity: " + storage_identity( );
      }
      else if( command == c_cmd_ciyam_session_storage_init )
      {
         string name( get_parm_val( parameters, c_cmd_parm_ciyam_session_storage_init_name ) );
         string prefix( get_parm_val( parameters, c_cmd_parm_ciyam_session_storage_init_prefix ) );

         init_storage( name, prefix, handler, has_parm_val( parameters, c_cmd_parm_ciyam_session_storage_init_admin ) );
      }
      else if( command == c_cmd_ciyam_session_storage_term )
         term_storage( handler );
      else if( command == c_cmd_ciyam_session_storage_create )
      {
         string name( get_parm_val( parameters, c_cmd_parm_ciyam_session_storage_create_name ) );
         string prefix( get_parm_val( parameters, c_cmd_parm_ciyam_session_storage_create_prefix ) );

         create_storage( name, prefix, handler, has_parm_val( parameters, c_cmd_parm_ciyam_session_storage_create_admin ) );
      }
      else if( command == c_cmd_ciyam_session_storage_attach )
      {
         string name( get_parm_val( parameters, c_cmd_parm_ciyam_session_storage_attach_name ) );

         attach_storage( name, handler, has_parm_val( parameters, c_cmd_parm_ciyam_session_storage_attach_admin ) );
      }
      else if( command == c_cmd_ciyam_session_storage_backup )
      {
         string name( get_parm_val( parameters, c_cmd_parm_ciyam_session_storage_backup_name ) );
         bool truncate_log( has_parm_val( parameters, c_cmd_parm_ciyam_session_storage_backup_truncate ) );

         int truncation_count = 0;

         init_storage( name, "", handler, true );
         backup_storage( handler, ( truncate_log ? &truncation_count : 0 ) );
         term_storage( handler );

         bool has_dead_keys = false;

         // NOTE: Scope to ensure streams are closed.
         {
            string sql_name( name + ".sql" );
            string dat_name( name + ".dat" );
            string idx_name( name + ".idx" );
            string hdr_name( name + ".hdr" );
            string log_name( name + ".log" );

            string sav_sql_name( sql_name + ".sav" );
            string sav_dat_name( dat_name + ".sav" );
            string sav_idx_name( idx_name + ".sav" );
            string sav_hdr_name( hdr_name + ".sav" );
            string sav_log_name( log_name + ".sav" );

            ifstream sqlf( sql_name.c_str( ), ios::in | ios::binary );
            ifstream datf( dat_name.c_str( ), ios::in | ios::binary );
            ifstream idxf( idx_name.c_str( ), ios::in | ios::binary );
            ifstream hdrf( hdr_name.c_str( ), ios::in | ios::binary );
            ifstream logf( log_name.c_str( ), ios::in | ios::binary );

            if( !sqlf || !datf || !idxf || !hdrf || !logf )
               throw runtime_error( "unable to open DB files for '" + name + "' (in use?)" );

            ofstream sav_sqlf( sav_sql_name.c_str( ), ios::out | ios::binary );
            ofstream sav_datf( sav_dat_name.c_str( ), ios::out | ios::binary );
            ofstream sav_idxf( sav_idx_name.c_str( ), ios::out | ios::binary );
            ofstream sav_hdrf( sav_hdr_name.c_str( ), ios::out | ios::binary );
            ofstream sav_logf( sav_log_name.c_str( ), ios::out | ios::binary );

            if( !sav_sqlf || !sav_datf || !sav_idxf || !sav_hdrf || !sav_logf )
               throw runtime_error( "unable to open backup files for '" + name + "'" );

            copy_stream( sqlf, sav_sqlf );
            copy_stream( datf, sav_datf );
            copy_stream( idxf, sav_idxf );
            copy_stream( hdrf, sav_hdrf );
            copy_stream( logf, sav_logf );

            string key_name( name + ".dead_keys.lst" );
            if( file_exists( key_name ) )
            {
               ifstream keyf( key_name.c_str( ), ios::in | ios::binary );
               if( !keyf )
                  throw runtime_error( "unable to open '" + key_name + "' for input" );

               string sav_key_name( key_name + ".sav" );

               ofstream sav_keyf( sav_key_name.c_str( ), ios::out | ios::binary );
               if( !sav_keyf )
                  throw runtime_error( "unable to open '" + sav_key_name + "' for output" );

               copy_stream( keyf, sav_keyf );
               has_dead_keys = true;
            }
         }

         string file_names( name + ".sql.sav" );
         file_names += " " + name + ".dat.sav";
         file_names += " " + name + ".idx.sav";
         file_names += " " + name + ".hdr.sav";
         file_names += " " + name + ".log.sav";

         if( has_dead_keys )
            file_names += " " + name + ".dead_keys.lst.sav";

         file_names += " " + name + ".backup.sql";

         string module_list( name + ".modules.lst" );
         file_names += " " + module_list;

         vector< string > modules;
         buffer_file_lines( module_list, modules );

         for( size_t i = 0; i < modules.size( ); i++ )
         {
            string module_init_list( modules[ i ] + ".init.lst" );
            if( file_exists( module_init_list ) )
            {
               file_names += " " + module_init_list;

               vector< string > init_classes;
               buffer_file_lines( module_init_list, init_classes );

               for( size_t j = 0; j < init_classes.size( ); j++ )
                  file_names += " " + modules[ i ] + "_" + init_classes[ j ] + ".csv";
            }
         }

         ostringstream osstr;
         osstr << setw( 3 ) << setfill( '0' ) << truncation_count;

         if( file_exists( name + ".backup.zip" ) )
            remove_file( name + ".backup.zip" );

         exec_system( "zip -q " + name + ".backup.zip " + file_names );

         if( truncate_log )
            exec_system( "zip -q " + name + "." + osstr.str( ) + ".zip " + name + ".log." + osstr.str( ) );

         remove_file( name + ".sql.sav" );
         remove_file( name + ".dat.sav" );
         remove_file( name + ".idx.sav" );
         remove_file( name + ".hdr.sav" );
         remove_file( name + ".log.sav" );
         remove_file( name + ".backup.sql" );

         if( truncate_log )
            remove_file( name + ".log." + osstr.str( ) );
      }
      else if( command == c_cmd_ciyam_session_storage_restore )
      {
         string name( get_parm_val( parameters, c_cmd_parm_ciyam_session_storage_restore_name ) );
         string prefix( get_parm_val( parameters, c_cmd_parm_ciyam_session_storage_restore_prefix ) );
         string trace_info( get_parm_val( parameters, c_cmd_parm_ciyam_session_storage_restore_trace_info ) );
         bool rebuild( has_parm_val( parameters, c_cmd_parm_ciyam_session_storage_restore_rebuild ) );
         bool partial( has_parm_val( parameters, c_cmd_parm_ciyam_session_storage_restore_partial ) );
         bool quicker( has_parm_val( parameters, c_cmd_parm_ciyam_session_storage_restore_quicker ) );

         string sql_name( name + ".sql" );
         string dat_name( name + ".dat" );
         string idx_name( name + ".idx" );
         string hdr_name( name + ".hdr" );
         string log_name( name + ".log" );
         string key_name( name + ".dead_keys.lst" );

         string sav_sql_name( sql_name + ".sav" );
         string sav_dat_name( dat_name + ".sav" );
         string sav_idx_name( idx_name + ".sav" );
         string sav_hdr_name( hdr_name + ".sav" );
         string sav_log_name( log_name + ".sav" );
         string sav_key_name( key_name + ".sav" );

         string backup_sql_name( name + ".backup.sql" );

         vector< string > module_list;
         string module_list_name( name + ".modules.lst" );
         if( rebuild )
         {
            if( !file_exists( module_list_name ) )
               throw runtime_error( "need '" + module_list_name + "' to perform a rebuild" );
            buffer_file_lines( module_list_name, module_list );
         }

         if( !rebuild && !partial && !file_exists( name + ".backup.zip" ) )
            throw runtime_error( "need '" + name + ".backup.zip' to preform a restore" );

         if( !rebuild && !partial )
         {
            string file_names( sav_sql_name );
            file_names += " " + sav_dat_name;
            file_names += " " + sav_idx_name;
            file_names += " " + sav_hdr_name;

            if( !file_exists( log_name ) )
               file_names += " " + sav_log_name;

            if( !file_exists( key_name ) )
               file_names += " " + sav_key_name;

            file_names += " " + backup_sql_name;
            file_names += " " + module_list_name;

            exec_system( "unzip -o -q " + name + ".backup.zip " + file_names );

            buffer_file_lines( module_list_name, module_list );

            file_names.erase( );
            for( size_t i = 0; i < module_list.size( ); i++ )
               file_names += " " + module_list[ i ] + ".init.lst";

            if( !file_names.empty( ) )
               exec_system( "unzip -o -q " + name + ".backup.zip" + file_names );

            file_names.erase( );
            for( size_t i = 0; i < module_list.size( ); i++ )
            {
               string module_init_list( module_list[ i ] + ".init.lst" );
               if( file_exists( module_init_list ) )
               {
                  file_names += " " + module_init_list;

                  vector< string > init_classes;
                  buffer_file_lines( module_init_list, init_classes );

                  for( size_t j = 0; j < init_classes.size( ); j++ )
                     file_names += " " + module_list[ i ] + "_" + init_classes[ j ] + ".csv";
               }
            }

            if( !file_names.empty( ) )
               exec_system( "unzip -o -q " + name + ".backup.zip" + file_names );
         }

         if( !rebuild && !partial && ( !file_exists( sav_sql_name ) || !file_exists( sav_dat_name )
          || !file_exists( sav_idx_name ) || !file_exists( sav_hdr_name ) || !file_exists( backup_sql_name ) ) )
            throw runtime_error( "incomplete or missing file set for backup restore" );

         if( rebuild )
         {
            remove_file( sql_name );
            remove_file( dat_name );
            remove_file( idx_name );
            remove_file( hdr_name );
         }
         else if( !partial )
         {
            copy_file( sav_sql_name, sql_name );
            copy_file( sav_dat_name, dat_name );
            copy_file( sav_idx_name, idx_name );
            copy_file( sav_hdr_name, hdr_name );

            if( !file_exists( log_name ) && file_exists( sav_log_name ) )
               copy_file( sav_log_name, log_name );

            if( !file_exists( key_name ) && file_exists( sav_key_name ) )
               copy_file( sav_key_name, key_name );
         }

         ofstream new_logf;
         string new_log_name( name + ".log.new" );

         bool in_trans = false;
         bool has_locked_tables = false;

         read_log_transformation_info( name + ".ltf", socket_handler.get_transformations( ) );

         session_skip_fk_fetches( quicker );
         session_skip_validation( quicker );
         session_skip_is_constained( quicker );

         int original_trace_flags( get_trace_flags( ) );

         init_storage( name, prefix, handler, true );

         try
         {
            if( !rebuild && !partial )
               restore_storage( handler );

            if( rebuild )
               upgrade_storage( handler );

            bool is_new = false;
            vector< string > module_tx_info;

            if( !module_count( ) )
            {
               if( module_list.empty( ) )
                  throw runtime_error( "no modules were found - DB must be rebuilt" );

               for( size_t i = 0; i < module_list.size( ); i++ )
               {
                  module_load( module_list[ i ], prefix, handler, false, false );
                  module_tx_info.push_back( "[1];module ==> " + module_list[ i ] );
               }

               is_new = true;

               new_logf.open( new_log_name.c_str( ), ios::out );
               if( !new_logf )
                  throw runtime_error( "unable to open log file '" + new_log_name + "' for output." );
            }

            storage_lock_all_tables( );
            has_locked_tables = true;

            string log_file( name + ".log" );

            ifstream inpf( log_file.c_str( ) );
            if( !inpf )
               throw runtime_error( "unable to open transaction log file '" + log_file + "' for input." );

            socket_handler.set_restore_error( "" );
            auto_ptr< restorable< bool > > ap_restoring( socket_handler.set_restoring( ) );

            time_t ts;
            string next;
            size_t line = 0;
            bool verified = false;
            bool is_partial = true;

            ts = time( 0 );

            int trace_flags = 0;
            size_t trace_start = 0;

            if( !trace_info.empty( ) )
            {
               string::size_type pos = trace_info.find( ':' );
               string flags( trace_info.substr( 0, pos ) );

               istringstream isstr( flags );
               isstr >> hex >> trace_flags;

               if( pos != string::npos )
               {
                  string start( trace_info.substr( pos + 1 ) );

                  istringstream isstr( start );
                  isstr >> trace_start;
               }
            }

            // NOTE: As multiple ops can have the same transaction id the test below is >= but we must
            // skip any ops that were part of the last transaction in the storage if it is not new.
            size_t next_tran_id = 0;
            if( !is_new )
               next_tran_id = next_transaction_id( ) + 1;

            size_t tline = 0;
            bool is_first = true;
            bool first_op = true;
            bool performed_init = false;
            while( getline( inpf, next ) )
            {
               // NOTE: In case the log file had been treated as binary during an FTP remove trailing CR.
               if( next.size( ) && next[ next.size( ) - 1 ] == '\r' )
                  next.erase( next.size( ) - 1 );

               if( is_first )
               {
                  is_first = false;

                  // NOTE: UTF-8 text files will often begin with an identifying sequence "EF BB BF" as the
                  // first three characters of the file so if the first byte is "EF" assume UTF-8 and strip.
                  if( next.size( ) >= 3 && next[ 0 ] == ( char )0xef )
                     next.erase( 0, 3 );
               }

               ++line;
               string::size_type pos = next.find( ']' );

               if( trace_flags && line >= trace_start )
               {
                  set_trace_flags( trace_flags );
                  trace_flags = 0;
               }

               // NOTE: As recovery can take a long time issue "progress" messages to ensure that
               // client applications don't end up timing out whilst waiting for the final response.
               if( time( 0 ) - ts >= 5 )
               {
                  ts = time( 0 );

                  // FUTURE: This message should be handled as a server string message.
                  handler.output_progress( "Recovered " + to_string( line ) + " log operations..." );

                  // NOTE: Commit at each progress point to avoid any lengthy commit delays.
                  tline = line;
                  in_trans = false;
                  transaction_commit( );

                  // FUTURE: This message should be handled as a server string message.
                  if( is_condemned_session( ) )
                     throw runtime_error( "Session was terminated by server." );
               }

               if( next.size( ) < 3 || next[ 0 ] != '[' || pos == string::npos )
                  throw runtime_error( "unexpected format found in transaction log line #" + to_string( line ) + " ==> " + next );

               size_t tran_id = from_string< size_t >( next.substr( 1, pos - 1 ) );
               string tran_info = next.substr( pos + 1 );

               if( !in_trans && tran_id >= 5 && line >= tline )
               {
                  in_trans = true;
                  transaction_start( );
               }

               // FUTURE: This message should be handled as a server string message.
               if( tran_id == 0 && !is_new && tran_info != storage_identity( ) )
                  throw runtime_error( "DB identity mismatch with transaction log." );

               if( tran_id == 0 )
               {
                  if( is_new && !verified )
                  {
                     new_logf << "[0]" << storage_identity( ) << '\n';
                     for( size_t i = 0; i < module_tx_info.size( ); i++ )
                        new_logf << module_tx_info[ i ] << '\n';
                  }
                  verified = true;
                  continue;
               }
               else if( !verified )
                  throw runtime_error( "unexpected DB identity missing from transaction log" );

               if( tran_id == 1 )
                  is_partial = false;
               else if( tran_id >= 2 && is_new && is_partial )
                  throw runtime_error( "cannot restore from empty DB with a partial transaction log" );

               if( tran_id > 1 && is_new && !performed_init )
               {
                  if( in_trans )
                     transaction_commit( );

                  tline = line;
                  in_trans = false;
                  performed_init = true;

                  // NOTE: Transaction id #2 is used for initial data records.
                  set_transaction_id( 2 );
                  record_initialiser init( new_logf );

                  for( size_t i = 0; i < module_list.size( ); i++ )
                  {
                     string module_init_list( module_list[ i ] + ".init.lst" );
                     if( file_exists( module_init_list ) )
                     {
                        vector< string > init_classes;
                        buffer_file_lines( module_init_list, init_classes );

                        for( size_t j = 0; j < init_classes.size( ); j++ )
                        {
                           string bulk_init_cmd( "perform_bulk_ops init " );

                           bulk_init_cmd += date_time::standard( ).as_string( false, false ) + " ";
                           bulk_init_cmd += module_list[ i ] + " " + init_classes[ j ] + " ";
                           bulk_init_cmd += module_list[ i ] + "_" + init_classes[ j ] + ".csv";

                           handler.execute_command( bulk_init_cmd );

                           string init_output( module_list[ i ] + "_" + init_classes[ j ] + ".csv: " );
                           init_output += buffer_file( module_list[ i ] + "_" + init_classes[ j ] + ".log" );

                           handler.output_progress( init_output );
                        }
                     }
                  }
               }

               // NOTE: Any operations whose transaction id is less than five is skipped during a restore.
               if( !tran_info.empty( ) && tran_id >= 5 )
               {
                  if( first_op )
                  {
                     first_op = false;

                     if( is_new )
                        next_tran_id = next_transaction_id( );
                  }

                  if( tran_info[ 0 ] != ';' && ( is_new || tran_id >= next_tran_id ) )
                  {
                     next_tran_id = set_transaction_id( tran_id );
                     handler.execute_command( tran_info );
                  }

                  if( is_new )
                     new_logf << '[' << next_tran_id << ']' << tran_info << '\n';

                  if( !socket_handler.get_restore_error( ).empty( ) )
                     throw runtime_error( "unexpected error: " + socket_handler.get_restore_error( )
                      + "\nprocessing transaction log line #" + to_string( line ) + " with info ==> " + tran_info );
               }
            }

            if( is_new )
            {
               new_logf.flush( );
               if( !new_logf.good( ) )
                  throw runtime_error( "unexpected bad log stream for '" + new_log_name + "'" );

               inpf.close( );
               new_logf.close( );

               copy_file( new_log_name, log_file );
               remove_file( new_log_name );
            }

            if( in_trans )
               transaction_commit( );

            // NOTE: Ensure that reserved transaction id's cannot be used later.
            if( next_transaction_id( ) < 4 )
               set_transaction_id( 4 );

            storage_unlock_all_tables( );

            session_skip_fk_fetches( false );
            session_skip_validation( false );
            session_skip_is_constained( false );

            set_trace_flags( original_trace_flags );

            if( !rebuild && !partial )
            {
               remove_file( sav_sql_name );
               remove_file( sav_dat_name );
               remove_file( sav_idx_name );
               remove_file( sav_hdr_name );

               if( file_exists( sav_log_name ) )
                  remove_file( sav_log_name );

               if( file_exists( sav_key_name ) )
                  remove_file( sav_key_name );

               remove_file( backup_sql_name );
            }

            term_storage( handler );
            socket_handler.get_transformations( ).clear( );
         }
         catch( ... )
         {
            if( in_trans )
               transaction_rollback( );

            if( has_locked_tables )
               storage_unlock_all_tables( );

            session_skip_fk_fetches( false );
            session_skip_validation( false );
            session_skip_is_constained( false );
            
            set_trace_flags( original_trace_flags );

            term_storage( handler );
            socket_handler.get_transformations( ).clear( );
            throw;
         }
      }
      else if( command == c_cmd_ciyam_session_storage_web_root )
      {
         bool expand( has_parm_val( parameters, c_cmd_parm_ciyam_session_storage_web_root_expand ) );

         string new_root( get_parm_val( parameters, c_cmd_parm_ciyam_session_storage_web_root_new_root ) );

         if( !new_root.empty( ) )
            storage_web_root( new_root );

         response = storage_web_root( expand );
      }
      else if( command == c_cmd_ciyam_session_storage_log_slice )
      {
         string name( get_parm_val( parameters, c_cmd_parm_ciyam_session_storage_log_slice_name ) );
         string modules( get_parm_val( parameters, c_cmd_parm_ciyam_session_storage_log_slice_modules ) );

         vector< string > module_list;
         if( !modules.empty( ) )
            split( modules, module_list );

         slice_storage_log( handler, name, module_list );
      }
      else if( command == c_cmd_ciyam_session_storage_log_splice )
      {
         string name( get_parm_val( parameters, c_cmd_parm_ciyam_session_storage_log_splice_name ) );
         string modules( get_parm_val( parameters, c_cmd_parm_ciyam_session_storage_log_splice_modules ) );

         vector< string > module_list;
         if( !modules.empty( ) )
            split( modules, module_list );

         splice_storage_log( handler, name, module_list );
      }
      else if( command == c_cmd_ciyam_session_storage_dump_locks )
      {
         dump_storage_locks( osstr );
         output_response_lines( socket, osstr.str( ) );
      }
      else if( command == c_cmd_ciyam_session_storage_trans_start )
         transaction_start( );
      else if( command == c_cmd_ciyam_session_storage_trans_commit )
         transaction_commit( );
      else if( command == c_cmd_ciyam_session_storage_trans_rollback )
         transaction_rollback( );
      else if( command == c_cmd_ciyam_session_system_variable )
      {
         string var_name( get_parm_val( parameters, c_cmd_parm_ciyam_session_system_variable_var_name ) );
         bool has_new_val( has_parm_val( parameters, c_cmd_parm_ciyam_session_system_variable_new_value ) );
         string new_value( get_parm_val( parameters, c_cmd_parm_ciyam_session_system_variable_new_value ) );

         if( has_new_val )
            set_system_variable( var_name, new_value );

         response = get_system_variable( var_name );
      }
      else if( command == c_cmd_ciyam_session_trace )
      {
         string flags( get_parm_val( parameters, c_cmd_parm_ciyam_session_trace_flags ) );

         if( !flags.empty( ) )
         {
            istringstream isstr( flags );

            int trace_flags;
            isstr >> hex >> trace_flags;

            set_trace_flags( trace_flags );
         }

         ostringstream osstr;
         osstr << hex << get_trace_flags( );

         response = osstr.str( );
      }
      else if( command == c_cmd_ciyam_session_trace_flags )
      {
         vector< string > trace_flag_names;

         list_trace_flags( trace_flag_names );

         int flag = 1;
         ostringstream osstr;
         for( size_t i = 0; i < trace_flag_names.size( ); i++ )
         {
            osstr << hex << setw( 4 ) << setfill( '0' ) << flag << '=' << trace_flag_names[ i ] << '\n';
            flag *= 2;
         }

         response = osstr.str( );
      }
      else if( command == c_cmd_ciyam_session_encrypt )
      {
         bool no_ssl( has_parm_val( parameters, c_cmd_parm_ciyam_session_encrypt_no_ssl ) );
         string password( get_parm_val( parameters, c_cmd_parm_ciyam_session_encrypt_password ) );

         response = encrypt_password( password, no_ssl );
      }
      else if( command == c_cmd_ciyam_session_sendmail )
      {
         string to( get_parm_val( parameters, c_cmd_parm_ciyam_session_sendmail_to ) );
         string subject( get_parm_val( parameters, c_cmd_parm_ciyam_session_sendmail_subject ) );
         string message( get_parm_val( parameters, c_cmd_parm_ciyam_session_sendmail_message ) );
         string tz_abbr( get_parm_val( parameters, c_cmd_parm_ciyam_session_sendmail_tz_abbr ) );
         string file_names( get_parm_val( parameters, c_cmd_parm_ciyam_session_sendmail_file_names ) );
         string html_source( get_parm_val( parameters, c_cmd_parm_ciyam_session_sendmail_html_source ) );
         string image_names( get_parm_val( parameters, c_cmd_parm_ciyam_session_sendmail_image_names ) );
         string image_prefix( get_parm_val( parameters, c_cmd_parm_ciyam_session_sendmail_image_prefix ) );

         if( tz_abbr.empty( ) )
            tz_abbr = get_timezone( );

         vector< string > all_file_names;
         if( !file_names.empty( ) )
            split( file_names, all_file_names );

         vector< string > all_image_names;
         if( !image_names.empty( ) )
            split( image_names, all_image_names );

         if( !message.empty( ) && message[ 0 ] == '@' )
         {
            if( file_exists( message.substr( 1 ) ) )
               message = buffer_file( message.substr( 1 ) );
         }

         send_email_message( to, subject, message,
          html_source, 0, &all_file_names, &tz_abbr, &all_image_names, &image_prefix );
      }
      else if( command == c_cmd_ciyam_session_schedule )
      {
         output_schedule( osstr );
         output_response_lines( socket, osstr.str( ) );
      }
      else if( command == c_cmd_ciyam_session_timezone )
         response = get_timezone( );
      else if( command == c_cmd_ciyam_session_checkmail )
      {
         string headers( get_parm_val( parameters, c_cmd_parm_ciyam_session_checkmail_headers ) );
         bool create_script( has_parm_val( parameters, c_cmd_parm_ciyam_session_checkmail_create_script ) );

         vector< string > email_headers;
         if( !headers.empty( ) )
            split( headers, email_headers );

         handler.output_progress( ";checkmail..." );
         response = check_email_headers( email_headers, create_script );
      }
      else if( command == c_cmd_ciyam_session_runscript )
      {
         string script_name( get_parm_val( parameters, c_cmd_parm_ciyam_session_runscript_script_name ) );

         run_script( script_name );
      }
      else if( command == c_cmd_ciyam_session_utc_offset )
      {
         string tz_abbr( get_parm_val( parameters, c_cmd_parm_ciyam_session_utc_offset_tz_abbr ) );
         string local_time( get_parm_val( parameters, c_cmd_parm_ciyam_session_utc_offset_local_time ) );

         float offset;
         get_tz_info( date_time( local_time ), tz_abbr, offset );

         response = tz_abbr + " " + to_string( offset );
      }
      else if( command == c_cmd_ciyam_session_wait )
      {
         string uid( get_parm_val( parameters, c_cmd_parm_ciyam_session_wait_uid ) );
         string milliseconds( get_parm_val( parameters, c_cmd_parm_ciyam_session_wait_milliseconds ) );

         set_uid( uid );
         msleep( atoi( milliseconds.c_str( ) ) );
      }
      else if( command == c_cmd_ciyam_session_test )
      {
         // Dummy command for prototyping.
      }
      else if( command == c_cmd_ciyam_session_quit )
      {
         if( !is_captured_session( ) )
            handler.set_finished( );
         socket.close( );
         return;
      }
   }
   catch( exception& x )
   {
      TRACE_LOG( TRACE_ANYTHING, string( "session error: " ) + x.what( ) );

      if( socket_handler.is_restoring( ) )
         socket_handler.set_restore_error( x.what( ) );
      else
      {
         send_okay_response = false;
         transaction_log_command( "" );
         response = string( c_response_error_prefix ) + x.what( );
      }
   }
   catch( ... )
   {
      TRACE_LOG( TRACE_ANYTHING, "session error: unexpected unknown exception caught" );

      if( socket_handler.is_restoring( ) )
         socket_handler.set_restore_error( "unexpected unknown exception caught" );
      else
      {
         send_okay_response = false;
         transaction_log_command( "" );
         response = string( c_response_error_prefix ) + "unexpected unknown exception caught";
      }
   }

   if( !socket_handler.is_restoring( ) )
      handler.issue_command_reponse( response, !send_okay_response );
}

class socket_command_processor : public command_processor
{
   public:
   socket_command_processor( tcp_socket& socket, command_handler& handler )
    : command_processor( handler ),
    socket( socket ),
    handler( handler )
   {
   }

   private:
   tcp_socket& socket;
   command_handler& handler;

   bool is_still_processing( ) { return is_captured_session( ) || socket.okay( ); }

   string get_cmd_and_args( );

   void output_command_usage( const string& wildcard_match_expr ) const;
};

string socket_command_processor::get_cmd_and_args( )
{
   string request;

   while( true )
   {
      if( socket.read_line( request, c_request_timeout ) <= 0 )
      {
         if( g_server_shutdown || is_condemned_session( ) || ( !is_captured_session( ) && !socket.had_timeout( ) ) )
         {
            // NOTE: If the server is being shutdown or this session has been condemned
            // or its socket has died (whilst not captured) then force a "quit" to occur.
            request = "quit";
            break;
         }

         // NOTE: If the socket is dead then the "read_line" will return instantly so for
         // a "captured" session manually perform a timeout sleep so CPU is not overused.
         if( is_captured_session( ) && !socket.had_timeout( ) )
         {
            msleep( c_request_timeout );
            continue;
         }
      }
      else
         break;
   }

#ifdef DEBUG
   cout << "request = '" << request << "'" << endl;
#endif
   return request;
}

void socket_command_processor::output_command_usage( const string& wildcard_match_expr ) const
{
#ifdef DEBUG
   cout << "<processing usage request>" << endl;
#endif

   if( !socket.set_delay( ) )
      issue_warning( "socket set_delay failure" );

   socket.write_line( "commands:" );
   socket.write_line( "=========" );

   socket.write_line( get_usage_for_commands( wildcard_match_expr ) );

   if( !socket.set_no_delay( ) )
      issue_warning( "socket set_no_delay failure" );
   socket.write_line( c_response_okay );
}

}

ciyam_session::ciyam_session( auto_ptr< tcp_socket >& ap_socket )
 :
 ap_socket( ap_socket )
{
   if( !( *this->ap_socket ) )
      throw runtime_error( "invalid socket..." );

   increment_session_count( );
}

ciyam_session::~ciyam_session( )
{
   decrement_session_count( );
}

void ciyam_session::on_start( )
{
#ifdef DEBUG
   cout << "started session..." << endl;
#endif
   try
   {
      socket_command_handler cmd_handler( *ap_socket );

      cmd_handler.add_commands( 0,
       ciyam_session_command_functor_factory, ARRAY_PTR_AND_SIZE( ciyam_session_command_definitions ) );

      cmd_handler.set_sess_id( init_session( cmd_handler ) );

      ap_socket->write_line( string( c_protocol_version ) + '\n' + string( c_response_okay ) );

      socket_command_processor processor( *ap_socket, cmd_handler );
      processor.process_commands( );

      term_storage( cmd_handler );
      module_unload_all( cmd_handler );

      term_session( );
   }
   catch( exception& x )
   {
      issue_error( x.what( ) );

      ap_socket->write_line( string( c_response_error_prefix ) + x.what( ) );
      ap_socket->close( );
   }
   catch( ... )
   {
      issue_error( "unexpected exception occurred" );

      ap_socket->write_line( string( c_response_error_prefix ) + "unexpected exception occurred" );
      ap_socket->close( );
   }

#ifdef DEBUG
   cout << "finished session..." << endl;
#endif
   delete this;
}

void ciyam_session::increment_session_count( )
{
   guard g( g_mutex );
   ++g_active_sessions;
}

void ciyam_session::decrement_session_count( )
{
   guard g( g_mutex );
   --g_active_sessions;
}

