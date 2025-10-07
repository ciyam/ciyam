// Copyright (c) 2005-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <cstring>
#  include <cstdlib>
#  include <string>
#  include <fstream>
#  include <iostream>
#endif

#include "fcgiapp.h"

#ifdef __GNUG__
#  include <unistd.h>
#  include <sys/stat.h>
#endif

#include "config.h"
#include "threads.h"
#include "date_time.h"
#include "utilities.h"

#define USE_MOD_FASTCGI_KLUDGE

#define USE_MULTIPLE_REQUEST_HANDLERS

using namespace std;

string g_exe_path;

const int c_chunk_size = 4096;

const int c_num_handlers = 10;

const char* const c_prefix_name = "name=";
const char* const c_prefix_filename = "filename=";

const char* const c_all_specials = " !\"#$%&'()*+,-./<=>?@[\\]^`{|}~";

void restore_specials( string& str )
{
   string specials( c_all_specials );

   vector< string > hex_specials;

   for( size_t i = 0; i < specials.size( ); i++ )
   {
      string special;

      hex_encode( special, ( const unsigned char* )&specials[ i ], 1 );

      special = "%" + special;

      string::size_type pos = str.find( special );

      while( pos != string::npos )
      {
         str.erase( pos, special.size( ) );

         str.insert( pos, 1, specials[ i ] );

         pos = str.find( special );
      }
   }
}

class request_handler : public thread
{
   public:
   request_handler( );

   void on_start( );

   void process_request( );

   private:
   FCGX_Stream* p_in;
   FCGX_Stream* p_out;
   FCGX_Stream* p_err;
   FCGX_ParamArray* p_env;

#ifdef USE_MULTIPLE_REQUEST_HANDLERS
   FCGX_Request request;
#else
   FCGX_ParamArray envp;
#endif
};

request_handler::request_handler( )
{
#ifdef USE_MULTIPLE_REQUEST_HANDLERS
   FCGX_InitRequest( &request, 0, 0 );
#endif
}

void request_handler::on_start( )
{
#ifdef USE_MULTIPLE_REQUEST_HANDLERS
   while( FCGX_Accept_r( &request ) >= 0 )
   {
      p_in = request.in;
      p_out = request.out;
      p_err = request.err;

      p_env = &request.envp;

      process_request( );
   }
#else
   while( FCGX_Accept( &p_in, &p_out, &p_err, &envp ) >= 0 )
   {
      p_env = &envp;

      process_request( );
   }
#endif

   delete this;
}

void request_handler::process_request( )
{
   FCGX_FPrintF( p_out, "Content-type: text/html\n" );

   const char* p_param = FCGX_GetParam( "CONTENT_LENGTH", *p_env );
   const char* p_raddr = FCGX_GetParam( "REMOTE_ADDR", *p_env );

   FCGX_FPrintF( p_out, "\r\n\r\n<html>\n<head>\n   <title>FCGI File Upload</title>\n" );

   // NOTE: It is expected that if a parent exists it will contain a "refresh" function
   // unless Chrome has been detected (in which case issue a "location.reload" instead).
   FCGX_FPrintF( p_out, "<script type=\"text/javascript\">\n" );
   FCGX_FPrintF( p_out, "if( window.parent )\n" );
   FCGX_FPrintF( p_out, "{\n" );
   FCGX_FPrintF( p_out, "   if( window.chrome )\n" );
   FCGX_FPrintF( p_out, "      window.parent.location.reload( );\n" );
   FCGX_FPrintF( p_out, "   else\n" );
   FCGX_FPrintF( p_out, "      window.parent.refresh( );\n" );
   FCGX_FPrintF( p_out, "}\n" );
   FCGX_FPrintF( p_out, "</script>\n" );

   FCGX_FPrintF( p_out, "</head>\n<body>\n" );

   size_t size = 0;
   size_t length = 0;

   if( p_param )
   {
      FCGX_FPrintF( p_out, "<p>Found CONTENT_LENGTH: %s</p>\n", p_param );

      length = atoi( p_param );
   }

   string name, file_name;

   char buf[ c_chunk_size ];
   char buf2[ c_chunk_size ];
   char dbuf[ c_chunk_size * 2 ];

   string disposition, file_source;

   if( FCGX_GetLine( buf, c_chunk_size, p_in ) )
   {
      string marker( buf );

      size += marker.size( );

      string::size_type pos = marker.find_first_of( "\r\n" );

      if( pos != string::npos )
         marker.erase( pos );

      marker = "\r\n" + marker;

      FCGX_GetLine( buf, c_chunk_size, p_in );

      disposition = string( buf );

      size += disposition.size( );

      // NOTE: It is expected that the HTML file input tag will provide the file upload data in its name attribute.
      pos = disposition.find( c_prefix_name );

      if( pos != string::npos )
      {
         for( size_t i = pos + strlen( c_prefix_name ) + 1; true; i++ )
         {
            if( disposition[ i ] == '"' )
               break;

            name += disposition[ i ];
         }
      }

      pos = disposition.find( c_prefix_filename );

      if( pos != string::npos )
      {
         for( size_t i = pos + strlen( c_prefix_filename ) + 1; true; i++ )
         {
            if( disposition[ i ] == '"' )
               break;

            file_source += disposition[ i ];
         }

         // NOTE: Special characters (such as quote characters themselves)
         // may have been encoded in the content so will decode these now.
         restore_specials( file_source );
      }

      string ext;

      pos = file_source.rfind( "." );

      if( pos == string::npos )
         FCGX_FPrintF( p_out, "<p>*** unexpected file extension not found in '%s' ***</p>", file_source.c_str( ) );
      else
      {
         ext = file_source.substr( pos );

         file_source.erase( pos );
      }

      // NOTE: Format for upload name is [<sid>:][<dir>;]<dest>[?<limit>] where:
      // <sid> is the session id (used as a confirmation output file that contains the file name)
      // <dir> is path below the standard child "/files" directory where the file will be created
      // <dest> is the destination file name (minus extension that is taken from the source file)
      // <limit> is an optional maximum file size limit
      string info( name );

      string file_id;
      string session_id;

      string path( g_exe_path + "/files" );

      pos = info.find( ':' );

      if( pos != string::npos )
      {
         session_id = info.substr( 0, pos );

         info.erase( 0, pos + 1 );
      }

      string sub_path;

      pos = info.find( ';' );

      if( pos != string::npos )
      {
         sub_path += "/" + info.substr( 0, pos );

         info.erase( 0, pos + 1 );
      }

      size_t max_size = 0;

      pos = info.find( '?' );

      if( pos != string::npos )
      {
         max_size = atol( info.substr( pos + 1 ).c_str( ) );

         info.erase( pos );
      }

      file_id = info;

      file_name = path + sub_path + "/" + file_id + ext;

#ifndef REMOVE_OR_COMMENT_THIS_OUT_IN_CONFIG_H
      // NOTE: In order to prevent any "arbitrary" upload from being able to take place
      // a verification file must contain the same information as the "name" attribute.
      string verification_file( path + "/tmp/" + session_id + "/" + file_id );

      if( !file_exists( verification_file )
       || ( buffer_file( verification_file ) != name ) )
      {
         max_size = ( c_chunk_size - 1 );

         session_id.erase( );

         file_name = path + "/" + date_time::standard( ).as_string( );
      }
#endif

      FCGX_GetLine( buf, c_chunk_size, p_in );

      string content_type( buf );

      size += content_type.size( );

      FCGX_GetLine( buf, c_chunk_size, p_in );

      string line_break( buf );

      size += line_break.size( );

      size_t len2 = 0;

      ofstream outf;

      if( !ext.empty( ) )
         outf.open( file_name.c_str( ), ios::out | ios::binary );

      if( !outf )
         FCGX_FPrintF( p_out, "<p>*** unable to open file '%s' for output ***</p>", file_name.c_str( ) );

#ifndef REMOVE_OR_COMMENT_THIS_OUT_IN_CONFIG_H
      if( outf && session_id.empty( ) )
      {
         outf << "Unauthorised upload from: " << string( p_raddr ) << '\n';
         outf << "(uploaded content follows)\n\n";
      }
#endif

      size_t written = 0;

      bool max_size_exceeded = false;

      while( true )
      {
         int len = FCGX_GetStr( buf, c_chunk_size, p_in );

         if( len < c_chunk_size )
         {
            if( len > 0 )
            {
               if( len2 )
                  memcpy( dbuf, buf2, len2 );

               memcpy( dbuf + len2, buf, len );
            }

            string last( dbuf, len2 + len );

            pos = last.find( marker );

            if( pos == string::npos )
               FCGX_FPrintF( p_out, "<p>*** unexpected end-of-file marker not found ***</p>" );
            else
            {
               outf.write( dbuf, pos );

               written += pos;
            }

            size += len;

            break;
         }

         if( len2 > 0 )
         {
            outf.write( buf2, len2 );

            written += len2;
         }

         memcpy( buf2, buf, len );

         len2 = len;

         size += len;

         if( max_size && ( written > max_size ) )
            break;
      }

      if( max_size && ( written > max_size ) )
      {
         if( !ext.empty( ) )
            file_remove( file_name.c_str( ) );

         max_size_exceeded = true;
      }

      if( !session_id.empty( ) )
      {
         string file_info;

         file_info = path + sub_path;

         file_info += "/" + session_id;

         ofstream outf( file_info.c_str( ) );

         if( max_size_exceeded )
            outf << ">" << max_size << endl;
         else if( !ext.empty( ) )
            outf << file_name << " " << file_source;
      }
   }

   if( length && size != length )
      FCGX_FPrintF( p_out, "<p>*** input was truncated after %ld bytes ***</p>", length );

   FCGX_FPrintF( p_out, "<p>%s</p>", disposition.c_str( ) );
   FCGX_FPrintF( p_out, "<p>Input content read was: %ld bytes</p>", size );
   FCGX_FPrintF( p_out, "<p>(wrote content to file %s...)</p>", file_name.c_str( ) );

   FCGX_FPrintF( p_out, "</body>\n</html>\n" );

#ifdef USE_MULTIPLE_REQUEST_HANDLERS
   FCGX_Finish_r( &request );
#else
   FCGX_Finish( );
#endif
}

int main( int /*argc*/, char* argv[ ] )
{
   int rc = 0;

   try
   {
      umask( WEB_FILES_UMASK );

      g_exe_path = string( argv[ 0 ] );

      size_t pos = g_exe_path.find_last_of( "/" );

      if( pos != string::npos )
         g_exe_path.erase( pos );
      else
         g_exe_path.erase( );

      if( !g_exe_path.empty( ) )
         set_cwd( g_exe_path );

#ifdef USE_MULTIPLE_REQUEST_HANDLERS
      FCGX_Init( );

      // NOTE: Currently Apache's "mod_fcgid" only supports single threaded FCGI servers therefore
      // check whether "mod_fastcgi" is in use before starting any other request handling threads.
#  ifdef USE_MOD_FASTCGI_KLUDGE
      if( has_environment_variable( "_FCGI_MUTEX_" ) ) // i.e. this is only found in "mod_fastcgi"
#  endif
      {
         // NOTE: Start all but one as separate threads - the main thread runs the final handler.
         for( size_t i = 1; i < c_num_handlers; i++ )
         {
            request_handler* p_request_handler = new request_handler;

            p_request_handler->start( );
         }
      }
#endif

      request_handler* p_request_handler = new request_handler;

      p_request_handler->on_start( );
   }
   catch( exception& x )
   {
      rc = 1;
   }
   catch( ... )
   {
      rc = 2;
   }

   return rc;
}
