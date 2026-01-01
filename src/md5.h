// Copyright (c) 2010-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef MD5_H
#  define MD5_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <cstdio>
#     include <fstream>
#     include <iostream>
#  endif

// MD5.CC - source code for the C++/object oriented translation and
//          modification of MD5.

// Translation and modification (c) 1995 by Mordechai T. Abzug 

// This translation/ modification is provided "as is," without express or 
// implied warranty of any kind.

// The translator/ modifier does not claim (1) that MD5 will do what you think 
// it does; (2) that this translation/ modification is accurate; or (3) that 
// this software is "merchantible."  (Language for this disclaimer partially 
// copied from the disclaimer below).

/* based on:

   MD5.H - header file for MD5C.C
   MDDRIVER.C - test driver for MD2, MD4 and MD5

   Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
rights reserved.

License to copy and use this software is granted provided that it
is identified as the "RSA Data Security, Inc. MD5 Message-Digest
Algorithm" in all material mentioning or referencing this software
or this function.

License is also granted to make and use derivative works provided
that such works are identified as "derived from the RSA Data
Security, Inc. MD5 Message-Digest Algorithm" in all material
mentioning or referencing the derived work.

RSA Data Security, Inc. makes no representations concerning either
the merchantability of this software or the suitability of this
software for any particular purpose. It is provided "as is"
without express or implied warranty of any kind.

These notices must be retained in any copies of any part of this
documentation and/or software.

*/

class MD5
{
   public:
   MD5( );
   MD5( unsigned char* string );
   MD5( std::istream& stream );
   MD5( FILE* file );
   MD5( std::ifstream& stream );

   void update( unsigned char* input, unsigned int input_length );
   void update( std::istream& stream );
   void update( FILE* file );
   void update( std::ifstream& stream );

   void init( );

   void finalize( );

   char* hex_digest( );
   unsigned char* raw_digest( );

   friend std::ostream& operator <<( std::ostream&, MD5 context );

   private:
   typedef unsigned int uint4;
   typedef unsigned char uint1;
   typedef unsigned short int uint2;

   uint4 state[ 4 ];
   uint4 count[ 2 ];
   uint1 buffer[ 64 ];
   uint1 digest[ 16 ];
   uint1 finalized;

   void transform( uint1* buffer );

   static void encode( uint1* dest, uint4* src, uint4 length );
   static void decode( uint4* dest, uint1* src, uint4 length );
   static void memcpy( uint1* dest, uint1* src, uint4 length );
   static void memset( uint1* start, uint1 val, uint4 length );

   static inline uint4 rotate_left( uint4 x, uint4 n );
   static inline uint4 F( uint4 x, uint4 y, uint4 z );
   static inline uint4 G( uint4 x, uint4 y, uint4 z );
   static inline uint4 H( uint4 x, uint4 y, uint4 z );
   static inline uint4 I( uint4 x, uint4 y, uint4 z );
   static inline void FF( uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac );
   static inline void GG( uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac );
   static inline void HH( uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac );
   static inline void II( uint4& a, uint4 b, uint4 c, uint4 d, uint4 x, uint4 s, uint4 ac );
};

#endif

