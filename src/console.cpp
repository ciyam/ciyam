// Copyright (c) 2003-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2019 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifdef __GNUG__
#  include <pwd.h>
#  include <paths.h>
#  include <fcntl.h>
#  include <stdio.h>
#  include <string.h>
#  include <stdlib.h>
#  include <unistd.h>
#  include <termios.h>
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

#include "config.h"

#ifdef __GNUG__
#  ifdef RDLINE_SUPPORT
extern "C"
{
#  include <readline/readline.h>
}
#  endif
#endif

using namespace std;

namespace
{

string get_line_using_get_char( )
{
   string str;

   while( true )
   {
      cout.flush( );
      char ch = get_char( 0, false );

      if( ch == '\r' || ch == '\n' || ch == 0x03 ) // i.e. ctrl-c
      {
         cout << endl;

         if( ch == 0x03 )
            str.erase( );

         break;
      }
#ifdef _WIN32
      else if( ch == '\b' || ch == 0x1b ) // i.e. BS or ESC
#else
      else if( ch == '\b' || ch == 0x21 ) // i.e. BS or ctrl-u
#endif
      {
         if( str.length( ) )
         {
            if( ch != '\b' )
            {
               cout << string( str.length( ), '\b' )
                << string( str.length( ), ' ' ) << string( str.length( ), '\b' );

               str.erase( );
            }
            else
            {
               cout << '\b' << ' ' << '\b';
               str = str.substr( 0, str.length( ) - 1 );
            }
         }
      }
      else
      {
         str += ch;
         cout << ch;
      }
   }

   return str;
}

}

char get_char( const char* p_prompt, bool flush_input )
{
   if( p_prompt && p_prompt[ 0 ] != 0 )
   {
      cout << p_prompt;
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

   if( !flush_input )
      tcsetattr( infd, TCSANOW, &systerm );
   else
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

string get_line( const char* p_prompt, bool use_cin )
{
   string str;

#ifdef __GNUG__
   // NOTE: If standard input is not a terminal (such as is the case with redirected input)
   // then don't use "readline" (as it does not behave as one might expect it would).
#  ifdef RDLINE_SUPPORT
   if( isatty( STDIN_FILENO ) )
   {
      char* p = readline( p_prompt );
      if( p )
      {
         str = string( p );
         free( p );
      }
   }
   else
#  endif
   {
      if( p_prompt && p_prompt[ 0 ] != 0 )
         cout << p_prompt;

      if( use_cin )
         getline( cin, str );
      else
         str = get_line_using_get_char( );
   }

   return str;
#else
   if( p_prompt && p_prompt[ 0 ] != 0 )
      cout << p_prompt;

   if( use_cin )
      getline( cin, str );
   else
      str = get_line_using_get_char( );

   return str;
#endif
}

#ifdef __GNUG__
string get_password( const char* p_prompt )
{
   string str;
   char* p = getpass( p_prompt );
   str = string( p );
   memset( p, '\0', str.size( ) );
   return str;
}
#endif

#ifdef _WIN32
string get_password( const char* p_prompt )
{
   string str;
   char buf[ 128 ] = "";

   if( p_prompt && p_prompt[ 0 ] != 0 )
   {
      cout << p_prompt;
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

void put_line( const char* p_chars, size_t len )
{
#ifdef _WIN32
   for( size_t i = 0; i < len; i++ )
      _putch( *( p_chars + i ) );
   _putch( '\n' );
#else
  int outfd = STDOUT_FILENO;

   // NOTE: If standard output is not a terminal (such as is the case with redirected output) then
   // instead attempt to open the TTY device directly.
   if( !isatty( outfd ) )
   {
      if( ( outfd = open( _PATH_TTY, O_RDWR ) ) == -1 )
      {
         cerr << "fatal: unable to open terminal device" << endl;
         exit( 1 );
      }
   }

   char ch = '\n';

   ( void )write( outfd, p_chars, len );
   ( void )write( outfd, &ch, 1 );

   if( outfd != STDOUT_FILENO )
      close( outfd );
#endif
}

