// Copyright (c) 2009-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2020 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef PROGRESS_H
#  define PROGRESS_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#  endif

struct progress
{
   virtual void output_progress( const std::string& /*message*/ ) { }
};

#endif

