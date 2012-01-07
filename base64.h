// Copyright (c) 2004
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

