// Copyright (c) 2004-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2014 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef BASE64_H
#  define BASE64_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#  endif

class base64
{
   public:
   static std::string encode( const std::string& input );
   static std::string decode( const std::string& input );
};

#endif

