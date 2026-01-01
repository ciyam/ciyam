// Copyright (c) 2004-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#include "ptypes.h"

using namespace std;

fp_class get_fp_class( double d )
{
   fp_class fpc( e_fp_class_finite );

   int c = isinf( d );

   if( c > 0 )
      fpc = e_fp_class_pos_inf;
   else if( c < 0 )
      fpc = e_fp_class_neg_inf;
   else if( isnan( d ) )
      fpc = e_fp_class_quiet_nan;

   return fpc;
}

fp_class get_fp_classf( float f )
{
   fp_class fpc( e_fp_class_finite );

   int c = isinff( f );

   if( c > 0 )
      fpc = e_fp_class_pos_inf;
   else if( c < 0 )
      fpc = e_fp_class_neg_inf;
   else if( isnanf( f ) )
      fpc = e_fp_class_quiet_nan;

   return fpc;
}

fp_class get_fp_classl( long double ld )
{
   fp_class fpc( e_fp_class_finite );

   int c = isinfl( ld );

   if( c > 0 )
      fpc = e_fp_class_pos_inf;
   else if( c < 0 )
      fpc = e_fp_class_neg_inf;
   else if( isnanl( ld ) )
      fpc = e_fp_class_quiet_nan;

   return fpc;
}
