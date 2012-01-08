// Copyright (c) 2005
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

#ifndef DATE_TIME_HELPER_H
#  define DATE_TIME_HELPER_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#     include <sstream>
#     include <iostream>
#  endif

#  include "format.h"
#  include "utilities.h"
#  include "date_time.h"

inline size_t size_determiner( const date_time* ) { return sizeof( date_time ); }

#  ifdef NEW_BORLAND_VERSION
#     pragma option push -w-8027
#  endif
inline std::string to_string( const julian& j )
{
   std::ostringstream oss;
   oss << ffmt( 6, 6 ) << j;

   return oss.str( );
}
#  ifdef NEW_BORLAND_VERSION
#     pragma option pop
#  endif

template< > inline date_time from_string< date_time >( const std::string& s )
{
   if( s.empty( ) )
      return date_time( );
   else
   {
      date_time dt( s );
      return dt;
   }
}

#endif

