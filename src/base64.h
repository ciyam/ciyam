// Copyright (c) 2004-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2018 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef BASE64_H
#  define BASE64_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#  endif

#  include "utilities.h"

class base64
{
   public:
   static size_t encode_size( size_t length );

   static std::string encode( const unsigned char* p_dat, size_t length );

   inline static std::string encode( const std::string& input )
   {
      return encode( ( const unsigned char* )input.c_str( ), input.length( ) );
   }

   static void validate( const std::string& input, bool* p_rc = 0 );

   static size_t decode_size( size_t length );
   static size_t decode_size( const std::string& input );

   static size_t decode( const std::string& input, unsigned char* p_data, size_t length );

   inline static std::string decode( const std::string& input )
   {
      std::string s( decode_size( input ), '\0' );
      decode( input, ( unsigned char* )s.c_str( ), s.length( ) );
      return s;
   }
};

inline std::string hex_to_base64( const std::string& input ) { return base64::encode( hex_decode( input ) ); }
inline std::string base64_to_hex( const std::string& input ) { return hex_encode( base64::decode( input ) ); }

#endif

