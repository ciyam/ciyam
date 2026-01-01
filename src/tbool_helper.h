// Copyright (c) 2006-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

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

inline std::string to_string( const tbool& tb )
{
   std::string s;
   if( tb.is_null( ) )
      s = "-1";
   else
      s = to_string( *tb );

   return s;
}

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
