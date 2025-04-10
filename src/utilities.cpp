// Copyright (c) 2004-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <cerrno>
#  include <cstdio>
#  include <cassert>
#  include <climits>
#  include <cstdlib>
#  include <cstring>
#  include <fstream>
#  include <iostream>
#  include <algorithm>
#  include <stdexcept>
#  include <sys/stat.h>
#  ifdef __GNUG__
#     include <grp.h>
#     include <pwd.h>
#     include <fcntl.h>
#     include <utime.h>
#     include <unistd.h>
#     include <sys/stat.h>
#     include <sys/time.h>
#  endif
#  ifdef _WIN32
#     ifdef __BORLANDC__
#        include <cio>
#        include <dir.h>
#        include <cfcntl>
#     endif
#     ifdef _MSC_VER
#        include <io.h>
#        include <fcntl.h>
#        include <direct.h>
#     endif
#     ifndef STRICT
#        define STRICT // Needed for "windows.h" by various Borland headers.
#     endif
#     define _WIN32_WINNT 0x0502 // Needed for the CreateHardLink API function and PROCESS_MEMORY_COUNTERS_EX.
#     define NOMINMAX
#     include <rpc.h>
#     include <psapi.h>
#     include <windows.h>
#     include <sys/utime.h>
#  endif
#endif

#ifdef _WIN32
typedef int mode_t;
#endif

#ifdef __GNUG__
#  define _open open
#  define _chmod chmod
#  define _close close
#  define _chdir chdir
#  define _mkdir mkdir
#  define _rmdir rmdir
#  define _utime utime
#  define _getcwd getcwd
#  define _putenv putenv
#  define _utimbuf utimbuf
#  define _MAX_PATH PATH_MAX
#endif

#include "utilities.h"

//#define DEBUG

using namespace std;

namespace
{

const int c_char_size = 256;

const unsigned char c_space = 0x20;
const unsigned char c_low_mask = 0x3f;
const unsigned char c_high_mask = 0xc0;

const unsigned int c_tail_buffer_size = 1024;
const unsigned int c_tail_maximum_lines = 10000;

#ifndef _WIN32
const int c_open_directory_perms = S_IRWXU | S_IRWXG | S_IRWXO;
const int c_group_directory_perms = S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH;
const int c_standard_directory_perms = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
const int c_restricted_directory_perms = S_IRWXU | S_IRGRP | S_IXGRP;
const int c_confidential_directory_perms = S_IRWXU;

const int c_default_directory_perms = c_standard_directory_perms;
#endif

const char c_soundex_map_values[ ] = "01230120022455012623010202";

const char* const c_env_var_characters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";

}

string::size_type find_nth_occurrence( const string& s, char ch, size_t n )
{
   string::size_type pos = string::npos;

   for( size_t i = 0; i < n; i++ )
   {
      if( pos == string::npos )
         pos = s.find( ch );
      else
         pos = s.find( ch, pos + 1 );

      if( pos == string::npos )
         break;
   }

   return pos;
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

bool are_hex_nibbles( const char* p_str )
{
   bool retval = false;

   while( p_str )
   {
      char ch = *p_str;

      retval = true;

      if( ch < '0' || ch > '9' )
      {
         if( ch != 'a' && ch != 'A'
          && ch != 'b' && ch != 'B'
          && ch != 'c' && ch != 'C'
          && ch != 'd' && ch != 'D'
          && ch != 'e' && ch != 'E'
          && ch != 'f' && ch != 'F' )
         {
            retval = false;
            break;
         }
      }

      ++p_str;
      if( *p_str == '\0' )
         break;
   }

   return retval;
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

   if( read( fd, buf, c_uuid_size ) != c_uuid_size )
      throw runtime_error( "unexpected failure to create uuid" );
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

uint32_t uuid::as_uint32_t( ) const
{
   return *( uint32_t* )( &buf[ c_uuid_size - sizeof( uint32_t ) ] );
}

uint64_t uuid::as_uint64_t( ) const
{
   return *( uint64_t* )( &buf[ c_uuid_size - sizeof( uint64_t ) ] );
}

void uuid::copy_bytes( unsigned char* p_buf, size_t num )
{
   if( ( num == 0 ) || ( num > c_uuid_size ) )
      throw runtime_error( "invalid num value " + to_string( num ) + " in uuid::copy_bytes" );

   memcpy( p_buf, buf, num );
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

uint32_t crc32( unsigned char* p_data, uint64_t len )
{
   uint32_t crc = 0xFFFFFFFF;

   while( len-- )
   {
      crc ^= *p_data++;

      for( int k = 0; k < 8; k++ )
         //CRC-32C polynomial 0x1EDC6F41 in reversed bit order.
         crc = crc & 1 ? ( crc >> 1 ) ^ 0x82f63b78 : crc >> 1;
   }

   return ~crc;
}

string soundex( const char* p_str, bool skip_prefix_specials )
{
   string retval;

   if( p_str && p_str[ 0 ] )
   {
      size_t offset = 0;

      char ch = toupper( p_str[ 0 ] );

      if( ( ch < 'A' ) || ( ch > 'Z' ) )
         ch = '?';

      char last_ch = ch;

      retval += ch;

      while( true )
      {
         ch = toupper( p_str[ ++offset ] );

         if( ch == 0 )
            break;

         if( ch == last_ch )
            continue;

         last_ch = ch;

         // NOTE: If requested will change the prefix
         // character for simple cases like "Knowing"
         // (from a 'K' to an 'N').
         if( ( offset == 1 ) && skip_prefix_specials )
         {
            bool replace = false;

            if( ( ch == 'N' ) && ( retval == "G" ) )
               replace = true;

            if( ( ch == 'N' ) && ( retval == "K" ) )
               replace = true;

            if( ( ch == 'N' ) && ( retval == "M" ) )
               replace = true;

            if( ( ch == 'N' ) && ( retval == "P" ) )
               replace = true;

            if( ( ch == 'S' ) && ( retval == "P" ) )
               replace = true;

            if( ( ch == 'T' ) && ( retval == "P" ) )
               replace = true;

            if( ( ch == 'R' ) && ( retval == "W" ) )
               replace = true;

            if( replace )
            {
               retval[ 0 ] = ch;
               continue;
            }
         }

         if( ( ch >= 'A' ) && ( ch <= 'Z' ) )
         {
            char mapped = c_soundex_map_values[ ch - 65 ];

            if( mapped != '0' )
               retval += mapped;

            if( retval.length( ) > 3 )
               break;
         }
      }
   }

   while( retval.length( ) < 4 )
      retval += '0';

   return retval;
}

string random_characters( size_t minimum, size_t max_extra, printable_type type )
{
   uuid entropy;

   string retval;

   size_t extra = 0;
   size_t offset = 0;

   unsigned char buf[ c_uuid_size ];

   if( max_extra )
      extra = ( entropy.as_uint64_t( ) % ( max_extra + 1 ) );

   size_t total_chars = ( minimum + extra );

   for( size_t i = 0; i < total_chars; i++ )
   {
      if( ( i % 8 ) == 0 )
      {
         offset = 0;

         entropy = uuid( );
         entropy.copy_bytes( buf );
      }

      unsigned char next = buf[ offset++ ];

      if( type == e_printable_type_numeric )
         retval += '0' + ( next % 10 );
      else if( type == e_printable_type_alpha_lower )
         retval += 'a' + ( next % 26 );
      else if( type == e_printable_type_alpha_mixed )
      {
         next = ( next % 52 );

         if( next < 26 )
            retval += 'A' + next;
         else
            retval += 'a' + ( next - 26 );
      }
      else if( type == e_printable_type_alpha_numeric )
      {
         next = ( next % 62 );

         if( next < 10 )
            retval += '0' + next;
         else if( next < 36 )
            retval += 'A' + ( next - 10 );
         else
            retval += 'a' + ( next - 36 );
      }
   }

   return retval;
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
#  define GET_PMC_USAGE( PMC_STRUCT, PMC_PARAM )\
   PMC_STRUCT pmc; \
   ::GetProcessMemoryInfo( ::GetCurrentProcess( ), ( PPROCESS_MEMORY_COUNTERS )&pmc, sizeof( pmc ) ); \
   size_kb = ( pmc.PMC_PARAM / 1024 );

   // NOTE: For newer versions of Windws PagefileUsage returns zero so use PrivateUsage instead.
#  if( defined( NTDDI_WIN7 ) || defined( NTDDI_WS08 ) )
      GET_PMC_USAGE( PROCESS_MEMORY_COUNTERS_EX, PrivateUsage )
#  else
      GET_PMC_USAGE( PROCESS_MEMORY_COUNTERS, PagefileUsage )
#  endif
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

string get_cwd( bool change_backslash_to_forwardslash )
{
   char buf[ _MAX_PATH ];

   if( !_getcwd( buf, _MAX_PATH ) )
      throw runtime_error( "unexpected error occurred calling _getcwd" );

#ifndef _WIN32
   ( void )change_backslash_to_forwardslash;
#else
   if( change_backslash_to_forwardslash )
   {
      for( size_t i = 0; i < _MAX_PATH; i++ )
      {
         if( buf[ i ] == '\\' )
            buf[ i ] = '/';
      }
   }
#endif

   return buf;
}

void set_cwd( const char* p_name, bool* p_rc )
{
   if( _chdir( p_name ) != 0 )
   {
      if( p_rc )
         *p_rc = false;
      else
         throw runtime_error( "unable to set cwd to '" + string( p_name ) + "'" );
   }
   else if( p_rc )
      *p_rc = true;
}

temp_umask::temp_umask( int um )
{
#ifdef _WIN32
   ( void )um;
#else
   oum = umask( um );
#endif
}

temp_umask::~temp_umask( )
{
#ifndef _WIN32
   umask( oum );
#endif
}

void create_dir( const char* p_name, bool* p_rc, dir_perms perms, int um )
{
#ifdef _WIN32
   ( void )perms;
   ( void )umask;

   if( _mkdir( p_name ) != 0 )
#else
   int oum = umask( um );
   int pval = c_default_directory_perms;

   switch( perms )
   {
      case e_dir_perms_open:
      pval = c_open_directory_perms;
      break;

      case e_dir_perms_group:
      pval = c_group_directory_perms;
      break;

      case e_dir_perms_standard:
      pval = c_standard_directory_perms;
      break;

      case e_dir_perms_restricted:
      pval = c_restricted_directory_perms;
      break;

      case e_dir_perms_confidential:
      pval = c_confidential_directory_perms;
      break;
   }

   if( _mkdir( p_name, pval ) )
#endif
   {
      if( p_rc )
         *p_rc = false;
      else
      {
#ifndef _WIN32
         umask( oum );
#endif
         throw runtime_error( "unable to create directory '" + string( p_name ) + "'" );
      }
   }
   else
   {
      if( p_rc )
         *p_rc = true;
   }

#ifndef _WIN32
   umask( oum );
#endif
}

bool dir_exists( const char* p_name, bool check_link_target )
{
   bool rc = false;
   struct stat statbuf;

#ifdef _WIN32
   ( void )check_link_target;

   if( stat( p_name, &statbuf ) == 0 && S_ISDIR( statbuf.st_mode ) )
      rc = true;
#else
   if( check_link_target )
   {
      if( stat( p_name, &statbuf ) == 0 && S_ISDIR( statbuf.st_mode ) )
         rc = true;
   }
   else
   {
      if( lstat( p_name, &statbuf ) == 0 && S_ISDIR( statbuf.st_mode ) )
         rc = true;
   }
#endif

   return rc;
}

void remove_dir( const char* p_name, bool* p_rc )
{
   if( _rmdir( p_name ) != 0 )
   {
      if( p_rc )
         *p_rc = false;
      else
         throw runtime_error( "unable to remove directory '" + string( p_name ) + "'" );
   }
}

bool file_touch( const char* p_name, time_t* p_tm, bool create_if_not_exists, bool force_sync )
{
   struct _utimbuf ut;
   struct _utimbuf* p_ut = 0;

   if( p_tm )
   {
      ut.actime = *p_tm;
      ut.modtime = *p_tm;

      p_ut = &ut;
   }

   int rc = _utime( p_name, p_ut );

   if( rc != 0 && create_if_not_exists )
   {
      if( !file_exists( p_name ) )
      {
         if( !force_sync )
         {
            ofstream outf( p_name );
            outf.close( );
         }
         else
         {
#ifndef _WIN32
            int fd = open( p_name, O_RDWR | O_CREAT, S_IREAD | S_IWRITE | S_IRGRP | S_IROTH );

            if( fd > 0 )
            {
               fsync( fd );
               close( fd );
            }
#endif
         }

         if( file_exists( p_name ) )
            rc = 0;
      }
   }

   return rc == 0;
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

bool file_rename( const char* p_old_name, const char* p_new_name )
{
   return ::rename( p_old_name, p_new_name ) == 0;
}

int64_t file_size( const char* p_name, unsigned char* p_hdr, size_t hdr_size )
{
   int64_t retval;

#ifdef _WIN32
   HANDLE hFile = ::CreateFile( p_name,
    GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0 );

   if( hFile == INVALID_HANDLE_VALUE )
      throw runtime_error( "unable to open file '" + string( p_name ) + "' for input in file_size" );

   if( p_hdr && hdr_size )
   {
      size_t num = 0;

      ::ReadFile( hFile, p_hdr, hdr_size, &num, 0 );

      if( num != hdr_size )
         memset( p_hdr, '\0', hdr_size );
   }

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
   if( p_hdr && hdr_size )
   {
      int fd = _open( p_name, O_RDONLY );

      if( fd <= 0 )
         throw runtime_error( "unable to open file '" + string( p_name ) + "' for input in file_size" );

      if( read( fd, p_hdr, hdr_size ) != hdr_size )
         memset( p_hdr, '\0', hdr_size );

      retval = lseek64( fd, INT64_C( 0 ), SEEK_END );

      if( retval < 0 )
      {
         _close( fd );
         throw runtime_error( "unexpected lseek64 error in file_size for file '" + string( p_name ) + "'" );
      }

      _close( fd );
   }
   else
   {
      struct stat statbuf;

      if( stat( p_name, &statbuf ) < 0 )
         throw runtime_error( "unable to stat '" + to_string( p_name ) + "'" );

      retval = statbuf.st_size;

      // NOTE: If "p_hdr" is null but "hdr_size" is non-zero
      // and the stat.st_size is zero will try performing an
      // open and fsync before repeating the stat command.
      if( !retval && hdr_size )
      {
         int fd = _open( p_name, O_RDONLY );

         if( fd <= 0 )
            throw runtime_error( "unable to open file '" + string( p_name ) + "' for input in file_size" );

         fsync( fd );

         _close( fd );

         if( stat( p_name, &statbuf ) < 0 )
            throw runtime_error( "unable to stat '" + to_string( p_name ) + "'" );

         retval = statbuf.st_size;
      }
   }
#endif

   return retval;
}

string file_user( const char* p_name )
{
   string str;

   struct stat statbuf;

   int rc = stat( p_name, &statbuf );

   if( rc != 0 )
      throw runtime_error( "unable to stat '" + to_string( p_name ) + "'" );

#ifdef __GNUG__
   struct passwd* p_passwd = getpwuid( statbuf.st_uid );

   if( p_passwd )
      str = p_passwd->pw_name;
#else
#  error need getpwuid equivelent for this compiler
#endif

   return str;
}

string file_group( const char* p_name )
{
   string str;

   struct stat statbuf;

   int rc = stat( p_name, &statbuf );

   if( rc != 0 )
      throw runtime_error( "unable to stat '" + to_string( p_name ) + "'" );

#ifdef __GNUG__
   struct group* p_group = getgrgid( statbuf.st_gid );

   if( p_group )
      str = p_group->gr_name;
#else
#  error need getpwuid equivelent for this compiler
#endif

   return str;
}

string file_perms( const char* p_name )
{
   string str;

   struct stat statbuf;

   int rc = stat( p_name, &statbuf );

   if( rc != 0 )
      throw runtime_error( "unable to stat '" + to_string( p_name ) + "'" );

#ifdef _WIN32
   string extra;

   if( statbuf.st_mode & _S_IREAD )
      str += 'r';
   else
      str += '-';
   extra += str;

   if( statbuf.st_mode & _S_IWRITE )
      str += 'w';
   else
      str += '-';
   extra += '-';

   if( statbuf.st_mode & S_IFDIR )
   {
      str += 'x';
      extra += 'x';
   }
   else
   {
      str += '-';
      extra += '-';
   }

   str += extra + extra;
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
         throw runtime_error( "cannot create a link in same directory with the same name '" + name + "'" );
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
      source = get_cwd( ) + "/" + source;

   if( symlink( source.c_str( ), name.c_str( ) ) != 0 )
      throw runtime_error( "failed to create link '"
       + name + "' to '" + source + "' (errno = " + to_string( errno ) + ")" );
#endif
}

void file_copy( const char* p_src, const char* p_dest, bool append )
{
   ios::openmode iflags = ( ios::in | ios::binary );
   ios::openmode oflags = ( !append ? ios::out | ios::binary : ios::app | ios::out | ios::binary );

   ifstream inpf( p_src, iflags );
   ofstream outf( p_dest, oflags );

   copy_stream( inpf, outf );
}

string valid_file_name( const string& str, bool* p_has_utf8_chars, bool allow_path_separators )
{
   string s;

   if( p_has_utf8_chars )
      *p_has_utf8_chars = false;

   size_t num_extra = 0;

   for( size_t i = 0; i < str.length( ); i++ )
   {
      char c = str[ i ];
      int ic = ( int )c;

      if( num_extra )
      {
         s += c;
         --num_extra;

         continue;
      }

      if( ic < 0 )
      {
         ic += 256;

         if( p_has_utf8_chars )
            *p_has_utf8_chars = true;

         size_t extra = 0;

         if( ic >= 192 && ic <= 223 )
            extra = 1;
         else if( ic >= 224 && ic <= 239 )
            extra = 2;
         else if( ic >= 240 && ic <= 247 )
            extra = 3;
         else if( ic >= 248 && ic <= 251 )
            extra = 4;
         else if( ic >= 252 && ic <= 253 )
            extra = 5;

         if( !extra )
            throw runtime_error( "unexpected UTF-8 encoding found in: " + str );

         // NOTE: If the argument was not provided then will omit UTF-8 characters.
         if( !p_has_utf8_chars )
            i += extra;
         else
         {
            s += c;

            num_extra = extra;
            *p_has_utf8_chars = true;
         }

         continue;
      }

      if( c == '"' )
         s += "'";

      if( c == '/' )
      {
         if( allow_path_separators )
            s += c;
      }
      else  if( c != '"' && c != ':' && c != '?'
       && c!= '*' && c != '<' && c != '>' && c != '|' && c != '\\' )
         s += c;
   }

   return s;
}

// NOTE: This Boyer-Moore implementation was taken from that published by Eduard Igushev.
boyer_moore::boyer_moore( const string& pattern )
 :
 pattern( pattern ),
 slide( c_char_size ),
 jump( pattern.size( ) )
{
   for( int i = 0; i < c_char_size; ++i )
      slide[ i ] = pattern.length( );

   int psize = ( int )( pattern.size( ) );

   for( int i = 0; i < psize; ++i )
      slide[ pattern[ i ] ] = psize - i - 1;

   for( int i = 0; i < psize; ++i )
      jump[ i ] = 2 * psize - i - 1;

   vector< int > matches( pattern.size( ) );

   int curr = psize - 1;
   int match = curr + 1;

   while( curr >= 0 )
   {
      while( match < psize && pattern[ curr ] != pattern[ match ] )
      {
         jump[ match ] = min( jump[ match ], psize - curr - 1 );
         match = matches[ match ];
      }

      matches[ curr ] = match;
      --curr;
      --match;
   }

   int curr_match = matches[ 0 ];

   for( int i = 0; i < psize; ++i )
   {
      jump[ i ] = min( jump[ i ], psize + curr_match - i - 1 );

      if( curr_match == i )
         curr_match = matches[ curr_match ];
   }
}

string::size_type boyer_moore::find( const string& text )
{
   if( text.empty( ) || pattern.empty( ) || text.size( ) < pattern.size( ) )
      return string::npos;
 
   int text_i = pattern.size( ) - 1;

   while( text_i < ( int )( text.size( ) ) )
   {
      int pattern_i = ( int )( pattern.size( ) - 1 );

      while( pattern_i >= 0 && text[ text_i ] == pattern[ pattern_i ] )
      {
         --text_i;
         --pattern_i;
      }
 
      if( pattern_i < 0 )
         return text_i + 1;
 
      text_i += max( slide[ pattern[ pattern_i ] ], jump[ pattern_i ] );
   }

   return string::npos;
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

string& replace( string& s, const char* p_findstr, const char* p_replstr )
{
   string str;
   size_t from = 0;

   if( !p_findstr || !p_replstr )
      throw runtime_error( "unexpected null string(s) in replace" );

   if( *p_findstr )
   {
      while( true )
      {
         size_t pos = s.find( p_findstr, from );

         if( pos == string::npos )
            break;

         str = s.substr( 0, pos );

         str += string( p_replstr );

         from = pos + strlen( p_replstr );

         pos += strlen( p_findstr );
         str += s.substr( pos );

         s = str;
      }
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

string escaped_shell_arg( const string& arg )
{
   string s( arg );

   if( !s.empty( ) && s != "\"\"" )
   {
      replace( s, "\"", "\\\"" );
      s = '"' + s + '"';
   }

   return s;
}

string escaped_shell_cmd( const string& cmd )
{
   string s( cmd );

   if( !s.empty( ) )
   {
#ifdef _WIN32
      replace( s, "^", "^^" );
#else
      replace( s, "\\", "\\\\" );
#endif

      size_t nsq = count( s.begin( ), s.end( ), '\'' );
      size_t ndq = count( s.begin( ), s.end( ), '\"' );

      string::size_type nsq_pos = string::npos;
      string::size_type ndq_pos = string::npos;

      if( nsq % 2 == 1 )
         nsq_pos = s.rfind( '\'' );

      if( ndq % 2 == 1 )
         ndq_pos = s.rfind( '\"' );

      if( nsq_pos != string::npos )
         s.insert( nsq_pos, "\\" );

      if( ndq_pos != string::npos )
      {
         if( nsq_pos != string::npos && nsq_pos < ndq_pos )
            ++ndq_pos;

         s.insert( ndq_pos, "\\" );
      }

#ifdef _WIN32
      replace( s, ",", "^,", "%", "^%" );
      replace( s, "&", "^&", "|", "^|" );
      replace( s, "<", "^<", ">", "^>" );
      replace( s, "!", "^^!", ".", "^." );
#else
      replace( s, "&", "\\&", "|", "\\|" );
      replace( s, "<", "\\<", ">", "\\>" );
      replace( s, "`", "\\`", "%", "\\%" );
      replace( s, "*", "\\*", "?", "\\?" );
      replace( s, "#", "\\#", "~", "\\~" );
      replace( s, "[", "\\[", "]", "\\]" );
      replace( s, "(", "\\(", ")", "\\)" );
      replace( s, ",", "\\,", ";", "\\;" );
      replace( s, "0x0a", "\\0x0a", "0xff", "\\0xff" );
#endif
   }

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

bool is_valid_utf8( const string& utf8 )
{
   size_t pos = 0;

   while( true )
   {
      int c = utf8[ pos ];

      if( c < 0 )
         c += 256;

      if( c < 127 )
         ++pos;
      else if( c >= 192 && c <= 223 )
         pos += 2;
      else if( c >= 224 && c <= 239 )
         pos += 3;
      else if( c >= 240 && c <= 247 )
         pos += 4;
      else if( c >= 248 && c <= 251 )
         pos += 5;
      else if( c >= 252 && c <= 253 )
         pos += 6;
      else
         return false;

      if( pos >= utf8.size( ) )
         break;
   }

   return true;
}

string& utf8_replace( string& utf8, const char* p_findstr, const char* p_replstr )
{
   if( !utf8.empty( ) )
   {
      size_t pos = 0;
      set< size_t > start_char_pos;

      while( true )
      {
         int c = utf8[ pos ];

         start_char_pos.insert( pos );

         if( c < 0 )
            c += 256;

         if( c < 127 )
            ++pos;
         else if( c >= 192 && c <= 223 )
            pos += 2;
         else if( c >= 224 && c <= 239 )
            pos += 3;
         else if( c >= 240 && c <= 247 )
            pos += 4;
         else if( c >= 248 && c <= 251 )
            pos += 5;
         else if( c >= 252 && c <= 253 )
            pos += 6;
         else
            throw runtime_error( "unexpected UTF-8 encoding found in: " + utf8 );

         if( pos >= utf8.length( ) )
            break;
      }

      string::size_type lpos = 0;

      while( true )
      {
         string::size_type pos = utf8.find( p_findstr, lpos );
         if( pos == string::npos )
            break;

         if( start_char_pos.count( pos ) )
         {
            utf8.replace( pos, strlen( p_findstr ), p_replstr );
            lpos = pos + strlen( p_replstr );
         }
         else
            lpos = pos + strlen( p_findstr );
      }
   }

   return utf8;
}

string& utf8_truncate( string& utf8, int trunc_limit, const char* p_overflow_suffix )
{
   if( !utf8.empty( ) )
   {
      // NOTE: Truncation will occur on a space if possible and if any
      // CR/LFs are found then truncates at the first such occurrence.
      int num_chars = 0;
      size_t pos = 0, tpos = 0;

      while( true )
      {
         int c = utf8[ pos ];

         if( c < 0 )
            c += 256;

         if( c < 127 )
         {
            ++pos;
            ++num_chars;

            if( utf8[ pos ] == ' ' )
               tpos = pos;
            else if( utf8[ pos ] == '\r' || utf8[ pos ] == '\n' )
            {
               tpos = pos;
               break;
            }
         }
         else if( c >= 192 && c <= 223 )
         {
            pos += 2;
            ++num_chars;
         }
         else if( c >= 224 && c <= 239 )
         {
            pos += 3;
            ++num_chars;
         }
         else if( c >= 240 && c <= 247 )
         {
            pos += 4;
            ++num_chars;
         }
         else if( c >= 248 && c <= 251 )
         {
            pos += 5;
            ++num_chars;
         }
         else if( c >= 252 && c <= 253 )
         {
            pos += 6;
            ++num_chars;
         }
         else
            throw runtime_error( "unexpected UTF-8 encoding found in: " + utf8 );

         // NOTE: If total number of characters found is less than the number
         // after which truncation should occur then clear the truncation pos.
         if( pos >= utf8.size( ) && num_chars <= trunc_limit )
            tpos = 0;

         if( pos >= utf8.size( ) || num_chars > trunc_limit )
            break;
      }

      if( tpos == 0 )
         tpos = pos;

      if( tpos >= utf8.size( ) )
         tpos = 0;

      if( tpos && tpos < utf8.size( ) )
      {
         utf8.erase( tpos );
         if( p_overflow_suffix )
            utf8 += p_overflow_suffix;
      }
   }

   return utf8;
}

bool has_environment_variable( const char* p_env_var_name )
{
   const char* p_env_var = getenv( p_env_var_name );

   return ( p_env_var && *p_env_var );
}

string get_environment_variable( const char* p_env_var_name )
{
   string value;

   const char* p_value = getenv( p_env_var_name );

   if( p_value && *p_value )
      value = string( p_value );

   return value;
}

void set_environment_variable( const char* p_env_var_name, const char* p_new_value )
{
#ifndef _WIN32
   setenv( p_env_var_name, p_new_value, 1 );
#else
   _putenv( ( char* )( string( p_env_var_name ) + "=" + string( p_new_value ) ).c_str( ) );
#endif
}

void replace_environment_variables( string& s, char c, bool as_quotes, const char* p_specials, char esc )
{
   string::size_type pos = s.find( c );

   vector< string > env_vars_to_overwrite;

   while( pos != string::npos )
   {
      if( s.size( ) > pos + 1
       && ( ( s[ pos + 1 ] == c ) || ( s[ pos + 1 ] == '*' )
       || ( s[ pos + 1 ] >= '0' && s[ pos + 1 ] <= '9' ) ) )
      {
         pos = s.find( c, pos + 2 );
         continue;
      }

      string::size_type npos = string::npos;

      int skip = 1;

      if( as_quotes )
         npos = s.find( c, pos + skip );
      else
      {
         if( s[ pos + skip ] == '~' )
            ++skip;

         npos = s.find_first_not_of( c_env_var_characters, pos + skip );
      }

      if( npos == string::npos )
      {
         if( as_quotes )
            break;
         else
            npos = s.length( );
      }

      if( npos != pos + skip )
      {
         string env_var_name( s.substr( pos + 1, npos - pos - 1 ) );

         if( env_var_name[ 0 ] == '~' )
         {
            env_var_name.erase( 0, 1 );
            env_vars_to_overwrite.push_back( env_var_name );
         }

         int line = 0;
         string prefix, suffix;

         if( as_quotes )
         {
            // NOTE: If using quote like markers then allow the following:
            // %<var_name>:<line>% so that a single line in a "multi-line"
            // environment variable can be extracted (a negative value can
            // be used to assign all but the one line) and it is supported
            // to use >PREFIX<SUFFIX after <line> where an optional PREFIX
            // and SUFFIX will limit the assignment to only the text which
            // appears after the PREFIX or before the SUFFIX (assuming the
            // PREFIX or SUFFIX is found).
            string::size_type lpos = env_var_name.find( ':' );
            if( lpos != string::npos )
            {
               string::size_type epos = env_var_name.rfind( '<' );

               if( epos != string::npos && epos > lpos )
               {
                  suffix = env_var_name.substr( epos + 1 );
                  env_var_name.erase( epos );
               }

               epos = env_var_name.find( '>', lpos );
               if( epos != string::npos )
               {
                  prefix = env_var_name.substr( epos + 1 );
                  env_var_name.erase( epos );
               }

               line = atoi( env_var_name.substr( lpos + 1 ).c_str( ) );
               env_var_name.erase( lpos );
            }
         }

         string env_var_value;
         char* p_env_var = getenv( env_var_name.c_str( ) );

         if( p_env_var )
         {
            string tmp_str( p_env_var );

            if( line == 0 )
               env_var_value.swap( tmp_str );
            else
            {
               vector< string > lines;
               split( p_env_var, lines, '\n' );

               if( line > 0 && line <= lines.size( ) )
                  env_var_value = lines[ line - 1 ];
               else if( line < 0 )
               {
                  line *= -1;

                  for( size_t i = 0; i < lines.size( ); i++ )
                  {
                     if( !env_var_value.empty( ) )
                        env_var_value += '\n';

                     if( line != i + 1 )
                        env_var_value += lines[ i ];
                  }
               }
            }

            // NOTE: For the optional PREFIX the text from the start
            // of the value up until the end of the first occurrence
            // of the PREFIX will be removed.
            if( !prefix.empty( ) )
            {
               string::size_type ppos = env_var_value.find( prefix );

               if( ppos != string::npos )
                  env_var_value.erase( 0, ppos + prefix.length( ) );
            }

            // NOTE: For the optional SUFFIX the text from the start
            // of the last occurrence of the SUFFIX up until the end
            // of the value will be removed.
            if( !suffix.empty( ) )
            {
               string::size_type spos = env_var_value.rfind( suffix );

               if( spos != string::npos )
                  env_var_value.erase( spos );
            }
         }

         if( esc )
            escape( env_var_value, p_specials, esc );

         s.erase( pos, npos - pos + as_quotes );
         s.insert( pos, env_var_value );

         npos = pos + env_var_value.size( );

         clear_key( env_var_value );
      }
      else
         s.erase( pos, 1 );

      pos = s.find( c, npos );
   }

   // NOTE: If an environment variable is prefixed with '~' then
   // its value will be overwritten with '*' characters (being a
   // way to prevent password discovery via an environment dump).
   for( size_t i = 0; i < env_vars_to_overwrite.size( ); i++ )
   {
      string next( env_vars_to_overwrite[ i ] );
      char* p_env_var = getenv( next.c_str( ) );

      if( p_env_var )
         memset( p_env_var, '*', strlen( p_env_var ) );
   }
}

string trim( const string& s, bool leading_only, bool trailing_only )
{
   string t;

   if( s.length( ) )
   {
      string ws( c_whitespace_chars );

      string::size_type start = 0;
      string::size_type finish = s.length( );

      if( !trailing_only )
      {
         for( string::size_type i = 0; i < s.length( ); i++ )
         {
            if( ws.find( s[ i ] ) == string::npos )
               break;
            ++start;
         }
      }

      if( start < finish )
      {
         if( !leading_only )
         {
            for( string::size_type i = s.length( ) - 1; ; i-- )
            {
               if( ws.find( s[ i ] ) == string::npos )
                  break;
               --finish;

               if( i == 0 )
                  break;
            }
         }

         t = s.substr( start, finish - start );
      }
   }

   return t;
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

string get_version_info( const string& s, version_info& ver_info )
{
   string rs;
   string str( s );

   string::size_type pos = str.find( '\n' );
   if( pos != string::npos )
   {
      rs = str.substr( pos + 1 );
      str.erase( pos );
   }

   pos = str.find( ':' );
   if( pos != string::npos )
   {
      ver_info.extra = str.substr( pos + 1 );
      str.erase( pos );
   }

   ver_info.ver = str;

   ver_info.major = 0;
   ver_info.minor = 0;

   pos = str.find( '.' );
   if( pos != string::npos )
      ver_info.minor = atoi( str.substr( pos + 1 ).c_str( ) );

   ver_info.major = atoi( str.substr( 0, pos ).c_str( ) );

   return rs;
}

bool check_version_info( const version_info& ver_info, int major_version, int minor_version, bool* p_old )
{
   bool okay = true;

   if( p_old )
      *p_old = false;

   if( major_version != ver_info.major )
      okay = false;
   else if( minor_version > ver_info.minor )
      okay = false;
   else if( minor_version < ver_info.minor )
   {
      if( p_old )
         *p_old = true;
   }

   return okay;
}

void parse_host_and_or_port( const string& host_and_or_port, string& host, int& port )
{
   if( !host_and_or_port.empty( ) )
   {
      if( host_and_or_port.find( '.' ) == string::npos
       && host_and_or_port.find( ':' ) == string::npos
       && host_and_or_port[ 0 ] >= '0' && host_and_or_port[ 0 ] <= '9' )
         port = atoi( host_and_or_port.c_str( ) );
      else
      {
         host = host_and_or_port;
         string::size_type pos = host.find( ':' );

         // NOTE: If host is an IPV6 address then use '-' as the port separator.
         if( pos == string::npos || host.find( ':', pos + 1 ) != string::npos )
            pos = host.find( '-' );

         if( pos != string::npos )
         {
            port = atoi( host.substr( pos + 1 ).c_str( ) );
            host.erase( pos );
         }
      }
   }
}

size_t setup_arguments( const char* p_input, vector< string >& arguments, char esc, const char* p_specials )
{
   arguments.clear( );

   assert( esc != '\'' && esc != '"' && esc != '\0' && !isspace( static_cast< unsigned char >( esc ) ) );

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
            throw runtime_error( "missing expected end-quote in: " + string( p_input ) );

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

void setup_arguments( int argc, const char* argv[ ],
 vector< string >& arguments, char esc, const char* p_specials )
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

void buffer_file( string& buffer, const char* p_file_name, size_t max_bytes, size_t* p_size, size_t start_pos )
{
   if( !p_file_name )
      throw runtime_error( "unexpected null pointer for p_file_name in buffer_file" );

#ifndef _MSC_VER
   FILE* fp = fopen( p_file_name, "rb" );
#else
   FILE* fp = fopen( p_file_name, "rb, ccs=UTF-8" );
#endif
   if( !fp )
      throw runtime_error( "unable to open file '" + string( p_file_name ) + "' for input in buffer_file" );

   fseek( fp, 0, SEEK_END );
   size_t size = ftell( fp );

   if( p_size )
      *p_size = size;

   if( start_pos > size )
      start_pos = size;

   if( max_bytes && ( size - start_pos > max_bytes ) )
      size = max_bytes;
   else
      size -= start_pos;

   if( size > 0 )
   {
      if( buffer.capacity( ) < size )
         buffer.reserve( size );

      buffer.resize( size );

      fseek( fp, start_pos, SEEK_SET );

      if( fread( &buffer[ 0 ], 1, ( size_t )size, fp ) != ( size_t )size )
         throw runtime_error( "reading from input file '" + string( p_file_name ) + "'" );
   }
   else
      buffer.resize( 0 );

   fclose( fp );
}

void write_file( const char* p_file_name, unsigned char* p_data, size_t length, bool append, size_t start_pos )
{
   if( !p_file_name )
      throw runtime_error( "unexpected null pointer for p_file_name in write_file" );

   FILE* fp = fopen( p_file_name, ( append ? "ab" : "wb" ) );
   if( !fp )
      throw runtime_error( "unable to open file '" + string( p_file_name ) + "' for output in write_file" );

   if( !append && start_pos )
      fseek( fp, start_pos, SEEK_SET );

   if( length && fwrite( p_data, 1, length, fp ) != length )
      throw runtime_error( "writing to output file '" + string( p_file_name ) + "'" );

   fclose( fp );
}

void write_file_lines( const string& file_name, const vector< string >& lines )
{
   ofstream outf( file_name.c_str( ) );

   for( size_t i = 0; i < lines.size( ); i++ )
      outf << lines[ i ] << '\n';

   outf.flush( );
}

string buffer_file_lines( const string& file_name, bool skip_blank_lines, bool strip_extra_crs )
{
   string str;
   vector< string > lines;

   buffer_file_lines( file_name, lines, skip_blank_lines, strip_extra_crs );

   for( size_t i = 0; i < lines.size( ); i++ )
   {
      if( i > 0 )
         str += '\n';
      str += lines[ i ];
   }

   return str;
}

void buffer_file_lines( const string& file_name, set< string >& lines, bool strip_extra_crs )
{
   ifstream inpf( file_name.c_str( ) );
   if( !inpf )
      throw runtime_error( "unable to open file '" + file_name + "' for input in buffer_file_lines" );

   string next;
   size_t line_num = 0;

   while( getline( inpf, next ) )
   {
      ++line_num;

      if( strip_extra_crs )
         remove_trailing_cr_from_text_file_line( next, line_num == 1 );

      if( !next.empty( ) )
         lines.insert( next );
   }

   if( !inpf.eof( ) )
      throw runtime_error(
       "unexpected error occurred whilst reading '" + file_name + "' for input in buffer_file_lines" );
}

void buffer_file_lines( const string& file_name,
 deque< string >& lines, bool skip_blank_lines, bool strip_extra_crs )
{
   ifstream inpf( file_name.c_str( ) );
   if( !inpf )
      throw runtime_error( "unable to open file '" + file_name + "' for input in buffer_file_lines" );

   string next;
   size_t line_num = 0;

   while( getline( inpf, next ) )
   {
      ++line_num;

      if( strip_extra_crs )
         remove_trailing_cr_from_text_file_line( next, line_num == 1 );

      if( !skip_blank_lines || !next.empty( ) )
         lines.push_back( next );
   }

   if( !inpf.eof( ) )
      throw runtime_error(
       "unexpected error occurred whilst reading '" + file_name + "' for input in buffer_file_lines" );
}

void buffer_file_lines( const string& file_name,
 vector< string >& lines, bool skip_blank_lines, bool strip_extra_crs )
{
   ifstream inpf( file_name.c_str( ) );
   if( !inpf )
      throw runtime_error( "unable to open file '" + file_name + "' for input in buffer_file_lines" );

   string next;
   size_t line_num = 0;

   while( getline( inpf, next ) )
   {
      ++line_num;

      if( strip_extra_crs )
         remove_trailing_cr_from_text_file_line( next, line_num == 1 );

      if( !skip_blank_lines || !next.empty( ) )
         lines.push_back( next );
   }

   if( !inpf.eof( ) )
      throw runtime_error(
       "unexpected error occurred whilst reading '" + file_name + "' for input in buffer_file_lines" );
}

void buffer_file_tail( const string& file_name,
 deque< string >& lines, unsigned int num_lines, bool skip_blank_lines, bool strip_extra_crs )
{
   uint64_t total_size = file_size( file_name );

   long start_pos = 0;

   if( total_size > c_tail_buffer_size )
      start_pos = total_size - c_tail_buffer_size;

   string data;
   bool finished_buffering = false;

   if( !num_lines )
      num_lines = c_tail_maximum_lines;

   long max_bytes = c_tail_buffer_size;

   while( lines.size( ) < num_lines )
   {
      if( !finished_buffering )
         data = buffer_file( file_name, max_bytes, 0, start_pos ) + data;

      if( start_pos == 0 )
         finished_buffering = true;

      if( lines.empty( ) && !data.empty( ) )
      {
         // NOTE: Will always ignore a final trailing blank line.
         while( data.size( ) && data[ data.size( ) - 1 ] == '\n' )
         {
            data.erase( data.size( ) - 1 );

            if( !skip_blank_lines )
               break;
         }
      }

      if( start_pos < c_tail_buffer_size )
      {
         max_bytes = start_pos;
         start_pos = 0;
      }
      else if( start_pos )
         start_pos -= c_tail_buffer_size;

      string next( data );

      string::size_type pos = data.rfind( '\n' );

      if( pos == string::npos )
         data.erase( );
      else
      {
         next = data.substr( pos + 1 );
         data.erase( pos );
      }

      if( strip_extra_crs )
         remove_trailing_cr_from_text_file_line( next, start_pos == 0 );

      if( !skip_blank_lines || !next.empty( ) )
         lines.push_front( next );

      if( pos == string::npos && start_pos == 0 )
         break;
   }
}

void buffer_file_items( const string& file_name,
 map< string, string >& items, char separator, bool strip_extra_crs )
{
   ifstream inpf( file_name.c_str( ) );
   if( !inpf )
      throw runtime_error( "unable to open file '" + file_name + "' for input in buffer_file_items" );

   string next;
   size_t line_num = 0;

   while( getline( inpf, next ) )
   {
      ++line_num;

      if( strip_extra_crs )
         remove_trailing_cr_from_text_file_line( next, line_num == 1 );

      if( !next.empty( ) )
      {
         string::size_type pos = next.find( separator );
         if( pos == string::npos )
            throw runtime_error( "unexpected missing separator '"
             + to_string( separator ) + "' in '" + file_name + "' at line #" + to_string( line_num ) );

         items.insert( make_pair( next.substr( 0, pos ), next.substr( pos + 1 ) ) );
      }
   }

   if( !inpf.eof( ) )
      throw runtime_error(
       "unexpected error occurred whilst reading '" + file_name + "' for input in buffer_file_items" );
}

void absolute_path( const string& relative_path, string& absolute_path, bool* p_rc )
{
   bool found = false;

#ifdef _WIN32
   char path[ _MAX_PATH ] = { 0 };
   found = ( _fullpath( path, relative_path.c_str( ), sizeof( path ) ) != 0 );
   absolute_path = path;
#else
   char path[ PATH_MAX ] = { 0 };
   found = realpath( relative_path.c_str( ), &path[ 0 ] );
   absolute_path = path;
#endif

   if( !found && !p_rc )
      throw runtime_error( "unable to determine absolute path for '" + relative_path + "'" );

   if( p_rc )
      *p_rc = found;
}

string file_name_without_path( const string& path, bool remove_extension )
{
   string file_name( path );

   string::size_type pos = path.find_last_of( ":/\\" );

   if( pos != string::npos )
      file_name.erase( 0, pos + 1 );

   if( remove_extension )
   {
      pos = file_name.find( '.' );

      if( pos != string::npos )
         file_name.erase( pos );
   }

   return file_name;
}

time_t last_modification_time( const string& file_name, bool try_avoiding_epoch_time )
{
   struct stat statbuf;

   if( stat( file_name.c_str( ), &statbuf ) != 0 )
      throw runtime_error( "cannot stat '" + file_name + "' (errno = " + to_string( errno ) + ")" );

   time_t retval = statbuf.st_mtime;

   // KLUDGE: It has been found in certain VM situations that mtime is zero
   // (i.e.the unix epoch) so assuming that the caller would prefer to have
   // a non-zero value (if available) will instead use "ctime" and if still
   // going to return zero will finally try using "atime".
   if( !retval && try_avoiding_epoch_time )
   {
      retval = statbuf.st_ctime;

      if( !retval && try_avoiding_epoch_time )
         retval = statbuf.st_atime;
   }

   return retval;
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

   // NOTE: Decode each of the triplets.
   for( int i = 0; i < triplets; i++, p_input++ )
   {
      int j = 2;
      for( composite = *( p_input + 3 ) - c_space; j < 7; j += 2, p_input++ )
          outs.put( ( *p_input - c_space ) | ( ( composite & ( c_high_mask >> j ) ) << j ) );
   }

   // NOTE: Decode any dangling bytes (0, 1, or 2 bytes which did not form a complete triplet).
   if( remainder > 1 )
   {
      int j = 2;
      remainder--;
      for( composite = *( p_input + remainder ) - c_space; j <  ( 2 * remainder + 1 ); j += 2, p_input++ )
        outs.put( ( *p_input - c_space ) | ( ( composite & ( c_high_mask >> j ) ) << j ) );
   }
}

void uuencode( const char* p_data, int num_bytes, ostream& outs )
{
   int triplets, remainder;
   unsigned char composite;

   triplets = num_bytes / 3;
   remainder = num_bytes % 3;

   // NOTE: Encode each of the triplets.
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

   // NOTE: Encode any dangling bytes (0, 1, or 2 bytes which do not form a complete triplet).
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

void hex_decode( const string& data, unsigned char* p_data, size_t len )
{
   if( data.length( ) % 2 != 0 )
      throw runtime_error( "incorrect hex for decoding (add a leading zero?)" );

   if( len < data.length( ) / 2 )
      throw runtime_error( "data buffer is " + to_string( len )
       + " bytes but hex_decode requires at least " + to_string( data.length( ) / 2 ) );

   memset( p_data, len, 0 );
   
   size_t n = 0;

   for( size_t i = 0; i < data.size( ); i += 2 )
   {
      unsigned char val = hex_nibble( data[ i ] );
      val <<= 4;

      val |= hex_nibble( data[ i + 1 ] );

      p_data[ n++ ] = val;
   }
}

void hex_encode( string& data, const unsigned char* p_data, size_t len, int max_chars_per_line )
{
   size_t n = 0;
   size_t line_chars = 0;

   size_t required = ( len * 2 );

   if( max_chars_per_line )
      required += ( len / max_chars_per_line );

   // NOTE: If had not been pre-allocated then must now be resized.
   if( data.length( ) < required )
      data.resize( required );

   for( size_t i = 0; i < len; i++ )
   {
      data[ n++ ] = ascii_digit( ( p_data[ i ] & 0xf0 ) >> 4 );
      data[ n++ ] = ascii_digit( p_data[ i ] & 0x0f );

      line_chars += 2;

      if( max_chars_per_line
       && ( line_chars >= max_chars_per_line ) && ( i != len - 1 ) )
      {
         line_chars = 0;
         data[ n++ ] = '\n';
      }
   }
}

void hex_reverse( string& reversed, const string& hex_value )
{
   size_t n = 0;

   // NOTE: If had not been pre-allocated then must now be resized.
   if( reversed.length( ) < hex_value.length( ) )
      reversed.resize( hex_value.length( ) );

   for( int i = hex_value.length( ) - 1; i >= 0; i -= 2 )
   {
      if( i - 1 >= 0 )
      {
         reversed[ n++ ] = hex_value[ i - 1 ];
         reversed[ n++ ] = hex_value[ i ];
      }
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

void read_strings( const string& file_name, map< string, string >& strings,
 const char* p_prefix, bool unescape_data, char esc, const char* p_specials )
{
   ifstream inpf( file_name.c_str( ) );
   if( !inpf )
      throw runtime_error( "unable to open file '" + file_name + "' for input in read_strings" );

   string next;
   bool is_first = true;
   while( getline( inpf, next ) )
   {
      remove_trailing_cr_from_text_file_line( next, is_first );

      if( is_first )
         is_first = false;

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
         throw runtime_error( "duplicate string key '" + key + "' found reading strings in read_strings" );

      strings.insert( string_value_type( key, data ) );
   }

   if( !inpf.eof( ) )
      throw runtime_error(
       "unexpected error occurred whilst reading '" + file_name + "' for input in read_strings" );
}

void write_strings( const string& file_name,
 const string_container& strings, bool escape_data, char esc, const char* p_specials )
{
   ofstream outf( file_name.c_str( ) );
   if( !outf )
      throw runtime_error( "unable to open file '" + file_name + "' for output in write_strings" );

   for( string_const_iterator i = strings.begin( ); i != strings.end( ); ++i )
   {
      string data( i->second );
      if( escape_data )
         escape( data, 0, esc, p_specials );

      outf << i->first << ' ' << '"' << data << '"' << '\n';
   }

   outf.flush( );

   if( !outf.good( ) )
      throw runtime_error( "output stream is bad for '" + file_name + "' in write_strings" );
}

string extract_text_from_html( const string& html )
{
   string text;

   string token;
   size_t tok_pos = 0;
   bool in_token = false;
   bool had_first_non_white = false;

   for( size_t i = 0; i < html.size( ); i++ )
   {
      if( html[ i ] == '<' )
      {
         tok_pos = i;
         in_token = true;
      }
      else if( html[ i ] == '>' )
      {
         in_token = false;
         token = lower( html.substr( tok_pos, i - tok_pos + 1 ) );

         // NOTE: Ignore text that is within an "empty span".
         if( token == "<span>" )
            in_token = true;

         if( token == "<br>" || token == "<br/>" )
            text += '\n';
         else if( token == "</p>" || token == "</title>" )
            text += "\n\n";
         else if( token == "</h1>" || token == "</h2>" || token == "</h3>" )
            text += '\n';
      }
      else if( !in_token )
      {
         if( html[ i ] == '&' )
         {
            had_first_non_white = true;
            string::size_type pos = html.find( ';', i + 1 );
            if( pos != string::npos )
            {
               string special = lower( html.substr( i + 1, pos - i - 1 ) );
               if( !special.empty( ) )
               {
                  if( special[ 0 ] == '#' )
                  {
                     char ch = 0;
                     for( size_t j = 1; j < special.size( ); j++ )
                     {
                        ch = ch * 10;
                        ch += special[ j ] - '0';
                     }

                     text += ch;
                  }
                  else if( special == "gt" )
                     text += '>';
                  else if( special == "lt" )
                     text += '<';
                  else if( special == "amp" )
                     text += '&';
                  else if( special == "apos" )
                     text += "'";
                  else if( special == "circ" )
                     text += '^';
                  else if( special == "nbsp" )
                     text += ' ';
                  else if( special == "tilde" )
                     text += '~';
                  else if( special == "quote" )
                     text += '"';
                  else
                     text += '?' + special + '?';
               }

               i = pos;
               continue;
            }
         }

         if( html[ i ] != '\r' && html[ i ] != '\n' )
         {
            if( had_first_non_white || ( html[ i ] != ' ' && html[ i ] != '\t' ) )
            {
               text += html[ i ];
               if( html[ i ] != ' ' && html[ i ] != '\t' )
                  had_first_non_white = true;
            }
         }
         else
            had_first_non_white = false;
      }
   }

   return trim( text );
}
