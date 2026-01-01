// Copyright (c) 2006-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

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

inline std::string to_string( const mtime& mt )
{
   return mt.as_string( true, false );
}

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
