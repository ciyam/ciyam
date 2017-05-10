// Copyright (c) 2017 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef ARRAY_H
#  define ARRAY_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <iosfwd>
#     include <string>
#  endif

#  include "macros.h"

#  ifdef CIYAM_BASE_LIB
#     ifdef CIYAM_BASE_IMPL
#        define ARRAY_DECL_SPEC DYNAMIC_EXPORT
#     else
#        define ARRAY_DECL_SPEC DYNAMIC_IMPORT
#     endif
#  else
#     define ARRAY_DECL_SPEC
#  endif

class ARRAY_DECL_SPEC array
{
   public:
   array( const std::string& type_and_or_state );

   std::string get_state( ) const { return ch + type + ':' + data; }

   void output_data( std::ostream& os );
   void place_chars( const std::string& chars );

   size_t get_ch_pos( );

   void move_up( );
   void move_down( );
   void move_left( );
   void move_right( );

   void rotate_forwards( );
   void rotate_backwards( );

   void perform_moves( const std::string& moves );

   private:
   char ch;

   size_t width;
   size_t height;

   std::string type;
   std::string data;
};

#endif

