// Copyright (c) 2011
//
// CIYAM Pty. Ltd.
// ACN 093 704 539
//
// ALL RIGHTS RESERVED
//
// Permission to use this software for non-commercial purposes is hereby granted. Permission to
// distribute this software privately is granted provided that the source code is unaltered and
// complete or that any alterations and omissions have been first approved by CIYAM. Commercial
// usage of this software is not permitted without first obtaining a license for such a purpose
// from CIYAM. This software may not be publicly distributed unless written permission to do so
// has been obtained from CIYAM.

#ifdef __BORLANDC__
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <fstream>
#  include <iomanip>
#  include <sstream>
#  include <iostream>
#  include <stdexcept>
#endif

#ifdef __GNUG__
#  include <unistd.h>
#  define _chdir chdir
#  define _mkdir mkdir
#  define _rmdir rmdir
#  define _getcwd getcwd
#  define _MAX_PATH PATH_MAX
#endif

#ifdef _WIN32
#  include <direct.h>
#endif

#include "fcgi_utils.h"

#include "sha1.h"
#include "threads.h"
#include "date_time.h"
#include "fcgi_info.h"
#include "utilities.h"
#include "fs_iterator.h"
#include "ciyam_interface.h"

using namespace std;

namespace
{

ofstream g_logfile;

mutex g_log_mutex;

string g_server_id;

set< string > g_non_persistent;

map< string, string > g_strings;

#ifndef _WIN32
const int c_default_directory_perms = S_IRWXU;
#endif

const char* const c_log_file = "ciyam_interface.log";
const char* const c_str_file = "ciyam_interface.txt";

const char* const c_extkeys_file = "extkeys.txt";

const char* const c_action_clone_key_id = "@id";
const char* const c_action_child_key_user = "@user";

const char* const c_nbsp = "&nbsp;";

string g_nbsp( c_nbsp );

inline const string& data_or_nbsp( const string& input )
{
   if( !input.empty( ) )
      return input;
   else
      return g_nbsp;
}

}

ostream& get_log( )
{
   return g_logfile;
}

void init_log( )
{
#ifndef DEBUG
   g_logfile.open( c_log_file, ios::out );
#else
   g_logfile.open( c_log_file, ios::out | ios::app );
#endif
}

scoped_flusher::scoped_flusher( ostream& os )
 : os( os )
{
   p_guard = new guard( g_log_mutex );
}

scoped_flusher::~scoped_flusher( )
{
   os << endl;
   delete p_guard;
}

const string& get_server_id( )
{
   return g_server_id;
}

void set_server_id( const string& id )
{
   g_server_id = id;
}

void init_strings( )
{
   read_strings( c_str_file, g_strings, "c_display_" );

   if( file_exists( c_extkeys_file ) )
      read_strings( c_extkeys_file, g_strings );
}

string get_string( const char* p_id )
{
   string str( p_id );

   if( g_strings.count( p_id ) )
      str = g_strings[ p_id ];

   return str;
}

string get_display_string( const string& key )
{
   string display_string;

   storage_info& sinfo( get_storage_info( ) );

   module_const_iterator mci, end;
   for( mci = sinfo.modules.begin( ), end = sinfo.modules.end( ); mci != end; ++mci )
   {
      display_string = mci->get_string( key );
      if( display_string != key )
         break;
   }

   return display_string;
}

string get_module_id( const string& module_name )
{
   string id;

   storage_info& sinfo( get_storage_info( ) );

   module_const_iterator mci, end;
   for( mci = sinfo.modules.begin( ), end = sinfo.modules.end( ); mci != end; ++mci )
   {
      if( mci->name == module_name )
      {
         id = mci->id;
         break;
      }
   }

   return id;
}

string get_module_id_for_attached_file( const source& source )
{
   string name( source.module );
   string bclass( source.bclass );

   if( !bclass.empty( ) )
   {
      string::size_type pos = bclass.find( ':' );
      if( pos != string::npos )
         bclass.erase( pos );

      name = bclass;
   }

   storage_info& sinfo( get_storage_info( ) );

   module_index_const_iterator mici;
   mici = sinfo.modules_index.find( name );
   if( mici == sinfo.modules_index.end( ) )
      throw runtime_error( "unexpected index entry for module '" + name + "' not found" );

   return ( mici->second )->id;
}

string get_module_page_name( const string& module_ref, bool check_for_index )
{
   string str( lower( module_ref ) );
   string page_name;

   if( check_for_index )
   {
      if( file_exists( "index.htm" ) )
         page_name = "index.htm";

      if( file_exists( "index.html" ) )
         page_name = "index.html";
   }

   if( page_name.empty( ) )
   {
      if( file_exists( str + ".htm" ) )
         page_name = str + ".htm";

      if( file_exists( str + ".html" ) )
         page_name = str + ".html";
   }

   return page_name;
}

#ifdef _WIN32
bool delete_files( const char* p_dir, bool recycle )
{
   int len = strlen( p_dir );
   char* p_from = new char[ len + 2 ];
   memcpy( ( void* )p_from, ( void* )p_dir, len );

   // NOTE: Requires two trailing zeros to mark the end of the file list.
   p_from[ len ] = 0;
   p_from[ len + 1 ] = 0;
  
   SHFILEOPSTRUCT fileop;
   fileop.hwnd = 0;
   fileop.wFunc = FO_DELETE;
   fileop.pFrom = p_from;
   fileop.pTo = 0;
   fileop.fFlags = FOF_NOCONFIRMATION | FOF_SILENT;
  
   if( recycle )
      fileop.fFlags |= FOF_ALLOWUNDO;

   fileop.fAnyOperationsAborted = 0;
   fileop.lpszProgressTitle = 0;
   fileop.hNameMappings = 0;

   int ret = ::SHFileOperation( &fileop );
   delete[ ] p_from;

  return ret == 0;
}
#endif

string get_hash( const string& values )
{
   DEBUG_TRACE << "(hash) values = " << values;

   sha1 hash( values );

   vector< string > sha1_quads;
   split( hash.get_digest_as_string( ',' ), sha1_quads );

   if( sha1_quads.size( ) != 5 )
      throw runtime_error( "unexpected hash result" );

   return sha1_quads[ 2 ];
}

string get_user_hash( const string& user_id )
{
   sha1 hash( g_server_id + user_id );

   vector< string > sha1_quads;
   split( hash.get_digest_as_string( ',' ), sha1_quads );

   if( sha1_quads.size( ) != 5 )
      throw runtime_error( "unexpected hash result" );

   return sha1_quads[ 2 ];
}

string get_checksum( const string& values )
{
   DEBUG_TRACE << "(checksum) values = " << values;

   sha1 hash( g_server_id + values );

   vector< string > sha1_quads;
   split( hash.get_digest_as_string( ',' ), sha1_quads );

   if( sha1_quads.size( ) != 5 )
      throw runtime_error( "unexpected hash result" );

   return sha1_quads[ 2 ];
}

string get_checksum( const session_info& sess_info, const string& values )
{
   DEBUG_TRACE << "(checksum) values = " << values;

   sha1 hash( sess_info.checksum_prefix + values );

   vector< string > sha1_quads;
   split( hash.get_digest_as_string( ',' ), sha1_quads );

   if( sha1_quads.size( ) != 5 )
      throw runtime_error( "unexpected hash result" );

   return sha1_quads[ 2 ];
}

string get_cookie_value( const string& session_id,
 const string& user_id, bool is_login_or_logout, int dtm_offset, int gmt_offset )
{
   string cookie;

   if( !get_storage_info( ).login_days )
   {
      cookie = "session=" + session_id + ",user=" + user_id
       + ",hash=" + get_user_hash( user_id ) + ",keep=,dtmoff=,gmtoff=";

      if( user_id == c_anon_user_key )
         cookie += "; Expires=Thu, 01-Jan-1970 00:00:01 UTC; path=/";
   }
   else
   {
      if( session_id == c_new_session )
         cookie = "session=,";
      else
         cookie = "session=" + session_id + ",";

      cookie += "user=" + user_id + ",hash=" + get_user_hash( user_id );

      // NOTE: The "dtm_offset" is the difference between what the client originally reported
      // as UTC at the time of the initial login and the UTC found on the server at that time.
      date_time dt( date_time::standard( ) + ( seconds )dtm_offset );

      seconds s( is_login_or_logout ? seconds_per_day( ) * -1 : c_timeout_seconds );

      if( session_id.empty( ) || ( session_id == c_new_session ) || g_non_persistent.count( session_id ) )
      {
         dt += s;
         cookie += ",keep=";
      }
      else
      {
         cookie += ",keep=true";
         dt += ( days )get_storage_info( ).login_days;
      }

      cookie += ",dtmoff=" + to_string( dtm_offset ) + ",gmtoff=" + to_string( gmt_offset );

      cookie += "; Expires=" + dt.weekday_name( true ) + ", " + to_string( ( int )dt.get_day( ) ) + "-" + dt.month_name( true )
       + "-" + to_string( dt.get_year( ) ) + " " + dt.get_time( ).as_string( e_time_format_hhmmss, true ) + " UTC; path=/";
   }

   return cookie;
}

void setup_gmt_and_dtm_offset( map< string, string >& input_data, session_info& sess_info )
{
   if( !sess_info.has_set_offsets )
   {
      string utcdtm( input_data[ c_param_utcdtm ] );
      if( !utcdtm.empty( ) )
      {
         date_time dt( input_data[ c_param_utcdtm ] );
         sess_info.dtm_offset = ( int )( dt - date_time::standard( ) );
      }

      // NOTE: The current GMT offset is used to determine "defcurrent" field
      // values (although it may not be the same as the user's set timezone).
      string gmt_offcur( input_data[ c_param_tzoffcur ] );
      if( !gmt_offcur.empty( ) )
         sess_info.gmt_offset = atoi( gmt_offcur.c_str( ) ) * -60;

      sess_info.has_set_offsets = true;
   }
}

bool is_non_persistent( const string& session_id )
{
   return g_non_persistent.count( session_id );
}

void add_non_persistent( const string& session_id )
{
   g_non_persistent.insert( session_id );
}

void remove_non_persistent( const string& session_id )
{
   if( g_non_persistent.count( session_id ) )
      g_non_persistent.erase( session_id );
}

void remove_session_temp_directory( const string& session_id, const char* p_prefix )
{
   string path( c_files_directory );
   path += "/" + string( c_tmp_directory );
   path += "/" + session_id;

   if( p_prefix )
      path = string( p_prefix ) + "/" + path;

   if( file_exists( path.c_str( ) ) )
   {
#ifdef _WIN32
      string directory( get_storage_info( ).web_root + "\\" );
      directory += c_files_directory;
      directory += "\\" + string( c_tmp_directory );
      directory += "\\" + session_id;

      delete_files( ( directory + "\\*.*" ).c_str( ) );
#else
      fs_iterator fsi( path, 0, true );

      while( fsi.has_next( ) )
         file_remove( fsi.get_full_name( ) );
#endif
   }

   if( file_exists( path.c_str( ) ) )
      _rmdir( path.c_str( ) );
}

string double_escaped( const string& src, const string& chars )
{
   string retval;

   for( size_t i = 0; i < src.length( ); i++ )
   {
      for( size_t j = 0; j < chars.length( ); j++ )
      {
         if( src[ i ] == chars[ j ] )
            retval += "\\\\";
      }

      retval += src[ i ];
   }

   return retval;
}

void str_replace( string& src, const char* p_find, const string& replace )
{
   string str;
   size_t pos = src.find( p_find );
   if( pos != string::npos )
   {
      str = src.substr( 0, pos );

      str += replace;

      pos += strlen( p_find );
      str += src.substr( pos );

      src = str;
   }
}

string remove_key( const string& src )
{
   string str( src );
   string::size_type pos = str.find( " [key: " );

   if( pos != string::npos )
   {
      string::size_type epos = str.find( "] ", pos + 1 );
      if( epos != string::npos )
         str.erase( pos, epos - pos + 1 );
   }

   return str;
}

string string_message( const string& display_string,
 const pair< string, string >& parm1, const char* p_closing )
{
   string result;

   string::size_type pos = display_string.find( parm1.first );
   if( pos == string::npos )
      throw runtime_error( "parameter '" + parm1.first + "' not found in string message '" + display_string + "'" );

   result += display_string.substr( 0, pos );
   result += parm1.second;

   pos += parm1.first.length( ) - 1;

   // NOTE: Check for a "closing" parameter such as "{#parm}" for simple HTML closing tags.
   string close( parm1.first );
   if( close.length( ) > 2 )
   {
      close[ 1 ] = '#';

      string::size_type cpos = display_string.find( close, pos + 1 );

      if( cpos == string::npos )
         cpos = pos;
      else
      {
         if( cpos != pos + 1 )
            result += display_string.substr( pos + 1, cpos - pos - 1 );

         if( p_closing )
            result += string( p_closing );

         cpos += close.length( ) - 1;
      }

      result += display_string.substr( cpos + 1 );
   }

   return result;
}

string string_message( const string& display_string,
 const pair< string, string >& parm1, const char* p_close1,
 const pair< string, string >& parm2, const char* p_close2 )
{
   string result = string_message( display_string, parm1, p_close1 );
   result = string_message( result, parm2, p_close2 );

   return result;
}

string replace_underbars( const string& src, char with )
{
   string str;

   for( size_t i = 0; i < src.size( ); i++ )
   {
      if( src[ i ] == '_' )
         str += with;
      else
         str += src[ i ];
   }

   return str;
}

string action_text( const string& s )
{
   string str;

   for( size_t i = 0; i < s.size( ); i++ )
   {
      if( s[ i ] != '!' && s[ i ] != '^' )
         str += s[ i ];
   }

   return str;
}

string format_date( const udate& ud, const char* p_dp )
{
   ostringstream osstr;

   bool output_century = true;
   bool output_decade = true;

   if( p_dp && ( string( p_dp ) == "months" || string( p_dp ) == "decades" ) )
      output_century = false;

   if( p_dp && string( p_dp ) == "months" )
      output_decade = false;

   if( output_century )
      osstr << ud.get_century( );

   if( output_decade )
      osstr << setfill( '0' ) << setw( 2 ) << ud.get_decade( );

   if( output_century || output_decade )
      osstr << '-';

   osstr << setfill( '0' )
    << setw( 2 ) << ( int )ud.get_month( )
    << '-' << setw( 2 ) << ( int )ud.get_day( );

   return osstr.str( );
}

string format_time( const mtime& mt, const char* p_tp )
{
   ostringstream osstr;

   bool output_seconds = true;

   if( p_tp && string( p_tp ) == "minutes" )
      output_seconds = false;

   osstr << setfill( '0' )
    << setw( 2 ) << ( int )mt.get_hour( )
    << ':' << setw( 2 ) << ( int )mt.get_minute( );

   if( output_seconds )
      osstr << ':' << setw( 2 ) << ( int )mt.get_second( );

   return osstr.str( );
}

string format_date_time( const date_time& dt, const char* p_tp )
{
   ostringstream osstr;

   bool output_seconds = true;

   if( p_tp && string( p_tp ) == "minutes" )
      output_seconds = false;

   osstr << dt.get_year( ) << setfill( '0' )
    << '-' << setw( 2 ) << ( int )dt.get_month( )
    << '-' << setw( 2 ) << ( int )dt.get_day( )
    << ' ' << setw( 2 ) << ( int )dt.get_hour( )
    << ':' << setw( 2 ) << ( int )dt.get_minute( );

   if( output_seconds )
      osstr << ':' << setw( 2 ) << ( int )dt.get_second( );

   return osstr.str( );
}

string escape_markup( const string& input )
{
   string output;

   for( size_t i = 0; i < input.size( ); i++ )
   {
      if( input[ i ] == '<' )
         output += "&lt;";
      else if( input[ i ] == '>' )
         output += "&gt;";
      else if( input[ i ] == '"' )
         output += "&#34;";
      else if( input[ i ] == '&' )
         output += "&#38;";
      else if( input[ i ] == '\t' )
         output += "&nbsp;&nbsp;&nbsp;";
      else if( input[ i ] == '\'' )
         output += "&#39;";
      else
         output += input[ i ];
   }

   return output;
}

string escape_specials( const string& input )
{
   string output;

   for( size_t i = 0; i < input.size( ); i++ )
   {
      if( input[ i ] == ' ' )
         output += "%20";
      else if( input[ i ] == '!' )
         output += "%21";
      else if( input[ i ] == '"' )
         output += "%22";
      else if( input[ i ] == '#' )
         output += "%23";
      else if( input[ i ] == '$' )
         output += "%24";
      else if( input[ i ] == '%' )
         output += "%25";
      else if( input[ i ] == '&' )
         output += "%26";
      else if( input[ i ] == '\'' )
         output += "%27";
      else if( input[ i ] == '(' )
         output += "%28";
      else if( input[ i ] == ')' )
         output += "%29";
      else if( input[ i ] == '*' )
         output += "%2A";
      else if( input[ i ] == '+' )
         output += "%2B";
      else if( input[ i ] == ',' )
         output += "%2C";
      else if( input[ i ] == '-' )
         output += "%2D";
      else if( input[ i ] == '.' )
         output += "%2E";
      else if( input[ i ] == '/' )
         output += "%2F";
      else if( input[ i ] == '<' )
         output += "%3C";
      else if( input[ i ] == '=' )
         output += "%3D";
      else if( input[ i ] == '>' )
         output += "%3E";
      else if( input[ i ] == '?' )
         output += "%3F";
      else if( input[ i ] == '@' )
         output += "%40";
      else if( input[ i ] == '[' )
         output += "%5B";
      else if( input[ i ] == '\\' )
         output += "%5C";
      else if( input[ i ] == ']' )
         output += "%5D";
      else if( input[ i ] == '^' )
         output += "%5E";
      else if( input[ i ] == '`' )
         output += "%60";
      else if( input[ i ] == '{' )
         output += "%7B";
      else if( input[ i ] == '|' )
         output += "%7C";
      else if( input[ i ] == '}' )
         output += "%7D";
      else if( input[ i ] == '~' )
         output += "%7E";
      else
         output += input[ i ];
   }

   return output;
}

string replace_spaces( const string& input, const char* p_rep, size_t spos )
{
   string str( input );

   string::size_type pos;

   if( p_rep && *p_rep != 0 )
   {
      for( pos = spos; pos < str.length( ); pos++ )
      {
         if( str[ pos ] != ' ' )
            break;

         str.replace( pos, 1, p_rep );
         pos += strlen( p_rep );
      }
   }

   return str;
}

string replace_crlfs_and_spaces( const string& input, const char* p_rep, const char* p_srep )
{
   string str( input );

   string::size_type pos;

   if( p_srep && *p_srep != 0 )
      str = replace_spaces( str, p_srep, 0 );

   while( true )
   {
      pos = str.find( "\\r\\n" );
      if( pos == string::npos )
         break;

      str.replace( pos, 4, p_rep );

      if( p_srep && *p_srep != 0 )
         str = replace_spaces( str, p_srep, pos + strlen( p_rep ) );
   }

   while( true )
   {
      pos = str.find( "\\r" );
      if( pos == string::npos )
         break;

      str.replace( pos, 2, p_rep );

      if( p_srep && *p_srep != 0 )
         str = replace_spaces( str, p_srep, pos + strlen( p_rep ) );
   }

   while( true )
   {
      pos = str.find( "\\n" );
      if( pos == string::npos )
         break;

      str.replace( pos, 2, p_rep );

      if( p_srep && *p_srep != 0 )
         str = replace_spaces( str, p_srep, pos + strlen( p_rep ) );
   }

   if( p_srep && *p_srep != 0 )
   {
      for( pos = 0; pos < str.length( ); pos++ )
      {
         if( pos > 0 && str[ pos ] == ' ' && str[ pos - 1 ] == ' ' )
         {
            str.replace( pos, 1, p_srep );
            pos += strlen( p_srep );
         }
      }
   }

   return str;
}

string valid_file_name( const string& str, bool* p_has_wide_chars )
{
   string s;

   if( p_has_wide_chars )
      *p_has_wide_chars = false;

   for( size_t i = 0; i < str.length( ); i++ )
   {
      char c = str[ i ];

      if( c < 0 && p_has_wide_chars )
         *p_has_wide_chars = true;

      if( c == '"' )
         s += "'";

      if( c != '"' && c != ':' && c != '?' && c!= '*'
       && c != '<' && c != '>' && c != '|' && c != '/' && c != '\\' )
         s += c;
   }

   return s;
}

void create_tmp_file_link( string& tmp_link_path,
 const string& file_name, const string& file_ext, const string& dest_file_name )
{
   string link_file_name( dest_file_name );
#ifndef _WIN32
   link_file_name = valid_file_name( link_file_name );
#else
   bool has_wide_chars;
   link_file_name = valid_file_name( link_file_name, &has_wide_chars );
#endif

   tmp_link_path += "/" + link_file_name + "." + file_ext;

#ifndef _WIN32
   file_remove( tmp_link_path );
   file_link( get_storage_info( ).web_root + "/" + file_name, tmp_link_path );
#else
   if( !has_wide_chars )
   {
      file_remove( tmp_link_path );
      file_link( get_storage_info( ).web_root + "/" + file_name, tmp_link_path );
   }
   else
   {
      wstring wsrc;
      wchar_t buffer[ 256 ];
      memset( buffer, '\0', sizeof( buffer ) );

      string s( get_storage_info( ).web_root + "/" + file_name );
      for( size_t i = 0; i < s.size( ); i++ )
         wsrc += ( wchar_t )s[ i ];

      ::MultiByteToWideChar( CP_UTF8, MB_ERR_INVALID_CHARS,
       tmp_link_path.c_str( ), tmp_link_path.length( ), buffer, 255 );

      wstring wname( buffer );

      file_remove( wname );
      file_linkw( wsrc.c_str( ), wname.c_str( ) );
   }
#endif
}

string exec_args( const string& input )
{
   string args;

   string::size_type pos = input.find( ':' );
   args += input.substr( 0, pos );

   if( pos != string::npos )
   {
      args += " \"";
      string next( input.substr( pos + 1 ) );

      while( true )
      {
         pos = next.find( ';' );
         string next_arg( next.substr( 0, pos ) );

         if( next_arg == "~" )
            args += "\\\"\\\"";
         else
            args += next_arg;

         if( pos == string::npos )
            break;

         args += " ";
         next.erase( 0, pos + 1 );
      }

      args += "\"";
   }

   return args;
}

void setup_directories( )
{
   DEBUG_TRACE << "[setup directories]";

   char buf[ _MAX_PATH ];
   _getcwd( buf, _MAX_PATH );

   storage_info& sinfo( get_storage_info( ) );

   if( _chdir( sinfo.web_root.c_str( ) ) != 0 )
      throw runtime_error( "unable to _chdir to '" + sinfo.web_root + "'" );

   if( _chdir( c_files_directory ) != 0 )
   {
#ifdef _WIN32
      if( _mkdir( c_files_directory ) != 0 )
#else
      if( _mkdir( c_files_directory, c_default_directory_perms ) != 0 )
#endif 
         throw runtime_error( "unable to create '" + string( c_files_directory ) + "' directory" );

      if( _chdir( c_files_directory ) != 0 )
         throw runtime_error( "unable to _chdir to '" + string( c_files_directory ) + "'" );
   }

   vector< string > dead_sessions;

   if( _chdir( c_tmp_directory ) != 0 )
   {
#ifdef _WIN32
      if( _mkdir( c_tmp_directory ) != 0 )
#else
      if( _mkdir( c_tmp_directory, c_default_directory_perms ) != 0 )
#endif
         throw runtime_error( "unable to create '" + string( c_tmp_directory ) + "' directory" );

      if( _chdir( c_tmp_directory ) != 0 )
         throw runtime_error( "unable to _chdir to '" + string( c_tmp_directory ) + "'" );
   }
   else
   {
      // NOTE: If sessions had ended without cleaning up their temporary directories
      // then these will be cleaned up.
      directory_filter df;
      fs_iterator dfsi( ".", &df );

      while( dfsi.has_next( ) )
         dead_sessions.push_back( dfsi.get_name( ) );
   }

   _chdir( ".." );

   for( size_t x = 0; x < dead_sessions.size( ); x++ )
      remove_session_temp_directory( dead_sessions[ x ], ".." );

   map< string, set< string > > module_file_class_ids;

   module_index_const_iterator mici;
   for( mici = sinfo.modules_index.begin( ); mici != sinfo.modules_index.end( ); ++mici )
   {
      const module_info& mod_info( *mici->second );

      view_info_const_iterator vici;
      for( vici = mod_info.view_info.begin( ); vici != mod_info.view_info.end( ); ++vici )
      {
         if( !( vici->second )->file_field_id.empty( ) )
         {
            string cid( ( vici->second )->cid );
            string bclass( ( vici->second )->bclass );

            string module( mici->first );

            if( !bclass.empty( ) )
            {
               string::size_type pos = bclass.find( ':' );
               module = bclass.substr( 0, pos );
            }

            module_file_class_ids[ module ].insert( cid );
         }
      }
   }

   map< string, set< string > >::iterator i;
   for( i = module_file_class_ids.begin( ); i != module_file_class_ids.end( ); ++i )
   {
      mici = sinfo.modules_index.find( i->first );
      if( mici == sinfo.modules_index.end( ) )
         throw runtime_error( "unexpected index entry for module '" + i->first + "' not found" );

      string module_id( ( mici->second )->id );

      if( _chdir( module_id.c_str( ) ) != 0 )
      {
         string name( module_id );
#ifdef _WIN32
         _mkdir( name.c_str( ) );
#else
         _mkdir( name.c_str( ), c_default_directory_perms );
#endif
         if( _chdir( name.c_str( ) ) != 0 )
            throw runtime_error( "unable to _chdir to '" + name + "'" );
      }

      for( set< string >::iterator si = i->second.begin( ), end = i->second.end( ); si != end; ++si )
      {
         string name( *si );

         if( _chdir( name.c_str( ) ) == 0 )
         {
            if( !file_exists( ".htaccess" ) )
            {
               ofstream htaf( ".htaccess", ios::out | ios::binary );
               htaf << "<Files *>\n";
               htaf << " deny from all\n";
               htaf << "</Files>\n";
            }
            _chdir( ".." );
         }
         else
         {
#ifdef _WIN32
            _mkdir( name.c_str( ) );
#else
            _mkdir( name.c_str( ), c_default_directory_perms );
#endif

            if( _chdir( name.c_str( ) ) != 0 )
               throw runtime_error( "unable to _chdir to '" + name + "'" );

            ofstream htaf( ".htaccess", ios::out | ios::binary );
            htaf << "<Files *>\n";
            htaf << " deny from all\n";
            htaf << "</Files>\n";

            ofstream outf( "_NOTE_.TXT" );
            outf << "NOTE: The files in this directory are tied to a DB and therefore should not be manually added or removed.";

            _chdir( ".." );
         }
      }

      _chdir( ".." );
   }

   // NOTE: Restore the original working directory.
   if( _chdir( buf ) != 0 )
      throw runtime_error( "unable to _chdir to '" + string( buf ) + "'" );
}

bool has_permission( const string& perm, const session_info& sess_info )
{
   bool has_perm = true;

   if( !perm.empty( ) )
   {
      bool negate = false;
      if( perm[ 0 ] == '!' )
         negate = true;

      if( ( !negate && !sess_info.user_perms.count( perm ) )
       || ( negate && sess_info.user_perms.count( perm.substr( 1 ) ) ) )
         has_perm = false;
   }

   return has_perm;
}

bool has_perm_extra( const string& perm_extra,
 const map< string, string >& source_extras, const session_info& sess_info )
{
   bool has_perm_extra = false;

   if( source_extras.count( perm_extra ) )
   {
      string extra_data( source_extras.find( perm_extra )->second );

      if( extra_data.empty( ) )
         has_perm_extra = true;
      else
         has_perm_extra = has_permission( extra_data, sess_info );
   }

   return has_perm_extra;
}

void replace_action_parms( string& id, string& action,
 const string& restrict_fields, const string& restrict_values )
{
   string::size_type spos = 0;
   while( !( ( action[ spos ] >= '0' && action[ spos ] <= '9' )
    || ( action[ spos ] >= 'A' && action[ spos ] <= 'Z' ) || ( action[ spos ] >= 'a' && action[ spos ] <= 'z' ) ) )
      ++spos;

   id = action.substr( spos );
   string::size_type pos = id.find( '+' );
   if( pos != string::npos )
      id.erase( pos );

   pos = action.find( '+' );
   if( pos != string::npos )
   {
      string parameters( action.substr( pos + 1 ) );
      action.erase( pos );

      vector< string > parms;
      split( parameters, parms, '+' );

      string str;
      for( size_t j = 0; j < parms.size( ); j++ )
      {
         if( j > 0 )
            str += ';';
         string next;
         if( parms[ j ] == "@rfields" )
            next = restrict_fields;
         else if( parms[ j ] == "@rvalues" )
            next = restrict_values;
         else
            next = parms[ j ];

         if( next.empty( ) )
            next = "~";

         str += next;
      }

      action += ":" + str;
   }
}

void replace_links_and_output( const string& s,
 const string& id, const string& module, const string& module_ref,
 ostream& os, bool is_content, bool output_hrefs, const string& session_id,
 const session_info& sess_info, const string& user_select_key, bool using_session_cookie, bool use_url_checksum )
{
   const module_info& mod_info( *get_storage_info( ).modules_index.find( module )->second );

   string cell_data( s );

   string::size_type lpos;
   while( ( lpos = cell_data.find( '{' ) ) != string::npos )
   {
      if( lpos != 0 )
      {
         if( is_content )
            os << unescaped( cell_data.substr( 0, lpos ) );
         else
            os << data_or_nbsp( escape_markup( unescaped( cell_data.substr( 0, lpos ) ) ) );

         cell_data.erase( 0, lpos );
      }

      string::size_type rpos = cell_data.find( '}' );
      if( rpos == string::npos )
         throw runtime_error( "unexpected manual link format in '" + cell_data + "'" );

      string::size_type npos = cell_data.find( ':' );
      if( npos == string::npos || npos > rpos )
         throw runtime_error( "unexpected manual link format in '" + cell_data + "'" );

      string next( cell_data.substr( 1, rpos - 1 ) );
      cell_data.erase( 0, rpos + 1 );

      string next_key( next.substr( 0, npos - 1 ) );

      string vid( id );
      string::size_type cpos = next_key.find( '$' );
      if( cpos != string::npos )
      {
         vid = next_key.substr( 0, cpos );
         next_key.erase( 0, cpos + 1 );

         vid = mod_info.view_cids.find( vid )->second;
      }

      bool is_href = false;
      if( output_hrefs )
      {
         is_href = true;
         os << "<a href=\"" << get_module_page_name( module_ref )
          << "?cmd=" << c_cmd_view << "&data=" << next_key << "&ident=";

         os << vid;

         if( !user_select_key.empty( ) )
            os << "&" << c_param_uselect << "=" << user_select_key;

         if( !using_session_cookie )
            os << "&session=" << session_id;

         if( use_url_checksum )
         {
            string checksum_values(
             string( c_cmd_view ) + next_key + vid + user_select_key );

            os << "&" << c_param_chksum << "=" << get_checksum( sess_info, checksum_values );
         }

         os << "\">";
      }

      if( is_content )
         os << unescaped(
          replace_crlfs_and_spaces( next.substr( npos ), "<br/>", "&nbsp;" ) );
      else
         os << data_or_nbsp( unescaped(
          replace_crlfs_and_spaces( escape_markup( next.substr( npos ) ), "<br/>", "&nbsp;" ) ) );

      if( is_href )
         os << "</a>";
   }

   if( !cell_data.empty( ) )
   {
      if( is_content )
         os << unescaped(
          replace_crlfs_and_spaces( cell_data, "<br/>", "&nbsp;" ) );
      else
         os << data_or_nbsp( unescaped(
          replace_crlfs_and_spaces( escape_markup( cell_data ), "<br/>", "&nbsp;" ) ) );
   }
}

void output_actions( ostream& os,
 const source& src, const string& cmd, const string& data,
 const session_info& sess_info, const string& ident, const string& key_and_version,
 const string& class_id, const string& class_name, const string& actions_value,
 const string& owner, const string& session_id, const string& user_select_key,
 bool using_session_cookie, bool use_url_checksum, bool has_hashval, string* p_default )
{
   const module_info& mod_info( *get_storage_info( ).modules_index.find( src.module )->second );

   vector< string > actions;

   if( !actions_value.empty( ) )
      split( actions_value, actions );

   string key( key_and_version );
   string::size_type pos = key.find( ' ' );
   if( pos != string::npos )
      key.erase( pos );

   // NOTE: Full action syntax is as follows (spaces are just here for clarity):
   //
   // [*?][@~#] [<][>] [!][^][-][_] Id[+arg1+arg2...] [$class[.field[=@user|value]]] [%@id|value] [&[!]perm]
   //
   // where args can be: @rfields|@rvalues|value

   size_t num_actions_output = 0;
   for( size_t i = 0; i < actions.size( ); i++ )
   {
      if( i > 0 )
         os << " ";

      string next_action( actions[ i ] );

      if( next_action.empty( ) )
         continue;

      string clone_key;
      string child_key( key );
      string child_class, child_field;

      pos = next_action.find( '&' );
      if( pos != string::npos )
      {
         if( !has_permission( next_action.substr( pos + 1 ), sess_info ) )
            continue;

         next_action.erase( pos );
      }

      pos = next_action.find( '$' );
      if( pos != string::npos )
      {
         child_class = next_action.substr( pos + 1 );

         next_action.erase( pos );

         pos = child_class.find( '%' );
         if( pos != string::npos )
         {
            clone_key = child_class.substr( pos + 1 );
            child_class.erase( pos );

            if( clone_key == c_action_clone_key_id )
               clone_key = key;
         }

         pos = child_class.find( '.' );
         if( pos != string::npos )
         {
            child_field = child_class.substr( pos + 1 );
            child_class.erase( pos );

            pos = child_field.find( '=' );
            if( pos != string::npos )
            {
               child_key = child_field.substr( pos + 1 );
               child_field.erase( pos );

               if( child_key == c_action_child_key_user )
                  child_key = sess_info.user_key;
            }
         }
      }

      string action( c_act_exec );
      bool needs_editing = false;
      bool needs_confirmation = false;

      // NOTE: Actions starting with '*' require the current record to be edited (presumably as some
      // extra data is required) and actions starting with a '?' will require end-user confirmation.
      if( next_action[ 0 ] == '*' )
      {
         action = c_act_edit;
         needs_editing = true;
         next_action.erase( 0, 1 );
      }
      else if( next_action[ 0 ] == '?' )
      {
         needs_confirmation = true;
         next_action.erase( 0, 1 );
      }

      // NOTE: Actions starting with '@' will only be available to an "admin" user, actions starting
      // with '~' are available to any user *except* the "admin" user and, finally, the actions that
      // start with '#' are only applicable to the user that owns the record (if specified as owned).
      if( next_action[ 0 ] == '@' )
      {
         if( !sess_info.is_admin_user )
            continue;

         next_action.erase( 0, 1 );
      }
      else if( next_action[ 0 ] == '~' )
      {
         if( sess_info.is_admin_user )
            continue;

         next_action.erase( 0, 1 );
      }
      else if( next_action[ 0 ] == '#' )
      {
         if( sess_info.user_key != owner )
            continue;

         next_action.erase( 0, 1 );
      }

      bool go_back = false;
      if( next_action[ 0 ] == '<' )
      {
         go_back = true;
         next_action.erase( 0, 1 );
      }

      bool is_default = false;
      if( next_action[ 0 ] == '>' )
      {
         is_default = true;
         next_action.erase( 0, 1 );
      }

      string next_id;
      replace_action_parms( next_id, next_action, "", "" );

      string next_label;
      if( next_action == "create_copy" )
         next_label = GDS( c_display_create_copy );
      else
         next_label = get_display_string( "procedure_" + next_id );

      if( is_default && p_default )
         *p_default = next_id;

      ++num_actions_output;
      os << "<input id=\"" << next_id << "\" name=\"" << next_id
       << "\" type=\"button\" class=\"button\" value=\"" << escape_markup( next_label ) << "\" onclick=\"";

      if( needs_confirmation )
         os << "if( confirm( '" << string_message( GDS( c_display_proceed_with_action ),
          make_pair( c_display_proceed_with_action_parm_label, next_label ) ) << "' ) ) { ";

      if( child_class.empty( ) )
      {
         if( use_url_checksum )
         {
            string checksum_values( action + cmd
             + data + next_action + ident + sess_info.user_other + to_string( sess_info.checksum_serial ) );

            if( has_hashval )
               checksum_values += c_hash_suffix;

            string new_checksum_value( get_checksum( sess_info, checksum_values ) );

            os << "var old_checksum = query_value( '" << c_param_chksum << "' ); ";
            os << "query_update( '" << c_param_chksum << "', '" << new_checksum_value << "', true ); ";
         }

         os << "dyn_load( null, 'act=" << action;

         if( needs_editing )
            os << "&chk=" << key_and_version;
         else
            os << "&app=" << key_and_version << "&cls=" << class_id
             << "&extra=' + get_all_field_values( document." << src.id << " ) + '";

         if( go_back )
            os << "&back=1";
         os << "&exec=" << next_action << "', false );";

         if( use_url_checksum )
            os << " query_update( '" << c_param_chksum << "', old_checksum, true );";

         if( go_back )
            os << " if( !had_act_error ) jump_back = true;";
      }
      else
      {
         os << "window.location.search = 'cmd=" << c_cmd_view << "&data="
          << c_new_record << clone_key << "&extra=" << child_key << "&field=" << child_field << "&ident="
          << mod_info.view_cids.find( child_class )->second;

         if( !user_select_key.empty( ) )
            os << "&" << c_param_uselect << "=" << user_select_key;

         if( !using_session_cookie )
            os << "&session=" << session_id;

         if( use_url_checksum )
         {
            string checksum_values( string( c_cmd_view ) + string( c_new_record ) + clone_key
             + mod_info.view_cids.find( child_class )->second + user_select_key + to_string( sess_info.checksum_serial ) );

            if( has_hashval )
               checksum_values += c_hash_suffix;

            os << "&" << c_param_chksum << "=" << get_checksum( sess_info, checksum_values );
         }

         os << "';";
      }

      if( needs_confirmation )
         os << " }";

      os << "\" style=\"cursor:pointer\">";
   }

   if( num_actions_output == 0 )
      os << "&nbsp;";
}

void parse_field_extra( const string& extra, map< string, string >& extra_data )
{
   vector< string > extras;
   split( extra, extras, '+' );

   for( size_t i = 0; i < extras.size( ); i++ )
   {
      string extra( extras[ i ] );
      string::size_type pos = extra.find( '=' );

      if( pos == string::npos )
         extra_data.insert( make_pair( extra, "" ) );
      else
         extra_data.insert( make_pair( extra.substr( 0, pos ), extra.substr( pos + 1 ) ) );
   }
}

void parse_key_ver_rev_state_and_type_info(
 const string& key_ver_rev_state_and_type_info,
 string& key_and_version, uint64_t& state, string& type_info )
{
   string str( key_ver_rev_state_and_type_info );

   string::size_type pos = str.find_last_of( " " );
   if( pos == string::npos )
      throw runtime_error( "unexpected key_ver_rev_state_and_type_info '" + key_ver_rev_state_and_type_info + "'" );

   type_info = str.substr( pos + 1 );

   str.erase( pos );
   pos = str.find_last_of( " " );
   if( pos == string::npos )
      throw runtime_error( "unexpected key_ver_rev_state_and_type_info '" + key_ver_rev_state_and_type_info + "'" );

   state = from_string< uint64_t >( str.substr( pos + 1 ) );

   key_and_version = str.substr( 0, pos );
}

void determine_fixed_query_info( string& fixed_fields,
 string& fixed_key_values, int& num_fixed_key_values, bool& is_reverse,
 const list_source& list, const string& fixed_parent_field, const string& fixed_parent_keyval,
 const map< string, string >& list_selections, const session_info& sess_info, string* p_set_field_values )
{
   if( !fixed_parent_field.empty( ) )
   {
      ++num_fixed_key_values;
      fixed_fields = fixed_parent_field;
      fixed_key_values = fixed_parent_keyval;
   }

   string user_other;

   if( !sess_info.other_aliases.count( sess_info.user_other )
    || ( list.type == c_list_type_user && list.type == c_list_type_user_child ) )
      user_other = sess_info.user_other;
   else
      user_other = sess_info.other_aliases.find( sess_info.user_other )->second;

   for( size_t i = 0; i < ( list.lici->second )->parents.size( ); i++ )
   {
      if( ( list.lici->second )->parents[ i ].operations.count( c_operation_select ) )
      {
         string suffix( ( list.lici->second )->parents[ i ].operations[ c_operation_select ] );

         string::size_type pos = suffix.find( '!' );
         if( pos != string::npos )
            suffix.erase( 0, pos + 1 );

         string name( list.id );
         name += c_prnt_suffix;
         name += ( '0' + i );

         if( list_selections.count( name ) )
         {
            ++num_fixed_key_values;

            if( num_fixed_key_values > 1 )
               fixed_fields += ",";
            fixed_fields += ( list.lici->second )->parents[ i ].field;

            if( num_fixed_key_values > 1 )
               fixed_key_values += ",";
            fixed_key_values += list_selections.find( name )->second + suffix;
         }
      }
      else if( ( list.lici->second )->parents[ i ].operations.count( c_operation_checked ) )
      {
         string name( list.id );
         name += c_prnt_suffix;
         name += ( '0' + i );

         if( !list_selections.count( name ) || list_selections.find( name )->second == c_true )
         {
            ++num_fixed_key_values;

            if( num_fixed_key_values > 1 )
               fixed_fields += ",";
            fixed_fields += ( list.lici->second )->parents[ i ].field;

            if( num_fixed_key_values > 1 )
               fixed_key_values += ",";
            fixed_key_values += ( list.lici->second )->parents[ i ].operations[ c_operation_checked ];
         }
      }
      // FUTURE: The "restricted" operation should only be applicable to "restricts" - so when
      // all "parents" have been changed to "restricts" this source code block can be removed.
      else if( ( list.lici->second )->parents[ i ].operations.count( c_operation_restricted ) )
      {
         string value( ( list.lici->second )->parents[ i ].operations[ c_operation_restricted ] );

         bool is_opt = false;
         bool is_applicable = true;
         if( !value.empty( ) && value[ 0 ] == '?' )
         {
            is_opt = true;
            value.erase( 0, 1 );
         }

         if( !value.empty( ) && value[ 0 ] == '@' )
         {
            if( value == c_parent_extra_user )
               value = sess_info.user_key;
            else if( value == c_parent_extra_group )
               value = sess_info.user_group;
            else if( value == c_parent_extra_other )
            {
               // NOTE: If is admin user then clear this value (so if optional it won't be used).
               if( !sess_info.is_admin_user )
                  value = user_other;
               else
                  value.erase( );
            }
            else if( value == c_parent_extra_parent )
               value = sess_info.user_parent;
            else
            {
               string::size_type pos = value.find( '=' );
               if( pos != string::npos && value.substr( 0, pos ) == c_extkey )
                  value = get_string( value.substr( pos + 1 ).c_str( ) );
            }
         }

         if( !is_opt || !value.empty( ) )
         {
            ++num_fixed_key_values;

            if( num_fixed_key_values > 1 )
               fixed_fields += ",";
            fixed_fields += ( list.lici->second )->parents[ i ].field;

            if( num_fixed_key_values > 1 )
               fixed_key_values += ",";
            fixed_key_values += value;
         }
      }
   }

   for( size_t i = 0; i < ( list.lici->second )->restricts.size( ); i++ )
   {
      map< string, string > restrict_extras;
      parse_field_extra( ( list.lici->second )->restricts[ i ].extra, restrict_extras );

      if( ( list.lici->second )->restricts[ i ].operations.count( c_operation_select ) )
      {
         if( !restrict_extras.count( c_field_extra_enum ) )
            continue;

         string name( list.id );
         name += c_rest_suffix;
         name += ( '0' + i );

         if( list_selections.count( name ) )
         {
            ++num_fixed_key_values;

            if( num_fixed_key_values > 1 )
               fixed_fields += ",";
            fixed_fields += ( list.lici->second )->restricts[ i ].field;

            if( num_fixed_key_values > 1 )
               fixed_key_values += ",";

            fixed_key_values += list_selections.find( name )->second;
         }
      }
      else if( ( list.lici->second )->restricts[ i ].operations.count( c_operation_restricted ) )
      {
         string value( ( list.lici->second )->restricts[ i ].operations[ c_operation_restricted ] );

         bool is_opt = false;
         bool is_applicable = true;
         if( !value.empty( ) && value[ 0 ] == '?' )
         {
            is_opt = true;
            value.erase( 0, 1 );
         }

         string append;
         string::size_type pos = value.find( '+' );
         if( pos != string::npos )
         {
            append = value.substr( pos + 1 );
            value.erase( pos );
         }

         if( !value.empty( ) && value[ 0 ] == '@' )
         {
            if( value == c_parent_extra_user )
               value = sess_info.user_key;
            else if( value == c_parent_extra_group )
               value = sess_info.user_group;
            else if( value == c_parent_extra_other )
            {
               // NOTE: If is admin user then clear this value (so if optional it won't be used).
               if( !sess_info.is_admin_user )
                  value = user_other;
               else
                  value.erase( );
            }
            else if( value == c_parent_extra_parent )
               value = sess_info.user_parent;
            else
            {
               string::size_type pos = value.find( '=' );
               if( pos != string::npos && value.substr( 0, pos ) == c_extkey )
                  value = get_string( value.substr( pos + 1 ).c_str( ) );
            }
         }

         if( !value.empty( ) && !append.empty( ) )
            value += append;

         if( !is_opt || !value.empty( ) )
         {
            ++num_fixed_key_values;

            if( num_fixed_key_values > 1 )
               fixed_fields += ",";
            fixed_fields += ( list.lici->second )->restricts[ i ].field;

            if( num_fixed_key_values > 1 )
               fixed_key_values += ",";
            fixed_key_values += value;
         }
      }
      else if( ( list.lici->second )->restricts[ i ].operations.count( c_operation_checked )
       || ( list.lici->second )->restricts[ i ].operations.count( c_operation_unchecked )
       || ( list.lici->second )->restricts[ i ].operations.count( c_operation_rchecked )
       || ( list.lici->second )->restricts[ i ].operations.count( c_operation_runchecked ) )
      {
         string name( list.id );
         name += c_rest_suffix;
         name += ( '0' + i );

         bool unchecked = false;
         if( ( list.lici->second )->restricts[ i ].operations.count( c_operation_unchecked )
          || ( list.lici->second )->restricts[ i ].operations.count( c_operation_runchecked ) )
            unchecked = true;

         // NOTE: A "reverse" checked restriction will result in the list order being reversed.
         bool reverse_checked = false;
         if( ( list.lici->second )->restricts[ i ].operations.count( c_operation_rchecked )
          || ( list.lici->second )->restricts[ i ].operations.count( c_operation_runchecked ) )
            reverse_checked = true;

         if( ( !unchecked && !list_selections.count( name ) )
          || ( list_selections.count( name ) && list_selections.find( name )->second == c_true ) )
         {
            string value;
            if( !unchecked )
            {
               if( !reverse_checked )
                  value = ( list.lici->second )->restricts[ i ].operations[ c_operation_checked ];
               else
                  value = ( list.lici->second )->restricts[ i ].operations[ c_operation_rchecked ];
            }
            else
            {
               if( !reverse_checked )
                  value = ( list.lici->second )->restricts[ i ].operations[ c_operation_unchecked ];
               else
                  value = ( list.lici->second )->restricts[ i ].operations[ c_operation_runchecked ];
            }

            if( restrict_extras.count( c_field_extra_transient ) )
            {
               if( p_set_field_values )
                  *p_set_field_values = ( list.lici->second )->restricts[ i ].field + "=" + value;
               else
                  throw runtime_error( "unexpected transient '"
                   + ( list.lici->second )->restricts[ i ].field + "' found in determine_fixed_query_info" );
            }
            else
            {
               ++num_fixed_key_values;

               if( num_fixed_key_values > 1 )
                  fixed_fields += ",";
               fixed_fields += ( list.lici->second )->restricts[ i ].field;

               if( num_fixed_key_values > 1 )
                  fixed_key_values += ",";

               fixed_key_values += value;
            }
         }
         else if( reverse_checked )
            is_reverse = !is_reverse;
      }
   }
}

