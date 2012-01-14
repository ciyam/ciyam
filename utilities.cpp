// Copyright (c) 2004
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
#  include <cerrno>
#  include <cstdio>
#  include <climits>
#  include <cstdlib>
#  include <cassert>
#  include <fstream>
#  include <iostream>
#  include <stdexcept>
#  include <sys/stat.h>
#  ifdef __GNUG__
#     include <fcntl.h>
#     include <unistd.h>
#     include <sys/time.h>
#  endif
#  ifdef _WIN32
#     ifdef __BORLANDC__
#        include <cio>
#        include <cfcntl>
#     endif
#     ifdef _MSC_VER
#        include <io.h>
#        include <fcntl.h>
#     endif
#     ifndef STRICT
#        define STRICT // Needed for "windows.h" by various Borland headers.
#     endif
#     define _WIN32_WINNT 0x0502 // Needed for the CreateHardLink API function and PROCESS_MEMORY_COUNTERS_EX.
#     define NOMINMAX
#     include <rpc.h>
#     include <psapi.h>
#     include <windows.h>
#  endif
#endif

#ifdef _WIN32
typedef int mode_t;
#endif

#ifdef __GNUG__
#  define _open open
#  define _chmod chmod
#  define _close close
#endif

#include "utilities.h"

//#define DEBUG

using namespace std;

namespace
{

const unsigned char c_space = 0x20;
const unsigned char c_low_mask = 0x3f;
const unsigned char c_high_mask = 0xc0;

}

string quote( const string& s, char quote_char, char escape_char )
{
   string qs;

   // NOTE: Escape or double the quote character (double if set to '\0').
   // A non '\0' escape character will itself be escaped if present also.
   qs += quote_char;
   for( size_t i = 0, len = s.length( ); i < len; i++ )
   {
      if( s[ i ] == quote_char )
      {
         if( escape_char == '\0' )
            qs += s[ i ];
         else
            qs += escape_char;
      }
      else if( escape_char != '\0' && s[ i ] == escape_char )
         qs += escape_char;

      qs += s[ i ];
   }
   qs += quote_char;

   return qs;
}

#ifndef _WIN32
void uuidgen( unsigned char buf[ ] )
{
   static int fd;

   // NOTE: This initialisation is not threadsafe and should therefore occur prior to multiple thread usage.
   if( !fd )
   {
      fd = open( "/dev/urandom", O_RDONLY );
      assert( fd > 0 );
   }

   read( fd, buf, c_uuid_size );
}
#endif

uuid::uuid( )
{
#ifndef _WIN32
   uuidgen( buf );
#else
   ::UuidCreate( ( UUID* )&buf );
#endif
}

uuid::uuid( const string& str )
{
   if( str.length( ) != c_uuid_size * 2 )
      throw runtime_error( "incorrect string length for uuid string ctor" );

   for( int i = 0; i < c_uuid_size; i++ )
      buf[ i ] = ( hex_nibble( str[ i * 2 ] ) << 4 ) + hex_nibble( str[ ( i * 2 ) + 1 ] );
}

string uuid::as_string( ) const
{
   string str( c_uuid_size * 2, '\0' );
   for( int i = 0; i < c_uuid_size; i++ )
   {
      str[ i * 2 ] = ascii_digit( ( buf[ i ] & 0xf0 ) >> 4 );
      str[ ( i * 2 ) + 1 ] = ascii_digit( buf[ i ] & 0x0f );
   }

   return str;
}

#ifdef __GNUG__
void msleep( unsigned long amt )
{
   timeval t;
   t.tv_sec = amt / 1000;
   t.tv_usec = ( amt % 1000 ) * 1000;

   ::select( 0, 0, 0, 0, &t );
}
#endif
#ifdef _WIN32
void msleep( unsigned long amt ) { ::Sleep( amt ); }
#endif


int get_pid( )
{
#ifndef _WIN32
   return getpid( );
#else
   return ::GetCurrentProcessId( );
#endif
}

int vmem_used( )
{
   int64_t size_kb = 0;

#ifdef _WIN32
   PROCESS_MEMORY_COUNTERS_EX pmc;
   ::GetProcessMemoryInfo( ::GetCurrentProcess( ), ( PPROCESS_MEMORY_COUNTERS )&pmc, sizeof( pmc ) );
   size_kb = ( pmc.PrivateUsage / 1024 );
#else
   ifstream inpf( "/proc/self/status" );

   string next;
   while( getline( inpf, next ) )
   {
      string::size_type pos = next.find( "VmSize:" );
      if( pos != string::npos )
      {
         string remainder( next.substr( pos + 7 ) );

         while( !remainder.empty( ) && ( remainder[ 0 ] == ' ' || remainder[ 0 ] == '\t' ) )
            remainder.erase( 0, 1 );

         for( size_t i = 0; i < remainder.size( ); i++ )
         {
            if( remainder[ i ] >= '0' && remainder[ i ] <= '9' )
               size_kb = ( size_kb * 10 ) + ( remainder[ i ] - '0' );
            else
               break;
         }

         break;
      }
   }
#endif

   return size_kb;
}

bool file_exists( const char* p_name, bool check_link_target )
{
   int rc;
   struct stat statbuf;

#ifdef _WIN32
   ( void )check_link_target;
   rc = stat( p_name, &statbuf );
#else
   if( check_link_target )
      rc = stat( p_name, &statbuf );
   else
      rc = lstat( p_name, &statbuf );
#endif

   return rc == 0;
}

bool file_remove( const char* p_name )
{
   return ::remove( p_name ) == 0;
}

#ifdef _WIN32
bool file_remove( const wchar_t* p_name )
{
   return ::DeleteFileW( p_name ) != 0;
}
#endif

int64_t file_size( const char* p_name )
{
   int64_t retval;

#ifdef _WIN32
   HANDLE hFile = ::CreateFile( p_name,
    GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );
   if( !hFile )
      throw runtime_error( "unable to open file '" + string( p_name ) + "' for input" );

   LARGE_INTEGER pos, npos;
   pos.QuadPart = INT64_C( 0 );
   if( !::SetFilePointerEx( hFile, pos, &npos, FILE_END ) )
   {
      CloseHandle( hFile );
      throw runtime_error( "unexpected SetFilePointerEx error in file_size for file '" + string( p_name ) + "'" );
   }
   retval = npos.QuadPart;
   ::CloseHandle( hFile );
#else
   int fd = _open( p_name, O_RDONLY );
   if( fd <= 0 )
      throw runtime_error( "unable to open file '" + string( p_name ) + "' for input" );

   retval = lseek64( fd, INT64_C( 0 ), SEEK_END );
   if( retval < 0 )
   {
      _close( fd );
      throw runtime_error( "unexpected lseek64 error in file_size for file '" + string( p_name ) + "'" );
   }
   _close( fd );
#endif

   return retval;
}

string file_perms( const char* p_name )
{
   string str;
   struct stat statbuf;
   int rc = stat( p_name, &statbuf );

   if( rc != 0 )
      throw runtime_error( "unable to stat '" + to_string( p_name ) + "'" );

#ifdef _WIN32
   if( statbuf.st_mode & _S_IREAD )
      str += 'r';
   else
      str += '-';

   if( statbuf.st_mode & _S_IWRITE )
      str += 'w';
   else
      str += '-';

   if( statbuf.st_mode & S_IFDIR )
      str += 'x';
   else
      str += '-';

   str = str + str + str;
#else
   if( statbuf.st_mode & S_IRUSR )
      str += 'r';
   else
      str += '-';

   if( statbuf.st_mode & S_IWUSR )
      str += 'w';
   else
      str += '-';

   if( statbuf.st_mode & S_IXUSR )
      str += 'x';
   else
      str += '-';

   if( statbuf.st_mode & S_IRGRP )
      str += 'r';
   else
      str += '-';

   if( statbuf.st_mode & S_IWGRP )
      str += 'w';
   else
      str += '-';

   if( statbuf.st_mode & S_IXGRP )
      str += 'x';
   else
      str += '-';

   if( statbuf.st_mode & S_IROTH )
      str += 'r';
   else
      str += '-';

   if( statbuf.st_mode & S_IWOTH )
      str += 'w';
   else
      str += '-';

   if( statbuf.st_mode & S_IXOTH )
      str += 'x';
   else
      str += '-';
#endif

   return str;
}

void file_perms( const string& name, const string& rwx_perms )
{
   mode_t mode = 0;
   bool invalid = false;

   if( rwx_perms.length( ) != 9 )
      throw runtime_error( "rwx_perms must be a 9 character string" );

   for( size_t i = 0; i < rwx_perms.size( ); i++ )
   {
      char ch( rwx_perms[ i ] );

#ifdef _WIN32
      if( i % 3 == 0 )
      {
         if( ch == 'r' )
            mode |= _S_IREAD;
         else if( ch != '-' )
            invalid = true;
      }
      else if( i % 3 == 1 )
      {
         if( ch == 'w' )
            mode |= _S_IWRITE;
         else if( ch != '-' )
            invalid = true;
      }
      else if( i % 3 == 1 )
      {
         if( ch != 'x' && ch != '-' )
            invalid = true;
      }
#else
      switch( i )
      {
         case 0:
         if( ch == 'r' )
            mode |= S_IRUSR;
         else if( ch != '-' )
            invalid = true;
         break;

         case 1:
         if( ch == 'w' )
            mode |= S_IWUSR;
         else if( ch != '-' )
            invalid = true;
         break;

         case 2:
         if( ch == 'x' )
            mode |= S_IXUSR;
         else if( ch != '-' )
            invalid = true;
         break;

         case 3:
         if( ch == 'r' )
            mode |= S_IRGRP;
         else if( ch != '-' )
            invalid = true;
         break;

         case 4:
         if( ch == 'w' )
            mode |= S_IWGRP;
         else if( ch != '-' )
            invalid = true;
         break;

         case 5:
         if( ch == 'x' )
            mode |= S_IXGRP;
         else if( ch != '-' )
            invalid = true;
         break;

         case 6:
         if( ch == 'r' )
            mode |= S_IROTH;
         else if( ch != '-' )
            invalid = true;
         break;

         case 7:
         if( ch == 'w' )
            mode |= S_IWOTH;
         else if( ch != '-' )
            invalid = true;
         break;

         case 8:
         if( ch == 'x' )
            mode |= S_IXOTH;
         else if( ch != '-' )
            invalid = true;
         break;
      }
#endif
   }

   if( invalid )
      throw runtime_error( "invalid rwx_perms string '" + rwx_perms + "' for '" + name + "'" );

   if( _chmod( name.c_str( ), mode ) != 0 )
      throw runtime_error( "_chmod call failed for '" + name + "'" );
}

void file_link( const char* p_src, const char* p_name, const wchar_t* p_wsrc, const wchar_t* p_wname )
{
   if( !p_src )
      throw runtime_error( "unexpected null p_src in file_link" );

   string name;
   string source( p_src );

   if( p_name )
      name = string( p_name );
   else
   {
      string::size_type pos = source.find_last_of( "/\\" );
      if( pos != string::npos )
         name = source.substr( pos + 1 );
      else
         throw runtime_error( "cannot create a link in same directory with the same name" );
   }
#ifdef _WIN32
   // NOTE: Under Win32 create a "hard link" as "symbolic links" are only supported post Vista.
   char buf[ 128 ];
   bool rc;
   if( p_wsrc )
      rc = ::CreateHardLinkW( p_wname, p_wsrc, 0 );
   else
      rc = ::CreateHardLink( name.c_str( ), source.c_str( ), 0 );

   if( !rc )
   {
      ::FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, 0, ::GetLastError( ), 0, buf, 128, 0 );
      throw runtime_error( buf );
   }
#else
   // NOTE: Under Linux create a "symbolic link" in case source and dest are on different devices.
   // Also when creating a "symbolic link" the full path to the source is required so if it wasn't
   // provided then assume the current working directory. If the link has become "invalid" then it
   // will fail the "access" test for file existence so this can be problematic if wanting to test
   // for existence in order to remove before re-creating.
   string::size_type pos = source.find( '/' );
   if( pos == string::npos )
   {
      char buf[ PATH_MAX ];
      getcwd( buf, PATH_MAX );

      source = string( buf ) + "/" + source;
   }

   if( symlink( source.c_str( ), name.c_str( ) ) != 0 )
      throw runtime_error( "failed to create link (errno = " + to_string( errno ) + ")" );
#endif
}

void file_copy( const char* p_src, const char* p_dest )
{
   ifstream inpf( p_src, ios::in | ios::binary );
   ofstream outf( p_dest, ios::out | ios::binary );

   copy_stream( inpf, outf );
}

void file_append( const char* p_src, const char* p_dest )
{
   ifstream inpf( p_src, ios::in | ios::binary );
   ofstream outf( p_dest, ios::out | ios::app | ios::binary );

   copy_stream( inpf, outf );
}

bool wildcard_match( const char* p_expr, const char* p_data )
{
   switch( p_expr[ 0 ] )
   {
      case '\0':
         return !p_data[ 0 ];

      case '*':
         return wildcard_match( p_expr + 1, p_data ) || p_data[ 0 ] && wildcard_match( p_expr, p_data + 1 );

      case '?':
         return p_data[ 0 ] && wildcard_match( p_expr + 1, p_data + 1 );

      default:
         return p_expr[ 0 ] == p_data[ 0 ] && wildcard_match( p_expr + 1, p_data + 1 );
   }
}

string& replace( std::string& s, const char* p_findstr, const char* p_replstr )
{
   string str;

   size_t from = 0;
   while( true )
   {
      size_t pos = s.find( p_findstr, from );
      if( pos == string::npos )
         break;

      str = s.substr( 0, pos );

      str += p_replstr;

      from = pos + strlen( p_replstr );

      pos += strlen( p_findstr );
      str += s.substr( pos );

      s = str;
   }

   return s;
}

string& escape( string& s, const char* p_chars,
 char esc, const char* p_specials, bool double_escape_specials )
{
   char esc_char( esc );
   if( esc == '\0' )
      esc_char = c_esc;

   size_t offset = !p_specials ? 0 : strlen( p_specials ) / 2;

   for( string::size_type i = 0; i < s.size( ); i++ )
   {
      char c = s[ i ];

      if( c == esc || ( p_chars && strchr( p_chars, c ) ) )
         s.insert( i++, 1, esc_char );
      else if( p_specials )
      {
         for( size_t j = 0; j < offset; j++ )
         {
            if( c == p_specials[ offset + j ] )
            {
               s.insert( i++, 1, esc_char );
               if( double_escape_specials )
                  s.insert( i++, 1, esc_char );

               s[ i ] = p_specials[ j ];
               break;
            }
         }
      }
   }

   return s;
}

string escaped( const string& s, const char* p_chars,
 char esc, const char* p_specials, bool double_escape_specials )
{
   string str;

   char esc_char( esc );
   if( esc == '\0' )
      esc_char = c_esc;

   size_t offset = !p_specials ? 0 : strlen( p_specials ) / 2;

   for( string::size_type i = 0, size = s.size( ); i < size; i++ )
   {
      char c = s[ i ];
      if( c == esc || ( p_chars && strchr( p_chars, c ) ) )
         str += esc_char;
      else if( p_specials )
      {
         for( size_t j = 0; j < offset; j++ )
         {
            if( c == p_specials[ offset + j ] )
            {
               str += esc_char;
               if( double_escape_specials )
                  str += esc_char;

               c = p_specials[ j ];
               break;
            }
         }
      }

      str += c;
   }

   return str;
}

string unescaped( const char* p_start, size_t len, const char* p_specials, char esc )
{
   string s( len, '\0' );

   assert( esc != '\0' && ( !p_specials || strlen( p_specials ) % 2 == 0 ) );

   size_t offset = !p_specials ? 0 : strlen( p_specials ) / 2;

   size_t n = 0;
   bool was_escape = false;
   for( size_t i = 0; i < len; i++ )
   {
      if( was_escape )
      {
         was_escape = false;
         char c = *( p_start + i );

         for( size_t j = 0; j < offset; j++ )
         {
            if( c == p_specials[ j ] )
            {
               c = p_specials[ offset + j ];
               break;
            }
         }

         s[ n++ ] = c;
      }
      else if( *( p_start + i ) == esc )
         was_escape = true;
      else
         s[ n++ ] = *( p_start + i );
   }

   if( n < len )
      s.erase( n, len - n );

   return s;
}

size_t find_end_of_escaped_sequence( const string& s, size_t p, char eos, char esc )
{
   string::size_type pos = string::npos;

   assert( esc != '\0' && eos != '\0' && esc != eos );

   bool is_escape = false;
   for( string::size_type i = p; i < s.size( ); i++ )
   {
      if( is_escape )
         is_escape = false;
      else if( s[ i ] == esc )
         is_escape = true;
      else if( s[ i ] == eos )
      {
         pos = i;
         break;
      }
   }

   return pos;
}

bool has_environment_variable( const char* p_env_var_name )
{
   return getenv( p_env_var_name ) != 0;
}

void replace_environment_variables( string& s, char bc, const char* p_chars, char esc )
{
   string::size_type pos = s.find( bc );
   while( pos != string::npos )
   {
      if( s.size( ) > pos + 1 && s[ pos + 1 ] >= '0' && s[ pos + 1 ] <= '9' )
      {
         pos = s.find( bc, pos + 1 );
         continue;
      }

      string::size_type npos = s.find( bc, pos + 1 );
      if( npos == string::npos )
         break;

      if( npos != pos + 1 )
      {
         string env_var_name( s.substr( pos + 1, npos - pos - 1 ) );

         string env_var_value;
         char* p_env_var( getenv( env_var_name.c_str( ) ) );
         if( p_env_var )
            env_var_value = string( p_env_var );

         if( esc )
            escape( env_var_value, p_chars, esc );

         s.erase( pos, npos - pos + 1 );
         s.insert( pos, env_var_value );

         npos = pos + env_var_value.size( );
      }
      else
      {
         s.erase( pos, 1 );
         npos = pos + 1;
      }

      pos = s.find( bc, npos );
   }
}

string replace_environment_variables( const char* p_str, char bc, const char* p_chars, char esc )
{
   string s( p_str );
   replace_environment_variables( s, bc, p_chars, esc );
   return s;
}

size_t split( const string& s, set< string >& c, char sep, char esc, bool unescape )
{
   vector< string > v;
   split( s, v, sep, esc, unescape );

   for( size_t i = 0; i < v.size( ); i++ )
      c.insert( v[ i ] );

   return c.size( );
}

size_t split( const string& s, deque< string >& c, char sep, char esc, bool unescape )
{
   vector< string > v;
   split( s, v, sep, esc, unescape );

   for( size_t i = 0; i < v.size( ); i++ )
      c.push_back( v[ i ] );

   return c.size( );
}

size_t split( const string& s, vector< string >& c, char sep, char esc, bool unescape )
{
   string::size_type len = s.length( );

   string next;
   bool had_sep = false;
   bool had_escape = false;
   for( size_t i = 0; i < len; i++ )
   {
      had_sep = false;

      if( had_escape )
      {
         had_escape = false;
         if( !unescape && s[ i ] != sep )
            next += esc;
         next += s[ i ];
      }
      else
      {
         if( esc && ( esc != sep ) && s[ i ] == esc )
            had_escape = true;
         else if( s[ i ] == sep )
         {
            if( esc != sep || i == len - 1 || s[ i + 1 ] != sep )
            {
               had_sep = true;
               c.push_back( next );

               next.erase( );
            }
            else
            {
               i++;
               next += sep;
            }   
         }
         else
            next += s[ i ];
      }
   }

   if( had_sep || !next.empty( ) || s.empty( ) )
      c.push_back( next );

   return c.size( );
}

size_t split( const string& s, set< string >& c, const string& sep )
{
   string::size_type spos = 0;
   string::size_type slen( sep.length( ) );
   while( true )
   {
      string::size_type pos = s.find( sep, spos );
      if( pos == string::npos )
      {
         if( !s.empty( ) )
            c.insert( s.substr( spos ) );
         break;
      }

      c.insert( s.substr( spos, pos - spos ) );
      spos = pos + slen;
   }

   return c.size( );
}

size_t split( const string& s, deque< string >& c, const string& sep )
{
   string::size_type spos = 0;
   string::size_type slen( sep.length( ) );
   while( true )
   {
      string::size_type pos = s.find( sep, spos );
      if( pos == string::npos )
      {
         if( !s.empty( ) )
            c.push_back( s.substr( spos ) );
         break;
      }

      c.push_back( s.substr( spos, pos - spos ) );
      spos = pos + slen;
   }

   return c.size( );
}

size_t split( const string& s, vector< string >& c, const string& sep )
{
   string::size_type spos = 0;
   string::size_type slen( sep.length( ) );
   while( true )
   {
      string::size_type pos = s.find( sep, spos );
      if( pos == string::npos )
      {
         if( !s.empty( ) )
            c.push_back( s.substr( spos ) );
         break;
      }

      c.push_back( s.substr( spos, pos - spos ) );
      spos = pos + slen;
   }

   return c.size( );
}

size_t split_size( const string& s, char sep, char esc )
{
   // FUTURE: The "split" function should call a "split_impl" that will take a pointer to
   // a vector which can be set to null when called via this function to be more efficient.
   vector< string > v;
   return split( s, v, sep, esc );
}

string join( const vector< string >& c, char sep, char esc, bool escape )
{
   string s;
   char specials[ 2 ];

   specials[ 0 ] = sep;
   specials[ 1 ] = '\0';

   for( size_t i = 0; i < c.size( ); i++ )
   {
      if( i > 0 && sep )
         s += sep;

      if( !c[ i ].empty( ) )
      {
         if( !escape )
            s += c[ i ];
         else
            s += escaped( c[ i ], specials, esc );
      }
   }

   return s;
}

string join( const vector< string >& c, const string& sep )
{
   string s;
   for( size_t i = 0; i < c.size( ); i++ )
   {
      if( i > 0 )
         s += sep;
      s += c[ i ];
   }

   return s;   
}

size_t setup_arguments( const char* p_input, vector< string >& arguments, char esc, const char* p_specials )
{
   arguments.clear( );

   assert( esc != '"' && esc != '\0' && !isspace( static_cast< unsigned char >( esc ) ) );

   const char* p_start( p_input );
   const char* p_finish( p_input );

   bool in_quotes = false;
   bool had_escape = false;
   bool began_without_quotes = false;

   while( true )
   {
      if( had_escape )
      {
         if( *p_finish == '\0' )
            throw runtime_error( "unexpected trailing escape" );

         had_escape = false;
         ++p_finish;
      }
      else if( *p_finish == esc )
      {
         if( !in_quotes && p_start == p_finish )
            began_without_quotes = true;

         had_escape = true;
         ++p_finish;
      }
      else if( *p_finish == '"' )
      {
         in_quotes = !in_quotes;
         if( !in_quotes && !began_without_quotes )
         {
            if( p_start == p_finish )
               arguments.push_back( string( ) );
            else
               arguments.push_back( unescaped( p_start, p_finish - p_start, p_specials, esc ) );
         }

         if( began_without_quotes )
            ++p_finish;
         else
            p_start = ++p_finish;
      }
      else if( *p_finish == '\0' || isspace( static_cast< unsigned char >( *p_finish ) ) )
      {
         if( in_quotes && *p_finish == '\0' )
            throw runtime_error( "missing expected end-quote" );

         if( in_quotes )
            ++p_finish;
         else
         {
            if( p_input != p_finish && p_start != p_finish )
               arguments.push_back( unescaped( p_start, p_finish - p_start, p_specials, esc ) );

            if( *p_finish == '\0' )
               break;

            p_start = ++p_finish;
            began_without_quotes = false;
         }
      }
      else
      {
         ++p_finish;
         if( !in_quotes )
            began_without_quotes = true;
      }
   }

#ifdef DEBUG
   cout << "\nsetup_arguments:" << endl;
   for( vector< string >::size_type i = 0; i < arguments.size( ); i++ )
      cout << i << " \"" << arguments[ i ] << "\"\n";
   cout << endl;
#endif

   return arguments.size( );
}

void setup_arguments( int argc, const char* argv[ ], vector< string >& arguments, char esc, const char* p_specials )
{
   arguments.clear( );

   for( int i = 1; i < argc; i++ )
   {
      string str( argv[ i ] );
      if( esc )
         unescape( str, p_specials, esc );

      arguments.push_back( str );
   }
}

string buffer_file( const string& file_name )
{
   FILE* fp = fopen( file_name.c_str( ), "rb" );
   if( !fp )
      throw runtime_error( "unable to open file '" + file_name + "' for input" );

   fseek( fp, 0, SEEK_END );
   long size = ftell( fp );

   string str( size, '\0' );

   fseek( fp, 0, SEEK_SET );
   if( fread( &str[ 0 ], 1, ( size_t )size, fp ) != ( size_t )size )
      throw runtime_error( "reading from input file '" + file_name + "'" );

   fclose( fp );

   return str;
}

void write_file( const string& file_name, const string& file_buffer )
{
   FILE* fp = fopen( file_name.c_str( ), "wb" );
   if( !fp )
      throw runtime_error( "unable to open file '" + file_name + "' for output" );

   if( fwrite( file_buffer.data( ), 1, file_buffer.size( ), fp ) != file_buffer.size( ) )
      throw runtime_error( "writing to output file '" + file_name + "'" );

   fclose( fp );
}

void buffer_file_lines( const string& file_name,
 vector< string >& lines, bool skip_blank_lines, bool strip_extra_crs )
{
   ifstream inpf( file_name.c_str( ) );
   if( !inpf )
      throw runtime_error( "unable to open file '" + file_name + "' for input" );

   string next;
   size_t line_num = 0;
   while( getline( inpf, next ) )
   {
      // NOTE: If a text file had been treated as binary during an FTP remove trailing CR.
      if( strip_extra_crs && next.size( ) && next[ next.size( ) - 1 ] == '\r' )
         next.erase( next.size( ) - 1 );

      if( ++line_num == 1 )
      {
         // NOTE: UTF-8 text files will often begin with an identifying sequence "EF BB BF" as the
         // first three characters of the file so if the first byte is "EF" assume UTF-8 and strip.
         if( next.size( ) >= 3 && next[ 0 ] == ( char )0xef )
            next.erase( 0, 3 );
      }

      if( !skip_blank_lines || !next.empty( ) )
         lines.push_back( next );
   }

   if( !inpf.eof( ) )
      throw runtime_error( "unexpected error occurred whilst reading '" + file_name + "' for input" );
}

void buffer_file_lines( const string& file_name, set< string >& lines, bool strip_extra_crs )
{
   ifstream inpf( file_name.c_str( ) );
   if( !inpf )
      throw runtime_error( "unable to open file '" + file_name + "' for input" );

   string next;
   size_t line_num = 0;
   while( getline( inpf, next ) )
   {
      // NOTE: If a text file had been treated as binary during an FTP remove trailing CR.
      if( strip_extra_crs && next.size( ) && next[ next.size( ) - 1 ] == '\r' )
         next.erase( next.size( ) - 1 );

      if( ++line_num == 1 )
      {
         // NOTE: UTF-8 text files will often begin with an identifying sequence "EF BB BF" as the
         // first three characters of the file so if the first byte is "EF" assume UTF-8 and strip.
         if( next.size( ) >= 3 && next[ 0 ] == ( char )0xef )
            next.erase( 0, 3 );
      }

      if( !next.empty( ) )
         lines.insert( next );
   }

   if( !inpf.eof( ) )
      throw runtime_error( "unexpected error occurred whilst reading '" + file_name + "' for input" );
}

bool absolute_path( const string& relative_path, string& absolute_path )
{
   bool found;

#ifdef _WIN32
   char path[ _MAX_PATH ] = { 0 };
   found = ( _fullpath( path, relative_path.c_str( ), sizeof( path ) ) != 0 );
   absolute_path = path;
#else
   char path[ PATH_MAX ] = { 0 };
   found = realpath( relative_path.c_str( ), &path[ 0 ] );
   absolute_path = path;
#endif

   return found;
}

time_t last_modification_time( const string& filename )
{
   struct stat statbuf;

   if( stat( filename.c_str( ), &statbuf ) != 0 )
      throw runtime_error( "cannot stat '" + filename + "' (errno = " + to_string( errno ) + ")" );

   return statbuf.st_mtime;
}

void output_string_as_text_lines( const string& str, ostream& os, size_t max_length, const char* p_prefix, const char* p_suffix )
{
   size_t offs = 0;
   while( true )
   {
      if( p_prefix )
         os << p_prefix;

      if( str.size( ) - offs <= max_length )
      {
         os << str.substr( offs );
         if( p_suffix )
            os << p_suffix;

         break;
      }

      os << str.substr( offs, max_length );
      if( p_suffix )
         os << p_suffix;

      offs += max_length;
   }
}

void uudecode( ostream& outs, const char* p_input, int num_bytes )
{
   int triplets, remainder;
   unsigned char composite;

   triplets = num_bytes / 4;
   remainder = num_bytes % 4;

   // Decode each of the triplets.
   for( int i = 0; i < triplets; i++, p_input++ )
   {
      int j = 2;
      for( composite = *( p_input + 3 ) - c_space; j < 7; j += 2, p_input++ )
          outs.put( ( *p_input - c_space ) | ( ( composite & ( c_high_mask >> j ) ) << j ) );
   }

   // Decode any dangling bytes (0, 1, or 2 bytes which did not form a complete triplet).
   if( remainder > 1 )
   {
      int j = 2;
      remainder--;
      for( composite = *( p_input + remainder ) - c_space;  j <  ( 2 * remainder + 1 ); j += 2, p_input++ )
        outs.put( ( *p_input - c_space ) | ( ( composite & ( c_high_mask >> j ) ) << j ) );
   }
}

void uuencode( const char* p_data, int num_bytes, ostream& outs )
{
   int triplets, remainder;
   unsigned char composite;

   triplets = num_bytes / 3;
   remainder = num_bytes % 3;

   // Encode each of the triplets.
   for( int i = 0; i < triplets; i++ )
   {
      composite = 0;
      for( int j = 2; j < 7; j += 2, p_data++ )
      {
         outs.put( ( *p_data & c_low_mask ) + c_space );
         composite |= ( *p_data & c_high_mask ) >> j;
      }

      outs.put( composite + c_space );
   }

   // Encode any dangling bytes (0, 1, or 2 bytes which do not form a complete triplet).
   if( remainder )
   {
      composite = 0;
      for( int j = 2; j < 2 * remainder + 1; j += 2, p_data++ )
      {
          outs.put( ( *p_data & c_low_mask ) + c_space );
          composite |= ( *p_data & c_high_mask ) >> j;
      }

      outs.put( composite + c_space );
   }
}

string decode_quoted_printable( const string& data )
{
   string s;

   for( size_t i = 0; i < data.size( ); i++ )
   {
      if( data[ i ] != '=' )
         s += data[ i ];
      else
      {
         if( i != data.size( ) - 1 && data[ i + 1 ] == '\n' )
         {
            ++i;
            continue;
         }

         s += ( hex_nibble( data[ i + 1 ] ) << 4 ) | hex_nibble( data[ i + 2 ] );
         i += 2;
      }
   }

   return s;
}

string encode_quoted_printable( const string& data, int max_chars_per_line )
{
   string s;
   int num_chars = 0;
   int almost_max_chars = ( int )( max_chars_per_line * 0.9 );

   for( size_t i = 0; i < data.length( ); i++ )
   {
      if( num_chars == max_chars_per_line - 1 )
      {
         if( data[ i ] != '\n' )
            s += "=";

         s += "\r\n";
         num_chars = 0;

         if( data[ i ] == '\n' )
            continue;
      }

      if( num_chars > almost_max_chars && ( data[ i ] == 9 || data[ i ] == 32 ) )
      {
         s += data[ i ];
         s += "=";
         s += "\r\n";
         num_chars = 0;
         continue;
      }

      if( data[ i ] == '\n' )
      {
         s += "\r\n";
         num_chars = 0;
         continue;
      }

      bool needs_encoding = false;

      if( data[ i ] != 9 && ( data[ i ] < 32 || data[ i ] > 126 || data[ i ] == 61 ) )
         needs_encoding = true;

      if( num_chars == 0
       && ( data[ i ] == 9 || data[ i ] == 32 )
       && ( i == data.length( ) - 1 || data[ i + 1 ] == '\n' ) )
         needs_encoding = true;

      if( !needs_encoding )
      {
         ++num_chars;
         s += data[ i ];
      }
      else
      {
         s += '=';
         ++num_chars;

         if( num_chars > max_chars_per_line - 2 )
         {
            s += "\r\n";
            s += '=';
            num_chars = 1;
         }

         ++num_chars;
         s += ascii_digit( ( data[ i ] & 0xf0 ) >> 4, true );

         ++num_chars;
         s += ascii_digit( data[ i ] & 0x0f, true );
      }
   }

   return s;
}

void read_strings( const string& filename, map< string, string >& strings,
 const char* p_prefix, bool unescape_data, char esc, const char* p_specials )
{
   ifstream inpf( filename.c_str( ) );
   if( !inpf )
      throw runtime_error( "unable to open file '" + filename + "' for input" );

   string next;
   bool is_first = true;
   while( getline( inpf, next ) )
   {
      // NOTE: In case the string file had been treated as binary during an FTP transfer remove trailing CR.
      if( next.size( ) && next[ next.size( ) - 1 ] == '\r' )
         next.erase( next.size( ) - 1 );

      if( is_first )
      {
         is_first = false;

         // NOTE: UTF-8 text files will often begin with an identifying sequence "EF BB BF" as the
         // first three characters of the file so if the first byte is "EF" assume UTF-8 and strip.
         if( next.size( ) >= 3 && next[ 0 ] == ( char )0xef )
            next.erase( 0, 3 );
      }

      if( next.empty( ) )
         continue;

      string::size_type pos = next.find( ' ' );
      if( pos == string::npos )
         throw runtime_error( "unexpected format for string '" + next + "'" );

      string key( p_prefix ? p_prefix : "" );
      key += next.substr( 0, pos );

      size_t npos = pos + 1;
      while( npos < next.size( ) && next[ npos ] == ' ' )
         ++npos;
      next.erase( 0, npos );

      while( !next.empty( ) && next[ next.size( ) - 1 ] == ' ' )
         next.erase( next.size( ) - 1 );

      if( next.length( ) < 2 || next[ 0 ] != '"' || next[ next.length( ) - 1 ] != '"' )
         throw runtime_error( "unexpected format for string data '" + next + "'" );

      string data( next.substr( 1, next.length( ) - 2 ) );

      if( unescape_data )
         unescape( data, p_specials, esc );

      if( strings.count( key ) )
         throw runtime_error( "duplicate string key '" + key + "' found reading strings" );

      strings.insert( string_value_type( key, data ) );
   }

   if( !inpf.eof( ) )
      throw runtime_error( "unexpected error occurred whilst reading '" + filename + "' for input" );
}

