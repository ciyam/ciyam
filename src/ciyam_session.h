// Copyright (c) 2006-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef CIYAM_SESSION_H
#  define CIYAM_SESSION_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#  endif

#  include "config.h"
#  include "macros.h"
#  include "sockets.h"
#  include "threads.h"
#  ifdef SSL_SUPPORT
#     include "ssl_socket.h"
#  endif

class ciyam_session : public thread
{
   public:
#  ifdef SSL_SUPPORT
   ciyam_session( ssl_socket* p_socket, const std::string& ip_addr );
   ciyam_session( std::unique_ptr< ssl_socket >& up_socket, const std::string& ip_addr );
#  else
   ciyam_session( tcp_socket* p_socket, const std::string& ip_addr );
   ciyam_session( std::unique_ptr< tcp_socket >& up_socket, const std::string& ip_addr );
#  endif

   ~ciyam_session( );

   bool is_own_pid( ) const { return pid_is_self; }

   void on_start( );

   static void increment_session_count( );
   static void decrement_session_count( );

   private:
   bool is_local;
   bool using_tls;
   bool pid_is_self;
   bool needs_key_exchange;

   std::string ip_addr;

#  ifdef SSL_SUPPORT
   std::unique_ptr< ssl_socket > up_socket;
#  else
   std::unique_ptr< tcp_socket > up_socket;
#  endif
};

#  ifdef SSL_SUPPORT
extern "C" void init_ciyam_session( ssl_socket* p_socket, const char* p_address );
#  else
extern "C" void init_ciyam_session( tcp_socket* p_socket, const char* p_address );
#  endif

#  ifdef SSL_SUPPORT
typedef void ( *fp_init_ciyam_session )( ssl_socket*, const char* );
#  else
typedef void ( *fp_init_ciyam_session )( tcp_socket*, const char* );
#  endif

#endif
