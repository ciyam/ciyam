// Copyright (c) 2017-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef OP_ALGO_HANDLER_H
#  define OP_ALGO_HANDLER_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <iosfwd>
#     include <string>
#     include <vector>
#  endif

class op_algo_handler
{
   public:
   virtual ~op_algo_handler( ) { }

   virtual op_algo_handler* create_clone( ) const = 0;

   virtual void init( const std::string& state ) = 0;

   virtual void init_random( ) = 0;

   virtual std::string type_key( ) const = 0;
   virtual std::string current_state( ) const = 0;

   virtual size_t default_max_rounds( ) const = 0;
   virtual size_t default_max_op_tries( ) const = 0;
   virtual size_t default_max_attempt_ops( ) const = 0;
   virtual size_t default_max_suggestions( ) const = 0;
   virtual size_t default_num_train_rounds( ) const = 0;

   virtual std::string step_repeat_suffix( size_t num_repeats ) const { return ""; }

   virtual bool is_final_state( ) const { return false; }

   virtual std::string random_ops( size_t num_ops ) const = 0;

   virtual void exec_ops( const std::string& ops ) = 0;

   virtual std::string cleanup_output( const std::string& original ) const { return original; }

   void suggest( std::ostream& os, const std::string& info );

   bool suggest_algo( std::ostream& os,
    const std::string& info, std::vector< std::string >* p_prefix_ops = 0,
    size_t rounds = 1, bool check_only_after_last_round = false, bool* p_found = 0 );

   void train( const std::string& info );

   void train_algo( const std::string& pat,
    const std::string& goal, const std::string& algo, size_t rounds = 0,
    size_t max_tries_allowed = 0, bool* p_can_keep = 0, bool* p_found_match = 0 );

   void attempt( std::ostream& os, const std::string& info );

   void attempt_own_algo( std::ostream& os,
    const std::string& pat, const std::string& goal, size_t max_attempt_ops = 0 );

   void output_algos( std::ostream& os );
};

struct temporary_algo_prefix
{
   temporary_algo_prefix( const std::string& prefix );
   ~temporary_algo_prefix( );

   std::string old_prefix;
};

void output_algos( std::ostream& os );
void output_algos( std::ostream& os, const std::string& type_keys, bool include_type = true );

void exec_algos_action( const std::string& act, const std::string& info_1, const std::string& info_2 );

#endif

