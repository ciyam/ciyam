// Copyright (c) 2003-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2016 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef CONSOLE_H
#  define CONSOLE_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#  endif

char get_char( const char* p_prompt = 0 );
inline char get_char( const std::string& prompt ) { return get_char( prompt.c_str( ) ); }

std::string get_line( const char* p_prompt = 0 );
inline std::string get_line( const std::string& prompt ) { return get_line( prompt.c_str( ) ); }

std::string get_password( const char* p_prompt = 0 );
inline std::string get_password( const std::string& prompt ) { return get_password( prompt.c_str( ) ); }

#endif

