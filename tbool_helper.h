// Copyright (c) 2006
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

#ifndef TBOOL_HELPER_H
#  define TBOOL_HELPER_H

#  include "nullable.h"
#  include "read_write_stream.h"

typedef nullable< bool > nullable_bool;

class tbool : public nullable_bool
{
   public:
   tbool( ) { }

   tbool( bool b ) : nullable_bool( b ) { }

   tbool& operator =( bool b )
   {
      nullable_bool::operator =( b );
      return *this;
   }
};

inline bool is_null( const tbool& tb ) { return tb.is_null( ); }

inline size_t size_determiner( const tbool* ) { return sizeof( char ); }

#  ifdef NEW_BORLAND_VERSION
#     pragma option push -w-8027
#  endif
inline std::string to_string( const tbool& tb )
{
   std::string s;
   if( tb.is_null( ) )
      s = "-1";
   else
      s = to_string( *tb );

   return s;
}
#  ifdef NEW_BORLAND_VERSION
#     pragma option pop
#  endif

template< > inline tbool from_string< tbool >( const std::string& s )
{
   tbool tb;
   if( s != "-1" )
      tb = from_string< bool >( s );

   return tb;
}

inline bool operator <( const tbool& lhs, bool rhs )
{
   if( lhs.is_null( ) )
      return false;

   return static_cast< const nullable_bool& >( lhs ) < rhs;
}

inline bool operator <( bool lhs, const tbool& rhs )
{
   if( rhs.is_null( ) )
      return false;

   return lhs < static_cast< const nullable_bool& >( rhs );
}

inline bool operator <( const tbool& lhs, const tbool& rhs )
{
   if( lhs.is_null( ) && !rhs.is_null( ) )
      return true;

   if( rhs.is_null( ) )
      return false;

   return static_cast< const nullable_bool& >( lhs ) < static_cast< const nullable_bool& >( rhs );
}

inline read_stream& operator >>( read_stream& rs, tbool& tb )
{
   char b;

   rs >> b;

   if( b < 0 )
      tb.set_null( );
   else
      tb = ( bool )b;

   return rs;
}

inline write_stream& operator <<( write_stream& ws, const tbool& tb )
{
   char b = -1;

   if( !tb.is_null( ) )
      b = ( char )*tb;

   ws << b;

   return ws;
}

#endif

