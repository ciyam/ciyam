// Copyright (c) 2003
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

#ifndef SOCKETS_H
#  define SOCKETS_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#     ifdef _WIN32
#        define NOMINMAX
#        include <winsock.h>
#     else
#        include <sys/socket.h>
#        include <arpa/inet.h>
#     endif
#  endif

#ifdef _WIN32
class winsock_init
{
   public:
   int status;
   WSADATA wsd;

   winsock_init( WORD wVersionRequested = 0x0002 );
   ~winsock_init( );
};
#endif

#ifdef _WIN32
typedef int socklen_type;
#else
typedef size_t socklen_type;
#  define SOCKET int
#  define INVALID_SOCKET -1
#endif

class ip_address : public sockaddr_in
{
   public:
   ip_address( int port = 0 );
   ip_address( const char* p_address, int port = 0 );
   ip_address& operator =( const char* p_address );

   protected:
   void resolve( const char* p_address, int port = 0 );
};

class tcp_socket
{
   public:
   tcp_socket( );
   tcp_socket( SOCKET socket );

   virtual ~tcp_socket( );

   bool open( );
   void close( );

   bool bind( const ip_address& addr );
   bool connect( const ip_address& addr, size_t timeout = 0 );

   bool listen( );
   SOCKET accept( ip_address& addr, size_t timeout = 0 ) const;

   bool set_delay( );
   bool set_no_delay( );

   bool set_no_linger( );
   bool set_reuse_addr( );

   bool has_input( size_t timeout = 0 ) const;
   bool can_output( size_t timeout = 0 ) const;

   virtual int recv( unsigned char* buf, int buflen, size_t timeout = 0 );
   virtual int send( const unsigned char* buf, int buflen, size_t timeout = 0 );

   int recv_n( unsigned char* buf, int buflen, size_t timeout = 0 );
   int send_n( const unsigned char* buf, int buflen, size_t timeout = 0 );

   int read_line( std::string& str, size_t timeout = 0 );
   int write_line( const std::string& str, size_t timeout = 0 );

   bool get_option( int type, int opt, char* p_buffer, socklen_type& buflen );
   bool set_option( int type, int opt, const char* p_buffer, socklen_type buflen );

   bool had_timeout( ) const { return timed_out; }
   bool had_blank_line( ) const { return blank_line; }

   bool okay( ) const { return socket != INVALID_SOCKET; }
   operator const void*( ) const { return socket != INVALID_SOCKET ? this : 0; }

   private:
   bool blank_line;

   SOCKET socket;

   // FUTURE: Need to add a member in order to detect the "would block" status before allowing these to be public.
   bool set_blocking( );
   bool set_non_blocking( );

   tcp_socket( const tcp_socket& );
   tcp_socket& operator =( const tcp_socket& );

   protected:
   bool timed_out;
   SOCKET get_socket( ) const { return socket; }
};

#endif

