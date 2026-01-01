// Copyright (c) 2002-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef DIFF_H
#  define DIFF_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <vector>
#     include <algorithm>
#  endif

// The "edit distance" algorithm which was adopted for this software can be directly attributed
// to the paper "An O(NP) Sequence Comparision Algorithm" published by Sun Wu, Udi Manber, Gene
// Myers and Webb Miller in August 1989.
//
// Although the paper mentions that the LCS can be determined in linear space using a recursive
// divide and conquer technique, this implementation instead uses a collection of matching path
// chunks (one for each set of diagonal matches), ensuring a maximum of O(NP) comparisions. The
// space which is required to determine the LCS with this techinque is directly proportional to
// the number of matching path chunks that are encountered by the algorithm (experimental tests
// indicate that the closer the LCS length is to the smaller of the two sequences the fewer the
// number of path chunks that are required to be added by the "edit_distance" function).
//
// The "bounded_array" was devised by Kevin Frey who also provided the initial source code from
// which the "edit_distance" function here evolved.

const int c_path_reserve = 1000;
const int c_max_diag_limit = 128;

struct path_item
{
   path_item( int x, int y, int len, int link ) : x( x ), y( y ), len( len ), link( link ) { }

   int x;
   int y;
   int len;
   int link;
};

template< typename T > class bounded_array
{
   private:
   int lower_bound;
   int upper_bound;

   std::vector< T > array;

   public:
   bounded_array( int lower_bound, int upper_bound, const T& init_value )
    :
    lower_bound( lower_bound ),
    upper_bound( upper_bound )
   {
      array.resize( upper_bound - lower_bound + 1, init_value );
   }

   int bias( int index ) const
   {
      return index - lower_bound;
   }

   const T& operator [ ]( int index ) const
   {
      return array[ bias( index ) ];
   }

   T& operator [ ]( int index )
   {
      return array[ bias( index ) ];
   }
};

template< typename C > class diff
{
   public:
   diff( const C& a, const C& b, bool get_path = true,
    bool get_minimal = true, int m = -1, int n = -1, int path_reserve = c_path_reserve )
    :
    A( a ),
    B( b ),
    M( m ),
    N( n ),
    swapped( false ),
    get_path( get_path ),
    get_minimal( get_minimal ),
    path_reserve( path_reserve ),
    next_path_item( -1 ),
    final_path_item( -1 ),
    edit_distance_value( 0 )
   {
      if( M == -1 )
         M = A.size( );
      if( N == -1 )
         N = B.size( );
      edit_distance_value = edit_distance( );
   }

   int get_path_length( ) const { return ( M + N - edit_distance_value ) / 2; }

   int get_edit_distance( ) const { return edit_distance_value; }

   bool get_next_match_chunk( std::pair< int, int >& match_point, int& num_matches )
   {
      if( next_path_item == -1 )
         return false;

      if( !swapped )
      {
         match_point.first = path_items[ next_path_item ].x;
         match_point.second = path_items[ next_path_item ].y;
      }
      else
      {
         match_point.first = path_items[ next_path_item ].y;
         match_point.second = path_items[ next_path_item ].x;
      }

      num_matches = path_items[ next_path_item ].len;
      next_path_item = path_items[ next_path_item ].link;

      return true;
   }

   bool get_first_match_chunk( std::pair< int, int >& match_point, int& num_matches )
   {
      next_path_item = final_path_item;
      return get_next_match_chunk( match_point, num_matches );
   }

   private:
   const C& A;
   const C& B;

   int M, N;

   bool swapped;
   bool get_path;
   bool get_minimal;
   bool was_reversed;

   int path_reserve;
   int next_path_item;
   int final_path_item;
   int edit_distance_value;

   std::vector< path_item > path_items;

   int edit_distance( );
};

template< typename C > int diff< C >::edit_distance( )
{
   const C* p_A = &A;
   const C* p_B = &B;

   if( M == 0 && N == 0 )
      return 0;

   if( M > N )
   {
      swapped = true;
      std::swap( M, N );
      std::swap( p_A, p_B );
   }

   int p( -1 );
   int delta( N - M );
   bounded_array< int > fp( -( M + 1 ), ( N + 1 ), -1 );
   bounded_array< int > fpp( -( M + 1 ), ( N + 1 ), -1 );

   if( get_path )
   {
      path_items.clear( );
      path_items.reserve( path_reserve );
   }

   int lp, oy, nx, ny, len;
   int mp = get_minimal ? 0 : ( M < c_max_diag_limit ? M : c_max_diag_limit );

   if( delta < 1 )
      delta = 1;

   do
   {
      ++p;

      // The following is to limit the amount of diagonal scanning in order to
      // avoid the worst case comparision scenario (i.e. M * N comparisons)...
      if( !mp || p < mp )
         lp = p;

      for( int k = 0 - lp; k <= delta - 1; k++ )
      {
         if( fp[ k - 1 ] + 1 > fp[ k + 1 ] )
         {
            oy = fp[ k - 1 ] + 1;
            fpp[ k ] = fpp[ k - 1 ];
         }
         else
         {
            oy = fp[ k + 1 ];
            fpp[ k ] = fpp[ k + 1 ];
         }

         len = 0;
         ny = oy;
         nx = ny - k;

         while( nx < M && ny < N && ( *p_A )[ nx ] == ( *p_B )[ ny ] )
            ++nx, ++ny, ++len;

         fp[ k ] = ny;

         if( len && get_path )
            path_items.push_back( path_item( nx - len, ny - len, len, fpp[ k ] ) );

         if( ny != oy )
            fpp[ k ] = path_items.size( ) - 1;
      }

      for( int k = delta + lp; k >= delta + 1; k-- )
      {
         if( fp[ k - 1 ] + 1 > fp[ k + 1 ] )
         {
            oy = fp[ k - 1 ] + 1;
            fpp[ k ] = fpp[ k - 1 ];
         }
         else
         {
            oy = fp[ k + 1 ];
            fpp[ k ] = fpp[ k + 1 ];
         }

         len = 0;
         ny = oy;
         nx = ny - k;

         while( nx < M && ny < N && ( *p_A )[ nx ] == ( *p_B )[ ny ] )
            ++nx, ++ny, ++len;

         fp[ k ] = ny;

         if( len && get_path )
            path_items.push_back( path_item( nx - len, ny - len, len, fpp[ k ] ) );

         if( ny != oy )
            fpp[ k ] = path_items.size( ) - 1;
      }

      if( fp[ delta - 1 ] + 1 > fp[ delta + 1 ] )
      {
         oy = fp[ delta - 1 ] + 1;
         fpp[ delta ] = fpp[ delta - 1 ];
      }
      else
      {
         oy = fp[ delta + 1 ];
         fpp[ delta ] = fpp[ delta + 1 ];
      }

      len = 0;
      ny = oy;
      nx = ny - delta;

      while( nx < M && ny < N && ( *p_A )[ nx ] == ( *p_B )[ ny ] )
         ++nx, ++ny, ++len;

      fp[ delta ] = ny;

      if( len && get_path )
         path_items.push_back( path_item( nx - len, ny - len, len, fpp[ delta ] ) );

      if( ny != oy )
         fpp[ delta ] = path_items.size( ) - 1;

   } while( ny < N );

   next_path_item = final_path_item = fpp[ delta ];

   if( get_path )
   {
      int next = -1;
      int i = final_path_item;

      while( i != -1 )
      {
         int prev = path_items[ i ].link;
         path_items[ i ].link = next;

         next = i;
         i = prev;
      }

      next_path_item = final_path_item = next;
   }

   return ( p == 0 ) ? 0 : delta + 2 * p;
}

#endif // DIFF_H

