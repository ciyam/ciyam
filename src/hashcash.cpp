// Copyright (c) 2011-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <stdexcept>
#endif

#include "hashcash.h"

#include "sha1.h"
#include "date_time.h"
#include "utilities.h"

using namespace std;

namespace
{

const int c_max_tries = 5000000;

const char* const c_hascash_alphabet = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+/=";

string g_alpabet( c_hascash_alphabet );

void increment_counter( string& counter )
{
   size_t last = counter.length( ) - 1;

   while( true )
   {
      size_t pos = g_alpabet.find( counter[ last ] );
      if( pos == string::npos )
         throw runtime_error( "unexpected invalid counter '" + counter + "'" );

      if( pos < g_alpabet.size( ) - 1 )
      {
         counter[ last ] = g_alpabet[ pos + 1 ];
         break;
      }
      else
      {
         counter[ last ] = g_alpabet[ 0 ];

         if( last == 0 )
            break;

         --last;
      }
   }
}

}

// FUTURE: The date sent should be checked with the YYMMDD portion in the hashcash string
// to verify that it is not too old and the "resource" portion checked against the user's
// email address (or a list of valid addresses) also previously received hashcash strings
// ought to be kept to ensure they haven't been used more than once.
bool check_hashcash( const string& hashcash )
{
   string::size_type pos = hashcash.find( ':' );
   if( pos == string::npos )
      return false;

   string::size_type npos = hashcash.find( ':', pos + 1 );
   if( npos == string::npos )
      return false;

   string bits( hashcash.substr( pos + 1, npos - pos - 1 ) );

   int num_bits = 0;
   for( size_t i = 0; i < bits.size( ); i++ )
   {
      num_bits *= 10;
      num_bits += bits[ i ] - '0';
   }

   if( num_bits < 16 )
      return false;

   while( num_bits % 4 != 0 )
      --num_bits;

   int found_zeros = 0;
   string digest( sha1( hashcash ).get_digest_as_string( ) );
   for( size_t i = 0; i < digest.size( ); i++ )
   {
      if( digest[ i ] == '0' )
         ++found_zeros;
      else
         break;
   }

   if( found_zeros * 4 < num_bits )
      return false;

   return true;
}

string create_hashcash( const string& resource, int num_bits, const char* p_ext )
{
   string retval;

   date_time dtm( date_time::standard( ) );

   string random;
   for( size_t i = 0; i < 16; i++ )
      random += g_alpabet[ rand( ) % g_alpabet.size( ) ];

   string prefix( "1:" + to_string( num_bits )
    + ":" + dtm.as_string( false, false ).substr( 2, 6 )
    + ":" + resource + ":" + ( p_ext ? p_ext : "" ) + ":" + random + ":" );

   sha1 hash;
   string counter( "aaaa" );

   for( size_t i = 0; i < c_max_tries; i++ )
   {
      int zeroes = 0;
      hash.update( prefix + counter );
      string digest = hash.get_digest_as_string( );

      for( size_t j = 0; j < digest.size( ); j++ )
      {
         if( digest[ j ] == '0' )
            ++zeroes;
         else
            break;
      }

      // KLUDGE: The work done will only be minimal here if num_bits % 4 == 0.
      if( zeroes * 4 >= num_bits )
      {
         retval = prefix + counter;
         break;
      }

      increment_counter( counter );
   }

   return retval;
}

