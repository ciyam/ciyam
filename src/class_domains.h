// Copyright (c) 2006-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef CLASS_DOMAINS_H
#  define CLASS_DOMAINS_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <set>
#     include <string>
#  endif

#  include "numeric.h"
#  include "date_time.h"
#  include "ciyam_base.h"
#  include "class_base.h"

#  ifdef __GNUG__
// KLUDGE: For some reason g++ is having troubles recognising the declarations in "class_base.h".
// Also it requires that the module strings are visible (need to check whether this is standard).
std::string formatted_int( int n, const std::string& mask );
std::string formatted_numeric( const numeric& n, const std::string& mask );
#     include "module_strings.h"
#  endif

template< typename T > std::string to_string( const T& t );

template< int N, int X > struct domain_int_range
{
   bool is_valid( int v, std::string& error ) const
   {
      if( v >= N && v <= X )
         return true;
      else
      {
         error = error_message( error );
         return false;
      }
   }

   std::string error_message( const std::string& mask ) const;
};

template< int N, int X > std::string domain_int_range< N, X >::error_message( const std::string& mask ) const
{
   std::string min( mask.empty( ) ? to_string( N ) : formatted_int( N, mask ) );

   std::string max( mask.empty( ) ? to_string( X ) : formatted_int( X, mask ) );

   return get_string_message(
    GS( c_str_value_must_be_in_range ),
    make_pair( c_str_value_must_be_in_range_from, min ),
    make_pair( c_str_value_must_be_in_range_to, max ) );
}

template< int Y1, int M1, int D1, int Y2, int M2, int D2 > struct domain_date_range
{
   domain_date_range( )
    :
    min( Y1, ( month )M1, D1 ),
    max( Y2, ( month )M2, D2 )
   {
   }

   bool is_valid( const udate& ud, std::string& error ) const
   {
      bool valid = true;

      if( ud < min || ud > max )
      {
         valid = false;
         error = error_message( );
      }

      return valid;
   }

   udate min;
   udate max;

   std::string error_message( ) const;
};

template< int Y1, int M1, int D1, int Y2, int M2, int D2 >
 std::string domain_date_range< Y1, M1, D1, Y2, M2, D2 >::error_message( ) const
{
   return get_string_message( GS( c_str_value_must_be_in_range ),
    make_pair( c_str_value_must_be_in_range_from, min.as_string( true ) ),
    make_pair( c_str_value_must_be_in_range_to, max.as_string( true ) ) );
}

template< int Y1, int M1, int D1, int H1, int N1, int S1, int Y2, int M2, int D2, int H2, int N2, int S2 > struct domain_datetime_range
{
   domain_datetime_range( )
    :
    min( Y1, ( month )M1, D1, H1, N1, ( second )S1 ),
    max( Y2, ( month )M2, D2, H2, N2, ( second )S2 )
   {
   }

   bool is_valid( const date_time& dtm, std::string& error ) const
   {
      bool valid = true;

      if( dtm < min || dtm > max )
      {
         valid = false;
         error = error_message( );
      }

      return valid;
   }

   date_time min;
   date_time max;

   std::string error_message( ) const;
};

template< int Y1, int M1, int D1, int H1, int N1, int S1, int Y2, int M2, int D2, int H2, int N2, int S2 >
 std::string domain_datetime_range< Y1, M1, D1, H1, N1, S1, Y2, M2, D2, H2, N2, S2 >::error_message( ) const
{
   return get_string_message( GS( c_str_value_must_be_in_range ),
    make_pair( c_str_value_must_be_in_range_from, min.as_string( true, false ) ),
    make_pair( c_str_value_must_be_in_range_to, max.as_string( true, false ) ) );
}

template< int I, int D = 10 > struct divisor
{
   static const int result = divisor< I / D, D >::result * D;
};

template< int D > struct divisor< 0, D >
{
   static const int result = 1;
};

template< typename N, int64_t W1, int64_t F1, int64_t W2, int64_t F2 > struct domain_numeric_range
{
   bool is_valid( const N& n, std::string& error ) const;

   std::string error_message( const std::string& mask ) const;
};

template< typename N, int64_t W1, int64_t F1, int64_t W2, int64_t F2 >
 bool domain_numeric_range< N, W1, F1, W2, F2 >::is_valid( const N& n, std::string& error ) const
{
   N n1( F1 );
   n1 /= divisor< F1 >::result;

   if( W1 < 0 )
      n1 *= -1.0;
   n1 += W1;

   N n2( F2 );
   n2 /= divisor< F2 >::result;

   if( W2 < 0 )
      n2 *= -1.0;
   n2 += W2;

   if( n >= n1 && n <= n2 )
      return true;
   else
   {
      error = error_message( error );
      return false;
   }
}

template< typename N, int64_t W1, int64_t F1, int64_t W2, int64_t F2 >
 std::string domain_numeric_range< N, W1, F1, W2, F2 >::error_message( const std::string& mask ) const
{
   std::string min( mask.empty( ) ? to_string( W1 ) + "." + to_string( F1 )
    : formatted_numeric( numeric( ( to_string( W1 ) + "." + to_string( F1 ) ).c_str( ) ), mask ) );

   std::string max( mask.empty( ) ? to_string( W2 ) + "." + to_string( F2 )
    : formatted_numeric( numeric( ( to_string( W2 ) + "." + to_string( F2 ) ).c_str( ) ), mask ) );

   return get_string_message(
    GS( c_str_value_must_be_in_range ),
    make_pair( c_str_value_must_be_in_range_from, min ),
    make_pair( c_str_value_must_be_in_range_to, max ) );
}

template< typename N, int S, int P, int F = 0 > struct domain_numeric_format
{
   bool is_valid( const N& n, std::string& error ) const;

   std::string error_message( ) const;

   std::string error_message_2( ) const;
   std::string error_message_4( ) const;
};

template< typename N, int S, int P, int F >
 bool domain_numeric_format< N, S, P, F >::is_valid( const N& n, std::string& error ) const
{
   bool valid = true;

   if( n.whole_digits( ) > S - P
    || n.decimal_digits( ) > P )
      valid = false;

   if( !valid )
      error = error_message( );
   else if( F > 0 )
   {
      double frac = n.frac( );

      if( F == 2 )
      {
         if( frac != 0.0 && frac != 0.5 )
         {
            valid = false;
            error = error_message_2( );
         }
      }
      else if( F == 4 )
      {
         if( frac != 0.0 && frac != 0.5 && frac != 0.25 && frac != 0.75 )
         {
            valid = false;
            error = error_message_4( );
         }
      }
      else
         error = "unexpected numeric fraction #" + to_string( F );
   }

   return valid;
}

template< typename N, int S, int P, int F > std::string domain_numeric_format< N, S, P, F >::error_message( ) const
{
   return get_string_message( GS( c_str_incorrect_numeric_format ),
    make_pair( c_str_incorrect_numeric_format_wholes, to_string( S - P ) ),
    make_pair( c_str_incorrect_numeric_format_decimals, to_string( P ) ) );
}

template< typename N, int S, int P, int F > std::string domain_numeric_format< N, S, P, F >::error_message_2( ) const
{
   return GS( c_str_incorrect_numeric_fraction_2 );
}

template< typename N, int S, int P, int F > std::string domain_numeric_format< N, S, P, F >::error_message_4( ) const
{
   return GS( c_str_incorrect_numeric_fraction_4 );
}

template< int M > struct domain_string_max_size
{
   bool is_valid( const std::string& s, std::string& error ) const
   {
      if( s.length( ) <= M )
         return true;
      else
      {
         error = error_message( );
         return false;
      }
   }

   static int max_size( ) { return M; }

   std::string error_message( ) const;
};

template< int M > std::string domain_string_max_size< M >::error_message( ) const
{
   return get_string_message( GS( c_str_exceeded_maximum ),
    make_pair( c_str_exceeded_maximum_limit, to_string( M ) ) );
}

inline bool label_character( char c )
{
   if( c == '_' || ( c >= 'a' && c <= 'z' ) || ( c >= 'A' && c <= 'Z' ) || ( c >= '0' && c <= '9' ) )
      return true;
   else
      return false;
}

struct domain_string_label_format
{
   bool is_valid( const std::string& s, std::string& error ) const;

   std::string error_message( ) const;
};

inline bool domain_string_label_format::is_valid( const std::string& s, std::string& error ) const
{
   bool valid = true;

   for( size_t i = 0; i < s.size( ); i++ )
   {
      if( !label_character( s[ i ] ) )
      {
         valid = false;
         break;
      }
   }

   if( !valid )
      error = error_message( );

   return valid;
}

inline std::string domain_string_label_format::error_message( ) const
{
   return GS( c_str_invalid_label );
}

inline bool filename_character( char c )
{
   if( ( c >= ' ' && c <= '~' )
    && c != '"' && c != ':' && c != '?' && c!= '*' && c != '<' && c != '>' && c != '|' && c != '/' && c != '\\' )
      return true;
   else
      return false;
}

struct domain_string_filename_format
{
   bool is_valid( const std::string& s, std::string& error ) const;

   std::string error_message( ) const;
};

inline bool domain_string_filename_format::is_valid( const std::string& s, std::string& error ) const
{
   bool valid = true;

   for( size_t i = 0; i < s.size( ); i++ )
   {
      if( !filename_character( s[ i ] ) )
      {
         valid = false;
         break;
      }
   }

   if( !valid )
      error = error_message( );

   return valid;
}

inline std::string domain_string_filename_format::error_message( ) const
{
   return GS( c_str_invalid_filename );
}

struct domain_string_identifier_format
{
   bool is_valid( const std::string& s, std::string& error ) const;

   std::string error_message( ) const;
};

inline bool domain_string_identifier_format::is_valid( const std::string& s, std::string& error ) const
{
   bool valid = true;

   if( s.empty( ) || !( ( s[ 0 ] >= 'a' && s[ 0 ] <= 'z' ) || ( s[ 0 ] >= 'A' && s[ 0 ] <= 'Z' ) ) )
      valid = false;
   else
   {
      for( size_t i = 1; i < s.size( ); i++ )
      {
         if( !label_character( s[ i ] ) )
         {
            valid = false;
            break;
         }
      }
   }

   if( !valid )
      error = error_message( );

   return valid;
}

inline std::string domain_string_identifier_format::error_message( ) const
{
   return GS( c_str_invalid_identifier );
}

template< typename T > struct dummy_domain
{
   bool is_valid( const T& t, std::string& ) const { return true; }
};

template< typename T,
 typename D1, typename D2, typename D3 = dummy_domain< T >, typename D4 = dummy_domain< T > > struct aggregate_domain
{
   bool is_valid( const T& t, std::string& error ) const
   {
      return d1.is_valid( t, error ) && d2.is_valid( t, error ) && d3.is_valid( t, error ) && d4.is_valid( t, error );
   }

   D1 d1;
   D2 d2;
   D3 d3;
   D4 d4;
};

#endif
