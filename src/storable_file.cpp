// Copyright (c) 2005-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2014 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <fstream>
#  include <utility>
#endif

#define CIYAM_BASE_IMPL

#include "storable_file.h"

#include "ods.h"
#include "read_write_stream.h"

using namespace std;

namespace
{

const int c_buf_size = 1024;

}

int_t storable_file::get_size_of( ) const
{
   return size_of( *this );
}

void storable_file::get_instance( read_stream& rs )
{
   rs >> *this;
}

void storable_file::put_instance( write_stream& ws ) const
{
   ws << *this;
}

int_t size_of( const storable_file& sf )
{
   return file_size( sf.file_name.c_str( ) );
}

read_stream& operator >>( read_stream& rs, storable_file& sf )
{
   rs.read_meta( sf.file_name );

   if( sf.file_name.empty( ) )
      throw runtime_error( "unexpected missing read_stream meta data for storable_file" );

   string::size_type pos = sf.file_name.find_last_of( ':' );
   if( pos == string::npos )
      throw runtime_error( "unexpected file length missing from storable_file read_stream meta data" );

   int64_t size = from_string< int64_t >( sf.file_name.substr( pos + 1 ) );
   sf.file_name.erase( pos );

   ofstream outf( sf.file_name.c_str( ), ios::out | ios::binary );
   if( !outf )
      throw runtime_error( "unable to open '" + sf.file_name + "' for output" );

   unsigned char data[ c_buf_size ];
   while( size )
   {
      size_t bytes = c_buf_size;
      if( size < bytes )
         bytes = size;

      rs.read( &data[ 0 ], bytes );

      outf.write( ( const char* )&data[ 0 ], bytes );

      size -= bytes;
   }

   return rs;
}

write_stream& operator <<( write_stream& ws, const storable_file& sf )
{
   string name( sf.file_name );
#ifndef _WIN32
   string::size_type pos = name.find_last_of( "/" );
#else
   string::size_type pos = name.find_last_of( ":/\\" );
#endif

   int64_t size = file_size( sf.file_name.c_str( ) );

   ifstream inpf( sf.file_name.c_str( ), ios::in | ios::binary );
   if( !inpf )
      throw runtime_error( "unable to open '" + sf.file_name + "' for input" );

   unsigned char data[ c_buf_size ];

   while( size )
   {
      size_t bytes = c_buf_size;
      if( size < bytes )
         bytes = size;

      if( !inpf.read( ( char* )&data[ 0 ], bytes ) )
         throw runtime_error( "unexpected error reading '" + sf.file_name + "' for write_stream output" );

      ws.write( &data[ 0 ], bytes );

      size -= ( int64_t )bytes;
   }

   inpf.close( );

   return ws;
}

