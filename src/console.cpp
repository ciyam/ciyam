// Copyright (c) 2003-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
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
#  error unsupported compiler platform...
#endif
#include <stdexcept>

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <iostream>
#endif

#include "console.h"

#include "config.h"
#include "numeric.h"
#include "progress.h"

#ifdef __GNUG__
#  define _write ::write
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

const int c_max_pwd_len = 128;

void get_line_using_get_char( string& str )
{
   while( true )
   {
      cout.flush( );
      char ch = get_char( 0, false );

      if( ( ch == '\r' ) || ( ch == '\n' ) || ( ch == 0x03 ) ) // i.e. ctrl-c
      {
         cout << endl;

         if( ch == 0x03 )
            str.erase( );

         break;
      }
      else if( ( ch == '\b' ) || ( ch == 0x21 ) ) // i.e. BS or ctrl-u
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
}

}

bool is_stdout_console( )
{
#ifdef __GNUG__
   if( isatty( STDOUT_FILENO ) )
      return true;
#else
   if( _isatty( _fileno( stdout ) ) )
      return true;
#endif

   return false;
}

char get_char( const char* p_prompt, bool flush_input )
{
   if( p_prompt && p_prompt[ 0 ] != 0 )
   {
      cout << p_prompt;
      cout.flush( );
   }

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
}

void get_line( string& str, const char* p_prompt, bool use_cin )
{
#ifdef __GNUG__
   // NOTE: If standard input is not a terminal (such as is the case with redirected input)
   // then don't use "readline" (as it does not behave as one might expect it would).
#  ifdef RDLINE_SUPPORT
   if( isatty( STDIN_FILENO ) )
   {
      char* p = readline( p_prompt );
      if( p )
      {
         size_t len = strlen( p );

         str.resize( len );
         memcpy( &str[ 0 ], p, len );

         memset( p, '\0', len );
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
         get_line_using_get_char( str );
   }
#else
   if( p_prompt && p_prompt[ 0 ] != 0 )
      cout << p_prompt;

   if( use_cin )
      getline( cin, str );
   else
      get_line_using_get_char( );
#endif
}

#ifdef __GNUG__
string get_password( const char* p_prompt, char* p_buf, size_t buflen )
{
   string str;

   char* p = getpass( p_prompt );
   size_t len = strlen( p );

   if( p_buf && buflen )
   {
      if( len >= buflen )
         throw runtime_error( "get_password buflen was exceeded" );

      strncpy( p_buf, p, len );
      p_buf[ len ] = '\0';
   }
   else
      str = string( p );

   memset( p, '\0', len );

   return str;
}
#endif

void put_line( const char* p_chars, size_t len, bool append_lf )
{
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

   if( _write( outfd, p_chars, len ) == -1 )
      throw runtime_error( "unexpected put_line write p_chars failure" );

   if( append_lf )
   {
      if( _write( outfd, &ch, 1 ) == -1 )
         throw runtime_error( "unexpected put_line write line feed failure" );
   }

   if( outfd != STDOUT_FILENO )
      close( outfd );
}

void console_progress::output_progress( const string& message, unsigned long num, unsigned long total )
{
   int new_length = 0;

   bool is_space = false;

   if( message == string( " " ) )
      is_space = true;

   if( is_space || message.length( ) != 1 )
      new_length = message.length( );
   else
      new_length = output_length + 1;

   if( is_space || message.length( ) != 1 )
      cout << '\r';

   cout << output_prefix << message;

   string formatted;

   if( !total )
      formatted = format_numeric( num, "############" );
   else
      formatted = format_percentage( num_decimals, previous_num, num, total );

   if( num || total )
   {
      new_length += formatted.length( );

      if( is_space )
      {
         cout << '(';
         ++new_length;
      }

      cout << formatted;

      if( is_space )
      {
         cout << ')';
         ++new_length;
      }
   }

   if( output_length > new_length )
   {
      cout << string( output_length - new_length, ' ' );
      cout << string( output_length - new_length, '\b' );
   }

   if( !message.length( ) )
   {
      if( output_prefix.empty( ) )
         cout << '\r';
      else
         cout << '\n';

      output_prefix.erase( );
   }

   cout.flush( );

   output_length = new_length;
}
