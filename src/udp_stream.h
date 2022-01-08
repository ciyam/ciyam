// Copyright (c) 2022 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef UDP_STREAM_H
#  define UDP_STREAM_H

#  include "macros.h"
#  include "threads.h"

#  ifdef CIYAM_BASE_IMPL
#     define CIYAM_BASE_DECL_SPEC DYNAMIC_EXPORT
#  else
#     define CIYAM_BASE_DECL_SPEC DYNAMIC_IMPORT
#  endif

class CIYAM_BASE_DECL_SPEC udp_stream_session : public thread
{
   public:
   udp_stream_session( int port, int sock );
   ~udp_stream_session( );

   void on_start( );

   private:
   int port;
   int sock;
};

extern "C" void CIYAM_BASE_DECL_SPEC init_udp_stream( int port, int sock );

typedef void ( *fp_init_udp_stream )( int, int );

#endif
