// Copyright (c) 2006
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

#ifndef MTIME_HELPER_H
#  define MTIME_HELPER_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#     include <sstream>
#     include <iostream>
#  endif

#  include "utilities.h"
#  include "date_time.h"
#  include "class_utilities.h"

REGISTER_TYPEOF( 32, mtime )

inline size_t size_determiner( const mtime* ) { return sizeof( mtime ); }

#  ifdef NEW_BORLAND_VERSION
#     pragma option push -w-8027
#  endif
inline std::string to_string( const mtime& mt )
{
   return mt.as_string( true, false );
}
#  ifdef NEW_BORLAND_VERSION
#     pragma option pop
#  endif

template< > inline mtime from_string< mtime >( const std::string& s )
{
   if( s.empty( ) )
      return mtime( );
   else
   {
      mtime mt( s );
      return mt;
   }
}

template< > inline bool is_valid_str_val< mtime >( const std::string& s ) { return is_valid_time( s ); }

#endif

