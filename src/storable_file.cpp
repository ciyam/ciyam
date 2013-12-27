// Copyright (c) 2005-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2013 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <utility>
#endif

#define CIYAM_BASE_IMPL

#include "storable_file.h"

#include "ods.h"
#include "read_write_stream.h"

using namespace std;

int_t storable_file::get_size_of( ) const
{
   return size_of( *this );
}

void storable_file::get_instance( read_stream& rs )
{
   rs >> *this;
}

void storable_file::put_instance( write_stream& ws ) const
{
   ws << *this;
}

int_t size_of( const storable_file& sf )
{
   return sizeof( size_t ) + sf.name.size( ) + sizeof( size_t ) + sf.data.size( );
}

read_stream& operator >>( read_stream& rs, storable_file& sf )
{
   rs >> sf.name;
   rs >> sf.data;
   return rs;
}

write_stream& operator <<( write_stream& ws, const storable_file& sf )
{
   ws << sf.name;
   ws << sf.data;
   return ws;
}

