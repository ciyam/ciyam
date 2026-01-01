// Copyright (c) 2003-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef FORMAT_H
#  define FORMAT_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <stddef.h>
#     include <iostream>
#  endif

#  include "ptypes.h"

class ifmt
{
   friend class ifmt_setter;

   public:
   ifmt( int width, char fillchar = '0' )
    :
    width( width ),
    fillchar( fillchar )
   {
   }

   private:
   int width;
   char fillchar;
};

class ffmt
{
   friend class ffmt_setter;

   public:
   ffmt( int width, int precision )
    :
    width( width ),
    precision( precision )
   {
   }

   private:
   int width;
   int precision;
};

class ifmt_setter
{
   public:
   ifmt_setter( std::ostream& os, ifmt const& format )
    :
    outstream( os ),
    old_width( os.width( ) ),
    old_fillchar( os.fill( ) )
   {
      os.width( format.width );
      os.fill( format.fillchar );
   }

  ~ifmt_setter( )
   {
      outstream.width( old_width );
      outstream.fill( old_fillchar );
   }

   operator std::ostream&( ) const { return outstream; }

   private:
   std::ostream& outstream;

   int old_width;
   char old_fillchar;
};

inline ifmt_setter operator <<( std::ostream& os, ifmt const& format )
{
  return ifmt_setter( os, format );
}

template< class T > std::ostream& operator <<( ifmt_setter const& fs, T const& t )
{
   return static_cast< std::ostream& >( fs ) << t;
}

class ffmt_setter
{
   public:
   ffmt_setter( std::ostream& os, ffmt const& format )
    :
    outstream( os ),
    old_flags( os.flags( ) ),
    old_width( os.width( ) ),
    old_precision( os.precision( ) )
   {
      os.setf( std::ios::fixed, std::ios::floatfield );

      os.width( format.width );
      os.precision( format.precision );
   }

  ~ffmt_setter( )
   {
      outstream.flags( old_flags );

      outstream.width( old_width );
      outstream.precision( old_precision );
   }

   operator std::ostream&( ) const { return outstream; }

   private:
   std::ostream& outstream;
   std::ios::fmtflags old_flags;

   int old_width;
   int old_precision;
};

inline ffmt_setter operator <<( std::ostream& os, ffmt const& format )
{
  return ffmt_setter( os, format );
}

template< class T > std::ostream& operator <<( ffmt_setter const& fs, T const& t )
{
   return static_cast< std::ostream& >( fs ) << t;
}

std::string format_int( int64_t i,
 char separator = ',', unsigned int grouping = 3 );

std::string format_bytes( int64_t size,
 bool use_iec = false, int decimal_precision = -1, char separator = ' ' );

int64_t unformat_bytes( const std::string& size_string,
 int64_t default_val = INT64_C( 0 ), bool use_iec_always = false );

std::string format_duration( int value, bool include_seconds = true );
int unformat_duration( const std::string& value );

void split_list_items(
 const std::string& list_data, std::vector< std::string >& list_items,
 std::vector< std::string >* p_secondary_values = 0, bool* p_secondary_prefixed = 0 );

#endif

