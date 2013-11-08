#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#include "tx_create.h"

#include "utilities.h"

//#define DEBUG
//#define TESTBED

using namespace std;

namespace
{

struct utxo_info
{
   string tx_id;
   int vout;
   string script;
};

const uint64_t c_min_fee = 10000;

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

void process_utxo_info( istream& is, multimap< uint64_t, utxo_info >& utxos )
{
   string str;
   utxo_info utxo;

   bool found = false;
   bool from_listunspent = false;

   while( getline( is, str ) )
   {
      string::size_type pos = str.find( "\"tx_hash\"" );
      if( pos != string::npos )
         from_listunspent = false;
      else
      {
         pos = str.find( "\"txid\"" );

         if( pos != string::npos )
         {
            from_listunspent = true;
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

                  // NOTE: The "tx_hash" from "blockchain.info" needs to be reversed (as hex bytes).
                  if( !from_listunspent )
                  {
                     string input( utxo.tx_id );
                     utxo.tx_id.erase( );

                     for( int i = input.length( ) - 1; i >= 0; i -= 2 )
                     {
                        if( i - 1 >= 0 )
                        {
                           utxo.tx_id += input[ i - 1 ];
                           utxo.tx_id += input[ i ];
                        }
                     }
                  }
               }
            }
         }

         if( !okay )
            throw runtime_error( "unexpected format for '" + str + "'" );
      }
      else if( found )
      {
         if( from_listunspent )
            pos = str.find( "\"vout\"" );
         else
            pos = str.find( "\"tx_output_n\"" );

         if( pos != string::npos )
         {
            bool okay = false;
            pos = str.find( ':' );
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
            if( from_listunspent )
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
               if( from_listunspent )
                  pos = str.find( "\"amount\"" );
               else
                  pos = str.find( "\"value\"" );

               if( pos != string::npos )
               {
                  bool okay = false;
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

                     utxos.insert( make_pair( value, utxo ) );
                  }

                  found = false;
               }
            }
         }
      }
   }
}

uint64_t utxos_balance( const string& file_name )
{
   uint64_t balance = 0;

   ifstream inpf( file_name.c_str( ) );
   if( !inpf )
      throw runtime_error( "unable to open '" + file_name + "' for input." );
   else
   {
      multimap< uint64_t, utxo_info > utxos;

      process_utxo_info( inpf, utxos );

      if( utxos.empty( ) )
         throw runtime_error( "no utxo information found" );

      for( multimap< uint64_t, utxo_info >::iterator i = utxos.begin( ); i!= utxos.end( ); ++i )
      {
         balance += i->first;
      }
   }

   return balance;
}

uint64_t select_utxos_for_transaction(
 const string& file_name, uint64_t amount, vector< utxo_info >& utxos_used, uint64_t& fee )
{
   uint64_t change = 0;

   ifstream inpf( file_name.c_str( ) );
   if( !inpf )
      throw runtime_error( "unable to open '" + file_name + "' for input." );
   else
   {
      uint64_t total = 0;

      multimap< uint64_t, utxo_info > utxos;

      process_utxo_info( inpf, utxos );

      if( utxos.empty( ) )
         throw runtime_error( "no utxo information found" );

      bool done = false;
      for( multimap< uint64_t, utxo_info >::iterator i = utxos.begin( ); i!= utxos.end( ); ++i )
      {
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
         multimap< uint64_t, utxo_info >::iterator i = utxos.end( );
         while( true )
         {
            --i;
            if( i->first > fee )
            {
               total += i->first;

               // NOTE: Increment the fee by the minimum amount for every 5 UTXO's.
               // FUTURE: The fee should be calculated more accurately and take "age" into account.
               if( ++num % 5 == 0 )
                  fee += c_min_fee;

               utxos_used.push_back( i->second );
#ifdef DEBUG
               cout << ( i->second ).tx_id << ':' << ( i->second ).vout << ' ' << ( i->second ).script << ' ' << i->first << '\n';
#endif

               if( total > amount + fee )
               {
                  change = total - amount - fee;
                  break;
               }
            }

            if( i == utxos.begin( ) )
               break;
         }
      }

      // NOTE: Rather than accumulate "dust" UTXOs just add any tiny change amount to the fee.
      if( change < c_min_fee )
      {
         fee += change;
         change = 0;
      }

      if( total != ( amount + fee + change ) )
         throw runtime_error( "insufficient funds available" );

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

void get_utxo_information( const string& source_address, const string& file_name )
{
   // FUTURE: The UTXO list should also be able to be obtained via bitcoind locally using "listunspent".
   string cmd( "curl -s http://blockchain.info/unspent?address=" + source_address + " >" + file_name );

   system( cmd.c_str( ) );
}

uint64_t get_utxos_balance_amt( const string& file_name )
{
   return utxos_balance( file_name );
}

string create_raw_transaction( const string& source_address,
 const string& destination_address, uint64_t amount, quote_style qs,
 uint64_t& fee, string& sign_tx_template, const string* p_file_name )
{
   string raw_transaction;
   vector< utxo_info > utxos_used;

   uint64_t change = 0;

   if( p_file_name && !p_file_name->empty( ) )
      change = select_utxos_for_transaction( *p_file_name, amount, utxos_used, fee );
   else
   {
      string file_name( "~" + source_address + ".txt" );

      get_utxo_information( source_address, file_name );

      change = select_utxos_for_transaction( file_name, amount, utxos_used, fee );

#ifndef TESTBED
      file_remove( file_name );
#else
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

   for( size_t i = 0; i < utxos_used.size( ); i++ )
   {
      if( i > 0 )
      {
         raw_transaction += ',';
         sign_tx_template += ',';
      }

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
   add_json_pair( raw_transaction, destination_address, to_string( d ), quote1, false );

   // NOTE: If there is change then send it back to the source address.
   if( change > 0 )
   {
      raw_transaction += ',';

      double d = ( change / 100000000.0 );
      add_json_pair( raw_transaction, source_address, to_string( d ), quote1, false );
   }

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
         cout << "usage: tx_create <source_address> <destination_address> <amount_in_satoshis> [<min_fee_in_satoshis>]" << endl;
      else
      {
         string source_address( argv[ 1 ] );
         string destination_address( argv[ 2 ] );
         string amount_in_satoshis( argv[ 3 ] );

         string min_fee_in_satoshis;
         if( argc > 4 )
            min_fee_in_satoshis = string( argv[ 4 ] );

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
         string create_raw_tx( create_raw_transaction( source_address, destination_address, amount, qs, min_fee, sign_tx_template ) );

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

