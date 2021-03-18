// Copyright (c) 2007-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2021 CIYAM Developers
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
#  ifdef _WIN32
#     include <cstring>
#     include <windows.h>
#     include <iphlpapi.h> // NOTE: Needs to be linked with "iphlpapi.lib".
#  else
#     include <cstdio>
#     include <cstring>
#     include <unistd.h>
#     include <net/if.h>
#     include <sys/ioctl.h>
#  endif
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
      if( i > 0 && sep != '\0' )
         os << ':';
      os << hex << setw( 2 ) << setfill( '0' ) << ( unsigned )*p_buffer++;
   }
}

}

#ifdef _WIN32
string get_mac_addr( char sep, const char *p_name )
{
   IP_ADAPTER_INFO* p;

   char* buffer( 0 );
   ULONG bufsize( 0 );

   // first call gets the real number of bytes required
   ::GetAdaptersInfo( ( IP_ADAPTER_INFO* )buffer, &bufsize );

   buffer = new char[ bufsize ];

   // second call gets the data (after allocating an appropriately-sized buffer)
   ::GetAdaptersInfo( ( IP_ADAPTER_INFO* )buffer, &bufsize );

   ostringstream osstr;
   for( p = ( IP_ADAPTER_INFO* )buffer; p; p = p->Next )
   {
      if( p_name && strcmp( p->AdapterName, p_name ) )
         continue;

      unsigned i;
      for( i = 0; i < p->AddressLength; i++ )
      {
         if( p->Address[ i ] != 0 )
            break;
      }

      // NOTE: If no name is provided then skip disconnected media if they appear first.
      if( !p_name && i == p->AddressLength )
         continue;

      output_buffer( osstr, p->Address, p->AddressLength, sep );

      if( !p_name )
         break;
   }

   delete[ ] buffer;

   return osstr.str( );
}
#else
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
#endif

#ifdef COMPILE_TESTBED_MAIN
int main( int argc, char * argv[ ] )
{
   try
   {
      string mac_addr( get_mac_addr( ':' ) );

      if( mac_addr.empty( ) )
      {
         cerr << "error: failed to get MAC address" << endl;
         return 1;
      }
      else
      {
         cout << mac_addr << endl;
         return 0;
      }
   }
   catch( exception& x )
   {
      cerr << "error: " << x.what( ) << endl;
      return 1;
   }
   catch( ... )
   {
      cerr << "unexpected exception caught" << endl;
      return 2;
   }
}
#endif

