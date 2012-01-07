// Copyright (c) 2004
//
// CIYAM Pty. Ltd.
// ACN 093 704 539
//
// ALL RIGHTS RESERVED
//
// Permission to use this software for non-commercial purposes is hereby granted. Permission to
// distribute this software privately is granted provided that the source code is unaltered and
// complete or that any alterations and omissions have been first approved by CIYAM. Commercial
// usage of this software is not permitted without first obtaining a license for such a purpose
// from CIYAM. This software may not be publicly distributed unless written permission to do so
// has been obtained from CIYAM.

#ifdef __BORLANDC__
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <stdexcept>
#endif

#include "tcp_read_write_buffer.h"

#include "sockets.h"

using namespace std;

void tcp_read_buffer::read_available( )
{
   unsigned char buf[ c_num_block_size_digits ];
   if( socket.recv_n( buf, c_num_block_size_digits, timeout ) < 0 )
   {
      if( socket.had_timeout( ) )
         throw runtime_error( "timed out during recv_n" );
      else
         throw runtime_error( "unexpected recv_n failure" );
   }

   size_t waiting = 0;
   for( int i = 0; i < c_num_block_size_digits; i++ )
   {
      waiting *= 10;
      waiting += buf[ i ] - '0';
   }

   int rc;
   if( excess )
   {
      buffered = min( capacity, excess );
      rc = socket.recv_n( &buffer[ 0 ], buffered, timeout );
   }
   else
   {
      buffered = min( capacity, waiting );
      rc = socket.recv_n( &buffer[ 0 ], buffered, timeout );
   }

   if( rc <= 0 )
   {
      if( socket.had_timeout( ) )
         throw runtime_error( "timed out during recv_n" );
      else
         throw runtime_error( "unexpected recv_n failure" );
   }

   excess = waiting - buffered;
   retrieved = 0;
}

void tcp_write_buffer::flush_data( )
{
   if( buffered )
   {
      buffer[ 0 ] = '0' + ( ( buffered % 10000 ) / 1000 );
      buffer[ 1 ] = '0' + ( ( buffered % 1000 ) / 100 );
      buffer[ 2 ] = '0' + ( ( buffered % 100 ) / 10 );
      buffer[ 3 ] = '0' + ( buffered % 10 );

      if( socket.send_n( &buffer[ 0 ], ( buffered + reserved ), timeout ) <= 0 )
      {
         if( socket.had_timeout( ) )
            throw runtime_error( "timed out during send_n" );
         else
            throw runtime_error( "unexpected send_n failure" );
      }

      buffered = 0;
   }
}

