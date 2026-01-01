// Copyright (c) 2004-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef TEXT_FILE_BUFFER_H
#  define TEXT_FILE_BUFFER_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <cstring>
#     include <string>
#     include <vector>
#     include <iosfwd>
#  endif

class line
{
   public:
   line( const char* p ) : p( p ) { }

   operator const char*( ) const { return p; }

   bool operator ==( const line& other ) const
   {
      return strcmp( p, other.p ) == 0;
   }

   friend std::ostream& operator <<( std::ostream& outf, const line& p );

   private:
   const char* p;
};

class text_file_buffer
{
   public:
   typedef line value_type;

   text_file_buffer( const std::string& file_name );

   ~text_file_buffer( )
   {
      delete p;
   }

   const line& operator [ ]( size_t offset ) const
   {
      return lines[ offset ];
   }

   size_t size( ) const { return lines.size( ); }

   std::string get_file_name( ) const { return file_name; }

   private:
   char* p;
   std::string file_name;
   std::vector< line > lines;
};

#endif

