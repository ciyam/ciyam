// Copyright (c) 2006-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2019 CIYAM Developers
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
#include "base64.h"
#include "config.h"
#include "format.h"
#ifdef HPDF_SUPPORT
#  include "pdf_gen.h"
#endif
#include "threads.h"
#include "progress.h"
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
#include "command_parser.h"
#include "ciyam_packages.h"
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

const size_t c_request_timeout = 500;

const int c_pdf_default_limit = 5000;

const size_t c_max_key_append_chars = 7;

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
   TRACE_LOG( TRACE_ANYTHING, string( "session error: " ) + message );
#endif
}

inline void issue_warning( const string& message )
{
#ifdef DEBUG
   cout << "session warning: " << message << endl;
#else
   TRACE_LOG( TRACE_SESSIONS, string( "session warning: " ) + message );
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

void set_variable( size_t handle, const string& vname,
 const string& value, auto_ptr< temporary_session_variable >* p_ap_tmp_bh = 0 )
{
   if( !storage_blockchain( ).empty( )
    && vname != get_special_var_name( e_special_var_bh )
    && get_raw_session_variable( get_special_var_name( e_special_var_storage ) ) != "ciyam" )
      throw runtime_error( "invalid field '" + vname + "'" );

   // NOTE: The special variable used for block height is set as a session variable as it
   // needs to apply to any other records created or updated in the same operation scope.
   if( vname == get_special_var_name( e_special_var_bh ) )
   {
      if( p_ap_tmp_bh && !p_ap_tmp_bh->get( ) )
         p_ap_tmp_bh->reset( new temporary_session_variable( vname, value ) );
   }
   else
      instance_set_variable( handle, "", vname, value );
}

void set_script_error_if_applicable( const string& error_message )
{
   // NOTE: The "run_script" function contains a detailed note about this.
   string args_file( get_raw_session_variable(
    get_special_var_name( e_special_var_args_file ) ) );

   if( !args_file.empty( ) )
   {
      string args_file_value = get_raw_system_variable( args_file );

      if( args_file_value == string( "1" ) )
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

   if( !permission_info.empty( ) && ( permission_info != string( c_anyone ) || !perm.empty( ) ) )
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
      if( uid == c_admin && ( permission_info == c_admin_only || permission_info == c_admin_owner ) )
         okay = true;

      if( !okay && ( permission_info == c_owner_only || permission_info == c_admin_owner ) )
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

string get_shortened_field_id( const string& module, const string& mclass, const string& field_id )
{
   // KLUDGE: If the module starts with a number assume it is Meta (which doesn't support field shortening).
   if( !module.empty( ) && module[ 0 ] >= '0' && module[ 0 ] <= '9' )
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

string& remove_uid_extra_from_log_command( string& log_command, bool is_traditional_app )
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

   if( !is_traditional_app )
   {
      pos = log_command.find( '"' );

      // NOTE: Remove group and timezone name args for logging if is a blockchain app.
      string::size_type gpos = log_command.find( " -g=" );
      if( gpos != string::npos && ( pos == string::npos || gpos < pos ) )
      {
         string::size_type spos = log_command.find( ' ', gpos + 1 );
         if( spos == string::npos )
            throw runtime_error( "unexpected group info in: " + log_command );
         else
            log_command.erase( gpos, spos - gpos );
      }

      string::size_type tpos = log_command.find( " -tz=" );
      if( tpos != string::npos && ( pos == string::npos || tpos < pos ) )
      {
         string::size_type spos = log_command.find( ' ', tpos + 1 );
         if( spos == string::npos )
            throw runtime_error( "unexpected tz info in: " + log_command );
         else
            log_command.erase( tpos, spos - tpos );
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

struct blockchain_transaction_commit_helper : public transaction_commit_helper
{
   blockchain_transaction_commit_helper(
    const string& blockchain, const string& storage_name,
    const string& transaction_cmd, const vector< string >* p_file_info = 0 )
    :
    blockchain( blockchain ),
    storage_name( storage_name ),
    transaction_cmd( transaction_cmd ),
    p_file_info( p_file_info )
   {
      lock_blockchain_transaction( ap_guard );
   }

   void at_commit( )
   {
      tx_hash = get_raw_session_variable( get_special_var_name( e_special_var_transaction ) );

      if( tx_hash.empty( ) )
         tx_hash = create_blockchain_transaction( blockchain, storage_name, transaction_cmd, p_file_info );
   }

   void after_commit( )
   {
      append_transaction_for_blockchain_application( storage_name, tx_hash );
      ap_guard.reset( );
   }

   void after_rollback( )
   {
      if( p_file_info )
      {
         // NOTE: Remove any files that had been imported into the files area.
         for( size_t i = 0; i < p_file_info->size( ); i++ )
         {
            string next( ( *p_file_info )[ i ] );

            vector< string > parts;
            split( next, parts, ' ' );

            if( parts.size( ) > 1 )
            {
               if( has_file( parts[ 1 ] ) )
                  delete_file( parts[ 1 ] );
            }
         }
      }
   }

   auto_ptr< guard > ap_guard;

   string tx_hash;
   string blockchain;
   string storage_name;
   string transaction_cmd;

   const vector< string >* p_file_info;
};

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

         if( !value.empty( ) && !tz_name.empty( ) )
         {
            bool is_encrypted = false;
            string type_name = get_field_type_name( handle, context, field, &is_encrypted );

            if( is_encrypted
             && uid_matches_session_mint_account( )
             && !get_raw_session_variable( get_special_var_name( e_special_var_blockchain ) ).empty( ) )
               value = decrypt( value );

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
         if( !field_context.empty( ) && all_contexts[ field_context ] > 2 )
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

      TRACE_LOG( TRACE_FLD_VALS, "field name (start) '" + field_value_pairs[ i ].substr( 0, pos ) + "'" );
      TRACE_LOG( TRACE_FLD_VALS, "field name (trans) '" + field_id_or_name + "'" );

      field_id_or_name = get_field_id_for_id_or_name( module, class_id, field_id_or_name );

      TRACE_LOG( TRACE_FLD_VALS, "field id (checked) '" + field_id_or_name + "'" );
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
   progress* p_progress = 0;
   trace_progress progress( TRACE_SOCK_OPS );

   if( get_trace_flags( ) & TRACE_SOCK_OPS )
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
    restoring( false )
   {
   }

#ifdef SSL_SUPPORT
   ssl_socket& get_socket( ) { return socket; }
#else
   tcp_socket& get_socket( ) { return socket; }
#endif

   const string& get_next_command( ) { return next_command; }

   bool is_restoring( ) const { return restoring; }

   map< string, string >& get_transformations( ) { return transformations; }

   void output_progress( const string& message )
   {
      progress* p_progress = 0;
      trace_progress progress( TRACE_SOCK_OPS );

      if( get_trace_flags( ) & TRACE_SOCK_OPS )
         p_progress = &progress;

      socket.write_line( string( c_response_message_prefix ) + message, c_request_timeout, p_progress );
   }

   const string& get_restore_error( ) const { return restore_error; }
   void set_restore_error( const string& new_error ) { restore_error = new_error; }

   auto_ptr< restorable< bool > > set_restoring( ) { return auto_ptr< restorable< bool > >( new restorable< bool >( restoring, true ) ); }

   private:
   string preprocess_command_and_args( const string& cmd_and_args );

   void postprocess_command_and_args( const string& cmd_and_args );

   void handle_unknown_command( const string& command )
   {
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

      string::size_type pos = str.find( ' ' );

      if( str[ 0 ] == '?' || str.substr( 0, pos ) == "help" )
      {
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

   return str;
}

void socket_command_handler::postprocess_command_and_args( const string& cmd_and_args )
{
   if( has_finished( ) )
      TRACE_LOG( TRACE_SESSIONS, "finished session" );
}

void socket_command_handler::handle_command_response( const string& response, bool is_special )
{
   progress* p_progress = 0;
   trace_progress progress( TRACE_SOCK_OPS );

   if( get_trace_flags( ) & TRACE_SOCK_OPS )
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
   cout << "processing command: " << command << endl;
#endif
   string response;
   bool send_okay_response = true;
   bool possibly_expected_error = false;

#ifdef SSL_SUPPORT
   ssl_socket& socket( socket_handler.get_socket( ) );
#else
   tcp_socket& socket( socket_handler.get_socket( ) );
#endif

   if( command != c_cmd_ciyam_session_quit && !socket_handler.is_restoring( ) )
      socket.set_delay( );

   set_dtm( "" );
   set_grp( "" );
   set_uid( "" );
   set_tz_name( "" );
   set_tmp_directory( "" );

   clear_perms( );

   increment_session_commands_executed( );
   set_last_session_cmd_and_hash( command, socket_handler.get_next_command( ) );

   progress* p_progress = 0;
#ifdef HPDF_SUPPORT
   progress* p_pdf_progress = 0;
#endif
   trace_progress progress( TRACE_SOCK_OPS );

   if( get_trace_flags( ) & TRACE_SOCK_OPS )
      p_progress = &progress;

#ifdef HPDF_SUPPORT
   trace_progress pdf_progress( TRACE_PDF_VALS );

   if( get_trace_flags( ) & TRACE_PDF_VALS )
      p_pdf_progress = &pdf_progress;
#endif

   try
   {
      ostringstream osstr;

      if( command == c_cmd_ciyam_session_version )
         response = c_protocol_version;
      else if( command == c_cmd_ciyam_session_identity )
      {
         string info( get_parm_val( parameters, c_cmd_parm_ciyam_session_identity_info ) );

         if( !info.empty( ) )
            set_identity( info );

         response = get_identity( true, true );
      }
      else if( command == c_cmd_ciyam_session_file_chk )
      {
         string tag_or_hash( get_parm_val( parameters, c_cmd_parm_ciyam_session_file_chk_tag_or_hash ) );

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
         string tag_or_hash( get_parm_val( parameters, c_cmd_parm_ciyam_session_file_get_tag_or_hash ) );

         string hash( tag_or_hash );

         if( has_tag( tag_or_hash ) )
            hash = tag_file_hash( tag_or_hash );

         if( !has_file( hash, false ) )
         {
            possibly_expected_error = true;
            throw runtime_error( "file not found" );
         }

         fetch_file( hash, socket );
      }
      else if( command == c_cmd_ciyam_session_file_put )
      {
         string filename( get_parm_val( parameters, c_cmd_parm_ciyam_session_file_put_filename ) );
         string tag( get_parm_val( parameters, c_cmd_parm_ciyam_session_file_put_tag ) );

         // NOTE: Although "filename" is used to make the command usage easier to understand for
         // end users it is expected that the value provided will actually be the SHA256 hash of
         // the file content (which "ciyam_client" determines automatically).
         store_file( filename, socket, tag.empty( ) ? 0 : tag.c_str( ) );

         // NOTE: Although it seems a little odd to be checking this *after* the "store_file" it
         // otherwise would make a mess of the protocol (and the "store_file" just quietly fails
         // to store a blacklisted file).
         if( file_has_been_blacklisted( filename ) )
            throw runtime_error( "file '" + filename + "' has been blacklisted" );
      }
      else if( command == c_cmd_ciyam_session_file_raw )
      {
         bool is_core( has_parm_val( parameters, c_cmd_parm_ciyam_session_file_raw_core ) );
         bool is_mime( has_parm_val( parameters, c_cmd_parm_ciyam_session_file_raw_mime ) );
         bool is_text( has_parm_val( parameters, c_cmd_parm_ciyam_session_file_raw_text ) );
         bool is_blob( has_parm_val( parameters, c_cmd_parm_ciyam_session_file_raw_blob ) );
         bool is_list( has_parm_val( parameters, c_cmd_parm_ciyam_session_file_raw_list ) );
         string data( get_parm_val( parameters, c_cmd_parm_ciyam_session_file_raw_data ) );
         string tag( get_parm_val( parameters, c_cmd_parm_ciyam_session_file_raw_tag ) );

         vector< pair< string, string > > extras;

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
            if( !data.empty( ) && data[ 0 ] == '@' )
               data = buffer_file_lines( data.substr( 1 ) );

            if( is_blob )
               data = c_file_type_char_core_blob + data;
            else if( is_list )
               data = c_file_type_char_core_list + data;
            else
               throw runtime_error( "unexpected unknown core type" );

            verify_core_file( data, true, &extras );
         }
         else if( is_mime )
         {
            if( is_blob )
               data = c_file_type_char_mime_blob + data;
            else
               throw runtime_error( "only blob type is supported for MIME" );
         }

         response = create_raw_file_with_extras( is_core ? "" : data, extras, !is_text, tag.c_str( ) );
      }
      else if( command == c_cmd_ciyam_session_file_hash )
      {
         string pat_or_tag( get_parm_val( parameters, c_cmd_parm_ciyam_session_file_hash_pat_or_tag ) );

         vector< string > tags;

         if( pat_or_tag.size( ) == 1
          || pat_or_tag.find_first_of( "?*" ) == string::npos )
            tags.push_back( pat_or_tag );
         else
         {
            string all_tags( list_file_tags( pat_or_tag ) );
            split( all_tags, tags, '\n' );
         }

         for( size_t i = 0; i < tags.size( ); i++ )
         {
            if( i > 0 )
               response += '\n';
            response += tag_file_hash( tags[ i ] );
         }
      }
      else if( command == c_cmd_ciyam_session_file_tag )
      {
         bool is_remove( has_parm_val( parameters, c_cmd_parm_ciyam_session_file_tag_remove ) );
         bool is_unlink( has_parm_val( parameters, c_cmd_parm_ciyam_session_file_tag_unlink ) );
         string hash( get_parm_val( parameters, c_cmd_parm_ciyam_session_file_tag_hash ) );
         string name( get_parm_val( parameters, c_cmd_parm_ciyam_session_file_tag_name ) );

         if( is_remove || is_unlink )
            tag_del( name, is_unlink );
         else
            tag_file( name, hash );
      }
      else if( command == c_cmd_ciyam_session_file_info )
      {
         bool content( has_parm_val( parameters, c_cmd_parm_ciyam_session_file_info_content ) );
         bool recurse( has_parm_val( parameters, c_cmd_parm_ciyam_session_file_info_recurse ) );
         string depth( get_parm_val( parameters, c_cmd_parm_ciyam_session_file_info_depth ) );
         string pat_or_hash( get_parm_val( parameters, c_cmd_parm_ciyam_session_file_info_pat_or_hash ) );

         int depth_val = 0;
         if( !depth.empty( ) )
            depth_val = atoi( depth.c_str( ) );

         file_expansion expansion = e_file_expansion_none;

         if( content )
            expansion = e_file_expansion_content;
         else if( recurse )
         {
            if( depth_val >= 0 )
               expansion = e_file_expansion_recursive;
            else
            {
               depth_val = 0;
               expansion = e_file_expansion_recursive_hashes;
            }
         }

         deque< string > tags_or_hashes;

         if( pat_or_hash.find_first_of( "?*" ) == string::npos )
            tags_or_hashes.push_back( pat_or_hash );
         else
            list_file_tags( pat_or_hash, 0, 0, 0, &tags_or_hashes );

         for( size_t i = 0; i < tags_or_hashes.size( ); i++ )
         {
            if( i > 0 )
               response += '\n';
            response += file_type_info( tags_or_hashes[ i ], expansion, depth_val, 0, true );
         }
      }
      else if( command == c_cmd_ciyam_session_file_kill )
      {
         string pat( get_parm_val( parameters, c_cmd_parm_ciyam_session_file_kill_pat ) );
         string hash( get_parm_val( parameters, c_cmd_parm_ciyam_session_file_kill_hash ) );
         bool quiet( has_parm_val( parameters, c_cmd_parm_ciyam_session_file_kill_quiet ) );
         bool recurse( has_parm_val( parameters, c_cmd_parm_ciyam_session_file_kill_recurse ) );

         try
         {
            if( !hash.empty( ) )
            {
               if( !recurse )
                  delete_file( hash );
               else
                  delete_file_tree( hash );
            }
            else
               delete_files_for_tags( pat );
         }
         catch( exception& )
         {
            if( !quiet )
               throw;
         }
      }
      else if( command == c_cmd_ciyam_session_file_tags )
      {
         string pat_or_hash( get_parm_val( parameters, c_cmd_parm_ciyam_session_file_tags_pat_or_hash ) );

         if( !pat_or_hash.empty( ) && pat_or_hash.find_first_of( "?*" ) == string::npos )
            response = get_hash_tags( pat_or_hash );
         else
            response = list_file_tags( pat_or_hash );
      }
      else if( command == c_cmd_ciyam_session_file_stats )
         response = get_file_stats( );
      else if( command == c_cmd_ciyam_session_file_resync )
      {
         resync_files_area( );
         response = get_file_stats( );
      }
      else if( command == c_cmd_ciyam_session_file_archive )
      {
         bool add( has_parm_val( parameters, c_cmd_parm_ciyam_session_file_archive_add ) );
         bool remove( has_parm_val( parameters, c_cmd_parm_ciyam_session_file_archive_remove ) );
         bool repair( has_parm_val( parameters, c_cmd_parm_ciyam_session_file_archive_repair ) );
         bool destroy( has_parm_val( parameters, c_cmd_parm_ciyam_session_file_archive_destroy ) );
         string name( get_parm_val( parameters, c_cmd_parm_ciyam_session_file_archive_name ) );
         string path( get_parm_val( parameters, c_cmd_parm_ciyam_session_file_archive_path ) );
         string size_limit( get_parm_val( parameters, c_cmd_parm_ciyam_session_file_archive_size_limit ) );

         possibly_expected_error = true;

         if( add )
            add_file_archive( name, path, unformat_bytes( size_limit ) );
         else if( remove || destroy )
         {
            // NOTE: To make sure that the console client doesn't time out issue a progress message.
            if( remove )
               handler.output_progress( "(removing file archive)" );
            else
               handler.output_progress( "(destroying file archive)" );

            remove_file_archive( name, destroy );
         }
         else if( repair )
         {
            // NOTE: To make sure that the console client doesn't time out issue a progress message.
            handler.output_progress( "(repairing file archive)" );

            repair_file_archive( name );
         }
      }
      else if( command == c_cmd_ciyam_session_file_archives )
      {
         bool minimal( has_parm_val( parameters, c_cmd_parm_ciyam_session_file_archives_minimal ) );
         bool status_update( has_parm_val( parameters, c_cmd_parm_ciyam_session_file_archives_update_status ) );
         string name( get_parm_val( parameters, c_cmd_parm_ciyam_session_file_archives_name ) );

         if( status_update )
         {
            possibly_expected_error = true;
            archives_status_update( name );
         }

         response = list_file_archives( minimal );
      }
      else if( command == c_cmd_ciyam_session_file_relegate )
      {
         string num_files( get_parm_val( parameters, c_cmd_parm_ciyam_session_file_relegate_num_files ) );
         string size_limit( get_parm_val( parameters, c_cmd_parm_ciyam_session_file_relegate_size_limit ) );
         bool destroy( has_parm_val( parameters, c_cmd_parm_ciyam_session_file_relegate_destroy ) );
         bool blacklist( has_parm_val( parameters, c_cmd_parm_ciyam_session_file_relegate_blacklist ) );
         string hash( get_parm_val( parameters, c_cmd_parm_ciyam_session_file_relegate_hash ) );
         string archive( get_parm_val( parameters, c_cmd_parm_ciyam_session_file_relegate_archive ) );

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

            response = relegate_timestamped_files( hash, archive, num, size );
         }
      }
      else if( command == c_cmd_ciyam_session_file_retrieve )
      {
         string hash( get_parm_val( parameters, c_cmd_parm_ciyam_session_file_retrieve_hash ) );
         string tag( get_parm_val( parameters, c_cmd_parm_ciyam_session_file_retrieve_tag ) );
         string days_ahead( get_parm_val( parameters, c_cmd_parm_ciyam_session_file_retrieve_days_ahead ) );

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
      else if( command == c_cmd_ciyam_session_peer_listen )
      {
         string port( get_parm_val( parameters, c_cmd_parm_ciyam_session_peer_listen_port ) );
         string blockchain( get_parm_val( parameters, c_cmd_parm_ciyam_session_peer_listen_blockchain ) );

         create_peer_listener( atoi( port.c_str( ) ), blockchain );
      }
      else if( command == c_cmd_ciyam_session_peer_reject )
      {
         bool list( has_parm_val( parameters, c_cmd_parm_ciyam_session_peer_reject_list ) );
         bool remove( has_parm_val( parameters, c_cmd_parm_ciyam_session_peer_reject_remove ) );
         string ip_addr( get_parm_val( parameters, c_cmd_parm_ciyam_session_peer_reject_ip_addr ) );

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
         string host( get_parm_val( parameters, c_cmd_parm_ciyam_session_peer_connect_host ) );
         string port( get_parm_val( parameters, c_cmd_parm_ciyam_session_peer_connect_port ) );
         bool force( has_parm_val( parameters, c_cmd_parm_ciyam_session_peer_connect_force ) );
         string blockchain( get_parm_val( parameters, c_cmd_parm_ciyam_session_peer_connect_blockchain ) );

         create_peer_initiator( atoi( port.c_str( ) ), host, blockchain, force );
      }
      else if( command == c_cmd_ciyam_session_peer_block_info )
      {
         string blockchain( get_parm_val( parameters, c_cmd_parm_ciyam_session_peer_block_info_blockchain ) );
         string password( get_parm_val( parameters, c_cmd_parm_ciyam_session_peer_block_info_password ) );
         string account( get_parm_val( parameters, c_cmd_parm_ciyam_session_peer_block_info_account ) );

         // NOTE: To make sure the console client doesn't time out issue a progress message.
         handler.output_progress( "(creating a new block)" );

         response = construct_new_block( blockchain, password, account, false, 0, true );
      }
      else if( command == c_cmd_ciyam_session_peer_account_info )
      {
         string blockchain( get_parm_val( parameters, c_cmd_parm_ciyam_session_peer_account_info_blockchain ) );
         string password( get_parm_val( parameters, c_cmd_parm_ciyam_session_peer_account_info_password ) );
         unsigned int exponent( atoi( get_parm_val( parameters, c_cmd_parm_ciyam_session_peer_account_info_exponent ).c_str( ) ) );

         response = construct_account_info( blockchain, password, exponent );
      }
      else if( command == c_cmd_ciyam_session_peer_account_mint )
      {
         string blockchain( get_parm_val( parameters, c_cmd_parm_ciyam_session_peer_account_mint_blockchain ) );
         string pubkey( get_parm_val( parameters, c_cmd_parm_ciyam_session_peer_account_mint_pubkey ) );
         bool stop( has_parm_val( parameters, c_cmd_parm_ciyam_session_peer_account_mint_stop ) );
         string password( get_parm_val( parameters, c_cmd_parm_ciyam_session_peer_account_mint_password ) );

         if( !pubkey.empty( ) )
            password = session_shared_decrypt( pubkey, password );

         response = use_peer_account( blockchain, password, stop );

         if( !password.empty( ) )
            set_session_mint_account( stop ? string( ) : response );
      }
      else if( command == c_cmd_ciyam_session_peer_persist_file )
      {
         string dest_hash( get_parm_val( parameters, c_cmd_parm_ciyam_session_peer_persist_file_dest_hash ) );
         string pubkey( get_parm_val( parameters, c_cmd_parm_ciyam_session_peer_persist_file_pubkey ) );
         string tag_or_hash( get_parm_val( parameters, c_cmd_parm_ciyam_session_peer_persist_file_tag_or_hash ) );
         string password( get_parm_val( parameters, c_cmd_parm_ciyam_session_peer_persist_file_password ) );

         if( !pubkey.empty( ) )
            password = session_shared_decrypt( pubkey, password );

         string src_hash( tag_or_hash );

         if( has_tag( tag_or_hash ) )
            src_hash = tag_file_hash( tag_or_hash );

         if( !dest_hash.empty( ) )
            decrypt_pulled_peer_file( dest_hash, src_hash, password );
         else
            response = create_peer_repository_entry_push_info( src_hash, password );
      }
      else if( command == c_cmd_ciyam_session_peer_transactions )
      {
         string blockchain( get_parm_val( parameters, c_cmd_parm_ciyam_session_peer_transactions_blockchain ) );

         if( !exists_file( blockchain + ".txs" ) )
            throw runtime_error( "no unprocessed txs found for blockchain: " + blockchain );

         vector< string > applications;
         uint64_t block_height = construct_transaction_scripts_for_blockchain( blockchain, "", applications );

         set_session_variable( get_special_var_name( e_special_var_block_height ), to_string( block_height ) );

         for( size_t i = 0; i < applications.size( ); i++ )
         {
            string application( applications[ i ] );

            if( exists_file( application + ".log" ) )
            {
               set_session_variable( get_special_var_name( e_special_var_application ), application );
               run_script( "app_blk_txs", false );
            }

            file_remove( applications[ i ] + ".txs.cin" );
         }
      }
      else if( command == c_cmd_ciyam_session_peer_transaction_info )
      {
         string blockchain( get_parm_val( parameters, c_cmd_parm_ciyam_session_peer_transaction_info_blockchain ) );
         string password( get_parm_val( parameters, c_cmd_parm_ciyam_session_peer_transaction_info_password ) );
         string account( get_parm_val( parameters, c_cmd_parm_ciyam_session_peer_transaction_info_account ) );
         string application( get_parm_val( parameters, c_cmd_parm_ciyam_session_peer_transaction_info_application ) );
         string tx_data( get_parm_val( parameters, c_cmd_parm_ciyam_session_peer_transaction_info_tx_data ) );

         response = construct_new_transaction( blockchain, password, account, application, tx_data, false );
      }
      else if( command == c_cmd_ciyam_session_crypto_hash )
      {
         bool use_sha512( has_parm_val( parameters, c_cmd_parm_ciyam_session_crypto_hash_sha512 ) );
         bool hex_decode( has_parm_val( parameters, c_cmd_parm_ciyam_session_crypto_hash_hex_decode ) );
         string data( get_parm_val( parameters, c_cmd_parm_ciyam_session_crypto_hash_data ) );

         response = crypto_digest( data, use_sha512, hex_decode );
      }
      else if( command == c_cmd_ciyam_session_crypto_keys )
      {
         string extkey( get_parm_val( parameters, c_cmd_parm_ciyam_session_crypto_keys_extkey ) );
         bool use_base64( has_parm_val( parameters, c_cmd_parm_ciyam_session_crypto_keys_base64 ) );
         string seed( get_parm_val( parameters, c_cmd_parm_ciyam_session_crypto_keys_seed ) );

         string pub_key, priv_key;

         if( seed.empty( ) )
            response = create_address_key_pair( extkey, pub_key, priv_key, use_base64 );
         else
            response = create_address_key_pair( extkey, pub_key, priv_key, seed, true, use_base64 );

         response += '\n' + pub_key + '\n' + priv_key;
      }
      else if( command == c_cmd_ciyam_session_crypto_seed )
      {
         string mnenomics_or_hex_seed( get_parm_val( parameters, c_cmd_parm_ciyam_session_crypto_seed_mnemonics_or_hex_seed ) );

         response = get_mnemonics_or_hex_seed( mnenomics_or_hex_seed );
      }
      else if( command == c_cmd_ciyam_session_crypto_sign )
      {
         string privkey( get_parm_val( parameters, c_cmd_parm_ciyam_session_crypto_sign_privkey ) );
         bool hex_decode( has_parm_val( parameters, c_cmd_parm_ciyam_session_crypto_sign_hex_decode ) );
         string message( get_parm_val( parameters, c_cmd_parm_ciyam_session_crypto_sign_message ) );

         response = crypto_sign( privkey, message, hex_decode );
      }
      else if( command == c_cmd_ciyam_session_crypto_chain )
      {
         size_t length = from_string< size_t >( get_parm_val( parameters, c_cmd_parm_ciyam_session_crypto_chain_length ) );
         bool use_base64( has_parm_val( parameters, c_cmd_parm_ciyam_session_crypto_chain_base64 ) );
         string seed( get_parm_val( parameters, c_cmd_parm_ciyam_session_crypto_chain_seed ) );

         response = generate_hash_chain( length, use_base64, seed.empty( ) ? 0 : seed.c_str( ) );
      }
      else if( command == c_cmd_ciyam_session_crypto_verify )
      {
         string pubkey( get_parm_val( parameters, c_cmd_parm_ciyam_session_crypto_verify_pubkey ) );
         bool hex_decode( has_parm_val( parameters, c_cmd_parm_ciyam_session_crypto_verify_hex_decode ) );
         string message( get_parm_val( parameters, c_cmd_parm_ciyam_session_crypto_verify_message ) );
         string signature( get_parm_val( parameters, c_cmd_parm_ciyam_session_crypto_verify_signature ) );

         crypto_verify( pubkey, message, signature, hex_decode );
      }
      else if( command == c_cmd_ciyam_session_crypto_pub_key )
      {
         string privkey( get_parm_val( parameters, c_cmd_parm_ciyam_session_crypto_pub_key_privkey ) );

         response = crypto_public( privkey, false, false );
      }
      else if( command == c_cmd_ciyam_session_crypto_addr_hash )
      {
         string address( get_parm_val( parameters, c_cmd_parm_ciyam_session_crypto_addr_hash_address ) );

         response = crypto_address_hash( address );
      }
      else if( command == c_cmd_ciyam_session_crypto_p2sh_addr )
      {
         string extkey( get_parm_val( parameters, c_cmd_parm_ciyam_session_crypto_p2sh_addr_extkey ) );
         string script( get_parm_val( parameters, c_cmd_parm_ciyam_session_crypto_p2sh_addr_script ) );

         response = crypto_p2sh_address( extkey, script );
      }
      else if( command == c_cmd_ciyam_session_crypto_p2sh_redeem )
      {
         string txid( get_parm_val( parameters, c_cmd_parm_ciyam_session_crypto_p2sh_redeem_txid ) );
         string index( get_parm_val( parameters, c_cmd_parm_ciyam_session_crypto_p2sh_redeem_index ) );
         string script( get_parm_val( parameters, c_cmd_parm_ciyam_session_crypto_p2sh_redeem_script ) );
         string address( get_parm_val( parameters, c_cmd_parm_ciyam_session_crypto_p2sh_redeem_address ) );
         string amount( get_parm_val( parameters, c_cmd_parm_ciyam_session_crypto_p2sh_redeem_amount ) );
         string wif_key( get_parm_val( parameters, c_cmd_parm_ciyam_session_crypto_p2sh_redeem_wif_privkey ) );
         string extras( get_parm_val( parameters, c_cmd_parm_ciyam_session_crypto_p2sh_redeem_extras ) );
         string lock_time( get_parm_val( parameters, c_cmd_parm_ciyam_session_crypto_p2sh_redeem_lock_time ) );

         response = construct_p2sh_redeem_transaction(
          txid, from_string< unsigned int >( index ),
          script, extras, address, crypto_amount( amount ),
          wif_key, true, lock_time.empty( ) ? 0 : from_string< uint32_t >( lock_time ) );
      }
      else if( command == c_cmd_ciyam_session_crypto_nonce_search )
      {
         string data( get_parm_val( parameters, c_cmd_parm_ciyam_session_crypto_nonce_search_data ) );
         bool faster( has_parm_val( parameters, c_cmd_parm_ciyam_session_crypto_nonce_search_faster ) );
         string start( get_parm_val( parameters, c_cmd_parm_ciyam_session_crypto_nonce_search_start ) );
         string range( get_parm_val( parameters, c_cmd_parm_ciyam_session_crypto_nonce_search_range ) );
         string difficulty( get_parm_val( parameters, c_cmd_parm_ciyam_session_crypto_nonce_search_difficulty ) );

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
         string data( get_parm_val( parameters, c_cmd_parm_ciyam_session_crypto_nonce_verify_data ) );
         string nonce( get_parm_val( parameters, c_cmd_parm_ciyam_session_crypto_nonce_verify_nonce ) );
         string difficulty( get_parm_val( parameters, c_cmd_parm_ciyam_session_crypto_nonce_verify_difficulty ) );

         nonce_difficulty difficulty_val = e_nonce_difficulty_easy;

         if( !difficulty.empty( ) )
            difficulty_val = ( nonce_difficulty )from_string< int >( difficulty );

         response = check_for_proof_of_work( data, from_string< size_t >( nonce ), 1, difficulty_val );
      }
      else if( command == c_cmd_ciyam_session_module_list )
      {
         module_list( osstr );
         output_response_lines( socket, osstr.str( ) );
      }
      else if( command == c_cmd_ciyam_session_module_load )
      {
         string module( get_parm_val( parameters, c_cmd_parm_ciyam_session_module_load_module ) );

         module_load( module, handler, true );
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
      else if( command == c_cmd_ciyam_session_module_class_procedure_list )
      {
         string module( get_parm_val( parameters, c_cmd_parm_ciyam_session_module_class_procedure_list_module ) );
         string class_id_or_name( get_parm_val( parameters, c_cmd_parm_ciyam_session_module_class_procedure_list_class ) );

         module_class_procedures_list( module, class_id_or_name, osstr );
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
         string method( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_execute_method ) );
         string args( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_execute_args ) );

         string method_and_args( method );
         if( !args.empty( ) )
            method_and_args += ' ' + args;

         response = execute_object_command( atoi( handle.c_str( ) ), context, method_and_args );
      }
      else if( command == c_cmd_ciyam_session_object_validate )
      {
         string handle( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_validate_handle ) );
         string context( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_validate_context ) );

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
         string handle( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_variable_handle ) );
         string context( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_variable_context ) );
         string name_or_expr( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_variable_name_or_expr ) );
         bool has_new_val( has_parm_val( parameters, c_cmd_parm_ciyam_session_object_variable_new_value ) );
         string new_value( get_parm_val( parameters, c_cmd_parm_ciyam_session_object_variable_new_value ) );

         if( has_new_val )
            instance_set_variable( atoi( handle.c_str( ) ), context, name_or_expr, new_value );
         else
            response = instance_get_variable( atoi( handle.c_str( ) ), context, name_or_expr );
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
         string dtm( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_fetch_dtm ) );
         string grp( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_fetch_grp ) );
         string tz_name( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_fetch_tz_name ) );
         string tmp_dir( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_fetch_tmp_dir ) );
         string filters( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_fetch_filters ) );
         string perms( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_fetch_perms ) );
         string security_info( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_fetch_security_info ) );
         string search_text( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_fetch_search_text ) );
         string search_query( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_fetch_search_query ) );
         string extra_vars( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_fetch_extra_vars ) );
         string key_info( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_fetch_key_info ) );
         string limit( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_fetch_limit ) );
         string set_values( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_fetch_set_values ) );
         string fields( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_fetch_fields ) );
         bool minimal( has_parm_val( parameters, c_cmd_parm_ciyam_session_perform_fetch_minimal ) );
         bool no_default_values( has_parm_val( parameters, c_cmd_parm_ciyam_session_perform_fetch_no_default_values ) );
         string map_file( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_fetch_map_file ) );
         bool create_pdf( has_parm_val( parameters, c_cmd_parm_ciyam_session_perform_fetch_create_pdf ) );
         string format_file( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_fetch_format_file ) );
         string output_file( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_fetch_output_file ) );
         string title_name( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_fetch_title_name ) );

         string blockchain( get_raw_session_variable( get_special_var_name( e_special_var_blockchain ) ) );

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
            if( !field_list[ i ].empty( ) && field_list[ i ][ 0 ] == '@' )
            {
               field_inserts.insert( make_pair( non_inserts, field_list[ i ] ) );
               field_list.erase( field_list.begin( ) + i );
               --i;
            }
            else
            {
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
                && ( data.substr( 0, 4 ) == "opt_" || data.substr( 0, 4 ) == "loc_" ) )
               {
                  pos = data.find( "_class" );
                  if( pos == string::npos || ( pos + strlen( "_class" ) != data.length( ) ) )
                     continue;
               }

               search_replaces.insert( make_pair( key, data ) );
            }
         }

         // KLUDGE: Assume a dynamic instance is needed if a context has been supplied
         // as there is no simple way to otherwise determine this (maybe it's not even
         // worth worrying about trying to optimise this behaviour).
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
            if( !i->first.empty( ) && i->first[ 0 ] == '@' )
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

            auto_ptr< system_variable_lock > ap_blockchain_lock;

            if( !is_anon_uid( ) && !blockchain.empty( )
             && !storage_locked_for_admin( ) && uid_matches_session_mint_account( ) )
            {
               string account( get_raw_session_variable( get_special_var_name( e_special_var_uid ) ) );

               set_session_secret(
                get_account_msg_secret( blockchain, get_account_password( blockchain, account ), account ) );

               ap_blockchain_lock.reset( new system_variable_lock( blockchain ) );
            }

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
                  op_instance_create( handle, context, key_info, false );
                  prepare_object_instance( handle, context, true );
                  op_instance_cancel( handle, context );

                  // NOTE: Set this variable in order for a "new" record to be able to detect (after
                  // the cancel that is immediately prior to this) that it was cloned.
                  instance_set_variable( handle, "", get_special_var_name( e_special_var_cloned ), "1" );
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
               string field_output( get_field_values( handle, context, field_list, tz_name, true ) );

               if( !field_output.empty( ) )
                  output += " " + field_output;

               socket.write_line( output, c_request_timeout, p_progress );
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
               if( !set_value_items.empty( ) )
                  instance_set_variable( handle, context, get_special_var_name( e_special_var_skip_after_fetch ), "1" );

               if( instance_iterate( handle, context,
                key_info, normal_fields, search_text, search_query, security_info,
                is_reverse ? e_iter_direction_backwards : e_iter_direction_forwards,
                true, num_limit, e_sql_optimisation_none, !filter_set.empty( ) ? &filter_set : 0 ) )
               {
                  do
                  {
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

                     if( ( !filter_set.empty( ) || instance_has_transient_filter_fields( handle, context ) )
                      && instance_filtered( handle, context ) )
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

                        string field_output( get_field_values( handle, context, field_list, tz_name,
                         false, false, summaries.empty( ) ? 0 : &raw_values, &field_inserts, &search_replaces,
                         no_default_values ? &default_values : 0 ) );

                        if( minimal )
                           output = field_output;
                        else
                        {
                           output = key_output;
                           if( !field_output.empty( ) )
                              output += " " + field_output;
                        }

                        if( summaries.empty( ) )
                           socket.write_line( output, c_request_timeout, p_progress );
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
                     socket.write_line( i->second, c_request_timeout, p_progress );
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

         string uid( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_create_uid ) );
         string dtm( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_create_dtm ) );
         string module( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_create_module ) );
         string mclass( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_create_mclass ) );
         string grp( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_create_grp ) );
         string tz_name( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_create_tz_name ) );
         string key( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_create_key ) );
         string field_values( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_create_field_values ) );
         string method( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_create_method ) );

         string blockchain( get_raw_session_variable( get_special_var_name( e_special_var_blockchain ) ) );

         bool is_blockchain_app = get_session_is_using_blockchain( );

         string field_values_to_log;
         string module_and_class( module + ' ' + mclass );

         if( tz_name.empty( ) )
            tz_name = get_timezone( );

         set_dtm_if_now( dtm, next_command );

         // NOTE: If no key was provided then automatically generate a key.
         if( key.empty( ) || key[ 0 ] == ' ' )
         {
            string new_key( gen_key( 0, blockchain.empty( ) ) );

            if( !blockchain.empty( ) )
            {
               string append( uid.substr( 0, uid.find_first_of( ":!" ) ) );
               new_key += append.substr( 0, min( append.length( ), c_max_key_append_chars ) );
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

         set_last_session_cmd_and_hash( command, next_command );

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

            size_t handle = create_object_instance( module, mclass, 0, false );

            try
            {
               set_dtm( dtm );
               set_grp( grp );
               set_uid( uid );
               set_class( mclass );
               set_module( module );
               set_tz_name( tz_name );

               auto_ptr< temporary_session_variable > ap_tmp_bh;
               auto_ptr< system_variable_lock > ap_blockchain_lock;
               auto_ptr< blockchain_transaction_commit_helper > ap_commit_helper;

               bool has_transaction_hash =
                !get_raw_session_variable( get_special_var_name( e_special_var_transaction ) ).empty( );

               bool skip_blockchain_lock =
                !get_raw_session_variable( get_special_var_name( e_special_var_skip_blockchain_lock ) ).empty( );

               if( !blockchain.empty( ) && !storage_locked_for_admin( ) )
               {
                  if( !has_transaction_hash )
                  {
                     string account( get_raw_session_variable( get_special_var_name( e_special_var_uid ) ) );

                     set_session_secret( get_account_msg_secret(
                      blockchain, get_account_password( blockchain, account ), account ) );
                  }

                  if( !skip_blockchain_lock )
                     ap_blockchain_lock.reset( new system_variable_lock( blockchain ) );
               }

               map< string, string > field_scope_and_perm_info_by_id;
               map< string, string > field_scope_and_perm_info_by_name;
               get_all_field_scope_and_permission_info( handle, "", field_scope_and_perm_info_by_id );
               get_all_field_scope_and_permission_info( handle, "", field_scope_and_perm_info_by_name, true );

               for( map< string, string >::iterator i = field_value_items.begin( ), end = field_value_items.end( ); i != end; ++i )
               {
                  // NOTE: If a field to be set starts with @ then it is instead assumed to be a "variable".
                  if( !i->first.empty( ) && i->first[ 0 ] == '@' )
                  {
                     if( !blockchain.empty( ) )
                        set_variable( handle, i->first, i->second );
                     else
                        set_variable( handle, i->first, i->second, &ap_tmp_bh );
                  }
               }

               op_instance_create( handle, "", key, false );

               set< string > fields_set;
               for( map< string, string >::iterator i = field_value_items.begin( ), end = field_value_items.end( ); i != end; ++i )
               {
                  // NOTE: If a field to be set starts with @ then it is instead assumed to be a "variable".
                  if( !i->first.empty( ) && i->first[ 0 ] != '@' )
                  {
                     bool is_encrypted = false;
                     bool is_transient = false;
                     bool was_date_time = false;

                     string type_name( get_field_type_name( handle, "", i->first, &is_encrypted, &is_transient ) );

                     if( !i->second.empty( ) && !tz_name.empty( ) )
                     {
                        if( type_name == "date_time" || type_name == "tdatetime" )
                        {
                           was_date_time = true;
                           i->second = convert_local_to_utc( i->second, tz_name );
                        }
                     }

                     if( ( is_blockchain_app && !is_init_uid( ) && !storage_locked_for_admin( ) )
                      || ( !is_blockchain_app && !is_system_uid( ) && !storage_locked_for_admin( ) ) )
                     {
                        string scope_and_perm_info;

                        if( field_scope_and_perm_info_by_id.count( i->first ) )
                           scope_and_perm_info = field_scope_and_perm_info_by_id[ i->first ];
                        else
                           scope_and_perm_info = field_scope_and_perm_info_by_name[ i->first ];

                        check_instance_field_permission( module, handle, true, scope_and_perm_info );
                     }

                     string value, method_name_and_args;

                     if( !blockchain.empty( ) )
                     {
                        method_name_and_args = string( was_date_time ? "cmd " : "get " );
                        method_name_and_args += i->first;

                        if( was_date_time )
                           method_name_and_args += " raw";

                        value = execute_object_command( handle, "", method_name_and_args );

                        method_name_and_args = "set ";
                     }
                     else
                        method_name_and_args = string( "set " );

                     method_name_and_args += i->first + " ";
                     method_name_and_args += "\"" + escaped( i->second, "\"", c_nul ) + "\"";

                     execute_object_command( handle, "", method_name_and_args );

                     fields_set.insert( i->first );

                     if( is_transient && is_blockchain_app && blockchain.empty( ) )
                        throw runtime_error( "invalid attempt to set transient field: " + i->first );

                     if( !blockchain.empty( ) )
                     {
                        // NOTE: Field values that are the same as the default ones are omitted from the log
                        // as are values for all transient fields (unless used with initial data).
                        if( ( !is_transient || is_init_uid( ) ) && value != i->second )
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

               if( ( is_blockchain_app && !is_init_uid( ) && !storage_locked_for_admin( ) )
                || ( !is_blockchain_app && !is_system_uid( ) && !storage_locked_for_admin( ) ) )
                  check_instance_op_permission( module, handle, get_create_instance_info( handle, "" ) );

               remove_uid_extra_from_log_command( next_command, !is_blockchain_app );

               if( !blockchain.empty( ) )
               {
                  replace_field_values_to_log( next_command, field_values_to_log );
                  replace_module_and_class_to_log( next_command, module_and_class, module, mclass );

                  if( !storage_locked_for_admin( ) )
                     ap_commit_helper.reset(
                      new blockchain_transaction_commit_helper( blockchain, storage_name( ), next_command ) );
               }

               if( instance_persistence_type_is_sql( handle ) )
                  transaction_log_command( next_command, ap_commit_helper.get( ) );

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

         string uid( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_update_uid ) );
         string dtm( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_update_dtm ) );
         string module( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_update_module ) );
         string mclass( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_update_mclass ) );
         string grp( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_update_grp ) );
         string tz_name( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_update_tz_name ) );
         string key( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_update_key ) );
         string ver_info( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_update_ver_info ) );
         string field_values( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_update_field_values ) );
         string method( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_update_method ) );
         string check_values( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_update_check_values ) );

         string blockchain( get_raw_session_variable( get_special_var_name( e_special_var_blockchain ) ) );

         bool is_blockchain_app = get_session_is_using_blockchain( );

         string field_values_to_log;
         string module_and_class( module + ' ' + mclass );

         if( tz_name.empty( ) )
            tz_name = get_timezone( );

         if( !ver_info.empty( ) && is_blockchain_app )
            throw runtime_error( "version info is not permitted for blockchain applications" );

         // NOTE: Ignore version information during storage recovery.
         if( socket_handler.is_restoring( ) )
            ver_info.erase( );

         set_dtm_if_now( dtm, next_command );

         set_last_session_cmd_and_hash( command, next_command );

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

            try
            {
               set_dtm( dtm );
               set_grp( grp );
               set_uid( uid );
               set_class( mclass );
               set_module( module );
               set_tz_name( tz_name );

               vector< string > file_info;
               vector< string > file_names;
               vector< string > file_hashes;
               vector< string > files_to_remove;

               auto_ptr< temporary_session_variable > ap_tmp_bh;
               auto_ptr< system_variable_lock > ap_blockchain_lock;
               auto_ptr< blockchain_transaction_commit_helper > ap_commit_helper;

               bool has_transaction_hash =
                !get_raw_session_variable( get_special_var_name( e_special_var_transaction ) ).empty( );

               bool skip_blockchain_lock =
                !get_raw_session_variable( get_special_var_name( e_special_var_skip_blockchain_lock ) ).empty( );

               if( !blockchain.empty( ) && !storage_locked_for_admin( ) )
               {
                  if( !has_transaction_hash )
                  {
                     string account( get_raw_session_variable( get_special_var_name( e_special_var_uid ) ) );

                     set_session_secret( get_account_msg_secret(
                      blockchain, get_account_password( blockchain, account ), account ) );
                  }

                  if( !skip_blockchain_lock )
                     ap_blockchain_lock.reset( new system_variable_lock( blockchain ) );
               }

               map< string, string > field_scope_and_perm_info_by_id;
               map< string, string > field_scope_and_perm_info_by_name;
               get_all_field_scope_and_permission_info( handle, "", field_scope_and_perm_info_by_id );
               get_all_field_scope_and_permission_info( handle, "", field_scope_and_perm_info_by_name, true );

               string update_fields;
               for( map< string, string >::iterator i = field_value_items.begin( ), end = field_value_items.end( ); i != end; ++i )
               {
                  // NOTE: If a field to be set starts with @ then it is instead assumed to be a "variable".
                  if( !i->first.empty( ) && i->first[ 0 ] == '@' )
                  {
                     if( !blockchain.empty( ) )
                        set_variable( handle, i->first, i->second );
                     else
                        set_variable( handle, i->first, i->second, &ap_tmp_bh );
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

               if( ( is_blockchain_app && !is_init_uid( ) && !storage_locked_for_admin( ) )
                || ( !is_blockchain_app && !is_system_uid( ) && !storage_locked_for_admin( ) ) )
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
                  if( !i->first.empty( ) && i->first[ 0 ] != '@' )
                  {
                     bool is_encrypted = false;
                     bool is_transient = false;
                     bool is_file_field = false;
                     bool was_date_time = false;

                     string type_name( get_field_type_name(
                      handle, "", i->first, &is_encrypted, &is_transient, &is_file_field ) );

                     if( !i->second.empty( ) && !tz_name.empty( ) )
                     {
                        if( type_name == "date_time" || type_name == "tdatetime" )
                        {
                           was_date_time = true;
                           i->second = convert_local_to_utc( i->second, tz_name );
                        }
                     }

                     if( ( is_blockchain_app && !is_init_uid( ) && !storage_locked_for_admin( ) )
                      || ( !is_blockchain_app && !is_system_uid( ) && !storage_locked_for_admin( ) ) )
                     {
                        string scope_and_perm_info;

                        if( field_scope_and_perm_info_by_id.count( i->first ) )
                           scope_and_perm_info = field_scope_and_perm_info_by_id[ i->first ];
                        else
                           scope_and_perm_info = field_scope_and_perm_info_by_name[ i->first ];

                        check_instance_field_permission( module, handle, false, scope_and_perm_info );
                     }

                     string value, method_name_and_args;

                     if( is_blockchain_app )
                     {
                        method_name_and_args = string( was_date_time ? "cmd " : "get " );
                        method_name_and_args += i->first;

                        if( was_date_time )
                           method_name_and_args += " raw";

                        value = execute_object_command( handle, "", method_name_and_args );

                        method_name_and_args = "set ";
                     }
                     else
                        method_name_and_args = string( "set " );

                     if( blockchain.empty( ) || !is_encrypted
                      || value.empty( ) != i->second.empty( ) || decrypt( value ) != decrypt( i->second ) )
                     {
                        method_name_and_args += i->first + " ";
                        method_name_and_args += "\"" + escaped( i->second, "\"", c_nul ) + "\"";

                        execute_object_command( handle, "", method_name_and_args );
                     }

                     fields_set.insert( i->first );

                     if( is_transient && is_blockchain_app && blockchain.empty( ) )
                        throw runtime_error( "invalid attempt to set transient field: " + i->first );

                     if( is_blockchain_app && blockchain.empty( ) && !storage_locked_for_admin( ) )
                     {
                        if( is_file_field && value != i->second )
                        {
                           if( !i->second.empty( ) )
                           {
                              string tag( "c" + storage_blockchain( ) + ".f"
                               + storage_name( ) + "." + module + "." + mclass + "." + i->second );

                              if( has_tag( tag ) )
                              {
                                 file_names.push_back( i->second );
                                 file_hashes.push_back( tag_file_hash( tag ) );
                              }
                           }
                           else
                           {
                              string tag( "c" + storage_blockchain( ) + ".f"
                               + storage_name( ) + "." + module + "." + mclass + "." + value );

                              file_names.push_back( value );
                              file_hashes.push_back( "~" + tag_file_hash( tag ) );

                              if( has_tag( tag ) )
                                 tag_del( tag );

                              files_to_remove.push_back( get_attached_file_path( module, mclass, value ) );
                           }
                        }
                     }

                     if( !blockchain.empty( ) )
                     {
                        // NOTE: Field values that are the same as the record's current ones are omitted from
                        // the log as are values for all transient fields.
                        if( ( !is_transient || is_init_uid( ) )
                         && ( ( value.empty( ) != i->second.empty( ) )
                         || ( !is_encrypted && value != i->second )
                         || ( is_encrypted && decrypt( value ) != decrypt( i->second ) ) ) )
                        {
                           if( !field_values_to_log.empty( ) )
                              field_values_to_log += ",";

                           if( is_file_field && !i->second.empty( ) )
                           {
                              string tag( "c" + blockchain + ".f"
                               + storage_name( ) + "." + module + "." + mclass + "." + i->second );

                              string hash;
                              if( has_transaction_hash && has_tag( tag ) )
                                 hash = tag_file_hash( tag );
                              else if( !has_transaction_hash )
                              {
                                 string filepath( get_attached_file_path( module, mclass, i->second ) );

                                 string data( c_file_type_str_blob );
                                 data += buffer_file( filepath );

                                 hash = create_raw_file( data, true, tag.c_str( ) );
                              }

                              if( !hash.empty( ) )
                              {
                                 file_names.push_back( i->second );
                                 file_hashes.push_back( hash );

                                 file_info.push_back( "fe " + hash + " " + i->second );
                              }
                           }
                           else if( is_file_field )
                           {
                              string tag( "c" + blockchain + ".f"
                               + storage_name( ) + "." + module + "." + mclass + "." + value );

                              file_names.push_back( value );
                              file_hashes.push_back( "~" + tag_file_hash( tag ) );

                              if( has_tag( tag ) )
                                 tag_del( tag );

                              files_to_remove.push_back( get_attached_file_path( module, mclass, value ) );
                           }

                           string field_id( get_shortened_field_id( module, mclass, i->first ) );

                           field_values_to_log += field_id + '=';
                           field_values_to_log += escaped( escaped( i->second, "," ), ",\"", c_nul, "rn\r\n" );
                        }
                     }
                  }
               }

               remove_uid_extra_from_log_command( next_command, !is_blockchain_app );

               if( !file_names.empty( ) )
               {
                  string all_file_names;

                  for( size_t i = 0; i < file_names.size( ); i++ )
                  {
                     if( !all_file_names.empty( ) )
                        all_file_names += "\n";
                     all_file_names += file_names[ i ];
                  }

                  set_session_variable( get_special_var_name( e_special_var_file_names ), all_file_names );
               }

               if( !file_hashes.empty( ) )
               {
                  string all_file_hashes;

                  for( size_t i = 0; i < file_hashes.size( ); i++ )
                  {
                     if( !all_file_hashes.empty( ) )
                        all_file_hashes += "\n";
                     all_file_hashes += file_hashes[ i ];
                  }

                  set_session_variable( get_special_var_name( e_special_var_file_hashes ), all_file_hashes );
               }

               if( !blockchain.empty( ) )
               {
                  if( !field_values_to_log.empty( ) )
                  {
                     replace_field_values_to_log( next_command, field_values_to_log );
                     replace_module_and_class_to_log( next_command, module_and_class, module, mclass );

                     if( !storage_locked_for_admin( ) )
                        ap_commit_helper.reset(
                         new blockchain_transaction_commit_helper(
                         blockchain, storage_name( ), next_command, &file_info ) );

                     transaction_log_command( next_command, ap_commit_helper.get( ) );
                  }
               }
               else if( instance_persistence_type_is_sql( handle ) )
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

               for( size_t i = 0; i < files_to_remove.size( ); i++ )
                  file_remove( files_to_remove[ i ] );

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

         string uid( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_destroy_uid ) );
         string dtm( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_destroy_dtm ) );
         string module( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_destroy_module ) );
         string mclass( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_destroy_mclass ) );
         string grp( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_destroy_grp ) );
         string tz_name( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_destroy_tz_name ) );
         string set_values( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_destroy_set_values ) );
         bool progress( has_parm_val( parameters, c_cmd_parm_ciyam_session_perform_destroy_progress ) );
         bool quiet( has_parm_val( parameters, c_cmd_parm_ciyam_session_perform_destroy_quiet ) );
         string key( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_destroy_key ) );
         string ver_info( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_destroy_ver_info ) );

         auto_ptr< temporary_session_variable > ap_tmp_bh;

         string blockchain( get_raw_session_variable( get_special_var_name( e_special_var_blockchain ) ) );

         bool is_blockchain_app = get_session_is_using_blockchain( );

         string module_and_class( module + ' ' + mclass );

         if( tz_name.empty( ) )
            tz_name = get_timezone( );

         if( !ver_info.empty( ) && is_blockchain_app )
            throw runtime_error( "version info is not permitted for blockchain applications" );

         // NOTE: Ignore version information during storage recovery.
         if( socket_handler.is_restoring( ) )
            ver_info.erase( );

         set_dtm_if_now( dtm, next_command );

         set_last_session_cmd_and_hash( command, next_command );

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

               auto_ptr< system_variable_lock > ap_blockchain_lock;
               auto_ptr< blockchain_transaction_commit_helper > ap_commit_helper;

               bool has_transaction_hash =
                !get_raw_session_variable( get_special_var_name( e_special_var_transaction ) ).empty( );

               bool skip_blockchain_lock =
                !get_raw_session_variable( get_special_var_name( e_special_var_skip_blockchain_lock ) ).empty( );

               if( !blockchain.empty( ) && !storage_locked_for_admin( ) )
               {
                  if( !has_transaction_hash )
                  {
                     string account( get_raw_session_variable( get_special_var_name( e_special_var_uid ) ) );

                     set_session_secret( get_account_msg_secret(
                      blockchain, get_account_password( blockchain, account ), account ) );
                  }

                  if( !skip_blockchain_lock )
                     ap_blockchain_lock.reset( new system_variable_lock( blockchain ) );
               }

               if( progress )
                  instance_set_variable( handle, "", get_special_var_name( e_special_var_progress ), "1" );

               for( map< string, string >::iterator i = set_value_items.begin( ), end = set_value_items.end( ); i != end; ++i )
               {
                  // NOTE: If a field to be set starts with @ then it is instead assumed to be a "variable".
                  if( !i->first.empty( ) && i->first[ 0 ] == '@' )
                  {
                     if( !blockchain.empty( ) )
                        set_variable( handle, i->first, i->second );
                     else
                        set_variable( handle, i->first, i->second, &ap_tmp_bh );
                  }
               }

               op_destroy_rc rc;
               op_instance_destroy( handle, "", key, ver_info, false, &rc );

               if( ( is_blockchain_app && !is_init_uid( ) && !storage_locked_for_admin( ) )
                || ( !is_blockchain_app && !is_system_uid( ) && !storage_locked_for_admin( ) ) )
                  check_instance_op_permission( module, handle, get_destroy_instance_info( handle, "" ) );

               remove_uid_extra_from_log_command( next_command, !is_blockchain_app );

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
                     if( !i->first.empty( ) && i->first[ 0 ] != '@' )
                     {
                        string method_name_and_args( "set " );
                        method_name_and_args += i->first + " ";
                        method_name_and_args += "\"" + escaped( i->second, "\"", c_nul ) + "\"";

                        execute_object_command( handle, "", method_name_and_args );
                     }
                  }

                  if( blockchain.empty( ) )
                  {
                     if( instance_persistence_type_is_sql( handle ) )
                        transaction_log_command( next_command );
                  }
                  else
                  {
                     replace_module_and_class_to_log( next_command, module_and_class, module, mclass );

                     if( !storage_locked_for_admin( ) )
                        ap_commit_helper.reset(
                         new blockchain_transaction_commit_helper( blockchain, storage_name( ), next_command ) );

                     transaction_log_command( next_command, ap_commit_helper.get( ) );
                  }

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

         string uid( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_execute_uid ) );
         string dtm( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_execute_dtm ) );
         string module( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_execute_module ) );
         string mclass( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_execute_mclass ) );
         string grp( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_execute_grp ) );
         string tz_name( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_execute_tz_name ) );
         string set_values( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_execute_set_values ) );
         string keys( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_execute_keys ) );
         string vers( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_execute_vers ) );
         string method( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_execute_method ) );
         string args( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_execute_args ) );

         string field_values_to_log;
         string module_and_class( module + ' ' + mclass );

         if( tz_name.empty( ) )
            tz_name = get_timezone( );

         auto_ptr< temporary_session_variable > ap_tmp_bh;

         string blockchain( get_raw_session_variable( get_special_var_name( e_special_var_blockchain ) ) );

         bool is_blockchain_app = get_session_is_using_blockchain( );

         if( !vers.empty( ) && is_blockchain_app )
            throw runtime_error( "version info is not permitted for blockchain applications" );

         // NOTE: Ignore version information during storage recovery.
         if( socket_handler.is_restoring( ) )
            vers.erase( );

         set_dtm_if_now( dtm, next_command );

         bool log_as_update = false;
         bool log_transaction = true;
         bool skip_transaction = false;

         // NOTE: If a method id/name is prefixed by an underbar then this command is deemed to be
         // a "non-transactional" command and will not be logged (nor will a be transaction used).
         if( !method.empty( ) && method[ 0 ] == '_' )
         {
            log_transaction = false;
            skip_transaction = true;

            method.erase( 0, 1 );
         }
         else
         {
            // NOTE: If method id/name is prefixed by a minus sign then the command will still be
            // logged but a transaction will not be issued whilst handling the command here.
            if( !method.empty( ) && method[ 0 ] == '-' )
            {
               skip_transaction = true;
               method.erase( 0, 1 );
            }

            // NOTE: If method id/name is prefixed by a forward slash then the command will not be
            // logged as an execute but as though an update command had been issued instead. It is
            // not valid to attempt to use this with multiple key values.
            if( !method.empty( ) && method[ 0 ] == '/' )
            {
               if( skip_transaction )
                  throw runtime_error( "invalid attempt to use log_as_update with skip_transaction in perform_execute" );

               log_as_update = true;
               method.erase( 0, 1 );
            }

            set_last_session_cmd_and_hash( command, next_command );
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

            if( !instance_persistence_type_is_sql( handle ) )
               log_transaction = false;

            bool has_any_set_flds = false;
            for( map< string, string >::iterator i = set_value_items.begin( ), end = set_value_items.end( ); i != end; ++i )
            {
               // NOTE: If a field to be set starts with @ then it is instead assumed to be a "variable".
               if( !i->first.empty( ) && i->first[ 0 ] == '@' )
               {
                  if( !blockchain.empty( ) )
                     set_variable( handle, i->first, i->second );
                  else
                     set_variable( handle, i->first, i->second, &ap_tmp_bh );
               }
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

            string client_message = instance_get_variable( handle, "", get_special_var_name( e_special_var_message ) );

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

               auto_ptr< system_variable_lock > ap_blockchain_lock;
               auto_ptr< blockchain_transaction_commit_helper > ap_commit_helper;

               string blockchain( get_raw_session_variable( get_special_var_name( e_special_var_blockchain ) ) );

               bool has_transaction_hash =
                !get_raw_session_variable( get_special_var_name( e_special_var_transaction ) ).empty( );

               bool skip_blockchain_lock =
                !get_raw_session_variable( get_special_var_name( e_special_var_skip_blockchain_lock ) ).empty( );

               if( !blockchain.empty( ) && !storage_locked_for_admin( ) )
               {
                  if( !has_transaction_hash )
                  {
                     string account( get_raw_session_variable( get_special_var_name( e_special_var_uid ) ) );

                     set_session_secret( get_account_msg_secret(
                      blockchain, get_account_password( blockchain, account ), account ) );
                  }

                  if( !skip_blockchain_lock )
                     ap_blockchain_lock.reset( new system_variable_lock( blockchain ) );
               }

               if( log_as_update && all_keys.size( ) != 1 )
                  throw runtime_error( "perform_execute cannot log_as_update with multiple keys" );

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

                  instance_prepare_execute( handle, "", next_key, next_ver, has_any_set_flds );

                  if( ( is_blockchain_app && !is_init_uid( ) && !storage_locked_for_admin( ) )
                   || ( !is_blockchain_app && !is_system_uid( ) && !storage_locked_for_admin( ) ) )
                     check_instance_op_permission( module, handle, get_execute_procedure_info( handle, "", method_id ) );

                  for( map< string, string >::iterator j = set_value_items.begin( ), end = set_value_items.end( ); j != end; ++j )
                  {
                     // NOTE: If a field to be set starts with @ then it is instead assumed to be a "variable".
                     if( !j->first.empty( ) && j->first[ 0 ] != '@' )
                     {
                        bool is_encrypted = false;
                        bool is_transient = false;

                        string type_name( get_field_type_name( handle, "", j->first, &is_encrypted, &is_transient ) );

                        if( !j->second.empty( ) && !tz_name.empty( ) )
                        {
                           if( type_name == "date_time" || type_name == "tdatetime" )
                              j->second = convert_local_to_utc( j->second, tz_name );
                        }

                        if( ( is_blockchain_app && !is_init_uid( ) && !storage_locked_for_admin( ) )
                         || ( !is_blockchain_app && !is_system_uid( ) && !storage_locked_for_admin( ) ) )
                        {
                           string scope_and_perm_info;

                           if( field_scope_and_perm_info_by_id.count( j->first ) )
                              scope_and_perm_info = field_scope_and_perm_info_by_id[ j->first ];
                           else
                              scope_and_perm_info = field_scope_and_perm_info_by_name[ j->first ];

                           check_instance_field_permission( module, handle, true, scope_and_perm_info );
                        }

                        string method_name_and_args( "set " );
                        method_name_and_args += j->first + " ";
                        method_name_and_args += "\"" + escaped( j->second, "\"", c_nul ) + "\"";

                        execute_object_command( handle, "", method_name_and_args );

                        if( i == 0 && !blockchain.empty( ) && log_transaction )
                        {
                           if( !field_values_to_log.empty( ) )
                              field_values_to_log += ",";

                           string field_id( get_shortened_field_id( module, mclass, j->first ) );

                           field_values_to_log += field_id + '=';
                           field_values_to_log += escaped( escaped( j->second, "," ), ",\"", c_nul, "rn\r\n" );
                        }
                     }
                  }

                  if( i == 0 && log_transaction && !log_as_update )
                  {
                     remove_uid_extra_from_log_command( next_command, !is_blockchain_app );

                     if( !blockchain.empty( ) )
                     {
                        string::size_type rpos = 0;

                        if( !field_values_to_log.empty( ) )
                           replace_field_values_to_log( next_command, field_values_to_log, "-v=", &rpos );

                        replace_method_with_shortened_id( next_command, method, rpos, module, mclass, method_id );

                        replace_module_and_class_to_log( next_command, module_and_class, module, mclass );

                        if( !storage_locked_for_admin( ) )
                           ap_commit_helper.reset(
                            new blockchain_transaction_commit_helper( blockchain, storage_name( ), next_command ) );
                     }

                     transaction_log_command( next_command, ap_commit_helper.get( ) );
                  }

                  if( has_any_set_flds )
                     prepare_object_instance( handle, "", false, false );

                  string next_response( instance_execute( handle, "", next_key, method_and_args ) );

                  if( i == 0 && log_transaction && log_as_update )
                  {
                     string fields_and_values( instance_get_fields_and_values( handle, "", next_key ) );

                     if( !fields_and_values.empty( ) )
                     {
                        next_command = "pu " + uid + " " + dtm + " "
                         + module + " " + mclass + " " + next_key + " =" + next_ver + " \"" + fields_and_values + "\"";

                        remove_uid_extra_from_log_command( next_command, !is_blockchain_app );

                        if( !blockchain.empty( ) && !storage_locked_for_admin( ) )
                           ap_commit_helper.reset(
                            new blockchain_transaction_commit_helper( blockchain, storage_name( ), next_command ) );

                        transaction_log_command( next_command, ap_commit_helper.get( ) );
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
                     socket.write_line( return_response, c_request_timeout, p_progress );
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
               TRACE_LOG( ( is_auto_uid( ) ? TRACE_ANYTHING : TRACE_SESSIONS ),
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
                     socket.write_line( c_response_okay_more, c_request_timeout, p_progress );
               }

               if( !is_first && !skip_transaction )
                  transaction_rollback( );

               destroy_object_instance( handle );
            }
            catch( ... )
            {
               TRACE_LOG( TRACE_ANYTHING, "session error: "
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
                     socket.write_line( c_response_okay_more, c_request_timeout, p_progress );
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
         string tz_name( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_bulk_ops_tz_name ) );
         string filename( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_bulk_ops_filename ) );
         string export_fields( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_bulk_ops_export_fields ) );
         bool destroy_records( has_parm_val( parameters, c_cmd_parm_ciyam_session_perform_bulk_ops_destroy_records ) );
         string search_text( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_bulk_ops_search_text ) );
         string search_query( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_bulk_ops_search_query ) );
         string fixed_field_values( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_bulk_ops_fixed_field_values ) );

         if( dtm == c_dtm_now )
            dtm = date_time::standard( ).as_string( );

         response = exec_bulk_ops( module, uid, dtm, mclass, filename,
          export_fields, tz_name, destroy_records, search_text, search_query, fixed_field_values, handler );
      }
      else if( command == c_cmd_ciyam_session_perform_package_export )
      {
         string module( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_package_export_module ) );
         string mclass( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_package_export_mclass ) );
         string keys( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_package_export_keys ) );
         string filename( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_package_export_filename ) );
         string skip_field_info( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_package_export_skip_field_info ) );
         string test_info( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_package_export_test_info ) );
         string exclude_info( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_package_export_exclude_info ) );
         string include_info( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_package_export_include_info ) );

         export_package( module, mclass, keys, exclude_info, skip_field_info, test_info, include_info, filename );
      }
      else if( command == c_cmd_ciyam_session_perform_package_import )
      {
         string uid( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_package_import_uid ) );
         string dtm( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_package_import_dtm ) );
         string module( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_package_import_module ) );
         string filename( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_package_import_filename ) );
         bool new_only( has_parm_val( parameters, c_cmd_parm_ciyam_session_perform_package_import_new_only ) );
         bool for_remove( has_parm_val( parameters, c_cmd_parm_ciyam_session_perform_package_import_for_remove ) );
         string key_prefix( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_package_import_key_prefix ) );
         string replace_info( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_package_import_replace_info ) );
         string skip_field_info( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_package_import_skip_field_info ) );

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
         string name( get_parm_val( parameters, c_cmd_parm_ciyam_session_perform_package_update_name ) );

         update_package( name );
      }
      else if( command == c_cmd_ciyam_session_session_list )
      {
         bool minimal( has_parm_val( parameters, c_cmd_parm_ciyam_session_session_list_minimal ) );

         list_sessions( osstr, !minimal );
         output_response_lines( socket, osstr.str( ) );
      }
      else if( command == c_cmd_ciyam_session_session_kill )
      {
         bool force( has_parm_val( parameters, c_cmd_parm_ciyam_session_session_kill_force ) );
         bool at_term( has_parm_val( parameters, c_cmd_parm_ciyam_session_session_kill_at_term ) );
         bool kill_all( has_parm_val( parameters, c_cmd_parm_ciyam_session_session_kill_all ) );
         string sess_ids( get_parm_val( parameters, c_cmd_parm_ciyam_session_session_kill_sess_ids ) );
         string seconds( get_parm_val( parameters, c_cmd_parm_ciyam_session_session_kill_seconds ) );

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
      else if( command == c_cmd_ciyam_session_session_lock )
      {
         bool lock_capture( has_parm_val( parameters, c_cmd_parm_ciyam_session_session_lock_capture ) );
         bool lock_release( has_parm_val( parameters, c_cmd_parm_ciyam_session_session_lock_release ) );
         bool at_term( has_parm_val( parameters, c_cmd_parm_ciyam_session_session_lock_at_term ) );
         bool lock_all( has_parm_val( parameters, c_cmd_parm_ciyam_session_session_lock_all ) );
         string sess_ids( get_parm_val( parameters, c_cmd_parm_ciyam_session_session_lock_sess_ids ) );

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
      else if( command == c_cmd_ciyam_session_session_timeout )
      {
         string seconds( get_parm_val( parameters, c_cmd_parm_ciyam_session_session_timeout_seconds ) );

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
      else if( command == c_cmd_ciyam_session_session_variable )
      {
         string name_or_expr( get_parm_val( parameters, c_cmd_parm_ciyam_session_session_variable_name_or_expr ) );
         bool has_new_val( has_parm_val( parameters, c_cmd_parm_ciyam_session_session_variable_new_value ) );
         string new_value( get_parm_val( parameters, c_cmd_parm_ciyam_session_session_variable_new_value ) );

         bool needs_response = false;

         if( !has_new_val )
            needs_response = true;
         else
            set_session_variable( name_or_expr, new_value, &needs_response, &handler );

         if( needs_response )
            response = get_session_variable( name_or_expr );
      }
      else if( command == c_cmd_ciyam_session_storage_info )
      {
         response = "Name: " + storage_name( ) + '\n';
         response += "Identity: " + storage_identity( ) + '\n';
         response += "Directory: " + storage_module_directory( );
      }
      else if( command == c_cmd_ciyam_session_storage_init )
      {
         string name( get_parm_val( parameters, c_cmd_parm_ciyam_session_storage_init_name ) );
         string directory( get_parm_val( parameters, c_cmd_parm_ciyam_session_storage_init_directory ) );

         init_storage( name, directory, handler, has_parm_val( parameters, c_cmd_parm_ciyam_session_storage_init_admin ) );
      }
      else if( command == c_cmd_ciyam_session_storage_term )
         term_storage( handler );
      else if( command == c_cmd_ciyam_session_storage_create )
      {
         string name( get_parm_val( parameters, c_cmd_parm_ciyam_session_storage_create_name ) );
         string directory( get_parm_val( parameters, c_cmd_parm_ciyam_session_storage_create_directory ) );

         create_storage( name, directory, handler, has_parm_val( parameters, c_cmd_parm_ciyam_session_storage_create_admin ) );
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

         bool is_meta = ( name == "Meta" );

         int truncation_count = 0;
         string sav_db_file_names;

         init_storage( name, "", handler, true );
         backup_storage( handler, ( truncate_log ? &truncation_count : 0 ), &sav_db_file_names );
         term_storage( handler );

         bool has_ltf = false;
         bool has_txs_log = false;
         bool has_undo_sql = false;
         bool has_dead_keys = false;
         bool has_autoscript = false;
         bool has_manuscript = false;

         string log_name( name + ".log" );
         string sql_name( name + ".sql" );

         string backup_sql_name( name + ".backup.sql" );

         string sav_log_name( log_name + ".sav" );
         string sav_sql_name( sql_name + ".sav" );

         string ltf_name( name + ".ltf" );
         string txs_name( name + ".txs.log" );
         string undo_name( name + ".undo.sql" );
         string keys_name( name + ".dead_keys.lst" );

         string sav_ltf_name( ltf_name + ".sav" );
         string sav_txs_name( txs_name + ".sav" );
         string sav_undo_name( undo_name + ".sav" );
         string sav_keys_name( keys_name + ".sav" );

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

            if( is_meta )
            {
               ifstream siof( server_sio_name.c_str( ), ios::in | ios::binary );

               if( !siof )
                  throw runtime_error( "unable to open server backup input files for '" + name + "'" );

               ofstream sav_siof( sav_server_sio_name.c_str( ), ios::out | ios::binary );

               if( !sav_siof )
                  throw runtime_error( "unable to open server backup output files for '" + name + "'" );

               copy_stream( siof, sav_siof );

               sav_server_db_file_names = ciyam_ods_instance( ).backup_database( ".sav", ' ' );
            }

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

            if( exists_file( txs_name ) )
            {
               ifstream txsf( txs_name.c_str( ), ios::in | ios::binary );
               if( !txsf )
                  throw runtime_error( "unable to open '" + txs_name + "' for input" );

               ofstream sav_txsf( sav_txs_name.c_str( ), ios::out | ios::binary );
               if( !sav_txsf )
                  throw runtime_error( "unable to open '" + sav_txs_name + "' for output" );

               copy_stream( txsf, sav_txsf );
               has_txs_log = true;
            }

            if( exists_file( undo_name ) )
            {
               ifstream undof( undo_name.c_str( ), ios::in | ios::binary );
               if( !undof )
                  throw runtime_error( "unable to open '" + undo_name + "' for input" );

               ofstream sav_undof( sav_undo_name.c_str( ), ios::out | ios::binary );
               if( !sav_undof )
                  throw runtime_error( "unable to open '" + sav_undo_name + "' for output" );

               copy_stream( undof, sav_undof );
               has_undo_sql = true;
            }

            if( exists_file( keys_name ) )
            {
               ifstream keysf( keys_name.c_str( ), ios::in | ios::binary );
               if( !keysf )
                  throw runtime_error( "unable to open '" + keys_name + "' for input" );

               ofstream sav_keysf( sav_keys_name.c_str( ), ios::out | ios::binary );
               if( !sav_keysf )
                  throw runtime_error( "unable to open '" + sav_keys_name + "' for output" );

               copy_stream( keysf, sav_keysf );
               has_dead_keys = true;
            }

            if( is_meta && exists_file( autoscript_name ) )
            {
               ifstream ascf( autoscript_name.c_str( ), ios::in | ios::binary );
               if( !ascf )
                  throw runtime_error( "unable to open '" + autoscript_name + "' for input" );

               ofstream sav_ascf( sav_autoscript_name.c_str( ), ios::out | ios::binary );
               if( !sav_ascf )
                  throw runtime_error( "unable to open '" + sav_autoscript_name + "' for output" );

               copy_stream( ascf, sav_ascf );

               has_autoscript = true;
            }

            if( is_meta && exists_file( manuscript_name ) )
            {
               ifstream mscf( manuscript_name.c_str( ), ios::in | ios::binary );
               if( !mscf )
                  throw runtime_error( "unable to open '" + manuscript_name + "' for input" );

               ofstream sav_mscf( sav_manuscript_name.c_str( ), ios::out | ios::binary );
               if( !sav_mscf )
                  throw runtime_error( "unable to open '" + sav_manuscript_name + "' for output" );

               copy_stream( mscf, sav_mscf );

               has_manuscript = true;
            }
         }

         string file_names( sav_db_file_names );

         file_names += " " + sav_sql_name;
         file_names += " " + sav_log_name;
         file_names += " " + backup_sql_name;

         if( has_ltf )
            file_names += " " + sav_ltf_name;

         if( has_txs_log )
            file_names += " " + sav_txs_name;

         if( has_undo_sql )
            file_names += " " + sav_undo_name;

         if( has_dead_keys )
            file_names += " " + sav_keys_name;

         string module_list( name + ".modules.lst" );
         file_names += " " + module_list;

         // NOTE: Although the initial data lists and .csv files are backed up they
         // can only be restored manually (because if these files were missing then
         // the application should be regenerated to recreate them).
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
            file_names += " " + sav_server_sio_name;
            file_names += " " + sav_server_db_file_names;
         }

         if( has_autoscript )
            file_names += " " + sav_autoscript_name;

         if( has_manuscript )
            file_names += " " + sav_manuscript_name;

         ostringstream osstr;
         osstr << setw( 3 ) << setfill( '0' ) << truncation_count;

         if( exists_file( name + ".backup.bun.gz" ) )
            remove_file( name + ".backup.bun.gz" );

#ifdef _WIN32
         string bundle( "bundle" );
#else
         string bundle( "./bundle" );
#endif
         exec_system( bundle + " -q " + name + ".backup.bun.gz " + file_names );

         if( truncate_log )
            exec_system( bundle + " -q " + name + "." + osstr.str( ) + ".bun.gz "
             + name + ".log." + osstr.str( ) + " " + name + ".tlg." + osstr.str( ) );

         remove_files( sav_db_file_names, ' ' );

         remove_file( sav_sql_name );
         remove_file( sav_log_name );

         remove_file( backup_sql_name );

         if( is_meta )
         {
            remove_file( sav_server_sio_name );
            remove_files( sav_server_db_file_names, ' ' );
         }

         if( has_ltf )
            remove_file( sav_ltf_name );

         if( has_txs_log )
            remove_file( sav_txs_name );

         if( has_undo_sql )
            remove_file( sav_undo_name );

         if( has_dead_keys )
            remove_file( sav_keys_name );

         if( has_autoscript )
            remove_file( sav_autoscript_name );

         if( has_manuscript )
            remove_file( sav_manuscript_name );

         if( truncate_log )
         {
            remove_file( name + ".log." + osstr.str( ) );
            remove_file( name + ".tlg." + osstr.str( ) );
         }
      }
      else if( command == c_cmd_ciyam_session_storage_rewind )
      {
         uint64_t block_height( from_string< uint64_t >(
          get_parm_val( parameters, c_cmd_parm_ciyam_session_storage_rewind_block_height ) ) );

         string blockchain( storage_identity( ) );
         string::size_type pos = blockchain.find( ':' );
         if( pos == string::npos )
            throw runtime_error( "current storage does not have a blockchain" );

         blockchain.erase( 0, pos + 1 );

         perform_storage_rewind( blockchain, block_height );
      }
      else if( command == c_cmd_ciyam_session_storage_comment )
      {
         string text( get_parm_val( parameters, c_cmd_parm_ciyam_session_storage_comment_text ) );

         storage_comment( text );
      }
      else if( command == c_cmd_ciyam_session_storage_restore )
      {
         string name( get_parm_val( parameters, c_cmd_parm_ciyam_session_storage_restore_name ) );
         string directory( get_parm_val( parameters, c_cmd_parm_ciyam_session_storage_restore_directory ) );
         string trace_info( get_parm_val( parameters, c_cmd_parm_ciyam_session_storage_restore_trace_info ) );
         int stop_at_tx( atoi( get_parm_val( parameters, c_cmd_parm_ciyam_session_storage_restore_stop_at_tx ).c_str( ) ) );
         bool rebuild( has_parm_val( parameters, c_cmd_parm_ciyam_session_storage_restore_rebuild ) );
         bool partial( has_parm_val( parameters, c_cmd_parm_ciyam_session_storage_restore_partial ) );
         bool quicker( has_parm_val( parameters, c_cmd_parm_ciyam_session_storage_restore_quicker ) );

         string db_file_names( ods_file_names( name, ' ' ) );

         string sql_name( name + ".sql" );
         string log_name( name + ".log" );
         string ltf_name( name + ".ltf" );
         string txs_name( name + ".txs.log" );
         string undo_name( name + ".undo.sql" );
         string keys_name( name + ".dead_keys.lst" );

         string sav_db_file_names( ods_backup_file_names( name, ".sav", ' ' ) );

         string sav_sql_name( sql_name + ".sav" );
         string sav_log_name( log_name + ".sav" );
         string sav_ltf_name( ltf_name + ".sav" );
         string sav_txs_name( txs_name + ".sav" );
         string sav_undo_name( undo_name + ".sav" );
         string sav_keys_name( keys_name + ".sav" );

         string backup_sql_name( name + ".backup.sql" );

         bool is_blockchain_app = false;
         auto_ptr< temporary_session_variable > ap_blockchain;

         vector< string > module_list;
         string module_list_name( name + ".modules.lst" );

         if( rebuild )
         {
            if( !exists_file( module_list_name ) )
               throw runtime_error( "need '" + module_list_name + "' to perform a rebuild" );

            buffer_file_lines( module_list_name, module_list );

            if( !module_list.empty( ) && !module_list[ 0 ].empty( )
             && module_list[ 0 ][ 0 ] >= '0' && module_list[ 0 ][ 0 ] <= '9' )
            {
               is_blockchain_app = true;

               ap_blockchain.reset( new temporary_session_variable(
                get_special_var_name( e_special_var_blockchain ), module_list[ 0 ] ) );

               module_list.erase( module_list.begin( ) );
            }
         }

         if( !rebuild && !partial && !exists_file( name + ".backup.bun.gz" ) )
            throw runtime_error( "need '" + name + ".backup.bun.gz' to preform a restore" );

         if( !rebuild && !partial )
         {
            string file_names( sav_db_file_names );

            file_names += " " + sav_sql_name;

            if( !exists_file( log_name ) )
               file_names += " " + sav_log_name;

            if( !exists_file( ltf_name ) )
               file_names += " " + sav_ltf_name;

            if( !exists_file( txs_name ) )
               file_names += " " + sav_txs_name;

            if( !exists_file( undo_name ) )
               file_names += " " + sav_undo_name;

            if( !exists_file( keys_name ) )
               file_names += " " + sav_keys_name;

            file_names += " " + backup_sql_name;
            file_names += " " + module_list_name;

#ifdef _WIN32
            string unbundle( "unbundle" );
#else
            string unbundle( "./unbundle" );
#endif

            exec_system( unbundle + " -o -q " + name + ".backup.bun.gz " + file_names );

            buffer_file_lines( module_list_name, module_list );

            file_names.erase( );
            for( size_t i = 0; i < module_list.size( ); i++ )
               file_names += " " + module_list[ i ] + ".init.lst";

            if( !file_names.empty( ) )
               exec_system( unbundle + " -o -q " + name + ".backup.bun.gz " + file_names );

            file_names.erase( );
            for( size_t i = 0; i < module_list.size( ); i++ )
            {
               string module_init_list( module_list[ i ] + ".init.lst" );
               if( exists_file( module_init_list ) )
               {
                  file_names += " " + module_init_list;

                  vector< string > init_classes;
                  buffer_file_lines( module_init_list, init_classes );

                  for( size_t j = 0; j < init_classes.size( ); j++ )
                     file_names += " " + module_list[ i ] + "_" + init_classes[ j ] + ".csv";
               }
            }

            if( !file_names.empty( ) )
               exec_system( unbundle + " -o -q " + name + ".backup.bun.gz" + file_names );
         }

         if( !rebuild && !partial && ( !exists_files( sav_db_file_names, ' ' )
          || !exists_file( sav_sql_name ) || !exists_file( backup_sql_name ) ) )
            throw runtime_error( "incomplete or missing file set for backup restore" );

         storage_admin_name_lock( name );

         if( rebuild )
         {
            remove_files( db_file_names, ' ' );

            remove_file( sql_name );

            if( is_blockchain_app )
               remove_file( undo_name );
         }
         else if( !partial )
         {
            copy_files( sav_db_file_names, db_file_names, ' ' );

            copy_file( sav_sql_name, sql_name );

            if( !exists_file( log_name ) && exists_file( sav_log_name ) )
               copy_file( sav_log_name, log_name );

            if( !exists_file( ltf_name ) && exists_file( sav_ltf_name ) )
               copy_file( sav_ltf_name, ltf_name );

            if( !exists_file( txs_name ) && exists_file( sav_txs_name ) )
               copy_file( sav_txs_name, txs_name );

            if( !exists_file( undo_name ) && exists_file( sav_undo_name ) )
               copy_file( sav_undo_name, undo_name );

            if( !exists_file( keys_name ) && exists_file( sav_keys_name ) )
               copy_file( sav_keys_name, keys_name );
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
            ap_blockchain.reset( );

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

            set_session_variable( get_special_var_name( e_special_var_restore ), "1" );

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

               if( !in_trans && line >= tline && tran_id >= c_tx_id_standard )
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
               else if( tran_id >= c_tx_id_initial && is_new && is_partial )
                  throw runtime_error( "cannot restore from empty DB with a partial transaction log" );

               if( tran_id > c_tx_id_module && is_new && !performed_init )
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

                  if( is_blockchain_app )
                     storage_comment( "block 0" );

                  session_skip_fk_fetches( old_skip_fetches );
               }

               // NOTE: Any operation whose transaction id is less than standard is skipped during a restore.
               if( !tran_info.empty( ) && tran_id >= c_tx_id_standard )
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
            if( next_transaction_id( ) < c_tx_id_standard - 1 )
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

               if( exists_file( sav_txs_name ) )
                  remove_file( sav_txs_name );

               if( exists_file( sav_undo_name ) )
                  remove_file( sav_undo_name );

               if( exists_file( sav_keys_name ) )
                  remove_file( sav_keys_name );

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
      else if( command == c_cmd_ciyam_session_storage_dump_cache )
      {
         dump_storage_cache( osstr );
         output_response_lines( socket, osstr.str( ) );
      }
      else if( command == c_cmd_ciyam_session_storage_dump_locks )
      {
         dump_storage_locks( osstr );
         output_response_lines( socket, osstr.str( ) );
      }
      else if( command == c_cmd_ciyam_session_storage_file_export )
      {
         string hash( get_parm_val( parameters, c_cmd_parm_ciyam_session_storage_file_export_hash ) );
         string module( get_parm_val( parameters, c_cmd_parm_ciyam_session_storage_file_export_module ) );
         string mclass( get_parm_val( parameters, c_cmd_parm_ciyam_session_storage_file_export_mclass ) );
         string filename( get_parm_val( parameters, c_cmd_parm_ciyam_session_storage_file_export_filename ) );

         module = resolve_module_id( module );
         mclass = resolve_class_id( module, mclass );

         string filepath( get_attached_file_path( module, mclass, filename ) );

         create_directories_for_file_name( filepath );

         extract_file( hash, filepath );
      }
      else if( command == c_cmd_ciyam_session_storage_file_import )
      {
         string module( get_parm_val( parameters, c_cmd_parm_ciyam_session_storage_file_import_module ) );
         string mclass( get_parm_val( parameters, c_cmd_parm_ciyam_session_storage_file_import_mclass ) );
         string filename( get_parm_val( parameters, c_cmd_parm_ciyam_session_storage_file_import_filename ) );
         string tag( get_parm_val( parameters, c_cmd_parm_ciyam_session_storage_file_import_tag ) );

         module = resolve_module_id( module );
         mclass = resolve_class_id( module, mclass );

         string filepath( get_attached_file_path( module, mclass, filename ) );

         string data( c_file_type_str_blob );
         data += buffer_file( filepath );

         response = create_raw_file( data, true, tag.empty( ) ? 0 : tag.c_str( ) );
      }
      else if( command == c_cmd_ciyam_session_storage_cache_clear )
         storage_cache_clear( );
      else if( command == c_cmd_ciyam_session_storage_cache_limit )
      {
         string new_limit( get_parm_val( parameters, c_cmd_parm_ciyam_session_storage_cache_limit_new_limit ) );

         size_t cache_limit;
         if( new_limit.empty( ) )
            cache_limit = storage_cache_limit( );
         else
            cache_limit = storage_cache_limit( from_string< size_t >( new_limit ) );

         response = to_string( cache_limit );
      }
      else if( command == c_cmd_ciyam_session_storage_trans_start )
         transaction_start( );
      else if( command == c_cmd_ciyam_session_storage_trans_commit )
         transaction_commit( );
      else if( command == c_cmd_ciyam_session_storage_trans_rollback )
         transaction_rollback( );
      else if( command == c_cmd_ciyam_session_system_mutexes )
      {
         ostringstream osstr;

         list_mutex_lock_ids_for_ciyam_base( osstr );
         list_mutex_lock_ids_for_ciyam_files( osstr );
         list_mutex_lock_ids_for_peer_session( osstr );

         osstr << "ciyam_session::g_mutex = " << g_mutex.get_lock_id( ) << '\n';

         list_mutex_lock_ids_for_ciyam_variables( osstr );
         list_mutex_lock_ids_for_ciyam_core_files( osstr );

         response = osstr.str( );
      }
      else if( command == c_cmd_ciyam_session_system_variable )
      {
         string name_or_expr( get_parm_val( parameters, c_cmd_parm_ciyam_session_system_variable_name_or_expr ) );
         bool has_new_val( has_parm_val( parameters, c_cmd_parm_ciyam_session_system_variable_new_value ) );
         string new_value( get_parm_val( parameters, c_cmd_parm_ciyam_session_system_variable_new_value ) );

         if( has_new_val )
            set_system_variable( name_or_expr, new_value );
         else
            response = get_system_variable( name_or_expr );
      }
      else if( command == c_cmd_ciyam_session_system_listeners )
      {
         ostringstream osstr;

         list_listeners( osstr );

         response = osstr.str( );
      }
      else if( command == c_cmd_ciyam_session_trace )
      {
         string flags( get_parm_val( parameters, c_cmd_parm_ciyam_session_trace_flags ) );

         if( !flags.empty( ) )
         {
            if( !( get_trace_flags( ) & TRACE_COMMANDS ) )
               log_trace_message( TRACE_COMMANDS, "trace " + flags );

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
      else if( command == c_cmd_ciyam_session_decode )
      {
         string data( get_parm_val( parameters, c_cmd_parm_ciyam_session_decode_data ) );

         response = hex_encode( base64::decode( data ) );
      }
      else if( command == c_cmd_ciyam_session_encode )
      {
         string data( get_parm_val( parameters, c_cmd_parm_ciyam_session_encode_data ) );

         response = base64::encode( hex_decode( data ) );
      }
      else if( command == c_cmd_ciyam_session_encrypt )
      {
         bool no_ssl( has_parm_val( parameters, c_cmd_parm_ciyam_session_encrypt_no_ssl ) );
         bool no_salt( has_parm_val( parameters, c_cmd_parm_ciyam_session_encrypt_no_salt ) );
         string password( get_parm_val( parameters, c_cmd_parm_ciyam_session_encrypt_password ) );

         response = encrypt_data( password, no_ssl, no_salt );
      }
      else if( command == c_cmd_ciyam_session_password )
      {
         string name( get_parm_val( parameters, c_cmd_parm_ciyam_session_password_name ) );

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
      else if( command == c_cmd_ciyam_session_passtotp )
      {
         string secret( get_parm_val( parameters, c_cmd_parm_ciyam_session_passtotp_secret ) );

         response = get_totp( secret );
      }
      else if( command == c_cmd_ciyam_session_sendmail )
      {
         string to( get_parm_val( parameters, c_cmd_parm_ciyam_session_sendmail_to ) );
         string subject( get_parm_val( parameters, c_cmd_parm_ciyam_session_sendmail_subject ) );
         string message( get_parm_val( parameters, c_cmd_parm_ciyam_session_sendmail_message ) );
         string tz_name( get_parm_val( parameters, c_cmd_parm_ciyam_session_sendmail_tz_name ) );
         string file_names( get_parm_val( parameters, c_cmd_parm_ciyam_session_sendmail_file_names ) );
         string html_source( get_parm_val( parameters, c_cmd_parm_ciyam_session_sendmail_html_source ) );
         string image_names( get_parm_val( parameters, c_cmd_parm_ciyam_session_sendmail_image_names ) );
         string image_prefix( get_parm_val( parameters, c_cmd_parm_ciyam_session_sendmail_image_prefix ) );

         if( tz_name.empty( ) )
            tz_name = get_timezone( );

         vector< string > all_file_names;
         if( !file_names.empty( ) )
            split( file_names, all_file_names );

         vector< string > all_image_names;
         if( !image_names.empty( ) )
            split( image_names, all_image_names );

         if( !message.empty( ) && message[ 0 ] == '@' )
         {
            if( exists_file( message.substr( 1 ) ) )
               message = buffer_file( message.substr( 1 ) );
         }

         send_email_message( to, subject, message,
          html_source, 0, &all_file_names, &tz_name, &all_image_names, &image_prefix );
      }
      else if( command == c_cmd_ciyam_session_schedule )
      {
         output_schedule( osstr );
         output_response_lines( socket, osstr.str( ) );
      }
      else if( command == c_cmd_ciyam_session_smtpinfo )
         response = get_smtp_username( ) + "@" + get_smtp_suffix( );
      else if( command == c_cmd_ciyam_session_starttls )
      {
#ifdef SSL_SUPPORT
         if( socket.is_secure( ) )
            throw runtime_error( "TLS is already active" );

         if( !get_using_ssl( ) )
            throw runtime_error( "SSL has not been initialised" );

         socket.ssl_accept( );
#else
         throw runtime_error( "SSL support not available" );
#endif
      }
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
      else if( command == c_cmd_ciyam_session_externals )
         response = list_externals( );
      else if( command == c_cmd_ciyam_session_runscript )
      {
         string script_name( get_parm_val( parameters, c_cmd_parm_ciyam_session_runscript_script_name ) );

         if( script_name.find_first_of( "?*" ) == string::npos )
            run_script( script_name );
         else
         {
            ostringstream osstr;

            list_scripts( script_name, osstr );
            response = osstr.str( );
         }
      }
      else if( command == c_cmd_ciyam_session_timezones )
      {
         string own_tz( get_timezone( ) );

         response = list_timezones( );

         if( response.find( own_tz + ' ' ) != string::npos )
            replace( response, own_tz + ' ', "*" + own_tz + ' ' );
      }
      else if( command == c_cmd_ciyam_session_utc_now )
         response = date_time::standard( ).as_string( true, false );
      else if( command == c_cmd_ciyam_session_utc_offset )
      {
         string tz_name( get_parm_val( parameters, c_cmd_parm_ciyam_session_utc_offset_tz_name ) );
         string local_time( get_parm_val( parameters, c_cmd_parm_ciyam_session_utc_offset_local_time ) );

         float offset;
         get_tz_info( date_time( local_time ), tz_name, offset );

         response = tz_name + " " + to_string( offset );
      }
      else if( command == c_cmd_ciyam_session_utc_to_local )
      {
         string tz_name( get_parm_val( parameters, c_cmd_parm_ciyam_session_utc_to_local_tz_name ) );
         string utc_time( get_parm_val( parameters, c_cmd_parm_ciyam_session_utc_to_local_utc_time ) );

         date_time local( utc_to_local( date_time( utc_time ), tz_name ) );

         response = local.as_string( e_time_format_hhmm, true ) + " " + tz_name;
      }
      else if( command == c_cmd_ciyam_session_utc_from_local )
      {
         string tz_name( get_parm_val( parameters, c_cmd_parm_ciyam_session_utc_from_local_tz_name ) );
         string local_time( get_parm_val( parameters, c_cmd_parm_ciyam_session_utc_from_local_local_time ) );

         date_time utc( local_to_utc( date_time( local_time ), tz_name ) );

         response = utc.as_string( e_time_format_hhmm, true );
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
         string arg_1( get_parm_val( parameters, c_cmd_parm_ciyam_session_test_arg_1 ) );
         string arg_2( get_parm_val( parameters, c_cmd_parm_ciyam_session_test_arg_2 ) );
         string arg_3( get_parm_val( parameters, c_cmd_parm_ciyam_session_test_arg_3 ) );

         // NOTE: Dummy command for testing or prototyping.
         response = "arg_1 = " + arg_1 + "\narg_2 = " + arg_2 + "\narg_3 = " + arg_3;
      }
      else if( command == c_cmd_ciyam_session_quit )
      {
         if( !is_captured_session( ) )
            handler.set_finished( );
         else if( !is_condemned_session( ) )
            condemn_this_session( );

         return;
      }
   }
   catch( exception& x )
   {
      TRACE_LOG( ( possibly_expected_error
       ? TRACE_COMMANDS | TRACE_SESSIONS : TRACE_ANYTHING ), string( "session error: " ) + x.what( ) );

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
      TRACE_LOG( TRACE_ANYTHING, "session error: " + string( c_unexpected_unknown_exception ) );

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
      handler.issue_command_reponse( response, !send_okay_response );
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

   string get_cmd_and_args( );

   void output_command_usage( const string& wildcard_match_expr ) const;
};

string socket_command_processor::get_cmd_and_args( )
{
   string request;

   if( is_first_command )
   {
      is_first_command = false;
      TRACE_LOG( TRACE_SESSIONS, "started session (tid = " + to_string( current_thread_id( ) ) + ")" );
   }

   while( true )
   {
      progress* p_progress = 0;
      trace_progress progress( TRACE_SOCK_OPS );

      if( get_trace_flags( ) & TRACE_SOCK_OPS )
         p_progress = &progress;

      if( socket.read_line( request, c_request_timeout, 0, p_progress ) <= 0 )
      {
         if( !is_captured_session( )
          && ( is_condemned_session( ) || g_server_shutdown || !socket.had_timeout( ) ) )
         {
            // NOTE: If the session is not captured and it has either been condemned or
            // the server is shutting down, or its socket has died then force a "quit".
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

         if( !socket.had_timeout( ) )
            break;
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
 pid_is_self( false )
{
   ap_socket.reset( p_socket );

   if( !( *ap_socket ) )
      throw runtime_error( "unexpected invalid socket in ciyam_session::ciyam_session" );

   if( ip_addr == c_local_ip_addr || ip_addr == c_local_ip_addr_for_ipv6 )
      is_local = true;

   string pid;
   ap_socket->read_line( pid, c_request_timeout );

   if( is_local && pid == to_string( get_pid( ) ) )
      pid_is_self = true;

   increment_session_count( );
}

#ifdef SSL_SUPPORT
ciyam_session::ciyam_session( auto_ptr< ssl_socket >& ap_socket, const string& ip_addr )
#else
ciyam_session::ciyam_session( auto_ptr< tcp_socket >& ap_socket, const string& ip_addr )
#endif
 :
 is_local( false ),
 pid_is_self( false ),
 ap_socket( ap_socket )
{
   if( !( *this->ap_socket ) )
      throw runtime_error( "unexpected invalid socket in ciyam_session::ciyam_session" );

   if( ip_addr == c_local_ip_addr || ip_addr == c_local_ip_addr_for_ipv6 )
      is_local = true;

   string pid;
   this->ap_socket->read_line( pid, c_request_timeout );

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
   cout << "started session..." << endl;
#endif
   try
   {
      socket_command_handler cmd_handler( *ap_socket );

      cmd_handler.add_commands( 0,
       ciyam_session_command_functor_factory, ARRAY_PTR_AND_SIZE( ciyam_session_command_definitions ) );

      ap_socket->write_line( string( c_protocol_version ) + '\n' + string( c_response_okay ), c_request_timeout );

      init_session( cmd_handler );

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
