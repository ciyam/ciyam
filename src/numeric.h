// Copyright (c) 2004-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef NUMERIC_H
#  define NUMERIC_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#     include <iosfwd>
#  endif

#  include "ptypes.h"

const char c_decimal = '.';
const char c_separator = ',';

class read_stream;
class write_stream;

class numeric
{
   typedef void ( numeric::*bool_type )( ) const;
   void dummy_function_for_bool_type( ) const { }

   public:
   enum
   {
      e_max_digits = 18,
      e_default_decimals = 5
   };

   enum round_method
   {
      e_round_method_none,
      e_round_method_up,
      e_round_method_down,
      e_round_method_normal,
      e_round_method_bankers,
      e_round_method_truncate = e_round_method_down
   };

   numeric( )
    :
    decimals( 0 ),
    mantissa( 0 )
   {
   }

   numeric( int i );

   numeric( int64_t i );
   inline numeric( uint64_t ui ) : decimals( 0 ), mantissa( ui ) { }

   numeric( double d, int max_decimals = e_default_decimals,
    round_method method = e_round_method_normal, bool simplify = false );

   explicit numeric( const char* p, char dec = c_decimal, char sep = c_separator );

   numeric& round( int num_decimals = e_default_decimals,
    round_method method = e_round_method_normal, bool simplify = false );

   double frac( ) const;
   int64_t trunc( ) const;

   operator bool_type( ) const
   {
      if( !mantissa )
         return 0;
      else
         return &numeric::dummy_function_for_bool_type;
   }

   template< typename T > bool operator ==( const T& rhs ) const
   {
      return *this == numeric( rhs );
   }

   template< typename T > bool operator !=( const T& rhs ) const
   {
      return *this != numeric( rhs );
   }

   numeric& operator ++( );
   numeric operator ++( int );

   numeric& operator --( );
   numeric operator --( int );

   numeric& operator +=( const numeric& n );
   numeric& operator -=( const numeric& n );
   numeric& operator *=( numeric n );
   numeric& operator /=( numeric n );

   std::string as_string( ) const;
   double as_double( ) const;

   int64_t as_int64_t( ) const;
   uint64_t as_uint64( ) const;

   std::string as_comparable_string( ) const;

   uint8_t digits( ) const;
   uint8_t whole_digits( ) const;

   inline uint8_t decimal_digits( ) const { return digits( ) - whole_digits( ); }

   static numeric max( );
   static numeric min( );

   static int max_digits( ) { return e_max_digits; }

   static numeric e( );
   static numeric pi( );
   static numeric phi( );

   friend void perform_add_or_subtract( numeric& n1, numeric n2, bool is_add );

   friend numeric operator +( const numeric& lhs, const numeric& rhs );
   friend numeric operator -( const numeric& lhs, const numeric& rhs );
   friend numeric operator *( const numeric& lhs, const numeric& rhs );
   friend numeric operator /( const numeric& lhs, const numeric& rhs );

   friend bool operator <( const numeric& lhs, const numeric& rhs );
   friend bool operator >( const numeric& lhs, const numeric& rhs );
   friend bool operator <=( const numeric& lhs, const numeric& rhs );
   friend bool operator >=( const numeric& lhs, const numeric& rhs );
   friend bool operator !=( const numeric& lhs, const numeric& rhs );
   friend bool operator ==( const numeric& lhs, const numeric& rhs );

   friend std::ostream& operator <<( std::ostream& os, const numeric& f );

   friend read_stream& operator >>( read_stream& rs, numeric& n );
   friend write_stream& operator <<( write_stream& ws, const numeric& n );

   private:
   uint8_t decimals;
   uint64_t mantissa;

   void output_to_char_buffer( char* p_buf ) const;

   friend void level_decimals( numeric& n1, numeric& n2 );
};

inline numeric& numeric::operator ++( )
{
   return operator +=( numeric( 1 ) );
}

inline numeric numeric::operator ++( int )
{
   numeric n( *this );

   ++*this;
   return n;
}

inline numeric& numeric::operator --( )
{
   return operator -=( numeric( 1 ) );
}

inline numeric numeric::operator --( int )
{
   numeric n( *this );

   --*this;
   return n;
}

inline numeric& numeric::operator +=( const numeric& n )
{
   perform_add_or_subtract( *this, n, true );

   return *this;
}

inline numeric& numeric::operator -=( const numeric& n )
{
   perform_add_or_subtract( *this, n, false );

   return *this;
}

inline numeric operator +( const numeric& lhs, const numeric& rhs )
{
   numeric n( lhs );
   n += rhs;
   return n;
}

inline numeric operator -( const numeric& lhs, const numeric& rhs )
{
   numeric n( lhs );
   n -= rhs;
   return n;
}

inline numeric operator *( const numeric& lhs, const numeric& rhs )
{
   numeric n( lhs );
   n *= rhs;
   return n;
}

inline numeric operator /( const numeric& lhs, const numeric& rhs )
{
   numeric n( lhs );
   n /= rhs;
   return n;
}

inline bool operator <=( const numeric& lhs, const numeric& rhs )
{
   return lhs == rhs ? true : lhs < rhs;
}

inline bool operator >=( const numeric& lhs, const numeric& rhs )
{
   return lhs == rhs ? true : lhs > rhs;
}

inline bool operator !=( const numeric& lhs, const numeric& rhs ) { return !( lhs == rhs ); }

numeric abs( const numeric& n );
numeric sqrt( const numeric& n );

std::string format_numeric( const numeric& n, const std::string& mask,
 const char* p_decimal_point = 0, const char* p_plus = 0, const char* p_minus = 0,
 const char* p_group_separator = 0, const char* p_plus_left = 0, const char* p_plus_right = 0,
 const char* p_minus_left = 0, const char* p_minus_right = 0, const char* p_currency_symbol = 0 );

std::string format_percentage(
 size_t& fracs, unsigned long& prior, unsigned long count, unsigned long total, const char suffix = '%' );

#endif
