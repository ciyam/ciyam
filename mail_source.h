// Copyright (c) 2011
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

#ifndef MAIL_SOURCE_H
#  define MAIL_SOURCE_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#     include <vector>
#     include <iosfwd>
#  endif

#  include "macros.h"

#  ifdef CIYAM_BASE_IMPL
#     define MAIL_SOURCE_DECL_SPEC DYNAMIC_EXPORT
#  else
#     define MAIL_SOURCE_DECL_SPEC DYNAMIC_IMPORT
#  endif

struct mail_source
{
   virtual ~mail_source( ) { }

   virtual void start_processing( ) = 0;

   virtual int get_num_messages( ) = 0;
   virtual void get_message_headers( int num, std::vector< std::string >& headers ) = 0;

   virtual void get_message( int num, std::ostream& os, bool* p_is_mime = 0 ) = 0;

   virtual void delete_message( int num ) = 0;

   virtual void finish_processing( ) = 0;
};

class MAIL_SOURCE_DECL_SPEC mbox_source : public mail_source
{
   public:
   mbox_source( );
   mbox_source( const std::string& username );

   ~mbox_source( );

   void start_processing( );

   int get_num_messages( );
   void get_message_headers( int num, std::vector< std::string >& headers );

   void get_message( int num, std::ostream& os, bool* p_is_mime = 0 );

   void delete_message( int num );

   void finish_processing( );

   private:
   struct impl;
   friend struct impl;
   impl* p_impl;
};

class MAIL_SOURCE_DECL_SPEC pop3_source : public mail_source
{
   public:
   pop3_source( );
   pop3_source( const std::string& username, const std::string& password );

   ~pop3_source( );

   void start_processing( );

   int get_num_messages( );
   void get_message_headers( int num, std::vector< std::string >& headers );

   void get_message( int num, std::ostream& os, bool* p_is_mime = 0 );

   void delete_message( int num );

   void finish_processing( );

   private:
   struct impl;
   friend struct impl;
   impl* p_impl;
};

#endif

