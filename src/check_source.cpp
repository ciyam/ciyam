// Copyright (c) 2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <memory.h>
#  include <vector>
#  include <string>
#  include <fstream>
#  include <iostream>
#  include <iterator>
#  include <algorithm>
#  ifndef __GNUG__
#     include <io.h>
#  else
#     include <unistd.h>
#  endif
#endif

#ifdef __GNUG__
#  define _access access
#endif

//#define DEBUG
//#define OUTPUT_WITHOUT_COMMENTS

#define TITLE "check_source"
#define VER_NO "v0.1a"

#define USAGE "Usage: check_source [-q] [-r] [-n] [-p<path>] [-i<masks>] [-m<marker>] <file(s)>\n\n"\
"where: -q = quiet (only show errors)\n"\
"       -r = return after first error\n"\
"       -n = allow nested c-style comments\n"\
"       -p<path> = path to prepend for mask files\n"\
"       -i<masks> = comma separated list of include masks\n"\
"       -m<marker> = marker that will always produce an error if found\n"

using namespace std;

#define DEBUG_ERROR( ) debug_error( __FILE__, __LINE__ )

void debug_error( const char* file, int line )
{
   cerr << "==> in " << file << " at line " << line << endl;
}

const char c_path_separator = '/';

const char* const c_check_ignore_file_name = "check_source_ignore";
const char* const c_check_include_file_name = "check_source_include";

string g_path_to_prepend;
string g_path_for_last_file;
string g_error_marker_symbol;

vector< string > g_cmd_include_masks;
vector< string > g_check_ignore_masks;
vector< string > g_check_include_masks;

const short c_ignore_block = 0x0001;

bool g_is_quiet = false;
bool g_allow_nested_c_style_comments = false;
bool g_return_after_first_error_found = false;

static struct
{
   const char* label;
   short value;
}
g_format_masks[ ] =
{
   { "Ignore_Block", c_ignore_block },
   { "", -1 },
};

short g_format_value = 0;

const short c_max_nested_blocks = 99;

int output_error( const string& file_name,
 const string& line, size_t line_num, const string& msg )
{
   cerr << "check_source: error: " << file_name << " " << line_num << ": " << msg << '\n';
   cerr << line << endl;
   return 1;
}

int output_error( const string& file_name,
 const string& line, size_t line_num, size_t column_num, const string& msg )
{
   cerr << "check_source: error: " << file_name << " "
    << line_num << ":" << column_num << " " << msg << '\n';
   cerr << line << endl;
   return 1;
}

int output_error( const string& file_name, const string& line,
 size_t line_num, size_t column_num, char op, const string& msg )
{
   cerr << "check_source: error: " << file_name << " "
    << line_num << ":" << column_num << " '" << op << "' " << msg << '\n';
   cerr << line << endl;
   return 1;
}

int output_error( const string& file_name, const string& line,
 size_t line1_num, size_t column1_num, char op1, size_t line2_num,
 size_t column2_num, char op2 )
{
   cerr << "check_source: error: " << file_name << " "
    << line1_num << ":" << column1_num << " '" << op1
    << "' for matching '" << op2 << "' at line " << line2_num << ":" << column2_num << '\n';
   cerr << line << endl;
   return 1;
}

int check_for_or_operator( const string& file_name,
 size_t line_num, const string& line, char ch, size_t pos )
{
   if( pos > 0 && line[ pos - 1 ] != ' ' && line[ pos - 1 ] != ch )
   {
#ifdef DEBUG
      DEBUG_ERROR( );
#endif
      return output_error( file_name, line, line_num, ( pos + 1 ), ch, "does not have a preceeding space" );
   }

   size_t len = line.length( );
   if( pos < len - 1
    && line[ pos + 1 ] != ' '
    && line[ pos + 1 ] != '='
    && line[ pos + 1 ] != ch )
   {
#ifdef DEBUG
      DEBUG_ERROR( );
#endif
      return output_error( file_name, line, line_num, ( pos + 2 ), ch, "does not have a following space" );
   }

   return 0;
}

int check_for_two_char_operator( const string& file_name,
 size_t line_num, const string& line, char ch1, char ch2, size_t pos )
{
   if( pos > 1 && line[ pos - 2 ] != ' ' )
   {
#ifdef DEBUG
      DEBUG_ERROR( );
#endif
      return output_error( file_name, line, line_num, ( pos + 1 ), ch1, "does not have a preceeding space" );
   }

   size_t len = line.length( );
   if( pos + 1 < len && line[ pos + 1 ] != ' ' )
   {
#ifdef DEBUG
      DEBUG_ERROR( );
#endif
      return output_error( file_name, line, line_num, ( pos + 2 ), ch2, "does not have a following space" );
   }
   return 0;
}

int check_for_lesser_than_operator( const string& file_name,
 size_t line_num, const string& line, char ch, size_t pos )
{
   size_t len = line.length( );
   if( pos + 1 < len && line[ pos + 1 ] != ' '
    && line[ pos + 1 ] != ch && line[ pos + 1 ] != '='
    && line[ pos + 1 ] != ';' && line[ pos + 1 ] != '(' )
   {
#ifdef DEBUG
      DEBUG_ERROR( );
#endif
      return output_error( file_name, line, line_num, ( pos + 2 ), ch, "does not have a following space" );
   }
   return 0;
}

int check_for_greater_than_operator( const string& file_name,
 size_t line_num, const string& line, char ch, size_t pos )
{
   if( pos > 0 && line[ pos - 1 ] != ' '
    && line[ pos - 1 ] != ch && line[ pos - 1 ] != '-' )
   {
#ifdef DEBUG
      DEBUG_ERROR( );
#endif
      return output_error( file_name, line, line_num, ( pos + 1 ), ch, "does not have a preceeding space" );
   }
   return 0;
}

int check_for_cast_to_numeric_literal( const string& file_name,
 size_t line_num, const string& line, char ch, size_t pos )
{
   size_t len = line.length( );
   for( size_t i = pos + 1; i < len; i++ )
   {
      if( line[ i ] == ' ' )
         break;
      if( !isdigit( line[ i ] ) )
      {
#ifdef DEBUG
         DEBUG_ERROR( );
#endif
         return output_error( file_name,
          line, line_num, ( pos + 2 ), ch, "does not have a following space" );
      }
   }
   return 0;
}

bool is_pos_or_neg_sign( const string& line, size_t pos )
{
   size_t i;
   for( i = 0; i < pos; i++ )
      if( line[ i ] != ' ' )
         break;

   if( i == pos )
      return true;

   if( pos > 1 && line[ pos - 1 ] == ' ' &&
    ( line[ pos - 2 ] == '=' || line[ pos - 2 ] == '+'
    || line[ pos - 2 ] == '-' || line[ pos - 2 ] == '/'
    || line[ pos - 2 ] == '*' || line[ pos - 2 ] == '%'
    || line[ pos - 2 ] == ',' || line[ pos - 2 ] == '['
    || line[ pos - 2 ] == ')' || line[ pos - 2 ] == '('
    || line[ pos - 2 ] == '{' ) )
      return true;

   if( pos > 1 && line[ pos - 1 ] == ' '
    && pos + 1 < line.length( ) && isdigit( line[ pos + 1 ] ) )
      return true;

   return pos < 1;
}

int check_for_operator_format( const string& file_name,
 size_t line_num, const string& line, char ch, size_t pos )
{
   int rc = 0;

   size_t len = line.length( );
   if( pos > 0
    && line[ pos - 1 ] != ' ' )
   {
      if( ( ch != '+' && ch != '-' )
       || ( line[ pos - 1 ] != ch
       && ( pos + 1 < len && line[ pos + 1 ] != ch ) ) )
      {
         if( ch != '-' || line[ pos + 1 ] != '>' )
         {
#ifdef DEBUG
            DEBUG_ERROR( );
#endif
            output_error( file_name,
             line, line_num, ( pos + 1 ), ch, "does not have a preceeding space" );

            rc++;
            if( g_return_after_first_error_found )
               return rc;
         }
      }
   }

   if( pos > 0
    && pos + 1 < len && line[ pos + 1 ] != ' '
    && ( ( ch != '+' && ch != '-' ) || line[ pos + 1 ] != ch )
    && line[ pos + 1 ] != '=' && line[ pos + 1 ] != '('
    && ( line[ pos ] != '-' || line[ pos + 1 ] != '>' ) )
   {
      if( ( ( ch != '+' && ch != '-' ) || ch != line[ pos - 1 ] )
       && ( ( ch != '+' && ch != '-' ) || !is_pos_or_neg_sign( line, pos ) ) )
      {
         if( ch != '/' ||
          ( line[ pos + 1 ] != ch && line[ pos + 1 ] != '*' ) )
         {
#ifdef DEBUG
            DEBUG_ERROR( );
#endif
            output_error( file_name,
             line, line_num, ( pos + 2 ), ch, "does not have a following space" );

            rc++;
            if( g_return_after_first_error_found )
               return rc;
         }
      }
   }

   return rc;
}

int check_for_no_preceeding_spaces( const string& file_name,
 size_t line_num, const string& line, char ch, size_t pos )
{
   int rc = 0;

   if( pos > 0 && line[ pos - 1 ] == ' ' )
   {
#ifdef DEBUG
      DEBUG_ERROR( );
#endif
      output_error( file_name,
       line, line_num, ( pos + 1 ), ch, "has one or more preceeding spaces" );
      rc++;
   }
   return rc;
}

int check_for_preceeding_spaces_for_semicolon( const string& file_name,
 size_t line_num, const string& line, char ch, size_t pos )
{
   int rc = 0;

   if( pos > 0 && line[ pos - 1 ] == ' ' )
   {
      size_t tmp( pos );
      while( --tmp > 0 )
         if( line[ tmp ] != ' ' )
            break;
      if( ( tmp == 0 && line[ tmp ] != ' ' )
       || ( tmp > 0 && line[ tmp ] != ch && line[ tmp ] != '(' ) )
      {
#ifdef DEBUG
         DEBUG_ERROR( );
#endif
         output_error( file_name,
          line, line_num, ( pos + 1 ), ch, "has one or more preceeding spaces" );
         rc++;
      }
   }
   return rc;
}

int check_following_char_for_brace( const string& file_name,
 size_t line_num, const string& line, char ch, size_t pos )
{
   size_t len = line.length( );
   if( pos + 1 < len && line[ pos + 1 ] != ' '
    && line[ pos + 1 ] != ',' && line[ pos + 1 ] != ';' )
   {
#ifdef DEBUG
      DEBUG_ERROR( );
#endif
      return output_error( file_name,
       line, line_num, ( pos + 2 ), ch, "does not have a following space" );
   }

   if( pos + 2 < len && line[ pos + 1 ] == ' ' && line[ pos + 2 ] == ' ' )
   {
#ifdef DEBUG
      DEBUG_ERROR( );
#endif
      return output_error( file_name,
       line, line_num, ( pos + 2 ), ch, "has two or more following spaces" );
   }

   return 0;
}

int check_for_all_or_one_preceeding_spaces( const string& file_name,
 size_t line_num, const string& line, char ch, size_t pos )
{
   size_t num_preceeding = 0;
   for( size_t i = 0; i < pos; i++ )
   {
      if( line[ i ] == ' ' )
         num_preceeding++;
      else
         num_preceeding = 0;
   }

   if( num_preceeding > 1 && num_preceeding < pos - 1 )
   {
#ifdef DEBUG
      DEBUG_ERROR( );
#endif
      return output_error( file_name,
       line, line_num, ( pos + 1 ), ch, "has two or more preceeding spaces" );
   }

   return 0;
}

int check_for_zero_or_one_following_spaces( const string& file_name,
 size_t line_num, const string& line, char ch, size_t pos )
{
   size_t len = line.length( );
   if( pos + 2 < len && line[ pos + 1 ] == ' ' && line[ pos + 2 ] == ' ' )
   {
#ifdef DEBUG
      DEBUG_ERROR( );
#endif
      return output_error( file_name,
       line, line_num, ( pos + 1 ), ch, "has two or more following spaces" );
   }

   return 0;
}

int check_for_one_following_space( const string& file_name,
 size_t line_num, const string& line, char ch, size_t pos )
{
   size_t len = line.length( );
   if( pos + 1 < len && line[ pos + 1 ] != ' ' )
   {
#ifdef DEBUG
      DEBUG_ERROR( );
#endif
      return output_error( file_name,
       line, line_num, ( pos + 2 ), ch, "does not have a following space" );
   }

   if( pos + 2 < len && line[ pos + 2 ] == ' ' )
   {
#ifdef DEBUG
      DEBUG_ERROR( );
#endif
      return output_error( file_name,
       line, line_num, ( pos + 2 ), ch, "has two or more following spaces" );
   }

   return 0;
}

int check_for_one_preceeding_space( const string& file_name,
 size_t line_num, const string& line, char ch, size_t pos )
{
   if( pos > 0 && line[ pos - 1 ] != ' '
    && ( ch != '&' || line[ pos - 1 ] != ')' ) )
   {
#ifdef DEBUG
      DEBUG_ERROR( );
#endif
      return output_error( file_name,
       line, line_num, ( pos + 1 ), ch, "does not have a preceeding space" );
   }

   size_t i;
   for( i = 0; i < pos; i++ )
      if( line[ i ] != ' ' )
         break;

   if( i == pos )
      return 0;

   if( pos > 1
    && ( ( ch != '&' && line[ pos - 2 ] == ' ' )
    || ( line[ pos - 1 ] == ' ' && line[ pos - 2 ] == ' ' ) ) )
   {
#ifdef DEBUG
      DEBUG_ERROR( );
#endif
      return output_error( file_name,
       line, line_num, ( pos + 1 ), ch, "has two or more preceeding spaces" );
   }

   return 0;
}

void get_ignore_masks( )
{
   string fname( g_path_to_prepend );
   if( !fname.empty( ) && fname[ fname.length( ) - 1 ] != c_path_separator )
      fname += c_path_separator;

   fname += g_path_for_last_file;
   fname += c_check_ignore_file_name;

#ifdef DEBUG
   cout << "check_source: debug: check for ignore file '" << fname << "'" << endl;
#endif
   g_check_ignore_masks.clear( );

   if( _access( fname.c_str( ), 0 ) == 0 )
   {
      ifstream inpf( fname.c_str( ) );
      if( !inpf )
      {
         cerr << "check_source: unable to open file '" << fname << "' for input" << endl;
         exit( 1 );
      }

      string next;
      while( inpf )
      {
         getline( inpf, next );
         if( !next.empty( ) )
         {
            while( true )
            {
               size_t pos = next.find( ' ' );
               g_check_ignore_masks.push_back( next.substr( 0, pos ) );
               if( pos != string::npos )
                  next.erase( 0, pos + 1 );
               else
                  break;
            }
         }
      }
   }
#ifdef DEBUG
   cout << "check_source: debug: g_check_ignore_masks.size( ) = " << g_check_ignore_masks.size( ) << endl;
   for( size_t i = 0; i < g_check_ignore_masks.size( ); i++ )
      cout << "check_source: debug: g_check_ignore_masks[ "
       << i << " ] = " << g_check_ignore_masks[ i ] << endl;
#endif
}

void get_include_masks( )
{
   string fname( g_path_to_prepend );
   if( !fname.empty( ) && fname[ fname.length( ) - 1 ] != c_path_separator )
      fname += c_path_separator;

   fname += g_path_for_last_file;
   fname += c_check_include_file_name;

#ifdef DEBUG
   cout << "check_source: debug: check for include file '" << fname << "'" << endl;
#endif
   g_check_include_masks.clear( );

   if( !g_cmd_include_masks.empty( ) )
      copy( g_cmd_include_masks.begin( ),
       g_cmd_include_masks.end( ), back_inserter( g_check_include_masks ) );

   if( _access( fname.c_str( ), 0 ) == 0 )
   {
      ifstream inpf( fname.c_str( ) );
      if( !inpf )
      {
         cerr << "check_source: unable to open file '" << fname << "' for input" << endl;
         exit( 1 );
      }

      string next;
      while( inpf )
      {
         getline( inpf, next );
         if( !next.empty( ) )
         {
            while( true )
            {
               size_t pos = next.find( ' ' );
               g_check_include_masks.push_back( next.substr( 0, pos ) );
               if( pos != string::npos )
                  next.erase( 0, pos + 1 );
               else
                  break;
            }
         }
      }
   }
#ifdef DEBUG
   cout << "check_source: debug: g_check_include_masks.size( ) = " << g_check_include_masks.size( ) << endl;
   for( size_t i = 0; i < g_check_include_masks.size( ); i++ )
      cout << "check_source: debug: g_check_include_masks[ "
       << i << " ] = " << g_check_include_masks[ i ] << endl;
#endif
}

bool patmatch( const char* pat, const char* str )
{
   switch( pat[ 0 ] )
   {
      case '\0':
         return !str[ 0 ];

      case '*':
         return patmatch( pat + 1, str ) || str[ 0 ] && patmatch( pat, str + 1 );

      case '?':
         return str[ 0 ] && patmatch( pat + 1, str + 1 );

      default:
         return pat[ 0 ] == str[ 0 ] && patmatch( pat + 1, str + 1 );
   }
}

int process_file( char* file_name )
{
   int rc = 0;
   int block_level = 0;
   int bracket_level = 0;
   int parenthesis_level = 0;
   int nested_c_comment_level = 0;

   size_t block_lines[ c_max_nested_blocks ];
   memset( block_lines, '\0', sizeof( block_lines ) );

   size_t block_indent[ c_max_nested_blocks ];
   memset( block_indent, '\0', sizeof( block_indent ) );

   short block_formats[ c_max_nested_blocks ];
   memset( block_formats, '\0', sizeof( block_formats ) );

   string line;
   string comment;
   size_t line_num = 0;
   size_t comment_num = 0;
   size_t tab_line_num = 0;
   bool in_char_literal = false;
   bool in_string_literal = false;
   bool in_c_style_comment = false;
   bool was_c_style_comment = false;

   string fname( file_name );
   size_t pos = fname.find_last_of( c_path_separator );
   if( pos != string::npos && g_path_for_last_file != fname.substr( 0, pos ) )
   {
      g_path_for_last_file = fname.substr( 0, pos + 1 );
#ifdef DEBUG
      cout << "check_source: debug: g_path_for_last_file = " << g_path_for_last_file << endl;
#endif
      get_ignore_masks( );
      get_include_masks( );
   }

   if( pos != string::npos )
      fname.erase( 0, pos + 1 );

#ifdef DEBUG
   cout << "check_source: debug: fname = " << fname << endl;
#endif
   for( size_t i = 0; i < g_check_ignore_masks.size( ); i++ )
   {
      if( patmatch( g_check_ignore_masks[ i ].c_str( ), fname.c_str( ) ) )
      {
#ifdef DEBUG
         cout << "check_source: debug: skipping '" << file_name
          << "' due to ignore mask match with '" << g_check_ignore_masks[ i ] << "'" << endl;
#endif
         if( !g_is_quiet )
            cout << "check_source: skipping: " << file_name << endl;
         return 0;
      }
   }

   bool is_matching = false;
   for( size_t i = 0; i < g_check_include_masks.size( ); i++ )
   {
      if( patmatch( g_check_include_masks[ i ].c_str( ), fname.c_str( ) ) )
      {
         is_matching = true;
         break;
      }
   }

   if( !is_matching )
   {
#ifdef DEBUG
      cout << "check_source: debug: skipping '" << file_name << "' due to no matching include mask" << endl;
#endif
      if( !g_is_quiet )
         cout << "check_source: skipping: " << file_name << endl;
      return 0;
   }

   if( _access( file_name, 0 ) != 0 )
   {
#ifdef DEBUG
      cout << "check_source: debug: skipping '" << file_name << "' due to file not found" << endl;
#endif
      if( !g_is_quiet )
         cout << "check_source: skipping: " << file_name << endl;
      return 0;
   }

#ifdef DEBUG
   cout << "check_source: debug: processing '" << file_name << "'..." << endl;
#endif
   ifstream inpf( file_name );

   if( !inpf )
   {
      cerr << "check_source: error: unable to open file '" << file_name << "' for input" << endl;
      return 1;
   }

   if( !g_is_quiet )
      cout << "check_source: checking: " << file_name << endl;

   while( inpf )
   {
      if( inpf.eof( ) )
         break;

      string output;
      char last_ch = '\0';
      bool in_cpp_style_comment = false;
      bool was_back_slash_literal = false;
      bool in_pre_processor_directive = false;

      ++line_num;
      getline( inpf, line );
#ifdef DEBUG
      cout << "check_source: debug: " << line << endl;
#endif
      for( size_t i = 0; i < line.length( ); i++ )
      {
         if( rc && g_return_after_first_error_found )
            return rc;

         if( !g_error_marker_symbol.empty( ) && line.find( g_error_marker_symbol ) != string::npos )
         {
#ifdef DEBUG
            DEBUG_ERROR( );
#endif
            rc++;
            output_error( file_name, line, line_num, "found marker symbol '" + g_error_marker_symbol + "'" );
            break;
         }

         if( !in_cpp_style_comment )
         {
            if( in_c_style_comment )
            {
               if( line[ i ] == '/' && last_ch == '*' )
               {
                  if( g_allow_nested_c_style_comments && nested_c_comment_level )
                     nested_c_comment_level--;
                  else
                  {
                     in_c_style_comment = false;
                     was_c_style_comment = true;
                  }
               }
               else if( g_allow_nested_c_style_comments )
               {
                  if( line[ i ] == '*' && last_ch == '/' )
                     nested_c_comment_level++;
               }
            }
            else if( !in_string_literal && line[ i ] == '*' && last_ch == '/' )
            {
               comment = line;
               comment_num = line_num;
               in_c_style_comment = true;
            }

            if( !in_char_literal && !in_string_literal
             && line[ i ] == '/' && last_ch == '/' )
            {
               in_cpp_style_comment = true;
               if( line.length( ) > i + 3
                && line[ i + 1 ] == '_' && line[ i + 2 ] == '_' )
               {
                  size_t pos = line.find( ' ', i + 2 );
                  string tok = line.substr( i + 3, pos );
                  if( tok == "{" )
                  {
                     block_lines[ block_level ] = line_num;
                     block_indent[ block_level ] = i;
                     if( !block_level )
                        block_formats[ block_level ] = 0;
                     else
                        block_formats[ block_level ] = block_formats[ block_level - 1 ];

                     block_formats[ block_level ] |= g_format_value;
                     g_format_value = 0;
                     block_level++;
                  }
                  else if( tok == "}" )
                  {
                     block_level--;
                     if( block_level < 0 )
                     {
#ifdef DEBUG
                        DEBUG_ERROR( );
#endif
                        rc++;
                        output_error( file_name,
                         line, line_num, "unexpected '}' with no matching '{'" );
                     }
                  }
                  else
                  {
                     for( int j = 0; g_format_masks[ j ].value != -1; j++ )
                     {
                        if( tok == g_format_masks[ j ].label )
                        {
                           g_format_value = g_format_masks[ j ].value;
                           break;
                        }
                     }
                  }
               }
            }

            if( line[ i ] == '\\' && last_ch == '\\' )
               was_back_slash_literal = !was_back_slash_literal;

            if( !in_c_style_comment && i == 0 && line[ i ] == '#' )
               in_pre_processor_directive = true;

            if( !in_c_style_comment && !in_cpp_style_comment )
            {
               if( !in_string_literal
                && line[ i ] == '\'' && ( last_ch != '\\' || was_back_slash_literal ) )
                  in_char_literal = !in_char_literal;

               if( !in_char_literal
                && line[ i ] == '\"' && ( last_ch != '\\' || was_back_slash_literal ) )
                  in_string_literal = !in_string_literal;

               if( !in_char_literal && !in_string_literal && !in_pre_processor_directive )
               {
                  if( i == 0 && line.length( ) > 3 && line.substr( 0, 4 ) == "====" )
                  {
#ifdef DEBUG
                     DEBUG_ERROR( );
#endif
                     rc++;
                     output_error( file_name,
                      line, line_num, "merge conflict marker found" );
                  }
                  else if( line[ i ] == '\t' && line_num != tab_line_num )
                  {
#ifdef DEBUG
                     DEBUG_ERROR( );
#endif
                     rc++;
                     tab_line_num = line_num;
                     output_error( file_name,
                      line, line_num, ( i + 1 ), "tab character(s) found in line" );
                  }
                  else if( line[ i ] == '{' )
                  {
                     block_lines[ block_level ] = line_num;
                     block_indent[ block_level ] = i;
                     if( !block_level )
                        block_formats[ block_level ] = 0;
                     else
                        block_formats[ block_level ] = block_formats[ block_level - 1 ];

                     block_formats[ block_level ] |= g_format_value;
                     g_format_value = 0;
                     block_level++;
                  }
                  else if( line[ i ] == '}' )
                  {
                     block_level--;
                     if( block_level < 0 )
                     {
#ifdef DEBUG
                        DEBUG_ERROR( );
#endif
                        rc++;
                        output_error( file_name,
                         line, line_num, "unexpected '}' with no matching '{'" );
                     }

                     if( !( block_formats[ block_level ] & c_ignore_block ) )
                        rc += check_following_char_for_brace( file_name, line_num, line, '}', i );

                     if( !( block_formats[ block_level ] & c_ignore_block ) )
                     {
                        if( i != block_indent[ block_level ] )
                        {
                           if( line_num == block_lines[ block_level ] )
                           {
                              if( block_indent[ block_level ]
                               && line[ block_indent[ block_level ] - 1 ] != ' ' )
                              {
#ifdef DEBUG
                                 DEBUG_ERROR( );
#endif
                                 rc++;
                                 output_error( file_name, line, line_num,
                                  ( block_indent[ block_level ] + 1 ),
                                  '}', "does not have a preceeding space" );
                              }

                              rc += check_for_one_following_space( file_name,
                               line_num, line, '{', block_indent[ block_level ] );

                              rc += check_for_one_preceeding_space( file_name, line_num, line, '}', i );
                           }
                           else
                           {
#ifdef DEBUG
                              DEBUG_ERROR( );
#endif
                              rc++;
                              output_error( file_name, line, line_num, ( i + 1 ), '}',
                               block_lines[ block_level ], ( block_indent[ block_level ] + 1 ), '{' );
                           }
                        }
                     }
                  }
                  else
                  {
                     if( line[ i ] == '(' )
                     {
                        rc += check_for_one_following_space( file_name, line_num, line, line[ i ], i );
                        parenthesis_level++;
                     }
                     else if( line[ i ] == ')' )
                     {
                        rc += check_for_one_preceeding_space( file_name, line_num, line, line[ i ], i );
                        parenthesis_level--;
                     }
                     else if( line[ i ] == '[' )
                     {
                        rc += check_for_one_following_space( file_name, line_num, line, line[ i ], i );
                        bracket_level++;
                     }
                     else if( line[ i ] == ']' )
                     {
                        rc += check_for_one_preceeding_space( file_name, line_num, line, line[ i ], i );
                        bracket_level--;
                     }
                     else if( line[ i ] == '|' )
                        rc += check_for_or_operator( file_name, line_num, line, line[ i ], i );
                     else if( line[ i ] == '&' && last_ch == '&' )
                        rc += check_for_one_following_space( file_name, line_num, line, line[ i ], i );
                     else if( line[ i ] == '&'
                      && i + 1 < line.length( )
                      && ( line[ i + 1 ] == '&' || isalpha( line[ i + 1 ] ) ) )
                        rc += check_for_one_preceeding_space( file_name, line_num, line, line[ i ], i );
                     else if( ( line[ i ] == ',' || line[ i ] == ';'
                      || ( line[ i ] == '&' && isalpha( last_ch ) ) )
                      && ( block_level == 0
                      || !( block_formats[ block_level - 1 ] & c_ignore_block ) ) )
                     {
                        if( line[ i ] != '&' || ( i + 1 < line.length( )
                         && line[ i + 1 ] != '(' && line[ i + 1 ] != ',' ) )
                        {
                           rc += check_for_one_following_space( file_name, line_num, line, line[ i ], i );
                        }

                        if( !( rc && g_return_after_first_error_found ) && line[ i ] == ',' )
                           rc += check_for_no_preceeding_spaces( file_name, line_num, line, line[ i ], i );

                        if( !( rc && g_return_after_first_error_found ) && line[ i ] == ';' )
                           rc += check_for_preceeding_spaces_for_semicolon( file_name, line_num, line, line[ i ], i );
                     }
                     else if( line[ i ] == '=' && last_ch == '&' )
                        rc += check_for_two_char_operator( file_name, line_num, line, last_ch, line[ i ], i );
                     else if( line[ i ] == '<' )
                     {
                        if( i + 1 < line.length( ) && line[ i + 1 ] == '<' )
                           rc += check_for_one_preceeding_space( file_name, line_num, line, line[ i ], i );
                        else
                           rc += check_for_lesser_than_operator( file_name, line_num, line, line[ i ], i );
                     }
                     else if( line[ i ] == '>' )
                     {
                        if( i + 1 < line.length( ) && line[ i + 1 ] == '>' )
                           rc += check_for_one_preceeding_space( file_name, line_num, line, line[ i ], i );
                        else
                           rc += check_for_greater_than_operator( file_name, line_num, line, line[ i ], i );
                     }
                     else if( line[ i ] == '-' && last_ch == ')'
                      && i + 1 < line.length( ) && line[ i + 1 ] != '>' )
                        rc += check_for_cast_to_numeric_literal( file_name, line_num, line, line[ i ], i );
                     else if( line[ i ] == '%' && ( last_ch == '%'
                      || ( i + 1 < line.length( ) && line[ i + 1 ] == '%' ) ) )
                        ;
                     else if( line[ i ] == '=' && ( last_ch == '!'
                      || last_ch == '+' || last_ch == '-' || last_ch == '%'
                      || last_ch == '^' || last_ch == '*' || last_ch == '/'
                      || last_ch == '>' || last_ch == '<' || last_ch == '=' || last_ch == '|' ) )
                        ;
                     else if( line[ i ] == '+' || line[ i ] == '-'
                      || line[ i ] == '%' || line[ i ] == '^' || line[ i ] == '='
                      || ( line[ i ] == '/' && !was_c_style_comment ) )
                     {
                        rc += check_for_operator_format( file_name, line_num, line, line[ i ], i );
                     }

                     // IMPORTANT: These are more general checks that simply ensure that if certain tokens
                     // have one space before or after them then only a single space is allowed (except if
                     // first non-space).
                     if( !block_level ||
                      !( block_formats[ block_level - 1 ] & c_ignore_block ) )
                     {
                        if( line[ i ] == '='
                         || line[ i ] == '&' || line[ i ] == '|'
                         || line[ i ] == '*' || line[ i ] == '/'
                         || line[ i ] == '%' || line[ i ] == '^'
                         || line[ i ] == '+' || line[ i ] == '-' )
                        {
                           rc += check_for_zero_or_one_following_spaces( file_name, line_num, line, line[ i ], i );
                           rc += check_for_all_or_one_preceeding_spaces( file_name, line_num, line, line[ i ], i );
                        }
                     }
                  }
               }
            }

            if( i && !was_c_style_comment && !in_c_style_comment && !in_cpp_style_comment )
               output += last_ch;
         }

         last_ch = line[ i ];

         if( last_ch != '\\' )
            was_back_slash_literal = false;
      }

      if( last_ch && !was_c_style_comment && !in_c_style_comment && !in_cpp_style_comment )
         output += last_ch;

      int len = output.length( );
      while( len > 0 )
      {
         if( output[ len - 1 ] == ' ' )
            --len;
         else
            break;
      }

#ifdef OUTPUT_WITHOUT_COMMENTS
      if( len || ( !inpf.eof( ) && !was_c_style_comment && !in_c_style_comment && !in_cpp_style_comment ) )
         cout << output << endl;
#endif

      for( len = line.length( ) - 1; len > 0; len-- )
         if( line[ len ] != ' ' )
            break;

      if( len >= 0 && line[ len ] != '\\' )
      {
         in_char_literal = false;
         in_string_literal = false;
      }

      if( was_c_style_comment )
         was_c_style_comment = false;
   }

   if( in_c_style_comment )
   {
#ifdef DEBUG
      DEBUG_ERROR( );
#endif
      return output_error( file_name, comment,
       comment_num, "unexpected eof whilst processing comment" );
   }

   return rc;
}

int main( int argc, char* argv[ ] )
{
   if( argc < 2 )
   {
      cout << TITLE << ' ' << VER_NO << endl;
      cout << USAGE << endl;
      return 1;
   }

   int first_arg = 1;
   if( argc > 2 )
   {
      if( string( argv[ first_arg ] ) == "-q" )
      {
         first_arg++;
         g_is_quiet = true;
      }

      if( argc > first_arg )
      {
         if( string( argv[ first_arg ] ) == "-r" )
         {
            first_arg++;
            g_return_after_first_error_found = true;
         }
      }

      if( argc > first_arg )
      {
         if( string( argv[ first_arg ] ) == "-n" )
         {
            first_arg++;
            g_allow_nested_c_style_comments = true;
         }
      }

      if( argc > first_arg )
      {
         string tmp( argv[ first_arg ] );
         if( tmp.length( ) > 2 && tmp.substr( 0, 2 ) == "-p" )
         {
            first_arg++;
            g_path_to_prepend = tmp.substr( 2 );
         }
      }

      if( argc > first_arg )
      {
         string tmp( argv[ first_arg ] );
         if( tmp.length( ) > 2 && tmp.substr( 0, 2 ) == "-i" )
         {
            tmp.erase( 0, 2 );
            first_arg++;
            while( true )
            {
               size_t pos = tmp.find( ',' );
               g_cmd_include_masks.push_back( tmp.substr( 0, pos ) );
               if( pos == string::npos )
                  break;
               else
                  tmp.erase( 0, pos + 1 );
            }
         }
      }

      if( argc > first_arg )
      {
         string tmp( argv[ first_arg ] );
         if( tmp.length( ) >= 2 && tmp.substr( 0, 2 ) == "-m" )
         {
            first_arg++;
            g_error_marker_symbol = tmp.substr( 2 );
         }
      }
   }

   get_ignore_masks( );
   get_include_masks( );

   int rc = 0;

   while( first_arg < argc )
      rc += process_file( argv[ first_arg++ ] );

   return rc;
}
