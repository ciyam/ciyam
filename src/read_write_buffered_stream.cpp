// Copyright (c) 2004-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <limits.h>
#  include <cassert>
#  include <limits>
#  include <memory>
#endif

#include "read_write_buffered_stream.h"

#include "macros.h"
#include "ptypes.h"
#include "utilities.h"

//#define READ_WRITE_BUFFERED_STREAM_TESTBED

using namespace std;

const char c_bool_true = '1';
const char c_bool_false = '0';

const char c_infinity = 'I';
const char c_quiet_nan = 'Q';

const char c_negative_sign = '-';
const char c_positive_sign = '+';

const char c_exponent_prefix = 'E';

const size_t c_bool_length = 1;
const size_t c_char_length = 1;
const size_t c_schar_length = 1;
const size_t c_uchar_length = 1;

const size_t c_min_wchar_length = 2;
const size_t c_min_short_length = 3;
const size_t c_min_ushort_length = 2;
const size_t c_min_int_length = 3;
const size_t c_min_uint_length = 2;
const size_t c_min_long_length = 3;
const size_t c_min_ulong_length = 2;
const size_t c_min_long_long_length = 3;
const size_t c_min_ulong_long_length = 2;
const size_t c_min_float_length = 4;
const size_t c_min_double_length = 4;
const size_t c_min_ldouble_length = 4;

const char c_variable_length_terminator = '\0';

const int c_max_conversion_buffer_size = 32;

void read_convert_type_none( read_buffer& rb, size_t len, unsigned char* p_output_buf )
{
   rb.read_data( p_output_buf, len );
}

void read_convert_type_bool( read_buffer& rb, size_t len, unsigned char* p_output_buf )
{
   rb.read_data( p_output_buf, len );

   if( *( p_output_buf ) == c_bool_true )
      *( bool* )( p_output_buf ) = true;
   else
      *( bool* )( p_output_buf ) = false;
}

void read_convert_type_char( read_buffer& rb, size_t len, unsigned char* p_output_buf )
{
   rb.read_data( p_output_buf, len );
}

void read_convert_type_schar( read_buffer& rb, size_t len, unsigned char* p_output_buf )
{
   rb.read_data( p_output_buf, len );
}

void read_convert_type_uchar( read_buffer& rb, size_t len, unsigned char* p_output_buf )
{
   rb.read_data( p_output_buf, len );
}

void read_convert_type_wchar( read_buffer& rb, size_t /*len*/, unsigned char* p_output_buf )
{
   wchar_t v = 0;

   unsigned char digit;
   while( true )
   {
      rb.read_data( &digit, sizeof( unsigned char ) );

      if( digit == c_variable_length_terminator )
         break;

      v *= 10;
      v += ( digit - '0' );
   }

   *( wchar_t* )p_output_buf = v;
}

void read_convert_type_short( read_buffer& rb, size_t /*len*/, unsigned char* p_output_buf )
{
   short v = 0;
   bool is_negative = false;

   unsigned char sign;
   rb.read_data( &sign, sizeof( unsigned char ) );

   if( sign == c_negative_sign )
      is_negative = true;

   unsigned char digit;
   while( true )
   {
      rb.read_data( &digit, sizeof( unsigned char ) );

      if( digit == c_variable_length_terminator )
         break;

      v *= 10;
      v += ( digit - '0' );
   }

   if( is_negative )
      v *= -1;

   *( short* )p_output_buf = v;
}

void read_convert_type_ushort( read_buffer& rb, size_t /*len*/, unsigned char* p_output_buf )
{
   unsigned short v = 0;

   unsigned char digit;
   while( true )
   {
      rb.read_data( &digit, sizeof( unsigned char ) );

      if( digit == c_variable_length_terminator )
         break;

      v *= 10;
      v += ( digit - '0' );
   }

   *( unsigned short* )p_output_buf = v;
}

void read_convert_type_int( read_buffer& rb, size_t /*len*/, unsigned char* p_output_buf )
{
   int v = 0;
   bool is_negative = false;

   unsigned char sign;
   rb.read_data( &sign, sizeof( unsigned char ) );

   if( sign == c_negative_sign )
      is_negative = true;

   unsigned char digit;
   while( true )
   {
      rb.read_data( &digit, sizeof( unsigned char ) );

      if( digit == c_variable_length_terminator )
         break;

      v *= 10;
      v += ( digit - '0' );
   }

   if( is_negative )
      v *= -1;

   *( int* )p_output_buf = v;
}

void read_convert_type_uint( read_buffer& rb, size_t /*len*/, unsigned char* p_output_buf )
{
   unsigned int v = 0;

   unsigned char digit;
   while( true )
   {
      rb.read_data( &digit, sizeof( unsigned char ) );

      if( digit == c_variable_length_terminator )
         break;

      v *= 10;
      v += ( digit - '0' );
   }

   *( unsigned int* )p_output_buf = v;
}

void read_convert_type_long( read_buffer& rb, size_t /*len*/, unsigned char* p_output_buf )
{
   long v = 0;
   bool is_negative = false;

   unsigned char sign;
   rb.read_data( &sign, sizeof( unsigned char ) );

   if( sign == c_negative_sign )
      is_negative = true;

   unsigned char digit;
   while( true )
   {
      rb.read_data( &digit, sizeof( unsigned char ) );

      if( digit == c_variable_length_terminator )
         break;

      v *= 10;
      v += ( digit - '0' );
   }

   if( is_negative )
      v *= -1;

   *( long* )p_output_buf = v;
}

void read_convert_type_ulong( read_buffer& rb, size_t /*len*/, unsigned char* p_output_buf )
{
   unsigned long v = 0;

   unsigned char digit;
   while( true )
   {
      rb.read_data( &digit, sizeof( unsigned char ) );

      if( digit == c_variable_length_terminator )
         break;

      v *= 10;
      v += ( digit - '0' );
   }

   *( unsigned long* )p_output_buf = v;
}

void read_convert_type_long_long( read_buffer& rb, size_t /*len*/, unsigned char* p_output_buf )
{
   long_long v = 0;
   bool is_negative = false;

   unsigned char sign;
   rb.read_data( &sign, sizeof( unsigned char ) );

   if( sign == c_negative_sign )
      is_negative = true;

   unsigned char digit;
   while( true )
   {
      rb.read_data( &digit, sizeof( unsigned char ) );

      if( digit == c_variable_length_terminator )
         break;

      v *= 10;
      v += ( digit - '0' );
   }

   if( is_negative )
      v *= -1;

   *( long_long* )p_output_buf = v;
}

void read_convert_type_ulong_long( read_buffer& rb, size_t /*len*/, unsigned char* p_output_buf )
{
   unsigned_long_long v = 0;

   unsigned char digit;
   while( true )
   {
      rb.read_data( &digit, sizeof( unsigned char ) );

      if( digit == c_variable_length_terminator )
         break;

      v *= 10;
      v += ( digit - '0' );
   }

   *( unsigned_long_long* )p_output_buf = v;
}

void read_convert_type_float( read_buffer& rb, size_t /*len*/, unsigned char* p_output_buf )
{
   float v = 0.0f;
   bool is_negative = false;

   unsigned char sign;
   rb.read_data( &sign, sizeof( unsigned char ) );

   if( sign == c_quiet_nan )
   {
#ifdef NO_SUPPORT_FOR_INF_OR_NAN
      throw runtime_error( "this compiled version has no support for NaN" );
#else
      v = get_quiet_nan< float >( );

      rb.read_data( &sign, sizeof( unsigned char ) );
      assert( sign == c_variable_length_terminator );
#endif
   }
   else
   {
      if( sign == c_negative_sign )
         is_negative = true;

      float mul = 16.0f;
      unsigned char digit;

      rb.read_data( &digit, sizeof( unsigned char ) );
      if( digit == c_infinity )
      {
#ifdef NO_SUPPORT_FOR_INF_OR_NAN
         throw runtime_error( "this compiled version has no support for +/-INF" );
#else
         if( !is_negative )
            v = get_pos_inf< float >( );
         else
            v = get_neg_inf< float >( );

         rb.read_data( &digit, sizeof( unsigned char ) );
         assert( digit == c_variable_length_terminator );
#endif
      }
      else
      {
         while( true )
         {
            if( digit == c_exponent_prefix )
               break;

            v += ( hex_nibble( digit ) * mul );
            mul /= 16.0f;

            rb.read_data( &digit, sizeof( unsigned char ) );
         }

         if( is_negative )
            v *= -1.0f;

         int exp = 0, m = 1;
         is_negative = false;

         rb.read_data( &sign, sizeof( unsigned char ) );
         if( sign == c_negative_sign )
            is_negative = true;

         while( true )
         {
            rb.read_data( &digit, sizeof( unsigned char ) );

            if( digit == c_variable_length_terminator )
               break;

            exp += ( ( digit - '0' ) * m );
            m *= 10;
         }

         if( is_negative )
            exp *= -1;

         while( exp != 1 )
         {
            if( exp > 1 )
            {
               --exp;
               v *= 16.0f;
            }
            else
            {
               ++exp;
               v /= 16.0f;
            }
         }
      }
   }

   *( float* )p_output_buf = v;
}

void read_convert_type_double( read_buffer& rb, size_t /*len*/, unsigned char* p_output_buf )
{
   double v = 0.0;
   bool is_negative = false;

   unsigned char sign;
   rb.read_data( &sign, sizeof( unsigned char ) );

   if( sign == c_quiet_nan )
   {
#ifdef NO_SUPPORT_FOR_INF_OR_NAN
      throw runtime_error( "this compiled version has no support for NaN" );
#else
      v = get_quiet_nan< double >( );

      rb.read_data( &sign, sizeof( unsigned char ) );
      assert( sign == c_variable_length_terminator );
#endif
   }
   else
   {
      if( sign == c_negative_sign )
         is_negative = true;

      double mul = 16.0;
      unsigned char digit;

      rb.read_data( &digit, sizeof( unsigned char ) );
      if( digit == c_infinity )
      {
#ifdef NO_SUPPORT_FOR_INF_OR_NAN
         throw runtime_error( "this compiled version has no support for +/-INF" );
#else
         if( !is_negative )
            v = get_pos_inf< double >( );
         else
            v = get_neg_inf< double >( );

         rb.read_data( &digit, sizeof( unsigned char ) );
         assert( digit == c_variable_length_terminator );
#endif
      }
      else
      {
         while( true )
         {
            if( digit == c_exponent_prefix )
               break;

            v += ( hex_nibble( digit ) * mul );
            mul /= 16.0;

            rb.read_data( &digit, sizeof( unsigned char ) );
         }

         if( is_negative )
            v *= -1.0;

         int exp = 0, m = 1;
         is_negative = false;

         rb.read_data( &sign, sizeof( unsigned char ) );

         if( sign == c_negative_sign )
            is_negative = true;

         while( true )
         {
            rb.read_data( &digit, sizeof( unsigned char ) );

            if( digit == c_variable_length_terminator )
               break;

            exp += ( ( digit - '0' ) * m );
            m *= 10;
         }

         if( is_negative )
            exp *= -1;

         while( exp != 1 )
         {
            if( exp > 1 )
            {
               --exp;
               v *= 16.0;
            }
            else
            {
               ++exp;
               v /= 16.0;
            }
         }
      }
   }

   *( double* )p_output_buf = v;
}

void read_convert_type_ldouble( read_buffer& rb, size_t /*len*/, unsigned char* p_output_buf )
{
   long double v = 0.0l;
   bool is_negative = false;

   unsigned char sign;
   rb.read_data( &sign, sizeof( unsigned char ) );

   if( sign == c_quiet_nan )
   {
#ifdef NO_SUPPORT_FOR_INF_OR_NAN
      throw runtime_error( "this compiled version has no support for NaN" );
#else
      v = get_quiet_nan< long double >( );

      rb.read_data( &sign, sizeof( unsigned char ) );
      assert( sign == c_variable_length_terminator );
#endif
   }
   else
   {
      if( sign == c_negative_sign )
         is_negative = true;

      long double mul = 16.0l;
      unsigned char digit;

      rb.read_data( &digit, sizeof( unsigned char ) );
      if( digit == c_infinity )
      {
#ifdef NO_SUPPORT_FOR_INF_OR_NAN
         throw runtime_error( "this compiled version has no support for +/-INF" );
#else
         if( !is_negative )
            v = get_pos_inf< long double >( );
         else
            v = get_neg_inf< long double >( );

         rb.read_data( &digit, sizeof( unsigned char ) );
         assert( digit == c_variable_length_terminator );
#endif
      }
      else
      {
         while( true )
         {
            if( digit == c_exponent_prefix )
               break;

            v += ( hex_nibble( digit ) * mul );
            mul /= 16.0l;

            rb.read_data( &digit, sizeof( unsigned char ) );
         }

         if( is_negative )
            v *= -1.0l;

         int exp = 0, m = 1;
         is_negative = false;

         rb.read_data( &sign, sizeof( unsigned char ) );

         if( sign == c_negative_sign )
            is_negative = true;

         while( true )
         {
            rb.read_data( &digit, sizeof( unsigned char ) );

            if( digit == c_variable_length_terminator )
               break;

            exp += ( ( digit - '0' ) * m );
            m *= 10;
         }

         if( is_negative )
            exp *= -1;

         while( exp != 1 )
         {
            if( exp > 1 )
            {
               --exp;
               v *= 16.0l;
            }
            else
            {
               ++exp;
               v /= 16.0l;
            }
         }
      }
   }

   *( long double* )p_output_buf = v;
}

void write_convert_type_none(
 const unsigned char* p_input_buf, size_t& /*len*/, unsigned char*& p_output_buf )
{
   p_output_buf = const_cast< unsigned char* >( p_input_buf );
}

void write_convert_type_bool( const unsigned char* p_input_buf, size_t& len, unsigned char*& p_output_buf )
{
   unsigned char* p_buf = p_output_buf;

   len = c_bool_length;
   if( *( const bool* )p_input_buf )
      *p_buf = c_bool_true;
   else
      *p_buf = c_bool_false;
}

void write_convert_type_char( const unsigned char* p_input_buf, size_t& len, unsigned char*& p_output_buf )
{
   unsigned char* p_buf = p_output_buf;

   len = c_char_length;
   *p_buf = *( const char* )p_input_buf;
}

void write_convert_type_schar( const unsigned char* p_input_buf, size_t& len, unsigned char*& p_output_buf )
{
   unsigned char* p_buf = p_output_buf;

   len = c_schar_length;
   *p_buf = *( const signed char* )p_input_buf;
}

void write_convert_type_uchar( const unsigned char* p_input_buf, size_t& len, unsigned char*& p_output_buf )
{
   unsigned char* p_buf = p_output_buf;

   len = c_uchar_length;
   *p_buf = *( const unsigned char* )p_input_buf;
}

void write_convert_type_wchar( const unsigned char* p_input_buf, size_t& len, unsigned char*& p_output_buf )
{
   unsigned char* p_buf = p_output_buf;
   wchar_t v( *( wchar_t* )p_input_buf );

   len = c_min_wchar_length;

   const int buf_size = ( sizeof( wchar_t ) * CHAR_BIT + 2 ) / 3 + 1 + c_min_wchar_length;
   p_buf[ buf_size - 1 ] = c_variable_length_terminator;

   size_t pos = buf_size - 2;
   while( true )
   {
      p_buf[ pos ] = '0' + ( unsigned char )( v % 10 );
      v /= 10;

      if( v == 0 )
         break;

       ++len;
       --pos;
   }

   p_output_buf = &p_buf[ pos ];
}

void write_convert_type_short( const unsigned char* p_input_buf, size_t& len, unsigned char*& p_output_buf )
{
   unsigned char* p_buf = p_output_buf;
   short v( *( const short* )p_input_buf );

   len = c_min_short_length;

   const int buf_size = numeric_limits< short >::digits10 + c_min_short_length;
   p_buf[ buf_size - 1 ] = c_variable_length_terminator;

   char sign;
   if( v < 0 )
   {
      v *= -1;
      sign = c_negative_sign;
   }
   else
      sign = c_positive_sign;

   size_t pos = buf_size - 2;
   while( true )
   {
      p_buf[ pos ] = '0' + ( unsigned char )( v % 10 );
      v /= 10;

      if( v == 0 )
         break;

      ++len;
      --pos;
   }

   p_buf[ --pos ] = sign;
   p_output_buf = &p_buf[ pos ];
}

void write_convert_type_ushort( const unsigned char* p_input_buf, size_t& len, unsigned char*& p_output_buf )
{
   unsigned char* p_buf = p_output_buf;
   unsigned short v( *( const unsigned short* )p_input_buf );

   len = c_min_ushort_length;

   const int buf_size = numeric_limits< unsigned short >::digits10 + c_min_ushort_length;
   p_buf[ buf_size - 1 ] = c_variable_length_terminator;

   size_t pos = buf_size - 2;
   while( true )
   {
      p_buf[ pos ] = '0' + ( unsigned char )( v % 10 );
      v /= 10;

      if( v == 0 )
         break;

      ++len;
      --pos;
   }

   p_output_buf = &p_buf[ pos ];
}

void write_convert_type_int( const unsigned char* p_input_buf, size_t& len, unsigned char*& p_output_buf )
{
   unsigned char* p_buf = p_output_buf;
   int v( *( const int* )p_input_buf );

   len = c_min_int_length;

   const int buf_size = numeric_limits< int >::digits10 + c_min_int_length;
   p_buf[ buf_size - 1 ] = c_variable_length_terminator;

   char sign;
   if( v < 0 )
   {
      v *= -1;
      sign = c_negative_sign;
   }
   else
      sign = c_positive_sign;

   size_t pos = buf_size - 2;
   while( true )
   {
      p_buf[ pos ] = '0' + ( unsigned char )( v % 10 );
      v /= 10;

      if( v == 0 )
         break;

      ++len;
      --pos;
   }

   p_buf[ --pos ] = sign;
   p_output_buf = &p_buf[ pos ];
}

void write_convert_type_uint( const unsigned char* p_input_buf, size_t& len, unsigned char*& p_output_buf )
{
   unsigned char* p_buf = p_output_buf;
   unsigned int v( *( const unsigned int* )p_input_buf );

   len = c_min_uint_length;

   const int buf_size = numeric_limits< unsigned int >::digits10 + c_min_uint_length;
   p_buf[ buf_size - 1 ] = c_variable_length_terminator;

   size_t pos = buf_size - 2;
   while( true )
   {
      p_buf[ pos ] = '0' + ( unsigned char )( v % 10 );
      v /= 10;

      if( v == 0 )
         break;
      ++len;
      --pos;
   }

   p_output_buf = &p_buf[ pos ];
}

void write_convert_type_long( const unsigned char* p_input_buf, size_t& len, unsigned char*& p_output_buf )
{
   unsigned char* p_buf = p_output_buf;
   long v( *( const long* )p_input_buf );

   len = c_min_long_length;

   const int buf_size = numeric_limits< long >::digits10 + c_min_long_length;
   p_buf[ buf_size - 1 ] = c_variable_length_terminator;

   char sign;
   if( v < 0 )
   {
      v *= -1;
      sign = c_negative_sign;
   }
   else
      sign = c_positive_sign;

   size_t pos = buf_size - 2;
   while( true )
   {
      p_buf[ pos ] = '0' + ( unsigned char )( v % 10 );
      v /= 10;

      if( v == 0 )
         break;

      ++len;
      --pos;
   }

   p_buf[ --pos ] = sign;
   p_output_buf = &p_buf[ pos ];
}

void write_convert_type_ulong( const unsigned char* p_input_buf, size_t& len, unsigned char*& p_output_buf )
{
   unsigned char* p_buf = p_output_buf;
   unsigned long v( *( const unsigned long* )p_input_buf );

   len = c_min_uint_length;

   const int buf_size = numeric_limits< unsigned long >::digits10 + c_min_ulong_length;
   p_buf[ buf_size - 1 ] = c_variable_length_terminator;

   size_t pos = buf_size - 2;
   while( true )
   {
      p_buf[ pos ] = '0' + ( unsigned char )( v % 10 );
      v /= 10;

      if( v == 0 )
         break;

      ++len;
      --pos;
   }

   p_output_buf = &p_buf[ pos ];
}

void write_convert_type_long_long(
 const unsigned char* p_input_buf, size_t& len, unsigned char*& p_output_buf )
{
   unsigned char* p_buf = p_output_buf;
   long_long v( *( const long_long* )p_input_buf );

   len = c_min_long_length;

   const int buf_size = numeric_limits< long_long >::digits10 + c_min_long_long_length;
   p_buf[ buf_size - 1 ] = c_variable_length_terminator;

   char sign;
   if( v < 0 )
   {
      v *= -1;
      sign = c_negative_sign;
   }
   else
      sign = c_positive_sign;

   size_t pos = buf_size - 2;
   while( true )
   {
      p_buf[ pos ] = '0' + ( unsigned char )( v % 10 );
      v /= 10;

      if( v == 0 )
         break;
      ++len;
      --pos;
   }

   p_buf[ --pos ] = sign;
   p_output_buf = &p_buf[ pos ];
}

void write_convert_type_ulong_long(
 const unsigned char* p_input_buf, size_t& len, unsigned char*& p_output_buf )
{
   unsigned char* p_buf = p_output_buf;
   unsigned_long_long v( *( const unsigned_long_long* )p_input_buf );

   len = c_min_uint_length;

   const int buf_size = numeric_limits< unsigned_long_long >::digits10 + c_min_ulong_long_length;
   p_buf[ buf_size - 1 ] = c_variable_length_terminator;

   size_t pos = buf_size - 2;
   while( true )
   {
      p_buf[ pos ] = '0' + ( unsigned char )( v % 10 );
      v /= 10;

      if( v == 0 )
         break;

      ++len;
      --pos;
   }

   p_output_buf = &p_buf[ pos ];
}

void write_convert_type_float( const unsigned char* p_input_buf, size_t& len, unsigned char*& p_output_buf )
{
   unsigned char* p_buf = p_output_buf;
   float v( *( const float* )p_input_buf );

#ifndef NO_SUPPORT_FOR_INF_OR_NAN
   fp_class fpc = get_fp_classf( v );

   if( fpc == e_fp_class_quiet_nan )
      *p_buf++ = c_quiet_nan;
   else
#endif
   {
      len = c_min_float_length;

      if( v < 0.0f )
      {
         v *= -1.0f;
         *p_buf++ = c_negative_sign;
      }
      else
         *p_buf++ = c_positive_sign;

#ifndef NO_SUPPORT_FOR_INF_OR_NAN
      if( fpc == e_fp_class_pos_inf || fpc == e_fp_class_neg_inf )
         *p_buf++ = c_infinity;
      else
#endif
      {
         int exp = 0;

         if( v != 0.0f )
         {
            while( v > 1.0f )
            {
               v /= 16.0f;
               exp++;
            }

            while( v < 1.0f )
            {
               v *= 16.0f;
               exp--;
            }
         }

         assert( ( v >= 1.0f ) && ( v < 16.0f ) );

         if( v == 0.0f )
         {
            ++len;
            *p_buf++ = '0';
         }
         else
         {
            do
            {
               int ch = ( int )v;
               v -= ch;

               ++len;
               *p_buf++ = ascii_digit( ch );

               v *= 16.0f;
            } while( v != 0.0f );
         }

         *p_buf++ = c_exponent_prefix;

         if( exp >= 0 )
            *p_buf++ = c_positive_sign;
         else
         {
            exp *= -1;
            *p_buf++ = c_negative_sign;
         }

         while( true )
         {
            ++len;
            *p_buf++ = '0' + ( unsigned char )( exp % 10 );
            exp /= 10;

            if( exp == 0 )
               break;
         }
      }
   }

   *p_buf = c_variable_length_terminator;
}

void write_convert_type_double( const unsigned char* p_input_buf, size_t& len, unsigned char*& p_output_buf )
{
   unsigned char* p_buf = p_output_buf;
   double v( *( const double* )p_input_buf );

#ifndef NO_SUPPORT_FOR_INF_OR_NAN
   fp_class fpc = get_fp_class( v );

   if( fpc == e_fp_class_quiet_nan )
      *p_buf++ = c_quiet_nan;
   else
#endif
   {
      len = c_min_double_length;

      if( v < 0.0 )
      {
         v *= -1.0;
         *p_buf++ = c_negative_sign;
      }
      else
         *p_buf++ = c_positive_sign;

#ifndef NO_SUPPORT_FOR_INF_OR_NAN
      if( fpc == e_fp_class_pos_inf || fpc == e_fp_class_neg_inf )
         *p_buf++ = c_infinity;
      else
#endif
      {
         int exp = 0;

         if( v != 0.0 )
         {
            while( v > 1.0 )
            {
               v /= 16.0;
               exp++;
            }

            while( v < 1.0 )
            {
               v *= 16.0;
               exp--;
            }
         }

         assert( ( v >= 1.0 ) && ( v < 16.0 ) );

         if( v == 0.0 )
         {
            ++len;
            *p_buf++ = '0';
         }
         else
         {
            do
            {
               int ch = ( int )v;
               v -= ch;

               ++len;
               *p_buf++ = ascii_digit( ch );

               v *= 16.0;
            } while( v != 0.0 );
         }

         *p_buf++ = c_exponent_prefix;

         if( exp >= 0 )
            *p_buf++ = c_positive_sign;
         else
         {
            exp *= -1;
            *p_buf++ = c_negative_sign;
         }

         while( true )
         {
            ++len;
            *p_buf++ = '0' + ( unsigned char )( exp % 10 );
            exp /= 10;

            if( exp == 0 )
               break;
         }
      }
   }

   *p_buf = c_variable_length_terminator;
}

void write_convert_type_ldouble( const unsigned char* p_input_buf, size_t& len, unsigned char*& p_output_buf )
{
   unsigned char* p_buf = p_output_buf;
   long double v( *( const long double* )p_input_buf );

#ifndef NO_SUPPORT_FOR_INF_OR_NAN
   fp_class fpc = get_fp_classl( v );

   if( fpc == e_fp_class_quiet_nan )
      *p_buf++ = c_quiet_nan;
   else
#endif
   {
      len = c_min_ldouble_length;

      if( v < 0.0l )
      {
         v *= -1.0l;
         *p_buf++ = c_negative_sign;
      }
      else
         *p_buf++ = c_positive_sign;

#ifndef NO_SUPPORT_FOR_INF_OR_NAN
      if( fpc == e_fp_class_pos_inf || fpc == e_fp_class_neg_inf )
         *p_buf++ = c_infinity;
      else
#endif
      {
         int exp = 0;

         if( v != 0.0l )
         {
            while( v > 1.0l )
            {
               v /= 16.0l;
               exp++;
            }

            while( v < 1.0l )
            {
               v *= 16.0l;
               exp--;
            }
         }

         assert( ( v >= 1.0l ) && ( v < 16.0l ) );

         if( v == 0.0l )
         {
            ++len;
            *p_buf++ = '0';
         }
         else
         {
            do
            {
               int ch = ( int )v;
               v -= ch;

               ++len;
               *p_buf++ = ascii_digit( ch );

               v *= 16.0l;
            } while( v != 0.0l );
         }

         *p_buf++ = c_exponent_prefix;

         if( exp >= 0 )
            *p_buf++ = c_positive_sign;
         else
         {
            exp *= -1;
            *p_buf++ = c_negative_sign;
         }

         while( true )
         {
            ++len;
            *p_buf++ = '0' + ( unsigned char )( exp % 10 );
            exp /= 10;

            if( exp == 0 )
               break;
         }
      }
   }

   *p_buf = c_variable_length_terminator;
}

typedef void ( *fp_read_convertor_func )( read_buffer& rb, size_t len, unsigned char* p_output_buf );

fp_read_convertor_func read_convertor_functions[ ] =
{
   &read_convert_type_none,
   &read_convert_type_bool,
   &read_convert_type_char,
   &read_convert_type_schar,
   &read_convert_type_uchar,
   &read_convert_type_wchar,
   &read_convert_type_short,
   &read_convert_type_ushort,
   &read_convert_type_int,
   &read_convert_type_uint,
   &read_convert_type_long,
   &read_convert_type_ulong,
   &read_convert_type_long_long,
   &read_convert_type_ulong_long,
   &read_convert_type_float,
   &read_convert_type_double,
   &read_convert_type_ldouble
};

typedef void ( *fp_write_convertor_func )(
 const unsigned char* p_input_buf, size_t& len, unsigned char*& p_output_buf );

fp_write_convertor_func write_convertor_functions[ ] =
{
   &write_convert_type_none,
   &write_convert_type_bool,
   &write_convert_type_char,
   &write_convert_type_schar,
   &write_convert_type_uchar,
   &write_convert_type_wchar,
   &write_convert_type_short,
   &write_convert_type_ushort,
   &write_convert_type_int,
   &write_convert_type_uint,
   &write_convert_type_long,
   &write_convert_type_ulong,
   &write_convert_type_long_long,
   &write_convert_type_ulong_long,
   &write_convert_type_float,
   &write_convert_type_double,
   &write_convert_type_ldouble
};

void ptf_read_buffered_stream::read( unsigned char* p_buf, size_t len, read_write_type type )
{
   ( *read_convertor_functions[ type ] )( rb, len, p_buf );
}

void ptf_write_buffered_stream::write( const unsigned char* p_buf, size_t len, read_write_type type )
{
   unsigned char buf[ c_max_conversion_buffer_size ];
   unsigned char* p_output_buf = buf;

   ( *write_convertor_functions[ type ] )( p_buf, len, p_output_buf );

   wb.write_data( p_output_buf, len );
}

#ifdef READ_WRITE_BUFFERED_STREAM_TESTBED
#  include <vector>
#  include <string>
#  include <iostream>

namespace
{

const int c_test_buffer_size = 5;

vector< string > read_write_buffered_data;

class test_read_buffer : public read_buffer
{
   public:
   test_read_buffer( )
    : read_buffer( c_test_buffer_size )
   {
      num = 0;
      offset = 0;
      excess = 0;
   }

   void read_available( );

   private:
   size_t num;
   size_t offset;
   size_t excess;
};

void test_read_buffer::read_available( )
{
   if( num >= read_write_buffered_data.size( ) )
   {
      cerr << "attempt to read beyond end of input" << endl;
      exit( 1 );
   }

   string next( read_write_buffered_data[ num ] );
   size_t waiting = next[ 0 ] - '0';

   if( excess )
   {
      buffered = min( capacity, excess );
      memcpy( &buffer[ 0 ], &next[ 1 + offset ], buffered );
   }
   else
   {
      buffered = min( capacity, waiting );
      memcpy( &buffer[ 0 ], &next[ 1 ], buffered );
   }

   excess = waiting - buffered;

   if( excess == 0 )
   {
      ++num;
      offset = 0;
   }
   else
      offset += buffered;

   retrieved = 0;
}

class test_write_buffer : public write_buffer
{
   public:
   test_write_buffer( )
    : write_buffer( c_test_buffer_size, 1 )
   {
   }

   ~test_write_buffer( )
   {
      if( !empty( ) )
         flush_data( );
   }

   void flush_data( );
};

void test_write_buffer::flush_data( )
{
   if( buffered )
   {
      cout << "*** flushing " << ( buffered + reserved ) << " bytes...\n";

      buffer[ 0 ] = '0' + buffered;

      for( size_t i = 0; i < ( buffered + reserved ); i++ )
         cout << buffer[ i ];
      cout << endl;

      read_write_buffered_data.push_back( string( ( const char* )&buffer[ 0 ], ( buffered + reserved ) ) );

      buffered = 0;
   }
}

struct test_class
{
   test_class( char c, int i, float f )
    :
    c( c ),
    i( i ),
    f( f )
   {
   }

   char c;
   int i;
   float f;
};

ostream& operator <<( ostream& os, test_class& tc )
{
   os << tc.c << tc.i << tc.f;
   return os;
}

read_stream& operator >>( read_stream& rs, test_class& tc )
{
   rs >> tc.c >> tc.i >> tc.f;
   return rs;
}

write_stream& operator <<( write_stream& ws, const test_class& tc )
{
   ws << tc.c << tc.i << tc.f;
   return ws;
}

}

int main( )
{
   test_write_buffer test_writer;
   ptf_write_buffered_stream test_ptf_write_buffered_stream( test_writer );

   test_class test_object( 'A', -1, 3.14159f );

   test_ptf_write_buffered_stream << test_object;
   test_ptf_write_buffered_stream.flush( );

   test_read_buffer test_read_buffer;
   ptf_read_buffered_stream test_ptf_read_buffered_stream( test_read_buffer );

   test_ptf_read_buffered_stream >> test_object;
   cout << test_object << endl;
}

#endif

