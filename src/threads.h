// Copyright (c) 2003-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef THREADS_H
#  define THREADS_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <iostream>
#  endif

#  ifndef _WIN32
#     include <pthread.h>
#  else
#     define NOMINMAX
#     define _WINSOCKAPI_
#     include <windows.h>
#  endif

#  ifndef _WIN32
#     define TLS( type ) type __thread
#  else
#     define TLS( type ) type __declspec( thread )
#  endif

class thread_id
{
   public:
   thread_id( ) : tid( 0 ) { }
#  ifdef _WIN32
   thread_id( DWORD tid ) : tid( tid ) { }
#  else
   thread_id( pthread_t tid ) : tid( tid ) { }
#  endif

   bool operator <( const thread_id& other ) const { return tid < other.tid; }
   bool operator >( const thread_id& other ) const { return tid > other.tid; }
   bool operator <=( const thread_id& other ) const { return tid <= other.tid; }
   bool operator >=( const thread_id& other ) const { return tid >= other.tid; }
   bool operator ==( const thread_id& other ) const { return tid == other.tid; }
   bool operator !=( const thread_id& other ) const { return !( *this == other ); }

   friend std::ostream& operator <<( std::ostream& outf, const thread_id& t );

   private:
#  ifdef _WIN32
   DWORD tid;
#  else
   pthread_t tid;
#  endif
};

inline std::ostream& operator <<( std::ostream& outf, const thread_id& t )
{
   outf << t.tid;

   return outf;
}

inline thread_id current_thread_id( )
{
#  ifndef _WIN32
   thread_id tid( ::pthread_self( ) );
#  else
   thread_id tid( ::GetCurrentThreadId( ) );
#  endif

   return tid;
}

class guard;

class mutex
{
   public:
#  ifdef _WIN32
   mutex( )
    :
    p_info( 0 )
   {
      ::InitializeCriticalSection( &cs );
   }

   ~mutex( )
   {
      ::DeleteCriticalSection( &cs );
   }
#  else
   mutex( )
    :
    tid( 0 ),
    p_info( 0 )
   {
      ::pthread_mutex_init( &ptm, 0 );
   }
#  endif

   void acquire( const guard* p_guard, const char* p_msg )
   {
      if( p_msg )
         pre_acquire( p_guard, p_msg );

#  ifndef _WIN32
      pthread_t self = ::pthread_self( );

      if( tid == self )
         ++count;
      else
      {
         ::pthread_mutex_lock( &ptm );

         count = 1;
         tid = self;
      }
#  else
      ::EnterCriticalSection( &cs );
#  endif

      lock_id = current_thread_id( );

      if( p_msg )
      {
         p_info = p_msg;
         post_acquire( p_guard, p_msg );
      }
   }

   void release( const guard* p_guard, const char* p_msg, const char* p_old_info )
   {
      p_info = p_old_info;

#  ifndef _WIN32
      if( !--count )
      {
         tid = 0;
         p_info = 0;
         lock_id = 0;

         ::pthread_mutex_unlock( &ptm );
      }
#  else
      lock_id = 0;
      ::LeaveCriticalSection( &cs );
#  endif

      if( p_msg )
         has_released( p_guard, p_msg );
   }

   thread_id get_lock_id( ) const { return lock_id; }

   const char* get_lock_info( ) const { return p_info; }

   private:
#  ifdef _WIN32
   CRITICAL_SECTION cs;
#  else
   int count;
   pthread_t tid;
   pthread_mutex_t ptm;
#  endif

   thread_id lock_id;

   const char* p_info;

   protected:
   virtual void pre_acquire( const guard* p_guard, const char* p_msg ) { }
   virtual void post_acquire( const guard* p_guard, const char* p_msg ) { }

   virtual void has_released( const guard* p_guard, const char* p_msg ) { }
};

class guard
{
   public:
   guard( mutex& m, const char* p_msg = 0 )
    :
    m( m ),
    p_msg( p_msg )
   {
      p_old_info = m.get_lock_info( );

      m.acquire( this, p_msg );
   }

   ~guard( )
   {
      m.release( this, p_msg, p_old_info );
   }

   private:
   mutex& m;

   const char* p_msg;
   const char* p_old_info;
};

#  ifdef _WIN32
unsigned long __stdcall threadfunc( void* pv );
#  else
void* threadfunc( void* p );
#  endif

struct thread
{
   void start( )
   {
#ifdef _WIN32
      ::CreateThread( 0, 0, threadfunc, this, 0, &tid );
#else
      pthread_attr_t tattr;
      ::pthread_attr_init( &tattr );
      ::pthread_attr_setdetachstate( &tattr, PTHREAD_CREATE_DETACHED );
      ::pthread_create( &tid, &tattr, threadfunc, ( void* )this );
#endif
   }

   virtual void on_start( ) = 0;

#  ifdef _WIN32
   DWORD tid;
#  else
   pthread_t tid;
#  endif
};

#  ifndef _WIN32
inline void* threadfunc( void* p )
{
   ( ( thread* )p )->on_start( );

   return 0;
}
#  else
inline unsigned long __stdcall threadfunc( void* pv )
{
   __try
   {
      ( ( thread* )pv )->on_start( );
   }
   __finally
   {
   }

   return 0;
}
#  endif

#endif
