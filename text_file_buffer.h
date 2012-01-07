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

