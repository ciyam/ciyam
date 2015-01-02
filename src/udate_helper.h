// Copyright (c) 2006-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2015 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef UDATE_HELPER_H
#  define UDATE_HELPER_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#     include <sstream>
#     include <iostream>
#  endif

#  include "utilities.h"
#  include "date_time.h"
#  include "class_utilities.h"

REGISTER_TYPEOF( 31, udate )

inline size_t size_determiner( const udate* ) { return sizeof( udate ); }

template< > inline udate from_string< udate >( const std::string& s )
{
   if( s.empty( ) )
      return udate( );
   else
   {
      udate ud( s );
      return ud;
   }
}

template< > inline bool is_valid_str_val< udate >( const std::string& s ) { return is_valid_date( s ); }

#endif

