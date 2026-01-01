// Copyright (c) 2009-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
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
   progress( ) : num_seconds( 1 ) { }

   virtual void output_progress(
    const std::string& message, unsigned long num = 0, unsigned long total = 0 )
   {
      ( void )num;
      ( void )total;
      ( void )message;
   }

   size_t num_seconds;
};

struct console_progress : progress
{
   console_progress( ) : num_decimals( 0 ), output_length( 0 ), previous_num( 0 ) { }

   ~console_progress( )
   {
      if( output_length )
         output_progress( "" );
   }

   void output_progress(
    const std::string& message, unsigned long num = 0, unsigned long total = 0 );

   size_t num_decimals;
   size_t output_length;

   std::string output_prefix;

   unsigned long previous_num;
};

#endif

