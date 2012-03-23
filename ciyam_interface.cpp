// Copyright (c) 2005
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
#  include <cmath>
#  include <ctime>
#  include <cstdio>
#  include <deque>
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

#include "fcgiapp.h"

#ifdef __GNUG__
#  define _chdir chdir
#  define _mkdir mkdir
#endif

#include "md5.h"
#include "sha1.h"
#include "base64.h"
#include "config.h"
#include "format.h"
#include "ptypes.h"
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
#include "fs_iterator.h"
#include "crypt_stream.h"
#include "ciyam_interface.h"

#define USE_UUID_FOR_LOGIN
//#define ALLOW_MULTIPLE_RECORD_ENTRY

#ifdef _WIN32
#  define USE_MOD_FASTCGI_KLUDGE
#endif
#define USE_MULTIPLE_REQUEST_HANDLERS

using namespace std;

namespace
{

const int c_num_handlers = 10;

const size_t c_max_fcgi_input_size = 65536;
const size_t c_max_param_input_size = 4096;

const int c_connect_timeout = 2500;
const int c_greeting_timeout = 2500;
const int c_initial_response_timeout = 7500;
const int c_subsequent_response_timeout = 2000;

#ifndef _WIN32
const int c_default_directory_perms = S_IRWXU;
#endif

const char* const c_license_file = "license.txt";

#ifndef _WIN32
const char* const c_kill_script = "ciyam_interface.kill.sh";
#else
const char* const c_kill_script = "ciyam_interface.kill.bat";
#endif

const char* const c_login_file = "login.htms";
const char* const c_footer_file = "footer.htms";
const char* const c_activate_file = "activate.htms";
const char* const c_password_file = "password.htms";
const char* const c_interface_file = "ciyam_interface.htms";
const char* const c_login_persistent_file = "login_persistent.htms";
const char* const c_password_persistent_file = "password_persistent.htms";

const char* const c_printlist_file = "printlist.htms";
const char* const c_printview_file = "printview.htms";

const char* const c_dummy_param_prefix = "dummy=";

const char* const c_http_param_ssl = "HTTPS";
const char* const c_http_param_host = "HTTP_HOST";
const char* const c_http_param_rhost = "REMOTE_HOST";
const char* const c_http_param_raddr = "REMOTE_ADDR";
const char* const c_http_param_query = "QUERY_STRING";
const char* const c_http_param_cookie = "HTTP_COOKIE";
const char* const c_http_param_uagent = "HTTP_USER_AGENT";

const char* const c_nbsp = "&nbsp;";

string g_nbsp( c_nbsp );

const char* const c_title = "@@title";
const char* const c_login = "@@login";
const char* const c_checked = "@@checked";
const char* const c_user_id = "@@user_id";
const char* const c_password = "@@password";
const char* const c_persistent = "@@persistent";
const char* const c_old_password = "@@old_password";
const char* const c_new_password = "@@new_password";
const char* const c_change_password = "@@change_password";
const char* const c_verify_password = "@@verify_password";
const char* const c_verify_new_password = "@@verify_new_password";

const char* const c_form_content_comment = "<!-- @@form_content -->";
const char* const c_extra_content_comment = "<!-- @@extra_content -->";

const char* const c_user_key_arg = "$user";

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
string g_password_html;
string g_activate_html;
string g_interface_html;

string g_login_persistent_html;
string g_password_persistent_html;

string g_display_login_info;
string g_display_change_password;

size_t g_max_user_limit = 1;

mutex g_session_mutex;

session_container g_sessions;

#ifdef USE_UUID_FOR_LOGIN
deque< string > g_uuids;
#endif

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

void disconnect_sockets( )
{
   guard g( g_socket_mutex );

   for( size_t i = 0; i < g_sockets.size( ); i++ )
   {
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

string hash_password( const string& salted_password )
{
   string s( salted_password );

   for( size_t i = 0; i < c_password_hash_rounds; i++ )
      s = sha1( s ).get_digest_as_string( );

   return s;
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
      LOG_TRACE << log_messages[ i ];
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

void output_login_logout( ostream& os,
 const string& extra_details, bool is_logout, bool is_error, const string& module_ref )
{
   os << "\n<div id=\"login_content\">\n";

   os << "\n<div id=\"nav\">\n";
   if( file_exists( "login_header.gif" ) )
      os << "<img src=\"login_header.gif\" alt=\"Login Header\"><br class=\"clear\"/>\n";
   os << "</div>\n";

   if( is_logout || is_error )
      os << "\n<div id=\"logout_text\">\n";

   if( !extra_details.empty( ) )
      os << extra_details;

   if( is_logout )
   {
      os << "<p align=\"center\">"
       << string_message( GDS( c_display_click_here_to_login ),
       make_pair( c_display_click_here_to_login_parm_href,
       "<a href=\"" + get_module_page_name( module_ref, true )
       + "?cmd=" + string( c_cmd_login ) + "\">" ), "</a>" ) << "</p>\n";
   }

   if( is_logout || is_error )
      os << "</div>\n";

   os << "</div>\n";
}

#ifdef USE_UUID_FOR_LOGIN
bool valid_uuid( const string& uuid )
{
   bool retval = false;

   guard g( g_session_mutex );

   for( size_t i = 0; i < g_uuids.size( ); i++ )
   {
      if( uuid == g_uuids[ i ] )
      {
         retval = true;
         break;
      }
   }

   return retval;
}

string get_latest_uuid( )
{
   guard g( g_session_mutex );

   return g_uuids.back( );
}

void strip_password( string& password, bool is_status_cmd )
{
   // NOTE: In order to help prevent a login replay attack a UUID that was
   // passed to the client must be appended to the password after an '@'.
   string::size_type pos = password.find( '@' );
   if( pos == string::npos )
   {
      if( !is_status_cmd )
         password.erase( );
   }
   else
   {
      string uuid = password.substr( pos + 1 );

      if( !valid_uuid( uuid ) )
         password.erase( );
      else
         password.erase( pos );
   }
}
#endif

void crypt_decoded( const string& pwd_hash, string& decoded, bool decode = true )
{
   // KLUDGE: As Javascript strings are UTF encoded to avoid losing data the encrypted
   // strings are actually "hex" (refer to similar KLUDGE comment in cat_interface.js).
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
      string next( sha1( last_hash ).get_digest_as_string( ) );

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
   if( file_exists( c_kill_script ) )
      file_remove( c_kill_script );

#ifdef USE_UUID_FOR_LOGIN
   int iter = 0;
#endif
   while( true )
   {
      msleep( 1000 );

      guard g( g_session_mutex );

#ifdef USE_UUID_FOR_LOGIN
      if( ++iter > 9 )
      {
         iter = 0;
         g_uuids.push_back( uuid( ).as_string( ) );

         if( g_uuids.size( ) > 3 )
            g_uuids.pop_front( );
      }
#endif

      vector< session_iterator > dead_sessions;

      time_t tm_now( time( 0 ) );

      for( session_iterator si = g_sessions.begin( ), end = g_sessions.end( ); si != end; ++si )
      {
         if( ( si->second )->locked )
            continue;

         if( tm_now - ( si->second )->tm_last_request > c_timeout_seconds )
         {
            LOG_TRACE << "session timeout";
            LOG_TRACE << "[logout: " << si->second->user_id
             << " at " << date_time::local( ).as_string( true, false )
             << " from " << si->second->ip_addr << "]";

            remove_session_temp_directory( si->second->session_id );

            delete si->second;
            dead_sessions.push_back( si );
         }
      }

      for( size_t i = 0; i < dead_sessions.size( ); i++ )
         g_sessions.erase( dead_sessions[ i ] );

      if( g_sessions.empty( ) )
      {
#ifndef USE_MULTIPLE_REQUEST_HANDLERS
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
               if(  t != mii->second->sio_mod )
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
      }

      if( !file_exists( c_kill_script ) )
      {
         ofstream outf( c_kill_script );
#ifdef _WIN32
         outf << "TASKKILL /F /PID " << get_pid( ) << '\n';
#else
         outf << "kill -9 " << get_pid( ) << '\n';
#endif
      }
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
   bool encrypt_data = false;
   bool temp_session = false;
   bool force_refresh = false;
   bool created_session = false;
   bool finished_session = false;

   string hash;
   string user;
   string title;
   string module_id;
   string module_ref;
   string module_name;
   string session_id;
   string interface_file;
   session_info* p_session_info = 0;
   ostringstream form_content, extra_content;

   DEBUG_TRACE << "[process request]";

   try
   {
      const char* p_ssl = FCGX_GetParam( c_http_param_ssl, *p_env );
      const char* p_host = FCGX_GetParam( c_http_param_host, *p_env );
      const char* p_rhost = FCGX_GetParam( c_http_param_rhost, *p_env );
      const char* p_raddr = FCGX_GetParam( c_http_param_raddr, *p_env );
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

      if( p_rhost )
         input_data.insert( make_pair( c_http_param_rhost, p_rhost ) );

      if( p_raddr )
         input_data.insert( make_pair( c_http_param_raddr, p_raddr ) );

      if( p_uagent )
         input_data.insert( make_pair( c_http_param_uagent, p_uagent ) );

      const char* p_query = FCGX_GetParam( c_http_param_query, *p_env );
      const char* p_cookie = FCGX_GetParam( c_http_param_cookie, *p_env );

      bool cookies_permitted( false );

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
      string username( input_data[ c_param_username ] );

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

      bool new_session = true;
      bool is_activation = ( cmd == c_cmd_activate );

      bool is_authorised = false;
      bool is_login_screen = false;

      if( !get_storage_info( ).modules_index.count( module_name ) )
         throw runtime_error( "cannot find information for module '" + module_name + "'" );

      set< string > login_opts;
      if( !get_storage_info( ).login_opts.empty( ) )
         split( get_storage_info( ).login_opts, login_opts, '+' );

      module_info& mod_info( *get_storage_info( ).modules_index.find( module_name )->second );

      module_id = mod_info.id;

      title = mod_info.title;

      if( !session_id.empty( ) && session_id != c_new_session )
      {
         if( user.empty( ) || ( !user.empty( ) && hash == get_user_hash( user ) ) )
         {
            p_session_info = get_session_info( session_id, false );

            if( !p_session_info )
            {
               session_id.erase( );
               if( is_kept && get_storage_info( ).login_days )
               {
                  username = user;
                  is_authorised = true;
               }
            }
         }
         else
            session_id.erase( );
      }

      if( cmd == c_cmd_status )
      {
         temp_session = true;
         session_id = c_new_session;
      }

      if( session_id.empty( ) || session_id == c_new_session )
      {
         if( username.empty( ) && password.empty( ) )
         {
            string login_html( !cookies_permitted || !get_storage_info( ).login_days
             || g_login_persistent_html.empty( ) ? g_login_html : g_login_persistent_html );

            if( cmd != c_cmd_activate )
            {
               is_login_screen = true;
               output_login_logout( extra_content, login_html, false, false, "" );
            }
            else
            {
               if( chksum != get_checksum( c_cmd_activate + user + data ) )
                  throw runtime_error( "Invalid URL" );

               extra_content << "\n<div id=\"login_content\">\n";

               extra_content << "\n<div id=\"nav\">\n";
               if( file_exists( "login_header.gif" ) )
                  extra_content << "<img src=\"login_header.gif\" alt=\"Login Header\"><br class=\"clear\"/>\n";
               extra_content << "</div>\n";

               extra_content << g_activate_html;

               extra_content << "</div>\n";
            }
         }
         else
         {
            if( cmd == c_cmd_activate )
            {
               username = user;
               cmd = c_cmd_login;
            }

            if( g_max_user_limit && get_num_sessions( ) >= g_max_user_limit )
               throw runtime_error( GDS( c_display_max_concurrent_users_logged_in ) );

            p_session_info = new session_info( get_storage_info( ) );

            created_session = true;
            p_session_info->locked = true;
            p_session_info->ip_addr = input_data[ c_http_param_raddr ];

            session_id = p_session_info->session_id = uuid( ).as_string( );

            if( !temp_session && ( is_authorised || persistent == c_true ) )
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
            p_session_info->tm_last_request = time( 0 );
      }

      string extra_content_func;
      bool has_just_logged_in = false;
      bool performed_file_attach_or_detach = false;

      if( !p_session_info )
      {
         if( !new_session )
         {
            DEBUG_TRACE << "session " << session_id << " terminated";

            remove_session_temp_directory( session_id );

            ostringstream osstr;
            osstr << "<p class=\"error\" align=\"center\">" << GDS( c_display_error )
             << ": " << GDS( c_display_your_session_has_been_terminated ) << ".</p>\n";

            output_login_logout( extra_content, osstr.str( ), true, true, module_ref );

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
         if( !p_session_info->p_socket )
         {
#ifndef USE_MULTIPLE_REQUEST_HANDLERS
            p_session_info->p_socket = &g_socket;
#else
            p_session_info->p_socket = get_tcp_socket( );
            if( !p_session_info->p_socket )
               throw runtime_error( "this service is currently busy - please try again later" );
#endif
            bool connection_okay = false;

            if( g_has_connected )
               connection_okay = true;
            else if( p_session_info->p_socket->okay( ) )
               connection_okay = true;
            else
            {
               DEBUG_TRACE << "[opening socket]";

               if( p_session_info->p_socket->open( ) )
               {
                  ip_address address( c_default_ciyam_host, c_default_ciyam_port );
                  if( p_session_info->p_socket->connect( address ), c_connect_timeout )
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

                     string license_info;
                     if( !simple_command( *p_session_info, "license", &license_info ) )
                        throw runtime_error( "Unable to determine license information." );

                     string::size_type pos = license_info.find( ':' );
                     if( pos == string::npos )
                        throw runtime_error( "unexpected license information '" + license_info + "'" );

                     string server_id( license_info.substr( 0, pos ) );

                     pos = server_id.find( '-' );
                     if( pos != string::npos )
                        server_id.erase( pos );

                     if( get_server_id( ) != server_id )
                     {
                        set_server_id( server_id );

                        ofstream outf( c_license_file );
                        outf << get_server_id( );
                     }

                     g_max_user_limit = ( size_t )atoi( license_info.substr( pos + 1 ).c_str( ) );

                     if( !simple_command( *p_session_info, "storage_init " + get_storage_info( ).storage_name ) )
                        throw runtime_error( "Unable to initialise '" + get_storage_info( ).storage_name + "' storage." );

                     string response;
                     if( !get_storage_info( ).has_web_root( ) )
                     {
                        if( !simple_command( *p_session_info, "storage_web_root -expand", &response ) )
                           throw runtime_error( "Unable to get web root from '" + get_storage_info( ).storage_name + "' storage." );

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
                           throw runtime_error( "Unable to get version information for module '" + mod_info.name + "'" );

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

            if( connection_okay && p_session_info->user_id.empty( ) )
            {
               if( mod_info.strings.empty( ) )
                  read_module_strings( mod_info, *p_session_info->p_socket );

               if( !mod_info.user_class_name.empty( ) )
               {
                  if( is_activation )
                  {
                     string field_list( mod_info.user_uid_field_id );
                     field_list += "," + mod_info.user_pwd_field_id;

                     string key_info( mod_info.user_uid_field_id );
                     key_info += " " + user;

                     bool login_okay = false;
                     pair< string, string > user_info;

                     if( !fetch_item_info( module_id, mod_info,
                      mod_info.user_class_id, key_info, field_list, "", *p_session_info, user_info, "" ) )
                        throw runtime_error( "unexpected error occurred checking activation" );

                     if( user_info.first.empty( ) )
                        throw runtime_error( GDS( c_display_unknown_or_invalid_user_id ) );

                     vector< string > user_data;
                     split( user_info.second, user_data );

                     if( user_data.size( ) < 2 )
                        throw runtime_error( "unexpected missing user information" );

                     string activate_password( password );
#ifdef USE_UUID_FOR_LOGIN
                     string::size_type pos = activate_password.find( '@' );
                     if( pos != string::npos )
                        activate_password.erase( pos );
#endif

                     // NOTE: An inactive account is expected to have its password being the same as the
                     // user id so this is checked before activation will be performed and therefore the
                     // password provided to activate must not be the same as the user id. If activation
                     // has been repeated then just treat as a standard login if the password is correct.
                     if( user_data[ 0 ] != user_data[ 1 ] )
                     {
                        if( activate_password == lower( hash_password( g_id + user_data[ 1 ] ) ) )
                           is_activation = false;
                        else
                           throw runtime_error( GDS( c_display_account_has_already_been_activated ) );
                     }

                     if( activate_password == lower( hash_password( g_id + user_data[ 0 ] ) ) )
                     {
                        force_refresh = true;
                        throw runtime_error( GDS( c_display_password_must_not_be_the_same_as_your_user_id ) );
                     }

                     if( is_activation )
                     {
                        vector< pair< string, string > > field_value_pairs;

                        activate_password = password_encrypt( activate_password, get_server_id( ) );

                        field_value_pairs.push_back( make_pair( mod_info.user_pwd_field_id, activate_password ) );
                        field_value_pairs.push_back( make_pair( mod_info.user_active_field_id, "1" ) );

                        if( !perform_update( module_id, mod_info.user_class_id, data,
                         field_value_pairs, *p_session_info->p_socket, data, p_session_info->tz_abbr ) )
                           throw runtime_error( "unexpected error occurred processing activation" );
                     }
                  }

#ifdef USE_UUID_FOR_LOGIN
                  strip_password( password, cmd == c_cmd_status );
#endif

                  fetch_user_record( g_id, module_id, module_name,
                   mod_info, *p_session_info, is_authorised || !base64_data.empty( ), true, username, password );

                  pwd_hash = p_session_info->user_pwd_hash;

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
#ifdef USE_UUID_FOR_LOGIN
                        strip_password( password, cmd == c_cmd_status );
#endif
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
                     throw runtime_error( GDS( c_display_you_are_currently_logged_in ) );

                  if( !mod_info.user_select_field.empty( ) )
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
                      mod_info, class_id, "", *p_session_info, false, 0, key_info,
                      field_list, "", "", "", *p_session_info->p_socket, p_session_info->select_data, "" ) )
                        throw runtime_error( "unexpected error occurred processing select info" );

                     if( mod_info.user_select_ofield.empty( ) )
                        sort_row_data_manually( p_session_info->select_data );

                     if( !mod_info.user_select_uo_field.empty( ) || !mod_info.user_select_sl_field.empty( ) )
                     {
                        deque< pair< string, string > >::iterator i;
                        for( i  = p_session_info->select_data.begin( ); i != p_session_info->select_data.end( ); ++i )
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

                  if( !permit_multiple_logins && !mod_info.user_qlink_class_id.empty( ) )
                     fetch_user_quick_links( mod_info, *p_session_info );
               }

               setup_gmt_and_dtm_offset( input_data, *p_session_info );

               // FUTURE: A User field should be available to be used to determine which
               // timezone abbreviation will be used above.
               if( p_session_info->tz_abbr.empty( ) )
                  p_session_info->tz_abbr = get_storage_info( ).tz_abbr;

               has_just_logged_in = true;
               p_session_info->logged_in = true;

               if( !temp_session )
               {
                  string path( c_files_directory );
                  path += "/" + string( c_tmp_directory );
                  path += "/" + session_id;

#ifdef _WIN32
                  if( _mkdir( path.c_str( ) ) != 0 )
#else
                  if( _mkdir( path.c_str( ), c_default_directory_perms ) != 0 )
#endif
                     throw runtime_error( "unable to create '" + path + "' directory" );

                  if( !is_non_persistent( session_id ) )
                     p_session_info->is_persistent = true;

                  LOG_TRACE << "[login: " << p_session_info->user_id
                   << " at " << date_time::local( ).as_string( true, false )
                   << " from " << p_session_info->ip_addr << "]";
               }
            }
         }

         string act( input_data[ c_param_act ] );
         string app( input_data[ c_param_app ] );
         string chk( input_data[ c_param_chk ] );
         string cls( input_data[ c_param_cls ] );
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
         if( keepchecks == "1" )
            keep_checks = true;

         int back_count = 1;
         if( !bcount.empty( ) )
         {
            back_count = atoi( bcount.c_str( ) );
            if( back_count < 1 )
               back_count = 1;
         }

         bool use_url_checksum = ( url_opts.count( c_url_opt_use_checksum ) > 0 );

         bool is_checksummed_home = ( cmd == c_cmd_home && username.empty( )
          && ( !uselect.empty( ) || !input_data[ c_param_utcdtm ].empty( ) ) );

         if( use_url_checksum && !cmd.empty( ) && ( is_checksummed_home
          || ( cmd != c_cmd_home && cmd != c_cmd_quit && cmd != c_cmd_login
          && cmd != c_cmd_status && ( cmd != c_cmd_view || ( act != c_act_cont && act != c_act_undo ) ) ) ) )
         {
            string prefix;

            if( act != c_act_qlink )
               prefix += act;

            string license_values( cmd + data );

            string checksum_values;

            if( cmd == c_cmd_home )
            {
               // NOTE: The uselect itself cannot be used in the checkum when it is being changed (as the
               // checksum value is worked out prior to the runtime selection) so uselextra is being used
               // instead. It's last value is remembered to ensure that direct tampering of uselect can't
               // succeed with a new (possibly random) uselextra value.
               checksum_values = session_id + uselextra;

               if( !uselect.empty( )
                && uselextra == p_session_info->last_uselextra && uselect != p_session_info->user_other )
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
            if( ( !hashval.empty( ) && hashval != get_hash( hash_values ) )
             || ( chksum != get_checksum( license_values ) && chksum != get_checksum( *p_session_info, checksum_values ) ) )
            {
#ifdef DEBUG
               for( map< string, string >::const_iterator ci = input_data.begin( ); ci != input_data.end( ); ++ci )
                  DEBUG_TRACE << "Input[ " << ci->first << " ] => " << ci->second;
#endif
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
            DEBUG_TRACE << "Input[ " << ci->first << " ] => " << ci->second;
#endif

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
            if( !has_just_logged_in
             && ( cmd == c_cmd_home || cmd == c_cmd_pwd || cmd == c_cmd_view
             || cmd == c_cmd_pview || cmd == c_cmd_list || cmd == c_cmd_plist ) )
               fetch_user_record( g_id, module_id, module_name, mod_info,
                *p_session_info, is_authorised, false, p_session_info->user_id, "" );
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

         // NOTE: If the user is able to select their group or other then assign it here.
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
                  if( uselect == "~" )
                     p_session_info->user_group.erase( );
                  else
                     p_session_info->user_group = uselect;
               }
            }
            else if( mod_info.user_select_field == mod_info.user_other_field_id )
            {
               if( has_just_logged_in || uselect.empty( )  )
                  uselect = p_session_info->default_user_other;
               else
               {
                  if( uselect == "~" )
                     p_session_info->user_other.erase( );
                  else
                     p_session_info->user_other = uselect;
               }
            }
         }

         view_source view;
         list_source list;
         list_source olist;

         string oident( ident );

         set< string > specials;

         string pdf_view_file_name;
         string pdf_list_file_name;

         map< string, int > child_names;
         vector< list_source > child_lists;
         map< string, row_error_container > child_row_errors;

         view_info_const_iterator vici = mod_info.view_info.end( );
         list_info_const_iterator lici = mod_info.list_info.end( );

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
            if( cmd != c_cmd_home || !mod_info.home_list_id.empty( ) )
            {
               if( cmd == c_cmd_home )
                  ident = mod_info.home_list_id;

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

               setup_list_fields( olist, pkey, module_name, *p_session_info );

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

               setup_list_fields( list, pkey, module_name, *p_session_info );
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
                     save_record( module_id, flags, app, chk, field, extra, exec, cont,
                      extrafields, is_new_record, new_field_and_values, extra_field_info, vici,
                      view, *p_session_info, act, data, new_key, error_message, was_invalid, had_send_or_recv_error );
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

                           if( !extra.empty( ) )
                              extra += ",";
                           extra += escaped( user_field_info[ view.user_force_fields[ i ] ], "," );
                        }
                     }
                  }

                  if( !perform_action( view.module, cls, act, app, field, fieldlist,
                   exec, extra, child_row_errors[ listarg ], *p_session_info->p_socket,
                   p_session_info->user_key, p_session_info->tz_abbr, p_session_info->gmt_offset ) )
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
                        data = error_message.substr( 1, error_message.size( ) - 2 );
                        error_message.erase( );

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
               else if( act == c_act_qlink )
                  add_quick_link( module_ref, cmd, data, extra, "", "", oident,
                   uselect, error_message, had_send_or_recv_error, mod_info, *p_session_info );
            }
            else if( cmd == c_cmd_home || cmd == c_cmd_list )
            {
               if( act == c_act_qlink )
                  add_quick_link( module_ref, cmd, findinfo, quicklink, listsrch, listsort, oident,
                   uselect, error_message, had_send_or_recv_error, mod_info, *p_session_info, &list_selections );
               else if( !perform_action( list.module, list.cid,
                act, app, field, "", exec, extra, list.row_errors, *p_session_info->p_socket,
                p_session_info->user_key, p_session_info->tz_abbr, p_session_info->gmt_offset ) )
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

         if( cmd == c_cmd_pwd )
         {
            server_command.erase( );

            if( input_data.count( c_param_newpwd ) )
            {
               string new_password( input_data[ c_param_newpwd ] );
               string old_password( input_data[ c_param_password ] );

               string::size_type pos = new_password.find( '@' );
               if( pos != string::npos )
                  new_password.erase( pos );

               pos = old_password.find( '@' );
               if( pos != string::npos )
                  old_password.erase( pos );

               if( new_password == lower( hash_password( g_id + p_session_info->user_id ) ) )
                  throw runtime_error( GDS( c_display_password_must_not_be_the_same_as_your_user_id ) );

               // NOTE: Only use the encrypted original password if it was decrypted when logging in.
               if( p_session_info->clear_password.length( ) )
               {
                  if( old_password == lower( hash_password( g_id + p_session_info->clear_password ) ) )
                     old_password = p_session_info->clear_password;
               }
               else
                  old_password = password_encrypt( old_password, get_server_id( ) );

               new_password = password_encrypt( new_password, get_server_id( ) );

               if( !perform_update( module_id, mod_info.user_class_id,
                p_session_info->user_key, mod_info.user_pwd_field_id, old_password,
                new_password, *p_session_info->p_socket, p_session_info->user_key, error_message ) )
               {
                  if( error_message.empty( ) )
                     throw runtime_error( "Unexpected server error occurred." );
               }
               else
                  p_session_info->clear_password.erase( );
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

         if( error_message.length( ) > strlen( c_response_error )
          && error_message.substr( 0, strlen( c_response_error ) ) == c_response_error )
            error_message = string( GDS( c_display_error ) ) + ": " + error_message.substr( strlen( c_response_error ) );

         string view_permission_value;
         string view_security_level_value;

         if( cmd == c_cmd_home )
         {
            server_command.erase( );

            bool issued_command = false;

            setup_gmt_and_dtm_offset( input_data, *p_session_info );

            if( !mod_info.home_info.empty( )
             && ( !p_session_info->user_group.empty( ) || !p_session_info->is_admin_user ) )
            {
               issued_command = true;
               string home_info_cmd = mod_info.home_info;

               while( true )
               {
                  string::size_type pos = home_info_cmd.find( c_user_key_arg );
                  if( pos == string::npos )
                     break;

                  home_info_cmd.replace( pos, strlen( c_user_key_arg ), p_session_info->user_key );
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

            if( !mod_info.home_list_id.empty( ) )
            {
               issued_command = true;
               list.id = c_list_prefix;

               string list_page_info( listinfo );

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

               if( !populate_list_info( list, list_selections, list_search_text,
                list_search_values, list_page_info, listsort, "", false, "", "", 0, *p_session_info ) )
                  had_send_or_recv_error = true;
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

            if( !fetch_item_info( view.module_id, mod_info, view.cid, item_key,
             view.field_list, set_field_values, *p_session_info, item_info, user_info ) )
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

                  size_t field_num = 0;
                  for( size_t i = 0; i < view.field_ids.size( ); i++ )
                  {
                     if( view.field_ids[ i ] == c_key_field )
                        continue;

                     // NOTE: Password fields that are < 20 characters are assumed to not have been encrypted.
                     if( item_values[ field_num ].length( ) >= 20
                      && ( view.password_fields.count( view.field_ids[ i ] )
                      || view.epassword_fields.count( view.field_ids[ i ] )
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

                  // NOTE: Populate lists for user selection (only if editing an existing or creating a new record).
                  if( is_new_record || act == c_act_edit )
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
                           if( previous_parents.count( info ) )
                              parent_row_data = view.parent_lists[ previous_parents[ info ] ];
                           else if( !fetch_parent_row_data( view.module_id, mod_info, data,
                            fld.field, fld.pclass, pfield, fld.pextra, *p_session_info, parent_key,
                            parent_row_data, *p_session_info->p_socket, &view.key_values, &view.fkey_values ) )
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

                  // NOTE: Obtain data for any specified child lists of the view item (unless creating or editing).
                  //
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

                        setup_list_fields( child_list, data, module_name, *p_session_info );

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

                        string child_list_info( c_list_prefix + to_string( child_lists.size( ) ) + c_info_suffix );
                        if( input_data.count( child_list_info ) )
                           child_list_info = input_data[ child_list_info ];
                        else
                           child_list_info.erase( );

                        string child_list_sort( c_list_prefix + to_string( child_lists.size( ) ) + c_sort_suffix );
                        if( input_data.count( child_list_sort ) )
                           child_list_sort = input_data[ child_list_sort ];
                        else
                           child_list_sort.erase( );

                        // NOTE: Only fetch data from the application server for the active child tab.
                        if( ++num == vtabc_num )
                        {
                           if( !populate_list_info( child_list, list_selections,
                            list_search_text, list_search_values, child_list_info, child_list_sort, data,
                            ( cmd == c_cmd_pview ), view.cid, child_list.new_pfield, 0, *p_session_info ) )
                              had_send_or_recv_error = true;
                        }

                        child_lists.push_back( child_list );
                        child_names.insert( make_pair( mod_info.get_string( child_lists.back( ).name ), child_lists.size( ) - 1 ) );
                     }
                  }

                  // NOTE: If this class has a file attachment field then check whether a new file has been attached.
                  //
                  // FUTURE: Currently no version information is included - the version/revision could be included as
                  // part of the "info" file data by including it in the "name" attribute (as is done for the class).
                  if( !had_send_or_recv_error && !view.attached_file_field.empty( ) )
                  {
                     string relative_prefix( string( c_files_directory )
                      + "/" + get_module_id_for_attached_file( view ) + "/" + ( vici->second )->cid + "/" );

                     string new_file_info( relative_prefix + "/" + session_id );

                     string old_file( relative_prefix + "/" + view.field_values[ view.attached_file_field ] );

                     if( file_exists( new_file_info.c_str( ) ) )
                     {
                        ifstream inpf( new_file_info.c_str( ) );

                        string new_file;
                        bool okay = false;

                        getline( inpf, new_file );

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

                           field_value_pairs.push_back( make_pair( view.attached_file_field, new_file_name ) );

                           date_time dtm_modified( date_time::standard( ) );

                           // NOTE: If a "modified" date/time field exists then update this to the current
                           // date/time (the 'U' prefix is used to indicate UTC).
                           if( !view.modify_datetime_field.empty( ) )
                              field_value_pairs.push_back(
                               make_pair( view.modify_datetime_field, "U" + dtm_modified.as_string( ) ) );

                           // FUTURE: Need to and report an error if the update fails.
                           if( perform_update( view.module_id, view.cid, data, field_value_pairs,
                            *p_session_info->p_socket, p_session_info->user_key, p_session_info->tz_abbr ) )
                           {
                              performed_file_attach_or_detach = true;

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

                        inpf.close( );
                        remove( new_file_info.c_str( ) );
                     }
                  }
               }

               // NOTE: If there is a PDF specification then generate the PDF file.
               if( !view.pdf_spec_name.empty( ) )
               {
                  string owner;

                  if( !view.owning_user_field.empty( ) && view.fk_field_values.count( view.owning_user_field ) )
                     owner = view.fk_field_values.find( view.owning_user_field )->second;

                  string pdf_title = get_view_or_list_header( qlink, view.name, mod_info, *p_session_info );

                  fetch_item_info( view.module_id, mod_info,
                   view.cid, item_key, view.field_list, set_field_values,
                   *p_session_info, item_info, user_info, &owner,
                   &view.pdf_spec_name, &pdf_title, &filename_value, &pdf_view_file_name );
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
             list_page_info, listsort, "", ( cmd == c_cmd_plist ), "", "", &specials, *p_session_info ) )
               had_send_or_recv_error = true;

            string link_name( get_view_or_list_header( qlink, olist.name, mod_info, *p_session_info, &list.name ) );

            // NOTE: If there is a PDF specification then generate the PDF file.
            if( cmd == c_cmd_plist && !had_send_or_recv_error && !list.pdf_spec_name.empty( ) )
            {
               populate_list_info( list, list_selections, list_search_text, list_search_values,
                list_page_info, listsort, "", ( cmd == c_cmd_plist ), "", "", &specials, *p_session_info,
                &list.pdf_spec_name, &link_name, &pdf_list_file_name );
            }
         }

         bool has_any_changing_records = false;

         // NOTE: If a browser refresh was issued after logging out then the initial command
         // will be "quit" so erase the command to ensure that a normal login will take place.
         if( created_session && cmd == c_cmd_quit )
         {
            cmd.erase( );
            server_command.erase( );
         }

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
               LOG_TRACE << "session terminated";

            ostringstream osstr;
            if( g_interface_html.find( c_form_content_comment ) == string::npos )
            {
               if( !had_send_or_recv_error )
                  osstr << "<p align=\"center\"><strong>"
                   << GDS( c_display_you_have_been_logged_out ) << ".</strong></p>\n";
               else
                  osstr << "<p class=\"error\" align=\"center\">" << GDS( c_display_error )
                   << ": " << GDS( c_display_your_session_has_been_terminated ) << ".</p>\n";
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

            if( cookies_permitted )
               extra_content_func += "document.cookie = '"
                + get_cookie_value( session_id, p_session_info->user_id,
                cmd == c_cmd_quit, p_session_info->dtm_offset, p_session_info->gmt_offset ) + "';";

            remove_session_temp_directory( session_id );

            output_login_logout( extra_content, osstr.str( ), true, false, module_ref );
         }

         if( finished_session )
         {
            remove_non_persistent( session_id );

            LOG_TRACE << "[logout: " << p_session_info->user_id
             << " at " << date_time::local( ).as_string( true, false )
             << " from " << p_session_info->ip_addr << "]";
         }
         else
         {
            string pwd_display_name;
            if( cookies_permitted && get_storage_info( ).login_days && !g_login_persistent_html.empty( ) )
               pwd_display_name = g_display_login_info;
            else
               pwd_display_name = g_display_change_password;

            if( cmd != c_cmd_pview && cmd != c_cmd_plist )
               extra_content << "\n<div id=\"normal_content\">\n";
            else
               extra_content << "\n<div id=\"print_content\">\n";

            if( cmd != c_cmd_pview && cmd != c_cmd_plist )
               extra_content << "<div id=\"remainder\">\n";
            else
               extra_content << "<div id=\"print_remainder\">\n";

            if( cmd != c_cmd_pview && cmd != c_cmd_plist && !module_access_denied )
            {
               extra_content << "\n<div id=\"header\">\n";

               string header_image_name( lower( module_ref ) + ".jpg" );
               if( file_exists( header_image_name.c_str( ) ) )
                  extra_content << "   <img src=\"" << header_image_name << "\" alt=\"Header\">\n";

               extra_content << "   <div id=\"navband\">\n";
               extra_content << "      <div id=\"nav\">\n";
               extra_content << "         <div id=\"username\">\n";

               if( !get_storage_info( ).user_info_view_id.empty( ) )
               {
                  extra_content << GDS( c_display_logged_in_as ) << " ";

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

                  extra_content << p_session_info->user_id;

                  if( has_user_link )
                     extra_content << "</a>";
               }
               else
                  extra_content << GDS( c_display_logged_in_as ) << " " << p_session_info->user_id;


               if( !mod_info.user_pwd_field_id.empty( ) )
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

               extra_content << " | <a href=\"" << get_module_page_name( module_ref ) << "?cmd=" << c_cmd_quit;

               if( !uselect.empty( ) )
                  extra_content << "&" << c_param_uselect << "=" << uselect;

               if( !cookies_permitted )
                  extra_content << "&session=" << session_id;

               if( use_url_checksum )
               {
                  string checksum_values( c_cmd_pwd + uselect );
                  extra_content << "&" << c_param_chksum << "=" << get_checksum( *p_session_info, checksum_values );
               }

               extra_content << "\">" << GDS( c_display_logout ) << "</a>";

               if( file_exists( "help.htm" ) )
                  extra_content << " | <a href=\"help.htm\" target=\"_blank\">" << GDS( c_display_help ) << "</a>";
               else if( file_exists( "help.html" ) )
                  extra_content << " | <a href=\"help.html\" target=\"_blank\">" << GDS( c_display_help ) << "</a>";

               extra_content << "\n         </div>\n";

               extra_content << "         <div id=\"uselects\">\n";

               // NOTE: If a user select option is specified and the user has access to it then include it.
               if( !mod_info.user_select_str_key.empty( )
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

               if( allow_module_switching )
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

               extra_content << "      </div >\n";
               extra_content << "   </div>\n";
               extra_content << "</div>\n";
            }

            extra_content << "\n<div id=\"main\">\n";

            if( module_access_denied )
            {
               extra_content << "<p align=\"center\">"
                << string_message( GDS( c_display_session_logged_into_other_module ),
                make_pair( c_display_session_logged_into_other_module_parm_href,
                "<a href=\"javascript:history.back( )\">" ), "</a>" ) << "</p>";
            }
            else if( cmd == c_cmd_pview || cmd == c_cmd_plist )
            {
               extra_content << "<p class=\"screen\" align=\"center\">"
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
               extra_content << "<h2>" << pwd_display_name << "</h2>";

               if( !input_data.count( c_param_newpwd ) )
               {
                  string password_html( !cookies_permitted || !get_storage_info( ).login_days
                   || g_password_persistent_html.empty( ) ? g_password_html : g_password_persistent_html );

                  string::size_type pos = password_html.find( c_checked );
                  if( pos != string::npos )
                     str_replace( password_html, c_checked, p_session_info->is_persistent ? "checked" : "" );

                  extra_content << password_html;
               }
               else
               {
                  if( error_message.empty( ) )
                     extra_content << "<p align=\"center\"><b>" << GDS( c_display_password_has_been_changed ) << "!</b></p>\n";
                  else
                     extra_content << "<p align=\"center\" class=\"error\">" << error_message << "</p>\n";
               }
            }
            else if( cmd == c_cmd_home )
            {
               extra_content << "<h2>" << GDS( c_display_welcome_to ) << " " << title << "</h2>\n";

               if( !user_home_info.empty( ) )
                  extra_content << "<p align=\"center\">" << user_home_info << "</p>\n";

               if( !mod_info.home_list_id.empty( ) )
               {
                  output_list_form( extra_content,
                   list, session_id, uselect, "", false,
                   cookies_permitted, true, false, list_selections, list_search_text,
                   list_search_values, 0, false, "", false, "", oident, *p_session_info, specials, use_url_checksum,
                   qlink, findinfo + listsrch, selected_records, encrypt_data, !hashval.empty( ), false, has_any_changing_records );
               }
            }
            else if( cmd == c_cmd_view || cmd == c_cmd_pview )
            {
               if( is_new_record )
                  extra_content << "<h2>"
                   << GDS( c_display_new ) << " " << mod_info.get_string( view.name ) << "</h2>\n";
               else
                  extra_content << "<h2>"
                   << get_view_or_list_header( qlink, view.name, mod_info, *p_session_info ) << "</h2>\n";

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
               else if( ( !view.perm.empty( ) && !p_session_info->user_perms.count( view.perm ) )
               || ( !p_session_info->is_admin_user
               && ( view.type == c_view_type_admin || view.type == c_view_type_admin_print ) )
               || ( !view.permission_field.empty( ) && !has_permission( view_permission_value, *p_session_info ) )
               || ( !view.security_level_field.empty( ) && view_security_level_value.length( ) < user_slevel.length( ) ) )
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
                   encrypt_data, new_field_and_values, *p_session_info, edit_field_list, edit_timeout_func,
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
                     extra_content << "<tr><td align=\"right\">\n";

                     extra_content << "<table id=\"vtabc\" class=\"vtab\">\n";

                     extra_content << "<thead>\n";
                     extra_content << "<tr>\n";

                     int n = 0;
                     for( map< string, int >::iterator i = child_names.begin( ); i != child_names.end( ); ++i )
                     {
                        string list_type = child_lists[ i->second ].lici->second->type;

                        bool is_okay = has_permission( child_lists[ i->second ].perm, *p_session_info );
                        if( !is_okay )
                        {
                           if( is_owner
                            && ( list_type == c_list_type_child_owner || list_type == c_list_type_child_admin_owner ) )
                              is_okay = true;

                           if( p_session_info->is_admin_user
                            && ( list_type == c_list_type_child_admin || list_type == c_list_type_child_admin_owner ) )
                              is_okay = true;
                        }
                        else if( list_type == c_list_type_child_owner )
                           is_okay = is_owner;
                        else if( list_type == c_list_type_child_admin )
                           is_okay = p_session_info->is_admin_user;
                        else if( list_type == c_list_type_child_admin_owner )
                           is_okay = ( is_owner || p_session_info->is_admin_user );

                        if( !is_okay )
                           continue;

                        string name( get_display_name( mod_info.get_string( child_lists[ i->second ].name ) ) );

                        if( ++n == vtabc_num )
                           extra_content << "<th class=\"tab\" align=\"center\">" << name << "</th>\n";
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
                     }

                     extra_content << "</tr>\n";
                     extra_content << "</thead>\n";

                     extra_content << "</table>\n";

                     n = 0;
                     for( map< string, int >::iterator i = child_names.begin( ); i != child_names.end( ); ++i )
                     {
                        if( ++n != vtabc_num )
                           continue;

                        string list_type = child_lists[ i->second ].lici->second->type;

                        bool is_okay = has_permission( child_lists[ i->second ].perm, *p_session_info );
                        if( !is_okay )
                        {
                           if( is_owner
                            && ( list_type == c_list_type_child_owner || list_type == c_list_type_child_admin_owner ) )
                              is_okay = true;

                           if( p_session_info->is_admin_user
                            && ( list_type == c_list_type_child_admin || list_type == c_list_type_child_admin_owner ) )
                              is_okay = true;
                        }
                        else if( list_type == c_list_type_child_owner )
                           is_okay = is_owner;
                        else if( list_type == c_list_type_child_admin )
                           is_okay = p_session_info->is_admin_user;
                        else if( list_type == c_list_type_child_admin_owner )
                           is_okay = ( is_owner || p_session_info->is_admin_user );

                        if( !is_okay )
                           continue;

                        output_list_form( extra_content, child_lists[ i->second ],
                         session_id, uselect, "", ( cmd == c_cmd_pview ), cookies_permitted,
                         true, ( act == c_act_edit ), list_selections, list_search_text, list_search_values,
                         state, true, data, keep_checks, ident, "", *p_session_info, specials, use_url_checksum, "",
                         findinfo + listsrch, selected_records, encrypt_data, !hashval.empty( ), is_owner, has_any_changing_records );
                     }

                     extra_content << "</td></tr></table>\n";
                  }

                  // FUTURE: The height should be grown dynamically by Javascript rather than hard-coded here.
                  if( !extra_html_content.empty( ) )
                     extra_content << "</br><iframe src=\""
                      << extra_html_content << "\" width=\"100%\" height=\"730px\"></iframe>";
               }

               if( cmd == c_cmd_pview && file_exists( ident + ".htms" ) )
                  extra_content << buffer_file( ident + ".htms" );
            }
            else if( cmd == c_cmd_list || cmd == c_cmd_plist )
            {
               if( ( !p_session_info->is_admin_user && list.type == c_list_type_admin )
                || ( !list.perm.empty( ) && !p_session_info->user_perms.count( list.perm ) ) )
                  extra_content << "<p align=\"center\" class=\"error\">"
                   << GDS( c_display_error ) << ": " << GDS( c_display_permission_denied ) << ".</p>\n";
               else
               {
                  extra_content << "<h2>"
                   << get_view_or_list_header( qlink, olist.name, mod_info, *p_session_info, &list.name ) << "</h2>\n";

                  // NOTE: For non-print lists display links to any "display variations" (if found).
                  if( cmd != c_cmd_plist )
                  {
                     extra_content << "<table width=\"100%\">\n";
                     extra_content << "<tr><td align=\"right\">\n";

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

                        extra_content << "</tr></table>\n";
                     }
                  }

                  output_list_form( extra_content, list, session_id, uselect, error_message,
                   ( cmd == c_cmd_plist ), cookies_permitted, true, false, list_selections,
                   list_search_text, list_search_values, 0, false, "", keep_checks, "", oident,
                   *p_session_info, specials, use_url_checksum, qlink, findinfo + listsrch,
                   selected_records, encrypt_data, !hashval.empty( ), false, has_any_changing_records, &pdf_list_file_name );

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
               extra_content_func += "auto_refresh_seconds = 5;\nauto_refresh( );";

            if( g_interface_html.find( c_form_content_comment ) != string::npos )
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

            extra_content << "</div>\n";

            if( cmd != c_cmd_pview && cmd != c_cmd_plist && !module_access_denied )
            {
               extra_content << "\n<div id=\"sidebar\">\n";
               extra_content << "   <ul>\n";

               if( cmd == c_cmd_home )
                  extra_content << "     <li class=\"sidebar_selected\">" << GDS( c_display_home ) << "</li>\n";
               else
               {
                  extra_content << "     <li><a href=\""
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

               bool has_selected_list = false;

               for( list_menu_const_iterator
                lmci = mod_info.list_menus.begin( ), end = mod_info.list_menus.end( ); lmci != end; ++lmci )
               {
                  bool has_perm( has_permission( lmci->second->perm, *p_session_info ) );

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

                  // NOTE: If the list type has been identified as "no access" then it will not be displayed.
                  if( lmci->second->type == c_list_type_no_access )
                     has_perm = false;

                  // NOTE: If the user does not have permission or if the list is a "group" type but the user
                  // doesn't belong to a group or if the list is an "admin" one but the user is not an admin
                  // user then omit it. Also omit "user" lists if the user is "admin" (it is probably not a
                  // good idea for the "admin" user to belong to a group but if so then the "admin" user will
                  // be able to access its "group" specific lists).
                  if( has_perm && lmci->second->type != c_list_type_home
                   && ( p_session_info->is_admin_user || lmci->second->type != c_list_type_admin )
                   && ( !p_session_info->is_admin_user || lmci->second->type != c_list_type_user )
                   && ( !p_session_info->user_group.empty( ) || lmci->second->type != c_list_type_group ) )
                  {
                     string display_name( get_display_name( lmci->first ) );

                     if( qlink.empty( ) && cmd == c_cmd_list && oident == lmci->second->id )
                     {
                        has_selected_list = true;
                        extra_content << "     <li class=\"sidebar_selected\">" << display_name << "</li>\n";
                     }
                     else
                     {
                        extra_content << "     <li><a href=\""
                         << get_module_page_name( module_ref )
                         << "?cmd=" << c_cmd_list << "&ident=" << lmci->second->id;

                        if( !uselect.empty( ) )
                           extra_content << "&" << c_param_uselect << "=" << uselect;

                        if( !cookies_permitted )
                           extra_content << "&session=" << session_id;

                        if( use_url_checksum )
                        {
                           string checksum_values( string( c_cmd_list ) + lmci->second->id + uselect );
                           extra_content << "&" << c_param_chksum << "=" << get_checksum( *p_session_info, checksum_values );
                        }

                        extra_content << "\">" << display_name << "</a></li>\n";
                     }
                  }
               }

               // NOTE: If the selected list has become inaccessible (to due a uselect change)
               // then force a jump back to "home".
               if( !has_selected_list && qlink.empty( ) && cmd == c_cmd_list && !uselect.empty( ) )
                  cmd.erase( );

               for( size_t i = 0; i < p_session_info->quick_link_data.size( ); i++ )
               {
                  string columns( p_session_info->quick_link_data[ i ].second );

                  vector< string > column_info;
                  raw_split( columns, column_info );

                  if( column_info.size( ) != 3 )
                     throw runtime_error( "unexpected incorrect # columns for quick link info" );

                  // FUTURE: To make quick link menu items look different italics are being hard-coded here.
                  // This should instead be implemented as a style.
                  if( !qlink.empty( ) && i == atoi( qlink.c_str( ) ) )
                     extra_content << "     <li class=\"sidebar_selected\"><i>" << column_info[ 1 ] << "</i></li>\n";
                  else
                  {
                     extra_content << "     <li><a href=\"" << column_info[ 0 ];
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

                     extra_content << "\"><i>" << column_info[ 1 ] << "</i></a></li>\n";
                  }
               }

               extra_content << "   </ul>\n";
               extra_content << "</div>\n";
            }

            if( cmd != c_cmd_pview && cmd != c_cmd_plist )
            {
               extra_content << "<br class=\"clear\"/>\n";
               extra_content << g_footer_html;
            }

            extra_content << "</div>\n";
            extra_content << "</div>\n";

            if( cmd.empty( ) && !module_access_denied )
            {
               string extra;

               if( !uselect.empty( ) )
                  extra += "&" + string( c_param_uselect ) + "=" + uselect;

               if( !cookies_permitted )
                  extra += "&session=" + session_id;

               if( use_url_checksum )
                  extra += "&chksum=" + get_checksum( *p_session_info, session_id + uselect );

               extra_content_func += "source = window.location.href.split( '?' );\n";
               extra_content_func += "var cd = new Date( ); window.location.replace( source[ 0 ] + '?cmd=home" + extra
                + "&uselextra=" + uselect + "&utcdtm=' + utc_dtm( cd ) + '&tzoffcur=' + escape( cd.getTimezoneOffset( ) ) );\n";
            }
            else if( performed_file_attach_or_detach )
               extra_content_func += "refresh( false );\n"; //KLUDGE: Refresh so current version is viewed (otherwise actions will fail).

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

            // NOTE: If editing then the timeout warning needs to result in a 'cont' action
            // (rather than just a page refresh) to ensure data entered/modified is not lost.
            if( !edit_timeout_func.empty( ) )
               extra_content_func += "warn_refresh_func = '" + edit_timeout_func + "';\n";
            extra_content_func += "warn_refresh_seconds = warn_refresh_default;\n";
            extra_content_func += "warn_refresh( );";

            if( p_session_info && p_session_info->logged_in )
               extra_content_func += "\nloggedIn = true;";

            if( !scrollx.empty( ) && !scrolly.empty( ) )
               extra_content_func += "\nscroll_page( " + scrollx + ", " + scrolly + " );";
         }
      }

      if( cmd != c_cmd_status )
      {
         extra_content_func += " serverId = '" + g_id + "';";
#ifdef USE_UUID_FOR_LOGIN
         extra_content_func += " uniqueId = '" + get_latest_uuid( ) + "';";

         // NOTE: Because the server generates a new UUID every 10 seconds (and keeps the previous
         // two generated UUIDs) if the login hasn't occurred within 30 seconds then refresh.
         if( is_login_screen )
            extra_content_func += " auto_refresh_seconds = 30; auto_refresh( );";
#endif
      }

      extra_content << "<input type=\"hidden\" value=\"" << extra_content_func << "\" id=\"extra_content_func\"/>\n";
   }
   catch( exception& x )
   {
      LOG_TRACE << "error: " << x.what( );

      ostringstream osstr;
      osstr << "<p class=\"error\" align=\"center\">"
       << GDS( c_display_error ) << ": " << x.what( ) << "</p>\n";

      bool is_logged_in = false;
      if( !created_session && p_session_info && p_session_info->logged_in )
      {
         is_logged_in = true;

         osstr << "<p align=\"center\">"
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

      output_login_logout( extra_content, osstr.str( ), !is_logged_in, true, created_session ? module_ref : "" );

      if( is_logged_in )
         extra_content << "<input type=\"hidden\" value=\"loggedIn = true;\" id=\"extra_content_func\"/>\n";
   }
   catch( ... )
   {
      extra_content << "<p class=\"error\" align=\"center\">" << GDS( c_display_unexpected_server_error ) << "</p>\n";

      LOG_TRACE << "unexpected error occurred";

      if( p_session_info && p_session_info->logged_in )
      {
         extra_content << "<p align=\"center\">"
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
      interface_html = g_interface_html;
   else
      interface_html = buffer_file( interface_file );

   // NOTE: UTF-8 text files can often begin with an identifying sequence "EF BB BF" as the
   // first three characters of the file so if the first byte is "EF" assume UTF-8 and strip.
   if( interface_html.size( ) >= 3 && interface_html[ 0 ] == ( char )0xef )
      interface_html.erase( 0, 3 );

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
         p_session_info->user_pwd_hash = sha1( p_session_info->user_pwd_hash ).get_digest_as_string( );
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

#ifdef USE_MULTIPLE_REQUEST_HANDLERS
   if( get_num_sessions( ) == 0 )
      disconnect_sockets( );
#endif
}

int main( int argc, char* argv[ ] )
{
   int rc = 0;

   try
   {
#ifndef _WIN32
      umask( 0 );
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

#ifdef USE_UUID_FOR_LOGIN
      g_uuids.push_back( uuid( ).as_string( ) );
#endif

      init_log( );

      LOG_TRACE << "[started at: " << date_time::local( ).as_string( true, false ) << "]";

      DEBUG_TRACE << "[read strings]";

      init_strings( );

      g_login_html = buffer_file( c_login_file );
      g_footer_html = buffer_file( c_footer_file );
      g_activate_html = buffer_file( c_activate_file );
      g_password_html = buffer_file( c_password_file );
      g_interface_html = buffer_file( c_interface_file );

      if( file_exists( c_license_file ) )
      {
         set_server_id( buffer_file( c_license_file ) );
         string key( get_server_id( ).substr( 0, 10 ) );

         MD5 md5;
         md5.update( ( unsigned char* )key.c_str( ), key.length( ) );
         md5.finalize( );

         auto_ptr< char > ap_digest( md5.hex_digest( ) );

         g_id = string( ap_digest.get( ) ).substr( 12 );
      }

      str_replace( g_login_html, c_login, GDS( c_display_login ) );
      str_replace( g_login_html, c_user_id, GDS( c_display_user_id ) );
      str_replace( g_login_html, c_password, GDS( c_display_password ) );

      str_replace( g_activate_html, c_login, GDS( c_display_login ) );
      str_replace( g_activate_html, c_password, GDS( c_display_password ) );
      str_replace( g_activate_html, c_persistent, GDS( c_display_automatically_login ) );
      str_replace( g_activate_html, c_verify_password, GDS( c_display_verify_password ) );

      str_replace( g_password_html, c_old_password, GDS( c_display_old_password ) );
      str_replace( g_password_html, c_new_password, GDS( c_display_new_password ) );
      str_replace( g_password_html, c_change_password, GDS( c_display_change_password ) );
      str_replace( g_password_html, c_verify_new_password, GDS( c_display_verify_new_password ) );

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

      DEBUG_TRACE << "[read storage info]";

      read_global_storage_info( );

      DEBUG_TRACE << "[now starting handlers]";

      // NOTE: Scope for timeout handler temporary object.
      {
         timeout_handler( ).start( );

#ifdef USE_MULTIPLE_REQUEST_HANDLERS
         FCGX_Init( );

         // FUTURE: Currently Apache's "mod_fcgid" only supports single threaded FCGI servers and under
         // Windows it is simply unable to get back a request that has been handled by any thread other
         // than the main one. Thus rather than force single threaded compilation a check is being made
         // to see if "mod_fastcgi" is in use before starting any other request handling threads.
#  ifdef USE_MOD_FASTCGI_KLUDGE
         if( has_environment_variable( "_FCGI_MUTEX_" ) ) // i.e. this var is only found in mod_fastcgi
#  endif
         {
            // Start all but one as separate threads - the main thread runs the final handler.
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
      LOG_TRACE << "[finished at: " << date_time::local( ).as_string( true, false ) << "]";
   }
   catch( exception& x )
   {
      get_log( ) << "error: " << x.what( ) << endl;
      rc = 1;
   }
   catch( ... )
   {
      get_log( ) << "unexpected exception caught" << endl;
      rc = 2;
   }

   if( file_exists( c_kill_script ) )
      file_remove( c_kill_script );

   return rc;
}

