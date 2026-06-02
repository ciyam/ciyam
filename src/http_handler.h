// Copyright (c) 2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef HTTP_HANDLER_H
#  define HTTP_HANDLER_H

#  include "config.h"
#  include "threads.h"
#  include "sockets.h"
#  ifdef SSL_SUPPORT
#     include "ssl_socket.h"
#  endif

class http_listener : public thread
{
   public:
   http_listener( int port );

   ~http_listener( );

   void on_start( );

   int port;
};

extern "C" void init_http_handler( int port );

typedef void ( *fp_init_http_handler )( int );

#endif
