// Copyright (c) 2005-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef NUMERIC_HELPER_H
#  define NUMERIC_HELPER_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#  endif

#  include "numeric.h"
#  include "utilities.h"
#  include "class_utilities.h"

REGISTER_TYPEOF( 40, numeric )

inline size_t size_determiner( const numeric* ) { return sizeof( numeric ); }

inline std::string to_string( const numeric& n )
{
   return n.as_string( );
}

template< > inline std::string to_comparable_string( const numeric& n )
{
   return n.as_comparable_string( );
}

template< > inline numeric from_string< numeric >( const std::string& s )
{
   numeric n( s.c_str( ) );
   return n;
}

template< > inline std::string to_formatted_string< numeric >( const numeric& n, const char* p_fmt )
{
   if( !p_fmt || std::string( p_fmt ).empty( ) )
      return to_string( n );
   else
      return format_numeric( n, p_fmt );
}

template< > inline int to_integer( const numeric& n ) { return ( int )n.as_double( ); }

template< > inline bool is_valid_str_val< numeric >( const std::string& s ) { return is_valid_numeric( s ); }

#endif

