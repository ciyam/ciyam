// Copyright (c) 2007-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <string>
#  include <sstream>
#  include <iomanip>
#  include <iostream>
#  include <stdexcept>
#  include <cstdio>
#  include <cstring>
#  include <unistd.h>
#  include <net/if.h>
#  include <sys/ioctl.h>
#endif

#include "mac.h"

using namespace std;

//#define COMPILE_TESTBED_MAIN

namespace
{

void output_buffer( ostream& os, unsigned char* p_buffer, int size, char sep )
{
   for( int i = 0; i < size; ++i )
   {
      if( ( i > 0 ) && ( sep != '\0' ) )
         os << ':';

      os << hex << setw( 2 ) << setfill( '0' ) << ( unsigned )*p_buffer++;
   }
}

}

string get_mac_addr( char sep, const char *p_name )
{
   char default_name[ 5 ] = "eth0";
   unsigned char mac_addr_buffer[ 7 ] = { 0, 0, 0, 0, 0, 0, 0 };

   int sckt;
   struct ifreq req;
   struct if_nameindex* p_if_list = 0;
   struct if_nameindex* p_if_save = 0;

#  ifndef SIOCGIFADDR
   // The kernel does not support the required ioctls
   return string( );
#  endif

   if( p_name == 0 )
      p_name = default_name;

   sckt = socket( PF_INET, SOCK_STREAM, 0 );

   if( sckt < 0 )
      throw runtime_error( "socket creation failure" );

   p_if_list = p_if_save = if_nameindex( );

   for( p_if_list; *( char* )p_if_list != 0; p_if_list++ )
   {
      if( p_name && strcmp( p_if_list->if_name, p_name ) )
         continue;

      strncpy( req.ifr_name, p_if_list->if_name, IF_NAMESIZE );

      if( ioctl( sckt, SIOCGIFHWADDR, &req ) != 0 )
         throw runtime_error( "unexpected ioctl failure" );

      memmove( ( void* )&mac_addr_buffer[ 0 ], ( void* )&req.ifr_ifru.ifru_hwaddr.sa_data[ 0 ], 6 );
      break;
   }

   if_freenameindex( p_if_save );
   close( sckt );

   ostringstream osstr;
   output_buffer( osstr, mac_addr_buffer, 6, sep );

   return osstr.str( );
}

#ifdef COMPILE_TESTBED_MAIN
int main( int argc, char * argv[ ] )
{
   int rc = 0;

   try
   {
      string mac_addr( get_mac_addr( ':' ) );

      if( !mac_addr.empty( ) )
         cout << mac_addr << endl;
      else
         throw runtime_error( "failed to get MAC address" );
   }
   catch( exception& x )
   {
      rc = 1;

      cerr << "error: " << x.what( ) << endl;
   }
   catch( ... )
   {
      rc = 2;

      cerr << "error: unexpected unknown exception caught" << endl;
   }

   return rc;
}
#endif
