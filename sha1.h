// Copyright (c) 2007
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

#ifndef SHA1_H
#  define SHA1_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#  endif

class sha1
{
   public:
   sha1( );
   sha1( const std::string& str );

   ~sha1( );

   void init( );
   void update( const std::string& str );
   void update( const unsigned char* p_data, unsigned int length );

   std::string get_digest_as_string( char separator = '\0' );

   private:
   struct impl;
   impl* p_impl;
};

#endif

