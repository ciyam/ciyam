// Copyright (c) 2004-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
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
   static size_t decode_size( const std::string& input, bool url_encoding = false );

   inline static size_t decode_size( size_t length, bool minimum_possible = false )
   {
      return ( ( length / 4 ) * 3 ) - ( minimum_possible ? 2 : 0 );
   }

   static size_t decode( const std::string& input,
    unsigned char* p_data, size_t length, bool url_encoding = false );

   inline static std::string decode( const std::string& input, bool url_encoding = false )
   {
      std::string str( decode_size( input, url_encoding ), '\0' );
      decode( input, ( unsigned char* )str.c_str( ), str.length( ), url_encoding );

      return str;
   }

   inline static size_t encode_size( size_t length ) { return ( ( length + 2 ) / 3 ) * 4; }

   static void encode( const unsigned char* p_dat, size_t length,
    char* p_enc, size_t* p_enc_len = 0, bool url_encoding = false );

   inline static std::string encode( const unsigned char* p_dat, size_t length, bool url_encoding = false )
   {
      std::string str( encode_size( length ), '\0' );

      size_t enc_len = 0;
      encode( p_dat, length, ( char* )str.data( ), &enc_len, url_encoding );

      if( !url_encoding )
         return str;
      else
         return str.substr( 0, enc_len );
   }

   inline static std::string encode( const std::string& input, bool url_encoding = false )
   {
      return encode( ( const unsigned char* )input.c_str( ), input.length( ), url_encoding );
   }

   inline static void encode( std::string& str, const std::string& input, bool url_encoding = false )
   {
      size_t enc_len = encode_size( input.length( ) );

      if( str.length( ) < enc_len )
         str.resize( enc_len );

      encode( ( const unsigned char* )input.c_str( ), input.length( ), ( char* )str.data( ), &enc_len, url_encoding );
   }

   inline static void encode( std::string& str, const unsigned char* p_data, size_t length, bool url_encoding = false )
   {
      size_t enc_len = encode_size( length );

      if( str.length( ) < enc_len )
         str.resize( enc_len );

      encode( p_data, length, ( char* )str.data( ), &enc_len, url_encoding );
   }

   static bool valid_characters( const std::string& input, bool url_encoding = false );

   static void validate( const std::string& input, bool* p_rc = 0, bool url_encoding = false );
};

inline std::string hex_to_base64( const std::string& input ) { return base64::encode( hex_decode( input ) ); }
inline std::string base64_to_hex( const std::string& input ) { return hex_encode( base64::decode( input ) ); }

#endif
