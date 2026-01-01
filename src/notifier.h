// Copyright (c) 2023-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef NOTIFIER_H
#  define NOTIFIER_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#     include <iosfwd>
#  endif

struct inotify_event;

class notifier
{
   public:
   notifier( bool ignore_hidden_files = true );
   notifier( const std::string& watch, bool recurse = true, bool ignore_hidden_files = true );

   ~notifier( );

   int add_watch( const std::string& watch, bool recurse = true, bool no_throw = false );

   void remove_watch( int wd );
   void remove_watch( const std::string& watch );

   size_t has_new_events( );

   void output_events( std::ostream& os );

   private:
   void init( bool ignore_hidden_files );

   void process_event( struct inotify_event* p_event, struct inotify_event* p_prior_event );

   struct impl;
   impl* p_impl;
};

#endif
