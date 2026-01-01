// Copyright (c) 2017-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <cstring>
#  include <map>
#  include <set>
#  include <deque>
#  include <vector>
#  include <string>
#  include <memory>
#  include <fstream>
#  include <iomanip>
#  include <iostream>
#  include <stdexcept>
#endif

#include "clz.h"

#include "utilities.h"

//#define DEBUG

#ifdef DEBUG
#  define DEBUG_DECODE
#  define DEBUG_ENCODE
#endif

//#define VISUALISE_BACKREFS
//#define COMPILE_TESTBED_MAIN

//#define NO_SHRINK_AND_EXPAND
//#define NO_SECOND_PASS_BACK_REFS

using namespace std;

namespace
{

// NOTE: This custom LZ encoding/decoding approach works with a 2KB chunk size
// although the exact number of bytes used can generally be less due to simple
// sequence and pattern repeat shrinking that is performed prior to outputting
// a chunk. Data values are 7-bit with the MSB of each byte being reserved for
// indicating either a back-ref (or a back-ref repeat) or a "special" shrunken
// pattern/sequence. Every back-ref (or back-ref repeat) consists of two bytes
// whose bit values are used as follows:
//
// [byte 1] [byte 2]
// 1xxxxyyy yyyyyyyy
//
// where: xxxx = 2^4 length (3-15) 0xF is being reserved for last pair repeats
// yyyyyyyyyyy = 2^11 offset (0-2047) or 2^11 last pair repeats value (1-2048)
//
// Back-ref repeats are differentiated by starting with 0xF (thus all back-ref
// lengths are limited to between 3 and 15 bytes) and values starting with the
// prefix 0xF8..0xFF are reserved for "specials" (used to shrink encoded data)
// so the maximum number of repeats is limited to 2048 as well. It should also
// be noted that a back-ref repeat must immediately follow a back-ref so other
// 0xF prefixes (0xF0..0xF7) are also used for specials (with these ones being
// only used where there can be no ambiguity with back-ref repeats).

const size_t c_max_offset = 2047;
const size_t c_max_repeats = 2048;

const size_t c_max_combines = 5;

const size_t c_max_dict_words = 256;
const size_t c_dict_words_offset = 10;
const size_t c_max_dict_words_run = 12;

const size_t c_min_pat_length = 3;
const size_t c_max_pat_length = 15;

const size_t c_meta_pat_length = 4;

const size_t c_max_encoded_chunk_size = c_max_offset + c_min_pat_length + 2;

const size_t c_max_unencoded_chunk_size = 8192;

const size_t c_max_special_pairs = 6;
const size_t c_max_special_repeats = 15;
const size_t c_max_special_step_vals = 15;

const size_t c_max_expand_meta_recursion = 10;

const size_t c_offset_low_pair_mask = 0x00ff;
const size_t c_offset_high_pair_mask = 0x0700;

const char c_back_ref = '\xff';

const unsigned char c_nibble_one = 0xf0;
const unsigned char c_nibble_two = 0x0f;

const unsigned char c_high_bit_value = 0x80;
const unsigned char c_high_five_bits = 0xf8;

const unsigned char c_max_repeats_hi = 0xf7;
const unsigned char c_max_repeats_lo = 0xff;

const unsigned char c_special_marker = 0xf9;
const unsigned char c_special_maxval = 0xff;

const unsigned char c_special_byte_inc = 0x80;
const unsigned char c_special_byte_dec = 0x90;

const unsigned char c_special_pair_inc = 0xa0;
const unsigned char c_special_pair_dec = 0xb0;

const unsigned char c_special_pair_low_inc = 0xc0;
const unsigned char c_special_pair_low_dec = 0xd0;

const unsigned char c_special_pair_high_inc = 0xe0;
const unsigned char c_special_pair_high_dec = 0xf0;

const unsigned char c_offset_high_byte_mask = 0x07;
const unsigned char c_pattern_high_byte_mask = 0x78;

const unsigned char c_meta_pattern_length_val = 0x88;

const unsigned char c_special_char_double_repeat = 0xf0;
const unsigned char c_special_char_multi_repeats = 0xf1;
const unsigned char c_special_compressed_numeric = 0xf2;
const unsigned char c_special_dict_pattern_lower = 0xf3;
const unsigned char c_special_dict_pattern_mixed = 0xf4;
const unsigned char c_special_dict_pattern_upper = 0xf5;
const unsigned char c_special_step_pattern_fixed = 0xf6;
const unsigned char c_special_step_pattern_multi = 0xf7;
const unsigned char c_special_dict_word_list_num = 0xf8;

#ifdef VISUALISE_BACKREFS
const char* const c_color_red = "\e[31m";
const char* const c_color_blue = "\e[34m";
const char* const c_color_cyan = "\e[36m";
const char* const c_color_green = "\e[32m";
const char* const c_color_purple = "\e[35m";
const char* const c_color_yellow = "\e[33m";
const char* const c_color_default = "\e[39m";
#endif

enum step_type
{
   e_step_type_none = 0,
   e_step_type_single = 1,
   e_step_type_double = 2,
   e_step_type_double_low = 3,
   e_step_type_double_high = 4
};

struct dict_word
{
   const char* p_w;
}
g_dict_words[ ] =
{
   { " -= " },
   { " += " },
   { " || " },
   { " |= " },
   { " && " },
   { " &= " },
   { " == " },
   { " != " },
   { " <= " },
   { " >= " },
   { " << " },
   { " >> " },
   { "Args" },
   { "Auto" },
   { "bool" },
   { "cerr" },
   { "char" },
   { "cout" },
   { "edit" },
   { "else" },
   { "endl" },
   { "Enum" },
   { "Exec" },
   { "File" },
   { "High" },
   { "inpf" },
   { "line" },
   { "Link" },
   { "main" },
   { "make" },
   { "map<" },
   { "miss" },
   { "name" },
   { "Next" },
   { "node" },
   { "open" },
   { "outf" },
   { "read" },
   { "root" },
   { "Save" },
   { "set<" },
   { "ship" },
   { "Size" },
   { "Sort" },
   { "Spec" },
   { "Text" },
   { "true" },
   { "Type" },
   { "var_" },
   { "View" },
   { "void" },
   { "what" },
   { "Work" },
   { "www." },
   { ".com" },
   { ".css" },
   { ".gif" },
   { ".jpg" },
   { ".org" },
   { ".png" },
   { ".txt" },
   { " (c) " },
   { "begin" },
   { "catch" },
   { "child" },
   { "Child" },
   { "CIYAM" },
   { "const" },
   { "empty" },
   { "error" },
   { "Exact" },
   { "Extra" },
   { "false" },
   { "field" },
   { "first" },
   { "graph" },
   { "Index" },
   { "Level" },
   { "Limit" },
   { "Model" },
   { "pair<" },
   { "reign" },
   { "split" },
   { "Total" },
   { "using" },
   { "value" },
   { "Value" },
   { "while" },
   { "Width" },
   { "@key," },
   { "Access" },
   { "Change" },
   { "except" },
   { "fields" },
   { "M100C1" },
   { "output" },
   { "please" },
   { "Plural" },
   { "record" },
   { "Record" },
   { "return" },
   { "Sample" },
   { "Search" },
   { "second" },
   { "size_t" },
   { "socket" },
   { "Static" },
   { "Status" },
   { "Symbol" },
   { "Unique" },
   { "vector" },
   { "verify" },
   { "#ifdef" },
   { "<sio/>" },
   { "</sio>" },
   { "Default" },
   { "Exclude" },
   { "http://" },
   { "include" },
   { "Initial" },
   { "license" },
   { "MIT/X11" },
   { "Numeric" },
   { "project" },
   { "Without" },
   { "#pragma" },
   { "children" },
   { "Commands" },
   { "generate" },
   { "https://" },
   { "ifstream" },
   { "int main" },
   { "Manually" },
   { "ofstream" },
   { "Security" },
   { "Specific" },
   { "software" },
   { "<record>" },
   { "<class/>" },
   { "</class>" },
   { "Algorithm" },
   { "Anonymous" },
   { "Behaviour" },
   { "Copyright" },
   { "directory" },
   { "Mandatory" },
   { "namespace" },
   { "Procedure" },
   { "stdexcept" },
   { "utilities" },
   { "x.what( )" },
   { "const char" },
   { "Developers" },
   { "Permission" },
   { "unexpected" },
   { "Create_Only" },
   { "Distributed" },
   { "Restriction" },
   { "STD_HEADERS" },
   { "const char* " },
   { "ifstream inpf" },
   { "ofstream outf" },
   { "Pty. Ltd. ACN" },
   { "runtime_error" },
   { "Specification" },
   { "mit-license.php" },
   { "using namespace" },
   { "( exception& x " },
};

struct dict_pattern
{
   const char* p_w1;
   const char* p_w2;
   const char* p_w3;
   const char* p_w4;
   const char* p_w5;
   const char* p_w6;
   const char* p_w7;
   const char* p_w8;
}
// NOTE: The following dictionary patterns need to be strictly
// alphabetically ordered (or they will not decode correctly).
g_dict_patterns[ ] =
{
   { "acc", "ace", "ack", "ade", "age", "ail", "ain", "air" },
   { "ale", "all", "and", "any", "app", "are", "ash", "ass" },
   { "bad", "ban", "bat", "bed", "beg", "ber", "bet", "bid" },
   { "big", "bin", "bit", "ble", "bon", "boo", "bow", "box" },
   { "bra", "bun", "bus", "but", "buy", "can", "car", "com" },
   { "cop", "cos", "cry", "cue", "cup", "cut", "dad", "day" },
   { "dig", "dim", "dip", "dog", "dur", "ear", "eat", "ect" },
   { "ell", "end", "ent", "ept", "ere", "ero", "ess", "est" },
   { "far", "fat", "fee", "few", "fit", "fix", "flu", "fly" },
   { "for", "ful", "fun", "fur", "gen", "ger", "get", "got" },
   { "gst", "gun", "gut", "had", "han", "hap", "has", "hen" },
   { "her", "him", "his", "hit", "hol", "hop", "hot", "how" },
   { "hub", "hug", "hut", "ice", "ick", "ide", "ied", "ill" },
   { "inc", "inf", "ing", "int", "ion", "ire", "ish", "ism" },
   { "ist", "ith", "its", "ity", "ive", "jag", "jar", "jaw" },
   { "jet", "jog", "jug", "jus", "ken", "key", "kin", "lad" },
   { "lag", "lap", "lay", "led", "leg", "let", "lip", "log" },
   { "low", "mal", "man", "mat", "med", "men", "met", "mod" },
   { "mum", "nam", "nce", "nde", "nes", "net", "new", "not" },
   { "now", "old", "oll", "ome", "one", "ong", "ord", "ore" },
   { "oth", "our", "out", "ove", "own", "pas", "pay", "pea" },
   { "ped", "per", "pie", "pit", "pod", "pop", "pos", "pot" },
   { "pre", "pub", "pun", "pup", "put", "qui", "ran", "rat" },
   { "rea", "red", "ria", "rib", "rid", "rig", "roc", "rot" },
   { "rou", "row", "rse", "rst", "rue", "run", "sal", "ser" },
   { "ses", "she", "shi", "sis", "ste", "sur", "tab", "tan" },
   { "tar", "tas", "tch", "tel", "ten", "ter", "tha", "the" },
   { "thr", "tio", "tip", "tis", "tle", "too", "toy", "tri" },
   { "tub", "two", "ude", "ugh", "und", "unt", "urn", "use" },
   { "val", "veg", "ver", "vet", "vie", "vow", "war", "was" },
   { "way", "wed", "wel", "who", "win", "wit", "won", "xed" },
   { "xen", "xes", "yea", "yen", "yet", "you", "zed", "zes" },
};

map< string, unsigned char > g_dict_lower;
map< string, unsigned char > g_dict_mixed;
map< string, unsigned char > g_dict_upper;

void dump_bytes( const char* p_prefix, unsigned char* p_input, size_t num, size_t mark = 0, char prefix_seperator = ' ' )
{
   if( p_prefix )
   {
      string prefix( p_prefix );

      if( prefix.length( ) )
      {
         cout << prefix;

         if( num < 64 && prefix_seperator )
            cout << prefix_seperator;
      }
   }

   cout << hex;

   for( size_t i = 0; i < num; i++ )
   {
      if( num > 64 && i % 64 == 0 )
         cout << '\n';

      if( mark && i == mark )
         cout << '|';

      cout << setw( 2 ) << setfill( '0' ) << ( int )p_input[ i ];

   }

   cout << dec << endl;
}

struct repeat_info
{
   repeat_info( ) : offset( 0 ), length( 0 ) { }

   size_t offset;
   size_t length;
};

typedef pair< unsigned char, unsigned char > byte_pair;

struct meta_pattern
{
   unsigned char byte1;
   unsigned char byte2;
   unsigned char byte3;
   unsigned char byte4;

   bool operator <( const meta_pattern& p ) const
   {
      bool retval = false;

      if( byte1 < p.byte1 )
         retval = true;
      else if( byte1 == p.byte1 )
      {
         if( byte2 < p.byte2 )
            retval = true;
         else if( byte2 == p.byte2 )
         {
            if( byte3 < p.byte3 )
               retval = true;
            else if( byte3 == p.byte3 )
               retval = byte4 < p.byte4;
         }
      }

      return retval;
   }

   bool operator ==( const meta_pattern& p ) const
   {
      return byte1 == p.byte1 && byte2 == p.byte2 && byte3 == p.byte3 && byte4 == p.byte4;
   }
};

typedef byte_pair meta_pair;

ostream& operator <<( ostream& os, const meta_pair& p )
{
   os << hex
    << setw( 2 ) << setfill( '0' ) << ( int )p.first << setw( 2 ) << setfill( '0' ) << ( int )p.second << dec;

   return os;
}

ostream& operator <<( ostream& os, const meta_pattern& p )
{
   os << hex
    << setw( 2 ) << setfill( '0' ) << ( int )p.byte1 << setw( 2 ) << setfill( '0' ) << ( int )p.byte2
    << setw( 2 ) << setfill( '0' ) << ( int )p.byte3 << setw( 2 ) << setfill( '0' ) << ( int )p.byte4 << dec;

   return os;
}

struct meta_pattern_info
{
   void clear( )
   {
      offsets.clear( );
      patterns.clear( );
   }

   bool has_offset( size_t offset ) { return offsets.count( offset ); }

   bool has_pattern( const meta_pattern& pat ) { return patterns.count( pat ); }

   void add_pattern( const meta_pattern& pat, size_t offset )
   {
      offsets[ offset ] = pat;

      patterns[ pat ].first = c_meta_pattern_length_val | ( ( offset & c_offset_high_pair_mask ) >> 8 );
      patterns[ pat ].second = ( offset & c_offset_low_pair_mask );
#ifdef DEBUG_ENCODE
cout << "add pattern: " << patterns[ pat ] << " ==> " << pat << " @" << offset << endl;
#endif
   }

   meta_pattern operator [ ]( size_t offset )
   {
      return offsets[ offset ];
   }

   meta_pair operator [ ]( const meta_pattern& pat )
   {
      return patterns[ pat ];
   }

   size_t last_offset( ) const
   {
      size_t val = 0;

      map< size_t, meta_pattern >::const_iterator ci = offsets.end( );

      if( !offsets.empty( ) )
         val = ( --ci )->first;
      
      return val;
   }

   size_t pattern_offset( const meta_pattern& pat )
   {
      size_t val = 0;

      if( has_pattern( pat ) )
      {
         for( map< size_t, meta_pattern >::iterator i = offsets.begin( ); i != offsets.end( ); ++i )
         {
            if( i->second == pat )
            {
               val = i->first;
               break;
            }
         }
      }

      return val;
   }

   void remove_at_offset( size_t offset )
   {
      if( has_offset( offset ) )
      {
         meta_pattern pat = offsets[ offset ];
         offsets.erase( offset );

#ifdef DEBUG_ENCODE
cout << "rem pattern: " << patterns[ pat ] << " ==> " << pat << " @" << offset << endl;
#endif
         patterns.erase( pat );
      }
   }

   void remove_offsets_from( size_t start )
   {
      while( true )
      {
         size_t next = last_offset( );

         if( next < start )
            break;

         remove_at_offset( next );
      }
   }

   map< size_t, meta_pattern > offsets;
   map< meta_pattern, meta_pair > patterns;
};

void check_meta_pattern( meta_pattern_info& meta_patterns, const meta_pattern& pat, unsigned char* p_buffer )
{
   size_t offset = ( meta_patterns[ pat ].first & c_offset_high_byte_mask ) << 8;
   offset += meta_patterns[ pat ].second;

   if( pat.byte1 != *( p_buffer + offset ) || pat.byte2 != *( p_buffer + offset + 1 )
      || pat.byte3 != *( p_buffer + offset + 2 ) || pat.byte4 != *( p_buffer + offset + 3 ) )
      throw runtime_error( " found invalid meta pattern @" + to_string( offset ) );
}

void check_meta_patterns( meta_pattern_info& meta_patterns, unsigned char* p_buffer, size_t output_offset )
{
   for( size_t i = 0; i < output_offset; i++ )
   {
      if( meta_patterns.has_offset( i ) )
      {
         meta_pattern pat = meta_patterns[ i ];

         size_t offset = ( meta_patterns[ pat ].first & c_offset_high_byte_mask ) << 8;
         offset += meta_patterns[ pat ].second;

#ifdef DEBUG_ENCODE
cout << meta_patterns[ pat ] << " for " << pat << " at @" << offset << endl;
#endif
         if( pat.byte1 != *( p_buffer + offset ) || pat.byte2 != *( p_buffer + offset + 1 )
          || pat.byte3 != *( p_buffer + offset + 2 ) || pat.byte4 != *( p_buffer + offset + 3 ) )
            throw runtime_error( " found invalid meta pattern @" + to_string( offset ) );
      }
   }
}

size_t found_stepping_bytes( unsigned char* p_buffer, size_t offset, size_t length, size_t& bytes, bool& ascending, size_t& last_val )
{
   size_t step_amount = 0;

   for( bytes = 1; bytes <= 4; bytes++ )
   {
      if( bytes == 1 && offset + 4 < length )
      {
         unsigned char byte1 = *( p_buffer + offset );
         unsigned char byte2 = *( p_buffer + offset + 1 );

         if( ( byte1 & c_high_bit_value )
          || ( byte2 & c_high_bit_value ) )
            break;

         if( byte1 == byte2 )
            continue;

         ascending = ( byte1 < byte2 ? true : false );
         step_amount = ( ascending ? byte2 - byte1 : byte1 - byte2 );

         unsigned char byte3 = *( p_buffer + offset + 2 );
         unsigned char byte4 = *( p_buffer + offset + 3 );

         if( ( byte3 & c_high_bit_value )
          || ( byte4 & c_high_bit_value ) )
         {
            step_amount = 0;
            break;
         }

         bool found = true;

         if( ( ascending && byte3 != byte2 + step_amount )
          || ( !ascending && byte3 != byte2 - step_amount )
          || ( ascending && byte4 != byte3 + step_amount )
          || ( !ascending && byte4 != byte3 - step_amount ) )
            found = false;
         else
         {
            unsigned char byte5 = *( p_buffer + offset + 4 );

            if( byte5 & c_high_bit_value )
               break;

            if( ( ascending && byte5 != byte4 + step_amount )
             || ( !ascending && byte5 != byte4 - step_amount ) )
               found = false;
         }

         if( found )
            break;
         else
            step_amount = 0;
      }
      else if( bytes == 2 && offset + 7 < length )
      {
         unsigned char byte1 = *( p_buffer + offset );
         unsigned char byte2 = *( p_buffer + offset + 1 );

         size_t val1 = ( byte1 << 8 ) + byte2;

         unsigned char byte3 = *( p_buffer + offset + 2 );
         unsigned char byte4 = *( p_buffer + offset + 3 );

         size_t val2 = ( byte3 << 8 ) + byte4;

         if( val1 == val2 )
            continue;

         ascending = ( val1 < val2 ? true : false );
         step_amount = ( ascending ? val2 - val1 : val1 - val2 );

         unsigned char byte5 = *( p_buffer + offset + 4 );
         unsigned char byte6 = *( p_buffer + offset + 5 );

         if( ( byte5 & c_high_bit_value )
          || ( byte6 & c_high_bit_value ) )
         {
            step_amount = 0;
            break;
         }

         size_t val3 = ( byte5 << 8 ) + byte6;

         unsigned char byte7 = *( p_buffer + offset + 6 );
         unsigned char byte8 = *( p_buffer + offset + 7 );

         if( ( byte7 & c_high_bit_value )
          || ( byte8 & c_high_bit_value ) )
         {
            step_amount = 0;
            break;
         }

         size_t val4 = ( byte7 << 8 ) + byte8;

         bool found = true;

         if( ( ascending && val3 != val2 + step_amount )
          || ( !ascending && val3 != val2 - step_amount )
          || ( ascending && val4 != val3 + step_amount )
          || ( !ascending && val4 != val3 - step_amount ) )
            found = false;

         if( found )
         {
            last_val = val2;
            break;
         }
         else
            step_amount = 0;
      }
      // FUTURE: Should also check for patterns of 3 and 4 bytes (and maybe more).
   }

   return step_amount;
}

size_t shrink_special_numeric( unsigned char* p_buffer, size_t len )
{
   size_t offset = 1;
   unsigned char next = 0;

   unsigned char ch = *p_buffer;

   *p_buffer = ( len == 4 ? c_special_char_multi_repeats : c_special_compressed_numeric );

   for( size_t i = 0; i < len; i++ )
   {
      if( ch == ' ' )
         next |= 0x0e;
      else if( ch >= '0' && ch <= '9' )
         next |= ( ch - '0' );
      else if( ch >= '+' && ch <= '.' )
         next |= ( ch - '+' + 0x0a );

      if( i % 2 == 0 )
         next <<= 4;
      else
      {
         *( p_buffer + offset++ ) = next;
         next = 0;
      }

      if( i < len )
         ch = *( p_buffer + i + 1 );
   }

   if( len == 4 )
   {
      --offset;
      return 1;
   }
   else
   {
      if( len % 2 == 1 )
         *( p_buffer + offset ) = ( next | 0x0f );
      else
      {
         next = 0xff;
         *( p_buffer + offset ) = next;
      }

      return ( len - offset - 1 );
   }
}

void output_repeats( unsigned char* p_shrunken, size_t repeats, size_t& num )
{
   // NOTE: If just two repeats then simply output one byte but
   // if more then need to also output the repeat number value.
   if( repeats == 2 )
      *( p_shrunken + num++ ) = c_special_char_double_repeat;
   else
   {
      *( p_shrunken + num++ ) = c_special_char_multi_repeats;
      *( p_shrunken + num++ ) = repeats;
   }
}

void shrink_output( unsigned char* p_buffer, size_t& length, byte_pair* p_mark_after_pair = 0 )
{
   size_t mark_pos = 0;
   size_t shrunken_mark_pos = 0;

   unsigned char shrunken[ c_max_encoded_chunk_size ];

   if( length <= c_max_encoded_chunk_size )
   {
      if( g_dict_lower.empty( ) )
         init_clz_info( );

      map< byte_pair, size_t > pairs;

      // NOTE: A "byte pair" is either a back-ref, a meta-pattern
      // or a pair repeat value (each of which will start with an
      // MSB set in the first byte). Thus pairs are being counted
      // here to find any repeats that will be candidates for the
      // "specials".
      for( size_t i = 0; i < length; i++ )
      {
         unsigned char next = *( p_buffer + i );

         if( i < length - 1 )
         {
            if( next & c_high_bit_value )
            {
               byte_pair next_pair;

               next_pair.first = next;
               next_pair.second = *( p_buffer + ++i );

               if( p_mark_after_pair && next_pair == *p_mark_after_pair )
                  mark_pos = i + 1;

               ++pairs[ next_pair ];
            }
         }
      }

      // NOTE: Only pairs with 3 or more repeats can be considered as "specials" so firstly
      // remove those that don't qualify.
      set< byte_pair > worst;
      for( map< byte_pair, size_t >::iterator pi = pairs.begin( ); pi != pairs.end( ); ++pi )
      {
         if( pi->second <= 2 )
            worst.insert( pi->first );
      }

      for( set< byte_pair >::iterator wi = worst.begin( ); wi != worst.end( ); ++wi )
         pairs.erase( *wi );

      multimap< size_t, byte_pair > ordered;

      // NOTE: Order the pairs and then remove the least repeated ones if there are more of
      // these than the maximum number of specials permitted and then number all remaining.
      for( map< byte_pair, size_t >::iterator pi = pairs.begin( ); pi != pairs.end( ); ++pi )
         ordered.insert( make_pair( pi->second, pi->first ) );

      while( ordered.size( ) > c_max_special_pairs )
         ordered.erase( ordered.begin( ) );

      set< byte_pair > specials;

      for( multimap< size_t, byte_pair >::iterator oi = ordered.begin( ); oi != ordered.end( ); ++oi )
         specials.insert( oi->second );

      map< byte_pair, size_t > special_nums;

      for( set< byte_pair >::iterator si = specials.begin( ); si != specials.end( ); ++si )
         special_nums[ *si ] = special_nums.size( ) - 1;

      size_t num = 0;
      size_t repeats = 0;

      bool steps_ascending = true;

      step_type step_type_val = e_step_type_none;

      size_t stepping_bytes = 0;
      size_t stepping_amount = 0;
      size_t stepping_last_val = 0;

      size_t dict_words_run = 0;
      size_t last_dict_word_pos = 0;

      size_t last_special_pos = 0;
      size_t last_back_ref_pos = 0;

      size_t special_numeric_start = 0;
      size_t special_numeric_length = 0;

      unsigned char last_ch = c_special_maxval;

      size_t available_specials = ( c_max_special_pairs - special_nums.size( ) );

      map< byte_pair, size_t > repeated_special_counts;
      map< size_t, byte_pair > repeated_special_offsets;

      map< size_t, size_t > dict_words_found;

      string buffer_str( ( const char* )p_buffer, length );

      for( size_t i = 0; i < ARRAY_SIZE( g_dict_words ); i++ )
      {
         if( i >= c_max_dict_words - c_dict_words_offset )
            throw runtime_error( "exceeded maximum CLZ dict words allowed" );

         string word( g_dict_words[ i ].p_w );

         string::size_type pos = buffer_str.find( word );

         // NOTE: Although the same word would not be expected to be found
         // twice (as the second occurrence would be a back-ref) if a word
         // is actually the prefix of a longer word then it still might be
         // found twice (or even more times).
         while( pos != string::npos )
         {
            dict_words_found[ pos ] = i;
            pos = buffer_str.find( word, pos + word.length( ) );
         }
      }

      for( size_t i = 0; i < length; i++ )
      {
         unsigned char next = *( p_buffer + i );

         if( mark_pos && !shrunken_mark_pos && i >= mark_pos )
            shrunken_mark_pos = num;

         bool just_had_back_ref = false;

         if( num > 2 && last_back_ref_pos >= num - 2 )
            just_had_back_ref = true;

         bool is_special_numeric = false;

         if( !repeats && !just_had_back_ref && ( i == 0 || !( last_ch & c_high_bit_value ) )
          && ( next == ' ' || ( next >= '0' && next <= '9' ) || ( next >= '+' && next <= '.' ) ) )
            is_special_numeric = true;

         if( !is_special_numeric && special_numeric_length )
         {
            if( special_numeric_start + special_numeric_length == num )
            {
               if( special_numeric_length >= 5
                || ( special_numeric_length == 4 && shrunken[ special_numeric_start ] != '0' ) )
                  num -= shrink_special_numeric( &shrunken[ special_numeric_start ], special_numeric_length );
            }

            special_numeric_length = 0;
         }

         if( !repeats && dict_words_found.count( i ) )
         {
            if( dict_words_run && last_dict_word_pos == num - 2 )
            {
               ++dict_words_run;
               last_dict_word_pos += 2;
            }
            else
            {
               dict_words_run = 1;
               last_dict_word_pos = num;
            }

            string word( g_dict_words[ dict_words_found[ i ] ].p_w );

            shrunken[ num++ ] = c_special_dict_word_list_num;
            shrunken[ num++ ] = c_dict_words_offset + ( unsigned char )dict_words_found[ i ];

            stepping_amount = 0;

            i += word.length( ) - 1;

            continue;
         }

         // NOTE: If a run of three or more dict words in a row was
         // found then change the byte that immediately follows the
         // dict word special indicator to instead become the total
         // number of words to follow (as now only each words index
         // is required).
         if( dict_words_run > 2 && last_dict_word_pos == num - 2 )
         {
            if( dict_words_run > c_max_dict_words_run )
               dict_words_run = c_max_dict_words_run;

            size_t from = num - ( dict_words_run * 2 ) + 1;
            size_t dest = from + 1;

            for( size_t j = 0; j < dict_words_run; j++ )
            {
               unsigned char val = shrunken[ from ];
               shrunken[ dest++ ] = val;

               if( j == 0 )
                  shrunken[ from ] = ( dict_words_run - 3 );

               from += 2;
            }

            num = dest;
            dict_words_run = 0;
         }

         if( stepping_amount
          && ( step_type_val == e_step_type_single || i < length - 1 ) )
         {
            if( stepping_bytes )
            {
               if( stepping_bytes == 2 && i < length - 1 )
               {
                  unsigned char low_byte = *( p_buffer + i + 1 );

                  size_t next_val = ( next << 8 ) + low_byte;

                  if( ( ( steps_ascending && next_val == stepping_last_val + stepping_amount )
                   || ( !steps_ascending && next_val == stepping_last_val - stepping_amount ) )
                   && ( shrunken[ num - 1 ] & c_nibble_two ) < c_max_special_step_vals )
                  {
                     ++i;
                     ++shrunken[ num - 1 ];
                     stepping_last_val = next_val;

                     continue;
                  }
                  else
                     stepping_bytes = stepping_amount = stepping_last_val = 0;
               }
            }
            else
            {
               bool okay = true;
               size_t next_val = last_ch;

               unsigned char new_next = next;

               if( step_type_val == e_step_type_double
               || step_type_val == e_step_type_double_high )
                  next_val = *( p_buffer + i - 2 );

               if( steps_ascending )
                  next_val += stepping_amount;
               else
                  next_val -= stepping_amount;

               if( step_type_val == e_step_type_double )
               {
                  if( ( steps_ascending && ( last_ch + stepping_amount ) != *( p_buffer + i + 1 ) )
                  || ( !steps_ascending && ( last_ch - stepping_amount ) != *( p_buffer + i + 1 ) ) )
                     okay = false;
               }
               else if( step_type_val == e_step_type_double_low )
               {
                  if( new_next == *( p_buffer + i - 2 ) )
                     new_next = *( p_buffer + i + 1 );
                  else
                     okay = false;
               }
               else if( step_type_val == e_step_type_double_high )
               {
                  if( last_ch != *( p_buffer + i + 1 ) )
                     okay = false;
               }

               if( okay && new_next == next_val
               && ( shrunken[ num - 1 ] & c_nibble_two ) < c_max_special_step_vals )
               {
                  if( step_type_val >= e_step_type_double )
                     ++i;

                  if( step_type_val != e_step_type_double_high )
                     last_ch = new_next;

                  ++shrunken[ num - 1 ];

                  continue;
               }
               else
                  stepping_amount = 0;
            }
         }

         if( repeats && ( next != last_ch || repeats == c_max_special_repeats ) )
         {
            output_repeats( shrunken, repeats, num );

            if( next == last_ch )
               last_ch = c_special_maxval;

            repeats = 0;
         }

         // NOTE: If a byte pair had been identified as a special pair then
         // append its special marker otherwise simply append the values.
         if( i < length - 1 && ( next & c_high_bit_value ) )
         {
            byte_pair next_pair;

            next_pair.first = next;
            next_pair.second = *( p_buffer + ++i );

            if( ( next & c_nibble_one ) < c_nibble_one )
               last_back_ref_pos = num;

            if( special_nums.count( next_pair ) )
            {
               shrunken[ num ] = ( c_special_maxval - special_nums[ next_pair ] );

               if( available_specials && last_special_pos == num - 1 )
               {
                  bool add_new_pair = true;

                  byte_pair new_pair( shrunken[ num - 1 ], shrunken[ num ] );

                  // NOTE: As runs of three (or more) specials are entirely possible overlaps
                  // could end up occurring, however, assuming the run "fefdfe" then in order
                  // to decide whether to use "fefd" or "fdfe" first just add "fefd" but then
                  // if "fdfe" had appeared more times previously then remove the last "fefd"
                  // before adding the new "fdfe" (this doesn't take into account what is yet
                  // to come so it could end up making a worser choice unfortunately).
                  if( repeated_special_offsets.count( num - 2 ) )
                  {
                     if( repeated_special_counts.count( new_pair )
                      < repeated_special_counts.count( repeated_special_offsets[ num - 2 ] ) )
                        add_new_pair = false;
                     else
                     {
                        --repeated_special_counts[ repeated_special_offsets[ num - 2 ] ];
                        repeated_special_offsets.erase( num - 2 );
                     }
                  }

                  if( add_new_pair )
                  {
                     ++repeated_special_counts[ new_pair ];
                     repeated_special_offsets[ num - 1 ] = new_pair;
                  }
               }

               last_special_pos = num++;

               // NOTE: Cannot permit a dict or numeric pattern to immediately follow a special
               // pair as "expand" cannot differentiate between back-refs and back-ref repeats.
               last_ch = c_special_maxval;
            }
            else
            {
               shrunken[ num++ ] = next_pair.first;
               shrunken[ num++ ] = next_pair.second;

               // NOTE: If this was a back-ref repeat then a dict or numeric pattern can follow
               // immediately after it as this wouldn't be ambiguous when processed in "expand".
               if( ( next & c_nibble_one ) == c_nibble_one )
                  last_ch = 0;
               else
                  last_ch = c_special_maxval;
            }
         }
         else
         {
            bool was_dict = false;
            bool new_repeat = false;

            // NOTE: Look for common dictionary word patterns of three bytes (which might just
            // be prefixes or suffixes or letters in the middle of an actual dictionary word).
            if( !repeats && i < length - 2 && ( i == 0 || !( last_ch & c_high_bit_value ) ) )
            {
               string pat( 1, next );
               pat += *( p_buffer + i + 1 );
               pat += *( p_buffer + i + 2 );

               map< string, unsigned char >::const_iterator ci = g_dict_lower.find( pat );

               if( ci != g_dict_lower.end( ) )
               {
                  was_dict = true;
                  shrunken[ num++ ] = c_special_dict_pattern_lower;
                  shrunken[ num++ ] = ci->second;
               }
               else
               {
                  ci = g_dict_mixed.find( pat );

                  if( ci != g_dict_mixed.end( ) )
                  {
                     was_dict = true;
                     shrunken[ num++ ] = c_special_dict_pattern_mixed;
                     shrunken[ num++ ] = ci->second;
                  }
                  else
                  {
                     ci = g_dict_upper.find( pat );

                     if( ci != g_dict_upper.end( ) )
                     {
                        was_dict = true;
                        shrunken[ num++ ] = c_special_dict_pattern_upper;
                        shrunken[ num++ ] = ci->second;
                     }
                  }
               }

               if( was_dict )
               {
                  i += 2;
                  last_ch = 0; // NOTE: To allow one dict pattern to follow another.

                  continue;
               }
            }

            if( !repeats && last_ch && next == last_ch )
            {
               // NOTE: This will be ignored if is already in a long
               // enough special numeric run (as follows).
               if( i < length - 1 && *( p_buffer + i + 1 ) == next )
                  new_repeat = true;
            }

            if( repeats && next == last_ch )
               ++repeats;
            else
            {
               bool found_steps = false;
               bool ignore_steps = false;

               if( is_special_numeric
                && ( special_numeric_start + special_numeric_length == num ) )
               {
                  if( special_numeric_length >= 3 )
                     new_repeat = false;

                  if( special_numeric_length >= 4 )
                     ignore_steps = true;
               }

               if( !ignore_steps && ( i < length - 3 )
                && ( i == 0 || !( last_ch & c_high_bit_value ) ) )
               {
                  unsigned char next_after = *( p_buffer + i + 1 );

                  // NOTE: A few simple stepping patterns are checked for first and if none of these can
                  // be found then a more generalised approach (requiring more bytes to encode) is used.
                  if( ( next + 1 ) == *( p_buffer + i + 1 )
                   && ( next + 2 ) == *( p_buffer + i + 2 )
                   && ( next + 3 ) == *( p_buffer + i + 3 ) )
                  {
                     if( !( ( next + 3 ) & c_high_bit_value ) )
                     {
                        stepping_bytes = 0;
                        stepping_amount = 1;
                        step_type_val = e_step_type_single;
                        found_steps = steps_ascending = true;

                        i += 3;
                        last_ch = *( p_buffer + i );

                        shrunken[ num++ ] = next;
                        shrunken[ num++ ] = c_special_step_pattern_fixed;
                        shrunken[ num++ ] = c_special_byte_inc;
                     }
                  }
                  else if( ( next - 1 ) == *( p_buffer + i + 1 )
                   && ( next - 2 ) == *( p_buffer + i + 2 )
                   && ( next - 3 ) == *( p_buffer + i + 3 ) )
                  {
                     if( !( ( next - 3 ) & c_high_bit_value ) )
                     {
                        found_steps = true;
                        stepping_bytes = 0;
                        stepping_amount = 1;
                        steps_ascending = false;
                        step_type_val = e_step_type_single;

                        i += 3;
                        last_ch = *( p_buffer + i );

                        shrunken[ num++ ] = next;
                        shrunken[ num++ ] = c_special_step_pattern_fixed;
                        shrunken[ num++ ] = c_special_byte_dec;
                     }
                  }
                  else if( i < length - 5
                   && ( next + 1 ) == *( p_buffer + i + 2 ) && ( next_after + 1 ) == *( p_buffer + i + 3 )
                   && ( next + 2 ) == *( p_buffer + i + 4 ) && ( next_after + 2 ) == *( p_buffer + i + 5 ) )
                  {
                     if( !( next_after & c_high_bit_value )
                      && !( ( next + 2 ) & c_high_bit_value )
                      && !( ( next_after + 2 ) & c_high_bit_value ) )
                     {
                        stepping_bytes = 0;
                        stepping_amount = 1;
                        step_type_val = e_step_type_double;
                        found_steps = steps_ascending = true;

                        i += 5;
                        last_ch = *( p_buffer + i );

                        shrunken[ num++ ] = next;
                        shrunken[ num++ ] = next_after;
                        shrunken[ num++ ] = c_special_step_pattern_fixed;
                        shrunken[ num++ ] = c_special_pair_inc;
                     }
                  }
                  else if( i < length - 5
                   && ( next - 1 ) == *( p_buffer + i + 2 ) && ( next_after - 1 ) == *( p_buffer + i + 3 )
                   && ( next - 2 ) == *( p_buffer + i + 4 ) && ( next_after - 2 ) == *( p_buffer + i + 5 ) )
                  {
                     if( !( next_after & c_high_bit_value )
                      && !( ( next - 2 ) & c_high_bit_value )
                      && !( ( next_after - 2 ) & c_high_bit_value ) )
                     {
                        found_steps = true;
                        stepping_bytes = 0;
                        stepping_amount = 1;
                        steps_ascending = false;
                        step_type_val = e_step_type_double;

                        i += 5;
                        last_ch = *( p_buffer + i );

                        shrunken[ num++ ] = next;
                        shrunken[ num++ ] = next_after;
                        shrunken[ num++ ] = c_special_step_pattern_fixed;
                        shrunken[ num++ ] = c_special_pair_dec;
                     }
                  }
                  else if( i < length - 5
                   && next == *( p_buffer + i + 2 ) && ( next_after + 1 ) == *( p_buffer + i + 3 )
                   && next == *( p_buffer + i + 4 ) && ( next_after + 2 ) == *( p_buffer + i + 5 ) )
                  {
                     if( !( next_after & c_high_bit_value )
                      && !( ( next_after + 2 ) & c_high_bit_value ) )
                     {
                        stepping_bytes = 0;
                        stepping_amount = 1;
                        found_steps = steps_ascending = true;
                        step_type_val = e_step_type_double_low;

                        i += 5;
                        last_ch = *( p_buffer + i );

                        shrunken[ num++ ] = next;
                        shrunken[ num++ ] = next_after;
                        shrunken[ num++ ] = c_special_step_pattern_fixed;
                        shrunken[ num++ ] = c_special_pair_low_inc;
                     }
                  }
                  else if( i < length - 5
                   && next == *( p_buffer + i + 2 ) && ( next_after - 1 ) == *( p_buffer + i + 3 )
                   && next == *( p_buffer + i + 4 ) && ( next_after - 2 ) == *( p_buffer + i + 5 ) )
                  {
                     if( !( next_after & c_high_bit_value )
                      && !( ( next_after - 2 ) & c_high_bit_value ) )
                     {
                        found_steps = true;
                        stepping_bytes = 0;
                        stepping_amount = 1;
                        steps_ascending = false;
                        step_type_val = e_step_type_double_low;

                        i += 5;
                        last_ch = *( p_buffer + i );

                        shrunken[ num++ ] = next;
                        shrunken[ num++ ] = next_after;
                        shrunken[ num++ ] = c_special_step_pattern_fixed;
                        shrunken[ num++ ] = c_special_pair_low_dec;
                     }
                  }
                  else if( i < length - 5
                   && ( next + 1 ) == *( p_buffer + i + 2 ) && next_after == *( p_buffer + i + 3 )
                   && ( next + 2 ) == *( p_buffer + i + 4 ) && next_after == *( p_buffer + i + 5 ) )
                  {
                     if( !( next_after & c_high_bit_value )
                      && !( ( next + 2 ) & c_high_bit_value ) )
                     {
                        stepping_bytes = 0;
                        stepping_amount = 1;
                        found_steps = steps_ascending = true;
                        step_type_val = e_step_type_double_high;

                        i += 5;
                        last_ch = *( p_buffer + i );

                        shrunken[ num++ ] = next;
                        shrunken[ num++ ] = next_after;
                        shrunken[ num++ ] = c_special_step_pattern_fixed;
                        shrunken[ num++ ] = c_special_pair_high_inc;
                     }
                  }
                  else if( i < length - 5
                   && ( next - 1 ) == *( p_buffer + i + 2 ) && next_after == *( p_buffer + i + 3 )
                   && ( next - 2 ) == *( p_buffer + i + 4 ) && next_after == *( p_buffer + i + 5 ) )
                  {
                     if( !( next_after & c_high_bit_value )
                      && !( ( next - 2 ) & c_high_bit_value ) )
                     {
                        found_steps = true;
                        stepping_bytes = 0;
                        stepping_amount = 1;
                        steps_ascending = false;
                        step_type_val = e_step_type_double_high;

                        i += 5;
                        last_ch = *( p_buffer + i );

                        shrunken[ num++ ] = next;
                        shrunken[ num++ ] = next_after;
                        shrunken[ num++ ] = c_special_step_pattern_fixed;
                        shrunken[ num++ ] = c_special_pair_high_dec;
                     }
                  }
                  else
                  {
                     // NOTE: If groups of bytes are found to be in a run of incrementing or decrementing steps then
                     // these can be shrunk also (this takes more space as the stepping amount is determined via the
                     // first two byte groups).
                     stepping_amount = found_stepping_bytes( p_buffer,
                      i, length, stepping_bytes, steps_ascending, stepping_last_val );

                     if( stepping_amount )
                     {
                        found_steps = true;

                        if( stepping_bytes == 1 )
                           step_type_val = e_step_type_single;
                        else if( stepping_bytes == 2 )
                           step_type_val = e_step_type_double;

                        shrunken[ num++ ] = next;
                        shrunken[ num++ ] = last_ch = *( p_buffer + ++i );

                        if( stepping_bytes == 2 )
                        {
                           shrunken[ num++ ] = *( p_buffer + ++i );
                           shrunken[ num++ ] = last_ch = *( p_buffer + ++i );
                        }

                        shrunken[ num++ ] = c_special_step_pattern_multi;
                        shrunken[ num++ ] = ( stepping_bytes - 1 ) << 4;
                     }
                  }
               }

               if( !found_steps )
               {
                  if( new_repeat )
                  {
                     ++repeats;
                     special_numeric_length = 0;
                  }
                  else
                  {
                     if( is_special_numeric )
                     {
                        if( !special_numeric_length || ( special_numeric_start + special_numeric_length == num ) )
                        {
                           if( !special_numeric_length )
                              special_numeric_start = num;
                           ++special_numeric_length;
                        }
                        else
                        {
                           // NOTE: Restart a special numeric characters run.
                           special_numeric_start = num;
                           special_numeric_length = 1;
                        }
                     }

                     shrunken[ num++ ] = last_ch = next;
                  }
               }
            }
         }
      }

      // NOTE: If finished either with a single character repeat pattern or a special compressed
      // numeric then complete the output for that pattern before then processing special pairs.
      if( repeats )
         output_repeats( shrunken, repeats, num );
      else if( special_numeric_length && special_numeric_start + special_numeric_length == num )
      {
         if( special_numeric_length >= 5
          || ( special_numeric_length == 4 && shrunken[ special_numeric_start ] != '0' ) )
            num -= shrink_special_numeric( &shrunken[ special_numeric_start ], special_numeric_length );
      }

      vector< byte_pair > extra_specials;

      if( !repeated_special_counts.empty( ) )
      {
         map< size_t, byte_pair > ordered;

         for( map< byte_pair, size_t >::iterator
          i = repeated_special_counts.begin( ); i != repeated_special_counts.end( ); ++i )
         {
            if( i->second > 2 )
               ordered.insert( make_pair( i->second, i->first ) );
         }

         while( ordered.size( ) > available_specials )
            ordered.erase( ordered.begin( ) );

         set< byte_pair > repeated_specials;
         map< byte_pair, size_t > repeated_specials_used;

         for( map< size_t, byte_pair >::iterator oi = ordered.begin( ); oi != ordered.end( ); ++oi )
            repeated_specials.insert( oi->second );

         size_t already_adjusted = 0;

         // NOTE: Any pairs of specials that had been repeated three or more times can also
         // become specials (called "extra-specials") if the maximum number of specials had
         // not been already used. Such replacements require memmove's to occur as they are
         // being done after the fact rather than when initially compressing as occurs with
         // the "normal" specials.
         for( map< size_t, byte_pair >::iterator
          i = repeated_special_offsets.begin( ); i != repeated_special_offsets.end( ); ++i )
         {
            if( repeated_specials.count( i->second ) )
            {
               size_t special_num = specials.size( );

               if( repeated_specials_used.count( i->second ) )
                  special_num = repeated_specials_used[ i->second ];
               else
               {
                  special_num += extra_specials.size( );
                  repeated_specials_used[ i->second ] = special_num;

                  extra_specials.push_back( i->second );
               }

               size_t offset = ( i->first - already_adjusted );

               shrunken[ offset ] = ( c_special_maxval - special_num );
               memmove( &shrunken[ offset + 1 ], &shrunken[ offset + 2 ], num - offset );

               --num;
               ++already_adjusted;
            }
         }
      }

      // NOTE: Specials are appended after the special marker.
      if( specials.size( ) )
         shrunken[ num++ ] = c_special_marker;

      for( set< byte_pair >::iterator si = specials.begin( ); si != specials.end( ); ++si )
      {
         shrunken[ num++ ] = si->first;
         shrunken[ num++ ] = si->second;
      }

      for( size_t i = 0; i < extra_specials.size( ); i++ )
      {
         shrunken[ num++ ] = extra_specials[ i ].first;
         shrunken[ num++ ] = extra_specials[ i ].second;
      }

      if( num < length )
      {
         memcpy( p_buffer, shrunken, length = num );

#ifdef DEBUG_ENCODE
         if( !shrunken_mark_pos )
            dump_bytes( "shrunken ==>", shrunken, num );
#endif
         if( shrunken_mark_pos )
            dump_bytes( "shrunken ==>", shrunken, num, shrunken_mark_pos );
      }
   }
}

bool uncompress_numeric_pair( unsigned char* p_buffer, size_t& length, unsigned char ch )
{
   bool retval = true;

   unsigned char nibble1 = ( ch & c_nibble_one ) >> 4;
   unsigned char nibble2 = ( ch & c_nibble_two );

   if( nibble1 == c_nibble_two )
      retval = false;
   else
   {
      if( nibble1 >= 0 && nibble1 <= 9 )
         *( p_buffer + length++ ) = '0' + nibble1;
      else if( nibble1 >= 10 && nibble1 <= 13 )
         *( p_buffer + length++ ) = '+' + ( nibble1 - 10 );
      else if( nibble1 == 14 )
         *( p_buffer + length++ ) = ' ';

      if( nibble2 >= 0 && nibble2 <= 9 )
         *( p_buffer + length++ ) = '0' + nibble2;
      else if( nibble2 >= 10 && nibble2 <= 13 )
         *( p_buffer + length++ ) = '+' + ( nibble2 - 10 );
      else if( nibble2 == 14 )
         *( p_buffer + length++ ) = ' ';

      if( nibble2 == c_nibble_two )
         retval = false;
   }

   return retval;
}

size_t expand_input( istream& is, unsigned char* p_buffer, size_t max_length )
{
   size_t length = 0;
   size_t num_specials = 0;

   unsigned char last_ch = 0;

   bool had_marker = false;
   bool process_steps = false;
   bool steps_are_multi = false;

   bool is_compressed_numeric = false;
   bool is_lower_dict_pattern = false;
   bool is_mixed_dict_pattern = false;
   bool is_upper_dict_pattern = false;

   set< size_t > back_refs;
   map< size_t, size_t > specials;

   if( g_dict_lower.empty( ) )
      init_clz_info( );

   memset( p_buffer, 0, max_length );

   while( is )
   {
      unsigned char ch;

      if( !is.read( ( char* )&ch, 1 ) )
         break;

      if( process_steps )
      {
         process_steps = false;

         bool is_low = true;
         bool is_both = false;
         bool ascending = true;
         bool is_byte_pair = false;

         size_t stepping_amount = 0;

         size_t num_repeats = ( ch & c_nibble_two );

         // NOTE: First handle a few very specific stepping patterns
         // with a more generalised approach (which is less compact)
         // to follow.
         if( ( ch & c_nibble_one ) == c_special_byte_inc )
         {
            num_repeats += 3;
            stepping_amount = 1;
         }
         else if( ( ch & c_nibble_one ) == c_special_byte_dec )
         {
            num_repeats += 3;
            ascending = false;
            stepping_amount = 1;
         }
         else if( ( ch & c_nibble_one ) == c_special_pair_inc )
         {
            num_repeats += 2;
            stepping_amount = 1;
            is_both = is_byte_pair = true;
         }
         else if( ( ch & c_nibble_one ) == c_special_pair_dec )
         {
            num_repeats += 2;
            ascending = false;
            stepping_amount = 1;
            is_both = is_byte_pair = true;
         }
         else if( ( ch & c_nibble_one ) == c_special_pair_low_inc )
         {
            num_repeats += 2;
            is_byte_pair = true;
            stepping_amount = 1;
         }
         else if( ( ch & c_nibble_one ) == c_special_pair_low_dec )
         {
            num_repeats += 2;
            ascending = false;
            is_byte_pair = true;
            stepping_amount = 1;
         }
         else if( ( ch & c_nibble_one ) == c_special_pair_high_inc )
         {
            is_low = false;
            num_repeats += 2;
            is_byte_pair = true;
            stepping_amount = 1;
         }
         else if( ( ch & c_nibble_one ) == c_special_pair_high_dec )
         {
            is_low = false;
            num_repeats += 2;
            ascending = false;
            is_byte_pair = true;
            stepping_amount = 1;
         }
         else
         {
            size_t bytes = ( ( ch & c_nibble_one ) >> 4 ) + 1;

            if( bytes == 1 )
            {
               unsigned char byte1 = *( p_buffer + length - 2 );
               unsigned char byte2 = *( p_buffer + length - 1 );

               ascending = ( byte1 < byte2 ? true : false );
               stepping_amount = ( ascending ? byte2 - byte1 : byte1 - byte2 ); 
            }
            else if( bytes == 2 )
            {
               unsigned char byte1 = *( p_buffer + length - 4 );
               unsigned char byte2 = *( p_buffer + length - 3 );
               unsigned char byte3 = *( p_buffer + length - 2 );
               unsigned char byte4 = *( p_buffer + length - 1 );

               size_t val1 = ( byte1 << 8 ) + byte2;
               size_t val2 = ( byte3 << 8 ) + byte4;

               ascending = ( val1 < val2 );
               stepping_amount = ( ascending ? val2 - val1 : val1 - val2 );

               size_t last_val = val2;

               for( size_t i = 0; i < num_repeats; i++ )
               {
                  size_t next_val = last_val;

                  if( ascending )
                     next_val += stepping_amount;
                  else
                     next_val -= stepping_amount;

                  byte1 = ( next_val >> 8 );
                  byte2 = ( next_val & 0xff );

                  *( p_buffer + length++ ) = byte1;
                  *( p_buffer + length++ ) = byte2;

                  last_val = next_val;
               }

               if( length >= max_length )
                  break;
               else
                  continue;
            }
            // FUTURE: Should handle patterns of 3 and 4 bytes as well (and maybe more).
         }

         if( !is_byte_pair )
         {
            for( size_t i = 0; i < num_repeats; i++ )
            {
               if( ascending )
                  *( p_buffer + length ) = *( p_buffer + length++ - 1 ) + stepping_amount;
               else
                  *( p_buffer + length ) = *( p_buffer + length++ - 1 ) - stepping_amount;
            }
         }
         else
         {
            for( size_t i = 0; i < num_repeats; i++ )
            {
               if( ascending )
               {
                  *( p_buffer + length ) = *( p_buffer + length++ - 2 ) + ( ( is_low && !is_both ) ? 0 : stepping_amount );
                  *( p_buffer + length ) = *( p_buffer + length++ - 2 ) + ( ( is_low || is_both ) ? stepping_amount : 0 );
               }
               else
               {
                  *( p_buffer + length ) = *( p_buffer + length++ - 2 ) - ( ( is_low && !is_both ) ? 0 : stepping_amount );
                  *( p_buffer + length ) = *( p_buffer + length++ - 2 ) - ( ( is_low || is_both ) ? stepping_amount : 0 );
               }
            }
         }

         if( length >= max_length )
            break;
         else
            continue;
      }
      else if( is_compressed_numeric )
      {
         is_compressed_numeric = uncompress_numeric_pair( p_buffer, length, ch );

         last_ch = *( p_buffer + length - 1 );

         continue;
      }
      else if( is_lower_dict_pattern || is_mixed_dict_pattern || is_upper_dict_pattern )
      {
         map< string, unsigned char >::const_iterator ci;

         if( is_lower_dict_pattern )
            ci = g_dict_lower.begin( );
         else if( is_mixed_dict_pattern )
            ci = g_dict_mixed.begin( );
         else
            ci = g_dict_upper.begin( );

         for( unsigned char i = 0; i < ch; i++ )
            ++ci;

         string pat( ci->first );
        
         *( p_buffer + length++ ) = pat[ 0 ];
         *( p_buffer + length++ ) = pat[ 1 ];
         *( p_buffer + length++ ) = pat[ 2 ];

         last_ch = 0;
         is_lower_dict_pattern = is_mixed_dict_pattern = is_upper_dict_pattern = false;

         if( length >= max_length )
            break;
         else
            continue;
      }

      if( !back_refs.count( length - 1 ) && ch >= c_special_dict_word_list_num )
      {
         if( ch == c_special_dict_word_list_num )
         {
            if( !is.read( ( char* )&ch, 1 ) )
               throw runtime_error( "unexpected eof found while expanding dict words" );

            size_t num_words = 1;

            // NOTE: If the value found is less than the minimum offset then it is instead
            // the number of word offsets to follow (for a sequential run of dict words).
            if( ch < c_dict_words_offset )
            {
               num_words = ch + 3;

               if( !is.read( ( char* )&ch, 1 ) )
                  throw runtime_error( "unexpected eof found while expanding dict words" );
            }

            while( true )
            {
               string word( g_dict_words[ ch - c_dict_words_offset ].p_w );

               for( size_t i = 0; i < word.length( ); i++ )
                  *( p_buffer + length++ ) = word[ i ];

               if( --num_words == 0 )
                  break;

               if( !is.read( ( char* )&ch, 1 ) )
                  throw runtime_error( "unexpected eof found while expanding dict words" );
            }

            --length; // NOTE: Due to the increment below.
         }
         // NOTE: Finish if found specials marker or keep track of a
         // special for later replacement/expansion.
         else if( ch == c_special_marker )
         {
            had_marker = true;
            break;
         }
         else
         {
            specials[ length++ ] = ( c_special_maxval - ch );

            if( ( c_special_maxval - ch ) + 1 > num_specials )
               num_specials = ( c_special_maxval - ch ) + 1;
         }
      }
      else
      {
         bool had_repeat = false;

         if( length >= 2 && back_refs.count( length - 2 )
          && ( ( *( p_buffer + length - 2 ) & c_nibble_one ) == c_nibble_one ) )
            had_repeat = true;

         // NOTE: The "back_refs" container used here is for holding both back-refs and back-ref
         // repeats (so that these are not confused with specials, dictionary patterns or single
         // character repeats).
         if( ( ch & c_high_bit_value )
          && !back_refs.count( length - 1 )
          && ( ( ( ch & c_nibble_one ) != c_nibble_one )
          || ( !had_repeat && ( back_refs.count( length - 2 ) || specials.count( length - 2 ) ) ) ) )
            back_refs.insert( length );

         bool is_expanded = false;

         if( ( ( ch & c_nibble_one ) == c_nibble_one ) )
         {
            if( !back_refs.count( length )
             && !back_refs.count( length - 1 ) && !specials.count( length - 2 ) )
            {
               is_expanded = true;

               if( ch == c_special_compressed_numeric )
                  is_compressed_numeric = true;
               else if( ch == c_special_dict_pattern_lower )
                  is_lower_dict_pattern = true;
               else if( ch == c_special_dict_pattern_mixed )
                  is_mixed_dict_pattern = true;
               else if( ch == c_special_dict_pattern_upper )
                  is_upper_dict_pattern = true;
               else if( ch == c_special_step_pattern_fixed || ch == c_special_step_pattern_multi )
               {
                  process_steps = true;
                  steps_are_multi = ( ch == c_special_step_pattern_multi );
               }
               else
               {
                  if( ch == c_special_char_double_repeat )
                  {
                     *( p_buffer + length++ ) = last_ch;
                     *( p_buffer + length++ ) = last_ch;
                  }
                  else
                  {
                     // NOTE: For more than two repeats the number
                     // of repeats is found in the following byte.
                     if( !is.read( ( char* )&ch, 1 ) )
                        throw runtime_error( "unexpected eof found while expanding repeats" );

                     // NOTE: A special compressed numeric which is
                     // exactly four digits in length is identified
                     // here (but it cannot start with a zero).
                     if( ch > c_nibble_two )
                     {
                        uncompress_numeric_pair( p_buffer, length, ch );

                        if( !is.read( ( char* )&ch, 1 ) )
                           throw runtime_error( "unexpected eof found while expanding special numeric" );

                        uncompress_numeric_pair( p_buffer, length, ch );
                     }
                     else
                     {
                        for( size_t i = 0; i < ch; i++ )
                           *( p_buffer + length++ ) = last_ch;
                     }
                  }

                  ch = c_nibble_one;
               }

               --length; // NOTE: Due to the increment below.
            }
         }

         last_ch = ch;

         if( !is_expanded )
            *( p_buffer + length ) = ch;
      }

      if( ++length >= max_length )
         break;
   }

   if( num_specials )
   {
      vector< byte_pair > special_pairs;

      if( !had_marker )
      {
         unsigned char ch;

         if( !is.read( ( char* )&ch, 1 ) || ch != c_special_marker )
            throw runtime_error( "unexpected missing specials marker in expand_input" );
      }

      for( size_t i = 0; i < num_specials; i++ )
      {
         unsigned char byte1, byte2;

         if( !is.read( ( char* )&byte1, 1 ) || !is.read( ( char* )&byte2, 1 ) )
            throw runtime_error( "unexpected missing special replacement in expand_input" );

         special_pairs.push_back( byte_pair( byte1, byte2 ) );
      }

      size_t already_adjusted = 0;

      for( map< size_t, size_t >::iterator i = specials.begin( ); i != specials.end( ); ++i )
      {
         size_t offset = ( i->first + already_adjusted );

         *( p_buffer + offset ) = special_pairs[ i->second ].first;
         *( p_buffer + offset + 1 ) = special_pairs[ i->second ].second;

         // NOTE: If extra specials were used then need to expand them as two normal specials
         // along with doing a memmove to make room for the doubled expansion.
         if( *( p_buffer + offset ) > c_special_marker )
         {
            size_t num_1 = c_special_maxval - *( p_buffer + offset );
            size_t num_2 = c_special_maxval - *( p_buffer + offset + 1 );

            memmove( p_buffer + offset + 2, p_buffer + offset, length - offset );

            length += 2;
            already_adjusted += 2;

            *( p_buffer + offset ) = special_pairs[ num_1 ].first;
            *( p_buffer + offset + 1 ) = special_pairs[ num_1 ].second;
            *( p_buffer + offset + 2 ) = special_pairs[ num_2 ].first;
            *( p_buffer + offset + 3 ) = special_pairs[ num_2 ].second;
         }
      }
   }

#ifdef DEBUG_DECODE
dump_bytes( "expanded ==>", p_buffer, length );
#endif
   return length;
}

size_t longest_sequence( unsigned char* p_input,
 size_t input_size, unsigned char* p_buffer, size_t buffer_size,
 size_t& offset, unsigned char* p_output_buffer = 0, size_t output_buffer_size = 0 )
{
   // NOTE: If a sequence of input bytes starting at the first byte of the
   // input buffer that is longer than the minimum pattern length is found
   // in the search buffer then will return that length (along with having
   // set the offset value) but if no such sequence is found to occur then
   // this function will simply return one.
   size_t start = 0;
   size_t length = 1;

   if( !p_output_buffer )
   {
      p_output_buffer = p_buffer;
      output_buffer_size = buffer_size;
   }

   vector< size_t > start_offsets;

#ifndef NO_SHRINK_AND_EXPAND
   bool long_numeric_sequence = false;
   size_t numeric_sequence_length = 0;
#endif

   if( input_size >= c_min_pat_length )
   {
      string::size_type pos = 0;

      // NOTE: Find all minimum pattern length starting buffer offsets
      // to use for matching as many characters in the input buffer as
      // possible.
      while( true )
      {
         if( *( p_input ) == *( p_buffer + pos )
          && *( p_input + 1 ) == *( p_buffer + pos + 1 )
          && *( p_input + 2 ) == *( p_buffer + pos + 2 ) )
            start_offsets.push_back( pos );

         ++pos;

         if( pos >= buffer_size )
            break;
      }

#ifndef NO_SHRINK_AND_EXPAND
      for( size_t i = 0; i < input_size; i++ )
      {
         unsigned char ch = *( p_input + i );

         if( ch == ' ' || ( ch >= '0' && ch <= '9' ) || ( ch >= '+' && ch <= '.' ) )
            ++numeric_sequence_length;
         else
            break;
      }

      // NOTE: If the input contains long special numeric sequence
      // then will ignore the matching sequence if that's found to
      // not be long enough.
      if( numeric_sequence_length >= ( c_min_pat_length * 3 ) )
         long_numeric_sequence = true;
#endif
   }

   for( size_t s = 0; s < start_offsets.size( ); s++ )
   {
      start = start_offsets[ s ];

      size_t i = ( s > 0 ? c_min_pat_length : c_min_pat_length - 1 );

      for( ; i < input_size; i++ )
      {
         if( start + i >= buffer_size )
            break;

         if( *( p_buffer + start + i ) != *( p_input + i ) )
            break;
         else if( i >= length )
         {
            length = i + 1;
            offset = start;
         }
      }

      if( i == input_size )
      {
         length = input_size;
         offset = start;
         break;
      }
   }

   if( length < c_min_pat_length )
      length = 1;
#ifndef NO_SHRINK_AND_EXPAND
   else if( long_numeric_sequence && ( length < c_min_pat_length * 2 ) )
      length = 1;
   else if( numeric_sequence_length >= length
    && ( output_buffer_size > c_min_pat_length * 2 ) && ( length < c_min_pat_length * 2 ) )
   {
      bool prior_numeric_sequence = true;

      // NOTE: If a short enough entirely special numeric sequence follows another one that has just
      // been output then ignore this matching sequence.
      for( size_t i = output_buffer_size - 1; i >= output_buffer_size - ( c_min_pat_length * 2 ); i-- )
      {
         unsigned char ch = *( p_output_buffer + i );

         if( !( ch == ' ' || ( ch >= '0' && ch <= '9' ) || ( ch >= '+' && ch <= '.' ) ) )
         {
            prior_numeric_sequence = false;
            break;
         }
      }

      if( prior_numeric_sequence )
         length = 1;
   }
#endif

   return length;
}

bool combine_meta_patterns( meta_pattern_info& meta_patterns,
 unsigned char* p_buffer, size_t& offset, size_t& last_pattern_offset, size_t last_second_pass_offset )
{
   bool can_combine = true;
   bool can_continue = false;

   if( offset <= ( c_min_pat_length + c_meta_pat_length ) )
      can_combine = false;
   else
   {
      bool is_back_ref = false;

      // NOTE: If the first byte that will be used
      // to identify a pattern is actually part of
      // a back-ref then it cannot be combined.
      for( size_t i = 0; i < offset - 4; i++ )
      {
         if( is_back_ref )
            is_back_ref = false;
         else if( *( p_buffer + i ) & c_high_bit_value )
            is_back_ref = true;
      }

      if( is_back_ref || last_pattern_offset <= last_second_pass_offset )
         can_combine = false;
   }

   if( can_combine )
   {
      meta_pattern pat;

      pat.byte1 = *( p_buffer + offset - 4 );
      pat.byte2 = *( p_buffer + offset - 3 );
      pat.byte3 = *( p_buffer + offset - 2 );
      pat.byte4 = *( p_buffer + offset - 1 );

      if( ( pat.byte1 & c_nibble_one ) != c_nibble_one )
      {
         // NOTE: Firstly handle a simple pattern replace.
         if( meta_patterns.has_pattern( pat ) && meta_patterns.pattern_offset( pat ) < offset - 4 )
         {
            meta_patterns.remove_offsets_from( offset - 6 );

            *( p_buffer + offset - 4 ) = meta_patterns[ pat ].first;
            *( p_buffer + offset - 3 ) = meta_patterns[ pat ].second;
#ifdef DEBUG_ENCODE
cout << "replaced " << pat << " @" << ( offset - 4 ) << " with: " << meta_patterns[ pat ] << " (combine)" << endl;
#endif

            offset -= 2;
            can_continue = true;
            last_pattern_offset = ( offset - 2 );
         }
         // NOTE: Secondly handle a simple pattern repeat.
         else if( ( pat.byte1 & c_high_bit_value ) && pat.byte1 == pat.byte3 && pat.byte2 == pat.byte4 )
         {
            meta_patterns.remove_offsets_from( offset - 4 );

            meta_pattern rpl( pat );

            rpl.byte3 = c_nibble_one;
            rpl.byte4 = 0x00;

            *( p_buffer + offset - 2 ) = rpl.byte3;
            *( p_buffer + offset - 1 ) = rpl.byte4;
#ifdef DEBUG_ENCODE
cout << "replaced " << pat << " @" << ( offset - 4 ) << " with: " << rpl << " (repeats)" << endl;
#endif

            if( !meta_patterns.has_pattern( rpl ) )
               meta_patterns.add_pattern( rpl, offset - 4 );

            can_continue = true;
            last_pattern_offset = ( offset - 4 );
         }
         // NOTE: Handle the combination of two patterns.
         else if( meta_patterns.has_offset( offset - 6 )
          && meta_patterns.has_pattern( pat ) && meta_patterns.pattern_offset( pat ) < offset - 4 )
         {
            meta_patterns.remove_offsets_from( offset - 6 );

            *( p_buffer + offset - 4 ) = meta_patterns[ pat ].first;
            *( p_buffer + offset - 3 ) = meta_patterns[ pat ].second;

            pat.byte1 = *( p_buffer + offset - 6 );
            pat.byte2 = *( p_buffer + offset - 5 );
            pat.byte3 = *( p_buffer + offset - 4 );
            pat.byte4 = *( p_buffer + offset - 3 );
#ifdef DEBUG_ENCODE
cout << "combined @" << ( offset - 4 ) << " with: " << pat << endl;
dump_bytes( "", p_buffer, offset );
#endif

            if( !meta_patterns.has_pattern( pat ) )
               meta_patterns.add_pattern( pat, offset - 4 );

            offset -= 2;
            can_continue = true;
            last_pattern_offset = ( offset - 2 );
         }
         else
         {
            // NOTE: If the first two bytes part of the current meta-pattern points
            // to an existing meta-pattern then possibly add as a new meta-pattern.
            if( ( pat.byte1 & c_high_five_bits ) == c_meta_pattern_length_val && !meta_patterns.has_pattern( pat ) )
            {
               size_t first_offset = ( ( pat.byte1 & c_offset_high_byte_mask ) << 8 ) + pat.byte2;

               if( meta_patterns.has_offset( first_offset ) )
               {
                  meta_patterns.remove_offsets_from( offset - 4 );
                  meta_patterns.add_pattern( pat, offset - 4 );
               }
            }
         }
      }
      else
      {
         if( pat.byte3 == *( p_buffer + offset - 6 ) && pat.byte4 == *( p_buffer + offset - 5 ) )
         {
            meta_patterns.remove_offsets_from( offset - 6 );

            if( *( p_buffer + offset - 4 ) != c_max_repeats_hi
             || *( p_buffer + offset - 3 ) != c_max_repeats_lo )
            {
               if( *( p_buffer + offset - 3 ) != c_max_repeats_lo )
                  ++( *( p_buffer + offset - 3 ) );
               else
               {
                  *( p_buffer + offset - 3 ) = 0;
                  ++( *( p_buffer + offset - 4 ) );
               }

               pat.byte1 = *( p_buffer + offset - 6 );
               pat.byte2 = *( p_buffer + offset - 5 );
               pat.byte3 = *( p_buffer + offset - 4 );
               pat.byte4 = *( p_buffer + offset - 3 );
#ifdef DEBUG_ENCODE
cout << "inc removed @" << ( offset - 6 ) << " with: " << pat << endl;
#endif

               if( !meta_patterns.has_pattern( pat ) )
                  meta_patterns.add_pattern( pat, offset - 6 );

               offset -= 2;
               can_continue = true;
               last_pattern_offset = ( offset - 4 );
            }
         }
         else
         {
            meta_pattern rpt;

            rpt.byte1 = *( p_buffer + offset - 6 );
            rpt.byte2 = *( p_buffer + offset - 5 );
            rpt.byte3 = *( p_buffer + offset - 4 );
            rpt.byte4 = *( p_buffer + offset - 3 );

            // NOTE: As the repeat is followed by an unrelated pattern now check to see if a pattern
            // that combines the earlier pattern and its repeat amount had been added earlier (prior
            // to the original patterns offset as its repeat may have been the first such occurrence
            // of the combined pattern and repeat) and if so replace and reduce the offset.
            if( meta_patterns.has_pattern( rpt )
             && meta_patterns.pattern_offset( rpt ) < ( offset - 6 ) )
            {
               meta_patterns.remove_offsets_from( offset - 8 );

               pat.byte1 = meta_patterns[ rpt ].first;
               pat.byte2 = meta_patterns[ rpt ].second;

               *( p_buffer + offset - 6 ) = pat.byte1;
               *( p_buffer + offset - 5 ) = pat.byte2;
               *( p_buffer + offset - 4 ) = pat.byte3;
               *( p_buffer + offset - 3 ) = pat.byte4;
#ifdef DEBUG_ENCODE
cout << "backrepl @" << ( offset - 4 ) << " with: " << pat << endl;
#endif

               if( !meta_patterns.has_pattern( pat ) )
                  meta_patterns.add_pattern( pat, offset - 4 );

               offset -= 2;
               can_continue = true;
               last_pattern_offset = ( offset - 2 );
            }
         }
      }
   }

   return can_continue;
}

void perform_meta_combines( meta_pattern_info& meta_patterns,
 unsigned char* p_buffer, size_t& end_offset, size_t& last_pattern_offset, size_t last_second_pass_offset )
{
   for( size_t i = 0; i < c_max_combines; i++ )
   {
      if( !combine_meta_patterns( meta_patterns, p_buffer, end_offset, last_pattern_offset, last_second_pass_offset ) )
         break;
   }
}

bool replace_meta_pattern(
 meta_pattern_info& meta_patterns, unsigned char* p_buffer,
 size_t offset, unsigned char& new_byte1, unsigned char& new_byte2,
 size_t& end_offset, size_t& last_pattern_offset, size_t last_second_pass_offset )
{
   bool was_replaced = false;

   if( offset >= c_min_pat_length )
   {
      meta_pattern pat;

      pat.byte1 = *( p_buffer + offset );
      pat.byte2 = *( p_buffer + offset + 1 );

      if( ( pat.byte1 & c_high_bit_value )
       && ( ( pat.byte1 & c_nibble_one ) != c_nibble_one )
       && pat.byte1 == new_byte1 && pat.byte2 == new_byte2 )
      {
         new_byte1 = c_nibble_one;
         new_byte2 = 0x00;
      }

      pat.byte3 = new_byte1;
      pat.byte4 = new_byte2;

      // NOTE: If the meta-pattern already exists then replace the last back-ref
      // with the back-ref to the meta-pattern otherwise add a new meta-pattern.
      if( meta_patterns.has_pattern( pat ) )
      {
         was_replaced = true;
         last_pattern_offset = offset;

         size_t old_end_offset = end_offset;
         bool had_prior_pattern = meta_patterns.has_offset( offset - 2 );

         meta_patterns.remove_offsets_from( offset - 2 );

#ifdef DEBUG_ENCODE
cout << "replaced " << pat << " @" << offset << " with: " << meta_patterns[ pat ] << " (for new)" << endl;
#endif
         *( p_buffer + offset ) = meta_patterns[ pat ].first;
         *( p_buffer + offset + 1 ) = meta_patterns[ pat ].second;

         perform_meta_combines( meta_patterns,
          p_buffer, end_offset, last_pattern_offset, last_second_pass_offset );

         if( had_prior_pattern && old_end_offset == end_offset )
         {
            pat.byte1 = *( p_buffer + offset - 2 );
            pat.byte2 = *( p_buffer + offset - 1 );
            pat.byte3 = *( p_buffer + offset );
            pat.byte4 = *( p_buffer + offset + 1 );

            if( !meta_patterns.has_pattern( pat ) )
               meta_patterns.add_pattern( pat, offset - 2 );
         }
#ifdef DEBUG_ENCODE
dump_bytes( "modified ==>", p_buffer, end_offset );
#endif
      }
      else if( ( pat.byte1 & c_high_bit_value ) && ( ( pat.byte1 & c_nibble_one ) != c_nibble_one ) )
      {
         meta_patterns.remove_offsets_from( offset );

         last_pattern_offset = offset;
         meta_patterns.add_pattern( pat, offset );
      }
#ifdef DEBUG_ENCODE
cout << "************************" << endl;
check_meta_patterns( meta_patterns, p_buffer, offset );
cout << "************************" << endl;
#endif
   }

   return was_replaced;
}

bool replace_extra_pattern( meta_pattern_info& meta_patterns,
 map< string, size_t >& extra_patterns, const string& pattern, unsigned char* p_buffer, size_t& output_offset )
{
   bool was_replaced = false;

   // NOTE: An extra pattern might have become overwritten due to previously
   // added meta-patterns being combined (if so it will be erased to be then
   // re-added).
   if( extra_patterns.count( pattern ) )
   {
      size_t offset = extra_patterns[ pattern ];

      bool okay = true;

      if( offset >= output_offset - pattern.length( ) )
         okay = false;
      else
      {
         for( size_t i = 0; i < pattern.length( ); i++ )
         {
            if( *( p_buffer + offset + i ) != ( unsigned char )pattern[ i ] )
            {
               okay = false;
               break;
            }
         }
      }

      if( !okay )
         extra_patterns.erase( pattern );
   }

   if( !extra_patterns.count( pattern ) )
      extra_patterns.insert( make_pair( pattern, output_offset - 2 ) );
   else
   {
      was_replaced = true;

      size_t offset = extra_patterns[ pattern ];

      unsigned char byte1 = c_high_bit_value | ( ( offset & 0x0f00 ) >> 8 );
      byte1 |= ( pattern.length( ) - c_min_pat_length ) << 4;

      unsigned char byte2 = ( offset & 0x00ff );

      bool was_incremented = false;

      if( ( byte1 & c_high_bit_value )
       && ( output_offset > c_min_pat_length )
       && *( p_buffer + output_offset - 4 ) == byte1
       && *( p_buffer + output_offset - 3 ) == byte2 )
      {
         byte1 = c_nibble_one;
         byte2 = 0x00;
      }
      else if( output_offset > ( c_min_pat_length * 2 )
       && *( p_buffer + output_offset - 6 ) == byte1
       && *( p_buffer + output_offset - 5 ) == byte2
       && ( ( *( p_buffer + output_offset - 4 ) & c_nibble_one ) == c_nibble_one ) )
      {
         if( *( p_buffer + output_offset - 4 ) != c_max_repeats_hi
          || *( p_buffer + output_offset - 3 ) != c_max_repeats_lo )
         {
            if( *( p_buffer + output_offset - 3 ) != c_max_repeats_lo )
               ++( *( p_buffer + output_offset - 3 ) );
            else
            {
               *( p_buffer + output_offset - 3 ) = 0;
               ++( *( p_buffer + output_offset - 4 ) );
            }

            output_offset -= 2;
            was_incremented = true;
         }
      }

      if( !was_incremented )
      {
         *( p_buffer + output_offset - 2 ) = byte1;
         *( p_buffer + output_offset - 1 ) = byte2;
      }

#ifdef DEBUG_ENCODE
dump_bytes( "extra pattern ==>", ( unsigned char* )pattern.c_str( ), pattern.length( ) );
cout << "now becomes: "
 << hex << setw( 2 ) << setfill( '0' ) << ( int )byte1
 << setw( 2 ) << setfill( '0' ) << ( int )byte2 << dec << endl;
#endif
   }

   if( was_replaced )
   {
      meta_patterns.remove_offsets_from( output_offset - 4 );

      unsigned char ch = *( p_buffer + output_offset - 4 );

      // NOTE: If the extra pattern follows a previous back-ref
      // (but a not repeat) then add this as a new meta-pattern
      // (if it had not already been added).
      if( ch < c_nibble_one && ( ch & c_high_bit_value ) )
      {
         meta_pattern pat;

         pat.byte1 = *( p_buffer + output_offset - 4 );
         pat.byte2 = *( p_buffer + output_offset - 3 );
         pat.byte3 = *( p_buffer + output_offset - 2 );
         pat.byte4 = *( p_buffer + output_offset - 1 );

         if( !meta_patterns.has_pattern( pat ) )
            meta_patterns.add_pattern( pat, output_offset - 4 );
#ifdef DEBUG_ENCODE
cout << "************************" << endl;
check_meta_patterns( meta_patterns, p_buffer, output_offset );
cout << "************************" << endl;
#endif
      }
   }

   return was_replaced;
}

// NOTE: Format must be either <pat><rpt> or <pat><pat> with <pat> values being either in the
// form of a simple 7-bit pattern or another meta-pattern which will be expanded recursively.
string expand_meta_pattern( const string& meta, const unsigned char* p_encoded, size_t indent = 0, size_t offset = 0 )
{
   string pattern( meta );

   if( indent > c_max_expand_meta_recursion )
      throw runtime_error( "maximum recursion depth for expand_meta_pattern exceeded" );

   if( meta.length( ) >= 2 && ( meta[ 0 ] & c_high_bit_value ) )
   {
      unsigned char byte1 = meta[ 0 ];
      unsigned char byte2 = meta[ 1 ];

      size_t pat_length = ( byte1 & c_pattern_high_byte_mask ) >> 3;
      pat_length += c_min_pat_length;

      size_t pat_offset = ( byte1 & c_offset_high_byte_mask ) << 8;
      pat_offset += byte2;

#ifdef DEBUG_DECODE
cout << "pat_length = " << pat_length << ", pat_offset = " << pat_offset << endl;
#endif
      if( offset && pat_offset > offset )
         pattern = c_back_ref + to_string( pat_length ) + ',' + to_string( pat_offset - offset );
      else
      {
         pattern = string( ( const char* )( p_encoded + pat_offset ), pat_length );

         if( pattern[ 0 ] & c_high_bit_value )
         {
#ifdef DEBUG_DECODE
cout << "pattern: ";
if( indent )
cout << string( indent * 6, ' ' );
dump_bytes( "", ( unsigned char* )pattern.c_str( ), pattern.length( ) );
#endif
            if( pattern.length( ) >= c_meta_pat_length - 1 )
            {
               string new_pattern;

               if( ( pattern[ c_meta_pat_length - 2 ] & c_nibble_one ) == c_nibble_one )
               {
                  size_t pat_repeats = ( pattern[ c_meta_pat_length - 2 ] & c_nibble_two ) << 8;
                  pat_repeats += ( unsigned char )pattern[ c_meta_pat_length - 1 ] + 1;

#ifdef DEBUG_DECODE
cout << "pat repeats = " << pat_repeats << endl;
#endif
                  pattern.erase( c_meta_pat_length - 2 );

                  for( size_t i = 0; i < pat_repeats + 1; i++ )
                     new_pattern += expand_meta_pattern( pattern, p_encoded, indent + 1 );
               }
               else
               {
                  new_pattern = expand_meta_pattern( pattern.substr( 0, c_meta_pat_length - 2 ), p_encoded, indent + 1 );
                  new_pattern += expand_meta_pattern( pattern.substr( c_meta_pat_length - 2 ), p_encoded, indent + 1 );
#ifdef DEBUG_DECODE
cout << "new pattern = " << new_pattern << endl;
#endif
               }

               pattern = new_pattern;
            }
         }
      }
   }

   return pattern;
}

}

void init_clz_info( )
{
   // NOTE: For multi-threaded applications this function should be called from the main thread
   // before starting up any child threads (to avoid any potential race condition).
   if( g_dict_lower.empty( ) )
   {
      unsigned char offset = 0;

      for( size_t i = 0; i <  ARRAY_SIZE( g_dict_patterns ); i++ )
      {
         g_dict_lower.insert( make_pair( string( g_dict_patterns[ i ].p_w1 ), offset++ ) );
         g_dict_lower.insert( make_pair( string( g_dict_patterns[ i ].p_w2 ), offset++ ) );
         g_dict_lower.insert( make_pair( string( g_dict_patterns[ i ].p_w3 ), offset++ ) );
         g_dict_lower.insert( make_pair( string( g_dict_patterns[ i ].p_w4 ), offset++ ) );
         g_dict_lower.insert( make_pair( string( g_dict_patterns[ i ].p_w5 ), offset++ ) );
         g_dict_lower.insert( make_pair( string( g_dict_patterns[ i ].p_w6 ), offset++ ) );
         g_dict_lower.insert( make_pair( string( g_dict_patterns[ i ].p_w7 ), offset++ ) );
         g_dict_lower.insert( make_pair( string( g_dict_patterns[ i ].p_w8 ), offset++ ) );
      }

      for( map< string, unsigned char >::iterator i = g_dict_lower.begin( ); i != g_dict_lower.end( ); ++i )
      {
         string mixed( i->first );
         mixed[ 0 ] -= 0x20;

         g_dict_mixed.insert( make_pair( mixed, i->second ) );
         g_dict_upper.insert( make_pair( upper( i->first ), i->second ) );
      }
   }
}

void decode_clz_data( istream& is, ostream& os )
{
   deque< string > outputs;
   set< size_t > meta_offsets;
#ifdef VISUALISE_BACKREFS
   deque< string > pretty_outputs;
#endif

   unsigned char input_buffer[ c_max_encoded_chunk_size ];

   while( true )
   {
#ifdef VISUALISE_BACKREFS
      if( !is.tellg( ) )
         cout << c_color_red << "[" << c_color_default;
      else
         cout << c_color_red << "][" << c_color_default;
#endif
#ifndef NO_SHRINK_AND_EXPAND
      size_t bytes_read = expand_input( is, input_buffer, c_max_encoded_chunk_size );
#else
      size_t bytes_read = is.readsome( ( char* )input_buffer, c_max_encoded_chunk_size );

      // NOTE: The "readsome" function may return when in fact there are further bytes to be read so if less than
      // the maximum encoded chunk size bytes were read then call a second time to try and fill the input buffer.
      if( bytes_read && bytes_read < c_max_encoded_chunk_size )
         bytes_read += is.readsome( ( char* )( input_buffer + bytes_read ), c_max_encoded_chunk_size - bytes_read );
#endif
      if( bytes_read == 0 )
         break;

#ifdef DEBUG_DECODE
cout << "bytes read = " << bytes_read << endl;
#endif
      if( bytes_read <= c_min_pat_length )
         os.write( ( const char* )input_buffer, bytes_read );
      else
      {
         size_t offset = 0;

         while( true )
         {
            unsigned char byte = input_buffer[ offset ];

            if( byte & c_high_bit_value )
               meta_offsets.insert( offset++ );

            if( ++offset > bytes_read - 1 )
               break;
         }

#ifdef DEBUG_DECODE
cout << "meta_offsets.size( ) = " <<  meta_offsets.size( ) << endl;
#endif
         if( meta_offsets.empty( ) )
            os.write( ( const char* )input_buffer, bytes_read );
         else
         {
            set< size_t >::iterator si = --meta_offsets.end( );

            size_t num_repeats = 0;
            size_t next_offset = *si;
            size_t last_repeat_offset = 0;

            size_t last_offset = bytes_read;

            // NOTE: Any bytes after the last meta-pair are immediately pushed to the back of the output.
            if( next_offset < bytes_read - 2 )
            {
               outputs.push_back( string( ( const char* )( input_buffer + next_offset + 2 ), last_offset - next_offset - 2 ) );
#ifdef VISUALISE_BACKREFS
               pretty_outputs.push_back( string( ( const char* )( input_buffer + next_offset + 2 ), last_offset - next_offset - 2 ) );
#endif
            }

            while( true )
            {
#ifdef DEBUG_DECODE
cout << "next_offset = " << next_offset << endl;
#endif
               string pat( ( const char* )( input_buffer + next_offset ), 2 );

               unsigned char byte1 = pat[ 0 ];
               unsigned char byte2 = pat[ 1 ];

#ifdef DEBUG_DECODE
dump_bytes( "meta:", ( unsigned char* )pat.c_str( ), 2 );
#endif
               if( ( byte1 & c_nibble_one ) == c_nibble_one )
               {
                  num_repeats = ( byte1 & c_nibble_two ) << 8;
                  num_repeats += ( byte2 + 1 );
#ifdef DEBUG_DECODE
cout << "meta repeats = " << num_repeats << endl;
#endif
               }
               else
               {
                  string output;
                  pat = expand_meta_pattern( pat, input_buffer, 0, next_offset );

                  for( size_t i = 0; i < num_repeats + 1; i++ )
                     output += pat;

#ifdef DEBUG_DECODE
cout << "num_repeats = " << num_repeats << ", expanded pat: " << pat << "\n output ==> " << output << endl;
#endif
                  outputs.push_front( output );

#ifdef VISUALISE_BACKREFS
                  pretty_outputs.push_front(
                   string( num_repeats == 0 ? c_color_green : c_color_purple ) + "[" + string( c_color_cyan )
                   + output + string( num_repeats == 0 ? c_color_green : c_color_purple ) + "]" + string( c_color_default ) );
#endif
                  num_repeats = 0;
               }

               last_offset = next_offset;

               if( si == meta_offsets.begin( ) )
                  break;

               next_offset = *--si;

               if( next_offset < last_offset - 2 )
               {
                  outputs.push_front( string( ( const char* )( input_buffer + next_offset + 2 ), last_offset - next_offset - 2 ) );
#ifdef VISUALISE_BACKREFS
                  pretty_outputs.push_front( string( ( const char* )( input_buffer + next_offset + 2 ), last_offset - next_offset - 2 ) );
#endif
               }
            }

            if( last_offset != 0 )
            {
               outputs.push_front( string( ( const char* )input_buffer, last_offset ) );
#ifdef VISUALISE_BACKREFS
               pretty_outputs.push_front( string( ( const char* )input_buffer, last_offset ) );
#endif
            }

            string final_output;
#ifdef VISUALISE_BACKREFS
            string pretty_output;
#endif

            for( size_t i = 0; i < outputs.size( ); i++ )
            {
               string next( outputs[ i ] );

               if( !next.empty( ) )
               {
                  if( next[ 0 ] != c_back_ref )
                  {
                     final_output += next;
#ifdef VISUALISE_BACKREFS
                     pretty_output += pretty_outputs[ i ];
#endif
                  }
                  else
                  {
                     next.erase( 0, 1 );
                     string::size_type pos = next.find( ',' );

                     if( pos != string::npos )
                     {
                        size_t length = from_string< size_t >( next.substr( 0, pos ) );
                        size_t offset = from_string< size_t >( next.substr( pos + 1 ) );

                        string back_ref_bytes( final_output.substr( final_output.length( ) - offset, length ) );

                        final_output += back_ref_bytes;
#ifdef VISUALISE_BACKREFS
                        pretty_output += c_color_cyan + string( "[" )
                         + c_color_green + back_ref_bytes + c_color_cyan + string( "]" ) + c_color_default;
#endif
                     }
                  }
               }
            }

            os << final_output;
#ifdef VISUALISE_BACKREFS
            cout << pretty_output;
#endif

            outputs.clear( );
            meta_offsets.clear( );
#ifdef VISUALISE_BACKREFS
            pretty_outputs.clear( );
#endif
         }
      }

      if( bytes_read < c_max_encoded_chunk_size )
         break;
   }
#ifdef VISUALISE_BACKREFS
   cout << c_color_red << "]" << c_color_default << endl;
#endif
}

void encode_clz_data( istream& is, ostream& os )
{
   size_t num = 0;
   size_t total_chunks = 1;
   size_t output_offset = 0;
   size_t unencoded_offset = 0;
   size_t last_pair_repeats = 0;
   size_t last_pattern_offset = 0;
   size_t last_back_ref_offset = 0;
   size_t last_second_pass_offset = 0;

   size_t max_offset = c_max_offset;
   size_t max_repeats = c_max_repeats;
   size_t max_chunk_size = c_max_encoded_chunk_size;

   repeat_info last_repeat_info;

   meta_pattern_info meta_patterns;
   map< string, size_t > extra_patterns;

   unsigned char input_buffer[ c_max_pat_length + 2 ];
   unsigned char output_buffer[ c_max_encoded_chunk_size ];
   unsigned char unencoded_buffer[ c_max_unencoded_chunk_size ];

   memset( input_buffer, 0, sizeof( input_buffer ) );
   memset( output_buffer, 0, sizeof( output_buffer ) );
   memset( unencoded_buffer, 0, sizeof( unencoded_buffer ) );

#ifndef NO_SHRINK_AND_EXPAND
   size_t max_word_length = 0;

   set< string > found_words;
   map< string, size_t > dict_words;
   multimap< string, string > shorter_words;

   for( size_t i = 0; i < ARRAY_SIZE( g_dict_words ); i++ )
   {
      string word( g_dict_words[ i ].p_w );

      if( word.length( ) > max_word_length )
         max_word_length = word.length( );

      dict_words.insert( make_pair( word, i ) );

      string shorter( word.substr( 0, word.length( ) - 1 ) );

      while( shorter.length( ) > c_min_pat_length )
      {
         if( dict_words.count( shorter ) )
            shorter_words.insert( make_pair( word, shorter ) );

         shorter.erase( shorter.length( ) - 1 );
      }
   }
#endif

   while( true )
   {
      bool at_eof = false;

      while( num < c_max_pat_length )
      {
         if( is.read( ( char* )input_buffer + num, 1 ) )
            ++num;
         else
         {
            at_eof = true;
            break;
         }

         if( unencoded_offset < c_max_unencoded_chunk_size )
            unencoded_buffer[ unencoded_offset++ ] = input_buffer[ num - 1 ];

         // FUTURE: Assuming the use of a special indicator then 8-bit data
         // could be converted to 7-bit on the fly so as not to require the
         // input to be restricted to 7-bit as is currently being enforced.
         if( input_buffer[ num - 1 ] & c_high_bit_value )
            throw runtime_error( "can only clz encode a 7-bit data stream" );

         if( output_offset + num > max_chunk_size )
            break;
      }

      if( !num )
         break;

#ifdef DEBUG_ENCODE
cout << "(read) num = " << num << ' ';
dump_bytes( "input data =", input_buffer, num );
cout << "last_pattern_offset = " << last_pattern_offset << endl;
cout << "last_back_ref_offset = " << last_back_ref_offset << endl;
cout << "last_second_pass_offset = " << last_second_pass_offset << endl;
#endif
      if( ( output_offset + num <= max_chunk_size )
       && ( num < c_min_pat_length || output_offset < c_min_pat_length ) )
      {
         if( last_pair_repeats )
         {
            unsigned char rbyte1 = c_nibble_one | ( ( --last_pair_repeats & 0x0f00 ) >> 8 );
            unsigned char rbyte2 = ( last_pair_repeats & 0x00ff );

            if( !replace_meta_pattern( meta_patterns, output_buffer, last_back_ref_offset,
             rbyte1, rbyte2, output_offset, last_pattern_offset, last_second_pass_offset ) )
            {
               output_buffer[ output_offset++ ] = rbyte1;
               output_buffer[ output_offset++ ] = rbyte2;
            }
         }

         last_pair_repeats = 0;

         bool was_extra_pattern = false;

         if( num < c_min_pat_length
          && output_offset > c_min_pat_length
          && ( ( input_buffer[ 0 ] & c_high_bit_value ) != c_high_bit_value )
          && last_pattern_offset && ( last_pattern_offset == output_offset - 2 )
          && ( ( output_buffer[ last_pattern_offset ] & c_nibble_one ) != c_nibble_one ) )
         {
            string pattern( ( const char* )&output_buffer[ last_pattern_offset ], 2 );
            pattern += string( ( const char* )input_buffer, num );

            was_extra_pattern = replace_extra_pattern( meta_patterns, extra_patterns, pattern, output_buffer, output_offset );
         }

         if( was_extra_pattern )
            last_back_ref_offset = 0;
         else
         {
            memcpy( output_buffer + output_offset, input_buffer, min( num, c_min_pat_length ) );
            output_offset += min( num, c_min_pat_length );
         }

         bool is_empty = false;

         // NOTE: If less than the minimum pattern length remains then the input buffer is now
         // empty due to being at the end of the file or having just filled the output buffer.
         if( num < c_min_pat_length )
         {
            if( at_eof )
               break;

            num = 0;
            is_empty = true;
         }
         else
         {
            memmove( input_buffer, input_buffer + c_min_pat_length, num - c_min_pat_length );
            num -= c_min_pat_length;
         }

         if( !is_empty && num == 0 )
            continue;
      }

#ifdef DEBUG_ENCODE
cout << "num now = " << num << ", output_offset = " << output_offset << endl;
#endif
      size_t offset = 0;
      size_t length = ( num > 0 ? 1 : 0 );

      bool has_following_sequence = false;
      bool input_starts_with_back_ref = ( num == 0 ? false : ( input_buffer[ 0 ] & c_high_bit_value ) );

#ifndef NO_SHRINK_AND_EXPAND
      if( num > c_min_pat_length && !input_starts_with_back_ref )
      {
         bool found = false;

         string pat( ( const char* )input_buffer, min( num, max_word_length ) );

         // NOTE: If a dictionary word that hasn't already appeared is found then
         // rather than risk splitting it with a back-ref simply skip over it (as
         // it will be shrunk to two bytes afterwards).
         while( pat.length( ) > c_min_pat_length )
         {
            if( !found_words.count( pat ) && dict_words.count( pat ) )
            {
               found = true;
               break;
            }

            pat.erase( pat.length( ) - 1 );
         }

         size_t length = pat.length( );

         if( last_pair_repeats )
            length += 2;

         if( found && output_offset < max_offset - length )
         {
            if( last_pair_repeats )
            {
               unsigned char rbyte1 = c_nibble_one | ( ( --last_pair_repeats & 0x0f00 ) >> 8 );
               unsigned char rbyte2 = ( last_pair_repeats & 0x00ff );

               if( !replace_meta_pattern( meta_patterns,
                output_buffer, last_back_ref_offset, rbyte1, rbyte2,
                output_offset, last_pattern_offset, last_second_pass_offset ) )
               {
                  output_buffer[ output_offset++ ] = rbyte1;
                  output_buffer[ output_offset++ ] = rbyte2;
               }
            }

            last_pair_repeats = 0;

            found_words.insert( pat );

            multimap< string, string >::iterator swi = shorter_words.lower_bound( pat );

            // NOTE: Any words that are prefixes of the one found need to be treated as
            // though they were found as well to ensure that they will later be used as
            // a back-ref rather than left as is.
            while( swi != shorter_words.end( ) )
            {
               if( swi->first != pat )
                  break;

               found_words.insert( swi->second );

               ++swi;
            }

            memcpy( output_buffer + output_offset, input_buffer, pat.length( ) );
            output_offset += pat.length( );

            if( num > pat.length( ) )
               memmove( input_buffer, input_buffer + pat.length( ), num - pat.length( ) );

            num -= pat.length( );

            continue;
         }
      }
#endif

      if( num >= c_min_pat_length
       && output_offset >= c_min_pat_length && output_offset <= max_chunk_size - 2 )
      {
         length = longest_sequence( input_buffer, num, output_buffer, output_offset, offset );

         // NOTE: If did not find a pattern at the start of the input buffer but can
         // two characters ahead then will not try to use a second pass back-ref (as
         // this gives a better chance for the first pass back-refs to be extended).
         if( length < c_min_pat_length && ( num > c_min_pat_length + 2 ) )
            has_following_sequence = ( longest_sequence( input_buffer + 2,
             num - 2, output_buffer, output_offset, offset ) >= c_min_pat_length );
      }

#ifndef NO_SECOND_PASS_BACK_REFS
      if( num >= c_min_pat_length
       && !has_following_sequence
       && length < c_min_pat_length
       && !input_starts_with_back_ref
       && unencoded_offset > num + c_min_pat_length
       && unencoded_offset < c_max_unencoded_chunk_size
       && output_offset < max_offset - c_min_pat_length )
      {
         size_t inverted_length = 0;
         size_t inverted_offset = 0;

         size_t temp_offset = min( unencoded_offset - num, max_offset - output_offset );
         size_t starting_offset = ( unencoded_offset - temp_offset - num );

         if( temp_offset - starting_offset > c_min_pat_length )
            inverted_length = longest_sequence( input_buffer, num,
             ( unencoded_buffer + starting_offset ), temp_offset, inverted_offset, output_buffer, output_offset );

         if( inverted_length > c_min_pat_length )
         {
            inverted_offset = output_offset + ( unencoded_offset - num - inverted_offset ) - starting_offset;

            // NOTE: In case the repeat bytes are not combined with the last pattern will need to
            // increment the offset here (but then decrement it back later if actually combined).
            if( last_pair_repeats )
               inverted_offset += 2;

            if( inverted_offset <= max_offset )
            {
               if( last_pair_repeats )
               {
                  unsigned char rbyte1 = c_nibble_one | ( ( --last_pair_repeats & 0x0f00 ) >> 8 );
                  unsigned char rbyte2 = ( last_pair_repeats & 0x00ff );

                  size_t old_output_offset = output_offset;

                  if( !replace_meta_pattern( meta_patterns,
                   output_buffer, last_back_ref_offset, rbyte1, rbyte2,
                   output_offset, last_pattern_offset, last_second_pass_offset ) )
                  {
                     output_buffer[ output_offset++ ] = rbyte1;
                     output_buffer[ output_offset++ ] = rbyte2;
                  }
                  else
                     inverted_offset -= ( 2 + ( old_output_offset - output_offset ) );
               }

               unsigned byte1 = c_high_bit_value | ( ( inverted_length - c_min_pat_length ) << 3 );
               byte1 |= ( ( inverted_offset & c_offset_high_pair_mask ) >> 8 );

               unsigned byte2 = ( inverted_offset & c_offset_low_pair_mask );

               last_second_pass_offset = output_offset;

               output_buffer[ output_offset++ ] = byte1;
               output_buffer[ output_offset++ ] = byte2;

               if( num > inverted_length )
                  memmove( input_buffer, input_buffer + inverted_length, num - inverted_length );

               num -= inverted_length;

               last_pair_repeats = last_back_ref_offset = 0;

               continue;
            }
         }
      }
#endif

#ifdef DEBUG_ENCODE
cout << "length = " << length << ", offset = " << offset << endl;
cout << "output_offset = " << output_offset << endl;
#endif
      bool at_end_of_buffer = false;

      // NOTE: Never output the just first part of a back-ref pair.
      if( length == 1 && input_starts_with_back_ref )
         ++length;
      else if( length >= c_min_pat_length
       && length < c_max_pat_length && offset + length == output_offset
       && input_buffer[ 0 ] == input_buffer[ 1 ] && input_buffer[ 1 ] == input_buffer[ 2 ] )
      {
         unsigned char ch = input_buffer[ 0 ];

         // NOTE: Don't allow a run of identical bytes to be any shorter than the
         // maximum pattern length in order to allow the maximum usage of the run
         // in a later back-ref.
         if( output_offset + length >= c_max_encoded_chunk_size )
            length = c_max_encoded_chunk_size - output_offset;
         else
         {
            for( ; length < num; length++ )
            {
               if( input_buffer[ length ] != ch 
                || ( output_offset + length >= c_max_encoded_chunk_size - 1 ) )
                  break;
            }
         }

         memcpy( output_buffer + output_offset, input_buffer, length );
#ifdef DEBUG_ENCODE
cout << "(appended " << length << " identical bytes)" << endl;
#endif

         if( num > length )
            memmove( input_buffer, input_buffer + length, num - length );

         output_offset += length;
         num -= length;

         if( output_offset >= c_max_encoded_chunk_size )
            at_end_of_buffer = true;
         else
            continue;
      }

#ifdef DEBUG_ENCODE
cout << "length now = " << length << endl;
#endif
      if( length < c_min_pat_length )
      {
         if( !input_starts_with_back_ref )
         {
            if( last_pair_repeats )
            {
               unsigned char rbyte1 = c_nibble_one | ( ( --last_pair_repeats & 0x0f00 ) >> 8 );
               unsigned char rbyte2 = ( last_pair_repeats & 0x00ff );

               if( !replace_meta_pattern( meta_patterns,
                output_buffer, last_back_ref_offset, rbyte1, rbyte2,
                output_offset, last_pattern_offset, last_second_pass_offset ) )
               {
                  output_buffer[ output_offset++ ] = rbyte1;
                  output_buffer[ output_offset++ ] = rbyte2;
               }
            }

            last_pair_repeats = 0;
         }
         else
            last_back_ref_offset = output_offset;

         if( length )
         {
            bool was_extra_pattern = false;

            if( length < c_min_pat_length
             && output_offset > c_min_pat_length
             && ( ( input_buffer[ 0 ] & c_high_bit_value ) != c_high_bit_value )
             && last_pattern_offset && ( last_pattern_offset == output_offset - 2 )
             && ( ( output_buffer[ last_pattern_offset ] & c_nibble_one ) != c_nibble_one ) )
            {
               string pattern( ( const char* )&output_buffer[ last_pattern_offset ], 2 );
               pattern += string( ( const char* )input_buffer, length );

               was_extra_pattern = replace_extra_pattern( meta_patterns, extra_patterns, pattern, output_buffer, output_offset );
            }

            if( was_extra_pattern )
               last_back_ref_offset = 0;
            else
            {
               memcpy( output_buffer + output_offset, input_buffer, length );
               output_offset += length;
            }

            if( num > length )
               memmove( input_buffer, input_buffer + length, num - length );

            num -= length;

            perform_meta_combines( meta_patterns,
            output_buffer, output_offset, last_back_ref_offset, last_second_pass_offset );
         }
      }
      else if( !at_end_of_buffer )
      {
         unsigned char byte1 = 0;
         unsigned char byte2 = 0;

         byte1 = c_high_bit_value | ( ( length - c_min_pat_length ) << 3 );
         byte1 |= ( ( offset & c_offset_high_pair_mask ) >> 8 );

         byte2 = ( offset & c_offset_low_pair_mask );

         bool bytes_same_as_last_pair = false;

         if( output_offset > c_min_pat_length
          && ( last_back_ref_offset == offset - 2 )
          && ( byte1 == output_buffer[ output_offset - 2 ] )
          && ( byte2 == output_buffer[ output_offset - 1 ] ) )
            bytes_same_as_last_pair = true;

#ifdef DEBUG_ENCODE
cout << "found pattern: " << hex << setw( 2 ) << setfill( '0' ) << ( int )byte1 << setw( 2 ) << setfill( '0' ) << ( int )byte2 << dec << endl;
#endif
         if( last_pair_repeats && ( !bytes_same_as_last_pair || last_pair_repeats >= max_repeats ) )
         {
            unsigned char rbyte1 = c_nibble_one | ( ( --last_pair_repeats & 0x0f00 ) >> 8 );
            unsigned char rbyte2 = ( last_pair_repeats & 0x00ff );

            if( !replace_meta_pattern( meta_patterns,
             output_buffer, last_back_ref_offset, rbyte1, rbyte2,
             output_offset, last_pattern_offset, last_second_pass_offset ) )
            {
               output_buffer[ output_offset++ ] = rbyte1;
               output_buffer[ output_offset++ ] = rbyte2;
            }

            last_pair_repeats = 0;
            bytes_same_as_last_pair = false;
         }

         if( !bytes_same_as_last_pair && output_offset >= max_chunk_size - 2 )
         {
            output_buffer[ output_offset++ ] = byte1;
            output_buffer[ output_offset++ ] = byte2;

            if( num > length )
               memmove( input_buffer, input_buffer + length, num - length );

            num -= length;
         }
         else
         {
            if( bytes_same_as_last_pair )
               ++last_pair_repeats;

            if( !last_pair_repeats )
            {
               bool was_replaced = false;

               size_t old_output_offset = output_offset;

               // NOTE: One back-ref that immediately follows another is handled as a meta-pattern.
               if( last_back_ref_offset && last_back_ref_offset == output_offset - 2 )
               {
                  was_replaced = replace_meta_pattern( meta_patterns,
                   output_buffer, last_back_ref_offset, byte1, byte2,
                   output_offset, last_pattern_offset, last_second_pass_offset );
               }

               if( was_replaced || ( ( byte1 & c_nibble_one ) == c_nibble_one ) )
               {
                  if( !was_replaced )
                  {
                     // NOTE: If reached here then although not replaced by a meta-pattern the
                     // pair has now been changed to become a repeat of the previous back-ref.
                     output_buffer[ output_offset++ ] = byte1;
                     output_buffer[ output_offset++ ] = byte2;
                  }

                  last_pair_repeats = 0;
                  memmove( input_buffer, input_buffer + length, num - length );
               }
               else
               {
                  if( length > 2 && num > length )
                     memmove( input_buffer + 2, input_buffer + length, num - length );

                  // NOTE: Insert the back-reference at the start of the input buffer to support
                  // back-referencing from an existing back-reference (to efficiently handle any
                  // steadily increasing in length repeating patterns).
                  input_buffer[ 0 ] = byte1;
                  input_buffer[ 1 ] = byte2;

                  num += 2;
               }
            }
            else if( num > length )
               memmove( input_buffer, input_buffer + length, num - length );

            num -= length;
         }
#ifdef DEBUG_ENCODE
cout << "num now = " << num << ", output_offset = " << output_offset << ", ";
dump_bytes( "input data =", input_buffer, num );
#endif
      }

#ifdef DEBUG_ENCODE
dump_bytes( "buffered ==>", output_buffer, output_offset, last_back_ref_offset );
#endif
      if( output_offset >= max_chunk_size )
         perform_meta_combines( meta_patterns,
          output_buffer, output_offset, last_back_ref_offset, last_second_pass_offset );

      if( output_offset >= max_chunk_size )
      {
#ifdef DEBUG_ENCODE
cout << "(outputting " << output_offset << " bytes)" << endl;
#endif
#ifndef NO_SHRINK_AND_EXPAND
         shrink_output( output_buffer, output_offset );
#  ifdef DEBUG_ENCODE
cout << "(now writing " << output_offset << " bytes)" << endl;
#  endif
#endif
         os.write( ( char* )output_buffer, output_offset );

         ++total_chunks;

#ifndef NO_SHRINK_AND_EXPAND
         found_words.clear( );
#endif
         meta_patterns.clear( );
         extra_patterns.clear( );

         memset( output_buffer, 0, sizeof( output_buffer ) );
         memset( unencoded_buffer, 0, sizeof( unencoded_buffer ) );

         output_offset = unencoded_offset = 0;
         last_pair_repeats = last_pattern_offset = last_back_ref_offset = 0;
      }
   }

   if( last_pair_repeats )
   {
      unsigned char rbyte1 = c_nibble_one | ( ( --last_pair_repeats & 0x0f00 ) >> 8 );
      unsigned char rbyte2 = ( last_pair_repeats & 0x00ff );

      if( !replace_meta_pattern( meta_patterns,
       output_buffer, last_back_ref_offset, rbyte1, rbyte2,
       output_offset, last_pattern_offset, last_second_pass_offset ) )
      {
         output_buffer[ output_offset++ ] = rbyte1;
         output_buffer[ output_offset++ ] = rbyte2;
      }
   }

   if( output_offset )
   {
      perform_meta_combines( meta_patterns,
       output_buffer, output_offset, last_back_ref_offset, last_second_pass_offset );

#ifdef DEBUG_ENCODE
cout << "final offset = " << output_offset << endl;
dump_bytes( "buffered ==>", output_buffer, output_offset );
cout << "(outputting " << output_offset << " bytes)" << endl;
#endif
#ifndef NO_SHRINK_AND_EXPAND
      shrink_output( output_buffer, output_offset );
#  ifdef DEBUG_ENCODE
cout << "(now writing " << output_offset << " bytes)" << endl;
#  endif
#endif
      os.write( ( const char* )output_buffer, output_offset );
   }
}

#ifdef COMPILE_TESTBED_MAIN
int main( int argc, char* argv[ ] )
{
   string arg_1;

   if( argc > 1 )
      arg_1 = string( argv[ 1 ] );

   if( arg_1.empty( ) || arg_1 == "encode" )
   {
      ifstream is( "clz.in", ios::in | ios::binary );
      ofstream os( "clz.out", ios::out | ios::binary );

      encode_clz_data( is, os );
   }

   if( arg_1.empty( ) || arg_1 == "decode" )
   {
      ifstream is( "clz.out", ios::in | ios::binary );
      decode_clz_data( is, cout );
   }
}
#endif
