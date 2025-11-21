// Copyright (c) 2011-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef MAIL_SOURCE_H
#  define MAIL_SOURCE_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <string>
#     include <vector>
#     include <iosfwd>
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

class mbox_source : public mail_source
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

class pop3_source : public mail_source
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

