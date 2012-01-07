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

#ifdef __BORLANDC__
#  include "precompile.h"
#endif
#pragma hdrstop

#include "ptypes.h"

using namespace std;

fp_class get_fp_class( double d )
{
   fp_class fpc( e_fp_class_finite );

#ifdef _WIN32
   int c = _fpclass( d );
   switch( c )
   {
      case _FPCLASS_PINF:
      fpc = e_fp_class_pos_inf;
      break;

      case _FPCLASS_NINF:
      fpc = e_fp_class_neg_inf;
      break;

      case _FPCLASS_QNAN:
      fpc = e_fp_class_quiet_nan;
      break;
   }
#else
   int c = isinf( d );

   if( c > 0 )
      fpc = e_fp_class_pos_inf;
   else if( c < 0 )
      fpc = e_fp_class_neg_inf;
   else if( isnan( d ) )
      fpc = e_fp_class_quiet_nan;
#endif

   return fpc;
}

fp_class get_fp_classf( float f )
{
   fp_class fpc( e_fp_class_finite );

#ifdef _WIN32
   int c = _fpclass( f );
   switch( c )
   {
      case _FPCLASS_PINF:
      fpc = e_fp_class_pos_inf;
      break;

      case _FPCLASS_NINF:
      fpc = e_fp_class_neg_inf;
      break;

      case _FPCLASS_QNAN:
      fpc = e_fp_class_quiet_nan;
      break;
   }
#else
   int c = isinff( f );

   if( c > 0 )
      fpc = e_fp_class_pos_inf;
   else if( c < 0 )
      fpc = e_fp_class_neg_inf;
   else if( isnanf( f ) )
      fpc = e_fp_class_quiet_nan;
#endif

   return fpc;
}

fp_class get_fp_classl( long double ld )
{
   fp_class fpc( e_fp_class_finite );

#ifdef _WIN32
#  ifdef _MSC_VER
   int c = _fpclass( ld );
#  else
   int c = _fpclassl( ld );
# endif
   switch( c )
   {
      case _FPCLASS_PINF:
      fpc = e_fp_class_pos_inf;
      break;

      case _FPCLASS_NINF:
      fpc = e_fp_class_neg_inf;
      break;

      case _FPCLASS_QNAN:
      fpc = e_fp_class_quiet_nan;
      break;
   }
#else
   int c = isinfl( ld );

   if( c > 0 )
      fpc = e_fp_class_pos_inf;
   else if( c < 0 )
      fpc = e_fp_class_neg_inf;
   else if( isnanl( ld ) )
      fpc = e_fp_class_quiet_nan;
#endif

   return fpc;
}

