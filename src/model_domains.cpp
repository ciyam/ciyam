// Copyright (c) 2006-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#include "model_domains.h"

#include "sio.h"
#include "model.h"
#include "utilities.h"
#include "date_time.h"

using namespace std;

namespace
{

const char* const c_attribute_date_range_var_minimum = "minimum";
const char* const c_attribute_date_range_var_maximum = "maximum";

const char* const c_attribute_datetime_range_var_minimum = "minimum";
const char* const c_attribute_datetime_range_var_maximum = "maximum";

const char* const c_attribute_int_range_var_minimum = "minimum";
const char* const c_attribute_int_range_var_maximum = "maximum";

const char* const c_attribute_numeric_range_var_minimum_whole = "minimum_whole";
const char* const c_attribute_numeric_range_var_minimum_fraction = "minimum_fraction";
const char* const c_attribute_numeric_range_var_maximum_whole = "maximum_whole";
const char* const c_attribute_numeric_range_var_maximum_fraction = "maximum_fraction";

const char* const c_attribute_numeric_format_var_maximum_digits = "maximum_digits";
const char* const c_attribute_numeric_format_var_maximum_decimals = "maximum_decimals";

const char* const c_attribute_string_set_value = "value";

const char* const c_attribute_string_max_size_var_max_size = "max_size";

const char* const c_attribute_string_format_var_type = "type";

int get_numeric_arg_value( const std::string& arg, const std::string& arg_name, bool allow_negative = false )
{
   int val = 0;
   bool is_negative = false;

   if( arg.empty( ) )
      throw runtime_error( "invalid empty value provided for numeric arg '" + arg_name + "'" );

   for( size_t i = 0; i < arg.size( ); i++ )
   {
      if( i == 0 && allow_negative && arg[ 0 ] == '-' )
      {
         is_negative = true;
         if( arg.size( ) > 1 )
            continue;
         else
            throw runtime_error( "invalid value '" + arg + "' provided for numeric arg '" + arg_name + "'" );
      }
      else if( arg[ i ] < '0' || arg[ i ] > '9' )
         throw runtime_error( "unexpected character '" + string( 1, arg[ i ] ) + "' in " + arg_name + " argument '" + arg + "'" );

      val *= 10;
      val += arg[ i ] - '0';
   }

   if( is_negative )
      val *= -1;

   return val;
}

}

DEFINE_CLASS_FACTORY( std::string, domain );

struct date_range_domain : domain
{
   void add( const vector< string >& args );

   string get_extra( ) const;

   string get_type_name( ) const;

   string get_list_display( ) const;

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   udate minimum;
   udate maximum;

   static string static_class_name( );
};

void date_range_domain::add( const vector< string >& args )
{
   if( args.size( ) > 2 )
      throw runtime_error( "unexpected number of args > 2 for 'range' domain" );

   if( args.size( ) < 1 )
      throw runtime_error( "'minimum' argument is required for the 'range' domain" );

   if( args.size( ) < 2 )
      throw runtime_error( "'maximum' argument is required for the 'range' domain" );

   minimum = udate( args[ 0 ] );
   maximum = udate( args[ 1 ] );

   if( minimum >= maximum )
      throw runtime_error( "'minimum' argument must be less than the 'maximum' argument for the 'range' domain" );
}

string date_range_domain::get_extra( ) const
{
   string str( "range=" );
   str += minimum.as_string( );
   str += "..";
   str += maximum.as_string( );

   return str;
}

string date_range_domain::get_type_name( ) const
{
   string str( "domain_date_range< " );
   str += to_string( minimum.get_year( ) );
   str += ", " + to_string( ( int )minimum.get_month( ) );
   str += ", " + to_string( ( int )minimum.get_day( ) );
   str += ", " + to_string( maximum.get_year( ) );
   str += ", " + to_string( ( int )maximum.get_month( ) );
   str += ", " + to_string( ( int )maximum.get_day( ) );
   str += " >";

   return str;
}

string date_range_domain::get_list_display( ) const
{
   string str( "range: " );
   str += minimum.as_string( true );
   str += "..";
   str += maximum.as_string( true );

   return str;
}

void date_range_domain::read_data( sio_reader& reader )
{
   minimum = udate( reader.read_attribute( c_attribute_date_range_var_minimum ) );
   maximum = udate( reader.read_attribute( c_attribute_date_range_var_maximum ) );
}

void date_range_domain::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_date_range_var_minimum, minimum.as_string( ) );
   writer.write_attribute( c_attribute_date_range_var_maximum, maximum.as_string( ) );
}

string date_range_domain::static_class_name( ) { return "date_range"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, domain, date_range_domain, static_class_name );

struct datetime_range_domain : domain
{
   void add( const vector< string >& args );

   string get_extra( ) const;

   string get_type_name( ) const;

   string get_list_display( ) const;

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   date_time minimum;
   date_time maximum;

   static string static_class_name( );
};

void datetime_range_domain::add( const vector< string >& args )
{
   if( args.size( ) > 2 )
      throw runtime_error( "unexpected number of args > 2 for 'range' domain" );

   if( args.size( ) < 1 )
      throw runtime_error( "'minimum' argument is required for the 'range' domain" );

   if( args.size( ) < 2 )
      throw runtime_error( "'maximum' argument is required for the 'range' domain" );

   minimum = date_time( args[ 0 ] );
   maximum = date_time( args[ 1 ] );

   if( minimum >= maximum )
      throw runtime_error( "'minimum' argument must be less than the 'maximum' argument for the 'range' domain" );
}

string datetime_range_domain::get_extra( ) const
{
   string str( "range=" );
   str += minimum.as_string( );
   str += "..";
   str += maximum.as_string( );

   return str;
}

string datetime_range_domain::get_type_name( ) const
{
   string str( "domain_datetime_range< " );
   str += to_string( minimum.get_year( ) );
   str += ", " + to_string( ( int )minimum.get_month( ) );
   str += ", " + to_string( ( int )minimum.get_day( ) );
   str += ", " + to_string( ( int )minimum.get_hour( ) );
   str += ", " + to_string( ( int )minimum.get_minute( ) );
   str += ", " + to_string( ( int )minimum.get_second( ) );
   str += ", " + to_string( maximum.get_year( ) );
   str += ", " + to_string( ( int )maximum.get_month( ) );
   str += ", " + to_string( ( int )maximum.get_day( ) );
   str += ", " + to_string( ( int )maximum.get_hour( ) );
   str += ", " + to_string( ( int )maximum.get_minute( ) );
   str += ", " + to_string( ( int )maximum.get_second( ) );
   str += " >";

   return str;
}

string datetime_range_domain::get_list_display( ) const
{
   string str( "range: " );
   str += minimum.as_string( true );
   str += "..";
   str += maximum.as_string( true );

   return str;
}

void datetime_range_domain::read_data( sio_reader& reader )
{
   minimum = date_time( reader.read_attribute( c_attribute_datetime_range_var_minimum ) );
   maximum = date_time( reader.read_attribute( c_attribute_datetime_range_var_maximum ) );
}

void datetime_range_domain::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_datetime_range_var_minimum, minimum.as_string( ) );
   writer.write_attribute( c_attribute_datetime_range_var_maximum, maximum.as_string( ) );
}

string datetime_range_domain::static_class_name( ) { return "datetime_range"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, domain, datetime_range_domain, static_class_name );

struct int_range_domain : domain
{
   void add( const vector< string >& args );

   string get_extra( ) const;

   string get_type_name( ) const;

   string get_list_display( ) const;

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   int minimum;
   int maximum;

   static string static_class_name( );
};

void int_range_domain::add( const vector< string >& args )
{
   if( args.size( ) > 2 )
      throw runtime_error( "unexpected number of args > 2 for 'range' domain" );

   if( args.size( ) < 1 )
      throw runtime_error( "'minimum' argument is required for the 'range' domain" );

   if( args.size( ) < 2 )
      throw runtime_error( "'maximum' argument is required for the 'range' domain" );

   minimum = get_numeric_arg_value( args[ 0 ], "minimum", true );
   maximum = get_numeric_arg_value( args[ 1 ], "maximum", true );

   if( minimum >= maximum )
      throw runtime_error( "'minimum' argument must be less than the 'maximum' argument for the 'range' domain" );
}

string int_range_domain::get_extra( ) const
{
   string str( "range=" );
   str += to_string( minimum );
   str += "..";
   str += to_string( maximum );

   return str;
}

string int_range_domain::get_type_name( ) const
{
   string str( "domain_int_range< " );
   str += to_string( minimum );
   str += ", " + to_string( maximum );
   str += " >";

   return str;
}

string int_range_domain::get_list_display( ) const
{
   string str( "range: " );
   str += to_string( minimum );
   str += "..";
   str += to_string( maximum );

   return str;
}

void int_range_domain::read_data( sio_reader& reader )
{
   minimum = from_string< int >( reader.read_attribute( c_attribute_int_range_var_minimum ) );
   maximum = from_string< int >( reader.read_attribute( c_attribute_int_range_var_maximum ) );
}

void int_range_domain::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_int_range_var_minimum, to_string( minimum ) );
   writer.write_attribute( c_attribute_int_range_var_maximum, to_string( maximum ) );
}

string int_range_domain::static_class_name( ) { return "int_range"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, domain, int_range_domain, static_class_name );

struct numeric_format_domain : domain
{
   void add( const vector< string >& args );

   string get_extra( ) const;

   string get_type_name( ) const;

   string get_list_display( ) const;

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   int maximum_digits;
   int maximum_decimals;

   static string static_class_name( );
};

void numeric_format_domain::add( const vector< string >& args )
{
   if( args.size( ) > 2 )
      throw runtime_error( "unexpected number of args > 2 for 'format' domain" );

   if( args.size( ) < 1 )
      throw runtime_error( "'max. digits' argument is required for the 'format' domain" );

   if( args.size( ) < 2 )
      throw runtime_error( "'max. decimals' argument is required for the 'format' domain" );

   maximum_digits = get_numeric_arg_value( args[ 0 ], "max. digits" );
   maximum_decimals = get_numeric_arg_value( args[ 1 ], "max. decimals" );

   if( maximum_decimals > maximum_digits )
      throw runtime_error( "'max. decimals' argument must be less than the 'max. digits' argument for the 'format' domain" );
}

string numeric_format_domain::get_extra( ) const
{
   string str( "format=" );
   str += to_string( maximum_digits );
   str += ".";
   str += to_string( maximum_decimals );

   return str;
}

string numeric_format_domain::get_type_name( ) const
{
   string str( "domain_numeric_format< numeric, " );
   str += to_string( maximum_digits );
   str += ", " + to_string( maximum_decimals );
   str += " >";

   return str;
}

string numeric_format_domain::get_list_display( ) const
{
   string str( "format: " );
   str += to_string( maximum_digits );
   str += ":";
   str += to_string( maximum_decimals );

   return str;
}

void numeric_format_domain::read_data( sio_reader& reader )
{
   maximum_digits = from_string< int >( reader.read_attribute( c_attribute_numeric_format_var_maximum_digits ) );
   maximum_decimals = from_string< int >( reader.read_attribute( c_attribute_numeric_format_var_maximum_decimals ) );
}

void numeric_format_domain::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_numeric_format_var_maximum_digits, to_string( maximum_digits ) );
   writer.write_attribute( c_attribute_numeric_format_var_maximum_decimals, to_string( maximum_decimals ) );
}

string numeric_format_domain::static_class_name( ) { return "numeric_format"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, domain, numeric_format_domain, static_class_name );

struct numeric_range_domain : domain
{
   void add( const vector< string >& args );

   string get_extra( ) const;

   string get_type_name( ) const;

   string get_list_display( ) const;

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   int minimum_whole;
   int minimum_fraction;
   int maximum_whole;
   int maximum_fraction;

   static string static_class_name( );
};

void numeric_range_domain::add( const vector< string >& args )
{
   if( args.size( ) > 2 )
      throw runtime_error( "unexpected number of args > 2 for 'range' domain" );

   if( args.size( ) < 1 )
      throw runtime_error( "'minimum' argument is required for the 'range' domain" );

   if( args.size( ) < 2 )
      throw runtime_error( "'maximum' argument is required for the 'range' domain" );

   size_t pos = args[ 0 ].find( '.' );

   minimum_whole = get_numeric_arg_value( args[ 0 ].substr( 0, pos ), "minimum_whole", true );

   if( pos == string::npos )
      minimum_fraction = 0;
   else
      minimum_fraction = get_numeric_arg_value( args[ 0 ].substr( pos + 1 ), "minimum_fraction" );

   pos = args[ 1 ].find( '.' );

   maximum_whole = get_numeric_arg_value( args[ 1 ].substr( 0, pos ), "maximum_whole", true );

   if( pos == string::npos )
      maximum_fraction = 0;
   else
      maximum_fraction = get_numeric_arg_value( args[ 1 ].substr( pos + 1 ), "maximum_fraction" );

   if( minimum_whole > maximum_whole || ( minimum_whole == maximum_whole && minimum_fraction >= maximum_fraction ) )
      throw runtime_error( "'minimum' argument must be less than the 'maximum' argument for the 'range' domain" );
}

string numeric_range_domain::get_extra( ) const
{
   string str( "range=" );
   str += to_string( minimum_whole );
   str += ".";
   str += to_string( minimum_fraction );
   str += "..";
   str += to_string( maximum_whole );
   str += ".";
   str += to_string( maximum_fraction );

   return str;
}

string numeric_range_domain::get_type_name( ) const
{
   string str( "domain_numeric_range< numeric, " );
   str += to_string( minimum_whole );
   str += ", " + to_string( minimum_fraction );
   str += ", " + to_string( maximum_whole );
   str += ", " + to_string( maximum_fraction );
   str += " >";

   return str;
}

string numeric_range_domain::get_list_display( ) const
{
   string str( "range: " );
   str += to_string( minimum_whole );
   str += ".";
   str += to_string( minimum_fraction );
   str += "..";
   str += to_string( maximum_whole );
   str += ".";
   str += to_string( maximum_fraction );

   return str;
}

void numeric_range_domain::read_data( sio_reader& reader )
{
   minimum_whole = from_string< int >( reader.read_attribute( c_attribute_numeric_range_var_minimum_whole ) );
   minimum_fraction = from_string< int >( reader.read_attribute( c_attribute_numeric_range_var_minimum_fraction ) );
   maximum_whole = from_string< int >( reader.read_attribute( c_attribute_numeric_range_var_maximum_whole ) );
   maximum_fraction = from_string< int >( reader.read_attribute( c_attribute_numeric_range_var_maximum_fraction ) );
}

void numeric_range_domain::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_numeric_range_var_minimum_whole, to_string( minimum_whole ) );
   writer.write_attribute( c_attribute_numeric_range_var_minimum_fraction, to_string( minimum_fraction ) );
   writer.write_attribute( c_attribute_numeric_range_var_maximum_whole, to_string( maximum_whole ) );
   writer.write_attribute( c_attribute_numeric_range_var_maximum_fraction, to_string( maximum_fraction ) );
}

string numeric_range_domain::static_class_name( ) { return "numeric_range"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, domain, numeric_range_domain, static_class_name );

struct string_max_size_domain : domain
{
   void add( const vector< string >& args );

   string get_extra( ) const;

   string get_type_name( ) const;

   string get_list_display( ) const;

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   int max_size;

   static string static_class_name( );
};

void string_max_size_domain::add( const vector< string >& args )
{
   if( args.size( ) > 1 )
      throw runtime_error( "unexpected number of args > 1 for 'max_size' domain" );

   if( args.size( ) < 1 )
      throw runtime_error( "'size' argument is required for the 'max_size' domain" );

   max_size = get_numeric_arg_value( args[ 0 ], "size" );

   if( max_size == 0 )
      throw runtime_error( "'size' argument must be > 0 for 'max_size' domain" );
}

string string_max_size_domain::get_extra( ) const
{
   string str( "max_size=" );
   str += to_string( max_size );

   return str;
}

string string_max_size_domain::get_type_name( ) const
{
   string str( "domain_string_max_size< " );
   str += to_string( max_size );
   str += " >";

   return str;
}

string string_max_size_domain::get_list_display( ) const
{
   string str( "max_size: " );
   str += to_string( max_size );

   return str;
}

void string_max_size_domain::read_data( sio_reader& reader )
{
   max_size = from_string< int >( reader.read_attribute( c_attribute_string_max_size_var_max_size ) );
}

void string_max_size_domain::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_string_max_size_var_max_size, to_string( max_size ) );
}

string string_max_size_domain::static_class_name( ) { return "string_max_size"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, domain, string_max_size_domain, static_class_name );

struct string_format_domain : domain
{
   void add( const vector< string >& args );

   string get_extra( ) const;

   string get_type_name( ) const;

   string get_list_display( ) const;

   void read_data( sio_reader& reader );
   void write_data( sio_writer& writer ) const;

   string type;

   static string static_class_name( );
};

void string_format_domain::add( const vector< string >& args )
{
   if( args.size( ) > 1 )
      throw runtime_error( "unexpected number of args > 1 for 'format' domain" );

   if( args.size( ) < 1 )
      throw runtime_error( "'type' argument is required for the 'format' domain" );

   type = args[ 0 ];

   if( type != "label" && type != "identifier" )
      throw runtime_error( "unknown format type '" + type + "' for 'format' domain" );
}

string string_format_domain::get_extra( ) const
{
   return string( );
}

string string_format_domain::get_type_name( ) const
{
   string str( "domain_string_" );
   str += type;
   str += "_format";

   return str;
}

string string_format_domain::get_list_display( ) const
{
   string str( "type: " );
   str += type;

   return str;
}

void string_format_domain::read_data( sio_reader& reader )
{
   type = reader.read_attribute( c_attribute_string_format_var_type );
}

void string_format_domain::write_data( sio_writer& writer ) const
{
   writer.write_attribute( c_attribute_string_format_var_type, type );
}

string string_format_domain::static_class_name( ) { return "string_format"; }

DEFINE_CLASS_FACTORY_INSTANTIATOR( string, domain, string_format_domain, static_class_name );

