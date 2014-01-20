// Copyright (c) 2006-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2014 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <cstring>
#  include <cstdlib>
#  include <map>
#  include <string>
#  include <sstream>
#  include <iostream>
#  include <stdexcept>
#  ifdef __GNUG__
#     include <unistd.h>
#  endif
#endif

#ifdef __GNUG__
#  define _putenv putenv
#endif

#include "config.h"
#include "macros.h"
#include "sockets.h"
#include "console.h"
#include "utilities.h"
#ifdef SSL_SUPPORT
#  include "ssl_socket.h"
#  ifdef _WIN32
#     include <openssl/applink.c>
#  endif
#endif
#include "command_parser.h"
#include "console_commands.h"

//#define DEBUG

#define USE_NO_DELAY

using namespace std;

namespace
{

#include "ciyam_constants.h"

const char* const c_app_title = "ciyam_client";
const char* const c_app_version = "0.1";

const char* const c_env_var_error = "ERROR";

const size_t c_command_timeout = 60000;
const size_t c_greeting_timeout = 10000;

string application_title( app_info_request request )
{
   if( request == e_app_info_request_title )
      return string( c_app_title );
   else if( request == e_app_info_request_version )
      return string( c_app_version );
   else if( request == e_app_info_request_title_and_version )
   {
      string title( c_app_title );
      title += " v";
      title += string( c_app_version );

      return title;
   }
   else
   {
      ostringstream osstr;
      osstr << "unknown app_info_request: " << request;
      throw runtime_error( osstr.str( ) );
   }
}

class ciyam_console_command_handler : public console_command_handler
{
   public:
#ifdef SSL_SUPPORT
   ciyam_console_command_handler( ssl_socket& socket )
#else
   ciyam_console_command_handler( tcp_socket& socket )
#endif
    :
    port( c_default_ciyam_port ),
    host( c_default_ciyam_host ),
    socket( socket )
   {
      set_custom_startup_options( 1, "[<port> or <host[:port]>]" );
   }

   const char* get_host( ) const { return host.c_str( ); }
   int get_port( ) const { return port; }

   private:
   int port;
   string host;

#ifdef SSL_SUPPORT
   ssl_socket& socket;
#else
   tcp_socket& socket;
#endif

   string preprocess_command_and_args( const string& cmd_and_args );

   void process_custom_startup_option( size_t num, const string& option );
};

string ciyam_console_command_handler::preprocess_command_and_args( const string& cmd_and_args )
{
   if( is_handling_startup_options( ) )
      return console_command_handler::preprocess_command_and_args( cmd_and_args );

   string str( cmd_and_args );

   if( !str.empty( ) )
   {
      if( str[ 0 ] != '?' )
         str = console_command_handler::preprocess_command_and_args( str );

      if( !str.empty( ) )
      {
         if( str == "encrypt" )
            str += " " + get_password( "Password: " );
#ifdef DEBUG
         cout << "sending command: " << str << endl;
#endif
         socket.write_line( str );

#ifdef SSL_SUPPORT
         if( str == "starttls" && !socket.is_secure( ) )
            socket.ssl_connect( );
#endif
         if( str == "quit" )
         {
            str.erase( );
            set_finished( );
         }
         else
         {
            try
            {
               string::size_type pos = str.find( ' ' );
               if( str.substr( 0, pos ) == "file_get" )
               {
                  file_transfer( str.substr( pos + 1 ), socket,
                   e_ft_direction_fetch, c_max_file_transfer_size,
                   c_response_okay_more, c_file_transfer_line_timeout, c_file_transfer_max_line_size );
               }
               else if( str.substr( 0, pos ) == "file_put" )
               {
                  file_transfer( str.substr( pos + 1 ), socket,
                   e_ft_direction_send, c_max_file_transfer_size,
                   c_response_okay_more, c_file_transfer_line_timeout, c_file_transfer_max_line_size );
               }
            }
            catch( exception& x )
            {
               str.erase( );
               string s( x.what( ) );

               size_t err_prefix_length( strlen( c_response_error_prefix ) );

               if( s.length( ) > err_prefix_length
                && s.substr( 0, err_prefix_length ) == string( c_response_error_prefix ) )
               {
                  s = "Error: " + s.substr( err_prefix_length );
               }

               cout << s << endl;
               return str;
            }

            string response;
            bool is_in_progress = false;
            while( response.empty( ) || response[ 0 ] != '(' )
            {
               response.erase( );
               if( socket.read_line( response, c_command_timeout ) <= 0 )
               {
                  if( socket.had_timeout( ) )
                  {
                     // NOTE: Ignore timeouts if last received a progress message.
                     if( is_in_progress )
                        continue;
                     else
                        throw runtime_error( "timeout occurred awaiting server response" );
                  }
                  else
                     throw runtime_error( "server has terminated this connection" );
               }

               is_in_progress = false;

               if( response == string( c_response_okay_more ) )
               {
#ifdef DEBUG
                  cout << response;
#endif
                  response.erase( );
                  continue;
               }

               if( response != string( c_response_okay ) )
               {
                  size_t start = 0;

                  bool is_error = false;
                  size_t err_prefix_length( strlen( c_response_error_prefix ) );

                  if( response.length( ) > err_prefix_length
                   && response.substr( 0, err_prefix_length ) == string( c_response_error_prefix ) )
                  {
                     is_error = true;
                     start = err_prefix_length;
                  }

                  bool is_message = false;
                  size_t msg_prefix_length( strlen( c_response_message_prefix ) );

                  if( response.length( ) > msg_prefix_length
                   && response.substr( 0, msg_prefix_length ) == string( c_response_message_prefix ) )
                  {
                     is_message = true;
                     start = msg_prefix_length;
                  }

                  if( is_error )
                     cout << "Error: ";

                  if( is_error || !get_is_quiet_command( ) )
                     cout << response.substr( start ) << endl;

                  if( is_error && getenv( c_env_var_error ) == 0 )
                     _putenv( ( char* )( string( c_env_var_error ) + "=" + response.substr( start ) ).c_str( ) );

                  // NOTE: Make sure that progress messages do not end the conversation.
                  if( is_message )
                  {
                     response.erase( );
                     is_in_progress = true;
                  }

               }
#ifdef DEBUG
               else
                  cout << response;
#endif
            }

            str.erase( );
         }
      }
   }

   return str;
}

void ciyam_console_command_handler::process_custom_startup_option( size_t num, const string& option )
{
   if( num == 0 )
   {
      if( !option.empty( ) && option[ 0 ] >= '0' && option[ 0 ] <= '9' )
         port = atoi( option.c_str( ) );
      else
      {
         host = option;
         string::size_type pos = host.find( ':' );
         if( pos != string::npos )
         {
            port = atoi( host.substr( pos + 1 ).c_str( ) );
            host.erase( pos );
         }
      }
   }
   else
      throw runtime_error( "unexpected custom startup option '" + option + "'" );
}

}

int main( int argc, char* argv[ ] )
{
#ifdef _WIN32
   winsock_init wsi;
#endif   

#ifdef SSL_SUPPORT
   if( file_exists( "ciyam_client.pem" ) )
      init_ssl( "ciyam_client.pem", "password" );
#endif

#ifdef SSL_SUPPORT
   ssl_socket socket;
#else
   tcp_socket socket;
#endif

   ciyam_console_command_handler cmd_handler( socket );

   try
   {
      // NOTE: Use block scope for startup command processor object...
      {
         startup_command_processor processor( cmd_handler, application_title, 0, argc, argv );
         processor.process_commands( );
      }

      if( !cmd_handler.has_option_quiet( ) )
         cout << application_title( e_app_info_request_title_and_version ) << endl;

      if( socket.open( ) )
      {
         ip_address address( cmd_handler.get_host( ), cmd_handler.get_port( ) );

         bool is_default = false;
         if( string( cmd_handler.get_host( ) ) == c_default_ciyam_host )
            is_default = true;

#ifdef _WIN32
         if( socket.connect( address, is_default ? 1000 : c_greeting_timeout ) )
#else
         // NOTE: If the server was started asynchronously in a script immediately prior
         // to the client then wait for half a second and then try again just to be sure.
         if( socket.connect( address ) || ( is_default && ( msleep( 500 ), socket.connect( address ) ) ) )
#endif
         {
#ifdef USE_NO_DELAY
            if( !socket.set_no_delay( ) )
               cout << "warning: set_no_delay failed..." << endl;
#endif
            string greeting;
            if( socket.read_line( greeting, c_greeting_timeout ) <= 0 )
            {
               string error;
               if( socket.had_timeout( ) )
                  error = "timeout occurred trying to connect to server";
               else
                  error = "server has terminated this connection";

               socket.close( );
               throw runtime_error( error );
            }

            version_info ver_info;
            if( get_version_info( greeting, ver_info ) != string( c_response_okay ) )
            {
               socket.close( );
               throw runtime_error( greeting );
            }

            if( !check_version_info( ver_info, c_protocol_major_version, c_protocol_minor_version ) )
            {
               socket.close( );
               throw runtime_error( "incompatible protocol version "
                + ver_info.ver + " (expecting " + string( c_protocol_version ) + ")" );
            }

            console_command_processor processor( cmd_handler );
            processor.process_commands( );
         }
         else
            throw runtime_error( "unable to connect to host '"
             + string( cmd_handler.get_host( ) ) + "' on port " + to_string( cmd_handler.get_port( ) ) );

         socket.close( );
      }
#ifdef __GNUG__
      if( isatty( STDIN_FILENO ) )
         cout << endl;
#endif
   }
   catch( exception& x )
   {
      cerr << "error: " << x.what( ) << endl;
   }

#ifdef SSL_SUPPORT
   term_ssl( );
#endif
}

