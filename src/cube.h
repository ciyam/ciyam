// Copyright (c) 2017-2025 CIYAM Developers
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
#  include "op_algo_handler.h"

const size_t c_cube_default_max_tries = 3;
const size_t c_cube_default_max_attempts = 5;
const size_t c_cube_default_max_suggests = 8;
const size_t c_cube_default_train_rounds = 300;

class cube : public op_algo_handler
{
   public:
   cube( const std::string& type_and_or_state );

   op_algo_handler* create_clone( ) const;

   void init( const std::string& state );

   void init_random( ) { scramble( ); }

   void reset( );

   void scramble( std::ostream* p_os = 0, size_t num_moves = 0, bool actually_scramble = true );

   std::string scramble_moves( size_t num_moves = 0, bool for_actual_scramble = false ) const;

   std::string type_key( ) const { return type; }

   std::string get_state( bool include_initial = true ) const;

   std::string current_state( ) const { return get_state( false ); }

   size_t default_max_rounds( ) const { return c_cube_default_max_tries; }
   size_t default_max_op_tries( ) const { return c_cube_default_max_tries; }
   size_t default_max_attempt_ops( ) const { return c_cube_default_max_attempts; }
   size_t default_max_suggestions( ) const { return c_cube_default_max_suggests; }
   size_t default_num_train_rounds( ) const { return c_cube_default_train_rounds; }

   std::string step_repeat_suffix( size_t num_repeats ) const { return num_repeats == 3 ? "'" : ""; }

   bool solved( ) const;

   bool is_final_state( ) const { return solved( ); }

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

   void move( const std::string& op );

   void move_top( const std::string& op );
   void move_back( const std::string& op );
   void move_left( const std::string& op );
   void move_front( const std::string& op );
   void move_right( const std::string& op );
   void move_bottom( const std::string& op );

   void perform_moves( const std::string& ops );

   std::string random_ops( size_t num_ops ) const;

   void exec_ops( const std::string& ops ) { perform_moves( ops ); }

   std::string cleanup_output( const std::string& original ) const;

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

