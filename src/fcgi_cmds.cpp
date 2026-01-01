// Copyright (c) 2011-2012 CIYAM Pty. Ltd. ACN 093 704 539
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
#  include <stdexcept>
#endif

#include "fcgi_cmds.h"

#include "regex.h"
#include "format.h"
#include "sha256.h"
#include "sockets.h"
#ifdef SSL_SUPPORT
#  include "ssl_socket.h"
#endif
#include "date_time.h"
#include "utilities.h"
#include "ciyam_core.h"
#include "fcgi_utils.h"
#include "crypt_stream.h"
#include "ciyam_interface.h"

using namespace std;

namespace
{

#include "ciyam_constants.h"

const int c_initial_response_timeout = 30000;
const int c_subsequent_response_timeout = 10000;

const char* const c_order_reverse = "reverse";

}

string get_uid_info( const session_info& sess_info, bool quote_if_has_space_in_name = true )
{
   string uid_info( sess_info.user_key );

   if( !uid_info.empty( ) )
   {
      if( !sess_info.user_slevel.empty( ) )
         uid_info += "!" + sess_info.user_slevel;

      if( sess_info.user_name.empty( ) )
         uid_info += ":" + sess_info.user_id;
      else
      {
         // NOTE: As the user name is not intended for anything other than
         // internal display purposes rather than escaping quotes they are
         // just changed (if escaping was to be used then it would require
         // rework in the server code for session command handling).
         string user_name( sess_info.user_name );
         replace( user_name, "\"", "'" );

         string::size_type pos = user_name.find( ' ' );

         if( pos == string::npos || !quote_if_has_space_in_name )
            uid_info += ":" + user_name;
         else
            uid_info = "\"" + uid_info + ":" + user_name + "\"";
      }
   }
   else
   {
      uid_info = "anon!";
      uid_info += c_default_security_level;
   }

   return uid_info;
}

void read_module_strings( module_info& info, tcp_socket& socket )
{
   string strings_cmd( "module_strings_list " );
   strings_cmd += info.id;

   if( socket.write_line( strings_cmd ) <= 0 )
      throw runtime_error( "Unable to retrieve '" + info.name + "' module strings." );

   info.strings.clear( );
   int timeout = c_initial_response_timeout;

   while( true )
   {
      string response;

      if( socket.read_line( response, timeout ) <= 0 )
      {
         if( socket.had_timeout( ) )
            throw runtime_error( "Timeout occurred reading module strings response." );
         else
            throw runtime_error( "Unexpected session closure during strings initialisation." );
      }

      if( response == c_response_okay )
         break;

      timeout = c_subsequent_response_timeout;

      string::size_type pos = response.find( ' ' );
      if( pos == string::npos )
         throw runtime_error( "unexpected string format '" + response + "'" );

      info.strings.insert( make_pair( response.substr( 0, pos ), response.substr( pos + 1 ) ) );
   }

   // NOTE: The list menus container is only constructed here as the model strings
   // will affect the order of the menu items.
   info.list_info.clear( );
   info.list_menus.clear( );

   for( size_t i = 0; i < info.lists.size( ); i++ )
   {
      info.lists[ i ].var_ids.clear( );
      info.list_info.insert( make_pair( info.lists[ i ].id, &info.lists[ i ] ) );

      if( !info.lists[ i ].pid.empty( ) )
      {
         if( !info.list_info.count( info.lists[ i ].pid ) )
            throw runtime_error( "unexpected variation parent id '" + info.lists[ i ].pid + "' not found" );

         info.list_info[ info.lists[ i ].pid ]->var_ids.push_back( info.lists[ i ].id );
      }
      else if( ( info.lists[ i ].type != c_list_type_home )
       && ( info.lists[ i ].type != c_list_type_home_anon ) && ( info.lists[ i ].type != c_list_type_child )
       && ( info.lists[ i ].type != c_list_type_user_child ) && ( info.lists[ i ].type != c_list_type_child_admin )
       && ( info.lists[ i ].type != c_list_type_child_owner ) && ( info.lists[ i ].type != c_list_type_child_admin_owner ) )
         info.list_menus.insert( make_pair( info.get_string( info.lists[ i ].name ), &info.lists[ i ] ) );
   }
}

bool simple_command( session_info& sess_info, const string& cmd, string* p_response, bool is_optional )
{
   DEBUG_TRACE( cmd );

   if( sess_info.p_socket->write_line( cmd ) <= 0 )
   {
      DEBUG_TRACE( "socket write failure" );
      return false;
   }

   size_t reponse_timeout = c_initial_response_timeout;

#ifdef SSL_SUPPORT
   if( cmd == "starttls" )
   {
      reponse_timeout = c_subsequent_response_timeout;

      sess_info.p_socket->ssl_connect( c_initial_response_timeout );
   }
#endif

   string response;

   if( sess_info.p_socket->read_line( response, reponse_timeout ) <= 0 )
   {
      if( sess_info.p_socket->had_timeout( ) )
         DEBUG_TRACE( "timeout awaiting initial response" );
      else
         DEBUG_TRACE( "unexpected socket closure getting initial response" );

      return false;
   }

   if( p_response )
   {
      if( response != c_response_okay )
      {
         *p_response = response;

         DEBUG_TRACE( response );

         if( response.find( c_response_error_prefix ) == 0 )
            return false;
      }
      else
      {
         if( !is_optional )
            return false;
         else
         {
            p_response->clear( );

            return true;
         }
      }

      response.clear( );

      if( sess_info.p_socket->read_line( response, c_subsequent_response_timeout ) <= 0 )
      {
         if( sess_info.p_socket->had_timeout( ) )
            DEBUG_TRACE( "timeout awaiting subsequent response" );
         else
            DEBUG_TRACE( "unexpected socket closure getting subsequent response" );

         return false;
      }
   }

   if( response != c_response_okay )
   {
      DEBUG_TRACE( "unexpected server response '" + response + "'" );

      return false;
   }

   return true;
}

bool perform_update( const string& module, const string& class_id,
 const string& key, const vector< pair< string, string > >& field_value_pairs,
 const session_info& sess_info, string* p_error_message, string* p_fields_and_values_prefix )
{
   bool okay = true;

   string fields_and_values;

   if( p_fields_and_values_prefix )
      fields_and_values = *p_fields_and_values_prefix;

   for( size_t i = 0; i < field_value_pairs.size( ); i++ )
   {
      if( !fields_and_values.empty( ) )
         fields_and_values += ',';

      fields_and_values += field_value_pairs[ i ].first;
      fields_and_values += "=";
      fields_and_values += escaped( field_value_pairs[ i ].second, ",\"" );
   }

   string cmd( string( c_cmd_update ) + " " + get_uid_info( sess_info ) + " "
    + date_time::standard( ).as_string( ) + " " + module + " " + class_id );

   if( !sess_info.user_group.empty( ) )
      cmd += " -g=" + sess_info.user_group;

   if( !sess_info.tz_name.empty( ) )
      cmd += " -tz=" + sess_info.tz_name;

   cmd += " " + key + " \"" + fields_and_values + "\"";

   DEBUG_TRACE( cmd );

   if( sess_info.p_socket->write_line( cmd ) > 0 )
   {
      string response;

      int rc = sess_info.p_socket->read_line( response, c_initial_response_timeout );

      if( ( rc <= 0 ) || ( response != c_response_okay ) )
      {
         okay = false;

         if( p_error_message )
            *p_error_message = response;
      }
   }
   else
      okay = false;

   return okay;
}

bool perform_update( const string& module,
 const string& class_id, const string& key, const string& field,
 const string& old_value, const string& new_value, const session_info& sess_info, string& error )
{
   bool okay = true;

   string cmd( string( c_cmd_update ) + " " + get_uid_info( sess_info ) + " "
    + date_time::standard( ).as_string( ) + " " + module + " " + class_id + " " + key + " \"" + field
    + "=" + escaped( new_value, ",\"" ) + "\" \"" + field + "=" + escaped( old_value, ",\"" ) + "\"" );

   DEBUG_TRACE( cmd );

   if( sess_info.p_socket->write_line( cmd ) > 0 )
   {
      string response;

      int rc = sess_info.p_socket->read_line( response, c_initial_response_timeout );

      if( ( rc <= 0 ) || ( response != c_response_okay ) )
      {
         okay = false;

         if( !response.empty( ) )
            error = response;
         else
            error = "server timeout occurred";
      }
   }
   else
      okay = false;

   return okay;
}

bool perform_action( const string& module_name,
 const string& class_id, const string& act, const string& app,
 const string& field, const string& fieldlist, const string& exec,
 const string& extra, row_error_container& row_errors, const session_info& sess_info )
{
   bool okay = true;

   vector< string > code_and_versions;

   if( !app.empty( ) )
      split( app, code_and_versions );

   const module_info& mod_info( *get_storage_info( ).modules_index.find( module_name )->second );

   string view_id;

   if( mod_info.view_cids.count( class_id ) )
      view_id = mod_info.view_cids.find( class_id )->second;

   string fields_and_values;

   if( act == c_act_link )
   {
      fields_and_values += field;
      fields_and_values += "=";
      fields_and_values += escaped( extra, ",\"" );
   }
   else if( !fieldlist.empty( ) )
   {
      vector< string > field_ids;
      vector< string > field_values;

      split( fieldlist, field_ids );
      split( extra, field_values );

      if( field_ids.size( ) != field_values.size( ) )
         throw runtime_error( "unexpected field_ids and field_values size mismatch in perform_action" );

      for( size_t i = 0; i < field_ids.size( ); i++ )
      {
         if( !fields_and_values.empty( ) )
            fields_and_values += ',';

         fields_and_values += field_ids[ i ];
         fields_and_values += "=";
         fields_and_values += escaped( escaped( field_values[ i ], "," ), ",\"" );
      }
   }

   date_time dt( date_time::standard( ) );
   string current_dtm( dt.as_string( ) );

   // NOTE: If a view is found for the class and it contains a "modified" date/time field then this field will be updated.
   if( !view_id.empty( ) && ( act == c_act_link ) )
   {
      for( size_t i = 0; i < mod_info.view_info.find( view_id )->second->fields.size( ); i++ )
      {
         fld_info fld( mod_info.view_info.find( view_id )->second->fields[ i ] );

         if( fld.field != c_key_field )
         {
            set< string > extras;

            if( !fld.extra.empty( ) )
               split( fld.extra, extras, '+' );

            if( extras.count( c_view_field_extra_modify_datetime ) )
            {
               fields_and_values += ',';
               fields_and_values += fld.field;
               fields_and_values += '=';
               fields_and_values += "U" + current_dtm;

               break;
            }
         }
      }
   }

   bool is_versioned = true;

   string exec_info( exec );

   // NOTE: If the method name has been prefixed with a '!' then it will be executed without version information.
   if( !exec_info.empty( ) && ( exec_info[ 0 ] == '!' ) )
   {
      is_versioned = false;
      exec_info.erase( 0, 1 );
   }

   // NOTE: If the method name has (also) been prefixed with a '^' then instance execution order will be reversed.
   bool is_reversed = false;
   if( !exec_info.empty( ) && ( exec_info[ 0 ] == '^' ) )
   {
      is_reversed = true;
      exec_info.erase( 0, 1 );
   }

   if( is_reversed || ( act == c_act_rdel ) )
      reverse( code_and_versions.begin( ), code_and_versions.end( ) );

   string key_list;
   string ver_list;

   if( act == c_act_exec )
   {
      if( app.empty( ) )
      {
         is_versioned = false;

         key_list = "\" \"";
         code_and_versions.push_back( "" );
      }
      else
      {
         for( size_t i = 0; i < code_and_versions.size( ); i++ )
         {
            string next_key( code_and_versions[ i ] );
            string next_ver;

            string::size_type pos = next_key.find( ' ' );

            if( pos != string::npos )
            {
               next_ver = next_key.substr( pos + 2 ); // NOTE: Skip the '=' prefix.
               next_key.erase( pos );
            }

            if( !key_list.empty( ) )
               key_list += ',';

            key_list += next_key;

            if( is_versioned )
            {
               if( !ver_list.empty( ) )
                  ver_list += ',';

               ver_list += next_ver;
            }
         }
      }
   }

   for( size_t i = 0; i < code_and_versions.size( ); i++ )
   {
      string act_cmd;

      if( act == c_act_link )
         act_cmd = c_cmd_update;
      else if( ( act == c_act_del ) || ( act == c_act_rdel ) )
         act_cmd = c_cmd_destroy;
      else if( act == c_act_exec )
         act_cmd = c_cmd_execute;
      else
         throw runtime_error( "Unknown list action '" + act + "'." );

      if( !key_list.empty( ) )
      {
         act_cmd += " " + get_uid_info( sess_info ) + " " + current_dtm + " " + mod_info.id + " " + class_id;

         if( !sess_info.user_group.empty( ) )
            act_cmd += " -g=" + sess_info.user_group;

         if( !sess_info.tz_name.empty( ) )
            act_cmd += " -tz=" + sess_info.tz_name;

         if( !fieldlist.empty( ) && ( act == c_act_exec ) )
            act_cmd += " \"-v=" + fields_and_values + "\"";

         act_cmd += " " + key_list;

         if( is_versioned )
            act_cmd += " =" + ver_list;
      }
      else
      {
         if( is_versioned )
         {
            act_cmd += " " + get_uid_info( sess_info ) + " " + current_dtm + " " + mod_info.id + " " + class_id;

            if( !sess_info.user_group.empty( ) )
               act_cmd += " -g=" + sess_info.user_group;

            if( !sess_info.tz_name.empty( ) )
               act_cmd += " -tz=" + sess_info.tz_name;

            if( !fieldlist.empty( ) && ( act == c_act_exec ) )
               act_cmd += " \"-v=" + fields_and_values + "\"";

            act_cmd += " " + code_and_versions[ i ];
         }
         else
         {
            string next_code_and_version( code_and_versions[ i ] );

            string::size_type pos = next_code_and_version.find( ' ' );

            if( pos != string::npos )
               next_code_and_version.erase( pos );

            act_cmd += " " + get_uid_info( sess_info ) + " " + current_dtm + " " + mod_info.id + " " + class_id;

            if( !sess_info.user_group.empty( ) )
               act_cmd += " -g=" + sess_info.user_group;

            if( !sess_info.tz_name.empty( ) )
               act_cmd += " -tz=" + sess_info.tz_name;

            if( !fieldlist.empty( ) )
               act_cmd += " \"-v=" + fields_and_values + "\"";

            act_cmd += " " + next_code_and_version;
         }
      }

      if( act == c_act_link )
         act_cmd += " \"" + fields_and_values + "\"";
      else if( act == c_act_exec )
         act_cmd += " " + exec_args( exec_info );

      DEBUG_TRACE( act_cmd );

      if( sess_info.p_socket->write_line( act_cmd ) <= 0 )
      {
         okay = false;
         break;
      }

      // NOTE: For a multi-record "exec" a single command (with multiple keys) is issued to the server.
      if( !key_list.empty( ) )
         break;
   }

   int timeout = c_initial_response_timeout;

   for( size_t i = 0; i < code_and_versions.size( ); i++ )
   {
      string output, response;

      while( response.empty( ) || ( response[ 0 ] != '(' ) )
      {
         if( !response.empty( ) )
            timeout = c_subsequent_response_timeout;

         response.clear( );

         if( sess_info.p_socket->read_line( response, timeout ) <= 0 )
         {
            okay = false;
            break;
         }

         // NOTE: For a multi-record "exec" the "c_response_okay_more" can occur for all but the last record.
         if( ( response != c_response_okay ) && ( response != c_response_okay_more ) )
            output += response;
      }

      if( !output.empty( ) )
         row_errors.insert( make_pair( code_and_versions[ i ].substr( 0, code_and_versions[ i ].find( ' ' ) ), output ) );

      response.erase( );
   }

   return okay;
}

bool fetch_item_info( const string& module, const module_info& mod_info,
 const string& class_id, const string& item_key, const string& field_list,
 const string& set_field_values, const string& query_info, const session_info& sess_info,
 pair< string, string >& item_info, const string* p_owner, const string* p_pdf_spec_name,
 const string* p_pdf_title, const string* p_pdf_link_filename, string* p_pdf_view_file_name,
 const map< string, string >* p_vext_items )
{
   bool okay = true;

   string fetch_cmd( string( c_cmd_fetch ) + " " + module + " " + class_id );

   string user_info( get_uid_info( sess_info, false ) );

   if( !user_info.empty( ) )
      fetch_cmd += " \"-u=" + user_info + "\"";

   fetch_cmd += " -d=" + date_time::standard( ).as_string( );

   if( !sess_info.user_group.empty( ) )
      fetch_cmd += " -g=" + sess_info.user_group;

   if( !sess_info.user_id.empty( ) )
      fetch_cmd += " -td=tmp/" + sess_info.session_id;

   if( !sess_info.tz_name.empty( ) )
      fetch_cmd += " -tz=" + sess_info.tz_name;

   string perms;

   if( sess_info.is_admin_user )
      perms = "@admin";

   if( p_owner && *p_owner == sess_info.user_key )
   {
      if( !perms.empty( ) )
         perms += ",";
      perms = "@owner";
   }

   map< string, string >::const_iterator i;
   for( i = sess_info.user_perms.begin( ); i != sess_info.user_perms.end( ); ++i )
   {
      if( !perms.empty( ) )
         perms += ",";
      perms += i->first;
   }

   if( !perms.empty( ) )
      fetch_cmd += " -p=" + perms;

   if( !query_info.empty( ) )
      fetch_cmd += " -q=" + query_info;

   if( get_storage_info( ).embed_images )
      fetch_cmd += " -x=@embed=1";

   fetch_cmd += " \"" + escaped( item_key, ",\"" ) + "\" #1";

   string field_values( set_field_values );

   if( !mod_info.user_extra1_field_id.empty( ) || !mod_info.user_extra2_field_id.empty( ) )
   {
      if( !field_values.empty( ) )
         field_values += ",";

      field_values += "@extra1="
       + escaped( escaped( sess_info.user_extra1, "," ), "\"" )
       + ",@extra2=" + escaped( escaped( sess_info.user_extra2, "," ), "\"" );
   }

   if( p_vext_items && !p_vext_items->empty( ) )
   {
      for( map< string, string >::const_iterator i = p_vext_items->begin( ); i != p_vext_items->end( ); ++i )
      {
         if( !field_values.empty( ) )
            field_values += ",";

         field_values += "@" + i->first + "=" + i->second;
      }
   }

   if( !field_values.empty( ) )
      fetch_cmd += " \"-v=" + field_values + "\"";

   if( !field_list.empty( ) )
      fetch_cmd += " " + field_list;

   if( p_pdf_spec_name )
   {
      fetch_cmd += " -pdf " + *p_pdf_spec_name + " \"";

      string link_file_name;
      if( p_pdf_link_filename )
         link_file_name = *p_pdf_link_filename;

      if( link_file_name.empty( ) )
         link_file_name = uuid( ).as_string( );

      string path( string( c_files_directory )
       + "/" + string( c_tmp_directory ) + "/" + sess_info.session_id + "/" + link_file_name + ".pdf" );

      if( p_pdf_view_file_name )
         *p_pdf_view_file_name = path;

      path = get_storage_info( ).web_root + "/" + path;

      replace( path.begin( ), path.end( ), '\\', '/' );

      fetch_cmd += path + "\"";

      if( p_pdf_title && !p_pdf_title->empty( ) )
         fetch_cmd += " \"" + *p_pdf_title + "\"";
   }

   DEBUG_TRACE( fetch_cmd );

   if( sess_info.p_socket->write_line( fetch_cmd ) <= 0 )
      okay = false;
   else
   {
      string response;

      int timeout = c_initial_response_timeout;

      while( response.empty( ) || ( response[ 0 ] != '(' ) )
      {
         if( !response.empty( ) )
            timeout = c_subsequent_response_timeout;

         response.clear( );

         if( sess_info.p_socket->read_line( response, timeout ) <= 0 )
         {
            okay = false;
            break;
         }

         if( !response.empty( ) && response[ 0 ] != '(' )
         {
            size_t pos = response.find( "]" ); //i.e. skip over key and version information

            if( pos == string::npos || pos < 2 )
               throw runtime_error( "unexpected missing key/version information" );

            string key_ver_rev_state_and_type_info = response.substr( 1, pos - 1 );

            item_info = make_pair( key_ver_rev_state_and_type_info,
             ( pos + 2 >= response.length( ) ) ? string( ) : response.substr( pos + 2 ) );
         }
         else if( response != c_response_okay )
            throw runtime_error( "unexpected server response: " + response );
      }
   }

   return okay;
}

bool fetch_list_info( const string& module,
 const module_info& mod_info, const string& class_id, const session_info& sess_info,
 bool is_reverse, int row_limit, const string& key_info, const string& field_list,
 const string& filters, const string& search_text, const string& search_query,
 const string& set_field_values, data_container& rows, const string& exclude_key_info,
 bool* p_prev, string* p_perms, const string* p_extra_debug, const set< string >* p_exclude_keys,
 const string* p_pdf_spec_name, const string* p_pdf_link_filename, string* p_pdf_view_file_name,
 bool* p_can_delete_any, bool is_printable, bool* p_any_actionable )
{
   bool okay = true;

   set< string > exclude_keys;
   if( !exclude_key_info.empty( ) )
      split( exclude_key_info, exclude_keys );

   string fetch_cmd( string( c_cmd_fetch ) + " " + module + " " + class_id );

   string extra_debug;
   if( p_extra_debug )
      extra_debug = *p_extra_debug;

   if( p_prev && *p_prev )
      is_reverse = !is_reverse;

   if( is_reverse )
      fetch_cmd += " -rev";

   string user_info( get_uid_info( sess_info, false ) );
   if( !user_info.empty( ) )
      fetch_cmd += " \"-u=" + user_info + "\"";

   fetch_cmd += " -d=" + date_time::standard( ).as_string( );

   if( !sess_info.user_group.empty( ) )
      fetch_cmd += " -g=" + sess_info.user_group;

   if( !sess_info.user_id.empty( ) )
      fetch_cmd += " -td=tmp/" + sess_info.session_id;

   if( !sess_info.tz_name.empty( ) )
      fetch_cmd += " -tz=" + sess_info.tz_name;

   if( !filters.empty( ) )
      fetch_cmd += " -f=" + filters;

   if( p_perms && !p_perms->empty( ) )
      fetch_cmd += " -p=" + *p_perms;

   if( !search_text.empty( ) )
      fetch_cmd += " \"-t=" + escaped( search_text, "\"" ) + "\"";

   if( !search_query.empty( ) )
      fetch_cmd += " \"-q=" + search_query + "\"";

   if( get_storage_info( ).embed_images || ( is_printable || p_pdf_spec_name ) )
   {
      if( ( is_printable || p_pdf_spec_name ) && get_storage_info( ).embed_images )
         fetch_cmd += " -x=@print=1,@embed=1";
      else if( is_printable || p_pdf_spec_name )
         fetch_cmd += " -x=@print=1";
      else
         fetch_cmd += " -x=@embed=1";
   }

   fetch_cmd += " \"" + key_info + "\"";

   if( row_limit > 0 )
      fetch_cmd += " #" + to_string( row_limit + ( p_pdf_spec_name ? 0 : 1 ) + ( p_prev ? ( int )*p_prev : 0 ) );

   if( !set_field_values.empty( )
    || !mod_info.user_extra1_field_id.empty( ) || !mod_info.user_extra2_field_id.empty( ) )
   {
      fetch_cmd += " \"-v=";

      if( !set_field_values.empty( ) )
         fetch_cmd += escaped( set_field_values, "\"" );

      if( !mod_info.user_extra1_field_id.empty( ) || !mod_info.user_extra2_field_id.empty( ) )
      {
         if( !set_field_values.empty( ) )
            fetch_cmd += ',';

         fetch_cmd += "@extra1="
          + escaped( escaped( sess_info.user_extra1, "," ), "\"" )
          + ",@extra2=" + escaped( escaped( sess_info.user_extra2, "," ), "\"" );
      }

      fetch_cmd += "\"";
   }

   if( !field_list.empty( ) )
      fetch_cmd += " " + field_list;

   if( p_pdf_spec_name )
   {
      fetch_cmd += " -pdf " + *p_pdf_spec_name + " \"";

      string link_file_name;
      if( p_pdf_link_filename )
         link_file_name = *p_pdf_link_filename;

      if( link_file_name.empty( ) )
         link_file_name = uuid( ).as_string( );

      string title( link_file_name );

      bool has_utf8_chars = false;
      string path( string( c_files_directory ) + "/" + string( c_tmp_directory )
       + "/" + sess_info.session_id + "/" + valid_file_name( link_file_name, &has_utf8_chars ) + ".pdf" );

      if( p_pdf_view_file_name )
         *p_pdf_view_file_name = path;

      path = get_storage_info( ).web_root + "/" + path;

      replace( path.begin( ), path.end( ), '\\', '/' );

      fetch_cmd += path + "\" \"" + title + "\"";
   }

   DEBUG_TRACE( extra_debug + fetch_cmd );

   if( sess_info.p_socket->write_line( fetch_cmd ) <= 0 )
      okay = false;
   else
   {
      string response;

      int timeout = c_initial_response_timeout;

      while( response.empty( ) || response[ 0 ] != '(' )
      {
         if( !response.empty( ) )
            timeout = c_subsequent_response_timeout;

         response.clear( );

         if( sess_info.p_socket->read_line( response, timeout ) <= 0 )
         {
            okay = false;
            break;
         }

         if( response[ 0 ] != '(' )
         {
            size_t pos = response.find( field_list.empty( ) ? "]" : "] " ); //i.e. skip over key and version information

            if( pos == string::npos || pos < 2 )
               throw runtime_error( "unexpected missing key/version information" );

            string key_ver_rev_state_and_type_info = response.substr( 1, pos - 1 );
            string key( key_ver_rev_state_and_type_info.substr( 0, key_ver_rev_state_and_type_info.find( ' ' ) ) );

            if( exclude_keys.count( key ) || ( p_exclude_keys && p_exclude_keys->count( key ) ) )
               continue;

            if( ( p_can_delete_any && !( *p_can_delete_any ) )
             || ( p_any_actionable && !( *p_any_actionable ) ) )
            {
               uint64_t state;
               string type_info;
               string key_and_version;

               parse_key_ver_rev_state_and_type_info( key_ver_rev_state_and_type_info, key_and_version, state, type_info );

               if( p_can_delete_any && !( state & c_state_undeletable ) )
                  *p_can_delete_any = true;

               if( p_any_actionable && !( state & c_state_unactionable ) )
                  *p_any_actionable = true;
            }

            if( !p_prev || !*p_prev )
               rows.push_back( make_pair( key_ver_rev_state_and_type_info,
                field_list.empty( ) ? string( ) : response.substr( pos + 2 ) ) );
            else
               rows.push_front( make_pair( key_ver_rev_state_and_type_info,
                field_list.empty( ) ? string( ) : response.substr( pos + 2 ) ) );
         }
         else if( response.length( ) > strlen( c_response_error_prefix )
          && response.substr( 0, strlen( c_response_error_prefix ) ) == c_response_error_prefix )
            throw runtime_error( response.substr( strlen( c_response_error_prefix ) ) );
      }
   }

   if( p_prev )
   {
      if( *p_prev && row_limit && rows.size( ) > ( size_t )( row_limit + 1 ) )
         rows.pop_front( );
      else
         *p_prev = false;
   }

   return okay;
}

bool fetch_parent_row_data( const string& module,
 const module_info& mod_info, const string& record_key, const string& field_id,
 const string& pclass_id, const string& parent_field, const string& parent_extras,
 const session_info& sess_info, const string& parent_key, data_container& parent_row_data,
 const values* p_key_values, const values* p_fkey_values, const values* p_skey_values,
 bool *p_has_view_id, const set< string >* p_exclude_keys, string* p_skey_required )
{
   bool okay = true;
   int num_fixed = 0;

   string extra_debug( "[field_id: " + field_id + "] " );

   string values, key_info, view_id_field;
   string pfield( parent_field );

   string filters( "*" );

   string perms;
   string* p_perms = 0;

   string exclude_key_info;

   if( p_has_view_id )
      *p_has_view_id = false;

   string user_other;

   if( !sess_info.other_aliases.count( sess_info.user_other ) )
      user_other = sess_info.user_other;
   else
      user_other = sess_info.other_aliases.find( sess_info.user_other )->second;

   string user_slevel;

   if( !sess_info.other_slevels.count( sess_info.user_other ) )
      user_slevel = sess_info.user_slevel;
   else
      user_slevel = sess_info.other_slevels.find( sess_info.user_other )->second;

   vector< string > extras;

   if( !parent_extras.empty( ) )
      split( parent_extras, extras, '+' );

   bool sort_manually = false;
   bool needs_decrypting = false;
   bool remove_manual_links = false;

   for( size_t i = 0; i < extras.size( ); i++ )
   {
      string::size_type pos = extras[ i ].find( '=' );

      if( pos != string::npos )
      {
         ++num_fixed;

         if( !key_info.empty( ) )
         {
            values += ",";
            key_info += ",";
         }

         values += extras[ i ].substr( pos + 1 );
         key_info += extras[ i ].substr( 0, pos );
      }
      else
      {
         pos = extras[ i ].find( ':' );
         string key( extras[ i ].substr( 0, pos ) );

         if( !key.empty( ) && ( key[ 0 ] == '@' || key[ 0 ] == '?' ) )
         {
            bool is_optional = false;
            if( key[ 0 ] == '?' )
            {
               key.erase( 0, 1 );
               is_optional = true;
            }

            string data;

            if( pos != string::npos )
               data = extras[ i ].substr( pos + 1 );

            string value;

            bool found_special = false;

            if( key == c_parent_extra_key )
            {
               value = record_key;
               found_special = true;
            }
            else if( key == c_parent_extra_null )
            {
               value.erase( );
               found_special = true;
            }
            else if( key == c_parent_extra_user )
            {
               value = sess_info.user_key;
               found_special = true;
            }
            else if( key == c_parent_extra_key0 )
            {
               if( p_key_values )
                  value = p_key_values->value0;
               found_special = true;
            }
            else if( key == c_parent_extra_key1 )
            {
               if( p_key_values )
                  value = p_key_values->value1;
               found_special = true;
            }
            else if( key == c_parent_extra_key2 )
            {
               if( p_key_values )
                  value = p_key_values->value2;
               found_special = true;
            }
            else if( key == c_parent_extra_key3 )
            {
               if( p_key_values )
                  value = p_key_values->value3;
               found_special = true;
            }
            else if( key == c_parent_extra_key4 )
            {
               if( p_key_values )
                  value = p_key_values->value4;
               found_special = true;
            }
            else if( key == c_parent_extra_key5 )
            {
               if( p_key_values )
                  value = p_key_values->value5;
               found_special = true;
            }
            else if( key == c_parent_extra_key6 )
            {
               if( p_key_values )
                  value = p_key_values->value6;
               found_special = true;
            }
            else if( key == c_parent_extra_key7 )
            {
               if( p_key_values )
                  value = p_key_values->value7;
               found_special = true;
            }
            else if( key == c_parent_extra_key8 )
            {
               if( p_key_values )
                  value = p_key_values->value8;
               found_special = true;
            }
            else if( key == c_parent_extra_key9 )
            {
               if( p_key_values )
                  value = p_key_values->value9;
               found_special = true;
            }
            else if( key == c_parent_extra_fkey0 )
            {
               if( p_fkey_values )
                  value = p_fkey_values->value0;
               found_special = true;
            }
            else if( key == c_parent_extra_fkey1 )
            {
               if( p_fkey_values )
                  value = p_fkey_values->value1;
               found_special = true;
            }
            else if( key == c_parent_extra_fkey2 )
            {
               if( p_fkey_values )
                  value = p_fkey_values->value2;
               found_special = true;
            }
            else if( key == c_parent_extra_fkey3 )
            {
               if( p_fkey_values )
                  value = p_fkey_values->value3;
               found_special = true;
            }
            else if( key == c_parent_extra_fkey4 )
            {
               if( p_fkey_values )
                  value = p_fkey_values->value4;
               found_special = true;
            }
            else if( key == c_parent_extra_fkey5 )
            {
               if( p_fkey_values )
                  value = p_fkey_values->value5;
               found_special = true;
            }
            else if( key == c_parent_extra_fkey6 )
            {
               if( p_fkey_values )
                  value = p_fkey_values->value6;
               found_special = true;
            }
            else if( key == c_parent_extra_fkey7 )
            {
               if( p_fkey_values )
                  value = p_fkey_values->value7;
               found_special = true;
            }
            else if( key == c_parent_extra_fkey8 )
            {
               if( p_fkey_values )
                  value = p_fkey_values->value8;
               found_special = true;
            }
            else if( key == c_parent_extra_fkey9 )
            {
               if( p_fkey_values )
                  value = p_fkey_values->value9;
               found_special = true;
            }
            else if( key == c_parent_extra_skey0 )
            {
               if( p_skey_values )
                  value = p_skey_values->value0;
               if( p_skey_required )
                  *p_skey_required = c_parent_extra_skey0;
               found_special = true;
            }
            else if( key == c_parent_extra_skey1 )
            {
               if( p_skey_values )
                  value = p_skey_values->value1;
               if( p_skey_required )
                  *p_skey_required = c_parent_extra_skey1;
               found_special = true;
            }
            else if( key == c_parent_extra_skey2 )
            {
               if( p_skey_values )
                  value = p_skey_values->value2;
               if( p_skey_required )
                  *p_skey_required = c_parent_extra_skey2;
               found_special = true;
            }
            else if( key == c_parent_extra_skey3 )
            {
               if( p_skey_values )
                  value = p_skey_values->value3;
               if( p_skey_required )
                  *p_skey_required = c_parent_extra_skey3;
               found_special = true;
            }
            else if( key == c_parent_extra_skey4 )
            {
               if( p_skey_values )
                  value = p_skey_values->value4;
               if( p_skey_required )
                  *p_skey_required = c_parent_extra_skey4;
               found_special = true;
            }
            else if( key == c_parent_extra_skey5 )
            {
               if( p_skey_values )
                  value = p_skey_values->value5;
               if( p_skey_required )
                  *p_skey_required = c_parent_extra_skey5;
               found_special = true;
            }
            else if( key == c_parent_extra_skey6 )
            {
               if( p_skey_values )
                  value = p_skey_values->value6;
               if( p_skey_required )
                  *p_skey_required = c_parent_extra_skey6;
               found_special = true;
            }
            else if( key == c_parent_extra_skey7 )
            {
               if( p_skey_values )
                  value = p_skey_values->value7;
               if( p_skey_required )
                  *p_skey_required = c_parent_extra_skey7;
               found_special = true;
            }
            else if( key == c_parent_extra_skey8 )
            {
               if( p_skey_values )
                  value = p_skey_values->value8;
               if( p_skey_required )
                  *p_skey_required = c_parent_extra_skey8;
               found_special = true;
            }
            else if( key == c_parent_extra_skey9 )
            {
               if( p_skey_values )
                  value = p_skey_values->value9;
               if( p_skey_required )
                  *p_skey_required = c_parent_extra_skey9;
               found_special = true;
            }
            else if( key == c_parent_extra_extkey )
            {
               string::size_type pos = data.find( '#' );
               if( pos != string::npos )
               {
                  value = get_extkey( data.substr( pos + 1 ).c_str( ) );
                  data.erase( pos );
               }
               found_special = true;
            }
            else if( key == c_parent_extra_group )
            {
               if( is_optional && sess_info.user_group.empty( ) )
                  continue;

               value = sess_info.user_group;
               found_special = true;
            }
            else if( key == c_parent_extra_other )
            {
               if( is_optional && user_other.empty( ) )
                  continue;

               value = user_other;
               found_special = true;
            }
            else if( key == c_parent_extra_parent )
            {
               // NOTE: If no field was explicitly provided then assume a self relationship.
               if( data.empty( ) )
                  data = field_id;

               value = parent_key;
               found_special = true;
            }
            else if( key == c_parent_extra_xself )
            {
               exclude_key_info = sess_info.user_key;
               continue;
            }
            else if( key == c_parent_extra_xfkey0 )
            {
               if( p_fkey_values )
                  exclude_key_info = p_fkey_values->value0;
               continue;
            }
            else if( key == c_parent_extra_xfkey1 )
            {
               if( p_fkey_values )
                  exclude_key_info = p_fkey_values->value1;
               continue;
            }
            else if( key == c_parent_extra_xfkey2 )
            {
               if( p_fkey_values )
                  exclude_key_info = p_fkey_values->value2;
               continue;
            }
            else if( key == c_parent_extra_xfkey3 )
            {
               if( p_fkey_values )
                  exclude_key_info = p_fkey_values->value3;
               continue;
            }
            else if( key == c_parent_extra_xfkey4 )
            {
               if( p_fkey_values )
                  exclude_key_info = p_fkey_values->value4;
               continue;
            }
            else if( key == c_parent_extra_xfkey5 )
            {
               if( p_fkey_values )
                  exclude_key_info = p_fkey_values->value5;
               continue;
            }
            else if( key == c_parent_extra_xfkey6 )
            {
               if( p_fkey_values )
                  exclude_key_info = p_fkey_values->value6;
               continue;
            }
            else if( key == c_parent_extra_xfkey7 )
            {
               if( p_fkey_values )
                  exclude_key_info = p_fkey_values->value7;
               continue;
            }
            else if( key == c_parent_extra_xfkey8 )
            {
               if( p_fkey_values )
                  exclude_key_info = p_fkey_values->value8;
               continue;
            }
            else if( key == c_parent_extra_xfkey9 )
            {
               if( p_fkey_values )
                  exclude_key_info = p_fkey_values->value9;
               continue;
            }
            else if( key == c_parent_extra_xparent )
            {
               exclude_key_info = parent_key;
               continue;
            }
            else if( key == c_parent_extra_manuallink )
               continue;
            else if( key == c_parent_extra_permission )
            {
               map< string, string >::const_iterator i;
               for( i = sess_info.user_perms.begin( ); i != sess_info.user_perms.end( ); ++i )
               {
                  if( !perms.empty( ) )
                     perms += ",";
                  perms += i->first;
               }
               p_perms = &perms;

               continue;
            }
            else if( key == c_parent_extra_xselfadmin )
            {
               exclude_key_info = sess_info.user_key + "," + to_string( c_admin_user_key );
               continue;
            }
            else if( key == c_parent_extra_decrypt )
            {
               needs_decrypting = true;
               continue;
            }
            else if( key == c_parent_extra_sort || key == c_parent_extra_sortlinks )
            {
               sort_manually = true;
               if( key == c_parent_extra_sortlinks )
                  remove_manual_links = true;
               continue;
            }
            else if( key == c_parent_extra_view )
            {
               if( p_has_view_id )
                  *p_has_view_id = true;

               view_id_field = data;
               continue;
            }

            if( !found_special )
               throw runtime_error( "unknown special parent extra '" + key + "'" );
            else
            {
               ++num_fixed;

               if( !key_info.empty( ) )
               {
                  values += ",";
                  key_info += ",";
               }

               values += value;
               key_info += data;
            }
         }
         else
         {
            if( !key_info.empty( ) )
               key_info += ",";
            key_info += extras[ i ];
         }
      }
   }

   if( pfield == c_key_field )
      pfield.erase( );
   else if( !sort_manually )
   {
      if( !key_info.empty( ) )
         key_info += ",";
      key_info += pfield;
   }

   if( num_fixed )
      key_info += "#" + string( 1, '0' + num_fixed );

   if( !key_info.empty( ) )
      key_info += " ";

   key_info += values;

   if( !view_id_field.empty( ) )
   {
      if( !pfield.empty( ) )
         pfield += ",";
      pfield += view_id_field;
   }

   okay = fetch_list_info( module, mod_info, pclass_id, sess_info, false, 0, key_info, pfield,
    filters, "", "", "", parent_row_data, exclude_key_info, 0, p_perms, &extra_debug, p_exclude_keys );

   if( needs_decrypting )
   {
      string sid;
      get_server_sid( sid );

      for( size_t i = 0; i < parent_row_data.size( ); i++ )
      {
         if( parent_row_data[ i ].second.empty( ) )
            continue;

         parent_row_data[ i ].second = data_decrypt( parent_row_data[ i ].second, sid );
      }

      clear_key( sid );
   }

   if( sort_manually )
      sort_row_data_manually( parent_row_data, remove_manual_links );

   return okay;
}

bool populate_list_info( list_source& list,
 const map< string, string >& list_selections,
 const map< string, string >& list_search_text,
 const map< string, string >& list_search_values,
 const string& listinfo, const string& listsort, const string& parent_key, bool is_printable,
 const view_source* p_view, const string& view_pfield, const set< string >* p_specials,
 const session_info& sess_info, const string* p_pdf_spec_name, const string* p_pdf_link_filename,
 string* p_pdf_view_file_name, uint64_t* p_parent_state )
{
   bool okay = true;

   int row_limit( sess_info.row_limit );

   if( is_printable )
      row_limit = sess_info.print_limit;
   else
   {
      if( ( list.lici->second )->extras.count( c_list_type_extra_limit ) )
         row_limit = atoi( ( list.lici->second )->extras.find( c_list_type_extra_limit )->second.c_str( ) );
      else if( ( list.lici->second )->extras.count( c_list_type_extra_no_limit )
       || ( list.lici->second )->extras.count( c_list_type_extra_sort_no_limit ) )
         row_limit = sess_info.print_limit; // NOTE: The maximum print limit is used as the upper limit.
   }

   bool sort_manually = false;

   if( ( list.lici->second )->extras.count( c_list_type_extra_sort_no_limit ) )
      sort_manually = true;

   string search_query;

   const module_info& mod_info( *get_storage_info( ).modules_index.find( list.module )->second );

   // NOTE: If query search values were specified then construct a search query string from them.
   if( !list_search_values.empty( ) )
   {
      map< string, string > query_values;
      map< string, string >::const_iterator ci, end;

      for( ci = list_search_values.begin( ), end = list_search_values.end( ); ci != end; ++ci )
      {
         string svname( ci->first );

         string field_id( svname.substr( 1 ) );
         string field_value( escaped( ci->second ) );

         if( p_specials )
         {
            // NOTE: If an "skey" owner was changed then ignore any existing restrictions for
            // the "skey" users (as these values are not applicable to the new "skey" owner).
            bool found_special = false;

            for( set< string >::const_iterator ci = p_specials->begin( ); ci != p_specials->end( ); ++ci )
            {
               for( size_t i = 0; i < ( list.lici->second )->parents.size( ); i++ )
               {
                  if( field_id == ( list.lici->second )->parents[ i ].field
                   && ( list.lici->second )->parents[ i ].pextra.find( *ci ) != string::npos )
                  {
                     found_special = true;
                     break;
                  }
               }

               if( found_special )
                  break;
            }

            if( found_special )
               continue;
         }

         // NOTE: Value prefix 'L' is used for a "contains" search for a string field.
         if( svname[ 0 ] == 'L' )
            field_value = '*' + replaced( escaped( field_value, "\"" ), ",", "\\\\\\," ) + '*';

         // NOTE: Value prefixes 'T' and 'U' are used for "datetime" values.
         if( !field_value.empty( ) && ( svname[ 0 ] >= 'T' ) && ( svname[ 0 ] <= 'U' ) )
         {
            date_time dt( field_value );

            dt -= ( seconds )sess_info.gmt_offset;

            field_value = format_date_time( dt );
         }

         // NOTE: For optional selects "~" is being used to indicate null.
         if( field_value == "~" && svname[ 0 ] >= 'A' && svname[ 0 ] <= 'G' )
            field_value.erase( );

         if( query_values.count( field_id ) )
         {
            string query_value( query_values[ field_id ] );

            if( svname[ 0 ] >= 'A' && svname[ 0 ] <= 'G' )
               query_value += '|';
            else if( svname[ 0 ] >= 'H' && svname[ 0 ] <= 'M' )
               query_value += '&';
            else
               query_value += "..";

            query_value += field_value;

            query_values[ field_id ] = query_value;
         }
         else
            query_values.insert( make_pair( field_id, field_value ) );
      }

      for( ci = query_values.begin( ), end = query_values.end( ); ci != end; ++ci )
      {
         if( !search_query.empty( ) )
            search_query += ',';

         search_query += ci->first + ":" + ci->second;
      }
   }

   string search_text;

   if( !list_search_text.empty( ) )
   {
      string name( list.id );

      name += c_srch_suffix;

      if( list_search_text.count( name ) )
         search_text = list_search_text.find( name )->second;
   }

   bool is_reverse( list.lici->second->order == c_order_reverse );

   string fixed_fields;
   string fixed_key_values;

   int num_fixed_key_values = 0;

   // NOTE: A "user child" list needs to constrain the parent field as just another
   // fixed field as the "user" context is needed as the parent context for the fetch.
   string fixed_parent_field, fixed_parent_keyval;

   if( !view_pfield.empty( ) && list.type == c_list_type_user_child )
   {
      fixed_parent_field = view_pfield;
      fixed_parent_keyval = parent_key;
   }

   string set_field_values;

   determine_fixed_query_info( fixed_fields, fixed_key_values, num_fixed_key_values,
    is_reverse, list, fixed_parent_field, fixed_parent_keyval, list_selections, sess_info,
    &set_field_values, p_parent_state );

   if( p_view && ( !p_view->vextra1_id.empty( ) || !p_view->vextra2_id.empty( ) ) )
   {
      if( !p_view->vextra1_id.empty( ) )
      {
         if( !set_field_values.empty( ) )
            set_field_values += ',';

         set_field_values += "@vextra1=" + p_view->vextra1_value;
      }

      if( !p_view->vextra2_id.empty( ) )
      {
         if( !set_field_values.empty( ) )
            set_field_values += ',';

         set_field_values += "@vextra2=" + p_view->vextra2_value;
      }
   }

   if( listsort.size( ) == 2 )
   {
      int sort_field = atoi( listsort.substr( 0, 1 ).c_str( ) );

      if( sort_field > 0 && sort_field <= ( int )list.index_fields.size( ) )
      {
         if( listsort[ 1 ] == 'R' )
            is_reverse = true;
         else
            is_reverse = false;

         list.all_index_fields = list.index_fields[ sort_field - 1 ].first;

         string::size_type pos = list.all_index_fields.find( ',' );

         list.unique_index = list.index_fields[ sort_field - 1 ].second;
         list.first_index_field = list.all_index_fields.substr( 0, pos );

         list.sort_field = sort_field;
      }
      else
         throw runtime_error( "unexpected out of range sort field" );
   }
   else
   {
      // NOTE: If ordered then assume that the first indexed column has been used
      // (if "ordered" then no index field information exists so just use the field).
      if( !list.first_index_field.empty( ) )
      {
         if( list.index_fields.empty( ) )
            list.all_index_fields = list.first_index_field;
         else
            list.all_index_fields = list.index_fields[ 0 ].first;
      }
   }

   list.is_reverse = is_reverse;

   string key_prefix( list.all_index_fields );

   // NOTE: If creating a printable version and any print summaries are present
   // then make sure they are added to the key info for query ordering purposes.
   if( is_printable )
   {
      key_prefix.erase( );

      for( size_t i = 0; i < list.print_summary_field_ids.size( ); i++ )
      {
         if( i > 0 || !key_prefix.empty( ) )
            key_prefix += ",";

         key_prefix += list.print_summary_field_ids[ i ];
      }

      if( !key_prefix.empty( ) && !list.all_index_fields.empty( ) )
         key_prefix += ",";

      key_prefix += list.all_index_fields;
   }

   if( !fixed_fields.empty( ) )
   {
      if( key_prefix.empty( ) )
         key_prefix = fixed_fields;
      else
         key_prefix = fixed_fields + "," + key_prefix;
   }

   if( num_fixed_key_values )
   {
      key_prefix += '#';

      key_prefix += ( '0' + num_fixed_key_values );

      key_prefix += " " + fixed_key_values;
   }
   else if( !list.first_index_field.empty( ) || ( is_printable && !list.print_summary_field_ids.empty( ) ) )
      key_prefix += " ";

   string key_info( key_prefix );
   string class_info( list.cid );

   if( p_view && !p_view->cid.empty( ) && list.type != c_list_type_user_child )
   {
      class_info = p_view->cid;

      class_info += ":_" + list.new_pfield;

      key_info = parent_key + ":" + key_info;
   }

   string field_list( is_printable ? list.pfield_list : list.field_list );

   // NOTE: A "group" list is a parent qualified list where the parent is the nominated "group" class whilst a
   // "user" list (and its variants) are parent qualified lists where the parent is the nominated "user" class.
   if( list.type == c_list_type_group || list.type == c_list_type_nongroup )
   {
      if( list.type == c_list_type_nongroup )
         key_info = ":" + key_prefix;
      else
         key_info = sess_info.user_group + ":" + key_prefix;

      class_info = ( list.lici->second )->pclass;

      class_info += ":_" + ( list.lici->second )->pfield;

      if( !field_list.empty( ) )
         field_list += ",";

      field_list += ( list.lici->second )->pfield;
   }
   else if( list.type == c_list_type_user || list.type == c_list_type_nonuser || list.type == c_list_type_user_child )
   {
      if( list.type == c_list_type_nonuser )
         key_info = ":" + key_prefix;
      else
         key_info = sess_info.user_key + ":" + key_prefix;

      class_info = mod_info.user_class_id;

      if( list.type != c_list_type_user_child )
      {
         class_info += ":_" + ( list.lici->second )->pfield;

         if( !field_list.empty( ) )
            field_list += ",";

         field_list += ( list.lici->second )->pfield;
      }
      else
      {
         class_info += ":_" + ( list.lici->second )->ufield;

         if( !field_list.empty( ) )
            field_list += ",";

         field_list += ( list.lici->second )->ufield;
      }
   }

   string old_key_info( key_info );

   bool next = false;
   bool prev = false;

   if( !listinfo.empty( ) )
   {
      if( listinfo[ 0 ] == 'N' )
         next = true;
      else if( listinfo[ 0 ] == 'P' )
         prev = true;

      if( next || prev )
      {
         if( num_fixed_key_values )
            key_info += ",";

         key_info += listinfo.substr( 1 );
      }
   }

   bool was_prev = prev;

   string filters( list.lici->second->filters );

   string perms;

   map< string, string >::const_iterator i;

   for( i = sess_info.user_perms.begin( ); i != sess_info.user_perms.end( ); ++i )
   {
      if( !perms.empty( ) )
         perms += ",";

      perms += i->first;
   }

   list.print_limited = false;
   list.can_delete_any = false;

   if( !fetch_list_info( list.module_id, mod_info, class_info,
    sess_info, is_reverse, row_limit, key_info, field_list, filters,
    search_text, search_query, set_field_values, list.row_data, "", &prev, &perms, 0, 0,
    p_pdf_spec_name, p_pdf_link_filename, p_pdf_view_file_name, &list.can_delete_any, is_printable, &list.can_action_any ) )
      okay = false;
   else if( is_printable )
   {
      if( row_limit && list.row_data.size( ) == row_limit + 1 )
      {
         list.row_data.pop_back( );
         list.print_limited = true;
      }
   }
   else
   {
      bool redo_fetch = false;

      // NOTE: If we have just processed a "next" but found no rows then instead process
      // as though it was a "prev" in order to make sure rows are displayed if possible.
      if( !listinfo.empty( ) && ( listinfo[ 0 ] == 'N' ) && list.row_data.empty( ) )
      {
         prev = true;
         next = false;

         redo_fetch = true;
      }

      // NOTE: If we have just processed a "prev" but found only the limit or less rows
      // then instead process as though it were a "first" so that the possibly the full
      // limit number of rows and a "next" can be displayed.
      if( was_prev && row_limit && ( list.row_data.size( ) <= ( size_t )row_limit ) )
      {
         prev = false;

         redo_fetch = true;

         list.row_data.clear( );

         key_info = old_key_info;
      }

      if( redo_fetch && !fetch_list_info( list.module_id, mod_info,
       class_info, sess_info, is_reverse, row_limit, key_info, list.field_list,
       filters, search_text, search_query, set_field_values, list.row_data, "", &prev, &perms ) )
         okay = false;

      size_t index_field = 0;
      size_t index_count = 0;

      for( size_t i = 0; i < list.field_ids.size( ); i++ )
      {
         if( list.field_ids[ i ] == list.first_index_field )
         {
            index_field = i + 1;

            vector< string > all_index_fields;
            index_count = split( list.all_index_fields, all_index_fields );

            break;
         }
      }

      if( okay )
      {
         // NOTE: If we have just processed a "next" or if "prev" was set true in "fetch_list_info"
         // to indicate there are previous rows then store the first row's key/data for "prev" link.
         if( row_limit && list.row_data.size( ) && !listinfo.empty( ) && ( next || prev ) )
         {
            string key_ver_and_state( list.row_data.front( ).first );
            string key( key_ver_and_state.substr( 0, key_ver_and_state.find( ' ' ) ) );

            if( !index_field )
               list.prev_key_info = key;
            else
            {
               vector< string > columns;
               raw_split( list.row_data.front( ).second, columns );

               list.prev_key_info.erase( );

               // NOTE: Need to escape the "indexed" values twice due to commas being significant
               // during the processing of the protocol command itself (i.e. as a list separator)
               // as well as within the implementation of the fetch command.
               for( size_t i = 0; i < index_count; i++ )
               {
                  if( i > 0 )
                     list.prev_key_info += ",";

                  list.prev_key_info += escaped( escaped( columns[ index_field - 1 + i ], "," ) );
               }

               if( !list.unique_index )
                  list.prev_key_info += "," + key;
            }
         }

         // NOTE: If one extra row was found then store the last row's key/data for "next" link.
         if( row_limit && ( list.row_data.size( ) == ( row_limit + 1 ) ) )
         {
            string key_ver_and_state( list.row_data.back( ).first );
            string key( key_ver_and_state.substr( 0, key_ver_and_state.find( ' ' ) ) );

            if( !index_field )
               list.next_key_info = key;
            else
            {
               vector< string > columns;
               raw_split( list.row_data.back( ).second, columns );

               list.next_key_info.erase( );

               // NOTE: Need to escape the "indexed" values twice due to commas being significant
               // during the processing of the protocol command itself (i.e. as a list separator)
               // as well as within the implementation of the fetch command.
               for( size_t i = 0; i < index_count; i++ )
               {
                  if( i > 0 )
                     list.next_key_info += ",";

                  list.next_key_info += escaped( escaped( columns[ index_field - 1 + i ], "," ) );
               }

               if( !list.unique_index )
                  list.next_key_info += "," + key;
            }

            list.row_data.pop_back( );
         }

         bool has_ignore_encrypted_field = false;

         size_t ignore_encryped_field_column = 0;

         vector< size_t > encrypted_columns;

         for( size_t i = 0; i < list.value_ids.size( ); i++ )
         {
            if( list.encrypted_fields.count( list.value_ids[ i ] ) )
               encrypted_columns.push_back( i );

            if( list.ignore_encrypted_field == list.value_ids[ i ] )
            {
               ignore_encryped_field_column = i;
               has_ignore_encrypted_field = true;
            }
         }

         // NOTE: Any encrypted columns are now decrypted. This is being done
         // before manual sorting (as they clearly might affect the sorting).
         if( !encrypted_columns.empty( ) )
         {
            string sid;

            get_server_sid( sid );

            for( size_t i = 0; i < list.row_data.size( ); i++ )
            {
               vector< string > columns;

               raw_split( list.row_data[ i ].second, columns );

               if( !has_ignore_encrypted_field
                || ( columns[ ignore_encryped_field_column ] != string( c_true_value ) ) )
               {
                  for( size_t j = 0; j < encrypted_columns.size( ); j++ )
                  {
                     if( columns[ encrypted_columns[ j ] ].empty( ) )
                        continue;

                     columns[ encrypted_columns[ j ] ]
                      = data_decrypt( columns[ encrypted_columns[ j ] ], sid );
                  }

                  list.row_data[ i ].second = join( columns );
               }
            }

            clear_key( sid );
         }

         if( sort_manually )
            sort_row_data_manually( list.row_data );

         if( !( list.lici->second )->dfield.empty( ) )
         {
            if( ( list.lici->second )->nclass.empty( ) )
            {
               if( ( list.lici->second )->dvalue.empty( ) )
               {
                  const enum_info& info( get_storage_info( ).enums.find( ( list.lici->second )->dfenum )->second );

                  data_container new_record_list;

                  for( size_t i = 0; i < info.values.size( ); i++ )
                  {
                     // NOTE: Enum values that start with a '-' are not included for user selection
                     // as they are deemed as being only available for internal application purposes.
                     if( info.values[ i ].first[ 0 ] == '-' )
                        continue;

                     new_record_list.push_back( make_pair(
                      info.values[ i ].first, get_display_string( info.values[ i ].second ) ) );
                  }

                  list.new_record_list = new_record_list;
               }
            }
            else
            {
               data_container new_record_list;

               if( !fetch_parent_row_data( list.module_id,
                 mod_info, "", "", ( list.lici->second )->nclass,
                ( list.lici->second )->nfield, ( list.lici->second )->nextra, sess_info, "",
                new_record_list, 0, 0, 0, &list.new_record_list_has_view_id ) )
                  okay = false;
               else
                  list.new_record_list = new_record_list;
            }
         }

         if( okay )
         {
            values skey_values;

            for( size_t i = 0; i < ( list.lici->second )->parents.size( ); i++ )
            {
               data_container parent_row_data;

               if( !( list.lici->second )->parents[ i ].operations.count( c_operation_restricted ) )
               {
                  if( ( list.lici->second )->parents[ i ].operations.count( c_operation_select ) )
                  {
                     string suffix( ( list.lici->second )->parents[ i ].operations[ c_operation_select ] );

                     string::size_type pos = suffix.find( '!' );
                     string::size_type npos = suffix.substr( 0, pos ).find( '#' );

                     if( npos != 0 && pos != string::npos )
                     {
                        string special( "@" + suffix.substr( 0, npos == string::npos ? pos : npos ) );

                        string* p_value = 0;

                        if( special == c_parent_extra_skey0 )
                           p_value = &skey_values.value0;
                        else if( special == c_parent_extra_skey1 )
                           p_value = &skey_values.value1;
                        else if( special == c_parent_extra_skey2 )
                           p_value = &skey_values.value2;
                        else if( special == c_parent_extra_skey3 )
                           p_value = &skey_values.value3;
                        else if( special == c_parent_extra_skey4 )
                           p_value = &skey_values.value4;
                        else if( special == c_parent_extra_skey5 )
                           p_value = &skey_values.value5;
                        else if( special == c_parent_extra_skey6 )
                           p_value = &skey_values.value6;
                        else if( special == c_parent_extra_skey7 )
                           p_value = &skey_values.value7;
                        else if( special == c_parent_extra_skey8 )
                           p_value = &skey_values.value8;
                        else if( special == c_parent_extra_skey9 )
                           p_value = &skey_values.value9;
                        else
                           throw runtime_error( "unexpected select special '" + special.substr( 1 ) + "' found" );

                        string svname( "A" );

                        svname += ( list.lici->second )->parents[ i ].field;

                        if( list_search_values.count( svname ) )
                        {
                           *p_value = list_search_values.find( svname )->second;

                           // NOTE: For optional selects "~" is being used to indicate null.
                           if( *p_value == "~" )
                              *p_value = "";
                        }
                        else
                        {
                           string sel_id( list.id );

                           sel_id += c_prnt_suffix;
                           sel_id += ( '0' + i );

                           if( list_selections.count( sel_id ) )
                              *p_value = list_selections.find( sel_id )->second;
                        }
                     }
                  }

                  if( !fetch_parent_row_data( list.module_id,
                   mod_info, "", ( list.lici->second )->parents[ i ].field,
                   ( list.lici->second )->parents[ i ].pclass, ( list.lici->second )->parents[ i ].pfield,
                   ( list.lici->second )->parents[ i ].pextra, sess_info, parent_key, parent_row_data,
                   0, 0, &skey_values, 0, &( list.lici->second )->parents[ i ].exclude_keys,
                   &( list.lici->second )->parents[ i ].skey ) )
                  {
                     okay = false;
                     break;
                  }
               }

               list.parent_lists.push_back( parent_row_data );
            }
         }
      }
   }

   return okay;
}

void fetch_sys_record( const string& module_id, const module_info& mod_info, session_info& sess_info )
{
   string field_list( mod_info.sys_name_field_id );
   field_list += "," + mod_info.sys_vendor_field_id;
   field_list += "," + mod_info.sys_actions_field_id;
   field_list += "," + mod_info.sys_message_field_id;
   field_list += "," + mod_info.sys_identity_field_id;

   // NOTE: It is being assumed that the System Information singleton's key is "system".
   string key_info( "system" );

   pair< string, string > sys_info;

   if( !fetch_item_info( module_id, mod_info,
    mod_info.sys_class_id, key_info, field_list, "", "", sess_info, sys_info ) )
      throw runtime_error( "unexpected error occurred fetching system information" );

   vector< string > sys_data;
   split( sys_info.second, sys_data );

   if( sys_data.size( ) < 5 )
      throw runtime_error( "unexpected missing sys information" );

   sess_info.sys_message = sys_data[ 3 ];
}

bool fetch_user_record(
 const string& gid, const string& module_id,
 const string& module_name, const module_info& mod_info,
 session_info& sess_info, bool is_authorised, bool check_password,
 string& username, const string& userhash, const string& password, const string& unique_data )
{
   string field_list( mod_info.user_uid_field_id );

   field_list += "," + mod_info.user_pwd_field_id;

   if( !mod_info.user_hdl_field_id.empty( ) )
      field_list += "," + mod_info.user_hdl_field_id;

   if( !mod_info.user_hash_field_id.empty( ) )
      field_list += "," + mod_info.user_hash_field_id;

   if( !mod_info.user_name_field_id.empty( ) )
      field_list += "," + mod_info.user_name_field_id;

   if( !mod_info.user_perm_field_id.empty( ) )
      field_list += "," + mod_info.user_perm_field_id;

   if( !mod_info.user_extra1_field_id.empty( ) )
      field_list += "," + mod_info.user_extra1_field_id;

   if( !mod_info.user_extra2_field_id.empty( ) )
      field_list += "," + mod_info.user_extra2_field_id;

   if( !mod_info.user_crypt_field_id.empty( ) )
      field_list += "," + mod_info.user_crypt_field_id;

   if( !mod_info.user_group_field_id.empty( ) )
      field_list += "," + mod_info.user_group_field_id;

   if( !mod_info.user_other_field_id.empty( ) )
      field_list += "," + mod_info.user_other_field_id;

   if( !mod_info.user_parent_field_id.empty( ) )
      field_list += "," + mod_info.user_parent_field_id;

   if( !mod_info.user_active_field_id.empty( ) )
      field_list += "," + mod_info.user_active_field_id;

   if( !mod_info.user_slevel_field_id.empty( ) )
      field_list += "," + mod_info.user_slevel_field_id;

   if( !mod_info.user_tz_name_field_id.empty( ) )
      field_list += "," + mod_info.user_tz_name_field_id;

   if( !mod_info.user_has_auth_field_id.empty( ) )
      field_list += "," + mod_info.user_has_auth_field_id;

   if( !mod_info.user_pin_value_field_id.empty( ) )
      field_list += "," + mod_info.user_pin_value_field_id;

   if( !mod_info.user_read_only_field_id.empty( ) )
      field_list += "," + mod_info.user_read_only_field_id;

   if( !mod_info.user_change_pwd_tm_field_id.empty( ) )
      field_list += "," + mod_info.user_change_pwd_tm_field_id;

   string key_info;

   if( userhash.empty( ) )
   {
      if( username.empty( ) )
         throw runtime_error( GDS( c_display_unknown_or_invalid_user_id ) );

      key_info = mod_info.user_uid_field_id;
      key_info += " " + username;
   }
   else
   {
      key_info = mod_info.user_hash_field_id;
      key_info += " " + userhash;
   }

   bool login_okay = false;

   pair< string, string > user_info;

   if( !fetch_item_info( module_id, mod_info,
    mod_info.user_class_id, key_info, field_list, "", "", sess_info, user_info ) )
      throw runtime_error( "unexpected error occurred processing login information" );

   if( user_info.first.empty( ) )
      throw runtime_error( GDS( c_display_unknown_or_invalid_user_id ) );

   string::size_type pos = user_info.first.find( " " );

   sess_info.user_key = user_info.first.substr( 0, pos );

   vector< string > user_data;
   split( user_info.second, user_data );

   if( user_data.size( ) < 2 )
      throw runtime_error( "unexpected missing user information" );

   bool matched_password = false;

   if( check_password )
   {
      string user_password( user_data[ 1 ] );

      // NOTE: Password fields that are < 20 characters are assumed to have not been
      // either hashed or encrypted.
      if( user_password.length( ) < 20 )
      {
         sess_info.pwd_encrypted = false;

         user_password = hash_password( gid + user_data[ 1 ]
          + ( userhash.empty( ) ? username : user_data[ 0 ] ), 100 );
      }
      else
      {
         string sid;
         get_server_sid( sid );

         user_password = data_decrypt( user_password, sid );

         clear_key( sid );
      }

      string final_password( user_password );

      if( !unique_data.empty( ) )
         final_password = sha256( user_password + unique_data ).get_digest_as_string( );

      if( password == final_password )
         matched_password = true;

      if( ( !username.empty( ) && user_data[ 0 ] != username ) || ( !is_authorised && !matched_password ) )
         throw runtime_error( GDS( c_display_unknown_or_invalid_user_id ) );

      if( !userhash.empty( ) )
         username = user_data[ 0 ];

      sess_info.user_pwd_hash = user_password;
   }

   size_t offset = 2;

   if( !mod_info.user_hdl_field_id.empty( ) )
      sess_info.user_handle = user_data[ offset++ ];

   if( !mod_info.user_hash_field_id.empty( ) )
   {
      string hash = user_data[ offset++ ];

      if( !userhash.empty( ) && hash != userhash )
      {
         // NOTE: If password is empty then assume is a new OpenID account.
         if( password.empty( ) )
            return false;
         else
            throw runtime_error( GDS( c_display_unknown_or_invalid_user_id ) );
      }
   }

   sess_info.user_id = username;

   if( !mod_info.user_name_field_id.empty( ) )
      sess_info.user_name = user_data[ offset++ ];

   sess_info.user_perms.clear( );

   if( !mod_info.user_perm_field_id.empty( ) )
   {
      string user_perm_info( user_data[ offset++ ] );
      vector< string > user_perms;

      if( !user_perm_info.empty( ) )
         split( user_perm_info, user_perms );

      for( size_t i = 0; i < user_perms.size( ); i++ )
      {
         string::size_type pos = user_perms[ i ].find( '=' );
         string key( user_perms[ i ].substr( 0, pos ) );
         string data;

         if( pos != string::npos )
            data = user_perms[ i ].substr( pos + 1 );

         sess_info.user_perms.insert( make_pair( key, data ) );
      }
   }

   // NOTE: If the module requires a user permission then make sure that the user has it.
   if( !mod_info.perm.empty( ) && !sess_info.user_perms.count( mod_info.perm ) )
      throw runtime_error( GDS( c_display_permission_denied ) );

   if( !mod_info.user_extra1_field_id.empty( ) )
      sess_info.user_extra1 = user_data[ offset++ ];

   if( !mod_info.user_extra2_field_id.empty( ) )
      sess_info.user_extra2 = user_data[ offset++ ];

   if( !mod_info.user_crypt_field_id.empty( ) )
      sess_info.user_crypt = user_data[ offset++ ];

   if( !mod_info.user_group_field_id.empty( ) )
      sess_info.user_group = user_data[ offset++ ];

   if( !mod_info.user_other_field_id.empty( ) )
      sess_info.user_other = user_data[ offset++ ];

   sess_info.default_user_group = sess_info.user_group;
   sess_info.default_user_other = sess_info.user_other;

   if( !mod_info.user_parent_field_id.empty( ) )
      sess_info.user_parent = user_data[ offset++ ];

   if( sess_info.user_key == c_admin_user_key )
      sess_info.is_admin_user = true;

   sess_info.user_module = module_name;

   bool is_active = true;

   if( !mod_info.user_active_field_id.empty( ) )
      is_active = ( user_data[ offset++ ] == c_true_value );

   if( !is_active )
      throw runtime_error( GDS( c_display_unknown_or_invalid_user_id ) );

   if( !mod_info.user_slevel_field_id.empty( ) )
      sess_info.user_slevel = user_data[ offset++ ];

   if( !mod_info.user_tz_name_field_id.empty( ) )
      sess_info.tz_name = user_data[ offset++ ];

   if( !mod_info.user_has_auth_field_id.empty( ) )
      sess_info.user_has_auth = user_data[ offset++ ];

   if( !mod_info.user_pin_value_field_id.empty( ) )
   {
      string last_pin_value( sess_info.user_pin_value );
      sess_info.user_pin_value = user_data[ offset++ ];

      if( last_pin_value != sess_info.user_pin_value )
         sess_info.last_user_pin_value = last_pin_value;
   }

   if( !mod_info.user_read_only_field_id.empty( ) )
      sess_info.is_read_only = from_string< bool >( user_data[ offset++ ] );

   if( !mod_info.user_change_pwd_tm_field_id.empty( ) )
      sess_info.change_pwd_tm = from_string< int64_t >( user_data[ offset++ ] );

   return true;
}

void fetch_user_quick_links( const module_info& mod_info, session_info& sess_info )
{
   string key_info( mod_info.user_qlink_pfield_id );

   if( !mod_info.user_qlink_test_field_id.empty( ) )
      key_info += "," + mod_info.user_qlink_test_field_id;

   key_info += "," + mod_info.user_qlink_order_field_id;

   if( mod_info.user_qlink_test_field_id.empty( ) )
      key_info += "#1 " + sess_info.user_key;
   else
      key_info += "#2 " + sess_info.user_key + "," + mod_info.user_qlink_test_field_val;

   string field_list( mod_info.user_qlink_url_field_id );

   field_list += "," + mod_info.user_qlink_name_field_id;
   field_list += "," + mod_info.user_qlink_checksum_field_id;

   sess_info.quick_link_data.clear( );

   if( !fetch_list_info( mod_info.id, mod_info, mod_info.user_qlink_class_id, sess_info, false,
    sess_info.quick_link_limit, key_info, field_list, "", "", "", "", sess_info.quick_link_data, "" ) )
      throw runtime_error( "unexpected error occurred processing quick link info" );

   // NOTE: Get rid of extra row put in for list scrolling purposes...
   if( sess_info.quick_link_data.size( ) > sess_info.quick_link_limit )
      sess_info.quick_link_data.pop_back( );
}

void add_user( const string& user_id, const string& user_name,
 const string& email, const string& clone_key, const string& password,
 string& error_message, const module_info& mod_info, session_info& sess_info,
 string* p_new_key, bool active, const string* p_gpg_key_file )
{
   bool okay = true;
   string new_user_cmd( c_cmd_create );

   new_user_cmd += " sys " + date_time::standard( ).as_string( )
    + " " + mod_info.id + " " + mod_info.user_class_id + " \" " + clone_key + "\"";

   string uid( escaped( user_id, "," ) );
   string pwd( escaped( password, "," ) );
   string name( escaped( user_name, "," ) );

   new_user_cmd += " \"" + mod_info.user_uid_field_id + "=" + escaped( uid, ",\"" );
   new_user_cmd += "," + mod_info.user_pwd_field_id + "=" + escaped( pwd, ",\"" );
   if( password.empty( ) )
      new_user_cmd += "," + mod_info.user_hash_field_id + "=" + escaped( uid, ",\"" );
   new_user_cmd += "," + mod_info.user_name_field_id + "=" + escaped( name, ",\"" );

   new_user_cmd += "," + mod_info.user_active_field_id + "=" + to_string( active );

   if( !email.empty( ) && !mod_info.user_email_field_id.empty( ) )
      new_user_cmd += "," + mod_info.user_email_field_id + "=" + escaped( email, ",\"" );

   string new_user_cmd_extra;

   if( p_gpg_key_file
    && !p_gpg_key_file->empty( ) && !mod_info.user_gpg_install_proc_id.empty( ) )
   {
      new_user_cmd += ",@file=" + *p_gpg_key_file;
      new_user_cmd_extra = " -x=" + mod_info.user_gpg_install_proc_id;
   }

   new_user_cmd += "\"" + new_user_cmd_extra;

   if( sess_info.p_socket->write_line( new_user_cmd ) <= 0 )
      okay = false;
   else
   {
      string response;

      if( sess_info.p_socket->read_line( response, c_initial_response_timeout ) <= 0 )
         okay = false;
      else if( response != c_response_okay )
      {
         string extra_response;

         if( !response.empty( ) && response[ 0 ] != '('
          && ( sess_info.p_socket->read_line( extra_response, c_subsequent_response_timeout ) <= 0 ) )
            okay = false;
         else
         {
            if( extra_response != c_response_okay )
               error_message = escape_markup( response + extra_response );
            else if( p_new_key )
               *p_new_key = response;
         }
      }
      else
         okay = false;
   }

   if( !okay )
      throw runtime_error( "unexpected okay response creating user" );
}

void add_quick_link( const string& module_ref,
 const string& cmd, const string& data, const string& extra,
 const string& listsrch, const string& listsort, const string& oident,
 const string& uselect, string& error_message, bool& had_send_or_recv_error,
 const module_info& mod_info, session_info& sess_info, const map< string, string >* p_list_selections )
{
   string URL( get_module_page_name( module_ref ) + "?cmd=" + cmd + "&ident=" + oident );

   string prefix;
   string suffix;

   if( cmd == c_cmd_view )
   {
      prefix = data;
      URL += "&data=" + data;
   }
   else if( cmd == c_cmd_list )
   {
      suffix = "+" + data + listsrch;
      URL += "&listextra=find&findinfo=" + escape_specials( data );

      if( !listsrch.empty( ) )
         URL += "&listsrch=" + escape_specials( listsrch );

      if( p_list_selections )
      {
         string name;
         for( int i = 0; i < 10; i++ )
         {
            name = c_list_prefix;
            name += c_prnt_suffix;
            name += to_string( i );

            if( p_list_selections->count( name ) )
            {
               suffix += p_list_selections->find( name )->second;
               URL += "&" + name + "=" + escape_specials( p_list_selections->find( name )->second );
            }

            name = c_list_prefix;
            name += c_rest_suffix;
            name += to_string( i );

            if( p_list_selections->count( name ) )
            {
               suffix += p_list_selections->find( name )->second;
               URL += "&" + name + "=" + escape_specials( p_list_selections->find( name )->second );
            }
         }
      }

      if( !listsort.empty( ) )
         URL += "&listsort=" + escape_specials( listsort );
   }
   else
      throw runtime_error( "add_quick_link is not impemented for '" + cmd + "' commands" );

   if( !uselect.empty( ) )
      URL += "&uselect=" + uselect;

   string qlink_cmd( c_cmd_create );

   qlink_cmd += " " + get_uid_info( sess_info )
    + " " + date_time::standard( ).as_string( )
    + " " + mod_info.id + " " + mod_info.user_qlink_class_id + " \"\"";

   escape( URL, "," );
   string name( escaped( extra, "," ) );
   string checksum( escaped( prefix + oident + suffix, "," ) );

   // NOTE: Commas need to be escaped twice due to application server command parsing.
   qlink_cmd += " \"" + mod_info.user_qlink_pfield_id + "=" + sess_info.user_key;
   qlink_cmd += "," + mod_info.user_qlink_url_field_id + "=" + escaped( URL, ",\"" );
   qlink_cmd += "," + mod_info.user_qlink_name_field_id + "=" + escaped( name, ",\"" );
   qlink_cmd += "," + mod_info.user_qlink_checksum_field_id + "=" + escaped( checksum, ",\"" ) + "\"";

   if( sess_info.p_socket->write_line( qlink_cmd ) <= 0 )
      had_send_or_recv_error = true;
   else
   {
      string response;
      if( sess_info.p_socket->read_line( response, c_initial_response_timeout ) <= 0 )
         had_send_or_recv_error = true;
      else if( response != c_response_okay )
      {
         string extra_response;

         if( !response.empty( ) && response[ 0 ] != '('
          && ( sess_info.p_socket->read_line( extra_response, c_subsequent_response_timeout ) <= 0 ) )
            had_send_or_recv_error = true;
         else
         {
            if( extra_response == c_response_okay )
               fetch_user_quick_links( mod_info, sess_info );
            else
               error_message = escape_markup( response + extra_response );
         }
      }
      else
         throw runtime_error( "unexpected okay response creating user quick link" );
   }
}

void save_record( const string& module_id,
 const string& flags, const string& app, const string& chk,
 const string& field, const string& extra, const string& exec, const string& cont,
 const string& fieldlist, bool is_new_record, const map< string, string >& new_field_and_values,
 const map< string, string >& extra_field_info, view_info_const_iterator& vici, const view_source& view,
 int vtab_num, session_info& sess_info, string& act, string& data, string& new_key, string& error_message,
 bool& was_invalid, bool& had_send_or_recv_error )
{
   string key_info;

   if( !is_new_record )
      key_info = chk;
   else
      key_info = "\"" + data + "\"";

   date_time dt( date_time::standard( ) );
   string current_dtm( dt.as_string( ) );

   const map< string, string >& view_extras( view.vici->second->extras );

   const module_info& mod_info( *get_storage_info( ).modules_index.find( view.module )->second );

   string act_cmd;

   if( is_new_record )
      act_cmd = c_cmd_create;
   else
      act_cmd = c_cmd_update;

   act_cmd += " " + get_uid_info( sess_info ) + " " + current_dtm + " " + view.module_id + " " + view.cid;

   if( !sess_info.user_group.empty( ) )
      act_cmd += " -g=" + sess_info.user_group;

   if( !sess_info.tz_name.empty( ) )
      act_cmd += " -tz=" + sess_info.tz_name;

   vector< string > values;

   if( !app.empty( ) )
      split( app, values );

   vector< string > fields;

   if( !fieldlist.empty( ) )
      split( fieldlist, fields );

   size_t num = 0;
   size_t used = 0;
   string field_values;

   bool found_field = false;
   bool ignore_encrypted = false;

   map< string, string > original_field_values;

   set< string > found_new_fields;
   set< string > sorted_fields( fields.begin( ), fields.end( ) );

   if( !extra_field_info.empty( ) && !view.ignore_encrypted_field.empty( ) )
   {
      map< string, string >::const_iterator i;

      for( i = extra_field_info.begin( ); i != extra_field_info.end( ); ++i )
      {
         if( i->first == view.ignore_encrypted_field )
            ignore_encrypted = ( i->second == string( c_true_value ) );
      }
   }

   string sid;
   get_server_sid( sid );

   for( size_t i = 0; i < fields.size( ); i++ )
   {
      string field_id( fields[ i ] );

      string next;

      if( num >= values.size( ) )
         original_field_values.insert( make_pair( field_id, "" ) );
      else
      {
         original_field_values.insert(
          make_pair( field_id, values.at( num ) ) );

         next = escaped( values.at( num++ ), "," );
      }

      if( field_id == c_key_field )
      {
         if( is_new_record )
            key_info = next;

         continue;
      }

      if( field_id == view.qr_scan_key_field )
      {
         string::size_type pos = next.find( ':' );

         // NOTE: Allows "i/I" rather than a ':' iff all other characters
         // are hex nibbles (in order to make clipboard selection easier).
         if( pos == string::npos )
         {
            string::size_type ipos = next.find_first_of( "iI" );

            if( ipos != string::npos )
            {
               if( are_hex_nibbles( next.substr( 0, ipos ) )
                && are_hex_nibbles( next.substr( ipos + 1 ) ) )
                  pos = ipos;
            }
         }

         key_info = next.substr( 0, pos );

         // NOTE: If no separator simply erase the field
         // otherwise erase it and everything before it.
         if( pos == string::npos )
            next.erase( );
         else
            next.erase( 0, pos + 1 );

         regex expr( c_regex_label, true, true );

         // NOTE: Perform an initial check (prior to issuing a command)
         // that the key value only contains simple "label" characters.
         if( expr.search( key_info ) != 0 )
         {
            act = c_act_edit;
            was_invalid = true;

            error_message = string( c_response_error_prefix )
             + string_message( GDS( c_display_invalid_key_value ),
             make_pair( c_display_invalid_key_value_parm_key, key_info ) );

            return;
         }
      }

      if( field_id == view.key_from_hash_field )
         key_info = sha256( next ).get_digest_as_string( ).substr( 0, 20 );

      // NOTE: If an "ignore_encrypted" field is editable then it will need to appear *before*
      // any actual fields that have the "encrypted" extra.
      if( field_id == view.ignore_encrypted_field )
         ignore_encrypted = ( next == string( c_true_value ) );

      size_t j = 0;

      for( ; j < view.field_ids.size( ); j++ )
      {
         if( view.field_ids[ j ] == field_id )
            break;
      }

      if( j == view.field_ids.size( ) )
         throw runtime_error( "unable to find field '" + field_id + "' in field_ids" );

      map< string, string > extra_data;

      if( !view.vici->second->fields[ j ].extra.empty( ) )
         parse_field_extra( view.vici->second->fields[ j ].extra, extra_data );

      if( view.int_fields.count( field_id ) )
      {
         if( new_field_and_values.count( field_id ) )
            next = new_field_and_values.find( field_id )->second;
         else
         {
            if( extra_data.count( c_field_extra_int_type ) )
            {
               string int_type = extra_data[ c_field_extra_int_type ];

               if( int_type == "bytes" )
                  next = to_string( unformat_bytes( next ) );
               else if( int_type == "duration_dhm" || int_type == "duration_hms" )
                  next = to_string( unformat_duration( next ) );
               else
                  throw runtime_error( "unsupported int_type '" + int_type + "'" );
            }
         }
      }
      else if( view.numeric_fields.count( field_id ) )
      {
         if( new_field_and_values.count( field_id ) )
            next = new_field_and_values.find( field_id )->second;
         else
         {
            if( extra_data.count( c_field_extra_numeric_type ) )
            {
               string numeric_type = extra_data[ c_field_extra_numeric_type ];

               if( numeric_type == "bytes" )
                  next = to_string( unformat_bytes( next ) );
               else
                  throw runtime_error( "unsupported numeric_type '" + numeric_type + "'" );
            }
         }
      }
      else if( view.datetime_fields.count( field_id ) )
      {
         if( new_field_and_values.count( field_id ) )
            next = new_field_and_values.find( field_id )->second;

         if( !next.empty( ) && sess_info.tz_name.empty( ) )
         {
            date_time dt( next );

            dt -= ( seconds )sess_info.gmt_offset;

            next = dt.as_string( );
         }
      }
      else if( !ignore_encrypted && view.encrypted_fields.count( field_id ) )
      {
         if( !next.empty( ) )
            next = data_encrypt( next, sid );
      }
      else
      {
         // NOTE: If the record is being created under a parent then the parent field
         // value is fixed and if field has an explicit "new" value then is also fixed.
         if( field_id == field )
            next = escaped( extra, "," );
         else if( new_field_and_values.count( field_id ) )
            next = new_field_and_values.find( field_id )->second;
         else if( view.new_field_values.count( field_id ) )
         {
            if( next.empty( ) )
               next = view.new_field_values.find( field_id )->second;
         }

         // NOTE: A fetch is required for manually provided foreign keys (where an
         // alternate key has been provided rather than the actual foreign key itself).
         fld_info fld( ( vici->second )->fields[ j ] );

         string::size_type pos = fld.pfield.find( '+' );

         if( !next.empty( ) && pos != string::npos && fld.pfield.substr( pos + 1 ) != c_key_field )
         {
            pair< string, string > item_info;
            string key_info( fld.pfield.substr( pos + 1 ) );
            key_info += "#1 " + next;

            string query_info( replaced( fld.pextra, "+", "," ) );

            if( !fetch_item_info( view.module_id, mod_info,
             fld.pclass, key_info, "", "", query_info, sess_info, item_info ) )
               throw runtime_error( "unexpected error occurred processing save" );

            // NOTE: Do not allow the "admin" user to be used by anyone except admin.
            if( !sess_info.is_admin_user
             && fld.pclass == mod_info.user_class_id && next == "admin" )
               item_info.first.erase( );

            // KLUDGE: In order for a sensible error to appear when the record is not found
            // replace the key with what was provided adding a space at either end (to make
            // the key invalid but include what the user had provided in the error message).
            if( item_info.first.empty( ) )
               next = " " + next + " ";
            else
            {
               pos = item_info.first.find( ' ' );
               next = item_info.first.substr( 0, pos );
            }
         }
      }

      if( view.html_fields.count( field_id ) )
         force_html_tags_to_lower_case( next );

      if( field_id == field )
         found_field = true;

      if( new_field_and_values.count( field_id ) )
         found_new_fields.insert( field_id );

      // NOTE: Ensure that only "\n" is being used for "end of line".
      replace( next, "\r\n", "\n" );

      // NOTE: Due to the first comma escaping this will result in the escaping for commas
      // (and for the escape character itself) to be doubled up. This is necessary because
      // of the unescaping that is being performed by the server's command parser.
      if( used++ > 0 )
         field_values += ',';

      field_values += field_id + '=' + escaped( next, "\"", '\\', "rn\r\n" );
   }

   clear_key( sid );

   // NOTE: After dealing with all the field values provided via the UI checks
   // the view for any "defcurrent" fields that have not already been provided.
   if( is_new_record )
   {
      date_time dt_current;

      get_session_dtm( sess_info, dt_current );

      for( size_t i = 0; i < view.field_ids.size( ); i++ )
      {
         if( sorted_fields.count( view.field_ids[ i ] ) )
            continue;

         string next;

         string field_id( view.field_ids[ i ] );
         string value_id( view.value_ids[ i ] );

         if( view.date_fields.count( value_id ) )
         {
            if( view.defcurrent_fields.count( value_id ) )
               next = dt_current.get_date( ).as_string( );
         }
         else if( view.time_fields.count( value_id ) )
         {
            if( view.defcurrent_fields.count( value_id ) )
               next = dt_current.get_time( ).as_string( );
         }
         else if( view.datetime_fields.count( value_id ) )
         {
            if( view.defcurrent_fields.count( value_id ) )
               next = "U" + date_time::standard( ).as_string( );
         }

         if( !next.empty( ) )
         {
            if( used++ > 0 )
               field_values += ',';
            field_values += field_id + '=' + escaped( next, "\"", '\\', "rn\r\n" );
         }
      }
   }

   // NOTE: Add the parent value if not already handled as a field in the view.
   if( !field.empty( ) && !found_field )
   {
      string value( escaped( extra, "," ) );

      if( used++ > 0 )
         field_values += ',';
      field_values += field + '=' + escaped( value, ",\"" );

      if( new_field_and_values.count( field ) )
         found_new_fields.insert( field );
   }

   // NOTE: If fixed field values should be provided for a new record that were not already
   // included as a field in the view itself then apppend the new field ids and values here.
   if( !new_field_and_values.empty( ) )
   {
      map< string, string >::const_iterator i;

      for( i = new_field_and_values.begin( ); i != new_field_and_values.end( ); ++i )
      {
         if( !found_new_fields.count( i->first ) )
         {
            if( used++ > 0 )
               field_values += ',';
            field_values += i->first + '=' + i->second;
         }
      }
   }

   if( !extra_field_info.empty( ) )
   {
      map< string, string >::const_iterator i;

      for( i = extra_field_info.begin( ); i != extra_field_info.end( ); ++i )
      {
         if( i->first != field )
         {
            field_values += ',';

            // NOTE: The 'field_hash' extra is only expected to be used as a hidden extra field when
            // a field that needs to be unique (such as a name field) has been encrypted. The use of
            // the hashed field value (after combining it with with the user's password hash) allows
            // this field to be indexed preventing the unencrypted field value from being used twice.
            if( view.field_hash_fields.count( i->first ) )
            {
               string src_and_dest( i->second );
               string::size_type pos = src_and_dest.find( ':' );

               string src_field( src_and_dest.substr( 0, pos ) );
               string dest_field( pos == string::npos ? src_field : src_and_dest.substr( pos + 1 ) );

               if( original_field_values.count( src_field ) )
               {
                  field_values += dest_field + '=';

                  field_values += sha256(
                   original_field_values[ src_field ] + sess_info.user_pwd_hash ).get_digest_as_string( );
               }
            }
            else
            {
               field_values += i->first + '=';
               field_values += escaped( escaped( i->second, "," ), ",\"", c_nul, "rn\r\n" );
            }
         }
      }
   }

   if( extra_field_info.count( view.key_suffix_field ) )
      key_info += " -ks=" + extra_field_info.find( view.key_suffix_field )->second;

   act_cmd += " " + key_info + " \"" + field_values + "\"";

   if( !exec.empty( ) )
      act_cmd += " -x=" + exec;

   DEBUG_TRACE( act_cmd );

   if( sess_info.p_socket->write_line( act_cmd ) <= 0 )
      had_send_or_recv_error = true;
   else
   {
      string response;

      if( sess_info.p_socket->read_line( response, c_initial_response_timeout ) <= 0 )
         had_send_or_recv_error = true;
      else if( response != c_response_okay )
      {
         string extra_response;

         if( !response.empty( ) && response[ 0 ] != '('
          && ( sess_info.p_socket->read_line( extra_response, c_subsequent_response_timeout ) <= 0 ) )
            had_send_or_recv_error = true;
         else
         {
            if( extra_response == c_response_okay )
               new_key = response;
            else
               error_message = escape_markup( response + extra_response );
         }
      }

      if( cont != c_true && !new_key.empty( ) )
      {
         data = new_key;
         is_new_record = false;
      }

      // NOTE: If an error has occurred then change the action to "edit" (so view remains in edit mode).
      if( !error_message.empty( ) )
      {
         act = c_act_edit;
         was_invalid = true;
      }
   }
}
