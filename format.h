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

std::string format_bytes( int64_t size );
int64_t unformat_bytes( const std::string& size_string, int64_t default_val = INT64_C( 0 ) );

std::string format_duration( int value, bool include_seconds = true );
int unformat_duration( const std::string& value );

#endif

