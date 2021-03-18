// Copyright (c) 2013-2021 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef SIOCONVERT_H
#  define SIOCONVERT_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#     include <iosfwd>
#  endif

class sio_graph;

enum json_format
{
   e_json_format_multi,
   e_json_format_single,
   e_json_format_compressed
};

void convert_sio_to_json( const sio_graph& sio, std::ostream& outs, json_format format = e_json_format_multi );

void convert_json_to_sio( const std::string& json, std::ostream& outs );

#endif

