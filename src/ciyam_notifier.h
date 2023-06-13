// Copyright (c) 2023 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef CIYAM_NOTIFIER_H
#  define CIYAM_NOTIFIER_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#     include <iosfwd>
#  endif

#  include "macros.h"
#  include "threads.h"

#  ifdef CIYAM_BASE_IMPL
#     define CIYAM_BASE_DECL_SPEC DYNAMIC_EXPORT
#  else
#     define CIYAM_BASE_DECL_SPEC DYNAMIC_IMPORT
#  endif

class CIYAM_BASE_DECL_SPEC ciyam_notifier : public thread
{
   public:
   ciyam_notifier( const std::string& watch_root );
   ~ciyam_notifier( );

   void on_start( );

   std::string get_next_unique( );

   private:
   size_t unique;

   std::string watch_root;
};

#endif
