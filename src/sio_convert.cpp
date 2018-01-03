// Copyright (c) 2013-2018 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <iostream>
#  include <stdexcept>
#endif

#include "sio_convert.h"

#include "sio.h"
#include "utilities.h"

using namespace std;

namespace
{

string compress( const string& json )
{
   string out;

   bool is_escape = false;
   bool is_in_quotes = false;

   for( size_t i = 0; i < json.size( ); i++ )
   {
      char ch = json[ i ];

      bool skip = false;

      if( is_escape )
         is_escape = false;
      else
      {
         char ch = json[ i ];

         if( ch == '\\' )
            is_escape = true;
         else if( ch == '"' )
            is_in_quotes = !is_in_quotes;

         if( !is_in_quotes
          && ( ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' || ch == '\v' || ch == '\f' ) )
            skip = true;
      }

      if( !skip )
         out += ch;
   }

   return out;
}

string extract_value( string& json )
{
   string value;

   if( json.empty( ) || json[ 0 ] != '"' )
      throw runtime_error( "unexpected json value '" + json + "'" );

   bool in_escape = false;
   for( size_t i = 1; i < json.size( ); i++ )
   {
      char ch = json[ i ];

      if( in_escape )
      {
         value += ch;
         in_escape = false;
      }
      else if( ch == '\\' )
         in_escape = true;
      else
      {
         if( ch == '"' )
         {
            json.erase( 0, i + 1 );
            return value;
         }
         else
            value += ch;
      }
   }

   throw runtime_error( "unexpected json value '" + json + "'" );
}

void process_sio( const section_node& node, ostream& outs, json_format format )
{
   string ws, sep, indent;

   switch( format )
   {
      case e_json_format_multi:
      ws = '\n';
      sep = ' ';
      indent = string( node.get_child_depth( ), ' ' );
      break;

      case e_json_format_single:
      ws = ' ';
      sep = ' ';
      break;
   }

   if( node.is_root_node( ) )
      outs << "[" << ws;
   else
      outs << indent << "{" << sep << "\"" << node.get_name( ) << "\":" << ws << indent << "[" << ws;

   size_t num_attributes = node.get_num_attributes( );
   for( size_t i = 0; i < num_attributes; i++ )
   {
      if( i > 0 )
         outs << "," << ws;

      if( format != e_json_format_single )
         outs << indent << sep;

      outs << "{" << sep << "\"" << node.get_attribute( i ).get_name( )
       << "\":" << sep << quote( node.get_attribute( i ).get_value( ) ) << sep << "}";
   }

   size_t num_children = node.get_num_child_nodes( );

   if( num_attributes && num_children )
      outs << "," << ws;

   for( size_t i = 0; i < num_children; i++ )
   {
      if( i > 0 )
         outs << "," << ws;
      process_sio( node.get_child_node( i ), outs, format );
   }

   if( num_attributes || num_children )
      outs << ws;

   if( node.is_root_node( ) )
      outs << "]";
   else
      outs << indent << "]";

   if( !node.is_root_node( ) )
      outs << ws << indent << "}";
}

bool process_json( string& json, sio_writer& sio, size_t depth = 0 )
{
   string name, value;

   while( !json.empty( ) )
   {
      if( json.length( ) < 5 )
         return false;

      if( json[ 0 ] != '{' || json[ 1 ] != '"' )
         return false;

      json.erase( 0, 2 );

      string::size_type pos = json.find( '"' );
      if( pos == string::npos )
         return false;

      name = json.substr( 0, pos );
      json.erase( 0, pos + 1 );

      if( json.empty( ) || json[ 0 ] != ':' )
         return false;

      json.erase( 0, 1 );

      if( json.empty( ) || ( json[ 0 ] != '"' && json[ 0 ] != '[' ) )
         return false;

      if( json[ 0 ] == '[' )
      {
         sio.start_section( name );

         json.erase( 0, 1 );

         if( !json.empty( ) && json[ 0 ] != ']' )
         {
            if( !process_json( json, sio, depth + 1 ) )
               return false;
         }

         if( !json.empty( ) && json[ 0 ] == ']' )
            json.erase( 0, 1 );

         bool is_complete = false;
         bool is_continuing = false;

         if( json.length( ) >= 2 )
         {
            if( json.substr( 0, 2 ) == "}]" )
               is_complete = true;
            else if( json.substr( 0, 2 ) == "}," )
               is_continuing = true;
         }

         if( json.length( ) >= 2 && ( is_complete || is_continuing ) )
         {
            json.erase( 0, 2 );

            if( !sio.is_root_section( ) )
               sio.finish_section( sio.get_current_section( ) );

            if( is_complete )
               return true;
         }
      }
      else
      {
         value = extract_value( json );

         if( json.empty( ) || json[ 0 ] != '}' )
            return false;

         sio.write_attribute( name, value );

         json.erase( 0, 1 );

         if( json.empty( ) || ( json[ 0 ] != ',' && json[ 0 ] != ']' ) )
            return false;

         if( json[ 0 ] == ']' )
            return true;
         else
            json.erase( 0, 1 );
      }
   }

   return true;
}

}

void convert_sio_to_json( const sio_graph& sio, ostream& outs, json_format format )
{
   process_sio( sio.get_root_node( ), outs, format );
}

void convert_json_to_sio( const string& json, ostream& outs )
{
   string compressed( compress( json ) );

   sio_writer sio( outs );

   if( !compressed.empty( ) )
   {
      if( compressed.size( ) < 2 )
         throw runtime_error( "invalid json '" + json + "'" );

      if( compressed[ 0 ] != '[' || compressed[ compressed.size( ) - 1 ] != ']' )
         throw runtime_error( "invalid json '" + json + "'" );

      compressed.erase( 0, 1 );

      if( !process_json( compressed, sio ) )
         throw runtime_error( "invalid json '" + json + "'" );
   }

   sio.finish_sections( );
}

