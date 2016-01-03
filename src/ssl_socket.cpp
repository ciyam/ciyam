// Copyright (c) 2010-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2016 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

//#define DEBUG

#include "ssl_socket.h"

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <cstdio>
#  include <iostream>
#  include <stdexcept>
#  ifdef __GNUG__
#     include <csignal>
#  endif
#endif

#include <openssl/crypto.h>

using namespace std;

namespace
{

SSL_CTX* p_ctx = 0;
const char* p_pass = 0;

#ifdef __GNUG__
void sigpipe_handle( int x ) { }
#endif

int password_cb( char* buf, int num, int rwflag, void* userdata )
{
   if( num < strlen( p_pass ) + 1 )
      return 0;

   strcpy( buf, p_pass );
   return strlen( p_pass );
}

#ifdef DEBUG
void print_cert_info( SSL* p_ssl )
{
   X509* p_cert = 0;
   X509_NAME* p_sub = 0;
   X509_NAME* p_issuer = 0;

   char buf[ 8192 ];

   cout << "Cipher: " << SSL_get_cipher( p_ssl ) << endl;

   cout << "Certificate information:\n";
   p_cert = SSL_get_peer_certificate( p_ssl );

   if( !p_cert )
   {
      cout << "No peer certificate found\n";
      return;
   }

   p_sub = X509_get_subject_name( p_cert );
   if( !p_sub )
      cout << "Could not find subject name in certificate" << endl;
   else
   {
      X509_NAME_oneline( p_sub, buf, sizeof( buf ) - 1 );
      cout << "Subject: " << buf << endl;
   }

   p_issuer = X509_get_issuer_name( p_cert );
   if( !p_issuer )
      cout << "Could not find issuer name in certificate" << endl;
   else
   {
      X509_NAME_oneline( p_issuer, buf, sizeof( buf ) - 1 );
      cout << "Issuer: " << buf << endl;
   }
}
#endif

}

static int g_server_session_id_context = 1;

#ifdef _WIN32
#  define MUTEX_TYPE HANDLE
#  define MUTEX_SETUP( x ) ( x ) = CreateMutex( 0, false, 0 )
#  define MUTEX_CLEANUP( x ) CloseHandle( x )
#  define MUTEX_LOCK( x ) WaitForSingleObject( ( x ), INFINITE )
#  define MUTEX_UNLOCK( x ) ReleaseMutex( x )
#  define THREAD_ID GetCurrentThreadId( )
#else
#  define MUTEX_TYPE pthread_mutex_t
#  define MUTEX_SETUP( x ) pthread_mutex_init( &( x ), 0 )
#  define MUTEX_CLEANUP( x ) pthread_mutex_destroy( &( x ) )
#  define MUTEX_LOCK( x ) pthread_mutex_lock( &( x ) )
#  define MUTEX_UNLOCK( x ) pthread_mutex_unlock( &( x ) )
#  define THREAD_ID pthread_self( )
#endif

static MUTEX_TYPE* gp_mutex_buf = 0;

static void locking_function( int mode, int n, const char* p_file, int line )
{
   ( void )p_file;
   ( void )line;

   if( mode & CRYPTO_LOCK )
      MUTEX_LOCK( gp_mutex_buf[ n ] );
   else
      MUTEX_UNLOCK( gp_mutex_buf[ n ] );
}

static unsigned long id_function( )
{
   return ( unsigned long )THREAD_ID;
}

bool thread_setup( )
{
   gp_mutex_buf = ( MUTEX_TYPE * )malloc( CRYPTO_num_locks( ) * sizeof( MUTEX_TYPE ) );

   if( !gp_mutex_buf )
      return false;

   for( int i = 0; i < CRYPTO_num_locks( ); i++ )
      MUTEX_SETUP( gp_mutex_buf[ i ] );

   CRYPTO_set_id_callback( id_function );
   CRYPTO_set_locking_callback( locking_function );

   return true;
}

bool thread_cleanup( )
{
   if( !gp_mutex_buf )
      return false;

   CRYPTO_set_id_callback( 0 );
   CRYPTO_set_locking_callback( 0 );

   for( int i = 0; i < CRYPTO_num_locks( ); i++ )
      MUTEX_CLEANUP( gp_mutex_buf[ i ] );

   free( gp_mutex_buf );
   gp_mutex_buf = 0;

   return true;
}

void init_ssl( const char* p_keyfile, const char* p_password, const char* p_CA_List, bool multi_threaded )
{
   if( p_ctx )
      throw runtime_error( "init_ssl has already been called" );

   bool okay = true;

   if( multi_threaded )
      okay = thread_setup( );

   if( okay )
      okay = SSL_library_init( );

   if( !okay )
      throw runtime_error( "OpenSSL initialisation failed" );

   SSL_load_error_strings( );

#ifdef __GNUG__
   signal( SIGPIPE, sigpipe_handle );
#endif

   const SSL_METHOD* p_meth( SSLv23_method( ) );
   p_ctx = SSL_CTX_new( p_meth );

   if( !( SSL_CTX_use_certificate_chain_file( p_ctx, p_keyfile ) ) )
      throw runtime_error( "init_ssl: can't read certificate file" );

   p_pass = p_password;
   SSL_CTX_set_default_passwd_cb( p_ctx, password_cb );

   if( !( SSL_CTX_use_PrivateKey_file( p_ctx, p_keyfile, SSL_FILETYPE_PEM ) ) )
      throw runtime_error( "init_ssl: can't read key file" );

   if( p_CA_List && !( SSL_CTX_load_verify_locations( p_ctx, p_CA_List, 0 ) ) )
      throw runtime_error( "init_ssl: can't read CA list" );

#if( OPENSSL_VERSION_NUMBER < 0x00905100L )
   SSL_CTX_set_verify_depth( p_ctx, 1 );
#endif

   SSL_CTX_set_session_id_context( p_ctx,
    ( const unsigned char* )&g_server_session_id_context, sizeof( g_server_session_id_context ) );
}

void term_ssl( )
{
   if( gp_mutex_buf )
      thread_cleanup( );

   SSL_CTX_free( p_ctx );
}

ssl_socket::ssl_socket( )
 : tcp_socket( ),
 p_ssl( 0 ),
 secure( false )
{
   p_ssl = SSL_new( p_ctx );
}

ssl_socket::ssl_socket( SOCKET socket )
 : tcp_socket( socket ),
 p_ssl( 0 ),
 secure( false )
{
   p_ssl = SSL_new( p_ctx );
}

ssl_socket::~ssl_socket( )
{
   if( p_ssl )
      SSL_free( p_ssl );
}

void ssl_socket::ssl_accept( )
{
   if( secure )
      throw runtime_error( "SSL handshake has already been performed" );

   SSL_set_fd( p_ssl, get_socket( ) );

   if( SSL_accept( p_ssl ) <= 0 )
      throw runtime_error( "SSL accept failure" );

   secure = true;
}

void ssl_socket::ssl_connect( )
{
   if( secure )
      throw runtime_error( "SSL handshake has already been performed" );

   SSL_set_fd( p_ssl, get_socket( ) );

   if( SSL_connect( p_ssl ) <= 0 )
      throw runtime_error( "SSL connect failure" );

   secure = true;
#ifdef DEBUG
   print_cert_info( p_ssl );
#endif
}

int ssl_socket::recv( unsigned char* buf, int buflen, size_t timeout )
{
   if( !secure )
      return tcp_socket::recv( buf, buflen, timeout );

   bool okay = true;

   timed_out = false;

   // NOTE: Need to test whether the SSL buffer is empty before testing the socket.
   if( timeout )
      okay = SSL_pending( p_ssl ) || has_input( timeout );

   int n = 0;
   if( !okay )
      timed_out = true;
   else
      n = SSL_read( p_ssl, ( char* )buf, buflen );

   return n;
}

int ssl_socket::send( const unsigned char* buf, int buflen, size_t timeout )
{
   if( !secure )
      return tcp_socket::send( buf, buflen, timeout );

   bool okay = true;

   timed_out = false;

   if( timeout )
      okay = can_output( timeout );

   int n = 0;
   if( !okay )
      timed_out = true;
   else
      n = SSL_write( p_ssl, ( const char* )buf, buflen );

   return n;
}

