// Copyright (c) 2017 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef CUBE_H
#  define CUBE_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <iosfwd>
#     include <string>
#  endif

#  include "macros.h"

#  ifdef CIYAM_BASE_LIB
#     ifdef CIYAM_BASE_IMPL
#        define CUBE_DECL_SPEC DYNAMIC_EXPORT
#     else
#        define CUBE_DECL_SPEC DYNAMIC_IMPORT
#     endif
#  else
#     define CUBE_DECL_SPEC
#  endif

class CUBE_DECL_SPEC cube
{
   public:
   cube( const std::string& type_and_or_state );

   void init( const std::string& state );

   void reset( );
   void scramble( std::ostream* p_os = 0 );

   std::string get_state( ) const;

   void output_sides( std::ostream& os ) const;

   void output_top_side( std::ostream& os ) const;
   void output_back_side( std::ostream& os ) const;
   void output_left_side( std::ostream& os ) const;
   void output_front_side( std::ostream& os ) const;
   void output_right_side( std::ostream& os ) const;
   void output_bottom_side( std::ostream& os ) const;

   void output_side( std::ostream& os, const std::string& name ) const;

   void output_matching_cubie( std::ostream& os, const std::string& cubie ) const;
   void output_matching_cubies( std::ostream& os, const std::string& color ) const;

   void flip( );

   void rotate( const std::string& op );

   void rotate_top( const std::string& op );
   void rotate_back( const std::string& op );
   void rotate_left( const std::string& op );
   void rotate_front( const std::string& op );
   void rotate_right( const std::string& op );
   void rotate_bottom( const std::string& op );

   void perform_rotations( const std::string& ops );

   private:
   std::string top;
   std::string bot;
   std::string lft;
   std::string rgt;
   std::string fnt;
   std::string bck;

   std::string type;
   std::string initial;

   void swap_row( std::string& lhs, std::string& rhs, int num );
   void swap_column( std::string& lhs, std::string& rhs, int lnum, int rnum = -1 );
   void swap_row_with_column( std::string& lhs, std::string& rhs, int rnum, int cnum = - 1 );

   void reverse_row( std::string& data, int num );
   void reverse_column( std::string& data, int num );

   void rotate_face( std::string& data, bool clockwise );

   void output_side_info( std::ostream& os, const std::string& name, const std::string& data ) const;
};

#endif

