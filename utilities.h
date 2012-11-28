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

#ifndef UTILITIES_H
#  define UTILITIES_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <cmath>
#     include <map>
#     include <set>
#     include <deque>
#     include <vector>
#     include <string>
#     include <limits>
#     include <sstream>
#     include <iostream>
#     include <streambuf>
#     include <algorithm>
#     include <sys/types.h>
#  endif

#  include "ptypes.h"

const char c_sep = ',';
const char c_esc = '\\';
const char c_nul = '\0';

const char* const c_whitespace_chars = "\f\t\n\v\r";

const char* const c_special_characters = "abftnvr\a\b\f\t\n\v\r";

const int c_uuid_size = 16;

const int c_int_digits10 = std::numeric_limits< int >::digits10;

template< int I, int B = 10 > struct power
{
   static const int result = B * power< I - 1, B >::result;
};

template< int B > struct power< 0, B >
{
   static const int result = 1;
};

// This "typeof" implementation was written by Bill Gibbons.
template< int N > struct typeof_class;
template< class T > struct wrap_type { typedef T U; };

#define REGISTER_TYPEOF( N, T ) \
 template< > struct typeof_class< N > { typedef wrap_type< T >::U V; }; \
 char ( *typeof_fct( const wrap_type< T >::U & ) )[ N ];

#define typeof( x ) typeof_class< sizeof( *typeof_fct( x ) ) >::V

REGISTER_TYPEOF( 1, bool )
REGISTER_TYPEOF( 2, char )
REGISTER_TYPEOF( 3, signed char )
REGISTER_TYPEOF( 4, unsigned char )
REGISTER_TYPEOF( 5, short )
REGISTER_TYPEOF( 6, unsigned short )
REGISTER_TYPEOF( 7, int )
REGISTER_TYPEOF( 8, unsigned int )
REGISTER_TYPEOF( 9, long )
REGISTER_TYPEOF( 10, unsigned long )
#  ifndef _LP64
REGISTER_TYPEOF( 11, int64_t )
REGISTER_TYPEOF( 12, uint64_t )
#  endif
REGISTER_TYPEOF( 13, float )
REGISTER_TYPEOF( 14, double )
REGISTER_TYPEOF( 15, wchar_t )
REGISTER_TYPEOF( 16, int ( * )( ) )
REGISTER_TYPEOF( 17, std::string )

std::string quote( const std::string& s, char quote_char = '"', char escape_char = '\\' );

inline void to_upper( std::string& s )
 { std::transform( s.begin( ), s.end( ), s.begin( ), ( int( * )( int ) )toupper ); }

inline void to_lower( std::string& s )
 { std::transform( s.begin( ), s.end( ), s.begin( ), ( int( * )( int ) )tolower ); }

#  ifdef NEW_BORLAND_VERSION
#     pragma option push -w-8027
#  endif
inline std::string upper( const std::string& s )
{
   std::string ss( s );
   to_upper( ss );
   return ss;
}

inline std::string lower( const std::string& s )
{
   std::string ss( s );
   to_lower( ss );
   return ss;
}
#  ifdef NEW_BORLAND_VERSION
#     pragma option pop
#  endif

inline double round( double x, int n )
{
   double p = std::pow( 10.0, n );
   return std::floor( 0.5 + p * x ) / p;
}

inline unsigned char hex_nibble( unsigned char ch )
{
   unsigned char retval = 0;

   if( ch >= '0' && ch <= '9' )
      retval = ch - '0';
   else
   {
      if( ch == 'a' || ch == 'A' )
         retval = 0x0a;
      else if( ch == 'b' || ch == 'B' )
         retval = 0x0b;
      else if( ch == 'c' || ch == 'C' )
         retval = 0x0c;
      else if( ch == 'd' || ch == 'D' )
         retval = 0x0d;
      else if( ch == 'e' || ch == 'E' )
         retval = 0x0e;
      else if( ch == 'f' || ch == 'F' )
         retval = 0x0f;
   }

   return retval;
}

inline void copy_stream( std::istream& src, std::ostream& dest ) { dest << src.rdbuf( ); }

template< class C, class T = std::char_traits< C > >
 class basic_teebuf : public std::basic_streambuf< C, T >
{
   typedef typename std::basic_streambuf< C, T > buf_type;
   typedef typename buf_type::char_type char_type;
   typedef typename buf_type::int_type int_type;
   typedef typename buf_type::traits_type traits_type;

   int_type overflow( int_type c = traits_type::eof( ) );

   buf_type* out1;
   buf_type* out2;

   public:
   basic_teebuf( buf_type* a, buf_type* b ) : out1( a ), out2( b ) { }
};

template< class C, class T >
 typename basic_teebuf< C, T >::int_type
 basic_teebuf< C, T >::overflow( typename basic_teebuf< C, T >::int_type c )
{
   int_type d = c;

   if( out1->sputc( c ) == traits_type::eof( ) )
      d = traits_type::eof( );

   if( out2->sputc( c ) == traits_type::eof( ) )
      d = traits_type::eof( );

   return d;
}

template< class C, class T = std::char_traits< C > >
 class basic_teestream : public std::basic_ostream< C, T >
{
#  ifdef __GNUG__
   basic_teebuf< C, T > buf;
#  else
   typename basic_teebuf< C, T > buf;
#  endif   

   public:
   basic_teestream(
    typename std::basic_ostream< C, T >& a,
    typename std::basic_ostream< C, T >& b )
    : a( a ), b( b ), buf( a.rdbuf( ), b.rdbuf( ) ), std::basic_ostream< C, T >( &buf )
   {
   }

   void flush( )
   {
      a.flush( );
      b.flush( );
   }

   typename std::basic_ostream< C, T >& a;
   typename std::basic_ostream< C, T >& b;
};

typedef basic_teebuf< char > tee_buf;
typedef basic_teestream< char > tee_stream;

inline unsigned char ascii_digit( unsigned char hex_nibble, bool use_upper_case = false )
{
   unsigned char retval = 0;

   if( hex_nibble <= 9 )
      retval = '0' + hex_nibble;
   else
   {
      if( hex_nibble == 0x0a )
         retval = use_upper_case ? 'A' : 'a';
      else if( hex_nibble == 0x0b )
         retval = use_upper_case ? 'B' : 'b';
      else if( hex_nibble == 0x0c )
         retval = use_upper_case ? 'C' : 'c';
      else if( hex_nibble == 0x0d )
         retval = use_upper_case ? 'D' : 'd';
      else if( hex_nibble == 0x0e )
         retval = use_upper_case ? 'E' : 'e';
      else if( hex_nibble == 0x0f )
         retval = use_upper_case ? 'F' : 'f';
   }

   return retval;
}

class uuid
{
   public:
   uuid( );
   uuid( const std::string& str );

   std::string as_string( ) const;

   private:
   unsigned char buf[ c_uuid_size ];
};

void msleep( unsigned long amt );

int get_pid( );

int vmem_used( );

bool file_exists( const char* p_name, bool check_link_target = true );
inline bool file_exists( const std::string& name, bool check_link_target = true ) { return file_exists( name.c_str( ), check_link_target ); }

bool file_remove( const char* p_name );
inline bool file_remove( const std::string& name ) { return file_remove( name.c_str( ) ); }
#  ifdef _WIN32
bool file_remove( const wchar_t* p_name );
inline bool file_remove( const std::wstring& name ) { return file_remove( name.c_str( ) ); }
#  endif

int64_t file_size( const char* p_name );
inline int64_t file_size( const std::string& name ) { return file_size( name.c_str( ) ); }

std::string file_perms( const char* p_name );
inline std::string file_perms( const std::string& name ) { return file_perms( name.c_str( ) ); }

void file_perms( const std::string& name, const std::string& rwx_perms );

void file_link( const char* p_src, const char* p_name = 0, const wchar_t* p_wsrc = 0, const wchar_t* p_wname = 0 );
inline void file_link( const std::string& src, const char* p_name = 0 ) { file_link( src.c_str( ), p_name ); }
inline void file_link( const std::string& src, const std::string& name ) { file_link( src.c_str( ), name.c_str( ) ); }
inline void file_linkw( const wchar_t* p_wsrc, const wchar_t* p_wname ) { file_link( "", "", p_wsrc, p_wname ); }

void file_copy( const char* p_src, const char* p_dest );
inline void file_copy( const std::string& src, const char* p_dest ) { file_copy( src.c_str( ), p_dest ); }
inline void file_copy( const std::string& src, const std::string& dest ) { file_copy( src.c_str( ), dest.c_str( ) ); }

void file_append( const char* p_src, const char* p_dest );
inline void file_append( const std::string& src, const char* p_dest ) { file_append( src.c_str( ), p_dest ); }
inline void file_append( const std::string& src, const std::string& dest ) { file_append( src.c_str( ), dest.c_str( ) ); }

std::string valid_file_name( const std::string& str, bool* p_has_utf8_chars = 0 );

#  ifdef NEW_BORLAND_VERSION
#     pragma option push -w-8026
#  endif
template< typename T > class restorable
{
   public:
   restorable( T& item ) : item( item ), value( item ), committed( false ) { }
   restorable( T& item, const T new_value ) : item( item ), value( item ), committed( false ) { item = new_value; }

   void commit( ) { committed = true; }

   ~restorable( )
   {
      if( !committed )
         item = value;
   }

   const T& final_value( ) { return committed ? item : value; }

   private:
   T& item;
   T value;

   bool committed;
};
#  ifdef NEW_BORLAND_VERSION
#     pragma option pop
#  endif

template< typename T > std::string signed_to_string( T val )
{
   const int buf_size = std::numeric_limits< T >::digits10 + 3;

   char buf[ buf_size ];
   buf[ buf_size - 1 ] = '\0';

   bool is_neg( val < 0 );

   if( is_neg )
      val *= -1;

   size_t pos = buf_size - 2;
   while( true )
   {
      buf[ pos ] = '0' + ( char )( val % 10 );
      val /= 10;

      if( !val || pos == 0 + is_neg )
         break;
      --pos;
   }

   if( is_neg )
      buf[ --pos ] = '-';

   return &buf[ pos ];
}

template< typename T > std::string unsigned_to_string( T val )
{
   const int buf_size = std::numeric_limits< T >::digits10 + 2;

   char buf[ buf_size ];
   buf[ buf_size - 1 ] = '\0';

   size_t pos = buf_size - 2;
   while( true )
   {
      buf[ pos ] = '0' + ( char )( val % 10 );
      val /= 10;

      if( !val || pos == 0 )
         break;
      --pos;
   }

   return &buf[ pos ];
}

template< typename T > struct string_converter
{
   std::string operator ( )( const T& t ) const;
};

#  ifdef NEW_BORLAND_VERSION
#     pragma option push -w-8027
#  endif
template< typename T > inline std::string string_converter< T >::operator ( )( const T& t ) const
{
   std::ostringstream osstr;
   osstr << t;
   return osstr.str( );
}
#  ifdef NEW_BORLAND_VERSION
#     pragma option pop
#  endif

template< > inline std::string string_converter< char >::operator ( )( const char& v ) const
{
   return std::string( 1, v );
}

template< > inline std::string string_converter< short >::operator ( )( const short& v ) const
{
   return signed_to_string< short >( v );
}

template< > inline std::string string_converter< int >::operator ( )( const int& v ) const
{
   return signed_to_string< int >( v );
}

template< > inline std::string string_converter< long >::operator ( )( const long& v ) const
{
   return signed_to_string< long >( v );
}

#  ifndef _LP64
template< > inline std::string string_converter< int64_t >::operator ( )( const int64_t& v ) const
{
   return signed_to_string< int64_t >( v );
}
#  endif

template< > inline std::string string_converter< unsigned char >::operator ( )( const unsigned char& v ) const
{
   return std::string( 1, v );
}

template< > inline std::string string_converter< unsigned >::operator ( )( const unsigned& v ) const
{
   return unsigned_to_string< unsigned >( v );
}

template< > inline std::string string_converter< unsigned short >::operator ( )( const unsigned short& v ) const
{
   return unsigned_to_string< unsigned short >( v );
}

template< > inline std::string string_converter< unsigned long >::operator ( )( const unsigned long& v ) const
{
   return unsigned_to_string< unsigned long >( v );
}

#  ifndef _LP64
template< > inline std::string string_converter< uint64_t >::operator ( )( const uint64_t& v ) const
{
   return unsigned_to_string< uint64_t >( v );
}
#  endif

template< typename T > inline std::string to_string( const T& t )
{
   return string_converter< T >( )( t );
}

#  ifdef __BORLANDC__
#     define USE_FROM_STRING_IMPL
#  endif

// NOTE: For some reason BCB is not able to use/handle the "from_string" std::string specialisation when
// explicit syntax (i.e. from_string< std::string >( s )) is used. So a work-around is to instead forward
// the "from_string" call to "from_string_impl" using implicit syntax.
#  ifndef USE_FROM_STRING_IMPL
template< typename T > inline T from_string( const std::string& s )
{
   std::istringstream iss( s );

   T t;
   iss >> t;
   return t;
}

template< > inline std::string from_string( const std::string& s )
{
   return s;
}
#  else
inline void from_string_impl( std::string& t, const std::string& s )
{
   t = s;
}

template< typename T > inline void from_string_impl( T& t, const std::string& s )
{
   std::istringstream iss( s );

   iss >> t;
}

#     ifdef NEW_BORLAND_VERSION
#        pragma option push -w-8027
#     endif
template< typename T > inline T from_string( const std::string& s )
{
   T t;
   from_string_impl( t, s );
   return t;
}
#     ifdef NEW_BORLAND_VERSION
#        pragma option pop
#     endif
#  endif

class boyer_moore
{
   public:
   boyer_moore( const std::string& pattern );

   std::string::size_type find( const std::string& text );

   private:
   std::string pattern;

   std::vector< int > jump;
   std::vector< int > slide;
};

bool wildcard_match( const char* p_expr, const char* p_data );

inline bool wildcard_match( const char* p_expr, const std::string& data ) { return wildcard_match( p_expr, data.c_str( ) ); }
inline bool wildcard_match( const std::string& expr, const char* p_data ) { return wildcard_match( expr.c_str( ), p_data ); }
inline bool wildcard_match( const std::string& expr, const std::string& data ) { return wildcard_match( expr.c_str( ), data.c_str( ) ); }

std::string& replace( std::string& s, const char* p_findstr, const char* p_replstr );

inline std::string& replace( std::string& s, std::string& findstr, const char* p_replstr )
{
   return replace( s, findstr.c_str( ), p_replstr );
}

inline std::string& replace( std::string& s, const std::string& findstr, const std::string& replstr )
{
   return replace( s, findstr.c_str( ), replstr.c_str( ) );
}

inline std::string& replace( std::string& s,
 const char* p_findstr1, const char* p_replstr1, const char* p_findstr2, const char* p_replstr2 )
{
   return replace( replace( s, p_findstr1, p_replstr1 ), p_findstr2, p_replstr2 );
}

inline std::string& replace( std::string& s,
 const std::string& findstr1, const char* p_replstr1, const char* p_findstr2, const char* p_replstr2 )
{
   return replace( replace( s, findstr1.c_str( ), p_replstr1 ), p_findstr2, p_replstr2 );
}

inline std::string& replace( std::string& s,
 const std::string& findstr1, const std::string& replstr1, const char* p_findstr2, const char* p_replstr2 )
{
   return replace( replace( s, findstr1.c_str( ), replstr1.c_str( ) ), p_findstr2, p_replstr2 );
}

inline std::string& replace( std::string& s,
 const std::string& findstr1, const std::string& replstr1, const std::string& findstr2, const char* p_replstr2 )
{
   return replace( replace( s, findstr1.c_str( ), replstr1.c_str( ) ), findstr2.c_str( ), p_replstr2 );
}

inline std::string& replace( std::string& s,
 const std::string& findstr1, const std::string& replstr1, const std::string& findstr2, const std::string& replstr2 )
{
   replace( s, findstr1.c_str( ), replstr1.c_str( ) );
   return replace( s, findstr2.c_str( ), replstr2.c_str( ) );
}

inline std::string replaced( const std::string& src, const char* p_findstr, const char* p_replstr )
{
   std::string s( src );
   replace( s, p_findstr, p_replstr );
   return s;
}

inline std::string replaced( const std::string& src, const char* p_findstr, const std::string& replstr )
{
   return replaced( src, p_findstr, replstr.c_str( ) );
}

inline std::string replaced( const std::string& src, const std::string& findstr, const std::string& replstr )
{
   return replaced( src, findstr.c_str( ), replstr.c_str( ) );
}

inline std::string replaced( const std::string& src,
 const char* p_findstr1, const char* p_replstr1, const char* p_findstr2, const char* p_replstr2 )
{
   return replaced( replaced( src, p_findstr1, p_replstr1 ), p_findstr2, p_replstr2 );
}

inline std::string replaced( const std::string& src,
 const std::string& findstr1, const char* p_replstr1, const char* p_findstr2, const char* p_replstr2 )
{
   return replaced( replaced( src, findstr1.c_str( ), p_replstr1 ), p_findstr2, p_replstr2 );
}

inline std::string replaced( const std::string& src,
 const std::string& findstr1, const std::string& replstr1, const char* p_findstr2, const char* p_replstr2 )
{
   return replaced( replaced( src, findstr1.c_str( ), replstr1.c_str( ) ), p_findstr2, p_replstr2 );
}

inline std::string replaced( const std::string& src,
 const std::string& findstr1, const std::string& replstr1, const std::string& findstr2, const char* p_replstr2 )
{
   return replaced( replaced( src, findstr1.c_str( ), replstr1.c_str( ) ), findstr2.c_str( ), p_replstr2 );
}

inline std::string replaced( const std::string& src,
 const std::string& findstr1, const std::string& replstr1, const std::string& findstr2, const std::string& replstr2 )
{
   return replaced( replaced( src, findstr1.c_str( ), replstr1.c_str( ) ), findstr2.c_str( ), replstr2.c_str( ) );
}

std::string& escape( std::string& s, const char* p_chars = 0,
 char esc = c_esc, const char* p_specials = 0, bool double_escape_specials = false );

std::string escaped( const std::string& s, const char* p_chars = 0,
 char esc = c_esc, const char* p_specials = 0, bool double_escape_specials = false );

std::string unescaped( const char* p_start, size_t len, const char* p_specials = 0, char esc = c_esc );

inline std::string& unescape( std::string& s, const char* p_specials = 0, char esc = c_esc )
{
   if( s.length( ) )
      s = unescaped( &*s.begin( ), s.length( ), p_specials, esc );
   return s;
}

#  ifdef NEW_BORLAND_VERSION
#     pragma option push -w-8027
#  endif
inline std::string unescaped( const std::string& s, const char* p_specials = 0, char esc = c_esc )
{
   return s.length( ) ? unescaped( &*s.begin( ), s.length( ), p_specials, esc ) : std::string( );
}
#  ifdef NEW_BORLAND_VERSION
#     pragma option pop
#  endif

size_t find_end_of_escaped_sequence( const std::string& s, size_t p, char eos, char esc = c_esc );

std::string& utf8_replace( std::string& utf8, const char* p_findstr, const char* p_replstr );

std::string& utf8_truncate( std::string& utf8, int trunc_limit, const char* p_overflow_suffix = 0 );

bool has_environment_variable( const char* p_env_var_name );

void replace_environment_variables( std::string& s, char bc = '%', const char* p_chars = 0, char esc = c_esc );
std::string replace_environment_variables( const char* p_str, char bc = '%', const char* p_chars = 0, char esc = c_esc );

size_t split( const std::string& s, std::set< std::string >& c, char sep = c_sep, char esc = c_esc, bool unescape = true );
size_t split( const std::string& s, std::deque< std::string >& c, char sep = c_sep, char esc = c_esc, bool unescape = true );
size_t split( const std::string& s, std::vector< std::string >& c, char sep = c_sep, char esc = c_esc, bool unescape = true );

inline size_t raw_split( const std::string& s,
 std::set< std::string >& c, char sep = c_sep, char esc = c_esc ) { return split( s, c, sep, esc, false ); }

inline size_t raw_split( const std::string& s,
 std::deque< std::string >& c, char sep = c_sep, char esc = c_esc )  { return split( s, c, sep, esc, false ); }

inline size_t raw_split( const std::string& s,
 std::vector< std::string >& v, char sep = c_sep, char esc = c_esc )  { return split( s, v, sep, esc, false ); }

size_t split( const std::string& s, std::set< std::string >& c, const std::string& sep );
size_t split( const std::string& s, std::deque< std::string >& c, const std::string& sep );
size_t split( const std::string& s, std::vector< std::string >& c, const std::string& sep );

size_t split_size( const std::string& s, char sep = c_sep, char esc = c_esc );

std::string join( const std::vector< std::string >& c, char sep = c_sep, char esc = c_esc, bool escape = true );

inline std::string raw_join( const std::vector< std::string >& c ) { return join( c, '\0', '\0', false ); }

std::string join( const std::vector< std::string >& c, const std::string& sep );

inline void remove_utf8_bom_impl( std::string& s )
{
   // NOTE: UTF-8 text files will sometimes begin with an identifying sequence "EF BB BF" as the
   // first three characters of the file (especially in Windows) so if this is found then remove.
   if( s.size( ) >= 3 && s[ 0 ] == ( char )0xef && s[ 1 ] == ( char )0xbb && s[ 2 ] == ( char )0xbf )
      s.erase( 0, 3 );
}

inline void remove_utf8_bom( std::string& s ) { remove_utf8_bom_impl( s ); }

inline void remove_trailing_cr_from_text_file_line( std::string& s, bool also_remove_uft8_bom = false )
{
   // NOTE: If a text file had been treated as binary during an FTP remove trailing CR.
   if( s.size( ) && s[ s.size( ) - 1 ] == '\r' )
      s.erase( s.size( ) - 1 );

   if( also_remove_uft8_bom )
      remove_utf8_bom_impl( s );
}

struct version_info
{
   int major;
   int minor;

   std::string ver;
};

std::string get_version_info( const std::string& s, version_info& ver_info );

bool check_version_info( const version_info& ver_info, int major_version, int minor_version, bool* p_old = 0 );

size_t setup_arguments( const char* p_input,
 std::vector< std::string >& arguments, char esc = c_esc, const char* p_specials = c_special_characters );

inline size_t setup_arguments( const std::string& s,
 std::vector< std::string >& arguments, char esc = c_esc, const char* p_specials = c_special_characters )
{
   return setup_arguments( s.c_str( ), arguments, esc, p_specials );
}

void setup_arguments( int argc, const char* argv[ ],
 std::vector< std::string >& arguments, char esc = '\0', const char* p_specials = 0 );

std::string buffer_file( const std::string& file_name );
void write_file( const std::string& file_name, const std::string& file_buffer );

void buffer_file_lines( const std::string& file_name,
 std::vector< std::string >& lines, bool skip_blank_lines = true, bool strip_extra_crs = true );

void buffer_file_lines( const std::string& file_name,
 std::set< std::string >& lines, bool strip_extra_crs = true );

bool absolute_path( const std::string& relative_path, std::string& absolute_path );

time_t last_modification_time( const std::string& filename );

void output_string_as_text_lines( const std::string& str, std::ostream& os,
 size_t max_length, const char* p_prefix = 0, const char* p_suffix = "\n" );

typedef std::map< std::string, std::string > string_container;
typedef string_container::iterator string_iterator;
typedef string_container::const_iterator string_const_iterator;
typedef string_container::value_type string_value_type;

void uudecode( std::ostream& outs, const char* p_input, int num_bytes );
void uuencode( const char* p_data, int num_bytes, std::ostream& outs );

std::string decode_quoted_printable( const std::string& data );
std::string encode_quoted_printable( const std::string& data, int max_chars_per_line );

void read_strings( const std::string& filename,
 string_container& strings, const char* p_prefix = 0,
 bool unescape_data = true, char esc = c_esc, const char* p_specials = c_special_characters );

std::string extract_text_from_html( const std::string& html );

#endif

