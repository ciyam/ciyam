// Copyright (c) 2017 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <cmath>
#  include <map>
#  include <memory>
#  include <vector>
#  include <fstream>
#  include <iostream>
#  include <stdexcept>
#endif

#include "op_algo_handler.h"

#include "utilities.h"

//#define DEBUG

using namespace std;

namespace
{

const size_t c_max_rounds_allowed = 100000000;

const char* const c_act_kill = "kill";
const char* const c_act_load = "load";
const char* const c_act_save = "save";

const char* const c_goal_found = "found";

const char* const c_type_separator = ":";
const char* const c_prefix_separator = "-";

bool char_match( char lhs, char rhs, char* p_sch = 0 )
{
   bool match = false;

   if( lhs == '.' )
      match = ( p_sch != 0 );
   else if( lhs == '?' || lhs == '!' || lhs == '`' )
   {
      if( p_sch )
      {
         // NOTE: Using the ` character forces re-capture for comparison
         // so it's possible to support the matching of sets that aren't
         // all the same (e.g. ..`?....`?.. will match two pairs such as
         // ..XX....YY.. whereas ..??....??.. matches ..XX....XX..).
         if( *p_sch == '\0' || lhs == '`' )
         {
            *p_sch = lhs;
            match = true;
         }
         else
         {
            if( lhs == '?' )
               match = ( lhs == *p_sch );
            else
               match = ( lhs != *p_sch );
         }
      }
   }
   else
   {
      if( islower( lhs ) )
         match = toupper( lhs ) != rhs;
      else
         match = ( lhs == rhs );
   }

   return match;
}

bool has_found_goal( const string& str, const string& goal )
{
   bool found = true;

   char sch = '\0';

   for( size_t i = 0; i < min( str.length( ), goal.length( ) ); i++ )
   {
      if( !char_match( goal[ i ], str[ i ], &sch ) )
      {
         found = false;
         break;
      }
   }

   return found;
}

bool has_partial_goal( const string& str, const string& goal )
{
   bool has_partial = false;

   for( size_t i = 0; i < min( str.length( ), goal.length( ) ); i++ )
   {
      if( char_match( str[ i ], goal[ i ] ) )
      {
         has_partial = true;
         break;
      }
   }

   return has_partial;
}

bool close_to_goal(
 const string& new_state, const string& old_state,
 const string& goal, bool must_be_closer_to_goal = true )
{
   int new_matches = 0;
   int old_matches = 0;

   size_t min_state_length( min( new_state.length( ), old_state.length( ) ) );

   for( size_t i = 0; i < min( min_state_length, goal.length( ) ); i++ )
   {
      if( char_match( goal[ i ], new_state[ i ] ) )
         ++new_matches;

      if( char_match( goal[ i ], old_state[ i ] ) )
         ++old_matches;
   }

   return must_be_closer_to_goal ? new_matches > old_matches : new_matches >= old_matches;
}

bool is_closer_to_goal( const string& new_state, const string& old_state, const string& goal )
{
   return close_to_goal( new_state, old_state, goal );
}

bool is_as_close_to_goal( const string& new_state, const string& old_state, const string& goal )
{
   return close_to_goal( new_state, old_state, goal, false );
}

bool matches_pattern( const string& pat, const string& str )
{
   bool matches = true;

   char sch = '\0';

   for( size_t i = 0; i < min( str.length( ), pat.length( ) ); i++ )
   {
      if( !char_match( pat[ i ], str[ i ], &sch ) )
      {
         matches = false;
         break;
      }
   }

   return matches;
}

string expand_pattern( const op_algo_handler& handler, const string& algo )
{
   string algx( algo );
   string::size_type pos = algx.find( '*' );

   while( pos != string::npos )
   {
      string random_op( handler.random_ops( 1 ) );

      algx.erase( pos, 1 );
      algx.insert( pos, random_op );

      pos = algx.find( '*', pos + random_op.length( ) );
   }

   return algx;
}

void parse_stage_info( const string& stage_info, string& stage_name, string& stage_pattern )
{
   string::size_type pos = stage_info.find( ' ' );

   if( pos == string::npos )
      throw runtime_error( "unexpected stage info '" + stage_info + "'" );

   stage_name = stage_info.substr( 0, pos );
   stage_pattern = stage_info.substr( pos + 1 );
}

string g_algo_prefix;

multimap< string, string > g_goal_algos;

}

void op_algo_handler::suggest( ostream& os, const string& info )
{
   int num_suggests = 0;
   bool has_had_output = false;

   string stop_at_stage;

   auto_ptr< op_algo_handler > ap_ops_handler( create_clone( ) );

#ifdef DEBUG
   cout << "[suggest] " << info << endl;
#endif
   while( !info.empty( ) )
   {
      string output;
      vector< string > lines;

      auto_ptr< op_algo_handler > ap_tmp_handler( ap_ops_handler->create_clone( ) );

      if( info[ 0 ] != '@' )
         lines.push_back( info );
      else
      {
         string filename( info.substr( 1 ) );

         string::size_type pos = filename.find( ':' );
         if( pos != string::npos )
         {
            stop_at_stage = filename.substr( pos + 1 );

            if( stop_at_stage.length( ) > 2 && stop_at_stage.substr( 0, 2 ) == "?:" )
               stop_at_stage.erase( 0, 2 );

            filename.erase( pos );
         }

         buffer_file_lines( filename, lines, false );
      }

      size_t start_from = 0;

      string last_pattern;

      bool last_was_found = false;
      bool ignore_next_for_start = false;

      map< int, bool > continuations;
      map< int, string > continuation_patterns;

      string stage_reached;

      // NOTE: Find the last goal that has already been reached to be
      // the starting point for algo suggestions.
      for( size_t i = 0; i < lines.size( ); i++ )
      {
         string next_line( lines[ i ] );

         continuations[ i ] = false;
         continuation_patterns[ i ] = string( );

         if( next_line.empty( ) || next_line[ 0 ] == '#' )
            continue;

         if( next_line[ 0 ] == ':' )
         {
            string stage_name, stage_pattern;
            parse_stage_info( next_line.substr( 1 ), stage_name, stage_pattern );

            if( has_found_goal( ap_ops_handler->current_state( ), stage_pattern ) )
            {
               stage_reached = stage_name;

               if( stage_reached == stop_at_stage )
                  break;
            }

            continue;
         }

         if( next_line[ 0 ] == '!' )
         {
            ignore_next_for_start = true;
            continue;
         }

         string::size_type pos = next_line.find( ' ' );
         if( pos != string::npos )
         {
            string state( ap_ops_handler->current_state( ) );
            string pattern( next_line.substr( 0, pos ) );

            if( pattern.find_first_not_of( "." ) == string::npos )
            {
               continuations[ i ] = true;
               continuation_patterns[ i ] = last_pattern;

               continue;
            }
            else
            {
               last_pattern = pattern;

                if( !ignore_next_for_start
                 && pattern.length( ) == state.length( ) && has_found_goal( state, pattern ) )
                  start_from = i;
            }

            if( !ignore_next_for_start
             && has_found_goal( state, next_line.substr( pos + 1 ) ) )
               start_from = i;

            ignore_next_for_start = false;
         }
      }

      if( !stop_at_stage.empty( ) && stage_reached == stop_at_stage )
         break;

      int num_found = 0;
      bool had_any_ops = false;

      size_t max_suggestions = default_max_suggestions( );

#ifdef DEBUG
      cout << "starting from line # " << ( start_from + 1 ) << endl;
      cout << "starting from stage: " << stage_reached << endl;
      cout << "stopping upon stage: " << stop_at_stage << endl;
#endif
      for( size_t i = start_from; i < lines.size( ); i++ )
      {
         string next_line( lines[ i ] );

         if( next_line.empty( ) || next_line[ 0 ] == '#' || next_line[ 0 ] == '!' )
            continue;

         if( next_line[ 0 ] == ':' )
         {
            string stage_name, stage_pattern;
            parse_stage_info( next_line.substr( 1 ), stage_name, stage_pattern );

            if( has_found_goal( ap_ops_handler->current_state( ), stage_pattern ) )
            {
               stage_reached = stage_name;
#ifdef DEBUG
               cout << "reached stage: " << stage_reached << endl;
#endif
               if( stop_at_stage == "?:" )
                  stop_at_stage = stage_reached;

               if( stage_reached == stop_at_stage )
                  break;
            }

            continue;
         }

         // NOTE: A continuation re-uses the previous pattern to attempt
         // to reach a different goal but will not keep a partial match.
         string::size_type pos = next_line.find( ' ' );
         if( pos != string::npos )
         {
            string patterns( next_line.substr( 0, pos ) );

            if( patterns.find_first_not_of( "." ) == string::npos )
            {
               if( continuation_patterns[ i ].empty( ) )
                  throw runtime_error( "unexpected continuation without a continuation pattern" );

               next_line = continuation_patterns[ i ] + " " + next_line.substr( pos + 1 );
            }
         }

#ifdef DEBUG
         cout << "line # " << ( i + 1 ) << endl;
#endif
         last_was_found = false;
         bool found_next = false;

         size_t max_rounds = default_max_rounds( );

         while( true )
         {
            map< int, string > lengths_and_algos;

            for( size_t j = 0; j < max_rounds; j++ )
            {
               stringstream ss;

               if( ap_tmp_handler->suggest_algo( ss, next_line, j + 1, true, &found_next ) )
               {
                  string result( ss.str( ) );

                  if( result == c_goal_found )
                  {
                     found_next = true;
                     ++num_found;
                     break;
                  }

                  if( !result.empty( ) )
                  {
                     if( found_next )
                        lengths_and_algos.clear( );

                     if( found_next || i == lines.size( ) - 1 || !continuations[ i + 1 ] )
                        lengths_and_algos.insert( make_pair( result.length( ), result ) );

                     if( found_next )
                        break;
                  }
               }
            }

            if( !lengths_and_algos.empty( ) )
            {
               string ops( lengths_and_algos.begin( )->second );

               if( had_any_ops )
                  output += ' ';
               output += ops;

               had_any_ops = true;

#ifdef DEBUG
               cout << "ops: " << ops << endl;
#endif
               ap_tmp_handler->exec_ops( ops );

               ap_ops_handler.reset( ap_tmp_handler->create_clone( ) );
#ifdef DEBUG
               cout << "state " << ap_tmp_handler->current_state( ) << endl;
#endif
            }
            else if( !found_next )
               break;

            if( found_next )
               break;
         }

         last_was_found = found_next;
      }

      if( num_found >= lines.size( ) )
      {
         os << c_goal_found;
         break;
      }

      if( output.empty( ) || ++num_suggests >= max_suggestions )
         break;
      else
      {
         // NOTE: It's possible that unwanted repeats can occur
         // due to the algorithm beginning with an op which had
         // been the last op from the previous algorithm (or in
         // fact an exact opposite op which should cancel out).
         output = cleanup_output( output );

         if( output.empty( ) )
            break;

         if( has_had_output )
            os << ' ';
         os << output;

         has_had_output = true;
      }

      if( ap_ops_handler->is_final_state( )
       || ( !stop_at_stage.empty( ) && stage_reached == stop_at_stage ) )
         break;

   }
}

bool op_algo_handler::suggest_algo( ostream& os,
 const string& info, size_t rounds, bool check_only_after_last_round, bool* p_found )
{
   bool rc = false;
   bool was_found = false;

   string::size_type pos = info.find( ' ' );
   if( pos == string::npos )
      throw runtime_error( "unexpected info '" + info + "'" );

   string pat( info.substr( 0, pos ) );
   string goal( info.substr( pos + 1 ) );

   string state( current_state( ) );

   if( p_found )
      *p_found = false;

#ifdef DEBUG
   cout << "goal: " << goal << endl;
   cout << "state " << state << endl;
#endif
   if( has_found_goal( state, goal ) )
   {
      rc = was_found = true;

      if( p_found )
         *p_found = true;

      os << c_goal_found;
   }
   else if( !goal.empty( ) )
   {
      auto_ptr< op_algo_handler > ap_best_handler( create_clone( ) );

      multimap< string, string >::iterator mi;
      string prefix( g_algo_prefix + type_key( ) + c_type_separator );

      string best_match, best_partial;
      string goal_algo_key( prefix + goal );

      mi = g_goal_algos.lower_bound( goal_algo_key );

      while( mi != g_goal_algos.end( ) )
      {
         if( mi->first != goal_algo_key )
            break;

         size_t max_rounds = default_max_rounds( );

         string algo( mi->second );

         pos = algo.find( '=' );
         if( pos != string::npos )
         {
            max_rounds = from_string< size_t >( algo.substr( 0, pos ) );
            algo.erase( 0, pos + 1 );
         }

         auto_ptr< op_algo_handler > ap_tmp_handler( create_clone( ) );
         auto_ptr< op_algo_handler > ap_new_handler( ap_tmp_handler->create_clone( ) );

         replace( algo, ",", " " );

         bool okay = true;
         bool found = false;

         bool has_parts = algo.find( ' ' ) != string::npos;

         bool is_full_length_pattern = false;

         if( pat.length( ) == state.length( ) )
         {
            rounds = max_rounds = 1;
            is_full_length_pattern = true;
         }

         if( rounds <= max_rounds )
         {
            // NOTE: If using a full length pattern then it is expected
            // that the current state must match as if it were the goal.
            if( is_full_length_pattern )
               okay = has_found_goal( ap_tmp_handler->current_state( ), pat );
         }

         if( okay )
         {
            string output;

            for( size_t j = 0; j < rounds; j++ )
            {
               if( has_parts )
               {
                  if( j > 0 )
                     output += ' ';

                  output += algo;
               }

               ap_new_handler->exec_ops( algo );
#ifdef DEBUG
               cout << algo << endl;
               cout << "news: " << ap_new_handler->current_state( ) << endl;
#endif

               if( check_only_after_last_round && j != rounds - 1 )
                  continue;

               string repeat_suffix( ap_new_handler->step_repeat_suffix( j + 1 ) );

               if( j > 0 && repeat_suffix.empty( ) )
                  repeat_suffix = to_string( j + 1 );

               if( has_found_goal( ap_new_handler->current_state( ), goal ) )
               {
                  if( !has_parts )
                     output = algo + repeat_suffix;

                  rc = found = was_found = true;

                  if( best_match.empty( ) || output.length( ) < best_match.length( ) )
                     best_match = output;

#ifdef DEBUG
                  cout << "[found] best_match: " << best_match << endl;
#endif
                  if( p_found )
                     *p_found = true;

                  break;
               }
               else if( !is_full_length_pattern )
               {
                  if( is_closer_to_goal( ap_new_handler->current_state( ), ap_best_handler->current_state( ), goal ) )
                  {
                     ap_tmp_handler.reset( ap_new_handler->create_clone( ) );
                     ap_best_handler.reset( ap_new_handler->create_clone( ) );

                     if( !has_parts )
                        output = algo + repeat_suffix;

                     best_partial = output;
#ifdef DEBUG
                     cout << "[partial] best_partial: " << best_partial << endl;
#endif
                  }
                  else if( !best_partial.empty( )
                     && is_as_close_to_goal( ap_new_handler->current_state( ), ap_best_handler->current_state( ), goal ) )
                  {
                     if( !has_parts )
                        output = algo + repeat_suffix;

                     if( output.length( ) < best_partial.length( ) )
                     {
                        best_partial = output;

                        ap_tmp_handler.reset( ap_new_handler->create_clone( ) );
                        ap_best_handler.reset( ap_new_handler->create_clone( ) );

#ifdef DEBUG
                        cout << "[partial] best_partial: " << best_partial << endl;
#endif
                     }
                  }
               }
            }
         }

         if( was_found )
            break;

         ++mi;
      }

      if( was_found )
      {
         rc = true;
         os << best_match;
      }
      else if( best_partial.empty( ) )
         rc = false;
      else
      {
         rc = true;
         os << best_partial;
      }
   }

   return rc;
}

void op_algo_handler::train( const string& info )
{
   if( !info.empty( ) )
   {
      vector< string > lines;

      if( info[ 0 ] != '@' )
         lines.push_back( info );
      else
         buffer_file_lines( info.substr( 1 ), lines, false );

      // NOTE: If multiple lines then first erase any previous training.
      if( lines.size( ) > 1 )
      {
         multimap< string, string >::iterator mi;
         string type_key_prefix( g_algo_prefix + type_key( ) + c_type_separator );

         while( true )
         {
            mi = g_goal_algos.lower_bound( type_key_prefix );

            if( mi == g_goal_algos.end( ) || mi->first.find( type_key_prefix ) != 0 )
               break;

            g_goal_algos.erase( mi );
         }
      }

      for( size_t i = 0; i < lines.size( ); i++ )
      {
         string next_line( lines[ i ] );

         if( next_line.empty( ) || next_line[ 0 ] == '#' )
            continue;

         vector< string > train_args;
         split( next_line, train_args, ' ' );

         if( train_args.size( ) < 3 )
            throw runtime_error( "unexpected train args < 3" );

         if( train_args.size( ) == 3 )
            train_algo( train_args[ 0 ], train_args[ 1 ], train_args[ 2 ] );
         else if( train_args.size( ) == 4 )
            train_algo( train_args[ 0 ],
             train_args[ 1 ], train_args[ 2 ], from_string< size_t >( train_args[ 3 ] ) );
         else
            train_algo( train_args[ 0 ], train_args[ 1 ],
             train_args[ 2 ], from_string< size_t >( train_args[ 3 ] ), from_string< size_t >( train_args[ 4 ] ) );
      }
   }
}

void op_algo_handler::train_algo( const string& pat,
 const string& goal, const string& algo, size_t rounds,
 size_t max_tries_allowed, bool* p_can_keep, bool* p_found_match )
{
   auto_ptr< op_algo_handler > ap_tmp_handler( create_clone( ) );

   string mask;
   vector< string > all_masks;

   if( pat.empty( ) )
      throw runtime_error( "invalid empty pat for test" );

   if( !rounds )
      rounds = default_num_train_rounds( );

   size_t max_tries = rounds;

   if( !max_tries_allowed )
      max_tries_allowed = default_max_op_tries( );

   if( p_can_keep )
      *p_can_keep = false;

   bool found_match = false;
   bool found_repeat = false;
   bool found_partial = false;

   vector< string > patterns;
   split( pat, patterns, '|' );

   bool is_full_state = false;
   bool using_random_patterns = false;

   if( algo.find( '*' ) != string::npos )
      using_random_patterns = true;
   else
      ap_tmp_handler->init_random( );

   string start( ap_tmp_handler->current_state( ) );

   if( goal.length( ) < start.length( ) )
      throw runtime_error( "invalid goal length < start length" );

   // NOTE: If there is only one pattern whose length matches the goal's
   // then it's expected that the current state will match this pattern.
   if( patterns.size( ) == 1 && patterns[ 0 ].size( ) == goal.size( ) )
   {
      is_full_state = true;

      ap_tmp_handler->init( patterns[ 0 ] );

      if( !using_random_patterns )
      {
         rounds = 1;
         start = goal;
      }

      string state( ap_tmp_handler->current_state( ) );

      for( size_t i = 0; i < goal.size( ); i++ )
      {
         if( goal[ i ] == '.' || goal[ i ] == state[ i ] )
            mask += '-';
         else
            mask += '*';
      }
   }

   found_match = has_found_goal( start, goal );
   found_partial = has_partial_goal( start, goal );

   if( rounds > c_max_rounds_allowed )
      throw runtime_error( "max rounds allowed for train_algo exceeded" );

   string algx( algo );

#ifdef DEBUG
   cout << "algo = " << algo << endl;
   cout << "pat >>\n" << pat << endl;
#endif
   for( size_t i = 0; i < rounds; i++ )
   {
      if( using_random_patterns )
      {
         ap_tmp_handler->init( start );
         algx = expand_pattern( *ap_tmp_handler, algo );
      }

      ap_tmp_handler->exec_ops( algx );

      string next( ap_tmp_handler->current_state( ) );

      if( next == start || using_random_patterns )
      {
         if( !found_repeat && !using_random_patterns )
         {
            max_tries = i;

            if( i == 0 )
               ++max_tries;
            else
               found_repeat = true;
         }
         else if( using_random_patterns )
         {
            max_tries = 1;
            found_match = has_found_goal( next, goal );
         }

         if( found_match )
            replace( mask, "?", "*" );
         else if( !found_partial )
            replace( mask, "?", "-" );

         all_masks.push_back( mask );

         if( !using_random_patterns )
         {
            if( is_full_state )
               break;

            mask.erase( );
            found_partial = false;

            ap_tmp_handler->init_random( );
            next = start = ap_tmp_handler->current_state( );

            found_match = has_found_goal( start, goal );
            found_partial = has_partial_goal( next, goal );
         }
      }

      if( using_random_patterns )
      {
         if( found_match )
            break;
      }
      else
      {
         if( is_full_state )
            break;

         bool had_match = false;
         bool had_partial = false;

         for( size_t j = 0; j < goal.length( ); j++ )
         {
            bool potential = false;

            for( size_t k = 0; k < patterns.size( ); k++ )
            {
               string next_pat( patterns[ k ] );

               if( next_pat == "*" )
               {
                  potential = true;
                  break;
               }

               if( j <= next.length( ) - next_pat.length( ) )
               {
                  if( matches_pattern( next_pat, next.substr( j, next_pat.length( ) ) ) )
                  {
                     potential = true;
                     break;
                  }
               }
            }

            if( goal[ j ] != '.' )
            {
               if( next[ j ] != goal[ j ] )
                  had_match = false;
               else
                  had_partial = true;
            }

            if( mask.length( ) < goal.length( ) )
            {
               if( goal[ j ] == '.' )
                  mask += '-';
               else
               {
                  if( next[ j ] == goal[ j ] )
                     mask += '*';
                  else if( potential )
                     mask += '?';
                  else
                     mask += '-';
               }
            }
            else
            {
               if( goal[ j ] == '.' )
                  mask[ j ] = '-';
               else
               {
                  if( next[ j ] == goal[ j ] )
                     mask[ j ] = '*';
                  else if( potential )
                     mask[ j ] = '?';
                  else
                     mask[ j ] = '-';
               }
            }
         }

         if( had_partial )
            found_partial = true;
      }
   }

   if( !is_full_state )
   {
      replace( mask, "?", "-" );
      replace( mask, "*", "-" );

      vector< size_t > weights( mask.size( ) );

      size_t total_weight = 0;

      for( size_t i = 0; i < all_masks.size( ); i++ )
      {
         for( size_t j = 0; j < all_masks[ i ].size( ); j++ )
         {
            size_t weight = 0;

            if( all_masks[ i ][ j ] == '?' )
               weight = 1;
            else if( all_masks[ i ][ j ] == '*' )
               weight = 2;

            total_weight += weight;
            weights[ j ] = weights[ j ] + weight;
         }
      }

      size_t median = total_weight / weights.size( );

      mask.erase( );
      for( size_t i = 0; i < weights.size( ); i++ )
      {
         if( weights[ i ] > median )
            mask += "*";
         else
            mask += "-";
      }
   }

#ifdef DEBUG
   if( using_random_patterns )
   {
      if( found_match )
         cout << algx << endl;
   }
   cout << goal << endl;
   cout << mask << endl;
#endif

   if( p_found_match )
      *p_found_match = found_match;

   if( mask.find( '*' ) != string::npos )
   {
      if( p_can_keep )
         *p_can_keep = true;
      else
      {
         size_t tries = min( max_tries, max_tries_allowed );

         string prefix( g_algo_prefix + type_key( ) + c_type_separator );
         g_goal_algos.insert( make_pair( prefix + goal, to_string( tries ) + '=' + algx ) );
      }
   }
}

void op_algo_handler::attempt( ostream& os, const string& info )
{
   vector< string > attempt_args;
   split( info, attempt_args, ' ' );

   if( attempt_args.size( ) < 2 )
      throw runtime_error( "attempt args < 2" );

   if( attempt_args.size( ) == 2 )
      attempt_own_algo( os, attempt_args[ 0 ], attempt_args[ 1 ] );
   else
      attempt_own_algo( os, attempt_args[ 0 ], attempt_args[ 1 ], from_string< size_t >( attempt_args[ 2 ] ) );
}

void op_algo_handler::attempt_own_algo( ostream& os, const string& pat, const string& goal, size_t max_attempt_ops )
{
   bool can_keep = false;
   bool found_match = false;

   string last_retained;

   size_t max_op_tries = default_max_op_tries( );

   if( !max_attempt_ops )
      max_attempt_ops = default_max_attempt_ops( );

   for( size_t i = 1; i <= max_attempt_ops; i++ )
   {
      for( size_t j = 0; j < 100; j++ )
      {
         string next_algo( random_ops( i ) );

         size_t rounds = default_num_train_rounds( );

         // NOTE: For algorithms with more than 3 ops extend the number of rounds.
         if( i > 3 )
            rounds *= ( i - 2 );

         train_algo( pat, goal, next_algo, rounds, max_op_tries, &can_keep, &found_match );

         if( can_keep )
         {
            auto_ptr< op_algo_handler > ap_tmp_handler( create_clone( ) );

            ap_tmp_handler->exec_ops( next_algo );

            if( is_closer_to_goal( ap_tmp_handler->current_state( ), current_state( ), goal ) )
            {
               last_retained = next_algo;
               break;
            }
         }

         if( found_match )
            break;

         // FUTURE: Rather than being hard-coded these break points
         // should be determined by each concrete "op algo" class.
         if( i == 1 && j >= 5 )
            break;

         if( i == 2 && j >= 10 )
            break;

         if( i == 3 && j >= 25 )
            break;

         if( i == 4 && j >= 50 )
            break;
      }

      if( found_match || !last_retained.empty( ) )
         break;
   }

   if( !last_retained.empty( ) )
      os << last_retained;
}

void op_algo_handler::output_algos( ostream& os )
{
   ::output_algos( os, type_key( ), false );
}

temporary_algo_prefix::temporary_algo_prefix( const string& prefix )
{
   old_prefix = g_algo_prefix;
   g_algo_prefix = prefix + c_prefix_separator;
}

temporary_algo_prefix::~temporary_algo_prefix( )
{
   g_algo_prefix = old_prefix;
}

void output_algos( ostream& os )
{
   string retval;

   multimap< string, string >::iterator mi;

   mi = g_goal_algos.lower_bound( g_algo_prefix );

   while( mi != g_goal_algos.end( ) )
   {
      if( !g_algo_prefix.empty( ) && mi->first.find( g_algo_prefix ) != 0 )
         break;

      os << mi->first.substr( g_algo_prefix.length( ) ) << " " << mi->second << '\n';

      ++mi;
   }
}

void output_algos( ostream& os, const string& type_keys, bool include_type )
{
   if( type_keys.empty( ) )
      output_algos( os );
   else
   {
      vector< string > types;
      split( type_keys, types );

      for( size_t i = 0; i < types.size( ); i++ )
      {
         string next_type_key( types[ i ] );
         string type_key_prefix( g_algo_prefix + next_type_key + c_type_separator );

         multimap< string, string >::iterator mi;

         mi = g_goal_algos.lower_bound( type_key_prefix );

         while( mi != g_goal_algos.end( ) )
         {
            if( mi->first.find( type_key_prefix ) != 0 )
               break;

            if( include_type )
               os << mi->first.substr( g_algo_prefix.length( ) );
            else
               os << mi->first.substr( type_key_prefix.length( ) );

            os << " " << mi->second << '\n';

            ++mi;
         }
      }
   }
}

void exec_algos_action( const string& act, const string& info_1, const string& info_2 )
{
   if( act == c_act_kill )
   {
      vector< string > types;
      split( info_1, types );

      for( size_t i = 0; i < types.size( ); i++ )
      {
         string next_type_key( types[ i ] );
         string type_key_prefix( g_algo_prefix );

         if( next_type_key != "*" )
            type_key_prefix += next_type_key + c_type_separator;

         multimap< string, string >::iterator mi;

         while( true )
         {
            mi = g_goal_algos.lower_bound( type_key_prefix );

            if( mi == g_goal_algos.end( ) || mi->first.find( type_key_prefix ) != 0 )
               break;

            g_goal_algos.erase( mi );
         }
      }
   }
   else if( act == c_act_load )
   {
      ifstream inpf( info_1.c_str( ) );
      if( !inpf )
         throw runtime_error( "unable to open '" + info_1 + "' for input" );

      string next;
      while( getline( inpf, next ) )
      {
         if( next.empty( ) )
            continue;

         string::size_type pos = next.find( ' ' );
         if( pos == string::npos )
            throw runtime_error( "unexpected algo line format '" + next + "'" );

         string type_key_and_goal( next.substr( 0, pos ) );
         string maximum_rounds_and_algo( next.substr( pos + 1 ) );

         pos = type_key_and_goal.find( c_type_separator );
         if( pos == string::npos )
            throw runtime_error( "unexpected type_key_and_goal value '" + type_key_and_goal + "'" );

         string type_key_prefix( g_algo_prefix + type_key_and_goal.substr( 0, pos + 1 ) );
         string goal( type_key_and_goal.substr( pos + 1 ) );

         g_goal_algos.insert( make_pair( type_key_prefix + goal, maximum_rounds_and_algo ) );
      }
   }
   else if( act == c_act_save )
   {
      ofstream outf( info_1.c_str( ) );
      if( !outf )
         throw runtime_error( "unable to open '" + info_1 + "' for output" );

      output_algos( outf, info_2 );
   }
   else
      throw runtime_error( "unknown output_algos act: " + act );
}

