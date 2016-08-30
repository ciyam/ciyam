// Copyright (c) 2006-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2016 CIYAM Developers
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

#include "regex.h"
#include "config.h"
#include "macros.h"
#include "sha256.h"
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

#ifdef ZLIB_SUPPORT
#  include <zlib.h>
#endif

//#define DEBUG

#define USE_NO_DELAY

using namespace std;

namespace
{

#include "ciyam_constants.h"

const char* const c_app_title = "ciyam_client";
const char* const c_app_version = "0.1";

const char* const c_cmd_exec = "exec";
const char* const c_cmd_parm_exec_command = "command";
const char* const c_cmd_parm_exec_arguments = "arguments";

const char* const c_env_var_pid = "PID";
const char* const c_env_var_error = "ERROR";
const char* const c_env_var_output = "OUTPUT";

const size_t c_pid_timeout = 5000;
const size_t c_command_timeout = 60000;
const size_t c_connect_timeout = 10000;
const size_t c_greeting_timeout = 10000;

const size_t c_max_length_for_output_env_var = 1024;

const unsigned long c_max_uncompressed_bytes = 100000;

const int64_t c_max_size_to_buffer = INT64_C( 1073741824 );

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

int g_pid = get_pid( );

string g_single_command;

class ciyam_console_startup_functor : public command_functor
{
   public:
   ciyam_console_startup_functor( command_handler& handler )
    : command_functor( handler )
   {
   }

   void operator ( )( const string& command, const parameter_info& parameters )
   {
      if( command == c_cmd_exec )
      {
         g_single_command = get_parm_val( parameters, c_cmd_parm_exec_command );

         string arguments( get_parm_val( parameters, c_cmd_parm_exec_arguments ) );

         if( !arguments.empty( ) )
            g_single_command += " " + arguments;
      }
   }
};

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
    socket( socket ),
    had_chk_command( false )
   {
      set_custom_startup_options( 1, "[<port> or <host[(:|-)port]>]" );
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

   bool had_chk_command;

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
      string::size_type pos = str.find( ' ' );

      if( str[ 0 ] != '?' && str.substr( 0, pos ) != "help" )
         str = console_command_handler::preprocess_command_and_args( str );

      if( !str.empty( ) )
      {
         bool was_chk = false;
         bool was_pip = false;
         bool was_chk_tag = false;
         bool was_chk_token = false;
         string get_dest_file, put_source_file;

         if( str.substr( 0, pos ) == "pip" )
            was_pip = true;

         if( str.substr( 0, pos ) == "chk"
          || str.substr( 0, pos ) == "get" || str.substr( 0, pos ) == "put"
          || str.substr( 0, pos ) == "file_get" || str.substr( 0, pos ) == "file_put"  )
         {
            string data( str.substr( pos + 1 ) );
            string extra;

            if( str.substr( 0, pos ) == "chk" )
            {
               was_chk = true;
               if( data.find( ' ' ) != string::npos )
                  was_chk_token = true;
            }

            if( str.substr( 0, pos ) == "get"
             || str.substr( 0, pos ) == "file_get" || str.substr( 0, pos ) == "file_put" )
            {
               string::size_type spos = data.find( ' ' );
               if( spos != string::npos )
               {
                  if( str.substr( 0, pos ) == "file_put" )
                     extra = data.substr( spos );
                  else
                     get_dest_file = data.substr( spos + 1 );

                  data.erase( spos );
               }
            }

            string prefix( c_file_type_str_blob );

            regex expr( c_regex_hash_256 );
            if( !was_chk_token && expr.search( data ) == string::npos && str.substr( 0, pos ) == "chk" )
               was_chk_tag = true;

            // NOTE: For testing purposes if a local file exists with the name given then
            // assumes what is wanted is a blob with this file content rather than a tag.
            if( was_chk_tag && file_exists( data ) )
            {
               str.erase( pos + 1 );
               data = buffer_file( data );

               str += sha256( prefix + data ).get_digest_as_string( ) + extra;

               cout << str << endl;

               was_chk_tag = false;
            }

            if( expr.search( data ) == string::npos
             && str.substr( 0, pos ) == "put" || str.substr( 0, pos ) == "file_put" )
            {
               str.erase( pos + 1 );

               put_source_file = data;

               if( file_size( put_source_file ) > c_max_size_to_buffer )
                  throw runtime_error( "file exceeds max. length restriction of " + to_string( c_max_size_to_buffer ) + " bytes" );

               data = buffer_file( put_source_file );

               str += sha256( prefix + data ).get_digest_as_string( ) + extra;

               cout << str << endl;
            }
         }

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
         if( str == "bye" || str == "quit" )
         {
            str.erase( );
            set_finished( );
         }
         else
         {
            bool was_get_or_put = false;

            try
            {
               string::size_type pos = str.find( ' ' );

               if( str.substr( 0, pos ) == "get" || str.substr( 0, pos ) == "put" )
                  was_get_or_put = true;

               if( str.substr( 0, pos ) == "get" || str.substr( 0, pos ) == "file_get" )
               {
                  string filename( !get_dest_file.empty( ) ? get_dest_file : str.substr( pos + 1 ) );

                  unsigned char prefix( !get_dest_file.empty( ) ? '\1' : '\0' );

                  file_transfer( filename, socket,
                   e_ft_direction_recv, c_max_file_transfer_size,
                   c_response_okay_more, c_file_transfer_initial_timeout,
                   c_file_transfer_line_timeout, c_file_transfer_max_line_size, &prefix );

#ifdef ZLIB_SUPPORT
                  if( prefix & c_file_type_char_compressed )
                  {
                     string data( buffer_file( filename ) );
                     string expanded_data( c_max_uncompressed_bytes, '\0' );

                     unsigned long usize = c_max_uncompressed_bytes;

                     if( uncompress( ( Bytef* )&expanded_data[ 0 ], &usize, ( Bytef* )&data[ 0 ], data.size( ) ) != Z_OK )
                        throw runtime_error( "bad compressed file or buffer not big enough" );

                     expanded_data.erase( usize );

                     write_file( filename, expanded_data );
                  }
#endif
               }
               else if( str.substr( 0, pos ) == "put" || str.substr( 0, pos ) == "file_put" )
               {
                  string filename( !put_source_file.empty( ) ? put_source_file : str.substr( pos + 1 ) );

                  unsigned char prefix( !put_source_file.empty( ) ? c_file_type_char_blob : '\0' );

                  file_transfer( filename, socket,
                   e_ft_direction_send, c_max_file_transfer_size,
                   c_response_okay_more, c_file_transfer_initial_timeout,
                   c_file_transfer_line_timeout, c_file_transfer_max_line_size, &prefix );
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
            bool had_not_found = false;
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
                     else if( had_not_found )
                        break;
                     else
                        throw runtime_error( "timeout occurred awaiting server response" );
                  }
                  else
                     throw runtime_error( "server has terminated this connection" );
               }

               if( was_pip || was_chk_tag || was_chk_token || was_get_or_put )
               {
                  // FUTURE: Verify the hash if "was_chk_token".
                  if( was_pip || was_chk_tag || was_chk_token )
                  {
                     cout << response << endl;

                     response.erase( );
                     socket.read_line( response );
                  }

                  if( was_get_or_put || response != string( c_response_okay ) )
                  {
                     string::size_type pos = response.find( ' ' );

                     if( pos == string::npos )
                        throw runtime_error( "unexpected response: " + response );

                     if( response.substr( 0, pos ) == "get" )
                     {
                        file_transfer( response.substr( pos + 1 ),
                         socket, e_ft_direction_send, c_max_file_transfer_size,
                         c_response_okay_more, c_file_transfer_line_timeout, c_file_transfer_max_line_size );
                     }
                     else if( response.substr( 0, pos ) == "put" )
                     {
                        file_transfer( response.substr( pos + 1 ),
                         socket, e_ft_direction_recv, c_max_file_transfer_size,
                         c_response_okay_more, c_file_transfer_line_timeout, c_file_transfer_max_line_size );

                        // NOTE: If is first "chk" and *not found* is returned then it is next expected
                        // that a file will be be sent and then fetched to ensure that the peer is able
                        // to store and fetch back the same file content.
                        if( was_chk_tag && !had_chk_command )
                        {
                           file_transfer( response.substr( pos + 1 ), socket,
                            e_ft_direction_send, c_max_file_transfer_size,
                            c_response_okay_more, c_file_transfer_line_timeout, c_file_transfer_max_line_size );
                        }
                     }
                     else if( response.substr( 0, pos ) == "chk" )
                     {
                        response.erase( 0, pos + 1 );
                        pos = response.find( ' ' );
                        if( pos == string::npos )
                           throw runtime_error( "unexpected chk args '" + response + "'" );

                        string hash( response.substr( 0, pos ) );
                        string token( response.substr( pos + 1 ) );

                        sha256 temp_hash;
                        temp_hash.update( token );
                        temp_hash.update( hash, true );

                        string hash_val( temp_hash.get_digest_as_string( ) );
                        cout << hash_val << endl;

                        socket.write_line( hash_val );
                        response.erase( );
                        socket.read_line( response );
                     }
                     else if( response.substr( 0, pos ) == "pip" )
                     {
                        cout << response.substr( pos + 1 ) << endl;

                        socket.write_line( "127.0.0.1" );
                        response.erase( );
                        socket.read_line( response );
                     }

                     was_pip = false;
                     was_chk_tag = false;
                     was_chk_token = false;
                     was_get_or_put = false;
                  }
               }

               if( was_chk )
                  had_chk_command = true;

               had_not_found = false;
               is_in_progress = false;

               if( response == string( c_response_okay_more ) )
               {
#ifdef DEBUG
                  cout << response;
#endif
                  response.erase( );
                  continue;
               }
               else if( response == string( c_response_not_found ) )
               {
                  had_not_found = true;
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
                     set_environment_variable( c_env_var_error, response.substr( start ).c_str( ) );

                  if( !is_error && !is_message && response.length( ) < c_max_length_for_output_env_var )
                     set_environment_variable( c_env_var_output, response.substr( start ).c_str( ) );

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
      if( !option.empty( )
       && option.find( '.' ) == string::npos
       && option[ 0 ] >= '0' && option[ 0 ] <= '9'  )
         port = atoi( option.c_str( ) );
      else
      {
         host = option;
         string::size_type pos = host.find( ':' );

         // NOTE: If host is an IPV6 address then use '-' as the port separator.
         if( pos == string::npos || host.find( ':', pos + 1 ) != string::npos )
            pos = host.find( '-' );

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

         cmd_handler.add_command( c_cmd_exec, 1,
          "<val//command>[<list//arguments// >]", "single command to execute", new ciyam_console_startup_functor( cmd_handler ) );

         processor.process_commands( );
      }

      const char* p_pid = getenv( c_env_var_pid );

      if( p_pid )
         g_pid = atoi( p_pid );

      if( !cmd_handler.has_option_quiet( ) )
         cout << application_title( e_app_info_request_title_and_version ) << endl;

      if( socket.open( ) )
      {
         ip_address address( cmd_handler.get_host( ), cmd_handler.get_port( ) );

         bool is_default = false;
         if( string( cmd_handler.get_host( ) ) == c_default_ciyam_host )
            is_default = true;

#ifdef _WIN32
         if( socket.connect( address, is_default ? 1000 : c_connect_timeout ) )
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
            if( socket.write_line( to_string( g_pid ), c_pid_timeout ) <= 0 )
            {
               string error;
               if( socket.had_timeout( ) )
                  error = "timeout occurred trying to connect to server";
               else
                  error = "application server has terminated this connection";

               socket.close( );
               throw runtime_error( error );
            }

            string greeting;
            if( socket.read_line( greeting, c_greeting_timeout ) <= 0 )
            {
               string error;
               if( socket.had_timeout( ) )
                  error = "timeout occurred trying to connect to server";
               else
                  error = "application server has terminated this connection";

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

            if( g_single_command.empty( ) )
               processor.process_commands( );
            else
               processor.execute_command( g_single_command );
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

