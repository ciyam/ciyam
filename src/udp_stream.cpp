// Copyright (c) 2022-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <csignal>
#  include <cstring>
#  include <memory>
#  include <string>
#  include <iostream>
#  include <stdexcept>
#endif

#define CIYAM_BASE_IMPL

#include "udp_stream.h"

#include "sockets.h"
#include "progress.h"
#include "utilities.h"
#include "date_time.h"
#include "ciyam_base.h"
#include "ciyam_files.h"
#include "ciyam_session.h"
#include "ciyam_variables.h"

//#define DEBUG

using namespace std;

extern volatile sig_atomic_t g_server_shutdown;

namespace
{

#include "ciyam_constants.h"

#include "trace_progress.cpp"

const size_t c_timeout = 100; // i.e. 1/10 sec
const size_t c_sleep_time = 250; // i.e. 1/4 sec

const size_t c_addr_size = 64;
const size_t c_buffer_size = 1500;

const size_t c_max_buffers = 100;

mutex g_mutex;

}

udp_stream_session::udp_stream_session( int port, int sock, udp_direction direction, size_t stream_num )
 :
 port( port ),
 sock( sock ),
 direction( direction ),
 stream_num( stream_num )
{
   ciyam_session::increment_session_count( );
}

udp_stream_session::~udp_stream_session( )
{
   ciyam_session::decrement_session_count( );
}

int udp_stream_session::recv_from( udp_socket& socket,
 ip_address& address, unsigned char* buffer, size_t buflen, size_t timeout, progress* p_progress )
{
   guard g( g_mutex );

   return socket.recv_from( buffer, buflen, address, timeout, p_progress );
}

int udp_stream_session::send_to( udp_socket& socket,
 ip_address& address, const unsigned char* buffer, size_t buflen, size_t timeout, progress* p_progress )
{
   guard g( g_mutex );

   return socket.send_to( buffer, buflen, address, timeout, p_progress );
}

void udp_stream_session::on_start( )
{
   string stream_name( ( direction == e_udp_direction_recv ) ? "udp recv stream" : "udp send stream" );

   if( stream_num )
      stream_name += ' ' + to_string( stream_num );

#ifdef DEBUG
   cout << "started " << stream_name << " session..." << endl;
#endif

   int port = get_stream_port( );
   int sock = get_stream_sock( );

   auto_ptr< ip_address > ap_addr;
   auto_ptr< udp_socket > ap_sock;

   unsigned char buffer[ c_buffer_size + 1 ];

   vector< pair< ip_address, string > > addr_data_pairs;

   memset( buffer, sizeof( buffer ), '\0' );

   for( size_t i = 0; i < c_max_buffers; i++ )
   {
      string data( c_buffer_size + 1, '\0' );

      addr_data_pairs.push_back( make_pair( ip_address( ), data ) );
   }

   if( direction == e_udp_direction_recv )
   {
      ap_addr.reset( new ip_address( port ) );
      ap_sock.reset( new udp_socket( sock ) );
   }

   try
   {
      TRACE_LOG( TRACE_INITIAL | TRACE_SESSION,
       "started " + stream_name + " session (tid = " + to_string( current_thread_id( ) ) + ")" );

      progress* p_progress = 0;
      trace_progress progress( TRACE_VERBOSE | TRACE_SOCKETS );

      while( true )
      {
         p_progress = 0;

         if( get_trace_flags( ) & ( TRACE_VERBOSE | TRACE_SOCKETS ) )
            p_progress = &progress;

         if( direction == e_udp_direction_recv )
         {
            int len = recv_from( *ap_sock, *ap_addr, buffer, sizeof( buffer ), c_timeout, p_progress );

            if( len > 0 )
            {
               addr_data_pairs[ 0 ].first = *ap_addr;
               memcpy( &addr_data_pairs[ 0 ].second[ 0 ], buffer, len );

               int num = 1;
               for( size_t i = 1; i < c_max_buffers; i++ )
               {
                  len = recv_from( *ap_sock, *ap_addr, buffer, sizeof( buffer ), ( c_timeout / 10 ), p_progress );

                  if( len <= 0 )
                     break;

                  num++;

                  addr_data_pairs[ i ].first = *ap_addr;
                  memcpy( &addr_data_pairs[ i ].second[ 0 ], buffer, len );
               }

               if( g_server_shutdown )
                  break;

               // NOTE: This several millisecond sleep helps to prevent packet loss by
               // giving up some time for the other stream sessions to read datagrams.
               msleep( 3 );

               set< string > existing_files;

               for( size_t i = 0; i < num; i++ )
               {
                  ip_address addr( addr_data_pairs[ i ].first );

                  string ip_addr( addr.get_addr_string( ) );

                  bool is_null = ( ip_addr == c_null_ip_addr || ip_addr == c_null_ip_addr_for_ipv6 );

                  if( is_null || ( ip_addr == c_local_ip_addr_for_ipv6 ) )
                  {
                     ip_addr = c_local_ip_addr;
                     addr = ip_address( c_local_ip_addr, port );
                  }

                  string data( addr_data_pairs[ i ].second.c_str( ) );

                  string::size_type pos = data.find( ':' );

                  if( pos != string::npos && pos > 1 )
                  {
                     size_t slot = from_string< size_t >( data.substr( 1, pos - 1 ) );

                     string sess_ip_addr( session_ip_addr( slot ) );

                     if( is_null || ( ip_addr == sess_ip_addr ) )
                     {
                        data.erase( 0, pos + 1 );

                        if( data.size( ) <= 64 )
                           data += ':' + ip_addr;

                        pos = data.find( ':' );

                        // NOTE: The chunk value should always be between 000 and 999.
                        if( pos == 3 )
                        {
                           size_t chunk = from_string< size_t >( data.substr( 0, pos ) );

                           data.erase( 0, pos + 1 );

                           pos = data.find( ':' );

                           if( pos != string::npos )
                           {
                              string hash( get_hash( data.substr( 0, pos ) ) );

                              // NOTE: If file already exists then send a UDP message to try and stop further packets being sent.
                              if( !hash.empty( ) )
                              {
                                 if( !existing_files.count( hash ) )
                                 {
                                    string slotx( get_session_variable( get_special_var_name( e_special_var_slotx ), slot ) );

                                    string data( slotx + ":XXX:" + hash );

                                    send_to( *ap_sock, addr, ( const unsigned char* )&data[ 0 ], data.length( ), c_timeout, p_progress );

                                    existing_files.insert( hash );
                                 }
                              }
                              else
                                 add_udp_recv_file_chunk_info( slot, chunk, data );
                           }
                        }
                     }
                  }
               }
            }
         }
         else
         {
            //nyi - send implementation to be added here...
            msleep( c_sleep_time );
         }

         if( g_server_shutdown )
            break;
      }

      TRACE_LOG( TRACE_INITIAL | TRACE_SESSION, "finished " + stream_name + " session" );
   }
   catch( exception& x )
   {
#ifdef DEBUG
      cout << "udp_stream error: " << x.what( ) << endl;
#endif
      TRACE_LOG( TRACE_MINIMAL, string( "udp_stream error: " ) + x.what( ) );
   }
   catch( ... )
   {
#ifdef DEBUG
      cout << "unexpected udp_stream exception..." << endl;
#endif
      TRACE_LOG( TRACE_MINIMAL, "udp_stream error: unexpected unknown exception caught" );
   }
#ifdef DEBUG
   cout << "finished " << stream_name << " session..." << endl;
#endif
   delete this;
}

void init_udp_streams( int port, int sock, int* p_num_recv, int* p_num_send, int* p_total_sessions )
{
   int num_recv = get_num_recv_stream_sessions( );
   int num_send = get_num_send_stream_sessions( );

   if( p_num_recv )
      num_recv = *p_num_recv;

   if( p_num_send )
      num_send = *p_num_send;

   if( num_recv < 0 )
      num_recv = 0;

   if( num_send < 0 )
      num_send = 0;

   if( num_recv == 1 )
   {
      udp_stream_session* p_udp_recv_stream_session = new udp_stream_session( port, sock, e_udp_direction_recv );
      p_udp_recv_stream_session->start( );
   }
   else
   {
      for( int i = 0; i < num_recv; i++ )
      {
         udp_stream_session* p_udp_recv_stream_session = new udp_stream_session( port, sock, e_udp_direction_recv, i + 1 );
         p_udp_recv_stream_session->start( );
      }
   }

   if( p_total_sessions )
      *p_total_sessions += num_recv;

   if( num_send == 1 )
   {
      udp_stream_session* p_udp_send_stream_session = new udp_stream_session( port, sock, e_udp_direction_send );
      p_udp_send_stream_session->start( );
   }
   else
   {
      for( int i = 0; i < num_send; i++ )
      {
         udp_stream_session* p_udp_send_stream_session = new udp_stream_session( port, sock, e_udp_direction_send, i + 1 );
         p_udp_send_stream_session->start( );
      }
   }

   if( p_total_sessions )
      *p_total_sessions += num_send;
}
