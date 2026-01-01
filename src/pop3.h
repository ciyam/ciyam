// Copyright (c) 2010-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef POP3_H
#  define POP3_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <vector>
#     include <string>
#     include <iosfwd>
#  endif

struct progress;

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

   pop3( const std::string& host, int port = 110,
    pop3_ctype ctype = e_pop3_ctype_insecure, progress* p_progress = 0 );

   ~pop3( );

   void init( const std::string& host, int port = 110,
    pop3_ctype ctype = e_pop3_ctype_insecure, progress* p_progress = 0 );

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
