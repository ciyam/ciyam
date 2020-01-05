// Copyright (c) 2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2020 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef REGEX_H
#  define REGEX_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <vector>
#     include <string>
#     include <iosfwd>
#  endif

const char* const c_regex_label = "[A-Za-z0-9_]+";
const char* const c_regex_integer = "^[0-9]{1,19}$";
const char* const c_regex_hash_256 = "^[A-Fa-f0-9]{64}$";
const char* const c_regex_html_tag = "<[/]?([A-Za-z][A-Za-z0-9]*)[\\s]*([^>]*)>";
const char* const c_regex_email_address = "[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,8}";
const char* const c_regex_bitcoin_address = "^[1-9A-HJ-NP-Za-km-z]{26,35}$";
const char* const c_regex_html_paired_tags = "<([A-Za-z][A-Za-z0-9]*)[\\s]*([^>]*)>(.*)</\\1>";
const char* const c_regex_floating_point_number = "[-+]?[0-9]+\\.[0-9]+";

// NOTE: This regular expression implementation does not perform backtracking and so will never
// match greedily (and does not support non-greedy match tokens such as +? *?). It does support
// back references via parenthesis grouping but does not support group options such as ?: which
// would normally indicate that a group does not create a back reference. Nested groups are not
// supported and alternations (such as: one|two|three) are also not supported (the latter could
// be added without too much difficulty). The design choices were made in order to do the least
// number of comparisons while still supporting non-trivial expressions.
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

