// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <memory>
#  include <string>
#  include <cstring>
#  include <csignal>
#  include <sstream>
#  include <stdexcept>
#endif

#include "http_handler.h"

#include "date_time.h"
#include "utilities.h"
#include "ciyam_base.h"
#include "ciyam_session.h"

//#define DEBUG

//#define LOCAL_REQUESTS_ONLY

using namespace std;

extern size_t g_active_listeners;

extern volatile sig_atomic_t g_server_shutdown;

namespace
{

const char* const c_ext_css = "css";

const char* const c_ext_gif = "gif";
const char* const c_ext_jpg = "jpg";
const char* const c_ext_png = "png";

const char* const c_ext_ttf = "ttf";

const char* const c_get_request = "GET ";

const char* const c_index_html = "index.html";

const char* const c_host_request_param = "Host: ";

const char* const c_http_1_1 = "HTTP/1.1";

const char* const c_http_200_OK = "200 OK";

const char* const c_http_404_Not_Found = "404 Not Found";

const char* const c_http_date_prefix = "Date: ";
const char* const c_http_modified_prefix = "Last-Modified: ";

const char* const c_http_connection_header = "Connection: keep-alive";

#ifdef LOCAL_REQUESTS_ONLY
const char* const c_http_keep_alive_header = "Keep-Alive: timeout=1, max=100";
#else
const char* const c_http_keep_alive_header = "Keep-Alive: timeout=10, max=100";
#endif

const char* const c_http_content_type_font_ttf = "Content-Type: font/ttf";

const char* const c_http_content_type_text_css = "Content-Type: text/css; charset=UTF-8";
const char* const c_http_content_type_text_html = "Content-Type: text/html; charset=UTF-8";

const char* const c_http_content_type_image_gif = "Content-Type: image/gif";
const char* const c_http_content_type_image_jpg = "Content-Type: image/jpg";
const char* const c_http_content_type_image_png = "Content-Type: image/png";

const char* const c_http_content_length_header_prefix = "Content-Length: ";

const char* const c_test_html_response = "<html>\n<head><title>Test HTML Page</title></head>\n<body><p>This is a HTML response for test purposes.</p></body>\n</html>\n";

const char* const c_not_found_html_response = "<html>\n<head><title>Document Not Found</title></head>\n<body><p>Unable to find 'DOCUMENT' (incorrect URL?).</p></body>\n</html>\n";

const char* const c_replace_document_marker = "DOCUMENT";

const int c_num_retries = 5;

#ifdef LOCAL_REQUESTS_ONLY
const int c_accept_timeout = 500;

const int c_initial_timeout = 100;
const int c_subsequent_timeout = 20;

const int c_response_timeout = 200;
#else
const int c_accept_timeout = 1000;

const int c_initial_timeout = 2000;
const int c_subsequent_timeout = 100;

const int c_response_timeout = 1000;
#endif

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

}

#ifdef SSL_SUPPORT
void handle_http_request( ssl_socket* p_socket, const string& address )
#else
void handle_http_request( tcp_socket* p_socket, const string& address )
#endif
{
   if( p_socket )
   {
      bool first = true;

#ifdef DEBUG
      cerr << "\n*** start request/response ***" << endl;
#endif

      p_socket->set_no_delay( );
      p_socket->set_no_linger( );

      while( true )
      {
#ifdef DEBUG
         cerr << "\n" << date_time::local( ).as_string( e_time_format_hhmmssth, true ) << '\n' << endl;
#endif
         string request;

         vector< string > headers;

         bool had_empty = false;

         int retries = c_num_retries;

         while( true )
         {
            string next_line;

            if( p_socket->read_line( next_line,
             ( first ? c_initial_timeout : c_subsequent_timeout ) ) <= 0 )
            {
               if( !( *p_socket ) || !retries-- )
                  break;
            }

            first = false;

            if( next_line.empty( ) )
            {
               had_empty = true;

               break;
            }

            if( request.empty( ) )
               request = next_line;
            else
               headers.push_back( next_line );
         }

         if( !had_empty || request.empty( ) )
         {
#ifdef DEBUG
            cerr << "(empty or incomplete request)\n" << endl;
#endif
            break;
         }

#ifdef DEBUG
         cerr << "[Request]\n" << request << endl;
#endif
         for( size_t i = 0; i < headers.size( ); i++ )
         {
#ifdef DEBUG
            if( i == 0 )
               cerr << "[Headers]" << endl;
            cerr << headers[ i ] << endl;
#endif
         }

         string document, protocol;

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
         }

#ifdef DEBUG
         cerr << "\nrequest ==> " << request << endl;
         cerr << "document ==> " << document << endl;
         cerr << "protocol ==> " << protocol << endl;
#endif

         bool found = false;

         ostringstream osstr;

         string header, response;

         if( !document.empty( ) && ( document[ 0 ] == '/' ) )
            document.erase( 0, 1 );

         if( document.empty( ) )
            document = c_index_html;

         string path( get_web_root( ) + "/" );

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

            date_time now( date_time::standard( ) );

            string formatted_dtm( format_date_time( now ) );

            date_time doc_modified( last_modification_time( path + document ) );

            string formatted_document_dtm( format_date_time( doc_modified ) );

            osstr << c_http_1_1 << ' ' << c_http_200_OK
             << '\n' << c_http_date_prefix << formatted_dtm
             << '\n' << c_http_modified_prefix << formatted_document_dtm
             << '\n' << c_http_connection_header << '\n' << c_http_keep_alive_header << '\n';

            if( extension == c_ext_css )
               osstr << c_http_content_type_text_css << '\n';
            else if( extension == c_ext_gif )
               osstr << c_http_content_type_image_gif << '\n';
            else if( extension == c_ext_jpg )
               osstr << c_http_content_type_image_jpg << '\n';
            else if( extension == c_ext_png )
               osstr << c_http_content_type_image_png << '\n';
            else if( extension == c_ext_ttf )
               osstr << c_http_content_type_font_ttf << '\n';
            else
               osstr << c_http_content_type_text_html << '\n';

            response = buffer_file( path + document );
         }

         if( found )
         {
            if( response.empty( ) )
               response = c_test_html_response;

            osstr << c_http_content_length_header_prefix
             << to_string( response.length( ) + 1 ) << "\n\n" << response;
         }
         else
         {
            response = c_not_found_html_response;

            replace( response, c_replace_document_marker, document );

            osstr << c_http_1_1
             << ' ' << c_http_404_Not_Found
             << '\n' << c_http_content_type_text_html
             << '\n' << c_http_content_length_header_prefix
             << to_string( response.length( ) + 1 ) << "\n\n" << response;
         }

         p_socket->write_line( osstr.str( ), c_response_timeout );
      }

#ifdef DEBUG
      cerr << "*** finished request/response ***" << endl;
#endif

      p_socket->close( );

      delete p_socket;
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
            break;

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
