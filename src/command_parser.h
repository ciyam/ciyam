// Copyright (c) 2003-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef COMMAND_PARSER_H
#  define COMMAND_PARSER_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <map>
#     include <vector>
#     include <string>
#     include <iosfwd>
#  endif

class command_parser
{
   public:
   command_parser( );
   virtual ~command_parser( );

   void clear( );

   void parse_syntax( const char* p_input );

   bool parse_command(
    const std::vector< std::string >& arguments, std::map< std::string, std::string >& parameters );

   bool okay( );

   void dump_nodes( std::ostream& ostr ) const;

   std::string get_usage( ) const;

   void output_usage( std::ostream& ostr ) const;
   void output_syntax( std::ostream& ostr ) const;

   void get_parameter_names( std::vector< std::string >& parameters, std::vector< std::string >* p_default_vals = 0 ) const;

   size_t get_error_pos( ) const;

   size_t get_num_nodes( ) const;

   private:
   struct impl;
   impl* p_impl;
};

#endif

