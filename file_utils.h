// Copyright (c) 2012
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

#ifndef FILE_UTILS_H
#  define FILE_UTILS_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#  endif

bool delete_directory_files( const std::string& path, bool include_directory = false );

#endif
