// Copyright (c) 2014 CIYAM Developers
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
#  include <cstdlib>
#  include <memory>
#  include <vector>
#  include <string>
#  include <sstream>
#  include <iostream>
#  include <algorithm>
#  include <stdexcept>
#endif

#include "peer_session.h"

#include "config.h"
#include "sha256.h"
#include "threads.h"
#include "utilities.h"
#include "date_time.h"
#include "ciyam_base.h"
#include "ciyam_files.h"
#include "ciyam_strings.h"
#include "command_parser.h"
#include "command_handler.h"
#include "command_processor.h"

//#define DEBUG

using namespace std;

extern size_t g_active_sessions;
extern volatile sig_atomic_t g_server_shutdown;

namespace
{

#include "ciyam_constants.h"

#include "peer_session.cmh"

#include "trace_progress.cpp"

mutex g_mutex;

int g_port = c_default_ciyam_port + 1;

const char* const c_hello = "hello";

const int c_accept_timeout = 250;
const int c_max_line_length = 500;

const size_t c_request_timeout = 5000;
const size_t c_greeting_timeout = 10000;

const size_t c_request_throttle_sleep_time = 500;

enum peer_state
{
   e_peer_state_invalid,
   e_peer_state_listener,
   e_peer_state_initiator,
   e_peer_state_waiting_for_get,
   e_peer_state_waiting_for_put
};

enum peer_trust_level
{
   e_peer_trust_level_none,
   e_peer_trust_level_normal
};

size_t g_num_peers = 0;

bool has_max_peers( )
{
   guard g( g_mutex );

   return g_num_peers >= get_max_peers( );
}

inline void issue_error( const string& message )
{
#ifdef DEBUG
   cerr << "session error: " << message << endl;
#else
   TRACE_LOG( TRACE_ANYTHING, string( "session error: " ) + message );
#endif
}

inline void issue_warning( const string& message )
{
#ifdef DEBUG
   cout << "session warning: " << message << endl;
#else
   TRACE_LOG( TRACE_SESSIONS, string( "session warning: " ) + message );
#endif
}

class socket_command_handler : public command_handler
{
   public:
#ifdef SSL_SUPPORT
   socket_command_handler( ssl_socket& socket, peer_state session_state, bool is_local )
#else
   socket_command_handler( tcp_socket& socket, peer_state session_state, bool is_local )
#endif
    :
    socket( socket ),
    is_local( is_local ),
    session_state( session_state ),
    session_trust_level( e_peer_trust_level_none )
   {
      had_usage = false;

      is_listener = ( session_state == e_peer_state_listener );
   }

#ifdef SSL_SUPPORT
   ssl_socket& get_socket( ) { return socket; }
#else
   tcp_socket& get_socket( ) { return socket; }
#endif

   const string& get_last_command( ) { return last_command; }
   const string& get_next_command( ) { return next_command; }

   bool get_is_local( ) const { return is_local; }
   bool get_is_listener( ) const { return is_listener; }

   string& prior_put( ) { return prior_put_hash; }

   void get_hello( );
   void put_hello( );

   void pip_peer( const string& ip_address );

   void chk_file( const string& hash );

   peer_state& state( ) { return session_state; }
   peer_trust_level& trust_level( ) { return session_trust_level; }

   void kill_session( )
   {
      if( !is_captured_session( ) )
         set_finished( );
      else if( !is_condemned_session( ) )
         condemn_this_session( );
   }

   private:
   string preprocess_command_and_args( const string& cmd_and_args );

   void postprocess_command_and_args( const string& cmd_and_args );

   void handle_unknown_command( const string& command )
   {
      socket.write_line( string( c_response_error_prefix ) + "unknown command '" + command + "'", c_request_timeout );
      kill_session( );
   }

   void handle_invalid_command( const command_parser& parser, const string& cmd_and_args )
   {
      socket.write_line( string( c_response_error_prefix ) + "invalid command usage '" + cmd_and_args + "'", c_request_timeout );
      kill_session( );
   }

   void handle_command_response( const string& response, bool is_special );

#ifdef SSL_SUPPORT
   ssl_socket& socket;
#else
   tcp_socket& socket;
#endif

   bool is_local;
   bool had_usage;
   bool is_listener;

   string last_command;
   string next_command;

   string prior_put_hash;

   peer_state session_state;
   peer_trust_level session_trust_level;
};

void socket_command_handler::get_hello( )
{
   progress* p_progress = 0;
   trace_progress progress( TRACE_SOCK_OPS );

   if( get_trace_flags( ) & TRACE_SOCK_OPS )
      p_progress = &progress;

   string data( c_file_type_str_blob );
   data += string( c_hello );

   string temp_hash( lower( sha256( data ).get_digest_as_string( ) ) );

   if( !has_file( temp_hash ) )
      create_raw_file( data );

   string temp_file_name( "~" + uuid( ).as_string( ) );
   socket.write_line( string( c_cmd_peer_session_get ) + " " + temp_hash, c_request_timeout, p_progress );

   store_temp_file( temp_file_name, socket );

   if( !temp_file_is_identical( temp_file_name, temp_hash ) )
   {
      file_remove( temp_file_name );
      throw runtime_error( "invalid get_hello" );
   }

   file_remove( temp_file_name );
}

void socket_command_handler::put_hello( )
{
   progress* p_progress = 0;
   trace_progress progress( TRACE_SOCK_OPS );

   if( get_trace_flags( ) & TRACE_SOCK_OPS )
      p_progress = &progress;

   string data( c_file_type_str_blob );
   data += string( c_hello );

   string temp_hash( lower( sha256( data ).get_digest_as_string( ) ) );

   if( !has_file( temp_hash ) )
      create_raw_file( data );

   socket.write_line( string( c_cmd_peer_session_put ) + " " + temp_hash, c_request_timeout, p_progress );

   fetch_file( temp_hash, socket );
}

void socket_command_handler::pip_peer( const string& ip_address )
{
   progress* p_progress = 0;
   trace_progress progress( TRACE_SOCK_OPS );

   if( get_trace_flags( ) & TRACE_SOCK_OPS )
      p_progress = &progress;

   socket.write_line( string( c_cmd_peer_session_pip ) + " " + ip_address, c_request_timeout, p_progress );

   string response;
   if( socket.read_line( response, c_request_timeout, 0, p_progress ) <= 0 )
   {
      string error;
      if( socket.had_timeout( ) )
         error = "timeout occurred getting peer response";
      else
         error = "server has terminated this connection";

      socket.close( );
      throw runtime_error( error );
   }
}

void socket_command_handler::chk_file( const string& hash )
{
   progress* p_progress = 0;
   trace_progress progress( TRACE_SOCK_OPS );

   if( get_trace_flags( ) & TRACE_SOCK_OPS )
      p_progress = &progress;

   string token( uuid( ).as_string( ) );

   string expected( hash_with_nonce( hash, token ) );

   socket.write_line( string( c_cmd_peer_session_chk )
    + " " + hash + " " + token, c_request_timeout, p_progress );

   string response;
   if( socket.read_line( response, c_request_timeout, 0, p_progress ) <= 0 )
   {
      string error;
      if( socket.had_timeout( ) )
         error = "timeout occurred getting peer response";
      else
         error = "server has terminated this connection";

      socket.close( );
      throw runtime_error( error );
   }

   if( response != expected )
      throw runtime_error( "unexpected invalid chk response: " + response );
}

string socket_command_handler::preprocess_command_and_args( const string& cmd_and_args )
{
   string str( cmd_and_args );

   if( session_state == e_peer_state_invalid )
      str.erase( );

   if( !str.empty( ) )
   {
      TRACE_LOG( TRACE_COMMANDS, cmd_and_args );

      string::size_type pos = str.find( ' ' );

      if( str[ 0 ] == '?' || str.substr( 0, pos ) == "help" )
      {
         if( !had_usage )
         {
            had_usage = true;

            string wildcard_match_expr;
            if( pos != string::npos )
               wildcard_match_expr = str.substr( pos + 1 );

            if( get_command_processor( ) )
               get_command_processor( )->output_command_usage( wildcard_match_expr );

            str.erase( );
         }
         else
            str = c_cmd_peer_session_bye;
      }
   }

   if( !str.empty( ) )
      next_command = str;

   return str;
}

void socket_command_handler::postprocess_command_and_args( const string& cmd_and_args )
{
   string::size_type pos = cmd_and_args.find( ' ' );
   last_command = cmd_and_args.substr( 0, pos );

   if( has_finished( ) )
      TRACE_LOG( TRACE_SESSIONS, "finished session" );
}

void socket_command_handler::handle_command_response( const string& response, bool is_special )
{
   progress* p_progress = 0;
   trace_progress progress( TRACE_SOCK_OPS );

   if( get_trace_flags( ) & TRACE_SOCK_OPS )
      p_progress = &progress;

   if( !response.empty( ) )
   {
      if( is_special && !socket.set_no_delay( ) )
         issue_warning( "socket set_no_delay failure" );

      socket.write_line( response, c_request_timeout, p_progress );
   }

   if( !is_special && is_listener )
   {
      if( !socket.set_no_delay( ) )
         issue_warning( "socket set_no_delay failure" );

      socket.write_line( c_response_okay, c_request_timeout, p_progress );
   }
}

class peer_session_command_functor : public command_functor
{
   public:
   peer_session_command_functor( command_handler& handler )
    : command_functor( handler ),
    socket_handler( dynamic_cast< socket_command_handler& >( handler ) )
   {
   }

   void operator ( )( const string& command, const parameter_info& parameters );

   socket_command_handler& socket_handler;
};

command_functor* peer_session_command_functor_factory( const string& /*name*/, command_handler& handler )
{
   return new peer_session_command_functor( handler );
}

void peer_session_command_functor::operator ( )( const string& command, const parameter_info& parameters )
{
#ifdef DEBUG
   cout << "processing command: " << command << endl;
#endif
   string response;
   bool send_okay_response = true;
   bool possibly_expected_error = false;

#ifdef SSL_SUPPORT
   ssl_socket& socket( socket_handler.get_socket( ) );
#else
   tcp_socket& socket( socket_handler.get_socket( ) );
#endif

   progress* p_progress = 0;
   trace_progress progress( TRACE_SOCK_OPS );

   if( get_trace_flags( ) & TRACE_SOCK_OPS )
      p_progress = &progress;

   if( command != c_cmd_peer_session_bye && !socket.set_delay( ) )
      issue_warning( "socket set_delay failure" );

   set_last_session_cmd_and_hash( command, socket_handler.get_next_command( ) );

   try
   {
      ostringstream osstr;

      if( command == c_cmd_peer_session_chk )
      {
         string hash( get_parm_val( parameters, c_cmd_parm_peer_session_chk_hash ) );
         string nonce( get_parm_val( parameters, c_cmd_parm_peer_session_chk_nonce ) );

         if( socket_handler.state( ) != e_peer_state_listener
          && socket_handler.state( ) != e_peer_state_waiting_for_get
          && socket_handler.state( ) != e_peer_state_waiting_for_put )
            throw runtime_error( "invalid state for chk" );

         bool has = has_file( hash );
         bool was_initial_state = ( socket_handler.state( ) == e_peer_state_listener );

         if( !has )
         {
            response = c_response_not_found;

            handler.issue_command_reponse( response, true );

            if( was_initial_state )
            {
               // NOTE: For the initial file transfer just a dummy "hello" blob.
               string data( c_file_type_str_blob );
               data += string( c_hello );

               string temp_hash( lower( sha256( data ).get_digest_as_string( ) ) );

               if( !has_file( temp_hash ) )
                  create_raw_file( data );

               handler.issue_command_reponse( "put " + temp_hash, true );
               fetch_file( temp_hash, socket );

               string temp_file_name( "~" + uuid( ).as_string( ) );
               store_temp_file( temp_file_name, socket );

               response.erase( );

               if( !temp_file_is_identical( temp_file_name, temp_hash ) )
               {
                  send_okay_response = false;
                  socket_handler.state( ) = e_peer_state_invalid;
               }
               else
               {
                  socket_handler.state( ) = e_peer_state_waiting_for_put;
                  socket_handler.trust_level( ) = e_peer_trust_level_normal;
               }

               increment_peer_files_uploaded( data.length( ) );
               increment_peer_files_downloaded( data.length( ) );

               file_remove( temp_file_name );
            }
         }
         else
         {
            if( !nonce.empty( ) )
               response = hash_with_nonce( hash, nonce );

            if( socket_handler.state( ) == e_peer_state_waiting_for_get )
               socket_handler.state( ) = e_peer_state_waiting_for_put;
            else
               socket_handler.state( ) = e_peer_state_waiting_for_get;

            if( !was_initial_state && socket_handler.get_is_listener( ) )
            {
               handler.issue_command_reponse( response, true );
               response.erase( );

               // KLUDGE: For now just use "hello" as the file.
               if( socket_handler.state( ) == e_peer_state_waiting_for_get )
                  socket_handler.get_hello( );
               else
                  socket_handler.put_hello( );
            }
         }
      }
      else if( command == c_cmd_peer_session_get )
      {
         string hash( get_parm_val( parameters, c_cmd_parm_peer_session_get_hash ) );

         if( socket_handler.state( ) != e_peer_state_waiting_for_get )
            throw runtime_error( "invalid state for get" );

         fetch_file( hash, socket );
         increment_peer_files_downloaded( file_bytes( hash ) );

         socket_handler.state( ) = e_peer_state_waiting_for_put;

         // KLUDGE: For now just use "hello" as the file to put.
         if( socket_handler.get_is_listener( ) )
            socket_handler.put_hello( );
      }
      else if( command == c_cmd_peer_session_put )
      {
         string hash( get_parm_val( parameters, c_cmd_parm_peer_session_put_hash ) );

         if( socket_handler.state( ) != e_peer_state_waiting_for_put )
            throw runtime_error( "invalid state for put" );

         if( !has_file( hash ) )
         {
            store_file( hash, socket );
            increment_peer_files_uploaded( file_bytes( hash ) );
         }
         else
         {
            string temp_file_name( "~" + uuid( ).as_string( ) );
            store_temp_file( temp_file_name, socket );
            file_remove( temp_file_name );
         }

         if( socket_handler.prior_put( ).empty( ) || ( rand( ) % 100 < 5 ) )
            socket_handler.prior_put( ) = hash;

         socket_handler.state( ) = e_peer_state_waiting_for_get;

         if( socket_handler.get_is_listener( ) )
         {
            // KLUDGE: For now just randomly perform a "chk", "pip" or a "get" (this should instead be
            // based upon the actual needs of the peer).
            if( rand( ) % 5 == 0 )
               socket_handler.chk_file( socket_handler.prior_put( ) );
            else if( rand( ) % 5 == 0 )
               socket_handler.pip_peer( "127.0.0.1" );
            else
               // KLUDGE: For now just use "hello" as the file to get.
               socket_handler.get_hello( );
         }
      }
      else if( command == c_cmd_peer_session_pip )
      {
         string addr( get_parm_val( parameters, c_cmd_parm_peer_session_pip_addr ) );

         if( socket_handler.get_last_command( ) == c_cmd_peer_session_pip )
            throw runtime_error( "invalid state for pip" );
         else
            response = "127.0.0.1"; // KLUDGE: This should return an actual peer IP address.

         if( socket_handler.state( ) != e_peer_state_waiting_for_get
          && socket_handler.state( ) != e_peer_state_waiting_for_put )
            throw runtime_error( "invalid state for pip" );

         if( socket_handler.state( ) == e_peer_state_waiting_for_put )
            socket_handler.state( ) = e_peer_state_waiting_for_get;
         else if( socket_handler.state( ) == e_peer_state_waiting_for_get )
            socket_handler.state( ) = e_peer_state_waiting_for_put;

         if( socket_handler.get_is_listener( ) )
         {
            handler.issue_command_reponse( response, true );
            response.erase( );

            // KLUDGE: For now just use "hello" as the file.
            if( socket_handler.state( ) == e_peer_state_waiting_for_get )
               socket_handler.get_hello( );
            else
               socket_handler.put_hello( );
         }
      }
      else if( command == c_cmd_peer_session_tls )
      {
         if( socket_handler.state( ) != e_peer_state_listener )
            throw runtime_error( "invalid state for tls" );

#ifdef SSL_SUPPORT
         if( socket.is_secure( ) )
            throw runtime_error( "TLS is already active" );

         if( !get_using_ssl( ) )
            throw runtime_error( "SSL has not been initialised" );

         socket.ssl_accept( );
#else
         throw runtime_error( "SSL support not available" );
#endif

         socket_handler.state( ) = e_peer_state_waiting_for_get;
      }
      else if( command == c_cmd_peer_session_bye )
      {
         if( !is_captured_session( ) )
            handler.set_finished( );
         else if( !is_condemned_session( ) )
            condemn_this_session( );

         return;
      }
   }
   catch( exception& x )
   {
      TRACE_LOG( ( possibly_expected_error ? TRACE_SESSIONS : TRACE_ANYTHING ), string( "peer session error: " ) + x.what( ) );

      send_okay_response = false;
      response = string( c_response_error_prefix ) + x.what( );

      socket_handler.state( ) = e_peer_state_invalid;
   }
   catch( ... )
   {
      TRACE_LOG( TRACE_ANYTHING, "peer session error: unexpected unknown exception caught" );

      send_okay_response = false;
      response = string( c_response_error_prefix ) + "unexpected unknown exception caught";

      socket_handler.state( ) = e_peer_state_invalid;
   }

   handler.issue_command_reponse( response, !send_okay_response );

   if( !send_okay_response )
   {
      if( !is_captured_session( ) )
         handler.set_finished( );
      else if( !is_condemned_session( ) )
         condemn_this_session( );
   }
   else if( !socket_handler.get_is_listener( )
    && !g_server_shutdown && !is_condemned_session( )
    && socket_handler.state( ) == e_peer_state_waiting_for_get )
   {
      string response;
      if( socket.read_line( response, c_request_timeout, 0, p_progress ) <= 0 )
      {
         string error;
         if( socket.had_timeout( ) )
            error = "timeout occurred getting peer response";
         else
            error = "server has terminated this connection";

         socket.close( );
         throw runtime_error( error );
      }

      if( response != string( c_response_okay ) )
      {
         socket.close( );
         throw runtime_error( "unexpected non-okay response from peer" );
      }

      // KLUDGE: For now just use "hello" as the file to put.
      socket_handler.put_hello( );
   }
}

class socket_command_processor : public command_processor
{
   public:
   socket_command_processor( tcp_socket& socket, command_handler& handler, bool is_local, bool is_listener )
    : command_processor( handler ),
    socket( socket ),
    handler( handler ),
    is_local( is_local ),
    is_listener( is_listener ),
    is_first_command( true )
   {
   }

   private:
   tcp_socket& socket;
   command_handler& handler;

   bool is_local;
   bool is_listener;
   bool is_first_command;

   bool is_still_processing( ) { return is_captured_session( ) || socket.okay( ); }

   string get_cmd_and_args( );

   void output_command_usage( const string& wildcard_match_expr ) const;
};

string socket_command_processor::get_cmd_and_args( )
{
   string request;

   if( is_first_command )
   {
      is_first_command = false;
      TRACE_LOG( TRACE_SESSIONS, ( !is_listener ? "started session (as peer)" : "started session (as listener)" ) );
   }

   socket_command_handler& socket_handler = dynamic_cast< socket_command_handler& >( handler );

   while( true )
   {
      progress* p_progress = 0;
      trace_progress progress( TRACE_SOCK_OPS );

      if( get_trace_flags( ) & TRACE_SOCK_OPS )
         p_progress = &progress;

      if( !is_listener && !g_server_shutdown && !is_condemned_session( ) )
      {
         if( socket_handler.state( ) == e_peer_state_waiting_for_put )
         {
            string response;
            if( socket.read_line( response, c_request_timeout, 0, p_progress ) <= 0 )
            {
               request = "bye";
               break;
            }

            if( response != string( c_response_okay ) )
            {
               request = "bye";
               break;
            }

            // KLUDGE: For now just randomly perform a "chk", "pip" or a "get" (this should instead be
            // based upon the actual needs of the peer).
            if( rand( ) % 5 == 0 && !socket_handler.prior_put( ).empty( ) )
               socket_handler.chk_file( socket_handler.prior_put( ) );
            else if( rand( ) % 5 == 0 )
               socket_handler.pip_peer( "127.0.0.1" );
            else
               // KLUDGE: For now just use "hello" as the file to get.
               socket_handler.get_hello( );
         }
      }

      if( socket.read_line( request, c_request_timeout, c_max_line_length, p_progress ) <= 0 )
      {
         if( !is_captured_session( )
          && ( is_condemned_session( ) || g_server_shutdown || !socket.had_timeout( ) ) )
         {
            // NOTE: If the session is not captured and it has either been condemned or
            // the server is shutting down, or its socket has died then force a "bye".
            request = "bye";
            break;
         }

         // NOTE: If the socket is dead then the "read_line" will return instantly so for
         // a "captured" session manually perform a timeout sleep so CPU is not overused.
         if( is_captured_session( ) && !socket.had_timeout( ) )
         {
            msleep( c_request_timeout );
            continue;
         }

         if( !is_local && socket.had_timeout( ) )
         {
            // NOTE: Don't allow zombies to hang around unless they are local.
            request = "bye";
            break;
         }
      }
      else
      {
         if( request != "bye" )
            msleep( c_request_throttle_sleep_time );

         if( request == c_response_okay || request == c_response_okay_more )
            request = "bye";

         break;
      }
   }

#ifdef DEBUG
   cout << "request = '" << request << "'" << endl;
#endif
   return request;
}

void socket_command_processor::output_command_usage( const string& wildcard_match_expr ) const
{
#ifdef DEBUG
   cout << "<processing usage request>" << endl;
#endif

   if( !socket.set_delay( ) )
      issue_warning( "socket set_delay failure" );

   string cmds( "commands:" );
   if( !wildcard_match_expr.empty( ) )
      cmds += ' ' + wildcard_match_expr;

   socket.write_line( cmds, c_request_timeout );
   socket.write_line( "=========", c_request_timeout );

   socket.write_line( get_usage_for_commands( wildcard_match_expr ), c_request_timeout );

   if( !socket.set_no_delay( ) )
      issue_warning( "socket set_no_delay failure" );

   socket.write_line( c_response_okay, c_request_timeout );
}

#ifdef SSL_SUPPORT
peer_session* construct_session( bool acceptor, auto_ptr< ssl_socket >& ap_socket, const string& ip_addr )
#else
peer_session* construct_session( bool acceptor, auto_ptr< tcp_socket >& ap_socket, const string& ip_addr )
#endif
{
   guard g( g_mutex );
   peer_session* p_session = 0;

   if( ip_addr == "127.0.0.1" || !has_session_with_ip_addr( ip_addr ) )
      p_session = new peer_session( acceptor, ap_socket, ip_addr );

   return p_session;
}

}

#ifdef SSL_SUPPORT
peer_session::peer_session( bool acceptor, auto_ptr< ssl_socket >& ap_socket, const string& ip_addr )
#else
peer_session::peer_session( bool acceptor, auto_ptr< tcp_socket >& ap_socket, const string& ip_addr )
#endif
 :
 is_local( false ),
 ip_addr( ip_addr ),
 acceptor( acceptor ),
 ap_socket( ap_socket )
{
   if( !( *this->ap_socket ) )
      throw runtime_error( "invalid socket..." );

   if( ip_addr == "127.0.0.1" )
      is_local = true;

   increment_session_count( );
}

peer_session::~peer_session( )
{
   decrement_session_count( );
}

void peer_session::on_start( )
{
#ifdef DEBUG
   cout << "started session..." << endl;
#endif
   try
   {
      socket_command_handler cmd_handler( *ap_socket,
       acceptor ? e_peer_state_listener : e_peer_state_initiator, is_local );

      cmd_handler.add_commands( 0,
       peer_session_command_functor_factory, ARRAY_PTR_AND_SIZE( peer_session_command_definitions ) );

      if( acceptor )
         ap_socket->write_line( string( c_protocol_version ) + '\n' + string( c_response_okay ), c_greeting_timeout );
      else
      {
         string greeting;
         if( ap_socket->read_line( greeting, c_greeting_timeout ) <= 0 )
         {
            string error;
            if( ap_socket->had_timeout( ) )
               error = "timeout occurred trying to connect to server";
            else
               error = "server has terminated this connection";

            ap_socket->close( );
            throw runtime_error( error );
         }

         version_info ver_info;
         if( get_version_info( greeting, ver_info ) != string( c_response_okay ) )
         {
            ap_socket->close( );
            throw runtime_error( greeting );
         }

         if( !check_version_info( ver_info, c_protocol_major_version, c_protocol_minor_version ) )
         {
            ap_socket->close( );
            throw runtime_error( "incompatible protocol version "
             + ver_info.ver + " (expecting " + string( c_protocol_version ) + ")" );
         }

         progress* p_progress = 0;
         trace_progress progress( TRACE_SOCK_OPS );

         if( get_trace_flags( ) & TRACE_SOCK_OPS )
            p_progress = &progress;

         string data( c_file_type_str_blob );
         data += string( c_hello );

         string hash( lower( sha256( data ).get_digest_as_string( ) ) );

         ap_socket->write_line( string( c_cmd_peer_session_chk ) + " " + hash, c_request_timeout, p_progress );

         cmd_handler.state( ) = e_peer_state_waiting_for_put;
      }

      init_session( cmd_handler, true, &ip_addr );

      socket_command_processor processor( *ap_socket, cmd_handler, is_local, acceptor );
      processor.process_commands( );

      ap_socket->close( );

      term_session( );
   }
   catch( exception& x )
   {
      issue_error( x.what( ) );

      ap_socket->write_line( string( c_response_error_prefix ) + x.what( ), c_request_timeout );
      ap_socket->close( );
   }
   catch( ... )
   {
      issue_error( "unexpected unknown exception occurred" );

      ap_socket->write_line( string( c_response_error_prefix ) + "unexpected exception occurred", c_request_timeout );
      ap_socket->close( );
   }

#ifdef DEBUG
   cout << "finished session..." << endl;
#endif
   delete this;
}

void peer_session::increment_session_count( )
{
   guard g( g_mutex );

   ++g_num_peers;
   ++g_active_sessions;
}

void peer_session::decrement_session_count( )
{
   guard g( g_mutex );

   --g_num_peers;
   --g_active_sessions;
}

void peer_listener::on_start( )
{
#ifdef DEBUG
   cout << "started peer_listener..." << endl;
#endif
   tcp_socket s;
   bool okay = s.open( );

   ip_address address( g_port );
   if( okay )
   {
      s.set_reuse_addr( );

      okay = s.bind( address );
      if( okay )
      {
         s.listen( );

         TRACE_LOG( TRACE_ANYTHING, "peer listener started on port " + to_string( g_port ) );

         while( s && ( !g_server_shutdown || g_active_sessions ) )
         {
            // NOTE: Check for accepts and create new sessions.
#ifdef SSL_SUPPORT
            auto_ptr< ssl_socket > ap_socket( new ssl_socket( s.accept( address, c_accept_timeout ) ) );
#else
            auto_ptr< tcp_socket > ap_socket( new tcp_socket( s.accept( address, c_accept_timeout ) ) );
#endif
            if( !g_server_shutdown && *ap_socket
             && !has_max_peers( ) && get_is_accepted_peer_id_addr( address.get_addr_string( ) ) )
            {
               peer_session* p_session = construct_session( true, ap_socket, address.get_addr_string( ) );

               if( p_session )
                  p_session->start( );
            }
         }

         s.close( );
      }
      else
         s.close( );
   }

#ifdef DEBUG
   cout << "finished peer_listener..." << endl;
#endif
   delete this;
}

void create_initial_peer_sessions( )
{
   set< string > initial_ips;
   get_initial_peer_ips( initial_ips );

   for( set< string >::iterator i = initial_ips.begin( ); i!= initial_ips.end( ); ++i )
   {
      string ip( *i );

      if( !get_is_accepted_peer_id_addr( ip ) )
         continue;

      if( g_server_shutdown || has_max_peers( ) )
         break;

#ifdef SSL_SUPPORT
      auto_ptr< ssl_socket > ap_socket( new ssl_socket );
#else
      auto_ptr< tcp_socket > ap_socket( new tcp_socket );
#endif

      if( ap_socket->open( ) )
      {
         ip_address address( ip.c_str( ), g_port );

         if( ap_socket->connect( address ) )
         {
            peer_session* p_session = construct_session( false, ap_socket, address.get_addr_string( ) );

            if( p_session )
               p_session->start( );
         }
      }
   }
}

