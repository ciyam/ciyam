// Copyright (c) 2006
//
// CIYAM Pty. Ltd.
// ACN 093 704 539
//
// ALL RIGHTS RESERVED
//
// Permission to use this software for non-commercial purposes is hereby granted. Permission to
// distribute this software privately is granted provided that the source code is unaltered and
// complete or that any alterations and omissions have been first approved by CIYAM. Commercial
// usage of this software is not permitted without first obtaining a license for such a purpose
// from CIYAM. This software may not be publicly distributed unless written permission to do so
// has been obtained from CIYAM.

#ifndef CIYAM_SESSION_H
#  define CIYAM_SESSION_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#  endif

#  include "sockets.h"
#  include "threads.h"

class ciyam_session : public thread
{
   public:
   ciyam_session( std::auto_ptr< tcp_socket >& ap_socket );
   ~ciyam_session( );

   void on_start( );

   static void increment_session_count( );
   static void decrement_session_count( );

   private:
   std::auto_ptr< tcp_socket > ap_socket;
};

#endif

