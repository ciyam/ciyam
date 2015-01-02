// Copyright (c) 2014-2015 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef PEER_SESSION_H
#  define PEER_SESSION_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#  endif

#  include "config.h"
#  include "sockets.h"
#  include "threads.h"
#  ifdef SSL_SUPPORT
#     include "ssl_socket.h"
#  endif

class peer_session : public thread
{
   public:
#  ifdef SSL_SUPPORT
   peer_session( bool acceptor, std::auto_ptr< ssl_socket >& ap_socket, const std::string& ip_addr );
#  else
   peer_session( bool acceptor, std::auto_ptr< tcp_socket >& ap_socket, const std::string& ip_addr );
#  endif

   ~peer_session( );

   void on_start( );

   static void increment_session_count( );
   static void decrement_session_count( );

   private:
   bool acceptor;
   bool is_local;
   std::string ip_addr;

#  ifdef SSL_SUPPORT
   std::auto_ptr< ssl_socket > ap_socket;
#  else
   std::auto_ptr< tcp_socket > ap_socket;
#  endif
};

class peer_listener : public thread
{
   void on_start( );
};

void create_initial_peer_sessions( );

#endif

