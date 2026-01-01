// Copyright (c) 2023-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <climits>
#  include <fcntl.h>
#  include <unistd.h>
#  include <sys/inotify.h>
#  include <map>
#  include <memory>
#  include <vector>
#  include <sstream>
#  include <iostream>
#  include <stdexcept>
#endif

#include "notifier.h"

//#define COMPILE_TESTBED_MAIN

#define BUF_LEN ( 10 * ( sizeof( struct inotify_event ) + NAME_MAX + 1 ) )

using namespace std;

namespace
{

const char c_separator = '|';

string get_ops( int mask, bool omit_dir = true )
{
   string ops;

   if( mask & IN_OPEN )
      ops += ",open";

   if( !omit_dir && ( mask & IN_ISDIR ) )
      ops += ",isdir";

   if( mask & IN_ACCESS )
      ops += ",access";

   if( mask & IN_ATTRIB )
      ops += ",attrib";

   if( mask & IN_CREATE )
      ops += ",create";

   if( mask & IN_DELETE )
      ops += ",delete";

   if( mask & IN_MODIFY )
      ops += ",modify";

   if( mask & IN_IGNORED )
      ops += ",ignored";

   if( mask & IN_UNMOUNT )
      ops += ",unmount";

   if( mask & IN_MOVED_TO )
      ops += ",moved_to";

   if( mask & IN_MOVE_SELF )
      ops += ",move_self";

   if( mask & IN_MOVED_FROM )
      ops += ",moved_from";

   if( mask & IN_Q_OVERFLOW )
      ops += ",q_overflow";

   if( mask & IN_CLOSE_WRITE )
      ops += ",close_write";

   if( mask & IN_DELETE_SELF )
      ops += ",delete_self";

   if( mask & IN_CLOSE_NOWRITE )
      ops += ",close_nowrite";

   if( !ops.empty( ) )
      ops.erase( 0, 1 );

   return ops;
}

}

struct notifier::impl
{
   int id;

   char* p_buf;

   string watch;

   vector< string > events;

   bool ignore_hidden_files;

   map< string, int > watches_wd;
   map< int, string > wd_watches;

   map< int, string > cookie_old_names;
};

notifier::notifier( bool ignore_hidden_files )
{
   init( ignore_hidden_files );
}

notifier::notifier( const string& watch, bool recurse, bool ignore_hidden_files )
{
   init( ignore_hidden_files );

   add_watch( watch, recurse );

   p_impl->watch = watch;
}

notifier::~notifier( )
{
   vector< int > watch_ids;

   map< int, string >::iterator wi;

   for( wi = p_impl->wd_watches.begin( ); wi != p_impl->wd_watches.end( ); ++wi )
      watch_ids.push_back( wi->first );

   for( size_t i = 0; i < watch_ids.size( ); i++ )
      remove_watch( watch_ids[ i ] );

   close( p_impl->id );

   delete p_impl->p_buf;
   delete p_impl;
}

int notifier::add_watch( const string& watch, bool recurse, bool no_throw )
{
   int wd = inotify_add_watch( p_impl->id, watch.c_str( ), IN_ALL_EVENTS );

   if( wd < 0 )
   {
      if( !no_throw )
         throw runtime_error( "invalid watch '" + watch + "'" );
   }
   else
   {
#ifdef COMPILE_TESTBED_MAIN
      cout << "adding watch: " << watch << endl;
#endif

      if( recurse )
      {
         p_impl->watches_wd.insert( make_pair( watch, wd ) );
         p_impl->wd_watches.insert( make_pair( wd, watch ) );
      }
   }

   return wd;
}

void notifier::remove_watch( int wd )
{
   inotify_rm_watch( p_impl->id, wd );

   map< int, string >::iterator wi = p_impl->wd_watches.find( wd );

   if( wi != p_impl->wd_watches.end( ) )
   {
#ifdef COMPILE_TESTBED_MAIN
      cout << "removing watch: " << wd << endl;
#endif
      p_impl->watches_wd.erase( wi->second );
      p_impl->wd_watches.erase( wi );
   }
}

void notifier::remove_watch( const string& watch )
{
   map< string, int >::iterator wi = p_impl->watches_wd.find( watch );

   if( wi != p_impl->watches_wd.end( ) )
      remove_watch( wi->second );
}

void notifier::process_event( struct inotify_event* p_event, struct inotify_event* p_prior_event )
{
   if( p_event )
   {
      int wd = p_event->wd;
      int mask = p_event->mask;
      int cookie = p_event->cookie;

      string ops( get_ops( mask ) );

      int ignored = IN_IGNORED;
      int open_dir = ( IN_OPEN | IN_ISDIR );
      int close_dir = ( IN_ISDIR | IN_CLOSE_NOWRITE );
      int create_dir = ( IN_ISDIR | IN_CREATE );
      int delete_dir = ( IN_ISDIR | IN_DELETE );
      int delete_self = IN_DELETE_SELF;
      int moved_dir_to = ( IN_ISDIR | IN_MOVED_TO );
      int moved_dir_from = ( IN_ISDIR | IN_MOVED_FROM );

      bool is_ignored = ( ( mask & ignored ) == ignored );
      bool is_open_dir = ( ( mask & open_dir ) == open_dir );
      bool is_close_dir = ( ( mask & close_dir ) == close_dir );
      bool is_create_dir = ( ( mask & create_dir ) == create_dir );
      bool is_delete_dir = ( ( mask & delete_dir ) == delete_dir );
      bool is_delete_self = ( ( mask & delete_self ) == delete_self );
      bool is_moved_dir_to = ( ( mask & moved_dir_to ) == moved_dir_to );
      bool is_moved_dir_from = ( ( mask & moved_dir_from ) == moved_dir_from );

      string new_watch;

      bool reportable_event = false;

      if( p_impl->wd_watches.count( wd ) )
         new_watch = p_impl->wd_watches[ wd ];

      string path( new_watch );

      if( !path.empty( ) )
         path += '/';

      string file_name;

      if( p_event->len )
         file_name = string( p_event->name );
      else if( p_impl->wd_watches.empty( ) )
         file_name = p_impl->watch;

      string prior_file_name( new_watch );

      int prior_mask = 0;

      if( p_prior_event )
      {
         prior_mask = p_prior_event->mask;

         if( p_prior_event->len )
            prior_file_name = string( p_prior_event->name );
      }

      bool is_hidden_file = false;

      if( !file_name.empty( ) && file_name[ 0 ] == '.' )
         is_hidden_file = true;

      // NOTE: Conditionally ignore hidden files and directories.
      if( !is_hidden_file || !p_impl->ignore_hidden_files )
      {
         if( p_impl->wd_watches.count( wd ) && ( is_open_dir || is_create_dir ) )
         {
            if( !file_name.empty( ) )
            {
               if( new_watch.empty( ) )
                  new_watch = file_name;
               else
                  new_watch += '/' + file_name;

               if( !p_impl->watches_wd.count( new_watch ) )
               {
                  if( is_create_dir )
                     reportable_event = true;

                  // NOTE: New watch may fail if directory was just deleted.
                  add_watch( new_watch, true, true );
               }
            }
         }
         else if( is_delete_dir || is_delete_self )
         {
            reportable_event = true;

            if( is_delete_self )
               remove_watch( wd );
            else if( !file_name.empty( ) )
               remove_watch( p_impl->wd_watches[ wd ] + '/' + file_name );
         }
         else if( !( mask & IN_ACCESS ) && !( mask & IN_OPEN )
          && !( mask & IN_CLOSE_WRITE ) && !( mask & IN_CLOSE_NOWRITE ) )
            reportable_event = true;

         if( cookie && is_moved_dir_from )
            p_impl->cookie_old_names[ cookie ] = ( path + file_name );

         // NOTE: Update watch map entries when a directory is renamed.
         if( cookie && is_moved_dir_to )
         {
            string old_name( p_impl->cookie_old_names[ cookie ] );
            string new_name( path + file_name );

            if( !old_name.empty( ) && ( old_name != new_name ) )
            {
               int old_wd = p_impl->watches_wd[ old_name ];

               if( old_wd )
               {
                  p_impl->wd_watches[ old_wd ] = new_name;

                  p_impl->watches_wd.erase( old_name );
                  p_impl->watches_wd.insert( make_pair( new_name, old_wd ) );

                  // NOTE: Need to update any child watch map entries also.
                  string old_prefix( old_name + '/' );
                  string new_prefix( new_name + '/' );

                  while( true )
                  {
                     map< string, int >::iterator i = p_impl->watches_wd.lower_bound( old_prefix );

                     if( ( i == p_impl->watches_wd.end( ) ) || ( i->first.find( old_prefix ) != 0 ) )
                        break;

                     string new_child_name( new_prefix + i->first.substr( old_prefix.length( ) ) );

                     int child_wd = i->second;

                     p_impl->wd_watches[ child_wd ] = new_child_name;

                     p_impl->watches_wd.erase( i->first );
                     p_impl->watches_wd.insert( make_pair( new_child_name, child_wd ) );
                  }
               }
            }
         }

         if( is_ignored || ( mask & IN_MOVE_SELF )
          || ( ( file_name == prior_file_name ) && ( mask & IN_ATTRIB ) && ( prior_mask & IN_CREATE ) ) )
            reportable_event = false;

         stringstream ss;

         if( !file_name.empty( ) && ( mask & IN_ISDIR ) )
            file_name += '/';

         // NOTE: Mark non-reportable events so that they will be ignored but can be viewed.
         if( !reportable_event )
            ss << '-';
         else
            ss << '+';

         ss << path << file_name << c_separator << ops << c_separator << cookie;

         p_impl->events.push_back( ss.str( ) );

         // NOTE: Accumulate mask bits for multiple file events.
         if( p_prior_event && ( file_name == prior_file_name ) )
            p_event->mask |= p_prior_event->mask;
      }
   }
}

size_t notifier::has_new_events( )
{
   int64_t num_bytes = read( p_impl->id, p_impl->p_buf, BUF_LEN );

   if( num_bytes > 0 )
   {
      struct inotify_event* p_prior_event = 0;

      p_impl->cookie_old_names.clear( );

      for( char* p = p_impl->p_buf; p < p_impl->p_buf + num_bytes; )
      {
         struct inotify_event* p_event = ( struct inotify_event * )p;

         process_event( p_event, p_prior_event );

         p_prior_event = p_event;

         p += sizeof( struct inotify_event ) + p_event->len;
      }
   }

   return p_impl->events.size( );
}

void notifier::output_events( ostream& os )
{
   if( p_impl->events.size( ) )
   {
#ifdef COMPILE_TESTBED_MAIN
      os << "(" << p_impl->events.size( ) << ")\n";
#endif
      for( size_t i = 0; i < p_impl->events.size( ); i++ )
      {
         if( i )
            os << '\n';
         os << p_impl->events[ i ];
      }

#ifdef COMPILE_TESTBED_MAIN
      os << endl;
#endif

      p_impl->events.clear( );
   }
}

void notifier::init( bool ignore_hidden_files )
{
   p_impl = new impl;

   p_impl->id = inotify_init( );

   if( p_impl->id < 0 )
      throw runtime_error( "inotify_init failed" );

   // NOTE: Prevents read from blocking.
   fcntl( p_impl->id, F_SETFL, fcntl( p_impl->id, F_GETFL ) | O_NONBLOCK );

   p_impl->p_buf = new char[ BUF_LEN ];

   p_impl->ignore_hidden_files = ignore_hidden_files;
}

#ifdef COMPILE_TESTBED_MAIN
int main( int argc, char* argv[ ] )
{
   if( argc < 2 || ( argv[ 1 ] == string( "/?" ) ) )
   {
      cout << "Usage: ciyam_notifier [<directory>]" << endl;
      return 0;
   }

   string directory( argv[ 1 ] );

   try
   {

      notifier n( directory );

      while( true )
      {
         if( n.has_new_events( ) )
            n.output_events( cout );
         else
            sleep( 1 );
      }
   }
   catch( exception& x )
   {
      cerr << "error: " << x.what( ) << endl;
   }
}
#endif
