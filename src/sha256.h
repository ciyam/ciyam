// Copyright (c) 2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef SHA256_H
#  define SHA256_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#  endif

const int c_sha256_digest_size = 32;

class sha256
{
   public:
   sha256( );
   sha256( const std::string& str );
   sha256( const unsigned char* p_data, unsigned int length );

   ~sha256( );

   void init( );
   void update( const std::string& str, bool is_filename = false );
   void update( const unsigned char* p_data, unsigned int length );

   void copy_digest_to_buffer( unsigned char* p_buffer, unsigned int length = c_sha256_digest_size );

   void get_digest_as_string( std::string& s );
   std::string get_digest_as_string( char separator = '\0' );

   private:
   struct impl;
   impl* p_impl;
};

std::string hmac_sha256( const std::string& key, const std::string& message );

void hmac_sha256( const std::string& key, const std::string& message, unsigned char* p_buffer );

#endif
