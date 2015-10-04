// Copyright (c) 2010-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2015 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef CRYPT_STREAM_H
#  define CRYPT_STREAM_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <iosfwd>
#     include <string>
#  endif

#  include "config.h"

void crypt_stream( std::iostream& io, const char* p_key, size_t key_length );

inline void crypt_stream( std::iostream& io, const std::string& key )
{
   crypt_stream( io, key.c_str( ), key.length( ) );
}

#  ifdef SSL_SUPPORT
enum crypt_op
{
   e_crypt_op_encrypt,
   e_crypt_op_decrypt
};

std::string aes_crypt( const std::string& s,
 const char* p_key, size_t key_length, crypt_op op, bool use_256 = true );
#  endif

std::string get_totp( const std::string& base32_encoded_secret, int freq = 30 );

std::string get_totp_secret( const std::string& user_unique, const std::string& system_unique );

std::string password_decrypt( const std::string& dat, const std::string& key, bool use_ssl = true );

std::string password_encrypt( const std::string& dat,
 const std::string& key, bool use_ssl = true, bool add_salt = true );

#endif
