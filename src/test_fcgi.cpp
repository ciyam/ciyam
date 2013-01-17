// Copyright (c) 2013 CIYAM Open Developers

#include <cstring>
#include <cstdlib>

#include "fcgiapp.h"

using namespace std;

int main( )
{
   FCGX_Stream* in;
   FCGX_Stream* out;
   FCGX_Stream* err;

   FCGX_ParamArray envp;

   while( FCGX_Accept( &in, &out, &err, &envp ) >= 0 )
   {
      FCGX_FPrintF( out, "Content-type: text/html\n" );

      const char* p_param = FCGX_GetParam( "HTTP_COOKIE", envp );

      if( !p_param )
         FCGX_FPrintF( out, "Set-Cookie: TEST=TEST_COOKIE\n" );

      FCGX_FPrintF( out, "\r\n\r\n<html>\n<head>\n   <title>Sample FCGI Post</title>\n</head>\n<body>\n" );

      if( p_param )
         FCGX_FPrintF( out, "<p>Found HTTP_COOKIE: %s</p>\n", p_param );

      const char* p_query = FCGX_GetParam( "QUERY_STRING", envp );
      if( p_query && strlen( p_query ) )
         FCGX_FPrintF( out, "<p>Query String: %s</p>\n", p_query );

      FCGX_FPrintF( out, "<pre>\nPosted Data: " );
      while( true )
      {
         int i = FCGX_GetChar( in );
         if( i == -1 )
            break;

         FCGX_PutChar( i, out );
      }

      FCGX_FPrintF( out, "</pre>\n" );
      FCGX_FPrintF( out, "</body>\n</html>\n" );
   }

   FCGX_Finish( );

   return 0;
}

