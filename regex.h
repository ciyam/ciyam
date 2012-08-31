// Copyright (c) 2012
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

#ifndef REGEX_H
#  define REGEX_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <vector>
#     include <string>
#     include <iosfwd>
#  endif

class regex
{
   public:
   regex( const std::string& input );
   ~regex( );

   std::string get_input( ) const;

   int get_min_size( ) const;
   int get_max_size( ) const;

   std::string::size_type search( const std::string& text,
    std::string::size_type* p_length = 0, std::vector< std::string >* p_refs = 0 );

   void dump( std::ostream& os );

   private:
   struct impl;
   impl* p_impl;
};

#endif

