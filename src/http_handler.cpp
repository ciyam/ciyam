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
#include "format.h"
#include "sha256.h"
#include "threads.h"
#include "date_time.h"
#include "utilities.h"
#include "ciyam_base.h"
#include "ssl_socket.h"
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

const int c_max_line_length = 2000;

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

const char* const c_web_lock_suffix = ".lock";
const char* const c_web_command_suffix = ".command";
const char* const c_web_message_suffix = ".message";
const char* const c_web_session_suffix = ".session";
const char* const c_web_started_suffix = ".started";

const char* const c_req_param_host = "Host:";

const char* const c_req_param_if_modified = "If-Modified-Since:";

const char* const c_http_1_1 = "HTTP/1.1";

const char* const c_http_200_OK = "200 OK";

const char* const c_http_301_Moved = "301 Moved Permanently";
const char* const c_http_304_Not_Mod = "304 Not Modified";
const char* const c_http_400_Bad_Req = "400 Bad Request";
const char* const c_http_404_Not_Found = "404 Not Found";
const char* const c_http_414_URI_Too_Long = "414 URI Too Long";

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
const char* const c_query_param_name_session = "session";
const char* const c_query_param_name_verbose = "verbose";
const char* const c_query_param_name_function = "function";
const char* const c_query_param_name_password = "password";

const char* const c_query_param_value_json = "json";
const char* const c_query_param_value_text = "text";

const char* const c_cws_function_terminate = "terminate";

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

const int c_error_response_delay = 250;

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

mutex g_mutex;

string g_server_id;

string g_version_string;

set< string > g_cws_access_tokens;

map< string, set< string > > g_cws_access_devices;

inline size_t get_num_access_devices( const string& token )
{
   return ( !g_cws_access_devices.count( token ) ? 0 : g_cws_access_devices[ token ].size( ) );
}

atomic< size_t > g_active_handlers;

atomic< size_t > g_request_handler;

size_t g_max_post_data_allowed = 0;

inline void increment_handlers( )
{
   ++g_active_handlers;
}

inline void deccrement_handlers( )
{
   --g_active_handlers;
}

bool has_web_session_access_token( const string& token,
 const string& token_file, const string& password, string& pin )
{
   guard g( g_mutex );

   bool retval = false;

   if( token == c_admin )
   {
      if( !file_exists( token_file ) )
      {
         pin = random_characters( 5, 0, e_printable_type_numeric );

         ofstream outf( token_file.c_str( ), ios::out );

         outf << pin;

         outf.close( );

         if( outf.good( ) )
         {
            retval = true;

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
         ofstream outf( token_file.c_str( ), ios::out );

         outf << sha256( password ).get_digest_as_string( );

         outf.close( );

         if( outf.good( ) )
            retval = true;
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

   size_t handler = ++g_request_handler;

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
         string request, next_line;

         request.reserve( c_max_line_length );
         next_line.reserve( c_max_line_length );

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

            if( request.empty( ) )
            {
               request = next_line;

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

         if( !had_empty || request.empty( ) )
         {
#ifdef DEBUG
            cerr << "(empty or incomplete request)" << endl;
#endif
            break;
         }

         had_request = true;

         TRACE_LOG( TRACE_DETAILS | TRACE_SESSION,
          to_comparable_string( handler, false, 8 ) + " - " + request );

#ifdef DEBUG
         cerr << "handler #" << handler << "\n[Request]\n" << request << endl;
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

         string document, protocol, qry_info;

         string::size_type pos = request.find( ' ' );

         if( pos != string::npos )
         {
            document = request.substr( pos + 1 );

            request.erase( pos );

            pos = document.find( ' ' );

            if( pos != string::npos )
            {
               protocol = document.substr( pos + 1 );

               document.erase( pos );
            }

            pos = document.find( '?' );

            if( pos != string::npos )
            {
               qry_info = document.substr( pos + 1 );

               document.erase( pos );
            }
         }

#ifdef DEBUG
         cerr << "\nrequest ==> " << request << endl;
         cerr << "document ==> " << document << endl;
         cerr << "protocol ==> " << protocol << endl;

         if( !qry_info.empty( ) )
            cerr << "qry_info ==> " << qry_info << endl;
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
         if( document.find( c_api_prefix ) != 0 )
         {
            string::size_type pos = document.rfind( '/' );

            if( ( pos != string::npos )
             && ( pos == ( document.length( ) - 1 ) ) )
               document += c_index_html;
         }

         map< string, string > params;

         if( !qry_info.empty( ) )
         {
            if( !parse_query_params( qry_info, params ) )
               error = "Invalid format for query parameters '" + qry_info + "'.";
            else
            {
               if( params.count( c_query_param_name_format ) )
               {
                  if( params[ c_query_param_name_format ] == c_query_param_value_json )
                     is_json_output = true;
                  else if( params[ c_query_param_name_format ] == c_query_param_value_text )
                     is_text_output = true;
                  else
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

                  error = "Unexpected error occurred reading post data.";
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

                           error = "Unexpected error occurred reading post data.";

                           break;
                        }
                     }
                  }
               }
            }
            else
               error = "Post data was too large (maximum allowed is " + format_bytes( g_max_post_data_allowed ) + ").";
         }

         bool unchanged = false;

         bool has_content_type = false;

         date_time now( date_time::standard( ) );

         string formatted_dtm( format_date_time( now ) );

         if( !data.empty( ) )
         {
            if( document == c_echo_endpoint )
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
            else if( document == c_upload_endpoint )
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
            if( document == c_cws_endpoint )
            {
               was_endpoint = true;

               string prefix( c_web_session_prefix );

               string access, device, session, password;

               if( params.count( c_query_param_name_access ) )
                  access = params[ c_query_param_name_access ];

               if( params.count( c_query_param_name_device ) )
                  device = params[ c_query_param_name_device ];

               if( params.count( c_query_param_name_session ) )
                  session = params[ c_query_param_name_session ];

               if( params.count( c_query_param_name_password ) )
                  password = params[ c_query_param_name_password ];

               string access_file( '.' + prefix + access ); // i.e. ".web_session.<access>"

               string pin;

               bool has_access_file = has_web_session_access_token( access, access_file, password, pin );

               if( has_access_file && pin.empty( ) && !g_cws_access_tokens.count( access ) )
                  g_cws_access_tokens.insert( access );

               if( access.empty( ) )
                  error = "Need a valid access token to use a web session.";
               else if( !pin.empty( ) )
               {
                  found = true;

                  if( !is_json_output )
                     response = pin;
                  else
                     response = "{\"pin\":\"" + pin + "\"}";
               }
               else if( !g_cws_access_tokens.count( access ) )
                  error = "Provided web session access token '" + access + "' is invalid.";
               else if( device.empty( ) )
               {
                  size_t num_devices = get_num_access_devices( access );

                  if( num_devices >= c_cws_max_devices )
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
                     error = "Invalid device identity '" + device + "'.";
                  else if( !g_cws_access_devices[ access ].count( device ) )
                  {
                     size_t num_devices = get_num_access_devices( access );

                     if( num_devices < c_cws_max_devices )
                        g_cws_access_devices[ access ].insert( device );
                     else
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
                           error = "This web session is not valid (or has expired).";
                        else
                        {
                           string function;

                           if( params.count( c_query_param_name_function ) )
                              function = params[ c_query_param_name_function ];

                           if( has_system_variable( web_lock_name ) )
                           {
                              string when_locked( get_system_variable( web_lock_name ) );

                              set_system_variable( web_lock_name, "" );
                              set_system_variable( web_message_name, "" );

                              set_system_variable( web_started_name, when_locked );
                           }

                           if( function == c_cws_function_terminate )
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
                           else
                           {
                              string script_name( prefix + "cin" );

                              bool running = has_system_variable( web_message_name );

                              if( !running )
                              {
                                 string cmd( "./ciyam_client -quiet -no_prompt -no_stderr -exec=\"<"
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

                                 error = "Was unable to start a web session with access token '" + access + "'.";
                              }
                              else
                              {
                                 found = true;

                                 if( !function.empty( ) )
                                    set_system_variable( web_command_name, function );
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
                                    response = "[no response provided]";
                              }
                           }
                        }
                     }
                  }
               }
            }
            else if( document == c_echo_endpoint )
            {
               found = true;

               was_endpoint = true;

               string echo( qry_info.empty( ) ? "(empty parameters)" : qry_info );

               if( !is_json_output )
                  response = echo;
               else
                  response = "{\"parameters\":\"" + escaped_json( echo ) + "\"}";
            }
            else if( document == c_ip_addr_endpoint )
            {
               found = true;

               was_endpoint = true;

               if( !is_json_output )
                  response = ip_addr;
               else
                  response = "{\"ip_addr\":\"" + escaped_json( ip_addr ) + "\"}";
            }
            else if( document == c_storage_endpoint )
            {
               found = true;

               was_endpoint = true;

               string storage( get_system_variable( e_special_var_storage ) );

               if( !is_json_output )
                  response = storage;
               else
                  response = "{\"storage\":\"" + storage + "\"}";
            }
            else if( document == c_version_endpoint )
            {
               found = true;

               was_endpoint = true;

               string version( get_system_variable( e_special_var_version ) );

               if( !is_json_output )
                  response = version;
               else
                  response = "{\"version\":\"" + version + "\"}";
            }
            else if( document == c_unix_now_endpoint )
            {
               found = true;

               was_endpoint = true;

               string unix_now( to_string( unix_time( ) ) );

               if( !is_json_output )
                  response = unix_now;
               else
                  response = "{\"unix_now\":\"" + escaped_json( unix_now ) + "\"}";
            }
            else if( document == c_post_limit_endpoint )
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

            if( !document.empty( ) && ( document[ 0 ] != '/' ) )
               path += '/';

            bool rc = true;

            // NOTE: Do a sanity check to make sure that the document
            // being requested is not outside of the "web root" path.
            string check_path( absolute_path( path + document, &rc ) );

            if( rc && ( check_path.find( path ) == 0 ) )
            {
               if( file_exists( path + document ) )
               {
                  found = true;

                  string extension;

#ifdef DEBUG
                  cerr << "\n(found '" << document << "')" << endl;
#endif
                  string::size_type ext_pos = document.rfind( '.' );

                  if( ext_pos != string::npos )
                     extension = document.substr( ext_pos + 1 );

                  date_time doc_modified( last_modification_time( path + document ) );

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

                     response = buffer_file( path + document );
                  }
               }
               else if( dir_exists( path + document ) )
               {
                  if( file_exists( path + document + "/index.html" ) )
                     moved_document = document + '/';
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
               replace( response, c_replace_document_marker, document );
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
#ifdef DEBUG
      cerr << "\nhttp_request error: " << x.what( ) << endl;
#endif
      TRACE_LOG( TRACE_INITIAL | TRACE_SESSION,
       string( "http_request error: " ) + x.what( ) );
   }
   catch( ... )
   {
#ifdef DEBUG
      cerr << "\nunexpected http_request exception" << endl;
#endif
      TRACE_LOG( TRACE_INITIAL | TRACE_SESSION,
       string( "http_request error: unexpected exception" ) );
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

      g_server_id = string( c_CIYAM );

      g_version_string = get_system_variable( e_special_var_version );

      if( !g_version_string.empty( ) )
         g_server_id += " (" + g_version_string + ")";

      g_max_post_data_allowed = get_max_http_post_allowed( );

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
