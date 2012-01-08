// Copyright (c) 2011
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

#ifndef FCGI_PARSER_H
#  define FCGI_PARSER_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <map>
#     include <string>
#  endif

void parse_input( char* buf, unsigned int len,
 std::map< std::string, std::string >& input_data, char separator = ';', bool keep_existing = false );

#endif

