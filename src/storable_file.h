// Copyright (c) 2005-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2014 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef STORABLE_FILE_H
#  define STORABLE_FILE_H

#  include "ods.h"
#  include "macros.h"
#  include "utilities.h"

#  ifdef CIYAM_BASE_IMPL
#     define FILE_DECL_SPEC DYNAMIC_EXPORT
#  else
#     define FILE_DECL_SPEC DYNAMIC_IMPORT
#  endif

class read_stream;
class write_stream;

class FILE_DECL_SPEC storable_file : public storable_base
{
   friend int_t size_of( const storable_file& sf );
   friend read_stream& operator >>( read_stream& rs, storable_file& sf );
   friend write_stream& operator <<( write_stream& ws, const storable_file& sf );

   public:
   storable_file( ) { }
   storable_file( const std::string& file_name ) : file_name( file_name ) { }

   std::string get_name( ) const { return file_name; }

   int_t get_size_of( ) const;
   void get_instance( read_stream& rs );
   void put_instance( write_stream& ws ) const;

   private:
   std::string file_name;
};

#endif

