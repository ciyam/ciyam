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
#include "sha256.h"
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

#include "ciyam_constants.h"

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

const char* const c_http_304_Not_Mod = "304 Not Modified";
const char* const c_http_400_Bad_Req = "400 Bad Request";
const char* const c_http_404_Not_Found = "404 Not Found";

const char* const c_http_date_prefix = "Date: ";
const char* const c_http_server_prefix = "Server: ";
const char* const c_http_modified_prefix = "Last-Modified: ";
const char* const c_http_content_type_prefix = "Content-Type: ";
const char* const c_http_content_length_prefix = "Content-Length: ";

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

const char* const c_not_found_html_response = "<!doctype html>\n<html>\n<head><title>Document Not Found</title></head>\n<body><p>Unable to find 'DOCUMENT' (incorrect endpoint?).</p></body>\n</html>\n";

const char* const c_replace_document_marker = "DOCUMENT";

const char* const c_query_param_name_token = "token";
const char* const c_query_param_name_device = "device";
const char* const c_query_param_name_format = "format";
const char* const c_query_param_name_session = "session";
const char* const c_query_param_name_verbose = "verbose";
const char* const c_query_param_name_function = "function";

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

const size_t c_max_data_for_post = 5000000;

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

         params[ name ] = value;
      }
   }

   return retval;
}

const size_t c_cws_max_devices = 10;

set< string > g_cws_tokens;

map< string, set< string > > g_cws_devices;

atomic< size_t > g_active_handlers;

inline void increment_handlers( )
{
   ++g_active_handlers;
}

inline void deccrement_handlers( )
{
   --g_active_handlers;
}

}

#ifdef SSL_SUPPORT
void handle_http_request( ssl_socket* p_socket, const string& ip_addr )
#else
void handle_http_request( tcp_socket* p_socket, const string& ip_addr )
#endif
{
   if( p_socket )
   {
      bool first = true;
      bool checked = false;
      bool using_tls = false;
      bool had_request = false;

      increment_handlers( );

#ifdef DEBUG
      cerr << "\n*** started request/response ***" << endl;
#endif

      try
      {
         p_socket->set_no_delay( );
         p_socket->set_no_linger( );

         date_time dtm_1( date_time::standard( ) );

#ifdef SSL_SUPPORT
         if( p_socket->is_tls_handshake( ) )
         {
            p_socket->ssl_accept( c_ssl_accept_timeout );

            using_tls = true;

#  ifdef DEBUG
            cerr << "(has accepted a TLS request)" << endl;
#  endif

         }
#endif
         int initial_timeout = c_initial_timeout;
         int subsequent_timeout = c_subsequent_timeout;

         while( true )
         {
            // NOTE: Multiple requests can be handled using one socket
            // but whether requests are handled this way is decided by
            // the software making the requests so these timing checks
            // and timeout adjustments are being made here in order to
            // try and maximise the request throughput (which might be
            // otherwise slowed down when requests are made locally).
            if( !first && !checked )
            {
               checked = true;

               date_time dtm_2( date_time::standard( ) );

               int64_t secs_1 = unix_time( dtm_1 );
               int64_t secs_2 = unix_time( dtm_2 );

               millisecond ms_1 = dtm_1.get_millisecond( );
               millisecond ms_2 = dtm_2.get_millisecond( );

               int64_t total_ms_1 = ( secs_1 * 1000 ) + ms_1;
               int64_t total_ms_2 = ( secs_2 * 1000 ) + ms_2;

               int64_t difference = 0;

               if( total_ms_1 > total_ms_2 )
                  difference = ( total_ms_1 - total_ms_2 );
               else
                  difference = ( total_ms_2 - total_ms_1 );

               if( difference <= c_minimum_timeout )
                  subsequent_timeout = c_minimum_timeout;
            }
#ifdef DEBUG
            cerr << "\n" << date_time::local( ).as_string( e_time_format_hhmmssth, true ) << '\n' << endl;
#endif
            string request;

            vector< string > header_lines;

            map< string, string > header_info;

            bool had_empty = false;

            int retries = c_num_retries;

            while( true )
            {
               string next_line;

               if( p_socket->read_line( next_line,
                ( first ? initial_timeout : subsequent_timeout ) ) <= 0 )
               {
                  if( p_socket->had_blank_line( ) )
                  {
                     had_empty = true;

                     break;
                  }

                  if( !( *p_socket ) || had_request || !retries-- )
                     break;
                  else
                     continue;
               }

               first = false;

               if( request.empty( ) )
                  request = next_line;
               else
                  header_lines.push_back( next_line );
            }

            if( !had_empty || request.empty( ) )
            {
#ifdef DEBUG
               cerr << "(empty or incomplete request)" << endl;
#endif
               break;
            }

            had_request = true;

#ifdef DEBUG
            cerr << "[Request]\n" << request << endl;
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

            ostringstream osstr;

            string data, error, header, response;

            // NOTE: If no file extension was provided
            // then assumes it is a directory name and
            // will append "/index.html" (or will just
            // append "index.html" if "/" was found to
            // be the last character).
            if( document.find( c_api_prefix ) != 0 )
            {
               string::size_type pos = document.rfind( '/' );

               if( pos != string::npos )
               {
                  string::size_type dpos = document.find( '.', pos + 1 );

                  if( ( dpos == string::npos ) && ( pos != document.length( ) - 1 ) )
                     document += '/';

                  if( dpos == string::npos )
                     document += c_index_html;
               }
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
                     else if( params[ c_query_param_name_format ] != c_query_param_value_text )
                        error = "Invalid query parameter value in '" + qry_info + "' for version.";
                  }

                  if( params.count( c_query_param_name_verbose ) )
                     is_verbose = true;
               }
            }

            if( error.empty( )
             && header_info.count( c_http_content_length_header ) )
            {
               size_t data_length = from_string< size_t >( header_info[ c_http_content_length_header ] );

               if( data_length <= c_max_data_for_post )
               {
                  data = string( data_length, '\0' );

                  int received = p_socket->recv_n( ( unsigned char* )data.data( ), data_length, c_subsequent_timeout );

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
                           int next_chunk = p_socket->recv_n(
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
                  error = "Post data is too large (maximum size is " + to_string( c_max_data_for_post ) + " bytes).";
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
                   << c_http_server_prefix << c_CIYAM << '\n' << c_http_date_prefix << formatted_dtm << '\n';

                  osstr << c_http_content_type_prefix << header_info[ c_http_content_type_header ] << '\n';

                  if( !is_json_output )
                     response = data;
                  else
                     response = "{\"data\":\"" + escaped_json( data ) + "\"}";

                  response += '\n';
               }
               else if( document == c_upload_endpoint )
               {
                  found = true;

                  was_endpoint = true;

                  string boundary, filename, local_filename;

                  has_content_type = true;

                  osstr << c_http_1_1 << ' ' << c_http_200_OK << '\n'
                   << c_http_server_prefix << c_CIYAM << '\n' << c_http_date_prefix << formatted_dtm << '\n';

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
                     response = "Uploaded as \"" + local_filename + "\" (" + to_string( uploaded_size ) + " bytes).";

                  response += '\n';
               }
            }
            else if( error.empty( ) )
            {
               if( document == c_cws_endpoint )
               {
                  was_endpoint = true;

                  string prefix( c_web_session_prefix );

                  string token, device, session;

                  if( params.count( c_query_param_name_token ) )
                     token = params[ c_query_param_name_token ];

                  if( params.count( c_query_param_name_device ) )
                     device = params[ c_query_param_name_device ];

                  if( params.count( c_query_param_name_session ) )
                     session = params[ c_query_param_name_session ];

                  string token_file( '.' + prefix + token ); // i.e. ".web_session.<token>"

                  if( !g_cws_tokens.count( token ) && file_exists( token_file ) )
                     g_cws_tokens.insert( token );

                  if( token.empty( ) )
                     error = "Need a valid token to use a web session.";
                  else if( !g_cws_tokens.count( token ) )
                     error = "Provided web session token '" + token + "' is invalid.";
                  else if( device.empty( ) )
                  {
                     size_t num_devices = ( !g_cws_devices.count( token ) ? 0 : g_cws_devices[ token ].size( ) );

                     if( num_devices >= c_cws_max_devices )
                        error = "Maximum devices have been created for web session token '" + token + "'.";
                     else
                     {
                        found = true;

                        string new_device( uuid( ).as_string( ).substr( 0, 10 ) );

                        if( !is_json_output )
                           response = new_device;
                        else
                           response = "{\"new_device\":\"" + new_device + "\"}";

                        g_cws_devices[ token ].insert( new_device );
                     }
                  }
                  else if( !g_cws_devices[ token ].count( device ) )
                     error = "Provided web session device '" + device + "' is invalid.";
                  else
                  {
                     string var_prefix( get_special_var_name( e_special_var_web ) + '.' );

                     string output_file_name( "/tmp/ciyam." + var_prefix.substr( 1 ) + token + '.' + device );

                     string web_lock_name( var_prefix + token + '.' + device + c_web_lock_suffix );

                     string web_command_name( var_prefix + token + '.' + device + c_web_command_suffix );
                     string web_message_name( var_prefix + token + '.' + device + c_web_message_suffix );
                     string web_session_name( var_prefix + token + '.' + device + c_web_session_suffix );
                     string web_started_name( var_prefix + token + '.' + device + c_web_started_suffix );

                     if( session.empty( ) )
                     {
                        string token_value( buffer_file( token_file ) );

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
                        // (being the "token_value" and "device" strings) and
                        // then combining its digest with the "unqiue" string
                        // to determine a "combined" hash.
                        if( error.empty( ) )
                        {
                           found = true;

                           sha256 hash_checked( token_value + device );

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
                                  + script_name + ' ' + token + ' ' + device + ' ' + session + "\" > /dev/null &" );

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

                                 error = "Was unable to start a web session with token '" + token + "'.";
                              }
                              else
                              {
                                 found = true;

                                 set_system_variable( web_command_name, "variable " + web_message_name );

                                 msleep( 150 );

                                 if( file_exists( output_file_name ) )
                                 {
                                    response = buffer_file( output_file_name );

                                    response = trim( response, false, true, "\n" );

                                    file_remove( output_file_name );
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
            }

            if( response.empty( ) )
            {
               string path( get_web_root( ) + "/" );

               bool rc = true;

               // NOTE: Do a sanity check to make sure that the document
               // being requested is not outside of the "web root" path.
               string check_path( absolute_path( path + document, &rc ) );

               if( rc && ( check_path.find( path ) == 0 ) )
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
                      << '\n' << c_http_server_prefix << c_CIYAM
                      << '\n' << c_http_date_prefix << formatted_dtm
                      << '\n' << c_http_modified_prefix << formatted_document_dtm
                      << '\n' << c_http_connection_header_info << '\n' << c_http_keep_alive_header_info << '\n';

                     osstr << c_http_content_type_prefix;

                     if( extension == c_ext_css )
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
            }

            if( found )
            {
               bool empty_but_not_unchanged = false;

               if( !unchanged && response.empty( ) )
                  empty_but_not_unchanged = true;

               if( !has_content_type )
               {
                  osstr << c_http_1_1 << ' ' << c_http_200_OK << '\n'
                   << c_http_server_prefix << c_CIYAM << '\n' << c_http_date_prefix << formatted_dtm << '\n';

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

                  if( !response.empty( ) )
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

               if( is_json_output )
                  error = "{\"error\":\"" + escaped_json( error ) + "\"}";

               error += '\n';

               osstr << c_http_1_1 << ' ' << c_http_400_Bad_Req << '\n'
                << c_http_content_type_prefix << c_http_content_type_text_plain_utf8
                << '\n' << c_http_content_length_prefix << to_string( error.length( ) ) << "\n\n" << error;
            }
            else
            {
               response = c_not_found_html_response;

               replace( response, c_replace_document_marker, document );

               osstr << c_http_1_1 << ' ' << c_http_404_Not_Found << '\n'
                << c_http_server_prefix << c_CIYAM << '\n' << c_http_date_prefix << formatted_dtm << '\n';

               osstr << c_http_content_type_prefix << c_http_content_type_text_html_utf8 << '\n';

               osstr << c_http_content_length_prefix << to_string( response.length( ) ) << "\n\n" << response;
            }

            response = osstr.str( );

            p_socket->send_n( ( unsigned char* )response.data( ), response.length( ), c_response_timeout );

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

#ifdef DEBUG
      cerr << "\n*** finished request/response ***" << endl;
#endif
      p_socket->close( );

      delete p_socket;

      deccrement_handlers( );
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
            handle_http_request( up_socket.release( ), address.get_addr_string( ) );
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
