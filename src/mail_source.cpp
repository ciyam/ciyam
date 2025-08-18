// Copyright (c) 2011-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <set>
#  include <memory>
#  include <fstream>
#  include <iostream>
#  include <stdexcept>
#endif

#define CIYAM_BASE_IMPL

#include "mail_source.h"

#include "pop3.h"
#include "threads.h"
#include "progress.h"
#include "utilities.h"
#include "ciyam_base.h"

using namespace std;

namespace
{

const char* const c_mime_header = "MIME-Version:";

mutex g_mutex;

set< string > mboxes;

void lock_mbox( const string& mbox )
{
   guard g( g_mutex );

   if( mboxes.count( mbox ) )
      throw runtime_error( "mailbox is currently busy" );

   mboxes.insert( mbox );
}

void unlock_mbox( const string& mbox )
{
   guard g( g_mutex );

   if( mboxes.count( mbox ) )
      mboxes.erase( mbox );
}

#include "trace_progress.cpp"

}

struct mbox_source::impl
{
   impl( ) : num_emails( 0 ), locked_mbox( false ) { }
   impl( const string& username ) : num_emails( 0 ), locked_mbox( false ), username( username ) { }

   void start_processing( );

   int get_num_messages( );
   void get_message_headers( int num, vector< string >& headers );

   void get_message( int num, ostream& os, bool* p_is_mime = 0 );

   void delete_message( int num );

   void finish_processing( );

   int num_emails;
   bool locked_mbox;

   string username;
   string mbox_file;

   vector< string > lines;
   set< int > mime_messages;
   set< int > deleted_messages;
   vector< pair< int, int > > header_info;
   vector< pair< int, int > > message_info;
};

void mbox_source::impl::start_processing( )
{
   if( username.empty( ) )
      username = get_mbox_username( );

   if( !username.empty( ) )
   {
      string::size_type pos = username.find( '@' );
      if( pos != string::npos )
         username.erase( pos );

      mbox_file = get_mbox_path( );
      mbox_file += "/" + username;

      lock_mbox( mbox_file );
      locked_mbox = true;

      if( file_exists( mbox_file ) )
         buffer_file_lines( mbox_file, lines, false );
   }

   num_emails = 0;
   int line_num = 0;
   int start_line = 0;
   bool is_mime = false;
   bool is_script = false;
   bool in_headers = false;

   for( size_t i = 0; i < lines.size( ); i++ )
   {
      string next_line( lines[ i ] );

      if( next_line.size( ) > 6 && next_line.substr( 0, 5 ) == "From " )
      {
         if( num_emails++ )
         {
            message_info.push_back( make_pair( start_line, line_num - start_line ) );
            if( is_mime )
               mime_messages.insert( num_emails - 1 );
         }

         is_mime = false;
         in_headers = true;
         start_line = line_num;
      }
      else
      {
         if( in_headers )
         {
            if( next_line.empty( ) )
            {
               in_headers = false;
               header_info.push_back( make_pair( start_line, line_num - start_line ) );
            }
            else if( lower( next_line ).find( lower( c_mime_header ) ) == 0 )
               is_mime = true;
         }
         else if( next_line[ 0 ] == '>' )
         {
            size_t j = 1;
            for( ; j < next_line.size( ); j++ )
            {
               if( next_line[ j ] != '>' )
                  break;
            }

            if( next_line.size( ) > j + 5 && next_line.substr( j, 5 ) == "From " )
            {
               next_line.erase( 0, 1 );
               lines[ i ] = next_line;
            }
         }
      }
      ++line_num;
   }

   if( line_num > start_line )
   {
      message_info.push_back( make_pair( start_line, line_num - start_line ) );
      if( is_mime )
         mime_messages.insert( num_emails - 1 );
   }
}

int mbox_source::impl::get_num_messages( )
{
   return num_emails;
}

void mbox_source::impl::get_message_headers( int num, vector< string >& headers )
{
   if( num <= 0 || num > num_emails )
      throw runtime_error( "invalid email number #" + to_string( num ) + " for get_message_headers" );

   bool found = false;
   size_t last_header = 0;
   size_t start = header_info[ num - 1 ].first;
   size_t total = header_info[ num - 1 ].second;

   bool get_all_headers = headers.empty( );
   for( size_t i = start + 1; i < start + total; i++ )
   {
      string next_line( lines[ i ] );
      if( get_all_headers )
         headers.push_back( next_line );
      else
      {
         if( found && !next_line.empty( ) && ( next_line[ 0 ] == ' ' || next_line[ 0 ] == '\t' ) )
         {
            headers[ last_header ] += "\n" + next_line;
            continue;
         }
         else
            found = false;

         for( vector< string >::size_type j = 0; j < headers.size( ); j++ )
         {
            string::size_type pos = lower( next_line ).find( lower( headers[ j ] ) );

            if( pos == 0 )
            {
               found = true;
               last_header = j;
               headers[ j ] = next_line;

               break;
            }
         }
      }
   }
}

void mbox_source::impl::get_message( int num, ostream& os, bool* p_is_mime )
{
   if( num <= 0 || num > num_emails )
      throw runtime_error( "invalid email number #" + to_string( num ) + " for get_message" );

   if( p_is_mime )
      *p_is_mime = ( mime_messages.count( num - 1 ) > 0 );

   size_t start = message_info[ num - 1 ].first;
   size_t total = message_info[ num - 1 ].second;

   for( size_t i = start + 1; i < start + total; i++ )
   {
      string next_line( lines[ i ] );
      os << next_line << '\n';
   }
}

void mbox_source::impl::delete_message( int num )
{
   if( num <= 0 || num > num_emails )
      throw runtime_error( "invalid email number #" + to_string( num ) + " for delete_message" );

   deleted_messages.insert( num - 1 );
}

void mbox_source::impl::finish_processing( )
{
   if( !deleted_messages.empty( ) )
   {
      ofstream outf( mbox_file.c_str( ), ios::trunc | ios::out );

      for( int i = 1; i <= num_emails; i++ )
      {
         if( !deleted_messages.count( i - 1 ) )
         {
            size_t start = message_info[ i - 1 ].first;
            size_t total = message_info[ i - 1 ].second;

            for( size_t i = start; i < start + total; i++ )
               outf << lines[ i ] << '\n';
         }
      }

      outf.flush( );
      if( !outf.good( ) )
         throw runtime_error( "unexpected bad output stream for '" + mbox_file + "'" );
   }

   if( locked_mbox )
      unlock_mbox( mbox_file );
}

mbox_source::mbox_source( )
{
   p_impl = new impl;
}

mbox_source::mbox_source( const string& username )
{
   p_impl = new impl( username );
}

mbox_source::~mbox_source( )
{
   delete p_impl;
}

void mbox_source::start_processing( )
{
   p_impl->start_processing( );
}

int mbox_source::get_num_messages( )
{
   return p_impl->get_num_messages( );
}

void mbox_source::get_message_headers( int num, vector< string >& headers )
{
   p_impl->get_message_headers( num, headers );
}

void mbox_source::get_message( int num, ostream& os, bool* p_is_mime )
{
   p_impl->get_message( num, os, p_is_mime );
}

void mbox_source::delete_message( int num )
{
   p_impl->delete_message( num );
}

void mbox_source::finish_processing( )
{
   p_impl->finish_processing( );
}

struct pop3_source::impl
{
   impl( );
   impl( const string& username, const string& password );

   void start_processing( );

   int get_num_messages( );
   void get_message_headers( int num, vector< string >& headers );

   void get_message( int num, ostream& os, bool* p_is_mime );

   void delete_message( int num );

   void finish_processing( );

   string username;
   string password;

   auto_ptr< pop3 > ap_pop;
   auto_ptr< progress > ap_progress;
};

pop3_source::impl::impl( )
{
   int port = 110;

   string host( get_pop3_server( ) );
   string::size_type pos = host.find( ':' );
   if( pos != string::npos )
   {
      port = atoi( host.substr( pos + 1 ).c_str( ) );
      host.erase( pos );
   }

   if( host.empty( ) )
      throw runtime_error( "missing POP3 server configuration information" );

   username = get_pop3_username( );
   password = get_pop3_password( );

   string suffix( get_pop3_suffix( ) );
   if( !suffix.empty( ) && username.find( '@' ) == string::npos )
      username += "@" + suffix;

   pop3_ctype ctype = e_pop3_ctype_insecure;

   string security( get_pop3_security( ) );
   if( !security.empty( ) )
   {
      if( security != "SSL" && security != "TLS" )
         throw runtime_error( "invalid POP3 security setting '" + security + "'" );

      if( !get_using_ssl( ) )
         throw runtime_error( "unable to use SSL/TLS without SSL support and a valid PEM file" );

      if( security == "SSL" )
         ctype = e_pop3_ctype_ssl;
      else
         ctype = e_pop3_ctype_tls;
   }

   if( get_trace_flags( ) & ( TRACE_DETAILS | TRACE_SOCKETS ) )
      ap_progress.reset( new trace_progress( TRACE_DETAILS | TRACE_SOCKETS ) );

   ap_pop.reset( new pop3( host, port, ctype, ap_progress.get( ) ) );
}

pop3_source::impl::impl( const string& username, const string& password )
 :
 username( username ),
 password( password )
{
   int port = 110;

   string host( get_pop3_server( ) );
   string::size_type pos = host.find( ':' );
   if( pos != string::npos )
   {
      port = atoi( host.substr( pos + 1 ).c_str( ) );
      host.erase( pos );
   }

   if( host.empty( ) )
      throw runtime_error( "missing POP3 server configuration information" );

   pop3_ctype ctype = e_pop3_ctype_insecure;

   string security( get_pop3_security( ) );
   if( !security.empty( ) )
   {
      if( security != "SSL" && security != "TLS" )
         throw runtime_error( "invalid POP3 security setting '" + security + "'" );

      if( !get_using_ssl( ) )
         throw runtime_error( "unable to use SSL/TLS without SSL support and a valid PEM file" );

      if( security == "SSL" )
         ctype = e_pop3_ctype_ssl;
      else
         ctype = e_pop3_ctype_tls;
   }

   if( get_trace_flags( ) & ( TRACE_DETAILS | TRACE_SOCKETS ) )
      ap_progress.reset( new trace_progress( TRACE_DETAILS | TRACE_SOCKETS ) );

   ap_pop.reset( new pop3( host, port, ctype, ap_progress.get( ) ) );
}

void pop3_source::impl::start_processing( )
{
   ap_pop->login( username, password );
}

int pop3_source::impl::get_num_messages( )
{
   return ap_pop->get_num_messages( );
}

void pop3_source::impl::get_message_headers( int num, vector< string >& headers )
{
   ap_pop->get_message_headers( num, headers );
}

void pop3_source::impl::get_message( int num, ostream& os, bool* p_is_mime )
{
   ap_pop->get_message( num, os, p_is_mime );
}

void pop3_source::impl::delete_message( int num )
{
   ap_pop->delete_message( num );
}

void pop3_source::impl::finish_processing( )
{
   ap_pop->disconnect( );
}

pop3_source::pop3_source( )
{
   p_impl = new impl;
}

pop3_source::pop3_source( const string& username, const string& password )
{
   p_impl = new impl( username, password );
}

pop3_source::~pop3_source( )
{
   delete p_impl;
}

void pop3_source::start_processing( )
{
   p_impl->start_processing( );
}

int pop3_source::get_num_messages( )
{
   return p_impl->get_num_messages( );
}

void pop3_source::get_message_headers( int num, vector< string >& headers )
{
   p_impl->get_message_headers( num, headers );
}

void pop3_source::get_message( int num, ostream& os, bool* p_is_mime )
{
   p_impl->get_message( num, os, p_is_mime );
}

void pop3_source::delete_message( int num )
{
   p_impl->delete_message( num );
}

void pop3_source::finish_processing( )
{
   p_impl->finish_processing( );
}

