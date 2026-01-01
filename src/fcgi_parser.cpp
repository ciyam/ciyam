// Copyright (c) 2011-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <cstdlib>
#  include <utility>
#  include <algorithm>
#  include <stdexcept>
#endif

#include "fcgi_parser.h"

#include "utilities.h"

using namespace std;

namespace
{

const char* find_next( const char* p_from, const char* p_to, char separator )
{
   bool is_escaped = false;

   const char* p_next( p_from );
   const char* p_found = 0;

   while( true )
   {
      if( is_escaped )
         is_escaped = false;
      else
      {
         if( *p_next == '\\' )
            is_escaped = true;
         else if( *p_next == separator )
         {
            p_found = p_next;
            break;
         }
      }

      if( ++p_next == p_to )
      {
         p_found = p_to;
         break;
      }
   }

   return p_found;
}

}

void parse_input( char* buf, unsigned int len,
 map< string, string >& input_data, char separator, bool keep_existing )
{
   if( !keep_existing )
      input_data.clear( );

   size_t pos = 0;
   const char* p_c;
   bool done = ( len == 0 );

   while( !done )
   {
      p_c = find_next( &buf[ pos ], &buf[ len ], separator );

      if( p_c == &buf[ len ] )
         done = true;

      size_t tlen = p_c - &buf[ pos ];
      p_c = find_next( &buf[ pos ], &buf[ pos + tlen ], '=' );
      if( p_c == &buf[ pos + tlen ] )
         throw runtime_error( "unexpected format for FCGI input '" + string( &buf[ pos ], tlen ) + "'" );

      size_t klen = p_c - &buf[ pos ];
      size_t dlen = tlen - klen - 1;

      size_t offset = 0;
      if( dlen >= 3 )
      {
         for( size_t i = 0; i <= dlen - offset; i++ )
         {
            if( offset )
               buf[ pos + klen + i ] = buf[ pos + klen + i + offset ];

            if( buf[ pos + klen + i ] == '%' )
            {
               int val = hex_nibble( tolower( buf[ pos + klen + i + 1 + offset ] ) );

               val <<= 4;
               val += hex_nibble( tolower( buf[ pos + klen + i + 2 + offset ] ) );

               buf[ pos + klen + i ] = ( char )val;
               offset += 2;
            }
            else if( buf[ pos + klen + i ] == '+' )
               buf[ pos + klen + i ] = ' ';
         }
      }

      string key( string( &buf[ pos ], klen ) );

      while( !key.empty( ) && key[ 0 ] == ' ' )
         key.erase( 0, 1 );

      if( input_data.count( key ) )
         input_data.erase( key );

      input_data.insert( make_pair( key, string( p_c + 1, dlen - offset ) ) );

      pos += ( tlen + 1 );
   }
}

