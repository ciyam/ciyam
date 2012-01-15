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

#ifndef CLASS_UTILITIES_H
#  define CLASS_UTILITIES_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#  endif

#  include "utilities.h"
#  include "ciyam_macros.h"
#  include "command_handler.h"

inline bool is_null( const std::string& s ) { return s.empty( ); }

// NOTE: It is being assumed that a type cannot be null unless it has its own "is_null" function
// (the function is not being inlined for BCB due to noisy warnings that it will otherwise issue).
#  ifdef __BORLANDC__
template< typename T > bool is_null( const T& ) { return false; }
#  else
template< typename T > inline bool is_null( const T& ) { return false; }
#  endif

template< typename T > inline std::string to_comparable_string( const T& t )
{
   return to_string( t );
}

template< > inline std::string to_comparable_string( const int& i )
{
   return int_to_comparable_string( i );
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

#  ifdef __BORLANDC__
#     define USE_STRING_SETTER_CLASS
#  endif

// NOTE: For some reason BCB cannot handle the member function signature overloads when using
// template functions so a template class with overloaded constructors is being used instead.
#  ifdef USE_STRING_SETTER_CLASS
template< class C, typename T > class func_string_setter
{
   public:
   func_string_setter( C& object, T& ( C::* ref_setter )( ), const std::string& value )
   {
      from_string( ( object.*ref_setter )( ), value );
   }

   func_string_setter( C& object, void ( C::* val_setter )( T ), const std::string& value )
   {
      ( object.*val_setter )( from_string< T >( value ) );
   }

   func_string_setter( C& object, void ( C::* cval_setter )( const T& ), const std::string& value )
   {
      ( object.*cval_setter )( from_string< T >( value ) );
   }
};
#  else
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
#  endif

#endif

