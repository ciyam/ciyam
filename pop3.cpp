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

#ifdef __BORLANDC__
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <cstdlib>
#  include <iostream>
#  include <algorithm>
#  include <stdexcept>
#endif

#include "pop3.h"

#include "config.h"
#include "sockets.h"
#ifdef SSL_SUPPORT
#  include "ssl_socket.h"
#endif
#include "utilities.h"

//#define DEBUG
#define USE_NO_DELAY

using namespace std;

namespace
{

// Sample POP3 client protocol:
// USER username
// PASS password
// STAT       ;may produce the total # and size of messages
// LIST       ;will produce a list of message numbers and sizes
// TOP x n    ;where x is the message number and n is the number of body lines (use n=0 to only fetch headers)
// RETR n     ;retrieve a message
// DELE n     ;delete a message
// QUIT
//
// POP3: RFC 1939 (http://www.faqs.org/rfcs/rfc1939.html)
// STLS: RFC 2595 (http://www.ietf.org/rfc/rfc2595.txt)
//
// STLS POP3 client protocol:
// STLS
// ;negotiate SSL connection
// USER username
// PASS password
// STAT       ;may produce the total # and size of messages
// LIST       ;will produce a list of message numbers and sizes
// TOP x n    ;where x is the message number and n is the number of body lines (use n=0 to only fetch headers)
// RETR n     ;retrieve a message
// DELE n     ;delete a message
// QUIT

const size_t c_initial_timeout = 25000;
const size_t c_subsequent_timeout = 5000;
const size_t c_final_response_timeout = 500;

// NOTE: The standard POP3 port is 110. For SSL it is usually 995 and for TLS it's usually 110.
const int c_pop3_default_port = 110;

const char* const c_prefix_ok = "+OK";
const char* const c_prefix_err = "-ERR";

const char* const c_request_top = "TOP";
const char* const c_request_dele = "DELE";
const char* const c_request_list = "LIST";
const char* const c_request_pass = "PASS";
const char* const c_request_quit = "QUIT";
const char* const c_request_retr = "RETR";
const char* const c_request_stat = "STAT";
const char* const c_request_stls = "STLS";
const char* const c_request_user = "USER";

const char* const c_mime_header = "MIME-Version:";

const char* const c_response_multi_terminator = ".";

void send_simple_request( tcp_socket& socket, const string& request )
{
   if( socket.write_line( request ) <= 0 )
      throw runtime_error( "send failure for request..." );
}

string get_simple_response( tcp_socket& socket )
{
   string response_line;
   size_t timeout = c_initial_timeout;

   if( socket.read_line( response_line, timeout ) <= 0 )
      throw runtime_error( "recv failure for simple response" );

   string::size_type pos = response_line.find( c_prefix_err );
   if( pos != string::npos )
      throw runtime_error( response_line.substr( pos + strlen( c_prefix_err ) + 1 ) );

   if( response_line.size( ) >= strlen( c_prefix_ok )
    && response_line.substr( 0, strlen( c_prefix_ok ) ) != c_prefix_ok )
      throw runtime_error( response_line );

   return response_line;
}

void get_multi_line_reponse( tcp_socket& socket, ostream& os, bool* p_is_mime = 0 )
{
   string next_response_line;
   size_t timeout = c_initial_timeout;

   if( p_is_mime )
      *p_is_mime = false;

   bool finished_headers = false;
   while( true )
   {
      if( socket.read_line( next_response_line, timeout ) <= 0 )
      {
         // NOTE: If an empty line is received then continue to read.
         if( socket.had_blank_line( ) )
         {
            finished_headers = true;
            os << '\n';
#ifdef DEBUG
      cout << "\n";
#endif
            continue;
         }

         throw runtime_error( "recv failure for multi-line response" );
      }

      if( p_is_mime && !finished_headers )
      {
         if( lower( next_response_line ).find( lower( c_mime_header ) ) == 0 )
            *p_is_mime = true;
      }

#ifdef DEBUG
      cout << next_response_line << "\n";
#endif
      if( timeout == c_initial_timeout
       && next_response_line.size( ) >= strlen( c_prefix_ok )
       && next_response_line.substr( 0, strlen( c_prefix_ok ) ) == c_prefix_ok )
      {
         timeout = c_subsequent_timeout;
         next_response_line.erase( );
         continue;
      }

      if( timeout == c_initial_timeout
       && next_response_line.size( ) >= strlen( c_prefix_err )
       && next_response_line.substr( 0, strlen( c_prefix_err ) ) == c_prefix_err )
         throw runtime_error( next_response_line.substr( strlen( c_prefix_err ) + 1 ) );

      if( next_response_line == c_response_multi_terminator )
         break;

      os << next_response_line << '\n';

      timeout = c_subsequent_timeout;
      next_response_line.erase( );
   }
}

void get_multi_line_reponse( tcp_socket& socket, vector< string >& response_lines, bool* p_is_mime = 0 )
{
   string next_response_line;
   size_t timeout = c_initial_timeout;

   if( p_is_mime )
      *p_is_mime = false;

   bool finished_headers = false;
   while( true )
   {
      if( socket.read_line( next_response_line, timeout ) <= 0 )
      {
         // NOTE: If an empty line is received then continue to read.
         if( socket.had_blank_line( ) )
         {
            finished_headers = true;
            response_lines.push_back( "" );
#ifdef DEBUG
      cout << "\n";
#endif
            continue;
         }
         throw runtime_error( "recv failure for multi-line response" );
      }

      if( p_is_mime && !finished_headers )
      {
         if( lower( next_response_line ).find( lower( c_mime_header ) ) == 0 )
            *p_is_mime = true;
      }

#ifdef DEBUG
      cout << next_response_line << "\n";
#endif
      if( timeout == c_initial_timeout
       && next_response_line.size( ) >= strlen( c_prefix_ok )
       && next_response_line.substr( 0, strlen( c_prefix_ok ) ) == c_prefix_ok )
      {
         timeout = c_subsequent_timeout;
         next_response_line.erase( );
         continue;
      }

      if( response_lines.empty( )
       && next_response_line.size( ) >= strlen( c_prefix_err )
       && next_response_line.substr( 0, strlen( c_prefix_err ) ) == c_prefix_err )
         throw runtime_error( next_response_line.substr( strlen( c_prefix_err ) + 1 ) );

      if( next_response_line == c_response_multi_terminator )
         break;

      response_lines.push_back( next_response_line );
      timeout = c_subsequent_timeout;
      next_response_line.erase( );
   }
}

int parse_stat_response( const string& response, long* p_octets )
{
   if( response.size( ) >= strlen( c_prefix_err )
    && response.substr( 0, strlen( c_prefix_err ) ) == c_prefix_err )
      throw runtime_error( "pop3 - " + response.substr( strlen( c_prefix_err ) + 1 ) );

   if( response.size( ) >= strlen( c_prefix_ok )
    && response.substr( 0, strlen( c_prefix_ok ) ) != c_prefix_ok )
      throw runtime_error( "pop3 - unexpected STAT response '" + response + "'" );

   string stat_details( response.substr( strlen( c_prefix_ok ) + 1 ) );

   string::size_type pos = stat_details.find( ' ' );
   if( pos == string::npos )
      throw runtime_error( "pop3 - unable to interpret '" + response + "' as a STAT response" );

   if( p_octets )
      *p_octets = atol( stat_details.substr( pos + 1 ).c_str( ) );

   return atoi( stat_details.substr( 0, pos ).c_str( ) );
}

void parse_list_response(
 const vector< string >& response_lines,
 vector< pair< int, long > >& message_list )
{
   for( vector< string >::size_type i = 0; i < response_lines.size( ); i++ )
   {
      if( response_lines[ i ].size( ) >= strlen( c_prefix_ok )
       && response_lines[ i ].substr( 0, strlen( c_prefix_ok ) ) == c_prefix_ok )
         continue;

      if( response_lines[ i ].size( ) >= strlen( c_prefix_err )
       && response_lines[ i ].substr( 0, strlen( c_prefix_err ) ) == c_prefix_err )
         throw runtime_error( response_lines[ i ].substr( strlen( c_prefix_err ) + 1 ) );

      string::size_type pos = response_lines[ i ].find( ' ' );
      if( pos == string::npos )
         throw runtime_error( "unable to interpret '" + response_lines[ i ] + "' as a list entry" );

      message_list.push_back( make_pair(
       atoi( response_lines[ i ].substr( 0, pos ).c_str( ) ),
       atol( response_lines[ i ].substr( pos + 1 ).c_str( ) ) ) );
   }
}

void parse_top_response( const vector< string >& response_lines, vector< string >& headers )
{
   bool found = false;
   size_t last_header = 0;

   bool get_all_headers = headers.empty( );
   for( vector< string >::size_type i = 0; i < response_lines.size( ); i++ )
   {
      if( get_all_headers )
         headers.push_back( response_lines[ i ] );
      else
      {
         if( found && !response_lines[ i ].empty( )
          && ( response_lines[ i ][ 0 ] == ' ' || response_lines[ i ][ 0 ] == '\t' ) )
         {
            headers[ last_header ] += "\n" + response_lines[ i ];
            continue;
         }
         else
            found = false;

         for( vector< string >::size_type j = 0; j < headers.size( ); j++ )
         {
            string::size_type pos = lower( response_lines[ i ] ).find( lower( headers[ j ] ) );

            if( pos == 0 )
            {
               found = true;
               last_header = j;
               headers[ j ] = response_lines[ i ];

               break;
            }
         }
      }
   }
}

}

struct pop3::impl
{
   impl( )
   {
      port = c_pop3_default_port;

      host = "localhost";
      ctype = e_pop3_ctype_insecure;

      is_open = false;

      messages = 0;
      total_size = 0L;
   }

   int port;
   string host;
   bool is_open;
   pop3_ctype ctype;

   int messages;
   long total_size;

   vector< pair< int, long > > message_list;

#ifdef SSL_SUPPORT
   ssl_socket socket;
#else
   tcp_socket socket;
#endif
};

pop3::pop3( )
{
   p_impl = new impl;

   init( "localhost" );
}

pop3::pop3( const string& host, int port, pop3_ctype ctype )
{
   p_impl = new impl;

   init( host, port, ctype );
}

pop3::~pop3( )
{
   delete p_impl;
   p_impl = 0;
}

void pop3::init( const string& host, int port, pop3_ctype ctype )
{
#ifndef SSL_SUPPORT
   if( ctype != e_pop3_ctype_insecure )
      throw runtime_error( "pop3::init - cannot init securely without SSL support" );
#endif
   p_impl->host = host;
   p_impl->port = port;
   p_impl->ctype = ctype;

   if( p_impl->is_open )
   {
      p_impl->socket.close( );

      p_impl->messages = 0;
      p_impl->total_size = 0;

      p_impl->message_list.clear( );
   }
}

void pop3::login( const string& user, const string& password )
{
   if( !p_impl->socket.open( ) )
      throw runtime_error( "pop3::login - unable to open socket" );

   try
   {
      string response;
      ip_address address( p_impl->host.c_str( ), p_impl->port );

      if( p_impl->socket.connect( address ) )
      {
#ifdef USE_NO_DELAY
#  ifndef DEBUG
         p_impl->socket.set_no_delay( );
#  else
         if( !p_impl->socket.set_no_delay( ) )
            cout << "warning: set_no_delay failed..." << endl;
#  endif
#endif
#ifdef SSL_SUPPORT
         // NOTE: For SSL all protocol is secure (unlike STARTTLS).
         // FUTURE: After a successful SSL connection the server certificate should
         // be checked (at the very least make sure that it is the host requested).
         if( p_impl->ctype == e_pop3_ctype_ssl )
            p_impl->socket.ssl_connect( );

         // NOTE: Get server greeting...
         response = get_simple_response( p_impl->socket );
         cout << '\n';
#ifdef DEBUG
         cout << response << '\n';
#endif

         if( p_impl->ctype == e_pop3_ctype_tls )
         {
            send_simple_request( p_impl->socket, c_request_stls );
            response = get_simple_response( p_impl->socket );
#  ifdef DEBUG
            cout << response << '\n';
#  endif

            // FUTURE: After a successful SSL connection the server certificate should
            // be checked (at the very least make sure that it is the host requested).
            p_impl->socket.ssl_connect( );
         }
#endif
         string user_req( c_request_user );
         user_req += ' ' + user;

         send_simple_request( p_impl->socket, user_req );
         response = get_simple_response( p_impl->socket );
#ifdef DEBUG
         cout << response << '\n';
#endif

         string pass_req( c_request_pass );
         pass_req += ' ' + password;

         send_simple_request( p_impl->socket, pass_req );
         response = get_simple_response( p_impl->socket );
#ifdef DEBUG
         cout << response << '\n';
#endif

         send_simple_request( p_impl->socket, c_request_stat );
         response = get_simple_response( p_impl->socket );
#ifdef DEBUG
         cout << response << '\n';
#endif
         p_impl->messages = parse_stat_response( response, &p_impl->total_size );

         p_impl->message_list.clear( );

         // NOTE: Make the message list request regardless of the STAT response
         // as some servers don't display the number of messages in their response.
         get_message_list( p_impl->message_list );
         if( p_impl->messages != p_impl->message_list.size( ) )
         {
            p_impl->messages = p_impl->message_list.size( );

            p_impl->total_size = 0;
            for( size_t i = 0; i < p_impl->message_list.size( ); i++ )
               p_impl->total_size += p_impl->message_list[ i ].second;
         }

         p_impl->is_open = true;
      }
      else
         throw runtime_error( "pop3::login - unable to connect to host '" + p_impl->host + "'" );
   }
   catch( ... )
   {
      p_impl->socket.close( );
      throw;
   }
}

int pop3::get_num_messages( ) const
{
   return p_impl->messages;
}

long pop3::get_total_messages_size( ) const
{
   return p_impl->total_size;
}

void pop3::get_message_list( vector< pair< int, long > >& message_list )
{
   send_simple_request( p_impl->socket, c_request_list );

   vector< string > response_lines;
   get_multi_line_reponse( p_impl->socket, response_lines );

   parse_list_response( response_lines, message_list );
}

void pop3::get_message( int message_num, ostream& os, bool* p_is_mime )
{
   string request( c_request_retr );
   request += ' ' + to_string( message_num );

   send_simple_request( p_impl->socket, request );
   get_multi_line_reponse( p_impl->socket, os, p_is_mime );
}

void pop3::get_message_headers( int message_num, vector< string >& headers )
{
   bool found = false;
   for( size_t i = 0; i < p_impl->message_list.size( ); i++ )
   {
      if( p_impl->message_list[ i ].first == message_num )
      {
         found = true;
         break;
      }
   }

   if( !found )
      throw runtime_error( "pop3 - message #" + to_string( message_num ) + " does not exist" );

   string top_request( c_request_top );
   top_request += ' ';
   top_request += to_string( message_num );
   top_request += " 0";

   send_simple_request( p_impl->socket, top_request );

   vector< string > response_lines;
   get_multi_line_reponse( p_impl->socket, response_lines );

   parse_top_response( response_lines, headers );
}

void pop3::delete_message( int message_num )
{
   bool found = false;
   for( size_t i = 0; i < p_impl->message_list.size( ); i++ )
   {
      if( p_impl->message_list[ i ].first == message_num )
      {
         found = true;
         break;
      }
   }

   if( !found )
      throw runtime_error( "pop3 - message #" + to_string( message_num ) + " does not exist" );

   string dele_request( c_request_dele );
   dele_request += ' ' + to_string( message_num );

   send_simple_request( p_impl->socket, dele_request );
   string response = get_simple_response( p_impl->socket );
#ifdef DEBUG
   cout << response << '\n';
#endif
}

void pop3::disconnect( )
{
   if( p_impl->is_open )
   {
      send_simple_request( p_impl->socket, c_request_quit );

      string response;
      size_t timeout( c_final_response_timeout );
      p_impl->socket.read_line( response, timeout );

#ifdef DEBUG
      if( !response.empty( ) )
         cout << response << '\n';
#endif

      p_impl->socket.close( );
      p_impl->is_open = false;
   }
}

