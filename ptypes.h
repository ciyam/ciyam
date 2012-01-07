// Copyright (c) 2004
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

#ifndef PTYPES_H
#  define PTYPES_H

#  include "macros.h"

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     ifndef _WIN32
#        include <cmath>
#     else
#        include <cfloat>
#     endif
#     include <limits>
#  endif

#  ifdef _WIN32
typedef __int64 long_long;
typedef unsigned __int64 unsigned_long_long;
#  else
typedef long long long_long;
typedef unsigned long long unsigned_long_long;
#  endif

#  ifndef _MSC_VER
#     include <stdint.h>
#  else
#     ifdef _WIN32_WCE
typedef char int8_t;
typedef short int16_t;
typedef int int32_t;
#     else
typedef __int8 int8_t;
typedef __int16 int16_t;
typedef __int32 int32_t;
#     endif
typedef __int64 int64_t;

#     ifdef _WIN32_WCE
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
#     else
typedef unsigned __int8 uint8_t;
typedef unsigned __int16 uint16_t;
typedef unsigned __int32 uint32_t;
#     endif
typedef unsigned __int64 uint64_t;

#     define INT8_C( x ) ( ( int8_t )x )
#     define INT16_C( x ) ( ( int16_t )x )
#     define INT32_C( x ) ( ( int32_t )x )
#     define INT64_C( x ) ( ( int64_t )x )

#     define UINT8_C( x ) ( ( uint8_t )x )
#     define UINT16_C( x ) ( ( uint16_t )x )
#     define UINT32_C( x ) ( ( uint32_t )x )
#     define UINT64_C( x ) ( ( uint64_t )x )
#  endif

#  ifdef _WIN32
#     ifdef _MSC_VER
typedef float fp32_t;
typedef double fp64_t;
#     endif
#     ifdef __BORLANDC__
typedef float fp32_t;
typedef double fp64_t;
typedef long double fp80_t;
#     endif
#  endif

#  ifdef __GNUG__
typedef float fp32_t;
typedef double fp64_t;
typedef long double fp96_t;
#  endif

template< typename T > inline T get_pos_inf( ) { return std::numeric_limits< T >::infinity( ); }
template< typename T > inline T get_neg_inf( ) { return get_pos_inf< T >( ) * T( -1.0 ); }
template< typename T > inline T get_quiet_nan( ) { return std::numeric_limits< T >::quiet_NaN( ); }

enum fp_class
{
   e_fp_class_finite,
   e_fp_class_pos_inf,
   e_fp_class_neg_inf,
   e_fp_class_quiet_nan
};

fp_class get_fp_class( double d );
fp_class get_fp_classf( float f );
fp_class get_fp_classl( long double ld );

#endif

