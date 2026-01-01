// Copyright (c) 2003-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef SOCKETS_H
#  define SOCKETS_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#     ifdef __GNUG__
#        include <sys/socket.h>
#        include <arpa/inet.h>
#     endif
#  endif

struct progress;

#  define SOCKET int
#  define INVALID_SOCKET -1

const size_t c_default_accept_timeout = 30000;
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

class socket_base
{
   typedef void ( socket_base::*bool_type )( ) const;
   void this_type_does_not_support_comparisons( ) const { }

   public:
   socket_base( );
   socket_base( SOCKET socket, bool close_in_dtor = false );

   virtual ~socket_base( );

   virtual bool get_delay( ) { return false; }

   virtual bool open( ) = 0;
   virtual void close( );

   bool bind( const ip_address& addr );

   SOCKET accept( ip_address& addr, size_t timeout = c_default_accept_timeout ) const;

   bool connect( const ip_address& addr, size_t timeout = c_default_connect_timeout );

   virtual void on_bind( ) { }

   bool listen( );

   bool set_no_linger( );
   bool set_reuse_addr( );

   bool has_input( size_t timeout = 0 ) const;
   bool can_output( size_t timeout = 0 ) const;

   int peek( unsigned char* p_buf, int buflen, size_t timeout = 0 );

   virtual int recv( unsigned char* p_buf, int buflen, size_t timeout = 0 );
   virtual int send( const unsigned char* p_buf, int buflen, size_t timeout = 0 );

   int recv_n( unsigned char* buf, int buflen, size_t timeout = 0, progress* p_progress = 0 );
   int send_n( const unsigned char* buf, int buflen, size_t timeout = 0, progress* p_progress = 0 );

   bool get_option( int type, int opt, char* p_buffer, socklen_t& buflen );
   bool set_option( int type, int opt, const char* p_buffer, socklen_t buflen );

   bool had_timeout( ) const { return timed_out; }

   bool okay( ) const { return socket != INVALID_SOCKET; }

   operator bool_type( ) const
   {
      if( socket == INVALID_SOCKET )
         return 0;
      else
         return &socket_base::this_type_does_not_support_comparisons;
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

   SOCKET get_socket( ) const { return socket; }

   private:
   socket_base( const socket_base& );
   socket_base& operator =( const socket_base& );

   protected:
   SOCKET socket;

   bool timed_out;
   bool close_in_dtor;

   // FUTURE: Need to add a member in order to detect the "would block" status before allowing these to be public.
   bool set_blocking( );
   bool set_non_blocking( );
};

class tcp_socket : public socket_base
{
   public:
   tcp_socket( );
   tcp_socket( SOCKET );

   bool open( );

   bool get_delay( );

   bool set_delay( );
   bool set_no_delay( );

   int read_line( std::string& str,
    size_t timeout = 0, int max_chars = 0, progress* p_progress = 0 );

   int read_line( char* p_data,
    size_t timeout = 0, int max_chars = 0, progress* p_progress = 0, std::string* p_str = 0 );

   int write_line( const std::string& str, size_t timeout = 0, progress* p_progress = 0 );
   int write_line( int len, const char* p_data, size_t timeout = 0, progress* p_progress = 0 );

   bool had_blank_line( ) const { return blank_line; }

   size_t get_num_read_lines( ) const { return num_read_lines; }
   size_t get_num_write_lines( ) const { return num_write_lines; }

   private:
   bool blank_line;

   size_t num_read_lines;
   size_t num_write_lines;
};

class udp_socket : public socket_base
{
   public:
   udp_socket( );
   udp_socket( SOCKET );

   bool open( );

   void on_bind( );

   int recv_from( unsigned char* p_buffer,
    size_t buflen, size_t timeout = 0, progress* p_progress = 0 );

   int recv_from( unsigned char* p_buffer, size_t buflen,
    ip_address& addr, size_t timeout = 0, progress* p_progress = 0 );

   int send_to( const unsigned char* p_buffer, size_t buflen,
    const ip_address& addr, size_t timeout = 0, progress* p_progress = 0 );
};

struct udp_helper
{
   udp_helper( const std::string& hash ) : hash( hash ), had_recv_help( false ), recv_percent( 0 ) { }

   virtual void recv_data( unsigned char* p_buffer, unsigned int buffer_size, size_t& start_offset ) { }

   std::string hash;

   bool had_recv_help;
   float recv_percent;
};

enum ft_direction
{
   e_ft_direction_send,
   e_ft_direction_recv,
   e_ft_direction_recv_app
};

struct ft_extra_info
{
   ft_extra_info( size_t initial_timeout = 0,
    size_t line_timeout = 0, size_t max_line_size = 0, unsigned char* p_prefix_char = 0,
    unsigned char* p_buffer = 0, unsigned int buffer_size = 0, const char* p_ack_skip_message = 0 )
    :
    p_ack_message( p_ack_message ),
    initial_timeout( initial_timeout ),
    line_timeout( line_timeout ),
    max_line_size( max_line_size ),
    p_prefix_char( p_prefix_char ),
    p_buffer( p_buffer ),
    buffer_size( buffer_size ),
    p_ack_skip_message( p_ack_skip_message )
   {
   }

   std::string extra_header;

   const char* p_ack_message;
   const char* p_ack_skip_message;

   unsigned char* p_buffer;
   unsigned char* p_prefix_char;

   size_t line_timeout;
   size_t max_line_size;
   size_t initial_timeout;

   unsigned int buffer_size;
};

size_t file_transfer( const std::string& name,
 tcp_socket& s, ft_direction d, size_t max_size, const char* p_ack_message,
 ft_extra_info* p_ft_extra = 0, progress* p_progress = 0, udp_helper* p_udp_helper = 0 );

void recv_test_datagrams( size_t num, int port, int sock, std::string& str, size_t timeout );

void send_test_datagrams( size_t num, const std::string& host_name,
 int port, size_t timeout, udp_socket* p_socket = 0, bool reverse = false );

#endif
