// Copyright (c) 2014-2015 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef CIYAM_CORE_FILES_H
#  define CIYAM_CORE_FILES_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#     include <vector>
#  endif

#  include "macros.h"

#  ifdef CIYAM_BASE_IMPL
#     define CLASS_BASE_DECL_SPEC DYNAMIC_EXPORT
#  else
#     define CLASS_BASE_DECL_SPEC DYNAMIC_IMPORT
#  endif

struct blockchain_info
{
   std::string chain_id;
   std::string checkpoint_hash;
   std::vector< std::string > earlier_block_height_hash_of_hashes;
   std::vector< std::string > all_specific_block_height_block_hashes;
};

void CLASS_BASE_DECL_SPEC verify_core_file( const std::string& content,
 bool check_sigs = true, std::vector< std::pair< std::string, std::string > >* p_extras = 0 );

#endif

