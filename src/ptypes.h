// Copyright (c) 2004-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef PTYPES_H
#  define PTYPES_H

#  include "macros.h"

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <cmath>
#     include <limits>
#     include <sys/types.h>
#  endif

typedef long long long_long;
typedef unsigned long long unsigned_long_long;

#  include <stdint.h>

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
