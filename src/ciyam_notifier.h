// Copyright (c) 2023-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef CIYAM_NOTIFIER_H
#  define CIYAM_NOTIFIER_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#     include <iosfwd>
#  endif

#  include "ptypes.h"
#  include "threads.h"

class ciyam_notifier : public thread
{
   public:
   ciyam_notifier( const std::string& watch_root,
    const std::string* p_initial_selections = 0, const std::string* p_paths_and_time_stamps = 0 );

   ~ciyam_notifier( );

   void on_start( );

   std::string get_next_unique( );

   private:
   size_t unique;
   bool has_existing;

   uint64_t unix_watch;

   size_t num_ignore_secs;

   std::string watch_root;
   std::string initial_selections;
   std::string paths_and_time_stamps;
};

#endif
