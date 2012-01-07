// Copyright (c) 2003
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

#ifdef __GNUG__
#  include <pwd.h>
#  include <paths.h>
#  include <fcntl.h>
#  include <stdio.h>
#  include <stdlib.h>
#  include <unistd.h>
#  include <termios.h>
extern "C"
{
#  include <readline/readline.h>
}
#else
#  ifdef _WIN32
#     include <conio.h>
#  else
#     error unsupported compiler platform...
#  endif
#endif

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <iostream>
#endif

#include "console.h"

using namespace std;

char get_char( const char* prompt )
{
   if( prompt[ 0 ] != 0 )
   {
      cout << prompt;
      cout.flush( );
   }

#ifdef _WIN32
   return ( char )_getch( );
#else
   struct termios systerm;

   int infd = STDIN_FILENO;

   // NOTE: If standard input is not a terminal (such as is the case with redirected input) then
   // instead attempt to open the TTY device directly.
   if( !isatty( infd ) )
   {
      if( ( infd = open( _PATH_TTY, O_RDWR ) ) == -1 )
      {
         cerr << "fatal: unable to open terminal device" << endl;
         exit( 1 );
      }
   }

   tcgetattr( infd, &systerm );
   systerm.c_lflag &= ~( ICANON | ECHO );
   systerm.c_cc[ VMIN ] = 0;
   systerm.c_cc[ VTIME ] = 1;
   tcsetattr( infd, TCSAFLUSH, &systerm );

   char ch;
   while( true )
   {
      if( !read( infd, &ch, 1 ) )
         continue;

      break;
   }

   fflush( stdout );
   systerm.c_lflag |= ( ICANON | ECHO );
   tcsetattr( infd, TCSANOW, &systerm );

   if( infd != STDIN_FILENO )
      close( infd );

   return ( int )ch;
#endif
}

string get_line( const char* prompt )
{
   string str;

#ifdef __GNUG__
   // NOTE: If standard input is not a terminal (such as is the case with redirected input) then
   // don't use "readline" (as it seems to be causing problems).
   if( isatty( STDIN_FILENO ) )
   {
      char* p = readline( prompt );
      if( p )
      {
         str = string( p );
         free( p );
      }
   }
   else
   {
      if( prompt[ 0 ] != 0 )
         cout << prompt;
      getline( cin, str );
   }

   return str;
#else
   if( prompt[ 0 ] != 0 )
      cout << prompt;

   getline( cin, str );
   return str;
#endif
}

#ifdef __GNUG__
string get_password( const char* prompt )
{
   string str;
   char* p = getpass( prompt );
   str = string( p );
   memset( p, '\0', str.size( ) );
   return str;
}
#endif

#ifdef _WIN32
string get_password( const char* prompt )
{
   string str;
   char buf[ 128 ] = "";

   if( prompt[ 0 ] != 0 )
   {
      cout << prompt;
      cout.flush( );
   }

   int pos = 0;
   while( pos < sizeof( buf ) )
   {
      char ch = _getch( );

      if( ch == '\b' )
      {
         if( pos > 0 )
            --pos;
      }
      else if( ch == '\r' )
      {
         buf[ pos ] = '\0';
         break;
      }
      else
         buf[ pos++ ] = ch;
   }

   if( buf[ 0 ] != '\0' )
   {
      str = string( buf );
      memset( buf, '\0', str.size( ) );
   }

   cout << '\n';

   return str;
}
#endif

