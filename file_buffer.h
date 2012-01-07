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

#ifndef FILE_BUFFER_H
#  define FILE_BUFFER_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <vector>
#     include <string>
#  endif

#  ifdef CAT_BASE_LIB
#     ifdef CAT_BASE_IMPL
#        define FILE_BUFFER_DECL_SPEC DYNAMIC_EXPORT
#     else
#        define FILE_BUFFER_DECL_SPEC DYNAMIC_IMPORT
#     endif
#  else
#     define FILE_BUFFER_DECL_SPEC
#  endif

class FILE_BUFFER_DECL_SPEC file_buffer
{
   public:
   file_buffer( ) { }
   file_buffer( const char* p_file_name );

   file_buffer( const file_buffer& src );

   file_buffer& operator =( const file_buffer& src );

   void load( const char* p_file_name );
   void save( const char* p_output_path ) const;

   const std::string& get_name( ) const { return name; }
   void set_name( const std::string& new_name ) { name = new_name; }

   size_t get_size( ) const { return data.size( ); }

   const unsigned char* get_data( ) const { return &data[ 0 ]; }

   protected:
   std::string name;
   std::vector< unsigned char > data;
};

#endif

