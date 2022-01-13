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

enum udp_direction
{
   e_udp_direction_recv,
   e_udp_direction_send
};

class CIYAM_BASE_DECL_SPEC udp_stream_session : public thread
{
   public:
   udp_stream_session( int port, int sock, udp_direction direction, size_t stream_num = 0 );
   ~udp_stream_session( );

   void on_start( );

   private:
   int port;
   int sock;

   size_t stream_num;

   udp_direction direction;
};

extern "C" void CIYAM_BASE_DECL_SPEC init_udp_streams( int port, int sock );

typedef void ( *fp_init_udp_streams )( int, int );

#endif
