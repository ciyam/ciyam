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

