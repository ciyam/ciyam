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

#ifdef __BORLANDC__
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <fstream>
#  include <iostream>
#  include <stdexcept>
#endif

#include "file_buffer.h"

#include "macros.h"

using namespace std;

file_buffer::file_buffer( const char* p_file_name )
{
   load( p_file_name );
}

file_buffer::file_buffer( const file_buffer& src )
 :
 name( src.name ),
 data( src.data )
{
}

file_buffer& file_buffer::operator =( const file_buffer& src )
{
   name = src.name;
   data = src.data;
   return *this;
}

void file_buffer::load( const char* p_file_name )
{
   ifstream inpf( p_file_name, ios::in | ios::binary );
   if( !inpf )
      throw runtime_error( "unable to open file '" + string( p_file_name ) + "' for input" );

   inpf.seekg( 0, ios::end );
   ifstream::pos_type size = inpf.tellg( );
   inpf.seekg( 0, ios::beg );

   data.resize( size );
   inpf.read( ( char* )&data[ 0 ], size );

   name = string( p_file_name );
#ifdef _WIN32
   string::size_type pos = name.find_last_of( '\\' );
   if( pos == string::npos )
      pos = name.find( ':' );
#else
   string::size_type pos = name.find_last_of( '/' );
#endif

   if( pos != string::npos )
      name.erase( 0, pos + 1 );
}

void file_buffer::save( const char* p_output_path ) const
{
   if( name.empty( ) )
      throw runtime_error( "file name is required for save" );

   string file_name( p_output_path );
   if( !file_name.empty( ) )
      file_name += '/';
   file_name += name;

   ofstream outf( file_name.c_str( ), ios::out | ios::binary );
   if( !outf )
      throw runtime_error( "unable to open file '" + file_name + "' for output" );

   if( !data.empty( ) )
      outf.write( ( const char* )&data[ 0 ], data.size( ) );

   outf.flush( );
   if( !outf.good( ) )
      throw runtime_error( "unexpected bad output stream for file '" + file_name + "'" );
}

