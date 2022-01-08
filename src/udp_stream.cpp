// Copyright (c) 2022 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <csignal>
#  include <memory>
#  include <string>
#  include <iostream>
#  include <stdexcept>
#endif

#define CIYAM_BASE_IMPL

#include "udp_stream.h"

#include "utilities.h"
#include "date_time.h"
#include "ciyam_base.h"
#include "ciyam_session.h"

//#define DEBUG

using namespace std;

extern volatile sig_atomic_t g_server_shutdown;

namespace
{

#include "ciyam_constants.h"

const size_t c_sleep_time = 1000; // i.e. 1 sec

}

udp_stream_session::udp_stream_session( int port, int sock )
 :
 port( port ),
 sock( sock )
{
   ciyam_session::increment_session_count( );
}

udp_stream_session::~udp_stream_session( )
{
   ciyam_session::decrement_session_count( );
}

void udp_stream_session::on_start( )
{
#ifdef DEBUG
   cout << "started udp_stream session..." << endl;
#endif
   try
   {
      TRACE_LOG( TRACE_SESSIONS,
       "started udp_stream session (tid = " + to_string( current_thread_id( ) ) + ")" );

      while( true )
      {
         msleep( c_sleep_time );

         if( g_server_shutdown )
            break;
      }

      TRACE_LOG( TRACE_SESSIONS, "finished udp_stream session" );
   }
   catch( exception& x )
   {
#ifdef DEBUG
      cout << "udp_stream error: " << x.what( ) << endl;
#endif
      TRACE_LOG( TRACE_ANYTHING, string( "udp_stream error: " ) + x.what( ) );
   }
   catch( ... )
   {
#ifdef DEBUG
      cout << "unexpected udp_stream exception..." << endl;
#endif
      TRACE_LOG( TRACE_ANYTHING, "udp_stream error: unexpected unknown exception caught" );
   }
#ifdef DEBUG
   cout << "finished udp_stream session..." << endl;
#endif
   delete this;
}

void init_udp_stream( int port, int sock )
{
   udp_stream_session* p_udp_stream_session = new udp_stream_session( port, sock );
   p_udp_stream_session->start( );
}