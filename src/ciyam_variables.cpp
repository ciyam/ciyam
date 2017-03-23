// Copyright (c) 2017 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <iostream>
#endif

#define CIYAM_BASE_IMPL

#include "ciyam_variables.h"

#include "ods.h"
#include "threads.h"
#include "date_time.h"
#include "utilities.h"
#include "ciyam_base.h"
#include "class_base.h"
#include "ods_file_system.h"

using namespace std;

namespace
{

#include "ciyam_constants.h"

const int c_max_lock_attempts = 20;
const int c_lock_attempt_sleep_time = 200;

const char c_persist_variable_prefix = '/';
const char c_restore_variable_prefix = '~';

const char* const c_special_variable_bh = "@bh";
const char* const c_special_variable_id = "@id";
const char* const c_special_variable_os = "@os";
const char* const c_special_variable_dtm = "@dtm";
const char* const c_special_variable_grp = "@grp";
const char* const c_special_variable_key = "@key";
const char* const c_special_variable_sec = "@sec";
const char* const c_special_variable_uid = "@uid";
const char* const c_special_variable_arg1 = "@arg1";
const char* const c_special_variable_arg2 = "@arg2";
const char* const c_special_variable_val1 = "@val1";
const char* const c_special_variable_val2 = "@val2";
const char* const c_special_variable_file = "@file";
const char* const c_special_variable_loop = "@loop";
const char* const c_special_variable_name = "@name";
const char* const c_special_variable_none = "@none";
const char* const c_special_variable_peer = "@peer";
const char* const c_special_variable_uuid = "@uuid";
const char* const c_special_variable_async = "@async";
const char* const c_special_variable_class = "@class";
const char* const c_special_variable_embed = "@embed";
const char* const c_special_variable_print = "@print";
const char* const c_special_variable_quiet = "@quiet";
const char* const c_special_variable_title = "@title";
const char* const c_special_variable_cloned = "@cloned";
const char* const c_special_variable_images = "@images";
const char* const c_special_variable_module = "@module";
const char* const c_special_variable_pubkey = "@pubkey";
const char* const c_special_variable_return = "@return";
const char* const c_special_variable_script = "@script";
const char* const c_special_variable_do_exec = "@do_exec";
const char* const c_special_variable_is_last = "@is_last";
const char* const c_special_variable_message = "@message";
const char* const c_special_variable_package = "@package";
const char* const c_special_variable_restore = "@restore";
const char* const c_special_variable_slowest = "@slowest";
const char* const c_special_variable_storage = "@storage";
const char* const c_special_variable_tz_name = "@tz_name";
const char* const c_special_variable_trigger = "@trigger";
const char* const c_special_variable_cmd_hash = "@cmd_hash";
const char* const c_special_variable_key_info = "@key_info";
const char* const c_special_variable_executed = "@executed";
const char* const c_special_variable_identity = "@identity";
const char* const c_special_variable_progress = "@progress";
const char* const c_special_variable_args_file = "@args_file";
const char* const c_special_variable_crypt_key = "@crypt_key";
const char* const c_special_variable_decrement = "@decrement";
const char* const c_special_variable_file_hash = "@file_hash";
const char* const c_special_variable_file_name = "@file_name";
const char* const c_special_variable_image_dir = "@image_dir";
const char* const c_special_variable_increment = "@increment";
const char* const c_special_variable_val_error = "@val_error";
const char* const c_special_variable_blockchain = "@blockchain";
const char* const c_special_variable_extra_info = "@extra_info";
const char* const c_special_variable_permission = "@permission";
const char* const c_special_variable_allow_async = "@allow_async";
const char* const c_special_variable_application = "@application";
const char* const c_special_variable_errors_only = "@errors_only";
const char* const c_special_variable_init_log_id = "@init_log_id";
const char* const c_special_variable_output_file = "@output_file";
const char* const c_special_variable_path_prefix = "@path_prefix";
const char* const c_special_variable_permissions = "@permissions";
const char* const c_special_variable_skip_update = "@skip_update";
const char* const c_special_variable_state_names = "@state_names";
const char* const c_special_variable_transaction = "@transaction";
const char* const c_special_variable_block_height = "@block_height";
const char* const c_special_variable_rewind_height = "@rewind_height";
const char* const c_special_variable_update_fields = "@update_fields";
const char* const c_special_variable_peer_initiator = "@peer_initiator";
const char* const c_special_variable_peer_responder = "@peer_responder";
const char* const c_special_variable_sys_var_prefix = "@sys_var_prefix";
const char* const c_special_variable_unix_timestamp = "@unix_timestamp";
const char* const c_special_variable_dummy_timestamp = "@dummy_timestamp";
const char* const c_special_variable_check_if_changed = "@check_if_changed";
const char* const c_special_variable_skip_after_fetch = "@skip_after_fetch";
const char* const c_special_variable_fields_and_values = "@fields_and_values";
const char* const c_special_variable_package_type_path = "@package_type_path";
const char* const c_special_variable_attached_file_path = "@attached_file_path";
const char* const c_special_variable_check_script_error = "@check_script_error";
const char* const c_special_variable_blockchain_head_hash = "@blockchain_head_hash";
const char* const c_special_variable_blockchain_info_hash = "@blockchain_info_hash";
const char* const c_special_variable_locally_minted_block = "@locally_minted_block";
const char* const c_special_variable_secondary_validation = "@secondary_validation";
const char* const c_special_variable_skip_blockchain_lock = "@skip_blockchain_lock";
const char* const c_special_variable_peer_is_synchronising = "@peer_is_synchronising";
const char* const c_special_variable_total_child_field_in_parent = "@total_child_field_in_parent";

mutex g_mutex;

map< string, string > g_variables;

}

string get_special_var_name( special_var var )
{
   string s;

   switch( var )
   {
      case e_special_var_bh:
      s = string( c_special_variable_bh );
      break;

      case e_special_var_id:
      s = string( c_special_variable_id );
      break;

      case  e_special_var_os:
      s = string( c_special_variable_os );
      break;

      case e_special_var_dtm:
      s = string( c_special_variable_dtm );
      break;

      case e_special_var_grp:
      s = string( c_special_variable_grp );
      break;

      case e_special_var_key:
      s = string( c_special_variable_key );
      break;

      case e_special_var_sec:
      s = string( c_special_variable_sec );
      break;

      case e_special_var_uid:
      s = string( c_special_variable_uid );
      break;

      case e_special_var_arg1:
      s = string( c_special_variable_arg1 );
      break;

      case e_special_var_arg2:
      s = string( c_special_variable_arg2 );
      break;

      case e_special_var_val1:
      s = string( c_special_variable_val1 );
      break;

      case e_special_var_val2:
      s = string( c_special_variable_val2 );
      break;

      case e_special_var_file:
      s = string( c_special_variable_file );
      break;

      case e_special_var_loop:
      s = string( c_special_variable_loop );
      break;

      case e_special_var_name:
      s = string( c_special_variable_name );
      break;

      case e_special_var_none:
      s = string( c_special_variable_none );
      break;

      case e_special_var_peer:
      s = string( c_special_variable_peer );
      break;

      case e_special_var_uuid:
      s = string( c_special_variable_uuid );
      break;

      case e_special_var_async:
      s = string( c_special_variable_async );
      break;

      case e_special_var_class:
      s = string( c_special_variable_class );
      break;

      case e_special_var_embed:
      s = string( c_special_variable_embed );
      break;

      case e_special_var_print:
      s = string( c_special_variable_print );
      break;

      case e_special_var_quiet:
      s = string( c_special_variable_quiet );
      break;

      case e_special_var_title:
      s = string( c_special_variable_title );
      break;

      case e_special_var_cloned:
      s = string( c_special_variable_cloned );
      break;

      case e_special_var_images:
      s = string( c_special_variable_images );
      break;

      case e_special_var_module:
      s = string( c_special_variable_module );
      break;

      case e_special_var_pubkey:
      s = string( c_special_variable_pubkey );
      break;

      case e_special_var_return:
      s = string( c_special_variable_return );
      break;

      case e_special_var_script:
      s = string( c_special_variable_script );
      break;

      case e_special_var_do_exec:
      s = string( c_special_variable_do_exec );
      break;

      case e_special_var_is_last:
      s = string( c_special_variable_is_last );
      break;

      case e_special_var_message:
      s = string( c_special_variable_message );
      break;

      case e_special_var_package:
      s = string( c_special_variable_package );
      break;

      case e_special_var_restore:
      s = string( c_special_variable_restore );
      break;

      case e_special_var_slowest:
      s = string( c_special_variable_slowest );
      break;

      case e_special_var_storage:
      s = string( c_special_variable_storage );
      break;

      case e_special_var_tz_name:
      s = string( c_special_variable_tz_name );
      break;

      case e_special_var_trigger:
      s = string( c_special_variable_trigger );
      break;

      case e_special_var_cmd_hash:
      s = string( c_special_variable_cmd_hash );
      break;

      case e_special_var_key_info:
      s = string( c_special_variable_key_info );
      break;

      case e_special_var_executed:
      s = string( c_special_variable_executed );
      break;

      case e_special_var_identity:
      s = string( c_special_variable_identity );
      break;

      case e_special_var_progress:
      s = string( c_special_variable_progress );
      break;

      case e_special_var_args_file:
      s = string( c_special_variable_args_file );
      break;

      case e_special_var_crypt_key:
      s = string( c_special_variable_crypt_key );
      break;

      case e_special_var_decrement:
      s = string( c_special_variable_decrement );
      break;

      case e_special_var_file_hash:
      s = string( c_special_variable_file_hash );
      break;

      case e_special_var_file_name:
      s = string( c_special_variable_file_name );
      break;

      case e_special_var_image_dir:
      s = string( c_special_variable_image_dir );
      break;

      case e_special_var_increment:
      s = string( c_special_variable_increment );
      break;

      case e_special_var_val_error:
      s = string( c_special_variable_val_error );
      break;

      case e_special_var_blockchain:
      s = string( c_special_variable_blockchain );
      break;

      case e_special_var_extra_info:
      s = string( c_special_variable_extra_info );
      break;

      case e_special_var_permission:
      s = string( c_special_variable_permission );
      break;

      case e_special_var_allow_async:
      s = string( c_special_variable_allow_async );
      break;

      case e_special_var_application:
      s = string( c_special_variable_application );
      break;

      case e_special_var_errors_only:
      s = string( c_special_variable_errors_only );
      break;

      case e_special_var_init_log_id:
      s = string( c_special_variable_init_log_id );
      break;

      case e_special_var_output_file:
      s = string( c_special_variable_output_file );
      break;

      case e_special_var_path_prefix:
      s = string( c_special_variable_path_prefix );
      break;

      case e_special_var_permissions:
      s = string( c_special_variable_permissions );
      break;

      case e_special_var_skip_update:
      s = string( c_special_variable_skip_update );
      break;

      case e_special_var_state_names:
      s = string( c_special_variable_state_names );
      break;

      case e_special_var_transaction:
      s = string( c_special_variable_transaction );
      break;

      case e_special_var_block_height:
      s = string( c_special_variable_block_height );
      break;

      case e_special_var_rewind_height:
      s = string( c_special_variable_rewind_height );
      break;

      case e_special_var_update_fields:
      s = string( c_special_variable_update_fields );
      break;

      case e_special_var_peer_initiator:
      s = string( c_special_variable_peer_initiator );
      break;

      case e_special_var_peer_responder:
      s = string( c_special_variable_peer_responder );
      break;

      case e_special_var_unix_timestamp:
      s = string( c_special_variable_unix_timestamp );
      break;

      case e_special_var_dummy_timestamp:
      s = string( c_special_variable_dummy_timestamp );
      break;

      case e_special_var_check_if_changed:
      s = string( c_special_variable_check_if_changed );
      break;

      case e_special_var_skip_after_fetch:
      s = string( c_special_variable_skip_after_fetch );
      break;

      case e_special_var_fields_and_values:
      s = string( c_special_variable_fields_and_values );
      break;

      case e_special_var_package_type_path:
      s = string( c_special_variable_package_type_path );
      break;

      case e_special_var_attached_file_path:
      s = string( c_special_variable_attached_file_path );
      break;

      case e_special_var_check_script_error:
      s = string( c_special_variable_check_script_error );
      break;

      case e_special_var_blockchain_head_hash:
      s = string( c_special_variable_blockchain_head_hash );
      break;

      case e_special_var_blockchain_info_hash:
      s = string( c_special_variable_blockchain_info_hash );
      break;

      case e_special_var_locally_minted_block:
      s = string( c_special_variable_locally_minted_block );
      break;

      case e_special_var_secondary_validation:
      s = string( c_special_variable_secondary_validation );
      break;

      case e_special_var_skip_blockchain_lock:
      s = string( c_special_variable_skip_blockchain_lock );
      break;

      case e_special_var_peer_is_synchronising:
      s = string( c_special_variable_peer_is_synchronising );
      break;

      case e_special_var_total_child_field_in_parent:
      s = string( c_special_variable_total_child_field_in_parent );
      break;

      default:
      throw runtime_error( "unexpected special var value #" + to_string( var ) );
   }

   return s;
}

system_variable_lock::system_variable_lock( const string& name )
 :
 name( name )
{
   bool acquired = false;

   for( size_t i = 0; i < c_max_lock_attempts; i++ )
   {
      // NOTE: Empty code block for scope purposes.
      {
         guard g( g_mutex );

         if( set_system_variable( name, "<locked>", "" ) )
         {
            acquired = true;
            break;
         }
      }

      msleep( c_lock_attempt_sleep_time );
   }

   if( !acquired )
      throw runtime_error( "unable to acquire lock for system variable '" + name + "'" );
}

system_variable_lock::~system_variable_lock( )
{
   set_system_variable( name, "" );
}

string get_raw_system_variable( const string& name )
{
   guard g( g_mutex );

   string retval;
   string var_name( name );

   bool had_persist_prefix = false;
   bool had_restore_prefix = false;

   if( !var_name.empty( ) )
   {
      if( name[ 0 ] == c_persist_variable_prefix )
         had_persist_prefix = true;
      else if( name[ 0 ] == c_restore_variable_prefix )
         had_restore_prefix = true;

      if( had_persist_prefix || had_restore_prefix )
         var_name.erase( 0, 1 );
   }

   // NOTE: The special system variable prefix is only intended for
   // testing purposes and is only applicable to unrestricted lists.
   string sys_var_prefix;

   if( g_variables.count( c_special_variable_sys_var_prefix ) )
      sys_var_prefix = g_variables[ c_special_variable_sys_var_prefix ];

   g_variables.insert(
    make_pair( to_string( c_special_variable_unix_timestamp ), to_string( unix_timestamp( ) ) ) );

   // NOTE: One or more persistent variables can have their values
   // either stored or restored depending upon the prefix used and
   // optional wildcard characters. If the name was the prefix and
   // nothing else then output all persistent variable names along
   // with their values (for the persist prefix) or instead output
   // just those whose values now differ (for the restore prefix).
   if( had_persist_prefix || had_restore_prefix )
   {
      bool output_all_persistent_variables = false;

      if( var_name.empty( ) && had_persist_prefix )
         output_all_persistent_variables = true;

      ods::bulk_write bulk_write( ciyam_ods_instance( ) );
      scoped_ods_instance ods_instance( ciyam_ods_instance( ) );

      ciyam_ods_file_system( ).set_root_folder( c_system_variables_folder );

      if( !var_name.empty( ) && had_persist_prefix
       && var_name.find_first_of( "?*" ) == string::npos )
      {
         string value;

         if( g_variables.count( var_name ) )
            value = g_variables[ var_name ];

         if( value.empty( ) )
         {
            if( ciyam_ods_file_system( ).has_file( var_name ) )
               ciyam_ods_file_system( ).remove_file( var_name );
         }
         else
            ciyam_ods_file_system( ).store_as_text_file( var_name, value );
      }
      else
      {
         vector< string > variable_files;

         string expr( sys_var_prefix );

         if( var_name.empty( ) || var_name == "*" )
            expr += "*";
         else
            expr = var_name;

         ciyam_ods_file_system( ).list_files( expr, variable_files );

         for( size_t i = 0; i < variable_files.size( ); i++ )
         {
            string next( variable_files[ i ] );

            string value;

            if( had_restore_prefix || output_all_persistent_variables )
            {
               ciyam_ods_file_system( ).fetch_from_text_file( next, value );

               if( !var_name.empty( ) )
                  g_variables[ next ] = value;
               else
               {
                  string next_value;

                  if( g_variables.count( next ) )
                     next_value = g_variables[ next ];

                  if( output_all_persistent_variables || value != next_value )
                  {
                     if( !retval.empty( ) )
                        retval += "\n";
                     retval += next + ' ' + value;
                  }
               }
            }
            else
            {
               if( g_variables.count( next ) )
                  value = g_variables[ next ];

               if( value.empty( ) )
                  ciyam_ods_file_system( ).remove_file( next );
               else
                  ciyam_ods_file_system( ).store_as_text_file( next, value );
            }
         }
      }
   }
   else if( var_name.find_first_of( "?*" ) != string::npos )
   {
      if( var_name == "*" )
         var_name = sys_var_prefix + var_name;

      map< string, string >::const_iterator ci;
      for( ci = g_variables.begin( ); ci != g_variables.end( ); ++ci )
      {
         if( wildcard_match( var_name, ci->first ) )
         {
            if( !retval.empty( ) )
               retval += "\n";
            retval += ci->first + ' ' + ci->second;
         }
      }
   }
   else
   {
      if( g_variables.count( var_name ) )
         retval = g_variables[ var_name ];
   }

   return retval;
}

struct raw_system_variable_getter : variable_getter
{
   string get_value( const string& name ) const { return get_raw_system_variable( name ); }
};

string get_system_variable( const string& name_or_expr )
{
   raw_system_variable_getter raw_getter;
   variable_expression expr( name_or_expr, raw_getter );

   return expr.get_value( );
}

void set_system_variable( const string& name, const string& value )
{
   guard g( g_mutex );

   string val( value );

   bool persist = false;

   if( !name.empty( ) && name[ 0 ] == c_persist_variable_prefix )
      persist = true;

   string var_name( !persist ? name : name.substr( 1 ) );

   if( val == string( c_special_variable_increment )
    || val == string( c_special_variable_decrement ) )
   {
      int num_value = !g_variables.count( var_name )
       ? 0 : from_string< int >( g_variables[ var_name ] );

      if( val == string( c_special_variable_increment ) )
         ++num_value;
      else if( num_value > 0 )
         --num_value;

      if( num_value == 0 )
         val.clear( );
      else
         val = to_string( num_value );
   }

   if( !val.empty( ) )
      g_variables[ var_name ] = val;
   else
   {
      if( g_variables.count( var_name ) )
         g_variables.erase( var_name );
   }

   if( persist )
   {
      ods::bulk_write bulk_write( ciyam_ods_instance( ) );
      scoped_ods_instance ods_instance( ciyam_ods_instance( ) );

      ciyam_ods_file_system( ).set_root_folder( c_system_variables_folder );

      if( !val.empty( ) )
         ciyam_ods_file_system( ).store_as_text_file( var_name, val );
      else if( ciyam_ods_file_system( ).has_file( var_name ) )
         ciyam_ods_file_system( ).remove_file( var_name );
   }
}

bool set_system_variable( const string& name, const string& value, const string& current )
{
   guard g( g_mutex );

   bool retval = false;

   if( !g_variables.count( name ) )
   {
      if( current.empty( ) )
         retval = true;
   }
   else if( current == g_variables[ name ] )
   {
      retval = true;
      g_variables.erase( name );
   }

   if( retval )
   {
      if( !value.empty( ) )
         g_variables[ name ] = value;
      else
      {
         if( g_variables.count( name ) )
            g_variables.erase( name );
      }
   }

   return retval;
}

void list_mutex_lock_ids_for_ciyam_variables( ostream& outs )
{
   outs << "ciyam_variables::g_mutex = " << g_mutex.get_lock_id( ) << '\n';
}

