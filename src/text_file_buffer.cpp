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
#  include <cstdio>
#  include <memory>
#  include <iostream>
#  include <stdexcept>
#endif

#include "text_file_buffer.h"

#define IGNORE_CR_BEFORE_LF

using namespace std;

ostream& operator <<( ostream& outf, const line& p )
{
   outf << p.p;
   return outf;
}

text_file_buffer::text_file_buffer( const string& file_name )
 :
 file_name( file_name )
{
   FILE* fp = fopen( file_name.c_str( ), "rb" );

   if( !fp )
      throw runtime_error( "unable to open file '" + file_name + "' for input" );

   fseek( fp, 0, SEEK_END );
   long size = ftell( fp );

   unique_ptr< char > up( new char[ size + 1 ] );

   fseek( fp, 0, SEEK_SET );
   if( fread( up.get( ), 1, ( size_t )size, fp ) != ( size_t )size )
      throw runtime_error( "reading from input file '" + file_name + "'" );

   fclose( fp );
   lines.reserve( size / 10 );

   char* tmp = up.get( );
   char* last = tmp;

   tmp[ size ] = '\0';

   for( int i = 0; i < size; i++ )
   {
      if( tmp[ i ] == '\n' )
      {
#ifdef IGNORE_CR_BEFORE_LF
         if( i == 0 || tmp[ i - 1 ] != '\r' )
            tmp[ i ] = '\0';
         else
            tmp[ i - 1 ] = '\0';
#else
         tmp[ i ] = '\0';
#endif
         lines.push_back( line( last ) );
#ifdef _RWSTD_VER
         if( lines.size( ) == lines.capacity( ) )
            lines.reserve( lines.capacity( ) * 2 );
#endif
         last = &tmp[ i + 1 ];
      }
   }

   p = up.release( );
}

