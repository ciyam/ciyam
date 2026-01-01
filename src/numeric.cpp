// Copyright (c) 2004-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <cmath>
#  include <cstdio>
#  include <iostream>
#  include <stdexcept>
#  include <algorithm>
#endif

#include "numeric.h"

#include "read_write_stream.h"

//#define ALLOW_ADJUST_TO_ZERO

using namespace std;

namespace
{

/* NOTE: This should always be a part of the regression tests...

   Test Rounding Results Table:

   None Up Down Normal Bankers
   ---------------------------
   -2.6 -3   -2     -3      -3
   -2.5 -3   -2     -3      -2
   -2.4 -3   -2     -2      -2
   -1.6 -2   -1     -2      -2
   -1.5 -2   -1     -2      -2
   -1.4 -2   -1     -1      -1
   -0.6 -1    0     -1      -1
   -0.5 -1    0     -1       0
   -0.4 -1    0      0       0
    0.4  1    0      0       0
    0.5  1    0      1       0
    0.6  1    0      1       1
    1.4  2    1      1       1
    1.5  2    1      2       2
    1.6  2    1      2       2
    2.4  3    2      2       2
    2.5  3    2      3       2
    2.6  3    2      3       3
*/

uint64_t power10[ ] =
{
   UINT64_C( 1 ),
   UINT64_C( 10 ),
   UINT64_C( 100 ),
   UINT64_C( 1000 ),
   UINT64_C( 10000 ),
   UINT64_C( 100000 ),
   UINT64_C( 1000000 ),
   UINT64_C( 10000000 ),
   UINT64_C( 100000000 ),
   UINT64_C( 1000000000 ),
   UINT64_C( 10000000000 ),
   UINT64_C( 100000000000 ),
   UINT64_C( 1000000000000 ),
   UINT64_C( 10000000000000 ),
   UINT64_C( 100000000000000 ),
   UINT64_C( 1000000000000000 ),
   UINT64_C( 10000000000000000 ),
   UINT64_C( 100000000000000000 ),
   UINT64_C( 1000000000000000000 ),
   UINT64_C( 10000000000000000000 )
};

const uint8_t c_negative_flag = 0x80;
const uint8_t c_decimals_mask = 0x7f;

const uint64_t c_zero = UINT64_C( 0 );

const uint64_t c_max_mantissa = UINT64_C( 999999999999999999 );

const char c_opt = '#';
const char c_mand = '0';
const char c_group = ',';
const char c_point = '.';
const char c_minus = '-';
const char c_mleft = '(';
const char c_mright = ')';
const char c_either = '*';
const char c_pminus = '+';
const char c_pmleft = '[';
const char c_pmright = ']';
const char c_currency = '$';

const char* const c_decimal_point = ".";
const char* const c_plus_sign = "+";
const char* const c_minus_sign = "-";
const char* const c_group_separator = ",";
const char* const c_plus_left = "";
const char* const c_plus_right = "";
const char* const c_minus_left = "(";
const char* const c_minus_right = ")";
const char* const c_currency_symbol = "$";
const char* const c_percent_symbol = "%";

}

inline void level_decimals( numeric& n1, numeric& n2 )
{
   uint8_t neg1( n1.decimals & c_negative_flag );
   n1.decimals &= c_decimals_mask;

   uint8_t neg2( n2.decimals & c_negative_flag );
   n2.decimals &= c_decimals_mask;

   if( n1.decimals > n2.decimals )
   {
      uint8_t d( n1.decimals - n2.decimals );

      if( n2.mantissa >= power10[ numeric::e_max_digits - d ] )
         throw runtime_error( "overflow occurred" );

      n2.mantissa *= power10[ d ];
      n2.decimals = n1.decimals;
   }
   else if( n1.decimals < n2.decimals )
   {
      uint8_t d( n2.decimals - n1.decimals );

      if( n1.mantissa >= power10[ numeric::e_max_digits - d ] )
         throw runtime_error( "underflow occurred" );

      n1.mantissa *= power10[ d ];
      n1.decimals = n2.decimals;
   }

   n1.decimals |= neg1;
   n2.decimals |= neg2;
}

void perform_add_or_subtract( numeric& n1, numeric n2, bool is_add )
{
   if( !is_add )
      n2.decimals ^= c_negative_flag;

   if( n1.mantissa != c_zero && n2.mantissa != c_zero )
   {
      numeric orig( n1 );
      level_decimals( n1, n2 );

      if( ( n1.decimals & c_negative_flag ) == ( n2.decimals & c_negative_flag ) )
      {
         uint64_t m( n1.mantissa );
         m += n2.mantissa;

         if( m < n1.mantissa || m > c_max_mantissa )
         {
            n1 = orig;
            throw runtime_error( "overflow occurred" );
         }

         n1.mantissa = m;
      }
      else if( !( n1.decimals & c_negative_flag ) && ( n2.decimals & c_negative_flag ) )
      {
         if( n2.mantissa > n1.mantissa )
         {
            n1.decimals |= c_negative_flag;
            n1.mantissa = n2.mantissa - n1.mantissa;
         }
         else
         {
            n1.decimals &= ~c_negative_flag;
            n1.mantissa -= n2.mantissa;
         }
      }
      else if( ( n1.decimals & c_negative_flag ) && !( n2.decimals & c_negative_flag ) )
      {
         if( n1.mantissa > n2.mantissa )
            n1.mantissa -= n2.mantissa;
         else
         {
            n1.decimals &= ~c_negative_flag;
            n1.mantissa = n2.mantissa - n1.mantissa;
         }
      }

      uint8_t negative_flag = n1.decimals & c_negative_flag;
      n1.decimals &= c_decimals_mask;

      while( n1.decimals > 0 && n1.mantissa % 10 == 0 )
      {
         --n1.decimals;
         n1.mantissa /= 10;
      }

      n1.decimals |= negative_flag;
   }
   else
   {
      if( n1.mantissa == c_zero )
         n1 = n2;
   }
}

numeric::numeric( int i )
 :
 decimals( 0 ),
 mantissa( 0 )
{
   if( i < 0 )
   {
      i *= -1;
      decimals = c_negative_flag;
   }

   mantissa = i;
}

numeric::numeric( int64_t i )
 :
 decimals( 0 ),
 mantissa( 0 )
{
   if( i < 0 )
   {
      i *= -1;
      decimals = c_negative_flag;
   }

   mantissa = i;
}

numeric::numeric( double d, int max_decimals, round_method method, bool simplify )
 :
 decimals( 0 ),
 mantissa( 0 )
{
   if( max_decimals < 0 || max_decimals > e_max_digits )
      throw runtime_error( "max_decimals invalid" );

   int md( max_decimals );
   if( md < e_max_digits )
      ++md;

   bool is_negative( false );
   if( d < 0.0 )
      is_negative = true;

   d = abs( d );
   double f = floor( d );
   mantissa = ( uint64_t )f;

   int digits = whole_digits( );
   if( digits + md > e_max_digits )
      throw runtime_error( "max_decimals overflow" );

   d -= f;
   d *= pow( 10.0, md );
   uint64_t x = ( uint64_t )d;

   int n( md );
   if( x )
   {
      while( x % 10 == 0 )
      {
         --n;
         x /= 10;
      }
   }
   else
      n = 0;

   mantissa *= power10[ n ];
   mantissa += x;

   decimals = n;

   if( is_negative && mantissa )
      decimals |= c_negative_flag;

   if( md != max_decimals )
      round( max_decimals, method, simplify );
}

numeric::numeric( const char* p, char dec, char sep )
 :
 decimals( 0 ),
 mantissa( 0 )
{
   int n = 0;
   const char* op( p );

   const char* lp( p );
   const char* np( p );

   if( p && *p )
   {
      bool dp = false;

      // NOTE: Locate the last digit or last non-zero digit after the decimal point
      // so that extraneos trailing zero decimals will not cause a digits overflow.
      while( true )
      {
         if( *np == dec )
         {
            dp = true;
            lp = np + 1;
         }
         else if( !dp )
            lp = np;
         else if( *np != '0' )
            lp = np;

         if( *++np == 0 )
            break;
      }

      ++lp;

      uint8_t d = 0;
      uint64_t m = 0;

      dp = false;
      if( *p == '-' )
      {
         ++p;
         decimals = c_negative_flag;
      }
      else if( *p == '+' )
         ++p;

      for( ; p != lp; ++p )
      {
         if( *p == dec )
         {
            if( dp )
               throw runtime_error( "invalid numeric '" + string( op ) + "'" );
            dp = true;
         }
         else if( *p != sep )
         {
            if( dp )
            {
               if( ++n > e_max_digits )
                  throw runtime_error( "max. digits overflow for '" + string( op ) + "'" );
               ++d;
            }
            else if( n == 0 && *p == '0' )
               continue;
            else if( ++n > e_max_digits )
               throw runtime_error( "max. digits overflow for '" + string( op ) + "'" );

            if( *p < '0' || *p > '9' )
               throw runtime_error( "invalid numeric '" + string( op ) + "'" );

            m = ( 10 * m ) + ( *p - '0' );
         }
      }

      while( d > 0 && m % 10 == 0 )
      {
         --d;
         m /= 10;
      }

      mantissa = m;
      decimals |= d;
   }
}

numeric& numeric::round( int num_decimals, round_method method, bool simplify )
{
   if( method == e_round_method_none )
      return *this;

   if( num_decimals < 0 || num_decimals > e_max_digits )
      throw runtime_error( "num_decimals invalid" );

   uint8_t negative_flag = decimals & c_negative_flag;
   decimals &= c_decimals_mask;

   int num = decimals - num_decimals;

   // NOTE: Do not perform rounding unless there are too many decimals.
   if( num > 0 )
   {
      bool round_upward( false );

      uint64_t m = mantissa % power10[ num ];
      uint64_t n = power10[ num ];

      if( method == e_round_method_up )
         round_upward = ( m > 0 );
      else if( method != e_round_method_down )
         round_upward = m >= ( n / 2 );

      if( method == e_round_method_bankers )
      {
         if( m == ( n / 2 ) )
            round_upward = ( ( mantissa / power10[ num ] ) % 2 == 1 );
      }

      mantissa /= power10[ num ];

      if( round_upward )
         ++mantissa;

      decimals = num_decimals;

      if( simplify )
      {
         while( decimals > 0 && mantissa % 10 == 0 )
         {
            --decimals;
            mantissa /= 10;
         }
      }
   }

   if( !simplify )
   {
      int num_decimals_now = decimal_digits( );

      while( num_decimals_now++ < num_decimals )
      {
         mantissa *= 10;
         ++decimals;
      }
   }

   if( mantissa )
      decimals |= negative_flag;

   return *this;
}

double numeric::frac( ) const
{
   uint64_t whole = mantissa / power10[ decimals & c_decimals_mask ];
   uint64_t fraction = mantissa - ( whole * power10[ decimals & c_decimals_mask ] );

   return ( double )fraction / power10[ decimals & c_decimals_mask ];
}

int64_t numeric::trunc( ) const
{
   uint64_t whole = mantissa / power10[ decimals & c_decimals_mask ];

   int64_t signed_whole( whole );

   if( decimals & c_negative_flag )
      signed_whole *= -1;

   return signed_whole;
}

numeric& numeric::operator *=( numeric n )
{
   if( mantissa != c_zero && n.mantissa != c_zero )
   {
      uint64_t m( mantissa );
      uint8_t d( decimals & c_decimals_mask );

      if( n.mantissa )
      {
         while( d && n.mantissa % 10 == 0 )
         {
            --d;
            n.mantissa /= 10;
         }
      }

      m *= n.mantissa;

      if( m / n.mantissa != mantissa )
         throw runtime_error( "overflow occurred" );

      uint8_t negative_flag = ( decimals & c_negative_flag ) ^ ( n.decimals & c_negative_flag );

      while( d && m > c_max_mantissa )
      {
         --d;
         m /= 10;
      }

      bool adjusted = false;
      if( d )
      {
         while( d > e_max_digits )
         {
            --d;
            m /= 10;
            adjusted = true;
         }
      }

#ifndef ALLOW_ADJUST_TO_ZERO
      if( adjusted && !m )
         throw runtime_error( "underflow occurred" );
#endif

      if( m > c_max_mantissa )
         throw runtime_error( "overflow occurred" );

      d += ( n.decimals & c_decimals_mask );

      if( d > e_max_digits )
         throw runtime_error( "underflow occurred" );

      mantissa = m;
      decimals = d;

      while( decimals > 0 && mantissa % 10 == 0 )
      {
         --decimals;
         mantissa /= 10;
      }

      decimals |= negative_flag;
   }
   else
   {
      mantissa = 0;
      decimals = 0;
   }

   return *this;
}

numeric& numeric::operator /=( numeric n )
{
   if( n.mantissa == c_zero )
      throw runtime_error( "attempt to divide numeric by zero" );

   uint8_t negative_flag = decimals & c_negative_flag;
   uint8_t d( decimals & c_decimals_mask );

   if( mantissa != c_zero )
   {
      uint64_t m( mantissa );

      while( m < power10[ e_max_digits ] )
      {
         ++d;
         m *= 10;
      }

      if( ( n.decimals & c_decimals_mask ) > d )
         throw runtime_error( "underflow occurred" );

      m /= n.mantissa;

      if( m % 10 == 0 )
      {
         --d;
         m /= 10;
      }

      d -= ( n.decimals & c_decimals_mask );

      while( d && m > c_max_mantissa )
      {
         --d;
         m /= 10;
      }

      while( d > e_max_digits && m % 10 == 0 )
      {
         --d;
         m /= 10;
      }

      bool adjusted = false;
      if( d )
      {
         while( d > e_max_digits )
         {
            --d;
            m /= 10;
            adjusted = true;
         }
      }

#ifndef ALLOW_ADJUST_TO_ZERO
      if( adjusted && !m )
         throw runtime_error( "underflow occurred" );
#endif

      if( d > e_max_digits || mantissa > c_max_mantissa )
         throw runtime_error( "underflow occurred" );

      while( d > 0 && m % 10 == 0 )
      {
         --d;
         m /= 10;
      }

      mantissa = m;
   }

   negative_flag = ( negative_flag == ( n.decimals & c_negative_flag ) ) ? 0 : c_negative_flag;

   decimals = d;
   if( mantissa )
      decimals |= negative_flag;

   return *this;
}

numeric numeric::max( )
{
   numeric n;
   n.mantissa = c_max_mantissa;

   return n;
}

numeric numeric::min( )
{
   numeric n;
   n.mantissa = c_max_mantissa;
   n.decimals = c_negative_flag;

   return n;
}

numeric numeric::e( )
{
   static numeric g_e( "2.71828183" );
   return g_e;
}

numeric numeric::pi( )
{
   static numeric g_pi( "3.14159265" );
   return g_pi;
}

numeric numeric::phi( )
{
   static numeric g_phi( "1.61803399" );

   return g_phi;
}

string numeric::as_string( ) const
{
   char buf[ e_max_digits + 3 ];
   output_to_char_buffer( buf );

   return string( buf );
}

double numeric::as_double( ) const
{
   double d = ( double )mantissa;

   d /= power10[ decimals & c_decimals_mask ];

   if( decimals & c_negative_flag )
      d *= -1.0;

   return d;
}

int64_t numeric::as_int64_t( ) const
{
   int64_t i = mantissa;

   i /= power10[ decimals & c_decimals_mask ];

   if( decimals & c_negative_flag )
      i *= -1.0;

   return i;
}

uint64_t numeric::as_uint64( ) const
{
   uint64_t ui = mantissa;

   ui /= power10[ decimals & c_decimals_mask ];

   return ui;
}

string numeric::as_comparable_string( ) const
{
   string cs;
   string str( as_string( ) );

   if( str[ 0 ] == '-' )
   {
      str.erase( 0, 1 );
      cs += '-';
   }
   else
      cs += '+';

   string::size_type pos = str.find( '.' );
   int digits = ( pos == string::npos ) ? str.size( ) : str.size( ) - 1;

   if( digits < e_max_digits )
      cs += string( e_max_digits - digits, '0' );

   cs += str;

   return cs;
}

uint8_t numeric::digits( ) const
{
   int digits = 0;

   for( int i = e_max_digits - 1; i >= 0; i-- )
   {
      if( mantissa >= power10[ i ] )
      {
         digits = i + 1;
         break;
      }
   }

   return digits;
}

uint8_t numeric::whole_digits( ) const
{
   int digits = 0;

   for( int i = e_max_digits - 1; i >= 0; i-- )
   {
      if( mantissa >= power10[ i ] )
      {
         digits = i + 1;
         break;
      }
   }

   int val = digits - ( decimals & c_decimals_mask );

   return val > 0 ? val : 0;
}

void numeric::output_to_char_buffer( char* p_buf ) const
{
   char* p( p_buf );

   if( mantissa == c_zero )
      *p++ = '0';
   else
   {
      if( decimals & c_negative_flag )
         *p++ = '-';

      bool had_first = false;
      uint8_t max_whole_digits = e_max_digits - ( decimals & c_decimals_mask );

      uint64_t v = mantissa;
      for( int i = 0; i < e_max_digits; i++ )
      {
         if( i == max_whole_digits )
         {
            if( !had_first )
               *p++ = '0';

            *p++ = '.';
            had_first = true;
         }

         uint64_t d = ( v / power10[ e_max_digits - i - 1 ] );
         if( d || had_first )
         {
            *p++ = ( char )d + '0';
            v -= ( d * power10[ e_max_digits - i - 1 ] );
            had_first = true;
         }
      }
   }

   *p = '\0';
}

bool operator <( const numeric& lhs, const numeric& rhs )
{
   if( ( lhs.decimals & c_negative_flag ) != ( rhs.decimals & c_negative_flag ) )
      return ( lhs.decimals & c_negative_flag ) != 0;

   uint8_t lhs_decimals = lhs.decimals & c_decimals_mask;
   uint8_t rhs_decimals = rhs.decimals & c_decimals_mask;

   uint64_t lhs_whole = lhs.mantissa / power10[ lhs_decimals ];
   uint64_t rhs_whole = rhs.mantissa / power10[ rhs_decimals ];

   if( !( lhs.decimals & c_negative_flag ) )
   {
      if( lhs_whole < rhs_whole )
         return true;
      if( rhs_whole < lhs_whole )
         return false;
   }
   else
   {
      if( rhs_whole < lhs_whole )
         return true;
      if( lhs_whole < rhs_whole )
         return false;
   }

   uint64_t lhs_fraction = lhs.mantissa - ( lhs_whole * power10[ lhs_decimals ] );
   uint64_t lhs_comparator = lhs_fraction * power10[ numeric::e_max_digits - lhs_decimals ];

   uint64_t rhs_fraction = rhs.mantissa - ( rhs_whole * power10[ rhs_decimals ] );
   uint64_t rhs_comparator = rhs_fraction * power10[ numeric::e_max_digits - rhs_decimals ];

   if( !( lhs.decimals & c_negative_flag ) )
      return lhs_comparator < rhs_comparator;
   else
      return rhs_comparator < lhs_comparator;
}

bool operator >( const numeric& lhs, const numeric& rhs )
{
   if( ( lhs.decimals & c_negative_flag ) != ( rhs.decimals & c_negative_flag ) )
      return ( rhs.decimals & c_negative_flag ) != 0;

   uint8_t lhs_decimals = lhs.decimals & c_decimals_mask;
   uint8_t rhs_decimals = rhs.decimals & c_decimals_mask;

   uint64_t lhs_whole = lhs.mantissa / power10[ lhs_decimals ];
   uint64_t rhs_whole = rhs.mantissa / power10[ rhs_decimals ];

   if( !( lhs.decimals & c_negative_flag ) )
   {
      if( lhs_whole > rhs_whole )
         return true;
      if( rhs_whole > lhs_whole )
         return false;
   }
   else
   {
      if( rhs_whole > lhs_whole )
         return true;
      if( lhs_whole > rhs_whole )
         return false;
   }

   uint64_t lhs_fraction = lhs.mantissa - ( lhs_whole * power10[ lhs_decimals ] );
   uint64_t lhs_comparator = lhs_fraction * power10[ numeric::e_max_digits - lhs_decimals ];

   uint64_t rhs_fraction = rhs.mantissa - ( rhs_whole * power10[ rhs_decimals ] );
   uint64_t rhs_comparator = rhs_fraction * power10[ numeric::e_max_digits - rhs_decimals ];

   if( !( lhs.decimals & c_negative_flag ) )
      return lhs_comparator > rhs_comparator;
   else
      return rhs_comparator > lhs_comparator;
}

bool operator ==( const numeric& lhs, const numeric& rhs )
{
   if( lhs.mantissa == rhs.mantissa && lhs.decimals == rhs.decimals )
      return true;

   if( ( lhs.decimals & c_negative_flag ) != ( rhs.decimals & c_negative_flag ) )
      return false;

   uint8_t lhs_decimals = lhs.decimals & c_decimals_mask;
   uint8_t rhs_decimals = rhs.decimals & c_decimals_mask;

   uint64_t lhs_whole = lhs.mantissa / power10[ lhs_decimals ];
   uint64_t rhs_whole = rhs.mantissa / power10[ rhs_decimals ];

   if( lhs_whole != rhs_whole )
      return false;

   uint64_t lhs_fraction = lhs.mantissa - ( lhs_whole * power10[ lhs_decimals ] );
   uint64_t lhs_comparator = lhs_fraction * power10[ numeric::e_max_digits - lhs_decimals ];

   uint64_t rhs_fraction = rhs.mantissa - ( rhs_whole * power10[ rhs_decimals ] );
   uint64_t rhs_comparator = rhs_fraction * power10[ numeric::e_max_digits - rhs_decimals ];

   return lhs_comparator == rhs_comparator;
}

ostream& operator <<( ostream& os, const numeric& n )
{
   char buf[ numeric::e_max_digits + 3 ];
   n.output_to_char_buffer( buf );

   os << buf;

   return os;
}

read_stream& operator >>( read_stream& rs, numeric& n )
{
   rs >> n.decimals >> n.mantissa;
   return rs;
}

write_stream& operator <<( write_stream& ws, const numeric& n )
{
   ws << n.decimals << n.mantissa;
   return ws;
}

numeric abs( const numeric& n )
{
   numeric abs_n( n );

   if( abs_n < 0 )
      abs_n *= -1;

   return abs_n;
}

numeric sqrt( const numeric& n )
{
   if( n < 1 )
      throw runtime_error( "invalid attempt to sqrt a number less than 1" );
      
   numeric low = 0;
   numeric mid, high;

   high = mid = n;
   numeric omid = -1;

   numeric accuracy = 1;
   accuracy /= power10[ numeric::e_default_decimals + 4 ];

   while( abs( omid - mid ) >= accuracy )
   {
      omid = mid;
      mid = ( high + low ) / 2;

      // NOTE: Avoid some potential mantissa overflows.
      if( mid > 10000000 )
         mid.round( 0 );
      else if( mid > 1000000 )
         mid.round( 1 );
      else if( mid > 100000 )
         mid.round( 2 );
      else if( mid > 10000 )
         mid.round( 3 );
      else if( mid > 1000 )
         mid.round( 4 );
      else if( mid > 100 )
         mid.round( numeric::e_default_decimals );
      else if( mid > 10 )
         mid.round( numeric::e_default_decimals + 2 );
      else if( mid > 2 )
         mid.round( numeric::e_default_decimals + 3 );
      else
         mid.round( numeric::e_default_decimals + 4 );

      if( mid * mid < n )
         low = mid;
      else
         high = mid;
   }

   // NOTE: Reduce the number of decimals to match constants such as pi.
   mid.round( numeric::e_default_decimals + 3, numeric::e_round_method_normal, true );

   return mid;
}

string format_numeric( const numeric& n, const string& mask,
 const char* p_decimal_point, const char* p_plus, const char* p_minus,
 const char* p_group_separator, const char* p_plus_left, const char* p_plus_right,
 const char* p_minus_left, const char* p_minus_right, const char* p_currency_symbol )
{
   string retval;

   string decimal_point( p_decimal_point ? p_decimal_point : c_decimal_point );
   string plus( p_plus ? p_plus : c_plus_sign );
   string minus( p_minus ? p_minus : c_minus_sign );
   string group_separator( p_group_separator ? p_group_separator : c_group_separator );
   string plus_left( p_plus_left ? p_plus_left : c_plus_left );
   string plus_right( p_plus_right ? p_plus_right : c_plus_right );
   string minus_left( p_minus_left ? p_minus_left : c_minus_left );
   string minus_right( p_minus_right ? p_minus_right : c_minus_right );
   string currency_symbol( p_currency_symbol ? p_currency_symbol : c_currency_symbol );

   string basic_output( n.as_string( ) );

   if( basic_output.empty( ) )
      throw runtime_error( "unexpected empty output from numeric::as_string( )" );

   bool is_negative = false;
   if( basic_output[ 0 ] == '-' )
   {
      is_negative = true;
      basic_output.erase( 0, 1 );
   }

   string whole_digits, decimal_digits;

   string::size_type pos = basic_output.find( '.' );
   if( pos != string::npos )
   {
      whole_digits = basic_output.substr( 0, pos );
      decimal_digits = basic_output.substr( pos + 1 );

      while( decimal_digits.size( ) && decimal_digits[ decimal_digits.size( ) - 1 ] == '0' )
         decimal_digits.erase( decimal_digits.size( ) - 1 );
   }
   else
      whole_digits = basic_output;

   size_t total_whole_digits = 0;
   size_t total_wholes_required = 0;

   size_t total_decimal_digits = 0;
   size_t total_decimals_required = 0;

   size_t mask_whole_side = mask.find( '.' );

   bool is_escape = false;
   for( size_t i = 0; i < mask.size( ); i++ )
   {
      if( mask[ i ] == '\\' )
         is_escape = !is_escape;

      if( !is_escape )
      {
         if( i < mask_whole_side )
         {
            if( mask[ i ] == '#' && total_wholes_required > 0 )
               throw runtime_error( "invalid numeric mask '" + mask + "'" );

            if( mask[ i ] == '#' || mask[ i ] == '0' )
               ++total_whole_digits;

            if( mask[ i ] == '0' )
               ++total_wholes_required;
         }
         else
         {
            if( mask[ i ] == '0' && total_decimal_digits > total_decimals_required )
               throw runtime_error( "invalid numeric mask '" + mask + "'" );

            if( mask[ i ] == '#' || mask[ i ] == '0' )
               ++total_decimal_digits;

            if( mask[ i ] == '0' )
               ++total_decimals_required;
         }
      }
   }

   while( whole_digits.size( ) < total_wholes_required )
      whole_digits = '0' + whole_digits;

   while( decimal_digits.size( ) < total_decimals_required )
      decimal_digits += '0';

   if( whole_digits.size( ) > total_whole_digits )
      throw runtime_error( "numeric value '" + n.as_string( ) + "' cannot be formatted with '" + mask + "'" );

   if( decimal_digits.size( ) > total_decimal_digits )
      throw runtime_error( "numeric value '" + n.as_string( ) + "' cannot be formatted with '" + mask + "'" );

   is_escape = false;

   bool in_decimals = false;
   bool has_output_whole = false;
   bool has_output_decimal = false;

   for( size_t i = 0; i < mask.size( ); i++ )
   {
      if( mask [ i ] == '\\' )
         is_escape = !is_escape;

      if( !is_escape )
      {
         switch( mask[ i ] )
         {
            case c_opt:
            if( !in_decimals )
            {
               if( !whole_digits.empty( ) && total_whole_digits <= whole_digits.size( ) )
               {
                  retval += whole_digits[ 0 ];
                  whole_digits.erase( 0, 1 );
                  has_output_whole = true;
               }

               --total_whole_digits;
            }
            else
            {
               if( !decimal_digits.empty( ) && total_decimal_digits <= decimal_digits.size( ) )
               {
                  retval += decimal_digits[ 0 ];
                  decimal_digits.erase( 0, 1 );
                  has_output_decimal = true;
               }

               --total_decimal_digits;
            }
            break;

            case c_mand:
            if( !in_decimals )
            {
               if( !whole_digits.empty( ) && ( total_wholes_required <= whole_digits.size( ) ) )
               {
                  retval += whole_digits[ 0 ];
                  whole_digits.erase( 0, 1 );

                  --total_wholes_required;
               }
               else
                  retval += '0';

               --total_whole_digits;
               has_output_whole = true;
            }
            else
            {
               if( !decimal_digits.empty( ) && ( total_decimals_required <= decimal_digits.size( ) ) )
               {
                  retval += decimal_digits[ 0 ];
                  decimal_digits.erase( 0, 1 );

                  --total_decimals_required;
               }
               else
                  retval += '0';

               --total_decimal_digits;
               has_output_decimal = true;
            }
            break;

            case c_group:
            if( ( !in_decimals && has_output_whole )
             || ( in_decimals && has_output_decimal ) )
               retval += group_separator;
            break;

            case c_point:
            in_decimals = true;
            if( total_decimals_required || !decimal_digits.empty( ) )
               retval += decimal_point;
            break;

            case c_minus:
            if( n < 0.0 )
               retval += minus;
            break;

            case c_mleft:
            if( n < 0.0 )
               retval += minus_left;
            break;

            case c_mright:
            if( n < 0.0 )
               retval += minus_right;
            break;

            case c_either:
            case c_pminus:
            if( n > 0.0 )
               retval += plus;
            else if( n < 0.0 )
               retval += minus;
            break;

            case c_pmleft:
            if( n > 0.0 )
               retval += plus_left;
            else if( n < 0.0 )
               retval += minus_left;
            break;

            case c_pmright:
            if( n > 0.0 )
               retval += plus_right;
            else if( n < 0.0 )
               retval += minus_right;
            break;

            case c_currency:
            retval += currency_symbol;
            break;

            default:
            retval += mask[ i ];
         }
      }
   }

   return retval;
}

string format_percentage( size_t& fracs, unsigned long& prior,
 unsigned long count, unsigned long total, const char suffix )
{
   unsigned long val( count );

   if( val > total )
      val = total;

   numeric cval( val );
   numeric pval( prior );

   string mask( "###" );

   if( !total )
      cval = pval = fracs = 0;
   else
   {
      cval /= total;
      pval /= total;
   }

   cval *= 100;
   pval *= 100;

   if( val && ( val != total ) )
   {
      if( !fracs )
      {
         if( cval > 0 && cval < 0.01 )
            fracs = 3;
         else if( cval > 0 && cval < 0.1 )
            fracs = 2;
         else if( cval > 0 && cval < 1 )
            fracs = 1;
      }

      float adjust_add = 1.0;
      float adjust_sub = 10.0;

      for( int i = 0; i < fracs; i++ )
      {
         adjust_add /= 10.0;
         adjust_sub /= 10.0;
      }

      numeric cval_orig( cval );

      cval.round( fracs, numeric::e_round_method_down );
      pval.round( fracs, numeric::e_round_method_down );

      size_t use_fracs( fracs );

      // NOTE: Adjust the number of decimals according to the progress rate.
      if( cval )
      {
         if( fracs < 3 && ( cval < ( pval + adjust_add ) ) )
         {
            ++fracs;

            cval = cval_orig;
            cval.round( fracs );
         }
         else if( fracs > 0 && ( cval >= ( pval + adjust_sub ) ) )
         {
            --fracs;
            cval.round( fracs, numeric::e_round_method_down );
         }
      }

      if( fracs )
         mask += '.' + string( fracs, '#' );
   }

   if( suffix )
      mask += suffix;

   prior = val;

   return format_numeric( cval, mask );
}
