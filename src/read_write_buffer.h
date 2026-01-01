// Copyright (c) 2004-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef READ_WRITE_BUFFER_H
#  define READ_WRITE_BUFFER_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <vector>
#  endif

class read_buffer
{
   public:
   read_buffer( std::size_t capacity );

   virtual ~read_buffer( ) { }

   void read_data( unsigned char* p_output, std::size_t len );

   virtual void read_available( ) = 0;

   bool exhausted( ) const { return buffered == retrieved; }

   std::size_t used( ) const { return buffered; }
   std::size_t fetched( ) const { return retrieved; }
   std::size_t available( ) const { return buffered - retrieved; }

   const unsigned char* buf( ) const { return &buffer[ 0 ]; }

   private:
   read_buffer( const read_buffer& );
   read_buffer& operator =( const read_buffer& );

   protected:
   std::size_t buffered;
   std::size_t capacity;
   std::size_t retrieved;

   std::vector< unsigned char > buffer;
};

class write_buffer
{
   public:
   write_buffer( std::size_t capacity, std::size_t reserved = 0 );

   virtual ~write_buffer( ) { }

   void write_data( const unsigned char* p_input, std::size_t len );

   virtual void flush_data( ) = 0;

   bool full( ) const { return buffered == capacity; }
   bool empty( ) const { return buffered == 0; }

   std::size_t used( ) const { return buffered; }
   std::size_t available( ) const { return capacity - buffered; }

   const unsigned char* buf( ) const { return &buffer[ 0 ]; }

   private:
   write_buffer( const write_buffer& );
   write_buffer& operator =( const write_buffer& );

   protected:
   std::size_t buffered;
   std::size_t reserved;
   std::size_t capacity;

   std::vector< unsigned char > buffer;
};

class read_write_buffer : public read_buffer, public write_buffer
{
   public:
   read_write_buffer( std::size_t capacity, std::size_t reserved = 0 )
    : read_buffer( capacity ), write_buffer( capacity, reserved )
   {
   }
};

#endif

