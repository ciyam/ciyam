// Copyright (c) 2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef FILE_UTILS_H
#  define FILE_UTILS_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#  endif

void create_directories( const std::string& file_path, int perm = 0, int mask = -1 );

bool files_are_identical( const std::string& file_path1, const std::string& file_path2 );

bool delete_directory_files( const std::string& path, bool include_directory_itself = false );

#endif
