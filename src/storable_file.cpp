// Copyright (c) 2005-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2016 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <memory>
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

const int c_buf_size = 4096;

}

void storable_file::set_extra( storable_extra* p_extra )
{
   storable_file_extra* p_file_extra = dynamic_cast< storable_file_extra* >( p_extra );

   if( p_file_extra )
   {
      file_name = p_file_extra->file_name;
      p_istream = p_file_extra->p_istream;
      p_ostream = p_file_extra->p_ostream;
      file_size = p_file_extra->file_size;
   }
   else
   {
      file_name.clear( );

      p_istream = 0;
      p_ostream = 0;

      file_size = 0;
   }
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
   if( sf.p_istream )
      return sf.file_size;
   else
      return file_size( sf.file_name.c_str( ) );
}

read_stream& operator >>( read_stream& rs, storable_file& sf )
{
   if( sf.file_name.empty( ) )
      throw runtime_error( "unexpected missing file_name for storable_file" );

   int64_t size = sf.get_ods( )->get_size( sf.get_id( ) );

   auto_ptr< ofstream > ap_outf;

   if( !sf.p_ostream )
   {
      ap_outf.reset( new ofstream( sf.file_name.c_str( ), ios::out | ios::binary ) );

      if( !( *ap_outf ) )
         throw runtime_error( "unable to open '" + sf.file_name + "' for output" );
   }

   unsigned char data[ c_buf_size ];
   while( size )
   {
      size_t bytes = c_buf_size;
      if( size < bytes )
         bytes = size;

      rs.read( &data[ 0 ], bytes );

      if( !sf.p_ostream )
         ap_outf->write( ( const char* )&data[ 0 ], bytes );
      else
         sf.p_ostream->write( ( const char* )&data[ 0 ], bytes );

      size -= bytes;
   }

   if( !sf.p_ostream )
      ap_outf->close( );

   return rs;
}

write_stream& operator <<( write_stream& ws, const storable_file& sf )
{
   int64_t size = size_of( sf );

   auto_ptr< ifstream > ap_inpf;

   if( !sf.p_istream )
   {
      ap_inpf.reset( new ifstream( sf.file_name.c_str( ), ios::in | ios::binary ) );

      if( !( *ap_inpf ) )
         throw runtime_error( "unable to open '" + sf.file_name + "' for input" );
   }

   unsigned char data[ c_buf_size ];

   while( size )
   {
      size_t bytes = c_buf_size;
      if( size < bytes )
         bytes = size;

      bool okay = true;

      if( !sf.p_istream )
         okay = ap_inpf->read( ( char* )&data[ 0 ], bytes );
      else
         okay = sf.p_istream->read( ( char* )&data[ 0 ], bytes );

      if( !okay )
         throw runtime_error( "unexpected error reading '" + sf.file_name + "' for write_stream output" );

      ws.write( &data[ 0 ], bytes );

      size -= ( int64_t )bytes;
   }

   if( !sf.p_istream )
      ap_inpf->close( );

   return ws;
}

