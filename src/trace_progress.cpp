// Copyright (c) 2013-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

// NOTE: This file is intended for #include usage in anonymous namespaces only.

struct trace_progress : progress
{
   trace_progress( uint32_t flags ) : flags( flags ) { }

   void output_progress( const string& message,
    unsigned long num = 0, unsigned long total = 0 );

   uint32_t flags;
};

void trace_progress::output_progress(
 const string& message, unsigned long num, unsigned long total )
{
   string extra;

   if( num || total )
   {
      extra += to_string( num );

      if( total )
         extra += '/' + to_string( total );
   }

   if( ( get_trace_flags( ) & flags ) == flags )
      log_trace_message( flags, message + extra );
}

