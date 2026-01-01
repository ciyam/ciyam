// Copyright (c) 2005-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef FILE_BUFFER_H
#  define FILE_BUFFER_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <vector>
#     include <string>
#  endif

class file_buffer
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
