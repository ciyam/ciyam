// Copyright (c) 2003-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <stdio.h>
#  include <string.h>
#  include <stdlib.h>
#  include <memory>
#  include <fstream>
#  include <iostream>
#  include <stdexcept>
#endif

#include "text_file_buffer.h"

using namespace std;

const size_t c_npos = ~0u;

const char* const c_diff_1 = "diff1";
const char* const c_diff_2 = "diff2";

const char* const c_opt_quiet = "-q";
const char* const c_opt_minimise = "-min";

//#define DEBUG

namespace
{

enum mode_t
{
   e_mode_nul,
   e_mode_add,
   e_mode_del
};

struct chunk_t
{
   size_t line;
   size_t size;
   size_t offs;

   mode_t mode;

   chunk_t( ) : line( 0 ), size( 0 ), offs( 0 ), mode( e_mode_nul ) { }
};

void dump_chunk( const chunk_t& chunk, ostream& outf )
{
   outf << "chunk.line = " << chunk.line << ", chunk.size = "
    << chunk.size << ", chunk.offs = " << chunk.offs << ", chunk.mode = " << chunk.mode << '\n';
}

void read_next_chunk( const char* p_diff_info, chunk_t& chunk, size_t line, size_t num_lines )
{
#ifdef DEBUG
   cout << "p_diff_info = " << p_diff_info << '\n';
#endif
   if( p_diff_info[ 0 ] == 0 )
      throw runtime_error( "unexpected empty diff info line" );

   if( p_diff_info[ 0 ] == 'd' )
      chunk.mode = e_mode_del;
   else if( p_diff_info[ 0 ] == 'a' )
      chunk.mode = e_mode_add;
   else
      throw runtime_error( "unexpected diff info operation" );

   ++p_diff_info;
   if( p_diff_info[ 0 ] == 0 )
      throw runtime_error( "unexpected truncated diff info line" );

   const char* p_num_lines = strchr( p_diff_info, ' ' );
   if( !p_num_lines || *( p_num_lines + 1 ) == '\0' )
      throw runtime_error( "unexpected truncated diff info line" );
   chunk.offs = 0;

   // KLUDGE: This "const" hack is being performed in order to simplify the "atoi" operations...
   ( char& )*p_num_lines++ = '\0';
   chunk.line = atoi( p_diff_info );
   chunk.size = atoi( p_num_lines );
   ( char& )*( p_num_lines - 1 ) = ' ';

   if( chunk.line > num_lines )
      throw runtime_error( "unexpected diff line overrun" );
   if( line > 1 && chunk.line < line - 1 )
      throw runtime_error( "unexpected diff line underrun" );
}

void output_differences( bool is_conflict, bool skip_original,
 vector< chunk_t >& chunks, size_t start, size_t finish, text_file_buffer& base, text_file_buffer& diff )
{
   size_t pos = start;
   for( vector< chunk_t >::size_type i = 0; i < chunks.size( ); i++ )
   {
      while( pos < chunks[ i ].line )
      {
         if( pos == 0 || !is_conflict )
            ++pos;
         else
         {
#ifdef DEBUG
            cout << "<d1>";
#endif
            cout << base[ pos++ - 1 ] << '\n';
         }
      }

      if( chunks[ i ].mode == e_mode_del )
         pos += chunks[ i ].size;
      else
      {
         if( pos == chunks[ i ].line )
         {
            if( pos == 0 )
               ++pos;
            else if( !skip_original )
            {
#ifdef DEBUG
               cout << "<d2>";
#endif
               cout << base[ pos++ - 1 ] << '\n';
            }
         }

         for( size_t j = 0; j < chunks[ i ].size; j++ )
         {
#ifdef DEBUG
            cout << "<d3>";
#endif
            cout << diff[ chunks[ i ].offs + j ] << '\n';
         }
      }
   }

   if( is_conflict )
   {
      while( pos <= finish )
      {
         if( pos == 0 )
            ++pos;
         else
         {
#ifdef DEBUG
            cout << "<d4>";
#endif
            cout << base[ pos++ - 1 ] << '\n';
         }
      }
   }
}

} // namespace

int main( int argc, char* argv[ ] )
{
   int rc = 0;
   int first_arg = 1;
   bool is_quiet = false;
   bool minimise_conflicts = false;

   if( argc == 4 && string( argv[ 1 ] ) == c_opt_quiet )
   {
      ++first_arg;
      is_quiet = true;
   }

   if( argc == 5 && string( argv[ 1 ] ) == c_opt_quiet )
   {
      ++first_arg;
      is_quiet = true;
   }

   if( argc == 6 && string( argv[ 1 ] ) == c_opt_quiet )
   {
      ++first_arg;
      is_quiet = true;
   }

   if( argc == 5 && string( argv[ 1 ] ) == c_opt_minimise )
   {
      ++first_arg;
      minimise_conflicts = true;
   }

   if( argc == 6 && string( argv[ 2 ] ) == c_opt_minimise )
   {
      ++first_arg;
      minimise_conflicts = true;
   }

   if( argc < 3 || argc > first_arg + 3 )
   {
      cout << "diffp v0.1i\n";
      cout << "Usage: diffp [" << c_opt_quiet << "] <file> <rcs diff>" << endl;
      cout << "   or: diffp [" << c_opt_quiet << "] [" << c_opt_minimise << "] <file> <rcs diff1> <rcs diff2>" << endl;
      return 0;
   }

   size_t xline = 0;
   string xname;
   bool had_conflict = false;

   try
   {
      text_file_buffer base( argv[ first_arg ] );
      text_file_buffer diff( argv[ first_arg + 1 ] );

      unique_ptr< text_file_buffer > up_diff_2( argc < first_arg + 3 ? 0 : new text_file_buffer( argv[ first_arg + 2 ] ) );

      size_t diff_line = 0;
      size_t diff_line_2 = 0;

      size_t base_lines = base.size( );
      size_t diff_lines = diff.size( );
      size_t diff_lines_2 = ( argc < first_arg + 3 ? 0 : up_diff_2->size( ) );

      if( !base_lines && !diff_lines && !diff_lines_2 )
      {
         if( argc < first_arg + 3 )
            cerr << "error: file and diff are both empty" << endl;
         else
            cerr << "error: file and both diffs are empty" << endl;
         return 1;
      }

      chunk_t diff_chunk;
      chunk_t diff_chunk_2;

      for( size_t line = 1; line <= base_lines; line++ )
      {
#ifdef DEBUG
         cout << "<< line = " << line << " >>\n";
         cout << "diff_line = " << diff_line << ", diff_line_2 = " << diff_line_2 << '\n';
         cout << "diff_lines = " << diff_lines << ", diff_lines_2 = " << diff_lines_2 << '\n';
#endif
         size_t start = c_npos;
         size_t finish = c_npos;

         vector< chunk_t > left_chunks;
         vector< chunk_t > right_chunks;

         size_t left_start = c_npos;
         size_t left_finish = c_npos;
         size_t right_start = c_npos;
         size_t right_finish = c_npos;

         bool has_left = false;
         bool has_right = false;
         bool has_left_add = false;
         bool has_right_add = false;

         bool has_more = true;

         while( has_more )
         {
#ifdef DEBUG
            cout << "[loop iteration]" << endl;
#endif
            has_more = false;
            if( diff_line != diff_lines )
            {
               xname = c_diff_1;
               xline = diff_line;
               read_next_chunk( diff[ diff_line ], diff_chunk, line, base_lines );

               left_start = diff_chunk.line;
               if( diff_chunk.mode == e_mode_add )
                  left_finish = diff_chunk.line;
               else
                  left_finish = left_start + diff_chunk.size - 1;

               if( left_start <= finish )
               {
#ifdef DEBUG
                  cout << "==> left_start = " << left_start << " (finish = " << finish << ")" << endl;
#endif
                  ++diff_line;
                  has_more = true;
                  has_left = true;

                  start = min( start, left_start );
                  if( finish == c_npos )
                     finish = left_finish;
                  else
                     finish = max( finish, left_finish );

                  if( diff_chunk.mode == e_mode_add )
                  {
                     has_left_add = true;
                     diff_chunk.offs = diff_line;
                     diff_line += diff_chunk.size;
                  }

                  left_chunks.push_back( diff_chunk );
               }
            }

            if( diff_line_2 != diff_lines_2 )
            {
               xname = c_diff_2;
               xline = diff_line_2;
               read_next_chunk( ( *up_diff_2 )[ diff_line_2 ], diff_chunk_2, line, base_lines );

               right_start = diff_chunk_2.line;
               if( diff_chunk_2.mode == e_mode_add )
                  right_finish = diff_chunk_2.line;
               else
                  right_finish = right_start + diff_chunk_2.size - 1;

               if( right_start <= finish )
               {
#ifdef DEBUG
                  cout << "==> right_start = " << right_start << " (finish = " << finish << ")" << endl;
#endif
                  ++diff_line_2;
                  has_more = true;
                  has_right = true;

                  if( has_left && left_chunks.size( ) == 1 && right_finish < left_start )
                  {
                     --diff_line;
                     if( has_left_add )
                        diff_line -= left_chunks.back( ).size;

                     has_left = false;
                     has_left_add = false;
                     left_chunks.pop_back( );

                     start = right_start;
                     finish = right_finish;
                  }
                  else
                  {
                     start = min( start, right_start );
                     if( finish == c_npos )
                        finish = right_finish;
                     else
                        finish = max( finish, right_finish );
                  }

                  if( diff_chunk_2.mode == e_mode_add )
                  {
                     has_right_add = true;
                     diff_chunk_2.offs = diff_line_2;
                     diff_line_2 += diff_chunk_2.size;
                  }

                  right_chunks.push_back( diff_chunk_2 );
               }
            }
         }

#ifdef DEBUG
         cout << "start = " << start << ", finish = " << finish << endl;
         cout << "has_left = " << has_left << ", has_right = " << has_right << endl;
#endif
         bool has_output = true;
         bool is_conflict = false;
         if( has_left && has_right && ( has_left_add || has_right_add ) )
            is_conflict = true;

         size_t trailing_matches = 0;
         size_t trailing_match_offset = c_npos;

         bool skip_left_original = false;
         bool skip_right_original = false;

         if( minimise_conflicts )
         {
            // NOTE: If either the left or right diff has deleted and then inserted at the same point
            // then if the other diff has inserted at the same point then rather than detect as being
            // a conflict the original lines will be omitted and the diff which deleted will have its
            // inserts output first followed the the other diff.
            if( left_chunks.size( ) == 1 && right_chunks.size( ) == 2 )
            {
               if( left_chunks[ 0 ].mode == e_mode_add
                && right_chunks[ 0 ].mode == e_mode_del && right_chunks[ 1 ].mode == e_mode_add )
               {
                  size_t insert_point = right_chunks[ 0 ].line + right_chunks[ 0 ].size - 1;
                  if( right_chunks[ 1 ].line == insert_point && left_chunks[ 0 ].line == insert_point )
                  {
                     is_conflict = false;
                     skip_left_original = true;
#ifdef DEBUG
                     cout << "<<< skip_left_original >>>" << endl;
#endif
                  }
               }
            }
            else if( left_chunks.size( ) == 2 && right_chunks.size( ) == 1 )
            {
               if( right_chunks[ 0 ].mode == e_mode_add
                && left_chunks[ 0 ].mode == e_mode_del && left_chunks[ 1 ].mode == e_mode_add )
               {
                  size_t insert_point = left_chunks[ 0 ].line + left_chunks[ 0 ].size - 1;
                  if( left_chunks[ 1 ].line == insert_point && right_chunks[ 0 ].line == insert_point )
                  {
                     is_conflict = false;
                     skip_right_original = true;
#ifdef DEBUG
                     cout << "<<< skip_right_original >>>" << endl;
#endif
                  }
               }
            }
         }

         if( is_conflict )
         {
#ifdef DEBUG
            cout << "************ is_conflict *************" << endl;
#endif
            while( start > 0 && line < start )
            {
#ifdef DEBUG
               cout << "<bd>";
#endif
               cout << base[ line++ - 1 ] << '\n';
            }

            if( minimise_conflicts )
            {
               // If both left and right begin adding a number of identical lines at the same spot
               // then output these first and don't include them in the marked differences area.
               if( left_chunks[ 0 ].line == right_chunks[ 0 ].line
                && left_chunks[ 0 ].mode == e_mode_add && right_chunks[ 0 ].mode == e_mode_add )
               {
#ifdef DEBUG
                  cout << "[first left and right additions match]" << endl;
#endif
                  size_t lo = left_chunks[ 0 ].offs;
                  size_t ro = right_chunks[ 0 ].offs;
                  size_t num = min( left_chunks[ 0 ].size, right_chunks[ 0 ].size );

                  // NOTE: Unless additions are at the start output the base line that preceeds them.
                  if( start > 0 )
                  {
                     ++start;
#ifdef DEBUG
                     cout << "<fd>";
#endif
                     cout << base[ line++ - 1 ] << '\n';
                  }

                  for( size_t i = 0; i < num; i++ )
                  {
                     if( strcmp( diff[ lo + i ], ( *up_diff_2 )[ ro + i ] ) != 0 )
                        break;

#ifdef DEBUG
                     cout << "<md>";
#endif
                     cout << diff[ lo + i ] << '\n';

                     ++left_chunks[ 0 ].offs;
                     ++right_chunks[ 0 ].offs;
                     --left_chunks[ 0 ].size;
                     --right_chunks[ 0 ].size;
                  }

                  // NOTE: If the only conflict was identical additions then it consider it resolved.
                  if( left_chunks.size( ) == 1 && right_chunks.size( ) == 1 )
                  {
                     if( !left_chunks[ 0 ].size && !right_chunks[ 0 ].size )
                     {
                        has_output = false;
                        is_conflict = false;
                     }
                  }
               }

               size_t ls = left_chunks.size( );
               size_t rs = right_chunks.size( );

               // If both left and right have a number of identical final additions then instead
               // of including them within the marked differences area output them afterwards.
               if( has_output
                && left_chunks[ ls - 1 ].size && right_chunks[ rs - 1 ].size
                && left_chunks[ ls - 1 ].line == right_chunks[ rs - 1 ].line
                && left_chunks[ ls - 1 ].mode == e_mode_add && right_chunks[ rs - 1 ].mode == e_mode_add )
               {
#ifdef DEBUG
                  cout << "[last left and right additions match]" << endl;
#endif
                  size_t left_size = left_chunks[ ls - 1 ].size;
                  size_t right_size = right_chunks[ rs - 1 ].size;
                  size_t minimum = min( left_size, right_size );

                  for( size_t i = 0; i < minimum; i++ )
                  {
                     if( strcmp( diff[ left_chunks[ ls - 1 ].offs + left_size - i - 1 ],
                      ( *up_diff_2 )[ right_chunks[ rs - 1 ].offs + right_size - i - 1 ] ) != 0 )
                        break;

                     --left_chunks[ ls - 1 ].size;
                     --right_chunks[ rs - 1 ].size;

                     ++trailing_matches;
                     trailing_match_offset = left_chunks[ ls - 1 ].offs + left_size - i - 1;
                  }

                  // NOTE: If conflict consisted of a matching delete followed by identical inserts
                  // then the conflict can be considered to have been resolved.
                  if( left_chunks.size( ) == 2 && right_chunks.size( ) == 2 )
                  {
                     if( left_chunks[ 0 ].size == right_chunks[ 0 ].size
                      && left_chunks[ 0 ].mode == e_mode_del && right_chunks[ 0 ].mode == e_mode_del )
                     {
                        if( !left_chunks[ 1 ].size && !right_chunks[ 1 ].size )
                        {
                           has_output = false;
                           is_conflict = false;
                        }
                     }
                  }
               }

               bool still_has_left = false;
               for( size_t i = 0; i < left_chunks.size( ); i++ )
               {
                  if( left_chunks[ i ].size )
                  {
                     still_has_left = true;
                     break;
                  }
               }

               bool still_has_right = false;
               for( size_t i = 0; i < right_chunks.size( ); i++ )
               {
                  if( right_chunks[ i ].size )
                  {
                     still_has_right = true;
                     break;
                  }
               }

               if( !still_has_left && !still_has_right )
               {
                  // NOTE: Leave "has_left" true to ensure that "output_differences" is called (otherwise the base
                  // line that duplicate left and right additions have occurred before would not itself be output).
                  has_right = false;
                  is_conflict = false;
               }
               else if( !still_has_left )
               {
                  has_left = false;
                  is_conflict = false;
               }
               else if( !still_has_right )
               {
                  has_right = false;
                  is_conflict = false;
               }
            }
         }

         if( start == c_npos )
         {
            if( line > 0 )
            {
#ifdef DEBUG
               cout << "<ns>";
#endif
               cout << base[ line - 1 ] << '\n';
            }
         }
         else
         {
            while( start > 0 && line < start )
            {
#ifdef DEBUG
               cout << "<bs>";
#endif
               cout << base[ line++ - 1 ] << '\n';
            }

            if( has_output && is_conflict && start > 0 && line <= finish )
               cout << "<<<<<< (0)\n";

            if( is_conflict && start > 0 )
            {
               while( line <= finish )
               {
#ifdef DEBUG
                  cout << "<bc>";
#endif
                  cout << base[ line++ - 1 ] << '\n';
               }
            }

            if( has_left && !skip_left_original )
            {
#ifdef DEBUG
               cout << "left_chunks.size( ) = " << left_chunks.size( ) << endl;
#endif
               if( is_conflict && has_output )
                  cout << ">>>>>> (1)\n";

               output_differences( is_conflict, false, left_chunks, start, finish, base, diff );
            }

            if( has_right )
            {
#ifdef DEBUG
               cout << "right_chunks.size( ) = " << right_chunks.size( ) << endl;
#endif
               if( is_conflict && has_output )
                  cout << ">>>>>> (2)\n";

               output_differences( is_conflict,
                skip_right_original, right_chunks, start, finish, base, *up_diff_2 );
            }

            if( has_left && skip_left_original )
            {
#ifdef DEBUG
               cout << "left_chunks.size( ) = " << left_chunks.size( ) << endl;
#endif
               if( is_conflict && has_output )
                  cout << ">>>>>> (1)\n";

               output_differences( is_conflict, true, left_chunks, start, finish, base, diff );
            }

            if( is_conflict && has_output )
            {
               had_conflict = true;
               cout << "======\n";
            }

            for( size_t i = 0; i < trailing_matches; i++ )
            {
#ifdef DEBUG
               cout << "<ac>";
#endif
               cout << diff[ trailing_match_offset + i ] << '\n';
            }
         }

         if( has_left || has_right )
            line = finish;
      }
   }
   catch( exception& x )
   {
      cerr << "error: " << x.what( );

      if( xline )
         cerr << " (" << xname << " line #" << xline << ')';

      cerr << endl;

      rc = 2;
   }

   if( had_conflict )
   {
      rc = 1;

      if( !is_quiet )
         cerr << "*** Error: One or more conflicts were found whilst merging." << endl;
   }

   return rc;
}
