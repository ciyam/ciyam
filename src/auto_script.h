// Copyright (c) 2011-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef AUTO_SCRIPT_H
#  define AUTO_SCRIPT_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <iosfwd>
#  endif

#  include "macros.h"
#  include "threads.h"

#  ifdef CIYAM_BASE_IMPL
#     define CIYAM_BASE_DECL_SPEC DYNAMIC_EXPORT
#  else
#     define CIYAM_BASE_DECL_SPEC DYNAMIC_IMPORT
#  endif

void CIYAM_BASE_DECL_SPEC output_schedule( std::ostream& os, bool from_now = false );

class CIYAM_BASE_DECL_SPEC autoscript_session : public thread
{
   public:
   autoscript_session( );
   ~autoscript_session( );

   void on_start( );
};

extern "C" void CIYAM_BASE_DECL_SPEC init_auto_script( );

typedef void ( *fp_init_auto_script )( );

#endif

