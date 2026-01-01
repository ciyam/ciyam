// Copyright (c) 2004-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef TCP_READ_WRITE_BUFFER_H
#  define TCP_READ_WRITE_BUFFER_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <utility>
#  endif

#  include "read_write_buffer.h"

class tcp_socket;

const int c_tcp_buffer_size = 8194;

const int c_num_block_size_digits = 4;

class tcp_read_buffer : public read_buffer
{
   public:
   tcp_read_buffer( tcp_socket& socket, size_t timeout = 0 )
    : read_buffer( c_tcp_buffer_size ),
    excess( 0 ),
    socket( socket ),
    timeout( timeout )
   {
   }

   void read_available( );

   private:
   size_t excess;

   tcp_socket& socket;
   size_t timeout;
};

class tcp_write_buffer : public write_buffer
{
   public:
   tcp_write_buffer( tcp_socket& socket, size_t timeout = 0 )
    : write_buffer( c_tcp_buffer_size, c_num_block_size_digits ),
    socket( socket ),
    timeout( timeout )
   {
   }

   void flush_data( );

   private:
   tcp_socket& socket;
   size_t timeout;
};

class tcp_read_write_buffer : public tcp_read_buffer, public tcp_write_buffer
{
   public:
   tcp_read_write_buffer( tcp_socket& socket, size_t timeout = 0 )
    : tcp_read_buffer( socket, timeout ), tcp_write_buffer( socket, timeout )
   {
   }
};

#endif

