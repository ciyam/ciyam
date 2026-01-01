// Copyright (c) 2013-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <map>
#  include <fstream>
#  include <iostream>
#  include <algorithm>
#  include <stdexcept>
#endif

#include "tx_create.h"

#include "utilities.h"

//#define DEBUG
//#define TESTBED

using namespace std;

namespace
{

const uint64_t c_min_fee = 10000;
const uint64_t c_dust_amount = 1000;

void add_json_pair( string& s, const string& name, const string& value, const string& quote, bool quote_value )
{
   s += quote;
   s += name;
   s += quote;
   s += ':';

   if( quote_value )
      s += quote;
   s += value;
   if( quote_value )
      s += quote;
}

uint64_t utxos_balance( const string& file_name )
{
   uint64_t balance = 0;

   ifstream inpf( file_name.c_str( ) );
   if( !inpf )
      throw runtime_error( "unable to open '" + file_name + "' for input." );
   else
   {
      vector< utxo_info > utxos;
      parse_utxo_info( inpf, utxos );

      if( utxos.empty( ) )
         throw runtime_error( "no utxo information found" );

      for( size_t i = 0; i < utxos.size( ); i++ )
      {
         if( utxos[ i ].amount > c_dust_amount )
            balance += utxos[ i ].amount;
      }
   }

   return balance;
}

uint64_t select_utxos_for_transaction(
 const string& file_name, uint64_t amount, vector< utxo_info >& utxos_used, uint64_t& fee )
{
   uint64_t change = 0;
   uint64_t original_fee( fee );

   ifstream inpf( file_name.c_str( ) );
   if( !inpf )
      throw runtime_error( "unable to open '" + file_name + "' for input." );
   else
   {
      uint64_t total = 0;

      vector< utxo_info > utxos;
      parse_utxo_info( inpf, utxos );

      if( utxos.empty( ) )
         throw runtime_error( "no utxo information found" );

      multimap< uint64_t, utxo_info > mapped_utxos;
      for( size_t i = 0; i < utxos.size( ); i++ )
         mapped_utxos.insert( make_pair( utxos[ i ].amount, utxos[ i ] ) );

      bool done = false;
      for( multimap< uint64_t, utxo_info >::iterator i = mapped_utxos.begin( ); i != mapped_utxos.end( ); ++i )
      {
         if( i->first <= c_dust_amount )
            continue;

         // NOTE: Prefer a single UTXO rather than multiple in order to keep the tx size to a minimum.
         if( i->first >= amount + fee )
         {
            done = true;
            total += i->first;
            change = total - amount - fee;

            utxos_used.push_back( i->second );

#ifdef DEBUG
            cout << ( i->second ).tx_id << ':' << ( i->second ).vout << ' ' << ( i->second ).script << ' ' << i->first << '\n';
#endif
            break;
         }
      }

      if( !done )
      {
         int num = 0;
         multimap< uint64_t, utxo_info >::iterator i = mapped_utxos.end( );
         while( true )
         {
            --i;
            if( i->first >= fee )
            {
               total += i->first;

               // NOTE: Increment the fee by the minimum amount for every 10 UTXO's.
               // FUTURE: The fee should be calculated more accurately and take "age" into account.
               if( ++num % 10 == 0 )
                  fee += c_min_fee;

               utxos_used.push_back( i->second );
#ifdef DEBUG
               cout << ( i->second ).tx_id << ':' << ( i->second ).vout << ' ' << ( i->second ).script << ' ' << i->first << '\n';
#endif

               if( total >= amount + fee )
               {
                  change = total - amount - fee;
                  break;
               }
            }

            if( i == mapped_utxos.begin( ) )
               break;
         }
      }

      // NOTE: Rather than accumulate "dust" UTXOs just add any tiny change amount to the fee.
      if( change < c_min_fee )
      {
         fee += change;
         change = 0;
      }

      // FUTURE: These error messages should be module strings.
      if( total != ( amount + fee + change ) )
      {
         if( fee != original_fee )
            throw runtime_error( "fee should be at least: " + to_string( fee ) );
         else
            throw runtime_error( "insufficient funds available" );
      }

#ifdef DEBUG
      cout << "amount: " << amount << endl;
      cout << "fee: " << fee << endl;
      cout << "change: " << change << endl;
      cout << "total: " << total << endl;
      cout << "added: " << ( amount + fee + change ) << endl;
#endif
   }

   return change;
}

}

#ifdef TESTBED
void get_utxo_information( const string& source_addresses, const string& file_name )
{
   string pipe_separated( source_addresses );
   for( size_t i = 0; i < pipe_separated.size( ); i++ )
   {
      if( pipe_separated[ i ] == ' ' || pipe_separated[ i ] == ',' )
         pipe_separated[ i ] = '|';
   }

   string cmd( "curl -s \"https://blockchain.info/unspent?active=" + pipe_separated + "\" >" + file_name );

   int rc = system( cmd.c_str( ) );
   ( void )rc;
}
#endif

void parse_utxo_info( istream& is, vector< utxo_info >& utxos )
{
   string str;
   utxo_info utxo;

   bool found = false;
   bool was_skipped = false;
   bool from_standard = false;

   while( getline( is, str ) )
   {
      string::size_type pos = str.find( "\"tx_hash\"" );
      if( pos != string::npos )
      {
         was_skipped = false;
         from_standard = false;
      }
      else
      {
         pos = str.find( "\"txid\"" );

         if( pos != string::npos )
         {
            was_skipped = false;
            from_standard = true;
            pos = str.find( "txid" );
         }
      }

      if( pos != string::npos )
      {
         found = true;
         bool okay = false;

         pos = str.find( ':' );
         if( pos != string::npos )
         {
            pos = str.find( "\"", pos + 1 );
            if( pos != string::npos )
            {
               string::size_type epos = str.find( "\"", pos + 1 );
               if( epos != string::npos )
               {
                  okay = true;
                  utxo.tx_id = str.substr( pos + 1, epos - pos - 1 );

                  string input( utxo.tx_id );
                  utxo.tx_id_rev.clear( );

                  utxo.tx_id_rev = hex_reverse( input );

                  // NOTE: The "tx_hash" from "blockchain.info" is already in reverse order.
                  if( !from_standard )
                     swap( utxo.tx_id, utxo.tx_id_rev );
               }
            }
         }

         if( !okay )
            throw runtime_error( "unexpected format for '" + str + "'" );
      }
      else if( found )
      {
         if( from_standard )
            pos = str.find( "\"vout\"" );
         else
            pos = str.find( "\"tx_output_n\"" );

         if( pos != string::npos && pos != str.length( ) - 1 )
         {
            bool okay = false;
            pos = str.find( ':', pos + 1 );
            if( pos != string::npos )
            {
               okay = true;
               utxo.vout = atoi( str.substr( pos + 1 ).c_str( ) );
            }

            if( !okay )
               throw runtime_error( "unexpected format for '" + str + "'" );
         }
         else
         {
            if( from_standard )
            {
               pos = str.find( "\"account\"" );
               if( pos != string::npos )
               {
                  pos = str.find( ':', pos + 1 );
                  if( pos != string::npos )
                  {
                     pos = str.find( "\"", pos + 1 );
                     if( pos != string::npos )
                     {
                        string::size_type epos = str.find( "\"", pos + 1 );
                        if( epos != string::npos )
                           utxo.account = str.substr( pos + 1, epos - pos - 1 );
                     }
                  }
               }

               pos = str.find( "\"address\"" );
               if( pos != string::npos )
               {
                  pos = str.find( ':', pos + 1 );
                  if( pos != string::npos )
                  {
                     pos = str.find( "\"", pos + 1 );
                     if( pos != string::npos )
                     {
                        string::size_type epos = str.find( "\"", pos + 1 );
                        if( epos != string::npos )
                           utxo.address = str.substr( pos + 1, epos - pos - 1 );
                     }
                  }
               }
            }

            if( from_standard )
               pos = str.find( "\"scriptPubKey\"" );
            else
               pos = str.find( "\"script\"" );

            if( pos != string::npos )
            {
               bool okay = false;
               pos = str.find( ':' );
               if( pos != string::npos )
               {
                  pos = str.find( "\"", pos + 1 );
                  if( pos != string::npos )
                  {
                     string::size_type epos = str.find( "\"", pos + 1 );
                     if( epos != string::npos )
                     {
                        okay = true;
                        utxo.script = str.substr( pos + 1, epos - pos - 1 );
                     }
                  }
               }

               if( !okay )
                  throw runtime_error( "unexpected format for '" + str + "'" );
            }
            else
            {
               if( from_standard )
               {
                  pos = str.find( "\"privKeySecret\"" );

                  if( pos != string::npos )
                  {
                     pos = str.find( ':' );
                     if( pos != string::npos )
                     {
                        pos = str.find( "\"", pos + 1 );
                        if( pos != string::npos )
                        {
                           string::size_type epos = str.find( "\"", pos + 1 );
                           if( epos != string::npos )
                              utxo.secret = str.substr( pos + 1, epos - pos - 1 );
                        }
                     }
                  }
               }

               if( from_standard )
                  pos = str.find( "\"amount\"" );
               else
                  pos = str.find( "\"value\"" );

               if( pos != string::npos )
               {
                  pos = str.find( ':' );
                  if( pos != string::npos )
                  {
                     uint64_t value = 0;
                     for( size_t i = pos + 1; i < str.length( ); i++ )
                     {
                        if( str[ i ] >= '0' && str[ i ] <= '9' )
                        {
                           value *= 10;
                           value += str[ i ] - '0';
                        }
                     }

                     utxo.amount = value;
                  }
                  else
                     throw runtime_error( "unexpected format for '" + str + "'" );
               }
               else
               {
                  pos = str.find( "\"confirmations\"" );

                  if( pos != string::npos )
                  {
                     pos = str.find( ':' );
                     if( pos != string::npos )
                     {
                        uint64_t value = 0;
                        for( size_t i = pos + 1; i < str.length( ); i++ )
                        {
                           if( str[ i ] >= '0' && str[ i ] <= '9' )
                           {
                              value *= 10;
                              value += str[ i ] - '0';
                           }
                        }

                        utxo.confirmations = value;

                        found = false;

                        // NOTE: Don't bother including UTXOs with amounts
                        // that are smaller than the minimum fee amount.
                        if( utxo.amount >= c_min_fee )
                           utxos.push_back( utxo );
                        else
                           was_skipped = true;

                        utxo.clear( );
                     }
                     else
                        throw runtime_error( "unexpected format for '" + str + "'" );
                  }
               }
            }
         }
      }
      else if( !was_skipped && !utxos.empty( ) )
      {
         pos = str.find( "\"spendable\"" );

         if( pos != string::npos )
         {
            pos = str.find( ':', pos );
            if( pos != string::npos )
            {
               if( str.find( "false", pos + 1 ) != string::npos )
                  utxos.back( ).spendable = false;
            }
         }
      }
   }
}

uint64_t get_utxos_balance_amt( const string& file_name )
{
   return utxos_balance( file_name );
}

string create_raw_transaction_command( const string& source_addresses,
 const string& destination_addresses, string& change_address, uint64_t amount,
 quote_style qs, uint64_t& fee, string& sign_tx_template, const string* p_file_name )
{
   string raw_transaction;
   vector< utxo_info > utxos_used;

   uint64_t change = 0;

   if( p_file_name && !p_file_name->empty( ) )
      change = select_utxos_for_transaction( *p_file_name, amount, utxos_used, fee );
   else
   {
#ifndef TESTBED
      throw runtime_error( "unexpected missing utxo information" );
#else
      string file_name( "~tx_create.txt" );

      get_utxo_information( source_addresses, file_name );

      change = select_utxos_for_transaction( file_name, amount, utxos_used, fee );

      _unlink( file_name.c_str( ) );
#endif
   }

   raw_transaction = "createrawtransaction ";
   sign_tx_template = "signrawtransaction @rawtx ";

   if( qs == e_quote_style_both_windows )
   {
      raw_transaction += '"';
      sign_tx_template += '"';
   }
   else if( qs == e_quote_style_both_linux )
   {
      raw_transaction += "'";
      sign_tx_template += "'";
   }
   else
   {
      raw_transaction += '"';
      sign_tx_template += "'";
   }

   raw_transaction += '[';
   sign_tx_template += '[';

   string quote1, quote2;
   if( qs == e_quote_style_both_linux )
   {
      quote1 = '"';
      quote2 = '"';
   }
   else if( qs == e_quote_style_both_windows )
   {
      quote1 = "\\\"";
      quote2 = "\\\"";
   }
   else
   {
      quote1 = "\\\"";
      quote2 = '"';
   }

   if( change_address.empty( ) && source_addresses != "*" )
   {
      change_address = source_addresses;
      string::size_type pos = change_address.find_first_of( " ,|" );
      if( pos != string::npos )
         change_address.erase( pos );
   }

   for( size_t i = 0; i < utxos_used.size( ); i++ )
   {
      if( i > 0 )
      {
         raw_transaction += ',';
         sign_tx_template += ',';
      }

      if( change_address.empty( ) )
         change_address = utxos_used[ i ].address;

      raw_transaction += '{';
      sign_tx_template += '{';

      add_json_pair( raw_transaction, "txid", utxos_used[ i ].tx_id, quote1, true );
      add_json_pair( sign_tx_template, "txid", utxos_used[ i ].tx_id, quote2, true );

      raw_transaction += ',';
      sign_tx_template += ',';

      add_json_pair( raw_transaction, "vout", to_string( utxos_used[ i ].vout ), quote1, false );
      add_json_pair( sign_tx_template, "vout", to_string( utxos_used[ i ].vout ), quote2, false );

      sign_tx_template += ',';
      add_json_pair( sign_tx_template, "scriptPubKey", utxos_used[ i ].script, quote2, true );

      raw_transaction += '}';
      sign_tx_template += '}';
   }

   raw_transaction += ']';
   sign_tx_template += ']';

   if( qs != e_quote_style_both_linux )
      raw_transaction += '"';
   else
      raw_transaction += "'";

   raw_transaction += ' ';
   if( qs != e_quote_style_both_linux )
      raw_transaction += '"';
   else
      raw_transaction += "'";

   raw_transaction += '{';

   double d = ( amount / 100000000.0 );

   int num_destinations = 1;
   string comma_separated( destination_addresses );
   for( size_t i = 0; i < comma_separated.size( ); i++ )
   {
      if( comma_separated[ i ] == ' ' || comma_separated[ i ] == '|' )
         comma_separated[ i ] = ',';

      if( comma_separated[ i ] == ',' )
         ++num_destinations;
   }

   d /= num_destinations;

   bool is_first = true;

   while( true )
   {
      if( is_first )
         is_first = false;
      else
         raw_transaction += ',';

      string::size_type pos = comma_separated.find( ',' );
      string destination_address( comma_separated.substr( 0, pos ) );

      add_json_pair( raw_transaction, destination_address, to_string( d ), quote1, false );

      if( pos == string::npos )
         break;

      comma_separated.erase( 0, pos + 1 );
   }

   // NOTE: If there is change then send it back to the change address.
   if( change > 0 )
   {
      raw_transaction += ',';

      double d = ( change / 100000000.0 );
      add_json_pair( raw_transaction, change_address, to_string( d ), quote1, false );
   }
   else
      change_address.erase( );

   raw_transaction += "}";

   if( qs != e_quote_style_both_linux )
      raw_transaction += '"';
   else
      raw_transaction += "'";

   if( qs == e_quote_style_both_windows )
      sign_tx_template += '"';
   else
      sign_tx_template += "'";

   return raw_transaction;
}

#ifdef TESTBED
int main( int argc, char* argv[ ] )
{
   try
   {
      if( argc < 4 )
         cout << "usage: tx_create <source_addresses> <destination_addresses> <amount_in_satoshis> [<change_address> [<min_fee_in_satoshis>]]" << endl;
      else
      {
         string source_addresses( argv[ 1 ] );
         string destination_addresses( argv[ 2 ] );
         string amount_in_satoshis( argv[ 3 ] );

         string change_address;
         if( argc > 4 )
            change_address = string( argv[ 4 ] );

         string min_fee_in_satoshis;
         if( argc > 5 )
            min_fee_in_satoshis = string( argv[ 5 ] );

         uint64_t amount = 0;

         for( size_t i = 0; i < amount_in_satoshis.size( ); i++ )
         {
            if( amount_in_satoshis[ i ] >= '0' && amount_in_satoshis[ i ] <= '9' )
            {
               amount *= 10;
               amount += amount_in_satoshis[ i ] - '0';
            }
         }

         uint64_t min_fee = c_min_fee;
         if( !min_fee_in_satoshis.empty( ) )
            min_fee = 0;

         for( size_t i = 0; i < min_fee_in_satoshis.size( ); i++ )
         {
            if( min_fee_in_satoshis[ i ] >= '0' && min_fee_in_satoshis[ i ] <= '9' )
            {
               min_fee *= 10;
               min_fee += min_fee_in_satoshis[ i ] - '0';
            }
         }

         quote_style qs( e_quote_style_both_windows );

         string sign_tx_template;

         string create_raw_tx( create_raw_transaction_command(
          source_addresses, destination_addresses, change_address, amount, qs, min_fee, sign_tx_template ) );

         cout << create_raw_tx << "\n\n";
         cout << sign_tx_template << endl;
      }
   }
   catch( exception& x )
   {
      cerr << "error: " << x.what( ) << endl;
      return 1;
   }
   catch( ... )
   {
      cerr << "error: unexpected exception caught" << endl;
      return 2;
   }

   return 0;
}
#endif

