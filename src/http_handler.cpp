// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <cstring>
#  include <csignal>
#  include <map>
#  include <set>
#  include <atomic>
#  include <memory>
#  include <string>
#  include <fstream>
#  include <sstream>
#  include <stdexcept>
#endif

#include "http_handler.h"

#include "mime.h"
#include "base64.h"
#include "format.h"
#include "sha256.h"
#include "threads.h"
#include "date_time.h"
#include "utilities.h"
#include "ciyam_base.h"
#include "class_base.h"
#include "ssl_socket.h"
#include "fs_iterator.h"
#include "crypt_stream.h"
#include "ciyam_session.h"
#include "ciyam_variables.h"

//#define DEBUG

//#define LOCAL_REQUESTS_ONLY

using namespace std;

extern atomic< size_t > g_active_listeners;

extern volatile sig_atomic_t g_server_shutdown;

namespace
{

const int c_device_length = 10;

const int c_max_line_length = 10000;

const int c_max_request_lines = 100;

#include "ciyam_constants.h"

const char* const c_ext_js = "js";
const char* const c_ext_css = "css";
const char* const c_ext_gif = "gif";
const char* const c_ext_htm = "htm";
const char* const c_ext_jpg = "jpg";
const char* const c_ext_png = "png";
const char* const c_ext_ttf = "ttf";
const char* const c_ext_html = "html";

const char* const c_data_text = "text";
const char* const c_data_image = "image";
const char* const c_data_filename = "filename";
const char* const c_data_application = "application";

const char* const c_all_specials = " !\"#$%&'()*+,-./<=>?@[\\]^`{|}~";

const char* const c_data_separator = "\r\n\r\n";

const char* const c_json_escape_specials = "bfnrt\b\f\n\r\t";

const char* const c_get_request = "GET ";
const char* const c_post_request = "POST ";

const char* const c_index_html = "index.html";

const char* const c_api_prefix = "/api.";

const char* const c_cws_endpoint = "/api.cws";
const char* const c_echo_endpoint = "/api.echo";
const char* const c_upload_endpoint = "/api.upload";
const char* const c_ip_addr_endpoint = "/api.ip_addr";
const char* const c_storage_endpoint = "/api.storage";
const char* const c_version_endpoint = "/api.version";
const char* const c_unix_now_endpoint = "/api.unix_now";
const char* const c_post_limit_endpoint = "/api.post_limit";

const char* const c_boundary_prefix = "boundary=";

const char* const c_web_session_prefix = "web_session.";

const char* const c_web_demo_pin_1 = "10101";
const char* const c_web_demo_pin_2 = "10201";
const char* const c_web_demo_pin_3 = "10301";

const char* const c_web_lock_suffix = ".lock";
const char* const c_web_command_suffix = ".command";
const char* const c_web_message_suffix = ".message";
const char* const c_web_session_suffix = ".session";
const char* const c_web_started_suffix = ".started";

const char* const c_web_session_okay_response = "[okay]";

const char* const c_req_param_host = "Host:";

const char* const c_req_param_if_modified = "If-Modified-Since:";

const char* const c_http_1_1 = "HTTP/1.1";

const char* const c_http_200_OK = "200 OK";

const char* const c_http_301_Moved = "301 Moved Permanently";
const char* const c_http_304_Not_Mod = "304 Not Modified";
const char* const c_http_400_Bad_Req = "400 Bad Request";
const char* const c_http_404_Not_Found = "404 Not Found";
const char* const c_http_414_URI_Too_Long = "414 URI Too Long";
const char* const c_http_500_Internal_Error = "500 Internal Server Error";

const char* const c_http_date_prefix = "Date: ";
const char* const c_http_server_prefix = "Server: ";
const char* const c_http_location_prefix = "Location: ";
const char* const c_http_modified_prefix = "Last-Modified: ";
const char* const c_http_content_type_prefix = "Content-Type: ";
const char* const c_http_content_length_prefix = "Content-Length: ";

const char* const c_http_host_header = "Host";

const char* const c_http_content_type_header = "Content-Type";
const char* const c_http_content_length_header = "Content-Length";
const char* const c_http_content_disposition_header = "Content-Disposition";

const char* const c_http_access_control_allow_origin_all = "Access-Control-Allow-Origin: *";

const char* const c_http_connection_header_info = "Connection: keep-alive";

#ifdef LOCAL_REQUESTS_ONLY
const char* const c_http_keep_alive_header_info = "Keep-Alive: timeout=1, max=100";
#else
const char* const c_http_keep_alive_header_info = "Keep-Alive: timeout=10, max=100";
#endif

const char* const c_http_content_type_font_ttf = "font/ttf";

const char* const c_http_content_type_text_csv = "text/csv";
const char* const c_http_content_type_text_plain = "text/plain";

const char* const c_http_content_type_text_javascript = "text/javascript";

const char* const c_http_content_type_text_css_utf8 = "text/css; charset=UTF-8";
const char* const c_http_content_type_text_html_utf8 = "text/html; charset=UTF-8";
const char* const c_http_content_type_text_plain_utf8 = "text/plain; charset=UTF-8";

const char* const c_http_content_type_image_gif = "image/gif";
const char* const c_http_content_type_image_jpg = "image/jpg";
const char* const c_http_content_type_image_png = "image/png";

const char* const c_http_content_type_application_form = "application/x-www-form-urlencoded";
const char* const c_http_content_type_application_json = "application/json";

const char* const c_http_content_type_multipart_form_data = "multipart/form-data";

const char* const c_http_content_disposition_form_data = "form-data";

const char* const c_html_test_response = "<!doctype html>\n<html>\n<head><title>Test HTML Page</title></head>\n<body><p>This is a HTML response for test purposes.</p></body>\n</html>\n";

const char* const c_has_moved_html_response = "<!doctype html>\n<html>\n<head><title>Document Has Moved</title></head>\n<body><p>Document has moved <a href=\"DOCUMENT\">here</a>.</p></body>\n</html>\n";
const char* const c_not_found_html_response = "<!doctype html>\n<html>\n<head><title>Document Not Found</title></head>\n<body><p>Unable to find document 'DOCUMENT' (incorrect endpoint?).</p></body>\n</html>\n";

const char* const c_replace_document_marker = "DOCUMENT";

const char* const c_query_param_name_access = "access";
const char* const c_query_param_name_device = "device";
const char* const c_query_param_name_format = "format";
const char* const c_query_param_name_passwd = "passwd";
const char* const c_query_param_name_payload = "payload";
const char* const c_query_param_name_request = "request";
const char* const c_query_param_name_session = "session";
const char* const c_query_param_name_verbose = "verbose";

const char* const c_query_param_value_json = "json";
const char* const c_query_param_value_text = "text";

const char* const c_cws_request_quit = "quit";
const char* const c_cws_request_unlock = "unlock";
const char* const c_cws_request_save_style = "save_style";
const char* const c_cws_request_view_style = "view_style";
const char* const c_cws_request_list_styles = "list_styles";
const char* const c_cws_request_access_token = "access_token";

const int c_num_retries = 5;

const int c_minimum_timeout = 30;

const int c_max_empty_chunks = 10;

const int c_accept_timeout = 250;

const int c_shutdown_timeout = 150;

#ifdef LOCAL_REQUESTS_ONLY
const int c_ssl_accept_timeout = 1000;

const int c_initial_timeout = 500;
const int c_subsequent_timeout = 100;

const int c_response_timeout = 200;
#else
const int c_ssl_accept_timeout = 10000;

const int c_initial_timeout = 5000;
const int c_subsequent_timeout = 1000;

const int c_response_timeout = 2000;
#endif

const size_t c_save_data_delay = 500;
const size_t c_error_response_delay = 250;

inline string escaped_json( const string& s )
{
   return escaped( s, "/\"", '\\', c_json_escape_specials );
}

void restore_specials( string& str )
{
   string specials( c_all_specials );

   vector< string > hex_specials;

   for( size_t i = 0; i < specials.size( ); i++ )
   {
      string special;

      hex_encode( special, ( const unsigned char* )&specials[ i ], 1 );

      special = "%" + special;

      string::size_type pos = str.find( special );

      while( pos != string::npos )
      {
         str.erase( pos, special.size( ) );

         str.insert( pos, 1, specials[ i ] );

         pos = str.find( special );
      }
   }
}

string& url_decode( string& str )
{
   for( size_t i = 0; i < str.size( ); i++ )
   {
      if( str[ i ] == '%' )
      {
         if( i < ( str.size( ) - 2 ) )
         {
            char ch = hex_nibble( str[ i + 1 ], true );

            ch <<= 4;

            ch |= hex_nibble( str[ i + 2 ], true );

            str.erase( i, 3 );
            str.insert( i, 1, ch );
         }
      }
   }

   return str;
}

string format_date_time( const date_time& dtm )
{
   string formatted_date_time( dtm.weekday_name( true ) );

   formatted_date_time += ", ";

   day d = dtm.get_day( );

   if( d < 10 )
      formatted_date_time += '0';

   formatted_date_time += to_string( ( int )d ) + ' ';

   formatted_date_time += dtm.month_name( true ) + ' ';

   formatted_date_time += to_string( dtm.get_year( ) ) + ' ';

   formatted_date_time += dtm.get_time( ).as_string( e_time_format_hhmmss, true );

   formatted_date_time += " GMT";

   return formatted_date_time;
}

bool parse_query_params( const string& qry_info, map< string, string >& params )
{
   bool retval = true;

   if( !qry_info.empty( ) )
   {
      vector< string > pairs;

      split( qry_info, pairs, '&' );

      for( size_t i = 0; i < pairs.size( ); i++ )
      {
         string next_pair( pairs[ i ] );

         string::size_type pos = next_pair.find( '=' );

         if( pos == string::npos )
         {
            retval = false;

            break;
         }

         string name( next_pair.substr( 0, pos ) );
         string value( next_pair.substr( pos + 1 ) );

         if( params.count( name ) )
         {
            retval = false;

            break;
         }

         params[ name ] = url_decode( value );
      }
   }

   return retval;
}

const size_t c_cws_max_devices = 10;

const size_t c_cws_seed_reserve = 120;

const size_t c_cws_access_length = 5;

const size_t c_admin_lock_attempts = 10;
const size_t c_admin_retry_timeout = 100;

const size_t c_max_pin_create_attempts = 10;

mutex g_mutex;

string g_none;

string g_server_id;

string g_css_suffix;

string g_version_string;

bool g_cws_admin_locked = false;

string g_cws_admin_token;

set< string > g_cws_access_tokens;

map< string, set< string > > g_cws_access_devices;

inline size_t get_num_access_devices( const string& token )
{
   return ( !g_cws_access_devices.count( token ) ? 0 : g_cws_access_devices[ token ].size( ) );
}

bool g_is_devt_system = false;

atomic< size_t > g_active_handlers;

atomic< size_t > g_cws_active_commands;

atomic< size_t > g_num_request_handler;

size_t g_max_post_data_allowed = 0;

inline void increment_handlers( )
{
   ++g_active_handlers;
}

inline void deccrement_handlers( )
{
   --g_active_handlers;
}

struct cws_active_command
{
   cws_active_command( bool lock_for_administration )
   {
      guard g( g_mutex );

      if( lock_for_administration )
      {
         for( size_t i = 0; i < c_admin_lock_attempts; i++ )
         {
            if( !g_cws_active_commands )
               break;

            msleep( c_admin_retry_timeout );
         }

         if( g_cws_active_commands )
            throw runtime_error( "timed out trying to lock for adminstration" );

         g_cws_admin_locked = true;
      }

      ++g_cws_active_commands;
   }

   ~cws_active_command( )
   {
      --g_cws_active_commands;
   }
};

void remove_web_access( const string& token, const string& token_file )
{
   guard g( g_mutex );

   if( file_exists( token_file ) )
   {
      string old_token( opt_buffer_file( token_file ) );

      file_remove( token_file );

      if( !old_token.empty( ) )
      {
         string old_token_file( token_file );

         replace( old_token_file, token, old_token );

         file_remove( old_token_file );
      }
   }
}

bool has_web_session_access_token( const string& token,
 const string& token_file, const string& password, string& pin )
{
   guard g( g_mutex );

   bool retval = false;

   temp_umask tum( 077 );

   if( token == c_admin )
   {
      if( !file_exists( token_file ) )
      {
         if( pin.empty( ) )
            pin = random_characters( 5, 0, e_printable_type_numeric );

         ofstream outf( token_file.c_str( ), ios::out );

         outf << pin;

         outf.close( );

         if( outf.good( ) )
         {
            retval = true;

            g_cws_admin_token = pin;

            string token_pin_file( token_file.substr( 0, token_file.rfind( '.' ) ) + '.' + pin );

            file_touch( token_pin_file, 0, true );
         }
      }
   }
   else
   {
      if( file_exists( token_file ) && file_size( token_file ) )
         retval = true;
      else
      {
         bool is_pin_token = true;

         for( size_t i = 0; i < token.length( ); i++ )
         {
            if( ( token[ i ] < '0' ) || ( token[ i ] > '9' ) )
            {
               is_pin_token = false;

               break;
            }
         }

         // NOTE: It is expected that an "access_token"
         // command will have been processed previously
         // with the matching file used to create a new
         // access PIN.
         if( !is_pin_token )
         {
            bool okay = false;

            for( size_t i = 0; i < c_max_pin_create_attempts; i++ )
            {
               pin = random_characters( 5, 0, e_printable_type_numeric );

               string token_pin_file( token_file.substr( 0, token_file.rfind( '.' ) ) + '.' + pin );

               if( file_exists( token_pin_file ) )
                  continue;

               okay = true;

               file_touch( token_pin_file, 0, true );

               break;
            }

            if( !okay )
               throw runtime_error( "was unable to create an access PIN" );

            file_remove( token_file );
         }
         else
         {
            ofstream outf( token_file.c_str( ), ios::out );

            outf << sha256( password ).get_digest_as_string( );

            outf.close( );

            if( outf.good( ) )
               retval = true;
         }
      }
   }

   return retval;
}

}

class http_request_handler : public thread
{
   public:
#ifdef SSL_SUPPORT
   http_request_handler( ssl_socket* p_socket, const string& ip_addr );
#else
   http_request_handler( tcp_socket* p_socket, const string& ip_addr );
#endif

   ~http_request_handler( );

   void on_start( );

   private:
   bool using_tls;

   string ip_addr;

#ifdef SSL_SUPPORT
   std::unique_ptr< ssl_socket > up_socket;
#else
   std::unique_ptr< tcp_socket > up_socket;
#endif
};

#ifdef SSL_SUPPORT
http_request_handler::http_request_handler( ssl_socket* p_socket, const string& ip_addr )
#else
http_request_handler::http_request_handler( tcp_socket* p_socket, const string& ip_addr )
#endif
 :
 using_tls( false ),
 ip_addr( ip_addr )
{
   up_socket.reset( p_socket );

   if( !( *up_socket ) )
      throw runtime_error( "unexpected invalid socket in http_request_handler::http_request_handler" );

   up_socket->set_no_delay( );
   up_socket->set_no_linger( );

#ifdef SSL_SUPPORT
   if( this->up_socket->is_tls_handshake( ) )
   {
      this->up_socket->ssl_accept( c_ssl_accept_timeout );

      using_tls = true;
   }
#endif

   increment_handlers( );
}

http_request_handler::~http_request_handler( )
{
   deccrement_handlers( );
}

void http_request_handler::on_start( )
{
   bool first = true;
   bool had_request = false;

   string protocol_prefix( !using_tls ? "http" : "https" );

   protocol_prefix += "://";

   string handler_error;

   size_t handler = ++g_num_request_handler;

#ifdef DEBUG
   cerr << "\n*** started request/response ***" << endl;
#endif
   try
   {
      while( true )
      {
#ifdef DEBUG
         cerr << "\n" << date_time::local( ).as_string( e_time_format_hhmmssth, true ) << '\n' << endl;
#endif
         string next_line, http_request;

         next_line.reserve( c_max_line_length );
         http_request.reserve( c_max_line_length );

         vector< string > header_lines;

         map< string, string > header_info;

         bool had_empty = false;

         int retries = c_num_retries;

         while( true )
         {
            if( up_socket->read_line( next_line,
             ( first ? c_initial_timeout : c_subsequent_timeout ), c_max_line_length ) <= 0 )
            {
               if( up_socket->had_blank_line( ) )
               {
                  had_empty = true;

                  break;
               }

               if( !( *up_socket ) || had_request || !retries-- )
                  break;
               else
                  continue;
            }

            first = false;

            if( http_request.empty( ) )
            {
               http_request = next_line;

               clear_key( next_line, true );
            }
            else
            {
               header_lines.push_back( next_line );

               next_line.erase( );
            }

            if( header_lines.size( ) >= c_max_request_lines )
            {
               ostringstream osstr;

               osstr << c_http_1_1 << ' ' << c_http_414_URI_Too_Long << "\n\n";

               string response( osstr.str( ) );

               up_socket->send_n( ( unsigned char* )response.data( ), response.length( ), c_response_timeout );

               throw runtime_error( "max. allowed request lines was exceeded" );
            }
         }

         if( !had_empty || http_request.empty( ) )
         {
#ifdef DEBUG
            cerr << "(empty or incomplete request)" << endl;
#endif
            break;
         }

         had_request = true;

         TRACE_LOG( TRACE_DETAILS | TRACE_SESSION,
          to_comparable_string( handler, false, 8 ) + " - " + http_request );

#ifdef DEBUG
         cerr << "handler #" << handler << "\n[Request]\n" << http_request << endl;
#endif
         for( size_t i = 0; i < header_lines.size( ); i++ )
         {
            string name, data;

            string next( header_lines[ i ] );

            string::size_type pos = next.find( ": " );

            name = next.substr( 0, pos );

            if( pos != string::npos )
               data = next.substr( pos + 2 );

            header_info[ name ] = data;
#ifdef DEBUG
            if( i == 0 )
               cerr << "\n[Header Info]" << endl;

            cerr << header_lines[ i ] << endl;
#endif
         }

         string http_document, http_protocol, http_qry_info;

         string::size_type pos = http_request.find( ' ' );

         if( pos != string::npos )
         {
            http_document = http_request.substr( pos + 1 );

            http_request.erase( pos );

            pos = http_document.find( ' ' );

            if( pos != string::npos )
            {
               http_protocol = http_document.substr( pos + 1 );

               http_document.erase( pos );
            }

            pos = http_document.find( '?' );

            if( pos != string::npos )
            {
               http_qry_info = http_document.substr( pos + 1 );

               http_document.erase( pos );
            }
         }

#ifdef DEBUG
         cerr << "\nrequest ==> " << http_request << endl;
         cerr << "document ==> " << http_document << endl;
         cerr << "protocol ==> " << http_protocol << endl;

         if( !http_qry_info.empty( ) )
            cerr << "qry_info ==> " << http_qry_info << endl;
#endif

         bool found = false;

         bool is_verbose = false;

         bool was_endpoint = false;

         bool is_json_output = false;
         bool is_text_output = true;

         bool has_format_parameter = false;

         ostringstream osstr;

         string data, error, header, response, moved_document;

         // NOTE: If document ends in '/' then will
         // automatically append "index.html".
         if( http_document.find( c_api_prefix ) != 0 )
         {
            string::size_type pos = http_document.rfind( '/' );

            if( ( pos != string::npos )
             && ( pos == ( http_document.length( ) - 1 ) ) )
               http_document += c_index_html;
         }

         map< string, string > params;

         if( !http_qry_info.empty( ) )
         {
            if( !parse_query_params( http_qry_info, params ) )
               // FUTURE: This message should be handled as a server string message.
               error = "Invalid format for query parameters '" + http_qry_info + "'.";
            else
            {
               if( params.count( c_query_param_name_format ) )
               {
                  if( params[ c_query_param_name_format ] == c_query_param_value_json )
                     is_json_output = true;
                  else if( params[ c_query_param_name_format ] == c_query_param_value_text )
                     is_text_output = true;
                  else
                     // FUTURE: This message should be handled as a server string message.
                     error = "Invalid format value '" + params[ c_query_param_name_format ] + "'.";

                  if( error.empty( ) )
                     has_format_parameter = true;
               }

               if( params.count( c_query_param_name_verbose ) )
                  is_verbose = true;
            }
         }

         if( error.empty( )
          && header_info.count( c_http_content_length_header ) )
         {
            size_t data_length = from_string< size_t >( header_info[ c_http_content_length_header ] );

            if( !g_max_post_data_allowed || ( data_length <= g_max_post_data_allowed ) )
            {
               data = string( data_length, '\0' );

               int received = up_socket->recv_n( ( unsigned char* )data.data( ), data_length, c_subsequent_timeout );

               if( received < 0 )
               {
                  data.erase( );

                  error = "unexpected error occurred reading post data";
               }
               else
               {
                  if( received < data_length )
                  {
                     size_t remaining = ( data_length - received );

                     bool had_error = false;

                     size_t empty_chunks = 0;

                     while( remaining )
                     {
#ifdef DEBUG
                        cerr << "received = " << received << endl;
                        cerr << "remaining = " << remaining << endl;
#endif
                        int next_chunk = up_socket->recv_n(
                         ( unsigned char* )data.data( ) + received, remaining, c_subsequent_timeout );
#ifdef DEBUG
                        cerr << "next_chunk = " << next_chunk << '\n' << endl;
#endif
                        if( next_chunk < 0 )
                           had_error = true;
                        else if( next_chunk == 0 )
                        {
                           if( ++empty_chunks > c_max_empty_chunks )
                              had_error = true;
                        }
                        else
                        {
                           received += next_chunk;
                           remaining -= next_chunk;
                        }

                        if( had_error )
                        {
                           data.erase( );

                           error = "unexpected error occurred reading post data";

                           break;
                        }
                     }
                  }
               }
            }
            else
               // FUTURE: This message should be handled as a server string message.
               error = "Post data was too large (maximum allowed is " + format_bytes( g_max_post_data_allowed ) + ").";
         }

         bool unchanged = false;

         bool has_content_type = false;

         date_time now( date_time::standard( ) );

         string formatted_dtm( format_date_time( now ) );

         if( !data.empty( ) )
         {
            if( http_document == c_echo_endpoint )
            {
               found = true;

               was_endpoint = true;

               if( !header_info.count( c_http_content_type_header ) )
                  header_info[ c_http_content_type_header ] = c_http_content_type_text_plain_utf8;

               has_content_type = true;

               osstr << c_http_1_1 << ' ' << c_http_200_OK << '\n'
                << c_http_server_prefix << g_server_id << '\n' << c_http_date_prefix << formatted_dtm << '\n';

               osstr << c_http_content_type_prefix << header_info[ c_http_content_type_header ] << '\n';

               if( !is_json_output )
               {
                  response = data;

                  if( !has_format_parameter )
                     response += '\n';
               }
               else
                  response = "{\"data\":\"" + escaped_json( data ) + "\"}";
            }
            else if( http_document == c_upload_endpoint )
            {
               found = true;

               was_endpoint = true;

               string boundary, filename, local_filename;

               has_content_type = true;

               osstr << c_http_1_1 << ' ' << c_http_200_OK << '\n'
                << c_http_server_prefix << g_server_id << '\n' << c_http_date_prefix << formatted_dtm << '\n';

               size_t uploaded_size = data.length( );

               if( header_info.count( c_http_content_type_header ) )
               {
                  vector< string > parts;

                  split( header_info[ c_http_content_type_header ], parts, "; " );

                  if( parts.size( ) && ( parts[ 0 ] == c_http_content_type_multipart_form_data ) )
                  {
                     if( parts.size( ) > 1 )
                     {
                        string info( parts[ 1 ] );

                        if( info.find( c_boundary_prefix ) == 0 )
                           boundary = info.substr( strlen( c_boundary_prefix ) );
                     }
                  }
               }

               osstr << c_http_content_type_prefix << c_http_content_type_text_plain_utf8 << '\n';

               if( !boundary.empty( ) )
               {
                  mime_decoder decoder( c_http_content_type_prefix + header_info[ c_http_content_type_header ] );

                  string type( decoder.get_type( ) );
                  string subtype( decoder.get_subtype( ) );
                  string boundary( decoder.get_boundary( ) );
#ifdef DEBUG
                  cerr << "\n(decoder) type = " << type << endl;
                  cerr << "(decoder) subtype = " << subtype << endl;
#endif
                  string extra;

                  if( type == "multipart" )
                     extra = "\r\n";

                  string::size_type pos = data.find( boundary + extra );

                  if( pos == 0 )
                  {
                     data.erase( 0, boundary.size( ) + extra.size( ) );

                     string final_boundary( extra );

                     final_boundary += boundary + "--" + extra;

                     pos = data.rfind( final_boundary );

                     if( pos != string::npos )
                        data.erase( pos );

                     pos = data.find( c_data_separator );

                     // NOTE: For multipart the data will
                     // contain header lines separated by
                     // CRLFs. The header lines are taken
                     // from the data with CRLFs replaced
                     // with just LFs and then decoded.
                     if( pos != string::npos )
                     {
                        string prefix( data.substr( 0, pos + strlen( c_data_separator ) ) );

                        data.erase( 0, pos + strlen( c_data_separator ) );

                        replace( prefix, "\r\n", "\n" );

                        mime_decoder data_decoder( prefix );

#ifdef DEBUG
                        cerr << "\ndata_decoder.get_type( ) = " << data_decoder.get_type( ) << endl;
                        cerr << "data_decoder.get_subtype( ) = " << data_decoder.get_subtype( ) << endl;
#endif
                        string form_data( data_decoder.get_form_data( ) );

                        if( !form_data.empty( ) )
                        {
                           vector< string > lines;

                           split( form_data, lines, '\n' );

                           for( size_t i = 0; i < lines.size( ); i++ )
                           {
                              string next_line( lines[ i ] );

                              string::size_type pos = next_line.find( '=' );

                              string name( next_line.substr( 0, pos ) );

                              string value;

                              if( pos != string::npos )
                                 value = next_line.substr( pos + 1 );

                              if( name == c_data_filename )
                              {
                                 filename = value;

                                 restore_specials( filename );
#ifdef DEBUG
                                 cerr << "\n==> filename uploaded \"" << filename << "\"" << endl;
#endif
                              }
                           }
                        }

#ifdef DEBUG
                        local_filename = "x";
#else
                        local_filename = uuid( ).as_string( );
#endif

                        ofstream outf( local_filename );

                        outf << data;
                     }
                  }
               }

               if( !is_verbose )
                  response = local_filename;
               else
                  response = "Uploaded as \"" + local_filename + "\" (" + format_bytes( uploaded_size ) + ").";

               if( !has_format_parameter )
                  response += '\n';
            }
         }
         else if( error.empty( ) )
         {
            if( http_document == c_cws_endpoint )
            {
               was_endpoint = true;

               string prefix( c_web_session_prefix );

               string access, device, passwd, payload, request, session;

               if( params.count( c_query_param_name_access ) )
                  access = params[ c_query_param_name_access ];

               if( params.count( c_query_param_name_device ) )
                  device = params[ c_query_param_name_device ];

               if( params.count( c_query_param_name_passwd ) )
                  passwd = params[ c_query_param_name_passwd ];

               if( params.count( c_query_param_name_payload ) )
                  payload = params[ c_query_param_name_payload ];

               if( params.count( c_query_param_name_request ) )
                  request = params[ c_query_param_name_request ];

               if( params.count( c_query_param_name_session ) )
                  session = params[ c_query_param_name_session ];

               if( !passwd.empty( ) )
               {
                  if( base64::valid_characters( passwd, true ) )
                     passwd = base64::decode( passwd, true );
                  else
                  {
                     string name_and_value( c_query_param_name_passwd );

                     name_and_value += '=' + passwd;

                     // FUTURE: This message should be handled as a server string message.
                     error = "Invalid Base64 URL format in query parameter '" + name_and_value + "'.";
                  }
               }

               string request_args;

               string::size_type pos = request.find( ' ' );

               if( pos != string::npos )
               {
                  request_args = request.substr( pos + 1 );

                  request.erase( pos );
               }

               string access_file( '.' + prefix + access ); // i.e. ".web_session.<access>"

               string pin;

               string access_seed;

               access_seed.reserve( c_cws_seed_reserve );

               bool is_encrypted = false;

               bool is_identity_none = !has_identity( &is_encrypted );

               bool is_identity_reset = ( access.length( ) > c_cws_access_length );

               // NOTE: Only permits an "identity reset" to occur
               // if the system identity has not yet been created
               // or if it is currently locked.
               if( error.empty( ) && is_identity_reset
                && !is_encrypted && !is_identity_none )
                  // FUTURE: This message should be handled as a server string message.
                  error = "System identity is not currently locked.";

               bool lock_for_administration = ( error.empty( )
                && ( access != c_admin ) && ( is_identity_none || is_identity_reset ) );

               cws_active_command active_command( lock_for_administration );

               // NOTE: An "identity reset" can be used
               // to either change the "admin password"
               // or create the system identity.
               if( error.empty( ) && is_identity_reset )
               {
                  string demo_pin;

                  if( access == c_demo_entropy_1 )
                     demo_pin = c_web_demo_pin_1;
                  else if( access == c_demo_entropy_2 )
                     demo_pin = c_web_demo_pin_2;
                  else if( access == c_demo_entropy_3 )
                     demo_pin = c_web_demo_pin_3;

                  if( is_identity_none )
                  {
                     access_seed = access;

                     access = c_admin;
                     access_file = '.' + prefix + access;
                  }
                  else if( file_exists( c_ciyam_server_sid_chk_file ) )
                  {
                     string sid_chk_data( buffer_file( c_ciyam_server_sid_chk_file ) );

                     sha256 hash( access );

                     hash.update( hash.get_digest_as_string( ) );

                     if( hash.get_digest_as_string( ) == sid_chk_data )
                     {
                        access_seed = access;

                        access = c_admin;
                        access_file = '.' + prefix + access;

                        remove_web_access( access, access_file );
                     }
                     else
                        // FUTURE: This message should be handled as a server string message.
                        error = "System identity mismatch (incorrect mnemonics?).";
                  }

                  if( access == c_admin )
                     pin = demo_pin;
               }

               // NOTE: Only allow administration to occur
               // if using the correct "admin" access PIN.
               if( g_cws_admin_locked && ( access != c_admin ) )
               {
                  bool okay = true;

                  if( g_cws_active_commands > 1 )
                     okay = false;
                  else
                  {
                     string admin_pin( opt_buffer_file( '.' + prefix + c_admin ) );

                     if( access != admin_pin )
                        okay = false;
                  }

                  if( !okay )
                     // FUTURE: This message should be handled as a server string message.
                     error = "System is currently locked for administration.";
               }

               if( error.empty( ) && !access.empty( ) )
               {
                  bool has_access_file = has_web_session_access_token( access, access_file, passwd, pin );

                  if( has_access_file && pin.empty( ) && !g_cws_access_tokens.count( access ) )
                     g_cws_access_tokens.insert( access );

                  // NOTE: If is "admin locked" or the system identity is "unknown"
                  // and the access token matches the "admin" PIN then will set the
                  // identity (after first hardening the password that is then used
                  // to encrypt the entropy provided in the "request").
                  if( has_access_file && !request.empty( )
                   && !passwd.empty( ) && ( is_identity_none || g_cws_admin_locked ) )
                  {
                     string admin_pin( buffer_file( '.' + prefix + c_admin ) );

                     if( access == admin_pin )
                     {
                        guard g( g_mutex );

                        string hardened( passwd );

                        harden_key_with_hash_rounds( hardened, hardened, hardened, c_key_rounds_multiplier );

                        data_encrypt( request, request, hardened );

                        try
                        {
                           set_identity( request );

                           set_identity( passwd, request.c_str( ) );
                        }
                        catch( exception& x )
                        {
                           error = x.what( );
                        }

                        clear_key( hardened );

                        g_cws_admin_locked = false;
                     }

                     clear_key( request );
                  }
               }

               clear_key( passwd );

               string request_and_args;

               if( error.empty( ) && !request.empty( ) )
               {
                  request_and_args = request;

                  if( !request_args.empty( ) )
                     request_and_args += ' ' + request_args;
               }

               // NOTE: A seed will only be needed when
               // no existing system identity is found.
               bool is_seed_needed = is_identity_none;

               if( !error.empty( ) || access.empty( ) )
               {
                  if( error.empty( ) )
                     // FUTURE: This message should be handled as a server string message.
                     error = "Need a valid access token to use a web session.";
               }
               else if( !pin.empty( ) )
               {
                  string seed;

                  seed.reserve( c_cws_seed_reserve );

                  if( !access_seed.empty( ) )
                     seed = access_seed;
                  else if( !is_seed_needed )
                     seed = g_none;
                  else
                  {
                     get_mnemonics_or_hex_seed( seed, "" );
                     get_mnemonics_or_hex_seed( seed, seed );
                  }

                  found = true;

                  if( !is_json_output )
                     response = pin + ' ' + seed;
                  else
                     response = "{\"pin\":\"" + pin + "\", \"seed\":\"" + seed + "\"}";

                  clear_key( seed );
                  clear_key( access_seed );
               }
               else if( !g_cws_access_tokens.count( access ) )
                  // FUTURE: This message should be handled as a server string message.
                  error = "Provided web session access token '" + access + "' is invalid.";
               else if( device.empty( ) )
               {
                  size_t num_devices = get_num_access_devices( access );

                  if( num_devices >= c_cws_max_devices )
                     // FUTURE: This message should be handled as a server string message.
                     error = "Maximum devices have been created for web session access token '" + access + "'.";
                  else
                  {
                     found = true;

                     string new_device( uuid( ).as_string( ).substr( 0, c_device_length ) );

                     if( !is_json_output )
                        response = new_device;
                     else
                        response = "{\"new_device\":\"" + new_device + "\"}";

                     g_cws_access_devices[ access ].insert( new_device );
                  }
               }
               else
               {
                  if( !are_hex_nibbles( device, false ) || ( device.length( ) != c_device_length ) )
                     // FUTURE: This message should be handled as a server string message.
                     error = "Invalid device identity '" + device + "'.";
                  else if( !g_cws_access_devices[ access ].count( device ) )
                  {
                     size_t num_devices = get_num_access_devices( access );

                     if( num_devices < c_cws_max_devices )
                        g_cws_access_devices[ access ].insert( device );
                     else
                        // FUTURE: This message should be handled as a server string message.
                        error = "Maximum devices have been created for web session access token '" + access + "'.";
                  }

                  if( error.empty( ) )
                  {
                     string var_prefix( get_special_var_name( e_special_var_web ) + '.' );

                     string output_file_name( "/tmp/ciyam." + var_prefix.substr( 1 ) + access + '.' + device );

                     string web_lock_name( var_prefix + access + '.' + device + c_web_lock_suffix );

                     string web_command_name( var_prefix + access + '.' + device + c_web_command_suffix );
                     string web_message_name( var_prefix + access + '.' + device + c_web_message_suffix );
                     string web_session_name( var_prefix + access + '.' + device + c_web_session_suffix );
                     string web_started_name( var_prefix + access + '.' + device + c_web_started_suffix );

                     if( session.empty( ) )
                     {
                        string access_token( buffer_file( access_file ) );

                        string unique( uuid( ).as_string( ) );

                        if( !is_json_output )
                           response = unique;
                        else
                           response = "{\"unique\":\"" + unique + "\"}";

                        int64_t now = unix_time( );

                        if( !set_system_variable( web_lock_name, to_string( now ), string( "" ) ) )
                        {
                           int64_t was = from_string< int64_t >( get_system_variable( web_lock_name ) );

                           if( now < ( was + 10 ) )
                              // FUTURE: This message should be handled as a server string message.
                              error = "Web session is currently busy (try again shortly).";
                           else
                              set_system_variable( web_lock_name, to_string( now ) );
                        }

                        // NOTE: If is not currently handling another session
                        // request for the same device then the session value
                        // is obtained from the leading characters of a hash.
                        // The hash is itself obtained using a "checked" hash
                        // (being the "access_token" and "device" values) and
                        // then combining its digest with the "unqiue" string
                        // to determine a "combined" hash.
                        if( error.empty( ) )
                        {
                           found = true;

                           sha256 hash_checked( access_token + device );

                           sha256 hash_combined( hash_checked.get_digest_as_string( ) + unique );

                           string digest( hash_combined.get_digest_as_string( ) );

                           set_system_variable( web_session_name, digest.substr( 0, 10 ) );
                        }
                     }
                     else
                     {
                        if( session != get_system_variable( web_session_name ) )
                           // FUTURE: This message should be handled as a server string message.
                           error = "This web session is not valid (or has expired).";
                        else
                        {
                           if( has_system_variable( web_lock_name ) )
                           {
                              string when_locked( get_system_variable( web_lock_name ) );

                              set_system_variable( web_lock_name, "" );
                              set_system_variable( web_message_name, "" );

                              set_system_variable( web_started_name, when_locked );
                           }

                           if( request == c_cws_request_quit )
                           {
                              found = true;

                              if( !is_json_output )
                                 response = "Session terminated.";
                              else
                                 response = "{\"message\":\"Session terminated.\"}\n";

                              set_system_variable( web_message_name, "" );
                              set_system_variable( web_session_name, "" );
                              set_system_variable( web_started_name, "" );
                           }
                           else if( request == c_cws_request_unlock )
                           {
                              if( request_args.empty( ) )
                              {
                                 if( access != g_cws_admin_token )
                                    // FUTURE: This message should be handled as a server string message.
                                    error = "Unlock tokens can only be created by the administrator.";
                                 else
                                 {
                                    found = true;

                                    string unlock_key( create_unlock_sid_hash_key( false, false ) );

                                    if( !is_json_output )
                                       response = unlock_key;
                                    else
                                       response = "{\"unlock_key\":\"" + escaped_json( unlock_key ) + "\"}\n";
                                 }
                              }
                              else
                              {
                                 try
                                 {
                                    set_identity( request_args );

                                    found = true;
                                 }
                                 catch( exception& x )
                                 {
                                    error = x.what( );
                                 }
                              }
                           }
                           else if( request == c_cws_request_save_style )
                           {
                              if( payload.empty( ) )
                                 // FUTURE: This message should be handled as a server string message.
                                 error = "Stylesheet data was not provided.";
                              else
                              {
                                 found = true;

                                 string file_name( get_web_root( ) + '/' + access + g_css_suffix );

                                 write_file( file_name, payload );

                                 set_system_variable( e_special_var_cws_styles, "" );

                                 // NOTE: This delay is used to reduce possible I/O overloading.
                                 msleep( c_save_data_delay );
                              }
                           }
                           else if( request == c_cws_request_view_style )
                           {
                              string file_name( get_web_root( ) + '/' + request_args + g_css_suffix );

                              if( !file_exists( file_name ) )
                                 // FUTURE: This message should be handled as a server string message.
                                 error = "Stylesheet '" + file_name + "' was not found.";
                              else
                              {
                                 found = true;

                                 string style_data( opt_buffer_file( file_name ) );

                                 if( !is_json_output )
                                    response = style_data;
                                 else
                                    response = "{\"style_data\":\"" + escaped_json( style_data ) + "\"}\n";
                              }
                           }
                           else if( request == c_cws_request_list_styles )
                           {
                              found = true;

                              file_filter ff;

                              fs_iterator fs( get_web_root( ), &ff );

                              string all_styles( g_none );

                              bool has_cached_styles = has_system_variable( e_special_var_cws_styles );

                              if( !has_cached_styles )
                                 all_styles = g_none;
                              else
                                 all_styles = get_system_variable( e_special_var_cws_styles );

                              if( !has_cached_styles )
                              {
                                 set< string > css_files;

                                 size_t suffix_length = g_css_suffix.length( );

                                 while( fs.has_next( ) )
                                 {
                                    string next( fs.get_name( ) );

                                    if( next.size( ) > suffix_length )
                                    {
                                       string::size_type pos = next.rfind( g_css_suffix );

                                       if( pos == ( next.length( ) - suffix_length ) )
                                          css_files.insert( next.substr( 0, pos ) );
                                    }
                                 }

                                 for( set< string >::iterator i = css_files.begin( ); i!= css_files.end( ); ++i )
                                    all_styles += ' ' + *i;

                                 set_system_variable( e_special_var_cws_styles, all_styles );
                              }

                              string::size_type pos = all_styles.find( ' ' );

                              if( pos != string::npos )
                                 all_styles.erase( 0, pos + 1 );

                              if( !is_json_output )
                                 response = all_styles;
                              else
                                 response = "{\"all_styles\":\"" + escaped_json( all_styles ) + "\"}\n";
                           }
                           else if( request == c_cws_request_access_token )
                           {
                              if( access != g_cws_admin_token )
                                 // FUTURE: This message should be handled as a server string message.
                                 error = "Access tokens can only be created by the administrator.";
                              else
                              {
                                 found = true;

                                 string access_token( random_characters( 5, 0, e_printable_type_alpha_mixed ) );

                                 if( access_token == c_admin )
                                    throw runtime_error( "unexpected invalid random access token 'admin'" );

                                 temp_umask tum( 077 );

                                 file_touch( '.' + prefix + access_token, 0, true );

                                 if( !is_json_output )
                                    response = access_token;
                                 else
                                    response = "{\"access_token\":\"" + access_token + "\"}\n";
                              }
                           }
                           else
                           {
                              string script_name( prefix + "cin" );

                              bool running = has_system_variable( web_message_name );

                              if( !running )
                              {
#ifndef SSL_SUPPORT
                                 string cmd( "./ciyam_client -quiet -no_prompt -no_stderr -exec=\"<"
#else
                                 string cmd( "./ciyam_client -tls -quiet -no_prompt -no_stderr -exec=\"<"
#endif
                                  + script_name + ' ' + access + ' ' + device + ' ' + session + "\" > /dev/null &" );

                                 int rc = system( cmd.c_str( ) );

                                 ( void )rc;

                                 msleep( 200 );
                              }

                              // NOTE: Allows for a couple of
                              // seconds to start the session
                              // script (which will result in
                              // an error if is not found).
                              for( size_t i = 0; i < 9; i++ )
                              {
                                 running = has_system_variable( web_message_name );

                                 if( running )
                                    break;

                                 msleep( 200 );
                              }

                              if( !running )
                              {
                                 set_system_variable( web_session_name, "" );
                                 set_system_variable( web_started_name, "" );

                                 // FUTURE: This message should be handled as a server string message.
                                 error = "Was unable to start a web session with access token '" + access + "'.";
                              }
                              else
                              {
                                 found = true;

                                 bool allowed_command = true;

                                 if( !request.empty( ) )
                                 {
                                    if( !g_is_devt_system || ( access != g_cws_admin_token ) )
                                       allowed_command = false;
                                    else
                                    {
                                       if( ( request[ 0 ] < 'a' ) || ( request[ 0 ] > 'z' ) )
                                          allowed_command = false;
                                    }
                                 }

                                 if( !allowed_command )
                                    response = "[bad request]";
                                 else
                                 {
                                    if( !request.empty( ) )
                                       set_system_variable( web_command_name, request_and_args );
                                    else
                                       set_system_variable( web_command_name, "variable " + web_message_name );

                                    for( size_t i = 0; i < 10; i++ )
                                    {
                                       msleep( 150 );

                                       if( file_exists( output_file_name ) )
                                       {
                                          response = buffer_file( output_file_name );

                                          response = trim( response, false, true, "\n" );

                                          file_remove( output_file_name );

                                          break;
                                       }
                                    }

                                    if( response.empty( ) )
                                       response = "[empty response]";
                                 }
                              }
                           }

                           if( found && response.empty( ) )
                           {
                              string okay_reponse( c_web_session_okay_response );

                              if( !is_json_output )
                                 response = okay_reponse;
                              else
                                 response = "{\"response\":\"" + okay_reponse + "\"}\n";
                           }
                        }
                     }
                  }
               }
            }
            else if( http_document == c_echo_endpoint )
            {
               found = true;

               was_endpoint = true;

               string echo( http_qry_info.empty( ) ? "(empty parameters)" : http_qry_info );

               if( !is_json_output )
                  response = echo;
               else
                  response = "{\"parameters\":\"" + escaped_json( echo ) + "\"}";
            }
            else if( http_document == c_ip_addr_endpoint )
            {
               found = true;

               was_endpoint = true;

               if( !is_json_output )
                  response = ip_addr;
               else
                  response = "{\"ip_addr\":\"" + escaped_json( ip_addr ) + "\"}";
            }
            else if( http_document == c_storage_endpoint )
            {
               found = true;

               was_endpoint = true;

               string storage( get_system_variable( e_special_var_storage ) );

               if( !is_json_output )
                  response = storage;
               else
                  response = "{\"storage\":\"" + storage + "\"}";
            }
            else if( http_document == c_version_endpoint )
            {
               found = true;

               was_endpoint = true;

               string version( get_system_variable( e_special_var_version ) );

               if( !is_json_output )
                  response = version;
               else
                  response = "{\"version\":\"" + version + "\"}";
            }
            else if( http_document == c_unix_now_endpoint )
            {
               found = true;

               was_endpoint = true;

               string unix_now( to_string( unix_time( ) ) );

               if( !is_json_output )
                  response = unix_now;
               else
                  response = "{\"unix_now\":\"" + unix_now + "\"}";
            }
            else if( http_document == c_post_limit_endpoint )
            {
               found = true;

               was_endpoint = true;

               size_t val = g_max_post_data_allowed;

               string post_limit( !is_verbose ? to_string( val ) : format_bytes( val ) );

               if( !is_json_output )
                  response = post_limit;
               else
                  response = "{\"post_limit\":\"" + escaped_json( post_limit ) + "\"}";
            }
         }

         if( response.empty( ) )
         {
            string path( get_web_root( ) );

            if( !http_document.empty( ) && ( http_document[ 0 ] != '/' ) )
               path += '/';

            bool rc = true;

            // NOTE: Do a sanity check to make sure that the document
            // being requested is not outside of the "web root" path.
            string check_path( absolute_path( path + http_document, &rc ) );

            if( rc && ( check_path.find( path ) == 0 ) )
            {
               if( file_exists( path + http_document ) )
               {
                  found = true;

                  string extension;

#ifdef DEBUG
                  cerr << "\n(found '" << http_document << "')" << endl;
#endif
                  string::size_type ext_pos = http_document.rfind( '.' );

                  if( ext_pos != string::npos )
                     extension = http_document.substr( ext_pos + 1 );

                  date_time doc_modified( last_modification_time( path + http_document ) );

                  if( header_info.count( c_req_param_if_modified ) )
                  {
                     string gmt_date_time( header_info[ c_req_param_if_modified ] );

                     try
                     {
                        date_time doc_last_modified( gmt_date_time );

                        if( doc_modified == doc_last_modified )
                        {
                           unchanged = true;

                           has_content_type = true;

                           osstr << c_http_1_1 << ' ' << c_http_304_Not_Mod << "\n\n";
                        }
                     }
                     catch( exception& x )
                     {
                        // NOTE: If format is invalid then just ignores it.
#ifdef DEBUG
                        cerr << "http_request error: " << x.what( ) << endl;
#endif
                     }
                  }

                  if( !unchanged )
                  {
                     string formatted_document_dtm( format_date_time( doc_modified ) );

                     has_content_type = true;

                     osstr << c_http_1_1 << ' ' << c_http_200_OK
                      << '\n' << c_http_server_prefix << g_server_id
                      << '\n' << c_http_date_prefix << formatted_dtm
                      << '\n' << c_http_modified_prefix << formatted_document_dtm
                      << '\n' << c_http_connection_header_info << '\n' << c_http_keep_alive_header_info << '\n';

                     osstr << c_http_content_type_prefix;

                     if( extension == c_ext_js )
                        osstr << c_http_content_type_text_javascript;
                     else if( extension == c_ext_css )
                        osstr << c_http_content_type_text_css_utf8;
                     else if( extension == c_ext_gif )
                        osstr << c_http_content_type_image_gif;
                     else if( extension == c_ext_jpg )
                        osstr << c_http_content_type_image_jpg;
                     else if( extension == c_ext_png )
                        osstr << c_http_content_type_image_png;
                     else if( extension == c_ext_ttf )
                        osstr << c_http_content_type_font_ttf;
                     else if( ( extension == c_ext_htm ) || ( extension == c_ext_html ) )
                        osstr << c_http_content_type_text_html_utf8;
                     else
                        osstr << c_http_content_type_text_plain_utf8;

                     osstr << '\n';

                     response = buffer_file( path + http_document );
                  }
               }
               else if( dir_exists( path + http_document ) )
               {
                  if( file_exists( path + http_document + "/index.html" ) )
                     moved_document = http_document + '/';
               }
            }
         }

         if( found )
         {
            bool empty_but_not_unchanged = false;

            if( !unchanged && response.empty( ) )
               empty_but_not_unchanged = true;

            if( !has_content_type )
            {
               osstr << c_http_1_1 << ' ' << c_http_200_OK << '\n'
                << c_http_server_prefix << g_server_id << '\n' << c_http_date_prefix << formatted_dtm << '\n';

               osstr << c_http_access_control_allow_origin_all << '\n';

               osstr << c_http_content_type_prefix;

               if( empty_but_not_unchanged )
                  osstr << c_http_content_type_text_html_utf8;
               else
               {
                  if( !is_json_output )
                     osstr << c_http_content_type_text_plain_utf8;
                  else
                     osstr << c_http_content_type_application_json;
               }

               osstr << '\n';

               if( !response.empty( ) && !has_format_parameter )
                  response += '\n';
            }

            if( empty_but_not_unchanged )
               response = c_html_test_response;

            if( !response.empty( ) )
               osstr << c_http_content_length_prefix << to_string( response.length( ) ) << "\n\n" << response;
         }
         else if( !error.empty( ) )
         {
            msleep( c_error_response_delay );

            if( !is_json_output )
               error = "Error: " + error;
            else
               error = "{\"error\":\"" + escaped_json( error ) + "\"}";

            error += '\n';

            osstr << c_http_1_1 << ' ' << c_http_400_Bad_Req << '\n'
             << c_http_content_type_prefix << c_http_content_type_text_plain_utf8
             << '\n' << c_http_content_length_prefix << to_string( error.length( ) ) << "\n\n" << error;
         }
         else
         {
            bool has_moved = !moved_document.empty( );

            string moved_document_url;

            if( !has_moved )
            {
               response = c_not_found_html_response;

               replace( response, c_replace_document_marker, http_document );
            }
            else
            {
               moved_document_url = protocol_prefix
                + header_info[ c_http_host_header ] + moved_document;

               response = c_has_moved_html_response;

               replace( response, c_replace_document_marker, moved_document_url );
            }

            osstr << c_http_1_1 << ' '
             << ( has_moved ? c_http_301_Moved : c_http_404_Not_Found ) << '\n'
             << c_http_server_prefix << g_server_id << '\n' << c_http_date_prefix << formatted_dtm << '\n';

            if( has_moved )
               osstr << c_http_location_prefix << moved_document_url << '\n';

            osstr << c_http_content_type_prefix << c_http_content_type_text_html_utf8 << '\n';

            osstr << c_http_content_length_prefix << to_string( response.length( ) ) << "\n\n" << response;
         }

         response = osstr.str( );

         up_socket->send_n( ( unsigned char* )response.data( ), response.length( ), c_response_timeout );

         if( was_endpoint )
            break;
      }
   }
   catch( exception& x )
   {
      handler_error = x.what( );

#ifdef DEBUG
      cerr << "\nhttp_request error: " << handler_error << endl;
#endif

      TRACE_LOG( TRACE_INITIAL | TRACE_SESSION,
       string( "http_request error: " ) + handler_error );
   }
   catch( ... )
   {
      handler_error = "unexpected exception";

#ifdef DEBUG
      cerr << "\nhttp_request error: " << handler_error << endl;
#endif

      TRACE_LOG( TRACE_INITIAL | TRACE_SESSION,
       string( "http_request error: " + handler_error ) );
   }

   if( *up_socket && !handler_error.empty( ) )
   {
      msleep( c_error_response_delay );

      handler_error = "Error: " + handler_error + "\n";

      ostringstream osstr;

      osstr << c_http_1_1 << ' ' << c_http_500_Internal_Error << '\n'
       << c_http_content_type_prefix << c_http_content_type_text_plain_utf8
       << '\n' << c_http_content_length_prefix << to_string( handler_error.length( ) ) << "\n\n" << handler_error;

      string response( osstr.str( ) );

      up_socket->send_n( ( unsigned char* )response.data( ), response.length( ), c_response_timeout );
   }

   up_socket->close( );
#ifdef DEBUG
   cerr << "\n*** finished request/response ***" << endl;
#endif

   delete this;
}

#ifdef SSL_SUPPORT
void init_http_handler( ssl_socket* p_socket, const string& address )
#else
void init_http_handler( tcp_socket* p_socket, const string& address )
#endif
{
   try
   {
      http_request_handler* p_handler = new http_request_handler( p_socket, address );

      if( g_server_shutdown )
         delete p_handler;
      else
         p_handler->start( );
   }
   catch( exception& x )
   {
#ifdef DEBUG
      cerr << "init_http_handler: " << x.what( ) << endl;
#else
      TRACE_LOG( TRACE_INITIAL | TRACE_SESSION, string( "init_http_handler: " ) + x.what( ) );
#endif
   }
   catch( ... )
   {
#ifdef DEBUG
      cerr << "init_http_handler: unexpected exception occurred" << endl;
#else
      TRACE_LOG( TRACE_INITIAL | TRACE_SESSION, string( "init_http_handler: unexpected exception occurred" ) );
#endif
   }
}


http_listener::http_listener( int port )
 :
 port( port )
{
   ++g_active_listeners;

   register_listener( port, "http" );
}

http_listener::~http_listener( )
{
   unregister_listener( port );

   --g_active_listeners;
}

void http_listener::on_start( )
{
   try
   {
      tcp_socket s;

      bool okay = s.open( );

      if( !okay )
         throw runtime_error( "unable to open socket for port #" + to_string( port ) );

      ip_address address( port );

      s.set_reuse_addr( );

      okay = s.bind( address );

      if( !okay )
         throw runtime_error( "unable to bind address to port #" + to_string( port ) );

      okay = s.listen( );

      if( !okay )
         throw runtime_error( "unable to start listening on port #" + to_string( port ) );

      TRACE_LOG( TRACE_MINIMAL, "http listener started on tcp port " + to_string( port ) );

      g_none = get_special_var_name( e_special_var_none );

      g_server_id = string( c_CIYAM );

      g_css_suffix = '.' + string( c_ext_css );

      g_version_string = get_system_variable( e_special_var_version );

      if( !g_version_string.empty( ) )
         g_server_id += " (" + g_version_string + ")";

      g_max_post_data_allowed = get_max_http_post_allowed( );

      string admin_web_session_file( c_web_session_prefix );

      admin_web_session_file = "." + admin_web_session_file + c_admin;

      if( file_exists( admin_web_session_file ) )
         g_cws_admin_token = opt_buffer_file( admin_web_session_file );

      g_is_devt_system = has_system_variable( e_special_var_system_is_for_devt );

      while( true )
      {
         if( g_server_shutdown )
         {
            if( !g_active_handlers )
               break;
            else
               msleep( c_shutdown_timeout );
         }

#ifdef SSL_SUPPORT
         unique_ptr< ssl_socket > up_socket( new ssl_socket( s.accept( address, c_accept_timeout ) ) );
#else
         unique_ptr< tcp_socket > up_socket( new tcp_socket( s.accept( address, c_accept_timeout ) ) );
#endif

         if( *up_socket )
            init_http_handler( up_socket.release( ), address.get_addr_string( ) );
      }
   }
   catch( exception& x )
   {
#ifdef DEBUG
      cerr << "http_listener error: " << x.what( ) << endl;
#endif
      TRACE_LOG( TRACE_MINIMAL, string( "http_listener error: " ) + x.what( ) );
   }
   catch( ... )
   {
#ifdef DEBUG
      cerr << "unexpected http_listener exception" << endl;
#endif
      TRACE_LOG( TRACE_MINIMAL, "http_listener error: unexpected unknown exception caught" );
   }

   TRACE_LOG( TRACE_MINIMAL, "http listener finished (tcp port " + to_string( port ) + ")" );

   delete this;
}

void init_http_handler( int port )
{
   http_listener* p_http_listener = new http_listener( port );

   p_http_listener->start( );
}
