// Copyright (c) 2004-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2022 CIYAM Developers
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
   inline static size_t encode_size( size_t length ) { return ( ( length + 2 ) / 3 ) * 4; }

   static void encode( const unsigned char* p_dat, size_t length, char* p_enc, size_t* p_enc_len = 0 );

   inline static std::string encode( const unsigned char* p_dat, size_t length )
   {
      std::string str( encode_size( length ), '\0' );
      encode( p_dat, length, ( char* )str.data( ) );

      return str;
   }

   inline static std::string encode( const std::string& input )
   {
      return encode( ( const unsigned char* )input.c_str( ), input.length( ) );
   }

   static bool valid_characters( const std::string& input );

   static void validate( const std::string& input, bool* p_rc = 0 );

   static size_t decode_size( const std::string& input );
   static size_t decode_size( size_t length, bool minimum_possible = false );

   static size_t decode( const std::string& input, unsigned char* p_data, size_t length );

   inline static std::string decode( const std::string& input )
   {
      std::string str( decode_size( input ), '\0' );
      decode( input, ( unsigned char* )str.c_str( ), str.length( ) );

      return str;
   }
};

inline std::string hex_to_base64( const std::string& input ) { return base64::encode( hex_decode( input ) ); }
inline std::string base64_to_hex( const std::string& input ) { return hex_encode( base64::decode( input ) ); }

#endif
