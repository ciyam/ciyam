// Copyright (c) 2017-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef CLZ_H
#  define CLZ_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <iosfwd>
#  endif

void init_clz_info( );

void decode_clz_data( std::istream& is, std::ostream& os );
void encode_clz_data( std::istream& is, std::ostream& os );

#endif
