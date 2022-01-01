// Copyright (c) 2014-2022 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef PEER_SESSION_H
#  define PEER_SESSION_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <iosfwd>
#     include <memory>
#     include <vector>
#     include <string>
#  endif

#  include "config.h"
#  include "macros.h"
#  include "sockets.h"
#  include "threads.h"
#  ifdef SSL_SUPPORT
#     include "ssl_socket.h"
#  endif

#  ifdef CIYAM_BASE_IMPL
#     define CIYAM_BASE_DECL_SPEC DYNAMIC_EXPORT
#  else
#     define CIYAM_BASE_DECL_SPEC DYNAMIC_IMPORT
#  endif

class CIYAM_BASE_DECL_SPEC peer_session : public thread
{
   public:
#  ifdef SSL_SUPPORT
   peer_session( bool is_responder, std::auto_ptr< ssl_socket >& ap_socket, const std::string& ip_addr, bool is_for_support = false );
#  else
   peer_session( bool is_responder, std::auto_ptr< tcp_socket >& ap_socket, const std::string& ip_addr, bool is_for_support = false );
#  endif

   ~peer_session( );

   void on_start( );

   static void increment_session_count( );
   static void decrement_session_count( );

   private:
   bool is_local;
   bool is_owner;
   bool is_responder;
   bool is_for_support;

   bool has_found_both_are_owners;

   std::string port;
   std::string ip_addr;
   std::string blockchain;

#  ifdef SSL_SUPPORT
   std::auto_ptr< ssl_socket > ap_socket;
#  else
   std::auto_ptr< tcp_socket > ap_socket;
#  endif
};

class CIYAM_BASE_DECL_SPEC peer_listener : public thread
{
   public:
   peer_listener( int port ) : port( port ) { }
   peer_listener( int port, const std::string& blockchain ) : port( port ), blockchain( blockchain ) { }

   void on_start( );

   private:
   int port;
   std::string blockchain;
};

void CIYAM_BASE_DECL_SPEC list_mutex_lock_ids_for_peer_session( std::ostream& outs );

std::string CIYAM_BASE_DECL_SPEC use_peer_account(
 const std::string& blockchain, const std::string& password, bool release = false, bool is_pwd_hash = false );

std::string CIYAM_BASE_DECL_SPEC get_account_password( const std::string& blockchain, const std::string& account );

void CIYAM_BASE_DECL_SPEC lock_blockchain_transaction( std::auto_ptr< guard >& ap_guard );

std::string CIYAM_BASE_DECL_SPEC create_blockchain_transaction(
 const std::string& blockchain, const std::string& application,
 const std::string& log_command, const std::vector< std::string >* p_file_info = 0 );

void CIYAM_BASE_DECL_SPEC create_peer_listener( int port, const std::string& blockchain );

void CIYAM_BASE_DECL_SPEC create_peer_initiator( int port,
 const std::string& ip_addr, const std::string& blockchain, bool force = false, size_t num_for_support = 0 );

void CIYAM_BASE_DECL_SPEC create_initial_peer_sessions( );

extern "C" void CIYAM_BASE_DECL_SPEC init_peer_sessions( int start_listeners );

typedef void ( *fp_init_peer_sessions )( int );

#endif
