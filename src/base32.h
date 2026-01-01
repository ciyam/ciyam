// Copyright (c) 2013-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef BASE32_H
#  define BASE32_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#  endif

class base32
{
   public:
   static std::string encode( const std::string& input );
   static std::string decode( const std::string& input );
};

#endif

