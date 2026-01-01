// Copyright (c) 2010-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef PDF_GEN_H
#  define PDF_GEN_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <map>
#     include <string>
#  endif

struct progress;

void generate_pdf_doc(
 const std::string& format_filename, const std::string& output_filename,
 const std::map< std::string, std::string >& variables, progress* p_progress = 0 );

#endif

