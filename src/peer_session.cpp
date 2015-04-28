// Copyright (c) 2014-2015 CIYAM Developers
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
#include "ciyam_core_files.h"
#include "command_processor.h"

using namespace std;

extern size_t g_active_sessions;
extern volatile sig_atomic_t g_server_shutdown;

namespace
{

#include "ciyam_constants.h"

#include "peer_session.cmh"

#include "trace_progress.cpp"

mutex g_mutex;

const char* const c_hello = "hello";

const int c_accept_timeout = 250;
const int c_max_line_length = 500;

const int c_minimum_port_num = 1000;

const int c_min_block_wait_passes = 5;
const int c_max_block_wait_passes = 10;

const size_t c_request_timeout = 5000;
const size_t c_greeting_timeout = 10000;

const size_t c_request_throttle_sleep_time = 250;

enum peer_state
{
   e_peer_state_invalid,
   e_peer_state_initiator,
   e_peer_state_responder,
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
   TRACE_LOG( TRACE_ANYTHING, string( "peer session error: " ) + message );
}

inline void issue_warning( const string& message )
{
   TRACE_LOG( TRACE_SESSIONS, string( "peer session warning: " ) + message );
}

string mint_new_block( const string& blockchain, new_block_info& new_block )
{
   string data;

   string password( get_system_variable( "@" + blockchain + "_pwd" ) );

   if( !password.empty( ) )
      data = construct_new_block( blockchain, password, &new_block );

   return data;
}

string store_new_block( const string& blockchain, const string& data )
{
   guard g( g_mutex );

   string hash;

   vector< pair< string, string > > extras;

   verify_core_file( data, true, &extras );
   create_raw_file_with_extras( "", extras );

   hash = construct_blockchain_info_file( blockchain );

   return hash;
}

void process_file( const string& hash, const string& blockchain )
{
   guard g( g_mutex );

   string::size_type pos = hash.find( ':' );

   string file_info( file_type_info( hash.substr( 0, pos ) ) );

   vector< string > info_parts;
   split( file_info, info_parts, ' ' );

   // NOTE: A core file will return three parts in the form: <type> <hash> <core_type>
   // (as non-core files don't have a "core type" only two parts will be found for them).
   if( info_parts.size( ) == 3 && hash.substr( 0, pos )
    != get_session_variable( get_special_var_name( e_special_var_blockchain_info_hash ) ) )
   {
      string core_type( info_parts[ 2 ] );

      if( is_block( core_type ) )
      {
         if( pos != string::npos )
         {
            vector< pair< string, string > > extras;

            try
            {
               verify_core_file( construct_blob_for_block_content(
                extract_file( hash.substr( 0, pos ), "" ), hash.substr( pos + 1 ) ), true, &extras );

               create_raw_file_with_extras( "", extras );

               construct_blockchain_info_file( blockchain );
            }
            catch( ... )
            {
               delete_file( hash.substr( 0, pos ) );
               throw;
            }
         }
      }
      else if( is_checkpoint_info( core_type ) )
      {
         try
         {
            string content( extract_file( hash.substr( 0, pos ), "" ) );

            checkpoint_info cp_info;
            get_checkpoint_info( blockchain, content, cp_info );

            if( !has_file( cp_info.checkpoint_hash ) )
            {
               // NOTE: Fetch any blocks that have not already been stored locally.
               for( size_t i = 0; i < cp_info.block_hashes_with_sigs.size( ); i++ )
               {
                  string hash_with_sig( cp_info.block_hashes_with_sigs[ i ] );

                  string::size_type pos = hash_with_sig.find( ':' );

                  if( !has_file( hash_with_sig.substr( 0, pos ) ) )
                     add_peer_file_hash_for_get( hash_with_sig );
               }

               // NOTE: Because the checkpoint won't be created locally until blocks
               // after its height have been processed the checkpoint itself must be
               // stored to avoid the peer simply getting stuck at the height of the
               // checkpoint.
               add_peer_file_hash_for_get( cp_info.checkpoint_hash );
            }

            delete_file( hash.substr( 0, pos ), false );
         }
         catch( ... )
         {
            delete_file( hash.substr( 0, pos ), false );
            throw;
         }
      }
      else if( is_blockchain_info( core_type ) )
      {
         try
         {
            string content( extract_file( hash.substr( 0, pos ), "" ) );

            blockchain_info bc_info;
            get_blockchain_info( content, bc_info );

            bool needs_checkpoint = false;

            for( size_t i = 0; i < bc_info.checkpoint_info.size( ); i++ )
            {
               string next_info( bc_info.checkpoint_info[ i ] );

               string::size_type pos = next_info.find( '.' );
               if( pos == string::npos )
                  throw runtime_error( "invalid checkpoint information: " + next_info );

               if( !has_file( next_info.substr( 0, pos ) ) )
               {
                  add_peer_file_hash_for_get( next_info.substr( pos + 1 ) );
                  needs_checkpoint = true;
                  break;
               }
            }

            if( needs_checkpoint )
               set_session_variable(
                get_special_var_name( e_special_var_peer_is_synchronising ), "1" );
            else
            {
               set_session_variable(
                get_special_var_name( e_special_var_peer_is_synchronising ), "" );

               set_session_variable(
                get_special_var_name( e_special_var_blockchain_info_hash ), hash.substr( 0, pos ) );

               // NOTE: Fetch any blocks that have not already been stored locally.
               for( size_t i = 0; i < bc_info.block_hashes_with_sigs.size( ); i++ )
               {
                  string hash_with_sig( bc_info.block_hashes_with_sigs[ i ] );

                  string::size_type pos = hash_with_sig.find( ':' );

                  if( !has_file( hash_with_sig.substr( 0, pos ) ) )
                     add_peer_file_hash_for_get( hash_with_sig );
               }
            }

            delete_file( hash.substr( 0, pos ), false );
         }
         catch( ... )
         {
            delete_file( hash.substr( 0, pos ), false );
            throw;
         }
      }
   }
}

class socket_command_handler : public command_handler
{
   public:
#ifdef SSL_SUPPORT
   socket_command_handler( ssl_socket& socket, peer_state session_state, bool is_local, const string& blockchain )
#else
   socket_command_handler( tcp_socket& socket, peer_state session_state, bool is_local, const string& blockchain )
#endif
    :
    socket( socket ),
    is_local( is_local ),
    blockchain( blockchain ),
    session_state( session_state ),
    session_trust_level( e_peer_trust_level_none )
   {
      had_usage = false;

      needs_blockchain_info = !blockchain.empty( );
      is_responder = ( session_state == e_peer_state_responder );

      last_issued_was_put = !is_responder;
   }

   ~socket_command_handler( )
   {
      if( !blockchain_info.second.empty( ) )
         file_remove( blockchain_info.second );
   }

#ifdef SSL_SUPPORT
   ssl_socket& get_socket( ) { return socket; }
#else
   tcp_socket& get_socket( ) { return socket; }
#endif

   const string& get_last_command( ) { return last_command; }
   const string& get_next_command( ) { return next_command; }

   bool get_is_local( ) const { return is_local; }
   bool get_is_responder( ) const { return is_responder; }

   bool get_last_issued_was_put( ) const { return last_issued_was_put; }
   void set_last_issued_was_put( bool val ) { last_issued_was_put = val; }

   bool get_needs_blockchain_info( ) const { return needs_blockchain_info; }
   void set_needs_blockchain_info( bool val ) { needs_blockchain_info = val; }

   const string& get_blockchain( ) const { return blockchain; }

   pair< string, string >& get_blockchain_info( ) { return blockchain_info; }

   string& prior_put( ) { return prior_put_hash; }

   void get_hello( );
   void put_hello( );

   void get_file( const string& hash );
   void put_file( const string& hash );

   void pip_peer( const string& ip_address );

   void chk_file( const string& hash, string* p_response = 0 );

   void issue_cmd_for_peer( );

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
   bool is_responder;

   bool last_issued_was_put;

   bool needs_blockchain_info;

   string blockchain;
   pair< string, string > blockchain_info;

   string last_command;
   string next_command;

   string prior_put_hash;

   peer_state session_state;
   peer_trust_level session_trust_level;
};

void socket_command_handler::get_hello( )
{
   last_issued_was_put = false;

   progress* p_progress = 0;
   trace_progress progress( TRACE_SOCK_OPS );

   if( get_trace_flags( ) & TRACE_SOCK_OPS )
      p_progress = &progress;

   string data( c_file_type_str_blob );
   data += string( c_hello );

   string temp_hash( sha256( data ).get_digest_as_string( ) );

   if( !has_file( temp_hash ) )
      create_raw_file( data );

   string temp_file_name( "~" + uuid( ).as_string( ) );
   socket.write_line( string( c_cmd_peer_session_get ) + " " + temp_hash, c_request_timeout, p_progress );

   try
   {
      store_temp_file( temp_file_name, socket );

      if( !temp_file_is_identical( temp_file_name, temp_hash ) )
      {
         file_remove( temp_file_name );
         throw runtime_error( "invalid get_hello" );
      }

      file_remove( temp_file_name );
   }
   catch( ... )
   {
      file_remove( temp_file_name );
      throw;
   }
}

void socket_command_handler::put_hello( )
{
   last_issued_was_put = true;

   progress* p_progress = 0;
   trace_progress progress( TRACE_SOCK_OPS );

   if( get_trace_flags( ) & TRACE_SOCK_OPS )
      p_progress = &progress;

   string data( c_file_type_str_blob );
   data += string( c_hello );

   string temp_hash( sha256( data ).get_digest_as_string( ) );

   if( !has_file( temp_hash ) )
      create_raw_file( data );

   socket.write_line( string( c_cmd_peer_session_put ) + " " + temp_hash, c_request_timeout, p_progress );

   fetch_file( temp_hash, socket );
}

void socket_command_handler::get_file( const string& hash )
{
   last_issued_was_put = false;

   progress* p_progress = 0;
   trace_progress progress( TRACE_SOCK_OPS );

   if( get_trace_flags( ) & TRACE_SOCK_OPS )
      p_progress = &progress;

   string::size_type pos = hash.find( ':' );

   socket.write_line( string( c_cmd_peer_session_get )
    + " " + hash.substr( 0, pos ), c_request_timeout, p_progress );

   store_file( hash.substr( 0, pos ), socket );
}

void socket_command_handler::put_file( const string& hash )
{
   last_issued_was_put = true;

   progress* p_progress = 0;
   trace_progress progress( TRACE_SOCK_OPS );

   if( get_trace_flags( ) & TRACE_SOCK_OPS )
      p_progress = &progress;

   socket.write_line( string( c_cmd_peer_session_put ) + " " + hash, c_request_timeout, p_progress );

   fetch_file( hash, socket );
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

void socket_command_handler::chk_file( const string& hash_or_tag, string* p_response )
{
   progress* p_progress = 0;
   trace_progress progress( TRACE_SOCK_OPS );

   if( get_trace_flags( ) & TRACE_SOCK_OPS )
      p_progress = &progress;

   string expected;

   if( p_response )
      socket.write_line( string( c_cmd_peer_session_chk )
       + " " + hash_or_tag, c_request_timeout, p_progress );
   else
   {
      string token( uuid( ).as_string( ) );

      expected = hash_with_nonce( hash_or_tag, token );

      socket.write_line( string( c_cmd_peer_session_chk )
       + " " + hash_or_tag + " " + token, c_request_timeout, p_progress );
   }

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

   if( response == string( c_response_not_found ) )
      response.erase( );

   if( p_response )
      *p_response = response;
   else if( response != expected )
      throw runtime_error( "unexpected invalid chk response: " + response );
}

void socket_command_handler::issue_cmd_for_peer( )
{
   // NOTE: If a prior put no longer exists locally then it is to be expected
   // it would not exist in the peer either.
   if( !prior_put( ).empty( ) && !has_file( prior_put( ) ) )
      prior_put( ).erase( );

   if( get_needs_blockchain_info( ) )
   {
      string blockchain_info_hash;
      chk_file( "c" + blockchain + ".info", &blockchain_info_hash );

      if( !blockchain_info_hash.empty( ) )
      {
         set_needs_blockchain_info( false );

         if( !has_file( blockchain_info_hash ) )
            add_peer_file_hash_for_get( blockchain_info_hash );
      }
   }
   // KLUDGE: For now just randomly perform a "chk", "pip" or a "get" (this should instead be
   // based upon the actual needs of the peer).
   else if( !prior_put( ).empty( ) && rand( ) % 5 == 0 )
      chk_file( prior_put( ) );
   else if( rand( ) % 5 == 0 )
      pip_peer( "127.0.0.1" );
   else if( get_last_issued_was_put( ) )
   {
      string next_hash( top_next_peer_file_hash_to_get( ) );

      while( !next_hash.empty( ) && has_file( next_hash.substr( 0, next_hash.find( ':' ) ) ) )
      {
         pop_next_peer_file_hash_to_get( );
         next_hash = top_next_peer_file_hash_to_get( );
      }   

      if( !next_hash.empty( ) )
      {
         get_file( next_hash );
         pop_next_peer_file_hash_to_get( );

         process_file( next_hash, blockchain );
      }
      else
      {
         get_hello( );

         if( !blockchain.empty( ) )
            set_needs_blockchain_info( true );
      }
   }
   else
   {
      string next_hash( top_next_peer_file_hash_to_put( ) );

      bool had_hash = !next_hash.empty( );

      if( next_hash.empty( ) || !has_file( next_hash ) )
         put_hello( );
      else
         put_file( next_hash );

      if( had_hash )
         pop_next_peer_file_hash_to_put( );
   }
}

string socket_command_handler::preprocess_command_and_args( const string& cmd_and_args )
{
   string str( cmd_and_args );

   if( session_state == e_peer_state_invalid )
      str = c_cmd_peer_session_bye;

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
      TRACE_LOG( TRACE_SESSIONS, get_blockchain( ).empty( )
       ? "finished peer session" : "finished peer session for blockchain " + get_blockchain( ) );
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

   if( !is_special && is_responder )
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

   string blockchain( socket_handler.get_blockchain( ) );

   try
   {
      ostringstream osstr;

      if( command == c_cmd_peer_session_chk )
      {
         string tag_or_hash( get_parm_val( parameters, c_cmd_parm_peer_session_chk_tag_or_hash ) );
         string nonce( get_parm_val( parameters, c_cmd_parm_peer_session_chk_nonce ) );

         if( socket_handler.state( ) != e_peer_state_responder
          && socket_handler.state( ) != e_peer_state_waiting_for_get
          && socket_handler.state( ) != e_peer_state_waiting_for_put )
            throw runtime_error( "invalid state for chk" );

         string hash( tag_or_hash );

         if( has_tag( tag_or_hash ) )
            response = hash = tag_file_hash( tag_or_hash );

         bool has = has_file( hash, false );
         bool was_initial_state = ( socket_handler.state( ) == e_peer_state_responder );

         if( !has )
         {
            response = c_response_not_found;

            if( was_initial_state )
            {
               if( !blockchain.empty( ) )
                  socket_handler.state( ) = e_peer_state_invalid;
               else
               {
                  handler.issue_command_reponse( response, true );

                  // NOTE: For the initial file transfer just a dummy "hello" blob.
                  string data( c_file_type_str_blob );
                  data += string( c_hello );

                  string temp_hash( sha256( data ).get_digest_as_string( ) );

                  if( !has_file( temp_hash ) )
                     create_raw_file( data );

                  handler.issue_command_reponse( "put " + temp_hash, true );
                  fetch_file( temp_hash, socket );

                  string temp_file_name( "~" + uuid( ).as_string( ) );

                  try
                  {
                     store_temp_file( temp_file_name, socket );

                     response.erase( );

                     if( !temp_file_is_identical( temp_file_name, temp_hash ) )
                        socket_handler.state( ) = e_peer_state_invalid;
                     else
                     {
                        socket_handler.state( ) = e_peer_state_waiting_for_put;
                        socket_handler.trust_level( ) = e_peer_trust_level_normal;
                     }

                     increment_peer_files_uploaded( data.length( ) );
                     increment_peer_files_downloaded( data.length( ) );

                     file_remove( temp_file_name );
                  }
                  catch( ... )
                  {
                     file_remove( temp_file_name );
                     throw;
                  }
               }
            }
         }
         else
         {
            if( !nonce.empty( ) )
               response = hash_with_nonce( hash, nonce );

            if( was_initial_state )
            {
               socket_handler.state( ) = e_peer_state_waiting_for_get;

               if( !blockchain.empty( ) )
               {
                  string all_tags( get_hash_tags( hash ) );

                  set< string > tags;
                  split( all_tags, tags, '\n' );

                  string tag( "c" + blockchain + ".head" );

                  if( !tags.count( tag ) )
                     throw runtime_error( "blockchain " + blockchain + " was not found" );
               }
            }
         }

         if( has && !blockchain.empty( ) && tag_or_hash == "c" + blockchain + ".info" )
         {
            if( !socket_handler.get_blockchain_info( ).second.empty( ) )
               file_remove( socket_handler.get_blockchain_info( ).second );

            socket_handler.get_blockchain_info( ).first = hash;
            socket_handler.get_blockchain_info( ).second = "~" + uuid( ).as_string( );

            copy_raw_file( hash, socket_handler.get_blockchain_info( ).second );
         }

         if( !was_initial_state && socket_handler.get_is_responder( ) )
         {
            handler.issue_command_reponse( response, true );
            response.erase( );

            socket_handler.issue_cmd_for_peer( );
         }
      }
      else if( command == c_cmd_peer_session_get )
      {
         string tag_or_hash( get_parm_val( parameters, c_cmd_parm_peer_session_get_tag_or_hash ) );

         if( socket_handler.state( ) != e_peer_state_waiting_for_get )
            throw runtime_error( "invalid state for get" );

         string hash( tag_or_hash );

         if( has_tag( tag_or_hash ) )
            hash = tag_file_hash( tag_or_hash );

         if( hash != socket_handler.get_blockchain_info( ).first )
         {
            fetch_file( hash, socket );
            increment_peer_files_uploaded( file_bytes( hash ) );
         }
         else
         {
            fetch_temp_file( socket_handler.get_blockchain_info( ).second, socket );
            increment_peer_files_uploaded( file_size( socket_handler.get_blockchain_info( ).second ) );

            file_remove( socket_handler.get_blockchain_info( ).second );

            socket_handler.get_blockchain_info( ).first.erase( );
            socket_handler.get_blockchain_info( ).second.erase( );
         }

         socket_handler.state( ) = e_peer_state_waiting_for_put;

         if( socket_handler.get_is_responder( ) )
         {
            handler.issue_command_reponse( response, true );
            response.erase( );

            socket_handler.issue_cmd_for_peer( );
         }
      }
      else if( command == c_cmd_peer_session_put )
      {
         string hash( get_parm_val( parameters, c_cmd_parm_peer_session_put_hash ) );

         if( socket_handler.state( ) != e_peer_state_waiting_for_put )
            throw runtime_error( "invalid state for put" );

         if( !has_file( hash ) )
         {
            store_file( hash, socket );
            increment_peer_files_downloaded( file_bytes( hash ) );
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

         if( socket_handler.get_is_responder( ) )
         {
            handler.issue_command_reponse( response, true );
            response.erase( );

            socket_handler.issue_cmd_for_peer( );
         }
      }
      else if( command == c_cmd_peer_session_pip )
      {
         string addr( get_parm_val( parameters, c_cmd_parm_peer_session_pip_addr ) );

         response = "127.0.0.1"; // KLUDGE: This should return an actual peer IP address.

         if( socket_handler.state( ) != e_peer_state_waiting_for_get
          && socket_handler.state( ) != e_peer_state_waiting_for_put )
            throw runtime_error( "invalid state for pip" );

         if( socket_handler.get_is_responder( ) )
         {
            handler.issue_command_reponse( response, true );
            response.erase( );

            socket_handler.issue_cmd_for_peer( );
         }
      }
      else if( command == c_cmd_peer_session_tls )
      {
         if( socket_handler.state( ) != e_peer_state_responder )
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
   else if( !socket_handler.get_is_responder( )
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

      socket_handler.issue_cmd_for_peer( );
   }
}

class socket_command_processor : public command_processor
{
   public:
   socket_command_processor( tcp_socket& socket,
    command_handler& handler, bool is_local, bool is_responder )
    : command_processor( handler ),
    socket( socket ),
    handler( handler ),
    new_block_wait( 0 ),
    is_local( is_local ),
    is_responder( is_responder ),
    needs_blockchain_info( false )
   {
      peer_special_variable = get_special_var_name( e_special_var_peer );
      initiator_special_variable = get_special_var_name( e_special_var_peer_initiator );
      responder_special_variable = get_special_var_name( e_special_var_peer_responder );

      string value( "1" );

      socket_command_handler& socket_handler = dynamic_cast< socket_command_handler& >( handler );

      if( !socket_handler.get_blockchain( ).empty( ) )
      {
         needs_blockchain_info = true;
         value = socket_handler.get_blockchain( );
      }

      set_session_variable( peer_special_variable, value );

      if( !is_responder )
         set_session_variable( initiator_special_variable, value );
      else
         set_session_variable( responder_special_variable, value );
   }

   private:
   tcp_socket& socket;
   command_handler& handler;

   string peer_special_variable;
   string initiator_special_variable;
   string responder_special_variable;

   int new_block_wait;
   string new_block_data;
   new_block_info new_block;

   bool is_local;
   bool is_responder;

   bool needs_blockchain_info;

   bool is_still_processing( ) { return is_captured_session( ) || socket.okay( ); }

   string get_cmd_and_args( );

   void output_command_usage( const string& wildcard_match_expr ) const;
};

string socket_command_processor::get_cmd_and_args( )
{
   string request;

   socket_command_handler& socket_handler = dynamic_cast< socket_command_handler& >( handler );

   string blockchain( socket_handler.get_blockchain( ) );

   while( true )
   {
      progress* p_progress = 0;
      trace_progress progress( TRACE_SOCK_OPS );

      if( get_trace_flags( ) & TRACE_SOCK_OPS )
         p_progress = &progress;

      if( !g_server_shutdown && !is_condemned_session( ) )
      {
         // NOTE: If a new better block has already been processed then build on it rather than wait.
         if( !new_block_data.empty( ) && has_better_block( blockchain, new_block.height, new_block.weight ) )
            new_block_data.erase( );

         if( !new_block_data.empty( ) )
         {
            if( !new_block.can_mint )
               new_block_data.erase( );
            else
            {
               if( new_block_wait > 0 )
                  --new_block_wait;
               else
               {
                  if( !has_better_block( blockchain, new_block.height, new_block.weight )
                   && !any_has_session_variable( get_special_var_name( e_special_var_peer_is_synchronising ) ) )
                     store_new_block( blockchain, new_block_data );

                  new_block_data.erase( );
               }
            }
         }
         else if( !blockchain.empty( )
          && is_first_using_session_variable( peer_special_variable, blockchain )
          && !any_has_session_variable( get_special_var_name( e_special_var_peer_is_synchronising ) ) )
         {
            new_block_data = mint_new_block( blockchain, new_block );
            new_block_wait = ( new_block.is_optimal ? c_min_block_wait_passes : c_max_block_wait_passes );
         }
      }

      if( !is_responder && !g_server_shutdown && !is_condemned_session( ) )
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

            socket_handler.issue_cmd_for_peer( );
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

         if( !is_local || !socket.had_timeout( ) )
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

   return request;
}

void socket_command_processor::output_command_usage( const string& wildcard_match_expr ) const
{
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
peer_session* construct_session( bool responder, auto_ptr< ssl_socket >& ap_socket, const string& ip_addr )
#else
peer_session* construct_session( bool responder, auto_ptr< tcp_socket >& ap_socket, const string& ip_addr )
#endif
{
   guard g( g_mutex );
   peer_session* p_session = 0;

   string::size_type pos = ip_addr.find( '=' );

   if( ip_addr.substr( 0, pos ) == "127.0.0.1" || !has_session_with_ip_addr( ip_addr.substr( 0, pos ) ) )
      p_session = new peer_session( responder, ap_socket, ip_addr );

   return p_session;
}

}

#ifdef SSL_SUPPORT
peer_session::peer_session( bool responder, auto_ptr< ssl_socket >& ap_socket, const string& ip_addr )
#else
peer_session::peer_session( bool responder, auto_ptr< tcp_socket >& ap_socket, const string& ip_addr )
#endif
 :
 is_local( false ),
 ip_addr( ip_addr ),
 responder( responder ),
 ap_socket( ap_socket )
{
   if( !( *this->ap_socket ) )
      throw runtime_error( "invalid socket..." );

   string::size_type pos = ip_addr.find( '=' );
   if( pos != string::npos )
   {
      blockchain = ip_addr.substr( pos + 1 );
      this->ip_addr.erase( pos );
   }

   if( this->ip_addr == "127.0.0.1" )
      is_local = true;

   increment_session_count( );
}

peer_session::~peer_session( )
{
   decrement_session_count( );
}

void peer_session::on_start( )
{
   try
   {
      socket_command_handler cmd_handler( *ap_socket,
       responder ? e_peer_state_responder : e_peer_state_initiator, is_local, blockchain );

      cmd_handler.add_commands( 0,
       peer_session_command_functor_factory, ARRAY_PTR_AND_SIZE( peer_session_command_definitions ) );

      if( responder )
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
      }

      init_session( cmd_handler, true, &ip_addr, &blockchain );

      TRACE_LOG( TRACE_SESSIONS,
       string( "started peer session " )
       + ( !responder ? "(as initiator)" : "(as responder)" )
       + ( blockchain.empty( ) ? "" : " for blockchain " + blockchain ) );

      bool okay = true;

      if( !responder )
      {
         progress* p_progress = 0;
         trace_progress progress( TRACE_SOCK_OPS );

         if( get_trace_flags( ) & TRACE_SOCK_OPS )
            p_progress = &progress;

         string hash_or_tag;

         if( !blockchain.empty( ) )
            hash_or_tag = string( "c" + blockchain + ".head" );

         if( hash_or_tag.empty( ) )
         {
            string data( c_file_type_str_blob );
            data += string( c_hello );

            hash_or_tag = sha256( data ).get_digest_as_string( );
         }

         ap_socket->write_line( string( c_cmd_peer_session_chk ) + " " + hash_or_tag, c_request_timeout, p_progress );

         if( !blockchain.empty( ) )
         {
            string blockchain_head_hash;

            if( ap_socket->read_line( blockchain_head_hash, c_request_timeout, c_max_line_length, p_progress ) <= 0 )
               okay = false;
         }

         cmd_handler.state( ) = e_peer_state_waiting_for_put;
      }

      if( okay )
      {
         socket_command_processor processor( *ap_socket, cmd_handler, is_local, responder );
         processor.process_commands( );
      }
      else
      {
         TRACE_LOG( TRACE_SESSIONS, blockchain.empty( )
          ? "finished peer session" : "finished peer session for blockchain " + blockchain );
      }

      ap_socket->close( );

      term_session( );
   }
   catch( exception& x )
   {
      issue_error( x.what( ) );

      ap_socket->write_line( string( c_response_error_prefix ) + x.what( ), c_request_timeout );
      ap_socket->close( );

      term_session( );
   }
   catch( ... )
   {
      issue_error( "unexpected unknown exception occurred" );

      ap_socket->write_line( string( c_response_error_prefix ) + "unexpected exception occurred", c_request_timeout );
      ap_socket->close( );

      term_session( );
   }

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
   tcp_socket s;

   try
   {
      bool okay = s.open( );

      ip_address address( port );
      if( okay )
      {
         s.set_reuse_addr( );

         okay = s.bind( address );
         if( okay )
         {
            s.listen( );

            TRACE_LOG( TRACE_ANYTHING,
             "peer listener started on port " + to_string( port )
             + ( blockchain.empty( ) ? "" : " for blockchain " + blockchain ) );

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
                  peer_session* p_session = construct_session(
                   true, ap_socket, address.get_addr_string( ) + '=' + blockchain );

                  if( p_session )
                     p_session->start( );
               }
            }

            s.close( );
         }
         else
            s.close( );
      }
   }
   catch( exception& x )
   {
      issue_error( x.what( ) );

      s.close( );
   }
   catch( ... )
   {
      issue_error( "unexpected unknown exception occurred" );

      s.close( );
   }

   TRACE_LOG( TRACE_SESSIONS,
    "peer listener finished (port " + to_string( port ) + ")"
    + ( blockchain.empty( ) ? "" : " for blockchain " + blockchain ) );

   delete this;
}

void create_peer_listener( int port, const string& blockchain )
{
   if( port < c_minimum_port_num )
      throw runtime_error( "invalid port " + to_string( port ) + " for peer listener" );

   if( !blockchain.empty( ) )
      register_blockchain( port, blockchain );

   peer_listener* p_peer_listener = new peer_listener( port, blockchain );
   p_peer_listener->start( );
}

void create_peer_initiator( int port, const string& ip_addr, const string& blockchain )
{
   if( port < c_minimum_port_num )
      throw runtime_error( "invalid port " + to_string( port ) + " for peer initiator" );

   if( !blockchain.empty( ) )
      register_blockchain( port, blockchain );

   if( !get_is_accepted_peer_id_addr( ip_addr ) )
      throw runtime_error( "ip address " + ip_addr + " is not permitted" );

   if( g_server_shutdown || has_max_peers( ) )
      throw runtime_error( "server is shutting down or has reached its maximum peer limit" );

#ifdef SSL_SUPPORT
   auto_ptr< ssl_socket > ap_socket( new ssl_socket );
#else
   auto_ptr< tcp_socket > ap_socket( new tcp_socket );
#endif

   if( ap_socket->open( ) )
   {
      ip_address address( ip_addr.c_str( ), port );

      if( ap_socket->connect( address ) )
      {
         peer_session* p_session = construct_session( false, ap_socket,
          address.get_addr_string( ) + "=" + ( !blockchain.empty( ) ? blockchain : get_blockchain_for_port( port ) ) );

         if( p_session )
            p_session->start( );
      }
   }
}

void create_initial_peer_sessions( )
{
   map< string, string > initial_ips;
   get_initial_peer_ips( initial_ips );

   for( map< string, string >::iterator i = initial_ips.begin( ); i!= initial_ips.end( ); ++i )
   {
      string ip_addr( i->first );

      int port = get_blockchain_port( i->second );

      if( !get_is_accepted_peer_id_addr( ip_addr ) )
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
         ip_address address( ip_addr.c_str( ), port );

         if( ap_socket->connect( address ) )
         {
            peer_session* p_session = construct_session( false,
             ap_socket, address.get_addr_string( ) + "=" + i->second );

            if( p_session )
               p_session->start( );
         }
      }
   }
}

