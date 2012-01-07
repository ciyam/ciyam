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

#ifndef READ_WRITE_BUFFERED_STREAM_H
#  define READ_WRITE_BUFFERED_STREAM_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <utility>
#  endif

#  include "read_write_buffer.h"
#  include "read_write_stream.h"

class read_buffered_stream : public read_stream
{
   public:
   read_buffered_stream( read_buffer& rb )
    :
    rb( rb )
   {
   }

   void read( unsigned char* p_buf, size_t len, read_write_type )
   {
      rb.read_data( p_buf, len );
   }

   private:
   read_buffer& rb;
};

class write_buffered_stream : public write_stream
{
   public:
   write_buffered_stream( write_buffer& wb )
    :
    wb( wb )
   {
   }

   void write( const unsigned char* p_buf, size_t len, read_write_type )
   {
      wb.write_data( p_buf, len );
   }

   void flush( ) { wb.flush_data( ); }

   private:
   write_buffer& wb;
};

class read_write_buffered_stream : public read_buffered_stream, public write_buffered_stream
{
   public:
   read_write_buffered_stream( read_buffer& rb, write_buffer& wb )
    : read_buffered_stream( rb ), write_buffered_stream( wb )
   {
   }
};

class ptf_read_buffered_stream : public read_stream
{
   public:
   ptf_read_buffered_stream( read_buffer& rb )
    :
    rb( rb )
   {
   }

   void read( unsigned char* p_buf, size_t len, read_write_type type );

   private:
   read_buffer& rb;
};

class ptf_write_buffered_stream : public write_stream
{
   public:
   ptf_write_buffered_stream( write_buffer& wb )
    :
    wb( wb )
   {
   }

   void write( const unsigned char* p_buf, size_t len, read_write_type type );

   void flush( ) { wb.flush_data( ); }

   private:
   write_buffer& wb;
};

class ptf_read_write_buffered_stream : public ptf_read_buffered_stream, public ptf_write_buffered_stream
{
   public:
   ptf_read_write_buffered_stream( read_buffer& rb, write_buffer& wb )
    : ptf_read_buffered_stream( rb ), ptf_write_buffered_stream( wb )
   {
   }

   ptf_read_write_buffered_stream( read_write_buffer& rwb )
    : ptf_read_buffered_stream( rwb ), ptf_write_buffered_stream( rwb )
   {
   }

   template< typename RWB > ptf_read_write_buffered_stream( RWB& rwb )
    : ptf_read_buffered_stream( rwb ), ptf_write_buffered_stream( rwb )
   {
   }
};

#endif

