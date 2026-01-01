// Copyright (c) 2005-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef CLASS_UTILITIES_H
#  define CLASS_UTILITIES_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#  endif

#  include "utilities.h"
#  include "ciyam_macros.h"
#  include "command_handler.h"

inline bool is_null( const std::string& s ) { return s.empty( ); }

template< > inline bool from_string< bool >( const std::string& s )
{
   if( s.empty( ) || ( s[ 0 ] == '0' || s[ 0 ] == 'f' || s[ 0 ] == 'F' ) )
      return false;
   else
      return true;
}

template< typename T > inline std::string to_formatted_string( const T& t, const char* p_fmt )
{
   ( void )p_fmt;

   return to_string( t );
}

// NOTE: It is being assumed that a type cannot be null unless it has its own "is_null" function
// (the function is not being inlined for BCB due to noisy warnings that it will otherwise issue)
// and also any string value is valid unless it has provided its own "is_valid_str_val" function.
template< typename T > inline bool is_null( const T& ) { return false; }
template< typename T > inline bool is_valid_str_val( const std::string& ) { return true; }

template< > inline bool is_valid_str_val< int >( const std::string& s ) { return is_valid_int( s ); }
template< > inline bool is_valid_str_val< bool >( const std::string& s ) { return is_valid_bool( s ); }

inline std::string to_string( int num, int min_digits )
{
   std::string s( to_string( num ) );

   if( s.length( ) < min_digits )
      s = std::string( min_digits - s.length( ), '0' ) + s;

   return s;
}

template< typename T > inline void append_value( std::string& str, const T& value, char separator = '\n' )
{
   if( !str.empty( ) )
      str += separator;
   str += to_string( value );
}

template< typename T > inline void prepend_value( std::string& str, const T& value, char separator = '\n' )
{
   std::string val( to_string( value ) );
   if( !str.empty( ) )
      val += separator;
   str = val + str;
}

inline const std::string& get_parm_val_from_string( const parameter_info& parameters, const std::string& parameter_name )
{
   return get_parm_val( parameters, parameter_name );
}

template< typename T > inline T get_parm_val_from_string( const parameter_info& parameters, const std::string& parameter_name )
{
   return from_string< T >( get_parm_val( parameters, parameter_name ) );
}

inline void string_getter( const std::string& val, std::string& dest )
{
   dest = val;
}

template< typename T > inline void string_getter( const T& val, std::string& dest )
{
   dest = to_string( val );
}

inline void string_setter( std::string& ref, const std::string& value )
{
   ref = value;
}

template< typename T > inline void string_setter( T& ref, const std::string& value )
{
   ref = from_string< T >( value );
}

template< class C, typename T >
 inline void func_string_setter( C& object, T& ( C::* ref_setter )( ), const std::string& value )
{
   from_string( ( object.*ref_setter )( ), value );
}

template< class C, typename T >
 inline void func_string_setter( C& object, void ( C::* val_setter )( T ), const std::string& value )
{
   ( object.*val_setter )( from_string< T >( value ) );
}

template< class C, typename T >
 inline void func_string_setter( C& object, void ( C::* cval_setter )( const T& ), const std::string& value )
{
   ( object.*cval_setter )( from_string< T >( value ) );
}

#endif
