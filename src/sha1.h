// Copyright (c) 2007-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2013 CIYAM Open Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

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

