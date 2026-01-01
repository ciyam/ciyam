// Copyright (c) 2009-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef MEMTRACK_H
#  define MEMTRACK_H

#  include <new>

#  ifndef MEMTRACK_MAX_ENTRIES
#     define MEMTRACK_MAX_ENTRIES 10000
#  endif
#  ifndef MEMTRACK_NUM_PADDING_CHARS
#     define MEMTRACK_NUM_PADDING_CHARS 10
#  endif

//#  define MEMTRACK_USE_PRINTF
#  ifndef MEMTRACK_MIN_PRINTF_STATE
#     define MEMTRACK_MIN_PRINTF_STATE 2
#  endif
//#  define MEMTRACK_USE_DETAILED_PRINTF
#  ifndef MEMTRACK_MIN_SIZE_FOR_DETAILED
#     define MEMTRACK_MIN_SIZE_FOR_DETAILED 1000
#  endif

size_t g_memtrack_allocs = 0;
size_t g_memtrack_deallocs = 0;

#  ifdef MEMTRACK_TRIVIAL_TRACKING
void* operator new( size_t size )
{
   ++g_memtrack_allocs;
   return malloc( size );
}

void* operator new[ ]( size_t size )
{
   ++g_memtrack_allocs;
   return malloc( size );
}

void operator delete( void* p )
{
   if( p )
   {
      free( p );
      ++g_memtrack_deallocs;
   }
}

void operator delete[ ]( void* p )
{
   if( p )
   {
      free( p );
      ++g_memtrack_deallocs;
   }
}
#  else
size_t g_memtrack_not_found = 0;
size_t g_memtrack_last_alloc = 0;
int64_t g_memtrack_allocated = 0;
int64_t g_memtrack_deallocated = 0;

int64_t memtrack_current_allocation( ) { return g_memtrack_allocated - g_memtrack_deallocated; }

enum memtrack_state
{
   e_memtrack_state_none,
   e_memtrack_state_freed,
   e_memtrack_state_allocated,
   e_memtrack_state_DOUBLE_FREE,
   e_memtrack_state_BUFFER_OVERRUN,
   e_memtrack_state_BUFFER_UNDERRUN
};

enum memtrack_bstate
{
   e_memtrack_bstate_none,
   e_memtrack_bstate_alloc,
   e_memtrack_bstate_MISMATCH
};

struct memtrack_info
{
   size_t size;
   memtrack_state state;
   memtrack_bstate bstate;
   unsigned char* p_data;
   unsigned char* p_malloc;
};

const int c_max_memtrack_info = MEMTRACK_MAX_ENTRIES;
const int c_num_padding_chars = MEMTRACK_NUM_PADDING_CHARS;

#     if( MEMTRACK_MAX_ENTRIES < 10 )
#        error MEMTRACK_MAX_ENTRIES must be >= 10
#     endif

memtrack_info g_memtrack_info[ MEMTRACK_MAX_ENTRIES ];

size_t check_for_errors( bool throw_if_errors = true, bool stop_at_first_error = true )
{
   size_t num_errors = 0;

#     ifdef MEMTRACK_USE_PRINTF
   printf( "[checking alloc table for errors]\n" );
#     endif
   for( int i = 0; i < c_max_memtrack_info; i++ )
   {
      if( g_memtrack_info[ i ].state == e_memtrack_state_DOUBLE_FREE
       || g_memtrack_info[ i ].state == e_memtrack_state_BUFFER_OVERRUN
       || g_memtrack_info[ i ].state == e_memtrack_state_BUFFER_UNDERRUN )
         ++num_errors;
      else if( g_memtrack_info[ i ].state == e_memtrack_state_allocated )
      {
         for( int j = 0; j < c_num_padding_chars; j++ )
         {
            if( *( g_memtrack_info[ i ].p_malloc + j ) != 'X' + ( j % 2 ) )
            {
               ++num_errors;
               g_memtrack_info[ i ].state = e_memtrack_state_BUFFER_UNDERRUN;
            }
            else if( *( g_memtrack_info[ i ].p_malloc
             + c_num_padding_chars + g_memtrack_info[ i ].size + j ) != 'X' + ( j % 2 ) )
            {
               ++num_errors;
               g_memtrack_info[ i ].state = e_memtrack_state_BUFFER_OVERRUN;
            }

            if( g_memtrack_info[ i ].state != e_memtrack_state_allocated )
               break;
         }
      }

#     ifdef MEMTRACK_USE_PRINTF
      if( g_memtrack_info[ i ].p_data && g_memtrack_info[ i ].state >= MEMTRACK_MIN_PRINTF_STATE )
      {
         printf( "size = %u, state = %i, bstate = %i, p_data = %p, p_malloc = %p", g_memtrack_info[ i ].size,
          g_memtrack_info[ i ].state, g_memtrack_info[ i ].bstate, g_memtrack_info[ i ].p_data, g_memtrack_info[ i ].p_malloc );

         if( g_memtrack_info[ i ].p_malloc )
         {
            printf( " " );
            for( size_t j = 0; j < g_memtrack_info[ i ].size + ( c_num_padding_chars * 2 ); j++ )
               printf( "%c", isprint( g_memtrack_info[ i ].p_malloc[ j ] ) ? g_memtrack_info[ i ].p_malloc[ j ] : '.' );
         }

         printf( "\n" );
      }
#     endif

      if( num_errors && stop_at_first_error )
         break;
   }

#     ifdef MEMTRACK_USE_PRINTF
   if( num_errors == 0 )
      printf( "[no errors found in alloc table]\n" );
   else if( stop_at_first_error )
      printf( "[*** stopped at first error found in alloc table ***]\n" );
   else
      printf( "[*** found %u errors in alloc table ***]\n", num_errors );
#     endif
   if( num_errors && throw_if_errors )
      throw std::bad_alloc( );

   return num_errors;
}

void* allocate( size_t size, bool is_bracket_op )
{
   if( size == 0 )
#     ifdef MEMTRACK_USE_PRINTF
   {
      printf( "[*** zero size allocate ***]\n" );
      return 0;
   }
#     else
      return 0;
#endif

   g_memtrack_last_alloc = size;
   void* p = malloc( size + ( c_num_padding_chars * 2 ) );
   if( p == 0 )
      throw std::bad_alloc( );

   int slot = c_max_memtrack_info;
   for( int i = 0; i < c_max_memtrack_info; i++ )
   {
      if( slot == c_max_memtrack_info && !g_memtrack_info[ i ].p_malloc )
         slot = i;

      if( p == g_memtrack_info[ i ].p_malloc )
      {
         slot = i;
         break;
      }
   }

   if( slot < c_max_memtrack_info )
   {
      g_memtrack_info[ slot ].size = size;
      g_memtrack_info[ slot ].state = e_memtrack_state_allocated;
      g_memtrack_info[ slot ].bstate = is_bracket_op ? e_memtrack_bstate_alloc : e_memtrack_bstate_none;
      g_memtrack_info[ slot ].p_data = ( unsigned char* )( p ) + c_num_padding_chars;
      g_memtrack_info[ slot ].p_malloc = ( unsigned char* )p;

      for( int i = 0; i < size + ( c_num_padding_chars * 2 ); i++ )
         *( g_memtrack_info[ slot ].p_malloc + i ) = 0;

      for( int i = 0; i < c_num_padding_chars; i++ )
      {
         *( g_memtrack_info[ slot ].p_malloc + i ) = 'X' + ( i % 2 );
         *( g_memtrack_info[ slot ].p_malloc + c_num_padding_chars + size + i ) = 'X' + ( i % 2 );
      }
   }

   ++g_memtrack_allocs;
   g_memtrack_allocated += size;

#     ifdef MEMTRACK_USE_DETAILED_PRINTF
   if( size >= MEMTRACK_MIN_SIZE_FOR_DETAILED )
      printf( "allocate: %p (size = %u) is_bracket_op = %d\n", p, size, is_bracket_op );
#     endif
   return ( void* )( ( unsigned char* )p + c_num_padding_chars );
}
                  
void deallocate( void* p, bool is_bracket_op )
{
   bool found = false;
   for( int i = 0; i < c_max_memtrack_info; i++ )
   {
      if( p == ( void* )( g_memtrack_info[ i ].p_data ) )
      {
#     ifdef MEMTRACK_USE_DETAILED_PRINTF
         if( g_memtrack_info[ i ].size >= MEMTRACK_MIN_SIZE_FOR_DETAILED )
            printf( "dallocate: %p (size = %u) is_bracket_op = %d \n",
             g_memtrack_info[ i ].p_malloc, g_memtrack_info[ i ].size, is_bracket_op );
#     endif

         found = true;
         if( g_memtrack_info[ i ].state == e_memtrack_state_allocated )
         {
            g_memtrack_info[ i ].state = e_memtrack_state_freed;

            if( ( is_bracket_op && g_memtrack_info[ i ].bstate != e_memtrack_bstate_alloc )
             || ( !is_bracket_op && g_memtrack_info[ i ].bstate != e_memtrack_bstate_none ) )
               g_memtrack_info[ i ].bstate = e_memtrack_bstate_MISMATCH;

            for( int j = 0; j < c_num_padding_chars; j++ )
            {
               if( *( g_memtrack_info[ i ].p_malloc + j ) != 'X' + ( j % 2 ) )
               {
                  g_memtrack_info[ i ].state = e_memtrack_state_BUFFER_UNDERRUN;
                  break;
               }
               else if( *( g_memtrack_info[ i ].p_malloc
                + c_num_padding_chars + g_memtrack_info[ i ].size + j ) != 'X' + ( j % 2 ) )
               {
                  g_memtrack_info[ i ].state = e_memtrack_state_BUFFER_OVERRUN;
                  break;
               }
            }

            g_memtrack_deallocated += g_memtrack_info[ i ].size;
            free( ( void* )g_memtrack_info[ i ].p_malloc );
            g_memtrack_info[ i ].p_malloc = 0;
            ++g_memtrack_deallocs;
         }
         else if( g_memtrack_info[ i ].state == e_memtrack_state_freed )
            g_memtrack_info[ i ].state = e_memtrack_state_DOUBLE_FREE;

         found = true;
         break;
      }
   }

   if( !found )
      ++g_memtrack_not_found;
}

void* operator new( size_t size )
{
   return allocate( size, false );
}

void* operator new[ ]( size_t size )
{
   return allocate( size, true );
}

void operator delete( void* p )
{
   if( p )
      deallocate( p, false );
}

void operator delete[ ]( void* p )
{
   if( p )
      deallocate( p, true );
}
#  endif

#endif

