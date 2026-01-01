// Copyright (c) 2004-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <string>
#  include <vector>
#  include <iostream>
#  include <stdexcept>
#endif

#include "format.h"
#include "macros.h"
#include "numeric.h"
#include "utilities.h"
#include "console_commands.h"

using namespace std;

#include "test_numeric.cmh"

const char* const c_app_title = "test_numeric";
const char* const c_app_version = "0.1";

const char* const c_error_prefix = "error: ";

bool g_application_title_called = false;

string application_title( app_info_request request )
{
   g_application_title_called = true;

   if( request == e_app_info_request_title )
      return string( c_app_title );
   else if( request == e_app_info_request_version )
      return string( c_app_version );
   else if( request == e_app_info_request_title_and_version )
   {
      string title( c_app_title );
      title += " v";
      title += string( c_app_version );

      return title;
   }
   else
   {
      ostringstream osstr;
      osstr << "unknown app_info_request: " << request;
      throw runtime_error( osstr.str( ) );
   }
}

class test_numeric_command_functor;

class test_numeric_command_handler : public console_command_handler
{
   friend class test_numeric_command_functor;

   public:
   test_numeric_command_handler( )
   {
   }

   private:
   numeric num;
};

class test_numeric_command_functor : public command_functor
{
   public:
   test_numeric_command_functor( test_numeric_command_handler& numeric_test_handler )
    : command_functor( numeric_test_handler ),
    num( numeric_test_handler.num )
   {
   }

   void operator ( )( const string& command, const parameter_info& parameters );

   private:
   numeric& num;
};

void test_numeric_command_functor::operator ( )( const string& command, const parameter_info& parameters )
{
   try
   {
      if( command == c_cmd_test_numeric_inc )
         handler.issue_command_response( to_string( ++num ) );
      else if( command == c_cmd_test_numeric_dec )
         handler.issue_command_response( to_string( --num ) );
      else if( command == c_cmd_test_numeric_min )
         handler.issue_command_response( to_string( numeric::min( ) ) );
      else if( command == c_cmd_test_numeric_max )
         handler.issue_command_response( to_string( numeric::max( ) ) );
      else if( command == c_cmd_test_numeric_e )
         handler.issue_command_response( to_string( numeric::e( ) ) );
      else if( command == c_cmd_test_numeric_pi )
         handler.issue_command_response( to_string( numeric::pi( ) ) );
      else if( command == c_cmd_test_numeric_phi )
         handler.issue_command_response( to_string( numeric::phi( ) ) );
      else if( command == c_cmd_test_numeric_abs )
         handler.issue_command_response( to_string( abs( num ) ) );
      else if( command == c_cmd_test_numeric_inv )
         handler.issue_command_response( to_string( 1 / num ) );
      else if( command == c_cmd_test_numeric_get )
         handler.issue_command_response( to_string( num ) );
      else if( command == c_cmd_test_numeric_cmp )
         handler.issue_command_response( num.as_comparable_string( ) );
      else if( command == c_cmd_test_numeric_set )
      {
         numeric n( get_parm_val( parameters, c_cmd_test_numeric_set_num ).c_str( ) );

         num = n;
         handler.issue_command_response( to_string( num ) );
      }
      else if( command == c_cmd_test_numeric_add )
      {
         numeric n( get_parm_val( parameters, c_cmd_test_numeric_add_num ).c_str( ) );

         num += n;
         handler.issue_command_response( to_string( num ) );
      }
      else if( command == c_cmd_test_numeric_sub )
      {
         numeric n( get_parm_val( parameters, c_cmd_test_numeric_sub_num ).c_str( ) );

         num -= n;
         handler.issue_command_response( to_string( num ) );
      }
      else if( command == c_cmd_test_numeric_mul )
      {
         numeric n( get_parm_val( parameters, c_cmd_test_numeric_mul_num ).c_str( ) );

         num *= n;
         handler.issue_command_response( to_string( num ) );
      }
      else if( command == c_cmd_test_numeric_div )
      {
         numeric n( get_parm_val( parameters, c_cmd_test_numeric_div_num ).c_str( ) );

         num /= n;
         handler.issue_command_response( to_string( num ) );
      }
      else if( command == c_cmd_test_numeric_sqrt )
         handler.issue_command_response( to_string( sqrt( num ) ) );
      else if( command == c_cmd_test_numeric_is_lt )
      {
         numeric n( get_parm_val( parameters, c_cmd_test_numeric_is_lt_num ).c_str( ) );

         bool is_true = num < n;
         handler.issue_command_response( is_true ? "true" : "false" );
      }
      else if( command == c_cmd_test_numeric_is_gt )
      {
         numeric n( get_parm_val( parameters, c_cmd_test_numeric_is_gt_num ).c_str( ) );

         bool is_true = num > n;
         handler.issue_command_response( is_true ? "true" : "false" );
      }
      else if( command == c_cmd_test_numeric_is_eq )
      {
         numeric n( get_parm_val( parameters, c_cmd_test_numeric_is_eq_num ).c_str( ) );

         bool is_true = num == n;
         handler.issue_command_response( is_true ? "true" : "false" );
      }
      else if( command == c_cmd_test_numeric_is_lteq )
      {
         numeric n( get_parm_val( parameters, c_cmd_test_numeric_is_lteq_num ).c_str( ) );

         bool is_true = num <= n;
         handler.issue_command_response( is_true ? "true" : "false" );
      }
      else if( command == c_cmd_test_numeric_is_gteq )
      {
         numeric n( get_parm_val( parameters, c_cmd_test_numeric_is_gteq_num ).c_str( ) );

         bool is_true = num >= n;
         handler.issue_command_response( is_true ? "true" : "false" );
      }
      else if( command == c_cmd_test_numeric_frac )
         handler.issue_command_response( to_string( num.frac( ) ) );
      else if( command == c_cmd_test_numeric_trunc )
         handler.issue_command_response( to_string( num.trunc( ) ) );
      else if( command == c_cmd_test_numeric_round )
      {
         numeric::round_method m( numeric::e_round_method_normal );
         if( has_parm_val( parameters, c_cmd_test_numeric_round_up ) )
            m = numeric::e_round_method_up;
         else if( has_parm_val( parameters, c_cmd_test_numeric_round_down ) )
            m = numeric::e_round_method_down;
         else if( has_parm_val( parameters, c_cmd_test_numeric_round_normal ) )
            m = numeric::e_round_method_normal;
         else if( has_parm_val( parameters, c_cmd_test_numeric_round_bankers ) )
            m = numeric::e_round_method_bankers;

         bool simplify = has_parm_val( parameters, c_cmd_test_numeric_round_simplify );

         int n = atoi( get_parm_val( parameters, c_cmd_test_numeric_round_decimals ).c_str( ) );
         try
         {
            num.round( n, m, simplify );
            handler.issue_command_response( to_string( num ) );
         }
         catch( exception& x )
         {
            handler.issue_command_response( string( c_error_prefix ) + x.what( ), true );
         }
      }
      else if( command == c_cmd_test_numeric_round_table )
      {
         //__Ignore_Block
         double values[ ] = { -2.6, -2.5, -2.4, -1.6, -1.5, -1.4,
          -0.6, -0.5, -0.4, 0.4, 0.5, 0.6, 1.4, 1.5, 1.6, 2.4, 2.5, 2.6 };

         vector< string > lines;
         lines.resize( ARRAY_SIZE( values ) );

         for( size_t i = 0; i < ARRAY_SIZE( values ); i++ )
         {
            if( values[ i ] >= 0 )
               lines[ i ] = ' ';

            ostringstream oss;
            oss << ffmt( 1, 1 ) << values[ i ];

            lines[ i ] += oss.str( );

            for( int m = 0; m < 4; m++ )
            {
               int indent = 0;

               numeric n( values[ i ] );

               switch( m )
               {
                  case 0:
                  n.round( 0, numeric::e_round_method_up );

                  indent = ( 5 - lines[ i ].length( ) );

                  if( indent > 0 )
                     lines[ i ] += string( indent, ' ' );

                  if( n >= 0.0 )
                     lines[ i ] += ' ';

                  lines[ i ] += to_string( n );
                  break;

                  case 1:
                  n.round( 0, numeric::e_round_method_down );

                  indent = ( 10 - lines[ i ].length( ) );

                  if( indent > 0 )
                     lines[ i ] += string( indent, ' ' );

                  if( n >= 0.0 )
                     lines[ i ] += ' ';

                  lines[ i ] += to_string( n );
                  break;

                  case 2:
                  n.round( 0, numeric::e_round_method_normal );

                  indent = ( 17 - lines[ i ].length( ) );

                  if( indent > 0 )
                     lines[ i ] += string( indent, ' ' );

                  if( n >= 0.0 )
                     lines[ i ] += ' ';

                  lines[ i ] += to_string( n );
                  break;

                  case 3:
                  n.round( 0, numeric::e_round_method_bankers );

                  indent = ( 25 - lines[ i ].length( ) );

                  if( indent > 0 )
                     lines[ i ] += string( indent, ' ' );

                  if( n >= 0.0 )
                     lines[ i ] += ' ';

                  lines[ i ] += to_string( n );
                  break;
               }
            }
         }

         handler.issue_command_response( "" );
         handler.issue_command_response( "None Up Down Normal Bankers" );
         handler.issue_command_response( "---------------------------" );

         for( size_t i = 0; i < ARRAY_SIZE( values ); i++ )
            handler.issue_command_response( lines[ i ] );

         handler.issue_command_response( "" );
      }
      else if( command == c_cmd_test_numeric_digits )
         handler.issue_command_response( to_string( ( int )num.digits( ) ) );
      else if( command == c_cmd_test_numeric_whole_digits )
         handler.issue_command_response( to_string( ( int )num.whole_digits( ) ) );
      else if( command == c_cmd_test_numeric_decimal_digits )
         handler.issue_command_response( to_string( ( int )num.decimal_digits( ) ) );
      else if( command == c_cmd_test_numeric_format )
      {
         string mask( get_parm_val( parameters, c_cmd_test_numeric_format_mask ) );

         handler.issue_command_response( format_numeric( num, mask ) );
      }
      else if( command == c_cmd_test_numeric_exit )
         handler.set_finished( );
   }
   catch( exception& x )
   {
      handler.issue_command_response( string( c_error_prefix ) + x.what( ), true );
   }
}

command_functor* test_numeric_command_functor_factory( const string& /*name*/, command_handler& handler )
{
   return new test_numeric_command_functor( dynamic_cast< test_numeric_command_handler& >( handler ) );
}

int main( int argc, char* argv[ ] )
{
   test_numeric_command_handler cmd_handler;

   try
   {
      // NOTE: Use block scope for startup command processor object...
      {
         startup_command_processor processor( cmd_handler, application_title, 0, argc, argv );

         processor.process_commands( );
      }

      if( !cmd_handler.has_option_quiet( ) )
         cout << application_title( e_app_info_request_title_and_version ) << endl;

      cmd_handler.add_commands( 0,
       test_numeric_command_functor_factory, ARRAY_PTR_AND_SIZE( test_numeric_command_definitions ) );

      console_command_processor processor( cmd_handler );
      processor.process_commands( );
   }
   catch( exception& x )
   {
      cerr << "error: " << x.what( ) << endl;
      return 1;
   }
   catch( ... )
   {
      cerr << "error: unexpected exception occurred" << endl;
      return 2;
   }
}

