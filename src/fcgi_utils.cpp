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
#  include <cstring>
#  include <memory>
#  include <fstream>
#  include <iomanip>
#  include <sstream>
#  include <iostream>
#  include <stdexcept>
#endif

#ifdef __GNUG__
#  include <unistd.h>
#endif

#include "fcgi_utils.h"

#include "md5.h"
#include "sha1.h"
#include "regex.h"
#include "config.h"
#include "sha256.h"
#include "threads.h"
#include "date_time.h"
#include "fcgi_info.h"
#include "utilities.h"
#include "file_utils.h"
#include "fs_iterator.h"
#include "crypt_stream.h"
#include "ciyam_interface.h"

using namespace std;

extern string g_footer_html;

namespace
{

#include "ciyam_constants.h"

mutex g_mutex;

string g_sid;

#include "sid.enc"

string g_identity;

set< string > g_non_persistent;

map< string, string > g_strings;
map< string, string > g_extkeys;

const char c_extended_action_id = 'x';

const char* const c_log_file = "ciyam_interface.log";
const char* const c_str_file = "ciyam_interface.txt";

const char* const c_action_ident_key_id = "@id";
const char* const c_action_child_key_user = "@user";

const char* const c_action_parm_no_log = "@no_log";
const char* const c_action_parm_rfields = "@rfields";
const char* const c_action_parm_rvalues = "@rvalues";
const char* const c_action_parm_not_changing = "@not_changing";

const char* const c_action_parm_file_existence = "@file:";

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

void log_trace_message( const string& message )
{
   guard g( g_mutex );

   ofstream outf( c_log_file, ios::out | ios::app );

   outf << '[' << date_time::local( ).as_string( e_time_format_hhmm, true ) << "] " << message << '\n';
}

void get_server_sid( string& sid )
{
   get_sid( sid );
}

void set_server_sid( const string& sid )
{
   set_sid( sid );
}

bool matches_server_sid( const string& chk )
{
   bool retval = false;

   string sid;
   get_sid( sid );

   if( chk == sid )
      retval = true;

   clear_key( sid );

   return retval;
}

void get_server_identity( string& id )
{
   id = g_identity;
}

void set_server_identity( const string& id )
{
   g_identity = id;
}

string get_id_from_server_identity( const char* p_server_id )
{
   string key( p_server_id ? string( p_server_id ) : g_identity );

   MD5 md5;
   md5.update( ( unsigned char* )key.c_str( ), key.length( ) );
   md5.finalize( );

   clear_key( key );

   unique_ptr< char > up_digest( md5.hex_digest( ) );

   return string( up_digest.get( ) ).substr( 12 );
}

void init_strings( )
{
   g_sid.reserve( c_key_reserve_size );

   read_strings( c_str_file, g_strings, "c_display_" );
}

void init_extkeys( )
{
   g_extkeys.clear( );

   if( file_exists( c_extkeys_file ) )
   {
      read_strings( c_extkeys_file, g_extkeys );
      get_storage_info( ).extkeys_mod = last_modification_time( c_extkeys_file );
   }
   else
      get_storage_info( ).extkeys_mod = 0;
}

string get_string( const char* p_id )
{
   string str( p_id ? p_id : "" );

   if( g_strings.count( p_id ) )
      str = g_strings[ p_id ];

   return str;
}

string get_extkey( const string& id )
{
   string str( id );

   if( g_extkeys.count( id ) )
      str = g_extkeys[ id ];

   return str;
}

string get_app_name( )
{
   // KLUDGE: This needs to be able to translated but is not currently a module
   // string (perhaps add an "app" string for this that defaults to the name of
   // the Application being generated).
   return get_storage_info( ).storage_name;
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

string replace_display_strings( const string& str )
{
   string output, input( str );

   string::size_type pos = 0;

   // NOTE: Will transform "@~tags: Awaiting" into "Tags: Awaiting"
   // (assuming that "c_display_tags" is the display string "Tags")
   // with the first character not in "a-z", "0-9" and "_"  treated
   // as the delimiter (thus ':' in the above example is optional).
   while( true )
   {
      string::size_type pos = input.find( "@~" );

      if( pos == string::npos )
      {
         output += input;
         break;
      }
      else
      {
         if( pos > 0 )
         {
            output += input.substr( 0, pos );
            input.erase( 0, pos );
         }

         input.erase( 0, 2 );

         if( input.empty( ) )
            break;
         else
         {
            pos = input.find_first_not_of( "0123456789_abcdefghijklmnopqrstuvwxyz_" );

            string display_key( "c_display_" + input.substr( 0, pos ) );

            output += get_string( display_key.c_str( ) );

            input.erase( 0, pos );
         }
      }
   }

   return output;
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

string get_hash( const string& values )
{
   DEBUG_TRACE( "(hash) values = " + values );

   sha1 hash( values );

   vector< string > sha1_quads;
   split( hash.get_digest_as_string( ',' ), sha1_quads );

   if( sha1_quads.size( ) != 5 )
      throw runtime_error( "unexpected hash result" );

   return sha1_quads[ 2 ];
}

string get_user_hash( const string& user_id )
{
   string sid;
   get_sid( sid );

   sha1 hash( sid + user_id );

   clear_key( sid );

   vector< string > sha1_quads;
   split( hash.get_digest_as_string( ',' ), sha1_quads );

   if( sha1_quads.size( ) != 5 )
      throw runtime_error( "unexpected hash result" );

   return sha1_quads[ 2 ];
}

string get_checksum( const string& values )
{
   DEBUG_TRACE( "(checksum) values = " + values );

   string sid;
   get_sid( sid );

   sha1 hash( sid + values );

   clear_key( sid );

   vector< string > sha1_quads;
   split( hash.get_digest_as_string( ',' ), sha1_quads );

   if( sha1_quads.size( ) != 5 )
      throw runtime_error( "unexpected hash result" );

   return sha1_quads[ 2 ];
}

void get_session_dtm( const session_info& sess_info, date_time& dt )
{
   if( !sess_info.current_dtm.empty( ) )
      dt = sess_info.current_dtm;
   else
      dt = date_time::standard( ) + ( seconds )sess_info.gmt_offset;
}

string get_checksum( const session_info& sess_info, const string& values )
{
   DEBUG_TRACE( "(checksum) values = " + values );

   sha1 hash( sess_info.checksum_prefix + values );

   vector< string > sha1_quads;
   split( hash.get_digest_as_string( ',' ), sha1_quads );

   if( sha1_quads.size( ) != 5 )
      throw runtime_error( "unexpected hash result" );

   return sha1_quads[ 2 ];
}

string get_cookie_value( const string& session_id,
 const string& user_id, bool is_login_or_logout, int dtm_offset )
{
   string cookie;

   if( !get_storage_info( ).login_days )
   {
      cookie = "session=" + session_id + ",keep=";

      if( user_id == c_anon_user_key )
         cookie += "; SameSite=None; Secure; Expires=Thu, 01-Jan-1970 00:00:01 UTC; path=/";
   }
   else
   {
      if( session_id == c_new_session )
         cookie = "session=,";
      else
         cookie = "session=" + session_id + ",";

      // NOTE: The "dtm_offset" is the difference between what the client originally reported
      // as UTC at the time of the initial login and the UTC found on the server at that time.
      date_time dt( date_time::standard( ) + ( seconds )dtm_offset );

      seconds s( is_login_or_logout ? seconds_per_day( ) * -1 : c_timeout_seconds );

      if( session_id.empty( ) || ( session_id == c_new_session ) || g_non_persistent.count( session_id ) )
      {
         dt += s;
         cookie += "keep=";
      }
      else
      {
         cookie += "keep=true";
         dt += ( days )get_storage_info( ).login_days;
      }

      cookie += "; SameSite=None; Secure; Expires="
       + dt.weekday_name( true ) + ", " + to_string( ( int )dt.get_day( ) ) + "-" + dt.month_name( true )
       + "-" + to_string( dt.get_year( ) ) + " " + dt.get_time( ).as_string( e_time_format_hhmmss, true ) + " UTC; path=/";
   }

   return cookie;
}

void setup_gmt_and_dtm_offset( map< string, string >& input_data, session_info& sess_info )
{
   string utcdtm( input_data[ c_param_utcdtm ] );

   if( !utcdtm.empty( ) )
   {
      date_time dt( input_data[ c_param_utcdtm ] );

      sess_info.dtm_offset = ( seconds )( dt - date_time::standard( ) );
   }

   // NOTE: The current GMT offset is used to determine "defcurrent" values
   // for date and times (as they are not adjusted like a datetime type is).
   string gmt_offcur( input_data[ c_param_tzoffcur ] );

   if( !gmt_offcur.empty( ) )
      sess_info.gmt_offset = atoi( gmt_offcur.c_str( ) ) * -60;
}

string hash_password( const string& salted_password, unsigned int specific_rounds )
{
   string s( salted_password );

   if( !specific_rounds )
      specific_rounds = c_password_hash_rounds;

   for( size_t i = 0; i < specific_rounds; i++ )
      s = sha256( s + salted_password ).get_digest_as_string( );

   return s;
}

bool is_ui_prototype( )
{
   return file_exists( "ui_prototype" );
}

bool is_non_persistent( const string& session_id )
{
   guard g( g_mutex );

   return g_non_persistent.count( session_id );
}

void add_non_persistent( const string& session_id )
{
   guard g( g_mutex );

   g_non_persistent.insert( session_id );
}

void remove_non_persistent( const string& session_id )
{
   guard g( g_mutex );

   if( g_non_persistent.count( session_id ) )
      g_non_persistent.erase( session_id );
}

void remove_session_temp_directory( const string& session_id )
{
   string path( c_files_directory );

   path += "/" + string( c_tmp_directory );
   path += "/" + session_id;

   if( file_exists( path.c_str( ) ) )
      delete_directory_files( path, true );
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

   if( pos == string::npos )
      pos = str.find( " [val: " );

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

string replace_crlfs_and_spaces( const string& input,
 const char* p_rep, const char* p_srep, bool only_when_doubled )
{
   string str( input );

   string::size_type pos;

   if( p_srep && *p_srep != 0 )
      str = replace_spaces( str, p_srep, 0 );

   // NOTE: Replace all paired CR-LF's with LF's.
   replace( str, "\\r\\n", "\\n" );

   // NOTE: Replace all remaining CR's with LF's.
   replace( str, "\\r", "\\n" );

   if( !only_when_doubled )
   {
      while( true )
      {
         pos = str.find( "\\n" );

         if( pos == string::npos )
            break;

         str.replace( pos, 2, p_rep );

         if( p_srep && *p_srep != 0 )
            str = replace_spaces( str, p_srep, pos + strlen( p_rep ) );
      }
   }
   else
   {
      // NOTE: Use the provided replacement for doubled LF's.
      while( true )
      {
         pos = str.find( "\\n\\n" );

         if( pos == string::npos )
            break;

         str.replace( pos, 4, p_rep );

         if( p_srep && *p_srep != 0 )
            str = replace_spaces( str, p_srep, pos + strlen( p_rep ) );
      }

      // NOTE: Use the space replacement for single LF's
      // (or if was not provided then just remove them).
      while( true )
      {
         pos = str.find( "\\n" );

         if( pos == string::npos )
            break;

         if( !p_srep )
            str.erase( pos, 2 );
         else
            str.replace( pos, 2, p_srep );

         if( p_srep && *p_srep != 0 )
            str = replace_spaces( str, p_srep, pos + strlen( p_rep ) );
      }
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

void force_html_tags_to_lower_case( string& html )
{
   static regex html_tag( c_regex_html_tag );

   string old_html( html );
   string new_html;

   while( true )
   {
      string::size_type len;
      vector< string > refs;

      string::size_type pos = html_tag.search( old_html, &len, &refs );

      if( pos == string::npos )
      {
         new_html += old_html;
         break;
      }

      if( refs.empty( ) )
         throw runtime_error( "unexpected missing refs for html_tag" );

      if( pos != 0 )
         new_html += old_html.substr( 0, pos );

      size_t rlen = refs[ 0 ].size( );

      // NOTE: If matched a closing tag then extend the length (as the '/'
      // is not included in the reference).
      if( rlen && old_html[ pos + 1 ] == '/' )
         ++rlen;

      new_html += lower( old_html.substr( pos, rlen + 1 ) );

      new_html += old_html.substr( pos + rlen + 1, len - rlen - 1 );

      old_html.erase( 0, len + pos );
   }

   html = new_html;
}

void create_tmp_file_link_or_copy( string& tmp_path, const string& file_name,
 const string& file_ext, const string& dest_file_name, const char* p_decryption_key )
{
   string link_file_name( dest_file_name );

   if( dest_file_name.empty( ) )
      link_file_name = file_ext;
      
   link_file_name = valid_file_name( link_file_name );

   if( dest_file_name.empty( ) )
      tmp_path += "/" + link_file_name;
   else
      tmp_path += "/" + link_file_name + "." + file_ext;

   if( p_decryption_key )
   {
      file_copy( get_storage_info( ).web_root + "/" + file_name, tmp_path );

      fstream fs;
      fs.open( tmp_path.c_str( ), ios::in | ios::out | ios::binary );

      if( fs )
      {
         crypt_stream( fs, harden_key_with_hash_rounds(
          p_decryption_key, file_name_without_path( file_name, true ) ) );

         fs.flush( );
         fs.close( );
      }
   }
   else
   {
      file_remove( tmp_path );
      file_link( get_storage_info( ).web_root + "/" + file_name, tmp_path );
   }
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
   DEBUG_TRACE( "[setup directories]" );

   string files_dir( c_files_directory );
   string file_directory_note( GDS( c_display_module_file_directory_note ) );

   storage_info& sinfo( get_storage_info( ) );

   if( !dir_exists( files_dir ) )
      create_dir( files_dir, 0, ( dir_perms )c_web_files_dir_perm_val, WEB_FILES_UMASK );

   string files_tmp_dir( files_dir + '/' + string( c_tmp_directory ) );

   if( !dir_exists( files_tmp_dir ) )
      create_dir( files_tmp_dir, 0, ( dir_perms )c_web_files_dir_perm_val, WEB_FILES_UMASK );

   vector< string > dead_sessions;

   // NOTE: If sessions had ended without cleaning up their temporary directories
   // then these will be cleaned up.
   directory_filter df;
   fs_iterator dfsi( files_tmp_dir, &df );

   while( dfsi.has_next( ) )
      dead_sessions.push_back( dfsi.get_name( ) );

   for( size_t x = 0; x < dead_sessions.size( ); x++ )
      remove_session_temp_directory( dead_sessions[ x ] );

   map< string, set< string > > module_file_class_ids;

   module_index_const_iterator mici;

   for( mici = sinfo.modules_index.begin( ); mici != sinfo.modules_index.end( ); ++mici )
   {
      const module_info& mod_info( *mici->second );

      view_info_const_iterator vici;
      for( vici = mod_info.view_info.begin( ); vici != mod_info.view_info.end( ); ++vici )
      {
         if( !( vici->second )->file_ids.empty( ) )
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

      string module_dir( files_dir + '/' + module_id );

      if( !dir_exists( module_dir ) )
         create_dir( module_dir, 0, ( dir_perms )c_web_files_dir_perm_val, WEB_FILES_UMASK );

      string notes_file( module_dir + "/_note_.txt" );

      if( !file_exists( notes_file ) )
      {
         ofstream outf( notes_file.c_str( ) );
         outf << file_directory_note;
      }

      string access_file( module_dir + "/.htaccess" );

      if( !file_exists( access_file ) )
      {
         // NOTE: Empty code block for scope purposes.
         {
            ofstream htaf( access_file.c_str( ), ios::out | ios::binary );

            htaf << "<Files *>\n";
            htaf << " <IfModule mod_authz_core.c>\n";
            htaf << "  Require all denied\n";
            htaf << " </IfModule>\n";
            htaf << "</Files>\n";
         }
      }

      for( set< string >::iterator si = i->second.begin( ), end = i->second.end( ); si != end; ++si )
      {
         string name( *si );

         string class_dir( module_dir + '/' + name );

         if( !dir_exists( class_dir ) )
            create_dir( class_dir, 0, ( dir_perms )c_web_files_dir_perm_val, WEB_FILES_UMASK );
      }
   }
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

bool replace_action_parms( string& id, string& action,
 const string& restrict_fields, const string& restrict_values, bool* p_no_log, bool* p_not_changing )
{
   bool retval = true;

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

      bool ignore = false;

      for( size_t j = 0; j < parms.size( ); j++ )
      {
         if( ( j > 0 ) && !ignore )
            str += ';';

         string next( parms[ j ] );

         if( next == c_action_parm_no_log )
         {
            ignore = true;

            if( p_no_log )
               *p_no_log = true;

            continue;
         }
         else if( next == c_action_parm_rfields )
            next = restrict_fields;
         else if( next == c_action_parm_rvalues )
            next = restrict_values;
         else if( next == c_action_parm_not_changing )
         {
            ignore = true;

            if( p_not_changing )
               *p_not_changing = true;

            continue;
         }
         else
         {
            pos = next.find( c_action_parm_file_existence );

            if( pos != string::npos )
            {
               ignore = true;

               string file_name( next.substr( strlen( c_action_parm_file_existence ) ) );

               bool exists = true;

               if( !file_name.empty( ) && file_name[ 0 ] == '!' )
               {
                  exists = false;
                  file_name.erase( 0, 1 );
               }

               if( ( exists && !file_exists( file_name ) )
                || ( !exists && file_exists( file_name ) ) )
               {
                  retval = false;
                  break;
               }

               continue;
            }
         }

         ignore = false;

         if( next.empty( ) )
            next = "~";

         str += next;
      }

      if( !str.empty( ) )
         action += ":" + str;
   }

   return retval;
}

string remove_links( const string& s )
{
   string rc, str( s );

   string::size_type lpos;

   while( ( lpos = str.find( '{' ) ) != string::npos )
   {
      if( lpos != 0 )
         rc += str.substr( 0, lpos );

      str.erase( 0, lpos );

      string::size_type rpos = str.find( '}' );

      if( rpos == string::npos )
         throw runtime_error( "unexpected manual link format in '" + s + "'" );

      string::size_type npos = str.find( ':' );

      if( npos == string::npos || npos > rpos )
         throw runtime_error( "unexpected manual link format in '" + s + "'" );

      rc += str.substr( npos + 1, rpos - npos - 1 );

      str.erase( 0, rpos + 1 );
   }

   rc += str;

   return rc;
}

void replace_links_and_output( const string& s,
 const string& id, const string& module, const string& module_ref,
 ostream& os, bool is_content, bool output_hrefs, const string& session_id,
 const session_info& sess_info, const string& user_select_key, bool using_session_cookie,
 bool use_url_checksum, const string* p_key, bool cr_lfs_only_when_doubled )
{
   const module_info& mod_info( *get_storage_info( ).modules_index.find( module )->second );

   string cell_data( s );

   string::size_type lpos;

   while( ( lpos = cell_data.find( '{' ) ) != string::npos )
   {
      if( lpos != 0 )
      {
         if( is_content )
            os << unescaped(
             replace_crlfs_and_spaces(
             cell_data.substr( 0, lpos ), "<br/>", "&nbsp;", cr_lfs_only_when_doubled ) );
         else
            os << data_or_nbsp( unescaped(
             replace_crlfs_and_spaces( escape_markup(
             cell_data.substr( 0, lpos ) ), "<br/>", "&nbsp;", cr_lfs_only_when_doubled ) ) );

         cell_data.erase( 0, lpos );
      }

      string::size_type rpos = cell_data.find( '}' );

      if( rpos == string::npos )
         throw runtime_error( "unexpected manual link format in '" + cell_data + "'" );

      string::size_type npos = cell_data.find( ':' );

      if( ( npos == string::npos ) || ( npos > rpos ) )
         throw runtime_error( "unexpected manual link format in '" + cell_data + "'" );

      string next( cell_data.substr( 1, rpos - 1 ) );

      cell_data.erase( 0, rpos + 1 );

      string next_key( next.substr( 0, npos - 1 ) );

      // NOTE: A generic "@key" can optionally be replaced by the record key (is
      // otherwise changed to be "dummy" so that it is easily noticed in a URL).
      if( next_key == c_key_field )
      {
         if( p_key )
            next_key = *p_key;
         else
            next_key = string( c_dummy );
      }

      string vid( id );

      string::size_type cpos = next_key.find( '$' );

      if( cpos != string::npos )
      {
         vid = next_key.substr( 0, cpos );
         next_key.erase( 0, cpos + 1 );

         if( !mod_info.view_cids.count( vid ) )
            throw runtime_error( "unknown class id '" + vid + "' in manual link: " + s );

         vid = mod_info.view_cids.find( vid )->second;
      }

      string display( next.substr( npos ) );

      string::size_type dtpos = next_key.find( '@' );

      if( dtpos != string::npos )
      {
         string date_time_str( next_key.substr( dtpos + 1 ) );

         next_key.erase( dtpos );

         // NOTE: If is expected that the application server will have adjusted
         // to a local timezone if the user has one specified, but if they have
         // not specified one then adjust according to their GMT offset.
         date_time dt( date_time_str );

         if( sess_info.tz_name.empty( ) )
            dt += ( seconds )sess_info.gmt_offset;

         dtpos = display.find( ';' );

         if( dtpos == string::npos )
            display = format_date_time( dt, display.c_str( ) );
         else
         {
            string formatted = format_date_time( dt, display.substr( 0, dtpos ).c_str( ) );

            display.erase( 0, dtpos + 1 );

            dtpos = display.find( '@' );

            if( dtpos != string::npos )
            {
               display.erase( dtpos, 1 );
               display.insert( dtpos, formatted );
            }
         }
      }

      bool is_href = false;

      if( output_hrefs && !next_key.empty( ) )
      {
         is_href = true;

         string special;

         string::size_type pos = next_key.find( '.' );

         // NOTE: The special parameter is optionally used to identify
         // a datachain (useful for record links which are invalid but
         // perhaps only due to a missing datachain).
         if( pos != string::npos )
         {
            special = next_key.substr( pos + 1 );
            next_key.erase( pos );
         }

         os << "<a href=\"" << get_module_page_name( module_ref )
          << "?cmd=" << c_cmd_view << "&data=" << next_key << "&ident=";

         os << vid;

         if( !user_select_key.empty( ) )
            os << "&" << c_param_uselect << "=" << user_select_key;

         if( !special.empty( ) )
            os << "&special=" << special;

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
          replace_crlfs_and_spaces( display, "<br/>", "&nbsp;", cr_lfs_only_when_doubled ) );
      else
         os << data_or_nbsp( unescaped(
          replace_crlfs_and_spaces( escape_markup( display ), "<br/>", "&nbsp;", cr_lfs_only_when_doubled ) ) );

      if( is_href )
         os << "</a>";
   }

   if( !cell_data.empty( ) )
   {
      if( is_content )
         os << unescaped(
          replace_crlfs_and_spaces( cell_data, "<br/>", "&nbsp;", cr_lfs_only_when_doubled ) );
      else
         os << data_or_nbsp( unescaped(
          replace_crlfs_and_spaces( escape_markup( cell_data ), "<br/>", "&nbsp;", cr_lfs_only_when_doubled ) ) );
   }
}

void output_form( const string& module_name, ostream& os,
 const string& extra_details, const string& msg, bool is_sign_in, const string& title )
{
   os << "\n<div id=\"normal_content\">\n";

   os << "\n<div id=\"header\"><div id=\"appname\">";
   os << "<a href=\"?cmd=" << c_cmd_home << "\">" << get_app_name( ) << "</a></div>\n";

   if( !extra_details.empty( ) )
   {
      if( !is_sign_in )
      {
         string title_string( !title.empty( ) ? title : GDS( c_display_activate_account ) );

         os << "<h3 class=\"right-top\">" << title_string << "</h3>" << endl;
      }
      else
         os << "<h3 class=\"right-top\">" << GDS( c_display_sign_in_using_credentials ) << "</h3>" << endl;
   }

   os << "   <div id=\"navband\">\n";
   os << "   </div>\n";
   os << "</div>\n";

   if( extra_details.empty( ) )
   {
      os << "\n<div id=\"logout_text\">\n";
      os << msg << "\n";
      os << "</div>\n";
   }
   else
   {
      os << extra_details;
      os << msg << "\n";
   }

   os << g_footer_html;

   os << "</div>\n";
}

void output_actions( ostream& os,
 const source& src, const string& cmd, const string& data,
 const session_info& sess_info, const string& ident, const string& key_and_version,
 const string& class_id, const string& class_name, const string& actions_value,
 const string& owner, const string& session_id, const string& user_select_key,
 const string& listarg, bool using_session_cookie, bool use_url_checksum, bool has_hashval,
 string* p_default, const string* p_pfield, bool is_changing, int back_count )
{
   const module_info& mod_info( *get_storage_info( ).modules_index.find( src.module )->second );

   vector< string > actions;

   if( !actions_value.empty( ) )
      split( actions_value, actions );

   string key( key_and_version );
   string::size_type pos = key.find( ' ' );

   if( pos != string::npos )
      key.erase( pos );

   // NOTE: Full action syntax is as follows (spaces are only used here for clarity):
   //
   // [*?][@~#%] [<][>] [:] [+][-][!][^][_][/] [Idx]Id[+arg1+arg2...] [$class[.field[=@user|value]]] [[%|*]@id|value]] [&[!]perm]
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
      string record_key( c_new_record );

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

            if( clone_key == c_action_ident_key_id )
               clone_key = key;
         }
         else
         {
            pos = child_class.find( '*' );

            if( pos != string::npos )
            {
               record_key = child_class.substr( pos + 1 );
               child_class.erase( pos );

               if( record_key == c_action_ident_key_id )
                  record_key = key;
            }
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

      // NOTE: Actions starting with '@' will only be available to an "admin" user while actions with
      // '~' are available to any user *except* the "admin" user. Actions starting with '#' will only
      // be applicable to the user that owns the record (if specified as owned) whilst those starting
      // with '%' are applicable to either the "owner" or the "admin" user.
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
      else if( next_action[ 0 ] == '%' )
      {
         if( !sess_info.is_admin_user && sess_info.user_key != owner )
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

      if( next_action[ 0 ] == ':' )
      {
         if( is_changing )
            continue;

         next_action.erase( 0, 1 );
      }

      string next_id;

      if( !p_pfield )
         replace_action_parms( next_id, next_action, "", "" );
      else
         replace_action_parms( next_id, next_action, *p_pfield, data );

      string next_label( next_id );

      pos = next_id.find( c_extended_action_id );

      // NOTE: If "<Id1>x<Id2>" is found then the label for "Id1"
      // is displayed but the action to be invoked will be "Id2".
      if( pos != string::npos )
      {
         next_id.erase( 0, pos + 1 );
         next_label.erase( pos );

         // NOTE: Need to also erase the label part from the 'action'.
         pos = next_action.find( next_label + c_extended_action_id );

         if( pos != string::npos )
            next_action.erase( pos, next_label.length( ) + 1 );
      }

      if( next_action == "create_copy" )
         next_label = GDS( c_display_create_copy );
      else
         next_label = get_display_string( "procedure_" + next_label );

      if( is_default && p_default )
         *p_default = next_id;

      if( num_actions_output )
         os << '\n';

      ++num_actions_output;

      os << "<input id=\"" << next_id << "\" name=\"" << next_id
       << "\" type=\"button\" class=\"button\" value=\"" << escape_markup( next_label ) << "\" onclick=\"";

      // NOTE: Prevent any further actions from being executed whilst is busy loading
      // and hide "auto progress" (if it is currently being displayed).
      os << "if( !load_in_progress( ) ) { hide_auto_progress( ); ";

      if( needs_confirmation )
      {
         // KLUDGE: If not executed before the "confirm" then can end up doing nothing.
         os << "window.onblur = window.onfocus = null; ";

         os << "if( confirm( '" << string_message( GDS( c_display_proceed_with_action ),
          make_pair( c_display_proceed_with_action_parm_label, next_label ) ) << "' ) ) { ";
      }

      if( child_class.empty( ) )
      {
         os << "this.style.display = 'none'; ";

         if( use_url_checksum )
         {
            string checksum_values( action + cmd
             + data + next_action + ident + sess_info.user_other + to_string( sess_info.checksum_serial ) );

            if( has_hashval )
               checksum_values += c_hash_suffix;

            string new_checksum_value( get_checksum( sess_info, checksum_values ) );

            os << "var old_checksum = query_value( '" << c_param_ochksum << "' ); ";
            os << "if( old_checksum == '' ) old_checksum = query_value( '" << c_param_chksum << "' ); ";

            os << "query_update( '" << c_param_chksum << "', '" << new_checksum_value << "', true ); ";
         }

         os << "dyn_load( null, 'act=" << action;

         if( needs_editing )
            os << "&chk=" << key_and_version;
         else
            os << "&app=" << key_and_version << "&cls=" << class_id
             << "&extra=' + get_all_field_values( document." << src.id << " ) + '";

         // NOTE: Although the "auto_back" sessionStorage item is what is necessary to actually go
         // back this URL query item is still being used in order for the "record not found" error
         // for the view to be omitted (providing a cleaner UI for a Delete action in the view).
         if( go_back )
            os << "&back=" << c_true;

         if( !listarg.empty( ) )
            os << "&listarg=" << listarg;

         os << "&exec=" << next_action << "', false );";

         if( use_url_checksum )
            os << " query_update( '" << c_param_chksum << "', old_checksum, true );";

         if( go_back )
            os << " sessionStorage.setItem( 'auto_back', '" << to_string( back_count ) << "' );";
      }
      else
      {
         os << "window.location.search = 'cmd=" << c_cmd_view << "&data=" << record_key << clone_key;

         if( record_key != string( c_new_record ) )
            os << "&act=edit";

         if( mod_info.view_cids.find( child_class ) == mod_info.view_cids.end( ) )
            throw runtime_error( "unexpected missing view id for class: " + child_class );

         os << "&extra=" << child_key << "&field=" << child_field
          << "&ident=" << mod_info.view_cids.find( child_class )->second;

         if( !listarg.empty( ) )
            os << "&listarg=" << listarg;

         if( !user_select_key.empty( ) )
            os << "&" << c_param_uselect << "=" << user_select_key;

         if( !using_session_cookie )
            os << "&session=" << session_id;

         if( use_url_checksum )
         {
            string checksum_values( string( c_cmd_view ) + record_key + clone_key
             + mod_info.view_cids.find( child_class )->second + user_select_key );

            if( record_key != string( c_new_record ) )
               checksum_values = "edit" + checksum_values;

            checksum_values += to_string( sess_info.checksum_serial );

            if( has_hashval )
               checksum_values += c_hash_suffix;

            os << "&" << c_param_chksum << "=" << get_checksum( sess_info, checksum_values );
         }

         os << "';";
      }

      if( needs_confirmation )
         os << " }";

      os << " }";

      os << "\" style=\"cursor:pointer\">";
   }
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
 const list_source& list, const string& fixed_parent_field,
 const string& fixed_parent_keyval, const map< string, string >& list_selections,
 const session_info& sess_info, string* p_set_field_values, size_t* p_parent_state )
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

            // NOTE: For optional selects "~" is being used to indicate null.
            string key_value( list_selections.find( name )->second );

            if( key_value == "~" )
               key_value.erase( );

            if( num_fixed_key_values > 1 )
               fixed_key_values += ",";

            fixed_key_values += key_value + suffix;
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
                  value = get_extkey( value.substr( pos + 1 ).c_str( ) );
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

      if( p_parent_state && restrict_extras.count( c_list_field_extra_pstate ) )
      {
         istringstream isstr( restrict_extras[ c_list_field_extra_pstate ] );

         uint64_t flag;
         isstr >> hex >> flag;

         if( !( *p_parent_state & flag ) )
            continue;
      }

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
                  value = get_extkey( value.substr( pos + 1 ).c_str( ) );
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
         bool reverse_unchecked = false;

         if( ( list.lici->second )->restricts[ i ].operations.count( c_operation_rchecked ) )
            reverse_checked = true;

         if( ( list.lici->second )->restricts[ i ].operations.count( c_operation_runchecked ) )
            reverse_unchecked = true;

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
               if( !reverse_unchecked )
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

            if( reverse_unchecked )
               is_reverse = !is_reverse;
         }
         else if( reverse_checked )
            is_reverse = !is_reverse;
      }
   }
}
