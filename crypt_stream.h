// Copyright (c) 2010
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

#ifndef CRYPT_STREAM_H
#  define CRYPT_STREAM_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <iosfwd>
#     include <string>
#  endif

#  include "config.h"

void crypt_stream( std::iostream& io, const char* p_key, size_t key_length );

#  ifdef SSL_SUPPORT
enum crypt_op
{
   e_crypt_op_encrypt,
   e_crypt_op_decrypt
};

void crypt_stream( std::iostream& io, const char* p_key, size_t key_length, crypt_op op );
#  endif

std::string password_encrypt( const std::string& password, const std::string& keyval );
std::string password_decrypt( const std::string& password, const std::string& keyval );

#endif
