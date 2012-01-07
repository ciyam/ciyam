// Copyright (c) 2010
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

#ifndef SSL_SOCKET_H
#  define SSL_SOCKET_H

#  include "sockets.h"

#  include <openssl/ssl.h>

void init_ssl( const char* p_keyfile, const char* p_password = 0, const char* p_CA_List = 0 );
void term_ssl( );

struct ssl_init
{
   ssl_init( const char* p_keyfile, const char* p_password = 0, const char* p_CA_List = 0 )
   {
      init_ssl( p_keyfile, p_password, p_CA_List );
   }

   ~ssl_init( ) { term_ssl( ); }
};

class ssl_socket : public tcp_socket
{
   public:
   ssl_socket( );
   ssl_socket( SOCKET socket );

   virtual ~ssl_socket( );

   void ssl_connect( );

   bool is_secure( ) const { return secure; }

   int recv( unsigned char* buf, int buflen, size_t timeout = 0 );
   int send( const unsigned char* buf, int buflen, size_t timeout = 0 );

   private:
   SSL* p_ssl;
   bool secure;
};

#endif

