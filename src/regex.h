// Copyright (c) 2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef REGEX_H
#  define REGEX_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <vector>
#     include <string>
#     include <iosfwd>
#     include <algorithm>
#  endif

const char* const c_regex_label = "[A-Za-z0-9_]+";
const char* const c_regex_height = "[0-9]{1,10}";
const char* const c_regex_integer = "[0-9]{1,19}";
const char* const c_regex_hash_256 = "[A-Fa-f0-9]{64}";
const char* const c_regex_html_tag = "<[/]?([A-Za-z][A-Za-z0-9]*)[\\s]*([^>]*)>";
const char* const c_regex_domain_name = "[a-z0-9.-]+\\.[a-z]{2,18}";
const char* const c_regex_email_address = "[A-Za-z0-9._%+-]+@[a-z0-9.-]+\\.[a-z]{2,18}";
const char* const c_regex_bitcoin_address = "[1-9A-HJ-NP-Za-km-z]{26,35}";
const char* const c_regex_html_paired_tags = "<([A-Za-z][A-Za-z0-9]*)[\\s]*([^>]*)>(.*)</\\1>";
const char* const c_regex_peerchain_identity = "[a-f0-9]{9}";
const char* const c_regex_floating_point_number = "[-+]?[0-9]+\\.[0-9]+";
const char* const c_regex_peerchain_description = "[A-Za-z0-9-+.' ]+";

enum regex_search_expense
{
   e_regex_search_expense_none,
   e_regex_search_expense_trivial,
   e_regex_search_expense_significant,
   e_regex_search_expense_overwhelming
};

struct regex_base
{
   virtual std::string::size_type search( const std::string& text,
    std::string::size_type* p_length, std::vector< std::string >* p_refs ) = 0;

   virtual regex_search_expense get_search_expense( ) const = 0;
};

// NOTE: This regular expression implementation does not perform backtracking and so will never
// match greedily (and does not support non-greedy match tokens such as +? *?). It does support
// back references via parenthesis grouping but does not support group options such as ?: which
// would normally indicate that a group does not create a back reference. Nested groups are not
// supported and alternations (such as: one|two|three) are also not supported (the latter could
// be added without too much difficulty). The design choices were made in order to do the least
// number of comparisons while still supporting non-trivial expressions.
class regex : public regex_base
{
   public:
   regex( const std::string& expr, bool match_at_start = false, bool match_at_finish = false );

   ~regex( );

   std::string get_expr( ) const;

   size_t get_min_size( ) const;
   size_t get_max_size( ) const;

   std::string::size_type search( const std::string& text,
    std::string::size_type* p_length = 0, std::vector< std::string >* p_refs = 0 );

   size_t get_search_iterations( ) const;

   regex_search_expense get_search_expense( ) const;

   void dump( std::ostream& os );

   private:
   struct impl;
   impl* p_impl;
};

// NOTE: The regular expression chain class supports the chaining of multiple standard regular
// expressions with either "&&" or "&!" separators. The "search" for this class calls "search"
// for each standard regular expression (using the same "text" argument) and then depending on
// the prior separator will either stop if has not matched (&&) or if it has matched (&!). The
// expression "^[a-z][-a-z]+$&&^.*[^-]$" will match "abcde" and "ab-de" along with "a---e" but
// will not match "-bcde", "abcd-" or "abc--". To stop "--" from being used the "&!" separator
// could be used to further extend the chain to "^[a-z][-a-z]+$&&^.*[^-]$&!^.*--.*$".
class regex_chain : public regex_base
{
   public:
   regex_chain( const std::string& expr );

   ~regex_chain( );

   std::string::size_type search( const std::string& text,
    std::string::size_type* p_length = 0, std::vector< std::string >* p_refs = 0 );

   inline regex_search_expense get_search_expense( ) const { return combined_search_expense; }

   private:
   void cleanup( );

   regex_search_expense combined_search_expense;

   std::vector< std::pair< regex*, bool > > regexes;
};

#endif
