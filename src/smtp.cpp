// Copyright (c) 2006-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <memory.h>
#  include <memory>
#  include <iostream>
#  include <stdexcept>
#endif

#include "smtp.h"

#include "md5.h"
#include "mime.h"
#include "base64.h"
#include "config.h"
#include "sockets.h"
#include "progress.h"
#include "date_time.h"
#include "utilities.h"
#ifdef SSL_SUPPORT
#  include "ssl_socket.h"
#endif

#define USE_NO_DELAY

using namespace std;

namespace
{

// Sample SMTP client protocol:
// HELO username
// MAIL FROM:<sender@domain>
// RCPT TO:<first@domain>
// RCPT TO:<second@domain>
// DATA
// From: "A. Sender" <sender@domain>
// To: "B. First" <first@domain>
// Cc: "C. Second" <second@domain>
// Subject: Test
//
// This is a test...
// .
// QUIT
//
// SMTP: RFC 821 (http://www.faqs.org/rfcs/rfc821.html)
// MIME: RFC 1521 (http://www.faqs.org/rfcs/rfc1521.html)
// ESMTP: RFC 2554 (http://www.faqs.org/rfcs/rfc2554.html) extends SMTP (use EHLO rather than HELO)
// CRAM-MD5: RFC 2195 (http://www.faqs.org/rfcs/rfc2195.html) extends ESMTP (to support AUTH CRAM-MD5)
// STARTTLS: RFC 2487 (http://www.ietf.org/rfc/rfc2487.txt) extends ESMTP (to support TLS security)
//
// Sample ESMTP connection (using LOGIN method):
// S: 220...hello...
// C: EHLO username
// S: 250...options...
// C: AUTH LOGIN
// S: 334 VXNlcm5hbWU6
// C: dXNlcg==             (where this is the b64 encoded string for "user")
// S: 334 UGFzc3dvcmQ6
// C: cGFzc3dvcmQ=         (where this is the b64 encoded string for "password")
// S: 235...succeeded...
//
// CRAM-MD5 Example:
//
// S: 220...hello...
// C: EHLO username
// S: 250...options...
// C: AUTH CRAM-MD5
// S: 334 BASE64(NONCE)
// C: BASE64(USERNAME, " ", MD5((SECRET XOR opad), MD5((SECRET XOR ipad), NONCE)))
// S: 235...succeeded...
//
// (if user is "joe" and password is "tanstaaftanstaaf" and nonce is <1896.697170952@xxxxxxxxxxxxxxxxxxxxxx>)
// S: 220...hello...
// C: EHLO joe
// S: 250...options...
// C: AUTH CRAM-MD5
// S: 334 PDE4OTYuNjk3MTcwOTUyQHBvc3RvZmZpY2UuZXhhbXBsZS5uZXQ+
// C: am9lIDNkYmM4OGYwNjI0Nzc2YTczN2IzOTA5M2Y2ZWI2NDI3
// S: 235...succeeded...
//
// STARTTLS Example:
//
// S: 220...hello...
// C: EHLO username
// S: 250...options...
// C: STARTTLS
// S: 220...ready...
// (negotiate SSL connection)
// C: EHLO username
// (continue as per AUTH PLAIN/LOGIN)

// NOTE: The standard SMTP port is 25. For TLS it is usually 465 and for STARTTLS it is 25 or 587.
const int c_smtp_default_port = 25;

const int c_smtp_prefix_length = 3;

const char* const c_smtp_prefix_info = "220";
const char* const c_smtp_prefix_auth = "235";
const char* const c_smtp_prefix_okay = "250";
const char* const c_smtp_prefix_user = "334";
const char* const c_smtp_prefix_data = "354";

const char* c_response_multi_terminator = ".";

const size_t c_initial_timeout = 5000;
const size_t c_subsequent_timeout = 1500;
const size_t c_final_response_timeout = 50;

const size_t c_max_chars_per_line = 510;

const unsigned char ipad = 0x36;
const unsigned char opad = 0x5c;

bool get_response( string& text, tcp_socket& socket,
 size_t timeout, progress* p_progress, bool* p_had_timeout = 0 )
{
   string response;

   if( p_progress && !text.empty( ) )
      p_progress->output_progress( text );

   text.erase( );

   bool okay = true;

   while( true )
   {
      response.erase( );

      if( socket.read_line( response, timeout ) <= 0 )
      {
         if( text.empty( ) )
         {
            okay = false;

            if( p_had_timeout )
            {
               *p_had_timeout = true;

               text = "unexpected timeout awaiting response";
            }
         }

         break;
      }

      if( !p_had_timeout || ( response == c_response_multi_terminator ) )
         break;

      if( !response.empty( ) )
      {
         if( !text.empty( ) )
            text += '\n';

         text += response;

         if( response.size( ) > c_smtp_prefix_length )
         {
            string prefix( response.substr( 0, c_smtp_prefix_length ) );

            if( ( prefix != c_smtp_prefix_info ) && ( prefix != c_smtp_prefix_okay )
             && ( prefix != c_smtp_prefix_auth ) && ( prefix != c_smtp_prefix_user ) && ( prefix != c_smtp_prefix_data ) )
               okay = false;
            // NOTE: If the prefix is followed by a "space" then no further lines are expected.
            else if( response[ c_smtp_prefix_length ] == ' ' )
               break;
         }
         else
            okay = false;
      }

      timeout = c_subsequent_timeout;
   }

   if( p_progress && !text.empty( ) )
      p_progress->output_progress( text );

   return okay;
}

string escape_fullstops_if_required( const string& src )
{
   string str;

   // NOTE: For an SMTP message a full stop followed by a CR/LF pair indicates the end of the message so
   // in accordance with the "transparency" section (4.5.2) of RFC 821 any text lines that commence with
   // a full stop are escaped by adding an extra full stop (these should be automatically removed by the
   // message receiver).
   bool is_start_of_line = true;

   for( size_t i = 0; i < src.length( ); i++ )
   {
      if( is_start_of_line && src[ i ] == '.' )
         str += ".";

      str += src[ i ];

      is_start_of_line = ( src[ i ] == '\n' );
   }

   return str;
}

string split_input_into_lines( const string& input, size_t max_chars_per_line )
{
   string str;
   string::size_type num = 0, pos = 0, spos = string::npos;

   for( size_t i = 0; i < input.size( ); i++ )
   {
      if( ++num > max_chars_per_line )
      {
         if( spos != string::npos )
         {
            str += input.substr( pos, spos - pos ) + "\r\n";

            num = 0;
            pos = spos + 1;
            spos = string::npos;
         }
         else
         {
            str += input.substr( pos, max_chars_per_line ) + "\r\n";

            num = 0;
            pos += max_chars_per_line;
         }
      }
      else if( input[ i ] == ' ' )
         spos = i;
   }

   if( pos < input.size( ) )
      str += input.substr( pos ) + "\r\n";

   return str;
}

string determine_challenge_response( const string& challenge, const string& user, const string& password )
{
   string nonce( challenge );
   string secret( password );

   string::size_type pos = nonce.find( ' ' );

   if( pos != string::npos )
      nonce = nonce.substr( pos + 1 );

   nonce = base64::decode( nonce.c_str( ) );

   if( secret.length( ) > 64 )
      secret = string( ( char* )MD5( ( unsigned char* )secret.c_str( ) ).raw_digest( ) );

   unsigned char opad_buf[ 65 ];
   unsigned char ipad_buf[ 65 ];

   memset( opad_buf, '\0', sizeof( opad_buf ) );
   memset( ipad_buf, '\0', sizeof( ipad_buf ) );

   memcpy( opad_buf, secret.c_str( ), secret.length( ) );
   memcpy( ipad_buf, secret.c_str( ), secret.length( ) );

   for( size_t i = 0; i < 64; i++ )
   {
      opad_buf[ i ] ^= opad;
      ipad_buf[ i ] ^= ipad;
   }

   MD5 md5_ipad;

   md5_ipad.update( ipad_buf, 64 );
   md5_ipad.update( ( unsigned char* )nonce.c_str( ), nonce.length( ) );

   md5_ipad.finalize( );

   unique_ptr< unsigned char > up_idigest( md5_ipad.raw_digest( ) );

   MD5 md5_opad;

   md5_opad.update( opad_buf, 64 );
   md5_opad.update( up_idigest.get( ), 16 );

   md5_opad.finalize( );

   unique_ptr< char > up_odigest( md5_opad.hex_digest( ) );

   string response( user + " " );

   response += string( up_odigest.get( ) );

   return base64::encode( response );
}

string transform_header_to_utf_8_if_required( const string& header )
{
   bool has_unicode = false;

   size_t start = 0;

   for( size_t i = 0; i < header.size( ); i++ )
   {
      if( start == 0 && header[ i ] == ':' )
         start = i;
      else if( header[ i ] == ' ' && start == i - 1 )
         ++start;
      else if( header[ i ] < 0 )
      {
         has_unicode = true;
         break;
      }
   }

   if( !has_unicode )
      return header;
   else
      return header.substr( 0, start + 1 ) + "=?utf-8?B?" + base64::encode( header.substr( start + 1 ) ) + "?=";
}

void send_message( const string& host_and_port,
 const smtp_user_info& user_info, const vector< string >& recipients,
 const string& subject, const string* p_message = 0, const vector< string >* p_extra_headers = 0,
 const vector< string >* p_file_names = 0, const string* p_html = 0, const vector< string >* p_image_names = 0,
 const string* p_image_path_prefix = 0, progress* p_progress = 0, bool* p_had_timeout = 0 )
{
   int port = c_smtp_default_port;

   string host( host_and_port );

   string::size_type pos = host.find( ':' );

   if( pos != string::npos )
   {
      port = atoi( host.substr( pos + 1 ).c_str( ) );
      host.erase( pos );
   }

   if( p_progress )
      p_progress->output_progress( "host = " + host + ", port = " + to_string( port ) );

   if( p_file_names )
   {
      int64_t num_bytes = INT64_C( 0 );

      for( size_t i = 0; i < p_file_names->size( ); i++ )
      {
         string next_file( ( *p_file_names )[ i ] );

         string::size_type pos = next_file.find( '?' );

         if( pos != string::npos )
            next_file.erase( 0, pos + 1 );

         if( !file_exists( next_file ) )
            throw runtime_error( "file '" + next_file + "' not found" );

         num_bytes += file_size( next_file );
      }

      if( p_progress )
         p_progress->output_progress( "total attached file data: " + to_string( num_bytes ) );

      if( user_info.max_attachment_bytes && ( num_bytes > user_info.max_attachment_bytes ) )
         throw runtime_error( "maximum allowed attached file data exceeded" );
   }

#ifdef SSL_SUPPORT
   ssl_socket socket;
#else
   tcp_socket socket;
#endif
   bool okay = socket.open( );

   if( okay )
   {
      ip_address address( host.c_str( ), port );

      if( p_progress )
         p_progress->output_progress( "connecting..." );

      if( socket.connect( address ) )
      {
#ifdef USE_NO_DELAY
         socket.set_no_delay( );
#endif
#ifdef SSL_SUPPORT
         // NOTE: For SSL all protocol is secure (unlike STARTTLS).
         // FUTURE: After a successful SSL connection the server certificate should
         // be checked (at the very least make sure that it is the host requested).
         if( user_info.use_ssl )
            socket.ssl_connect( );
#endif
         string str( "(connected now reading greeting)" );

         // NOTE: Read (and ignore) the connection message...
         if( !get_response( str, socket, c_initial_timeout, p_progress, p_had_timeout ) )
            throw runtime_error( str );

         if( user_info.auth_type == e_smtp_auth_type_none )
         {
            str = string( "HELO " );
            str += user_info.domain;

            socket.write_line( str );

            if( !get_response( str, socket, c_initial_timeout, p_progress, p_had_timeout ) )
               throw runtime_error( str );
         }
         else
         {
            str = string( "EHLO " );
            str += user_info.domain;

            socket.write_line( str );

            if( !get_response( str, socket, c_initial_timeout, p_progress, p_had_timeout ) )
               throw runtime_error( str );

#ifdef SSL_SUPPORT
            // NOTE: For STARTTLS the initial EHLO is unsecure (enabling it to be
            // compatible with non-secure SMTP) and therefore needs to be re-sent
            // after the connection is secured.
            if( !user_info.use_ssl && user_info.use_tls )
            {
               str = "STARTTLS";

               socket.write_line( str );

               if( !get_response( str, socket, c_initial_timeout, p_progress, p_had_timeout ) )
                  throw runtime_error( str );

               // FUTURE: After a successful SSL connection the server certificate should
               // be checked (at the very least make sure that it is the host requested).
               socket.ssl_connect( );

               str = string( "EHLO " );
               str += user_info.domain;

               socket.write_line( str );

               if( !get_response( str, socket, c_initial_timeout, p_progress, p_had_timeout ) )
                  throw runtime_error( str );
            }
#endif
            if( user_info.auth_type == e_smtp_auth_type_plain )
            {
               str = string( "AUTH PLAIN " );

               string auth_str( user_info.username );

               auth_str += '\0';
               auth_str += user_info.username;
               auth_str += '\0';
               auth_str += user_info.password;

               str += base64::encode( auth_str );
            }
            else if( user_info.auth_type == e_smtp_auth_type_login )
               str = string( "AUTH LOGIN" );
            else if( user_info.auth_type == e_smtp_auth_type_cram_md5 )
               str = string( "AUTH CRAM-MD5" );
            else
               throw runtime_error( "unexpected smtp_auth_type in send_message" );

            socket.write_line( str );

            // NOTE: Don't allow progress tracking to see the password.
            str.erase( );

            if( !get_response( str, socket, c_initial_timeout, p_progress, p_had_timeout ) )
               throw runtime_error( str );

            if( user_info.auth_type != e_smtp_auth_type_plain )
            {
               if( user_info.auth_type == e_smtp_auth_type_login )
               {
                  str = base64::encode( user_info.username );

                  socket.write_line( str );

                  if( !get_response( str, socket, c_initial_timeout, p_progress, p_had_timeout ) )
                     throw runtime_error( str );

                  str = base64::encode( user_info.password );

                  socket.write_line( str );

                  // NOTE: Don't allow progress tracking to see the password.
                  str.erase( );

                  if( !get_response( str, socket, c_initial_timeout, p_progress, p_had_timeout ) )
                     throw runtime_error( str );
               }
               else if( user_info.auth_type == e_smtp_auth_type_cram_md5 )
               {
                  str = determine_challenge_response( str, user_info.username, user_info.password );

                  socket.write_line( str );

                  // NOTE: Don't allow progress tracking to see the password.
                  str.erase( );

                  if( !get_response( str, socket, c_initial_timeout, p_progress, p_had_timeout ) )
                     throw runtime_error( str );
               }
            }
         }

         str = string( "MAIL FROM:" );

         string from_header( "From: " );

         string::size_type pos = user_info.address.find( '<' );

         if( pos == string::npos )
            str += "<" + user_info.address + ">\r\n";
         else
            str += user_info.address.substr( pos ) + "\r\n";

         socket.write_line( str );

         from_header += user_info.address;

         if( !get_response( str, socket, c_initial_timeout, p_progress, p_had_timeout ) )
            throw runtime_error( str );

         string to_header, cc_header;

         bool is_to = true;
         bool is_cc = false;

         for( size_t i = 0; i < recipients.size( ); i++ )
         {
            // NOTE: A "blank" recipient is used to separate the "To" header from the "Cc" one (any
            // further "blank" recipient will cause subsequent recipients to be considered as "Bcc").
            if( recipients[ i ].empty( ) )
            {
               if( !is_cc )
               {
                  is_to = false;
                  is_cc = true;
               }
               else
                  is_cc = false;

               continue;
            }

            str = string( "RCPT TO:" );

            pos = recipients[ i ].find( '<' );

            if( pos == string::npos )
               str += "<" + recipients[ i ] + ">\r\n";
            else
               str += recipients[ i ].substr( pos ) + "\r\n";

            socket.write_line( str );

            if( !get_response( str, socket, c_initial_timeout, p_progress, p_had_timeout ) )
               throw runtime_error( str );

            if( is_to )
            {
               if( !to_header.empty( ) )
                  to_header += ", ";
               else
                  to_header = "To: ";

               to_header += recipients[ i ];
            }
            else if( is_cc )
            {
               if( !cc_header.empty( ) )
                  cc_header += ", ";
               else
                  cc_header = "Cc: ";

               cc_header += recipients[ i ];
            }
         }

         str = string( "DATA\r\n" );

         socket.write_line( str );

         if( !get_response( str, socket, c_initial_timeout, p_progress, p_had_timeout ) )
            throw runtime_error( str );

         string subject_header( "Subject: " );

         subject_header += subject;

         str = transform_header_to_utf_8_if_required( from_header );
         str += string( "\r\n" );

         str += transform_header_to_utf_8_if_required( subject_header );
         str += string( "\r\n" );

         str += transform_header_to_utf_8_if_required( to_header );
         str += string( "\r\n" );

         if( !cc_header.empty( ) )
         {
            str += transform_header_to_utf_8_if_required( cc_header );
            str += string( "\r\n" );
         }

         date_time dt( date_time::standard( ) );

         if( user_info.p_dt )
            dt = *user_info.p_dt;

         string date_header( "Date: " );

         date_header += dt.weekday_name( ).substr( 0, 3 );
         date_header += ", " + to_string( ( int )dt.get_day( ) ) + " " + dt.month_name( ).substr( 0, 3 );
         date_header += " " + to_string( dt.get_year( ) );
         date_header += " " + dt.get_time( ).as_string( e_time_format_hhmmss, true );

         if( user_info.utc_offset >= 0 )
            date_header += " +";
         else
            date_header += " -";

         if( abs( user_info.utc_offset ) < 10 )
            date_header += "0";

         date_header += to_string( ( int )user_info.utc_offset );

         int minutes = ( int )( ( user_info.utc_offset - ( int )user_info.utc_offset ) * 60.0 );

         if( abs( minutes ) < 10 )
            date_header += "0";

         date_header += to_string( minutes );

         if( user_info.p_tz_abbr && !user_info.p_tz_abbr->empty( ) )
            date_header += " (" + *user_info.p_tz_abbr + ")";

         str += date_header;
         str += string( "\r\n" );

         // NOTE: It is being assumed that if extra headers were supplied
         // then Message-ID is amongst them.
         if( !p_extra_headers )
         {
            string message_id_header( "Message-ID: <" );
            message_id_header += uuid( ).as_string( ) + ">";

            str += message_id_header;
            str += string( "\r\n" );
         }
         else
         {
            for( size_t i = 0; i < p_extra_headers->size( ); i++ )
            {
               if( !( *p_extra_headers )[ i ].empty( ) )
               {
                  str += transform_header_to_utf_8_if_required( ( *p_extra_headers )[ i ] );
                  str += string( "\r\n" );
               }
            }
         }

         unique_ptr< mime_encoder > up_mime;

         bool has_html = ( p_html && !p_html->empty( ) );
         bool has_files = ( p_file_names && !p_file_names->empty( ) );
         bool has_images = ( p_image_names && !p_image_names->empty( ) );
         bool has_message = ( p_message && !p_message->empty( ) );

         const char* p_charset = 0;

         if( user_info.p_charset )
            p_charset = user_info.p_charset->c_str( );

         string extracted_message;

         if( has_html && !has_message )
         {
            extracted_message = extract_text_from_html( *p_html );
            p_message = &extracted_message;
            has_message = true;
         }

         // NOTE: If there is HTML or attached files then format the message as MIME.
         if( has_html || has_files )
         {
            up_mime.reset( new mime_encoder( ) );

            if( has_html )
            {
               if( has_images )
               {
                  up_mime->create_child( "related" );

                  if( !has_message )
                     up_mime->get_child( ).add_html( *p_html, p_charset );
                  else
                  {
                     up_mime->get_child( ).create_child( "alternative" );

                     up_mime->get_child( ).get_child( ).add_text( *p_message, p_charset );
                     up_mime->get_child( ).get_child( ).add_html( *p_html, p_charset );
                  }

                  const char* p_path_prefix = 0;

                  if( p_image_path_prefix )
                     p_path_prefix = ( *p_image_path_prefix ).c_str( );

                  for( size_t i = 0; i < p_image_names->size( ); i++ )
                     up_mime->get_child( ).add_image( ( *p_image_names )[ i ], p_path_prefix );
               }
               else
               {
                  if( !has_message )
                     up_mime->add_html( *p_html, p_charset );
                  else
                  {
                     up_mime->create_child( "related" );

                     up_mime->get_child( ).create_child( "alternative" );

                     up_mime->get_child( ).get_child( ).add_text( *p_message, p_charset );
                     up_mime->get_child( ).get_child( ).add_html( *p_html, p_charset );
                  }
               }
            }
            else if( has_message )
               up_mime->add_text( *p_message, p_charset );

            if( has_files )
            {
               for( size_t i = 0; i < p_file_names->size( ); i++ )
                  up_mime->add_file( ( *p_file_names )[ i ] );
            }

            str += up_mime->get_data( );
         }
         else if( p_message )
         {
            // NOTE: Limit the lines to one less than the maximum as a fullstop escape character
            // may be required at the start of one or more lines.
            string message_text( split_input_into_lines( *p_message, c_max_chars_per_line - 1 ) );

            str += string( "\r\n" );

            str += escape_fullstops_if_required( message_text );

            str += string( "\r\n" );
         }

         str += string( ".\r\n" );

         socket.write_line( str );

         if( !get_response( str, socket, c_initial_timeout, p_progress, p_had_timeout ) )
            throw runtime_error( str );

         str = "QUIT\r\n";

         socket.write_line( str );

         // NOTE: Read (but ignore) any disconnection message...
         get_response( str, socket, c_final_response_timeout, p_progress );
      }
      else
         throw runtime_error( "unable to connect to '" + host + "' on port #" + to_string( port ) );
   }
   else
      throw runtime_error( "unable to open socket" );
}

}

string html_to_text( const string& html )
{
   return extract_text_from_html( html );
}

void send_smtp_message( const string& host,
 const smtp_user_info& user_info, const vector< string >& recipients, const string& subject )
{
   send_message( host, user_info, recipients, subject );
}

void send_smtp_message( const string& host,
 const smtp_user_info& user_info, const vector< string >& recipients, const string& subject, const string& message )
{
   send_message( host, user_info, recipients, subject, &message );
}

void send_smtp_message( const string& host, const smtp_user_info& user_info,
 const vector< string >& recipients, const string& subject, const string& message, const vector< string >& file_names )
{
   send_message( host, user_info, recipients, subject, &message, 0, &file_names );
}

void send_smtp_message( const string& host, const smtp_user_info& user_info,
 const vector< string >& recipients, const string& subject, const string& message,
 const string& html, const vector< string >* p_extra_headers, const vector< string >* p_file_names,
 const vector< string >* p_image_names, const string* p_image_path_prefix, progress* p_progress, bool* p_had_timeout )
{
   send_message( host, user_info, recipients, subject, &message,
    p_extra_headers, p_file_names, &html, p_image_names, p_image_path_prefix, p_progress, p_had_timeout );
}
