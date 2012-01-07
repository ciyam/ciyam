// Copyright (c) 2004
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

#ifdef __BORLANDC__
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

   auto_ptr< char > ap( new char[ size + 1 ] );

   fseek( fp, 0, SEEK_SET );
   if( fread( ap.get( ), 1, ( size_t )size, fp ) != ( size_t )size )
      throw runtime_error( "reading from input file '" + file_name + "'" );

   fclose( fp );
   lines.reserve( size / 10 );

   char* tmp = ap.get( );
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

   p = ap.release( );
}

