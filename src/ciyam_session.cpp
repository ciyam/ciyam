// Copyright (c) 2006-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <ctime>
#  include <cstring>
#  include <csignal>
#  include <cstdlib>
#  include <set>
#  include <map>
#  include <deque>
#  include <memory>
#  include <vector>
#  include <string>
#  include <sstream>
#  include <fstream>
#  include <iomanip>
#  include <iostream>
#  include <algorithm>
#  include <stdexcept>
#endif

#define CIYAM_BASE_IMPL

#include "ciyam_session.h"

#include "ods.h"
#include "sio.h"
#include "regex.h"
#include "base64.h"
#include "config.h"
#include "format.h"
#ifdef HPDF_SUPPORT
#  include "pdf_gen.h"
#endif
#include "sockets.h"
#include "threads.h"
#include "progress.h"
#include "pointers.h"
#include "utilities.h"
#include "date_time.h"
#include "ciyam_base.h"
#include "class_base.h"
#include "hash_chain.h"
#include "auto_script.h"
#include "ciyam_files.h"
#include "crypt_stream.h"
#include "peer_session.h"
#include "ciyam_strings.h"
#include "ciyam_base_ops.h"
#include "ciyam_channels.h"
#include "ciyam_notifier.h"
#include "ciyam_packages.h"
#include "command_parser.h"
#include "ciyam_variables.h"
#include "command_handler.h"
#include "ciyam_core_files.h"
#include "module_interface.h"
#include "command_processor.h"
#include "module_management.h"

//#define DEBUG

using namespace std;

extern size_t g_active_sessions;
extern volatile sig_atomic_t g_server_shutdown;

namespace
{

mutex g_mutex;

#include "ciyam_constants.h"

#include "ciyam_session.cmh"

#include "trace_progress.cpp"

const size_t c_request_timeout = 500; // i.e. 1/2 sec
const size_t c_udp_wait_timeout = 50; // i.e. 1/20 sec
const size_t c_listen_wait_timeout = 50; // i.e. 1/20 sec

const size_t c_udp_wait_repeats = 10;
const size_t c_listen_wait_repeats = 20;

const int c_pdf_default_limit = 10000;

const size_t c_response_reserve_size = 1024;

const size_t c_max_key_append_chars = 7;

const char* const c_passtotp_prefix = "passtotp.";

const char* const c_unexpected_unknown_exception = "unexpected unknown exception caught";

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
const char* const c_log_transformation_op_change_field_value = "change_field_value";
const char* const c_log_transformation_op_map_first_arg_field_ids = "map_first_arg_field_ids";
const char* const c_log_transformation_op_instance_change_field_value = "instance_change_field_value";

inline void issue_error( const string& message )
{
#ifdef DEBUG
   cerr << "session error: " << message << endl;
#else
   TRACE_LOG( TRACE_MINIMAL, string( "session error: " ) + message );
#endif
}

inline void issue_warning( const string& message )
{
#ifdef DEBUG
   cerr << "session warning: " << message << endl;
#else
   TRACE_LOG( TRACE_INITIAL | TRACE_SESSION, string( "session warning: " ) + message );
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

inline string convert_local_to_utc( const string& local, const string& tz_name )
{
   string s;

   if( !local.empty( ) )
   {
      // NOTE: If a date_time string starts with 'U' then it is considered as already being UTC.
      if( local[ 0 ] == 'U' )
         s = local.substr( 1 );
      else
         s = local_to_utc( date_time( local ), tz_name ).as_string( );
   }

   return s;
}

void check_key_has_suffix( const string& key, const string& suffix )
{
   if( !suffix.empty( ) )
   {
      bool has_suffix = false;

      string::size_type pos = key.find( suffix );

      if( pos != string::npos )
      {
         string last_suffix_key_var_name(
          get_special_var_name( e_special_var_last_suffixed_key ) );

         if( pos + suffix.length( ) == key.length( ) )
         {
            has_suffix = true;
            set_session_variable( last_suffix_key_var_name, key );
         }
         else
         {
            string last_suffixed_key( get_raw_session_variable( last_suffix_key_var_name ) );

            if( !last_suffixed_key.empty( ) && ( key.find( last_suffixed_key ) == 0 ) )
               has_suffix = true;
         }
      }

      if( !has_suffix )
         throw runtime_error( "key suffix '" + suffix + "' not found in key value '" + key + "'" );
   }
}

void check_not_possible_protocol_response( const string& value )
{
   string response( c_response_okay );

   if( !value.empty( ) && ( value[ 0 ] == response[ 0 ] ) )
      throw runtime_error( "invalid value '" + value + "' (could be confused with a protocol response)" );
}

void set_script_error_if_applicable( const string& error_message )
{
   // NOTE: The "run_script" function contains a detailed note about this.
   string args_file( get_raw_session_variable(
    get_special_var_name( e_special_var_args_file ) ) );

   if( !args_file.empty( ) )
   {
      string args_file_value = get_raw_system_variable( args_file );

      if( args_file_value == string( c_true_value ) )
         set_system_variable( args_file, error_message );
   }
}

void check_instance_op_permission( const string& module,
 size_t handle, string permission_info, bool is_field_perm = false )
{
   string uid( get_uid( ) );

   string perm;

   if( !is_field_perm )
      perm = instance_get_variable( handle, "", get_special_var_name( e_special_var_permission ) );

   if( !permission_info.empty( ) && ( !perm.empty( ) || ( permission_info != string( c_anyone ) ) ) )
   {
      string permission;

      string::size_type pos = permission_info.find( "=!" );

      if( pos != string::npos )
      {
         permission = permission_info.substr( pos + 2 );
         permission_info.erase( pos );
      }

      // NOTE: If a "permission" field exists for the record then it will override the default permission.
      if( !perm.empty( ) )
         permission = perm;

      bool okay = false;

      if( ( uid == c_admin )
       && ( ( permission_info == c_admin_only ) || ( permission_info == c_admin_owner ) ) )
         okay = true;

      if( !okay && ( ( permission_info == c_owner_only ) || ( permission_info == c_admin_owner ) ) )
      {
         string owner( get_instance_owner( handle, "" ) );

         if( uid == owner )
            okay = true;
      }

      if( !okay && !permission.empty( ) )
      {
         string uclass( get_raw_session_variable( "@" + module + c_user_class_suffix ) );

         if( !uclass.empty( ) )
         {
            size_t uhandle = create_object_instance( module, uclass, 0, false );

            instance_fetch_rc rc;
            instance_fetch( uhandle, "", uid, &rc );

            if( rc == e_instance_fetch_rc_okay )
            {
               string perms( instance_get_variable( uhandle, "", get_special_var_name( e_special_var_permissions ) ) );

               if( !perms.empty( ) )
               {
                  set< string > all_perms;
                  split( perms, all_perms );

                  if( all_perms.count( permission ) )
                     okay = true;
               }
            }

            destroy_object_instance( uhandle );
         }
      }

      if( !okay )
         throw runtime_error( GS( c_str_permission_denied ) );
   }
}

void check_instance_field_permission( const string& module,
 size_t handle, bool is_create, const string& scope_and_perm_info )
{
   string::size_type pos = scope_and_perm_info.find( ',' );
   if( pos == string::npos )
      throw runtime_error( "unexpected scope_and_perm_info '" + scope_and_perm_info + "'" );

   string scope( scope_and_perm_info.substr( 0, pos ) );
   string perm_info( scope_and_perm_info.substr( pos + 1 ) );

   bool okay = true;

   if( scope == string( c_create ) && !is_create )
      okay = false;
   else if( scope == string( c_update ) && is_create )
      okay = false;

   if( !okay )
      throw runtime_error( GS( c_str_permission_denied ) );

   if( !perm_info.empty( ) )
      check_instance_op_permission( module, handle, perm_info, true );
}

void append_datachain_as_variable_if_found( size_t handle, string& field_values_to_log )
{
   string datachain_var_name( get_special_var_name( e_special_var_datachain ) );

   string datachain( get_raw_session_variable( datachain_var_name ) );

   if( !datachain.empty( ) )
   {
      if( !field_values_to_log.empty( ) )
         field_values_to_log += ',';

      field_values_to_log += datachain_var_name + '=' + datachain;

      instance_set_variable( handle, "", datachain_var_name, datachain );
   }
}

string get_shortened_field_id( const string& module, const string& mclass, const string& field_id )
{
   // NOTE: If the module starts with a number then is assuming that it is Meta
   // (which does not allow field shortening).
   if( !module.empty( ) && ( module[ 0 ] >= '0' ) && ( module[ 0 ] <= '9' ) )
      return field_id;

   string::size_type pos = field_id.find( mclass );

   if( pos != string::npos )
      pos += mclass.length( );
   else
   {
      pos = field_id.find( module );

      if( pos != string::npos )
         pos += module.length( );
   }

   if( pos == string::npos )
      return field_id;
   else
      return field_id.substr( pos );
}

void replace_field_values_to_log( string& next_command,
 const string& field_values_to_log, const char* p_prefix = 0, string::size_type* p_rpos = 0 )
{
   if( p_prefix )
   {
      string::size_type pos = next_command.find( p_prefix );

      if( pos != string::npos )
      {
         char eos = ' ';

         if( pos > 0 && next_command[ pos - 1 ] == '"' )
            eos = '"';

         string::size_type rpos = find_end_of_escaped_sequence( next_command, pos + 1, eos );

         if( rpos == string::npos )
            throw runtime_error( "invalid next_command: " + next_command );

         if( p_rpos )
            *p_rpos = pos + strlen( p_prefix ) + field_values_to_log.length( );

         next_command = next_command.substr( 0, pos + strlen( p_prefix ) )
          + field_values_to_log + next_command.substr( rpos );
      }
   }
   else
   {
      string::size_type pos = next_command.find( '"' );

      if( pos != string::npos )
      {
         string::size_type rpos = next_command.rfind( '"' );

         if( rpos == string::npos )
            throw runtime_error( "invalid next_command: " + next_command );

         bool okay = false;

         string::size_type npos = next_command.find( '"', pos + 1 );

         // NOTE: The first pair of quotes might be around the key info so skip if that is the case.
         for( size_t i = pos + 1; i != npos; i++ )
         {
            if( next_command[ i ] == '=' )
            {
               okay = true;
               break;
            }
         }

         if( !okay )
            pos = next_command.find( '"', npos + 1 );

         if( pos != string::npos )
            next_command = next_command.substr( 0, pos + 1 )
             + field_values_to_log + next_command.substr( rpos );
      }
   }
}

void replace_module_and_class_to_log( string& next_command,
 const string& module_and_class, const string& module, const string& mclass )
{
   string non_prefixed_class( mclass );

   if( mclass.find( module ) == 0 )
      non_prefixed_class.erase( 0, module.length( ) );

   string new_module_and_class( module + ' ' + non_prefixed_class );

   string::size_type pos = next_command.find( module_and_class );
   if( pos != string::npos )
   {
      next_command.erase( pos, module_and_class.size( ) );
      next_command.insert( pos, new_module_and_class );
   }
}

void replace_method_with_shortened_id( string& next_command, const string& method,
 string::size_type rpos, const string& module, const string& mclass, const string& method_id )
{
   string::size_type pos = next_command.find( method, rpos );
   if( pos == string::npos )
      throw runtime_error( "unexpected missing method '" + method + "' in '" + next_command + "'" );

   next_command.erase( pos, method.length( ) );

   string shortened_method_id( method_id );
   if( shortened_method_id.find( mclass ) == 0 )
      shortened_method_id.erase( 0, mclass.length( ) );
   else if( shortened_method_id.find( module ) == 0 )
      shortened_method_id.erase( 0, module.length( ) );

   next_command.insert( pos, shortened_method_id );
}

string& remove_uid_extra_from_log_command( string& log_command )
{
   string::size_type pos = log_command.find( ' ' );
   if( pos != string::npos )
   {
      string::size_type npos;
      if( log_command.size( ) > pos && log_command[ pos + 1 ] == '"' )
      {
         npos = log_command.find( '"', ++pos + 1 );
         if( npos != string::npos )
            ++npos;
      }
      else
         npos = log_command.find( ' ', pos + 1 );

      if( npos != string::npos )
      {
         string uid_info( log_command.substr( pos + 1, npos - pos - 1 ) );

         string::size_type xpos = uid_info.find_first_of( "!:;@#" );
         if( xpos != string::npos )
         {
            string prefix = log_command.substr( 0, pos );
            if( prefix[ prefix.size( ) - 1 ] != ' ' )
               prefix += ' ';

            log_command = prefix + uid_info.substr( 0, xpos ) + log_command.substr( npos );
         }
      }
   }

   return log_command;
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
   string method( id_or_name );
   string method_name( id_or_name );

   if( p_transformations && !p_transformations->empty( ) )
   {
      string ltf_key( c_log_transformation_scope_update_execute_only );
      ltf_key += " " + module + " " + mclass + " " + string( c_log_transformation_op_map_method_id ) + " " + id_or_name;

      if( p_transformations->count( ltf_key ) )
         method = p_transformations->find( ltf_key )->second;
   }

   const procedure_info_container& procedure_info( get_procedure_info_for_module_class( module, mclass ) );

   if( procedure_info.count( method ) )
   {
      method_name = procedure_info.find( method )->second.name;

      if( p_method_id )
         *p_method_id = method;
   }
   else if( procedure_info.count( mclass + method ) )
   {
      method_name = procedure_info.find( mclass + method )->second.name;

      if( p_method_id )
         *p_method_id = mclass + method;
   }
   else if( procedure_info.count( module + mclass + method ) )
   {
      method_name = procedure_info.find( module + mclass + method )->second.name;

      if( p_method_id )
         *p_method_id = module + mclass + method;
   }
   else if( p_method_id )
   {
      for( procedure_info_const_iterator pici = procedure_info.begin( ); pici != procedure_info.end( ); ++pici )
      {
         if( pici->second.name == method )
         {
            *p_method_id = pici->first;
            break;
         }
      }
   }

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

#ifdef HPDF_SUPPORT
void add_pdf_variables( size_t handle,
 const string& parent_context, const vector< string >& field_list,
 vector< summary_info >& summaries, map< string, string >& pdf_variables,
 const string& tz_name, bool is_single_record, size_t row_num, bool add_values = true )
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

         bool is_encrypted = false;
         bool is_transient = false;

         string type_name( get_field_type_name( handle, "", field, &is_encrypted, &is_transient ) );

         if( !value.empty( ) && !tz_name.empty( ) )
         {
            if( type_name == "date_time" || type_name == "tdatetime" )
               value = utc_to_local( date_time( value ), tz_name ).as_string( );
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
         if( !field_context.empty( ) && ( all_contexts[ field_context ] > 2 ) )
            value += " " + get_field_display_name( handle, context, field );

         pdf_variables.insert( make_pair( name, value ) );

         string uom_symbol( get_field_uom_symbol( handle, context, field ) );
         if( !uom_symbol.empty( ) )
            pdf_variables.insert( make_pair( name + "_(uom)", "(" + uom_symbol + ")" ) );

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
#endif

void parse_field_values( const string& module,
 const string& class_id, const string& field_values,
 map< string, string >& field_value_items, const map< string, string >* p_transformations = 0 )
{
   vector< string > field_value_pairs;

   if( !field_values.empty( ) )
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

      TRACE_LOG( TRACE_DETAILS | TRACE_OBJECTS, "field name (start) '" + field_value_pairs[ i ].substr( 0, pos ) + "'" );
      TRACE_LOG( TRACE_DETAILS | TRACE_OBJECTS, "field name (trans) '" + field_id_or_name + "'" );

      field_id_or_name = get_field_id_for_id_or_name( module, class_id, field_id_or_name );

      TRACE_LOG( TRACE_DETAILS | TRACE_OBJECTS, "field id (checked) '" + field_id_or_name + "'" );
      TRACE_LOG( TRACE_DETAILS | TRACE_OBJECTS, "field value (data) '" + field_value_pairs[ i ].substr( pos + 1 ) + "'" );

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
   progress* p_progress = 0;
   trace_progress progress( TRACE_VERBOSE | TRACE_SOCKETS );

   if( get_trace_flags( ) & ( TRACE_VERBOSE | TRACE_SOCKETS ) )
      p_progress = &progress;

   vector< string > lines;
   split( response, lines, '\n' );

   for( size_t i = 0; i < lines.size( ); i++ )
      socket.write_line( lines[ i ], c_request_timeout, p_progress );
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
   if( exists_file( file_name ) )
   {
      ifstream inpf( file_name.c_str( ) );

      string next_line;
      while( getline( inpf, next_line ) )
      {
         remove_trailing_cr_from_text_file_line( next_line );

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
                  pos = next_line.find_last_of( '=' );
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
#ifdef SSL_SUPPORT
   socket_command_handler( ssl_socket& socket )
#else
   socket_command_handler( tcp_socket& socket )
#endif
    :
    socket( socket ),
    lock_expires( 0 ),
    restoring( false )
   {
      bool is_encrypted = false;
      bool has_system_id = has_identity( &is_encrypted );

      if( is_encrypted || !has_system_id )
         locked_identity = true;
      else
         locked_identity = false;

      locked_rpc = !get_rpc_password( ).empty( );
   }

#ifdef SSL_SUPPORT
   ssl_socket& get_socket( ) { return socket; }
#else
   tcp_socket& get_socket( ) { return socket; }
#endif

   const string& get_next_command( ) { return next_command; }

   bool is_locked( ) const { return ( locked_rpc || locked_identity ); }

   bool is_restoring( ) const { return restoring; }

   void unlock_rpc( ) { locked_rpc = false; }
   void unlock_identity( ) { locked_identity = false; }

   void set_lock_expires( unsigned int seconds )
   {
      lock_expires = unix_time( ) + seconds;
   }

   void check_lock_expiry( )
   {
      if( lock_expires && unix_time( ) > lock_expires )
      {
         locked_rpc = true;
         lock_expires = 0;
      }
   }

   bool has_restricted_commands( ) const
   {
      return !restricted_commands.empty( );
   }

   void check_restricted_command( const string& cmd ) const
   {
      if( !restricted_commands.count( cmd ) )
         throw runtime_error( "command '" + cmd + "' is not currently permitted" );
   }

   void set_restricted_commands( const string& key, const string& cmds )
   {
      if( key.empty( ) )
         throw runtime_error( "restriction key value required" );

      if( !restricted_key.empty( ) && ( key != restricted_key ) )
         throw runtime_error( "incorrect restriction key value" );

      if( restricted_key.empty( ) )
         restricted_key = key;

      if( cmds.empty( ) )
         restricted_commands.clear( );
      else
      {
         split( cmds, restricted_commands );

         // NOTE: Always allow the restrict command itself (to unlock) and
         // also ensure that "session_terminate" will always be permitted.
         restricted_commands.insert( c_cmd_ciyam_session_session_restrict );
         restricted_commands.insert( c_cmd_ciyam_session_session_terminate );
      }
   }

   map< string, string >& get_transformations( ) { return transformations; }

   void output_progress( const string& message, unsigned long num = 0, unsigned long total = 0 )
   {
      progress* p_progress = 0;
      trace_progress progress( TRACE_VERBOSE | TRACE_SOCKETS );

      if( get_trace_flags( ) & ( TRACE_VERBOSE | TRACE_SOCKETS ) )
         p_progress = &progress;

      string extra;

      if( num || total )
      {
         extra += to_string( num );

         if( total )
            extra += '/' + to_string( total );
      }

      set_session_progress_message( message + extra );

      socket.write_line( string( c_response_message_prefix ) + message + extra, c_request_timeout, p_progress );
   }

   const string& get_restore_error( ) const { return restore_error; }
   void set_restore_error( const string& new_error ) { restore_error = new_error; }

   auto_ptr< restorable< bool > > set_restoring( ) { return auto_ptr< restorable< bool > >( new restorable< bool >( restoring, true ) ); }

   private:
   void preprocess_command_and_args( string& str, const string& cmd_and_args, bool /*skip_command_usage*/ );

   void postprocess_command_and_args( const string& cmd_and_args );

   void handle_unknown_command( const string& command, const string& cmd_and_args )
   {
      ( void )cmd_and_args;

      socket.write_line( string( c_response_error_prefix ) + "unknown command '" + command + "'", c_request_timeout );
   }

   void handle_invalid_command( const command_parser& parser, const string& cmd_and_args )
   {
      socket.write_line( string( c_response_error_prefix ) + "invalid command usage '" + cmd_and_args + "'", c_request_timeout );
   }

   void handle_command_response( const string& response, bool is_special );

#ifdef SSL_SUPPORT
   ssl_socket& socket;
#else
   tcp_socket& socket;
#endif

   bool locked;
   bool restoring;

   bool locked_rpc;
   bool locked_identity;

   int64_t lock_expires;

   string next_command;
   string restore_error;

   string restricted_key;
   set< string > restricted_commands;

   map< string, string > transformations;
};

void socket_command_handler::preprocess_command_and_args( string& str, const string& cmd_and_args, bool /*skip_command_usage*/ )
{
   str = cmd_and_args;

   if( !str.empty( ) )
   {
      string::size_type pos = string::npos;

      // NOTE: In order to prevent accidental leakage of sensitive information
      // (such as key entropy or passwords) a command can be prefixed with '/'
      // to truncate the log output immediately after the command name.
      if( ( str.length( ) > 1 ) && ( str[ 0 ] == '/' ) )
      {
         str.erase( 0, 1 );

         pos = cmd_and_args.find( ' ' );
      }

      TRACE_LOG( TRACE_DETAILS | TRACE_SESSION, cmd_and_args.substr( 0, pos ) );

      if( str[ 0 ] == '?' || str.find( "help" ) == 0 )
      {
         string::size_type pos = str.find( ' ' );

         string wildcard_match_expr;

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
}

void socket_command_handler::postprocess_command_and_args( const string& cmd_and_args )
{
   if( has_finished( ) )
      TRACE_LOG( TRACE_INITIAL | TRACE_SESSION, "finished session" );
}

void socket_command_handler::handle_command_response( const string& response, bool is_special )
{
   progress* p_progress = 0;
   trace_progress progress( TRACE_VERBOSE | TRACE_SOCKETS );

   if( get_trace_flags( ) & ( TRACE_VERBOSE | TRACE_SOCKETS ) )
      p_progress = &progress;

   if( !is_restoring( ) )
      set_slowest_if_applicable( );

   if( !response.empty( ) )
   {
      if( is_special )
         socket.set_no_delay( );

      socket.write_line( response, c_request_timeout, p_progress );
   }

   if( !is_special )
   {
      socket.set_no_delay( );
      socket.write_line( c_response_okay, c_request_timeout, p_progress );
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
   cerr << "processing command: " << command << endl;
#endif
   string response;

   response.reserve( c_response_reserve_size );

   bool clear_response = false;
   bool send_okay_response = true;
   bool possibly_expected_error = false;

#ifdef SSL_SUPPORT
   ssl_socket& socket( socket_handler.get_socket( ) );
#else
   tcp_socket& socket( socket_handler.get_socket( ) );
#endif

   if( !socket_handler.is_restoring( ) && command != c_cmd_ciyam_session_session_terminate )
      socket.set_delay( );

   set_dtm( "" );
   set_grp( "" );
   set_uid( "" );
   set_tz_name( "" );
   set_tmp_directory( "" );

   clear_perms( );

   set_session_progress_message( "" );
   increment_session_commands_executed( );

   set_last_session_cmd( command );

#ifdef HPDF_SUPPORT
   progress* p_pdf_progress = 0;

   trace_progress pdf_progress( TRACE_DETAILS | TRACE_VARIOUS );

   if( get_trace_flags( ) & ( TRACE_DETAILS | TRACE_VARIOUS ) )
      p_pdf_progress = &pdf_progress;
#endif

   progress* p_sock_progress = 0;

   trace_progress sock_progress( TRACE_VERBOSE | TRACE_SOCKETS );

   if( get_trace_flags( ) & ( TRACE_VERBOSE | TRACE_SOCKETS ) )
      p_sock_progress = &sock_progress;

   try
   {
      ostringstream osstr;

      socket_handler.check_lock_expiry( );

      if( socket_handler.is_locked( )
       && command != c_cmd_ciyam_session_starttls
       && command != c_cmd_ciyam_session_crypto_seed
       && command != c_cmd_ciyam_session_utils_encrypt
       && command != c_cmd_ciyam_session_system_identity
       && command != c_cmd_ciyam_session_session_terminate
       && command != c_cmd_ciyam_session_session_rpc_unlock )
      {
         bool okay = true;

         if( ( command != c_cmd_ciyam_session_system_variable )
          && ( command != c_cmd_ciyam_session_session_variable ) )
            okay = false;
         else
         {
            // NOTE: When locked will allow the "@args_file" session variable
            // or system variable named with the session variable's non-empty
            // value to be set (required so that special application protocol
            // scripts can be executed even if locked). All system or session
            // variables can be viewed (as long as wildcards are not used) so
            // that the "unlock_identity" script can be supported.
            if( parameters.size( ) > 2 )
            {
               string args_file_name( get_special_var_name( e_special_var_args_file ) );

               if( command == c_cmd_ciyam_session_system_variable )
               {
                  if( parameters.size( ) != 3 )
                     okay = false;
                  else
                  {
                     string args_file( get_raw_session_variable( args_file_name ) );

                     if( args_file.empty( )
                      || !has_parm_val( parameters, c_cmd_ciyam_session_system_variable_name_or_expr ) )
                        okay = false;
                     else if( args_file
                      != ( get_parm_val( parameters, c_cmd_ciyam_session_system_variable_name_or_expr ) ) )
                        okay = false;
                  }
               }
               else
               {
                  if( parameters.size( ) != 3 )
                     okay = false;
                  else
                  {
                     if( !has_parm_val( parameters, c_cmd_ciyam_session_session_variable_name_or_expr ) )
                        okay = false;
                     else if( args_file_name
                      != get_parm_val( parameters, c_cmd_ciyam_session_session_variable_name_or_expr ) )
                        okay = false;
                  }
               }
            }
            else
            {
               string name_or_expr;

               if( command == c_cmd_ciyam_session_system_variable )
                  name_or_expr = get_parm_val( parameters, c_cmd_ciyam_session_system_variable_name_or_expr );
               else
                  name_or_expr = get_parm_val( parameters, c_cmd_ciyam_session_session_variable_name_or_expr );

               if( name_or_expr.find_first_of( "*?" ) != string::npos )
                  okay = false;
            }
         }

         if( !okay )
            // FUTURE: This message should be handled as a server string message.
            throw runtime_error( "Session RPC access denied." );
      }

      if( socket_handler.has_restricted_commands( ) )
      {
         possibly_expected_error = true;

         socket_handler.check_restricted_command( command );
      }

      if( command == c_cmd_ciyam_session_crypto_addr )
      {
         string extkey( get_parm_val( parameters, c_cmd_ciyam_session_crypto_addr_extkey ) );
         bool decrypt = has_parm_val( parameters, c_cmd_ciyam_session_crypto_addr_decrypt );
         bool uncompressed = has_parm_val( parameters, c_cmd_ciyam_session_crypto_addr_uncompressed );
         string secret( get_parm_val( parameters, c_cmd_ciyam_session_crypto_addr_secret ) );

         string pub_key, priv_key;

         priv_key.reserve( c_secret_reserve_size );

         if( decrypt )
         {
            secret.reserve( c_secret_reserve_size );
            decrypt_data( secret, secret );
         }

         response = create_address_key_pair( extkey, pub_key, priv_key, secret, decrypt, true, !uncompressed );

         clear_key( secret );
         clear_key( priv_key );
      }
      else if( command == c_cmd_ciyam_session_crypto_hash )
      {
         bool use_sha512 = has_parm_val( parameters, c_cmd_ciyam_session_crypto_hash_sha512 );
         bool hex_decode = has_parm_val( parameters, c_cmd_ciyam_session_crypto_hash_hex_decode );
         string extra_rounds( get_parm_val( parameters, c_cmd_ciyam_session_crypto_hash_extra_rounds ) );
         bool data_from_file = has_parm_val( parameters, c_cmd_ciyam_session_crypto_hash_data_from_file );
         string data_or_filename( get_parm_val( parameters, c_cmd_ciyam_session_crypto_hash_data_or_filename ) );
         string data_suffix_text( get_parm_val( parameters, c_cmd_ciyam_session_crypto_hash_data_suffix_text ) );
         string data_update_text( get_parm_val( parameters, c_cmd_ciyam_session_crypto_hash_data_update_text ) );

         string data;
         data.reserve( c_secret_reserve_size );

         // NOTE: Supports "<special>:<extra>" where <extra> will be appended before the optional suffix text.
         string::size_type pos = data_or_filename.find( ':' );

         data = data_or_filename.substr( 0, pos );

         if( data_from_file )
            data = load_file( data, true );
         else if( data == get_special_var_name( e_special_var_sid ) )
            get_identity( data, false, true );
         else if( data == get_special_var_name( e_special_var_encrypted_password ) )
            decrypt_data( data, get_session_variable(
             get_special_var_name( e_special_var_encrypted_password ) ) );

         if( data_from_file && data.empty( ) )
            response = "(file not found)";
         else
         {
            if( pos != string::npos )
               data += data_or_filename.substr( pos + 1 );

            data += data_suffix_text;

            string* p_update = 0;

            if( !data_update_text.empty( ) )
               p_update = &data_update_text;

            response = crypto_digest( data, use_sha512, hex_decode, from_string< size_t >( extra_rounds ), p_update );
         }

         if( !data_from_file )
            clear_key( data );
      }
      else if( command == c_cmd_ciyam_session_crypto_keys )
      {
         string extkey( get_parm_val( parameters, c_cmd_ciyam_session_crypto_keys_extkey ) );
         bool decrypt = has_parm_val( parameters, c_cmd_ciyam_session_crypto_keys_decrypt );
         bool encrypt = has_parm_val( parameters, c_cmd_ciyam_session_crypto_keys_encrypt );
         bool use_base64 = has_parm_val( parameters, c_cmd_ciyam_session_crypto_keys_base64 );
         bool uncompressed = has_parm_val( parameters, c_cmd_ciyam_session_crypto_keys_uncompressed );
         string secret( get_parm_val( parameters, c_cmd_ciyam_session_crypto_keys_secret ) );
         string suffix_text( get_parm_val( parameters, c_cmd_ciyam_session_crypto_keys_suffix_text ) );

         string pub_key, priv_key;

         if( secret.empty( ) )
         {
            if( !suffix_text.empty( ) )
               throw runtime_error( "unexpected suffix text supplied for empty secret" );

            response = create_address_key_pair( extkey, pub_key, priv_key, use_base64, !uncompressed );
         }
         else
         {
            if( decrypt )
               decrypt_data( secret, secret );
            else if( secret == get_special_var_name( e_special_var_sid ) )
               get_identity( secret, false, true );

            secret += suffix_text;

            response = create_address_key_pair( extkey, pub_key, priv_key, secret, true, use_base64, !uncompressed );
         }

         if( encrypt )
            encrypt_data( priv_key, priv_key );

         response += '\n' + pub_key + '\n' + priv_key;

         clear_key( secret );
         clear_key( pub_key );
         clear_key( priv_key );
      }
      else if( command == c_cmd_ciyam_session_crypto_seed )
      {
         string pubkey( get_parm_val( parameters, c_cmd_ciyam_session_crypto_seed_pubkey ) );
         string mnenomics_or_hex_seed( get_parm_val( parameters, c_cmd_ciyam_session_crypto_seed_mnemonics_or_hex_seed ) );

         if( !mnenomics_or_hex_seed.empty( ) && !pubkey.empty( ) )
         {
            mnenomics_or_hex_seed.reserve( c_secret_reserve_size );
            session_shared_decrypt( mnenomics_or_hex_seed, pubkey, mnenomics_or_hex_seed );
         }

         get_mnemonics_or_hex_seed( response, mnenomics_or_hex_seed );

         clear_response = true;
      }
      else if( command == c_cmd_ciyam_session_crypto_sign )
      {
         string privkey( get_parm_val( parameters, c_cmd_ciyam_session_crypto_sign_privkey ) );
         bool hex_decode = has_parm_val( parameters, c_cmd_ciyam_session_crypto_sign_hex_decode );
         string message( get_parm_val( parameters, c_cmd_ciyam_session_crypto_sign_message ) );

         response = crypto_sign( privkey, message, hex_decode );
      }
      else if( command == c_cmd_ciyam_session_crypto_chain )
      {
         size_t length = from_string< size_t >( get_parm_val( parameters, c_cmd_ciyam_session_crypto_chain_length ) );
         bool use_base64 = has_parm_val( parameters, c_cmd_ciyam_session_crypto_chain_base64 );
         bool hind_only = has_parm_val( parameters, c_cmd_ciyam_session_crypto_chain_hind_only );
         string secret( get_parm_val( parameters, c_cmd_ciyam_session_crypto_chain_secret ) );

         response = generate_hash_chain( length, use_base64, secret.empty( ) ? 0 : secret.c_str( ), '\n', hind_only );
      }
      else if( command == c_cmd_ciyam_session_crypto_verify )
      {
         string pubkey( get_parm_val( parameters, c_cmd_ciyam_session_crypto_verify_pubkey ) );
         bool hex_decode = has_parm_val( parameters, c_cmd_ciyam_session_crypto_verify_hex_decode );
         string message( get_parm_val( parameters, c_cmd_ciyam_session_crypto_verify_message ) );
         string signature( get_parm_val( parameters, c_cmd_ciyam_session_crypto_verify_signature ) );

         crypto_verify( pubkey, message, signature, hex_decode );
      }
      else if( command == c_cmd_ciyam_session_crypto_lamport )
      {
         bool is_sign = has_parm_val( parameters, c_cmd_ciyam_session_crypto_lamport_sign );
         bool is_verify = has_parm_val( parameters, c_cmd_ciyam_session_crypto_lamport_verify );
         string filename( get_parm_val( parameters, c_cmd_ciyam_session_crypto_lamport_filename ) );
         string mnenomics_or_hex_seed( get_parm_val( parameters, c_cmd_ciyam_session_crypto_lamport_mnemonics_or_hex_seed ) );
         string additional_entropy_text( get_parm_val( parameters, c_cmd_ciyam_session_crypto_lamport_additional_entropy_text ) );

         string::size_type pos = filename.find( ':' );

         if( pos != string::npos )
         {
            string pub_key( filename.substr( 0, pos ) );
            string signature( filename.substr( pos + 1 ) );

            if( has_tag( pub_key ) )
               pub_key = tag_file_hash( pub_key );

            if( has_tag( signature ) )
               signature = tag_file_hash( signature );

            filename = pub_key + ':' + signature;
         }

         const char* p_extra = 0;

         if( !additional_entropy_text.empty( ) )
            p_extra = additional_entropy_text.c_str( );

         response = crypto_lamport( filename, mnenomics_or_hex_seed, is_sign, is_verify, p_extra );
      }
      else if( command == c_cmd_ciyam_session_crypto_pub_key )
      {
         string privkey( get_parm_val( parameters, c_cmd_ciyam_session_crypto_pub_key_privkey ) );
         bool uncompressed = has_parm_val( parameters, c_cmd_ciyam_session_crypto_pub_key_uncompressed );

         response = crypto_public( privkey, !are_hex_nibbles( privkey ), false, !uncompressed );
      }
      else if( command == c_cmd_ciyam_session_crypto_checksum )
      {
         string hashes( get_parm_val( parameters, c_cmd_ciyam_session_crypto_checksum_hashes ) );

         response = crypto_checksum( hashes );
      }
      else if( command == c_cmd_ciyam_session_crypto_addr_hash )
      {
         string address( get_parm_val( parameters, c_cmd_ciyam_session_crypto_addr_hash_address ) );

         response = crypto_address_hash( address );
      }
      else if( command == c_cmd_ciyam_session_crypto_p2sh_addr )
      {
         string extkey( get_parm_val( parameters, c_cmd_ciyam_session_crypto_p2sh_addr_extkey ) );
         string script( get_parm_val( parameters, c_cmd_ciyam_session_crypto_p2sh_addr_script ) );

         response = crypto_p2sh_address( extkey, script );
      }
      else if( command == c_cmd_ciyam_session_crypto_p2sh_redeem )
      {
         string txid( get_parm_val( parameters, c_cmd_ciyam_session_crypto_p2sh_redeem_txid ) );
         string index( get_parm_val( parameters, c_cmd_ciyam_session_crypto_p2sh_redeem_index ) );
         string script( get_parm_val( parameters, c_cmd_ciyam_session_crypto_p2sh_redeem_script ) );
         string address( get_parm_val( parameters, c_cmd_ciyam_session_crypto_p2sh_redeem_address ) );
         string amount( get_parm_val( parameters, c_cmd_ciyam_session_crypto_p2sh_redeem_amount ) );
         string wif_key( get_parm_val( parameters, c_cmd_ciyam_session_crypto_p2sh_redeem_wif_privkey ) );
         string extras( get_parm_val( parameters, c_cmd_ciyam_session_crypto_p2sh_redeem_extras ) );
         string lock_time( get_parm_val( parameters, c_cmd_ciyam_session_crypto_p2sh_redeem_lock_time ) );

         response = construct_p2sh_redeem_transaction(
          txid, from_string< unsigned int >( index ),
          script, extras, address, crypto_amount( amount ),
          wif_key, true, lock_time.empty( ) ? 0 : from_string< uint32_t >( lock_time ) );
      }
      else if( command == c_cmd_ciyam_session_crypto_nonce_search )
      {
         string data( get_parm_val( parameters, c_cmd_ciyam_session_crypto_nonce_search_data ) );
         bool faster = has_parm_val( parameters, c_cmd_ciyam_session_crypto_nonce_search_faster );
         string start( get_parm_val( parameters, c_cmd_ciyam_session_crypto_nonce_search_start ) );
         string range( get_parm_val( parameters, c_cmd_ciyam_session_crypto_nonce_search_range ) );
         string difficulty( get_parm_val( parameters, c_cmd_ciyam_session_crypto_nonce_search_difficulty ) );

         uint32_t start_val;
         uint32_t range_val = 16;

         nonce_difficulty difficulty_val = e_nonce_difficulty_easy;

         if( start.empty( ) )
            start_val = get_random( );
         else
            start_val = from_string< uint32_t >( start );

         if( !range.empty( ) )
            range_val = from_string< uint32_t >( range );

         if( !difficulty.empty( ) )
            difficulty_val = ( nonce_difficulty )from_string< int >( difficulty );

         // NOTE: To make sure the console client doesn't time out issue a progress message.
         handler.output_progress( "(checking for a valid nonce)" );

         response = check_for_proof_of_work( data, start_val, range_val, difficulty_val, !faster );
      }
      else if( command == c_cmd_ciyam_session_crypto_nonce_verify )
      {
         string data( get_parm_val( parameters, c_cmd_ciyam_session_crypto_nonce_verify_data ) );
         string nonce( get_parm_val( parameters, c_cmd_ciyam_session_crypto_nonce_verify_nonce ) );
         string difficulty( get_parm_val( parameters, c_cmd_ciyam_session_crypto_nonce_verify_difficulty ) );

         nonce_difficulty difficulty_val = e_nonce_difficulty_easy;

         if( !difficulty.empty( ) )
            difficulty_val = ( nonce_difficulty )from_string< int >( difficulty );

         response = check_for_proof_of_work( data, from_string< size_t >( nonce ), 1, difficulty_val );
      }
      else if( command == c_cmd_ciyam_session_file_chk )
      {
         string tag_or_hash( get_parm_val( parameters, c_cmd_ciyam_session_file_chk_tag_or_hash ) );

         string hash( tag_or_hash );

         if( has_tag( tag_or_hash ) )
            response = hash = tag_file_hash( tag_or_hash );

         if( !has_file( hash, false ) )
         {
            possibly_expected_error = true;
            throw runtime_error( "file not found" );
         }
      }
      else if( command == c_cmd_ciyam_session_file_get )
      {
         string tag_or_hash( get_parm_val( parameters, c_cmd_ciyam_session_file_get_tag_or_hash ) );

         string hash( tag_or_hash );

         if( has_tag( tag_or_hash ) )
            hash = tag_file_hash( tag_or_hash );

         if( !has_file( hash, false ) )
         {
            possibly_expected_error = true;
            throw runtime_error( "file '" + hash + "' not found" );
         }

         fetch_file( hash, socket, p_sock_progress );
      }
      else if( command == c_cmd_ciyam_session_file_put )
      {
         string filename( get_parm_val( parameters, c_cmd_ciyam_session_file_put_filename ) );
         string tag( get_parm_val( parameters, c_cmd_ciyam_session_file_put_tag ) );

         check_not_possible_protocol_response( tag );

         bool is_new = false;

         // NOTE: Although "filename" is used to make the command usage easier to understand for
         // end users it is expected that the value provided will actually be the SHA256 hash of
         // the file content (including "prefix" which "ciyam_client" determines automatically).
         is_new = store_file( filename, socket,
          ( tag.empty( ) ? 0 : tag.c_str( ) ), p_sock_progress, true, 0, false, 0, 0, &handler );

         // NOTE: Although it seems a little odd to be checking this *after* the "store_file" it
         // otherwise would make a mess of the protocol (and the "store_file" just quietly fails
         // to store a blacklisted file).
         if( is_new && file_has_been_blacklisted( filename ) )
            throw runtime_error( "file '" + filename + "' has been blacklisted" );

         if( get_stream_sock( ) )
            clear_udp_recv_file_chunks( );

         set_session_variable( get_special_var_name( e_special_var_last_file_put ), filename );
      }
      else if( command == c_cmd_ciyam_session_file_raw )
      {
         bool is_core = has_parm_val( parameters, c_cmd_ciyam_session_file_raw_core );
         bool is_mime = has_parm_val( parameters, c_cmd_ciyam_session_file_raw_mime );
         bool is_text = has_parm_val( parameters, c_cmd_ciyam_session_file_raw_text );
         bool is_blob = has_parm_val( parameters, c_cmd_ciyam_session_file_raw_blob );
         bool is_list = has_parm_val( parameters, c_cmd_ciyam_session_file_raw_list );
         string data( get_parm_val( parameters, c_cmd_ciyam_session_file_raw_data ) );
         string tag( get_parm_val( parameters, c_cmd_ciyam_session_file_raw_tag ) );

         string archive;

         string::size_type pos = tag.find( ':' );

         // NOTE: If a tag is in the form "xxx:yyy" then "xxx" is assumed to be an
         // archive name for the purposes of creating test raw files in an archive
         // and if data is just digits then will instead be a considered as a size
         // for a string of dots (if ":yyy" then instead uses random characters).
         if( pos != string::npos )
         {
            archive = tag.substr( 0, pos );
            tag.erase( 0, pos + 1 );

            bool is_size = true;

            for( size_t i = 0; i < data.size( ); i++ )
            {
               if( ( data[ i ] < '0' ) || ( data[ i ] > '9' ) )
               {
                  is_size = false;
                  break;
               }
            }

            if( is_size )
            {
               if( !archive.empty( ) )
                  data = string( from_string< size_t >( data ), '.' );
               else
               {
                  size_t num_bytes = from_string< size_t >( data );
                  data.erase( );

                  for( size_t i = 0; i < num_bytes; i++ )
                     data += ( char )( 32 + ( rand( ) % 95 ) );
               }
            }
         }

         check_not_possible_protocol_response( tag );

         // NOTE: A list can be constructed with a comma separated list of existing tags.
         if( is_list )
         {
            string::size_type pos = data.find( '\n' );

            if( pos == string::npos && data.find( ',' ) != string::npos )
            {
               string list_data;

               if( !data.empty( ) && ( data[ 0 ] == ',' ) )
                  data.erase( 0, 1 );

               vector< string > items;

               if( !data.empty( ) )
                  split( data, items );
               else
                  throw runtime_error( "unexpected empty tag list for raw file" );

               for( size_t i = 0; i < items.size( ); i++ )
               {
                  string next( items[ i ] );

                  if( i > 0 )
                     list_data += '\n';
                  list_data += tag_file_hash( next ) + ' ' + next;
               }

               data = list_data;
            }
         }

         if( !is_core && !is_mime )
         {
            if( is_blob )
               data = c_file_type_str_blob + data;
            else if( is_list )
               data = c_file_type_str_list + data;
            else
               throw runtime_error( "unexpected unknown type" );
         }
         else if( is_core )
         {
            if( !data.empty( ) && ( data[ 0 ] == '@' ) )
               data = buffer_file_lines( data.substr( 1 ) );

            if( is_blob )
               data = c_file_type_str_core_blob + data;
            else if( is_list )
               data = c_file_type_str_core_list + data;
            else
               throw runtime_error( "unexpected unknown core type" );

            verify_core_file( data, true );
         }
         else if( is_mime )
         {
            if( is_blob )
               data = c_file_type_str_mime_blob + data;
            else
               throw runtime_error( "only blob type is supported for MIME" );
         }

         response = create_raw_file( data, !is_text, tag.c_str( ) );

         if( !archive.empty( ) )
            create_raw_file_in_archive( archive, response, data );
      }
      else if( command == c_cmd_ciyam_session_file_tag )
      {
         bool is_remove = has_parm_val( parameters, c_cmd_ciyam_session_file_tag_remove );
         bool is_unlink = has_parm_val( parameters, c_cmd_ciyam_session_file_tag_unlink );
         string hash( get_parm_val( parameters, c_cmd_ciyam_session_file_tag_hash ) );
         string names( get_parm_val( parameters, c_cmd_ciyam_session_file_tag_names ) );

         vector< string > tag_names;
         split( names, tag_names );

         for( size_t i = 0; i < tag_names.size( ); i++ )
         {
            string next( tag_names[ i ] );

            check_not_possible_protocol_response( next );

            if( is_remove || is_unlink )
               tag_del( next, is_unlink );
            else
               tag_file( next, hash, false, true );
         }
      }
      else if( command == c_cmd_ciyam_session_file_core )
      {
         bool is_type = has_parm_val( parameters, c_cmd_ciyam_session_file_core_type );
         bool is_header = has_parm_val( parameters, c_cmd_ciyam_session_file_core_header );
         bool is_attribute = has_parm_val( parameters, c_cmd_ciyam_session_file_core_attribute );
         string tag_or_hash( get_parm_val( parameters, c_cmd_ciyam_session_file_core_tag_or_hash ) );
         string key_value( get_parm_val( parameters, c_cmd_ciyam_session_file_core_key_value ) );

         string hash( tag_or_hash );

         if( !hash.empty( ) && has_tag( tag_or_hash ) )
            hash = tag_file_hash( tag_or_hash );

         if( !has_file( hash, false ) )
            throw runtime_error( "file '" + tag_or_hash + "' not found" );

         unsigned char type_and_extra = '\0';

         if( !is_core_file( hash ) )
            throw runtime_error( "file '" + tag_or_hash + "' is not a core file" );

         core_file_data core_data( extract_file( hash, "", 0, 0, &type_and_extra ) );

         if( is_type )
            response = core_data.get_type( );
         else
         {
            if( key_value.empty( ) )
               throw runtime_error( "missing required key value" );

            if( is_header )
               response = core_data.get_header( key_value );
            else
               response = core_data.get_attribute( key_value );
         }
      }
      else if( command == c_cmd_ciyam_session_file_hash )
      {
         bool is_quiet = has_parm_val( parameters, c_cmd_ciyam_session_file_hash_quiet );
         string pat_or_tag( get_parm_val( parameters, c_cmd_ciyam_session_file_hash_pat_or_tag ) );

         // NOTE: Use !<prefix> to find the first matching full hash.
         if( !pat_or_tag.empty( ) && ( pat_or_tag[ 0 ] == '!' ) )
            response = get_hash( pat_or_tag.substr( 1 ) );
         else
         {
            vector< string > tags;

            if( pat_or_tag.size( ) == 1
             || pat_or_tag.find_first_of( "?*" ) == string::npos )
               tags.push_back( pat_or_tag );
            else
            {
               string all_tags( list_file_tags( pat_or_tag ) );

               if( !all_tags.empty( ) )
                  split( all_tags, tags, '\n' );
            }

            for( size_t i = 0; i < tags.size( ); i++ )
            {
               bool rc = false;

               if( i > 0 )
                  response += '\n';
               response += tag_file_hash( tags[ i ], is_quiet ? &rc : 0 );
            }
         }
      }
      else if( command == c_cmd_ciyam_session_file_info )
      {
         bool content = has_parm_val( parameters, c_cmd_ciyam_session_file_info_content );
         bool recurse = has_parm_val( parameters, c_cmd_ciyam_session_file_info_recurse );
         bool total_blobs = has_parm_val( parameters, c_cmd_ciyam_session_file_info_total_blobs );
         bool total_items = has_parm_val( parameters, c_cmd_ciyam_session_file_info_total_items );
         bool total_encrypted = has_parm_val( parameters, c_cmd_ciyam_session_file_info_total_encrypted );
         bool total_repo_entries = has_parm_val( parameters, c_cmd_ciyam_session_file_info_total_repo_entries );
         string repository( get_parm_val( parameters, c_cmd_ciyam_session_file_info_repository ) );
         string depth( get_parm_val( parameters, c_cmd_ciyam_session_file_info_depth ) );
         string prefix( get_parm_val( parameters, c_cmd_ciyam_session_file_info_prefix ) );
         string pat_or_hash( get_parm_val( parameters, c_cmd_ciyam_session_file_info_pat_or_hash ) );

         possibly_expected_error = true;

         if( total_blobs || total_items || total_encrypted || total_repo_entries )
         {
            date_time dtm( date_time::local( ) );

            size_t total = 0;
            size_t item_pos = 0;

            bool recurse = true;

            string item_hash;

            string::size_type pos = pat_or_hash.find( ':' );

            if( pos != string::npos )
            {
               item_hash = pat_or_hash.substr( pos + 1 );

               if( item_hash[ item_hash.length( ) - 1 ] == '!' )
               {
                  recurse = false;
                  item_hash.erase( item_hash.length( ) - 1 );
               }
            }

            string tag_or_hash( pat_or_hash.substr( 0, pos ) );

            file_total_type total_type = e_file_total_type_all_items;

            if( total_blobs )
               total_type = e_file_total_type_blobs_only;
            else if( total_encrypted )
               total_type = e_file_total_type_encrypted_only;
            else if( total_repo_entries )
               total_type = e_file_total_type_repository_entries;

            file_list_item_pos( repository, tag_or_hash, total,
             total_type, item_hash, item_pos, recurse, &handler, &dtm );

            if( item_hash.empty( ) )
               response = to_string( total );
            else
               response = to_string( item_pos ) + '/' + to_string( total );
         }
         else
         {
            int depth_val = c_cmd_ciyam_session_file_info_depth_default;

            if( !depth.empty( ) )
               depth_val = atoi( depth.c_str( ) );

            file_expansion expansion = e_file_expansion_none;

            if( content )
               expansion = e_file_expansion_content;
            else if( recurse )
            {
               if( depth_val != 0 )
                  expansion = e_file_expansion_recursive;
               else
                  expansion = e_file_expansion_recursive_hashes;
            }

            deque< string > tags_or_hashes;

            if( pat_or_hash.find_first_of( "?*" ) == string::npos )
               tags_or_hashes.push_back( pat_or_hash );
            else
               list_file_tags( pat_or_hash, 0, 0, 0, 0, &tags_or_hashes );

            bool allow_all_after = false;

            if( !prefix.empty( )
             && ( prefix.find( ':' ) == string::npos )
             && ( prefix[ prefix.length( ) - 1 ] == '*' ) )
            {
               allow_all_after = true;
               prefix.erase( prefix.length( ) - 1 );
            }

            bool output_total_blob_size = false;

            if( !prefix.empty( ) && ( prefix[ 0 ] == '?' ) )
            {
               prefix.erase( 0, 1 );
               output_total_blob_size = true;
            }

            date_time dtm( date_time::local( ) );

            for( size_t i = 0; i < tags_or_hashes.size( ); i++ )
            {
               if( i > 0 )
                  response += '\n';

               response += file_type_info( tags_or_hashes[ i ], expansion, depth_val, 0, true,
                ( prefix.empty( ) ? 0 : prefix.c_str( ) ), allow_all_after, output_total_blob_size, 0, &dtm );
            }
         }
      }
      else if( command == c_cmd_ciyam_session_file_kill )
      {
         string pat( get_parm_val( parameters, c_cmd_ciyam_session_file_kill_pat ) );
         string excludes( get_parm_val( parameters, c_cmd_ciyam_session_file_kill_excludes ) );
         string tag_or_hash( get_parm_val( parameters, c_cmd_ciyam_session_file_kill_tag_or_hash ) );
         bool quiet = has_parm_val( parameters, c_cmd_ciyam_session_file_kill_quiet );
         bool recurse = has_parm_val( parameters, c_cmd_ciyam_session_file_kill_recurse );

         string hash( tag_or_hash );

         if( !hash.empty( ) && has_tag( tag_or_hash ) )
            hash = tag_file_hash( tag_or_hash );

         try
         {
            if( !hash.empty( ) )
            {
               if( !recurse )
                  delete_file( hash );
               else
                  delete_file_tree( hash, &handler );
            }
            else
            {
               const char* p_excludes = 0;

               if( !excludes.empty( ) )
                  p_excludes = excludes.c_str( );

               delete_files_for_tags( pat, &handler, p_excludes );
            }
         }
         catch( exception& )
         {
            if( !quiet )
               throw;
         }
      }
      else if( command == c_cmd_ciyam_session_file_list )
      {
         string add_tags( get_parm_val( parameters, c_cmd_ciyam_session_file_list_add_tags ) );
         string del_items( get_parm_val( parameters, c_cmd_ciyam_session_file_list_del_items ) );
         bool sort = has_parm_val( parameters, c_cmd_ciyam_session_file_list_sort );
         string tag_or_hash( get_parm_val( parameters, c_cmd_ciyam_session_file_list_tag_or_hash ) );
         string new_tag( get_parm_val( parameters, c_cmd_ciyam_session_file_list_new_tag ) );
         string old_tag( get_parm_val( parameters, c_cmd_ciyam_session_file_list_old_tag ) );

         if( tag_or_hash.find( ':' ) == string::npos )
            response = create_list_file( add_tags, del_items, sort, tag_or_hash, new_tag, old_tag );
         else
            response = create_list_tree( add_tags, del_items, sort, tag_or_hash, new_tag, old_tag );
      }
      else if( command == c_cmd_ciyam_session_file_tags )
      {
         bool extract = has_parm_val( parameters, c_cmd_ciyam_session_file_tags_extract );
         string depth( get_parm_val( parameters, c_cmd_ciyam_session_file_tags_depth ) );
         string prefix( get_parm_val( parameters, c_cmd_ciyam_session_file_tags_prefix ) );
         string tag_or_hash( get_parm_val( parameters, c_cmd_ciyam_session_file_tags_tag_or_hash ) );
         string pat_or_hash( get_parm_val( parameters, c_cmd_ciyam_session_file_tags_pat_or_hash ) );
         string includes( get_parm_val( parameters, c_cmd_ciyam_session_file_tags_includes ) );
         string excludes( get_parm_val( parameters, c_cmd_ciyam_session_file_tags_excludes ) );

         if( extract )
         {
            int depth_val = c_cmd_ciyam_session_file_tags_depth_default;
            if( !depth.empty( ) )
               depth_val = atoi( depth.c_str( ) );

            size_t total = 0;
            date_time dtm( date_time::local( ) );

            response = extract_tags_from_lists( tag_or_hash, prefix, depth_val, 0, &handler, &dtm, &total );
         }
         else
         {
            if( !pat_or_hash.empty( ) && ( pat_or_hash.find_first_of( "?*" ) == string::npos ) )
               response = get_hash_tags( pat_or_hash );
            else
            {
               if( includes.empty( ) )
                  response = list_file_tags( pat_or_hash, 0, 0, 0, 0, 0, true, &handler );
               else
               {
                  vector< string > pats;
                  split( includes, pats );

                  for( size_t i = 0; i < pats.size( ); i++ )
                  {
                     if( i > 0 )
                        response += '\n';

                     response += list_file_tags( pats[ i ], excludes.c_str( ), 0, 0, 0, 0, true, &handler );
                  }
               }
            }
         }
      }
      else if( command == c_cmd_ciyam_session_file_test )
      {
         bool content = has_parm_val( parameters, c_cmd_ciyam_session_file_test_content );
         string num_packets( get_parm_val( parameters, c_cmd_ciyam_session_file_test_num_packets ) );

         size_t num = from_string< size_t >( num_packets );

         bool has_any = false;

         set< size_t > chunks;
         map< string, string > content_chunks;

         date_time dtm( date_time::local( ) );

         if( num > 1000 )
            throw runtime_error( "num value must be between 0 and 1000" );

         if( num )
         {
            if( session_ip_addr( ) == c_local_ip_addr )
               msleep( c_udp_wait_timeout );

            for( size_t i = 0; i < c_udp_wait_repeats; i++ )
            {
               if( has_udp_recv_file_chunk_info( ) )
               {
                  has_any = true;
                  break;
               }

               msleep( c_udp_wait_timeout );
            }

            if( has_any )
            {
               for( size_t i = 0; i < ( num * 2 ); i++ )
               {
                  size_t chunk;

                  if( i && ( i % num == 0 ) )
                     msleep( c_udp_wait_timeout );

                  string next( get_udp_recv_file_chunk_info( chunk ) );

                  if( next.empty( ) )
                  {
                     // NOTE: This millisecond of sleep can help to prevent packet loss by
                     // giving up some time for the UDP receive threads to read datagrams.
                     if( i % 10 == 0 )
                        msleep( 1 );
                  }
                  else
                  {
                     chunks.insert( chunk );

                     if( content )
                        content_chunks.insert( make_pair( to_comparable_string( chunk, false, 3 ), next ) );

                     if( chunks.size( ) >= num )
                        break;
                  }
               }
            }

         }

         clear_udp_recv_file_chunks( );

         date_time dtm_now( date_time::local( ) );

         size_t milliseconds = elapsed_since_last_recv( dtm, &dtm_now );

         if( content )
         {
            for( map< string, string >::iterator i = content_chunks.begin( ); i != content_chunks.end( ); ++i )
            {
               if( !response.empty( ) )
                  response += '\n';

               response += i->first + ' ' + i->second;
            }
         }

         if( !response.empty( ) )
            response += '\n';

         response += "packets = " + to_string( chunks.size( ) )
          + '/' + to_string( num ) + ", milliseconds = " + to_string( milliseconds );

         msleep( c_udp_wait_timeout );

         if( has_udp_recv_file_chunk_info( ) )
         {
            msleep( c_udp_wait_timeout * 2 );

            clear_udp_recv_file_chunks( );

            response += " (just missed late arrivals)";
         }
      }
      else if( command == c_cmd_ciyam_session_file_crypt )
      {
         bool decrypt = has_parm_val( parameters, c_cmd_ciyam_session_file_crypt_decrypt );
         bool encrypt = has_parm_val( parameters, c_cmd_ciyam_session_file_crypt_encrypt );
         bool recrypt = has_parm_val( parameters, c_cmd_ciyam_session_file_crypt_recrypt );
         bool recurse = has_parm_val( parameters, c_cmd_ciyam_session_file_crypt_recurse );
         bool blobs_only = has_parm_val( parameters, c_cmd_ciyam_session_file_crypt_blobs_only );
         bool blobs_only_repo = has_parm_val( parameters, c_cmd_ciyam_session_file_crypt_blobs_only_repo );
         string repository( get_parm_val( parameters, c_cmd_ciyam_session_file_crypt_repository ) );
         string cipher( get_parm_val( parameters, c_cmd_ciyam_session_file_crypt_cipher ) );
         string tag_or_hash( get_parm_val( parameters, c_cmd_ciyam_session_file_crypt_tag_or_hash ) );
         string password( get_parm_val( parameters, c_cmd_ciyam_session_file_crypt_password ) );
         string pubkey( get_parm_val( parameters, c_cmd_ciyam_session_file_crypt_pubkey ) );

         if( !pubkey.empty( ) )
         {
            password.reserve( c_secret_reserve_size );
            session_shared_decrypt( password, pubkey, password );
         }

         if( password == get_special_var_name( e_special_var_sid ) )
            get_identity( password, false, true );

         try
         {
            size_t total = 0;
            date_time dtm( date_time::local( ) );

            crypt_target target = e_crypt_target_all;
            crypt_operation operation = e_crypt_operation_reverse;

            if( decrypt )
               operation = e_crypt_operation_decrypt;
            else if( encrypt )
               operation = e_crypt_operation_encrypt;
            else if( recrypt )
               operation = e_crypt_operation_recrypt;

            if( blobs_only )
               target = e_crypt_target_blobs_only;
            else if( blobs_only_repo )
               target = e_crypt_target_blobs_only_repo;

            stream_cipher cipher_value = e_stream_cipher_chacha20;

            if( !cipher.empty( ) )
               cipher_value = stream_cipher_value( cipher );

            crypt_file( repository, tag_or_hash, password,
             recurse, target, &handler, &dtm, &total, operation, 0, cipher_value );
         }
         catch( ... )
         {
            clear_key( password );
            possibly_expected_error = true;
            throw;
         }

         clear_key( password );
      }
      else if( command == c_cmd_ciyam_session_file_stats )
         response = get_file_stats( );
      else if( command == c_cmd_ciyam_session_file_touch )
      {
         string hash( get_parm_val( parameters, c_cmd_ciyam_session_file_touch_hash ) );
         string archive( get_parm_val( parameters, c_cmd_ciyam_session_file_touch_archive ) );

         touch_file( hash, archive, !archive.empty( ) );
      }
      else if( command == c_cmd_ciyam_session_file_resync )
      {
         bool remove_invalid_tags = has_parm_val( parameters, c_cmd_ciyam_session_file_resync_remove_invalid_tags );

         resync_files_area( &handler, remove_invalid_tags );

         response = get_file_stats( );
      }
      else if( command == c_cmd_ciyam_session_file_archive )
      {
         bool add = has_parm_val( parameters, c_cmd_ciyam_session_file_archive_add );
         bool clear = has_parm_val( parameters, c_cmd_ciyam_session_file_archive_clear );
         bool remove = has_parm_val( parameters, c_cmd_ciyam_session_file_archive_remove );
         bool repair = has_parm_val( parameters, c_cmd_ciyam_session_file_archive_repair );
         bool resize = has_parm_val( parameters, c_cmd_ciyam_session_file_archive_resize );
         bool destroy = has_parm_val( parameters, c_cmd_ciyam_session_file_archive_destroy );
         string name( get_parm_val( parameters, c_cmd_ciyam_session_file_archive_name ) );
         string path( get_parm_val( parameters, c_cmd_ciyam_session_file_archive_path ) );
         string size_limit( get_parm_val( parameters, c_cmd_ciyam_session_file_archive_size_limit ) );
         string new_size_limit( get_parm_val( parameters, c_cmd_ciyam_session_file_archive_new_size_limit ) );

         possibly_expected_error = true;

         if( add )
            add_file_archive( name, path, unformat_bytes( size_limit ) );
         else if( clear )
            clear_file_archive( name );
         else if( remove || destroy )
            remove_file_archive( name, destroy, false, &handler );
         else if( repair )
            repair_file_archive( name, &handler );
         else if( resize )
            resize_file_archive( name, unformat_bytes( new_size_limit ), &handler );
      }
      else if( command == c_cmd_ciyam_session_file_archives )
      {
         bool minimal = has_parm_val( parameters, c_cmd_ciyam_session_file_archives_minimal );
         bool path_only = has_parm_val( parameters, c_cmd_ciyam_session_file_archives_path_only );
         bool status_update = has_parm_val( parameters, c_cmd_ciyam_session_file_archives_update_status );
         string name( get_parm_val( parameters, c_cmd_ciyam_session_file_archives_name ) );

         if( status_update )
         {
            possibly_expected_error = true;
            archives_status_update( name );
         }

         archive_list_type list_type = e_archive_list_type_full;

         if( minimal )
            list_type = e_archive_list_type_minimal;
         else if( path_only )
            list_type = e_archive_list_type_path_only;

         response = list_file_archives( list_type, 0, 0, false, name.empty( ) ? 0 : &name );
      }
      else if( command == c_cmd_ciyam_session_file_relegate )
      {
         string num_files( get_parm_val( parameters, c_cmd_ciyam_session_file_relegate_num_files ) );
         string size_limit( get_parm_val( parameters, c_cmd_ciyam_session_file_relegate_size_limit ) );
         bool destroy = has_parm_val( parameters, c_cmd_ciyam_session_file_relegate_destroy );
         bool blacklist = has_parm_val( parameters, c_cmd_ciyam_session_file_relegate_blacklist );
         string hash( get_parm_val( parameters, c_cmd_ciyam_session_file_relegate_hash ) );
         string archive( get_parm_val( parameters, c_cmd_ciyam_session_file_relegate_archive ) );

         if( destroy || blacklist )
            delete_file_from_archive( hash, archive, blacklist );
         else
         {
            uint32_t num = 0;

            if( !num_files.empty( ) )
               num = from_string< uint32_t >( num_files );

            int64_t size = 0;

            if( !size_limit.empty( ) )
               size = unformat_bytes( size_limit );

            bool delete_always = false;

            if( archive == "*" )
            {
               archive.erase( );
               delete_always = true;
            }

            response = relegate_one_or_num_oldest_files( hash, archive, num, size, delete_always );
         }
      }
      else if( command == c_cmd_ciyam_session_file_retrieve )
      {
         string hash( get_parm_val( parameters, c_cmd_ciyam_session_file_retrieve_hash ) );
         string tag( get_parm_val( parameters, c_cmd_ciyam_session_file_retrieve_tag ) );
         string days_ahead( get_parm_val( parameters, c_cmd_ciyam_session_file_retrieve_days_ahead ) );

         size_t days = 0;

         if( !days_ahead.empty( ) )
            days = from_string< size_t >( days_ahead );

         try
         {
            response = retrieve_file_from_archive( hash, tag, days );
         }
         catch( ... )
         {
            possibly_expected_error = true;
            throw;
         }
      }
      else if( command == c_cmd_ciyam_session_file_repo_entry )
      {
         string repository( get_parm_val( parameters, c_cmd_ciyam_session_file_repo_entry_repository ) );
         string entry_hash( get_parm_val( parameters, c_cmd_ciyam_session_file_repo_entry_entry_hash ) );
         bool remove = has_parm_val( parameters, c_cmd_ciyam_session_file_repo_entry_remove );

         string local_hash, local_public_key, master_public_key;

         if( !remove )
         {
            if( fetch_repository_entry_record( repository, entry_hash,
             local_hash, local_public_key, master_public_key, false ) )
            {
               response = "entry_hash: " + entry_hash
                + "\nlocal_hash: " + local_hash
                + "\nlocal_public_key: " + local_public_key
                + "\nmaster_public_key: " + master_public_key;
            }
         }
         else
         {
            ods::bulk_write bulk_write( system_ods_instance( ) );

            if( fetch_repository_entry_record( repository, entry_hash,
             local_hash, local_public_key, master_public_key, false ) )
            {
               destroy_repository_entry_record( repository, entry_hash );

               if( !local_hash.empty( ) )
                  delete_file_from_archive( local_hash, "" );
            }
         }
      }
      else if( command == c_cmd_ciyam_session_file_repo_entries )
      {
         string repository( get_parm_val( parameters, c_cmd_ciyam_session_file_repo_entries_repository ) );
         bool total = has_parm_val( parameters, c_cmd_ciyam_session_file_repo_entries_total );
         bool remove_all = has_parm_val( parameters, c_cmd_ciyam_session_file_repo_entries_remove_all );
         bool remove_obsolete = has_parm_val( parameters, c_cmd_ciyam_session_file_repo_entries_remove_obsolete );

         date_time dtm( date_time::local( ) );

         if( total )
            response = to_string( count_total_repository_entries( repository, &dtm, &handler ) );
         else if( remove_all )
            response = to_string( remove_all_repository_entries( repository, &dtm, &handler ) );
         else
            response = to_string( remove_obsolete_repository_entries( repository, &dtm, &handler ) );
      }
      else if( command == c_cmd_ciyam_session_module_list )
      {
         module_list( osstr );

         if( has_session_variable( get_special_var_name( e_special_var_single_string_response ) ) )
            response = osstr.str( );
         else
            output_response_lines( socket, osstr.str( ) );
      }
      else if( command == c_cmd_ciyam_session_module_load )
      {
         string module( get_parm_val( parameters, c_cmd_ciyam_session_module_load_module ) );

         module_load( module, handler, true );
      }
      else if( command == c_cmd_ciyam_session_module_unload )
      {
         string module( get_parm_val( parameters, c_cmd_ciyam_session_module_unload_module ) );

         module_unload( module, handler, true );
      }
      else if( command == c_cmd_ciyam_session_module_class_list )
      {
         string module( get_parm_val( parameters, c_cmd_ciyam_session_module_class_list_module ) );
         string pattern( get_parm_val( parameters, c_cmd_ciyam_session_module_class_list_pattern ) );

         module_class_list( module, osstr, ( pattern.empty( ) ? 0 : pattern.c_str( ) ) );

         if( has_session_variable( get_special_var_name( e_special_var_single_string_response ) ) )
            response = osstr.str( );
         else
            output_response_lines( socket, osstr.str( ) );
      }
      else if( command == c_cmd_ciyam_session_module_strings_list )
      {
         string module( get_parm_val( parameters, c_cmd_ciyam_session_module_strings_list_module ) );

         module_strings_list( module, osstr );

         if( has_session_variable( get_special_var_name( e_special_var_single_string_response ) ) )
            response = osstr.str( );
         else
            output_response_lines( socket, osstr.str( ) );
      }
      else if( command == c_cmd_ciyam_session_module_class_field_list )
      {
         string module( get_parm_val( parameters, c_cmd_ciyam_session_module_class_field_list_module ) );
         string class_id_or_name( get_parm_val( parameters, c_cmd_ciyam_session_module_class_field_list_class ) );

         module_class_fields_list( module, class_id_or_name, osstr );

         if( has_session_variable( get_special_var_name( e_special_var_single_string_response ) ) )
            response = osstr.str( );
         else
            output_response_lines( socket, osstr.str( ) );
      }
      else if( command == c_cmd_ciyam_session_module_class_procedure_list )
      {
         string module( get_parm_val( parameters, c_cmd_ciyam_session_module_class_procedure_list_module ) );
         string class_id_or_name( get_parm_val( parameters, c_cmd_ciyam_session_module_class_procedure_list_class ) );

         module_class_procedures_list( module, class_id_or_name, osstr );

         if( has_session_variable( get_special_var_name( e_special_var_single_string_response ) ) )
            response = osstr.str( );
         else
            output_response_lines( socket, osstr.str( ) );
      }
      else if( command == c_cmd_ciyam_session_object_list )
      {
         list_object_instances( osstr );

         if( has_session_variable( get_special_var_name( e_special_var_single_string_response ) ) )
            response = osstr.str( );
         else
            output_response_lines( socket, osstr.str( ) );
      }
      else if( command == c_cmd_ciyam_session_object_fetch )
      {
         string handle( get_parm_val( parameters, c_cmd_ciyam_session_object_fetch_handle ) );
         string context( get_parm_val( parameters, c_cmd_ciyam_session_object_fetch_context ) );
         string key( get_parm_val( parameters, c_cmd_ciyam_session_object_fetch_key ) );

         instance_fetch( atoi( handle.c_str( ) ), context, key );
      }
      else if( command == c_cmd_ciyam_session_object_create )
      {
         string module( get_parm_val( parameters, c_cmd_ciyam_session_object_create_module ) );
         string mclass( get_parm_val( parameters, c_cmd_ciyam_session_object_create_mclass ) );
         string handle( get_parm_val( parameters, c_cmd_ciyam_session_object_create_handle ) );
         bool dynamic = has_parm_val( parameters, c_cmd_ciyam_session_object_create_dynamic );

         mclass = get_class_id_for_id_or_name( module, mclass );

         response = to_string( create_object_instance( module, mclass, atoi( handle.c_str( ) ), dynamic ) );
      }
      else if( command == c_cmd_ciyam_session_object_destroy )
      {
         string handle( get_parm_val( parameters, c_cmd_ciyam_session_object_destroy_handle ) );

         destroy_object_instance( atoi( handle.c_str( ) ) );
      }
      else if( command == c_cmd_ciyam_session_object_execute )
      {
         string handle( get_parm_val( parameters, c_cmd_ciyam_session_object_execute_handle ) );
         string context( get_parm_val( parameters, c_cmd_ciyam_session_object_execute_context ) );
         string method( get_parm_val( parameters, c_cmd_ciyam_session_object_execute_method ) );
         string args( get_parm_val( parameters, c_cmd_ciyam_session_object_execute_args ) );

         string method_and_args( method );

         if( !args.empty( ) )
            method_and_args += ' ' + args;

         try
         {
            response = execute_object_command( atoi( handle.c_str( ) ), context, method_and_args );
         }
         catch( ... )
         {
            possibly_expected_error = true;
            throw;
         }
      }
      else if( command == c_cmd_ciyam_session_object_op_apply )
      {
         string handle( get_parm_val( parameters, c_cmd_ciyam_session_object_op_apply_handle ) );
         string context( get_parm_val( parameters, c_cmd_ciyam_session_object_op_apply_context ) );

         op_instance_apply( atoi( handle.c_str( ) ), context );
      }
      else if( command == c_cmd_ciyam_session_object_validate )
      {
         string handle( get_parm_val( parameters, c_cmd_ciyam_session_object_validate_handle ) );
         string context( get_parm_val( parameters, c_cmd_ciyam_session_object_validate_context ) );

         try
         {
            validate_object_instance( atoi( handle.c_str( ) ), context );
         }
         catch( ... )
         {
            possibly_expected_error = true;
            throw;
         }
      }
      else if( command == c_cmd_ciyam_session_object_variable )
      {
         string handle( get_parm_val( parameters, c_cmd_ciyam_session_object_variable_handle ) );
         string context( get_parm_val( parameters, c_cmd_ciyam_session_object_variable_context ) );
         string name_or_expr( get_parm_val( parameters, c_cmd_ciyam_session_object_variable_name_or_expr ) );
         bool has_new_val = has_parm_val( parameters, c_cmd_ciyam_session_object_variable_new_value );
         string new_value( get_parm_val( parameters, c_cmd_ciyam_session_object_variable_new_value ) );

         possibly_expected_error = true;

         if( has_new_val )
         {
            check_not_possible_protocol_response( new_value );

            instance_set_variable( atoi( handle.c_str( ) ), context, name_or_expr, new_value );
         }
         else
         {
            response = instance_get_variable( atoi( handle.c_str( ) ), context, name_or_expr );

            check_not_possible_protocol_response( response );
         }
      }
      else if( command == c_cmd_ciyam_session_object_op_cancel )
      {
         string handle( get_parm_val( parameters, c_cmd_ciyam_session_object_op_cancel_handle ) );
         string context( get_parm_val( parameters, c_cmd_ciyam_session_object_op_cancel_context ) );

         op_instance_cancel( atoi( handle.c_str( ) ), context );
      }
      else if( command == c_cmd_ciyam_session_object_op_create )
      {
         string handle( get_parm_val( parameters, c_cmd_ciyam_session_object_op_create_handle ) );
         string context( get_parm_val( parameters, c_cmd_ciyam_session_object_op_create_context ) );
         string key( get_parm_val( parameters, c_cmd_ciyam_session_object_op_create_key ) );

         op_instance_create( atoi( handle.c_str( ) ), context, key );
      }
      else if( command == c_cmd_ciyam_session_object_op_update )
      {
         string handle( get_parm_val( parameters, c_cmd_ciyam_session_object_op_update_handle ) );
         string context( get_parm_val( parameters, c_cmd_ciyam_session_object_op_update_context ) );
         string key( get_parm_val( parameters, c_cmd_ciyam_session_object_op_update_key ) );
         string ver_info( get_parm_val( parameters, c_cmd_ciyam_session_object_op_update_ver_info ) );

         op_instance_update( atoi( handle.c_str( ) ), context, key, ver_info );
      }
      else if( command == c_cmd_ciyam_session_object_op_destroy )
      {
         string handle( get_parm_val( parameters, c_cmd_ciyam_session_object_op_destroy_handle ) );
         string context( get_parm_val( parameters, c_cmd_ciyam_session_object_op_destroy_context ) );
         string key( get_parm_val( parameters, c_cmd_ciyam_session_object_op_destroy_key ) );
         string ver_info( get_parm_val( parameters, c_cmd_ciyam_session_object_op_destroy_ver_info ) );

         op_instance_destroy( atoi( handle.c_str( ) ), context, key, ver_info );
      }
      else if( command == c_cmd_ciyam_session_object_iterate_next )
      {
         string handle( get_parm_val( parameters, c_cmd_ciyam_session_object_iterate_next_handle ) );
         string context( get_parm_val( parameters, c_cmd_ciyam_session_object_iterate_next_context ) );

         bool rc = instance_iterate_next( atoi( handle.c_str( ) ), context );

         if( !rc )
         {
            send_okay_response = false;
            response = c_response_not_found;
         }
      }
      else if( command == c_cmd_ciyam_session_object_iterate_stop )
      {
         string handle( get_parm_val( parameters, c_cmd_ciyam_session_object_iterate_stop_handle ) );
         string context( get_parm_val( parameters, c_cmd_ciyam_session_object_iterate_stop_context ) );

         instance_iterate_stop( atoi( handle.c_str( ) ), context );
      }
      else if( command == c_cmd_ciyam_session_object_review_begin )
      {
         string handle( get_parm_val( parameters, c_cmd_ciyam_session_object_review_begin_handle ) );
         string context( get_parm_val( parameters, c_cmd_ciyam_session_object_review_begin_context ) );
         string key( get_parm_val( parameters, c_cmd_ciyam_session_object_review_begin_key ) );

         instance_review_begin( atoi( handle.c_str( ) ), context, key );
      }
      else if( command == c_cmd_ciyam_session_object_review_finish )
      {
         string handle( get_parm_val( parameters, c_cmd_ciyam_session_object_review_finish_handle ) );
         string context( get_parm_val( parameters, c_cmd_ciyam_session_object_review_finish_context ) );

         instance_review_finish( atoi( handle.c_str( ) ), context );
      }
      else if( command == c_cmd_ciyam_session_object_iterate_forwards )
      {
         string handle( get_parm_val( parameters, c_cmd_ciyam_session_object_iterate_forwards_handle ) );
         string context( get_parm_val( parameters, c_cmd_ciyam_session_object_iterate_forwards_context ) );
         string key_info( get_parm_val( parameters, c_cmd_ciyam_session_object_iterate_forwards_key_info ) );
         bool not_inclusive = has_parm_val( parameters, c_cmd_ciyam_session_object_iterate_forwards_not_inc );

         bool rc = instance_iterate( atoi( handle.c_str( ) ),
          context, key_info, "", "", "", e_iter_direction_forwards, !not_inclusive );

         if( !rc )
         {
            send_okay_response = false;
            response = c_response_not_found;
         }
      }
      else if( command == c_cmd_ciyam_session_object_iterate_backwards )
      {
         string handle( get_parm_val( parameters, c_cmd_ciyam_session_object_iterate_backwards_handle ) );
         string context( get_parm_val( parameters, c_cmd_ciyam_session_object_iterate_backwards_context ) );
         string key_info( get_parm_val( parameters, c_cmd_ciyam_session_object_iterate_backwards_key_info ) );
         bool not_inclusive = has_parm_val( parameters, c_cmd_ciyam_session_object_iterate_backwards_not_inc );

         bool rc = instance_iterate( atoi( handle.c_str( ) ),
          context, key_info, "", "", "", e_iter_direction_backwards, !not_inclusive );

         if( !rc )
         {
            send_okay_response = false;
            response = c_response_not_found;
         }
      }
      else if( command == c_cmd_ciyam_session_peer_listen )
      {
         bool remove = has_parm_val( parameters, c_cmd_ciyam_session_peer_listen_remove );
         string port( get_parm_val( parameters, c_cmd_ciyam_session_peer_listen_port ) );
         string blockchains( get_parm_val( parameters, c_cmd_ciyam_session_peer_listen_blockchains ) );

         string var_name( '@' + port );

         if( remove )
            set_system_variable( var_name, '~' + unprefixed_blockchains( blockchains ) );
         else
            create_peer_listener( atoi( port.c_str( ) ), unprefixed_blockchains( blockchains ) );

         if( remove )
         {
            bool okay = false;

            for( size_t i = 0; i < c_listen_wait_repeats; i++ )
            {
               msleep( c_listen_wait_timeout );

               if( !has_system_variable( var_name ) )
               {
                  okay = true;
                  break;
               }
            }

            if( !okay )
               throw runtime_error( "listener request not processed in time (peer starter session inactive?)" );
         }
      }
      else if( command == c_cmd_ciyam_session_peer_reject )
      {
         bool list = has_parm_val( parameters, c_cmd_ciyam_session_peer_reject_list );
         bool remove = has_parm_val( parameters, c_cmd_ciyam_session_peer_reject_remove );
         string ip_addr( get_parm_val( parameters, c_cmd_ciyam_session_peer_reject_ip_addr ) );

         if( list )
            response = list_peer_ip_addrs_for_rejection( );
         else
         {
            if( !remove )
               add_peer_ip_addr_for_rejection( ip_addr );
            else
               remove_peer_ip_addr_from_rejection( ip_addr );
         }
      }
      else if( command == c_cmd_ciyam_session_peer_connect )
      {
         bool force = has_parm_val( parameters, c_cmd_ciyam_session_peer_connect_force );
         size_t num_supporters = atoi( get_parm_val( parameters, c_cmd_ciyam_session_peer_connect_num_supporters ).c_str( ) );
         string blockchain( get_parm_val( parameters, c_cmd_ciyam_session_peer_connect_blockchain ) );
         string host_and_or_port( get_parm_val( parameters, c_cmd_ciyam_session_peer_connect_host_and_or_port ) );
         bool type_hub = has_parm_val( parameters, c_cmd_ciyam_session_peer_connect_hub );
         bool type_data = has_parm_val( parameters, c_cmd_ciyam_session_peer_connect_data );
         bool type_user = has_parm_val( parameters, c_cmd_ciyam_session_peer_connect_user );
         bool type_backup = has_parm_val( parameters, c_cmd_ciyam_session_peer_connect_backup );
         bool type_shared = has_parm_val( parameters, c_cmd_ciyam_session_peer_connect_shared );

         peerchain_type chain_type = e_peerchain_type_any;

         if( type_hub )
            chain_type = e_peerchain_type_hub;
         else if( type_data )
            chain_type = e_peerchain_type_data;
         else if( type_user )
            chain_type = e_peerchain_type_user;
         else if( type_backup )
            chain_type = e_peerchain_type_backup;
         else if( type_shared )
            chain_type = e_peerchain_type_shared;

         string paired_primary, paired_secondary;

         string secret_hash, secret_hash_name( get_special_var_name( e_special_var_secret_hash ) );

         string::size_type pos = blockchain.find( ':' );

         if( pos == string::npos )
            secret_hash = get_raw_system_variable( secret_hash_name + '_' + blockchain );
         else
         {
            paired_primary = blockchain.substr( 0, pos );
            paired_secondary = blockchain.substr( pos + 1 );

            secret_hash = get_raw_system_variable( secret_hash_name + '_' + paired_primary );

            if( secret_hash.empty( ) )
               secret_hash = get_raw_system_variable( secret_hash_name + '_' + paired_secondary );
         }

         if( type_hub && !paired_primary.empty( ) )
            throw runtime_error( "invalid paired blockchain usage with type hub" );

         auto_ptr< temporary_session_variable > ap_temp_secret_hash;

         if( !secret_hash.empty( ) )
            ap_temp_secret_hash.reset( new temporary_session_variable( secret_hash_name, secret_hash ) );

         if( paired_primary.empty( ) )
            create_peer_initiator( blockchain,
             host_and_or_port, force, num_supporters, true, false, false, chain_type );
         else
         {
            create_peer_initiator( paired_primary + ':' + paired_primary,
             host_and_or_port, force, num_supporters, true, false, false, chain_type );

            create_peer_initiator( paired_secondary + ':' + paired_primary,
             host_and_or_port, force, num_supporters, true, false, false, chain_type );
         }
      }
      else if( command == c_cmd_ciyam_session_peer_persist_file )
      {
         string dest_hash( get_parm_val( parameters, c_cmd_ciyam_session_peer_persist_file_dest_hash ) );
         string pubkey( get_parm_val( parameters, c_cmd_ciyam_session_peer_persist_file_pubkey ) );
         string tag_or_hash( get_parm_val( parameters, c_cmd_ciyam_session_peer_persist_file_tag_or_hash ) );
         string password( get_parm_val( parameters, c_cmd_ciyam_session_peer_persist_file_password ) );

         bool is_dummy_for_testing = ( password == string( c_dummy ) );

         if( !pubkey.empty( ) )
         {
            password.reserve( c_secret_reserve_size );
            session_shared_decrypt( password, pubkey, password );
         }

         if( password == get_special_var_name( e_special_var_sid ) )
            get_identity( password, false, true );

         string src_hash( tag_or_hash );

         if( has_tag( tag_or_hash ) )
            src_hash = tag_file_hash( tag_or_hash );

         if( !dest_hash.empty( ) )
            decrypt_pulled_peer_file( dest_hash, src_hash, password, is_dummy_for_testing );
         else
            response = create_peer_repository_entry_push_info( src_hash, password, 0, true, is_dummy_for_testing );

         clear_key( password );
      }
      else if( command == c_cmd_ciyam_session_peer_channel_height )
      {
         bool minimal = has_parm_val( parameters, c_cmd_ciyam_session_peer_channel_height_minimal );
         bool reversed = has_parm_val( parameters, c_cmd_ciyam_session_peer_channel_height_reversed );
         string identity( get_parm_val( parameters, c_cmd_ciyam_session_peer_channel_height_identity ) );
         string minimum_height( get_parm_val( parameters, c_cmd_ciyam_session_peer_channel_height_minimum_height ) );

         size_t minimum = 0;

         if( !minimum_height.empty( ) )
            minimum = from_string< size_t >( minimum_height );

         response = peer_channel_height( identity, minimal, reversed, minimum );
      }
      else if( command == c_cmd_ciyam_session_perform_fetch )
      {
         string module( get_parm_val( parameters, c_cmd_ciyam_session_perform_fetch_module ) );
         string mclass( get_parm_val( parameters, c_cmd_ciyam_session_perform_fetch_mclass ) );
         bool is_reverse = has_parm_val( parameters, c_cmd_ciyam_session_perform_fetch_reverse );
         string uid( get_parm_val( parameters, c_cmd_ciyam_session_perform_fetch_uid ) );
         string dtm( get_parm_val( parameters, c_cmd_ciyam_session_perform_fetch_dtm ) );
         string grp( get_parm_val( parameters, c_cmd_ciyam_session_perform_fetch_grp ) );
         string tz_name( get_parm_val( parameters, c_cmd_ciyam_session_perform_fetch_tz_name ) );
         string tmp_dir( get_parm_val( parameters, c_cmd_ciyam_session_perform_fetch_tmp_dir ) );
         string filters( get_parm_val( parameters, c_cmd_ciyam_session_perform_fetch_filters ) );
         string perms( get_parm_val( parameters, c_cmd_ciyam_session_perform_fetch_perms ) );
         string search_text( get_parm_val( parameters, c_cmd_ciyam_session_perform_fetch_search_text ) );
         string search_query( get_parm_val( parameters, c_cmd_ciyam_session_perform_fetch_search_query ) );
         string extra_vars( get_parm_val( parameters, c_cmd_ciyam_session_perform_fetch_extra_vars ) );
         string key_info( get_parm_val( parameters, c_cmd_ciyam_session_perform_fetch_key_info ) );
         string limit( get_parm_val( parameters, c_cmd_ciyam_session_perform_fetch_limit ) );
         string set_values( get_parm_val( parameters, c_cmd_ciyam_session_perform_fetch_set_values ) );
         string fields( get_parm_val( parameters, c_cmd_ciyam_session_perform_fetch_fields ) );
         bool minimal = has_parm_val( parameters, c_cmd_ciyam_session_perform_fetch_minimal );
         bool no_default_values = has_parm_val( parameters, c_cmd_ciyam_session_perform_fetch_no_default_values );
         string map_file( get_parm_val( parameters, c_cmd_ciyam_session_perform_fetch_map_file ) );
         bool create_pdf = has_parm_val( parameters, c_cmd_ciyam_session_perform_fetch_create_pdf );
         string format_file( get_parm_val( parameters, c_cmd_ciyam_session_perform_fetch_format_file ) );
         string output_file( get_parm_val( parameters, c_cmd_ciyam_session_perform_fetch_output_file ) );
         string title_name( get_parm_val( parameters, c_cmd_ciyam_session_perform_fetch_title_name ) );

         bool single_string_response = false;

         if( has_session_variable( get_special_var_name( e_special_var_single_string_response ) ) )
            single_string_response = true;

#ifndef HPDF_SUPPORT
         if( create_pdf )
            throw runtime_error( "pdf generation has not been compiled into this server" );
#endif
         if( tz_name.empty( ) )
            tz_name = get_timezone( );

         string context;

         string::size_type pos = mclass.find( ':' );

         if( pos != string::npos )
         {
            context = mclass.substr( pos + 1 );
            mclass.erase( pos );
         }

         mclass = get_class_id_for_id_or_name( module, mclass );

         // NOTE: For usage with "getmeta/getmetap" script.
         if( ( module == c_meta_model_name ) && ( fields == "@all" ) )
            fields = get_meta_class_field_list( mclass );

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

         multimap< size_t, string > field_inserts;

         vector< string > field_list;

         if( !fields.empty( ) )
            split( fields, field_list );

         string normal_fields;

         size_t non_inserts = 0;

         vector< summary_info > summaries;

         for( size_t i = 0; i < field_list.size( ); i++ )
         {
            if( !field_list[ i ].empty( ) && ( field_list[ i ][ 0 ] == '@' ) )
            {
               field_inserts.insert( make_pair( non_inserts, field_list[ i ] ) );
               field_list.erase( field_list.begin( ) + i );

               --i;
            }
            else
            {
               if( !field_list[ i ].empty( ) && ( field_list[ i ][ 0 ] == '!' ) )
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

               ++non_inserts;

               if( !normal_fields.empty( ) )
                  normal_fields += ",";
               normal_fields += field_list[ i ];
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

         map< string, string > search_replaces;

         if( !map_file.empty( ) )
         {
            string path( storage_web_root( true ) );

            path += "/" + map_file;

            ifstream inpf( path.c_str( ) );

            if( !inpf )
               throw runtime_error( "unable to open file '" + path + "' for input in perform_fetch" );

            string next;

            while( getline( inpf, next ) )
            {
               if( next.empty( ) )
                  continue;

               string::size_type pos = next.find( '=' );

               if( pos == string::npos )
                  continue;

               if( pos == next.size( ) - 1 )
                  continue;

               if( next.find( "tmp_" ) == 0 )
                  continue;

               string key = next.substr( pos + 1 );
               string data = next.substr( 0, pos );

               // NOTE: Package options (whether direct or indirect) are only mapped back
               // to values if they are actually class keys (as other values could easily
               // be found within class keys) therefore although useful the map file does
               // not guarantee that a package record can be restored to the exact format
               // appearing in the package file.
               if( data.length( ) > 3
                && ( ( data.substr( 0, 4 ) == "opt_" ) || ( data.substr( 0, 4 ) == "loc_" ) ) )
               {
                  pos = data.find( "_class" );

                  if( ( pos == string::npos ) || ( pos + strlen( "_class" ) != data.length( ) ) )
                     continue;
               }

               search_replaces.insert( make_pair( key, data ) );
            }
         }

         size_t handle = create_object_instance( module, mclass, 0,
          !context.empty( ) || get_module_class_has_derivations( module, mclass ) );

         vector< string > default_values;

         if( no_default_values )
            get_field_values( handle, context, field_list, tz_name, true, false, &default_values );

         // NOTE: The purpose of "extra_vars" is to allow the setting of instance
         // variables that don't have a '@' prefix.
         if( !extra_vars.empty( ) )
         {
            vector< string > extras;
            split( extra_vars, extras );

            for( size_t i = 0; i < extras.size( ); i++ )
            {
               string next( extras[ i ] );
               string::size_type pos = next.find( '=' );

               if( pos == string::npos )
                  throw runtime_error( "unexpected format for extras: " + extra_vars );

               instance_set_variable( handle, context, next.substr( 0, pos ), next.substr( pos + 1 ) );
            }
         }

         for( map< string, string >::iterator i = set_value_items.begin( ), end = set_value_items.end( ); i != end; ++i )
         {
            // NOTE: If a field to be set starts with @ then it is instead assumed to be a "variable".
            if( !i->first.empty( ) && ( i->first[ 0 ] == '@' ) )
               instance_set_variable( handle, context, i->first, i->second );
            else
            {
               // NOTE: Field values are being set here as they may be required for
               // determining details to be used for performing the iteration query.
               string method_name_and_args( "set " );

               method_name_and_args += i->first + " ";

               method_name_and_args += "\"" + escaped( i->second, "\"", c_nul ) + "\"";

               execute_object_command( handle, context, method_name_and_args );
            }
         }

         try
         {
            set_dtm( dtm );
            set_grp( grp );
            set_uid( uid );

            set_tz_name( tz_name );

            set_tmp_directory( tmp_dir );
            
            set< string > perm_set;

            if( !perms.empty( ) )
               split( perms, perm_set );

            set_perms( perm_set );

            // NOTE: For peer record links that are actually to local records the
            // key suffix needs to be replaced by the session identity suffix.
            if( !key_info.empty( ) && ( key_info.find( ' ' ) == string::npos ) )
            {
               string::size_type pos = key_info.find( '_' );

               string key_prefix( key_info.substr( 0, pos ) );

               string key_suffix;

               if( pos != string::npos )
                  key_suffix = key_info.substr( pos );

               if( key_prefix.length( ) > 9 )
               {
                  string identity( key_prefix.substr( key_prefix.length( ) - 9 ) );

                  string data_key( get_system_variable(
                   get_special_var_name( e_special_var_blockchain_data_key ) + '_' + identity ) );

                  if( !data_key.empty( ) )
                  {
                     string identity_suffix( get_session_identity_suffix( ) );

                     if( !identity_suffix.empty( ) )
                     {
                        key_prefix = replaced_suffix( key_prefix, identity, identity_suffix );

                        key_info = key_prefix + key_suffix;
                     }
                  }
               }
            }

            // NOTE: If a space is provided as the key then fetch the default record values or if the
            // first character of the key is a space then clone a default record from another.
            if( !key_info.empty( ) && ( key_info[ 0 ] == ' ' ) )
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
                  op_instance_create( handle, context, key_info, false );
                  prepare_object_instance( handle, context, true );
                  op_instance_cancel( handle, context );

                  // NOTE: Set this variable in order for a "new" record to be able to detect (after
                  // the cancel that is immediately prior to this) that it was cloned.
                  instance_set_variable(
                   handle, "", get_special_var_name( e_special_var_cloned ), c_true_value );
               }

               for( map< string, string >::iterator i = set_value_items.begin( ), end = set_value_items.end( ); i != end; ++i )
               {
                  // NOTE: If a field to be set starts with @ then it is instead assumed to be a "variable".
                  if( !i->first.empty( ) && ( i->first[ 0 ] != '@' ) )
                  {
                     string method_name_and_args( "set " );

                     method_name_and_args += i->first + " ";

                     method_name_and_args += "\"" + escaped( i->second, "\"", c_nul ) + "\"";

                     execute_object_command( handle, context, method_name_and_args );
                  }
               }

               prepare_object_instance( handle, context, true );

               string output( "[" + instance_key_info( handle, context ) + "]" );

               string field_output( get_field_values( handle, context, field_list, tz_name, true, false, 0, &field_inserts ) );

               if( !field_output.empty( ) )
                  output += " " + field_output;

               socket.write_line( output, c_request_timeout, p_sock_progress );
            }
            // NOTE: If just performing a simple record fetch then use "instance_fetch"
            // instead of "instance_iterate" (reduces complexity and supports caching).
            else if( !create_pdf && !key_info.empty( ) && ( num_limit == 1 ) && parent_key.empty( )
             && search_text.empty( ) && search_query.empty( ) && filter_set.empty( ) && set_value_items.empty( ) )
            {
               instance_fetch_rc rc;
               instance_fetch( handle, context, key_info, &rc );

               if( rc == e_instance_fetch_rc_okay )
               {
                  string key_output( "[" + instance_key_info( handle, context ) + "]" );

                  string field_output( get_field_values( handle, context, field_list, tz_name,
                   false, false, 0, &field_inserts, &search_replaces, ( no_default_values ? &default_values : 0 ) ) );

                  if( minimal )
                     response = field_output;
                  else
                  {
                     response = key_output;

                     if( !field_output.empty( ) )
                        response += " " + field_output;
                  }
               }
            }
            else
            {
               size_t num_found = 0;

               // NOTE: If a parent was provided then set it so that child iteration will work (but
               // without the extra overhead of actually fetching the parent).
               if( !parent_key.empty( ) )
                  instance_set_parent( handle, "", parent_key );

               map< string, string > pdf_gen_variables;
               multimap< string, string > summary_sorted_values;

#ifdef HPDF_SUPPORT
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
#endif
               // NOTE: The call to "after_fetch" occurs via "prepare_object_instance" instead.
               if( !set_value_items.empty( ) )
                  instance_set_variable( handle, context,
                   get_special_var_name( e_special_var_skip_after_fetch ), c_true_value );

               if( instance_iterate( handle, context,
                key_info, normal_fields, search_text, search_query,
                is_reverse ? e_iter_direction_backwards : e_iter_direction_forwards,
                true, num_limit, e_sql_optimisation_none, !filter_set.empty( ) ? &filter_set : 0 ) )
               {
                  bool is_first = true;

                  do
                  {
                     for( map< string, string >::iterator i = set_value_items.begin( ), end = set_value_items.end( ); i != end; ++i )
                     {
                        // NOTE: If a field to be set starts with @ then it is instead assumed to be a "variable".
                        if( !i->first.empty( ) && ( i->first[ 0 ] != '@' ) )
                        {
                           string method_name_and_args( "set " );

                           method_name_and_args += i->first + " ";

                           method_name_and_args += "\"" + escaped( i->second, "\"", c_nul ) + "\"";

                           execute_object_command( handle, context, method_name_and_args );
                        }
                     }

                     if( !set_value_items.empty( ) )
                        prepare_object_instance( handle, context, false, true, is_first );

                     is_first = false;

                     bool has_any_filters = !filter_set.empty( );

                     if( !has_any_filters )
                        has_any_filters = instance_has_transient_filter_fields( handle, context );

                     if( has_any_filters && instance_filtered( handle, context ) )
                        continue;

                     // NOTE: If iterating over all records (e.g. for a FK selection) then
                     // special records (such as the 'admin' user record) will be filtered
                     // automatically. This implicit filtering will only occur if there is
                     // either no record limit or if record filters were provided directly
                     // or indirectly due to transient fields (otherwise list paging would
                     // not function correctly as it relies upon an extra record fetch).
                     if( ( !num_limit || has_any_filters ) && instance_uid_filtered( handle, context ) )
                        continue;

#ifdef HPDF_SUPPORT
                     if( create_pdf )
                        add_pdf_variables( handle, context,
                         field_list, summaries, pdf_gen_variables, tz_name, num_limit == 1, num_found );
                     else
#endif
                     {
                        string output;
                        vector< string > raw_values;

                        string key_output( "[" + instance_key_info( handle, context ) + "]" );

                        string field_output( get_field_values( handle, context,
                         field_list, tz_name, false, false, ( summaries.empty( ) ? 0 : &raw_values ),
                         &field_inserts, &search_replaces, ( no_default_values ? &default_values : 0 ) ) );

                        if( minimal )
                           output = field_output;
                        else
                        {
                           output = key_output;

                           if( !field_output.empty( ) )
                              output += " " + field_output;
                        }

                        if( summaries.empty( ) )
                        {
                           if( single_string_response )
                           {
                              if( !response.empty( ) )
                                 response += '\n';

                              response += output;
                           }
                           else
                              socket.write_line( output, c_request_timeout, p_sock_progress );
                        }
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

                     if( g_server_shutdown || ( num_limit && ( ++num_found >= num_limit ) ) )
                     {
                        instance_iterate_stop( handle, context );

                        break;
                     }
                  } while( instance_iterate_next( handle, context ) );
               }

               if( create_pdf )
               {
#ifdef HPDF_SUPPORT
                  if( !num_found )
                  {
                     add_pdf_variables( handle, context,
                      field_list, summaries, pdf_gen_variables, tz_name, num_limit == 1, 0, false );
                  }

                  if( summaries.empty( ) )
                     generate_pdf_doc( format_file, output_file, pdf_gen_variables, p_pdf_progress );
                  else
                  {
                     map< string, string > pdf_final_variables;

                     add_final_pdf_variables( pdf_gen_variables, summaries, pdf_final_variables );

                     generate_pdf_doc( format_file, output_file, pdf_final_variables, p_pdf_progress );
                  }
#endif
               }
               else if( !summaries.empty( ) )
               {
                  for( multimap< string, string >::iterator
                   i = summary_sorted_values.begin( ); i != summary_sorted_values.end( ); ++i )
                  {
                     if( single_string_response )
                     {
                        if( !response.empty( ) )
                           response += '\n';

                        response += i->second;
                     }
                     else
                        socket.write_line( i->second, c_request_timeout, p_sock_progress );
                  }
               }
            }

            destroy_object_instance( handle );
         }
         catch( exception& )
         {
            possibly_expected_error = true;

            destroy_object_instance( handle );

            throw;
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

         string uid( get_parm_val( parameters, c_cmd_ciyam_session_perform_create_uid ) );
         string dtm( get_parm_val( parameters, c_cmd_ciyam_session_perform_create_dtm ) );
         string module( get_parm_val( parameters, c_cmd_ciyam_session_perform_create_module ) );
         string mclass( get_parm_val( parameters, c_cmd_ciyam_session_perform_create_mclass ) );
         string grp( get_parm_val( parameters, c_cmd_ciyam_session_perform_create_grp ) );
         string tz_name( get_parm_val( parameters, c_cmd_ciyam_session_perform_create_tz_name ) );
         string key( get_parm_val( parameters, c_cmd_ciyam_session_perform_create_key ) );
         string key_suffix( get_parm_val( parameters, c_cmd_ciyam_session_perform_create_key_suffix ) );
         string field_values( get_parm_val( parameters, c_cmd_ciyam_session_perform_create_field_values ) );
         string method( get_parm_val( parameters, c_cmd_ciyam_session_perform_create_method ) );

         bool peer_limited_arguments = !get_raw_session_variable(
          get_special_var_name( e_special_var_peer_limited_arguments ) ).empty( );

         if( peer_limited_arguments )
         {
            grp.erase( );
            method.erase( );
            tz_name.erase( );
         }

         string field_values_to_log;
         string module_and_class( module + ' ' + mclass );

         if( tz_name.empty( ) )
            tz_name = get_timezone( );

         set_dtm_if_now( dtm, next_command );

         // NOTE: Use of the session variable '@key_suffix' will check a provided key value while the key_suffix
         // argument is used when generating a new key (by providing this value as a temporary identity suffix).
         string check_key_suffix( get_raw_session_variable( get_special_var_name( e_special_var_key_suffix ) ) );

         if( !check_key_suffix.empty( ) )
            check_key_has_suffix( key, check_key_suffix );

         // NOTE: If no key was provided then will automatically generate a key.
         if( key.empty( ) || ( key[ 0 ] == ' ' ) )
         {
            auto_ptr< temporary_identity_suffix > ap_identity_suffix;

            if( !key_suffix.empty( ) )
               ap_identity_suffix.reset( new temporary_identity_suffix( key_suffix ) );

            string new_key( gen_key( ) );

            // NOTE: If the system identity had been recognised as a "demo" one then (if located) will
            // replace the generated key with the first matching entry found in a module specific file.
            if( has_raw_system_variable( get_special_var_name( e_special_var_system_is_for_demo ) ) )
            {
               string identity( get_raw_system_variable(
                get_special_var_name( e_special_var_system_identity ) ) );

               string module_name( get_module_name_for_id_or_name( module ) );
               string demo_keys_file_name( module_name + c_demo_keys_suffix );

               // NOTE: Expected format for "demo keys" is: <identity>:<class_name>=<key_value>
               if( file_exists( demo_keys_file_name ) )
               {
                  vector< string > lines;
                  vector< string > new_lines;

                  string class_name( get_class_name_for_id_or_name( module, mclass ) );

                  buffer_file_lines( demo_keys_file_name, lines );

                  bool used_demo_key = false;

                  for( size_t i = 0; i < lines.size( ); i++ )
                  {
                     string next_line( lines[ i ] );

                     if( used_demo_key )
                        new_lines.push_back( next_line );
                     else
                     {
                        string next( next_line );

                        string::size_type pos = next.find( identity + ':' );

                        if( pos == 0 )
                        {
                           next.erase( 0, identity.size( ) + 1 );

                           pos = next.find( '=' );

                           if( class_name == next.substr( 0, pos ) )
                           {
                              used_demo_key = true;
                              new_key = next.substr( pos + 1 );
                           }
                        }

                        if( !used_demo_key )
                           new_lines.push_back( next_line );
                     }
                  }

                  if( used_demo_key )
                  {
                     if( new_lines.empty( ) )
                        file_remove( demo_keys_file_name );
                     else
                        write_file_lines( demo_keys_file_name, new_lines );
                  }
               }
            }

            size_t remove_length = 2; // i.e. To remove "" if that is how the key was provided.
            string::size_type spos = next_command.find( uid ) + uid.length( ) + 1;

            // NOTE: The generated key is inserted into the "next_command" so an actual key value
            // will appear in the transaction log (otherwise log operations could not be restored).
            string::size_type ipos = next_command.find( "\"\"", spos );
            if( ipos == string::npos )
            {
               remove_length = 3; // i.e. Includes the trailing quote if the key was provided as " ".
               ipos = next_command.find( "\" ", spos );
            }

            if( ipos == string::npos )
               throw runtime_error( "unable to find empty key in: " + next_command );

            // NOTE: If there is a clone key then insert after the first quote otherwise
            // remove the quotes (as well as the space if the key was provided as that).
            if( key.size( ) > 1 )
               ++ipos;
            else
               next_command.erase( ipos, remove_length );

            next_command.insert( ipos, new_key );

            // NOTE: If there is a clone key then need to keep it.
            if( key.size( ) > 1 )
               key = new_key + key;
            else
               key = new_key;
         }

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
            if( !method.empty( ) )
               transaction_start( );

            bool using_verbose_logging = get_storage_using_verbose_logging( );

            size_t handle = create_object_instance( module, mclass, 0, false );

            try
            {
               set_dtm( dtm );
               set_grp( grp );
               set_uid( uid );

               set_class( mclass );
               set_module( module );
               set_tz_name( tz_name );

               map< string, string > field_scope_and_perm_info_by_id;
               map< string, string > field_scope_and_perm_info_by_name;

               get_all_field_scope_and_permission_info( handle, "", field_scope_and_perm_info_by_id );
               get_all_field_scope_and_permission_info( handle, "", field_scope_and_perm_info_by_name, true );

               if( get_raw_session_variable( get_special_var_name( e_special_var_skip_set_vars ) ).empty( ) )
               {
                  for( map< string, string >::iterator i = field_value_items.begin( ), end = field_value_items.end( ); i != end; ++i )
                  {
                     // NOTE: If a field to be set starts with @ then it is instead assumed to be a "variable".
                     if( !i->first.empty( ) && ( i->first[ 0 ] == '@' ) )
                        instance_set_variable( handle, "", i->first, i->second );
                  }
               }

               op_instance_create( handle, "", key, false );

               set< string > fields_set;

               for( map< string, string >::iterator i = field_value_items.begin( ), end = field_value_items.end( ); i != end; ++i )
               {
                  // NOTE: If a field to be set starts with @ then it is instead assumed to be a "variable".
                  if( !i->first.empty( ) && ( i->first[ 0 ] != '@' ) )
                  {
                     bool is_encrypted = false;
                     bool is_transient = false;
                     bool was_date_time = false;

                     string type_name( get_field_type_name( handle, "", i->first, &is_encrypted, &is_transient ) );

                     if( !i->second.empty( ) )
                     {
                        if( type_name == "date_time" || type_name == "tdatetime" )
                        {
                           was_date_time = true;

                           if( !tz_name.empty( ) )
                              i->second = convert_local_to_utc( i->second, tz_name );
                        }
                     }

                     if( !is_system_uid( ) && !storage_locked_for_admin( ) )
                     {
                        string scope_and_perm_info;

                        if( field_scope_and_perm_info_by_id.count( i->first ) )
                           scope_and_perm_info = field_scope_and_perm_info_by_id[ i->first ];
                        else
                           scope_and_perm_info = field_scope_and_perm_info_by_name[ i->first ];

                        check_instance_field_permission( module, handle, true, scope_and_perm_info );
                     }

                     string value, method_name_and_args( "get " );

                     if( !using_verbose_logging )
                     {
                        method_name_and_args += i->first;

                        value = execute_object_command( handle, "", method_name_and_args );

                        if( !value.empty( ) && was_date_time )
                        {
                           method_name_and_args = "cmd " + i->first + " raw";
                           value = execute_object_command( handle, "", method_name_and_args );
                        }
                     }

                     method_name_and_args = "set ";

                     method_name_and_args += i->first + " ";

                     method_name_and_args += "\"" + escaped( i->second, "\"", c_nul ) + "\"";

                     execute_object_command( handle, "", method_name_and_args );

                     fields_set.insert( i->first );

                     if( !using_verbose_logging )
                     {
                        // NOTE: If not using verbose logging then unchanged field values will be omitted.
                        if( value != i->second )
                        {
                           if( !field_values_to_log.empty( ) )
                              field_values_to_log += ",";

                           string field_id( get_shortened_field_id( module, mclass, i->first ) );

                           field_values_to_log += field_id + '=';
                           field_values_to_log += escaped( escaped( i->second, "," ), ",\"", c_nul, "rn\r\n" );
                        }
                     }
                  }
               }

               if( !is_system_uid( ) && !storage_locked_for_admin( ) )
                  check_instance_op_permission( module, handle, get_create_instance_info( handle, "" ) );

               string uid_check( get_raw_session_variable( get_special_var_name( e_special_var_uid_check ) ) );

               if( !uid_check.empty( ) )
               {
                  if( !uid_check.empty( ) && ( get_uid( ) != uid_check ) )
                     throw runtime_error( "unexpected uid found '" + get_uid( ) + "' but needs to be '" + uid_check + "'" );

                  append_datachain_as_variable_if_found( handle, field_values_to_log );
               }

               remove_uid_extra_from_log_command( next_command );

               if( !using_verbose_logging )
               {
                  replace_field_values_to_log( next_command, field_values_to_log );
                  replace_module_and_class_to_log( next_command, module_and_class, module, mclass );
               }

               if( instance_persistence_uses_log( handle ) )
                  transaction_log_command( next_command );

               op_instance_apply( handle, "", false, 0, &fields_set );

               response = key.substr( 0, key.find( ' ' ) );

               // NOTE: If a method name has also been provided then execute it now.
               if( !method.empty( ) )
               {
                  string method_name = resolve_method_name( module,
                   mclass, method, &socket_handler.get_transformations( ) );

                  instance_execute( handle, "", response, method_name );
                  transaction_commit( );
               }

               destroy_object_instance( handle );
            }
            catch( exception& )
            {
               if( !method.empty( ) )
                  transaction_rollback( );

               possibly_expected_error = true;
               destroy_object_instance( handle );
               throw;
            }
            catch( ... )
            {
               if( !method.empty( ) )
                  transaction_rollback( );

               destroy_object_instance( handle );
               throw;
            }
         }
      }
      else if( command == c_cmd_ciyam_session_perform_update )
      {
         string next_command( socket_handler.get_next_command( ) );

         string uid( get_parm_val( parameters, c_cmd_ciyam_session_perform_update_uid ) );
         string dtm( get_parm_val( parameters, c_cmd_ciyam_session_perform_update_dtm ) );
         string module( get_parm_val( parameters, c_cmd_ciyam_session_perform_update_module ) );
         string mclass( get_parm_val( parameters, c_cmd_ciyam_session_perform_update_mclass ) );
         string grp( get_parm_val( parameters, c_cmd_ciyam_session_perform_update_grp ) );
         string tz_name( get_parm_val( parameters, c_cmd_ciyam_session_perform_update_tz_name ) );
         string key( get_parm_val( parameters, c_cmd_ciyam_session_perform_update_key ) );
         string ver_info( get_parm_val( parameters, c_cmd_ciyam_session_perform_update_ver_info ) );
         string field_values( get_parm_val( parameters, c_cmd_ciyam_session_perform_update_field_values ) );
         string method( get_parm_val( parameters, c_cmd_ciyam_session_perform_update_method ) );
         string check_values( get_parm_val( parameters, c_cmd_ciyam_session_perform_update_check_values ) );

         bool peer_limited_arguments = !get_raw_session_variable(
          get_special_var_name( e_special_var_peer_limited_arguments ) ).empty( );

         if( peer_limited_arguments )
         {
            grp.erase( );
            method.erase( );
            tz_name.erase( );
            ver_info.erase( );
            check_values.erase( );
         }

         string field_values_to_log;
         string module_and_class( module + ' ' + mclass );

         if( tz_name.empty( ) )
            tz_name = get_timezone( );

         set_dtm_if_now( dtm, next_command );

         // NOTE: Ignore version information during storage recovery.
         if( socket_handler.is_restoring( ) )
            ver_info.erase( );

         string extra_field_values_prefix( get_special_var_name( e_special_var_extra_field_values ) + "," );

         if( field_values.find( extra_field_values_prefix ) == 0 )
         {
            field_values.erase( 0, extra_field_values_prefix.length( ) );
            set_session_variable( get_special_var_name( e_special_var_extra_field_values ), "!" );
         }

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

         ltf_key += " " + module + " " + mclass + " " + string( c_log_transformation_op_change_field_value );

         if( !socket_handler.get_transformations( ).empty( ) )
         {
            perform_field_value_transformations( socket_handler.get_transformations( ), ltf_key, field_value_items );
            if( !check_values.empty( ) )
               perform_field_value_transformations( socket_handler.get_transformations( ), ltf_key, check_value_items );
         }

         ltf_key = string( c_log_transformation_scope_create_update_only );

         ltf_key += " " + module + " " + mclass
          + " " + string( c_log_transformation_op_instance_change_field_value ) + " " + key;

         if( !socket_handler.get_transformations( ).empty( ) )
         {
            perform_field_value_transformations( socket_handler.get_transformations( ), ltf_key, field_value_items );

            if( !check_values.empty( ) )
               perform_field_value_transformations( socket_handler.get_transformations( ), ltf_key, check_value_items );
         }

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
            if( !method.empty( ) )
               transaction_start( );

            size_t handle = create_object_instance( module, mclass,
             0, get_module_class_has_derivations( module, mclass ) );

            bool using_verbose_logging = get_storage_using_verbose_logging( );

            try
            {
               set_dtm( dtm );
               set_grp( grp );
               set_uid( uid );

               set_class( mclass );
               set_module( module );
               set_tz_name( tz_name );

               map< string, string > field_scope_and_perm_info_by_id;
               map< string, string > field_scope_and_perm_info_by_name;

               get_all_field_scope_and_permission_info( handle, "", field_scope_and_perm_info_by_id );
               get_all_field_scope_and_permission_info( handle, "", field_scope_and_perm_info_by_name, true );

               string update_fields;

               bool skip_set_vars = !get_raw_session_variable( get_special_var_name( e_special_var_skip_set_vars ) ).empty( );

               for( map< string, string >::iterator i = field_value_items.begin( ), end = field_value_items.end( ); i != end; ++i )
               {
                  // NOTE: If a field to be set starts with @ then it is instead assumed to be a "variable".
                  if( !i->first.empty( ) && ( i->first[ 0 ] == '@' ) )
                  {
                     if( !skip_set_vars )
                        instance_set_variable( handle, "", i->first, i->second );
                  }
                  else
                  {
                     if( !update_fields.empty( ) )
                        update_fields += ',';

                     update_fields += i->first;
                  }
               }

               instance_set_variable( handle, "", get_special_var_name( e_special_var_update_fields ), update_fields );

               op_instance_update( handle, "", key, ver_info, false );

               if( !is_system_uid( ) && !storage_locked_for_admin( ) )
                  check_instance_op_permission( module, handle, get_update_instance_info( handle, "" ) );

               for( map< string, string >::iterator i = check_value_items.begin( ), end = check_value_items.end( ); i != end; ++i )
               {
                  string method_name_and_args( "get " );

                  method_name_and_args += i->first;

                  string value = execute_object_command( handle, "", method_name_and_args );

                  if( value != i->second )
                     throw runtime_error( get_string_message( GS( c_str_field_is_incorrect ),
                      make_pair( c_str_parm_field_is_incorrect_field, get_field_name_for_id( handle, "", i->first ) ) ) );
               }

               set< string > fields_set;

               for( map< string, string >::iterator i = field_value_items.begin( ), end = field_value_items.end( ); i != end; ++i )
               {
                  // NOTE: If a field to be set starts with @ then it is instead assumed to be a "variable".
                  if( !i->first.empty( ) && ( i->first[ 0 ] != '@' ) )
                  {
                     bool is_encrypted = false;
                     bool is_transient = false;
                     bool is_file_field = false;
                     bool was_date_time = false;

                     string type_name( get_field_type_name(
                      handle, "", i->first, &is_encrypted, &is_transient, &is_file_field ) );

                     if( !i->second.empty( ) )
                     {
                        if( type_name == "date_time" || type_name == "tdatetime" )
                        {
                           was_date_time = true;

                           if( !tz_name.empty( ) )
                              i->second = convert_local_to_utc( i->second, tz_name );
                        }
                     }

                     if( !is_system_uid( ) && !storage_locked_for_admin( ) )
                     {
                        string scope_and_perm_info;

                        if( field_scope_and_perm_info_by_id.count( i->first ) )
                           scope_and_perm_info = field_scope_and_perm_info_by_id[ i->first ];
                        else
                           scope_and_perm_info = field_scope_and_perm_info_by_name[ i->first ];

                        check_instance_field_permission( module, handle, false, scope_and_perm_info );
                     }

                     string value, method_name_and_args( "get " );

                     if( !using_verbose_logging )
                     {
                        method_name_and_args += i->first;

                        value = execute_object_command( handle, "", method_name_and_args );

                        if( !value.empty( ) && was_date_time )
                        {
                           method_name_and_args = "cmd " + i->first + " raw";

                           value = execute_object_command( handle, "", method_name_and_args );
                        }
                     }

                     method_name_and_args = "set ";

                     method_name_and_args += i->first + " ";

                     method_name_and_args += "\"" + escaped( i->second, "\"", c_nul ) + "\"";

                     execute_object_command( handle, "", method_name_and_args );

                     fields_set.insert( i->first );

                     if( !using_verbose_logging )
                     {
                        // NOTE: If not using verbose logging then unchanged field values will be omitted.
                        if( value != i->second )
                        {
                           if( !field_values_to_log.empty( ) )
                              field_values_to_log += ",";

                           string field_id( get_shortened_field_id( module, mclass, i->first ) );

                           field_values_to_log += field_id + '=';
                           field_values_to_log += escaped( escaped( i->second, "," ), ",\"", c_nul, "rn\r\n" );
                        }
                     }
                  }
               }

               remove_uid_extra_from_log_command( next_command );

               string uid_check( get_raw_session_variable( get_special_var_name( e_special_var_uid_check ) ) );

               if( !uid_check.empty( ) )
               {
                  if( !uid_check.empty( ) && ( get_uid( ) != uid_check ) )
                     throw runtime_error( "unexpected uid found '" + get_uid( ) + "' but needs to be '" + uid_check + "'" );

                  append_datachain_as_variable_if_found( handle, field_values_to_log );
               }

               if( !using_verbose_logging )
               {
                  replace_field_values_to_log( next_command, field_values_to_log );

                  replace_module_and_class_to_log( next_command, module_and_class, module, mclass );
               }

               if( instance_persistence_uses_log( handle ) )
                  transaction_log_command( next_command );

               op_instance_apply( handle, "", false, 0, &fields_set );

               // NOTE: If a method name has also been provided then execute it now.
               if( !method.empty( ) )
               {
                  string method_name = resolve_method_name( module,
                   mclass, method, &socket_handler.get_transformations( ) );

                  response = instance_execute( handle, "", key, method_name );

                  transaction_commit( );
               }

               destroy_object_instance( handle );
            }
            catch( exception& x )
            {
               if( !method.empty( ) )
                  transaction_rollback( );

               possibly_expected_error = true;

               destroy_object_instance( handle );

               throw;
            }
            catch( ... )
            {
               if( !method.empty( ) )
                  transaction_rollback( );

               destroy_object_instance( handle );

               throw;
            }
         }
      }
      else if( command == c_cmd_ciyam_session_perform_destroy )
      {
         string next_command( socket_handler.get_next_command( ) );

         string uid( get_parm_val( parameters, c_cmd_ciyam_session_perform_destroy_uid ) );
         string dtm( get_parm_val( parameters, c_cmd_ciyam_session_perform_destroy_dtm ) );
         string module( get_parm_val( parameters, c_cmd_ciyam_session_perform_destroy_module ) );
         string mclass( get_parm_val( parameters, c_cmd_ciyam_session_perform_destroy_mclass ) );
         string grp( get_parm_val( parameters, c_cmd_ciyam_session_perform_destroy_grp ) );
         string tz_name( get_parm_val( parameters, c_cmd_ciyam_session_perform_destroy_tz_name ) );
         string set_values( get_parm_val( parameters, c_cmd_ciyam_session_perform_destroy_set_values ) );
         bool progress = has_parm_val( parameters, c_cmd_ciyam_session_perform_destroy_progress );
         bool quiet = has_parm_val( parameters, c_cmd_ciyam_session_perform_destroy_quiet );
         string key( get_parm_val( parameters, c_cmd_ciyam_session_perform_destroy_key ) );
         string ver_info( get_parm_val( parameters, c_cmd_ciyam_session_perform_destroy_ver_info ) );

         string module_and_class( module + ' ' + mclass );

         if( tz_name.empty( ) )
            tz_name = get_timezone( );

         // NOTE: Ignore version information during storage recovery.
         if( socket_handler.is_restoring( ) )
            ver_info.erase( );

         set_dtm_if_now( dtm, next_command );

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
            map< string, string > set_value_items;

            if( !set_values.empty( ) )
               parse_field_values( module, mclass, set_values, set_value_items, &socket_handler.get_transformations( ) );

            size_t handle = create_object_instance( module, mclass,
             0, get_module_class_has_derivations( module, mclass ) );

            try
            {
               set_dtm( dtm );
               set_grp( grp );
               set_uid( uid );

               set_class( mclass );
               set_module( module );
               set_tz_name( tz_name );

               if( progress )
                  instance_set_variable( handle, "",
                   get_special_var_name( e_special_var_progress ), c_true_value );

               for( map< string, string >::iterator i = set_value_items.begin( ), end = set_value_items.end( ); i != end; ++i )
               {
                  // NOTE: If a field to be set starts with @ then it is instead assumed to be a "variable".
                  if( !i->first.empty( ) && ( i->first[ 0 ] == '@' ) )
                     instance_set_variable( handle, "", i->first, i->second );
               }

               op_destroy_rc rc;

               bool force_internal = has_session_variable(
                get_special_var_name( e_special_var_force_internal ) );

               op_instance_destroy( handle, "", key, ver_info, force_internal, &rc );

               if( !is_system_uid( ) && !storage_locked_for_admin( ) )
                  check_instance_op_permission( module, handle, get_destroy_instance_info( handle, "" ) );

               remove_uid_extra_from_log_command( next_command );

               // NOTE: If the "quiet" option is used then will quietly ignore records that are
               // not found. But otherwise if not okay then call without the return code so the
               // exception will be issued (FUTURE - this could be handled more efficiently).
               if( rc != e_op_destroy_rc_okay && ( !quiet || rc != e_op_destroy_rc_not_found ) )
                  op_instance_destroy( handle, "", key, ver_info, false );

               if( rc == e_op_destroy_rc_okay )
               {
                  for( map< string, string >::iterator i = set_value_items.begin( ), end = set_value_items.end( ); i != end; ++i )
                  {
                     // NOTE: If a field to be set starts with @ then it is instead assumed to be a "variable".
                     if( !i->first.empty( ) && ( i->first[ 0 ] != '@' ) )
                     {
                        string method_name_and_args( "set " );

                        method_name_and_args += i->first + " ";

                        method_name_and_args += "\"" + escaped( i->second, "\"", c_nul ) + "\"";

                        execute_object_command( handle, "", method_name_and_args );
                     }
                  }

                  if( instance_persistence_uses_log( handle ) )
                     transaction_log_command( next_command );

                  op_instance_apply( handle, "", false );
               }

               destroy_object_instance( handle );
            }
            catch( exception& )
            {
               possibly_expected_error = true;

               destroy_object_instance( handle );

               throw;
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

         string uid( get_parm_val( parameters, c_cmd_ciyam_session_perform_execute_uid ) );
         string dtm( get_parm_val( parameters, c_cmd_ciyam_session_perform_execute_dtm ) );
         string module( get_parm_val( parameters, c_cmd_ciyam_session_perform_execute_module ) );
         string mclass( get_parm_val( parameters, c_cmd_ciyam_session_perform_execute_mclass ) );
         string grp( get_parm_val( parameters, c_cmd_ciyam_session_perform_execute_grp ) );
         string tz_name( get_parm_val( parameters, c_cmd_ciyam_session_perform_execute_tz_name ) );
         string set_values( get_parm_val( parameters, c_cmd_ciyam_session_perform_execute_set_values ) );
         string keys( get_parm_val( parameters, c_cmd_ciyam_session_perform_execute_keys ) );
         string vers( get_parm_val( parameters, c_cmd_ciyam_session_perform_execute_vers ) );
         string method( get_parm_val( parameters, c_cmd_ciyam_session_perform_execute_method ) );
         string args( get_parm_val( parameters, c_cmd_ciyam_session_perform_execute_args ) );

         string key_prefix;
         string field_values_to_log;

         string module_and_class( module + ' ' + mclass );

         if( tz_name.empty( ) )
            tz_name = get_timezone( );

         // NOTE: Ignore version information during storage recovery.
         if( socket_handler.is_restoring( ) )
            vers.erase( );

         set_dtm_if_now( dtm, next_command );

         bool log_as_update = false;
         bool log_transaction = true;
         bool skip_transaction = false;

         // NOTE: If a method id/name is prefixed by an underbar then this command is deemed to be
         // a "non-transactional" command and will not be logged (nor will a be transaction used).
         if( !method.empty( ) && ( method[ 0 ] == '_' ) )
         {
            log_transaction = false;
            skip_transaction = true;

            method.erase( 0, 1 );
         }
         else
         {
            // NOTE: If method id/name is prefixed by a minus sign then the command will still be
            // logged but a transaction will not be issued whilst handling the command here.
            if( !method.empty( ) && ( method[ 0 ] == '-' ) )
            {
               skip_transaction = true;
               method.erase( 0, 1 );
            }

            // NOTE: If method id/name is prefixed by a forward slash then the command will not be
            // logged as an execute but as though an update command had been issued instead. It is
            // not valid to attempt to use this with multiple key values.
            if( !method.empty( ) && ( method[ 0 ] == '/' ) )
            {
               if( skip_transaction )
                  throw runtime_error( "invalid attempt to use log_as_update with skip_transaction in perform_execute" );

               log_as_update = true;
               method.erase( 0, 1 );
            }
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

            string method_and_args( resolve_method_name(
             module, mclass, method, &socket_handler.get_transformations( ), &method_id ) );

            map< string, string > set_value_items;

            if( !set_values.empty( ) )
               parse_field_values( module, mclass, set_values, set_value_items, &socket_handler.get_transformations( ) );

            // NOTE: Special case for @notifier child records.
            if( !args.empty( ) )
            {
               string::size_type pos = args.find( ' ' );

               if( args.substr( 0, pos ) == get_special_var_name( e_special_var_notifier ) )
               {
                  key_prefix = args.substr( pos + 1 ) + '/';
                  args.erase( );
               }
            }

            if( !args.empty( ) )
            {
               string execute_args( args );

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

               method_and_args += ' ' + execute_args;
            }
            else
            {
               string ltf_key( c_log_transformation_scope_execute_only );

               ltf_key += " " + module + " " + mclass
                + " " + method_id + " " + string( c_log_transformation_op_no_args_append );

               if( socket_handler.get_transformations( ).count( ltf_key ) )
               {
                  method_and_args += ' ';
                  method_and_args += socket_handler.get_transformations( )[ ltf_key ];
               }
            }

            size_t handle = create_object_instance( module, mclass,
             0, get_module_class_has_derivations( module, mclass ) );

            bool using_verbose_logging = get_storage_using_verbose_logging( );

            if( !instance_persistence_uses_log( handle ) )
               log_transaction = false;

            bool has_any_set_flds = false;

            for( map< string, string >::iterator i = set_value_items.begin( ), end = set_value_items.end( ); i != end; ++i )
            {
               // NOTE: If a field to be set starts with @ then it is instead assumed to be a "variable".
               if( !i->first.empty( ) && ( i->first[ 0 ] == '@' ) )
                  instance_set_variable( handle, "", i->first, i->second );
               else
                  has_any_set_flds = true;
            }

            map< string, string > field_scope_and_perm_info_by_id;
            map< string, string > field_scope_and_perm_info_by_name;

            if( has_any_set_flds )
            {
               get_all_field_scope_and_permission_info( handle, "", field_scope_and_perm_info_by_id );
               get_all_field_scope_and_permission_info( handle, "", field_scope_and_perm_info_by_name, true );
            }

            string client_message( instance_get_variable( handle, "", get_special_var_name( e_special_var_message ) ) );

            if( !client_message.empty( ) && !storage_locked_for_admin( ) )
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
               set_dtm( dtm );
               set_grp( grp );
               set_uid( uid );

               set_class( mclass );
               set_module( module );
               set_tz_name( tz_name );

               if( log_as_update && ( all_keys.size( ) != 1 ) )
                  throw runtime_error( "perform_execute cannot log_as_update with multiple keys" );

               if( all_vers.size( ) && ( all_keys.size( ) != all_vers.size( ) ) )
                  throw runtime_error( "unexpected # keys ("
                   + to_string( all_keys.size( ) ) + ") not equal to # vers (" + to_string( all_vers.size( ) ) + ")" );

               vector< string > initial_field_values;

               for( size_t i = 0; i < all_keys.size( ); i++ )
               {
                  string next_key( key_prefix + all_keys[ i ] );
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

                  instance_prepare_execute( handle, "", next_key, next_ver, has_any_set_flds );

                  if( log_as_update )
                     instance_get_field_values( handle, "", initial_field_values );

                  if( !is_system_uid( ) && !storage_locked_for_admin( ) )
                     check_instance_op_permission( module, handle, get_execute_procedure_info( handle, "", method_id ) );

                  for( map< string, string >::iterator j = set_value_items.begin( ), end = set_value_items.end( ); j != end; ++j )
                  {
                     // NOTE: If a field to be set starts with @ then it is instead assumed to be a "variable".
                     if( !j->first.empty( ) && ( j->first[ 0 ] != '@' ) )
                     {
                        bool is_encrypted = false;
                        bool is_transient = false;
                        bool was_date_time = false;

                        string type_name( get_field_type_name( handle, "", j->first, &is_encrypted, &is_transient ) );

                        if( !j->second.empty( ) )
                        {
                           if( type_name == "date_time" || type_name == "tdatetime" )
                           {
                              was_date_time = true;

                              if( !tz_name.empty( ) )
                                 j->second = convert_local_to_utc( j->second, tz_name );
                           }
                        }

                        if( !is_system_uid( ) && !storage_locked_for_admin( ) )
                        {
                           string scope_and_perm_info;

                           if( field_scope_and_perm_info_by_id.count( j->first ) )
                              scope_and_perm_info = field_scope_and_perm_info_by_id[ j->first ];
                           else
                              scope_and_perm_info = field_scope_and_perm_info_by_name[ j->first ];

                           check_instance_field_permission( module, handle, true, scope_and_perm_info );
                        }

                        string value, method_name_and_args( "get " );

                        if( !using_verbose_logging )
                        {
                           method_name_and_args += j->first;

                           value = execute_object_command( handle, "", method_name_and_args );

                           if( !value.empty( ) && was_date_time )
                           {
                              method_name_and_args = "cmd " + j->first + " raw";
                              value = execute_object_command( handle, "", method_name_and_args );
                           }
                        }

                        method_name_and_args = "set ";

                        method_name_and_args += j->first + " ";

                        method_name_and_args += "\"" + escaped( j->second, "\"", c_nul ) + "\"";

                        execute_object_command( handle, "", method_name_and_args );

                        if( ( i == 0 ) && log_transaction && !using_verbose_logging )
                        {
                           // NOTE: If not using verbose logging then unchanged field values will be omitted.
                           if( value != j->second )
                           {
                              if( !field_values_to_log.empty( ) )
                                 field_values_to_log += ",";

                              string field_id( get_shortened_field_id( module, mclass, j->first ) );

                              field_values_to_log += field_id + '=';
                              field_values_to_log += escaped( escaped( j->second, "," ), ",\"", c_nul, "rn\r\n" );
                           }
                        }
                     }
                  }

                  if( ( i == 0 ) && log_transaction && !log_as_update )
                  {
                     remove_uid_extra_from_log_command( next_command );

                     if( !using_verbose_logging )
                     {
                        string::size_type rpos = 0;

                        replace_field_values_to_log( next_command, field_values_to_log, "-v=", &rpos );

                        if( field_values_to_log.empty( ) )
                           replace( next_command, " \"-v=\"", "" );

                        replace_method_with_shortened_id( next_command, method, rpos, module, mclass, method_id );

                        replace_module_and_class_to_log( next_command, module_and_class, module, mclass );
                     }

                     transaction_log_command( next_command );
                  }

                  if( has_any_set_flds )
                     prepare_object_instance( handle, "", false, false );

                  string next_response( instance_execute( handle, "", next_key, method_and_args ) );

                  if( ( i == 0 ) && log_transaction && log_as_update )
                  {
                     string fields_and_values( instance_get_fields_and_values( handle, "", &initial_field_values ) );

                     if( !fields_and_values.empty( ) )
                     {
                        next_command = "pu " + uid + " " + dtm + " " + module
                         + " " + mclass + " " + next_key + " =" + next_ver + " \"" + fields_and_values + "\"";

                        if( !using_verbose_logging )
                           replace_module_and_class_to_log( next_command, module_and_class, module, mclass );

                        remove_uid_extra_from_log_command( next_command );

                        transaction_log_command( next_command );
                     }
                  }

                  // NOTE: A special object instance variable can be supplied as the return value (so a
                  // return value is possible even if the procedure does not have any output arguments).
                  if( next_response.empty( ) )
                  {
                     next_response = instance_get_variable(
                      handle, "", get_special_var_name( e_special_var_return ) );

                     instance_set_variable( handle, "", get_special_var_name( e_special_var_return ), "" );
                  }

                  string return_response;

                  // NOTE: Cannot have CR/LF pairs in a response (as the client will get confused) so if
                  // these are found in the initial return string then change them to just LF's.
                  for( size_t j = 0; j < next_response.size( ); j++ )
                  {
                     if( ( j < ( next_response.size( ) - 1 ) )
                      && ( next_response[ j ] == '\r' ) && ( next_response[ j + 1 ] == '\n' ) )
                     {
                        ++i;
                        return_response += '\n';
                     }
                     else
                        return_response += next_response[ j ];
                  }

                  ++num_responses;

                  if( return_response.empty( ) && ( i < all_keys.size( ) - 1 ) )
                     return_response = c_response_okay_more;

                  if( !socket_handler.is_restoring( ) && !return_response.empty( ) )
                     socket.write_line( return_response, c_request_timeout, p_sock_progress );
               }

               if( !is_first && !skip_transaction )
               {
                  skip_transaction = true;

                  transaction_commit( );

                  string sess_retval( get_raw_session_variable( get_special_var_name( e_special_var_return ) ) );

                  if( !sess_retval.empty( ) )
                  {
                     response = sess_retval;
                     set_session_variable( get_special_var_name( e_special_var_return ), "" );
                  }
                  else
                     response = instance_get_variable( handle, "", get_special_var_name( e_special_var_return ) );

                  if( !response.empty( ) )
                     set_script_error_if_applicable( "@" + response );
               }

               destroy_object_instance( handle );
            }
            catch( exception& x )
            {
               TRACE_LOG( ( is_auto_uid( ) ? TRACE_MINIMAL : TRACE_INITIAL | TRACE_SESSION ),
                string( "session error: " ) + x.what( ) + " [" + method_id + "]" );

               set_script_error_if_applicable( x.what( ) );

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
                     socket.write_line( c_response_okay_more, c_request_timeout, p_sock_progress );
               }

               if( !is_first && !skip_transaction )
                  transaction_rollback( );

               destroy_object_instance( handle );
            }
            catch( ... )
            {
               TRACE_LOG( TRACE_MINIMAL, "session error: "
                + string( c_unexpected_unknown_exception ) + " [" + method_id + "]" );

               set_script_error_if_applicable( c_unexpected_unknown_exception );

               if( socket_handler.is_restoring( ) )
                  socket_handler.set_restore_error( c_unexpected_unknown_exception );
               else
               {
                  send_okay_response = false;

                  transaction_log_command( "" );

                  response = string( c_response_error_prefix ) + c_unexpected_unknown_exception;

                  // NOTE: As the client is expecting a response for each key provided when an
                  // an exception is thrown fill out any remaining responses with "okay more".
                  for( size_t i = num_responses; i < all_keys.size( ) - 1; i++ )
                     socket.write_line( c_response_okay_more, c_request_timeout, p_sock_progress );
               }

               if( !is_first && !skip_transaction )
                  transaction_rollback( );

               destroy_object_instance( handle );
            }
         }
      }
      else if( command == c_cmd_ciyam_session_perform_bulk_ops )
      {
         string uid( get_parm_val( parameters, c_cmd_ciyam_session_perform_bulk_ops_uid ) );
         string dtm( get_parm_val( parameters, c_cmd_ciyam_session_perform_bulk_ops_dtm ) );
         string module( get_parm_val( parameters, c_cmd_ciyam_session_perform_bulk_ops_module ) );
         string mclass( get_parm_val( parameters, c_cmd_ciyam_session_perform_bulk_ops_mclass ) );
         string tz_name( get_parm_val( parameters, c_cmd_ciyam_session_perform_bulk_ops_tz_name ) );
         string filename( get_parm_val( parameters, c_cmd_ciyam_session_perform_bulk_ops_filename ) );
         string export_fields( get_parm_val( parameters, c_cmd_ciyam_session_perform_bulk_ops_export_fields ) );
         bool destroy_records = has_parm_val( parameters, c_cmd_ciyam_session_perform_bulk_ops_destroy_records );
         string search_text( get_parm_val( parameters, c_cmd_ciyam_session_perform_bulk_ops_search_text ) );
         string search_query( get_parm_val( parameters, c_cmd_ciyam_session_perform_bulk_ops_search_query ) );
         string fixed_field_values( get_parm_val( parameters, c_cmd_ciyam_session_perform_bulk_ops_fixed_field_values ) );

         if( dtm == c_dtm_now )
            dtm = date_time::standard( ).as_string( );

         response = exec_bulk_ops( module, uid, dtm, mclass, filename,
          export_fields, tz_name, destroy_records, search_text, search_query, fixed_field_values, handler );
      }
      else if( command == c_cmd_ciyam_session_perform_package_export )
      {
         string module( get_parm_val( parameters, c_cmd_ciyam_session_perform_package_export_module ) );
         string mclass( get_parm_val( parameters, c_cmd_ciyam_session_perform_package_export_mclass ) );
         string keys( get_parm_val( parameters, c_cmd_ciyam_session_perform_package_export_keys ) );
         string filename( get_parm_val( parameters, c_cmd_ciyam_session_perform_package_export_filename ) );
         string skip_field_info( get_parm_val( parameters, c_cmd_ciyam_session_perform_package_export_skip_field_info ) );
         string test_info( get_parm_val( parameters, c_cmd_ciyam_session_perform_package_export_test_info ) );
         string exclude_info( get_parm_val( parameters, c_cmd_ciyam_session_perform_package_export_exclude_info ) );
         string include_info( get_parm_val( parameters, c_cmd_ciyam_session_perform_package_export_include_info ) );

         export_package( module, mclass, keys, exclude_info, skip_field_info, test_info, include_info, filename );
      }
      else if( command == c_cmd_ciyam_session_perform_package_import )
      {
         string uid( get_parm_val( parameters, c_cmd_ciyam_session_perform_package_import_uid ) );
         string dtm( get_parm_val( parameters, c_cmd_ciyam_session_perform_package_import_dtm ) );
         string module( get_parm_val( parameters, c_cmd_ciyam_session_perform_package_import_module ) );
         string filename( get_parm_val( parameters, c_cmd_ciyam_session_perform_package_import_filename ) );
         bool new_only = has_parm_val( parameters, c_cmd_ciyam_session_perform_package_import_new_only );
         bool for_remove = has_parm_val( parameters, c_cmd_ciyam_session_perform_package_import_for_remove );
         string key_prefix( get_parm_val( parameters, c_cmd_ciyam_session_perform_package_import_key_prefix ) );
         string replace_info( get_parm_val( parameters, c_cmd_ciyam_session_perform_package_import_replace_info ) );
         string skip_field_info( get_parm_val( parameters, c_cmd_ciyam_session_perform_package_import_skip_field_info ) );

         if( dtm == c_dtm_now )
            dtm = date_time::standard( ).as_string( );

         try
         {
            import_package( module, uid, dtm, filename, key_prefix, replace_info, skip_field_info, new_only, for_remove );
         }
         catch( exception& )
         {
            possibly_expected_error = true;
            throw;
         }
      }
      else if( command == c_cmd_ciyam_session_perform_package_update )
      {
         string name( get_parm_val( parameters, c_cmd_ciyam_session_perform_package_update_name ) );

         update_package( name );
      }
      else if( command == c_cmd_ciyam_session_session_kill )
      {
         bool force = has_parm_val( parameters, c_cmd_ciyam_session_session_kill_force );
         bool at_term = has_parm_val( parameters, c_cmd_ciyam_session_session_kill_at_term );
         bool kill_all = has_parm_val( parameters, c_cmd_ciyam_session_session_kill_all );
         string sess_ids( get_parm_val( parameters, c_cmd_ciyam_session_session_kill_sess_ids ) );
         string seconds( get_parm_val( parameters, c_cmd_ciyam_session_session_kill_seconds ) );

         int num_seconds = 0;
         if( !seconds.empty( ) )
            num_seconds = atoi( seconds.c_str( ) );

         if( kill_all )
            condemn_all_other_sessions( num_seconds, force, at_term );
         else if( !sess_ids.empty( ) )
         {
            vector< string > session_ids;
            split( sess_ids, session_ids );

            for( size_t i = 0; i < session_ids.size( ); i++ )
               condemn_session( atoi( session_ids[ i ].c_str( ) ), num_seconds, force, at_term );
         }
      }
      else if( command == c_cmd_ciyam_session_session_list )
      {
         bool minimal = has_parm_val( parameters, c_cmd_ciyam_session_session_list_minimal );

         list_sessions( osstr, !minimal, !minimal );

         if( has_session_variable( get_special_var_name( e_special_var_single_string_response ) ) )
            response = osstr.str( );
         else
            output_response_lines( socket, osstr.str( ) );
      }
      else if( command == c_cmd_ciyam_session_session_lock )
      {
         bool lock_capture = has_parm_val( parameters, c_cmd_ciyam_session_session_lock_capture );
         bool lock_release = has_parm_val( parameters, c_cmd_ciyam_session_session_lock_release );
         bool at_term = has_parm_val( parameters, c_cmd_ciyam_session_session_lock_at_term );
         bool lock_all = has_parm_val( parameters, c_cmd_ciyam_session_session_lock_all );
         string sess_ids( get_parm_val( parameters, c_cmd_ciyam_session_session_lock_sess_ids ) );

         if( lock_all )
         {
            if( lock_capture )
               capture_all_other_sessions( );
            else
               release_all_other_sessions( at_term );
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
                  release_session( atoi( session_ids[ i ].c_str( ) ), at_term );
            }
         }
      }
      else if( command == c_cmd_ciyam_session_session_wait )
      {
         bool lock = has_parm_val( parameters, c_cmd_ciyam_session_session_wait_lock );
         string uid( get_parm_val( parameters, c_cmd_ciyam_session_session_wait_uid ) );
         string milliseconds( get_parm_val( parameters, c_cmd_ciyam_session_session_wait_milliseconds ) );

         set_uid( uid );

         int msecs = atoi( milliseconds.c_str( ) );

         bool use_dots = false;

         if( msecs < 0 )
         {
            msecs *= -1;
            use_dots = true;
         }

         auto_ptr< guard > ap_guard;

         if( lock )
            ap_guard.reset( new guard( g_mutex, "wait" ) );

         if( msecs <= 2000 )
            msleep( msecs );
         else
         {
            while( true )
            {
               if( use_dots )
                  handler.output_progress( "." );
               else
                  handler.output_progress( "(wait " + format_duration( msecs / 1000, true ) + ")" );

               msleep( min( 1000, msecs ) );

               if( msecs <= 1000 )
                  break;

               msecs -= 1000;
            }

            handler.output_progress( " " );
         }
      }
      else if( command == c_cmd_ciyam_session_session_timeout )
      {
         string seconds( get_parm_val( parameters, c_cmd_ciyam_session_session_timeout_seconds ) );

         unsigned int val;
         if( !seconds.empty( ) )
         {
            val = atoi( seconds.c_str( ) );
            set_session_timeout( val );
         }
         else
            val = get_session_timeout( );

         response = to_string( val );
      }
      else if( command == c_cmd_ciyam_session_session_restrict )
      {
         string key( get_parm_val( parameters, c_cmd_ciyam_session_session_restrict_key ) );
         string commands( get_parm_val( parameters, c_cmd_ciyam_session_session_restrict_commands ) );

         possibly_expected_error = true;

         socket_handler.set_restricted_commands( key, commands );
      }
      else if( command == c_cmd_ciyam_session_session_variable )
      {
         string sess_id( get_parm_val( parameters, c_cmd_ciyam_session_session_variable_sess_id ) );
         string name_or_expr( get_parm_val( parameters, c_cmd_ciyam_session_session_variable_name_or_expr ) );
         bool num_found = has_parm_val( parameters, c_cmd_ciyam_session_session_variable_num_found );
         bool has_new_val = has_parm_val( parameters, c_cmd_ciyam_session_session_variable_new_value );
         string new_value( get_parm_val( parameters, c_cmd_ciyam_session_session_variable_new_value ) );

         string* p_sess_id = 0;

         if( !sess_id.empty( ) )
            p_sess_id = &sess_id;

         bool needs_response = false;

         possibly_expected_error = true;

         if( num_found )
         {
            size_t num = 0;

            if( !has_new_val )
               num = num_have_session_variable( name_or_expr );
            else
               num = num_have_session_variable( name_or_expr, new_value );

            response = to_string( num );
         }
         else
         {
            if( !has_new_val )
               needs_response = true;
            else
            {
               string null_value( get_special_var_name( e_special_var_null ) );

               if( new_value == null_value )
                  new_value.erase( );

               check_not_possible_protocol_response( new_value );

               set_session_variable( name_or_expr, new_value, &needs_response, &handler, p_sess_id );
            }

            if( needs_response )
            {
               response = get_session_variable( name_or_expr, p_sess_id );

               check_not_possible_protocol_response( response );
            }
         }
      }
      else if( command == c_cmd_ciyam_session_session_terminate )
      {
         if( !is_captured_session( ) )
            handler.set_finished( );
         else if( !is_condemned_session( ) )
            condemn_this_session( );

         return;
      }
      else if( command == c_cmd_ciyam_session_session_rpc_unlock )
      {
         string password( get_parm_val( parameters, c_cmd_ciyam_session_session_rpc_unlock_password ) );
         string seconds( get_parm_val( parameters, c_cmd_ciyam_session_session_rpc_unlock_seconds ) );

         if( password == get_rpc_password( ) )
            socket_handler.unlock_rpc( );

         unsigned int val;
         if( !seconds.empty( ) )
         {
            val = atoi( seconds.c_str( ) );
            socket_handler.set_lock_expires( val );
         }
      }
      else if( command == c_cmd_ciyam_session_starttls )
      {
         possibly_expected_error = true;

#ifdef SSL_SUPPORT
         if( socket.is_secure( ) )
            throw runtime_error( "TLS is already active" );

         if( !get_using_ssl( ) )
            throw runtime_error( "SSL has not been initialised" );

         socket.ssl_accept( );

         session_is_using_tls( );
#else
         throw runtime_error( "SSL support not available" );
#endif
      }
      else if( command == c_cmd_ciyam_session_storage_info )
      {
         response = "Name: " + storage_name( ) + '\n';
         response += "Identity: " + storage_identity( ) + '\n';
         response += "Directory: " + storage_module_directory( );
      }
      else if( command == c_cmd_ciyam_session_storage_init )
      {
         string name( get_parm_val( parameters, c_cmd_ciyam_session_storage_init_name ) );
         string directory( get_parm_val( parameters, c_cmd_ciyam_session_storage_init_directory ) );
         bool admin = has_parm_val( parameters, c_cmd_ciyam_session_storage_init_admin );

         if( !admin && has_system_variable( get_special_var_name( e_special_var_preparing_backup ) ) )
            // FUTURE: This message should be handled as a server string message.
            throw runtime_error( "Application is being prepared for Backup." );

         if( !admin && has_system_variable( get_special_var_name( e_special_var_preparing_restore ) ) )
            // FUTURE: This message should be handled as a server string message.
            throw runtime_error( "Application is being prepared for Restore." );

         init_storage( name, directory, handler, admin );
      }
      else if( command == c_cmd_ciyam_session_storage_term )
         term_storage( handler );
      else if( command == c_cmd_ciyam_session_storage_attach )
      {
         string name( get_parm_val( parameters, c_cmd_ciyam_session_storage_attach_name ) );

         attach_storage( name, handler, has_parm_val( parameters, c_cmd_ciyam_session_storage_attach_admin ) );
      }
      else if( command == c_cmd_ciyam_session_storage_backup )
      {
         string name( get_parm_val( parameters, c_cmd_ciyam_session_storage_backup_name ) );
         bool truncate_log = has_parm_val( parameters, c_cmd_ciyam_session_storage_backup_truncate );

         bool is_meta = ( name == c_meta_model_name );

         int truncation_count = 0;
         string sav_db_file_names;

         init_storage( name, "", handler, true );

         backup_storage( handler, ( truncate_log ? &truncation_count : 0 ), &sav_db_file_names );

         term_storage( handler );

         bool has_ltf = false;
         bool has_dead_keys = false;
         bool has_demo_keys = false;

         string log_name( name + ".log" );
         string sql_name( name + ".sql" );

         string backup_sql_name( name + ".backup.sql" );

         string sav_log_name( log_name + ".sav" );
         string sav_sql_name( sql_name + ".sav" );

         string ltf_name( name + ".ltf" );
         string sav_ltf_name( ltf_name + ".sav" );

         string dead_keys_name( name + c_dead_keys_suffix );
         string sav_dead_keys_name( dead_keys_name + ".sav" );

         string demo_keys_name( name + c_demo_keys_suffix );
         string sav_demo_keys_name( demo_keys_name + ".sav" );

         string autoscript_name( "autoscript.sio" );
         string manuscript_name( "manuscript.sio" );

         string sav_autoscript_name( autoscript_name + ".sav" );
         string sav_manuscript_name( manuscript_name + ".sav" );

         string server_sio_name( "ciyam_server.sio" );
         string sav_server_sio_name( server_sio_name + ".sav" );

         string sav_server_db_file_names;

         // NOTE: Scope to ensure streams are closed.
         {
            ifstream logf( log_name.c_str( ), ios::in | ios::binary );
            ifstream sqlf( sql_name.c_str( ), ios::in | ios::binary );

            if( !logf || !sqlf )
               throw runtime_error( "unable to open backup input files for '" + name + "' (in use?)" );

            ofstream sav_logf( sav_log_name.c_str( ), ios::out | ios::binary );
            ofstream sav_sqlf( sav_sql_name.c_str( ), ios::out | ios::binary );

            if( !sav_logf || !sav_sqlf )
               throw runtime_error( "unable to open backup output files for '" + name + "'" );

            copy_stream( logf, sav_logf );
            copy_stream( sqlf, sav_sqlf );

            if( exists_file( ltf_name ) )
            {
               ifstream ltff( ltf_name.c_str( ), ios::in | ios::binary );

               if( !ltff )
                  throw runtime_error( "unable to open '" + ltf_name + "' for input" );

               ofstream sav_ltff( sav_ltf_name.c_str( ), ios::out | ios::binary );

               if( !sav_ltff )
                  throw runtime_error( "unable to open '" + sav_ltf_name + "' for output" );

               copy_stream( ltff, sav_ltff );

               has_ltf = true;
            }

            if( exists_file( dead_keys_name ) )
            {
               ifstream keysf( dead_keys_name.c_str( ), ios::in | ios::binary );

               if( !keysf )
                  throw runtime_error( "unable to open '" + dead_keys_name + "' for input" );

               ofstream sav_keysf( sav_dead_keys_name.c_str( ), ios::out | ios::binary );

               if( !sav_keysf )
                  throw runtime_error( "unable to open '" + sav_dead_keys_name + "' for output" );

               copy_stream( keysf, sav_keysf );

               has_dead_keys = true;
            }

            if( exists_file( demo_keys_name ) )
            {
               ifstream keysf( demo_keys_name.c_str( ), ios::in | ios::binary );

               if( !keysf )
                  throw runtime_error( "unable to open '" + demo_keys_name + "' for input" );

               ofstream sav_keysf( sav_demo_keys_name.c_str( ), ios::out | ios::binary );

               if( !sav_keysf )
                  throw runtime_error( "unable to open '" + sav_demo_keys_name + "' for output" );

               copy_stream( keysf, sav_keysf );

               has_demo_keys = true;
            }

            if( is_meta )
            {
               ifstream ascf( autoscript_name.c_str( ), ios::in | ios::binary );

               if( !ascf )
                  throw runtime_error( "unable to open '" + autoscript_name + "' for input" );

               ofstream sav_ascf( sav_autoscript_name.c_str( ), ios::out | ios::binary );

               if( !sav_ascf )
                  throw runtime_error( "unable to open '" + sav_autoscript_name + "' for output" );

               copy_stream( ascf, sav_ascf );

               ifstream mscf( manuscript_name.c_str( ), ios::in | ios::binary );

               if( !mscf )
                  throw runtime_error( "unable to open '" + manuscript_name + "' for input" );

               ofstream sav_mscf( sav_manuscript_name.c_str( ), ios::out | ios::binary );

               if( !sav_mscf )
                  throw runtime_error( "unable to open '" + sav_manuscript_name + "' for output" );

               copy_stream( mscf, sav_mscf );

               ifstream siof( server_sio_name.c_str( ), ios::in | ios::binary );

               if( !siof )
                  throw runtime_error( "unable to open server backup input files for '" + name + "'" );

               ofstream sav_siof( sav_server_sio_name.c_str( ), ios::out | ios::binary );

               if( !sav_siof )
                  throw runtime_error( "unable to open server backup output files for '" + name + "'" );

               copy_stream( siof, sav_siof );

               sav_server_db_file_names = system_ods_instance( ).backup_database( ".sav", ' ' );
            }
         }

         string file_names( sav_db_file_names );

         file_names += " " + sav_sql_name;
         file_names += " " + sav_log_name;
         file_names += " " + backup_sql_name;

         if( has_ltf )
            file_names += " " + sav_ltf_name;

         if( has_dead_keys )
            file_names += " " + sav_dead_keys_name;

         if( has_demo_keys )
            file_names += " " + sav_demo_keys_name;

         string module_list( name + ".modules.lst" );

         file_names += " " + module_list;

         // NOTE: Although the initial data lists and .csv files are backed up they
         // can only be restored manually (because if these files were missing then
         // the application should be regenerated in order to recreate them).
         vector< string > modules;

         buffer_file_lines( module_list, modules );

         for( size_t i = 0; i < modules.size( ); i++ )
         {
            string module_init_list( modules[ i ] + ".init.lst" );

            if( exists_file( module_init_list ) )
            {
               file_names += " " + module_init_list;

               vector< string > init_classes;
               buffer_file_lines( module_init_list, init_classes );

               for( size_t j = 0; j < init_classes.size( ); j++ )
                  file_names += " " + modules[ i ] + "_" + init_classes[ j ] + ".csv";
            }
         }

         if( is_meta )
         {
            file_names += " Meta.cin";

            file_names += " " + sav_autoscript_name;
            file_names += " " + sav_manuscript_name;
            file_names += " " + sav_server_sio_name;

            file_names += " " + sav_server_db_file_names;
         }
         else
         {
            if( file_exists( name + ".app.vars.xrep" ) )
               file_names += " " + name + ".app.vars.xrep";
         }

         ostringstream osstr;

         osstr << setw( 3 ) << setfill( '0' ) << truncation_count;

         if( exists_file( name + ".backup.bun.gz" ) )
            remove_file( name + ".backup.bun.gz" );

         exec_system( "./bundle -q " + name + ".backup.bun.gz " + file_names );

         if( truncate_log )
            exec_system( "./bundle -q " + name + "." + osstr.str( ) + ".bun.gz "
             + name + ".log." + osstr.str( ) + " " + name + ".tlg." + osstr.str( ) );

         remove_files( sav_db_file_names, ' ' );

         remove_file( sav_sql_name );
         remove_file( sav_log_name );

         remove_file( backup_sql_name );

         if( has_ltf )
            remove_file( sav_ltf_name );

         if( has_dead_keys )
            remove_file( sav_dead_keys_name );

         if( has_demo_keys )
            remove_file( sav_demo_keys_name );

         if( is_meta )
         {
            remove_file( sav_autoscript_name );
            remove_file( sav_manuscript_name );
            remove_file( sav_server_sio_name );

            remove_files( sav_server_db_file_names, ' ' );
         }

         if( truncate_log )
         {
            remove_file( name + ".log." + osstr.str( ) );
            remove_file( name + ".tlg." + osstr.str( ) );
         }
      }
      else if( command == c_cmd_ciyam_session_storage_create )
      {
         string name( get_parm_val( parameters, c_cmd_ciyam_session_storage_create_name ) );
         string directory( get_parm_val( parameters, c_cmd_ciyam_session_storage_create_directory ) );

         create_storage( name, directory, handler, has_parm_val( parameters, c_cmd_ciyam_session_storage_create_admin ) );
      }
      else if( command == c_cmd_ciyam_session_storage_comment )
      {
         string text( get_parm_val( parameters, c_cmd_ciyam_session_storage_comment_text ) );

         storage_comment( text );
      }
      else if( command == c_cmd_ciyam_session_storage_restore )
      {
         string name( get_parm_val( parameters, c_cmd_ciyam_session_storage_restore_name ) );
         string directory( get_parm_val( parameters, c_cmd_ciyam_session_storage_restore_directory ) );
         string trace_info( get_parm_val( parameters, c_cmd_ciyam_session_storage_restore_trace_info ) );
         string stop_regex( get_parm_val( parameters, c_cmd_ciyam_session_storage_restore_stop_regex ) );
         int stop_at_tx = atoi( get_parm_val( parameters, c_cmd_ciyam_session_storage_restore_stop_at_tx ).c_str( ) );
         bool rebuild = has_parm_val( parameters, c_cmd_ciyam_session_storage_restore_rebuild );
         bool partial = has_parm_val( parameters, c_cmd_ciyam_session_storage_restore_partial );
         bool quicker = has_parm_val( parameters, c_cmd_ciyam_session_storage_restore_quicker );

         auto_ptr< regex > ap_regex;

         if( !stop_regex.empty( ) )
         {
            if( ( stop_regex.length( ) > 2 )
             && ( stop_regex[ 0 ] = '"' ) && ( stop_regex[ stop_regex.length( ) - 1 ] == '"' ) )
               stop_regex = stop_regex.substr( 1, stop_regex.length( ) - 2 );

            ap_regex.reset( new regex( stop_regex ) );
         }

         bool is_meta = ( name == c_meta_model_name );

         string db_file_names( ods_file_names( name, ' ' ) );

         string sql_name( name + ".sql" );
         string log_name( name + ".log" );

         string backup_sql_name( name + ".backup.sql" );

         string ltf_name( name + ".ltf" );
         string dead_keys_name( name + c_dead_keys_suffix );

         // KLUDGE: Demo key files are actually created per module so tying this
         // file to the storage name is actually incorrect but currently working
         // provided that the storage name matches its (one and only) module.
         string demo_keys_name( name + c_demo_keys_suffix );

         string sav_db_file_names( ods_backup_file_names( name, ".sav", ' ' ) );

         string sav_sql_name( sql_name + ".sav" );
         string sav_log_name( log_name + ".sav" );

         string sav_ltf_name( ltf_name + ".sav" );

         string sav_dead_keys_name( dead_keys_name + ".sav" );
         string sav_demo_keys_name( demo_keys_name + ".sav" );

         string autoscript_name( "autoscript.sio" );
         string manuscript_name( "manuscript.sio" );

         string sav_autoscript_name( autoscript_name + ".sav" );
         string sav_manuscript_name( manuscript_name + ".sav" );

         string server_sio_name( "ciyam_server.sio" );
         string sav_server_sio_name( server_sio_name + ".sav" );

         string sav_server_db_file_names;

         vector< string > module_list;
         string module_list_name( name + ".modules.lst" );

         if( rebuild )
         {
            if( !exists_file( module_list_name ) )
               throw runtime_error( "need '" + module_list_name + "' to perform a rebuild" );

            buffer_file_lines( module_list_name, module_list );
         }

         if( !rebuild && !partial && !exists_file( name + ".backup.bun.gz" ) )
            throw runtime_error( "need '" + name + ".backup.bun.gz' to perform a restore" );

         if( !rebuild && !partial )
         {
            string file_names( sav_db_file_names );

            file_names += " " + sav_sql_name;
            file_names += " " + sav_log_name;

            file_names += " " + sav_ltf_name;
            file_names += " " + sav_dead_keys_name;
            file_names += " " + sav_demo_keys_name;

            file_names += " " + backup_sql_name;
            file_names += " " + module_list_name;

            if( is_meta )
            {
               file_names += " " + sav_autoscript_name;
               file_names += " " + sav_manuscript_name;
               file_names += " " + sav_server_sio_name;

               sav_server_db_file_names = ods_backup_file_names( "ciyam_server", ".sav", ' ' );

               file_names += " " + sav_server_db_file_names;
            }

            exec_system( "./unbundle -o -qw " + name + ".backup.bun.gz " + file_names );

            // NOTE: The ".init.lst" and ".csv" files are omitted (as they should have been
            // generated) and are only included in the backup itself for debugging purposes.
         }

         if( !rebuild && !partial && ( !exists_files( sav_db_file_names, ' ' )
          || !exists_file( sav_sql_name ) || !exists_file( backup_sql_name ) ) )
            throw runtime_error( "incomplete or missing file set for backup restore" );

         storage_admin_name_lock( name );

         if( rebuild )
         {
            remove_files( db_file_names, ' ' );

            remove_file( sql_name );
         }
         else if( !partial )
         {
            copy_files( sav_db_file_names, db_file_names, ' ' );

            copy_file( sav_sql_name, sql_name );

            if( exists_file( sav_log_name ) )
               copy_file( sav_log_name, log_name );

            if( exists_file( sav_ltf_name ) )
               copy_file( sav_ltf_name, ltf_name );

            if( exists_file( sav_dead_keys_name ) )
               copy_file( sav_dead_keys_name, dead_keys_name );

            if( exists_file( sav_demo_keys_name ) )
               copy_file( sav_demo_keys_name, demo_keys_name );
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

         init_storage( name, directory, handler, true );

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
                  module_load( module_list[ i ], handler, false, false );
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

            set_session_variable( get_special_var_name( e_special_var_restore ), c_true_value );

            string next;

            size_t line = 0;

            bool verified = false;
            bool is_partial = true;

            time_t ts = time( 0 );

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
            size_t start_tran_id = c_tx_id_standard;

            if( !is_new )
               start_tran_id = next_transaction_id( ) + 1;

            size_t new_tran_id = start_tran_id;
            size_t last_tran_id = start_tran_id;

            size_t tline = 0;

            bool is_first = true;
            bool first_op = true;
            bool performed_init = false;
            bool is_skipping_legacy = false;
            bool finished_skipping_legacy = false;

            while( getline( inpf, next ) )
            {
               remove_trailing_cr_from_text_file_line( next, is_first );

               if( is_first )
                  is_first = false;

               ++line;

               string::size_type pos = next.find( ']' );

               if( trace_flags && ( line >= trace_start ) )
               {
                  set_trace_flags( trace_flags );
                  trace_flags = 0;
               }

               // NOTE: As recovery can take a long time issue "progress" messages to ensure that
               // client applications don't end up timing out whilst waiting for the final response.
               if( ( time( 0 ) - ts ) >= 5 )
               {
                  ts = time( 0 );

                  // FUTURE: This message should be handled as a server string message.
                  handler.output_progress( "Recovered " + to_string( line ) + " log operations..." );

                  // NOTE: Commit at each progress point to avoid any lengthy commit delays.
                  if( in_trans )
                  {
                     tline = line;
                     in_trans = false;
                     transaction_commit( );
                  }

                  // FUTURE: This message should be handled as a server string message.
                  if( is_condemned_session( ) )
                     throw runtime_error( "Session was terminated by server." );
               }

               if( next.size( ) < 3 || next[ 0 ] != '[' || pos == string::npos )
                  throw runtime_error( "unexpected format found in transaction log line #" + to_string( line ) + " ==> " + next );

               size_t tran_id = from_string< size_t >( next.substr( 1, pos - 1 ) );
               string tran_info = next.substr( pos + 1 );

               if( stop_at_tx && tran_id >= stop_at_tx )
                  break;

               if( ap_regex.get( ) )
               {
                  if( ap_regex->search( next ) != string::npos )
                     break;
               }

               if( !in_trans && ( line >= tline ) && ( tran_id >= c_tx_id_standard ) )
               {
                  in_trans = true;
                  transaction_start( );
               }

               // FUTURE: This message should be handled as a server string message.
               if( !tran_id && !is_new && ( tran_info != storage_identity( ) ) )
                  throw runtime_error( "DB identity mismatch with transaction log." );

               if( !tran_id )
               {
                  if( is_new && !verified )
                  {
                     vector< string > new_tx_log_lines;

                     // NOTE: Restore the original identity.
                     storage_identity( tran_info );

                     new_logf << c_storage_identity_tx_id << storage_identity( ) << '\n';

                     for( size_t i = 0; i < module_tx_info.size( ); i++ )
                        new_logf << module_tx_info[ i ] << '\n';
                  }

                  verified = true;
                  continue;
               }
               else if( !verified )
                  throw runtime_error( "unexpected DB identity missing from transaction log" );

               if( tran_id == c_tx_id_module )
                  is_partial = false;
               else if( ( tran_id >= c_tx_id_initial ) && is_new && is_partial )
                  throw runtime_error( "cannot restore from empty DB with a partial transaction log" );

               if( ( tran_id > c_tx_id_module ) && is_new && !performed_init )
               {
                  if( in_trans )
                     transaction_commit( );

                  tline = line;

                  in_trans = false;
                  performed_init = true;

                  // NOTE: Set the special transaction id for initial data records.
                  set_transaction_id( c_tx_id_initial );

                  record_initialiser init( new_logf );

                  bool old_skip_fetches = session_skip_fk_fetches( );

                  session_skip_fk_fetches( true );

                  if( name == c_meta_model_name )
                  {
                     ifstream std_inpf( "Meta_init_std.cin" );

                     if( !std_inpf )
                        throw runtime_error( "unable to open Meta_init_std.cin for input" );

                     string std_next;

                     // FUTURE: This message should be handled as a server string message.
                     handler.output_progress( "Restoring standard Meta records..." );

                     while( getline( std_inpf, std_next ) )
                     {
                        if( std_next.empty( ) )
                           continue;

                        if( std_next[ 0 ] != ';' )
                           handler.execute_command( std_next );

                        if( !socket_handler.get_restore_error( ).empty( ) )
                           throw runtime_error( "unexpected error: " + socket_handler.get_restore_error( )
                            + "\nprocessing transaction log line #" + to_string( line ) + " with std ==> " + std_next );
                     }

                     // NOTE: Optional extra initial data records.
                     if( file_exists( "Meta_init_extra.cin" ) )
                     {
                        ifstream extra_inpf( "Meta_init_extra.cin" );

                        if( !extra_inpf )
                           throw runtime_error( "unable to open Meta_init_extra.cin for input" );

                        string extra_next;

                        // FUTURE: This message should be handled as a server string message.
                        handler.output_progress( "Restoring additional Meta records..." );

                        while( getline( extra_inpf, extra_next ) )
                        {
                           if( extra_next.empty( ) )
                              continue;

                           if( extra_next[ 0 ] != ';' )
                              handler.execute_command( extra_next );

                           if( !socket_handler.get_restore_error( ).empty( ) )
                              throw runtime_error( "unexpected error: " + socket_handler.get_restore_error( )
                               + "\nprocessing transaction log line #" + to_string( line ) + " with extra ==> " + extra_next );
                        }
                     }
                  }
                  else
                  {
                     for( size_t i = 0; i < module_list.size( ); i++ )
                     {
                        string module_init_list( module_list[ i ] + ".init.lst" );

                        if( exists_file( module_init_list ) )
                        {
                           vector< string > init_classes;
                           buffer_file_lines( module_init_list, init_classes );

                           for( size_t j = 0; j < init_classes.size( ); j++ )
                           {
                              string bulk_init_cmd( "perform_bulk_ops init " );

                              bulk_init_cmd += "20011111111002 ";
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

                  session_skip_fk_fetches( old_skip_fetches );
               }

               // NOTE: Any operation whose transaction id is less than standard is skipped during a restore.
               if( !tran_info.empty( ) && ( tran_id >= c_tx_id_standard ) )
               {
                  if( first_op )
                  {
                     first_op = false;

                     if( is_new )
                        start_tran_id = next_transaction_id( );
                  }
                  else if( tran_id > last_tran_id )
                     ++new_tran_id;

                  last_tran_id = tran_id;

                  if( tran_info[ 0 ] == ';' )
                     storage_comment( tran_info.substr( 1 ) );

                  if( tran_info[ 0 ] != ';' && ( is_new || tran_id >= start_tran_id ) )
                  {
                     set_transaction_id( new_tran_id );

                     handler.execute_command( tran_info );
                  }

                  if( is_new )
                     new_logf << '[' << new_tran_id << ']' << tran_info << '\n';

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
            if( next_transaction_id( ) < ( c_tx_id_standard - 1 ) )
               set_transaction_id( c_tx_id_standard - 1 );

            storage_unlock_all_tables( );

            session_skip_fk_fetches( false );
            session_skip_validation( false );
            session_skip_is_constained( false );

            set_trace_flags( original_trace_flags );

            if( !rebuild && !partial )
            {
               remove_files( sav_db_file_names, ' ' );

               remove_file( sav_sql_name );

               if( exists_file( sav_log_name ) )
                  remove_file( sav_log_name );

               if( exists_file( sav_ltf_name ) )
                  remove_file( sav_ltf_name );

               if( exists_file( sav_dead_keys_name ) )
                  remove_file( sav_dead_keys_name );

               if( exists_file( sav_demo_keys_name ) )
                  remove_file( sav_demo_keys_name );

               remove_file( backup_sql_name );
            }

            term_storage( handler );
            socket_handler.get_transformations( ).clear( );

            set_session_variable( get_special_var_name( e_special_var_restore ), "" );
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

            set_session_variable( get_special_var_name( e_special_var_restore ), "" );

            throw;
         }
      }
      else if( command == c_cmd_ciyam_session_storage_variable )
      {
         string name( get_parm_val( parameters, c_cmd_ciyam_session_storage_variable_name ) );
         bool has_new_val = has_parm_val( parameters, c_cmd_ciyam_session_storage_variable_new_value );
         string new_value( get_parm_val( parameters, c_cmd_ciyam_session_storage_variable_new_value ) );

         possibly_expected_error = true;

         if( has_new_val )
         {
            check_not_possible_protocol_response( new_value );

            set_storage_variable( name, new_value );
         }
         else
            response = get_storage_variable( name );
      }
      else if( command == c_cmd_ciyam_session_storage_web_root )
      {
         bool expand = has_parm_val( parameters, c_cmd_ciyam_session_storage_web_root_expand );

         string new_root( get_parm_val( parameters, c_cmd_ciyam_session_storage_web_root_new_root ) );

         if( !new_root.empty( ) )
            storage_web_root( new_root );

         response = storage_web_root( expand );
      }
      else if( command == c_cmd_ciyam_session_storage_log_slice )
      {
         string name( get_parm_val( parameters, c_cmd_ciyam_session_storage_log_slice_name ) );
         string modules( get_parm_val( parameters, c_cmd_ciyam_session_storage_log_slice_modules ) );

         vector< string > module_list;
         if( !modules.empty( ) )
            split( modules, module_list );

         slice_storage_log( handler, name, module_list );
      }
      else if( command == c_cmd_ciyam_session_storage_bulk_start )
      {
         bool is_write = has_parm_val( parameters, c_cmd_ciyam_session_storage_bulk_start_write );

         storage_bulk_start( is_write );
      }
      else if( command == c_cmd_ciyam_session_storage_dump_cache )
      {
         dump_storage_cache( osstr );

         if( has_session_variable( get_special_var_name( e_special_var_single_string_response ) ) )
            response = osstr.str( );
         else
            output_response_lines( socket, osstr.str( ) );
      }
      else if( command == c_cmd_ciyam_session_storage_dump_locks )
      {
         dump_storage_locks( osstr );

         if( has_session_variable( get_special_var_name( e_special_var_single_string_response ) ) )
            response = osstr.str( );
         else
            output_response_lines( socket, osstr.str( ) );
      }
      else if( command == c_cmd_ciyam_session_storage_log_splice )
      {
         string name( get_parm_val( parameters, c_cmd_ciyam_session_storage_log_splice_name ) );
         string modules( get_parm_val( parameters, c_cmd_ciyam_session_storage_log_splice_modules ) );

         vector< string > module_list;
         if( !modules.empty( ) )
            split( modules, module_list );

         splice_storage_log( handler, name, module_list );
      }
      else if( command == c_cmd_ciyam_session_storage_bulk_finish )
         storage_bulk_finish( );
      else if( command == c_cmd_ciyam_session_storage_lock_create )
      {
         string type( get_parm_val( parameters, c_cmd_ciyam_session_storage_lock_create_type ) );
         string class_id( get_parm_val( parameters, c_cmd_ciyam_session_storage_lock_create_class_id ) );
         string instance( get_parm_val( parameters, c_cmd_ciyam_session_storage_lock_create_instance ) );
         string num_tries( get_parm_val( parameters, c_cmd_ciyam_session_storage_lock_create_num_tries ) );

         possibly_expected_error = true;

         size_t num_tries_val = c_cmd_ciyam_session_storage_lock_create_num_tries_default;

         if( !num_tries.empty( ) )
            num_tries_val = from_string< size_t >( num_tries );

         response = to_string( obtain_storage_lock( type, class_id, instance, num_tries_val ) );
      }
      else if( command == c_cmd_ciyam_session_storage_lock_modify )
      {
         string handle( get_parm_val( parameters, c_cmd_ciyam_session_storage_lock_modify_handle ) );
         string new_type( get_parm_val( parameters, c_cmd_ciyam_session_storage_lock_modify_new_type ) );
         string num_tries( get_parm_val( parameters, c_cmd_ciyam_session_storage_lock_modify_num_tries ) );

         possibly_expected_error = true;

         size_t num_tries_val = c_cmd_ciyam_session_storage_lock_modify_num_tries_default;

         if( !num_tries.empty( ) )
            num_tries_val = from_string< size_t >( num_tries );

         transform_obtained_lock( from_string< size_t >( handle ), new_type, num_tries_val );
      }
      else if( command == c_cmd_ciyam_session_storage_lock_remove )
      {
         string handle( get_parm_val( parameters, c_cmd_ciyam_session_storage_lock_remove_handle ) );

         release_obtained_lock( from_string< size_t >( handle ) );
      }
      else if( command == c_cmd_ciyam_session_storage_cache_clear )
         storage_cache_clear( );
      else if( command == c_cmd_ciyam_session_storage_cache_limit )
      {
         string new_limit( get_parm_val( parameters, c_cmd_ciyam_session_storage_cache_limit_new_limit ) );

         size_t cache_limit;
         if( new_limit.empty( ) )
            cache_limit = storage_cache_limit( );
         else
            cache_limit = storage_cache_limit( from_string< size_t >( new_limit ) );

         response = to_string( cache_limit );
      }
      else if( command == c_cmd_ciyam_session_storage_export_data )
         export_storage( handler );
      else if( command == c_cmd_ciyam_session_storage_file_export )
      {
         string hash( get_parm_val( parameters, c_cmd_ciyam_session_storage_file_export_hash ) );
         string module( get_parm_val( parameters, c_cmd_ciyam_session_storage_file_export_module ) );
         string mclass( get_parm_val( parameters, c_cmd_ciyam_session_storage_file_export_mclass ) );
         string filename( get_parm_val( parameters, c_cmd_ciyam_session_storage_file_export_filename ) );

         module = resolve_module_id( module );
         mclass = resolve_class_id( module, mclass );

         string filepath( get_attached_file_path( module, mclass, filename ) );

         create_directories_for_file_name( filepath );

         extract_file( hash, filepath );
      }
      else if( command == c_cmd_ciyam_session_storage_file_import )
      {
         string module( get_parm_val( parameters, c_cmd_ciyam_session_storage_file_import_module ) );
         string mclass( get_parm_val( parameters, c_cmd_ciyam_session_storage_file_import_mclass ) );
         string filename( get_parm_val( parameters, c_cmd_ciyam_session_storage_file_import_filename ) );
         string tag( get_parm_val( parameters, c_cmd_ciyam_session_storage_file_import_tag ) );

         module = resolve_module_id( module );
         mclass = resolve_class_id( module, mclass );

         string filepath( get_attached_file_path( module, mclass, filename ) );

         string data( c_file_type_str_blob );
         data += buffer_file( filepath );

         response = create_raw_file( data, true, tag.empty( ) ? 0 : tag.c_str( ) );
      }
      else if( command == c_cmd_ciyam_session_storage_channel_list )
      {
         ostringstream osstr;

         storage_channel_list( osstr );

         response = osstr.str( );
      }
      else if( command == c_cmd_ciyam_session_storage_channel_create )
      {
         string identity( get_parm_val( parameters, c_cmd_ciyam_session_storage_channel_create_identity ) );
         string channel_information( get_parm_val( parameters, c_cmd_ciyam_session_storage_channel_create_channel_information ) );

         storage_channel_create( identity.c_str( ), channel_information.c_str( ) );
      }
      else if( command == c_cmd_ciyam_session_storage_channel_destroy )
      {
         string identity( get_parm_val( parameters, c_cmd_ciyam_session_storage_channel_destroy_identity ) );

         storage_channel_destroy( identity.c_str( ) );
      }
      else if( command == c_cmd_ciyam_session_storage_channel_documents )
      {
         bool open = has_parm_val( parameters, c_cmd_ciyam_session_storage_channel_documents_open );
         bool close = has_parm_val( parameters, c_cmd_ciyam_session_storage_channel_documents_close );
         bool opened = has_parm_val( parameters, c_cmd_ciyam_session_storage_channel_documents_opened );
         bool flagged = has_parm_val( parameters, c_cmd_ciyam_session_storage_channel_documents_flagged );
         bool pending = has_parm_val( parameters, c_cmd_ciyam_session_storage_channel_documents_pending );
         bool waiting = has_parm_val( parameters, c_cmd_ciyam_session_storage_channel_documents_waiting );
         bool retrieved = has_parm_val( parameters, c_cmd_ciyam_session_storage_channel_documents_retrieved );
         bool reviewing = has_parm_val( parameters, c_cmd_ciyam_session_storage_channel_documents_reviewing );
         bool submitting = has_parm_val( parameters, c_cmd_ciyam_session_storage_channel_documents_submitting );
         bool height_retrieved = has_parm_val( parameters, c_cmd_ciyam_session_storage_channel_documents_height_retrieved );
         bool height_submitted = has_parm_val( parameters, c_cmd_ciyam_session_storage_channel_documents_height_submitted );
         bool update_retrieved = has_parm_val( parameters, c_cmd_ciyam_session_storage_channel_documents_update_retrieved );
         bool update_submitted = has_parm_val( parameters, c_cmd_ciyam_session_storage_channel_documents_update_submitted );
         bool prepare_for_submit = has_parm_val( parameters, c_cmd_ciyam_session_storage_channel_documents_prepare_for_submit );
         string identity( get_parm_val( parameters, c_cmd_ciyam_session_storage_channel_documents_identity ) );

         channel_documents_type specific_type = e_channel_documents_type_submitting;

         if( pending )
            specific_type = e_channel_documents_type_pending;
         else if( waiting )
            specific_type = e_channel_documents_type_waiting;
         else if( retrieved )
            specific_type = e_channel_documents_type_retrieved;
         else if( reviewing )
            specific_type = e_channel_documents_type_reviewing;

         if( open )
            storage_channel_documents_open( identity.c_str( ) );
         else if( close )
            storage_channel_documents_close( identity.c_str( ) );
         else if( opened )
            response = to_string( storage_channel_documents_opened( identity ) );
         else if ( !flagged && !update_retrieved && !update_submitted && !prepare_for_submit )
            response = storage_channel_documents( identity, height_retrieved | height_submitted, height_retrieved );
         else if( update_retrieved || update_submitted )
            response = storage_channel_documents_update( identity, update_submitted );
         else if( !flagged )
            response = storage_channel_documents_prepare( identity );
         else
            response = storage_channel_documents_specific( identity, specific_type );
      }
      else if( command == c_cmd_ciyam_session_storage_datachain_details )
      {
         bool list = has_parm_val( parameters, c_cmd_ciyam_session_storage_datachain_details_list );
         string identity( get_parm_val( parameters, c_cmd_ciyam_session_storage_datachain_details_identity ) );
         string data_type( get_parm_val( parameters, c_cmd_ciyam_session_storage_datachain_details_data_type ) );
         bool height = has_parm_val( parameters, c_cmd_ciyam_session_storage_datachain_details_height );
         string new_height( get_parm_val( parameters, c_cmd_ciyam_session_storage_datachain_details_new_height ) );
         bool remove = has_parm_val( parameters, c_cmd_ciyam_session_storage_datachain_details_remove );
         bool is_link = has_parm_val( parameters, c_cmd_ciyam_session_storage_datachain_details_link );
         bool is_unlink = has_parm_val( parameters, c_cmd_ciyam_session_storage_datachain_details_unlink );
         string channel( get_parm_val( parameters, c_cmd_ciyam_session_storage_datachain_details_channel ) );

         if( list )
         {
            if( data_type.empty( ) )
            {
               ostringstream osstr;

               list_datachains( osstr );

               response = osstr.str( );
            }
            else
            {
               size_t type_value = from_string< size_t >( data_type );

               vector< string > datachains;

               list_datachains( datachains );

               for( size_t i = 0; i < datachains.size( ); i++ )
               {
                  string next( datachains[ i ] );

                  size_t next_type;

                  get_datachain_info( next, &next_type );

                  if( next_type == type_value )
                  {
                     if( !response.empty( ) )
                        response += '\n';
                     response += next;
                  }
               }
            }
         }
         else
         {
            if( is_link )
               link_channel_to_datachain( channel, identity );
            else if( is_unlink )
               unlink_channel_from_datachain( channel, identity );
            else if( remove )
               remove_datachain_info( identity );
            else if( !data_type.empty( ) )
               create_datachain_info( identity, from_string< size_t >( data_type ) );
            else if( !new_height.empty( ) )
               set_datachain_height( identity, from_string< size_t >( new_height ) );
            else
            {
               size_t current_height;

               response = get_datachain_info( identity, 0, &current_height );

               if( height )
                  response = to_string( current_height );
            }
         }
      }
      else if( command == c_cmd_ciyam_session_storage_transaction_start )
         transaction_start( );
      else if( command == c_cmd_ciyam_session_storage_transaction_commit )
         transaction_commit( );
      else if( command == c_cmd_ciyam_session_storage_transaction_rollback )
         transaction_rollback( );
      else if( command == c_cmd_ciyam_session_system_trace )
      {
         string new_flags( get_parm_val( parameters, c_cmd_ciyam_session_system_trace_new_flags ) );

         if( !new_flags.empty( ) )
         {
            IF_NOT_IS_TRACING( TRACE_DETAILS | TRACE_SESSION )
               log_trace_message( TRACE_DETAILS | TRACE_SESSION, "trace " + new_flags );

            istringstream isstr( new_flags );

            uint32_t trace_flags;
            isstr >> hex >> trace_flags;

            set_trace_flags( trace_flags );
         }

         ostringstream osstr;
         osstr << hex << get_trace_flags( );

         response = osstr.str( );
      }
      else if( command == c_cmd_ciyam_session_system_gen_key )
      {
         string num_keys( get_parm_val( parameters, c_cmd_ciyam_session_system_gen_key_num_keys ) );

         size_t num_val = 1;

         if( !num_keys.empty( ) )
            num_val = from_string< size_t >( num_keys );

         for( size_t i = 0; i < num_val; i++ )
         {
            if( i > 0 )
               response += '\n';
            response += gen_key( );
         }
      }
      else if( command == c_cmd_ciyam_session_system_mutexes )
      {
         ostringstream osstr;

         list_trace_mutex_lock_ids( osstr, &g_mutex, "ciyam_session::g_mutex = " );

         response = osstr.str( );
      }
      else if( command == c_cmd_ciyam_session_system_version )
         response = c_protocol_version;
      else if( command == c_cmd_ciyam_session_system_identity )
      {
         bool is_raw = has_parm_val( parameters, c_cmd_ciyam_session_system_identity_raw );
         string info( get_parm_val( parameters, c_cmd_ciyam_session_system_identity_info ) );
         string pubkey( get_parm_val( parameters, c_cmd_ciyam_session_system_identity_pubkey ) );
         string encrypted( get_parm_val( parameters, c_cmd_ciyam_session_system_identity_encrypted ) );

         const char* p_pubkey = 0;

         if( !pubkey.empty( ) )
         {
            if( pubkey == c_dummy )
               pubkey.erase( );

            p_pubkey = pubkey.c_str( );
         }

         if( !info.empty( ) && !pubkey.empty( ) )
         {
            info.reserve( c_secret_reserve_size );
            session_shared_decrypt( info, pubkey, info );

            // NOTE: If using the 'init_identity' application protocol script then
            // the entropy can follow the password after a separator which will be
            // either one (or if the password itself contains spaces) two spaces.
            if( !encrypted.empty( ) && ( info.size( ) >= 32 ) )
            {
               string::size_type pos = info.find( c_two_spaces );

               if( pos == string::npos )
                  pos = info.find( ' ' );

               if( pos != string::npos )
                  info.erase( pos );
            }

            if( count( info.begin( ), info.end( ), ' ' ) == 11 )
               get_mnemonics_or_hex_seed( info, info );
         }

         if( !info.empty( ) )
         {
            bool was_locked = socket_handler.is_locked( );

            set_identity( info, encrypted.empty( ) ? 0 : encrypted.c_str( ) );

            bool is_encrypted = false;
            bool has_system_id = has_identity( &is_encrypted );

            if( was_locked && !is_encrypted && has_system_id )
               socket_handler.unlock_identity( );
         }

         clear_key( info );

         get_identity( response, !is_raw, false, p_pubkey );
      }
      else if( command == c_cmd_ciyam_session_system_log_tail )
      {
         string lines( get_parm_val( parameters, c_cmd_ciyam_session_system_log_tail_lines ) );
         bool is_backup = has_parm_val( parameters, c_cmd_ciyam_session_system_log_tail_backup );
         bool is_script = has_parm_val( parameters, c_cmd_ciyam_session_system_log_tail_script );
         bool is_server = has_parm_val( parameters, c_cmd_ciyam_session_system_log_tail_server );
         bool is_restore = has_parm_val( parameters, c_cmd_ciyam_session_system_log_tail_restore );
         bool is_interface = has_parm_val( parameters, c_cmd_ciyam_session_system_log_tail_interface );
         bool is_application = has_parm_val( parameters, c_cmd_ciyam_session_system_log_tail_application );
         string application_name( get_parm_val( parameters, c_cmd_ciyam_session_system_log_tail_application_name ) );

         possibly_expected_error = true;

         unsigned int num = c_cmd_ciyam_session_system_log_tail_lines_default;

         if( !lines.empty( ) )
            num = atoi( lines.c_str( ) );

         string log_file_name;

         if( is_script || is_server )
            log_file_name = get_log_files_dir( );
         else if( !is_application )
            log_file_name = get_web_root( );

         if( !log_file_name.empty( ) )
            log_file_name += '/';

         if( is_script || is_server )
            log_file_name += ( is_script ? c_ciyam_script : c_ciyam_server );
         else if( is_application )
         {
            if( !application_name.empty( ) )
               log_file_name += application_name;
            else
               log_file_name += c_meta_model_name;
         }
         else
         {
            if( is_backup || is_restore || application_name.empty( ) )
               log_file_name += c_meta_app_directory;
            else
               log_file_name += lower( application_name );

            log_file_name += '/';

            if( is_backup || is_restore )
               log_file_name += ( is_backup ? c_backup : c_restore );
            else
               log_file_name += c_ciyam_interface;
         }

         log_file_name += c_log_file_ext;

         if( is_backup || is_restore )
            log_file_name += c_sav_file_ext;

         deque< string > log_lines;

         buffer_file_tail( log_file_name, log_lines, num );

         for( size_t i = 0; i < log_lines.size( ); i++ )
         {
            if( i > 0 )
               response += '\n';
            response += log_lines[ i ];
         }
      }
      else if( command == c_cmd_ciyam_session_system_notifier )
      {
         bool is_start = has_parm_val( parameters, c_cmd_ciyam_session_system_notifier_start );
         bool is_finish = has_parm_val( parameters, c_cmd_ciyam_session_system_notifier_finish );
         bool is_suspend = has_parm_val( parameters, c_cmd_ciyam_session_system_notifier_suspend );
         bool is_unsuspend = has_parm_val( parameters, c_cmd_ciyam_session_system_notifier_unsuspend );
         string file_or_directory( get_parm_val( parameters, c_cmd_ciyam_session_system_notifier_file_or_directory ) );

         if( is_finish )
         {
            string var_name( c_notifier_prefix + file_or_directory );

            if( !has_raw_system_variable( var_name ) )
               var_name += '/';

            // NOTE: As a notifier can delete itself will just
            // do nothing if no system variable is found.
            if( has_raw_system_variable( var_name ) )
            {
               set_system_variable( var_name, c_finishing, c_watching );
               set_system_variable( var_name, c_finishing, c_suspended );

               bool okay = false;

               for( size_t i = 0; i < c_max_notifer_checks; i++ )
               {
                  msleep( c_notifer_check_wait );

                  if( !has_raw_system_variable( var_name ) )
                  {
                     okay = true;
                     break;
                  }
               }

               if( !okay )
                  throw runtime_error( "system variable not cleared for '" + file_or_directory + "'" );
            }
         }
         else if( is_suspend )
         {
            string var_name( c_notifier_prefix + file_or_directory );

            if( !has_raw_system_variable( var_name ) )
               var_name += '/';

            if( get_raw_system_variable( var_name ) == c_watching )
               set_system_variable( var_name, c_suspended );
         }
         else if( is_unsuspend )
         {
            string var_name( c_notifier_prefix + file_or_directory );

            if( !has_raw_system_variable( var_name ) )
               var_name += '/';

            if( get_raw_system_variable( var_name ) == c_suspended )
               set_system_variable( var_name, c_watching );
         }
         else
         {
            string file( file_or_directory );
            string directory( file_or_directory + '/' );

            if( has_raw_system_variable( file )
             || has_raw_system_variable( directory ) )
               throw runtime_error( "detected conflicting system variable for '" + file_or_directory + "'" );

            ciyam_notifier* p_notifier = new ciyam_notifier( file_or_directory );

            p_notifier->start( );

            bool okay = false;

            for( size_t i = 0; i < c_max_notifer_checks; i++ )
            {
               msleep( c_notifer_check_wait );

               if( has_raw_system_variable( c_notifier_prefix + file )
                || has_raw_system_variable( c_notifier_prefix + directory ) )
               {
                  okay = true;
                  break;
               }
            }

            if( !okay )
               throw runtime_error( "system variable not found for '" + file_or_directory + "'" );
         }
      }
      else if( command == c_cmd_ciyam_session_system_passtotp )
      {
         string secret( get_parm_val( parameters, c_cmd_ciyam_session_system_passtotp_secret ) );

         // NOTE: A secret can be stored by encrypting it (i.e. "encrypt") and storing it as a "system variable"
         // whose name is prefixed by "passtotp.". To identify that the secret has been thus stored use a '*' as
         // the first character of "secret" followed by the suffix of the "passtotp.*" variable (so for the name
         // "passtotp.test" you would use a secret "*test".
         if( !secret.empty( ) && ( secret[ 0 ] == '*' ) )
            decrypt_data( secret, get_system_variable( string( c_passtotp_prefix ) + secret.substr( 1 ) ) );

         response = get_totp( secret );

         clear_key( secret );
      }
      else if( command == c_cmd_ciyam_session_system_password )
      {
         string name( get_parm_val( parameters, c_cmd_ciyam_session_system_password_name ) );

         if( name == "gpg" )
            response = get_encrypted_gpg_password( );
         else if( name == "pem" )
            response = get_encrypted_pem_password( );
         else if( name == "sql" )
            response = get_encrypted_sql_password( );
         else if( name == "pop3" )
            response = get_encrypted_pop3_password( );
         else if( name == "smtp" )
            response = get_encrypted_smtp_password( );
         else
            throw runtime_error( "unknown system password name '" + name + "'" );
      }
      else if( command == c_cmd_ciyam_session_system_schedule )
      {
         bool reload = has_parm_val( parameters, c_cmd_ciyam_session_system_schedule_reload );
         bool from_now = has_parm_val( parameters, c_cmd_ciyam_session_system_schedule_from_now );

         if( reload )
         {
            set_system_variable( get_special_var_name( e_special_var_autoscript_reload ), c_true_value );

            // NOTE: Wait here long enough for the schedule to have been reconstructed before it is output.
            msleep( c_auto_script_msleep * 2 );
         }

         output_schedule( osstr, from_now );

         if( has_session_variable( get_special_var_name( e_special_var_single_string_response ) ) )
            response = osstr.str( );
         else
            output_response_lines( socket, osstr.str( ) );
      }
      else if( command == c_cmd_ciyam_session_system_sendmail )
      {
         string to( get_parm_val( parameters, c_cmd_ciyam_session_system_sendmail_to ) );
         string subject( get_parm_val( parameters, c_cmd_ciyam_session_system_sendmail_subject ) );
         string message( get_parm_val( parameters, c_cmd_ciyam_session_system_sendmail_message ) );
         string tz_name( get_parm_val( parameters, c_cmd_ciyam_session_system_sendmail_tz_name ) );
         string file_names( get_parm_val( parameters, c_cmd_ciyam_session_system_sendmail_file_names ) );
         string html_source( get_parm_val( parameters, c_cmd_ciyam_session_system_sendmail_html_source ) );
         string image_names( get_parm_val( parameters, c_cmd_ciyam_session_system_sendmail_image_names ) );
         string image_prefix( get_parm_val( parameters, c_cmd_ciyam_session_system_sendmail_image_prefix ) );

         if( tz_name.empty( ) )
            tz_name = get_timezone( );

         vector< string > all_file_names;
         if( !file_names.empty( ) )
            split( file_names, all_file_names );

         vector< string > all_image_names;
         if( !image_names.empty( ) )
            split( image_names, all_image_names );

         if( !message.empty( ) && ( message[ 0 ] == '@' ) )
         {
            if( exists_file( message.substr( 1 ) ) )
               message = buffer_file( message.substr( 1 ) );
         }

         send_email_message( to, subject, message,
          html_source, 0, &all_file_names, &tz_name, &all_image_names, &image_prefix );
      }
      else if( command == c_cmd_ciyam_session_system_variable )
      {
         string name_or_expr( get_parm_val( parameters, c_cmd_ciyam_session_system_variable_name_or_expr ) );
         bool has_new_val = has_parm_val( parameters, c_cmd_ciyam_session_system_variable_new_value );
         string new_value( get_parm_val( parameters, c_cmd_ciyam_session_system_variable_new_value ) );
         bool has_check_val = has_parm_val( parameters, c_cmd_ciyam_session_system_variable_check_value );
         string check_value( get_parm_val( parameters, c_cmd_ciyam_session_system_variable_check_value ) );

         possibly_expected_error = true;

         if( has_new_val )
         {
            string null_value( get_special_var_name( e_special_var_null ) );

            if( new_value == null_value )
               new_value.erase( );

            if( check_value == null_value )
               check_value.erase( );

            check_not_possible_protocol_response( new_value );

            if( !has_check_val )
               set_system_variable( name_or_expr, new_value, false, &handler );
            else
               response = to_string( set_system_variable( name_or_expr, new_value, check_value ) );
         }
         else
         {
            response = get_system_variable( name_or_expr, false );

            check_not_possible_protocol_response( response );
         }
      }
      else if( command == c_cmd_ciyam_session_system_checkmail )
      {
         string headers( get_parm_val( parameters, c_cmd_ciyam_session_system_checkmail_headers ) );
         bool create_script = has_parm_val( parameters, c_cmd_ciyam_session_system_checkmail_create_script );

         vector< string > email_headers;
         if( !headers.empty( ) )
            split( headers, email_headers );

         handler.output_progress( ";checkmail..." );
         response = check_email_headers( email_headers, create_script );
      }
      else if( command == c_cmd_ciyam_session_system_externals )
         response = list_externals( );
      else if( command == c_cmd_ciyam_session_system_listeners )
      {
         ostringstream osstr;

         list_listeners( osstr );

         response = osstr.str( );
      }
      else if( command == c_cmd_ciyam_session_system_ntfy_send )
      {
         string uid( get_parm_val( parameters, c_cmd_ciyam_session_system_ntfy_send_uid ) );
         string message( get_parm_val( parameters, c_cmd_ciyam_session_system_ntfy_send_message ) );

         send_ntfy_message( uid, message, true );
      }
      else if( command == c_cmd_ciyam_session_system_ntfy_topic )
      {
         string uid( get_parm_val( parameters, c_cmd_ciyam_session_system_ntfy_topic_uid ) );

         response = ntfy_topic( uid );
      }
      else if( command == c_cmd_ciyam_session_system_run_script )
      {
         string script_name( get_parm_val( parameters, c_cmd_ciyam_session_system_run_script_script_name ) );
         string arg_val_pairs( get_parm_val( parameters, c_cmd_ciyam_session_system_run_script_arg_val_pairs ) );

         possibly_expected_error = true;

         bool async = true;

         if( !script_name.empty( ) && ( script_name[ 0 ] == '!' ) )
         {
            async = false;
            script_name.erase( 0, 1 );
         }

         if( script_name.find_first_of( "?*" ) == string::npos )
         {
            vector< ref_count_ptr< temporary_session_variable > > temporary_session_variables;

            if( !arg_val_pairs.empty( ) )
            {
               vector< string > pairs;

               split( arg_val_pairs, pairs );

               for( size_t i = 0; i < pairs.size( ); i++ )
               {
                  string next( pairs[ i ] );
                  string::size_type pos = next.find( '=' );

                  if( pos == string::npos )
                     throw runtime_error( "invalid arg and value pair format '" + next + "'" );

                  temporary_session_variables.push_back(
                   ref_count_ptr< temporary_session_variable >(
                   new temporary_session_variable( next.substr( 0, pos ), next.substr( pos + 1 ) ) ) );
               }
            }

            check_script_args( script_name );

            int rc = run_script( script_name, async );

            if( rc < 0 )
               throw runtime_error( "failed to execute script '" + script_name + "'" );
         }
         else
         {
            ostringstream osstr;

            list_scripts( script_name, osstr );
            response = osstr.str( );
         }
      }
      else if( command == c_cmd_ciyam_session_system_smtp_owner )
      {
         response = get_smtp_username( );

         if( response.find( '@' ) == string::npos )
         {
            string suffix_or_domain( get_smtp_suffix_or_domain( ) );

            if( !suffix_or_domain.empty( ) )
               response += "@" + suffix_or_domain;
         }
      }
      else if( command == c_cmd_ciyam_session_system_time_zones )
      {
         string own_tz( get_timezone( ) );

         response = list_time_zones( );

         if( response.find( own_tz + ' ' ) != string::npos )
            replace( response, own_tz + ' ', "*" + own_tz + ' ' );
      }
      else if( command == c_cmd_ciyam_session_system_export_repo )
      {
         // NOTE: To make sure the console client doesn't time out issue a progress message.
         handler.output_progress( "(exporting repository entries)" );

         export_repository_entries( );
      }
      else if( command == c_cmd_ciyam_session_system_trace_flags )
      {
         bool list = has_parm_val( parameters, c_cmd_ciyam_session_system_trace_flags_list );
         bool unset = has_parm_val( parameters, c_cmd_ciyam_session_system_trace_flags_unset );
         string names( get_parm_val( parameters, c_cmd_ciyam_session_system_trace_flags_names ) );

         if( list )
         {
            ostringstream osstr;

            list_trace_flags( osstr );

            response = osstr.str( );
         }
         else
         {
            if( !names.empty( ) )
            {
               string cmd( "trace_flags " );

               if( !unset )
                  cmd += "-set";
               else
                  cmd += "-unset";

               IF_NOT_IS_TRACING( TRACE_DETAILS | TRACE_SESSION )
                  log_trace_message( TRACE_DETAILS | TRACE_SESSION, cmd + ' ' + names );

               trace_flag_names( names, unset );
            }
            else
               response = get_trace_flag_names( );
         }
      }
      else if( command == c_cmd_ciyam_session_system_trace_level )
      {
         string new_level( get_parm_val( parameters, c_cmd_ciyam_session_system_trace_level_new_level ) );

         if( new_level.empty( ) )
            response = get_trace_level( );
         else
         {
            IF_NOT_IS_TRACING( TRACE_DETAILS | TRACE_SESSION )
               log_trace_message( TRACE_DETAILS | TRACE_SESSION, "trace_level " + new_level );

            set_trace_level( new_level );
         }
      }
      else if( command == c_cmd_ciyam_session_system_trace_levels )
      {
         vector< string > trace_level_names;

         list_trace_levels( trace_level_names );

         for( size_t i = 0; i < trace_level_names.size( ); i++ )
         {
            if( i > 0 )
               response += '\n';

            response += trace_level_names[ i ];
         }
      }
      else if( command == c_cmd_ciyam_session_system_trace_message )
      {
         string message_flag( get_parm_val( parameters, c_cmd_ciyam_session_system_trace_message_flag ) );
         string message_text( get_parm_val( parameters, c_cmd_ciyam_session_system_trace_message_text ) );

         uint64_t trace_flag = TRACE_MINIMAL;

         if( !message_flag.empty( ) )
         {
            istringstream isstr( message_flag );

            isstr >> hex >> trace_flag;
         }

         if( get_trace_flags( ) & trace_flag )
            log_trace_message( trace_flag, message_text );
      }
      else if( command == c_cmd_ciyam_session_test )
      {
         string arg_1( get_parm_val( parameters, c_cmd_ciyam_session_test_arg_1 ) );
         string arg_2( get_parm_val( parameters, c_cmd_ciyam_session_test_arg_2 ) );
         string arg_3( get_parm_val( parameters, c_cmd_ciyam_session_test_arg_3 ) );

         // NOTE: Dummy command for testing or prototyping.
         response = "arg_1 = " + arg_1 + "\narg_2 = " + arg_2 + "\narg_3 = " + arg_3;
      }
      else if( command == c_cmd_ciyam_session_utc_now )
         response = date_time::standard( ).as_string( true, false );
      else if( command == c_cmd_ciyam_session_utc_offset )
      {
         string tz_name( get_parm_val( parameters, c_cmd_ciyam_session_utc_offset_tz_name ) );
         string local_time( get_parm_val( parameters, c_cmd_ciyam_session_utc_offset_local_time ) );

         float offset;
         get_tz_info( date_time( local_time ), tz_name, offset );

         response = tz_name + " " + to_string( offset );
      }
      else if( command == c_cmd_ciyam_session_utc_to_local )
      {
         string tz_name( get_parm_val( parameters, c_cmd_ciyam_session_utc_to_local_tz_name ) );
         string utc_time( get_parm_val( parameters, c_cmd_ciyam_session_utc_to_local_utc_time ) );

         date_time local( utc_to_local( date_time( utc_time ), tz_name ) );

         response = local.as_string( e_time_format_hhmm, true ) + " " + tz_name;
      }
      else if( command == c_cmd_ciyam_session_utc_from_local )
      {
         string tz_name( get_parm_val( parameters, c_cmd_ciyam_session_utc_from_local_tz_name ) );
         string local_time( get_parm_val( parameters, c_cmd_ciyam_session_utc_from_local_local_time ) );

         date_time utc( local_to_utc( date_time( local_time ), tz_name ) );

         response = utc.as_string( e_time_format_hhmm, true );
      }
      else if( command == c_cmd_ciyam_session_utc_to_unix_time )
      {
         string utc_time( get_parm_val( parameters, c_cmd_ciyam_session_utc_to_unix_time_utc_time ) );

         date_time utc;

         if( utc_time != c_dtm_now )
            utc = date_time( utc_time );
         else
            utc = date_time::standard( );

         response = to_string( unix_time( utc ) );
      }
      else if( command == c_cmd_ciyam_session_utc_from_unix_time )
      {
         string unix_time( get_parm_val( parameters, c_cmd_ciyam_session_utc_from_unix_time_unix_time ) );

         date_time utc( from_string< int64_t >( unix_time ) );

         response = utc.as_string( e_time_format_hhmmss, true );
      }
      else if( command == c_cmd_ciyam_session_utils_regex )
      {
         string expr( get_parm_val( parameters, c_cmd_ciyam_session_utils_regex_expr_or_name ) );
         string text( get_parm_val( parameters, c_cmd_ciyam_session_utils_regex_text_to_check ) );
         bool chain = has_parm_val( parameters, c_cmd_ciyam_session_utils_regex_chain );
         bool refs = has_parm_val( parameters, c_cmd_ciyam_session_utils_regex_refs );
         bool input = has_parm_val( parameters, c_cmd_ciyam_session_utils_regex_input );
         bool matched = has_parm_val( parameters, c_cmd_ciyam_session_utils_regex_matched );

         bool rc = false;

         regex_output_type output_type = e_regex_output_type_automatic;

         if( refs )
            output_type = e_regex_output_type_refs;
         else if( input )
            output_type = e_regex_output_type_input;
         else if( matched )
            output_type = e_regex_output_type_matched;

         string found( check_with_regex( expr, text, &rc, output_type, chain ) );

         if( rc )
            response = found;
      }
      else if( command == c_cmd_ciyam_session_utils_decode )
      {
         bool url = has_parm_val( parameters, c_cmd_ciyam_session_utils_decode_url );
         bool text = has_parm_val( parameters, c_cmd_ciyam_session_utils_decode_text );
         string data( get_parm_val( parameters, c_cmd_ciyam_session_utils_decode_data ) );

         if( text )
            response = base64::decode( data, url );
         else
            response = hex_encode( base64::decode( data, url ) );
      }
      else if( command == c_cmd_ciyam_session_utils_encode )
      {
         bool url = has_parm_val( parameters, c_cmd_ciyam_session_utils_encode_url );
         bool text = has_parm_val( parameters, c_cmd_ciyam_session_utils_encode_text );
         string data( get_parm_val( parameters, c_cmd_ciyam_session_utils_encode_data ) );

         if( text )
            response = base64::encode( data, url );
         else
            response = base64::encode( hex_decode( data ), url );
      }
      else if( command == c_cmd_ciyam_session_utils_decrypt )
      {
         bool harden_key = has_parm_val( parameters, c_cmd_ciyam_session_utils_decrypt_harden_key );
         bool pwd_and_data = has_parm_val( parameters, c_cmd_ciyam_session_utils_decrypt_pwd_and_data );
         string data( get_parm_val( parameters, c_cmd_ciyam_session_utils_decrypt_data ) );
         string pubkey( get_parm_val( parameters, c_cmd_ciyam_session_utils_decrypt_pubkey ) );

         if( !data.empty( ) && !pubkey.empty( ) )
         {
            data.reserve( c_secret_reserve_size );
            session_shared_decrypt( data, pubkey, data );
         }

         scoped_clear_key clear_data( data );

         decrypt_data( response, data, false, harden_key, pwd_and_data );

         clear_response = true;

         // NOTE: Decrypted output is expected to be printable (so will verify that it is).
         if( !response.empty( ) )
         {
            bool invalid = false;

            for( size_t i = 0; i < response.size( ); i++ )
            {
               if( ( response[ i ] < ' ' ) || ( response[ i ] > '~' ) )
               {
                  invalid = true;
                  break;
               }
            }

            if( invalid )
               throw runtime_error( "unexpected non-printable response (incorrect password?)" );
         }
      }
      else if( command == c_cmd_ciyam_session_utils_encrypt )
      {
         bool no_ssl = has_parm_val( parameters, c_cmd_ciyam_session_utils_encrypt_no_ssl );
         bool no_salt = has_parm_val( parameters, c_cmd_ciyam_session_utils_encrypt_no_salt );
         bool harden_key = has_parm_val( parameters, c_cmd_ciyam_session_utils_encrypt_harden_key );
         bool pwd_and_data = has_parm_val( parameters, c_cmd_ciyam_session_utils_encrypt_pwd_and_data );
         string data( get_parm_val( parameters, c_cmd_ciyam_session_utils_encrypt_data ) );
         string pubkey( get_parm_val( parameters, c_cmd_ciyam_session_utils_encrypt_pubkey ) );

         if( !data.empty( ) && !pubkey.empty( ) )
         {
            data.reserve( c_secret_reserve_size );
            session_shared_decrypt( data, pubkey, data );
         }

         scoped_clear_key clear_data( data );

         encrypt_data( response, data, no_ssl, false, no_salt, harden_key, pwd_and_data );
      }
      else if( command == c_cmd_ciyam_session_utils_entropy )
      {
         string max_extra( get_parm_val( parameters, c_cmd_ciyam_session_utils_entropy_max_extra ) );
         string minimum_chars( get_parm_val( parameters, c_cmd_ciyam_session_utils_entropy_minimum_chars ) );
         bool numeric = has_parm_val( parameters, c_cmd_ciyam_session_utils_entropy_numeric );
         bool alpha_lower = has_parm_val( parameters, c_cmd_ciyam_session_utils_entropy_alpha_lower );

         size_t max_extra_val = from_string< size_t >( max_extra );
         size_t minimum_chars_val = from_string< size_t >( minimum_chars );

         printable_type type = e_printable_type_alpha_numeric;

         if( numeric )
            type = e_printable_type_numeric;
         else if( alpha_lower )
            type = e_printable_type_alpha_lower;

         response = random_characters( minimum_chars_val, max_extra_val, type );
      }
      else if( command == c_cmd_ciyam_session_utils_soundex )
      {
         bool skip_prefix_specials = has_parm_val( parameters, c_cmd_ciyam_session_utils_soundex_skip_prefix_specials );
         string word_or_phonic_letters( get_parm_val( parameters, c_cmd_ciyam_session_utils_soundex_word_or_phonic_letters ) );

         response = soundex( word_or_phonic_letters, skip_prefix_specials );
      }
   }
   catch( exception& x )
   {
      TRACE_LOG( ( possibly_expected_error
       ? TRACE_INITIAL | TRACE_SESSION : TRACE_MINIMAL ), string( "session error: " ) + x.what( ) );

      set_script_error_if_applicable( x.what( ) );

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
      TRACE_LOG( TRACE_MINIMAL, "session error: " + string( c_unexpected_unknown_exception ) );

      set_script_error_if_applicable( c_unexpected_unknown_exception );

      if( socket_handler.is_restoring( ) )
         socket_handler.set_restore_error( c_unexpected_unknown_exception );
      else
      {
         send_okay_response = false;
         transaction_log_command( "" );
         response = string( c_response_error_prefix ) + c_unexpected_unknown_exception;
      }
   }

   if( !socket_handler.is_restoring( ) )
      handler.issue_command_response( response, !send_okay_response );

   if( clear_response )
      clear_key( response );
}

class socket_command_processor : public command_processor
{
   public:
   socket_command_processor( tcp_socket& socket, command_handler& handler )
    : command_processor( handler ),
    socket( socket ),
    handler( handler ),
    is_first_command( true )
   {
   }

   private:
   tcp_socket& socket;
   command_handler& handler;

   bool is_first_command;

   bool is_still_processing( ) { return is_captured_session( ) || socket.okay( ); }

   void get_cmd_and_args( string& cmd_and_args );

   void output_command_usage( const string& wildcard_match_expr ) const;
};

void socket_command_processor::get_cmd_and_args( string& cmd_and_args )
{
   if( is_first_command )
   {
      is_first_command = false;
      TRACE_LOG( TRACE_INITIAL | TRACE_SESSION, "started session (tid = " + to_string( current_thread_id( ) ) + ")" );
   }

   while( true )
   {
      progress* p_progress = 0;
      trace_progress progress( TRACE_VERBOSE | TRACE_SOCKETS );

      if( get_trace_flags( ) & ( TRACE_VERBOSE | TRACE_SOCKETS ) )
         p_progress = &progress;

      if( socket.read_line( cmd_and_args, c_request_timeout, 0, p_progress ) <= 0 )
      {
         if( !is_captured_session( )
          && ( is_condemned_session( ) || g_server_shutdown || !socket.had_timeout( ) ) )
         {
            // NOTE: If the session is not captured and it has either been condemned or
            // the server is shutting down, or its socket has died then force a "quit".
            cmd_and_args = c_cmd_ciyam_session_session_terminate;
            break;
         }

         // NOTE: If the socket is dead then the "read_line" will return instantly so for
         // a "captured" session manually perform a timeout sleep so CPU is not overused.
         if( is_captured_session( ) && !socket.had_timeout( ) )
         {
            msleep( c_request_timeout );
            continue;
         }

         if( !socket.had_timeout( ) )
            break;
      }
      else
         break;
   }

#ifdef DEBUG
   cerr << "cmd_and_args = '" << cmd_and_args << "'" << endl;
#endif
}

void socket_command_processor::output_command_usage( const string& wildcard_match_expr ) const
{
#ifdef DEBUG
   cerr << "<processing usage request>" << endl;
#endif

   socket.set_delay( );

   string cmds( "commands:" );
   if( !wildcard_match_expr.empty( ) )
      cmds += ' ' + wildcard_match_expr;

   socket.write_line( cmds, c_request_timeout );
   socket.write_line( "=========", c_request_timeout );

   socket.write_line( get_usage_for_commands( wildcard_match_expr ), c_request_timeout );

   socket.set_no_delay( );
   socket.write_line( c_response_okay, c_request_timeout );
}

}

#ifdef SSL_SUPPORT
ciyam_session::ciyam_session( ssl_socket* p_socket, const string& ip_addr )
#else
ciyam_session::ciyam_session( tcp_socket* p_socket, const string& ip_addr )
#endif
 :
 is_local( false ),
 using_tls( false ),
 pid_is_self( false ),
 needs_key_exchange( false ),
 ip_addr( ip_addr )
{
   ap_socket.reset( p_socket );

   if( !( *ap_socket ) )
      throw runtime_error( "unexpected invalid socket in ciyam_session::ciyam_session" );

   if( ip_addr == c_local_ip_addr || ip_addr == c_local_ip_addr_for_ipv6 )
      is_local = true;

#ifdef SSL_SUPPORT
   if( this->ap_socket->is_tls_handshake( ) )
   {
      this->ap_socket->ssl_accept( );

      using_tls = true;
   }
#endif

   string pid;
   ap_socket->read_line( pid, c_request_timeout );

   string::size_type pos = pid.find( c_key_exchange_suffix );

   if( pos != string::npos )
   {
      pid.erase( pos );
      needs_key_exchange = true;
   }

   if( is_local && pid == to_string( get_pid( ) ) )
      pid_is_self = true;

   increment_session_count( );

   // NOTE: Call "run_script" with "@none" to reload "manuscript.sio" if has changed.
   run_script( get_special_var_name( e_special_var_none ) );
}

#ifdef SSL_SUPPORT
ciyam_session::ciyam_session( auto_ptr< ssl_socket >& ap_socket, const string& ip_addr )
#else
ciyam_session::ciyam_session( auto_ptr< tcp_socket >& ap_socket, const string& ip_addr )
#endif
 :
 is_local( false ),
 using_tls( false ),
 pid_is_self( false ),
 needs_key_exchange( false ),
 ip_addr( ip_addr ),
 ap_socket( ap_socket )
{
   if( !( *this->ap_socket ) )
      throw runtime_error( "unexpected invalid socket in ciyam_session::ciyam_session" );

   if( ip_addr == c_local_ip_addr || ip_addr == c_local_ip_addr_for_ipv6 )
      is_local = true;

#ifdef SSL_SUPPORT
   if( this->ap_socket->is_tls_handshake( ) )
   {
      this->ap_socket->ssl_accept( );

      using_tls = true;
   }
#endif

   string pid;
   this->ap_socket->read_line( pid, c_request_timeout );

   string::size_type pos = pid.find( c_key_exchange_suffix );

   if( pos != string::npos )
   {
      pid.erase( pos );
      needs_key_exchange = true;
   }

   if( is_local && pid == to_string( get_pid( ) ) )
      pid_is_self = true;

   increment_session_count( );
}

ciyam_session::~ciyam_session( )
{
   decrement_session_count( );
}

void ciyam_session::on_start( )
{
#ifdef DEBUG
   cerr << "started session..." << endl;
#endif
   try
   {
      socket_command_handler cmd_handler( *ap_socket );

      cmd_handler.add_commands( 0,
       ciyam_session_command_functor_factory, ARRAY_PTR_AND_SIZE( ciyam_session_command_definitions ) );

      ap_socket->write_line( string( c_protocol_version )
       + ':' + to_string( get_files_area_item_max_size( ) )
       + '\n' + string( c_response_okay ), c_request_timeout );

      init_session( cmd_handler, false, &ip_addr );

      if( using_tls )
         session_is_using_tls( );

      if( needs_key_exchange )
      {
         string slot_and_pubkey( get_session_variable( get_special_var_name( e_special_var_slot ) ) );

         slot_and_pubkey += '-' + get_session_variable( get_special_var_name( e_special_var_pubkey ) );

         // NOTE: After handshake exchange public keys then commence application protocol.
         ap_socket->write_line( slot_and_pubkey );

         string slotx, pubkeyx, slotx_and_pubkeyx;
         ap_socket->read_line( slotx_and_pubkeyx, c_request_timeout );

         string::size_type pos = slotx_and_pubkeyx.find( '-' );

         if( pos != string::npos )
         {
            slotx = slotx_and_pubkeyx.substr( 0, pos );
            pubkeyx = slotx_and_pubkeyx.substr( pos + 1 );
         }

         if( slotx.empty( ) )
            slotx = string( c_none );

         if( pubkeyx.empty( ) )
            pubkeyx = string( c_none );

         set_session_variable( get_special_var_name( e_special_var_slotx ), slotx );
         set_session_variable( get_special_var_name( e_special_var_pubkeyx ), pubkeyx );
      }

      socket_command_processor processor( *ap_socket, cmd_handler );
      processor.process_commands( );

      ap_socket->close( );

      term_storage( cmd_handler );
      module_unload_all( cmd_handler );

      term_session( );
   }
   catch( exception& x )
   {
      issue_error( x.what( ) );

      ap_socket->write_line( string( c_response_error_prefix ) + x.what( ), c_request_timeout );
      ap_socket->close( );

      term_session( );
   }
   catch( ... )
   {
      issue_error( "unexpected unknown exception occurred" );

      ap_socket->write_line( string( c_response_error_prefix ) + "unexpected exception occurred", c_request_timeout );
      ap_socket->close( );

      term_session( );
   }

#ifdef DEBUG
   cerr << "finished session..." << endl;
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

#ifdef SSL_SUPPORT
void init_ciyam_session( ssl_socket* p_socket, const char* p_address )
#else
void init_ciyam_session( tcp_socket* p_socket, const char* p_address )
#endif
{
   ciyam_session* p_session = new ciyam_session( p_socket, p_address );

   // NOTE: Even if the server is being shut down will still start sessions
   // that were initiated by the server itself (so that operations that use
   // a separate session for completion are correctly performed). Therefore
   // non-essential scripts should not be executed by the server if already
   // shutting down.
   if( g_server_shutdown && !p_session->is_own_pid( ) )
      delete p_session;
   else
      p_session->start( );
}
