// Copyright (c) 2013-2019 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

// NOTE: This file is intended for #include usage in anonymous namespaces only.

struct trace_progress : progress
{
   trace_progress( int flag ) : flag( flag ) { }

   void output_progress( const string& message );

   int flag;
};

void trace_progress::output_progress( const string& message )
{
   if( get_trace_flags( ) & flag )
      log_trace_message( flag, message );
}

