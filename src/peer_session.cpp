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

mutex g_mutex;

#include "ciyam_constants.h"

#include "peer_session.cmh"

int g_port = c_default_ciyam_port + 1;

const char* const c_hello = "hello";

const int c_accept_timeout = 250;
const int c_max_line_length = 100;

const size_t c_request_timeout = 30000;

enum peer_state
{
   e_peer_state_invalid,
   e_peer_state_listener,
   e_peer_state_initiator,
   e_peer_state_waiting_for_get,
   e_peer_state_waiting_for_put,
   e_peer_state_waiting_for_get_or_put
};

enum peer_trust_level
{
   e_peer_trust_level_none,
   e_peer_trust_level_weak,
   e_peer_trust_level_normal
};

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

void output_response_lines( tcp_socket& socket, const string& response )
{
   vector< string > lines;
   split( response, lines, '\n' );
   for( size_t i = 0; i < lines.size( ); i++ )
      socket.write_line( lines[ i ] );
}

class socket_command_handler : public command_handler
{
   public:
#ifdef SSL_SUPPORT
   socket_command_handler( ssl_socket& socket, peer_state session_state )
#else
   socket_command_handler( tcp_socket& socket, peer_state session_state )
#endif
    :
    socket( socket ),
    session_state( session_state ),
    session_trust_level( e_peer_trust_level_none )
   {
      had_usage = false;

      is_listener = ( session_state == e_peer_state_listener );
   }

   size_t get_sess_id( ) const { return sess_id; }
   void set_sess_id( size_t new_sess_id ) { sess_id = new_sess_id; }

#ifdef SSL_SUPPORT
   ssl_socket& get_socket( ) { return socket; }
#else
   tcp_socket& get_socket( ) { return socket; }
#endif

   const string& get_next_command( ) { return next_command; }

   bool get_is_listener( ) const { return is_listener; }

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
      socket.write_line( string( c_response_error_prefix ) + "unknown command '" + command + "'" );
      kill_session( );
   }

   void handle_invalid_command( const command_parser& parser, const string& cmd_and_args )
   {
      socket.write_line( string( c_response_error_prefix ) + "invalid command usage '" + cmd_and_args + "'" );
      kill_session( );
   }

   void handle_command_response( const string& response, bool is_special );

   size_t sess_id;
#ifdef SSL_SUPPORT
   ssl_socket& socket;
#else
   tcp_socket& socket;
#endif

   bool had_usage;
   bool is_listener;

   string next_command;

   peer_state session_state;
   peer_trust_level session_trust_level;
};

string socket_command_handler::preprocess_command_and_args( const string& cmd_and_args )
{
   string str( cmd_and_args );

   if( session_state == e_peer_state_invalid )
      str.erase( );

   if( !str.empty( ) )
   {
      TRACE_LOG( TRACE_COMMANDS, cmd_and_args );

      if( str[ 0 ] == '?' )
      {
         if( !had_usage )
         {
            had_usage = true;

            string wildcard_match_expr;

            string::size_type pos = str.find( ' ' );
            if( pos != string::npos )
               wildcard_match_expr = str.substr( pos + 1 );

            if( get_command_processor( ) )
               get_command_processor( )->output_command_usage( wildcard_match_expr );

            str.erase( );
         }
         else
            str = c_cmd_peer_session_quit;
      }
   }

   if( !str.empty( ) )
      next_command = str;

   return str;
}

void socket_command_handler::postprocess_command_and_args( const string& cmd_and_args )
{
   if( has_finished( ) )
      TRACE_LOG( TRACE_SESSIONS, "finished session" );
}

void socket_command_handler::handle_command_response( const string& response, bool is_special )
{
   if( !response.empty( ) )
   {
      if( is_special && !socket.set_no_delay( ) )
         issue_warning( "socket set_no_delay failure" );
      socket.write_line( response );
   }

   if( !is_special )
   {
      if( !socket.set_no_delay( ) )
         issue_warning( "socket set_no_delay failure" );
      socket.write_line( c_response_okay );
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

   if( command != c_cmd_peer_session_quit && !socket.set_delay( ) )
      issue_warning( "socket set_delay failure" );

   set_last_session_cmd_and_hash( command, socket_handler.get_next_command( ) );

   try
   {
      ostringstream osstr;

      if( command == c_cmd_peer_session_file_chk )
      {
         string filename( get_parm_val( parameters, c_cmd_parm_peer_session_file_chk_filename ) );

         if( socket_handler.state( ) != e_peer_state_listener )
            throw runtime_error( "invalid state for file_chk" );

         bool has = has_file( filename );

         if( !has )
         {
            response = c_response_not_found;

            handler.issue_command_reponse( response, true );

            // NOTE: For the initial file transfer just a dummy "hello" file.
            string name( c_hello );
            string hash( lower( sha256( name ).get_digest_as_string( ) ) );

            if( !has_file( hash ) )
               init_file( hash, c_hello );

            handler.issue_command_reponse( "file_put " + hash, true );
            fetch_file( hash, socket );

            string temp_file_name( "~" + uuid( ).as_string( ) );
            store_temp_file( temp_file_name, socket );

            response.erase( );

            if( !temp_file_is_identical( temp_file_name, hash ) )
               socket_handler.state( ) = e_peer_state_invalid;
            else
            {
               socket_handler.state( ) = e_peer_state_waiting_for_put;
               socket_handler.trust_level( ) = e_peer_trust_level_weak;
            }

            increment_peer_files_uploaded( name.length( ) );
            increment_peer_files_downloaded( name.length( ) );

            file_remove( temp_file_name );
         }
         else
            socket_handler.state( ) = e_peer_state_waiting_for_get;
      }
      else if( command == c_cmd_peer_session_file_get )
      {
         string filename( get_parm_val( parameters, c_cmd_parm_peer_session_file_get_filename ) );

         if( socket_handler.state( ) != e_peer_state_waiting_for_get
          && socket_handler.state( ) != e_peer_state_waiting_for_get_or_put )
            throw runtime_error( "invalid state for file_get" );

         fetch_file( filename, socket );
         increment_peer_files_downloaded( file_bytes( filename ) );

         socket_handler.state( ) = e_peer_state_waiting_for_put;
      }
      else if( command == c_cmd_peer_session_file_put )
      {
         string filename( get_parm_val( parameters, c_cmd_parm_peer_session_file_put_filename ) );

         if( socket_handler.state( ) != e_peer_state_waiting_for_put
          && socket_handler.state( ) != e_peer_state_waiting_for_get_or_put )
            throw runtime_error( "invalid state for file_put" );

         store_file( filename, socket );
         increment_peer_files_uploaded( file_bytes( filename ) );

         socket_handler.state( ) = e_peer_state_waiting_for_get;
      }
      else if( command == c_cmd_peer_session_starttls )
      {
         if( socket_handler.state( ) != e_peer_state_listener )
            throw runtime_error( "invalid state for starttls" );

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
      else if( command == c_cmd_peer_session_quit )
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
}

class socket_command_processor : public command_processor
{
   public:
   socket_command_processor( tcp_socket& socket, command_handler& handler, bool is_local )
    : command_processor( handler ),
    socket( socket ),
    handler( handler ),
    is_local( is_local ),
    is_first_command( true )
   {
   }

   private:
   tcp_socket& socket;
   command_handler& handler;

   bool is_local;
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
      TRACE_LOG( TRACE_SESSIONS, "started session" );
   }

   while( true )
   {
      if( socket.read_line( request, c_request_timeout, c_max_line_length ) <= 0 )
      {
         if( !is_captured_session( )
          && ( is_condemned_session( ) || g_server_shutdown || !socket.had_timeout( ) ) )
         {
            // NOTE: If the session is not captured and it has either been condemned or
            // the server is shutting down, or its socket has died then force a "quit".
            request = "quit";
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
            request = "quit";
            break;
         }
      }
      else
         break;
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

   socket.write_line( "commands:" );
   socket.write_line( "=========" );

   socket.write_line( get_usage_for_commands( wildcard_match_expr ) );

   if( !socket.set_no_delay( ) )
      issue_warning( "socket set_no_delay failure" );
   socket.write_line( c_response_okay );
}

}

#ifdef SSL_SUPPORT
peer_session::peer_session( bool acceptor, auto_ptr< ssl_socket >& ap_socket, const string& ip_addr )
#else
peer_session::peer_session( bool acceptor, auto_ptr< tcp_socket >& ap_socket, const string& ip_addr )
#endif
 :
 is_local( false ),
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
      socket_command_handler cmd_handler( *ap_socket, acceptor ? e_peer_state_listener : e_peer_state_initiator );

      cmd_handler.add_commands( 0,
       peer_session_command_functor_factory, ARRAY_PTR_AND_SIZE( peer_session_command_definitions ) );

      cmd_handler.set_sess_id( init_session( cmd_handler, true ) );

      ap_socket->write_line( string( c_protocol_version ) + '\n' + string( c_response_okay ) );

      socket_command_processor processor( *ap_socket, cmd_handler, is_local );
      processor.process_commands( );

      ap_socket->close( );

      term_session( );
   }
   catch( exception& x )
   {
      issue_error( x.what( ) );

      ap_socket->write_line( string( c_response_error_prefix ) + x.what( ) );
      ap_socket->close( );
   }
   catch( ... )
   {
      issue_error( "unexpected unknown exception occurred" );

      ap_socket->write_line( string( c_response_error_prefix ) + "unexpected exception occurred" );
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
   ++g_active_sessions;
}

void peer_session::decrement_session_count( )
{
   guard g( g_mutex );
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
            if( !g_server_shutdown && *ap_socket && get_is_accepted_ip_addr( address.get_addr_string( ) ) )
            {
               peer_session* p_session = new peer_session( true, ap_socket, address.get_addr_string( ) );
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

