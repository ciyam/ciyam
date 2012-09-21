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

const char* const c_regex_html_tag = "<([A-Za-z][A-Za-z0-9]*)([^>]*)>";
const char* const c_regex_email_address = "\\b[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,4}\\b";
const char* const c_regex_html_paired_tags = "<([A-Za-z][A-Za-z0-9]*)([^>]*)>(.*)</\\1>";
const char* const c_regex_floating_point_number = "\\b[-+]?[0-9]+\\.[0-9]+\\b";

// NOTE: This regular expression implementation does not perform backtracking and so will never
// match greedily (and does not support non-greedy match tokens such as +? *?). It does support
// back references via parenthesis grouping but does not support group options such as ?: which
// would normally indicate that a group does not create a back reference. Nested groups are not
// supported and alternations (such as: one|two|three) are also not supported (the latter could
// be added without too much difficulty). The design choices were made in order to do the least
// number of comparisons while still supporting non-trivial expressions. To have more extensive
// regular expressions it would be recommended to use a 3rd party library (such as PCRE).
class regex
{
   public:
   regex( const std::string& expr );
   ~regex( );

   std::string get_expr( ) const;

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

