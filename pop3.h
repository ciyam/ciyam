// Copyright (c) 2010
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

#ifndef POP3_H
#  define POP3_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <vector>
#     include <string>
#     include <iosfwd>
#  endif

enum pop3_ctype
{
   e_pop3_ctype_tls,
   e_pop3_ctype_ssl,
   e_pop3_ctype_insecure
};

class pop3
{
   public:
   pop3( );
   pop3( const std::string& host, int port = 110, pop3_ctype ctype = e_pop3_ctype_insecure );

   ~pop3( );

   void init( const std::string& host, int port = 110, pop3_ctype ctype = e_pop3_ctype_insecure );

   void login( const std::string& user, const std::string& password );

   int get_num_messages( ) const;

   long get_total_messages_size( ) const;

   void get_message( int message_num, std::ostream& os, bool* p_is_mime = 0 );

   void get_message_headers( int message_num, std::vector< std::string >& headers );

   void delete_message( int message_num );

   void disconnect( );

   private:
   void get_message_list( std::vector< std::pair< int, long > >& message_list );

   struct impl;
   friend struct impl;
   impl* p_impl;
};

#endif
