// Copyright (c) 2005-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <cmath>
#  include <ctime>
#  include <cstdio>
#  include <cstring>
#  include <deque>
#  include <memory>
#  include <sstream>
#  include <fstream>
#  include <iomanip>
#  include <iostream>
#  include <algorithm>
#  include <stdexcept>
#endif

#ifdef __BORLANDC__
#  include <dir.h>
#  ifndef STRICT
#     define STRICT // Needed for "windows.h" by various Borland headers.
#  endif
#endif
#ifdef _WIN32
#  define _WIN32_WINNT 0x0500 // Needed for the CreateHardLink API function.
#  define NOMINMAX
#  define _WINSOCKAPI_
#  include <windows.h>
#  include <tchar.h>
#  include <direct.h>
#  include <shellapi.h>
#endif
#ifdef __GNUG__
#  include <unistd.h>
#  include <sys/stat.h>
#endif

#include <fcgiapp.h>

#include "ciyam_interface.h"

#include "sha1.h"
#include "smtp.h"
#include "regex.h"
#include "base64.h"
#include "config.h"
#include "ptypes.h"
#include "sha256.h"
#include "numeric.h"
#include "sockets.h"
#ifdef SSL_SUPPORT
#  include "ssl_socket.h"
#  include "crypto_keys.h"
#  ifdef _WIN32
#     include <openssl/applink.c>
#  endif
#endif
#include "threads.h"
#include "date_time.h"
#include "utilities.h"
#include "fcgi_cmds.h"
#include "fcgi_info.h"
#include "fcgi_list.h"
#include "fcgi_view.h"
#include "fcgi_utils.h"
#include "fcgi_parser.h"
#include "crypt_stream.h"
#include "fcgi_process.h"

#define QUIETER_ERROR_LOGGING

#define ALWAYS_REPLACE_SESSION
//#define ALLOW_MULTIPLE_RECORD_ENTRY

#ifdef _WIN32
#  define USE_MOD_FASTCGI_KLUDGE
#endif

//#define USE_MAXIMUM_REQUEST_HANDLERS
#define USE_MULTIPLE_REQUEST_HANDLERS

using namespace std;

namespace
{

#include "ciyam_constants.h"

#ifdef USE_MULTIPLE_REQUEST_HANDLERS
#  ifndef USE_MAXIMUM_REQUEST_HANDLERS
const int c_num_handlers = 10;
#  else
const int c_num_handlers = 100;
#  endif
#endif

const size_t c_max_fcgi_input_size = 65536;
const size_t c_max_param_input_size = 4096;

const int c_pid_timeout = 2500;
const int c_pubkey_timeout = 1000;
const int c_connect_timeout = 2500;
const int c_greeting_timeout = 2500;

const char* const c_unlock = "unlock";

const char* const c_id_file = "../meta/identity.txt";
const char* const c_eid_file = "../meta/encrypted.txt";

const char* const c_stop_file = "../meta/ciyam_interface.stop";

const char* const c_backup_log_file = "../meta/backup.log";
const char* const c_restore_log_file = "../meta/restore.log";

const char* const c_has_restored_file = ".has_restored";

const char* const c_prepare_backup_file = "../meta/.prepare.backup";
const char* const c_prepare_restore_file = "../meta/.prepare.restore";

#ifdef _WIN32
const char* const c_kill_script = "ciyam_interface.kill.bat";
#endif

const char* const c_login_htms = "login.htms";
const char* const c_backup_htms = "backup.htms";
const char* const c_footer_htms = "footer.htms";
const char* const c_openup_htms = "openup.htms";
const char* const c_signup_htms = "signup.htms";
const char* const c_unlock_htms = "unlock.htms";
const char* const c_prepare_htms = "prepare.htms";
const char* const c_restore_htms = "restore.htms";
const char* const c_activate_htms = "activate.htms";
const char* const c_identity_htms = "identity.htms";
const char* const c_password_htms = "password.htms";
const char* const c_ssl_signup_htms = "ssl_signup.htms";
const char* const c_no_identity_htms = "no_identity.htms";
const char* const c_authenticate_htms = "authenticate.htms";
const char* const c_login_password_htms = "login_password.htms";
const char* const c_ciyam_interface_htms = "ciyam_interface.htms";
const char* const c_login_persistent_htms = "login_persistent.htms";
const char* const c_password_persistent_htms = "password_persistent.htms";

const char* const c_printlist_file = "printlist.htms";
const char* const c_printview_file = "printview.htms";

const char* const c_dummy_param_prefix = "dummy=";

const char* const c_nbsp = "&nbsp;";

string g_nbsp( c_nbsp );

const char* const c_login = "@@login";
const char* const c_title = "@@title";
const char* const c_continue = "@@continue";
const char* const c_pin_name = "@@pin_name";
const char* const c_direction = "@@direction";
const char* const c_persistent = "@@persistent";
const char* const c_pin_message = "@@pin_message";
const char* const c_account_type = "@@account_type";
const char* const c_old_password = "@@old_password";
const char* const c_new_password = "@@new_password";
const char* const c_send_request = "@@send_request";
const char* const c_email_address = "@@email_address";
const char* const c_account_type_0 = "@@account_type_0";
const char* const c_account_type_1 = "@@account_type_1";
const char* const c_account_type_2 = "@@account_type_2";
const char* const c_account_type_3 = "@@account_type_3";
const char* const c_unlock_message = "@@unlock_message";
const char* const c_change_password = "@@change_password";
const char* const c_verify_new_password = "@@verify_new_password";
const char* const c_open_up_introduction = "@@open_up_introduction";
const char* const c_sign_up_introduction = "@@sign_up_introduction";
const char* const c_sign_up_extra_details = "@@sign_up_extra_details";
const char* const c_sign_up_gpg_expert_tip = "@@sign_up_gpg_expert_tip";
const char* const c_ssl_sign_up_introduction = "@@ssl_sign_up_introduction";
const char* const c_ssl_sign_up_extra_details = "@@ssl_sign_up_extra_details";

const char* const c_user_other_none = "~";

const char* const c_reset_identity_password = "*******";

// NOTE: When not using multiple request handlers a single socket is being used to access
// the application server (in order to allow testing without possible concurrency issues).
#ifndef USE_MULTIPLE_REQUEST_HANDLERS
#  ifdef SSL_SUPPORT
ssl_socket g_socket;
#  else
tcp_socket g_socket;
#  endif
#endif

bool g_has_connected = false;

bool g_is_disconnected = false;

bool g_is_blockchain_application = false;

size_t g_max_user_limit = 1;

mutex g_session_mutex;

session_container g_sessions;

map< string, string > g_uuid_for_ip_addr;

}

string g_id;
string g_seed;
string g_id_pwd;
string g_bad_seed;
string g_seed_error;

int g_unlock_attempts = 0;

bool g_reset_identity = false;

string g_login_html;
string g_backup_html;
string g_footer_html;
string g_openup_html;
string g_signup_html;
string g_unlock_html;
string g_prepare_html;
string g_restore_html;
string g_activate_html;
string g_identity_html;
string g_password_html;
string g_ssl_signup_html;
string g_no_identity_html;
string g_authenticate_html;

string g_login_password_html;
string g_ciyam_interface_html;
string g_login_persistent_html;
string g_password_persistent_html;

string g_restore_checksum;

string g_display_login_info;
string g_display_change_password;
string g_display_sign_up_for_an_account;

#ifdef USE_MULTIPLE_REQUEST_HANDLERS
#  ifdef SSL_SUPPORT
vector< pair< bool, ssl_socket* > > g_sockets;
#  else
vector< pair< bool, tcp_socket* > > g_sockets;
#  endif

mutex g_socket_mutex;

#  ifdef SSL_SUPPORT
ssl_socket* get_tcp_socket( )
#  else
tcp_socket* get_tcp_socket( )
#  endif
{
   guard g( g_socket_mutex );

#  ifdef SSL_SUPPORT
   ssl_socket* p_socket = 0;
#  else
   tcp_socket* p_socket = 0;
#endif

   if( g_sockets.empty( ) )
      g_sockets.resize( c_num_handlers );

   for( size_t i = 0; i < g_sockets.size( ); i++ )
   {
      if( !g_sockets[ i ].first )
      {
         g_sockets[ i ].first = true;

         if( !g_sockets[ i ].second )
#  ifdef SSL_SUPPORT
            g_sockets[ i ].second = new ssl_socket;
#  else
            g_sockets[ i ].second = new tcp_socket;
#  endif
         p_socket = g_sockets[ i ].second;
         break;
      }
   }

   return p_socket;
}

void release_socket( tcp_socket* p_socket )
{
   guard g( g_socket_mutex );

   for( size_t i = 0; i < g_sockets.size( ); i++ )
   {
      if( g_sockets[ i ].second == p_socket )
      {
         g_sockets[ i ].first = false;

         break;
      }
   }
}

void disconnect_socket( tcp_socket* p_socket )
{
   guard g( g_socket_mutex );

   for( size_t i = 0; i < g_sockets.size( ); i++ )
   {
      if( g_sockets[ i ].second == p_socket )
      {
         if( g_sockets[ i ].second && g_sockets[ i ].second->okay( ) )
         {
            g_sockets[ i ].second->close( );
            delete g_sockets[ i ].second;
         }

         g_sockets[ i ].first = false;
         g_sockets[ i ].second = 0;
         break;
      }
   }
}

void disconnect_sockets( bool released_only )
{
   guard g( g_socket_mutex );

   for( size_t i = 0; i < g_sockets.size( ); i++ )
   {
      if( released_only && g_sockets[ i ].first )
         continue;

      if( g_sockets[ i ].second && g_sockets[ i ].second->okay( ) )
      {
         g_sockets[ i ].second->close( );

         delete g_sockets[ i ].second;
      }

      g_sockets[ i ].first = false;
      g_sockets[ i ].second = 0;
   }
}

void remove_sockets( )
{
   guard g( g_socket_mutex );

   for( size_t i = 0; i < g_sockets.size( ); i++ )
   {
      if( g_sockets[ i ].second )
      {
         delete g_sockets[ i ].second;

         g_sockets[ i ].second = 0;
      }
   }
}
#endif

bool get_is_disconnected( )
{
   guard g( g_session_mutex );

   return g_is_disconnected;
}

void set_is_disconnected( bool is_disconnected )
{
   guard g( g_session_mutex );

   g_is_disconnected = is_disconnected;
}

size_t get_num_sessions( )
{
   guard g( g_session_mutex );

   return g_sessions.size( );
}

void add_session_info( const string& session_id, session_info* p_session_info )
{
   guard g( g_session_mutex );

   g_sessions.insert( session_value_type( session_id, p_session_info ) );
}

void destroy_session( const string& session_id )
{
   guard g( g_session_mutex );

   session_iterator si = g_sessions.find( session_id );

   if( si != g_sessions.end( ) )
   {
      delete si->second;
      g_sessions.erase( si );
   }
}

void disconnect_all_session_sockets( )
{
   guard g( g_session_mutex );

   for( session_iterator si = g_sessions.begin( ), end = g_sessions.end( ); si != end; ++si )
   {
      if( si->second && si->second->p_socket )
         si->second->p_socket = 0;
   }

   disconnect_sockets( false );

   set_is_disconnected( true );
}

bool has_session_info( const string& session_id )
{
   guard g( g_session_mutex );

   return ( g_sessions.count( session_id ) > 0 );
}

bool has_user_session_info( const string& user_id, const char* p_module = 0 )
{
   guard g( g_session_mutex );

   for( session_iterator si = g_sessions.begin( ), end = g_sessions.end( ); si != end; ++si )
   {
      if( p_module && string( p_module ) != ( si->second )->user_module )
         continue;

      if( user_id == ( si->second )->user_id )
         return true;
   }

   return false;
}

void destroy_user_session_info( const string& user_id, const char* p_module = 0 )
{
   guard g( g_session_mutex );

   session_iterator si, end;

   for( si = g_sessions.begin( ), end = g_sessions.end( ); si != end; ++si )
   {
      if( p_module && string( p_module ) != ( si->second )->user_module )
         continue;

      if( user_id == ( si->second )->user_id )
         break;
   }

   if( si != g_sessions.end( ) )
   {
      if( si->second->locked )
         throw runtime_error( GDS( c_display_you_are_currently_logged_in ) );

      delete si->second;
      g_sessions.erase( si );
   }
}

session_info* get_session_info( const string& session_id, bool lock_session = true )
{
   guard g( g_session_mutex );

   session_iterator si = g_sessions.find( session_id );

   if( si == g_sessions.end( ) )
      return 0;
   else if( si->second->locked )
      return 0;
   else
   {
      if( lock_session )
         si->second->locked = true;
      return si->second;
   }
}

inline const string& data_or_nbsp( const string& input )
{
   if( !input.empty( ) )
      return input;
   else
      return g_nbsp;
}

string format_int_value( int i, const string& mask )
{
   return format_numeric( i, mask );
}

string format_numeric_value( const numeric& n, const string& mask )
{
   return format_numeric( n, mask );
}

void parse_fcgi_input( FCGX_Stream* p_in, map< string, string >& input_data, char separator = '&', bool keep_existing = true )
{
   char buf[ c_max_fcgi_input_size + 1 ];
   memset( buf, '\0', sizeof( buf ) );

   int len = FCGX_GetStr( buf, c_max_fcgi_input_size, p_in );
   buf[ len ] = '\0';

   parse_input( buf, len, input_data, separator, keep_existing );
}

void parse_param_input( const char* p_param, map< string, string >& input_data, char separator = ';', bool keep_existing = true )
{
   char buf[ c_max_param_input_size + 1 ];

   size_t len = min( strlen( p_param ), c_max_param_input_size );

   memcpy( buf, p_param, len );
   buf[ len ] = '\0';

   parse_input( buf, len, input_data, separator, keep_existing );
}

void read_global_storage_info( )
{
   vector< string > log_messages;
   read_storage_info( get_storage_info( ), log_messages );

   g_is_blockchain_application = !get_storage_info( ).blockchain.empty( );

   for( size_t i = 0; i < log_messages.size( ); i++ )
      LOG_TRACE( log_messages[ i ] );
}

string get_unique( const string& id, const string& ip_addr )
{
   guard g( g_socket_mutex );

   // FUTURE: If a huge number of connections are made from different IP addresses that
   // never perform a successful login then the map being used could chew up very large
   // amounts of memory (perhaps also use an IP address to time stamp map so that those
   // unsuccessful IP address entries that are older can be removed periodically).
   if( !g_uuid_for_ip_addr.count( ip_addr ) )
      g_uuid_for_ip_addr.insert( make_pair( ip_addr, uuid( ).as_string( ) ) );

   string unique( g_uuid_for_ip_addr[ ip_addr ] );

   return sha256( id + unique ).get_digest_as_string( );
}

void clear_unique( map< string, string >& input_data )
{
   guard g( g_socket_mutex );

   if( g_uuid_for_ip_addr.count( input_data[ c_http_param_raddr ] ) )
      g_uuid_for_ip_addr.erase( input_data[ c_http_param_raddr ] );
}

void crypt_decoded( const string& pwd_hash, string& decoded, bool decode = true )
{
   // KLUDGE: As Javascript strings are UTF encoded to avoid losing data the encrypted
   // strings are actually "hex" (also refer to similar KLUDGE in ciyam_interface.js).
   if( decode )
   {
      string s;
      for( size_t i = 0; i < decoded.size( ); i += 2 )
      {
         unsigned char ch = hex_nibble( decoded[ i ] );
         ch <<= 4;
         ch |= hex_nibble( decoded[ i + 1 ] );

         s += ch;
      }
      decoded = s;
   }

   string full_key( pwd_hash );
   string last_hash( full_key );

   while( full_key.size( ) < decoded.size( ) )
   {
      string next( sha256( last_hash ).get_digest_as_string( ) );

      full_key += next;
      last_hash = next;
   }

   for( size_t i = 0; i < decoded.size( ); i++ )
      decoded[ i ] ^= ( unsigned char )full_key[ i ];

   // KLUDGE: (see above)
   if( !decode )
   {
      string s;
      for( size_t i = 0; i < decoded.size( ); i++ )
      {
         unsigned char ch = decoded[ i ];
         s += ascii_digit( ( ch & 0xf0 ) >> 4 );
         s += ascii_digit( ch & 0x0f );
      }
      decoded = s;
   }
   else
   {
      string::size_type pos = decoded.find( '\0' );
      if( pos != string::npos )
         decoded.erase( pos );
   }
}

bool process_log_file( const string& module_name, const string& log_file_name,
 const string& title, const string& html, const string& text_to_replace, ostringstream& extra_content )
{
   bool has_output_form = false;

   vector< string > lines;
   buffer_file_lines( log_file_name, lines );

   bool has_any_error = false;
   bool has_completed = false;

   // NOTE: If the second last line starts with four '=' characters is assuming
   // that the script has completed and will rename the file (allowing the user
   // time to view the file data which disappears after the page is refreshed).
   if( lines.size( ) >= 3 && ( lines[ lines.size( ) - 2 ].find( "===" ) == 0 ) )
   {
      has_completed = true;
      file_rename( log_file_name, string( log_file_name ) + c_sav_file_ext );

      for( size_t i = 0; i < lines.size( ); i++ )
      {
         string next_line( lines[ i ] );

         if( lower( next_line ).find( c_error ) != string::npos )
         {
            has_any_error = true;
            break;
         }
      }
   }

   // NOTE: Unless not completed or an error was found don't display anything.
   if( has_any_error || !has_completed )
   {
      string output_html( html );

      str_replace( output_html, text_to_replace.c_str( ), join( lines, '\n' ) );

      output_form( module_name, extra_content, output_html, "", false, title );

      has_output_form = true;

      if( !get_is_disconnected( ) )
         disconnect_all_session_sockets( );
   }

   return has_output_form;
}

void reset_admin_password( session_info& sess_info,
 const string& module_id, const module_info& mod_info, string& pwd, string& sid )
{
   string admin_pwd_hash( hash_password( g_id + pwd + c_admin_user_key ) );

   string admin_user_hash( sha256( c_admin_user_key + admin_pwd_hash ).get_digest_as_string( ) );

   string encrypted_pwd( data_encrypt( admin_pwd_hash, sid ) );

   clear_key( pwd );
   clear_key( sid );

   file_remove( c_has_restored_file );

   vector< pair< string, string > > pwd_field_value_pairs;

   pwd_field_value_pairs.push_back( make_pair( mod_info.user_pwd_field_id, encrypted_pwd ) );
   pwd_field_value_pairs.push_back( make_pair( mod_info.user_hash_field_id, admin_user_hash ) );

   string error_message;

   // NOTE: Temporarily assumes the "admin" user in order to update the password.
   restorable< string > temp_user_key( sess_info.user_key, c_admin_user_key );

   if( !perform_update( module_id, mod_info.user_class_id,
    c_admin_user_key, pwd_field_value_pairs, sess_info, &error_message ) )
   {
      if( !error_message.empty( ) )
         throw runtime_error( error_message );
      else
         throw runtime_error( "unexpected server error occurred trying to update 'admin' password" );
   }
}

class timeout_handler : public thread
{
   public:
   void on_start( );
};

void timeout_handler::on_start( )
{
#ifdef _WIN32
   if( file_exists( c_kill_script ) )
      file_remove( c_kill_script );
#endif
   while( true )
   {
      msleep( 1000 );

      if( get_is_disconnected( ) )
         continue;

      guard g( g_session_mutex );

      vector< session_iterator > dead_sessions;

      time_t tm_now( time( 0 ) );

      for( session_iterator si = g_sessions.begin( ), end = g_sessions.end( ); si != end; ++si )
      {
         if( ( si->second )->locked )
            continue;

         if( tm_now - ( si->second )->tm_last_request > c_timeout_seconds )
         {
            // KLUDGE: For some unknown reason occasionally this code is being executed for
            // anonymous sessions (this should be investigated further at some stage).
            if( !si->second->user_id.empty( ) || !si->second->user_name.empty( ) )
            {
               LOG_TRACE( "[timeout: "
                + ( si->second->user_name.empty( ) ? si->second->user_id : si->second->user_name )
                + " at " + date_time::local( ).as_string( true, false ) + " from " + si->second->ip_addr + "]" );
            }

            if( si->second->p_socket )
            {
               release_socket( si->second->p_socket );
               si->second->p_socket = 0;
            }

            remove_session_temp_directory( si->second->session_id );

            delete si->second;
            dead_sessions.push_back( si );
         }
      }

      for( size_t i = 0; i < dead_sessions.size( ); i++ )
         g_sessions.erase( dead_sessions[ i ] );

#ifdef USE_MULTIPLE_REQUEST_HANDLERS
      if( file_exists( c_stop_file ) )
         disconnect_sockets( true );
#endif
      if( g_sessions.empty( ) )
      {
#ifdef USE_MULTIPLE_REQUEST_HANDLERS
         disconnect_sockets( false );
#else
         if( g_has_connected )
         {
            g_has_connected = false;
            g_socket.write_line( c_cmd_quit );
            g_socket.close( );
         }
#endif
         bool has_changed = false;

         time_t t = last_modification_time( c_fcgi_sio );

         if( t != get_storage_info( ).sio_mod )
            has_changed = true;
         else
         {
            module_index_iterator mii;

            for( mii = get_storage_info( ).modules_index.begin( ); mii != get_storage_info( ).modules_index.end( ); ++mii )
            {
               t = last_modification_time( mii->first + c_fcgi_sio_ext );

               if( t != mii->second->sio_mod )
               {
                  has_changed = true;
                  break;
               }
            }
         }

         if( has_changed )
         {
            read_global_storage_info( );

            module_index_iterator mii;

            for( mii = get_storage_info( ).modules_index.begin( ); mii != get_storage_info( ).modules_index.end( ); ++mii )
               read_module_info( mii->first, *mii->second, get_storage_info( ) );
         }

         if( file_exists( c_extkeys_file ) )
         {
            time_t t = last_modification_time( c_extkeys_file );

            if( t != get_storage_info( ).extkeys_mod )
               init_extkeys( );
         }
      }

#ifdef _WIN32
      if( !file_exists( c_kill_script ) )
      {
         ofstream outf( c_kill_script );
         outf << "TASKKILL /F /PID " << get_pid( ) << '\n';
      }
#endif
   }
}

class request_handler : public thread
{
   public:
   request_handler( );

   void on_start( );

   void process_request( );

   private:
   FCGX_Stream* p_in;
   FCGX_Stream* p_out;
   FCGX_Stream* p_err;
   FCGX_ParamArray* p_env;

#ifdef USE_MULTIPLE_REQUEST_HANDLERS
   FCGX_Request request;
#else
   FCGX_ParamArray envp;
#endif
};

request_handler::request_handler( )
{
#ifdef USE_MULTIPLE_REQUEST_HANDLERS
   if( FCGX_InitRequest( &request, 0, 0 ) != 0 )
      throw runtime_error( "*** FCGX_InitRequest failed ***" );
#endif
}

void request_handler::on_start( )
{
#ifdef USE_MULTIPLE_REQUEST_HANDLERS
   while( FCGX_Accept_r( &request ) >= 0 )
   {
      p_in = request.in;
      p_out = request.out;
      p_err = request.err;

      p_env = &request.envp;

      process_request( );
   }
#else
   while( FCGX_Accept( &p_in, &p_out, &p_err, &envp ) >= 0 )
   {
      p_env = &envp;

      process_request( );
   }
#endif

   delete this;
}

void request_handler::process_request( )
{
   bool okay = true;
   bool is_ssl = false;
   bool is_vertical = false;
   bool encrypt_data = false;
   bool temp_session = false;
   bool display_error = true;
   bool force_refresh = false;
   bool login_refresh = false;
   bool created_session = false;
   bool using_anonymous = false;
   bool finished_session = false;
   bool identity_failure = false;

   string hash;
   string user;
   string raddr;
   string title;
   string unique_id;
   string module_id;
   string module_ref;
   string module_name;

   string session_id;
   string interface_file;

   session_info* p_session_info = 0;

   ostringstream form_content, extra_content;

   bool cookies_permitted = false;

   DEBUG_TRACE( "[process request]" );

   try
   {
      const char* p_ssl = FCGX_GetParam( c_http_param_ssl, *p_env );
      const char* p_host = FCGX_GetParam( c_http_param_host, *p_env );
      const char* p_raddr = FCGX_GetParam( c_http_param_raddr, *p_env );
      const char* p_rhost = FCGX_GetParam( c_http_param_rhost, *p_env );
      const char* p_ruser = FCGX_GetParam( c_http_param_ruser, *p_env );
      const char* p_uagent = FCGX_GetParam( c_http_param_uagent, *p_env );

      map< string, string > input_data;

      if( p_ssl )
      {
         is_ssl = true;
         input_data.insert( make_pair( c_http_param_ssl, p_ssl ) );
      }
      else if( get_storage_info( ).encrypt_data )
         encrypt_data = true;

      if( p_host )
         input_data.insert( make_pair( c_http_param_host, p_host ) );

      if( p_raddr )
         input_data.insert( make_pair( c_http_param_raddr, p_raddr ) );

      if( p_rhost )
         input_data.insert( make_pair( c_http_param_rhost, p_rhost ) );

      if( p_ruser )
         input_data.insert( make_pair( c_http_param_ruser, p_ruser ) );

      if( p_uagent )
         input_data.insert( make_pair( c_http_param_uagent, p_uagent ) );

      const char* p_query = FCGX_GetParam( c_http_param_query, *p_env );
      const char* p_cookie = FCGX_GetParam( c_http_param_cookie, *p_env );

      if( p_cookie )
      {
         map< string, string > all_cookies;
         parse_param_input( p_cookie, all_cookies );

         if( all_cookies.count( c_param_session ) )
         {
            string session_cookie( c_param_session );
            session_cookie += "=" + all_cookies[ c_param_session ];

            parse_param_input( session_cookie.c_str( ), input_data, ',' );
         }
      }

      if( input_data.find( c_param_session ) != input_data.end( ) )
         cookies_permitted = true;

      if( p_query )
         parse_param_input( p_query, input_data, '&' );

      parse_fcgi_input( p_in, input_data, '&' );

      string cmd( input_data[ c_param_cmd ] );
      string userhash( input_data[ c_param_userhash ] );
      string username( input_data[ c_param_username ] );

      raddr = input_data[ c_http_param_raddr ];
      session_id = input_data[ c_param_session ];

      string pwd_hash;

      // NOTE: If an existing session is present then obtain its
      // password hash for decrypting base64 data (if required).
      if( !session_id.empty( ) && session_id != c_new_session )
      {
         p_session_info = get_session_info( session_id, false );

         if( p_session_info )
         {
            if( p_session_info->content_pwd_hash.empty( ) )
               p_session_info->content_pwd_hash = p_session_info->user_pwd_hash;

            pwd_hash = p_session_info->content_pwd_hash;
         }
      }

      string ver( input_data[ c_param_ver ] );

      // NOTE: It is expected that for any UI version change that the FCGI
      // server will have been restarted so don't bother with the check if
      // the session is present. If wanting to change this then would need
      // to make sure that the "ver" param is included for every call that
      // can occur into here (which currently is not the case - as you can
      // test by deleting a list item).
      if( !p_session_info && atoi( ver.c_str( ) ) < c_ui_script_version )
         throw runtime_error( GDS( c_display_client_script_out_of_date ) );

      bool has_decrypted = false;
      bool has_failed_to_decrypt = false;

      string base64_data( input_data[ c_param_base64 ] );

      if( !base64_data.empty( ) )
      {
         string decoded( base64::decode( base64_data ) );

         bool check_decoded = false;

         if( !pwd_hash.empty( ) )
         {
            check_decoded = true;

            crypt_decoded( pwd_hash, decoded );
         }

         if( check_decoded )
         {
            size_t pos = decoded.find( c_dummy_param_prefix );

            if( pos != string::npos )
            {
               has_decrypted = true;

               parse_input( ( char* )decoded.c_str( ), decoded.size( ), input_data, '&', true );
            }
         }

         if( !has_decrypted )
            has_failed_to_decrypt = true;
      }

      // NOTE: Erase the base64 data if it wasn't decrypted so that other code can
      // just check if it's present to know that client-crypto is currently in use.
      if( !encrypt_data || has_failed_to_decrypt )
         input_data.erase( c_param_base64 );

      string back( input_data[ c_param_back ] );
      string data( input_data[ c_param_data ] );
      string keep( input_data[ c_param_keep ] );
      string chksum( input_data[ c_param_chksum ] );
      string hashval( input_data[ c_param_hashval ] );
      string newhash( input_data[ c_param_newhash ] );
      string password( input_data[ c_param_password ] );
      string persistent( input_data[ c_param_persistent ] );

      cmd = input_data[ c_param_cmd ];
      hash = input_data[ c_param_hash ];
      user = input_data[ c_param_user ];
      session_id = input_data[ c_param_session ];

      bool is_kept = ( keep == c_true );

      string::size_type spos = session_id.find( '=' );

      if( spos != string::npos )
         session_id.erase( 0, spos + 1 );

      module_name = input_data[ c_param_module ];
      module_ref = get_storage_info( ).get_module_ref( module_name );

      if( cmd == c_cmd_activate && ( !username.empty( ) || !userhash.empty( ) ) )
      {
         data.erase( );
         user.erase( );

         cmd = c_cmd_login;
      }

      bool new_session = true;
      bool is_activation = ( cmd == c_cmd_activate );

      bool is_authorised = false;
      bool is_login_screen = false;
      bool is_backup_or_restore = false;

      bool is_prepare_backup = file_exists( c_prepare_backup_file );
      bool is_prepare_restore = file_exists( c_prepare_restore_file );

      if( g_restore_checksum.length( ) && ( chksum != g_restore_checksum ) )
         g_restore_checksum.erase( );

      if( is_prepare_backup || is_prepare_restore )
      {
         string prepare_html( g_prepare_html );
         string prepare_title( is_prepare_backup ? GDS( c_display_backup_preparation ) : GDS( c_display_restore_completion ) );
         string prepare_display_text( is_prepare_backup ? GDS( c_display_backup_in_progress ) : GDS( c_display_restore_in_progress ) );

         str_replace( prepare_html, c_prepare_text, prepare_display_text );

         output_form( module_name, extra_content, prepare_html, "", false, prepare_title );

         is_backup_or_restore = true;

         if( !get_is_disconnected( ) )
            disconnect_all_session_sockets( );
      }
      else if( file_exists( c_backup_log_file ) )
      {
         is_backup_or_restore = process_log_file( module_name, c_backup_log_file,
          GDS( c_display_backup_in_progress ), g_backup_html, c_backup_text, extra_content );
      }
      else if( file_exists( c_restore_log_file ) )
      {
         g_restore_checksum = chksum;

         is_backup_or_restore = process_log_file( module_name, c_restore_log_file,
          GDS( c_display_restore_in_progress ), g_restore_html, c_restore_text, extra_content );

         if( !file_exists( c_restore_log_file ) )
            file_touch( c_has_restored_file, 0, true );
      }
      else if( file_exists( c_stop_file ) )
      {
         msleep( 2500 );

         if( file_exists( c_stop_file ) )
         {
            using_anonymous = true;
            throw runtime_error( GDS( c_display_under_maintenance_try_again_later ) );
         }
      }
      else
         set_is_disconnected( false );

      if( !get_storage_info( ).modules_index.count( module_name ) )
         throw runtime_error( GDS( c_display_module_name_missing_or_invalid ) );

      set< string > login_opts;

      if( !get_storage_info( ).login_opts.empty( ) )
         split( get_storage_info( ).login_opts, login_opts, '+' );

      module_info& mod_info( *get_storage_info( ).modules_index.find( module_name )->second );

      title = mod_info.title;
      module_id = mod_info.id;

      if( g_id.empty( ) && file_exists( c_id_file ) )
         g_id = buffer_file( c_id_file );

      bool is_invalid_session = false;

      if( !session_id.empty( ) && ( session_id != c_new_session ) && ( cmd != c_cmd_join ) && ( cmd != c_cmd_status ) )
      {
         if( user.empty( ) || ( !user.empty( ) && ( hash == get_user_hash( user ) ) ) )
         {
            p_session_info = get_session_info( session_id, false );

            if( !p_session_info )
            {
               if( cmd.empty( ) || ( cmd == c_cmd_home ) || ( cmd == c_cmd_quit ) || ( cmd == c_cmd_login )
                || ( !password.empty( ) && ( cmd == c_cmd_password ) ) || !username.empty( ) || !userhash.empty( ) )
               {
                  if( cmd != c_cmd_password )
                     cmd = c_cmd_home;

                  session_id.erase( );
               }

               if( is_kept && get_storage_info( ).login_days )
               {
                  username = user;
                  cmd = c_cmd_home;
                  session_id.erase( );
                  is_authorised = true;
               }
               else if( !session_id.empty( ) && ( session_id != get_unique( g_id, raddr ) ) )
               {
                  if( user.empty( ) )
                     session_id.erase( );
                  else
                     is_invalid_session = true;
               }
            }
         }
      }

      bool is_sign_in = false;
      bool needs_identity = true;

      if( !is_kept
       && cmd != c_cmd_password && cmd != c_cmd_credentials
       && !is_invalid_session && mod_info.allows_anonymous_access
       && ( username.empty( ) && userhash.empty( ) && !p_session_info ) )
      {
         is_authorised = true;
         using_anonymous = true;
      }

      if( cmd != c_cmd_status )
         unique_id = get_unique( g_id, raddr );

      if( cmd == c_cmd_identity )
      {
         g_seed = input_data[ c_param_data ];
         g_id_pwd = input_data[ c_param_extra ];

         bool missing_mnemonics = false;

         if( g_seed.empty( ) )
            missing_mnemonics = true;
         else
         {
            if( g_seed != c_unlock )
            {
               crypt_decoded( unique_id, g_seed );

               g_seed = trim( g_seed );

               size_t num_spaces = count( g_seed.begin( ), g_seed.end( ), ' ' );

               if( num_spaces < 11 )
                  missing_mnemonics = true;
            }

            crypt_decoded( unique_id, g_id_pwd );
         }

         if( missing_mnemonics )
         {
            display_error = false;

            force_refresh = false;
            login_refresh = false;

            identity_failure = true;

            g_bad_seed = g_seed;
            g_seed.erase( );

            g_seed_error = GDS( c_display_error ) + ": " + GDS( c_display_missing_mnemonics );

            throw runtime_error( "unable to set/update identity information" );
         }
         else
         {
            if( !mod_info.allows_anonymous_access )
            {
               is_sign_in = true;

               // KLUDGE: When the initial identity is provided
               // the first login will fail so force a refresh.
               if( g_seed != c_unlock )
                  login_refresh = true;
            }
            else
               cmd = c_cmd_home;
         }
      }
      else if( g_id.empty( ) )
         cmd = c_cmd_home;

      if( ( cmd == c_cmd_password ) || ( cmd == c_cmd_credentials ) )
      {
         cmd = c_cmd_home;
         is_sign_in = true;

         if( is_invalid_session )
         {
            session_id.erase( );
            is_invalid_session = false;
         }
      }

      if( using_anonymous || ( cmd == c_cmd_join ) || ( cmd == c_cmd_status ) )
      {
         temp_session = true;
         session_id = c_new_session;
      }

      if( is_ssl && cmd.empty( ) && input_data.count( c_http_param_ruser ) )
         cmd = c_cmd_open;

      string activation_file;

      if( is_activation )
         activation_file = ( data + chksum );

      if( session_id.empty( ) || ( session_id == c_new_session ) )
      {
         if( cmd == c_cmd_open )
         {
            string user_id = input_data[ c_http_param_ruser ];

            if( !user_id.empty( ) )
            {
               temp_session = false;
               is_authorised = true;

               userhash = sha256( user_id ).get_digest_as_string( );
            }
         }

         // NOTE: If persistent logins are not permitted ignore any attempt to use them.
         if( !get_storage_info( ).login_days )
         {
            persistent.erase( );

            if( password.empty( ) )
               userhash.erase( );
         }

         if( username.empty( )
          && userhash.empty( ) && password.empty( ) && !needs_identity
          && ( !using_anonymous || ( is_activation && file_exists( activation_file ) ) ) )
         {
            if( !is_backup_or_restore )
            {
               string login_html( !cookies_permitted || !get_storage_info( ).login_days
                || g_login_persistent_html.empty( ) ? g_login_html : g_login_persistent_html );

               if( cmd != c_cmd_activate )
               {
                  is_login_screen = true;
                  output_form( module_name, extra_content, login_html );
               }
               else
               {
                  string activate_html( g_activate_html );
                  str_replace( activate_html, c_username, user );

                  string message( "<p><b>"
                   + string_message( GDS( c_display_provide_password_to_activate ),
                   make_pair( c_display_provide_password_to_activate_parm_id, user ) ) + "</b></p>" );

                  output_form( module_name, extra_content, activate_html, message, false );
               }
            }
         }
         else if( !is_backup_or_restore )
         {
            if( cmd == c_cmd_activate )
            {
               username = user;
               cmd = c_cmd_login;
            }
            else if( !using_anonymous )
               cmd = c_cmd_home;

            if( g_max_user_limit && ( get_num_sessions( ) >= g_max_user_limit ) )
               throw runtime_error( GDS( c_display_max_concurrent_users_logged_in ) );

            p_session_info = new session_info( get_storage_info( ) );

            created_session = true;

            p_session_info->locked = true;
            p_session_info->ip_addr = raddr;

            if( temp_session )
            {
               // NOTE: For temporary sessions force the session id to be the unique id for the
               // IP address and derive both the hash values and checksum prefixes from "g_id".
               // This allows users to keep URL links for anonymous sessions that will still be
               // valid when used at another time or from another machine.
               session_id = p_session_info->session_id = unique_id;

               p_session_info->hashval_prefix = sha1( g_id + g_id ).get_digest_as_string( );
               p_session_info->checksum_prefix = sha1( g_id + p_session_info->hashval_prefix ).get_digest_as_string( );

               input_data[ c_param_session ] = session_id;
            }
            else
               session_id = p_session_info->session_id = uuid( ).as_string( );

            if( !temp_session && ( cmd != c_cmd_open )
             && ( is_authorised || ( persistent == c_true ) ) )
            {
               string dtmoff( input_data[ c_param_dtmoff ] );

               if( !dtmoff.empty( ) )
                  p_session_info->dtm_offset = atoi( dtmoff.c_str( ) );

               string gmtoff( input_data[ c_param_gmtoff ] );

               if( !gmtoff.empty( ) )
                  p_session_info->gmt_offset = atoi( gmtoff.c_str( ) );

               p_session_info->is_persistent = true;
            }
            else
               add_non_persistent( session_id );
         }
      }
      else
      {
         new_session = false;
         p_session_info = get_session_info( session_id );

         if( p_session_info )
         {
            if( ( cmd == c_cmd_open ) || ( cmd == c_cmd_activate ) )
               cmd = c_cmd_home;

            p_session_info->tm_last_request = time( 0 );
         }
      }

      string extra_content_func;

      bool is_identity_form = false;
      bool has_just_logged_in = false;

      if( !p_session_info )
      {
         if( !new_session )
         {
            DEBUG_TRACE( "session " + session_id + " terminated" );

            remove_session_temp_directory( session_id );

            ostringstream osstr;

            if( !is_sign_in )
               osstr << "<p class=\"error\" align=\"center\">" << GDS( c_display_error )
                << ": " << GDS( c_display_your_session_has_been_terminated ) << ".</p>\n";

            string login_html( !cookies_permitted || !get_storage_info( ).login_days
             || g_login_persistent_html.empty( ) ? g_login_html : g_login_persistent_html );

            output_form( module_name, extra_content, login_html, osstr.str( ) );

            if( cookies_permitted )
               extra_content_func += "document.cookie = '" + get_cookie_value( "", c_anon_user_key, true ) + "';";
         }
         else if( cookies_permitted )
         {
            if( !user.empty( ) )
               cmd.erase( );
            else
               extra_content_func += "document.cookie = '" + get_cookie_value( "", c_anon_user_key, true ) + "';";
         }
      }
      else if( !is_backup_or_restore )
      {
         guard g( g_session_mutex );

         bool was_unlock = false;
         bool is_meta_module = false;
         bool has_output_form = false;
         bool has_set_identity = false;

         if( module_name == "Meta" )
         {
            is_meta_module = true;
            p_session_info->is_meta_module = true;
         }

         string id_file_name( c_id_file );
         string eid_file_name( c_eid_file );

         if( !p_session_info->p_socket )
         {
#ifndef USE_MULTIPLE_REQUEST_HANDLERS
            p_session_info->p_socket = &g_socket;
#else
            p_session_info->p_socket = get_tcp_socket( );

            if( !p_session_info->p_socket )
               throw runtime_error( GDS( c_display_service_busy_try_again_later ) );
#endif
            bool connection_okay = false;

            // NOTE: Before the FCGI interface has ever connected to the server if the "identity.txt"
            // file was not present then it will have an empty identity string. In production systems
            // this should never occur, but for a development environment a newly created application
            // will not have an identity so the very first attempt to "log in" (if the application is
            // not allowing anonymous access) the user password will have been hashed with an "empty"
            // identity string (so the original string is being copied here for this very situation).
            string id_for_login( g_id );

            // NOTE: If was attempting to reset the identity password
            // then will need to repeat the initial socket connection.
            if( g_reset_identity )
               p_session_info->p_socket->close( );

            if( g_has_connected )
               connection_okay = true;
            else if( p_session_info->p_socket->okay( ) )
               connection_okay = true;
            else
            {
               DEBUG_TRACE( "[opening socket]" );

               if( p_session_info->p_socket->open( ) )
               {
                  ip_address address( c_default_ciyam_host, c_default_ciyam_port );

#ifdef FCGI_UI_LOG_CONNECTIONS
                  LOG_TRACE( "(connecting for " + raddr + ")" );
#endif

                  bool has_connected = p_session_info->p_socket->connect( address, c_connect_timeout );

                  // NOTE: Will pause and then attempt to connect once again before reporting that
                  // the application server is unavailable (this is because the first connect will
                  // sometimes fail after a system restore due to the new server socket acceptor).
                  // To prevent unnecessary delays initially only waits for 25ms but if this isn't
                  // sufficient will then wait another 2.5s before retrying again.
                  if( !has_connected )
                  {
                     p_session_info->p_socket->close( );

                     msleep( 25 );

                     DEBUG_TRACE( "[re-opening socket]" );

                     if( p_session_info->p_socket->open( ) )
                     {
                        has_connected = p_session_info->p_socket->connect( address, c_connect_timeout );

                        if( !has_connected )
                        {
                           p_session_info->p_socket->close( );

                           msleep( 2500 );

                           DEBUG_TRACE( "[re-opening socket again]" );

                           if( p_session_info->p_socket->open( ) )
                              has_connected = p_session_info->p_socket->connect( address, c_connect_timeout );
                        }
                     }
                  }

                  if( !has_connected )
                     throw runtime_error( GDS( c_display_application_server_unavailable ) );
                  else
                  {
                     p_session_info->p_socket->set_no_delay( );

                     if( p_session_info->p_socket->write_line(
                      to_string( get_pid( ) ) + c_key_exchange_suffix, c_pid_timeout ) <= 0 )
                     {
                        string error;

                        if( p_session_info->p_socket->had_timeout( ) )
                           error = "timeout occurred trying to connect to server";
                        else
                           error = "application server has terminated this connection";

                        throw runtime_error( error );
                     }

                     string greeting;

                     if( p_session_info->p_socket->read_line( greeting, c_greeting_timeout ) <= 0 )
                     {
                        string error;

                        if( p_session_info->p_socket->had_timeout( ) )
                           error = "timeout occurred trying to connect to server";
                        else
                           error = "application server has terminated this connection";

                        throw runtime_error( error );
                     }

                     version_info ver_info;

                     if( get_version_info( greeting, ver_info ) != string( c_response_okay ) )
                        throw runtime_error( greeting );

                     // FUTURE: Some sort of "upgrade available" message should probably be displayed
                     // if the client is using an older minor protocol version than the app server.
                     bool is_older = false;

                     if( !check_version_info( ver_info, c_protocol_major_version, c_protocol_minor_version, &is_older ) )
                        throw runtime_error( "incompatible protocol version "
                         + ver_info.ver + " (expecting " + string( c_protocol_version ) + ")" );

                     // NOTE: After initial handshake exchange public keys (currently sets "none" as own).
                     string pubkey( c_none );
                     string slotx, pubkeyx, slotx_and_pubkeyx;

                     if( p_session_info->p_socket->read_line( slotx_and_pubkeyx, c_pubkey_timeout ) <= 0 )
                     {
                        string error;

                        if( p_session_info->p_socket->had_timeout( ) )
                           error = "timeout occurred trying to connect to server";
                        else
                           error = "application server has terminated this connection";

                        throw runtime_error( error );
                     }

                     string::size_type pos = slotx_and_pubkeyx.find( '-' );

                     if( pos != string::npos )
                     {
                        slotx = slotx_and_pubkeyx.substr( 0, pos );
                        pubkeyx = slotx_and_pubkeyx.substr( pos + 1 );
                     }

                     string slot( slotx );

                     if( !slot.empty( ) )
                        slot[ 0 ] = 'C';
                     else
                        slot = string( c_none );

                     string slot_and_pubkey( slot + '-' + pubkey );

                     p_session_info->p_socket->write_line( slot_and_pubkey, c_pubkey_timeout );

#ifdef SSL_SUPPORT
                     if( get_storage_info( ).use_tls )
                     {
                        if( !simple_command( *p_session_info, "starttls" ) )
                           throw runtime_error( "unable to starttls" );
                     }
#endif

                     if( !get_storage_info( ).rpc_password.empty( ) )
                     {
                        if( !simple_command( *p_session_info, "session_rpc_unlock " + get_storage_info( ).rpc_password ) )
                           throw runtime_error( "unable to unlock RPC access" );
                     }

                     string identity_info;

#ifdef SSL_SUPPORT
                     public_key pub_key( pubkeyx );
                     private_key priv_key;
#endif

                     string id_pwd( g_id_pwd );

                     if( g_seed.empty( ) )
                     {
#ifdef SSL_SUPPORT
                        if( !simple_command( *p_session_info, "identity -k=" + priv_key.get_public( ), &identity_info ) )
                           throw runtime_error( "unable to determine identity information" );
#else
                        if( !simple_command( *p_session_info, "identity -k=dummy", &identity_info ) )
                           throw runtime_error( "unable to determine identity information" );
#endif
                     }
                     else
                     {

                        if( g_seed == c_unlock )
                        {
#ifdef SSL_SUPPORT
                           if( !simple_command( *p_session_info, "identity -k=" + priv_key.get_public( )
                            + " " + priv_key.encrypt_message( pub_key, g_id_pwd, 0, true ), &identity_info ) )
                              throw runtime_error( "unable to unlock encrypted identity information" );
#else
                           if( !simple_command( *p_session_info, "identity -k=dummy " + quote( g_id_pwd ), &identity_info ) )
                              throw runtime_error( "unable to unlock encrypted identity information" );
#endif
                           was_unlock = true;

                           if( !file_exists( c_has_restored_file ) )
                           {
                              clear_key( g_id_pwd );
                              g_id_pwd.erase( );
                           }

                           g_seed.erase( );

                           msleep( 250 );
                        }
                        else
                        {
                           // NOTE: The seed is encrypted and sent as the identity (so that the application
                           // server will return the "encrypted identity") before then sending the password
                           // to unlock it.
                           string password( g_id_pwd );
                           string encrypted_seed( g_seed );

                           bool skip_save_encrypted = false;

                           if( password.length( ) >= c_encrypted_length )
                              skip_save_encrypted = true;

                           harden_key_with_hash_rounds( password, password, password, c_key_rounds_multiplier );

                           data_encrypt( encrypted_seed, encrypted_seed, password );

                           clear_key( password );

                           if( !simple_command( *p_session_info, "identity " + encrypted_seed, &identity_info ) )
                              throw runtime_error( "unable to set encrypted identity information" );

                           string::size_type pos = identity_info.find( '!' );

                           if( pos == string::npos )
                              throw runtime_error( "unexpected encrypted identity information '" + identity_info + "'" );

                           string current_identity( identity_info.substr( 0, pos ) );

                           if( g_reset_identity )
                           {
                              string entropy;

                              if( !simple_command( *p_session_info,
                               "crypto_seed -k=" + priv_key.get_public( )
                               + " " + priv_key.encrypt_message( pub_key, g_seed, 0, true ), &entropy ) )
                              {
                                 g_bad_seed = g_seed;
                                 g_seed.erase( );

                                 force_refresh = false;
                                 login_refresh = false;

                                 identity_failure = true;

                                 if( entropy.find( c_response_error_prefix ) == 0 )
                                 {
                                    display_error = false;

                                    g_seed_error = GDS( c_display_error ) + ": "
                                     + entropy.substr( strlen( c_response_error_prefix ) );
                                 }

                                 throw runtime_error( "unable to determine seed entropy information" );
                              }

                              if( !entropy.empty( ) )
                              {
                                 sha256 hash( entropy );

                                 string entropy_hash( sha1( hash.get_digest_as_string( ) ).get_digest_as_string( ) );

                                 if( g_id == entropy_hash )
                                    g_reset_identity = false;
                                 else
                                 {
                                    g_bad_seed = g_seed;
                                    g_seed.erase( );

                                    display_error = false;

                                    force_refresh = false;
                                    login_refresh = false;

                                    identity_failure = true;

                                    g_seed_error = GDS( c_display_error ) + ": " + GDS( c_display_incorrect_mnemonics );

                                    throw runtime_error( "incorrect mnemonics provided" );
                                 }
                              }
                           }

#ifdef SSL_SUPPORT
                           if( !simple_command( *p_session_info, "identity -k=" + priv_key.get_public( ) + " "
                            + priv_key.encrypt_message( pub_key, g_id_pwd, 0, true ) + " " + encrypted_seed, &identity_info ) )
                           {
                              g_bad_seed = g_seed;
                              g_seed.erase( );

                              force_refresh = false;
                              login_refresh = false;

                              identity_failure = true;

                              if( identity_info.find( c_response_error_prefix ) == 0 )
                              {
                                 display_error = false;

                                 g_seed_error = GDS( c_display_error ) + ": "
                                  + identity_info.substr( strlen( c_response_error_prefix ) );
                              }

                              throw runtime_error( "unable to set/update identity information" );
                           }
#else
                           if( !simple_command( *p_session_info, "identity -k=dummy "
                            + quote( g_id_pwd ) + " " + encrypted_seed, &identity_info ) )
                           {
                              g_bad_seed = g_seed;
                              g_seed.erase( );

                              force_refresh = false;
                              login_refresh = false;

                              identity_failure = true;

                              if( identity_info.find( c_response_error_prefix ) == 0 )
                              {
                                 display_error = false;

                                 g_seed_error = GDS( c_display_error ) + ": "
                                  + identity_info.substr( strlen( c_response_error_prefix ) );
                              }

                              throw runtime_error( "unable to set/update identity information" );
                           }
#endif
                           if( !skip_save_encrypted && !file_exists( eid_file_name ) )
                           {
                              ofstream outf( eid_file_name.c_str( ) );
                              outf << current_identity;

                              if( !using_anonymous )
                                 login_refresh = true;
                           }
                        }
                     }

                     pos = identity_info.find( '!' );

                     if( pos == string::npos )
                        throw runtime_error( "unexpected identity information '" + identity_info + "'" );

                     string sid, server_identity( identity_info.substr( 0, pos ) );
#ifdef SSL_SUPPORT
                     // NOTE: If mnemonics then will have been encrypted (confirmed by finding the salt marker).
                     if( server_identity.find( ':' ) != string::npos )
                        server_identity = priv_key.decrypt_message( pub_key, server_identity );
#endif

                     pos = identity_info.find( ' ', pos + 1 );

                     if( pos != string::npos )
                     {
                        sid = identity_info.substr( pos + 1 );
#ifdef SSL_SUPPORT
                        sid = priv_key.decrypt_message( pub_key, sid );
#endif
                     }

                     // NOTE: If a "sid" exists and matches what was expected (or "g_id"
                     // is empty) then make sure the "identity.txt" file has been saved.
                     if( !sid.empty( ) && ( g_id.empty( ) || matches_server_sid( sid ) ) )
                     {
                        g_unlock_attempts = 0;

                        if( g_id.empty( ) )
                        {
                           set_server_sid( sid );

                           g_id = server_identity;

                           if( !using_anonymous )
                              login_refresh = true;
                        }

                        if( !g_seed.empty( ) )
                        {
                           g_seed.erase( );
                           g_id_pwd.erase( );

                           has_set_identity = true;
                        }

                        if( !file_exists( id_file_name ) )
                        {
                           ofstream outf( id_file_name.c_str( ) );
                           outf << server_identity;
                        }

                        if( !file_exists( id_file_name ) )
                           throw runtime_error( "unable to create identity file (incorrect directory perms?)" );
                     }
                     else
                     {
                        if( was_unlock )
                        {
                           ++g_unlock_attempts;

                           // NOTE: Will allow resetting the identity password
                           // after any failed unlock attempt iff the password
                           // is a special value (the reset will still require
                           // the original mnemonics to be provided).
                           if( id_pwd == c_reset_identity_password )
                           {
                              g_unlock_attempts = 0;

                              g_reset_identity = true;

                              file_remove( eid_file_name );
                           }
                        }

                        string old_id( g_id );

                        g_id = server_identity;

                        string encrypted_identity;

                        if( file_exists( eid_file_name.c_str( ) ) )
                        {
                           encrypted_identity = buffer_file( eid_file_name.c_str( ) );

                           if( ( server_identity != encrypted_identity ) && !file_exists( id_file_name.c_str( ) ) )
                           {
                              login_refresh = true;

                              g_seed = string( c_unlock );
                           }
                        }

                        // NOTE: If is the first time but the identity matches what had already
                        // been saved previously then do not output the "system identity" form.
                        if( !g_seed.empty( ) || ( g_id == old_id ) )
                        {
                           set_server_sid( sid );

                           if( g_seed.empty( ) && file_exists( c_has_restored_file ) )
                              has_set_identity = true;

                           if( !g_seed.empty( ) )
                           {
                              ofstream outf( id_file_name.c_str( ) );
                              outf << server_identity;

                              login_refresh = true;

                              has_set_identity = true;

                              clear_key( g_seed );
                              g_seed.erase( );
                           }

                           if( !g_id.empty( ) && !file_exists( id_file_name ) )
                              throw runtime_error( "unable to create identity file (incorrect directory perms?)" );

                           if( was_unlock )
                              display_error = false;
                        }
                        else
                        {
                           bool needs_to_unlock = false;

                           if( !encrypted_identity.empty( ) )
                           {
                              if( g_unlock_attempts || ( server_identity == encrypted_identity ) )
                                 needs_to_unlock = true;
                           }

                           if( needs_to_unlock )
                           {
                              string unlock_html( g_unlock_html );

                              // NOTE: After three failed attempts
                              // will sleep (to ensure brute force
                              // cracking would be far too slow).
                              if( g_unlock_attempts >= 3 )
                              {
                                 msleep( 2500 );

                                 g_unlock_attempts = 0;
                              }

                              output_form( module_name, extra_content,
                               unlock_html, "", false, GDS( c_display_system_unlock ) );
                           }
                           else if( is_meta_module || g_is_blockchain_application )
                           {
                              is_identity_form = true;

                              string encrypted_seed( server_identity );

                              // NOTE: Special case where either a page refresh
                              // (or browser restart) occurs before the identiy
                              // password has been updated.
                              if( g_bad_seed.empty( )
                               && ( encrypted_seed.find( ' ' ) == string::npos ) )
                              {
                                 encrypted_seed.erase( );

                                 g_seed_error = GDS( c_display_original_mnemonics_required );
                              }

                              string identity_html( g_identity_html );

                              str_replace( identity_html,
                               c_identity_introduction_1, GDS( c_display_identity_introduction_1 ) );

                              str_replace( identity_html,
                               c_identity_introduction_2, GDS( c_display_identity_introduction_2 ) );

                              str_replace( identity_html, c_identity_error, g_seed_error );

                              if( g_reset_identity || !g_bad_seed.empty( ) )
                              {
                                 encrypted_seed = g_bad_seed;

                                 g_bad_seed.erase( );
                                 g_seed_error.erase( );
                              }

                              // NOTE: Encrypt mnemonics using the "unique_id" value as the key.
                              crypt_decoded( unique_id, encrypted_seed, false );

                              str_replace( identity_html, c_identity_mnemonics, encrypted_seed );

                              str_replace( identity_html, c_mnemonics, GDS( c_display_mnemonics ) );
                              str_replace( identity_html, c_confirm_identity, GDS( c_display_confirm_identity ) );

                              str_replace( identity_html, c_password, GDS( c_display_password ) );
                              str_replace( identity_html, c_verify_password, GDS( c_display_verify_password ) );

                              output_form( module_name, extra_content,
                               identity_html, "", false, GDS( c_display_confirm_identity ) );
                           }
                           else
                           {
                              string no_identity_html( g_no_identity_html );

                              str_replace( no_identity_html,
                               c_identity_missing, GDS( c_display_identity_missing_message ) );

                              str_replace( no_identity_html, c_identity_retry_message,
                               string_message( GDS( c_display_click_here_to_retry ),
                               make_pair( c_display_click_here_to_retry_parm_href,
                               "<a href=\"javascript:refresh( false )\">" ), "</a>" ) );

                              output_form( module_name, extra_content,
                               no_identity_html, "", false, GDS( c_display_identity_missing ) );
                           }

                           g_id = old_id;

                           has_output_form = true;
                        }
                     }

                     if( !has_output_form )
                     {
                        g_max_user_limit = ( size_t )atoi( identity_info.substr( pos + 1 ).c_str( ) );

                        string response;

                        if( !simple_command( *p_session_info, "storage_init " + get_storage_info( ).storage_name, &response, true ) )
                           throw runtime_error( response );

                        if( !get_storage_info( ).has_web_root( ) )
                        {
                           if( !simple_command( *p_session_info, "storage_web_root -expand", &response ) )
                              throw runtime_error( "unable to get web root from '" + get_storage_info( ).storage_name + "' storage" );

                           get_storage_info( ).web_root = response;

                           setup_directories( );
                        }

                        module_index_iterator mii;

                        for( mii = get_storage_info( ).modules_index.begin( );
                         mii != get_storage_info( ).modules_index.end( ); ++mii )
                        {
                           module_info& mod_info( *mii->second );
                           read_module_strings( mod_info, *p_session_info->p_socket );

                           if( !simple_command( *p_session_info, mod_info.name + "_ver", &response ) )
                              throw runtime_error( "unable to get version information for module '" + mod_info.name + "'" );

                           mod_info.title = response;
                        }

#ifdef USE_MULTIPLE_REQUEST_HANDLERS
                        connection_okay = true;
#else
                        g_has_connected = connection_okay = true;
#endif
                     }

                     if( has_set_identity )
                     {
                        if( sid == sha256( id_pwd ).get_digest_as_string( ) )
                           id_pwd = sid;

                        reset_admin_password( *p_session_info, module_id, mod_info, id_pwd, sid );

                        g_seed.erase( );
                        g_id_pwd.erase( );

                        g_restore_checksum.erase( );
                     }

                     clear_key( sid );

                     if( was_unlock )
                        display_error = false;
                  }
               }
               else
                  throw runtime_error( "unable to open socket" );
            }

            bool was_openid = false;

            if( connection_okay && ( cmd == c_cmd_open ) && !userhash.empty( ) )
            {
               if( fetch_user_record( id_for_login, module_id, module_name,
                mod_info, *p_session_info, true, true, username, userhash, password, unique_id ) )
               {
                  cmd = c_cmd_home;

                  was_openid = true;
                  temp_session = false;
                  using_anonymous = false;

                  p_session_info->user_id.erase( );
               }
            }

            if( connection_okay && mod_info.strings.empty( ) )
               read_module_strings( mod_info, *p_session_info->p_socket );

            if( connection_okay && ( cmd != c_cmd_open ) && p_session_info->user_id.empty( ) )
            {
               bool is_replacement_session = false;

               if( ( cmd != c_cmd_join ) && !mod_info.user_class_name.empty( ) )
               {
                  if( is_activation )
                  {
                     if( !file_exists( activation_file ) )
                        throw runtime_error( GDS( c_display_invalid_or_already_activated ) );

                     string field_list( mod_info.user_uid_field_id );
                     field_list += "," + mod_info.user_pwd_field_id;

                     string key_info( mod_info.user_uid_field_id );
                     key_info += " " + user;

                     bool login_okay = false;
                     pair< string, string > user_info;

                     if( !fetch_item_info( module_id, mod_info,
                      mod_info.user_class_id, key_info, field_list, "", "", *p_session_info, user_info ) )
                        throw runtime_error( "unexpected error occurred checking activation" );

                     if( user_info.first.empty( ) )
                        throw runtime_error( GDS( c_display_unknown_or_invalid_user_id ) );

                     vector< string > user_data;
                     split( user_info.second, user_data );

                     if( user_data.size( ) < 2 || user_data[ 0 ] != username )
                        throw runtime_error( "unexpected missing user information" );

                     string activate_password( password );

                     // NOTE: An inactive account is expected to have its password being the same as the
                     // user id so this is checked before activation will be performed and therefore the
                     // password provided to activate must not be the same as the user id. If activation
                     // has been repeated then just treat as a standard login if the password is correct.
                     if( user_data[ 0 ] != user_data[ 1 ] )
                     {
                        if( activate_password == hash_password( id_for_login + user_data[ 1 ] + user ) )
                           is_activation = false;
                        else
                           throw runtime_error( GDS( c_display_account_has_already_been_activated ) );
                     }

                     if( activate_password == lower( hash_password( id_for_login + user_data[ 0 ] + user ) ) )
                     {
                        force_refresh = true;
                        throw runtime_error( GDS( c_display_password_must_not_be_the_same_as_your_user_id ) );
                     }

                     vector< pair< string, string > > field_value_pairs;

                     string sid;
                     get_server_sid( sid );

                     string encrypted_password( data_encrypt( activate_password, sid ) );

                     clear_key( sid );

                     field_value_pairs.push_back( make_pair( mod_info.user_pwd_field_id, encrypted_password ) );
                     field_value_pairs.push_back( make_pair( mod_info.user_active_field_id, "1" ) );

                     p_session_info->user_key = data;

                     if( !perform_update( module_id, mod_info.user_class_id, data, field_value_pairs, *p_session_info ) )
                        throw runtime_error( "unexpected error occurred processing activation" );

                     temp_session = false;
                     file_remove( activation_file );

                     is_authorised = true;
                     using_anonymous = false;

                     password = activate_password;
                     p_session_info->user_id = user;
                     userhash = sha256( user + activate_password ).get_digest_as_string( );
                  }

                  if( using_anonymous )
                  {
                     p_session_info->user_module = module_name;
                     p_session_info->user_group = mod_info.anon_ginfo;
                  }
                  else
                  {
                     // NOTE: Avoid displaying an error if the user information has yet to be provided.
                     if( username.empty( ) && userhash.empty( ) )
                        display_error = false;

                     fetch_user_record( id_for_login,
                      module_id, module_name, mod_info, *p_session_info,
                      is_authorised || ( persistent == c_true ) || !base64_data.empty( ),
                      true, username, userhash, password, unique_id );

                     pwd_hash = p_session_info->user_pwd_hash;
                  }

                  if( !is_authorised )
                  {
                     if( !base64_data.empty( ) )
                     {
                        string decoded( base64::decode( base64_data ) );

                        crypt_decoded( pwd_hash, decoded );

                        size_t pos = decoded.find( c_dummy_param_prefix );

                        if( pos != string::npos )
                           parse_input( ( char* )decoded.c_str( ), decoded.size( ), input_data, '&', true );

                        password = input_data[ c_param_password ];

                        if( password != p_session_info->user_pwd_hash )
                           throw runtime_error( GDS( c_display_unknown_or_invalid_user_id ) );
                     }
                  }

                  bool permit_multiple_logins = false;
                  bool permit_module_switching = false;

                  if( login_opts.count( c_login_opt_allow_multiple ) )
                     permit_multiple_logins = true;

                  if( login_opts.count( c_login_opt_allow_switching ) )
                     permit_module_switching = true;

                  if( !permit_multiple_logins
                   && ( ( permit_module_switching && has_user_session_info( p_session_info->user_id ) )
                   || ( !permit_module_switching
                   && has_user_session_info( p_session_info->user_id, p_session_info->user_module.c_str( ) ) ) ) )
                  {
                     // NOTE: If a new (persistent) session has started but an old one still exists then remove
                     // the old one (i.e. assume the browser window was closed or that the browser had crashed).
#ifdef ALWAYS_REPLACE_SESSION
                     if( created_session )
#else
                     if( created_session && ( persistent == c_true ) )
#endif
                     {
                        is_replacement_session = true;

                        destroy_user_session_info( p_session_info->user_id,
                         permit_module_switching ? 0 : p_session_info->user_module.c_str( ) );
                     }
                     else
                        throw runtime_error( GDS( c_display_you_are_currently_logged_in ) );
                  }

                  if( !using_anonymous )
                     clear_unique( input_data );

                  if( !p_session_info->user_key.empty( ) && !mod_info.user_select_field.empty( ) )
                  {
                     string class_id( mod_info.user_class_id );

                     if( mod_info.user_select_cfield.empty( ) )
                        class_id += ":" + mod_info.user_select_field;
                     else
                        class_id += ":_" + mod_info.user_select_cfield;

                     string key_info( p_session_info->user_key );
                     key_info += ':';

                     if( !mod_info.user_select_ofield.empty( ) )
                     {
                        key_info += mod_info.user_select_ofield;
                        key_info += ' ';
                     }

                     string field_list;

                     if( !mod_info.user_select_uo_field.empty( ) )
                        field_list = mod_info.user_select_uo_field;

                     if( !mod_info.user_select_sl_field.empty( ) )
                     {
                        if( !field_list.empty( ) )
                           field_list += ",";
                        field_list += mod_info.user_select_sl_field;
                     }

                     if( !field_list.empty( ) )
                        field_list += ",";
                     field_list += mod_info.user_select_pfield;

                     if( !fetch_list_info( module_id,
                      mod_info, class_id, *p_session_info, false, 0, key_info,
                      field_list, "", "", "", "", p_session_info->select_data, "" ) )
                        throw runtime_error( "unexpected error occurred processing select info" );

                     if( mod_info.user_select_ofield.empty( ) )
                        sort_row_data_manually( p_session_info->select_data );

                     if( !mod_info.user_select_uo_field.empty( ) || !mod_info.user_select_sl_field.empty( ) )
                     {
                        deque< pair< string, string > >::iterator i;
                        for( i = p_session_info->select_data.begin( ); i != p_session_info->select_data.end( ); ++i )
                        {
                           string key_ver_rev_state_and_type_info = i->first;
                           string key( key_ver_rev_state_and_type_info.substr( 0, key_ver_rev_state_and_type_info.find( ' ' ) ) );

                           string column_values( i->second );
                           if( !mod_info.user_select_uo_field.empty( ) )
                           {
                              string::size_type pos = column_values.find( "," );
                              if( pos == string::npos )
                                 throw runtime_error( "unexpected column values '" + column_values + "' for user_other query result" );

                              // KLUDGE: Prevent "user other" from being aliased as "admin" in case "user other" is
                              // actually being used as a "user" restriction (this situation should be determined).
                              if( column_values.substr( 0, pos ) == c_admin_user_key )
                                 p_session_info->other_aliases.insert( make_pair( key, p_session_info->user_key ) );
                              else
                                 p_session_info->other_aliases.insert( make_pair( key, column_values.substr( 0, pos ) ) );

                              column_values.erase( 0, pos + 1 );
                           }

                           if( !mod_info.user_select_sl_field.empty( ) )
                           {
                              string::size_type pos = column_values.find( "," );
                              if( pos == string::npos )
                                 throw runtime_error( "unexpected column values '" + column_values + "' for user_other query result" );

                              p_session_info->other_slevels.insert( make_pair( key, column_values.substr( 0, pos ) ) );
                              column_values.erase( 0, pos + 1 );
                           }

                           i->second = column_values;
                        }
                     }
                  }

                  if( !p_session_info->user_key.empty( ) && !mod_info.user_qlink_class_id.empty( ) )
                     fetch_user_quick_links( mod_info, *p_session_info );
               }

               if( !temp_session )
               {
                  if( p_session_info->user_id.empty( ) && p_session_info->user_name.empty( ) )
                     throw runtime_error( GDS( c_display_unknown_or_invalid_user_id ) );

                  has_just_logged_in = true;
                  p_session_info->logged_in = true;

                  p_session_info->is_openid = was_openid;

                  string path( c_files_directory );
                  path += "/" + string( c_tmp_directory );
                  path += "/" + session_id;

                  bool rc;
                  create_dir( path, &rc, ( dir_perms )c_web_files_dir_perm_val, WEB_FILES_UMASK );

                  if( !is_non_persistent( session_id ) )
                     p_session_info->is_persistent = true;

                  if( !is_replacement_session )
                  {
                     LOG_TRACE( "[login: "
                      + ( p_session_info->user_name.empty( ) ? p_session_info->user_id : p_session_info->user_name )
                      + " at " + date_time::local( ).as_string( true, false ) + " from " + p_session_info->ip_addr + "]" );
                  }

                  if( p_session_info->user_has_auth == c_true_value )
                     p_session_info->needs_pin = true;

                  if( cmd == c_cmd_open )
                     cmd = c_cmd_home;
               }
            }
         }

         if( !has_output_form )
         {
            string act( input_data[ c_param_act ] );
            string app( input_data[ c_param_app ] );
            string chk( input_data[ c_param_chk ] );
            string cls( input_data[ c_param_cls ] );
            string pin( input_data[ c_param_pin ] );
            string cont( input_data[ c_param_cont ] );
            string exec( input_data[ c_param_exec ] );
            string vtab( input_data[ c_param_vtab ] );
            string vtabc( input_data[ c_param_vtabc ] );
            string extra( input_data[ c_param_extra ] );
            string field( input_data[ c_param_field ] );
            string flags( input_data[ c_param_flags ] );
            string ident( input_data[ c_param_ident ] );
            string qlink( input_data[ c_param_qlink ] );
            string bcount( input_data[ c_param_bcount ] );
            string scrollx( input_data[ c_param_scrollx ] );
            string scrolly( input_data[ c_param_scrolly ] );
            string special( input_data[ c_param_special ] );
            string uselect( input_data[ c_param_uselect ] );
            string listarg( input_data[ c_param_listarg ] );
            string listvar( input_data[ c_param_listvar ] );
            string listinfo( input_data[ c_param_listinfo ] );
            string listsort( input_data[ c_param_listsort ] );
            string listsrch( input_data[ c_param_listsrch ] );
            string findinfo( input_data[ c_param_findinfo ] );
            string fieldlist( input_data[ c_param_fieldlist ] );
            string listextra( input_data[ c_param_listextra ] );
            string uselextra( input_data[ c_param_uselextra ] );
            string userfetch( input_data[ c_param_userfetch ] );
            string quicklink( input_data[ c_param_quicklink ] );
            string keepchecks( input_data[ c_param_keepchecks ] );
            string extrafields( input_data[ c_param_extrafields ] );
            string extravalues( input_data[ c_param_extravalues ] );

            set< string > url_opts;

            if( !get_storage_info( ).url_opts.empty( ) )
               split( get_storage_info( ).url_opts, url_opts, '+' );

            bool use_url_checksum = ( url_opts.count( c_url_opt_use_checksum ) > 0 );

            bool is_checksummed_home = ( ( cmd == c_cmd_home ) && !uselect.empty( ) );

            if( use_url_checksum && !cmd.empty( ) && ( cmd != c_cmd_join ) && ( cmd != c_cmd_open )
             && ( is_checksummed_home || ( ( cmd != c_cmd_home ) && ( cmd != c_cmd_quit ) && ( cmd != c_cmd_login )
             && ( cmd != c_cmd_status ) && ( cmd != c_cmd_view || ( ( act != c_act_cont ) && ( act != c_act_undo ) ) ) ) ) )
            {
               string prefix;

               if( act != c_act_qlink )
                  prefix += act;

               string identity_values( cmd + data );

               string checksum_values;

               if( cmd == c_cmd_home )
               {
                  // NOTE: The uselect itself cannot be used in the checkum when it is being changed (as the
                  // checksum value is worked out prior to the runtime selection) so uselextra is being used
                  // instead. It's last value is remembered to ensure that direct tampering of uselect can't
                  // succeed with a new (possibly random) uselextra value.
                  checksum_values = session_id + uselextra;

                  string user_other( p_session_info->user_other );

                  if( user_other.empty( ) )
                     user_other = c_user_other_none;

                  if( !uselect.empty( )
                   && uselextra == p_session_info->last_uselextra && uselect != user_other )
                     checksum_values += "invalid";
               }
               else
               {
                  checksum_values = prefix + cmd + data + exec + ident;

                  if( !act.empty( ) || uselextra.empty( ) )
                     checksum_values += uselect;
                  else
                     checksum_values += uselextra;
               }

               // NOTE: For actions (and for new records) a serial number is being used to both prevent
               // URL tampering and replaying (also a defense against any browser repeating POST bugs).
               if( ( !act.empty( ) && ( act != c_act_view ) )
                || ( ( cmd == c_cmd_view ) && data.find( c_new_record ) == 0 ) )
                  checksum_values += to_string( p_session_info->checksum_serial++ );

               string name, other_values;

               for( int i = 0; i < 10; i++ )
               {
                  name = c_list_prefix;
                  name += c_prnt_suffix;
                  name += to_string( i );

                  if( input_data.count( name ) )
                     other_values += input_data.find( name )->second;

                  name = c_list_prefix;
                  name += c_rest_suffix;
                  name += to_string( i );

                  if( input_data.count( name ) )
                     other_values += input_data.find( name )->second;
               }

               if( !hashval.empty( ) )
                  checksum_values += c_hash_suffix;

               string hash_values( p_session_info->hashval_prefix + findinfo + listsrch + other_values );

               bool ignore_checksum = false;

               // NOTE: Special case where the admin password is the identity.
               if( !chksum.empty( ) && ( chksum == g_restore_checksum ) )
               {
                  cmd = c_cmd_home;

                  login_refresh = true;
                  ignore_checksum = true;

                  string sid;
                  get_server_sid( sid );

                  g_restore_checksum.erase( );

                  reset_admin_password( *p_session_info, module_id, mod_info, sid, sid );
               }

               // NOTE: For list searches part of an SHA1 hash of the "findinfo" parameter
               // is used to ensure the search data hasn't been changed via URL tampering.
               if( !ignore_checksum && !has_just_logged_in
                && ( ( !hashval.empty( ) && ( hashval != get_hash( hash_values ) ) )
                || ( ( chksum != get_checksum( identity_values ) )
                && ( chksum != get_checksum( *p_session_info, checksum_values ) ) ) ) )
               {
#ifdef DEBUG
                  for( map< string, string >::const_iterator ci = input_data.begin( ); ci != input_data.end( ); ++ci )
                     DEBUG_TRACE( "Input[ " + ci->first + " ] => " + ci->second );
#endif
                  // NOTE: If a session has just been created then assumes that the
                  // invalid URL was actually due to an already "timed out" session.
                  if( using_anonymous || !created_session )
                     throw runtime_error( GDS( c_display_invalid_url ) );
                  else
                     throw runtime_error( GDS( c_display_your_session_has_been_timed_out ) );
               }
            }

            p_session_info->last_uselextra = uselextra;

            string new_field_info;
            string new_value_info;

            map< string, string > new_field_and_values;

            bool was_invalid = false;

            if( input_data.count( c_param_newflds ) )
               new_field_info = input_data[ c_param_newflds ];

            if( input_data.count( c_param_newvals ) )
               new_value_info = input_data[ c_param_newvals ];

            if( !new_field_info.empty( ) )
            {
               while( true )
               {
                  string::size_type pos = new_field_info.find( "," );
                  string next_field( new_field_info.substr( 0, pos ) );

                  if( pos != string::npos )
                     new_field_info.erase( 0, pos + 1 );

                  pos = new_value_info.find( "," );
                  string next_value( new_value_info.substr( 0, pos ) );

                  if( pos != string::npos )
                     new_value_info.erase( 0, pos + 1 );

                  if( next_value == c_parent_extra_user )
                     next_value = p_session_info->user_key;
                  else if( next_value == c_parent_extra_group )
                     next_value = p_session_info->user_group;

                  new_field_and_values.insert( make_pair( next_field, next_value ) );

                  if( pos == string::npos )
                     break;
               }
            }

            int vtab_num = 1;

            if( !vtab.empty( ) )
               vtab_num = atoi( vtab.c_str( ) );

            string new_key;
            string error_message;
            string user_home_info;

            string server_command( cmd );

            bool is_new_record = false;

            if( !data.empty( ) && data[ 0 ] == ' ' )
               is_new_record = true;

            bool module_access_denied = false;

            if( module_name != p_session_info->user_module )
               module_access_denied = true;

            bool allow_module_switching = false;

            if( login_opts.count( c_login_opt_allow_switching ) )
            {
               module_access_denied = false;
               allow_module_switching = true;
               p_session_info->user_module = module_name;
            }

            if( !mod_info.perm.empty( ) && !p_session_info->user_perms.count( mod_info.perm ) )
               throw runtime_error( GDS( c_display_permission_denied ) );

            if( ( cmd == c_cmd_login ) || module_access_denied )
            {
               act.erase( );
               cmd.erase( );

               server_command.erase( );
            }
            else if( cmd == c_cmd_pview )
               interface_file = string( c_printview_file );
            else if( cmd == c_cmd_plist )
               interface_file = string( c_printlist_file );

#ifdef DEBUG
            for( map< string, string >::const_iterator ci = input_data.begin( ); ci != input_data.end( ); ++ci )
               DEBUG_TRACE( "Input[ " + ci->first + " ] => " + ci->second );
#endif

            // NOTE: When logging in after having previously logged out the initial URL will actually
            // be the "quit" command so now verify that the session being logged out matches the same
            // one that the quit was issued from (via "extra").
            if( ( cmd == c_cmd_quit ) && ( extra != session_id ) )
            {
               cmd = c_cmd_home;
               server_command.erase( );
            }

            if( cmd.empty( ) )
               cmd = c_cmd_home;

            bool had_send_or_recv_error = false;
            bool record_not_found_error = false;

            // NOTE: As the user's record could be changed at any time (and especially
            // to ensure a session cannot continue if its account is now de-activated)
            // the user information is checked again here.
            try
            {
               if( !has_just_logged_in && p_session_info->logged_in
                && ( ( cmd == c_cmd_home ) || ( cmd == c_cmd_pwd ) || ( cmd == c_cmd_view )
                || ( cmd == c_cmd_pview ) || ( cmd == c_cmd_list ) || ( cmd == c_cmd_plist ) ) )
                  fetch_user_record( g_id, module_id, module_name, mod_info,
                   *p_session_info, is_authorised, false, p_session_info->user_id, "", "", "" );
            }
            catch( ... )
            {
               // NOTE: If an exception is thrown reading the user information then it is
               // assumed that the user is no longer valid to be logged in (perhaps being
               // due to de-activation or module permission changes). It makes sense that
               // at this point the user would need to try and login again so the session
               // is being killed in the same way as would occur for an invalid login.
               p_session_info->logged_in = false;
               created_session = true;

               throw;
            }

            // NOTE: The PIN is compared against the current and previous (if has already checked)
            // expected value to allow for a little bit more margin with the device time accuracy.
            if( !pin.empty( )
             && ( ( pin == p_session_info->user_pin_value )
             || ( pin == p_session_info->last_user_pin_value ) ) )
               p_session_info->needs_pin = false;

            // NOTE: If not logging out and needs either a PIN or to change
            // password then modify the command to handle either situation.
            if( cmd != c_cmd_quit )
            {
               if( p_session_info->needs_pin )
                  cmd = c_cmd_home;
               else if( p_session_info->change_pwd_tm
                && ( unix_time( ) >= p_session_info->change_pwd_tm ) )
                  cmd = c_cmd_pwd;
            }

            // NOTE: For a save or continue edit action it is expected that a field list and
            // a corresponding set of user values will be provided. If a "view" contains one
            // or more "always_editable" fields then these will be passed via the field list
            // for an "edit", "view" or any "exec" action.
            map< string, string > user_field_info;
            map< string, string > extra_field_info;

            if( !fieldlist.empty( ) && ( ( act == c_act_edit )
             || ( act == c_act_cont ) || ( act == c_act_save ) || ( act == c_act_exec ) || ( act == c_act_view ) ) )
            {
               vector< string > field_ids;
               vector< string > field_values;

               split( fieldlist, field_ids );

               if( act != c_act_exec )
                  raw_split( app, field_values );
               else
                  raw_split( extra, field_values );

               if( field_ids.size( ) != field_values.size( ) )
                  throw runtime_error( "unexpected field_ids and field_values size mismatch" );

               for( size_t i = 0; i < field_ids.size( ); i++ )
                  user_field_info.insert( make_pair( field_ids[ i ], field_values[ i ] ) );

               if( !extrafields.empty( ) )
               {
                  vector< string > extra_field_ids;
                  vector< string > extra_field_values;

                  split( extrafields, extra_field_ids );

                  raw_split( extravalues, extra_field_values );

                  if( extra_field_ids.size( ) != extra_field_values.size( ) )
                     throw runtime_error( "unexpected extra_field_ids and extra_field_values size mismatch" );

                  for( size_t i = 0; i < extra_field_ids.size( ); i++ )
                  {
                     user_field_info.insert( make_pair( extra_field_ids[ i ], extra_field_values[ i ] ) );
                     extra_field_info.insert( make_pair( extra_field_ids[ i ], extra_field_values[ i ] ) );
                  }
               }
            }

            set< string > selected_records;

            if( !app.empty( ) && act != c_act_save && act != c_act_cont && act != c_act_view )
            {
               vector< string > record_keys;

               split( app, record_keys );

               for( size_t i = 0; i < record_keys.size( ); i++ )
               {
                  string::size_type pos = record_keys[ i ].find( ' ' ); // i.e. for optional version information
                  selected_records.insert( record_keys[ i ].substr( 0, pos ) );
               }
            }

            set< string > menu_opts;
            if( !get_storage_info( ).menu_opts.empty( ) )
               split( get_storage_info( ).menu_opts, menu_opts, '+' );

            is_vertical = menu_opts.count( c_menu_opt_use_vertical_menu );

            map< string, string > list_search_text;
            map< string, string > list_search_values;

            if( !listsrch.empty( ) )
               list_search_text.insert( make_pair( c_param_listsrch, listsrch ) );

            for( int i = 0; i < 10; i++ )
            {
               string child_name( c_list_prefix );

               child_name += ( '0' + i );
               child_name += c_srch_suffix;

               if( input_data.count( child_name ) )
                  list_search_text.insert( make_pair( child_name, input_data[ child_name ] ) );
            }

            map< string, string > list_selections;

            // NOTE: Any selections found are put into a map here to simplify finding them later.
            for( int i = 0; i < 10; i++ )
            {
               string name( c_list_prefix );

               name += c_prnt_suffix;
               name += ( '0' + i );

               if( input_data.count( name ) )
                  list_selections.insert( make_pair( name, input_data[ name ] ) );

               string child_list( c_list_prefix );

               child_list += ( '0' + i );

               for( int j = 0; j < 10; j++ )
               {
                  string child_name( child_list + c_prnt_suffix );

                  child_name += ( '0' + j );

                  if( input_data.count( child_name ) )
                     list_selections.insert( make_pair( child_name, input_data[ child_name ] ) );
               }
            }

            // NOTE: In the same manner any field restrictions are found are also put into the same map.
            for( int i = 0; i < 10; i++ )
            {
               string name( c_list_prefix );

               name += c_rest_suffix;
               name += ( '0' + i );

               if( input_data.count( name ) )
                  list_selections.insert( make_pair( name, input_data[ name ] ) );

               string child_list( c_list_prefix );

               child_list += ( '0' + i );

               for( int j = 0; j < 10; j++ )
               {
                  string child_name( child_list + c_rest_suffix );
                  child_name += ( '0' + j );

                  if( input_data.count( child_name ) )
                     list_selections.insert( make_pair( child_name, input_data[ child_name ] ) );
               }
            }

            // NOTE: These special extra variables are used to indicate a restrict check/uncheck
            // has occurred for a view child list when fetching the parent view.
            map< string, string > view_extra_vars;

            for( int i = 0; i < 10; i++ )
            {
               string name( c_vext_prefix );

               name += ( '0' + i );

               if( input_data.count( name ) )
                  view_extra_vars.insert( make_pair( name, input_data[ name ] ) );
            }

            // NOTE: If the user is able to select their group or "other" then assign it here.
            if( !mod_info.user_select_field.empty( )
             && ( mod_info.user_select_perm.empty( )
             || p_session_info->user_perms.count( mod_info.user_select_perm ) ) )
            {
               if( mod_info.user_select_field == mod_info.user_group_field_id )
               {
                  if( has_just_logged_in || uselect.empty( ) )
                     uselect = p_session_info->default_user_group;
                  else
                  {
                     if( uselect == c_user_other_none )
                        p_session_info->user_group.erase( );
                     else
                        p_session_info->user_group = uselect;
                  }
               }
               else if( mod_info.user_select_field == mod_info.user_other_field_id )
               {
                  if( has_just_logged_in || uselect.empty( ) )
                     uselect = p_session_info->default_user_other;
                  else
                  {
                     if( uselect == c_user_other_none )
                        p_session_info->user_other.erase( );
                     else
                        p_session_info->user_other = uselect;
                  }
               }
            }

            view_source view;
            list_source list;
            list_source olist;

            map< string, list_source > home_lists;

            string oident( ident );

            map< string, row_error_container > child_row_errors;

            view_info_const_iterator vici = mod_info.view_info.end( );
            list_info_const_iterator lici = mod_info.list_info.end( );

            bool skip_force_fields = false;

            bool is_in_edit = false;

            if( ( act == c_act_edit ) || ( act == c_act_save )
             || ( act == c_act_cont ) || ( is_new_record && ( act.empty( ) || ( act == c_act_undo ) ) ) )
               is_in_edit = true;

            if( ( cmd == c_cmd_view ) || ( cmd == c_cmd_pview ) )
            {
               vici = mod_info.view_info.find( ident );
               if( vici == mod_info.view_info.end( ) )
                  throw runtime_error( "unknown view '" + ident + "'" );

               view.vici = vici;

               setup_view_fields( view, *vici->second,
                mod_info, *p_session_info, ident, login_opts, module_id, module_ref, is_in_edit, is_new_record );
            }
            else if( ( cmd == c_cmd_home ) || ( cmd == c_cmd_list ) || ( cmd == c_cmd_plist ) )
            {
               if( cmd == c_cmd_home )
               {
                  for( lici = mod_info.list_info.begin( ); lici != mod_info.list_info.end( ); ++lici )
                  {
                     list_source next_list;

                     if( ( lici->second->type == c_list_type_home )
                      || ( using_anonymous && ( lici->second->type == c_list_type_home_anon ) ) )
                     {
                        next_list.module_id = module_id;
                        next_list.module_ref = module_ref;

                        next_list.lici = lici;
                        setup_list_fields( next_list, "", module_name, *p_session_info, false );

                        home_lists.insert( make_pair( mod_info.get_string( next_list.name ), next_list ) );
                     }
                  }
               }
               else
               {
                  lici = mod_info.list_info.find( ident );

                  if( lici == mod_info.list_info.end( ) )
                     throw runtime_error( "Unknown list id '" + ident + "'." );

                  olist.module_id = module_id;
                  olist.module_ref = module_ref;

                  olist.lici = lici;

                  string pkey;

                  if( ( lici->second )->type == c_list_type_group )
                     pkey = p_session_info->user_group;
                  else if( ( lici->second )->type == c_list_type_user || ( lici->second )->type == c_list_type_user_child )
                     pkey = p_session_info->user_key;

                  setup_list_fields( olist, pkey, module_name, *p_session_info, cmd == c_cmd_plist );

                  // NOTE: Repeat the setup as the current list "variation" may not be the original list.
                  if( !listvar.empty( ) )
                     ident = listvar;

                  lici = mod_info.list_info.find( ident );

                  if( lici == mod_info.list_info.end( ) )
                     throw runtime_error( "Unknown list id '" + ident + "'." );

                  list.module_id = module_id;
                  list.module_ref = module_ref;

                  list.lici = lici;

                  pkey.erase( );

                  if( ( lici->second )->type == c_list_type_group )
                     pkey = p_session_info->user_group;
                  else if( ( lici->second )->type == c_list_type_user || ( lici->second )->type == c_list_type_user_child )
                     pkey = p_session_info->user_key;

                  setup_list_fields( list, pkey, module_name, *p_session_info, cmd == c_cmd_plist );
               }
            }

            if( !act.empty( ) )
            {
               if( cmd == c_cmd_view )
               {
                  if( act == c_act_save )
                  {
                     if( error_message.empty( ) )
                     {
                        if( !view.create_user_key_field.empty( )
                         && !extra_field_info.count( view.create_user_key_field ) )
                           extra_field_info.insert( make_pair( view.create_user_key_field, p_session_info->user_key ) );

                        save_record( module_id, flags, app, chk, field, extra, exec, cont, fieldlist,
                         is_new_record, new_field_and_values, extra_field_info, vici, view, vtab_num,
                         *p_session_info, act, data, new_key, error_message, was_invalid, had_send_or_recv_error );

                        // NOTE: When initially determining the "view source" it had to be assumed that editing
                        // was still occurring (in order to have all the required field information perform the
                        // save). If the save was successful then the "view source" needs to now be replaced by
                        // the non-edit version.
                        if( error_message.empty( ) )
                        {
                           view_source non_edit_view;

                           non_edit_view.vici = view.vici;

                           setup_view_fields( non_edit_view, *vici->second,
                            mod_info, *p_session_info, ident, login_opts, module_id, module_ref, false, false );

                           is_in_edit = false;

                           view = non_edit_view;
                        }

                        // NOTE: If the record saved was a user record then if not admin assume a
                        // user has changed their own details (so re-read the user record details).
                        if( error_message.empty( ) && !had_send_or_recv_error
                         && mod_info.user_class_id == view.cid && !p_session_info->is_admin_user )
                           fetch_user_record( g_id, module_id, module_name, mod_info,
                            *p_session_info, is_authorised, false, p_session_info->user_id, "", "", "" );
                     }
                  }
                  else if( act == c_act_cont )
                     act = c_act_edit;
                  else if( ( act == c_act_del ) || ( act == c_act_rdel ) || ( act == c_act_link ) || ( act == c_act_exec ) )
                  {
                     if( act == c_act_exec )
                     {
                        set< string > fields;

                        split( fieldlist, fields );

                        for( size_t i = 0; i < view.user_force_fields.size( ); i++ )
                        {
                           if( !fields.count( view.user_force_fields[ i ] ) && user_field_info.count( view.user_force_fields[ i ] ) )
                           {
                              if( !fieldlist.empty( ) )
                                 fieldlist += ",";

                              fieldlist += view.user_force_fields[ i ];

                              if( !fieldlist.empty( ) )
                                 extra += ",";

                              extra += escaped( user_field_info[ view.user_force_fields[ i ] ], "," );
                           }
                        }
                     }

                     if( !perform_action( view.module, cls, act, app, field,
                      fieldlist, exec, extra, child_row_errors[ listarg ], *p_session_info ) )
                        had_send_or_recv_error = true;

                     // NOTE: Handle an error/response (is stored as the first record in "child_row_errors").
                     if( ( app.empty( ) || listarg.empty( ) ) && child_row_errors[ listarg ].size( ) )
                     {
                        error_message = escape_markup( child_row_errors[ listarg ].begin( )->second );

                        // NOTE: If the output from an "exec" is in the form "{xxx}" then it used assumed that
                        // the "xxx" is the key of another same class record which becomes the current record.
                        if( !is_in_edit && ( act == c_act_exec ) && error_message.size( ) > 2
                         && error_message[ 0 ] == '{' && error_message[ error_message.size( ) - 1 ] == '}' )
                        {
                           string s( error_message.substr( 1, error_message.size( ) - 2 ) );

                           error_message.erase( );

                           // NOTE: The "key" returned can actually instead be a specialised instruction.
                           if( s == "@no_send" )
                              skip_force_fields = true;
                           else
                           {
                              data = s;

                              if( use_url_checksum )
                              {
                                 string checksum_values( c_cmd_view + data + ident + uselect );
                                 string new_checksum_value = get_checksum( *p_session_info, checksum_values );

                                 extra_content_func += "query_update( '" + to_string( c_param_data ) + "', '" + data + "', true );\n";
                                 extra_content_func += "query_update( '" + to_string( c_param_chksum ) + "', '" + new_checksum_value + "', true );\n";
                              }
                              else
                                 extra_content_func += "query_update( '" + to_string( c_param_data ) + "', '" + data + "', true );\n";
                           }
                        }
                     }
                  }
                  else if( act == c_act_qlink )
                     add_quick_link( module_ref, cmd, data, extra, "", "", oident,
                      uselect, error_message, had_send_or_recv_error, mod_info, *p_session_info );
               }
               else if( cmd == c_cmd_home || cmd == c_cmd_list )
               {
                  if( act == c_act_qlink )
                     add_quick_link( module_ref, cmd, findinfo, quicklink, listsrch, listsort, oident,
                      uselect, error_message, had_send_or_recv_error, mod_info, *p_session_info, &list_selections );
                  else if( !perform_action( list.module,
                   list.cid, act, app, field, "", exec, extra, list.row_errors, *p_session_info ) )
                     had_send_or_recv_error = true;

                  // NOTE: Handle an error/response (is stored as the first record in "row_errors").
                  if( app.empty( ) && list.row_errors.size( ) )
                     error_message = escape_markup( list.row_errors.begin( )->second );
               }

               // NOTE: Any action that performed on the "quick link" class will result in
               // the quick links being re-fetched (i.e. assume it's the user's own links).
               if( ( cmd == c_cmd_view || cmd == c_cmd_list )
                && !mod_info.user_qlink_class_id.empty( ) && !p_session_info->quick_link_data.empty( ) )
                  fetch_user_quick_links( mod_info, *p_session_info );
            }

            // NOTE: If the user has selected a "timezone" then work out the current local
            // time (for display as well as for use in "defcurrent" date and time fields).
            if( p_session_info->tz_name.empty( ) )
               p_session_info->tz_abbr = p_session_info->current_dtm = string( );
            else
            {
               string cmd, time_info;

               cmd = "utc_to_local " + p_session_info->tz_name
                + " " + date_time::standard( ).as_string( e_time_format_hhmm );

               if( simple_command( *p_session_info, cmd, &time_info ) )
               {
                  string::size_type pos = time_info.rfind( ' ' );
                  p_session_info->current_dtm = time_info.substr( 0, pos );

                  if( pos != string::npos )
                     p_session_info->tz_abbr = time_info.substr( pos + 1 );
               }
            }

            if( cmd == c_cmd_pwd )
            {
               server_command.erase( );

               if( input_data.count( c_param_newpwd ) )
               {
                  string new_password( input_data[ c_param_newpwd ] );
                  string old_password( input_data[ c_param_password ] );

                  if( new_password == hash_password( g_id + p_session_info->user_id + p_session_info->user_id ) )
                     error_message = string( c_response_error_prefix ) + GDS( c_display_password_must_not_be_the_same_as_your_user_id );
                  else if( old_password != p_session_info->user_pwd_hash && p_session_info->pwd_encrypted )
                     error_message = string( c_response_error_prefix ) + GDS( c_display_old_password_is_incorrect );
                  else
                  {
                     string sid;
                     get_server_sid( sid );

                     string encrypted_new_password( data_encrypt( new_password, sid ) );

                     sid.clear( );

                     vector< pair< string, string > > pwd_field_value_pairs;
                     pwd_field_value_pairs.push_back( make_pair( mod_info.user_pwd_field_id, encrypted_new_password ) );

                     pwd_field_value_pairs.push_back( make_pair(
                      mod_info.user_hash_field_id, sha256( p_session_info->user_id + new_password ).get_digest_as_string( ) ) );

                     string user_crypt_key;
                     if( !mod_info.user_crypt_field_id.empty( ) )
                     {
                        if( p_session_info->user_crypt.empty( ) )
                           user_crypt_key = uuid( ).as_string( );
                        else
                           user_crypt_key = data_decrypt( p_session_info->user_crypt, old_password );

                        user_crypt_key = data_encrypt( user_crypt_key, new_password );

                        pwd_field_value_pairs.push_back( make_pair( mod_info.user_crypt_field_id, user_crypt_key ) );
                     }

                     if( !perform_update( module_id, mod_info.user_class_id,
                      p_session_info->user_key, pwd_field_value_pairs, *p_session_info, &error_message ) )
                     {
                        if( error_message.empty( ) )
                           throw runtime_error( "unexpected server error occurred." );
                     }

                     p_session_info->pwd_encrypted = true;
                     p_session_info->user_crypt = user_crypt_key;
                     p_session_info->user_pwd_hash = new_password;
                  }
               }
               else if( !persistent.empty( ) )
               {
                  if( persistent == c_true )
                  {
                     remove_non_persistent( session_id );
                     p_session_info->is_persistent = true;
                  }
                  else
                  {
                     add_non_persistent( session_id );
                     p_session_info->is_persistent = false;
                  }
               }
            }
            else if( cmd == c_cmd_join || cmd == c_cmd_open )
               server_command.erase( );

            if( error_message.length( ) > strlen( c_response_error_prefix )
             && error_message.substr( 0, strlen( c_response_error_prefix ) ) == c_response_error_prefix )
               error_message = GDS( c_display_error ) + ": "
                + error_message.substr( strlen( c_response_error_prefix ) );

            setup_gmt_and_dtm_offset( input_data, *p_session_info );

            process_fcgi_request( mod_info, p_session_info, cmd, act, data, field,
             extra, qlink, password, pin, raddr, uselect, userhash, userfetch, exec, ident, oident,
             special, back, vtabc, bcount, hashval, cont, listarg, listvar, listinfo, listsort, listsrch, listextra,
             findinfo, app, keepchecks, new_key, session_id, module_id, module_ref, module_name, scrollx, scrolly,
             server_command, menu_opts, selected_records, input_data, view, vici, view_extra_vars, list, olist, lici,
             list_selections, list_search_text, list_search_values, home_lists, user_home_info, user_field_info,
             new_field_and_values, child_row_errors, error_message, extra_content, extra_content_func,
             activation_file, okay, had_send_or_recv_error, record_not_found_error, finished_session,
             module_access_denied, cookies_permitted, temp_session, using_anonymous, use_url_checksum,
             is_ssl, is_sign_in, is_in_edit, is_new_record, was_invalid, skip_force_fields, created_session,
             allow_module_switching, vtab_num );
         }
      }

      if( cmd != c_cmd_status )
      {
         extra_content_func += " serverId = '" + g_id + "';";
         extra_content_func += " uniqueId = '" + unique_id + "';";

         // NOTE: Mnemonics are (simplistically) encrypted to prevent clear text observation via packet sniffing.
         if( is_identity_form )
            extra_content_func += " document.identity.entropy.value = decrypt_hex_data( document.identity.entropy.value, uniqueId );";
      }

      if( is_backup_or_restore )
      {
         if( file_exists( c_prepare_backup_file ) || file_exists( c_prepare_restore_file ) )
         {
            if( !extra_content_func.empty( ) )
               extra_content_func += '\n';

            extra_content_func += "auto_refresh_seconds = 5;\nauto_refresh( );";
         }
         else
         {
            string scrollx( input_data[ c_param_scrollx ] );
            string scrolly( input_data[ c_param_scrolly ] );

            if( !scrollx.empty( ) && !scrolly.empty( ) )
            {
               if( !extra_content_func.empty( ) )
                  extra_content_func += '\n';

               extra_content_func += "scroll_page( " + scrollx + ", " + scrolly + " );";
            }

            if( file_exists( c_backup_log_file ) || file_exists( c_restore_log_file ) )
            {
               if( !extra_content_func.empty( ) )
                  extra_content_func += '\n';

               extra_content_func += "auto_refresh_seconds = 5;\nauto_refresh( );";
            }
         }
      }

      extra_content << "<input type=\"hidden\" value=\"" << extra_content_func << "\" id=\"extra_content_func\"/>\n";
   }
   catch( exception& x )
   {
      string error( x.what( ) );

      string::size_type pos = error.find( c_response_error_prefix );

      if( pos == 0 )
         error.erase( 0, strlen( c_response_error_prefix ) );

#ifdef QUIETER_ERROR_LOGGING
      bool log_error = true;

      // NOTE: Filter out some of the more "noisy" (and less serious) error messages.
      if( !g_seed_error.empty( )
       || error == GDS( c_display_invalid_url )
       || error == GDS( c_display_client_script_out_of_date )
       || error == GDS( c_display_unknown_or_invalid_user_id )
       || error == GDS( c_display_you_are_currently_logged_in )
       || error == GDS( c_display_module_name_missing_or_invalid ) )
         log_error = false;

      if( log_error )
#endif
      {
         LOG_TRACE( string( "error: " ) + error
          + " (at " + date_time::local( ).as_string( true, false ) + " from " + raddr + ")" );
      }

      ostringstream osstr;

      if( display_error )
         osstr << "<p class=\"error\" align=\"center\">" << GDS( c_display_error ) << ": " << error << "</p>\n";

      bool act_as_logged_in = false;
      bool has_output_extra = false;

      if( !created_session && p_session_info && p_session_info->logged_in )
      {
         act_as_logged_in = true;
         has_output_extra = true;

         osstr << "<p class=\"text_with_back\">"
          << string_message( GDS( c_display_click_here_to_go_back ),
          make_pair( c_display_click_here_to_go_back_parm_href,
          "<a href=\"javascript:history.back( )\">" ), "</a>" ) << "</p>\n";
      }
      else
      {
         okay = false;

         if( force_refresh )
         {
            act_as_logged_in = true;
            has_output_extra = true;

            osstr << "<p align=\"center\">"
             << string_message( GDS( c_display_click_here_to_go_back ),
             make_pair( c_display_click_here_to_go_back_parm_href,
             "<a href=\"javascript:refresh( true )\">" ), "</a>" ) << "</p>\n";
         }
         else if( login_refresh )
         {
            act_as_logged_in = true;
            has_output_extra = true;

            // NOTE: An automatic refresh will occur but output this
            // just in case it fails due to browser compatibility.
            osstr << "<p align=\"center\">"
             << string_message( GDS( c_display_click_here_to_login ),
             make_pair( c_display_click_here_to_login_parm_href,
             "<a href=\"" + get_module_page_name( module_ref, true )
             + "?cmd=" + string( c_cmd_login ) + "\">" ), "</a>" ) + "</p>\n";
         }
         else if( identity_failure )
         {
            login_refresh = true;

            act_as_logged_in = true;
            has_output_extra = true;

            // NOTE: An automatic refresh will occur but output this
            // just in case it fails due to browser compatibility.
            osstr << "<p align=\"center\">"
             << string_message( GDS( c_display_click_here_to_retry ),
             make_pair( c_display_click_here_to_retry_parm_href,
             "<a href=\"" + get_module_page_name( module_ref, true ) + "\">" ), "</a>" ) + "</p>\n";
         }
      }

      if( !act_as_logged_in && !using_anonymous )
      {
         string login_html( !cookies_permitted || !get_storage_info( ).login_days
          || g_login_persistent_html.empty( ) ? g_login_html : g_login_persistent_html );

         if( created_session && p_session_info->logged_in )
         {
            login_html = "<p>" + string_message( GDS( c_display_click_here_to_login ),
             make_pair( c_display_click_here_to_login_parm_href,
             "<a href=\"" + get_module_page_name( module_ref, true )
             + "?cmd=" + string( c_cmd_login ) + "\">" ), "</a>" ) + "</p>";

            output_form( module_name, extra_content, osstr.str( ), login_html );
         }
         else
            output_form( module_name, extra_content, login_html, osstr.str( ) );
      }
      else
      {
         extra_content << "\n<div id=\"normal_content\">\n";

         extra_content << "\n<div id=\"header\"><div id=\"appname\">";
         extra_content << "<a href=\"?cmd=" << c_cmd_home << "\">" << get_app_name( ) << "</a></div>\n";
         extra_content << "</div>\n";

         extra_content << osstr.str( );

         if( !has_output_extra )
         {
            extra_content << "<p align=\"center\">"
             << string_message( GDS( c_display_click_here_to_retry ),
             make_pair( c_display_click_here_to_retry_parm_href,
             "<a href=\"javascript:refresh( false )\">" ), "</a>" ) << "</p>\n";
         }

         extra_content << g_footer_html;
         extra_content << "</div>\n";
      }

      if( act_as_logged_in )
      {
         extra_content << "<input type=\"hidden\" value=\"loggedIn = true;";

         if( login_refresh )
            extra_content << " refresh( false );\" id=\"extra_content_func\"/>\n";
         else
            extra_content << " had_act_error = true;\" id=\"extra_content_func\"/>\n";
      }
      else
      {
         extra_content << "<input type=\"hidden\" value=\"serverId = '"
          + g_id + "'; uniqueId = '" + unique_id + "'; had_act_error = true;\" id=\"extra_content_func\"/>\n";
      }
   }
   catch( ... )
   {
      extra_content << "<p class=\"error\" align=\"center\">" << GDS( c_display_unexpected_server_error ) << "</p>\n";

      LOG_TRACE( "unexpected error occurred" );

      if( p_session_info && p_session_info->logged_in )
      {
         extra_content << "<p class=\"text_with_back\">"
          << string_message( GDS( c_display_click_here_to_go_back ),
          make_pair( c_display_click_here_to_go_back_parm_href,
          "<a href=\"javascript:history.back( )\">" ), "</a>" ) << "</p>\n";

         extra_content << "<input type=\"hidden\" value=\"loggedIn = true; had_act_error = true\" id=\"extra_content_func\"/>\n";
      }
      else
      {
         okay = false;

         extra_content << "<p align=\"center\">"
          << string_message( GDS( c_display_click_here_to_login ),
          make_pair( c_display_click_here_to_login_parm_href, "<a href=\""
          + get_module_page_name( module_ref, true ) + "?cmd=" + string( c_cmd_login ) + "\">" ), "</a>" ) << "</p>\n";
      }
   }

   if( p_session_info && ( !okay || temp_session ) )
   {
      if( temp_session || created_session )
      {
         if( !temp_session )
            remove_session_temp_directory( session_id );

#ifdef USE_MULTIPLE_REQUEST_HANDLERS
         if( p_session_info->p_socket )
         {
            release_socket( p_session_info->p_socket );
            p_session_info->p_socket = 0;
         }
#endif
         remove_non_persistent( session_id );

         if( has_session_info( session_id ) )
            destroy_session( session_id );
         else
            delete p_session_info;

         p_session_info = 0;
      }
   }

   string interface_html;

   if( interface_file.empty( ) )
      interface_html = g_ciyam_interface_html;
   else
      interface_html = buffer_file( interface_file );

   remove_utf8_bom( interface_html );

   string direction;
   if( is_vertical )
      direction = "vertical";
   else
      direction = "horizontal";

   replace( interface_html, c_direction, direction );

   string output;

   size_t pos = interface_html.find( c_title );

   if( pos != string::npos )
   {
      output += interface_html.substr( 0, pos );
      output += title;
      pos += strlen( c_title );
   }
   else
      pos = 0;

   size_t cpos = interface_html.find( c_form_content_comment, pos );

   if( cpos != string::npos )
   {
      output += interface_html.substr( pos, cpos - pos );
      output += form_content.str( );
      cpos += strlen( c_form_content_comment );
   }
   else
      cpos = pos;

   size_t epos = interface_html.find( c_extra_content_comment, cpos );

   if( epos != string::npos )
   {
      output += interface_html.substr( cpos, epos - cpos );
      output += extra_content.str( );

      cpos = epos + strlen( c_extra_content_comment );
   }

   output += interface_html.substr( cpos );

   if( encrypt_data )
   {
      if( p_session_info && p_session_info->logged_in )
      {
         // NOTE: Add some random noise to further obfuscate the encrypted
         // output (otherwise analysis of the encrypted output with a good
         // idea of the expected plain text output could lead to revealing
         // the initial hash of the key).
         string str( "<!-- " );

         str += random_characters( 25, 75, e_printable_type_alpha_lower );

         str += " -->\n";

         output = str + output + str;

         while( output.length( ) % 40 )
            output += ' ';

         if( p_session_info->content_pwd_hash.empty( ) )
            p_session_info->content_pwd_hash = p_session_info->user_pwd_hash;

         crypt_decoded( p_session_info->content_pwd_hash, output, false );

         output = base64::encode( output );
      }
   }

#ifdef DEBUG
   ofstream outf( "debug.htms" );
   outf << output << endl;
#endif

   FCGX_FPrintF( p_out, "Content-Type: text/html; charset=UTF-8\n" );
   FCGX_FPrintF( p_out, "\r\n\r\n" );

   FCGX_FPrintF( p_out, "%s\n", output.c_str( ) );

#ifdef USE_MULTIPLE_REQUEST_HANDLERS
   FCGX_Finish_r( &request );
#else
   FCGX_Finish( );
#endif

#ifdef USE_MULTIPLE_REQUEST_HANDLERS
   if( p_session_info && p_session_info->p_socket )
   {
      release_socket( p_session_info->p_socket );
      p_session_info->p_socket = 0;
   }
#endif

   if( finished_session )
      destroy_session( session_id );
   else if( p_session_info )
      p_session_info->locked = false;
}

int main( int argc, char* argv[ ] )
{
   int rc = 0;

   try
   {
#ifndef _WIN32
      umask( WEB_FILES_UMASK );
#endif
      string exe_path = string( argv[ 0 ] );

#ifndef _WIN32
      size_t pos = exe_path.find_last_of( "/" );
#else
      size_t pos = exe_path.find_last_of( "\\" );
#endif
      if( pos != string::npos )
         exe_path.erase( pos );
      else
         exe_path.erase( );

      if( !exe_path.empty( ) )
         set_cwd( exe_path );

      LOG_TRACE( "[started at: " + date_time::local( ).as_string( true, false ) + "]" );

#ifdef SSL_SUPPORT
      if( file_exists( c_ciyam_pem ) )
#  ifndef USE_MULTIPLE_REQUEST_HANDLERS
         init_ssl( c_ciyam_pem );
#  else
         init_ssl( c_ciyam_pem, 0, 0, true );
#  endif
#endif

      DEBUG_TRACE( "cwd = " + get_cwd( ) );

      DEBUG_TRACE( "[read strings]" );

      init_strings( );
      init_extkeys( );

      g_login_html = buffer_file( c_login_htms );
      g_backup_html = buffer_file( c_backup_htms );
      g_footer_html = buffer_file( c_footer_htms );
      g_openup_html = buffer_file( c_openup_htms );
      g_signup_html = buffer_file( c_signup_htms );
      g_unlock_html = buffer_file( c_unlock_htms );
      g_prepare_html = buffer_file( c_prepare_htms );
      g_restore_html = buffer_file( c_restore_htms );
      g_activate_html = buffer_file( c_activate_htms );
      g_identity_html = buffer_file( c_identity_htms );
      g_password_html = buffer_file( c_password_htms );
      g_ssl_signup_html = buffer_file( c_ssl_signup_htms );
      g_no_identity_html = buffer_file( c_no_identity_htms );
      g_authenticate_html = buffer_file( c_authenticate_htms );

      g_ciyam_interface_html = buffer_file( c_ciyam_interface_htms );

      if( file_exists( c_id_file ) )
         g_id = buffer_file( c_id_file );

      str_replace( g_login_html, c_login, GDS( c_display_login ) );
      str_replace( g_login_html, c_password, GDS( c_display_password ) );
      str_replace( g_login_html, c_username, GDS( c_display_username ) );

      str_replace( g_openup_html, c_user_name, GDS( c_display_user_name ) );
      str_replace( g_openup_html, c_account_type, GDS( c_display_account_type ) );
      str_replace( g_openup_html, c_send_request, GDS( c_display_send_request ) );
      str_replace( g_openup_html, c_account_type_0, GDS( c_display_account_type_0 ) );
      str_replace( g_openup_html, c_account_type_1, GDS( c_display_account_type_1 ) );
      str_replace( g_openup_html, c_account_type_2, GDS( c_display_account_type_2 ) );
      str_replace( g_openup_html, c_account_type_3, GDS( c_display_account_type_3 ) );
      str_replace( g_openup_html, c_open_up_introduction, GDS( c_display_open_up_openid_account_introduction ) );

      str_replace( g_unlock_html, c_continue, GDS( c_display_continue ) );
      str_replace( g_unlock_html, c_unlock_message, GDS( c_display_unlock_message ) );

      str_replace( g_signup_html, c_username, GDS( c_display_username ) );
      str_replace( g_signup_html, c_account_type, GDS( c_display_account_type ) );
      str_replace( g_signup_html, c_send_request, GDS( c_display_send_request ) );
      str_replace( g_signup_html, c_account_type_0, GDS( c_display_account_type_0 ) );
      str_replace( g_signup_html, c_account_type_1, GDS( c_display_account_type_1 ) );
      str_replace( g_signup_html, c_account_type_2, GDS( c_display_account_type_2 ) );
      str_replace( g_signup_html, c_account_type_3, GDS( c_display_account_type_3 ) );
      str_replace( g_signup_html, c_gpg_public_key, GDS( c_display_gpg_public_key ) );
      str_replace( g_signup_html, c_sign_up_introduction, GDS( c_display_sign_up_main_form_client_security_introduction ) );
      str_replace( g_signup_html, c_sign_up_extra_details, GDS( c_display_sign_up_main_form_client_security_extra_details ) );
      str_replace( g_signup_html, c_sign_up_gpg_expert_tip, GDS( c_display_sign_up_main_form_client_security_gpg_expert_tip ) );

      str_replace( g_activate_html, c_login, GDS( c_display_login ) );
      str_replace( g_activate_html, c_password, GDS( c_display_password ) );
      str_replace( g_activate_html, c_persistent, GDS( c_display_automatic_login ) );
      str_replace( g_activate_html, c_verify_password, GDS( c_display_verify_password ) );

      str_replace( g_password_html, c_old_password, GDS( c_display_old_password ) );
      str_replace( g_password_html, c_new_password, GDS( c_display_new_password ) );
      str_replace( g_password_html, c_change_password, GDS( c_display_change_password ) );
      str_replace( g_password_html, c_verify_new_password, GDS( c_display_verify_new_password ) );

      str_replace( g_ssl_signup_html, c_email, GDS( c_display_email ) );
      str_replace( g_ssl_signup_html, c_password, GDS( c_display_password ) );
      str_replace( g_ssl_signup_html, c_username, GDS( c_display_username ) );
      str_replace( g_ssl_signup_html, c_verify_password, GDS( c_display_verify_password ) );
      str_replace( g_ssl_signup_html, c_account_type, GDS( c_display_account_type ) );
      str_replace( g_ssl_signup_html, c_send_request, GDS( c_display_send_request ) );
      str_replace( g_ssl_signup_html, c_account_type_0, GDS( c_display_account_type_0 ) );
      str_replace( g_ssl_signup_html, c_account_type_1, GDS( c_display_account_type_1 ) );
      str_replace( g_ssl_signup_html, c_account_type_2, GDS( c_display_account_type_2 ) );
      str_replace( g_ssl_signup_html, c_account_type_3, GDS( c_display_account_type_3 ) );
      str_replace( g_ssl_signup_html, c_ssl_sign_up_introduction, GDS( c_display_ssl_sign_up_main_form ) );
      str_replace( g_ssl_signup_html, c_ssl_sign_up_extra_details, GDS( c_display_ssl_sign_up_main_form_extra_details ) );

      str_replace( g_authenticate_html, c_pin_name, GDS( c_display_pin ) );
      str_replace( g_authenticate_html, c_pin_message, GDS( c_display_pin_message ) );
      str_replace( g_authenticate_html, c_continue, GDS( c_display_continue ) );

      if( file_exists( c_login_password_htms ) )
      {
         g_login_password_html = buffer_file( c_login_password_htms );

         str_replace( g_login_password_html, c_login, GDS( c_display_login ) );
         str_replace( g_login_password_html, c_password, GDS( c_display_password ) );
      }

      if( file_exists( c_login_persistent_htms ) )
      {
         g_login_persistent_html = buffer_file( c_login_persistent_htms );

         str_replace( g_login_persistent_html, c_login, GDS( c_display_login ) );
         str_replace( g_login_persistent_html, c_password, GDS( c_display_password ) );
         str_replace( g_login_persistent_html, c_username, GDS( c_display_username ) );
         str_replace( g_login_persistent_html, c_persistent, GDS( c_display_automatic_login ) );
      }

      if( file_exists( c_password_persistent_htms ) )
      {
         g_password_persistent_html = buffer_file( c_password_persistent_htms );

         str_replace( g_password_persistent_html, c_old_password, GDS( c_display_old_password ) );
         str_replace( g_password_persistent_html, c_new_password, GDS( c_display_new_password ) );
         str_replace( g_password_persistent_html, c_persistent, GDS( c_display_automatic_login ) );
         str_replace( g_password_persistent_html, c_change_password, GDS( c_display_change_password ) );
         str_replace( g_password_persistent_html, c_change_password, GDS( c_display_change_password ) );
         str_replace( g_password_persistent_html, c_verify_new_password, GDS( c_display_verify_new_password ) );
      }

      g_display_login_info = GDS( c_display_login_info );
      g_display_change_password = GDS( c_display_change_password );
      g_display_sign_up_for_an_account = GDS( c_display_sign_up_for_an_account );

      DEBUG_TRACE( "[read storage info]" );

      read_global_storage_info( );

      DEBUG_TRACE( "[now starting handlers]" );

      // NOTE: Scope for timeout handler temporary object.
      {
         timeout_handler( ).start( );

#ifdef USE_MULTIPLE_REQUEST_HANDLERS
         FCGX_Init( );

         // NOTE: Apache's "mod_fcgid" only supports single threaded FCGI servers so check
         // if "mod_fastcgi" is in use before starting any other request handling threads.
#  ifdef USE_MOD_FASTCGI_KLUDGE
         if( has_environment_variable( "_FCGI_MUTEX_" ) ) // i.e. this var is only found in mod_fastcgi
#  endif
         {
            // NOTE: Start all but one as separate threads - the main thread runs the final handler.
            for( size_t i = 1; i < c_num_handlers; i++ )
            {
               request_handler* p_request_handler = new request_handler;
               p_request_handler->start( );
            }
         }
#endif

         request_handler* p_request_handler = new request_handler;
         p_request_handler->on_start( );
      }

#ifdef USE_MULTIPLE_REQUEST_HANDLERS
      remove_sockets( );
#endif
      LOG_TRACE( "[finished at: " + date_time::local( ).as_string( true, false ) + "]" );
   }
   catch( exception& x )
   {
      LOG_TRACE( string( "error: " ) + x.what( ) );
      rc = 1;
   }
   catch( ... )
   {
      LOG_TRACE( "unexpected exception caught" );
      rc = 2;
   }

#ifdef _WIN32
   if( file_exists( c_kill_script ) )
      file_remove( c_kill_script );
#endif
   return rc;
}
