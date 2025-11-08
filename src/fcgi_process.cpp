// Copyright (c) 2013-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <cstring>
#  include <fstream>
#  include <iostream>
#  include <stdexcept>
#endif

#include "fcgi_process.h"

#include "regex.h"
#include "format.h"
#include "sockets.h"
#ifdef SSL_SUPPORT
#  include "ssl_socket.h"
#endif
#include "threads.h"
#include "fcgi_cmds.h"
#include "fcgi_info.h"
#include "fcgi_list.h"
#include "fcgi_view.h"
#include "date_time.h"
#include "utilities.h"
#include "ciyam_core.h"
#include "fcgi_utils.h"
#include "crypt_stream.h"
#include "ciyam_interface.h"

using namespace std;

extern string g_id;

extern string g_login_html;
extern string g_footer_html;
extern string g_openup_html;
extern string g_signup_html;
extern string g_password_html;
extern string g_ssl_signup_html;
extern string g_authenticate_html;

extern string g_ciyam_interface_html;
extern string g_login_persistent_html;
extern string g_password_persistent_html;

extern string g_display_login_info;
extern string g_display_change_password;
extern string g_display_sign_up_for_an_account;

extern size_t get_num_sessions( );

extern void add_session_info( const string& session_id, session_info* p_session_info );

namespace
{

#include "ciyam_constants.h"

mutex g_join_mutex;

const int c_max_pin_time_difference = 30;

const int c_security_delay_time = 2000;

const int c_initial_response_timeout = 7500;
const int c_subsequent_response_timeout = 2000;

const int c_auto_refresh_seconds_hello = 2;
const int c_auto_refresh_seconds_local = 3;
const int c_auto_refresh_seconds_remote = 5;

const int c_slow_refresh_seconds = 7;

const int c_warn_refresh_seconds = ( c_timeout_seconds / 2 );

const char* const c_checked = "@@checked";
const char* const c_app_name = "@@app_name";
const char* const c_pin_value = "@@pin_value";
const char* const c_selected_ = "@@selected_";
const char* const c_error_message = "@@error_message";

const char* const c_openid_username = "openid_username";
const char* const c_openid_ext_email = "openid.ext1.value.email";

const char* const c_user_key_arg = "$user";
const char* const c_user_perms_arg = "$perms";

const char* const c_dummy_server_command = "wait";

const char* const c_sign_up_types_map = "sign_up_types.map";
const char* const c_sign_up_testers_file = "sign_up_testers.lst";

string get_display_name( const string& str )
{
   string display_name( str );

   if( !display_name.empty( ) && display_name[ 0 ] == '@' )
   {
      string::size_type pos = display_name.find( ' ' );
      if( pos != string::npos )
         display_name.erase( 0, pos + 1 );
   }

   if( display_name == "!" )
      display_name.erase( );

   return display_name;
}

string get_view_or_list_header( const string& qlink,
 const string& name, const module_info& mod_info, const session_info& sess_info, const string* p_vname = 0 )
{
   string str;

   size_t qlink_num = atoi( qlink.c_str( ) );
   if( !qlink.empty( ) && qlink_num < sess_info.quick_link_data.size( ) )
   {
      string columns( sess_info.quick_link_data[ qlink_num ].second );

      vector< string > column_info;
      raw_split( columns, column_info );

      if( column_info.size( ) != 3 )
         throw runtime_error( "unexpected incorrect # columns for quick link info" );

      str = column_info[ 1 ];
   }
   else
   {
      str = get_display_name( mod_info.get_string( name ) );

      if( p_vname && !p_vname->empty( ) && name != *p_vname )
      {
         str += " - ";
         str += get_display_name( mod_info.get_string( *p_vname ) );
      }
   }

   return str;
}

}

void process_fcgi_request( module_info& mod_info, session_info* p_session_info, string& cmd,
 string& act, string& data, string& field, string& extra, string& qlink, string& password,
 const string& pin, const string& raddr, const string& uselect, const string& userhash,
 const string& userfetch, const string& exec, const string& ident, const string& oident,
 const string& special, const string& back, const string& vtabc, const string& bcount, const string& hashval,
 const string& cont, const string& listarg, const string& listvar, const string& listinfo,
 const string& listsort, const string& listsrch, const string& listextra,
 const string& findinfo, const string& app, const string& keepchecks, const string& new_key,
 const string& session_id, const string& module_id, const string& module_ref,
 const string& module_name, const string& scrollx, const string& scrolly, string& server_command,
 const set< string >& menu_opts, const set< string >& selected_records, map< string, string >& input_data,
 view_source& view, view_info_const_iterator& vici, map< string, string >& view_extra_vars,
 list_source& list, list_source& olist, list_info_const_iterator& lici, map< string, string >& list_selections,
 map< string, string >& list_search_text, map< string, string >& list_search_values,
 map< string, list_source >& home_lists, string& user_home_info, map< string, string >& user_field_info,
 map< string, string >& new_field_and_values, map< string, row_error_container >& child_row_errors,
 string& error_message, ostringstream& extra_content, string& extra_content_func, string& activation_file,
 bool& okay, bool& had_send_or_recv_error, bool& record_not_found_error, bool& finished_session, bool module_access_denied,
 bool cookies_permitted, bool temp_session, bool using_anonymous, bool use_url_checksum, bool is_ssl, bool is_sign_in, bool is_in_edit,
 bool is_new_record, bool was_invalid, bool skip_force_fields, bool created_session, bool allow_module_switching, int vtab_num )
{
   int vtabc_num = 1;

   if( !vtabc.empty( ) )
      vtabc_num = atoi( vtabc.c_str( ) );

   set< string > specials;

   int back_count = 1;

   if( !bcount.empty( ) )
   {
      back_count = atoi( bcount.c_str( ) );

      if( back_count < 1 )
         back_count = 1;
   }

   bool keep_checks = false;

   if( keepchecks == c_true_value )
      keep_checks = true;

   bool embed_images = false;

   if( get_storage_info( ).embed_images )
      embed_images = true;

   string title( mod_info.title );

   map< string, int > child_names;
   vector< list_source > child_lists;

   string edit_timeout_func;

   string pdf_view_file_name;
   string pdf_list_file_name;

   string view_permission_value;
   string view_security_level_value;

   bool performed_file_attach_or_detach = false;
   bool has_any_potentially_changing_records = false;

   string app_dir_name( lower( get_storage_info( ).storage_name ) );

   // NOTE: For each tab (with 0 being for the fields above the tabs)
   // a "show" option allows for more or less fields to be displayed.
   map< string, string > show_opts;

   for( int i = 0; i < 10; i++ )
   {
      string show_opt( c_show_prefix );

      show_opt += ( '0' + i );

      if( input_data.count( show_opt ) )
         show_opts.insert( make_pair( show_opt, input_data[ show_opt ] ) );
   }

   if( cmd == c_cmd_home )
   {
      server_command.erase( );

      bool issued_command = false;

      if( !mod_info.sys_class_id.empty( ) )
      {
         issued_command = true;

         fetch_sys_record( module_id, mod_info, *p_session_info );

         ostringstream osstr;

         string sys_message;

         if( !using_anonymous )
            sys_message = p_session_info->sys_message;

         replace_links_and_output( sys_message, "",
          mod_info.name, module_ref, osstr, true, true, session_id, *p_session_info,
          uselect, cookies_permitted, use_url_checksum );

         p_session_info->sys_message = osstr.str( );
      }

      if( !temp_session && !mod_info.home_info.empty( )
       && ( !p_session_info->user_group.empty( ) || !p_session_info->is_admin_user ) )
      {
         issued_command = true;
         string home_info_cmd = mod_info.home_info;

         string perms;
         map< string, string >::const_iterator i;

         for( i = p_session_info->user_perms.begin( ); i != p_session_info->user_perms.end( ); ++i )
         {
            if( !perms.empty( ) )
               perms += ",";
            perms += i->first;
         }

         while( true )
         {
            bool found = false;

            string::size_type pos = home_info_cmd.find( c_user_key_arg );

            if( pos != string::npos )
            {
               found = true;
               home_info_cmd.replace( pos, strlen( c_user_key_arg ), p_session_info->user_key );
            }

            pos = home_info_cmd.find( c_user_perms_arg );

            if( pos != string::npos )
            {
               found = true;
               home_info_cmd.replace( pos, strlen( c_user_perms_arg ), perms );
            }

            if( !found )
               break;
         }

         if( p_session_info->p_socket->write_line( home_info_cmd ) <= 0 )
            had_send_or_recv_error = true;
         else
         {
            string response;

            int timeout = c_initial_response_timeout;

            while( response.empty( ) || response[ 0 ] != '(' )
            {
               if( !response.empty( ) )
                  timeout = c_subsequent_response_timeout;

               response.clear( );

               if( p_session_info->p_socket->read_line( response, timeout ) <= 0 )
               {
                  okay = false;
                  had_send_or_recv_error = true;
               }

               if( response[ 0 ] != '(' )
               {
                  if( !user_home_info.empty( ) )
                     user_home_info += "<br/>\n";

                  user_home_info += replace_crlfs_and_spaces( response, "<br/>", "&nbsp;" );
               }
            }
         }
      }

      for( map< string, list_source >::iterator i = home_lists.begin( ); i != home_lists.end( ); ++i )
      {
         if( !populate_list_info( i->second, list_selections,
          list_search_text, list_search_values, "", "", "", false, 0, "", 0, *p_session_info ) )
         {
            had_send_or_recv_error = true;
            break;
         }
      }

      string dummy_command( string( c_dummy_server_command ) + " \"-u="
       + p_session_info->user_key + ":" + p_session_info->user_id + "\" 0" );

      // NOTE: Issue a dummy command to ensure that a server session timeout won't occur.
      if( !issued_command && !simple_command( *p_session_info, dummy_command ) )
         throw runtime_error( "unexpected response to dummy server command" );
   }
   else if( cmd == c_cmd_pwd )
   {
      string dummy_command( string( c_dummy_server_command ) + " \"-u="
       + p_session_info->user_key + ":" + p_session_info->user_id + "\" 0" );

      // NOTE: Issue a dummy command to ensure that a server session timeout won't occur.
      if( !simple_command( *p_session_info, dummy_command ) )
         throw runtime_error( "unexpected response to dummy server command" );
   }
   else if( ( cmd == c_cmd_view ) || ( cmd == c_cmd_pview ) )
   {
      server_command.erase( );
      pair< string, string > item_info;

      string item_key( data );
      string set_field_values;

      if( is_new_record )
      {
         // NOTE: If editing a record with fixed field values then set these fields in the fetch.
         if( !field.empty( ) )
         {
            set_field_values = field;
            set_field_values += "=" + escaped( escaped( extra, "," ), ",\"", c_nul, "rn\r\n" );
         }

         if( !view.new_field_values.empty( ) )
         {
            set< string > user_force_fields( view.user_force_fields.begin( ), view.user_force_fields.end( ) );

            map< string, string >::iterator i;
            for( i = view.new_field_values.begin( ); i != view.new_field_values.end( ); ++i )
            {
               if( !user_field_info.count( i->first ) && !user_force_fields.count( i->first ) )
               {
                  if( !set_field_values.empty( ) )
                     set_field_values += ",";

                  set_field_values += i->first;
                  set_field_values += "=" + escaped( escaped( i->second, "," ), ",\"", c_nul, "rn\r\n" );
               }
            }
         }

         if( !new_field_and_values.empty( ) )
         {
            map< string, string >::iterator i;
            for( i = new_field_and_values.begin( ); i != new_field_and_values.end( ); ++i )
            {
               if( !set_field_values.empty( ) )
                  set_field_values += ",";

               set_field_values += i->first;
               set_field_values += "=" + escaped( escaped( i->second, "," ), ",\"", c_nul, "rn\r\n" );
            }
         }

         if( !view.create_user_key_field.empty( ) )
         {
            if( !set_field_values.empty( ) )
               set_field_values += ",";

            set_field_values += view.create_user_key_field;
            set_field_values += "=" + p_session_info->user_key;
         }
      }

      bool was_user_force_field = false;

      if( is_new_record
       || ( act == c_act_edit ) || ( act == c_act_exec ) || ( act == c_act_view ) )
      {
         // NOTE: In the case where an edit occurs via an action a field assignment may
         // be required via use of the field/extra query string variables.
         if( ( act == c_act_edit ) && !is_new_record && !field.empty( ) )
         {
            if( !set_field_values.empty( ) )
               set_field_values += ",";

            set_field_values += field + "=" + extra;
         }

         // NOTE: If a "force", "fetch" or "fkey" marked field has been changed then
         // the user-provided value will be passed to the application server.
         if( !userfetch.empty( ) )
         {
            if( user_field_info.count( userfetch ) )
            {
               if( !set_field_values.empty( ) )
                  set_field_values += ",";

               set_field_values += userfetch;
               set_field_values += "=" + escaped( escaped( user_field_info[ userfetch ], "," ), ",\"", c_nul, "rn\r\n" );

               // NOTE: Set an instance variable so the application server can identify the trigger field.
               if( is_new_record || ( act == c_act_edit ) )
                  set_field_values += ",@trigger=" + userfetch;
            }
         }

         // NOTE: Any "force" fields will always have their user provided values
         // sent to the application server.
         if( !skip_force_fields )
         {
            string sid;
            get_server_sid( sid );

            for( size_t i = 0; i < view.user_force_fields.size( ); i++ )
            {
               if( userfetch == view.user_force_fields[ i ] )
                  was_user_force_field = true;
               else if( user_field_info.count( view.user_force_fields[ i ] ) )
               {
                  if( view.user_force_fields[ i ] != field )
                  {
                     if( !set_field_values.empty( ) )
                        set_field_values += ",";
                     set_field_values += view.user_force_fields[ i ];

                     // NOTE: If the "force" field is flagged as an encrypted field
                     // then will encrypt it now (unless it is also a hidden field).
                     if( !view.hidden_fields.count( view.user_force_fields[ i ] )
                      && view.encrypted_fields.count( view.user_force_fields[ i ] ) )
                     {
                       set_field_values += "=" + data_encrypt(
                        user_field_info[ view.user_force_fields[ i ] ], sid );
                     }
                     else
                        set_field_values += "=" + escaped( escaped(
                         user_field_info[ view.user_force_fields[ i ] ], "," ), ",\"", c_nul, "rn\r\n" );
                  }
               }
            }

            clear_key( sid );
         }
      }

      // NOTE: The view field list is assembled here as the edit state is now certain (i.e. if a save failed).
      for( size_t i = 0; i < ( vici->second )->fields.size( ); i++ )
      {
         fld_info fld( ( vici->second )->fields[ i ] );

         if( fld.field != c_key_field )
         {
            string pfield( fld.pfield );

            // NOTE: If a foreign key has been marked for manual entry then the parent field
            // will be modified from the display field to the manual entry field if editing.
            string::size_type pos = pfield.find( '+' );
            if( pos != string::npos )
            {
               if( !is_new_record && act != c_act_edit )
                  pfield.erase( pos );
               else
                  pfield.erase( 0, pos + 1 );

               if( pfield == c_key_field )
                  pfield.erase( );
            }

            if( !view.field_list.empty( ) )
               view.field_list += ",";

            view.field_list += fld.field;

            if( !pfield.empty( ) )
               view.field_list += "." + pfield;
         }
      }

      // NOTE: The actions field is included so that actions can be executed in view mode.
      if( !view.actions_field.empty( ) )
         view.field_list += "," + view.actions_field;

      // NOTE: Foreign keys are included so that hyperlinks to them can be provided in view mode.
      for( size_t i = 0; i < view.fk_field_ids.size( ); i++ )
         view.field_list += "," + view.fk_field_ids[ i ];

      string user_info( p_session_info->user_key + ":" + p_session_info->user_id );

      if( act == c_act_exec )
      {
         if( !set_field_values.empty( ) )
            set_field_values += ",";

         string procedure_id;
         for( size_t i = 0; i < exec.size( ); i++ )
         {
            if( ( exec[ i ] >= '0' && exec[ i ] <= '9' )
             || ( exec[ i ] >= 'A' && exec[ i ] <= 'Z' ) )
               procedure_id += exec[ i ];

            if( exec[ i ] == ':' || exec[ i ] == ';' || exec[ i ] == ',' || exec[ i ] == '+' || exec[ i ] == ' ' )
               break;
         }

         set_field_values += "@executed=" + procedure_id;
      }

      if( !fetch_item_info( view.module_id,
       mod_info, view.cid, item_key, view.field_list, set_field_values,
       "", *p_session_info, item_info, 0, 0, 0, 0, 0, &view_extra_vars ) )
         had_send_or_recv_error = true;
      else
      {
         uint64_t state;
         string type_info;
         string key_and_version;
         vector< string > item_values;

         raw_split( item_info.second, item_values );

         string key_ver_rev_state_and_type_info( item_info.first );

         if( !key_ver_rev_state_and_type_info.empty( ) )
            parse_key_ver_rev_state_and_type_info( key_ver_rev_state_and_type_info, key_and_version, state, type_info );

         if( !is_new_record && key_ver_rev_state_and_type_info.empty( ) )
            record_not_found_error = true;

         string filename_value;

         bool ignore_encrypted = false;

         if( !record_not_found_error )
         {
            view.state = state;
            view.key_info = key_and_version;

            size_t field_num = 0;

            bool has_always_editable = false;

            string sid;
            get_server_sid( sid );

            for( size_t i = 0; i < view.field_ids.size( ); i++ )
            {
               if( view.field_ids[ i ] == c_key_field )
                  continue;

               string item_value;
               if( field_num < item_values.size( ) )
                  item_value = item_values[ field_num ];

               map< string, string > extra_data;
               if( !view.vici->second->fields[ i ].extra.empty( ) )
                  parse_field_extra( view.vici->second->fields[ i ].extra, extra_data );

               // NOTE: If an "ignore_encrypted" field is present then it will need to appear *before*
               // any actual fields that have the "encrypted" extra.
               if( view.field_ids[ i ] == view.ignore_encrypted_field )
                  ignore_encrypted = ( item_value == string( c_true_value ) );

               if( !( state & c_state_is_changing ) && extra_data.count( c_view_field_extra_always_editable ) )
                  has_always_editable = true;

               if( state & c_state_potentially_changing )
                  has_any_potentially_changing_records = true;

               // NOTE: Encrypted fields that are < 20 characters are assumed to not have been encrypted.
               if( !ignore_encrypted
                && item_value.length( ) >= 20
                && !view.file_fields.count( view.field_ids[ i ] )
                && !view.image_fields.count( view.field_ids[ i ] )
                && !view.hidden_fields.count( view.field_ids[ i ] )
                && view.encrypted_fields.count( view.field_ids[ i ] ) )
               {
                  item_value = data_decrypt( item_value, sid );

                  item_values[ field_num ] = item_value;
               }

               if( view.field_ids[ i ] == view.filename_field )
                  filename_value = item_value;

               if( view.field_ids[ i ] == view.permission_field )
                  view_permission_value = item_value;

               if( view.field_ids[ i ] == view.security_level_field )
                  view_security_level_value = item_value;

               if( view.key_ids.id0 == view.value_ids[ i ] )
                  view.key_values.value0 = item_value;
               else if( view.key_ids.id1 == view.value_ids[ i ] )
                  view.key_values.value1 = item_value;
               else if( view.key_ids.id2 == view.value_ids[ i ] )
                  view.key_values.value2 = item_value;
               else if( view.key_ids.id3 == view.value_ids[ i ] )
                  view.key_values.value3 = item_value;
               else if( view.key_ids.id4 == view.value_ids[ i ] )
                  view.key_values.value4 = item_value;
               else if( view.key_ids.id5 == view.value_ids[ i ] )
                  view.key_values.value5 = item_value;
               else if( view.key_ids.id6 == view.value_ids[ i ] )
                  view.key_values.value6 = item_value;
               else if( view.key_ids.id7 == view.value_ids[ i ] )
                  view.key_values.value7 = item_value;
               else if( view.key_ids.id8 == view.value_ids[ i ] )
                  view.key_values.value8 = item_value;
               else if( view.key_ids.id9 == view.value_ids[ i ] )
                  view.key_values.value9 = item_value;

               if( view.vextra1_id == view.value_ids[ i ] )
                  view.vextra1_value = item_value;
               else if( view.vextra2_id == view.value_ids[ i ] )
                  view.vextra2_value = item_value;

               if( field_num < item_values.size( ) )
                  view.field_values[ view.value_ids[ i ] ] = item_values[ field_num++ ];
            }

            clear_key( sid );

            // NOTE: If a "force" field triggered the POST then user values for any
            // "forced" fields are replaced by the value retrieved from the server.
            if( was_user_force_field )
            {
               for( size_t j = 0; j < view.server_forced_fields.size( ); j++ )
               {
                  if( user_field_info.count( view.server_forced_fields[ j ] ) )
                     user_field_info[ view.server_forced_fields[ j ] ] = view.field_values[ view.server_forced_fields[ j ] ];
               }
            }

            // NOTE: The value for the action field that was previously appended to the fetch is extracted here.
            if( !view.actions_field.empty( ) && field_num < item_values.size( ) )
               view.actions_value = item_values[ field_num++ ];

            // NOTE: Values for foreign key fields that were previously appended to the fetch are extracted here.
            for( size_t i = 0; i < view.fk_field_ids.size( ); i++ )
            {
               bool is_forced = false;
               for( size_t j = 0; j < view.server_forced_fields.size( ); j++ )
               {
                  if( view.fk_field_ids[ i ] == view.server_forced_fields[ j ] )
                  {
                     is_forced = true;
                     break;
                  }
               }

               string item_value;
               if( field_num < item_values.size( ) )
                  item_value = item_values[ field_num ];

               // NOTE: If a "forced" field was actually a foreign key then replace its value with the key.
               if( is_forced && user_field_info.count( view.fk_field_ids[ i ] ) )
                  user_field_info[ view.fk_field_ids[ i ] ] = item_value;

               // NOTE: If field is not "forced" and has a user value then replace the item value with
               // the user value.
               if( !is_forced && user_field_info.count( view.fk_field_ids[ i ] ) )
                  item_value = user_field_info[ view.fk_field_ids[ i ] ];

               if( view.fkey_ids.id0 == view.fk_field_ids[ i ] )
                  view.fkey_values.value0 = item_value;
               else if( view.fkey_ids.id1 == view.fk_field_ids[ i ] )
                  view.fkey_values.value1 = item_value;
               else if( view.fkey_ids.id2 == view.fk_field_ids[ i ] )
                  view.fkey_values.value2 = item_value;
               else if( view.fkey_ids.id3 == view.fk_field_ids[ i ] )
                  view.fkey_values.value3 = item_value;
               else if( view.fkey_ids.id4 == view.fk_field_ids[ i ] )
                  view.fkey_values.value4 = item_value;
               else if( view.fkey_ids.id5 == view.fk_field_ids[ i ] )
                  view.fkey_values.value5 = item_value;
               else if( view.fkey_ids.id6 == view.fk_field_ids[ i ] )
                  view.fkey_values.value6 = item_value;
               else if( view.fkey_ids.id7 == view.fk_field_ids[ i ] )
                  view.fkey_values.value7 = item_value;
               else if( view.fkey_ids.id8 == view.fk_field_ids[ i ] )
                  view.fkey_values.value8 = item_value;
               else if( view.fkey_ids.id9 == view.fk_field_ids[ i ] )
                  view.fkey_values.value9 = item_value;

               if( field_num < item_values.size( ) )
                  view.fk_field_values[ view.fk_field_ids[ i ] ] = item_values[ field_num++ ];
            }

            // NOTE: If a "key" source is editable then use its current value.
            if( user_field_info.count( view.key_ids.id0 ) )
               view.key_values.value0 = user_field_info[ view.key_ids.id0 ];

            if( user_field_info.count( view.key_ids.id1 ) )
               view.key_values.value1 = user_field_info[ view.key_ids.id1 ];

            if( user_field_info.count( view.key_ids.id2 ) )
               view.key_values.value2 = user_field_info[ view.key_ids.id2 ];

            if( user_field_info.count( view.key_ids.id3 ) )
               view.key_values.value3 = user_field_info[ view.key_ids.id3 ];
                     
            if( user_field_info.count( view.key_ids.id4 ) )
               view.key_values.value4 = user_field_info[ view.key_ids.id4 ];

            if( user_field_info.count( view.key_ids.id5 ) )
               view.key_values.value5 = user_field_info[ view.key_ids.id5 ];

            if( user_field_info.count( view.key_ids.id6 ) )
               view.key_values.value6 = user_field_info[ view.key_ids.id6 ];

            if( user_field_info.count( view.key_ids.id7 ) )
               view.key_values.value7 = user_field_info[ view.key_ids.id7 ];

            if( user_field_info.count( view.key_ids.id8 ) )
               view.key_values.value8 = user_field_info[ view.key_ids.id8 ];

            if( user_field_info.count( view.key_ids.id9 ) )
               view.key_values.value9 = user_field_info[ view.key_ids.id9 ];

            // NOTE: If an "fkey" source is editable then use its current value.
            if( user_field_info.count( view.fkey_ids.id0 ) )
               view.fkey_values.value0 = user_field_info[ view.fkey_ids.id0 ];

            if( user_field_info.count( view.fkey_ids.id1 ) )
               view.fkey_values.value1 = user_field_info[ view.fkey_ids.id1 ];

            if( user_field_info.count( view.fkey_ids.id2 ) )
               view.fkey_values.value2 = user_field_info[ view.fkey_ids.id2 ];

            if( user_field_info.count( view.fkey_ids.id3 ) )
               view.fkey_values.value3 = user_field_info[ view.fkey_ids.id3 ];

            if( user_field_info.count( view.fkey_ids.id4 ) )
               view.fkey_values.value4 = user_field_info[ view.fkey_ids.id4 ];

            if( user_field_info.count( view.fkey_ids.id5 ) )
               view.fkey_values.value5 = user_field_info[ view.fkey_ids.id5 ];

            if( user_field_info.count( view.fkey_ids.id6 ) )
               view.fkey_values.value6 = user_field_info[ view.fkey_ids.id6 ];

            if( user_field_info.count( view.fkey_ids.id7 ) )
               view.fkey_values.value7 = user_field_info[ view.fkey_ids.id7 ];

            if( user_field_info.count( view.fkey_ids.id8 ) )
               view.fkey_values.value8 = user_field_info[ view.fkey_ids.id8 ];

            if( user_field_info.count( view.fkey_ids.id9 ) )
               view.fkey_values.value9 = user_field_info[ view.fkey_ids.id9 ];

            // NOTE: Populate lists for user selection (only if editing an existing or creating a new
            // record or if the view contains one ore more "is_always_editable" fields).
            // FUTURE: If not editing or new then only fetch data for the "is_always_editable" fields.
            if( ( cmd != c_cmd_pview )
             && ( is_new_record || has_always_editable || ( act == c_act_edit ) ) )
            {
               if( !special.empty( ) )
                  specials.insert( special );

               map< string, string > previous_parents;

               for( size_t i = 0; i < ( vici->second )->fields.size( ); i++ )
               {
                  fld_info fld( ( vici->second )->fields[ i ] );

                  string field_id( fld.field );

                  string value_id( field_id );

                  if( !fld.pfield.empty( ) )
                     value_id += "." + fld.pfield;

                  // NOTE: If a "key/fkey" user is also an "key/fkey" owner then its "key/fkey" value
                  // must be erased if the "key/fkey" it uses has been changed. This is cascaded down
                  // in field order (so "key/fkey" users must appear after their owners). Because the
                  // string "find" function is being used the "fkey" tests are being performed before
                  // any "key" tests are.
                  for( set< string >::const_iterator ci = specials.begin( ); ci != specials.end( ); ++ci )
                  {
                     if( fld.pextra.find( *ci ) != string::npos )
                     {
                        if( fld.extra.find( c_view_field_extra_fkey0 ) != string::npos )
                        {
                           view.fkey_values.value0.erase( );
                           specials.insert( c_parent_extra_fkey0 );
                        }
                        else if( fld.extra.find( c_view_field_extra_fkey1 ) != string::npos )
                        {
                           view.fkey_values.value1.erase( );
                           specials.insert( c_parent_extra_fkey1 );
                        }
                        else if( fld.extra.find( c_view_field_extra_fkey2 ) != string::npos )
                        {
                           view.fkey_values.value2.erase( );
                           specials.insert( c_parent_extra_fkey2 );
                        }
                        else if( fld.extra.find( c_view_field_extra_fkey3 ) != string::npos )
                        {
                           view.fkey_values.value3.erase( );
                           specials.insert( c_parent_extra_fkey3 );
                        }
                        else if( fld.extra.find( c_view_field_extra_fkey4 ) != string::npos )
                        {
                           view.fkey_values.value4.erase( );
                           specials.insert( c_parent_extra_fkey4 );
                        }
                        else if( fld.extra.find( c_view_field_extra_fkey5 ) != string::npos )
                        {
                           view.fkey_values.value5.erase( );
                           specials.insert( c_parent_extra_fkey5 );
                        }
                        else if( fld.extra.find( c_view_field_extra_fkey6 ) != string::npos )
                        {
                           view.fkey_values.value6.erase( );
                           specials.insert( c_parent_extra_fkey6 );
                        }
                        else if( fld.extra.find( c_view_field_extra_fkey7 ) != string::npos )
                        {
                           view.fkey_values.value7.erase( );
                           specials.insert( c_parent_extra_fkey7 );
                        }
                        else if( fld.extra.find( c_view_field_extra_fkey8 ) != string::npos )
                        {
                           view.fkey_values.value8.erase( );
                           specials.insert( c_parent_extra_fkey8 );
                        }
                        else if( fld.extra.find( c_view_field_extra_fkey9 ) != string::npos )
                        {
                           view.fkey_values.value9.erase( );
                           specials.insert( c_parent_extra_fkey9 );
                        }
                        else if( fld.extra.find( c_view_field_extra_key0 ) != string::npos )
                        {
                           view.key_values.value0.erase( );
                           specials.insert( c_parent_extra_key0 );
                        }
                        else if( fld.extra.find( c_view_field_extra_key1 ) != string::npos )
                        {
                           view.key_values.value1.erase( );
                           specials.insert( c_parent_extra_key1 );
                        }
                        else if( fld.extra.find( c_view_field_extra_key2 ) != string::npos )
                        {
                           view.key_values.value2.erase( );
                           specials.insert( c_parent_extra_key2 );
                        }
                        else if( fld.extra.find( c_view_field_extra_key3 ) != string::npos )
                        {
                           view.key_values.value3.erase( );
                           specials.insert( c_parent_extra_key3 );
                        }
                        else if( fld.extra.find( c_view_field_extra_key4 ) != string::npos )
                        {
                           view.key_values.value4.erase( );
                           specials.insert( c_parent_extra_key4 );
                        }
                        else if( fld.extra.find( c_view_field_extra_key5 ) != string::npos )
                        {
                           view.key_values.value5.erase( );
                           specials.insert( c_parent_extra_key5 );
                        }
                        else if( fld.extra.find( c_view_field_extra_key6 ) != string::npos )
                        {
                           view.key_values.value6.erase( );
                           specials.insert( c_parent_extra_key6 );
                        }
                        else if( fld.extra.find( c_view_field_extra_key7 ) != string::npos )
                        {
                           view.key_values.value7.erase( );
                           specials.insert( c_parent_extra_key7 );
                        }
                        else if( fld.extra.find( c_view_field_extra_key8 ) != string::npos )
                        {
                           view.key_values.value8.erase( );
                           specials.insert( c_parent_extra_key8 );
                        }
                        else if( fld.extra.find( c_view_field_extra_key9 ) != string::npos )
                        {
                           view.key_values.value9.erase( );
                           specials.insert( c_parent_extra_key9 );
                        }
                     }
                  }

                  // NOTE: Parent fields that are protected or fixed are not editable and are therefore
                  // skipped (unless was provided with a default value from the interface specification).
                  if( !fld.pclass.empty( ) && field_id != field
                   && ( !view.protected_fields.count( value_id ) || view.new_field_values.count( field_id ) ) )
                  {
                     data_container parent_row_data;

                     // NOTE: If the foreign key has been flagged for manual entry (as would occur in
                     // situations where the number of records is too unwieldy to hold in a drop list)
                     // then do not fetch the foreign key records.
                     string pfield( fld.pfield );

                     string::size_type pos = pfield.find( '+' );

                     if( pos != string::npos )
                        continue;

                     string parent_key;

                     if( !field.empty( ) )
                        parent_key = extra;

                     string info( fld.pclass + pfield + fld.pextra + parent_key );

                     // NOTE: As a performance optimisation if a previous field has the exact same
                     // parent query information then simply copy this data rather than re-query.
                     //
                     // FUTURE: Currently "skey" dependent queries are issued even when the "skey"
                     // needed has not been selected (such unnecessary queries should be removed).
                     if( previous_parents.count( info ) )
                        parent_row_data = view.parent_lists[ previous_parents[ info ] ];
                     else if( !fetch_parent_row_data( view.module_id, mod_info,
                      data, fld.field, fld.pclass, pfield, fld.pextra, *p_session_info,
                      parent_key, parent_row_data, &view.key_values, &view.fkey_values ) )
                     {
                        had_send_or_recv_error = true;
                        break;
                     }

                     previous_parents.insert( make_pair( info, field_id ) );

                     view.parent_lists.insert( make_pair( field_id, parent_row_data ) );

                     // NOTE: For mandatory fields where data has been found that are key/fkey
                     // field owners grab the key/fkey value from the first record (if no value
                     // was provided from the fetched record or user provided values).
                     if( !parent_row_data.empty( ) && view.mandatory_fields.count( value_id ) )
                     {
                        string key( parent_row_data[ 0 ].first );

                        string::size_type pos = key.find( ' ' );

                        if( pos != string::npos )
                           key.erase( pos );

                        if( view.key_values.value0.empty( ) && field_id == view.key_ids.id0 )
                           view.key_values.value0 = key;

                        if( view.key_values.value1.empty( ) && field_id == view.key_ids.id1 )
                           view.key_values.value1 = key;

                        if( view.key_values.value2.empty( ) && field_id == view.key_ids.id2 )
                           view.key_values.value2 = key;

                        if( view.key_values.value3.empty( ) && field_id == view.key_ids.id3 )
                           view.key_values.value3 = key;

                        if( view.key_values.value4.empty( ) && field_id == view.key_ids.id4 )
                           view.key_values.value4 = key;

                        if( view.key_values.value5.empty( ) && field_id == view.key_ids.id5 )
                           view.key_values.value5 = key;

                        if( view.key_values.value6.empty( ) && field_id == view.key_ids.id6 )
                           view.key_values.value6 = key;

                        if( view.key_values.value7.empty( ) && field_id == view.key_ids.id7 )
                           view.key_values.value7 = key;

                        if( view.key_values.value8.empty( ) && field_id == view.key_ids.id8 )
                           view.key_values.value8 = key;
                                 
                        if( view.key_values.value9.empty( ) && field_id == view.key_ids.id9 )
                           view.key_values.value9 = key;

                        if( view.fkey_values.value0.empty( ) && field_id == view.fkey_ids.id0 )
                           view.fkey_values.value0 = key;

                        if( view.fkey_values.value1.empty( ) && field_id == view.fkey_ids.id1 )
                           view.fkey_values.value1 = key;

                        if( view.fkey_values.value2.empty( ) && field_id == view.fkey_ids.id2 )
                           view.fkey_values.value2 = key;

                        if( view.fkey_values.value3.empty( ) && field_id == view.fkey_ids.id3 )
                           view.fkey_values.value3 = key;

                        if( view.fkey_values.value4.empty( ) && field_id == view.fkey_ids.id4 )
                           view.fkey_values.value4 = key;

                        if( view.fkey_values.value5.empty( ) && field_id == view.fkey_ids.id5 )
                           view.fkey_values.value5 = key;

                        if( view.fkey_values.value6.empty( ) && field_id == view.fkey_ids.id6 )
                           view.fkey_values.value6 = key;

                        if( view.fkey_values.value7.empty( ) && field_id == view.fkey_ids.id7 )
                           view.fkey_values.value7 = key;

                        if( view.fkey_values.value8.empty( ) && field_id == view.fkey_ids.id8 )
                           view.fkey_values.value8 = key;

                        if( view.fkey_values.value9.empty( ) && field_id == view.fkey_ids.id9 )
                           view.fkey_values.value9 = key;
                     }
                  }
               }
            }

            if( vtabc.empty( ) && view.vici->second->extras.count( c_view_type_extra_vtabc ) )
               vtabc_num = atoi( view.vici->second->extras[ c_view_type_extra_vtabc ].c_str( ) );

            // FUTURE: Printable views do not currently support child lists.
            if( !had_send_or_recv_error && !is_new_record && act != c_act_edit && cmd != c_cmd_pview )
            {
               int num = 0;

               map< string, list_info_const_iterator > children;

               // NOTE: First determine the order of child tabs according to name.
               for( lici = mod_info.list_info.begin( ); lici != mod_info.list_info.end( ); ++lici )
               {
                  if( ( lici->second->pclass == view.cid )
                   && ( ( lici->second->type == c_list_type_child )
                   || ( lici->second->type == c_list_type_user_child )
                   || ( lici->second->type == c_list_type_child_admin )
                   || ( lici->second->type == c_list_type_child_owner )
                   || ( lici->second->type == c_list_type_child_admin_owner ) ) )
                     children.insert( make_pair( mod_info.get_string( lici->second->name ), lici ) );
               }

               for( map< string, list_info_const_iterator >::iterator i = children.begin( ); i != children.end( ); ++i )
               {
                  list_info_const_iterator lici = i->second;

                  list_source child_list;

                  child_list.id = c_list_prefix + to_string( child_lists.size( ) );

                  child_list.module_id = module_id;
                  child_list.module_ref = module_ref;

                  child_list.lici = lici;

                  setup_list_fields( child_list, data, module_name, *p_session_info, cmd == c_cmd_pview );

                  string class_view_id;

                  bool class_does_not_match = false;

                  // NOTE: For derived classes a child list intended to display instances of
                  // a base class may use a parent self relationship to the derived class so
                  // the correct view needs to be determined by checking the class id's.
                  view_info_const_iterator vici;

                  for( vici = mod_info.view_info.begin( ); vici != mod_info.view_info.end( ); ++vici )
                  {
                     if( ( vici->second )->cid == ( lici->second )->cid )
                        class_view_id = ( vici->second )->id;

                     if( ( vici->second )->id == ( lici->second )->view
                      && ( vici->second )->cid != ( lici->second )->cid )
                        class_does_not_match = true;
                  }

                  if( !class_view_id.empty( ) && class_does_not_match )
                     child_list.view = class_view_id;

                  if( listarg == child_list.id )
                     child_list.row_errors = child_row_errors[ listarg ];

                  child_lists.push_back( child_list );

                  child_names.insert( make_pair( mod_info.get_string( child_lists.back( ).name ), child_lists.size( ) - 1 ) );
               }
            }

            // NOTE: If this class has file attachments then check whether a new file has been attached
            // or an existing one needs to be removed.
            //
            // FUTURE: Currently no version information is included - the version/revision could be included as
            // part of the "info" file data by including it in the "name" attribute (as is done for the class).
            if( !had_send_or_recv_error && view.has_file_attachments )
            {
               string class_id( ( vici->second )->cid );

               string module_id( get_module_id_for_attached_file( view ) );

               string relative_prefix( string( c_files_directory ) + "/" + module_id + "/" + class_id );

               string new_file_info( relative_prefix + "/" + session_id );

               if( ( act == c_act_remove ) || file_exists( new_file_info ) )
               {
                  string new_file;
                  string new_source;

                  if( file_exists( new_file_info ) )
                  {
                     ifstream inpf( new_file_info.c_str( ) );

                     getline( inpf, new_file );

                     string::size_type pos = new_file.find( ' ' );

                     // NOTE: The new source is the file name that
                     // came from the browser (without extension).
                     if( pos != string::npos )
                     {
                        new_source = new_file.substr( pos + 1 );

                        new_file.erase( pos );
                     }
                  }

                  vector< pair< string, string > > field_value_pairs;

                  // NOTE: It is expected that the file name in the "new_file_info" has been prefixed
                  // with the file's path so this is now removed for the attached file field's value.
                  string new_file_name( new_file );

                  if( !new_file_name.empty( ) && ( new_file_name[ 0 ] == '>' ) )
                  {
                     size_t max_size = atoi( new_file_name.substr( 1 ).c_str( ) );

                     error_message = string( GDS( c_display_error ) ) + ": " +
                      string_message( GDS( c_display_upload_max_length_exceeded ),
                      make_pair( c_display_upload_max_length_exceeded_parm_size, format_bytes( max_size ) ) );
                  }
                  else
                  {
#ifndef _WIN32
                     string::size_type pos = new_file.find_last_of( "/" );
#else
                     string::size_type pos = new_file.find_last_of( "/\\" );
#endif
                     if( pos != string::npos )
                        new_file_name = new_file.substr( pos + 1 );

                     string file_field_id;

                     if( new_file_name.empty( ) )
                        file_field_id = app;
                     else
                     {
                        pos = new_file_name.find_last_of( "-" );

                        if( pos == string::npos )
                           throw runtime_error( "unexpected attached file name '" + new_file_name + "'" );

                        file_field_id = new_file_name.substr( pos + 1 );

                        pos = file_field_id.find( '.' );

                        if( pos != string::npos )
                           file_field_id.erase( pos );
                     }

                     field_value_pairs.push_back( make_pair( file_field_id, new_file_name ) );

                     // NOTE: This needs to passed to "valid_file_name" to prevent UTF-8 characters from being stripped.
                     bool has_utf8_chars = false;

                     if( !new_source.empty( ) && !view.filename_field.empty( ) )
                        field_value_pairs.push_back( make_pair( view.filename_field, valid_file_name( new_source, &has_utf8_chars ) ) );

                     // NOTE: If a "modified" date/time field exists then update this to the current
                     // date/time.
                     if( !view.modify_datetime_field.empty( ) )
                        field_value_pairs.push_back(
                         make_pair( view.modify_datetime_field, "U" + date_time::standard( ).as_string( ) ) );

                     string fields_and_values_prefix;

                     if( act != c_act_remove )
                        fields_and_values_prefix = "@extra_field_values";

                     if( perform_update( view.module_id,
                      view.cid, data, field_value_pairs, *p_session_info, &error_message, &fields_and_values_prefix ) )
                     {
                        performed_file_attach_or_detach = true;

                        if( !view.field_values[ file_field_id ].empty( ) )
                        {
                           string old_file( relative_prefix + "/" + view.field_values[ file_field_id ] );

                           if( ( old_file != new_file ) && file_exists( old_file ) )
                              file_remove( old_file );
                        }

                        if( new_file.empty( ) )
                           view.new_file_name = ".";
                        else
                           view.new_file_name = new_file;
                     }
                     else
                     {
                        if( !new_file.empty( ) )
                           file_remove( new_file );

                        if( error_message.length( ) > strlen( c_response_error_prefix )
                         && error_message.substr( 0, strlen( c_response_error_prefix ) ) == c_response_error_prefix )
                           error_message = GDS( c_display_error ) + ": "
                            + error_message.substr( strlen( c_response_error_prefix ) );
                     }
                  }

                  if( file_exists( new_file_info ) )
                  {
                     // NOTE: If this file removal is omitted
                     // then the UI will be caught in a loop.
                     file_remove( new_file_info );

                     p_session_info->was_file_remove = true;
                  }
               }
            }
         }

         // NOTE: If there is a PDF specification then generate the file (unless session is anonymous).
         if( !using_anonymous && !view.pdf_spec_name.empty( ) )
         {
            string owner;

            if( !view.owning_user_field.empty( ) && view.fk_field_values.count( view.owning_user_field ) )
               owner = view.fk_field_values.find( view.owning_user_field )->second;

            string pdf_title = get_view_or_list_header( qlink, view.name, mod_info, *p_session_info );

            if( !fetch_item_info( view.module_id, mod_info,
             view.cid, item_key, view.field_list, set_field_values,
             "", *p_session_info, item_info, &owner, &view.pdf_spec_name,
             &pdf_title, &filename_value, &pdf_view_file_name ) )
               throw runtime_error( "unexpected error occurred generating PDF" );
         }
      }
   }
   else if( ( cmd == c_cmd_list ) || ( cmd == c_cmd_plist ) )
   {
      server_command.erase( );
      list.id = c_list_prefix;

      string list_page_info;
      if( cmd == c_cmd_list )
         list_page_info = listinfo;

      if( listextra == c_list_extra_find )
      {
         vector< string > pairs;

         if( !findinfo.empty( ) )
            split( findinfo, pairs );

         for( size_t i = 0; i < pairs.size( ); i++ )
         {
            string::size_type pos = pairs[ i ].find( '=' );

            if( pos == string::npos )
               throw runtime_error( "unexpected pair format '" + pairs[ i ] + "'" );

            list_search_values.insert( make_pair( pairs[ i ].substr( 0, pos ), pairs[ i ].substr( pos + 1 ) ) );
         }
      }

      // NOTE: As an "skey" user could itself be an "skey" owner need to create a set of
      // effectively changed "skey" owners so all dependent "skey" users can be correctly
      // identified in order to ignore their restrictions.
      if( !special.empty( ) )
      {
         specials.insert( special );

         size_t last_size = specials.size( );

         while( true )
         {
            for( set< string >::iterator si = specials.begin( ); si != specials.end( ); ++si )
            {
               for( size_t i = 0; i < ( list.lici->second )->parents.size( ); i++ )
               {
                  if( ( list.lici->second )->parents[ i ].pextra.find( *si ) != string::npos
                   && ( list.lici->second )->parents[ i ].operations.count( c_operation_select ) )
                  {
                     string suffix( ( list.lici->second )->parents[ i ].operations[ c_operation_select ] );

                     string::size_type pos = suffix.find( '!' );
                     string::size_type npos = suffix.substr( 0, pos ).find( '#' );

                     if( npos != 0 && pos != string::npos )
                        specials.insert( "@" + suffix.substr( 0, npos == string::npos ? pos : npos ) );
                  }
               }
            }

            if( specials.size( ) == last_size )
               break;

            last_size = specials.size( );
         }
      }

      if( !populate_list_info( list,
       list_selections, list_search_text, list_search_values, list_page_info,
       listsort, "", ( cmd == c_cmd_plist ), 0, "", &specials, *p_session_info ) )
         had_send_or_recv_error = true;
      else if( list.row_data.size( ) )
      {
         // FUTURE: For improved efficiency (and for child lists
         // if is wanted) "has_any_potentially_changing_records"
         // should be handled as "has_any_changing_records" is.
         for( size_t i = 0; i < list.row_data.size( ); i++ )
         {
            uint64_t state = 0;

            string type_info;
            string key_and_version;

            string key_ver_rev_state_and_type_info( list.row_data[ i ].first );

            parse_key_ver_rev_state_and_type_info( key_ver_rev_state_and_type_info, key_and_version, state, type_info );

            if( state & c_state_potentially_changing )
            {
               has_any_potentially_changing_records = true;
               break;
            }
         }
      }

      string link_name( get_view_or_list_header( qlink, olist.name, mod_info, *p_session_info, &list.name ) );

      // NOTE: If there is a PDF specification then generate the file (unless session is anonymous).
      if( !using_anonymous && ( cmd == c_cmd_plist ) && !had_send_or_recv_error && !list.pdf_spec_name.empty( ) )
      {
         populate_list_info( list, list_selections, list_search_text, list_search_values,
          list_page_info, listsort, "", ( cmd == c_cmd_plist ), 0, "", &specials, *p_session_info,
          &list.pdf_spec_name, &link_name, &pdf_list_file_name );
      }
   }
#ifndef REMOVE_OR_COMMENT_THIS_OUT_IN_CONFIG_H
   else if( cmd != c_cmd_quit )
      server_command.erase( );
#endif

   bool is_editable = false;
   bool view_has_a_child_list = false;
   bool has_any_changing_records = false;

   if( !finished_session && !server_command.empty( ) )
   {
      if( server_command != c_cmd_quit && p_session_info->p_socket->write_line( server_command ) <= 0 )
         had_send_or_recv_error = true;

      if( server_command == c_cmd_quit )
         finished_session = true;
   }

   if( had_send_or_recv_error || finished_session )
   {
      finished_session = true;

      if( had_send_or_recv_error )
         LOG_TRACE( "session terminated" );

      ostringstream osstr;

      if( had_send_or_recv_error || !mod_info.allows_anonymous_access )
      {
         if( g_ciyam_interface_html.find( c_form_content_comment ) == string::npos )
         {
            if( !had_send_or_recv_error )
               osstr << "<p align=\"center\"><strong>"
                << GDS( c_display_you_have_been_logged_out ) << "</strong></p>\n";
            else if( !is_sign_in )
               osstr << "<p class=\"error\" align=\"center\">" << GDS( c_display_error )
                << ": " << GDS( c_display_your_session_has_been_terminated ) << "</p>\n";
         }
         else
         {
            if( !had_send_or_recv_error )
               osstr << "<p align=\"center\"><strong>Session "
                << session_id << " has been terminated.</strong></p>\n";
            else
               osstr
                << "<p class=\"error\" align=\"center\">"
                << "Error: Session " << session_id << " has been terminated by the server.</p>\n";
         }
      }

      if( cookies_permitted )
         extra_content_func += "document.cookie = '" + get_cookie_value( session_id,
          p_session_info->user_id, cmd == c_cmd_quit, p_session_info->dtm_offset ) + "';";

      remove_session_temp_directory( session_id );

      if( had_send_or_recv_error || !mod_info.allows_anonymous_access )
      {
         string login_html( !cookies_permitted || !get_storage_info( ).login_days
          || g_login_persistent_html.empty( ) ? g_login_html : g_login_persistent_html );

         output_form( module_name, extra_content, login_html, osstr.str( ) );
      }
   }

   if( finished_session )
   {
      remove_non_persistent( session_id );

      LOG_TRACE( "[logout: " + ( p_session_info->user_name.empty( )
       ? p_session_info->user_id : p_session_info->user_name ) + " from " + p_session_info->ip_addr + "]" );

      // FUTURE: If this module allows anonymous access and a logout has just occurred
      // then currently a page refresh is being forced to occur in order to reload the
      // "home" page anonymously. It would be cleaner if this refresh could be avoided
      // but would require careful review of the session handling to work out how.
      if( mod_info.allows_anonymous_access )
      {
         extra_content.clear( );
         extra_content_func += "refresh( false );\n";
      }
   }
   else
   {
      string pwd_display_name;

      if( cookies_permitted && get_storage_info( ).login_days && !g_login_persistent_html.empty( ) )
         pwd_display_name = g_display_login_info;
      else
         pwd_display_name = g_display_change_password;

      if( ( cmd != c_cmd_pview ) && ( cmd != c_cmd_plist ) )
         extra_content << "\n<div id=\"normal_content\">\n";
      else
         extra_content << "\n<div id=\"print_content\">\n";

      if( ( cmd != c_cmd_pview ) && ( cmd != c_cmd_plist ) && !module_access_denied )
      {
         extra_content << "\n<div id=\"header\"><div id=\"appname\">";

         extra_content << "<a href=\"" << get_module_page_name( module_ref ) << "?cmd=" << c_cmd_home;

         if( !uselect.empty( ) )
         {
            extra_content << "&" << c_param_uselect << "=" << uselect;
            extra_content << "&" << c_param_uselextra << "=" << uselect;
         }

         if( !cookies_permitted )
            extra_content << "&session=" << session_id;

         if( use_url_checksum )
            extra_content << "&chksum=" << get_checksum( *p_session_info, session_id + uselect );
         extra_content << "\">" << get_app_name( ) << "</a></div>\n";

         extra_content << "   <div id=\"navband\">\n";
         extra_content << "      <div id=\"nav\">\n";
               
         if( using_anonymous )
         {
            if( ( cmd != c_cmd_join ) && ( cmd != c_cmd_open ) && !mod_info.user_class_id.empty( ) )
            {
               if( !get_storage_info( ).blockchain.empty( ) )
               {
                  extra_content << "<div id=\"sign\"><ul><li><a href=\"http://" << input_data[ c_http_param_host ]
                   << "/" << app_dir_name << "/" << get_module_page_name( module_ref ) << "?cmd=" << c_cmd_credentials;
                  extra_content << "\">" << GDS( c_display_sign_in ) << "</a></li>";

                  extra_content << "</ul></div>";
               }
               else
               {
                  // FUTURE: Support for HTTPS should be an option and if not being used then Sign In/Up
                  // should not be menus but just direct links to the "client crypto" implementations.
                  extra_content << "<div id=\"sign\"><ul><li><a href=\"#\">" << GDS( c_display_sign_in ) << "</a>";

                  extra_content << "<div id=\"sign_in_up\"><ul>";

                  if( file_exists( "../openid/" + app_dir_name, false ) )
                     extra_content << "<li><a href=\"https://" << input_data[ c_http_param_host ]
                      << "/openid/" << app_dir_name << "\">" << GDS( c_display_openid ) << "</a></li>";

                  extra_content << "<li><a href=\"https://" << input_data[ c_http_param_host ]
                   << "/" << app_dir_name << "/" << get_module_page_name( module_ref ) << "?cmd=" << c_cmd_credentials;
                  extra_content << "\">" << GDS( c_display_standard ) << "</a></li>";

                  extra_content << "<li><a href=\"http://" << input_data[ c_http_param_host ]
                   << "/" << app_dir_name << "/" << get_module_page_name( module_ref ) << "?cmd=" << c_cmd_credentials;
                  extra_content << "\">" << GDS( c_display_client_crypto ) << "</a></li>";

                  extra_content << "</ul></div></li>";

                  // NOTE: To limit "sign ups" to specific IP addresses simply add them
                  // as lines to the list of "sign up testers" file (to let *all* users
                  // have access simply remove the file).
                  set< string > testers;

                  if( file_exists( c_sign_up_testers_file ) )
                  {
                     testers.insert( "10.0.0.1" );

                     testers.insert( c_local_ip_addr );
                     testers.insert( c_local_ip_addr_for_ipv6 );

                     buffer_file_lines( c_sign_up_testers_file, testers );
                  }

                  if( testers.empty( ) || testers.count( p_session_info->ip_addr ) )
                  {
                     extra_content << "<li><a class=\"grey\" href=\"#\">" << GDS( c_display_sign_up ) << "</a>";

                     extra_content << "<div id=\"sign_in_up\"><ul>";

                     if( file_exists( "../openid/" + app_dir_name, false ) )
                        extra_content << "<li><a class=\"grey\" href=\"https://" << input_data[ c_http_param_host ]
                         << "/openid/" << app_dir_name << "\">" << GDS( c_display_openid ) << "</a></li>";

                     extra_content << "<li><a class=\"grey\" href=\"https://" << input_data[ c_http_param_host ]
                      << "/" << app_dir_name << "/" <<  get_module_page_name( module_ref ) << "?cmd=" << c_cmd_join;
                     extra_content << "\">" << GDS( c_display_standard ) << "</a></li>";

                     extra_content << "<li><a class=\"grey\" href=\"http://" << input_data[ c_http_param_host ]
                      << "/" << app_dir_name << "/" <<  get_module_page_name( module_ref ) << "?cmd=" << c_cmd_join;
                     extra_content << "\">" << GDS( c_display_client_crypto ) << "</a></li>";

                     extra_content << "</div></li></ul>";
                  }

                  extra_content << "</ul></div>\n";
               }
            }
         }
         else
         {
            extra_content << "         <div id=\"username\">\n";

            if( !p_session_info->needs_pin && !get_storage_info( ).user_info_view_id.empty( ) )
            {
               extra_content << GDS( c_display_logged_in_as ) << "&nbsp;&nbsp;";

               string user_info_view_id( get_storage_info( ).user_info_view_id );
               string user_info_module_ref( get_storage_info( ).user_info_module_ref );

               if( !mod_info.user_info_view_id.empty( ) )
               {
                  user_info_view_id = mod_info.user_info_view_id;
                  user_info_module_ref = module_ref;
               }

               bool has_user_link = false;

               if( ( cmd != c_cmd_view ) || ( ident != user_info_view_id ) || ( data != p_session_info->user_key ) )
               {
                  module_const_iterator mci, end;

                  for( mci = get_storage_info( ).modules.begin( ), end = get_storage_info( ).modules.end( ); mci != end; ++mci )
                  {
                     string next_module( mci->name );

                     if( !mci->perm.empty( ) && !p_session_info->user_perms.count( mci->perm ) )
                        continue;

                     string module_ref( get_storage_info( ).get_module_ref( next_module ) );

                     if( ( module_ref == user_info_module_ref )
                      && ( ( module_name == next_module ) || allow_module_switching ) )
                     {
                        has_user_link = true;

                        extra_content << "<a href=\"" << get_module_page_name( user_info_module_ref )
                         << "?cmd=" << c_cmd_view << "&data=" << p_session_info->user_key << "&ident=" << user_info_view_id;

                        if( !uselect.empty( ) )
                           extra_content << "&" << c_param_uselect << "=" << uselect;

                        if( !cookies_permitted )
                           extra_content << "&session=" << session_id;

                        if( use_url_checksum )
                        {
                           string checksum_values( to_string( c_cmd_view )
                            + p_session_info->user_key + user_info_view_id + uselect );

                           extra_content << "&" << c_param_chksum << "=" << get_checksum( *p_session_info, checksum_values );
                        }

                        extra_content << "\">";

                        break;
                     }
                  }
               }

               if( p_session_info->user_name.empty( ) )
                  extra_content << p_session_info->user_id;
               else
                  extra_content << p_session_info->user_name;

               if( !p_session_info->is_admin_user && !p_session_info->user_handle.empty( ) )
               {
                  if( p_session_info->user_handle != p_session_info->user_name )
                     extra_content << " (" << p_session_info->user_handle << ")";
               }

               if( has_user_link )
                  extra_content << "</a>";
            }
            else
               extra_content << GDS( c_display_logged_in_as ) << " "
                << ( p_session_info->user_name.empty( ) ? p_session_info->user_id : p_session_info->user_name );

            if( !p_session_info->needs_pin
             && !mod_info.user_pwd_field_id.empty( ) && !p_session_info->is_openid )
            {
               if( ( cmd == c_cmd_pwd ) && !input_data.count( c_param_newpwd ) )
                  extra_content << " | " << pwd_display_name << "";
               else
               {
                  extra_content << " | <a href=\"" << get_module_page_name( module_ref ) << "?cmd=" << c_cmd_pwd;

                  if( !uselect.empty( ) )
                     extra_content << "&" << c_param_uselect << "=" << uselect;

                  if( !cookies_permitted )
                     extra_content << "&session=" << session_id;

                  if( use_url_checksum )
                  {
                     string checksum_values( c_cmd_pwd + uselect );
                     extra_content << "&" << c_param_chksum << "=" << get_checksum( *p_session_info, checksum_values );
                  }

                  extra_content << "\">" << pwd_display_name << "</a>";
               }
            }

            if( !p_session_info->is_persistent || !mod_info.allows_anonymous_access )
            {
               extra_content << " | <a href=\"" << get_module_page_name( module_ref ) << "?cmd=" << c_cmd_quit;

               if( !uselect.empty( ) )
                  extra_content << "&" << c_param_uselect << "=" << uselect;

               extra_content << "&extra=" << session_id;

               if( !cookies_permitted )
                  extra_content << "&session=" << session_id;

               if( use_url_checksum )
               {
                  string checksum_values( c_cmd_quit + uselect );
                  extra_content << "&" << c_param_chksum << "=" << get_checksum( *p_session_info, checksum_values );
               }

               extra_content << "\">" << GDS( c_display_logout ) << "</a>";
            }

            if( file_exists( "help.htm" ) )
               extra_content << " | <a href=\"help.htm\" target=\"_blank\">" << GDS( c_display_help ) << "</a>";
            else if( file_exists( "help.html" ) )
               extra_content << " | <a href=\"help.html\" target=\"_blank\">" << GDS( c_display_help ) << "</a>";

            extra_content << "\n         </div>\n";
         }
              
         extra_content << "         <div id=\"uselects\">\n";

         // NOTE: If a user select option is specified and the user has access to it then include it.
         if( !p_session_info->needs_pin
          && !mod_info.user_select_str_key.empty( )
          && p_session_info->select_data.size( )
          && ( mod_info.user_select_perm.empty( )
          || p_session_info->user_perms.count( mod_info.user_select_perm ) ) )
         {
            extra_content << mod_info.get_string( mod_info.user_select_str_key ) << "&nbsp;\n";

            extra_content << "            <select ";

            // NOTE: Only permit user select changes from "home" and in lists.
            if( !cmd.empty( ) && cmd != c_cmd_home && cmd != c_cmd_list )
               extra_content << "disabled = \"disabled\" ";

            extra_content << "onchange=\"";

            if( use_url_checksum )
            {
               string checksum_values;
               string something_random = uuid( ).as_string( ).substr( 10 );

               if( cmd == c_cmd_home )
                  checksum_values = p_session_info->session_id + something_random;
               else
               {
                  if( cmd != c_cmd_view )
                     checksum_values = cmd;
                  else
                     checksum_values = string( c_cmd_view ) + data;

                  checksum_values += oident + something_random;

                  if( !hashval.empty( ) )
                     checksum_values += c_hash_suffix;
               }

               string new_checksum_value( get_checksum( *p_session_info, checksum_values ) );

               extra_content << "query_update( '" << c_param_uselextra << "', '" << something_random << "', true ); ";
               extra_content << "query_update( '" << c_param_chksum << "', '" << new_checksum_value << "', true ); ";
            }

            extra_content << "sel_qry_update( this, '" << c_param_uselect << "' );\">\n";

            if( !mod_info.user_select_is_strict )
               extra_content << "               <option value=\"~\">&lt;" << GDS( c_display_none ) << "&gt;&nbsp;&nbsp;</option>\n";

            for( size_t i = 0; i < p_session_info->select_data.size( ); i++ )
            {
               string key_and_ver_info( p_session_info->select_data[ i ].first );
               string::size_type pos = key_and_ver_info.find( ' ' );

               string key( key_and_ver_info.substr( 0, pos ) );

               extra_content << "               <option value=\""
                << key << "\"" << ( ( uselect == key ) ? " selected>" : ">" )
                << p_session_info->select_data[ i ].second << "</option>\n";
            }

            extra_content << "            </select>\n";
         }

         if( !using_anonymous && !p_session_info->needs_pin
          && allow_module_switching && ( cmd != c_cmd_join ) )
         {
            bool had_first = false;

            module_const_iterator mci, end;

            for( mci = get_storage_info( ).modules.begin( ), end = get_storage_info( ).modules.end( ); mci != end; ++mci )
            {
               string next_module( mci->name );

               bool can_link = true;

               if( !mci->perm.empty( ) && !p_session_info->user_perms.count( mci->perm ) )
               {
                  if( !menu_opts.count( c_menu_opt_show_inaccessible_modules ) )
                     continue;

                  can_link = false;
               }

               string module_ref( get_storage_info( ).get_module_ref( next_module ) );

               // NOTE: If no HTML page exists for the module then it is not applicable to the UI.
               if( get_module_page_name( module_ref ).empty( ) )
                  continue;

               if( !had_first )
               {
                  had_first = true;
                  extra_content << "            &nbsp;&nbsp;" << GDS( c_display_switch_to_module ) << "&nbsp;";
               }
               else
                  extra_content << " | ";

               string title( mci->get_string( "title" ) );

               string prefix( get_storage_info( ).module_prefix );

               // NOTE: Remove the module prefix (if present) and trailing version information.
               str_replace( prefix, "_", " " );

               if( prefix.length( ) && title.find( prefix ) == 0 )
                  title.erase( 0, prefix.length( ) );

               string::size_type pos = title.find_last_of( " " );

               if( pos != string::npos )
                  title.erase( pos );

               if( !can_link || ( module_name == next_module ) )
                  extra_content << title;
               else
               {
                  extra_content << "<a href=\"" << get_module_page_name( module_ref ) << "?cmd=" << c_cmd_home;

                  if( !uselect.empty( ) )
                  {
                     extra_content << "&" << c_param_uselect << "=" << uselect;
                     extra_content << "&" << c_param_uselextra << "=" << uselect;
                  }

                  if( !cookies_permitted )
                     extra_content << "&session=" << session_id;

                  if( use_url_checksum )
                     extra_content << "&chksum=" << get_checksum( *p_session_info, session_id + uselect );

                  extra_content << "\">" << title << "</a>";
               }
            }
         }

         extra_content << "\n         </div>\n";

         if( !using_anonymous )
         {
            string time_info;

            // NOTE: If a user has a selected timezone then "current_dtm" will be the server's
            // UTC time converted to that otherwise just starts with the current UTC value and
            // adds the UTC offset. In either case the UTC skew (i.e. "dtm_offset") value will
            // be taken into account (so what is displayed makes the most sense to the user).
            if( !p_session_info->current_dtm.empty( ) )
            {
               date_time dt( p_session_info->current_dtm );

               dt += ( seconds )p_session_info->dtm_offset;

               time_info = dt.as_string( e_time_format_hhmm, true );

               if( !p_session_info->tz_abbr.empty( ) )
                  time_info += ' ' + p_session_info->tz_abbr;
            }
            else
            {
               date_time dt( date_time::standard( ) + ( seconds )p_session_info->dtm_offset );

               dt += ( seconds )p_session_info->gmt_offset;

               time_info = dt.as_string( e_time_format_hhmm, true );
            }

            extra_content << "         <div id=\"timeinfo\">" << time_info << "\n         </div>";
         }

         // NOTE: Display an indicator if is using client-side crypto.
         if( p_session_info
          && p_session_info->logged_in
          && get_storage_info( ).encrypt_data )
            extra_content << "         <div id=\"lockinfo\"></div>\n";

         extra_content << "      </div >\n";
         extra_content << "   </div>\n";

         if( cmd == c_cmd_join || cmd == c_cmd_open )
            extra_content << "<h3 class=\"right-top\">" << g_display_sign_up_for_an_account << "</h3>";

         extra_content << "</div>\n";
      }

      map< string, vector< string > > menu_items;

      if( !p_session_info->needs_pin && ( cmd != c_cmd_join ) && ( cmd != c_cmd_open )
       && ( cmd != c_cmd_pview ) && ( cmd != c_cmd_plist ) && !module_access_denied )
      {
         extra_content << "\n<div id=\"menuband\" class=\"clearfix\">\n";
         extra_content << "   <ul id=\"menubar\">\n";

         // NOTE: Don't display the home menu if logged in (to leave more room for other menus).
         if( !p_session_info->logged_in )
         {
            extra_content << "     <li class=\"\">";

            extra_content << "<a href=\""
             << get_module_page_name( module_ref ) << "?cmd=" << c_cmd_home;

            if( !uselect.empty( ) )
            {
               extra_content << "&" << c_param_uselect << "=" << uselect;
               extra_content << "&" << c_param_uselextra << "=" << uselect;
            }

            if( !cookies_permitted )
               extra_content << "&session=" << session_id;

            if( use_url_checksum )
               extra_content << "&chksum=" << get_checksum( *p_session_info, session_id + uselect );

            extra_content << "\">" << GDS( c_display_home ) << "</a></li>\n";
         }

         string user_group;

         bool is_admin_user = false;

         if( using_anonymous )
         {
            is_admin_user = false;

            if( p_session_info->is_meta_module )
               user_group = "guests";
            else
               user_group = c_everyone;
         }
         else
         {
            user_group = p_session_info->user_group;
            is_admin_user = p_session_info->is_admin_user;
         }

         bool has_selected_list = false;

         for( list_menu_const_iterator
          lmci = mod_info.list_menus.begin( ), end = mod_info.list_menus.end( ); lmci != end; ++lmci )
         {
            string list_perm( lmci->second->perm );

            if( !list_perm.empty( ) && ( list_perm[ 0 ] == '!' ) )
               list_perm.erase( 0, 1 );

            bool has_perm = has_permission( list_perm, *p_session_info );

            // FUTURE: Currently logic that requires a permission for an "admin" list is not supported
            // (i.e. either the user is "admin" OR has the "perm"). If this is to be changed then this
            // code would need to be changed.
            if( !has_perm && p_session_info->is_admin_user && ( lmci->second->type == c_list_type_admin ) )
               has_perm = true;

            if( has_perm && lmci->second->extras.count( c_list_type_extra_show_if_default_other ) )
            {
               // NOTE: If both of the "if default other" options are present then do not
               // display for an "admin" user.
               if( p_session_info->is_admin_user
                && lmci->second->extras.count( c_list_type_extra_new_if_default_other ) )
                  has_perm = false;
               else
                  has_perm = p_session_info->is_default_other( );
            }

            // NOTE: For the "admin" user only display lists specifically targeted for "admin"
            // (to reduce unnecessary clutter as "admin" is not intended for "standard" usage).
            if( p_session_info->is_admin_user
             && ( ( lmci->second->type != c_list_type_admin )
             && ( lmci->second->type != c_list_type_child_admin ) ) )
               has_perm = false;

            if( using_anonymous && !lmci->second->extras.count( c_list_type_extra_allow_anonymous ) )
               has_perm = false;

            // NOTE: If the list type has been identified as "no access" then it will not be displayed.
            if( lmci->second->type == c_list_type_no_access )
               has_perm = false;

            // NOTE: If the user does not have permission or if the list is a
            // "group" type but the user doesn't belong to a group then omit it.
            if( has_perm && lmci->second->type != c_list_type_home
             && ( !user_group.empty( ) || lmci->second->type != c_list_type_group )
             && ( is_admin_user || !lmci->second->perm.empty( )
             || ( ( lmci->second->type != c_list_type_admin )
             && ( lmci->second->type != c_list_type_child_admin ) ) ) )
            {
               string display_name( get_display_name( lmci->first ) );

               string::size_type pos = display_name.find( ' ' );
               string menu_name = display_name.substr( 0, pos );

               string item_name( menu_name );
               if( pos != string::npos )
                  item_name = display_name.substr( pos + 1 );

               item_name += '!' + lmci->second->id;
               menu_items[ menu_name ].push_back( item_name );
            }
         }

         for( map< string, vector< string > >::iterator i = menu_items.begin( ); i != menu_items.end( ); ++i )
         {
            // NOTE: If a menu has only one item (and that item has the same name as the menu) then rather
            // than create a menu just make the menu itself behave as the menu item link.
            string id, item_name;

            bool menu_is_item = false;

            if( i->second.size( ) == 1 )
            {
               string::size_type pos = i->second[ 0 ].find( '!' );

               item_name = i->second[ 0 ].substr( 0, pos );

               if( pos != string::npos )
                  id = i->second[ 0 ].substr( pos + 1 );

               if( item_name == i->first )
                  menu_is_item = true;
            }

            if( menu_is_item )
            {
               extra_content << "     <li class=\"\"><a href=\""
                << get_module_page_name( module_ref ) << "?cmd=" << c_cmd_list << "&ident=" << id;

               if( !uselect.empty( ) )
                  extra_content << "&" << c_param_uselect << "=" << uselect;

               if( !cookies_permitted )
                  extra_content << "&session=" << session_id;

               if( use_url_checksum )
               {
                  string checksum_values( string( c_cmd_list ) + id + uselect );
                  extra_content << "&" << c_param_chksum << "=" << get_checksum( *p_session_info, checksum_values );
               }

               extra_content << "\">" << item_name << "</a>\n";
            }
            else
            {
               extra_content << "     <li class=\"\"><a href=\"#\">" << i->first << "</a>\n";

               extra_content << "      <ul>\n";

               for( size_t j = 0; j < i->second.size( ); j++ )
               {
                  string::size_type pos = i->second[ j ].find( '!' );

                  string id, item_name( i->second[ j ].substr( 0, pos ) );

                  if( pos != string::npos )
                     id = i->second[ j ].substr( pos + 1 );

                  extra_content << "         <li class=\"\"><a href=\""
                   << get_module_page_name( module_ref ) << "?cmd=" << c_cmd_list << "&ident=" << id;

                  if( !uselect.empty( ) )
                     extra_content << "&" << c_param_uselect << "=" << uselect;

                  if( !cookies_permitted )
                     extra_content << "&session=" << session_id;

                  if( use_url_checksum )
                  {
                     string checksum_values( string( c_cmd_list ) + id + uselect );

                     extra_content << "&" << c_param_chksum << "=" << get_checksum( *p_session_info, checksum_values );
                  }

                  extra_content << "\">" << item_name << "</a></li>\n";
               }

               extra_content << "      </ul>\n";
            }

            extra_content << "     </li>\n";
         }

         // NOTE: If the selected list has become inaccessible (to due a uselect change)
         // then force a jump back to "home".
         if( !has_selected_list && qlink.empty( ) && ( cmd == c_cmd_list ) && !uselect.empty( ) )
            cmd.erase( );

         if( p_session_info->quick_link_data.size( ) )
         {
            extra_content << "     <li class=\"\"><a href=\"#\">"
             << get_display_name( mod_info.get_string( mod_info.user_qlink_list_id + "_name" ) ) << "</a>\n";

            extra_content << "      <ul>\n";
         }

         for( size_t i = 0; i < p_session_info->quick_link_data.size( ); i++ )
         {
            string columns( p_session_info->quick_link_data[ i ].second );

            vector< string > column_info;

            raw_split( columns, column_info );

            if( column_info.size( ) != 3 )
               throw runtime_error( "unexpected incorrect # columns for quick link info" );

            // FUTURE: To make quick link menu items look different italics are being hard-coded here.
            // This should instead be implemented as a style (only applicable if using the "sidebar").
            if( !qlink.empty( ) && i == atoi( qlink.c_str( ) ) )
               extra_content << "     <li class=\"\"><a href=\"#\">" << column_info[ 1 ] << "</a></li>\n";
            else
            {
               extra_content << "     <li class=\"\">";

               extra_content << "<a href=\"" << column_info[ 0 ];
               extra_content << "&" << c_param_qlink << "=" << to_string( i );

               if( use_url_checksum )
               {
                  if( column_info[ 0 ].find( "?cmd=list" ) == string::npos )
                  {
                     string checksum_values( string( c_cmd_view ) + column_info[ 2 ] + uselect );

                     extra_content << "&" << c_param_chksum << "=" << get_checksum( *p_session_info, checksum_values );
                  }
                  else
                  {
                     string::size_type pos = column_info[ 2 ].find( '+' );

                     if( pos == string::npos )
                        throw runtime_error( "unexpected quick link data format" );

                     string checksum_values( string( c_cmd_list ) + column_info[ 2 ].substr( 0, pos ) + uselect );

                     extra_content << "&" << c_param_chksum << "="
                      << get_checksum( *p_session_info, checksum_values + c_hash_suffix );

                     extra_content << "&" << c_param_hashval
                      << "=" << get_hash( p_session_info->hashval_prefix + column_info[ 2 ].substr( pos + 1 ) );
                  }
               }

               extra_content << "\">" << column_info[ 1 ] << "</a></li>\n";
            }
         }

         if( p_session_info->quick_link_data.size( ) )
            extra_content << "      </ul>\n";

         extra_content << "   </ul>\n";
         extra_content << "</div>\n";
      }

      if( ( cmd != c_cmd_pview ) && ( cmd != c_cmd_plist ) )
      {
         bool found_slides = false;

         if( cmd == c_cmd_home && !p_session_info->logged_in )
         {
            extra_content << "    <div class=\"home clearfix\">\n";
            extra_content << "        <div class=\"home_container clearfix\">\n";

            string home_title( mod_info.get_string( "home_title" ) );

            if( home_title.empty( ) )
               home_title = title;

            extra_content << "            <h2>" << home_title << "</h2>\n";

            for( map< string, list_source >::iterator i = home_lists.begin( ); i != home_lists.end( ); ++i )
            {
               bool can_output = true;

               if( p_session_info->is_admin_user
                || i->second.row_data.empty( ) || ( using_anonymous
                && !i->second.lici->second->extras.count( c_list_type_extra_allow_anonymous ) ) )
                  can_output = false;

               if( can_output )
               {
                  string header;

                  if( !i->second.name.empty( ) )
                     header = get_view_or_list_header( qlink, i->second.name, mod_info, *p_session_info );

                  if( header.empty( ) )
                  {
                     if( !found_slides )
                     {
                        found_slides = true;

                        extra_content_func += "totalNumSlides = " + to_string( ( i->second ).row_data.size( ) ) + "; show_next_slide( );";

                        extra_content << "            <div class=\"home_slides_container clearfix\" id=\"home_slides_container\">\n";

                        extra_content << "                <a class=\"nav_left\" onclick=\"goto_prev_slide( );\"></a>\n";
                        extra_content << "                <a class=\"nav_right\" onclick=\"goto_next_slide( );\"></a>\n";
                        extra_content << "                <ul class=\"paginate\" id=\"home_slides_paginate\">\n";
                        extra_content << "                    <li class=\"active\" onclick=\"goto_slide( 0 );\"></li>\n";

                        for( size_t s = 1; s < ( i->second ).row_data.size( ); s++ )
                           extra_content << "                    <li class=\"inactive\" onclick=\"goto_slide( " << to_string( s ) << " );\"></li>\n";

                        extra_content << "                </ul>\n";
                     }

                     for( size_t s = 0; s < ( i->second ).row_data.size( ); s++ )
                     {
                        if( s == 0 )
                           extra_content << "               <div class=\"home_slide clearfix\">\n";
                        else
                           extra_content << "               <div class=\"home_slide clearfix invisible\">\n";

                        if( !p_session_info->sys_message.empty( ) )
                           extra_content << "               <p class=\"caption center\">" << p_session_info->sys_message << "</p>\n";

                        string key, data;

                        vector< string > columns;

                        key = ( i->second ).row_data[ s ].first;

                        size_t pos = key.find( ' ' );

                        if( pos != string::npos )
                           key.erase( pos );

                        raw_split( ( i->second ).row_data[ s ].second, columns );

                        if( !columns.empty( ) )
                           data = columns[ columns.size( ) - 1 ];

                        data = replace_display_strings( data );

                        replace_links_and_output( data, ( i->second ).view,
                         ( i->second ).module, ( i->second ).module_ref, extra_content, true, true,
                         session_id, *p_session_info, uselect, cookies_permitted, use_url_checksum, &key, true );

                        extra_content << "               </div>\n";
                     }
                  }
               }
            }

            // NOTE: If no slides actually exist then create a dummy title slide.
            if( !found_slides )
            {
               extra_content << "            <div class=\"home_slides_container clearfix\" id=\"home_slides_container\">\n";
               extra_content << "               <div class=\"home_slide clearfix\">\n";

               if( !p_session_info->sys_message.empty( ) )
                  extra_content << "               <p class=\"caption center\">" << p_session_info->sys_message << "</p>\n";

               extra_content << "               <p class=\"title center\"><b>"
                << GDS( c_display_welcome_to ) << ' ' << title << "</b></p>\n";

               extra_content << "               </div>\n";
            }

            extra_content << "            </div>\n";

            extra_content << "        </div>\n";
            extra_content << "    </div>\n";
         }
         else if( cmd == c_cmd_home )
         {
            if( !p_session_info->sys_message.empty( ) )
               extra_content << "               <p class=\"caption center\">" << p_session_info->sys_message << "</p>\n";
         }
      }

      if( ( cmd != c_cmd_pview ) && ( cmd != c_cmd_plist ) )
         extra_content << "<div id=\"remainder\">\n";
      else
         extra_content << "<div id=\"print_remainder\">\n";

      extra_content << "<div id=\"main\" class=\"menubar_width clearfix\">\n";

      if( module_access_denied )
      {
         extra_content << "<p class=\"text_with_back\">"
          << string_message( GDS( c_display_session_logged_into_other_module ),
          make_pair( c_display_session_logged_into_other_module_parm_href,
          "<a href=\"javascript:history.back( )\">" ), "</a>" ) << "</p>";
      }
      else if( ( cmd == c_cmd_pview ) || ( cmd == c_cmd_plist ) )
      {
         extra_content << "<p class=\"screen text_with_back\">"
          << string_message( GDS( c_display_click_the_printer_icon_to_print ),
          make_pair( c_display_click_the_printer_icon_to_print_parm_href,
          "<a href=\"javascript:history.back( )\">" ), "</a>" ) << "</p>";
      }

      if( !qlink.empty( ) )
      {
         size_t qlink_num = atoi( qlink.c_str( ) );
         if( qlink_num >= p_session_info->quick_link_data.size( ) )
            qlink.erase( );
      }

      if( cmd == c_cmd_pwd )
      {
         if( !input_data.count( c_param_newpwd ) )
         {
            string password_html( !cookies_permitted || !get_storage_info( ).login_days
             || g_password_persistent_html.empty( ) ? g_password_html : g_password_persistent_html );

            string::size_type pos = password_html.find( c_checked );

            if( pos != string::npos )
               str_replace( password_html, c_checked, p_session_info->is_persistent ? "checked" : "" );

            str_replace( password_html, c_username, p_session_info->user_id );

            extra_content << password_html;
         }
         else
         {
            if( error_message.empty( ) )
               extra_content << "<p align=\"center\"><b>" << GDS( c_display_password_has_been_changed ) << "</b></p>\n";
            else
               extra_content << "<p align=\"center\" class=\"error\">" << error_message << "</p>\n";
         }
      }
      else if( cmd == c_cmd_join )
      {
         guard g( g_join_mutex );

         bool has_completed = false;
         bool had_unexpected_error = false;

         string email, account_type, req_username, oreq_username, gpg_public_key;

         email = input_data[ c_param_email ];

         req_username = oreq_username = input_data[ c_param_requsername ];

         if( !email.empty( ) || input_data.count( c_param_gpgpubkey ) )
         {
            account_type = input_data[ c_param_accttype ];
            gpg_public_key = input_data[ c_param_gpgpubkey ];

            bool is_help_request = false;

            if( !gpg_public_key.empty( ) )
            {
               string::size_type pos = req_username.find( "help " );

               if( pos == 0 )
               {
                  is_help_request = true;
                  req_username.erase( 0, 5 );
               }
            }

            if( req_username != "anon" )
            {
               string field_list( mod_info.user_uid_field_id );
               field_list += "," + mod_info.user_pwd_field_id;

               string key_info( mod_info.user_uid_field_id );
               key_info += " " + req_username;

               pair< string, string > user_info;

               if( !fetch_item_info( module_id, mod_info,
                mod_info.user_class_id, key_info, field_list, "", "", *p_session_info, user_info ) )
                  throw runtime_error( "unexpected error occurred checking for user" );

               vector< string > user_data;

               split( user_info.second, user_data );

               if( user_data.size( ) < 2 )
                  throw runtime_error( "unexpected missing user information" );

               if( req_username == user_data[ 0 ] )
                  error_message = "<p class=\"error\" align=\"center\">"
                   + string_message( GDS( c_display_user_id_has_already_been_taken ),
                   make_pair( c_display_user_id_has_already_been_taken_parm_id, req_username ) ) + "</p>";

               if( is_ssl )
                  msleep( c_security_delay_time );
            }
            else
            {
               msleep( c_security_delay_time );

               req_username += uuid( ).as_string( ).substr( 0, 10 );
            }

            string clone_key;

            map< string, string > sign_up_types_map;

            if( file_exists( c_sign_up_types_map ) )
            {
               buffer_file_items( c_sign_up_types_map, sign_up_types_map );

               clone_key = sign_up_types_map[ account_type ];
            }

            if( error_message.empty( ) && !gpg_public_key.empty( ) )
            {
               string key, email_addr;

               string file_name( "~" + uuid( ).as_string( ) );

               string gpg_file_name( file_name + ".gpg" );
               string tmp_file_name( file_name + ".tmp" );

               regex expr( "-----BEGIN PGP PUBLIC KEY BLOCK-----.*-----END PGP PUBLIC KEY BLOCK-----" );

               string::size_type len = 0;
               string::size_type pos = expr.search( gpg_public_key, &len );

               if( pos == string::npos )
                  error_message = "<p class=\"error\" align=\"center\">" + GDS( c_display_gpg_public_key_is_not_correctly_formatted ) + "</p>";
               else
               {
                  string pubkey( gpg_public_key.substr( pos, len ) );

                  write_file( gpg_file_name, pubkey );

                  string cmd( "gpg --batch --import " );

                  cmd += gpg_file_name + " >" + tmp_file_name + " 2>&1";

                  int rc = system( cmd.c_str( ) );
                  ( void )rc;

                  vector< string > lines;

                  buffer_file_lines( tmp_file_name, lines );

                  if( lines.size( ) < 3 )
                     had_unexpected_error = true;
                  else if( lines[ 0 ].find( "CRC error" ) != string::npos )
                  {
                     error_message = "<p class=\"error\" align=\"center\">"
                      + GDS( c_display_gpg_public_key_appears_to_be_corrupt )
                      + "<br/>" + GDS( c_display_re_copy_and_paste_your_gpg_public_key ) + "</p>";
                  }
                  else
                  {
                     string result;

                     string first_line( lines[ 0 ] );

                     regex expr( "\".*\"" );

                     string::size_type len = 0;
                     string::size_type pos = expr.search( first_line, &len );

                     if( pos == string::npos )
                        had_unexpected_error = true;
                     else
                     {
                        result = first_line.substr( pos + len + 1 );

                        if( ( result == "not changed" ) && !is_help_request )
                        {
                           error_message = "<p class=\"error\" align=\"center\">"
                            + GDS( c_display_gpg_encrypted_email_should_have_already_been_sent )
                            + "<br/>" + GDS( c_display_please_re_check_your_email ) + "</p>";

                           error_message += "<p>" + string_message(
                            GDS( c_display_to_instead_see_your_gpg_encrypted_information_here ), make_pair(
                            c_display_to_instead_see_your_gpg_encrypted_information_here_parm_id, oreq_username ) ) + "</p>";
                        }
                     }

                     if( ( result == "imported" ) || ( ( result == "not changed" ) && is_help_request ) )
                     {
                        pos = first_line.find( " key " );

                        if( pos == string::npos )
                           had_unexpected_error = true;
                        else
                        {
                           string::size_type epos = first_line.find( ':' );

                           if( epos == string::npos )
                              had_unexpected_error = true;
                           else
                           {
                              key = first_line.substr( pos + 5, 8 );

                              pos = first_line.find_last_of( '<' );

                              if( pos != string::npos )
                              {
                                 regex expr( "<" + string( c_regex_email_address ) + ">" );

                                 if( expr.search( first_line.substr( pos ), &len ) != 0 )
                                    pos = string::npos;
                              }

                              if( pos == string::npos )
                              {
                                 cmd = "gpg --batch --delete-key --yes " + key + ">" + tmp_file_name + " 2>&1";

                                 if( system( cmd.c_str( ) ) != 0 )
                                    LOG_TRACE( buffer_file( tmp_file_name ) );

                                 error_message = "<p class=\"error\" align=\"center\">" + GDS( c_display_email_not_found_in_gpg_public_key ) + "</p>";
                              }
                              else
                                 email_addr = first_line.substr( pos + 1, len - 2 );
                           }
                        }
                     }
                     else if( error_message.empty( ) )
                        had_unexpected_error = true;
                  }

                  if( !error_message.empty( ) || had_unexpected_error )
                     file_remove( gpg_file_name );
                        
                  file_remove( tmp_file_name );
               }

               if( error_message.empty( ) && !had_unexpected_error )
               {
                  string password( uuid( ).as_string( ).substr( 2 ) );

                  string gpg_message( buffer_file( "join.txt" ) );

                  str_replace( gpg_message, c_app_name, get_app_name( ) );
                  str_replace( gpg_message, c_password, password );
                  str_replace( gpg_message, c_username, req_username );

                  write_file( key, gpg_message );

                  string gpg_password;

                  if( !simple_command( *p_session_info, "password gpg", &gpg_password ) )
                     throw runtime_error( "unable to access GPG password" );

                  string sid;
                  get_server_sid( sid );

                  gpg_password = data_decrypt( gpg_password, sid );

                  string smtp_sender;

                  if( !simple_command( *p_session_info, "smtpinfo", &smtp_sender ) )
                     throw runtime_error( "unable to access SMTP info" );

                  cmd = "gpg --armor --recipient " + key
                   + " --encrypt --sign --trust-model always --batch --local-user "
                   + smtp_sender + " --passphrase " + gpg_password + " " + key + ">" + key + ".tmp 2>&1";

                  int rc = system( cmd.c_str( ) );
                  ( void )rc;

                  if( !file_exists( key + ".asc" ) )
                  {
                     gpg_message = buffer_file( key + ".tmp" );

                     LOG_TRACE( "error: " + gpg_message );

                     has_completed = false;
                     had_unexpected_error = true;
                  }
                  else
                  {
                     password = hash_password( g_id + password + req_username );

                     password = data_encrypt( password, sid );

                     bool is_anon_email_addr = false;

                     string::size_type pos = email_addr.find( "@" );

                     if( pos != string::npos )
                     {
                        if( email_addr.substr( 0, pos ) == c_anon_user_key )
                           is_anon_email_addr = true;
                     }

                     if( !is_help_request )
                     {
                        string email, gpg_key_file;

                        if( !is_anon_email_addr )
                        {
                           email = data_encrypt( email_addr, sid );
                           gpg_key_file = get_storage_info( ).web_root + '/' + gpg_file_name;
                        }

                        // NOTE: The GPG key is installed when the User is created as it is being
                        // assumed that the FCGI interface's GPG key ring is not the same as that
                        // of the application server.
                        add_user( req_username, req_username, email, clone_key,
                         password, error_message, mod_info, *p_session_info, 0, true, &gpg_key_file );

                        if( !error_message.empty( ) )
                        {
                           if( error_message.find( c_response_error_prefix ) == 0 )
                              error_message.erase( 0, strlen( c_response_error_prefix ) );

                           error_message = "<p class=\"error\" align=\"center\">" + remove_key( error_message ) + "</p>";

                           if( !key.empty( ) && file_exists( key + ".asc" ) )
                           {
                              string cmd( "gpg --batch --delete-key --yes " + key + ">" + tmp_file_name + " 2>&1" );

                              if( system( cmd.c_str( ) ) != 0 )
                                 LOG_TRACE( buffer_file( tmp_file_name ) );
                           }
                        }
                        else
                        {
                           LOG_TRACE( "[add_user: " + req_username + " from " + raddr + "]" );
                        }
                     }

                     clear_key( sid );

                     file_remove( gpg_file_name );
                     file_remove( tmp_file_name );

                     if( error_message.empty( ) )
                     {
                        if( is_help_request || is_anon_email_addr )
                        {
                           has_completed = true;

                           gpg_message = buffer_file( key + ".asc" );

                           if( !is_help_request )
                              extra_content << "<p><b>"
                               << GDS( c_display_welcome_aboard ) << " " << oreq_username << " !<br/><br/>";

                           extra_content << GDS( c_display_your_gpg_encrypted_account_credentials )
                            << "</b><p>\n" << "<p align=\"center\"><pre>" << gpg_message << "</pre></p>";
                        }
                        else
                        {
                           string msg_file( "message?" + get_cwd( ) + "/" + key + ".asc" );

                           string smtp_result;

                           simple_command( *p_session_info, "sendmail "
                            + email_addr + " \"" + GDS( c_display_welcome_aboard ) + "!\" \""
                            + GDS( c_display_see_attachment_for_details ) + "\" -attach=" + msg_file, &smtp_result );

                           if( !smtp_result.empty( ) && smtp_result != c_response_okay )
                              error_message = "<p class=\"error\" align=\"center\">" + smtp_result + "</p>";
                        }
                     }

                     file_remove( key + ".asc" );
                  }

                  file_remove( key );
                  file_remove( key + ".tmp" );

                  if( !has_completed && !had_unexpected_error && error_message.empty( ) )
                  {
                     has_completed = true;

                     extra_content << "<p align=\"center\"><b>" << GDS( c_display_welcome_aboard ) << " "
                      << oreq_username << " !<br/><br/>" << GDS( c_display_a_gpg_encrypted_email_with_your_credentials ) << "</b></p>\n";
                  }
               }
            }
            else if( error_message.empty( ) && !email.empty( ) )
            {
               string sid;
               get_server_sid( sid );

               string encrypted_email( data_encrypt( email, sid ) );

               clear_key( sid );

               string new_key;

               add_user( req_username, req_username, encrypted_email,
                clone_key, req_username, error_message, mod_info, *p_session_info, &new_key, false );

               // FUTURE: It would be better if the email was sent from the User class rather
               // than being done separately here (that way a User record won't be created in
               // the case where sending the activation email fails).
               if( error_message.empty( ) )
               {
                  LOG_TRACE( "[add_user: " + req_username + " from " + raddr + "]" );

                  string smtp_result;
                  ostringstream osstr;

                  string checksum( get_checksum( "activate" + req_username + new_key ) );

                  activation_file = new_key + checksum;

                  osstr << string_message( GDS( c_display_activation_email_content ),
                   make_pair( c_display_activation_email_content_perm_user_id, req_username ) )
                   << "\n\n" << ( is_ssl ? "https" : "http" ) << "://" << input_data[ c_http_param_host ]
                   << "/" << lower( get_storage_info( ).storage_name ) << "/" << get_module_page_name( module_ref, true )
                   << "?cmd=activate&data=" << new_key << "&user=" << req_username << "&chksum=" << checksum;

                  simple_command( *p_session_info, "sendmail "
                   + email + " \"" + GDS( c_display_welcome_aboard ) + "!\" \""
                   + osstr.str( ) + "\"", &smtp_result );

                  if( !smtp_result.empty( ) && smtp_result != c_response_okay )
                     error_message = "<p class=\"error\" align=\"center\">" + smtp_result + "</p>";
               }

               if( !error_message.empty( ) )
               {
                  if( error_message.find( c_response_error_prefix ) == 0 )
                     error_message.erase( 0, strlen( c_response_error_prefix ) );

                  error_message = "<p class=\"error\" align=\"center\">" + error_message + "</p>";
               }
               else
               {
                  has_completed = true;
                  ofstream outf( activation_file.c_str( ) );

                  extra_content << "<p><b>"
                   << GDS( c_display_welcome_aboard ) << " " << oreq_username << " !<br/><br/>";

                  extra_content << "<p>" << string_message( GDS( c_display_activation_email_details ),
                   make_pair( c_display_activation_email_details_parm_email, email ) ) << "</p>";
               }
            }

            if( had_unexpected_error )
               error_message = "<p class=\"error\" align=\"center\">" + GDS( c_display_sorry_unexpected_error_processing_gpg ) + "</p>";
         }

         if( !has_completed )
         {
            string signup_html( !is_ssl ? g_signup_html : g_ssl_signup_html );

            str_replace( signup_html, c_username, oreq_username );
            str_replace( signup_html, c_error_message, error_message );

            if( is_ssl )
            {
               str_replace( signup_html, c_email, email );
               str_replace( signup_html, c_password, password );
               str_replace( signup_html, c_verify_password, password );
            }
            else
               str_replace( signup_html, c_gpg_public_key, gpg_public_key );

            str_replace( signup_html, string( c_selected_ + account_type ).c_str( ), "selected" );

            error_message.erase( );

            extra_content << signup_html;
         }
      }
      else if( cmd == c_cmd_open )
      {
         bool has_completed = false;

         string account_type( input_data[ c_param_accttype ] );
         string req_username( input_data[ c_param_requsername ] );

         if( !req_username.empty( ) )
         {
            string clone_key;

            map< string, string > sign_up_types_map;

            if( file_exists( c_sign_up_types_map ) )
            {
               buffer_file_items( c_sign_up_types_map, sign_up_types_map );
               clone_key = sign_up_types_map[ account_type ];
            }

            string email( input_data[ c_openid_ext_email ] );

            string encrypted_email;

            if( !email.empty( ) )
            {
               string sid;
               get_server_sid( sid );

               encrypted_email = data_encrypt( email, sid );

               clear_key( sid );
            }

            add_user( userhash, req_username, encrypted_email,
             clone_key, "", error_message, mod_info, *p_session_info );

            if( !error_message.empty( ) )
            {
               if( error_message.find( c_response_error_prefix ) == 0 )
                  error_message.erase( 0, strlen( c_response_error_prefix ) );

               error_message = "<p class=\"error\" align=\"center\">" + error_message + "</p>";
            }
            else
            {
               has_completed = true;

               LOG_TRACE( "[add_user: " + req_username + " from " + raddr + "]" );

               extra_content << "<p align=\"center\"><b>"
                << GDS( c_display_welcome_aboard ) << " " << req_username << " !</p>\n";

               extra_content_func += "auto_refresh_seconds = "
                + to_string( c_auto_refresh_seconds_hello ) + ";\nauto_refresh( );";
            }
         }

         if( !has_completed )
         {
            string openup_html( g_openup_html );

            str_replace( openup_html, c_error_message, error_message );

            if( req_username.empty( ) )
            {
               req_username = input_data[ c_openid_username ];

               if( req_username.empty( ) )
               {
                  req_username = input_data[ c_openid_ext_email ];
                  string::size_type pos = req_username.find( '@' );
                  if( pos != string::npos )
                     req_username.erase( pos );
               }
            }

            str_replace( openup_html, c_user_name, req_username );

            extra_content << openup_html;
         }
      }
      else if( cmd == c_cmd_home )
      {
         if( p_session_info->needs_pin )
         {
            string authenticate_html( g_authenticate_html );

            if( !pin.empty( ) )
            {
               msleep( c_security_delay_time );

               error_message = "<p class=\"error\" align=\"center\">" + GDS( c_display_incorrect_pin ) + "</p>";
            }

            str_replace( authenticate_html, c_pin_value, pin );
            str_replace( authenticate_html, c_error_message, error_message );

            extra_content << "<h3>" << GDS( c_display_authentication_required ) << "</h3>\n";

            if( abs( p_session_info->dtm_offset ) >= c_max_pin_time_difference )
               extra_content << "<p><b>" << GDS( c_display_local_time_bad ) << "</b></p>\n";

            extra_content << authenticate_html;
         }
         else
         {
            if( !user_home_info.empty( ) )
               extra_content << "<p align=\"center\">" << user_home_info << "</p>\n";

            for( map< string, list_source >::iterator i = home_lists.begin( ); i != home_lists.end( ); ++i )
            {
               bool can_output = true;

               if( p_session_info->is_admin_user
                || i->second.row_data.empty( ) || ( using_anonymous
                && !i->second.lici->second->extras.count( c_list_type_extra_allow_anonymous ) ) )
                  can_output = false;

               if( can_output )
               {
                  string header;

                  if( !i->second.name.empty( ) )
                     header = get_view_or_list_header( qlink, i->second.name, mod_info, *p_session_info );

                  if( !header.empty( ) )
                  {
                     extra_content << "<h3>" << header << "</h3>\n";

                     output_list_form( extra_content,
                      i->second, session_id, uselect, "", false,
                      cookies_permitted, true, false, list_selections, list_search_text,
                      list_search_values, 0, false, "", false, "", i->second.lici->second->id, *p_session_info,
                      extra_content_func, specials, use_url_checksum, qlink, findinfo + listsrch, selected_records,
                      embed_images, !hashval.empty( ), false, has_any_changing_records, back_count );
                  }
               }
            }
         }
      }
      else if( ( cmd == c_cmd_view ) || ( cmd == c_cmd_pview ) )
      {
         extra_content << "<div id=\"title\">";
         extra_content << "<h3><span class=\"alignleft\">";

         if( is_new_record )
            extra_content << GDS( c_display_new ) << " " << mod_info.get_string( view.name );
         else
            extra_content << get_view_or_list_header( qlink, view.name, mod_info, *p_session_info );

         extra_content << "</span>";

         if( cmd != c_cmd_pview )
            extra_content << "<span class=\"alignright\"><a href=\"javascript:go_back( "
             << back_count << " );\">" << GDS( c_display_back ) << "</a></span>";

         extra_content << "</h3></div>\n";

         string owner;

         if( !view.owning_user_field.empty( ) && view.fk_field_values.count( view.owning_user_field ) )
            owner = view.fk_field_values.find( view.owning_user_field )->second;

         string user_slevel;

         if( !p_session_info->other_slevels.count( p_session_info->user_other ) )
            user_slevel = p_session_info->user_slevel;
         else
            user_slevel = p_session_info->other_slevels.find( p_session_info->user_other )->second;

         string view_perm( view.perm );
         if( !view_perm.empty( ) && view_perm[ 0 ] == '!' )
            view_perm.erase( 0, 1 );

         bool does_not_have_perm = false;

         if( !view_perm.empty( ) && !p_session_info->user_perms.count( view_perm ) )
            does_not_have_perm = true;

         if( record_not_found_error )
         {
            // NOTE: If a "jump back" is to occur after an action then don't display
            // any error here (so that deleting via a view action can be supported).
            if( back.empty( ) )
            {
               bool missing_datachain = false;

               // NOTE: For blockchain applications 'special' is being used to hold
               // the datachain identity in a record link (so the missing datachain
               // message can appear rather than missing record error message).
               if( !special.empty( ) && !get_storage_info( ).blockchain.empty( ) )
               {
                  string expected( c_bc_prefix + special + c_zenith_suffix );

                  string response;

                  // NOTE: It is expected that a zenith tag should always exist.
                  simple_command( *p_session_info, "file_tags " + expected + "*", &response, true );

                  if( response != expected )
                     missing_datachain = true;
               }

               if( missing_datachain )
                  extra_content << "<p align=\"center\">"
                   << string_message( GDS( c_display_datachain_not_found ),
                   make_pair( c_display_datachain_not_found_parm_identity, special ) ) << "</p>\n";
               else
                  extra_content << "<p align=\"center\">" << GDS( c_display_record_not_found ) << "</p>\n";
            }
         }
         else if( view.type == c_view_type_none
          || ( does_not_have_perm && !p_session_info->is_admin_user
          && ( view.type == c_view_type_admin || view.type == c_view_type_admin_print ) )
          || ( does_not_have_perm && owner != p_session_info->user_key
          && ( view.type == c_view_type_owner || view.type == c_view_type_owner_print ) )
          || ( does_not_have_perm && !p_session_info->is_admin_user && owner != p_session_info->user_key
          && ( view.type == c_view_type_admin_owner || view.type == c_view_type_admin_owner_print ) )
          || ( !view.permission_field.empty( ) && !has_permission( view_permission_value, *p_session_info ) )
          || ( !view.security_level_field.empty( ) && view_security_level_value.length( ) < user_slevel.length( ) ) )
            extra_content << "<p align=\"center\" class=\"error\">"
             << GDS( c_display_error ) << ": " << GDS( c_display_permission_denied ) << ".</p>\n";
         else
         {
            view.id = "view";

            string edit_field_list;

            bool is_owner = false;

            string extra_html_content;

            is_editable = output_view_form( extra_content,
             act, was_invalid, user_field_info, exec, cont, data, error_message,
             extra, field, view, vtab_num, session_id, uselect, cookies_permitted,
             embed_images, new_field_and_values, *p_session_info, edit_field_list, edit_timeout_func,
             extra_content_func, use_url_checksum, !qlink.empty( ), show_opts, cmd == c_cmd_pview,
             back_count, pdf_view_file_name, is_owner, extra_html_content, has_any_changing_records );

            p_session_info->was_file_remove = false;

            extra_content_func += "dataFieldList = '" + edit_field_list + "';\n";

            uint64_t state( view.state );

            if( !is_editable )
               state |= c_state_uneditable;

            // NOTE: For new records (and currently for "printable" views) do not output child lists.
            // FUTURE: For "printable" views it makes sense to be able to support child lists however
            // they should be identified differently (perhaps as "printchild") as the normal sublists
            // may not be applicable to the printed version.
            if( !is_new_record && ( cmd != c_cmd_pview ) )
            {
               extra_content << "<br/><br/>\n";

               if( !is_ui_prototype( ) )
               {
                  extra_content << "<table width=\"100%\">\n";
                  extra_content << "<tr><td>\n";
               }

               if( is_ui_prototype( ) )
               {
                  extra_content << "<div class=\"width-fix vtab tab-alloc\" style=\"\"><br/>\n";
                  extra_content << "<ul class=\"tabs-hz\">\n";
               }
               else
               {
                  extra_content << "<table id=\"vtabc\" class=\"vtab\">\n";

                  extra_content << "<thead>\n";
                  extra_content << "<tr>\n";
               }

               int n = 0, x = 0, last = 0;

               map< int, bool > children_okay;

               set< string > children_not_permitted;

               for( map< string, int >::iterator i = child_names.begin( ); i != child_names.end( ); ++i )
               {
                  ++x;

                  string list_perm( child_lists[ i->second ].perm );
                  string list_type = child_lists[ i->second ].lici->second->type;

                  // FUTURE: Currently the only expected format for child list permission is that of
                  // <child_type>=<perm> (which is treated as though it was <child_type>=!<perm> for
                  // a non-empty <perm>). If <child_type>=!<perm> were to be supported syntactically
                  // then this logic would need to be reworked accordingly.
                  bool is_okay = has_permission( list_perm, *p_session_info );

                  if( list_type == c_list_type_child_admin )
                  {
                     if( p_session_info->is_admin_user )
                        is_okay = true;
                     else if( list_perm.empty( ) )
                        is_okay = false;
                  }
                  else if( list_type == c_list_type_child_owner )
                  {
                     if( is_owner )
                        is_okay = true;
                     else if( list_perm.empty( ) )
                        is_okay = false;
                  }
                  else if( list_type == c_list_type_child_admin_owner )
                  {
                     if( is_owner || p_session_info->is_admin_user )
                        is_okay = true;
                     else if( list_perm.empty( ) )
                        is_okay = false;
                  }

                  if( child_lists[ i->second ].lici->second->extras.count( c_list_type_extra_pstate ) )
                  {
                     istringstream isstr( child_lists[ i->second ].lici->second->extras[ c_list_type_extra_pstate ] );

                     uint64_t flag;
                     isstr >> hex >> flag;

                     if( !( state & flag ) )
                        is_okay = false;
                  }

                  if( using_anonymous
                   && !child_lists[ i->second ].lici->second->extras.count( c_list_type_extra_allow_anonymous ) )
                     is_okay = false;

                  children_okay[ x - 1 ] = is_okay;

                  if( is_okay )
                     last = ++n;
                  else
                     children_not_permitted.insert( i->first );
               }

               // NOTE: If the selected tab child has become invalid
               // due to parent state then will just select the last
               // accessible tab found.
               if( vtabc_num > last )
                  vtabc_num = last;

               x = 0;
               n = 0;

               bool is_first = true;

               for( map< string, int >::iterator i = child_names.begin( ); i != child_names.end( ); ++i )
               {
                  ++x;

                  if( !children_okay[ x - 1 ] )
                     continue;

                  string name( get_display_name( mod_info.get_string( child_lists[ i->second ].name ) ) );

                  string li_class;

                  if( is_first )
                  {
                     is_first = false;
                     li_class = "first";
                  }

                  if( ++n == vtabc_num )
                  {
                     if( !li_class.empty( ) )
                        li_class += " ";

                     li_class += "active";

                     if( is_ui_prototype( ) )
                        extra_content << "<li class=\"" << li_class << "\">" << name << "</li>\n";
                     else
                        extra_content << "<th class=\"selected_tab\" align=\"center\">" << name << "</th>\n";
                  }
                  else
                  {
                     if( is_ui_prototype( ) )
                        extra_content << "<li class=\"" << li_class << "\"><a href=\"javascript:";
                     else
                        extra_content << "<td class=\"tab\" align=\"center\"><a href=\"javascript:";

                     if( use_url_checksum )
                     {
                        string checksum_values( string( c_cmd_view ) + data + ident + uselect );
                        string new_checksum_value( get_checksum( *p_session_info, checksum_values ) );

                        extra_content << "query_update( '"
                         << c_param_chksum << "', '" << new_checksum_value << "', true ); ";
                     }

                     extra_content << "query_update( 'bcount', '" << to_string( back_count + 1 ) << "', true ); ";
                     extra_content << "query_update( 'vtabc', '" << n << "' );\">" << name << "</a>";

                     if( is_ui_prototype( ) )
                        extra_content << "</li>\n";
                     else
                        extra_content << "</td>\n";
                  }

                  // NOTE: Only fetch data from the application server for the active child tab.
                  if( n == vtabc_num )
                  {
                     string child_list_info( c_list_prefix + to_string( x - 1 ) + c_info_suffix );

                     if( input_data.count( child_list_info ) )
                        child_list_info = input_data[ child_list_info ];

                     string child_list_sort( c_list_prefix + to_string( x - 1 ) + c_sort_suffix );

                     if( input_data.count( child_list_sort ) )
                        child_list_sort = input_data[ child_list_sort ];

                     if( !populate_list_info( child_lists[ i->second ], list_selections,
                      list_search_text, list_search_values, child_list_info, child_list_sort, data,
                      ( cmd == c_cmd_pview ), &view, child_lists[ i->second ].new_pfield, 0, *p_session_info, 0, 0, 0, &state ) )
                        had_send_or_recv_error = true;
                  }
               }

               if( is_ui_prototype( ) )
               {
                  extra_content << "</ul>\n";
                  extra_content << "</div>\n";
               }
               else
               {
                  extra_content << "</tr>\n";
                  extra_content << "</thead>\n";

                  extra_content << "</table>\n";
               }

               if( n && is_ui_prototype( ) )
                  extra_content << "<div class=\"menu width-fix width-fix-view childlistborder\">\n";

               n = 0;

               for( map< string, int >::iterator i = child_names.begin( ); i != child_names.end( ); ++i )
               {
                  if( children_not_permitted.count( i->first ) )
                     continue;

                  if( ++n != vtabc_num )
                     continue;

                  view_has_a_child_list = true;

                  output_list_form( extra_content, child_lists[ i->second ],
                   session_id, uselect, "", ( cmd == c_cmd_pview ), cookies_permitted,
                   true, ( act == c_act_edit ), list_selections, list_search_text, list_search_values,
                   state, true, data, keep_checks, ident, "", *p_session_info, extra_content_func, specials, use_url_checksum, "",
                   findinfo + listsrch, selected_records, embed_images, !hashval.empty( ), is_owner, has_any_changing_records, back_count );
               }

               if( n && is_ui_prototype( ) )
                  extra_content << "</div>\n";
            }

            // FUTURE: The height should be grown dynamically via Javascript rather than hard-coded here.
            if( !extra_html_content.empty( ) )
               extra_content << "</br><iframe src=\""
                << extra_html_content << "\" width=\"100%\" height=\"730px\"></iframe>";
         }

         if( cmd == c_cmd_pview && file_exists( ident + ".htms" ) )
            extra_content << buffer_file( ident + ".htms" );
      }
      else if( ( cmd == c_cmd_list ) || ( cmd == c_cmd_plist ) )
      {
         string list_perm( list.perm );

         if( !list_perm.empty( ) && list_perm[ 0 ] == '!' )
            list_perm.erase( 0, 1 );

         if( !p_session_info->is_admin_user // NOTE: "admin" is always permitted access
          && ( ( list.type == c_list_type_admin && list_perm.empty( ) )
          || ( !list_perm.empty( ) && !p_session_info->user_perms.count( list_perm ) ) ) )
            extra_content << "<p align=\"center\" class=\"error\">"
             << GDS( c_display_error ) << ": " << GDS( c_display_permission_denied ) << ".</p>\n";
         else
         {
            extra_content << "<div id=\"title\">";
            extra_content << "<h3><span class=\"alignleft\">"
             << get_view_or_list_header( qlink, olist.name, mod_info, *p_session_info, &list.name ) << "</span>";

            if( cmd != c_cmd_plist )
               extra_content << "<span class=\"alignright\"><a href=\"javascript:go_back( "
                << back_count << " );\">" << GDS( c_display_back ) << "</a></span>";

            extra_content << "</h3></div>\n";

            // NOTE: For non-print lists display links to any "display variations" (if found).
            if( cmd != c_cmd_plist )
            {
               vector< string >* p_var_ids = &olist.lici->second->var_ids;
               if( !p_var_ids->empty( ) )
               {
                  extra_content << "<table class=\"vtab\"><tr>\n";

                  if( listvar.empty( ) )
                     extra_content << "<th class=\"tab\" align=\"center\">"
                      << GDS( c_display_standard ) << "</th>\n";
                  else
                  {
                     extra_content << "  <td class=\"tab\" align=\"center\"><a href=\""
                      << get_module_page_name( module_ref ) << "?cmd=" << c_cmd_list << "&ident=" << oident;

                     if( !uselect.empty( ) )
                        extra_content << "&" << c_param_uselect << "=" << uselect;

                     if( !cookies_permitted )
                        extra_content << "&session=" << session_id;

                     if( use_url_checksum )
                     {
                        string checksum_values( to_string( c_cmd_list ) + oident + uselect );

                        extra_content << "&" << c_param_chksum
                         << "=" << get_checksum( *p_session_info, checksum_values );
                     }

                     extra_content << "\">" << GDS( c_display_standard ) << "</a></td>\n";
                  }

                  for( size_t i = 0; i < p_var_ids->size( ); i++ )
                  {
                     list_info_const_iterator lici = mod_info.list_info.find( ( *p_var_ids )[ i ] );

                     if( lici == mod_info.list_info.end( ) )
                        throw runtime_error( "Unknown variation list id '" + ( *p_var_ids )[ i ] + "'." );

                     if( listvar == ( *p_var_ids )[ i ] )
                        extra_content << "  <th class=\"tab\" align=\"center\">"
                         << mod_info.get_string( lici->second->name ) << "</th>\n";
                     else
                     {
                        extra_content << "  <td class=\"tab\" align=\"center\"><a href=\""
                         << get_module_page_name( module_ref ) << "?cmd=" << c_cmd_list
                         << "&ident=" << oident << "&listvar=" << ( *p_var_ids )[ i ];

                        if( !uselect.empty( ) )
                           extra_content << "&" << c_param_uselect << "=" << uselect;

                        if( !cookies_permitted )
                           extra_content << "&session=" << session_id;

                        if( use_url_checksum )
                        {
                           string checksum_values( to_string( c_cmd_list ) + oident + uselect );
                           extra_content << "&" << c_param_chksum
                            << "=" << get_checksum( *p_session_info, checksum_values );
                        }

                        extra_content << "\">" << mod_info.get_string( lici->second->name ) << "</a></td>\n";
                     }
                  }

                  extra_content << "</td></tr></table>\n";
               }
            }

            string ui_class_changing( c_ui_class_changing );
            string ui_group_changing( c_ui_group_changing );

            string ui_class_name( lower( list.mclass ) );

            replace( ui_class_changing, c_ui_class_marker, ui_class_name );

            string ui_group_name( '@' + ui_class_name );
            string::size_type pos = ui_group_name.find( '_' );

            replace( ui_group_changing, c_ui_group_marker, ui_group_name.substr( 0, pos ) );

            if( file_exists( ui_class_changing ) || file_exists( ui_group_changing ) )
               has_any_changing_records = true;

            output_list_form( extra_content, list, session_id, uselect, error_message,
             ( cmd == c_cmd_plist ), cookies_permitted, true, false, list_selections,
             list_search_text, list_search_values, 0, false, "", keep_checks, "", oident,
             *p_session_info, extra_content_func, specials, use_url_checksum, qlink, findinfo + listsrch,
             selected_records, embed_images, !hashval.empty( ), false, has_any_changing_records, back_count, &pdf_list_file_name );
         }
      }
      else if( !server_command.empty( ) )
      {
         extra_content << "<pre>\n";

         string response;

         int timeout = c_initial_response_timeout;

         while( response.empty( ) || response[ 0 ] != '(' )
         {
            response.erase( );

            if( p_session_info->p_socket->read_line( response, timeout ) <= 0 )
            {
               if( p_session_info->p_socket->had_timeout( ) )
                  extra_content << "\n*** response truncated due to timeout ***\n";
               else
                  extra_content << "\n*** response truncated due to closure ***\n";

               break;
            }

            timeout = c_subsequent_response_timeout;

            extra_content << escape_markup( response ) << "\n";
         }

         extra_content << "</pre>\n";
      }

      if( created_session )
         add_session_info( session_id, p_session_info );

      if( !is_in_edit )
      {
         if( has_any_changing_records || has_any_potentially_changing_records )
         {
            string seconds;

            bool is_blockchain_application = false;

            if( !p_session_info->is_meta_module
             && !get_storage_info( ).blockchain.empty( ) )
               is_blockchain_application = true;

            if( !is_blockchain_application
             && ( p_session_info->ip_addr == string( c_local_ip_addr ) ) )
               seconds = to_string( c_auto_refresh_seconds_local );
            else
               seconds = to_string( c_auto_refresh_seconds_remote );

            if( !has_any_changing_records )
               extra_content_func += "slow_refresh_seconds = "
                + to_string( c_slow_refresh_seconds ) + ";\nslow_refresh( );\n";
            else
               extra_content_func += "auto_refresh_seconds = " + seconds + ";\nauto_refresh( );\n";
         }

         if( !has_any_changing_records )
         {
            if( ( cmd == c_cmd_view )
             && ( view.state & c_state_force_focus_refresh ) )
               extra_content_func += "reload_after_refocus( );\n";
         }
      }

      if( g_ciyam_interface_html.find( c_form_content_comment ) != string::npos )
      {
         if( created_session )
            extra_content << "<p>Created session " << session_id << ".</p>\n";
         else
            extra_content << "<p>Located session " << session_id << ".</p>\n";

         extra_content << "<p>\n";

         for( map< string, string >::const_iterator ci = input_data.begin( ); ci != input_data.end( ); ++ci )
            extra_content << "Input[ " << ci->first << " ] => " << ci->second << "<br/>\n";

         extra_content << "</p>\n";

         extra_content << "<p>Total number of sessions = " << get_num_sessions( ) << ".</p>\n";
      }

      extra_content << "</div>\n</div>\n\n";

      if( cmd != c_cmd_pview && cmd != c_cmd_plist )
         extra_content << g_footer_html;

      extra_content << "</div>\n";

      if( performed_file_attach_or_detach )
      {
         // NOTE: Set this so that the UI can detect that a file upload has just taken place.
         if( act != c_act_remove )
            extra_content_func += "sessionStorage.setItem( 'uploaded', 'true' );\n";

         // KLUDGE: Refresh in order to see an uploaded file and/or read the updated record.
         extra_content_func += "refresh( false );\n";
      }

      // NOTE: Erase any existing cookie value first.
      if( created_session && cookies_permitted )
         extra_content_func += "document.cookie = '" + get_cookie_value( "", c_anon_user_key ) + "';\n";

      if( cookies_permitted )
         extra_content_func += "document.cookie = '" + get_cookie_value( session_id,
          p_session_info->user_id, cmd == c_cmd_quit, p_session_info->dtm_offset ) + "';\n";

      extra_content_func += "displayReload = '"
       + replaced( GDS( c_display_reload ), "\"", "\\x22", "'", "\\x27" ) + "';\n";

      extra_content_func += "displayTimeout = '"
       + replaced( GDS( c_display_timeout ), "\"", "\\x22", "'", "\\x27" ) + "';\n";

      extra_content_func += "displayInvalidDate = '"
       + replaced( GDS( c_display_invalid_date ), "\"", "\\x22", "'", "\\x27" ) + "';\n";

      extra_content_func += "displayInvalidTime = '"
       + replaced( GDS( c_display_invalid_time ), "\"", "\\x22", "'", "\\x27" ) + "';\n";

      extra_content_func += "displayIncorrectDateFormat = '"
       + replaced( GDS( c_display_incorrect_date_format ), "\"", "\\x22", "'", "\\x27" ) + "';\n";

      extra_content_func += "displayIncorrectTimeFormat = '"
       + replaced( GDS( c_display_incorrect_time_format ), "\"", "\\x22", "'", "\\x27" ) + "';\n";

      extra_content_func += "displayEnterRequiredValues = '"
       + replaced( GDS( c_display_enter_required_values ), "\"", "\\x22", "'", "\\x27" ) + "';\n";

      extra_content_func += "displayIncorrectBytesFormat = '"
       + replaced( GDS( c_display_incorrect_bytes_format ), "\"", "\\x22", "'", "\\x27" ) + "';\n";

      extra_content_func += "displaySelectOneOrMoreItems = '"
       + replaced( GDS( c_display_select_one_or_more_items ), "\"", "\\x22", "'", "\\x27" ) + "';\n";

      extra_content_func += "displayIncorrectIntegerFormat = '"
       + replaced( GDS( c_display_incorrect_integer_format ), "\"", "\\x22", "'", "\\x27" ) + "';\n";

      extra_content_func += "displayIncorrectNumericFormat = '"
       + replaced( GDS( c_display_incorrect_numeric_format ), "\"", "\\x22", "'", "\\x27" ) + "';\n";

      extra_content_func += "displayIncorrectDurationFormat = '"
       + replaced( GDS( c_display_incorrect_duration_format ), "\"", "\\x22", "'", "\\x27" ) + "';\n";

      extra_content_func += "displayMaximumTextLimitReached = '"
       + replaced( GDS( c_display_maximum_text_limit_reached ), "\"", "\\x22", "'", "\\x27" ) + "';\n";

      extra_content_func += "displayYourSessionIsAboutToExpire = '"
       + replaced( GDS( c_display_your_session_is_about_to_expire ), "\"", "\\x22", "'", "\\x27" ) + "';\n";

      extra_content_func += "displayYourSessionHasBecomeInvalid = '"
       + replaced( GDS( c_display_your_session_has_become_invalid ), "\"", "\\x22", "'", "\\x27" ) + "';\n";

      extra_content_func += "displaySelectedRecordsWillBeRemoved = '"
       + replaced( GDS( c_display_selected_records_will_be_removed ), "\"", "\\x22", "'", "\\x27" ) + "';\n";

      extra_content_func += "displayPasswordVerificationIncorrect = '"
       + replaced( GDS( c_display_password_verification_incorrect ), "\"", "\\x22", "'", "\\x27" ) + "';\n";

      if( ( cont != c_true ) && !new_key.empty( ) )
      {
         string extra( ident );

         if( !uselect.empty( ) )
            extra += "&" + string( c_param_uselect ) + "=" + uselect;

         if( !cookies_permitted )
            extra += "&session=" + session_id;

         if( use_url_checksum )
         {
            string checksum_values( string( c_cmd_view ) + new_key + ident + uselect );
            extra += "&" + to_string( c_param_chksum ) + "=" + get_checksum( *p_session_info, checksum_values );
         }

         extra_content_func += "source = window.location.href.split( '?' );\n";
         extra_content_func += "window.location.replace( "
          "source[ 0 ] + '?cmd=" + to_string( c_cmd_view ) + "&data=" + new_key + "&ident=" + extra + "' );\n";
      }

      if( !temp_session )
      {
         // NOTE: If editing then the timeout warning needs to result in a 'cont' action
         // (rather than just a page refresh) to ensure data entered/modified is not lost.
         if( !is_in_edit )
            extra_content_func += "warn_refresh_func = '';\n";
         else
            extra_content_func += "warn_refresh_func = '" + edit_timeout_func + "';\n";

         extra_content_func += "warn_refresh_seconds = " + to_string( c_warn_refresh_seconds ) + ";\n";
         extra_content_func += "warn_refresh( );";
      }

      if( p_session_info && p_session_info->logged_in )
         extra_content_func += "\nloggedIn = true;";

      if( !scrollx.empty( ) && !scrolly.empty( ) )
         extra_content_func += "\nscroll_page( " + scrollx + ", " + scrolly + " );";
   }
}
