// Copyright (c) 2014 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <memory.h>
#  include <fstream>
#  include <iostream>
#  include <stdexcept>
#endif

#include "hash_chain.h"

#include "sha256.h"
#include "utilities.h"

using namespace std;

namespace
{

const int c_hash_buf_size = 32;

}

struct hash_chain::impl
{
   impl( const string& name, bool is_new, unsigned int size, bool use_seed );

   bool has_been_depleted( ) const;

   bool check_and_update_if_good( const string& new_hash, unsigned int check_limit );

   string get_next_hash_to_publish( const string& password );

   string name;
   uint32_t rounds;

   unsigned char buffer[ c_hash_buf_size ];
   unsigned char seedbuf[ c_hash_buf_size ];
};

hash_chain::impl::impl( const string& name, bool is_new, unsigned int size, bool use_seed )
 :
 name( name )
{
   rounds = ( uint32_t )size;

   memset( buffer, 0, c_hash_buf_size );
   memset( seedbuf, 0, c_hash_buf_size );

   if( is_new )
   {
      // NOTE: If rounds is non-zero then is considered to be an "internal" chain
      // and if zero then is considered to be an "external" chain.
      if( rounds )
      {
         ofstream outf( name.c_str( ), ios::out | ios::binary );

         if( !outf )
            throw runtime_error( "unable to open '" + name + "' for output" );

         string seed;
         if( use_seed )
            seed = uuid( ).as_string( );

         sha256 hash( seed );
         hash.copy_digest_to_buffer( buffer );

         if( !outf.write( ( const char* )&rounds, sizeof( uint32_t ) ) )
            throw runtime_error( "unexpected error writing rounds to file '" + name + "'" );

         if( !outf.write( ( const char* )buffer, c_hash_buf_size ) )
            throw runtime_error( "unexpected error writing hash data to file '" + name + "'" );

         outf.flush( );

         if( !outf.good( ) )
            throw runtime_error( "unexpected bad output stream for hash chain '" + name + "'" );
      }
      else
      {
         ofstream outf( name.c_str( ), ios::out | ios::binary );

         if( !outf.write( ( const char* )&rounds, sizeof( uint32_t ) ) )
            throw runtime_error( "unexpected error writing rounds to file '" + name + "'" );

         if( !outf.write( ( const char* )buffer, c_hash_buf_size ) )
            throw runtime_error( "unexpected error writing hash data to file '" + name + "'" );

         outf.flush( );

         if( !outf.good( ) )
            throw runtime_error( "unexpected bad output stream for hash chain '" + name + "'");
      }
   }

   ifstream inpf( name.c_str( ), ios::in | ios::binary );

   if( !inpf )
      throw runtime_error( "unable to open '" + name + "' for input" );

   if( !inpf.read( ( char* )&rounds, sizeof( uint32_t ) ) )
      throw runtime_error( "unexpected error reading rounds from file '" + name + "'" );

   if( !inpf.read( ( char* )buffer, c_hash_buf_size ) )
      throw runtime_error( "unexpected error reading hash data from file '" + name + "'" );

   memcpy( seedbuf, buffer, c_hash_buf_size );
}

bool hash_chain::impl::has_been_depleted( ) const
{
   if( rounds == 1 )
      return true;
   else
      return false;
}

bool hash_chain::impl::check_and_update_if_good( const string& new_hash, unsigned int check_limit )
{
   if( rounds )
      throw runtime_error( "invalid 'check_and_update_if_good' call for internal hash chain '" + name + "'" );

   if( new_hash.size( ) != c_hash_buf_size * 2 )
      throw runtime_error( "unexpected invalid length hash '" + new_hash + "'" );

   unsigned char new_buf[ c_hash_buf_size ];

   for( size_t i = 0; i < c_hash_buf_size * 2; i +=2 )
   {
      unsigned char ch = hex_nibble( new_hash[ i ] );

      ch <<= 4;
      ch |= hex_nibble( new_hash[ i + 1 ] );

      new_buf[ i / 2 ] = ch;
   }

   bool is_new = true;
   for( size_t i = 0; i < c_hash_buf_size; i++ )
   {
      if( buffer[ i ] != 0 )
      {
         is_new = false;
         break;
      }
   }

   if( !is_new )
   {
      unsigned char check_buf[ c_hash_buf_size ];

      sha256 hash( new_buf, c_hash_buf_size );
      hash.copy_digest_to_buffer( check_buf );

      bool matched = false;
      for( size_t i = 0; i < check_limit; i++ )
      {
         bool found = true;
         for( size_t j = 0; j < c_hash_buf_size; j++ )
         {
            if( buffer[ j ] != check_buf[ j ] )
            {
               found = false;
               break;
            }
         }

         if( found )
         {
            matched = true;
            break;
         }

         hash.update( check_buf, c_hash_buf_size );
         hash.copy_digest_to_buffer( check_buf );
      }

      if( !matched )
         return false;
   }

   ofstream outf( name.c_str( ), ios::out | ios::binary );

   if( !outf )
      throw runtime_error( "unable to open '" + name + "' for output" );

   if( !outf.write( ( const char* )&rounds, sizeof( uint32_t ) ) )
      throw runtime_error( "unexpected error writing rounds to file '" + name + "'" );

   if( !outf.write( ( const char* )new_buf, c_hash_buf_size ) )
      throw runtime_error( "unexpected error writing hash data to file '" + name + "'" );

   outf.flush( );

   if( !outf.good( ) )
      throw runtime_error( "unexpected bad output stream for hash chain '" + name + "'" );

   memcpy( buffer, new_buf, c_hash_buf_size );

   return true;
}

string hash_chain::impl::get_next_hash_to_publish( const string& password )
{
   if( !rounds )
      throw runtime_error( "invalid 'get_next_hash_to_publish' call for external hash chain '" + name + "'" );

   if( rounds == 1 )
      throw runtime_error( "hash chain '" + name + "' has been depleted" );

   // NOTE: If password is not what was used originally then "external" users will not match
   // the next hash, however, provided the correct password is used within the "check limit"
   // range this won't be a disastor (just part of the chain will have been "burned"). While
   // this can make a chain more "fault resistent" it also supports *gaming* for what can be
   // considered a "better next hash" so by default is not being used (thus "check limit" is
   // defaulted to 1 in "check_and_update_if_good").
   sha256 hash( password );
   hash.update( seedbuf, c_hash_buf_size );

   for( uint32_t i = 0; i < rounds; i++ )
   {
      hash.copy_digest_to_buffer( buffer );
      hash.update( buffer, sizeof( buffer ) );
   }

   --rounds;

   ofstream outf( name.c_str( ), ios::out | ios::binary );

   if( !outf.write( ( const char* )&rounds, sizeof( uint32_t ) ) )
      throw runtime_error( "unexpected error writing rounds to file '" + name + "'" );

   if( !outf.write( ( const char* )seedbuf, c_hash_buf_size ) )
      throw runtime_error( "unexpected error writing hash data to file '" + name + "'" );

   outf.flush( );

   if( !outf.good( ) )
      throw runtime_error( "unexpected bad output stream for hash chain '" + name + "'" );

   return lower( hash.get_digest_as_string( ) );
}

hash_chain::hash_chain( const string& name, bool is_new, size_t rounds, bool use_seed )
{
   p_impl = new impl( name, is_new, rounds, use_seed );
}

hash_chain::~hash_chain( )
{
   delete p_impl;
}

bool hash_chain::has_been_depleted( ) const
{
   return p_impl->has_been_depleted( );
}

bool hash_chain::check_and_update_if_good( const string& new_hash, unsigned int check_limit )
{
   return p_impl->check_and_update_if_good( new_hash, check_limit );
}

string hash_chain::get_next_hash_to_publish( const string& password )
{
   return p_impl->get_next_hash_to_publish( password );
}

