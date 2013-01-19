// Copyright (c) 2005-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2013 CIYAM Open Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

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

