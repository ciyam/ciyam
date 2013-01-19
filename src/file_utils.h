// Copyright (c) 2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2013 CIYAM Open Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef FILE_UTILS_H
#  define FILE_UTILS_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#  endif

bool delete_directory_files( const std::string& path, bool include_directory = false );

#endif
