// Copyright (c) 2003-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2021 CIYAM Developers
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
#include "format.h"
#include "progress.h"
#include "utilities.h"

using namespace std;

namespace
{

const int c_default_line_size = 1024;

const int c_max_progress_output_bytes = 132;

const char* const c_bye = "bye";
const char* const c_base64_format = ".b64";

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

int tcp_socket::recv_n( unsigned char* buf, int buflen, size_t timeout, progress* p_progress )
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

   if( p_progress && rcvd )
   {
      string suffix;

      if( rcvd > c_max_progress_output_bytes )
         suffix = "...[" + format_bytes( rcvd ) + ']';

      p_progress->output_progress( ">R> " + string( ( const char* )buf, min( rcvd, c_max_progress_output_bytes ) ) + suffix );
   }

   return rcvd;
}

int tcp_socket::send_n( const unsigned char* buf, int buflen, size_t timeout, progress* p_progress )
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

   if( p_progress )
   {
      string suffix;
      string write_string( "<W< " );

      if( !get_delay( ) )
         write_string = string( "<W<!" );

      if( sent > c_max_progress_output_bytes )
         suffix = "...[" + format_bytes( sent ) + ']';

      p_progress->output_progress( write_string + string( ( const char* )buf, min( sent, c_max_progress_output_bytes ) ) + suffix );
   }

   return sent;
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
         p_progress->output_progress( ">R> " + *p_str );
      else
         p_progress->output_progress( ">R> " + string( p_data, o ) );
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

         p_progress->output_progress( write_string + string( p_data, len - 2 ) );
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
 const char* p_ack_message, size_t initial_timeout, size_t line_timeout,
 size_t max_line_size, unsigned char* p_prefix_char, unsigned char* p_buffer,
 unsigned int buffer_size, progress* p_progress, const char* p_ack_skip_message )
{
   bool not_base64 = false;
   bool max_size_exceeded = false;

   if( !max_line_size )
      max_line_size = c_default_line_size;

   string unexpected_data;

   if( !p_ack_message )
      throw runtime_error( "invalid null ack message in file_transfer" );

   string ack_message_str( p_ack_message );
   string ack_message_line( ack_message_str + "\r\n" );

   string ack_message_skip;

   if( p_ack_skip_message )
      ack_message_skip = string( p_ack_skip_message );

   int ack_msg_line_len = ( int )ack_message_line.length( );

   s.set_no_delay( );
 
   if( d == e_ft_direction_send )
   {
      ifstream inpf;
      stringstream ss;

      int64_t total_size = 0;
      istream* p_istream = 0;

      if( p_buffer && buffer_size )
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

      bool has_prefix_char = ( p_prefix_char && *p_prefix_char );

      size_t max_unencoded = base64::decode_size( max_line_size, true );

      auto_ptr< char > ap_buf1( new char[ max_line_size ] );
      auto_ptr< char > ap_buf2( new char[ max_line_size ] );

      string next;
      bool is_first = true;

      if( has_prefix_char )
         total_size += 1;

      size_t total_encoded = base64::encode_size( total_size );

      string size_info( to_string( total_size ) + ':' );

      if( total_encoded < max_line_size )
         max_line_size = total_encoded;

      size_info += to_string( max_line_size ) + string( c_base64_format );

      while( true )
      {
         if( is_first )
         {
            s.write_line( size_info, initial_timeout, p_progress );

            s.read_line( next, initial_timeout, c_default_line_size, p_progress );

            if( s.had_timeout( ) )
               throw runtime_error( "timeout occurred reading send response for size_info in file transfer" );

            // NOTE: If the receiver has already got the file (or a peer session has been stopped) then quietly end the transfer.
            if( !next.empty( ) && ( next == c_bye || next == ack_message_skip ) )
               break;

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
         }

         size_t count = max_unencoded;
         size_t offset = 0;

         if( is_first && has_prefix_char )
         {
            ++offset;
            *( ap_buf1.get( ) ) = *p_prefix_char;
         }

         is_first = false;

         if( !p_istream->read( ap_buf1.get( ) + offset, max_unencoded - offset ) )
            count = p_istream->gcount( ) + offset;

         if( !count )
            break;

         size_t enc_len = 0;

         base64::encode( ( const unsigned char* )ap_buf1.get( ), count, ap_buf2.get( ), &enc_len );

         while( enc_len < max_line_size )
            *( ap_buf2.get( ) + enc_len++ ) = '.';

         if( s.send_n( ( unsigned char* )ap_buf2.get( ), max_line_size, line_timeout, p_progress ) != max_line_size )
            throw runtime_error( "unable to send " + to_string( max_line_size ) + " bytes using send_n in file transfer" );

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

         if( p_istream->eof( ) )
            break;
      }
   }
   else
   {
      bool use_recv_buffer = ( p_buffer && buffer_size );

      if( use_recv_buffer && buffer_size < max_size )
         throw runtime_error( "buffer_size < max_size for file_transfer" );
   
      ios::openmode oflags = ios::binary;

      if( d == e_ft_direction_recv_app )
         oflags |= ios::app;

      ofstream outf( name.c_str( ), oflags );
      if( !outf )
         throw runtime_error( "file '" + name + "' could not be opened for output" );

      size_t written = 0;
      bool is_first = true;

      int64_t total_size = 0;

      string next, decoded;

      unsigned char* p_buf = p_buffer;

      while( true )
      {
         if( is_first )
         {
            scoped_empty_file_delete delete_empty_file( name );

            s.read_line( next, initial_timeout, max_line_size, p_progress );

            if( s.had_timeout( ) )
               throw runtime_error( "timeout occurred reading headerline for file transfer" );

            // NOTE: If "Error/error" is found in the message then just throw it as is.
            if( lower( next ).find( "error" ) != string::npos )
               throw runtime_error( next );

            // NOTE: This can occur in the case where a peer session has been stopped.
            if( next == c_bye )
               break;

            // FUTURE: A ".bin" format should be added to support binary file transfers.
            string::size_type pos = next.find( ':' );
            string::size_type fpos = next.find( c_base64_format );

            if( pos == string::npos || fpos == string::npos || fpos < pos )
            {
               s.write_line( "error: invalid file transfer header line", line_timeout, p_progress );
               throw runtime_error( "invalid file transfer header line for recv" );
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

            s.write_line( ack_msg_line_len, &ack_message_line[ 0 ], line_timeout, p_progress );
         }

         // NOTE: If the receiver has already got the file (or a peer session has been stopped) then quietly end the transfer.
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

         if( is_first && p_prefix_char )
         {
            offset = 1;
            --total_size;
            *p_prefix_char = decoded[ 0 ];
         }

         is_first = false;

         size_t decoded_size = decoded.length( ) - offset;

         if( written + decoded_size > max_size )
         {
            max_size_exceeded = true;
            break;
         }

         if( !outf.write( &decoded[ offset ], decoded_size ) )
            throw runtime_error( "unexpected error writing to file '" + name + "'" );

         written += decoded_size;

         if( use_recv_buffer )
         {
            memcpy( p_buf, &decoded[ offset ], decoded_size );
            p_buf += decoded_size;
         }

         s.write_line( ack_msg_line_len, &ack_message_line[ 0 ], line_timeout, p_progress );

         if( written >= total_size )
            break;
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
