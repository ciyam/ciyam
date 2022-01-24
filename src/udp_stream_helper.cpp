// Copyright (c) 2022 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

// NOTE: This file is intended for #include usage in anonymous namespaces only.

struct udp_stream_helper : udp_helper
{
   udp_stream_helper( const string& hash ) : udp_helper( hash ) { }

   void recv_data( unsigned char* p_buffer, unsigned int buflen, size_t& start_offset );
};

void udp_stream_helper::recv_data(
 unsigned char* p_buffer, unsigned int buflen, size_t& start_offset )
{
   if( p_buffer )
   {
      bool has_any = false;

      // NOTE: Wait an increasing amount of time for UDP data to be received.
      for( size_t i = 1; i < 8; i++ )
      {
         msleep( i );

         size_t num = 0;

         if( has_udp_recv_file_chunk_info( &num ) )
         {
            // NOTE: If has not found many chunks then sleep a little longer.
            if( num <= 10 )
               msleep( 5 );

            has_any = true;
            break;
         }
      }

      if( has_any )
      {
         size_t chunk = 0;
         size_t remaining = buflen;

         while( true )
         {
            string next( get_udp_recv_file_chunk_info( chunk, true ) );

            if( next.empty( ) || next.size( ) < 59 )
               break;

            if( next.substr( 0, 9 ) != hash.substr( 0, 9 ) )
               break;

            //nyi - should validate hash info here...
            next.erase( 0, 55 );

            bool rc = false;

            base64::validate( next, &rc );

            if( !rc )
               break;

            string data( base64::decode( next ) );

            size_t len = data.length( );

            if( len > remaining )
               break;

            memcpy( ( p_buffer + start_offset ), &data[ 0 ], len );

            // NOTE: Give up some time for further UDP packet receives.
            if( ++chunk % 100 == 0 )
               msleep( 10 );

            remaining -= len;
            start_offset += len;
         }
      }
   }
}
