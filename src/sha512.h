// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef SHA512_H
#  define SHA512_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#  endif

#  include "ptypes.h"

const int c_sha512_digest_size = 64;

class sha512
{
   public:
   sha512( );
   sha512( const std::string& str );
   sha512( const unsigned char* p_data, unsigned int length );

   void update( const std::string& str, bool is_filename = false );
   void update( const unsigned char* p_message, unsigned int len );

   void copy_digest_to_buffer( unsigned char* p_buffer, unsigned int length = c_sha512_digest_size );

   void get_digest_as_string( std::string& s );
   std::string get_digest_as_string( char separator = '\0' );

   protected:
   void init( );
   void finalise( );

   void do_update( const unsigned char* p_message, unsigned int len );
   void transform( const unsigned char* p_message, unsigned int block_nb );

   private:
   bool is_final;

   unsigned int m_len;
   unsigned int m_tot_len;

   unsigned char m_block[ 256 ];
   unsigned char m_digest[ c_sha512_digest_size ];

   uint64_t m_h[ 8 ];

   const static uint64_t sha512_k[ ];
};

std::string hmac_sha512( const std::string& key, const std::string& message );

void hmac_sha512( const std::string& key, const std::string& message, unsigned char* p_buffer );

#endif
