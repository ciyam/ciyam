// Copyright (c) 2004-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <memory.h>
#  include <cassert>
#  include <memory>
#endif

#include "read_write_buffer.h"

//#define READ_WRITE_BUFFER_TESTBED

using namespace std;

read_buffer::read_buffer( size_t capacity )
 :
 buffered( 0 ),
 capacity( capacity ),
 retrieved( 0 )
{
   assert( capacity > 0 );
   buffer.resize( capacity );
}

void read_buffer::read_data( unsigned char* p_output, size_t len )
{
   while( len )
   {
      if( exhausted( ) )
         read_available( );

      size_t chunk = min( available( ), len );
      memcpy( p_output, &buffer[ retrieved ], chunk );

      len -= chunk;
      p_output += chunk;
      retrieved += chunk;
   }
}

write_buffer::write_buffer( size_t capacity, size_t reserved )
 :
 buffered( 0 ),
 reserved( reserved ),
 capacity( capacity )
{
   assert( capacity > 0 );
   buffer.resize( capacity + reserved );
}

void write_buffer::write_data( const unsigned char* p_input, size_t len )
{
   while( len )
   {
      size_t chunk = min( available( ), len );
      memcpy( &buffer[ reserved + buffered ], p_input, chunk );

      len -= chunk;
      p_input += chunk;
      buffered += chunk;

      if( full( ) )
         flush_data( );
   }
}

#ifdef READ_WRITE_BUFFER_TESTBED
#  include <string>
#  include <iostream>

namespace
{

const int c_test_buffer_size = 5;

vector< string > test_data_transporter;

class test_read_buffer : public read_buffer
{
   public:
   test_read_buffer( )
    : read_buffer( c_test_buffer_size )
   {
      num = 0;
      offset = 0;
      excess = 0;
   }

   void read_available( );

   private:
   size_t num;
   size_t offset;
   size_t excess;
};

void test_read_buffer::read_available( )
{
   if( num >= test_data_transporter.size( ) )
   {
      cerr << "attempt to read beyond end of input" << endl;
      exit( 1 );
   }

   string next( test_data_transporter[ num ] );
   size_t waiting = next[ 0 ] - '0';

   if( excess )
   {
      buffered = min( capacity, excess );
      memcpy( &buffer[ 0 ], &next[ 1 + offset ], buffered );
   }
   else
   {
      buffered = min( capacity, waiting );
      memcpy( &buffer[ 0 ], &next[ 1 ], buffered );
   }

   excess = waiting - buffered;

   if( excess == 0 )
   {
      ++num;
      offset = 0;
   }
   else
      offset += buffered;

   retrieved = 0;
}

class test_write_buffer : public write_buffer
{
   public:
   test_write_buffer( )
    : write_buffer( c_test_buffer_size, 1 )
   {
   }

   ~test_write_buffer( )
   {
      if( !empty( ) )
         flush_data( );
   }

   void flush_data( );
};

void test_write_buffer::flush_data( )
{
   cout << "*** flushing " << ( buffered + reserved ) << " bytes...\n";

   buffer[ 0 ] = '0' + buffered;

   for( size_t i = 0; i < ( buffered + reserved ); i++ )
      cout << buffer[ i ];
   cout << endl;

   test_data_transporter.push_back( string( ( const char* )&buffer[ 0 ], ( buffered + reserved ) ) );

   buffered = 0;
}

}

int main( )
{
   test_read_buffer test_reader;
   test_write_buffer test_writer;

   test_writer.write_data( ( const unsigned char* )"one.", 4 );
   test_writer.write_data( ( const unsigned char* )"two.", 4 );
   test_writer.write_data( ( const unsigned char* )"three", 5 );
   test_writer.flush_data( );

   char unsigned buf[ 5 ];

   test_reader.read_data( buf, 2 );
   buf[ 2 ] = '\0';
   cout << "==> buf = '" << buf << "'" << endl;

   test_reader.read_data( buf, 2 );
   buf[ 2 ] = '\0';
   cout << "==> buf = '" << buf << "'" << endl;

   test_reader.read_data( buf, 4 );
   buf[ 4 ] = '\0';
   cout << "==> buf = '" << buf << "'" << endl;

   test_reader.read_data( buf, 5 );
   buf[ 5 ] = '\0';
   cout << "==> buf = '" << buf << "'" << endl;
}

#endif

