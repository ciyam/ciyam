// Copyright (c) 2003-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2017 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <stdio.h>
#  include <memory.h>
#  include <memory>
#  include <fstream>
#  include <iostream>
#  include <stdexcept>
#  ifndef _WIN32
#     include <fcntl.h>
#     include <netdb.h>
#     include <stdarg.h>
#     include <unistd.h>
#     include <sys/time.h>
#     include <netinet/in.h>
#     include <netinet/tcp.h>
#  endif
#endif

#include "sockets.h"

#include "base64.h"
#include "progress.h"
#include "utilities.h"

using namespace std;

namespace
{

const int c_default_buf_size = 65536;

}

#ifdef _WIN32
winsock_init::winsock_init( WORD wVersionRequested )
{
   status = ::WSAStartup( wVersionRequested, &wsd );
}

winsock_init::~winsock_init( )
{
   ::WSACleanup( );
}
#endif

#ifndef _WIN32
#  define HOSTENT hostent
#  define IN_ADDR in_addr
#  define SOCKET_ERROR -1
#endif

ip_address::ip_address( int port )
{
   memset( this, 0, sizeof( sockaddr_in ) );
   sin_family = AF_INET;
   sin_addr.s_addr = htonl( INADDR_ANY );
   sin_port = htons( ( u_short )port );
}

ip_address::ip_address( const char* p_address, int port )
{
   resolve( p_address, port );
}

ip_address& ip_address::operator =( const char* p_address )
{
   resolve( p_address );
   return *this;
}

void ip_address::resolve( const char* p_address, int port )
{
   memset( this, 0, sizeof( sockaddr_in ) );
   sin_family = AF_INET;
   sin_addr.s_addr = inet_addr( p_address );
   if( sin_addr.s_addr == INADDR_NONE && strcmp( p_address, "255.255.255.255" ) != 0 )
   {
      HOSTENT* p_host = ::gethostbyname( p_address );
      if( p_host )
         sin_addr.s_addr = ( ( IN_ADDR* )p_host->h_addr )->s_addr;
      else
         sin_addr.s_addr = INADDR_ANY;
   }

   sin_port = htons( ( u_short )port );
}

string ip_address::get_addr_string( ) const
{
   char buf[ 64 ];
#ifndef _WIN32
   ::inet_ntop( AF_INET, &( sin_addr ), buf, sizeof( buf ) ); // NOTE: use AF_INET ==> AF_INET6 for ipv6
#else
   DWORD addr_size = sizeof( buf );
   ::WSAAddressToString( ( LPSOCKADDR )this, sizeof( ip_address ), 0, buf, &addr_size );

   int dots = 0;
   for( size_t i = 0; i < sizeof( buf ); i++ )
   {
      if( buf[ i ] == '.' )
         ++dots;
      else if( buf[ i ] == ':' && dots == 3 )
      {
         buf[ i ] = '\0';
         break;
      }
   }
#endif
   return string( buf );
}

tcp_socket::tcp_socket( )
 :
 timed_out( false ),
 blank_line( false ),
 socket( INVALID_SOCKET )
{
}

tcp_socket::tcp_socket( SOCKET socket )
 :
 timed_out( false ),
 blank_line( false ),
 socket( socket )
{
}

tcp_socket::~tcp_socket( )
{
   if( socket != INVALID_SOCKET )
      close( );
}

bool tcp_socket::open( )
{
   // NOTE: In order to stop connections from taking too long a 10 second timeout is specified
   // (other later operations such as sends and receives can have specific timeouts provided).
   struct timeval tv;
   tv.tv_sec = 10; // i.e. 10 seconds

   ::setsockopt( socket, SOL_SOCKET, SO_RCVTIMEO, ( const char* )&tv, sizeof( struct timeval ) );
   ::setsockopt( socket, SOL_SOCKET, SO_SNDTIMEO, ( const char* )&tv, sizeof( struct timeval ) );

   if( socket == INVALID_SOCKET )
      socket = ::socket( AF_INET, SOCK_STREAM, 0 );

   return ( socket != INVALID_SOCKET );
}

void tcp_socket::close( )
{
   if( socket != INVALID_SOCKET )
   {
      ::shutdown( socket, 2 );
#ifndef _WIN32
      ::close( socket );
#else
      ::closesocket( socket );
#endif
   }

   socket = INVALID_SOCKET;
}

bool tcp_socket::bind( const ip_address& addr )
{
   return ::bind( socket, ( const sockaddr* )&addr, sizeof( sockaddr ) ) != SOCKET_ERROR;
}

bool tcp_socket::connect( const ip_address& addr, size_t timeout )
{
   if( timeout )
   {
      bool connected = false;
      if( set_non_blocking( ) )
      {
         ::connect( socket, ( const sockaddr* )&addr, sizeof( sockaddr ) );

         fd_set fdset;
         struct timeval tv;

         tv.tv_sec = timeout / 1000;
         tv.tv_usec = ( timeout % 1000 ) * 1000;
         
         FD_ZERO( &fdset );
         FD_SET( socket, &fdset );
         if( ::select( socket + 1, 0, &fdset, 0, &tv ) == 1 )
         {
            int so_error = 1;
            socklen_t len = sizeof( so_error );

            ::getsockopt( socket, SOL_SOCKET, SO_ERROR, ( char* )&so_error, &len );

            if( !so_error )
            {
               set_blocking( );
               connected = true;
            }
         }
      }

      return connected;
   }
   else
      return ::connect( socket, ( const sockaddr* )&addr, sizeof( sockaddr ) ) != SOCKET_ERROR;
}

bool tcp_socket::listen( )
{
   return ::listen( socket, SOMAXCONN ) != SOCKET_ERROR;
}

SOCKET tcp_socket::accept( ip_address& addr, size_t timeout ) const
{
   bool okay = true;

   if( timeout )
      okay = has_input( timeout );

   if( !okay )
      return INVALID_SOCKET;
   else
   {
      socklen_t len = sizeof( sockaddr );
      return ::accept( socket, ( sockaddr* )&addr, &len );
   }
}

bool tcp_socket::get_delay( )
{
   int val = 0;
   socklen_t len = sizeof( val );

   get_option( IPPROTO_TCP, TCP_NODELAY, ( char* )&val, len );

   return ( val == 0 );
}

bool tcp_socket::set_delay( )
{
   int val = 0;
   return set_option( IPPROTO_TCP, TCP_NODELAY, ( const char* )&val, sizeof( val ) );
}

bool tcp_socket::set_no_delay( )
{
   int val = 1;
   return set_option( IPPROTO_TCP, TCP_NODELAY, ( const char* )&val, sizeof( val ) );
}

bool tcp_socket::set_blocking( )
{
#ifdef _WIN32
   unsigned long val = 0;
   return ::ioctlsocket( socket, FIONBIO, &val ) != SOCKET_ERROR;
#else
   long val;
   if( val = ::fcntl( socket, F_GETFL, 0 ) < 0 )
      return false;

   val &= ~O_NONBLOCK;
   return ::fcntl( socket, F_SETFL, val ) >= 0;
#endif
}

bool tcp_socket::set_non_blocking( )
{
#ifdef _WIN32
   unsigned long val = 1;
   return ::ioctlsocket( socket, FIONBIO, &val ) != SOCKET_ERROR;
#else
   long val;
   if( val = ::fcntl( socket, F_GETFL, 0 ) < 0 )
      return false;

   val |= O_NONBLOCK;
   return ::fcntl( socket, F_SETFL, val ) >= 0;
#endif
}

bool tcp_socket::set_no_linger( )
{
#ifdef _WIN32
   LINGER val;
#else
   struct linger val;
#endif
   val.l_onoff = 1;
   val.l_linger = 0;
   return set_option( SOL_SOCKET, SO_LINGER, ( const char* )&val, sizeof( val ) );
}

bool tcp_socket::set_reuse_addr( )
{
   int val = 1;
   return set_option( SOL_SOCKET, SO_REUSEADDR, ( const char* )&val, sizeof( val ) );
}

bool tcp_socket::has_input( size_t timeout ) const
{
   bool okay;
   fd_set rfds;
   struct timeval tv;

   tv.tv_sec = timeout / 1000;
   tv.tv_usec = ( timeout % 1000 ) * 1000;

   FD_ZERO( &rfds );
   FD_SET( socket, &rfds );

   // NOTE: This function will indicate success even if the "select" return code has
   // an error as it is expected that the error will occur during the receive itself.
   okay = ::select( socket + 1, &rfds, 0, 0, &tv ) != 0;

   FD_CLR( socket, &rfds );
   return okay;
}

bool tcp_socket::can_output( size_t timeout ) const
{
   bool okay;
   fd_set wfds;
   struct timeval tv;

   tv.tv_sec = timeout / 1000;
   tv.tv_usec = ( timeout % 1000 ) * 1000;

   FD_ZERO( &wfds );
   FD_SET( socket, &wfds );

   // NOTE: This function will indicate success even if the "select" return code has
   // an error as it is expected that the error will occur during the send operation.
   okay = ::select( socket + 1, 0, &wfds, 0, &tv ) != 0;

   FD_CLR( socket, &wfds );

   return okay;
}

int tcp_socket::recv( unsigned char* buf, int buflen, size_t timeout )
{
   bool okay = true;

   timed_out = false;

   if( timeout )
      okay = has_input( timeout );

   int n = 0;
   if( !okay )
      timed_out = true;
   else
      n = ::recv( socket, ( char* )buf, buflen, 0 );

   return n;
}

int tcp_socket::send( const unsigned char* buf, int buflen, size_t timeout )
{
   bool okay = true;

   timed_out = false;

   if( timeout )
      okay = can_output( timeout );

   int n = 0;
   if( !okay )
      timed_out = true;
   else
      n = ::send( socket, ( const char* )buf, buflen, 0 );

   return n;
}

int tcp_socket::recv_n( unsigned char* buf, int buflen, size_t timeout )
{
   int n;
   int rcvd = 0;

   while( rcvd != buflen )
   {
      n = recv( buf + rcvd, buflen - rcvd, timeout );
      if( n <= 0 )
         break;

      rcvd += n;
   }

   return rcvd;
}

int tcp_socket::send_n( const unsigned char* buf, int buflen, size_t timeout )
{
   int n;
   int sent = 0;

   while( sent != buflen )
   {
      n = send( buf + sent, buflen - sent, timeout );
      if( n <= 0 )
         break;

      sent += n;
   }

   return sent;
}

int tcp_socket::read_line( string& str, size_t timeout, int max_chars, progress* p_progress )
{
   int n = 0;
   unsigned char b, lb = '\0';

   blank_line = false;

   // NOTE: This is not very efficient as it reads one character at a time through "recv". If using
   // a non-blocking socket this could probably be made more efficient by working with some sort of
   // internal buffer (which would unfortunately complicate the whole reading process).
   while( recv( &b, 1, timeout ) > 0 )
   {
      n++;

      if( b == '\n' && lb == '\r' )
      {
         n -= 2;
         if( n == 0 )
            blank_line = true;
         break;
      }

      if( lb != '\0' )
      {
         if( !max_chars || str.size( ) < max_chars )
            str += lb;
         else
            throw runtime_error( "max. line length exceeded" );
      }

      lb = b;
   }

   if( p_progress && !str.empty( ) )
      p_progress->output_progress( ">R> " + str );

   return n;
}

int tcp_socket::write_line( const string& str, size_t timeout, progress* p_progress )
{
   int n;
   bool truncate = false;
   bool terminated = false;

   string::size_type len( str.length( ) );

   if( len >= 1 && str[ len - 1 ] == '\n' )
   {
      if( len >= 2 && str[ len - 2 ] == '\r' )
         terminated = true;
      else
         truncate = true;
   }

   string s;
   const char* p_data( 0 );

   if( len == 0 || ( truncate && len == 1 ) || ( terminated && len == 2 ) )
      n = 0;
   else
   {
      if( terminated )
         p_data = str.c_str( );
      else
      {
         if( !truncate )
            s = str;
         else
            s = str.substr( 0, len - 1 );

         s += "\r\n";

         len += 2 - truncate;
         p_data = s.c_str( );
      }
   }

   if( p_data )
   {
      if( p_progress )
      {
         string write_string( "<W< " );

         if( !get_delay( ) )
            write_string = string( "<W<!" );

         p_progress->output_progress( write_string + str );
      }

      n = send_n( ( const unsigned char* )p_data, len, timeout );
   }

   return n;
}

bool tcp_socket::get_option( int type, int opt, char* p_buffer, socklen_t& buflen )
{
   return ::getsockopt( socket, type, opt, p_buffer, &buflen ) != SOCKET_ERROR;
}

bool tcp_socket::set_option( int type, int opt, const char* p_buffer, socklen_t buflen )
{
   return ::setsockopt( socket, type, opt, p_buffer, buflen ) != SOCKET_ERROR;
}

void file_transfer( const string& name,
 tcp_socket& s, ft_direction d, size_t max_size,
 const char* p_ack_message, size_t initial_timeout, size_t line_timeout, size_t max_line_size,
 unsigned char* p_prefix_char, unsigned char* p_buffer, unsigned int buffer_size, progress* p_progress )
{
   bool not_base64 = false;
   bool max_size_exceeded = false;

   string unexpected_data;

   bool use_recv_buffer = ( p_buffer && buffer_size );
   
   bool has_prefix_char = ( p_prefix_char && *p_prefix_char );

   if( d == e_ft_direction_send )
   {
      if( !file_exists( name ) )
         throw runtime_error( "file not found" );

      ifstream inpf( name.c_str( ), ios::binary );
      if( !inpf )
         throw runtime_error( "file '" + name + "' could not be opened for input" );

      size_t buf_size = max_line_size
       ? base64::decode_size( max_line_size + has_prefix_char ) : c_default_buf_size;

      auto_ptr< char > ap_buf( new char[ buf_size ] );

      bool is_first = true;

      while( true )
      {
         size_t count = buf_size;

         if( !inpf.read( ap_buf.get( ), buf_size ) )
            count = inpf.gcount( );

         if( !count )
            break;

         string next( string( ap_buf.get( ), count ) );

         if( is_first && has_prefix_char )
            next = ( char )*p_prefix_char + next;

         next = base64::encode( next );

         s.write_line( next, is_first ? initial_timeout : line_timeout, p_progress );

         next.erase( );
         s.read_line( next, is_first ? initial_timeout : line_timeout, max_line_size, p_progress );

         if( s.had_timeout( ) )
            throw runtime_error( "timeout occurred reading send response for file transfer" );

         if( next != string( p_ack_message ) )
         {
            // NOTE: If "error" is found in the message then just throw it as is.
            if( next.find( "error" ) != string::npos )
               throw runtime_error( next );
            else
               throw runtime_error( "was expecting '" + string( p_ack_message ) + "' but found '" + next + "'" );
         }

         if( inpf.eof( ) )
            break;

         is_first = false;
      }

      s.write_line( p_ack_message, line_timeout, p_progress );
   }
   else
   {
      if( use_recv_buffer && buffer_size < max_size )
         throw runtime_error( "buffer_size < max_size for file_transfer" );

      ofstream outf( name.c_str( ), ios::binary );
      if( !outf )
         throw runtime_error( "file '" + name + "' could not be opened for output" );

      string next;
      size_t written = 0;
      bool is_first = true;

      unsigned char* p_buf = p_buffer;

      while( true )
      {
         next.erase( );
         s.read_line( next, is_first ? initial_timeout : line_timeout, max_line_size, p_progress );

         if( s.had_timeout( ) )
            throw runtime_error( "timeout occurred reading next line for file transfer" );

         if( next.empty( ) || next == string( p_ack_message ) )
            break;

         // FUTURE: This should actually check if any non-base64 character is present.
         if( next.find( ' ' ) != string::npos )
         {
            not_base64 = true;
            unexpected_data = next;
            break;
         }

         string decoded( base64::decode( next ) );

         if( is_first && p_prefix_char && *p_prefix_char )
         {
            *p_prefix_char = decoded[ 0 ];
            decoded.erase( 0, 1 );
         }

         if( !outf.write( &decoded[ 0 ], decoded.length( ) ) )
            throw runtime_error( "unexpected error writing to file '" + name + "'" );

         written += decoded.length( );
         if( written > max_size )
         {
            max_size_exceeded = true;
            break;
         }

         if( use_recv_buffer )
         {
            memcpy( p_buf, &decoded[ 0 ], decoded.size( ) );
            p_buf += decoded.size( );
         }

         s.write_line( p_ack_message, line_timeout, p_progress );

         is_first = false;
      }

      outf.close( );
   }

   if( not_base64 || max_size_exceeded )
   {
      file_remove( name.c_str( ) );

      if( not_base64 )
      {
         if( unexpected_data.empty( ) )
            unexpected_data = "unexpected empty data";
         throw runtime_error( unexpected_data );
      }
      else
         throw runtime_error( "maximum file length exceeded" );
   }
}

