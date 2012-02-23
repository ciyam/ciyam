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

#ifndef NUMERIC_HELPER_H
#  define NUMERIC_HELPER_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#  endif

#  include "numeric.h"
#  include "utilities.h"
#  include "class_utilities.h"

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

template< > inline int to_integer( const numeric& n ) { return ( int )n.as_double( ); }

template< > inline bool is_valid_str_val< numeric >( const std::string& s ) { return is_valid_numeric( s ); }

#endif

