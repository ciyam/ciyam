// Copyright (c) 2006-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2018 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef CIYAM_SESSION_H
#  define CIYAM_SESSION_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#  endif

#  include "config.h"
#  include "sockets.h"
#  include "threads.h"
#  ifdef SSL_SUPPORT
#     include "ssl_socket.h"
#  endif

class ciyam_session : public thread
{
   public:
#  ifdef SSL_SUPPORT
   ciyam_session( std::auto_ptr< ssl_socket >& ap_socket, const std::string& ip_addr );
#  else
   ciyam_session( std::auto_ptr< tcp_socket >& ap_socket, const std::string& ip_addr );
#  endif

   ~ciyam_session( );

   bool is_own_pid( ) const { return pid_is_self; }

   void on_start( );

   static void increment_session_count( );
   static void decrement_session_count( );

   private:
   bool is_local;
   bool pid_is_self;
#  ifdef SSL_SUPPORT
   std::auto_ptr< ssl_socket > ap_socket;
#  else
   std::auto_ptr< tcp_socket > ap_socket;
#  endif
};

#endif

