// Copyright (c) 2003
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

   std::string get_usage( ) const;

   void output_usage( std::ostream& ostr ) const;
   void output_syntax( std::ostream& ostr ) const;

   void get_parameter_names( std::vector< std::string >& parameters ) const;

   size_t get_error_pos( ) const;

   size_t get_num_nodes( ) const;

   private:
   struct impl;
   impl* p_impl;
};

#endif

