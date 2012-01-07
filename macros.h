// Copyright (c) 2003
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

#ifndef MACROS_H
#  define MACROS_H

#  define ARG_USED( x ) ( void )( x )

#  define STRING_IT( x ) #x
#  define STRINGIZE( x ) STRING_IT( x )

#  define CONCAT_IT( x, y ) x##y
#  define CONCATIZE( x, y ) CONCAT_IT( x, y )

#  define ARRAY_SIZE( array ) ( sizeof( array ) / sizeof( array[ 0 ] ) )
#  define ARRAY_PTR_AND_SIZE( array ) array, sizeof( array ) / sizeof( array[ 0 ] )

#  define DEBUG_PRINT( x ) std::cout << #x << " => [" << ( x ) << "]" << std::endl

#  define MEMBER_SIZEOF( class_name, member_name ) ( sizeof( reinterpret_cast< class_name* >( 0 )->member_name ) )

#  define APPEND_LINE_NUMBER( text ) CONCATIZE( text, __LINE__ )

#  ifdef __BORLANDC__
#     if( __BORLANDC__ < 0x0560 )
#        define OLD_BORLAND_VERSION
#     else
#        define NEW_BORLAND_VERSION
#     endif
#  endif

#  ifdef _WIN32
#     define DYNAMIC_EXPORT __declspec( dllexport )
#     define DYNAMIC_IMPORT __declspec( dllimport )
#     define DYNAMIC_LIBRARY HINSTANCE
#  else
#     define DYNAMIC_EXPORT
#     define DYNAMIC_IMPORT
#     define DYNAMIC_LIBRARY void*
#  endif

#endif

