// Copyright (c) 2005-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef STORABLE_FILE_H
#  define STORABLE_FILE_H

#  include "ods.h"
#  include "utilities.h"

class read_stream;
class write_stream;

struct progress;

struct storable_file_extra : public storable_extra
{
   storable_file_extra( const std::string& file_name,
    std::ostream* p_ostream = 0, progress* p_progress = 0 )
    :
    file_name( file_name ),
    p_istream( 0 ),
    p_ostream( p_ostream ),
    file_size( 0 ),
    p_progress( p_progress )
   {
   }

   storable_file_extra( const std::string& file_name,
    std::istream& isstr, int64_t file_size, progress* p_progress = 0 )
    :
    file_name( file_name ),
    p_istream( &isstr ),
    p_ostream( 0 ),
    file_size( file_size ),
    p_progress( p_progress )
   {
   }

   std::string file_name;
   std::istream* p_istream;
   std::ostream* p_ostream;

   int64_t file_size;

   progress* p_progress;
};

class storable_file : public storable_base
{
   friend int64_t size_of( const storable_file& sf );
   friend read_stream& operator >>( read_stream& rs, storable_file& sf );
   friend write_stream& operator <<( write_stream& ws, const storable_file& sf );

   public:
   storable_file( )
    :
    p_istream( 0 ),
    p_ostream( 0 ),
    file_size( 0 ),
    p_progress( 0 )
   {
   }

   storable_file( const std::string& file_name )
    :
    file_name( file_name ),
    p_istream( 0 ),
    p_ostream( 0 ),
    file_size( 0 ),
    p_progress( 0 )
   {
   }

   void set_extra( storable_extra* p_extra );

   int64_t get_size_of( ) const;

   void get_instance( read_stream& rs );
   void put_instance( write_stream& ws ) const;

   // NOTE: Refer to the NOTE in "oid_pointer.h" about this function.
   static bool can_copy_direct( ) { return false; }

   private:
   std::string file_name;
   std::istream* p_istream;
   std::ostream* p_ostream;

   int64_t file_size;

   progress* p_progress;
};

#endif
