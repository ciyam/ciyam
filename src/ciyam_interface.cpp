// Copyright (c) 2005-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2013 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef __BORLANDC__
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

#ifdef __GNUG__
#  define _chdir chdir
#  define _mkdir mkdir
#endif

#include "ciyam_interface.h"

#include "salt.h"
#include "sha1.h"
#include "smtp.h"
#include "regex.h"
#include "base64.h"
#include "config.h"
#include "format.h"
#include "ptypes.h"
#include "sha256.h"
#include "numeric.h"
#include "threads.h"
#include "sockets.h"
#include "utilities.h"
#include "date_time.h"
#include "fcgi_cmds.h"
#include "fcgi_info.h"
#include "fcgi_list.h"
#include "fcgi_view.h"
#include "fcgi_utils.h"
#include "fcgi_parser.h"
#include "crypt_stream.h"

#define ALWAYS_REPLACE_SESSION
//#define ALLOW_MULTIPLE_RECORD_ENTRY

#ifdef _WIN32
#  define USE_MOD_FASTCGI_KLUDGE
#endif

#define USE_MULTIPLE_REQUEST_HANDLERS

using namespace std;

namespace
{

#include "ciyam_constants.h"

const int c_num_handlers = 10;

const size_t c_max_fcgi_input_size = 65536;
const size_t c_max_param_input_size = 4096;

const int c_connect_timeout = 2500;
const int c_greeting_timeout = 2500;
const int c_initial_response_timeout = 7500;
const int c_subsequent_response_timeout = 2000;

const int c_max_pin_time_difference = 30;

const int c_auto_refresh_seconds_local = 5;
const int c_auto_refresh_seconds_remote = 10;

const char* const c_local_ip_addr = "127.0.0.1";

const char* const c_identity_file = "identity.txt";

const char* const c_stop_file = "ciyam_interface.stop";

#ifdef _WIN32
const char* const c_kill_script = "ciyam_interface.kill.bat";
#endif

#ifndef _WIN32
const int c_default_directory_perms = S_IRWXU | S_IRWXG;
#endif

const char* const c_login_file = "login.htms";
const char* const c_footer_file = "footer.htms";
const char* const c_openup_file = "openup.htms";
const char* const c_signup_file = "signup.htms";
const char* const c_activate_file = "activate.htms";
const char* const c_password_file = "password.htms";
const char* const c_ssl_signup_file = "ssl_signup.htms";
const char* const c_authenticate_file = "authenticate.htms";
const char* const c_ciyam_interface_file = "ciyam_interface.htms";
const char* const c_login_persistent_file = "login_persistent.htms";
const char* const c_password_persistent_file = "password_persistent.htms";

const char* const c_printlist_file = "printlist.htms";
const char* const c_printview_file = "printview.htms";

const char* const c_sign_up_types_map = "sign_up_types.map";
const char* const c_sign_up_testers_file = "sign_up_testers.lst";

const char* const c_dummy_param_prefix = "dummy=";

const char* const c_http_param_ssl = "HTTPS";
const char* const c_http_param_host = "HTTP_HOST";
const char* const c_http_param_raddr = "REMOTE_ADDR";
const char* const c_http_param_rhost = "REMOTE_HOST";
const char* const c_http_param_ruser = "REMOTE_USER";
const char* const c_http_param_query = "QUERY_STRING";
const char* const c_http_param_cookie = "HTTP_COOKIE";
const char* const c_http_param_uagent = "HTTP_USER_AGENT";

const char* const c_openid_username = "openid_username";
const char* const c_openid_ext_email = "openid.ext1.value.email";

const char* const c_nbsp = "&nbsp;";

string g_nbsp( c_nbsp );

const char* const c_email = "@@email";
const char* const c_login = "@@login";
const char* const c_title = "@@title";
const char* const c_checked = "@@checked";
const char* const c_user_id = "@@user_id";
const char* const c_app_name = "@@app_name";
const char* const c_continue = "@@continue";
const char* const c_password = "@@password";
const char* const c_pin_name = "@@pin_name";
const char* const c_selected_ = "@@selected_";
const char* const c_direction = "@@direction";
const char* const c_pin_value = "@@pin_value";
const char* const c_user_name = "@@user_name";
const char* const c_persistent = "@@persistent";
const char* const c_pin_message = "@@pin_message";
const char* const c_account_type = "@@account_type";
const char* const c_old_password = "@@old_password";
const char* const c_new_password = "@@new_password";
const char* const c_send_request = "@@send_request";
const char* const c_email_address = "@@email_address";
const char* const c_error_message = "@@error_message";
const char* const c_account_type_0 = "@@account_type_0";
const char* const c_account_type_1 = "@@account_type_1";
const char* const c_account_type_2 = "@@account_type_2";
const char* const c_account_type_3 = "@@account_type_3";
const char* const c_gpg_public_key = "@@gpg_public_key";
const char* const c_change_password = "@@change_password";
const char* const c_verify_password = "@@verify_password";
const char* const c_verify_new_password = "@@verify_new_password";
const char* const c_open_up_introduction = "@@open_up_introduction";
const char* const c_sign_up_introduction = "@@sign_up_introduction";
const char* const c_sign_up_extra_details = "@@sign_up_extra_details";
const char* const c_sign_up_gpg_expert_tip = "@@sign_up_gpg_expert_tip";
const char* const c_ssl_sign_up_introduction = "@@ssl_sign_up_introduction";
const char* const c_ssl_sign_up_extra_details = "@@ssl_sign_up_extra_details";

const char* const c_form_content_comment = "<!-- @@form_content -->";
const char* const c_extra_content_comment = "<!-- @@extra_content -->";

const char* const c_user_key_arg = "$user";
const char* const c_user_perms_arg = "$perms";
const char* const c_user_other_none = "~";

const char* const c_dummy_server_command = "wait";

// NOTE: When not using multiple request handlers a global socket is being used to access the
// application server in order to permit testing without any possible concurrency issues arising.
#ifndef USE_MULTIPLE_REQUEST_HANDLERS
tcp_socket g_socket;
#endif

bool g_has_connected = false;

string g_id;

string g_login_html;
string g_footer_html;
string g_openup_html;
string g_signup_html;
string g_password_html;
string g_activate_html;
string g_ssl_signup_html;
string g_authenticate_html;
string g_ciyam_interface_html;
string g_login_persistent_html;
string g_password_persistent_html;

string g_display_login_info;
string g_display_change_password;
string g_display_sign_up_for_an_account;

size_t g_max_user_limit = 1;

mutex g_join_mutex;
mutex g_session_mutex;

session_container g_sessions;

map< string, string > g_uuid_for_ip_addr;

}

#ifdef USE_MULTIPLE_REQUEST_HANDLERS
vector< pair< bool, tcp_socket* > > g_sockets;
mutex g_socket_mutex;

tcp_socket* get_tcp_socket( )
{
   guard g( g_socket_mutex );

   tcp_socket* p_socket = 0;

   if( g_sockets.empty( ) )
      g_sockets.resize( c_num_handlers );

   for( size_t i = 0; i < g_sockets.size( ); i++ )
   {
      if( !g_sockets[ i ].first )
      {
         g_sockets[ i ].first = true;

         if( !g_sockets[ i ].second )
            g_sockets[ i ].second = new tcp_socket;

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

   for( size_t i = 0; i < log_messages.size( ); i++ )
      LOG_TRACE( log_messages[ i ] );
}

string get_app_name( )
{
   // KLUDGE: This needs to be able to translated but is not currently a module
   // string (perhaps add an "app" string for this that defaults to the name of
   // the Application being generated).
   return get_storage_info( ).storage_name;
}

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

void output_login_logout( const string& module_name, ostream& os, const string& extra_details, const string& msg = "" )
{
   os << "\n<div id=\"normal_content\">\n";

   os << "\n<div id=\"header\"><div id=\"appname\">";
   os << "<a href=\"?cmd=" << c_cmd_home << "\">" << get_app_name( ) << "</a></div>\n";
   os << "   <div id=\"navband\">\n";
   os << "   </div>\n";
   os << "</div>\n";

   if( extra_details.empty( ) )
   {
      os << "\n<div id=\"logout_text\">\n";
      os << msg << "\n";
      os << "</div>\n";
   }
   else
   {
      os << "<h3 class=\"center\">" << GDS( c_display_sign_in_using_credentials ) << "</h3>" << endl;
      os << extra_details;
      os << msg << "\n";
   }

   os << g_footer_html;

   os << "</div>\n";
}

string get_unique( const string& id, const string& ip_addr )
{
   guard g( g_socket_mutex );

   // FUTURE: If a huge number of connections are made from different IP addresses that
   // never perform a successful login then the map being used could chew up very large
   // amounts of memory (perhaps also need an IP address to timestamp map so that those
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
   bool embed_images = false;
   bool encrypt_data = false;
   bool temp_session = false;
   bool force_refresh = false;
   bool created_session = false;
   bool finished_session = false;

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

   bool cookies_permitted( false );

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

      if( get_storage_info( ).embed_images )
         embed_images = true;

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

      // NOTE: If an existing session is present then obtain its
      // password hash for decrypting base64 data (if required).
      string pwd_hash;
      if( !session_id.empty( ) && session_id != c_new_session )
      {
         p_session_info = get_session_info( session_id, false );
         if( p_session_info )
            pwd_hash = p_session_info->user_pwd_hash;
      }

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
               parse_input( ( char* )decoded.c_str( ), decoded.size( ), input_data, '&', true );
         }
      }

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

      if( file_exists( c_stop_file ) )
      {
         msleep( 3000 );
         if( file_exists( c_stop_file ) )
            throw runtime_error( GDS( c_display_under_maintenance_try_again_later ) );
      }

      string app_dir_name( lower( get_storage_info( ).storage_name ) );

      if( !get_storage_info( ).modules_index.count( module_name ) )
         throw runtime_error( "cannot find information for module '" + module_name + "'" );

      set< string > login_opts;
      if( !get_storage_info( ).login_opts.empty( ) )
         split( get_storage_info( ).login_opts, login_opts, '+' );

      module_info& mod_info( *get_storage_info( ).modules_index.find( module_name )->second );

      module_id = mod_info.id;

      title = mod_info.title;

      bool is_invalid_session = false;
      if( !session_id.empty( ) && session_id != c_new_session && cmd != c_cmd_join && cmd != c_cmd_status )
      {
         if( user.empty( ) || ( !user.empty( ) && hash == get_user_hash( user ) ) )
         {
            p_session_info = get_session_info( session_id, false );

            if( !p_session_info )
            {
               if( cmd.empty( ) || cmd == c_cmd_home
                || cmd == c_cmd_quit || cmd == c_cmd_login || !username.empty( ) || !userhash.empty( ) )
               {
                  cmd = c_cmd_home;
                  session_id.erase( );
               }

               if( is_kept && get_storage_info( ).login_days )
               {
                  username = user;
                  cmd = c_cmd_home;
                  session_id.erase( );
                  is_authorised = true;
                  is_invalid_session = false;
               }
               else if( !session_id.empty( )
                && session_id != get_unique( g_id, raddr ) )
                  is_invalid_session = true;
            }
         }
      }

      bool is_sign_in = false;
      bool using_anonymous = false;

      if( !is_kept
       && cmd != c_cmd_credentials
       && !is_invalid_session && mod_info.allows_anonymous_access
       && ( username.empty( ) && userhash.empty( ) && !p_session_info ) )
      {
         is_authorised = true;
         using_anonymous = true;
      }

      if( cmd == c_cmd_credentials )
      {
         cmd = c_cmd_home;
         is_sign_in = true;

         if( is_invalid_session )
         {
            session_id.erase( );
            is_invalid_session = false;
         }   
      }

      if( using_anonymous || cmd == c_cmd_join || cmd == c_cmd_status )
      {
         temp_session = true;
         session_id = c_new_session;
      }

      if( is_ssl && cmd.empty( ) && input_data.count( c_http_param_ruser ) )
         cmd = c_cmd_open;

      if( cmd != c_cmd_status )
         unique_id = get_unique( g_id, raddr );

      string activation_file;
      if( is_activation )
         activation_file = data + chksum;

      if( session_id.empty( ) || session_id == c_new_session )
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

         if( username.empty( ) && userhash.empty( ) && password.empty( )
          && ( !using_anonymous || ( is_activation && file_exists( activation_file ) ) ) )
         {
            string login_html( !cookies_permitted || !get_storage_info( ).login_days
             || g_login_persistent_html.empty( ) ? g_login_html : g_login_persistent_html );

            if( cmd != c_cmd_activate )
            {
               is_login_screen = true;
               output_login_logout( module_name, extra_content, login_html );
            }
            else
            {
               if( chksum != get_checksum( c_cmd_activate + user + data ) )
                  throw runtime_error( "Invalid URL" );

               string activate_html( g_activate_html );
               str_replace( activate_html, c_user_id, user );

               string message( "<p><b>"
                + string_message( GDS( c_display_provide_password_to_activate ),
                make_pair( c_display_provide_password_to_activate_parm_id, user ) ) + "</b></p>" );

               output_login_logout( module_name, extra_content, activate_html, message );
            }
         }
         else
         {
            if( cmd == c_cmd_activate )
            {
               username = user;
               cmd = c_cmd_login;
            }
            else if( !using_anonymous )
               cmd = c_cmd_home;

            if( g_max_user_limit && get_num_sessions( ) >= g_max_user_limit )
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

            if( !temp_session && cmd != c_cmd_open && ( is_authorised || persistent == c_true ) )
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
            if( cmd == c_cmd_open )
               cmd = c_cmd_home;

            p_session_info->tm_last_request = time( 0 );
         }
      }

      string extra_content_func;
      bool has_just_logged_in = false;
      bool performed_file_attach_or_detach = false;

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

            output_login_logout( module_name, extra_content, login_html, osstr.str( ) );

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
      else
      {
         guard g( g_session_mutex );

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
                  if( p_session_info->p_socket->connect( address, c_connect_timeout ) )
                  {
                     p_session_info->p_socket->set_no_delay( );

                     string greeting;
                     if( p_session_info->p_socket->read_line( greeting, c_greeting_timeout ) <= 0 )
                     {
                        string error;
                        if( p_session_info->p_socket->had_timeout( ) )
                           error = "timeout occurred trying to connect to server";
                        else
                           error = "server has terminated this connection";

                        throw runtime_error( error );
                     }

                     version_info ver_info;
                     if( get_version_info( greeting, ver_info ) != string( c_response_okay ) )
                        throw runtime_error( greeting );

                     // FUTURE: Some sort of "upgrade available" message should probably be displayed
                     // if the client is using an older minor protocol version than the app server.
                     bool is_older;
                     if( !check_version_info( ver_info, c_protocol_major_version, c_protocol_minor_version, &is_older ) )
                        throw runtime_error( "incompatible protocol version "
                         + ver_info.ver + " (expecting " + string( c_protocol_version ) + ")" );

                     string identity_info;
                     if( !simple_command( *p_session_info, "identity", &identity_info ) )
                        throw runtime_error( "Unable to determine identity information." );

                     string::size_type pos = identity_info.find( ':' );
                     if( pos == string::npos )
                        throw runtime_error( "unexpected identity information '" + identity_info + "'" );

                     string server_id( identity_info.substr( 0, pos ) );

                     string reg_key;
                     string::size_type npos = server_id.find( '-' );
                     if( npos != string::npos )
                     {
                        reg_key = server_id.substr( npos + 1, pos - npos );
                        server_id.erase( npos );
                     }

                     if( reg_key != get_storage_info( ).reg_key )
                        throw runtime_error( GDS( c_display_system_is_under_maintenance ) );

                     if( get_server_id( ) != server_id )
                     {
                        set_server_id( server_id );
                        g_id = get_id_from_server_id( );

                        if( !file_exists( c_identity_file ) )
                        {
                           ofstream outf( c_identity_file );
                           outf << get_server_id( );
                        }

                        // NOTE: As the original "g_id" value was empty any URL link or attempt to login
                        // could fail so force the page to refresh using the now corrected "g_id" value.
                        if( cmd != c_cmd_open )
                           extra_content_func += "refresh( false );\n";
                     }

                     g_max_user_limit = ( size_t )atoi( identity_info.substr( pos + 1 ).c_str( ) );

                     if( !simple_command( *p_session_info, "storage_init " + get_storage_info( ).storage_name ) )
                        throw runtime_error( "unable to initialise '" + get_storage_info( ).storage_name + "' storage" );

                     string response;
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
                  else
                     throw runtime_error( GDS( c_display_application_server_unavailable ) );
               }
               else
                  throw runtime_error( "unable to open socket" );
            }

            bool was_openid = false;
            if( connection_okay && cmd == c_cmd_open && !userhash.empty( ) )
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

            if( connection_okay && cmd != c_cmd_open && p_session_info->user_id.empty( ) )
            {
               bool is_replacement_session = false;

               if( cmd != c_cmd_join && !mod_info.user_class_name.empty( ) )
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

                     if( user_data.size( ) < 2 )
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

                     string encrypted_password( password_encrypt( activate_password, get_server_id( ) ) );

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
                     p_session_info->user_module = module_name;
                  else
                  {
                     fetch_user_record( id_for_login, module_id, module_name, mod_info, *p_session_info,
                      is_authorised || persistent == c_true || !base64_data.empty( ), true, username, userhash, password, unique_id );

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
                     if( created_session && persistent == c_true )
#endif
                     {
                        is_replacement_session = true;
                        destroy_user_session_info( p_session_info->user_id, permit_module_switching ? 0 : p_session_info->user_module.c_str( ) );
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
                  has_just_logged_in = true;
                  p_session_info->logged_in = true;

                  p_session_info->is_openid = was_openid;

                  string path( c_files_directory );
                  path += "/" + string( c_tmp_directory );
                  path += "/" + session_id;

#ifdef _WIN32
                  if( _mkdir( path.c_str( ) ) != 0 )
#else
                  int um = umask( 0 );
                  if( _mkdir( path.c_str( ), c_default_directory_perms ) != 0 )
#endif
                     throw runtime_error( "unable to create '" + path + "' directory" );

#ifndef _WIN32
                  umask( um );
#endif
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

         bool keep_checks = false;
         if( keepchecks == c_true_value )
            keep_checks = true;

         int back_count = 1;
         if( !bcount.empty( ) )
         {
            back_count = atoi( bcount.c_str( ) );
            if( back_count < 1 )
               back_count = 1;
         }

         bool use_url_checksum = ( url_opts.count( c_url_opt_use_checksum ) > 0 );

         bool is_checksummed_home = ( cmd == c_cmd_home && !uselect.empty( ) );

         if( use_url_checksum && !cmd.empty( ) && cmd != c_cmd_join && cmd != c_cmd_open
          && ( is_checksummed_home || ( cmd != c_cmd_home && cmd != c_cmd_quit && cmd != c_cmd_login
          && cmd != c_cmd_status && ( cmd != c_cmd_view || ( act != c_act_cont && act != c_act_undo ) ) ) ) )
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
            if( ( !act.empty( ) && act != c_act_view ) || ( cmd == c_cmd_view && data.find( c_new_record ) == 0 ) )
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

            // NOTE: For list searches part of an SHA1 hash of the "findinfo" parameter is used to
            // ensure the search data hasn't been changed via URL tampering.
            if( !has_just_logged_in && ( ( !hashval.empty( ) && hashval != get_hash( hash_values ) )
             || ( chksum != get_checksum( identity_values ) && chksum != get_checksum( *p_session_info, checksum_values ) ) ) )
            {
#ifdef DEBUG
               for( map< string, string >::const_iterator ci = input_data.begin( ); ci != input_data.end( ); ++ci )
                  DEBUG_TRACE( "Input[ " + ci->first + " ] => " + ci->second );
#endif
               // NOTE: If a session has just been created then assume the invalid URL was
               // actually due to an already "timed out" session.
               if( created_session && !using_anonymous )
                  throw runtime_error( GDS( c_display_your_session_has_been_timed_out ) );
               else
                  throw runtime_error( "Invalid URL" );
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

         int vtabc_num = 1;
         if( !vtabc.empty( ) )
            vtabc_num = atoi( vtabc.c_str( ) );

         string new_key;
         string error_message;
         string hpassword_salt;
         string user_home_info;
         string edit_timeout_func;

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

         if( cmd == c_cmd_login || module_access_denied )
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
         if( cmd == c_cmd_quit && extra != session_id )
         {
            cmd = c_cmd_home;
            server_command.erase( );
         }

         if( cmd.empty( ) )
            cmd = c_cmd_home;

         bool had_send_or_recv_error = false;
         bool record_not_found_error = false;

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

         // NOTE: As the user's record could be changed at any time (and especially
         // to ensure a session cannot continue if its account is now de-activated)
         // the user information is checked again here.
         try
         {
            if( !has_just_logged_in && p_session_info->logged_in
             && ( cmd == c_cmd_home || cmd == c_cmd_pwd || cmd == c_cmd_view
             || cmd == c_cmd_pview || cmd == c_cmd_list || cmd == c_cmd_plist ) )
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
          && ( pin == p_session_info->user_pin_value || pin == p_session_info->last_user_pin_value ) )
            p_session_info->needs_pin = false;

         if( p_session_info->needs_pin && cmd != c_cmd_quit )
            cmd = c_cmd_home;

         // NOTE: If a new password hash is passed from the client after logging in then
         // encrypt and store it in the application server "files area" for later usage.
         if( p_session_info->logged_in && !newhash.empty( ) )
         {
            string cmd( "file_init " );
            cmd += p_session_info->user_key;
            cmd += " " + password_encrypt( newhash, get_server_id( ) );
            simple_command( *p_session_info, cmd );
         }

         // NOTE: For a save or continue edit action it is expected that a field list and
         // a corresponding set of user values will be provided. If a "view" contains one
         // or more "always_editable" fields then these will be passed via the field list
         // for an "edit", "view" or any "exec" action.
         map< string, string > user_field_info;
         map< string, string > extra_field_info;
         if( !fieldlist.empty( )
          && ( act == c_act_edit || act == c_act_cont || act == c_act_save || act == c_act_exec || act == c_act_view ) )
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

         set< string > specials;

         string pdf_view_file_name;
         string pdf_list_file_name;

         map< string, int > child_names;
         vector< list_source > child_lists;
         map< string, row_error_container > child_row_errors;

         view_info_const_iterator vici = mod_info.view_info.end( );
         list_info_const_iterator lici = mod_info.list_info.end( );

         bool skip_force_fields = false;

         bool is_in_edit = false;
         if( act == c_act_edit || act == c_act_save
          || act == c_act_cont || ( is_new_record && ( act.empty( ) || act == c_act_undo ) ) )
            is_in_edit = true;

         if( cmd == c_cmd_view || cmd == c_cmd_pview )
         {
            vici = mod_info.view_info.find( ident );
            if( vici == mod_info.view_info.end( ) )
               throw runtime_error( "unknown view '" + ident + "'" );

            view.vici = vici;

            setup_view_fields( view, *vici->second,
             mod_info, *p_session_info, ident, login_opts, module_id, module_ref, is_in_edit, is_new_record );
         }
         else if( cmd == c_cmd_home || cmd == c_cmd_list || cmd == c_cmd_plist )
         {
            if( cmd == c_cmd_home )
            {
               for( lici = mod_info.list_info.begin( ); lici != mod_info.list_info.end( ); ++lici )
               {
                  list_source next_list;

                  if( lici->second->type == c_list_type_home )
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
               string act_cmd;

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

                     // NOTE: If the record saved was a user record then if not admin assume a
                     // user has changed their own details (so re-read the user record details).
                     if( error_message.empty( ) && !had_send_or_recv_error
                      && mod_info.user_class_id == view.cid && !p_session_info->is_admin_user )
                        fetch_user_record( g_id, module_id, module_name, mod_info,
                         *p_session_info, is_authorised, false, p_session_info->user_id, "", "", "" );
                  }
               }
               else if( act == c_act_cont )
               {
                  act = c_act_edit;
                  was_invalid = true;
               }
               else if( act == c_act_del || act == c_act_link || act == c_act_exec )
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
                     if( !is_in_edit && act == c_act_exec && error_message.size( ) > 2
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
               else if( !perform_action( list.module, list.cid, act, app,
                   field, "", exec, extra, list.row_errors, *p_session_info ) )
                     had_send_or_recv_error = true;

               // NOTE: Handle an error/response (is stored as the first record in "row_errors").
               // Also if no message was returned for a non-instance procedure call then output
               // a "success" message so it is clear something has occurred.
               if( app.empty( ) && list.row_errors.size( ) )
                  error_message = escape_markup( list.row_errors.begin( )->second );

               if( app.empty( ) && error_message.empty( ) )
                  error_message = GDS( c_display_completed_successfully );
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
               else if( old_password != p_session_info->user_pwd_hash )
                  error_message = string( c_response_error_prefix ) + GDS( c_display_old_password_is_incorrect );
               else
               {
                  string new_user_pwd_hash( new_password );
                  new_password = password_encrypt( new_password, get_server_id( ) );

                  vector< pair< string, string > > pwd_field_value_pairs;
                  pwd_field_value_pairs.push_back( make_pair( mod_info.user_pwd_field_id, new_password ) );

                  if( !perform_update( module_id, mod_info.user_class_id,
                   p_session_info->user_key, pwd_field_value_pairs, *p_session_info, &error_message ) )
                  {
                     if( error_message.empty( ) )
                        throw runtime_error( "unexpected server error occurred." );
                  }
                  else
                     p_session_info->user_pwd_hash = new_user_pwd_hash;
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
            error_message = string( GDS( c_display_error ) ) + ": " + error_message.substr( strlen( c_response_error_prefix ) );

         string view_permission_value;
         string view_security_level_value;

         setup_gmt_and_dtm_offset( input_data, *p_session_info );

         if( cmd == c_cmd_home )
         {
            server_command.erase( );

            bool issued_command = false;

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
                     home_info_cmd.replace( pos, strlen( c_user_key_arg ), get_uid_info( *p_session_info ) );
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

            string dummy_command( string( c_dummy_server_command ) + " -u="
             + p_session_info->user_key + ":" + p_session_info->user_id + " 0" );

            // NOTE: Issue a dummy command to ensure that a server session timeout won't occur.
            if( !issued_command && !simple_command( *p_session_info, dummy_command ) )
               throw runtime_error( "unexpected response to dummy server command" );
         }
         else if( cmd == c_cmd_pwd )
         {
            string dummy_command( string( c_dummy_server_command ) + " -u="
             + p_session_info->user_key + ":" + p_session_info->user_id + " 0" );

            // NOTE: Issue a dummy command to ensure that a server session timeout won't occur.
            if( !simple_command( *p_session_info, dummy_command ) )
               throw runtime_error( "unexpected response to dummy server command" );
         }
         else if( cmd == c_cmd_view || cmd == c_cmd_pview )
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
            if( is_new_record || act == c_act_edit || act == c_act_exec || act == c_act_view )
            {
               // NOTE: In the case where an edit occurs via an action a field assignment may
               // be required via use of the field/extra query string variables.
               if( act == c_act_edit && data != c_new_record && !field.empty( ) )
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
                     set_field_values += ",@trigger=" + userfetch;
                  }
               }

               // NOTE: Any "force" fields will always have their user provided values
               // sent to the application server.
               if( !skip_force_fields )
               {
                  for( size_t i = 0; i < view.user_force_fields.size( ); i++ )
                  {
                     if( userfetch == view.user_force_fields[ i ] )
                        was_user_force_field = true;
                     else if( user_field_info.count( view.user_force_fields[ i ] ) )
                     {
                        if( !set_field_values.empty( ) )
                           set_field_values += ",";
                        set_field_values += view.user_force_fields[ i ];
                        set_field_values += "=" + escaped( escaped( user_field_info[ view.user_force_fields[ i ] ], "," ), ",\"", c_nul, "rn\r\n" );
                     }
                  }
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

               set_field_values += "@executed=" + exec;
            }

            if( !fetch_item_info( view.module_id, mod_info, view.cid, item_key,
             view.field_list, set_field_values, "", *p_session_info, item_info, 0, 0, 0, 0, 0, &view_extra_vars ) )
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

               if( !record_not_found_error )
               {
                  view.state = state;
                  view.key_info = key_and_version;

                  bool has_always_editable = false;

                  size_t field_num = 0;
                  for( size_t i = 0; i < view.field_ids.size( ); i++ )
                  {
                     if( view.field_ids[ i ] == c_key_field )
                        continue;

                     map< string, string > extra_data;
                     if( !view.vici->second->fields[ i ].extra.empty( ) )
                        parse_field_extra( view.vici->second->fields[ i ].extra, extra_data );

                     if( !( state & c_state_is_changing ) && extra_data.count( c_view_field_extra_always_editable ) )
                        has_always_editable = true;

                     if( view.field_ids[ i ] == view.hpassword_salt_field )
                        hpassword_salt = item_values[ field_num ];

                     // NOTE: Password fields that are < 20 characters are assumed to not have been encrypted.
                     if( item_values[ field_num ].length( ) >= 20
                      && ( view.password_fields.count( view.field_ids[ i ] )
                      || view.encrypted_fields.count( view.field_ids[ i ] )
                      || view.hpassword_fields.count( view.field_ids[ i ] ) ) )
                        item_values[ field_num ] = password_decrypt( item_values[ field_num ], get_server_id( ) );

                     if( view.field_ids[ i ] == view.filename_field )
                        filename_value = item_values[ field_num ];

                     if( view.field_ids[ i ] == view.permission_field )
                        view_permission_value = item_values[ field_num ];

                     if( view.field_ids[ i ] == view.security_level_field )
                        view_security_level_value = item_values[ field_num ];

                     if( view.key_ids.id0 == view.value_ids[ i ] )
                        view.key_values.value0 = item_values[ field_num ];
                     else if( view.key_ids.id1 == view.value_ids[ i ] )
                        view.key_values.value1 = item_values[ field_num ];
                     else if( view.key_ids.id2 == view.value_ids[ i ] )
                        view.key_values.value2 = item_values[ field_num ];
                     else if( view.key_ids.id3 == view.value_ids[ i ] )
                        view.key_values.value3 = item_values[ field_num ];
                     else if( view.key_ids.id4 == view.value_ids[ i ] )
                        view.key_values.value4 = item_values[ field_num ];
                     else if( view.key_ids.id5 == view.value_ids[ i ] )
                        view.key_values.value5 = item_values[ field_num ];
                     else if( view.key_ids.id6 == view.value_ids[ i ] )
                        view.key_values.value6 = item_values[ field_num ];
                     else if( view.key_ids.id7 == view.value_ids[ i ] )
                        view.key_values.value7 = item_values[ field_num ];
                     else if( view.key_ids.id8 == view.value_ids[ i ] )
                        view.key_values.value8 = item_values[ field_num ];
                     else if( view.key_ids.id9 == view.value_ids[ i ] )
                        view.key_values.value9 = item_values[ field_num ];

                     if( view.vextra1_id == view.value_ids[ i ] )
                        view.vextra1_value = item_values[ field_num ];
                     else if( view.vextra2_id == view.value_ids[ i ] )
                        view.vextra2_value = item_values[ field_num ];

                     view.field_values[ view.value_ids[ i ] ] = item_values[ field_num++ ];
                  }

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
                  if( !view.actions_field.empty( ) )
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

                     // NOTE: If a "forced" field was actually a foreign key then replace its value with the key.
                     if( is_forced && user_field_info.count( view.fk_field_ids[ i ] ) )
                        user_field_info[ view.fk_field_ids[ i ] ] = item_values[ field_num ];

                     // NOTE: If field is not "forced" and has a user value then replace the item value with
                     // the user value.
                     if( !is_forced && user_field_info.count( view.fk_field_ids[ i ] ) )
                        item_values[ field_num ] = user_field_info[ view.fk_field_ids[ i ] ];

                     if( view.fkey_ids.id0 == view.fk_field_ids[ i ] )
                        view.fkey_values.value0 = item_values[ field_num ];
                     else if( view.fkey_ids.id1 == view.fk_field_ids[ i ] )
                        view.fkey_values.value1 = item_values[ field_num ];
                     else if( view.fkey_ids.id2 == view.fk_field_ids[ i ] )
                        view.fkey_values.value2 = item_values[ field_num ];
                     else if( view.fkey_ids.id3 == view.fk_field_ids[ i ] )
                        view.fkey_values.value3 = item_values[ field_num ];
                     else if( view.fkey_ids.id4 == view.fk_field_ids[ i ] )
                        view.fkey_values.value4 = item_values[ field_num ];
                     else if( view.fkey_ids.id5 == view.fk_field_ids[ i ] )
                        view.fkey_values.value5 = item_values[ field_num ];
                     else if( view.fkey_ids.id6 == view.fk_field_ids[ i ] )
                        view.fkey_values.value6 = item_values[ field_num ];
                     else if( view.fkey_ids.id7 == view.fk_field_ids[ i ] )
                        view.fkey_values.value7 = item_values[ field_num ];
                     else if( view.fkey_ids.id8 == view.fk_field_ids[ i ] )
                        view.fkey_values.value8 = item_values[ field_num ];
                     else if( view.fkey_ids.id9 == view.fk_field_ids[ i ] )
                        view.fkey_values.value9 = item_values[ field_num ];

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
                  if( cmd != c_cmd_pview && ( is_new_record || has_always_editable || act == c_act_edit ) )
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
                        if( lici->second->pclass == view.cid
                         && ( lici->second->type == c_list_type_child
                         || lici->second->type == c_list_type_user_child
                         || lici->second->type == c_list_type_child_admin
                         || lici->second->type == c_list_type_child_owner
                         || lici->second->type == c_list_type_child_admin_owner ) )
                        {
                           children.insert( make_pair( mod_info.get_string( lici->second->name ), lici ) );
                        }
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
                     string relative_prefix( string( c_files_directory )
                      + "/" + get_module_id_for_attached_file( view ) + "/" + ( vici->second )->cid + "/" );

                     string new_file_info( relative_prefix + "/" + session_id );

                     if( act == c_act_remove || file_exists( new_file_info.c_str( ) ) )
                     {
                        string new_file;
                        bool okay = false;

                        if( file_exists( new_file_info.c_str( ) ) )
                        {
                           ifstream inpf( new_file_info.c_str( ) );
                           getline( inpf, new_file );
                        }

                        vector< pair< string, string > > field_value_pairs;

                        // NOTE: It is expected that the file name in the "new_file_info" will be prefixed
                        // by the file's path so this will now be removed for the attached file field value.
                        string new_file_name( new_file );
                        if( !new_file_name.empty( ) && new_file_name[ 0 ] == '>' )
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

                           // NOTE: If a "modified" date/time field exists then update this to the current
                           // date/time.
                           if( !view.modify_datetime_field.empty( ) )
                           {
                              date_time dtm_modified;
                              get_session_dtm( *p_session_info, dtm_modified );

                              field_value_pairs.push_back(
                               make_pair( view.modify_datetime_field, dtm_modified.as_string( ) ) );
                           }

                           // FUTURE: Need to and report an error if the update fails.
                           if( perform_update( view.module_id, view.cid, data, field_value_pairs, *p_session_info ) )
                           {
                              performed_file_attach_or_detach = true;

                              string old_file( relative_prefix + "/" + view.field_values[ file_field_id ] );

                              if( file_exists( old_file.c_str( ) ) )
                                 remove( old_file.c_str( ) );

                              if( !new_file.empty( ) )
                              {
                                 okay = true;
                                 view.new_file_name = new_file;
                              }
                              else
                                 view.new_file_name = ".";
                           }
                           else if( !new_file.empty( ) )
                              remove( new_file.c_str( ) );
                        }

                        if( file_exists( new_file_info.c_str( ) ) )
                           remove( new_file_info.c_str( ) );
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
         else if( cmd == c_cmd_list || cmd == c_cmd_plist )
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

            if( !populate_list_info( list, list_selections, list_search_text, list_search_values,
             list_page_info, listsort, "", ( cmd == c_cmd_plist ), 0, "", &specials, *p_session_info ) )
               had_send_or_recv_error = true;

            string link_name( get_view_or_list_header( qlink, olist.name, mod_info, *p_session_info, &list.name ) );

            // NOTE: If there is a PDF specification then generate the file (unless session is anonymous).
            if( !using_anonymous && cmd == c_cmd_plist && !had_send_or_recv_error && !list.pdf_spec_name.empty( ) )
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
               extra_content_func += "document.cookie = '"
                + get_cookie_value( session_id, p_session_info->user_id,
                cmd == c_cmd_quit, p_session_info->dtm_offset, p_session_info->gmt_offset ) + "';";

            remove_session_temp_directory( session_id );

            if( had_send_or_recv_error || !mod_info.allows_anonymous_access )
            {
               string login_html( !cookies_permitted || !get_storage_info( ).login_days
                || g_login_persistent_html.empty( ) ? g_login_html : g_login_persistent_html );

               output_login_logout( module_name, extra_content, login_html, osstr.str( ) );
            }
         }

         if( finished_session )
         {
            remove_non_persistent( session_id );

            LOG_TRACE( "[logout: "
             + ( p_session_info->user_name.empty( ) ? p_session_info->user_id : p_session_info->user_name )
             + " at " + date_time::local( ).as_string( true, false ) + " from " + p_session_info->ip_addr + "]" );

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

            is_vertical = menu_opts.count( c_menu_opt_use_vertical_menu );

            if( cmd != c_cmd_pview && cmd != c_cmd_plist )
               extra_content << "\n<div id=\"normal_content\">\n";
            else
               extra_content << "\n<div id=\"print_content\">\n";

            if( cmd != c_cmd_pview && cmd != c_cmd_plist && !module_access_denied )
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
                  if( cmd != c_cmd_join && cmd != c_cmd_open )
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
                        testers.insert( "127.0.0.1" );

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
                     if( cmd != c_cmd_view || ident != user_info_view_id || data != p_session_info->user_key )
                     {
                        module_const_iterator mci, end;
                        for( mci = get_storage_info( ).modules.begin( ), end = get_storage_info( ).modules.end( ); mci != end; ++mci )
                        {
                           string next_module( mci->name );

                           if( !mci->perm.empty( ) && !p_session_info->user_perms.count( mci->perm ) )
                              continue;

                           string module_ref( get_storage_info( ).get_module_ref( next_module ) );

                           if( module_ref == user_info_module_ref && ( module_name == next_module || allow_module_switching ) )
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

                     if( has_user_link )
                        extra_content << "</a>";
                  }
                  else
                     extra_content << GDS( c_display_logged_in_as ) << " "
                      << ( p_session_info->user_name.empty( ) ? p_session_info->user_id : p_session_info->user_name );

                  if( !p_session_info->needs_pin && !mod_info.user_pwd_field_id.empty( ) && !p_session_info->is_openid )
                  {
                     if( cmd == c_cmd_pwd && !input_data.count( c_param_newpwd ) )
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

               if( !p_session_info->needs_pin && allow_module_switching && cmd != c_cmd_join )
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

                     if( !can_link || module_name == next_module )
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

                  if( !p_session_info->current_dtm.empty( ) )
                  {
                     time_info = date_time( p_session_info->current_dtm ).as_string( e_time_format_hhmm, true );
                     if( !p_session_info->tz_abbr.empty( ) )
                        time_info += ' ' + p_session_info->tz_abbr;
                  }
                  else
                  {
                     date_time dt( date_time::standard( ) + ( seconds )p_session_info->gmt_offset );
                     time_info = dt.as_string( e_time_format_hhmm, true );
                  }

                  extra_content << "         <div id=\"timeinfo\">" << time_info << "\n         </div>";
               }

               extra_content << "      </div >\n";
               extra_content << "   </div>\n";
               extra_content << "</div>\n";
            }

            map< string, vector< string > > menu_items;

            if( !p_session_info->needs_pin && cmd != c_cmd_join && cmd != c_cmd_open
             && cmd != c_cmd_pview && cmd != c_cmd_plist && !module_access_denied )
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
               bool is_admin_user;

               if( using_anonymous )
               {
                  user_group = "guests";
                  is_admin_user = false;
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
                  if( !list_perm.empty( ) && list_perm[ 0 ] == '!' )
                     list_perm.erase( 0, 1 );

                  bool has_perm( has_permission( list_perm, *p_session_info ) );

                  // FUTURE: Currently logic that requires a permission for an "admin" list is not supported
                  // (i.e. either the user is "admin" OR has the "perm"). If this is to be changed then this
                  // code would need to be changed.
                  if( !has_perm && p_session_info->is_admin_user && lmci->second->type == c_list_type_admin )
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
                  if( p_session_info->is_admin_user && lmci->second->type != c_list_type_admin
                   && lmci->second->type != c_list_type_child_admin && lmci->second->type != c_list_type_child_admin_owner )
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
                   || ( lmci->second->type != c_list_type_admin && lmci->second->type != c_list_type_child_admin ) ) )
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
                  extra_content << "     <li class=\"\"><a href=\"#\">" << i->first << "</a>\n";

                  if( i->second.size( ) )
                  {
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
               if( !has_selected_list && qlink.empty( ) && cmd == c_cmd_list && !uselect.empty( ) )
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

            if( cmd != c_cmd_pview && cmd != c_cmd_plist )
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

                     if( p_session_info->is_admin_user || ( using_anonymous
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

                              vector< string > columns;
                              raw_split( ( i->second ).row_data[ s ].second, columns );

                              string data;
                              if( !columns.empty( ) )
                                 data = columns[ columns.size( ) - 1 ];

                              replace_links_and_output( data, ( i->second ).view,
                               ( i->second ).module, ( i->second ).module_ref, extra_content, true, true,
                               session_id, *p_session_info, uselect, cookies_permitted, use_url_checksum );

                              extra_content << "               </div>\n";
                           }
                        }
                     }
                  }

                  if( found_slides )
                     extra_content << "            </div>\n";

                  extra_content << "        </div>\n";
                  extra_content << "    </div>\n";
               }
            }

            if( cmd != c_cmd_pview && cmd != c_cmd_plist )
               extra_content << "<div id=\"remainder\">\n";
            else
               extra_content << "<div id=\"print_remainder\">\n";

            extra_content << "\n<div id=\"main\" class=\"menubar_width clearfix\">\n";

            if( module_access_denied )
            {
               extra_content << "<p class=\"text_with_back\">"
                << string_message( GDS( c_display_session_logged_into_other_module ),
                make_pair( c_display_session_logged_into_other_module_parm_href,
                "<a href=\"javascript:history.back( )\">" ), "</a>" ) << "</p>";
            }
            else if( cmd == c_cmd_pview || cmd == c_cmd_plist )
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
               extra_content << "<h3 class=\"center\">" << pwd_display_name << "</h3>";

               if( !input_data.count( c_param_newpwd ) )
               {
                  string password_html( !cookies_permitted || !get_storage_info( ).login_days
                   || g_password_persistent_html.empty( ) ? g_password_html : g_password_persistent_html );

                  string::size_type pos = password_html.find( c_checked );
                  if( pos != string::npos )
                     str_replace( password_html, c_checked, p_session_info->is_persistent ? "checked" : "" );

                  str_replace( password_html, c_user_id, p_session_info->user_id );

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

               extra_content << "<h3>" << g_display_sign_up_for_an_account << "</h3>";

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
                        msleep( 2000 );
                  }
                  else
                  {
                     msleep( 2000 );
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
                     regex expr( "-----BEGIN PGP PUBLIC KEY BLOCK-----.*-----END PGP PUBLIC KEY BLOCK-----" );

                     string::size_type len;
                     string::size_type pos = expr.search( gpg_public_key, &len );

                     if( pos == string::npos )
                        error_message = "<p class=\"error\" align=\"center\">" + GDS( c_display_gpg_public_key_is_not_correctly_formatted ) + "</p>";
                     else
                     {
                        string pubkey( gpg_public_key.substr( pos, len ) );
                        write_file( "x.gpg", pubkey );

                        string cmd( "gpg --batch --import " );
                        cmd += "x.gpg >x.tmp 2>&1";

                        system( cmd.c_str( ) );

                        vector< string > lines;
                        buffer_file_lines( "x.tmp", lines );

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

                           string::size_type len;
                           string::size_type pos = expr.search( first_line, &len );

                           if( pos == string::npos )
                              had_unexpected_error = true;
                           else
                           {
                              result = first_line.substr( pos + len + 1 );

                              if( result == "not changed" && !is_help_request )
                              {
                                 error_message = "<p class=\"error\" align=\"center\">"
                                  + GDS( c_display_gpg_encrypted_email_should_have_already_been_sent )
                                  + "<br/>" + GDS( c_display_please_re_check_your_email ) + "</p>";

                                 error_message += "<p>" + string_message(
                                  GDS( c_display_to_instead_see_your_gpg_encrypted_information_here ), make_pair(
                                  c_display_to_instead_see_your_gpg_encrypted_information_here_parm_id, oreq_username ) ) + "</p>";
                              }
                           }

                           if( result == "imported" || ( result == "not changed" && is_help_request ) )
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
                                       cmd = "gpg --batch --delete-key --yes " + key + ">x.tmp 2>&1";
                                       if( system( cmd.c_str( ) ) != 0 )
                                          LOG_TRACE( buffer_file( "x.tmp" ) );

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
                           file_remove( "x.gpg" );
                        
                        file_remove( "x.tmp" );
                     }

                     if( error_message.empty( ) && !had_unexpected_error )
                     {
                        string password( uuid( ).as_string( ).substr( 2 ) );

                        string gpg_message( buffer_file( "join.txt" ) );

                        str_replace( gpg_message, c_app_name, get_app_name( ) );
                        str_replace( gpg_message, c_user_id, req_username );
                        str_replace( gpg_message, c_password, password );

                        write_file( key, gpg_message );

                        string gpg_password;
                        if( !simple_command( *p_session_info, "password gpg", &gpg_password ) )
                           throw runtime_error( "unable to access GPG password" );

                        gpg_password = password_decrypt( gpg_password, get_server_id( ) + string( c_salt_value ) );

                        string smtp_sender;
                        if( !simple_command( *p_session_info, "smtpinfo", &smtp_sender ) )
                           throw runtime_error( "unable to access SMTP info" );

                        cmd = "gpg --armor --recipient " + key
                         + " --encrypt --sign --trust-model always --batch --local-user "
                         + smtp_sender + " --passphrase " + gpg_password + " " + key + ">" + key + ".tmp 2>&1";

                        system( cmd.c_str( ) );

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
                           password = password_encrypt( password, get_server_id( ) );

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
                                 email = password_encrypt( email_addr, get_server_id( ) );
                                 gpg_key_file = get_storage_info( ).web_root + "/x.gpg";
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
                                    file_remove( key + "x.asc" );

                                    string cmd(  "gpg --batch --delete-key --yes " + key + ">x.tmp 2>&1" );
                                    if( system( cmd.c_str( ) ) != 0 )
                                       LOG_TRACE( buffer_file( "x.tmp" ) );
                                 }
                              }
                              else
                              {
                                 LOG_TRACE( "[add_user: " + req_username + " at "
                                  + date_time::local( ).as_string( true, false ) + " from " + raddr + "]" );
                              }
                           }

                           file_remove( "x.gpg" );
                           file_remove( "x.tmp" );

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
                                 string msg_file( "message?" + get_cwd( true ) + "/" + key + ".asc" );

                                 string smtp_result;

                                 simple_command( *p_session_info, "sendmail "
                                  + email_addr + " \"" + GDS( c_display_welcome_aboard ) + "!\" \""
                                  + GDS( c_display_see_attachment_for_details ) + "\" -attach=" + msg_file, &smtp_result );

                                 if( !smtp_result.empty( ) && smtp_result != c_response_okay )
                                    error_message = "<p class=\"error\" align=\"center\">" + smtp_result + "</p>";
                              }
                           }
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
                     string encrypted_email( password_encrypt( email, get_server_id( ) ) );

                     string new_key;
                     add_user( req_username, req_username, encrypted_email,
                      clone_key, req_username, error_message, mod_info, *p_session_info, &new_key, false );

                     // FUTURE: It would be better if the email was sent from the User class rather
                     // than being done separately here (that way a User record won't be created in
                     // the case where sending the activation email fails).
                     if( error_message.empty( ) )
                     {
                        LOG_TRACE( "[add_user: " + req_username + " at "
                         + date_time::local( ).as_string( true, false ) + " from " + raddr + "]" );

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

                  str_replace( signup_html, c_error_message, error_message );

                  str_replace( signup_html, c_user_id, oreq_username );

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
                     encrypted_email = password_encrypt( email, get_server_id( ) );

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

                     LOG_TRACE( "[add_user: " + req_username + " at "
                      + date_time::local( ).as_string( true, false ) + " from " + raddr + "]" );

                     extra_content << "<p align=\"center\"><b>"
                      << GDS( c_display_welcome_aboard ) << " " << req_username << " !</p>\n";

                     extra_content_func += "auto_refresh_seconds = 3;\nauto_refresh( );";
                  }
               }

               if( !has_completed )
               {
                  extra_content << "<h3>" << g_display_sign_up_for_an_account << "</h3>";

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
                     msleep( 2000 );
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

                     if( p_session_info->is_admin_user || ( using_anonymous
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
            else if( cmd == c_cmd_view || cmd == c_cmd_pview )
            {
               extra_content << "<div id=\"title\">";
               extra_content << "<h3><span class=\"alignleft\">";

               if( is_new_record )
                  extra_content << GDS( c_display_new ) << " " << mod_info.get_string( view.name );
               else
                  extra_content << get_view_or_list_header( qlink, view.name, mod_info, *p_session_info );

               extra_content << "</span>";

               if( cmd != c_cmd_pview )
                  extra_content << "<span class=\"alignright\"><a href=\"javascript:history.go( -"
                   << back_count << " );\">" << GDS( c_display_back ) << "</a></span>";

               extra_content << "</h3></div>\n";

               string user_slevel;

               if( !p_session_info->other_slevels.count( p_session_info->user_other ) )
                  user_slevel = p_session_info->user_slevel;
               else
                  user_slevel = p_session_info->other_slevels.find( p_session_info->user_other )->second;

               if( record_not_found_error )
               {
                  // NOTE: If a "jump back" is to occur after an action then don't display
                  // any error here (so that deleting via a view action can be supported).
                  if( back.empty( ) )
                  {
                     extra_content << "<p align=\"center\" class=\"error\">"
                      << GDS( c_display_error ) << ": " << GDS( c_display_record_not_found ) << ".</p>\n";
                  }
               }
               else if( !p_session_info->is_admin_user // NOTE: "admin" is always permitted access
                && ( ( !view.perm.empty( ) && !p_session_info->user_perms.count( view.perm ) )
                || ( view.type == c_view_type_admin || view.type == c_view_type_admin_print )
                || ( !view.permission_field.empty( ) && !has_permission( view_permission_value, *p_session_info ) )
                || ( !view.security_level_field.empty( ) && view_security_level_value.length( ) < user_slevel.length( ) ) ) )
                  extra_content << "<p align=\"center\" class=\"error\">"
                   << GDS( c_display_error ) << ": " << GDS( c_display_permission_denied ) << ".</p>\n";
               else
               {
                  view.id = "view";

                  string edit_field_list;

                  bool is_owner;
                  string extra_html_content;

                  bool is_editable = output_view_form( extra_content,
                   act, was_invalid, user_field_info, exec, cont, data, error_message,
                   extra, field, view, vtab_num, session_id, uselect, cookies_permitted,
                   embed_images, new_field_and_values, *p_session_info, edit_field_list, edit_timeout_func,
                   extra_content_func, use_url_checksum, !qlink.empty( ), show_opts, cmd == c_cmd_pview,
                   back_count, pdf_view_file_name, is_owner, extra_html_content, has_any_changing_records );

                  extra_content_func += "dataFieldList = '" + edit_field_list + "';\n";

                  uint64_t state( view.state );

                  if( !is_editable )
                     state |= c_state_uneditable;

                  // NOTE: For new records (and currently for "printable" views) do not output child lists.
                  // FUTURE: For "printable" views it makes sense to be able to have child lists although
                  // they should be identified differently (perhaps as "printchild") as the normal sublists
                  // may not be applicable to the printed version.
                  if( !is_new_record && view.type != c_view_type_print && view.type != c_view_type_admin_print )
                  {
                     extra_content << "<br/><br/>\n";
                     extra_content << "<table width=\"100%\">\n";
                     extra_content << "<tr><td>\n";

                     extra_content << "<table id=\"vtabc\" class=\"vtab\">\n";

                     extra_content << "<thead>\n";
                     extra_content << "<tr>\n";

                     int n = 0, x = 0;
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

                        if( !is_okay )
                        {
                           children_not_permitted.insert( i->first );
                           continue;
                        }

                        string name( get_display_name( mod_info.get_string( child_lists[ i->second ].name ) ) );

                        if( ++n == vtabc_num )
                           extra_content << "<th class=\"selected_tab\" align=\"center\">" << name << "</th>\n";
                        else
                        {
                           extra_content << "<td class=\"tab\" align=\"center\"><a href=\"javascript:";

                           if( use_url_checksum )
                           {
                              string checksum_values( string( c_cmd_view ) + data + ident + uselect );
                              string new_checksum_value( get_checksum( *p_session_info, checksum_values ) );

                              extra_content << "query_update( '"
                               << c_param_chksum << "', '" << new_checksum_value << "', true ); ";
                           }

                           extra_content << "query_update( 'bcount', '" << to_string( back_count + 1 ) << "', true ); ";
                           extra_content << "query_update( 'vtabc', '" << n << "' );\">" << name << "</a></td>\n";
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
                            ( cmd == c_cmd_pview ), &view, child_lists[ i->second ].new_pfield, 0, *p_session_info ) )
                              had_send_or_recv_error = true;
                        }
                     }

                     extra_content << "</tr>\n";
                     extra_content << "</thead>\n";

                     extra_content << "</table>\n";

                     n = 0;
                     for( map< string, int >::iterator i = child_names.begin( ); i != child_names.end( ); ++i )
                     {
                        if( children_not_permitted.count( i->first ) )
                           continue;

                        if( ++n != vtabc_num )
                           continue;

                        output_list_form( extra_content, child_lists[ i->second ],
                         session_id, uselect, "", ( cmd == c_cmd_pview ), cookies_permitted,
                         true, ( act == c_act_edit ), list_selections, list_search_text, list_search_values,
                         state, true, data, keep_checks, ident, "", *p_session_info, extra_content_func, specials, use_url_checksum, "",
                         findinfo + listsrch, selected_records, embed_images, !hashval.empty( ), is_owner, has_any_changing_records, back_count );
                     }

                     extra_content << "</td></tr></table>\n";
                  }

                  // FUTURE: The height should be grown dynamically via Javascript rather than hard-coded here.
                  if( !extra_html_content.empty( ) )
                     extra_content << "</br><iframe src=\""
                      << extra_html_content << "\" width=\"100%\" height=\"730px\"></iframe>";
               }

               if( cmd == c_cmd_pview && file_exists( ident + ".htms" ) )
                  extra_content << buffer_file( ident + ".htms" );
            }
            else if( cmd == c_cmd_list || cmd == c_cmd_plist )
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
                     extra_content << "<span class=\"alignright\"><a href=\"javascript:history.go( -"
                      << back_count << " );\">" << GDS( c_display_back ) << "</a></span>";

                  extra_content << "</h3></div>\n";

                  // NOTE: For non-print lists display links to any "display variations" (if found).
                  if( cmd != c_cmd_plist )
                  {
                     extra_content << "<table width=\"100%\">\n";
                     extra_content << "<tr><td>\n";

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

                  output_list_form( extra_content, list, session_id, uselect, error_message,
                   ( cmd == c_cmd_plist ), cookies_permitted, true, false, list_selections,
                   list_search_text, list_search_values, 0, false, "", keep_checks, "", oident,
                   *p_session_info, extra_content_func, specials, use_url_checksum, qlink, findinfo + listsrch,
                   selected_records, embed_images, !hashval.empty( ), false, has_any_changing_records, back_count, &pdf_list_file_name );

                  if( cmd != c_cmd_plist )
                     extra_content << "</td></tr></table>\n";
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

            if( !is_in_edit && has_any_changing_records )
            {
               string seconds;
               if( p_session_info->ip_addr == string( c_local_ip_addr ) )
                  seconds = to_string( c_auto_refresh_seconds_local );
               else
                  seconds = to_string( c_auto_refresh_seconds_remote );

               extra_content_func += "auto_refresh_seconds = " + seconds + ";\nauto_refresh( );";
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

               extra_content << "<p>Total number of sessions = " << g_sessions.size( ) << ".</p>\n";
            }

            extra_content << "</div>\n</div>\n\n";

            if( cmd != c_cmd_pview && cmd != c_cmd_plist )
               extra_content << g_footer_html;

            extra_content << "</div>\n";

            if( performed_file_attach_or_detach )
               extra_content_func += "refresh( false );\n"; // KLUDGE: Refresh so current version is viewed (otherwise actions will fail).

            // NOTE: Erase any existing cookie value first.
            if( created_session && cookies_permitted )
               extra_content_func += "document.cookie = '" + get_cookie_value( "", c_anon_user_key ) + "';\n";

            if( cookies_permitted )
               extra_content_func += "document.cookie = '"
                + get_cookie_value( session_id, p_session_info->user_id,
                cmd == c_cmd_quit, p_session_info->dtm_offset, p_session_info->gmt_offset ) + "';\n";

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

            if( cont != c_true && !new_key.empty( ) )
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
               if( !edit_timeout_func.empty( ) )
                  extra_content_func += "warn_refresh_func = '" + edit_timeout_func + "';\n";
               extra_content_func += "warn_refresh_seconds = warn_refresh_default;\n";
               extra_content_func += "warn_refresh( );\n";
            }

            if( p_session_info && p_session_info->logged_in )
               extra_content_func += "\nloggedIn = true;";

            if( !scrollx.empty( ) && !scrolly.empty( ) )
               extra_content_func += "\nscroll_page( " + scrollx + ", " + scrolly + " );";
         }
      }

      if( cmd != c_cmd_status )
      {
         extra_content_func += " serverId = '" + g_id + "';";
         extra_content_func += " uniqueId = '" + unique_id + "';";
      }

      extra_content << "<input type=\"hidden\" value=\"" << extra_content_func << "\" id=\"extra_content_func\"/>\n";
   }
   catch( exception& x )
   {
      LOG_TRACE( string( "error: " ) + x.what( )
       + " (at " + date_time::local( ).as_string( true, false ) + " from " + raddr + ")" );

      ostringstream osstr;
      osstr << "<p class=\"error\" align=\"center\">" << GDS( c_display_error ) << ": " << x.what( ) << "</p>\n";

      bool is_logged_in = false;
      if( !created_session && p_session_info && p_session_info->logged_in )
      {
         is_logged_in = true;

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
            is_logged_in = true;

            osstr << "<p align=\"center\">"
             << string_message( GDS( c_display_click_here_to_go_back ),
             make_pair( c_display_click_here_to_go_back_parm_href,
             "<a href=\"javascript:refresh( )\">" ), "</a>" ) << "</p>\n";
         }
      }

      if( !is_logged_in )
      {
         string login_html( !cookies_permitted || !get_storage_info( ).login_days
          || g_login_persistent_html.empty( ) ? g_login_html : g_login_persistent_html );

         if( created_session && p_session_info->logged_in )
         {
            login_html = "<p>" + string_message( GDS( c_display_click_here_to_login ),
             make_pair( c_display_click_here_to_login_parm_href,
             "<a href=\"" + get_module_page_name( module_ref, true )
             + "?cmd=" + string( c_cmd_login ) + "\">" ), "</a>" ) + "</p>";

            output_login_logout( module_name, extra_content, osstr.str( ), login_html );
         }
         else
            output_login_logout( module_name, extra_content, login_html, osstr.str( ) );
      }
      else
         extra_content << osstr.str( );

      if( is_logged_in )
         extra_content << g_footer_html;

      if( is_logged_in )
         extra_content << "<input type=\"hidden\" value=\"loggedIn = true;\" id=\"extra_content_func\"/>\n";
      else
         extra_content << "<input type=\"hidden\" value=\"serverId = '" + g_id + "'; uniqueId = '" + unique_id + "';\" id=\"extra_content_func\"/>\n";
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

         extra_content << "<input type=\"hidden\" value=\"loggedIn = true;\" id=\"extra_content_func\"/>\n";
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
            release_socket( p_session_info->p_socket );
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
         int num = 25 + rand( ) % 75;
         for( int i = 0; i < num; i++ )
            str += 'a' + rand( ) % 26;

         str += " -->\n";

         output = str + output + str;

         while( output.length( ) % 40 )
            output += ' ';

         crypt_decoded( p_session_info->user_pwd_hash, output, false );

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
      umask( DEFAULT_UMASK );
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
         _chdir( exe_path.c_str( ) );

      LOG_TRACE( "[started at: " + date_time::local( ).as_string( true, false ) + "]" );

      DEBUG_TRACE( "[read strings]" );

      init_strings( );
      init_extkeys( );

      g_login_html = buffer_file( c_login_file );
      g_footer_html = buffer_file( c_footer_file );
      g_openup_html = buffer_file( c_openup_file );
      g_signup_html = buffer_file( c_signup_file );
      g_activate_html = buffer_file( c_activate_file );
      g_password_html = buffer_file( c_password_file );
      g_ssl_signup_html = buffer_file( c_ssl_signup_file );
      g_authenticate_html = buffer_file( c_authenticate_file );

      g_ciyam_interface_html = buffer_file( c_ciyam_interface_file );

      if( file_exists( c_identity_file ) )
      {
         string identity( buffer_file( c_identity_file ) );
         set_server_id( identity );

         g_id = get_id_from_server_id( );
      }

      str_replace( g_login_html, c_login, GDS( c_display_login ) );
      str_replace( g_login_html, c_user_id, GDS( c_display_user_id ) );
      str_replace( g_login_html, c_password, GDS( c_display_password ) );

      str_replace( g_openup_html, c_user_name, GDS( c_display_user_name ) );
      str_replace( g_openup_html, c_account_type, GDS( c_display_account_type ) );
      str_replace( g_openup_html, c_send_request, GDS( c_display_send_request ) );
      str_replace( g_openup_html, c_account_type_0, GDS( c_display_account_type_0 ) );
      str_replace( g_openup_html, c_account_type_1, GDS( c_display_account_type_1 ) );
      str_replace( g_openup_html, c_account_type_2, GDS( c_display_account_type_2 ) );
      str_replace( g_openup_html, c_account_type_3, GDS( c_display_account_type_3 ) );
      str_replace( g_openup_html, c_open_up_introduction, GDS( c_display_open_up_openid_account_introduction ) );

      str_replace( g_signup_html, c_user_id, GDS( c_display_user_id ) );
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
      str_replace( g_activate_html, c_persistent, GDS( c_display_automatically_login ) );
      str_replace( g_activate_html, c_verify_password, GDS( c_display_verify_password ) );

      str_replace( g_password_html, c_old_password, GDS( c_display_old_password ) );
      str_replace( g_password_html, c_new_password, GDS( c_display_new_password ) );
      str_replace( g_password_html, c_change_password, GDS( c_display_change_password ) );
      str_replace( g_password_html, c_verify_new_password, GDS( c_display_verify_new_password ) );

      str_replace( g_ssl_signup_html, c_email, GDS( c_display_email ) );
      str_replace( g_ssl_signup_html, c_user_id, GDS( c_display_user_id ) );
      str_replace( g_ssl_signup_html, c_password, GDS( c_display_password ) );
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

      if( file_exists( c_login_persistent_file ) )
      {
         g_login_persistent_html = buffer_file( c_login_persistent_file );

         str_replace( g_login_persistent_html, c_login, GDS( c_display_login ) );
         str_replace( g_login_persistent_html, c_user_id, GDS( c_display_user_id ) );
         str_replace( g_login_persistent_html, c_password, GDS( c_display_password ) );
         str_replace( g_login_persistent_html, c_persistent, GDS( c_display_automatically_login ) );
      }

      if( file_exists( c_password_persistent_file ) )
      {
         g_password_persistent_html = buffer_file( c_password_persistent_file );

         str_replace( g_password_persistent_html, c_old_password, GDS( c_display_old_password ) );
         str_replace( g_password_persistent_html, c_new_password, GDS( c_display_new_password ) );
         str_replace( g_password_persistent_html, c_persistent, GDS( c_display_automatically_login ) );
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

         // KLUDGE: For some unknown reason when this FCGI interface is started automatically by Apache
         // (under Windows) it can crash, however, with the delay here this problem seems to be avoided.
         msleep( 500 );

         // FUTURE: Currently Apache's "mod_fcgid" only supports single threaded FCGI servers and under
         // Windows it is simply unable to get back a request that has been handled by any thread other
         // than the main one. Thus rather than force single threaded compilation a check is being made
         // to see if "mod_fastcgi" is in use before starting any other request handling threads.
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

