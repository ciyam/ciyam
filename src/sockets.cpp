// Copyright (c) 2003-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2025 CIYAM Developers
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
#  ifdef __GNUG__
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
#include "format.h"
#include "progress.h"
#include "utilities.h"

using namespace std;

namespace
{

const int c_test_buf_size = 1024;

const int c_default_line_size = 1024;

const int c_append_to_size_info = 1000;

const int c_max_progress_output_bytes = 132;

const char c_app_suffix = '!';
const char c_udp_suffix = '~';

const char c_extra_separator = '#';

const char* const c_bye = "bye";
const char* const c_base64_format = ".b64";

const char* const c_local_host = "localhost";

const char* const c_env_var_name_slotx = "SLOTX";

struct scoped_empty_file_delete
{
   scoped_empty_file_delete( const string& file_name ) : file_name( file_name ) { }

   ~scoped_empty_file_delete( )
   {
      if( !file_name.empty( )
       && file_exists( file_name ) && file_size( file_name ) == 0 )
         file_remove( file_name );
   }

   string file_name;
};

}

#ifdef __GNUG__
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
#ifdef __GNUG__
   char buf[ 64 ];

   ::inet_ntop( AF_INET, &( sin_addr ), buf, sizeof( buf ) ); // NOTE: use AF_INET ==> AF_INET6 for ipv6
#else
#  error get_addr_string not implemented for this platform
#endif
   return string( buf );
}

socket_base::socket_base( )
 :
 timed_out( false ),
 close_in_dtor( true ),
 socket( INVALID_SOCKET )
{
}

socket_base::socket_base( SOCKET socket, bool close_in_dtor )
 :
 timed_out( false ),
 close_in_dtor( close_in_dtor ),
 socket( socket )
{
}

socket_base::~socket_base( )
{
   if( close_in_dtor && socket != INVALID_SOCKET )
      close( );
}

void socket_base::close( )
{
   if( socket != INVALID_SOCKET )
   {
      ::shutdown( socket, 2 );
      ::close( socket );
   }

   socket = INVALID_SOCKET;
}

bool socket_base::bind( const ip_address& addr )
{
   return ::bind( socket, ( const sockaddr* )&addr, sizeof( sockaddr ) ) != SOCKET_ERROR;
}

SOCKET socket_base::accept( ip_address& addr, size_t timeout ) const
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

bool socket_base::connect( const ip_address& addr, size_t timeout )
{
   if( timeout )
   {
      bool connected = false;

      if( set_non_blocking( ) )
      {
         ::connect( socket, ( const sockaddr* )&addr, sizeof( sockaddr ) );

         struct sockaddr_in peeraddr;
         socklen_t peeraddrlen;

         fd_set fdset;
         struct timeval tv;

         tv.tv_sec = timeout / 1000;
         tv.tv_usec = ( timeout % 1000 ) * 1000;

         FD_ZERO( &fdset );
         FD_SET( socket, &fdset );

         if( ::select( socket + 1, 0, &fdset, 0, &tv ) == 1 )
         {
            // NOTE: The "getpeername" call is used to detect connection when non-blocking.
            if( ::getpeername( socket, ( struct sockaddr* )&peeraddr, &peeraddrlen ) == 0 )
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
      }

      return connected;
   }
   else
      return ::connect( socket, ( const sockaddr* )&addr, sizeof( sockaddr ) ) != SOCKET_ERROR;
}

bool socket_base::listen( )
{
   return ::listen( socket, SOMAXCONN ) != SOCKET_ERROR;
}

bool socket_base::set_blocking( )
{
#ifdef __GNUG__
   long val;

   if( val = ::fcntl( socket, F_GETFL, 0 ) < 0 )
      return false;

   val &= ~O_NONBLOCK;

   return ::fcntl( socket, F_SETFL, val ) >= 0;
#else
#  error set_blocking not implemented for this platform
#endif
}

bool socket_base::set_non_blocking( )
{
#ifdef __GNUG__
   long val;

   if( val = ::fcntl( socket, F_GETFL, 0 ) < 0 )
      return false;

   val |= O_NONBLOCK;

   return ::fcntl( socket, F_SETFL, val ) >= 0;
#else
#  error set_non_blocking not implemented for this platform
#endif
}

bool socket_base::set_no_linger( )
{
   struct linger val;

   val.l_onoff = 1;
   val.l_linger = 0;

   return set_option( SOL_SOCKET, SO_LINGER, ( const char* )&val, sizeof( val ) );
}

bool socket_base::set_reuse_addr( )
{
   int val = 1;

   return set_option( SOL_SOCKET, SO_REUSEADDR, ( const char* )&val, sizeof( val ) );
}

bool socket_base::has_input( size_t timeout ) const
{
   bool okay = false;

   if( socket != INVALID_SOCKET )
   {
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
   }

   return okay;
}

bool socket_base::can_output( size_t timeout ) const
{
   bool okay = false;

   if( socket != INVALID_SOCKET )
   {
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
   }

   return okay;
}

int socket_base::peek( unsigned char* p_buf, int buflen, size_t timeout )
{
   bool okay = true;

   timed_out = false;

   int n = 0;

   if( timeout )
      okay = has_input( timeout );

   if( !okay )
      timed_out = true;
   else
      n = ::recv( socket, ( char* )p_buf, buflen, MSG_PEEK );

   return n;
}

int socket_base::recv( unsigned char* p_buf, int buflen, size_t timeout )
{
   bool okay = true;

   timed_out = false;

   if( timeout )
      okay = has_input( timeout );

   int n = 0;

   if( !okay )
      timed_out = true;
   else
      n = ::recv( socket, ( char* )p_buf, buflen, 0 );

   return n;
}

int socket_base::send( const unsigned char* p_buf, int buflen, size_t timeout )
{
   bool okay = true;

   timed_out = false;

   if( timeout )
      okay = can_output( timeout );

   int n = 0;

   if( !okay )
      timed_out = true;
   else
      n = ::send( socket, ( const char* )p_buf, buflen, 0 );

   return n;
}

int socket_base::recv_n( unsigned char* p_buf, int buflen, size_t timeout, progress* p_progress )
{
   int n;
   int rcvd = 0;

   while( rcvd != buflen )
   {
      n = recv( p_buf + rcvd, buflen - rcvd, timeout );

      if( n <= 0 )
         break;

      rcvd += n;
   }

   if( p_progress && rcvd )
   {
      string suffix;

      if( rcvd > c_max_progress_output_bytes )
         suffix = "...[" + format_bytes( rcvd ) + ']';

      p_progress->output_progress( ">R> " + string( ( const char* )p_buf, min( rcvd, c_max_progress_output_bytes ) ) + suffix );
   }

   return rcvd;
}

int socket_base::send_n( const unsigned char* p_buf, int buflen, size_t timeout, progress* p_progress )
{
   int n;
   int sent = 0;

   while( sent != buflen )
   {
      n = send( p_buf + sent, buflen - sent, timeout );

      if( n <= 0 )
         break;

      sent += n;
   }

   if( p_progress && sent )
   {
      string suffix;
      string write_string( "<W< " );

      if( !get_delay( ) )
         write_string = string( "<W<!" );

      if( sent > c_max_progress_output_bytes )
         suffix = "...[" + format_bytes( sent ) + ']';

      p_progress->output_progress( write_string + string( ( const char* )p_buf, min( sent, c_max_progress_output_bytes ) ) + suffix );
   }

   return sent;
}

bool socket_base::get_option( int type, int opt, char* p_buffer, socklen_t& buflen )
{
   return ::getsockopt( socket, type, opt, p_buffer, &buflen ) != SOCKET_ERROR;
}

bool socket_base::set_option( int type, int opt, const char* p_buffer, socklen_t buflen )
{
   return ::setsockopt( socket, type, opt, p_buffer, buflen ) != SOCKET_ERROR;
}

tcp_socket::tcp_socket( )
 : socket_base( ),
 blank_line( false ),
 num_read_lines( 0 ),
 num_write_lines( 0 )
{
}

tcp_socket::tcp_socket( SOCKET socket )
 : socket_base( socket ),
 blank_line( false ),
 num_read_lines( 0 ),
 num_write_lines( 0 )
{
}

bool tcp_socket::open( )
{
   // NOTE: In order to stop connections from taking too long a 20 second timeout is specified
   // (other later operations such as sends and receives can have specific timeouts provided).
   struct timeval tv;
   tv.tv_sec = 20; // i.e. 20 seconds

   ::setsockopt( socket, SOL_SOCKET, SO_RCVTIMEO, ( const char* )&tv, sizeof( struct timeval ) );
   ::setsockopt( socket, SOL_SOCKET, SO_SNDTIMEO, ( const char* )&tv, sizeof( struct timeval ) );

   if( socket == INVALID_SOCKET )
      socket = ::socket( AF_INET, SOCK_STREAM, 0 );

   return ( socket != INVALID_SOCKET );
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

int tcp_socket::read_line( string& str, size_t timeout, int max_chars, progress* p_progress )
{
   if( max_chars )
   {
      str.resize( max_chars );

      int len = read_line( &str[ 0 ], timeout, max_chars, p_progress );

      str.erase( len );

      return len;
   }
   else
      return read_line( 0, timeout, max_chars, p_progress, &str );
}

int tcp_socket::read_line( char* p_data, size_t timeout, int max_chars, progress* p_progress, string* p_str )
{
   int n = 0, o = 0;
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

         ++num_read_lines;

         break;
      }

      if( lb != '\0' )
      {
         if( !max_chars || o < max_chars )
         {
            if( p_str )
               *p_str += lb;

            if( p_data )
               *( p_data + o ) = lb;

            o++;
         }
         else
            throw runtime_error( "max. line length exceeded" );
      }

      lb = b;
   }

   if( p_progress && o )
   {
      if( p_str )
         p_progress->output_progress( ">R> " + to_comparable_string( num_read_lines, false, 9 ) + ' ' + *p_str );
      else
         p_progress->output_progress( ">R> " + to_comparable_string( num_read_lines, false, 9 ) + ' ' + string( p_data, o ) );
   }

   return n;
}

int tcp_socket::write_line( const string& str, size_t timeout, progress* p_progress )
{
   int n = 0;

   bool truncate = false;
   bool terminated = false;

   string::size_type len( str.length( ) );

   if( len >= 1 && str[ len - 1 ] == '\n' )
   {
      if( len < 2 || str[ len - 2 ] != '\r' )
         truncate = true;
      else
         terminated = true;
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

   if( len && p_data )
      n = write_line( len, p_data, timeout, p_progress );

   return n;
}

int tcp_socket::write_line( int len, const char* p_data, size_t timeout, progress* p_progress )
{
   int n = 0;

   if( len )
   {
      if( len < 2 || !( p_data[ len - 2 ] == '\r' && p_data[ len - 1 ] == '\n' ) )
         throw runtime_error( "write_line data must have CRLF termination" );

      if( p_progress )
      {
         string write_string( "<W< " );

         if( !get_delay( ) )
            write_string = string( "<W<!" );

         write_string += to_comparable_string( num_write_lines + 1, false, 9 ) + ' ';

         p_progress->output_progress( write_string + string( p_data, len - 2 ) );
      }

      n = send_n( ( const unsigned char* )p_data, len, timeout );

      if( n == len )
         ++num_write_lines;
   }

   return n;
}

udp_socket::udp_socket( )
 : socket_base( )
{
}

udp_socket::udp_socket( SOCKET socket )
 : socket_base( socket )
{
}

bool udp_socket::open( )
{
   if( socket == INVALID_SOCKET )
      socket = ::socket( AF_INET, SOCK_DGRAM, 0 );

   return ( socket != INVALID_SOCKET );
}

void udp_socket::on_bind( )
{
   set_non_blocking( );
}

int udp_socket::recv_from( unsigned char* p_buffer, size_t buflen, size_t timeout, progress* p_progress )
{
   bool okay = true;

   timed_out = false;

   if( timeout )
      okay = has_input( timeout );

   int n = 0;
   socklen_t addrlen;

   if( !okay )
      timed_out = true;
   else
      n = ::recvfrom( socket, p_buffer, buflen, 0, 0, 0 );

   if( n > 0 && p_progress )
   {
      string suffix;

      if( n > c_max_progress_output_bytes )
         suffix = "...[" + format_bytes( n ) + ']';

      p_progress->output_progress( ">R>~" + string( ( const char* )p_buffer, min( n, c_max_progress_output_bytes ) ) + suffix );
   }

   return n;
}

int udp_socket::recv_from( unsigned char* p_buffer, size_t buflen, ip_address& addr, size_t timeout, progress* p_progress )
{
   bool okay = true;

   timed_out = false;

   if( timeout )
      okay = has_input( timeout );

   int n = 0;
   socklen_t addrlen;

   if( !okay )
      timed_out = true;
   else
      n = ::recvfrom( socket, p_buffer, buflen, 0, ( struct sockaddr* )&addr, ( socklen_t* )&addrlen );

   if( n > 0 && p_progress )
   {
      string suffix;

      if( n > c_max_progress_output_bytes )
         suffix = "...[" + format_bytes( n ) + ']';

      p_progress->output_progress( ">R>~" + string( ( const char* )p_buffer, min( n, c_max_progress_output_bytes ) ) + suffix );
   }

   return n;
}

int udp_socket::send_to( const unsigned char* p_buffer, size_t buflen, const ip_address& addr, size_t timeout, progress* p_progress )
{
   bool okay = true;

   timed_out = false;

   if( timeout )
      okay = can_output( timeout );

   int n = 0;

   if( !okay )
      timed_out = true;
   else
      n = ::sendto( socket, p_buffer, buflen, 0, ( const struct sockaddr* )&addr, ( socklen_t )sizeof( addr ) );

   if( n > 0 && p_progress )
   {
      string suffix;
      string write_string( "<W<~" );

      if( n > c_max_progress_output_bytes )
         suffix = "...[" + format_bytes( n ) + ']';

      p_progress->output_progress( write_string + string( ( const char* )p_buffer, min( n, c_max_progress_output_bytes ) ) + suffix );
   }

   return n;
}

size_t file_transfer( const string& name, tcp_socket& s,
 ft_direction d, size_t max_size, const char* p_ack_message,
 ft_extra_info* p_ft_extra, progress* p_progress, udp_helper* p_udp_helper )
{
   size_t total_size = 0;

   bool not_base64 = false;
   bool max_size_exceeded = false;
   bool received_empty_file = false;

   bool has_file_name = !name.empty( );

   if( !max_size )
      throw runtime_error( "invalid zero max_size in file_transfer" );

   size_t line_timeout = 0;
   size_t initial_timeout = 0;

   size_t max_line_size = c_default_line_size;

   unsigned char* p_buffer = 0;
   unsigned int buffer_size = 0;

   string extra_header, unexpected_data;

   if( !p_ack_message )
      throw runtime_error( "invalid null ack message in file_transfer" );

   string ack_message_str( p_ack_message );
   string ack_message_line( ack_message_str + "\r\n" );

   string ack_message_skip;

   if( p_ft_extra )
   {
      line_timeout = p_ft_extra->line_timeout;
      initial_timeout = p_ft_extra->initial_timeout;

      extra_header = p_ft_extra->extra_header;

      if( p_ft_extra->max_line_size )
      {
         max_line_size = p_ft_extra->max_line_size;

         if( max_line_size < c_default_line_size )
            throw runtime_error( "unexpected p_ft_extra->max_line_size < c_default_line_size" );
      }

      if( p_ft_extra->p_buffer )
      {
         p_buffer = p_ft_extra->p_buffer;
         buffer_size = p_ft_extra->buffer_size;
      }

      if( p_ft_extra->p_ack_skip_message )
         ack_message_skip = string( p_ft_extra->p_ack_skip_message );
   }

   int ack_msg_line_len = ( int )ack_message_line.length( );

   s.set_no_delay( );
 
   if( d == e_ft_direction_send )
   {
      ifstream inpf;
      stringstream ss;

      istream* p_istream = 0;

      bool has_prefix_char = ( p_ft_extra && p_ft_extra->p_prefix_char && *p_ft_extra->p_prefix_char );

      if( buffer_size || ( name.empty( ) && has_prefix_char ) )
         total_size = buffer_size;
      else
      {
         if( !file_exists( name ) )
            throw runtime_error( "file '" + name + "' not found" );

         total_size = file_size( name );

         inpf.open( name.c_str( ), ios::binary );

         if( !inpf )
            throw runtime_error( "file '" + name + "' could not be opened for input" );

         p_istream = &inpf;
      }

      size_t max_unencoded = base64::decode_size( max_line_size, true );

      unique_ptr< char > up_buf1( new char[ max_line_size ] );
      unique_ptr< char > up_buf2( new char[ max_line_size ] );

      bool is_first = true;

      if( has_prefix_char )
         total_size += 1;

      size_t total_encoded = base64::encode_size( total_size );

      string next, size_info( to_string( total_size ) + ':' );

      if( total_encoded < max_line_size )
         max_line_size = total_encoded;

      size_info += to_string( max_line_size ) + string( c_base64_format );

      if( !extra_header.empty( ) )
         size_info += c_extra_separator + extra_header;

      bool append_content_to_size_info = false;

      if( ( total_size + size_info.size( ) ) <= c_append_to_size_info )
      {
         size_info += c_app_suffix;

         append_content_to_size_info = true;

         if( !p_istream )
         {
            p_istream = &ss;
            ss << string( ( const char* )p_buffer, buffer_size );
         }

         size_t offset = 0;

         if( has_prefix_char )
            ++offset;

         string content;

         if( total_size > offset )
         {
            string data( total_size - offset, '\0' );

            if( !p_istream->read( &data[ 0 ], total_size - offset ) )
               throw runtime_error( "unexpected failure to read data for size info append in file transfer" );

            content = base64::encode( has_prefix_char ? data : data.substr( 1 ) );

            if( !has_prefix_char )
               size_info += hex_encode( ( unsigned char* )&data[ 0 ], 1 );
         }

         // NOTE: Prefix the base64 encoded content with the file type and extra in hex.
         if( has_prefix_char )
            size_info += hex_encode( p_ft_extra->p_prefix_char, 1 );

         size_info += content;
      }
      else if( p_udp_helper )
         size_info += c_udp_suffix;

      while( true )
      {
         if( is_first )
         {
            s.write_line( size_info, initial_timeout, p_progress );

            if( append_content_to_size_info )
               break;

            s.read_line( next, initial_timeout, c_default_line_size, p_progress );

            if( s.had_timeout( ) )
               throw runtime_error( "timeout occurred reading send response for size_info in file transfer" );

            // NOTE: If the receiver has already got the file (or a peer session has been stopped)
            // then just quietly end the transfer.
            if( !next.empty( ) && ( next == c_bye || next == ack_message_skip ) )
               break;

            string extra;

            string::size_type pos = next.find( ':' );

            if( pos != string::npos && next.find( ack_message_str ) == 0 )
            {
               extra = next.substr( pos + 1 );
               next.erase( pos );
            }
            
            if( next != ack_message_str )
            {
               // NOTE: If "Error/error" is found in the message then just throw it as is.
               if( lower( next ).find( "error" ) != string::npos )
                  throw runtime_error( next );
               else if( next.empty( ) )
                  throw runtime_error( "unexpected first empty ack response in file transfer" );
               else
                  throw runtime_error( "was expecting '" + ack_message_str + "' but found '" + next + "'" );
            }

            if( !p_istream )
            {
               p_istream = &ss;
               ss << string( ( const char* )p_buffer, buffer_size );
            }

            if( !extra.empty( ) )
            {
               size_t start_offset = from_string< size_t >( extra );

               if( p_udp_helper && total_size )
               {
                  p_udp_helper->had_recv_help = true;
                  p_udp_helper->recv_percent = ( ( float )start_offset / ( float )total_size ) * 100.0;
               }

               if( start_offset == total_size )
                  break;
               else if( start_offset )
               {
                  is_first = false;

                  if( has_prefix_char )
                     --start_offset;

                  p_istream->seekg( start_offset, ios::beg );
               }
            }
         }

         size_t count = max_unencoded;

         size_t offset = 0;

         if( is_first && has_prefix_char )
         {
            ++offset;
            *( up_buf1.get( ) ) = *p_ft_extra->p_prefix_char;
         }

         is_first = false;

         if( !p_istream->read( up_buf1.get( ) + offset, max_unencoded - offset ) )
            count = p_istream->gcount( ) + offset;

         if( !count )
            break;

         size_t enc_len = 0;

         base64::encode( ( const unsigned char* )up_buf1.get( ), count, up_buf2.get( ), &enc_len );

         while( enc_len < max_line_size )
            *( up_buf2.get( ) + enc_len++ ) = '.';

         if( s.send_n( ( unsigned char* )up_buf2.get( ), max_line_size, line_timeout, p_progress ) != max_line_size )
            throw runtime_error( "unable to send " + to_string( max_line_size ) + " bytes using send_n in file transfer" );

         if( p_istream->eof( ) )
            break;

         s.read_line( next, line_timeout, c_default_line_size, p_progress );

         if( s.had_timeout( ) )
            throw runtime_error( "timeout occurred reading send response for next line in file transfer" );

         if( next != ack_message_str )
         {
            // NOTE: If "Error/error" is found in the message then just throw it as is.
            if( lower( next ).find( "error" ) != string::npos )
               throw runtime_error( next );
            else if( next.empty( ) )
               throw runtime_error( "unexpected empty ack response for send in file transfer" );
            else
               throw runtime_error( "was expecting '" + ack_message_str + "' but found '" + next + "'" );
         }
      }
   }
   else
   {
      bool use_recv_buffer = ( buffer_size != 0 );

      if( use_recv_buffer && ( buffer_size < max_size ) )
         throw runtime_error( "buffer_size < max_size for file_transfer" );
   
      ios::openmode oflags = ios::binary;

      if( d == e_ft_direction_recv_app )
         oflags |= ios::app;

      ofstream outf;

      if( has_file_name )
      {
         outf.open( name.c_str( ), oflags );

         if( !outf )
            throw runtime_error( "file '" + name + "' could not be opened for output" );
      }

      size_t written = 0;

      bool is_first = true;

      string next, decoded;

      while( true )
      {
         if( is_first )
         {
            scoped_empty_file_delete delete_empty_file( name );

            int received = s.read_line( next, initial_timeout, max_line_size, p_progress );

            if( !received || s.had_timeout( ) )
               throw runtime_error( "timeout occurred reading header line for file transfer" );

            // NOTE: This can occur in the case where a peer session has been stopped.
            if( next == c_bye )
               break;

            string::size_type pos = next.find( ':' );

            if( pos == string::npos )
            {
               // NOTE: If "Error/error" is found in the message then just throw it as is.
               if( lower( next ).find( "error" ) != string::npos )
                  throw runtime_error( next );

               s.write_line( "error: invalid file transfer header line", line_timeout, p_progress );
               throw runtime_error( "invalid file transfer header line for recv" );
            }

            // FUTURE: A ".raw" format should be added to support binary file transfers.
            string::size_type fpos = next.find( c_base64_format );

            if( ( fpos == string::npos ) || ( fpos < pos ) )
            {
               s.write_line( "error: invalid file transfer header (unknown format)", line_timeout, p_progress );
               throw runtime_error( "invalid file transfer header (unknown format) for recv" );
            }

            string::size_type apos = next.find( c_app_suffix );

            // NOTE: For very small files the content is appended to the size info line.
            if( apos != string::npos )
            {
               string content( next.substr( apos + 1 ) );

               if( content.size( ) < 2 )
                  throw runtime_error( "unexpected invalid content in file transfer" );

               next.erase( apos );

               string::size_type xpos = next.find( c_extra_separator );

               if( xpos != string::npos )
               {
                  if( p_ft_extra )
                     p_ft_extra->extra_header = next.substr( xpos + 1 );

                  next.erase( xpos );
               }

               // NOTE: Encoded content is expected to be prefixed by the file type and extra in hex.
               string type_and_extra( hex_decode( content.substr( 0, 2 ) ) );

               content.erase( 0, 2 );

               if( !content.empty( ) && !base64::valid_characters( content ) )
               {
                  not_base64 = true;
                  unexpected_data = next;

                  break;
               }

               string data;

               if( !p_ft_extra || !p_ft_extra->p_prefix_char )
                  data += type_and_extra;
               else
                  *p_ft_extra->p_prefix_char = type_and_extra[ 0 ];

               if( !content.empty( ) )
                  data += base64::decode( content );

               if( p_buffer )
                  memcpy( p_buffer, &data[ 0 ], data.length( ) );

               if( has_file_name && !data.empty( ) && !outf.write( &data[ 0 ], data.length( ) ) )
                  throw runtime_error( "unexpected error writing to file '" + name + "'" );

               if( has_file_name )
               {
                  outf.close( );

                  if( data.empty( ) )
                     received_empty_file = true;
               }

               total_size = from_string< int64_t >( next.substr( 0, pos ) );

               if( total_size > max_size )
                  throw runtime_error( "total_size " + to_string( total_size ) + " exceeds max_size " + to_string( max_size ) );

               break;
            }

            bool had_sent_udp = false;

            if( next[ next.length( ) - 1 ] == c_udp_suffix )
            {
               had_sent_udp = true;
               next.erase( next.length( ) - 1 );
            }

            string::size_type xpos = next.find( c_extra_separator );

            if( xpos != string::npos )
            {
               if( p_ft_extra )
                  p_ft_extra->extra_header = next.substr( xpos + 1 );

               next.erase( xpos );
            }

            next.erase( fpos );

            total_size = from_string< int64_t >( next.substr( 0, pos ) );

            size_t chunk_size = from_string< size_t >( next.substr( pos + 1 ) );

            if( !total_size || !chunk_size
             || ( chunk_size > max_line_size ) )
            {
               s.write_line( "error: invalid file transfer size info", line_timeout, p_progress );
               throw runtime_error( "invalid file transfer size info for recv" );
            }

            // NOTE: Don't delete the file if made it this far.
            delete_empty_file.file_name.erase( );

            max_line_size = chunk_size;
            next.resize( max_line_size );

            size_t start_offset = 0;

            if( had_sent_udp && p_udp_helper && use_recv_buffer && ( ack_message_str != ack_message_skip ) )
            {
               p_udp_helper->recv_data( p_buffer, buffer_size, start_offset );

               if( start_offset && has_file_name && !outf.write( ( const char* )p_buffer, start_offset ) )
                  throw runtime_error( "unexpected error writing to file '" + name + "'" );

               string ack_with_start_offset( ack_message_str + ':' + to_string( start_offset ) );

               s.write_line( ack_with_start_offset, line_timeout, p_progress );

               if( start_offset )
               {
                  is_first = false;

                  if( p_ft_extra && p_ft_extra->p_prefix_char )
                     *p_ft_extra->p_prefix_char = *p_buffer;

                  if( start_offset == total_size )
                     break;

                  written += start_offset;
                  p_buffer += start_offset;
               }
            }
            else
               s.write_line( ack_msg_line_len, &ack_message_line[ 0 ], line_timeout, p_progress );
         }

         // NOTE: If the receiver has already got the file (or a peer session has been stopped)
         // then just quietly end the transfer.
         if( ( ack_message_str == c_bye ) || ( ack_message_str == ack_message_skip ) )
            break;

         int received = s.recv_n( ( unsigned char* )&next[ 0 ], max_line_size, line_timeout, p_progress );

         if( !received || s.had_timeout( ) )
            throw runtime_error( "timeout occurred reading next line for file transfer" );

         if( received < max_line_size )
            next.erase( received );

         string::size_type pos = next.find( '.' );

         if( pos != string::npos )
            next.erase( pos );

         if( !base64::valid_characters( next ) )
         {
            not_base64 = true;
            unexpected_data = next;

            break;
         }

         decoded.resize( max_line_size );
         size_t len = base64::decode( next, ( unsigned char* )&decoded[ 0 ], max_line_size );

         decoded.erase( len );

         size_t offset = 0;

         if( is_first && p_ft_extra && p_ft_extra->p_prefix_char )
         {
            offset = 1;
            --total_size;

            *p_ft_extra->p_prefix_char = decoded[ 0 ];
         }

         is_first = false;

         size_t decoded_size = decoded.length( ) - offset;

         if( ( written + decoded_size ) > max_size )
         {
            max_size_exceeded = true;
            break;
         }

         if( has_file_name && !outf.write( &decoded[ offset ], decoded_size ) )
            throw runtime_error( "unexpected error writing to file '" + name + "'" );

         written += decoded_size;

         if( use_recv_buffer )
         {
            memcpy( p_buffer, &decoded[ offset ], decoded_size );
            p_buffer += decoded_size;
         }

         if( written >= total_size )
            break;

         s.write_line( ack_msg_line_len, &ack_message_line[ 0 ], line_timeout, p_progress );
      }

      if( has_file_name )
         outf.close( );
   }

   if( received_empty_file )
      file_touch( name, 0, true );
   else if( not_base64 || max_size_exceeded )
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

   return total_size;
}

void recv_test_datagrams( size_t num, int port, int sock, string& str, size_t timeout )
{
   udp_socket s( sock );

   ip_address address( port );

   unsigned char buffer[ c_test_buf_size ];

   size_t found = 0;

   for( size_t i = 0; i < ( num * 3 ); i++ )
   {
      int len = s.recv_from( buffer, sizeof( buffer ), address, timeout );

      if( len > 0 )
      {
         if( !str.empty( ) )
            str += '\n';

         string next( ( size_t )len, '\0' );

         memcpy( &next[ 0 ], buffer, len );

         next += " <== " + address.get_addr_string( );

         str += next;

         if( ++found >= num )
            break;
      }
   }
}

void send_test_datagrams( size_t num, const string& host_name, int port, size_t timeout, udp_socket* p_socket, bool reverse )
{
   unique_ptr< udp_socket > up_udp_socket;

   if( !p_socket || !*p_socket )
   {
      up_udp_socket.reset( new udp_socket );

      if( !up_udp_socket->open( ) )
         throw runtime_error( "unable to open udp_socket in send_test_datagrams" );

      up_udp_socket->set_reuse_addr( );

      p_socket = up_udp_socket.get( );
   }

   ip_address address( host_name.c_str( ), port );

   string prefix( get_environment_variable( c_env_var_name_slotx ) );

   size_t x = 0;

   if( reverse )
      x = num - 1;

   int pause = 100;

   if( num <= 10 )
      pause = 2;
   else if( num <= 100 )
      pause = 10;

   for( size_t i = 0; i < num; i++ )
   {
      string data( to_comparable_string( x, false, 3 ) );

      if( !reverse )
         ++x;
      else
         --x;

      if( !prefix.empty( ) )
         data = prefix + ':' + data + ':';

      while( data.length( ) < 64 )
         data += reverse ? '<' : '>';

      int n = p_socket->send_to( ( unsigned char* )data.data( ), data.length( ), address, timeout );

      if( n == 0 )
         throw runtime_error( "unable to send a UDP datagram to " + host_name );

      // NOTE: This ten millisecond sleep is provided in order to help prevent packet loss.
      if( i % pause == 0 )
         msleep( 10 );
   }
}
