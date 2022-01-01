// Copyright (c) 2003-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2022 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef SOCKETS_H
#  define SOCKETS_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#     ifdef _WIN32
#        define NOMINMAX
#        include <winsock2.h>
#     else
#        include <sys/socket.h>
#        include <arpa/inet.h>
#     endif
#  endif

struct progress;

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
typedef int socklen_t;
#else
#  define SOCKET int
#  define INVALID_SOCKET -1
#endif

const size_t c_default_connect_timeout = 30000;

class ip_address : public sockaddr_in
{
   public:
   ip_address( int port = 0 );
   ip_address( const char* p_address, int port = 0 );
   ip_address& operator =( const char* p_address );

   std::string get_addr_string( ) const;

   protected:
   void resolve( const char* p_address, int port = 0 );
};

class tcp_socket
{
   typedef void ( tcp_socket::*bool_type )( ) const;
   void this_type_does_not_support_comparisons( ) const { }

   public:
   tcp_socket( );
   tcp_socket( SOCKET socket );

   virtual ~tcp_socket( );

   bool open( );
   void close( );

   bool bind( const ip_address& addr );
   bool connect( const ip_address& addr, size_t timeout = c_default_connect_timeout );

   bool listen( );
   SOCKET accept( ip_address& addr, size_t timeout = 0 ) const;

   bool get_delay( );
   bool set_delay( );
   bool set_no_delay( );

   bool set_no_linger( );
   bool set_reuse_addr( );

   bool has_input( size_t timeout = 0 ) const;
   bool can_output( size_t timeout = 0 ) const;

   virtual int recv( unsigned char* buf, int buflen, size_t timeout = 0 );
   virtual int send( const unsigned char* buf, int buflen, size_t timeout = 0 );

   int recv_n( unsigned char* buf, int buflen, size_t timeout = 0, progress* p_progress = 0 );
   int send_n( const unsigned char* buf, int buflen, size_t timeout = 0, progress* p_progress = 0 );

   int read_line( std::string& str, size_t timeout = 0, int max_chars = 0, progress* p_progress = 0 );
   int read_line( char* p_data, size_t timeout = 0, int max_chars = 0, progress* p_progress = 0, std::string* p_str = 0 );

   int write_line( const std::string& str, size_t timeout = 0, progress* p_progress = 0 );
   int write_line( int len, const char* p_data, size_t timeout = 0, progress* p_progress = 0 );

   bool get_option( int type, int opt, char* p_buffer, socklen_t& buflen );
   bool set_option( int type, int opt, const char* p_buffer, socklen_t buflen );

   bool had_timeout( ) const { return timed_out; }
   bool had_blank_line( ) const { return blank_line; }

   bool okay( ) const { return socket != INVALID_SOCKET; }

   operator bool_type( ) const
   {
      if( socket == INVALID_SOCKET )
         return 0;
      else
         return &tcp_socket::this_type_does_not_support_comparisons;
   }

   template< typename T > bool operator ==( const T& rhs ) const
   {
      this_type_does_not_support_comparisons( );
      return false;
   }

   template< typename T > bool operator !=( const T& rhs ) const
   {
      this_type_does_not_support_comparisons( );
      return false;
   }

   private:
   bool blank_line;

   SOCKET socket;

   tcp_socket( const tcp_socket& );
   tcp_socket& operator =( const tcp_socket& );

   protected:
   bool timed_out;
   SOCKET get_socket( ) const { return socket; }

   // FUTURE: Need to add a member in order to detect the "would block" status before allowing these to be public.
   bool set_blocking( );
   bool set_non_blocking( );
};

enum ft_direction
{
   e_ft_direction_send,
   e_ft_direction_recv,
   e_ft_direction_recv_app
};

void file_transfer(
 const std::string& name, tcp_socket& s, ft_direction d, size_t max_size,
 const char* p_ack_message, size_t initial_timeout = 0, size_t line_timeout = 0,
 size_t max_line_size = 0, unsigned char* p_prefix_char = 0, unsigned char* p_buffer = 0,
 unsigned int buffer_size = 0, progress* p_progress = 0, const char* p_ack_skip_message = 0 );

#endif
