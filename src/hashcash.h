// Copyright (c) 2011-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef HASHCASH_H
#  define HASHCASH_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#  endif

bool check_hashcash( const std::string& hashcash );

std::string create_hashcash( const std::string& resource, int num_bits = 20, const char* p_ext = 0 );

#endif
