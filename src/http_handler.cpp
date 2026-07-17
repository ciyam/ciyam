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
#include "regex.h"
#include "base64.h"
#include "format.h"
#include "threads.h"
#include "date_time.h"
#include "utilities.h"
#include "ciyam_base.h"
#include "class_base.h"
#include "ssl_socket.h"
#include "ciyam_session.h"
#include "ciyam_variables.h"
#include "ciyam_web_session.h"

//#define DEBUG

//#define LOCAL_REQUESTS_ONLY

using namespace std;

string g_none_tag;
string g_none_var;

string g_cws_admin_token;
string g_cws_admin_device;

string g_web_session_check;

string g_cws_username_for_prefix;

bool g_is_devt_system = false;

extern string g_temporary_directory;

extern atomic< size_t > g_active_listeners;

extern volatile sig_atomic_t g_server_shutdown;

namespace
{

const size_t c_max_line_length = 10000;

const size_t c_max_request_lines = 100;

#include "ciyam_constants.h"

constexpr const char* c_html = "html";

constexpr const char* c_ext_js = "js";
constexpr const char* c_ext_css = "css";
constexpr const char* c_ext_gif = "gif";
constexpr const char* c_ext_htm = "htm";
constexpr const char* c_ext_jpg = "jpg";
constexpr const char* c_ext_png = "png";
constexpr const char* c_ext_ttf = "ttf";
constexpr const char* c_ext_html = "html";

constexpr const char* c_data_text = "text";
constexpr const char* c_data_image = "image";
constexpr const char* c_data_filename = "filename";
constexpr const char* c_data_application = "application";

constexpr const char* c_all_specials = " !\"#$%&'()*+,-./<=>?@[\\]^`{|}~";

constexpr const char* c_data_separator = "\r\n\r\n";

constexpr const char* c_get_request = "GET";
constexpr const char* c_put_request = "PUT";
constexpr const char* c_head_request = "HEAD";
constexpr const char* c_post_request = "POST";
constexpr const char* c_trace_request = "TRACE";
constexpr const char* c_delete_request = "DELETE";
constexpr const char* c_options_request = "OPTIONS";

constexpr const char* c_index_html = "index.html";

constexpr const char* c_redirects_file = ".redirects";

constexpr const char* c_echo_endpoint = "/echo";
constexpr const char* c_upload_endpoint = "/upload";
constexpr const char* c_ip_addr_endpoint = "/ip-addr";
constexpr const char* c_storage_endpoint = "/storage";
constexpr const char* c_version_endpoint = "/version";
constexpr const char* c_unix_now_endpoint = "/unix-now";
constexpr const char* c_post_limit_endpoint = "/post-limit";

constexpr const char* c_cws_endpoint = "/cws";
constexpr const char* c_cws_endpoint_prefix = "/cws/";

constexpr const char* c_boundary_prefix = "boundary=";

constexpr const char* c_req_param_host = "Host:";

constexpr const char* c_req_param_if_modified = "If-Modified-Since:";

constexpr const char* c_http_1_1 = "HTTP/1.1";

constexpr const char* c_http_200_OK = "200 OK";

constexpr const char* c_http_301_Moved = "301 Moved Permanently";
constexpr const char* c_http_304_Not_Mod = "304 Not Modified";
constexpr const char* c_http_400_Bad_Req = "400 Bad Request";
constexpr const char* c_http_404_Not_Found = "404 Not Found";
constexpr const char* c_http_414_URI_Too_Long = "414 URI Too Long";
constexpr const char* c_http_500_Internal_Error = "500 Internal Server Error";

constexpr const char* c_http_date_prefix = "Date: ";
constexpr const char* c_http_server_prefix = "Server: ";
constexpr const char* c_http_location_prefix = "Location: ";
constexpr const char* c_http_modified_prefix = "Last-Modified: ";
constexpr const char* c_http_content_type_prefix = "Content-Type: ";
constexpr const char* c_http_content_length_prefix = "Content-Length: ";

constexpr const char* c_http_host_header = "Host";

constexpr const char* c_http_content_type_header = "Content-Type";
constexpr const char* c_http_content_length_header = "Content-Length";
constexpr const char* c_http_content_disposition_header = "Content-Disposition";

constexpr const char* c_http_access_control_allow_origin_all = "Access-Control-Allow-Origin: *";

constexpr const char* c_http_connection_header_info = "Connection: keep-alive";

#ifdef LOCAL_REQUESTS_ONLY
constexpr const char* c_http_keep_alive_header_info = "Keep-Alive: timeout=1, max=100";
#else
constexpr const char* c_http_keep_alive_header_info = "Keep-Alive: timeout=10, max=100";
#endif

constexpr const char* c_http_content_type_font_ttf = "font/ttf";

constexpr const char* c_http_content_type_text_csv = "text/csv";
constexpr const char* c_http_content_type_text_plain = "text/plain";

constexpr const char* c_http_content_type_text_javascript = "text/javascript";

constexpr const char* c_http_content_type_text_css_utf8 = "text/css; charset=UTF-8";
constexpr const char* c_http_content_type_text_html_utf8 = "text/html; charset=UTF-8";
constexpr const char* c_http_content_type_text_plain_utf8 = "text/plain; charset=UTF-8";

constexpr const char* c_http_content_type_image_gif = "image/gif";
constexpr const char* c_http_content_type_image_jpg = "image/jpg";
constexpr const char* c_http_content_type_image_png = "image/png";

constexpr const char* c_http_content_type_application_form = "application/x-www-form-urlencoded";
constexpr const char* c_http_content_type_application_json = "application/json";

constexpr const char* c_http_content_type_multipart_form_data = "multipart/form-data";

constexpr const char* c_http_content_disposition_form_data = "form-data";

constexpr const char* c_html_test_response = "<!doctype html>\n<html>\n<head><title>Test HTML Page</title></head>\n<body><p>This is a HTML response for test purposes.</p></body>\n</html>\n";

constexpr const char* c_has_moved_html_response = "<!doctype html>\n<html>\n<head><title>Document Has Moved</title></head>\n<body><p>Document has moved <a href=\"DOCUMENT\">here</a>.</p></body>\n</html>\n";

constexpr const char* c_not_found_html_response = "<!doctype html>\n<html>\n<head><title>Document Not Found</title></head>\n<body><p>Unable to find document 'DOCUMENT'.</p></body>\n</html>\n";
constexpr const char* c_not_endpoint_html_response = "<!doctype html>\n<html>\n<head><title>Document Not Found</title></head>\n<body><p>Unable to find 'DOCUMENT' (incorrect endpoint?).</p></body>\n</html>\n";

constexpr const char* c_upload_filename = "upload";

constexpr const char* c_replace_document_marker = "DOCUMENT";

constexpr const char* c_query_param_name_verbose = "verbose";

constexpr const char* c_query_param_value_json = "json";
constexpr const char* c_query_param_value_text = "text";

constexpr const char* c_query_param_name_format = "format";

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

constexpr const char* c_cws_force_admin_update = "cws_force_admin_update";

mutex g_mutex;

string g_server_id;

string g_version_string;

string g_cws_endpoint_prefix;

atomic< size_t > g_active_handlers;

atomic< size_t > g_num_request_handler;

atomic< time_t > g_redirects_file_mod = 0;

map< string, string > g_redirects;

size_t g_max_post_data_allowed = 0;

inline void increment_handlers( )
{
   ++g_active_handlers;
}

inline void deccrement_handlers( )
{
   --g_active_handlers;
}

bool redirects_file_has_changed( const char* p_file_name )
{
   bool changed = false;

   time_t t = 0;

   if( p_file_name && file_exists( p_file_name ) )
      t = last_modification_time( p_file_name );

   if( t != g_redirects_file_mod )
      changed = true;

   g_redirects_file_mod = t;

   return changed;
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

         string http_verb, http_document, http_protocol, http_qry_info;

         string::size_type pos = http_request.find( ' ' );

         http_verb = http_request.substr( 0, pos );

         http_request_type request_type = e_http_request_type_unknown;

         if( http_verb == c_get_request )
            request_type = e_http_request_type_get;
         else if( http_verb == c_put_request )
            request_type = e_http_request_type_put;
         else if( http_verb == c_head_request )
            request_type = e_http_request_type_head;
         else if( http_verb == c_post_request )
            request_type = e_http_request_type_post;
         else if( http_verb == c_trace_request )
            request_type = e_http_request_type_trace;
         else if( http_verb == c_delete_request )
            request_type = e_http_request_type_delete;
         else if( http_verb == c_options_request )
            request_type = e_http_request_type_options;

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

         string data, error, header, access, device, response, session, moved_document;

         // NOTE: If document ends in '/' then will
         // automatically append "index.html".
         if( !http_document.empty( ) )
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

               if( params.count( c_query_param_name_access ) )
                  access = params[ c_query_param_name_access ];

               if( params.count( c_query_param_name_device ) )
                  device = params[ c_query_param_name_device ];

               if( params.count( c_query_param_name_session ) )
                  session = params[ c_query_param_name_session ];

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
                           boundary = info.substr( CONST_LENGTH( c_boundary_prefix ) );
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
                        string prefix( data.substr( 0, pos + CONST_LENGTH( c_data_separator ) ) );

                        data.erase( 0, pos + CONST_LENGTH( c_data_separator ) );

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
                        local_filename = g_temporary_directory + '/';

                        bool has_valid_session = false;

                        // NOTE: If the upload is part of a "web sesssion" then verify
                        // that the system variable for the provided access and device
                        // exists and that its value matches the provided "session" so
                        // a filename in the form "<session>.tmp" is created.
                        if( !access.empty( ) && !device.empty( ) && !session.empty( ) )
                        {
                           string var_prefix( get_special_var_name( e_special_var_web ) + '.' );

                           string check_session( get_system_variable( var_prefix
                            + access + '.' + device + '.' + string( c_query_param_name_session ) ) );

                           if( session == check_session )
                           {
                              has_valid_session = true;
                              local_filename += session;
                           }
                        }

                        // NOTE: If not a "web session" upload
                        // then just use the name "upload" (so
                        // that testing will not inadvertantly
                        // fill up the temporary file system).
                        if( !has_valid_session )
                           local_filename += c_upload_filename;

                        local_filename += c_tmp_file_ext;
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
            if( ( http_document == c_cws_endpoint )
             || ( http_document.find( g_cws_endpoint_prefix ) == 0 ) )
            {
               was_endpoint = true;

               string prefix( c_web_access_prefix );

               cws_paramaters cws_params;

               cws_params.is_json_output = is_json_output;

               if( params.count( c_query_param_name_access ) )
                  cws_params.access = params[ c_query_param_name_access ];

               if( params.count( c_query_param_name_device ) )
                  cws_params.device = params[ c_query_param_name_device ];

               if( params.count( c_query_param_name_passwd ) )
                  cws_params.passwd = params[ c_query_param_name_passwd ];

               if( params.count( c_query_param_name_options ) )
                  cws_params.options = params[ c_query_param_name_options ];

               if( params.count( c_query_param_name_payload ) )
                  cws_params.payload = params[ c_query_param_name_payload ];

               if( params.count( c_query_param_name_request ) )
                  cws_params.request = params[ c_query_param_name_request ];

               if( params.count( c_query_param_name_session ) )
                  cws_params.session = params[ c_query_param_name_session ];

               string endpoint_suffix;

               if( http_document.find( g_cws_endpoint_prefix ) == 0 )
                  endpoint_suffix = http_document.substr( g_cws_endpoint_prefix.length( ) );

               if( !cws_params.passwd.empty( ) )
               {
                  if( base64::valid_characters( cws_params.passwd, true ) )
                  {
                     bool bad_credentials = false;

                     cws_params.passwd = base64::decode( cws_params.passwd, true );

                     string::size_type pos = cws_params.passwd.find( ':' );

                     // NOTE: Only report a username being "taken" if
                     // has a user info record with no password hash.
                     if( pos != string::npos )
                     {
                        string name( cws_params.passwd.substr( 0, pos ) );

                        if( name == c_admin )
                        {
                           if( cws_params.access != g_cws_admin_token )
                              bad_credentials = true;
                        }
                        else
                        {
                           if( !has_empty_user_hash( cws_params.access ) )
                              bad_credentials = true;
                           else if( has_user_name( name ) )
                              // FUTURE: This message should be handled as a server string message.
                              error = "Username '" + name + "' has already been taken.";
                           else
                           {
                              regex_chain expr( "^[a-z][-a-z0-9]{2,29}$&&^.*[^-]$&!^.*--.*$" );

                              if( expr.search( name ) != 0 )
                                 // FUTURE: This message should be handled as a server string message.
                                 error = "Username must start with 'a-z' followed by 'a-z' or '0-9' characters (optionally use '-' for separators).";
                           }
                        }
                     }
                     else if( !has_user_info( cws_params.access ) )
                        bad_credentials = true;

                     if( bad_credentials )
                        // FUTURE: This message should be handled as a server string message.
                        error = "User credentials are either invalid or incorrect.";
                  }
                  else
                  {
                     string name_and_value( c_query_param_name_passwd );

                     name_and_value += '=' + cws_params.passwd;

                     // FUTURE: This message should be handled as a server string message.
                     error = "Invalid base64 URL format in query parameter '" + name_and_value + "'.";
                  }
               }

               bool use_none_response = false;

               found = process_cws_request( request_type,
                endpoint_suffix, cws_params, use_none_response, response, error );
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

         int64_t head_size = 0;

         if( response.empty( ) )
         {
            string path( c_html );

            // NOTE: It is expected that "html"
            // is most likely to be a soft-link
            // so use "file_target" in order to
            // determine the actual start path.
            string start( file_target( path ) );

            if( !http_document.empty( ) && ( http_document[ 0 ] != '/' ) )
               path += '/';

            bool rc = true;

            // NOTE: Do a sanity check to make sure that the document
            // being requested is actually below the start directory.
            // Although "rc" is not being checked after the call it's
            // needed to prevent an exception from being thrown (when
            // the path does not exist).
            string check_path( absolute_path( path + http_document, &rc ) );

            if( check_path.find( start ) == 0 )
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
                      << '\n' << c_http_modified_prefix << formatted_document_dtm << '\n';

                     if( request_type != e_http_request_type_head )
                        osstr << c_http_connection_header_info << '\n' << c_http_keep_alive_header_info << '\n';

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

                     // NOTE: No need to buffer file content if the
                     // request type is "HEAD" (but sets "response"
                     // to "dummy" so checking if empty will work).
                     if( request_type == e_http_request_type_head )
                     {
                        response = c_dummy;

                        head_size = file_size( path + http_document );
                     }
                     else
                        response = buffer_file( path + http_document );
                  }
               }
               else
               {
                  guard g( g_mutex );

                  string redirects_file( path + '/' + string( c_redirects_file ) );

                  // NOTE: The ".redirects" file is an optional mapping
                  // file where each line is in the following format:
                  //
                  // /src /dest
                  //
                  // Some examples are as follows:
                  //
                  // /old_dir /new_dir
                  // /old_dir /new_page.html
                  // /old_page.html /new_page.html
                  //
                  if( redirects_file_has_changed( redirects_file.c_str( ) ) )
                  {
                     g_redirects.clear( );

                     if( file_exists( redirects_file ) )
                     {
                        string redirect_info( buffer_file( redirects_file ) );

                        vector< string > redirect_lines;

                        if( !redirect_info.empty( ) )
                        {
                           split( redirect_info, redirect_lines, '\n' );

                           for( size_t i = 0; i < redirect_lines.size( ); i++ )
                           {
                              string next_line( redirect_lines[ i ] );

                              string::size_type pos = next_line.find( ' ' );

                              if( pos != string::npos )
                              {
                                 string src( next_line.substr( 0, pos ) );
                                 string dest( next_line.substr( pos + 1 ) );

                                 if( !dest.empty( ) )
                                    g_redirects.insert( make_pair( src, dest ) );
                              }
                           }
                        }
                     }
                  }

                  string possibly_expected( http_document + '/' + string( c_index_html ) );

                  if( file_exists( path + possibly_expected ) )
                     moved_document = possibly_expected;
                  else
                  {
                     string check_document( http_document );

                     size_t len = check_document.length( );

                     // NOTE: Allow for a trailing underbar to be used to test
                     // a redirect while the original file is still available.
                     if( ( len > 1 ) && ( check_document[ len - 1 ] == '_' ) )
                     {
                        check_document.erase( len - 1 );

                        possibly_expected = check_document + '/' + string( c_index_html );
                     }

                     if( g_redirects.count( check_document ) )
                        moved_document = g_redirects[ check_document ];
                     else if( g_redirects.count( possibly_expected ) )
                        moved_document = g_redirects[ possibly_expected ];
                  }
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
            {
               osstr << c_http_content_length_prefix;

               if( head_size )
                  osstr << head_size;
               else
                  osstr << to_string( response.length( ) );

               osstr << "\n\n";

               // NOTE: If the handling a "HEAD" request then
               // output the headers but no response content.
               if( request_type != e_http_request_type_head )
                  osstr << response;
            }
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
               bool has_file_extension = ( http_document.find( '.' ) != string::npos );

               response = ( has_file_extension ? c_not_found_html_response : c_not_endpoint_html_response );

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
}

http_listener::~http_listener( )
{
   --g_active_listeners;
}

void http_listener::on_start( )
{
   bool started = false;

   string listener_web_rest( "web_rest" );

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

      listener_registration registration( port, listener_web_rest );

      TRACE_LOG( TRACE_MINIMAL, listener_web_rest + " listener started on tcp port " + to_string( port ) );

      started = true;

      g_none_tag = '<' + string( c_none ) + '>';
      g_none_var = get_special_var_name( e_special_var_none );

      g_server_id = string( c_CIYAM );

      g_version_string = get_system_variable( e_special_var_version );

      if( !g_version_string.empty( ) )
         g_server_id += " (" + g_version_string + ")";

      g_is_devt_system = has_system_variable( e_special_var_system_is_for_devt );

      g_max_post_data_allowed = get_max_http_post_allowed( );

      string admin_web_access_file( c_web_access_prefix );

      admin_web_access_file += c_admin;

      g_cws_admin_token = opt_buffer_file( admin_web_access_file );

      g_cws_admin_device = opt_buffer_file( c_web_device_name_admin );

      g_web_session_check = get_special_var_name( e_special_var_web_session_check );

      g_cws_endpoint_prefix = string( c_cws_endpoint ) + '/';

      g_cws_username_for_prefix = get_special_var_name( e_special_var_cws_username_for ) + '_';

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
         {
            if( file_exists( c_cws_force_admin_update ) )
            {
               g_cws_admin_token = opt_buffer_file( admin_web_access_file );
               g_cws_admin_device = opt_buffer_file( c_web_device_name_admin );

               file_remove( c_cws_force_admin_update );
            }

            init_http_handler( up_socket.release( ), address.get_addr_string( ) );
         }
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

   if( started )
      TRACE_LOG( TRACE_MINIMAL, listener_web_rest + " listener finished (tcp port " + to_string( port ) + ")" );

   delete this;
}

void init_http_handler( int port )
{
   http_listener* p_http_listener = new http_listener( port );

   p_http_listener->start( );
}
