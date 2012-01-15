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

#ifndef STORABLE_FILE_H
#  define STORABLE_FILE_H

#  include "ods.h"
#  include "macros.h"
#  include "utilities.h"
#  include "file_buffer.h"

#  ifdef CIYAM_BASE_IMPL
#     define FILE_DECL_SPEC DYNAMIC_EXPORT
#  else
#     define FILE_DECL_SPEC DYNAMIC_IMPORT
#  endif

class read_stream;
class write_stream;

class FILE_DECL_SPEC storable_file : public file_buffer, public storable_base
{
   friend int_t size_of( const storable_file& sf );
   friend read_stream& operator >>( read_stream& rs, storable_file& sf );
   friend write_stream& operator <<( write_stream& ws, const storable_file& sf );

   public:
   storable_file( ) { }
   storable_file( const char* p_file_name ) : file_buffer( p_file_name ) { }

   storable_file( const storable_file& src ) : file_buffer( src ) { }

   storable_file& operator =( const storable_file& src )
   {
      file_buffer::operator =( src );
      storable_base::operator =( src );

      return *this;
   }

   int_t get_size_of( ) const;
   void get_instance( read_stream& rs );
   void put_instance( write_stream& ws ) const;
};

#endif

