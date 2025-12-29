// Copyright (c) 2010-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef SSL_SOCKET_H
#  define SSL_SOCKET_H

#  include "sockets.h"

#  include <openssl/ssl.h>

const size_t c_default_ssl_accept_timeout = 30000;
const size_t c_default_ssl_connect_timeout = 30000;

void init_ssl( const char* p_keyfile,
 const char* p_password = 0, const char* p_CA_List = 0, bool multi_threaded = false );

void term_ssl( );

struct ssl_init
{
   ssl_init( const char* p_keyfile,
    const char* p_password = 0, const char* p_CA_List = 0, bool multi_threaded = false )
   {
      init_ssl( p_keyfile, p_password, p_CA_List, multi_threaded );
   }

   inline ~ssl_init( ) { term_ssl( ); }
};

class ssl_socket : public tcp_socket
{
   public:
   ssl_socket( );

   ssl_socket( SOCKET socket );

   virtual ~ssl_socket( );

   void close( );

   void ssl_accept( size_t timeout = c_default_ssl_accept_timeout, bool* p_rc = 0 );

   void ssl_connect( size_t timeout = c_default_ssl_connect_timeout, bool* p_rc = 0 );

   bool is_secure( ) const { return secure; }

   bool is_tls_handshake( );

   int recv( unsigned char* buf, int buflen, size_t timeout = 0 );

   int send( const unsigned char* buf, int buflen, size_t timeout = 0 );

   private:
   SSL* p_ssl;
   bool secure;
};

#endif
