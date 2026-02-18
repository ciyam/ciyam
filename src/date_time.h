// Copyright (c) 2003-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifndef DATE_TIME_H
#  define DATE_TIME_H

#  ifndef HAS_PRECOMPILED_STD_HEADERS
#     include <ctime>
#     include <string>
#     include <iosfwd>
#  endif

#  include "ptypes.h"

enum month
{
   e_month_january = 1,
   e_month_february = 2,
   e_month_march = 3,
   e_month_april = 4,
   e_month_may = 5,
   e_month_june = 6,
   e_month_july = 7,
   e_month_august = 8,
   e_month_september = 9,
   e_month_october = 10,
   e_month_november = 11,
   e_month_december = 12
};

typedef uint8_t day;
typedef uint16_t year;

enum weekday
{
   e_weekday_monday = 1,
   e_weekday_tuesday = 2,
   e_weekday_wednesday = 3,
   e_weekday_thursday = 4,
   e_weekday_friday = 5,
   e_weekday_saturday = 6,
   e_weekday_sunday = 7
};

enum day_type
{
   e_day_type_day = 1,
   e_day_type_weekday = 2,
   e_day_type_weekendday = 3
};

enum occurrence
{
   e_occurrence_first = 1,
   e_occurrence_second = 2,
   e_occurrence_third = 3,
   e_occurrence_fourth = 4,
   e_occurrence_last = 5,
   e_occurrence_second_last = 6,
   e_occurrence_third_last = 7,
   e_occurrence_fourth_last = 8
};

enum day_of_significance
{
   e_day_of_significance_easter_sunday,
   e_day_of_significance_chinese_new_year
};

enum time_of_significance
{
   e_time_of_significance_sun_rising,
   e_time_of_significance_sun_setting
};

enum time_format
{
   e_time_format_hhmm = 0,
   e_time_format_hhmmss = 1,
   e_time_format_hhmmsst = 2,
   e_time_format_hhmmssth = 3,
   e_time_format_hhmmsstht = 4
};

enum date_format
{
   e_date_format_mmdd = 0,
   e_date_format_yymm = 1,
   e_date_format_yymmdd = 2,
   e_date_format_yyyymmdd = 3
};

enum chinese_new_year_format
{
   e_chinese_new_year_format_pinyin,
   e_chinese_new_year_format_chinese,
   e_chinese_new_year_format_english,
   e_chinese_new_year_format_combined
};

typedef fp64_t julian;

typedef uint32_t daynum;

typedef uint8_t hour;
typedef uint8_t minute;
typedef uint8_t second;

typedef uint8_t tenth;
typedef uint8_t hundredth;
typedef uint8_t thousandth;

typedef uint32_t millisecond;

typedef int32_t days;

typedef fp64_t seconds;
typedef int64_t milliseconds;

struct minutes
{
   minutes( int32_t m ) : m( m ) { }
   int32_t m;
};

struct hours
{
   hours( int32_t h ) : h( h ) { }
   int32_t h;
};

struct weeks
{
   weeks( int32_t w ) : w( w ) { }
   int32_t w;
};

struct months
{
   months( int32_t m ) : m( m ) { }
   int32_t m;
};

struct years
{
   years( int32_t y ) : y( y ) { }
   int32_t y;
};

// The tzadjust value is the difference from GMT to local time (any daylight savings adjustment should
// be included in this value also). Use negative values for southern latitudes and western longitudes.
struct locality
{
   locality( int tzadjust = 0, double latitude = 51.5, double longitude = 0.0 )
    :
    tzadjust( tzadjust ),
    latitude( latitude ),
    longitude( longitude )
   {
   }

   static locality Australia_Melbourne_Standard( );
   static locality Australia_Melbourne_Daylight( );
   static locality China_Beijing( );

   int8_t tzadjust;
   fp64_t latitude;
   fp64_t longitude;
};

inline locality locality::Australia_Melbourne_Standard( ) { return locality( 10, -37.83, 144.97 ); }
inline locality locality::Australia_Melbourne_Daylight( ) { return locality( 11, -37.83, 144.97 ); }
inline locality locality::China_Beijing( ) { return locality( 8, 39.92, 116.43 ); }

class date_time;

class read_stream;
class write_stream;

class mtime
{
   friend class date_time;

   public:
   mtime( );

   explicit mtime( const char* s );
   explicit mtime( const std::string& s );

   mtime( hour hr, minute mn );
   mtime( hour hr, minute mn, second sc );
   mtime( hour hr, minute mn, second sc, millisecond m );
   mtime( hour hr, minute mn, second sc, tenth te, hundredth hd = 0, thousandth th = 0 );

   mtime( hour hr, minute mn, seconds s );

   mtime( const mtime& src );
   mtime& operator =( const mtime& src );
   mtime& operator =( const std::string& s );

   mtime& operator ++( );
   mtime operator ++( int );

   mtime& operator --( );
   mtime operator --( int );

   mtime& operator +=( hours h );
   mtime& operator -=( hours h );

   mtime& operator +=( minutes m );
   mtime& operator -=( minutes m );

   mtime& operator +=( seconds s );
   mtime& operator -=( seconds s );

   mtime& operator +=( milliseconds m );
   mtime& operator -=( milliseconds m );

   operator seconds( ) const { return ( seconds )( ms / 1000 ); }
   operator milliseconds( ) const { return ms; }

   hour get_hour( ) const;
   minute get_minute( ) const;
   second get_second( ) const;
   millisecond get_millisecond( ) const { return ms % 1000; }

   std::string as_string( time_format tf, bool use_separators = false ) const;
   std::string as_string( bool use_separators = false, bool include_milliseconds = true ) const;

   static mtime local( );
   static mtime standard( );

   static mtime minimum( );
   static mtime maximum( );

   friend bool operator <( const mtime& lhs, const mtime& rhs );
   friend bool operator <=( const mtime& lhs, const mtime& rhs );

   friend bool operator >( const mtime& lhs, const mtime& rhs );
   friend bool operator >=( const mtime& lhs, const mtime& rhs );

   friend bool operator ==( const mtime& lhs, const mtime& rhs );

   friend std::ostream& operator <<( std::ostream& os, const mtime& src );

   friend read_stream& operator >>( read_stream& rs, mtime& dest );
   friend write_stream& operator <<( write_stream& ws, const mtime& src );

   private:
   millisecond ms;

   mtime( millisecond ms );
};

inline bool operator !=( const mtime& lhs, const mtime& rhs )
{
   return !( lhs == rhs );
}

inline mtime operator +( const mtime& src, seconds s )
{
   mtime mt( src );
   mt += s;
   return mt;
}

inline mtime operator -( const mtime& src, seconds s )
{
   mtime mt( src );
   mt -= s;
   return mt;
}

inline mtime operator +( const mtime& src, milliseconds m )
{
   mtime mt( src );
   mt += m;
   return mt;
}

inline mtime operator -( const mtime& src, milliseconds m )
{
   mtime mt( src );
   mt -= m;
   return mt;
}

inline milliseconds operator -( const mtime& lhs, const mtime& rhs )
{
   return ( milliseconds )lhs - ( milliseconds )rhs;
}

struct yyyymmdd
{
   year yr;
   month mo;
   day dy;
};

class udate
{
   friend class date_time;

   public:
   udate( );

   explicit udate( julian jdt );

   explicit udate( const char* s );
   explicit udate( const std::string& s );

   udate( year yr, month mo, day dy );
   udate( year yr, month mo, weekday wd, occurrence occ );
   udate( year yr, month mo, day_type dt, occurrence occ );
   udate( year yr, month mo, day dy, day_type dt, occurrence occ );

   udate( year yr, day_of_significance dos );

   udate( const udate& src );
   udate& operator =( const udate& src );
   udate& operator =( const std::string& s );

   udate& operator ++( );
   udate operator ++( int );

   udate& operator --( );
   udate operator --( int );

   udate& operator +=( years y );
   udate& operator -=( years y );

   udate& operator +=( months m );
   udate& operator -=( months m );

   udate& operator +=( days d );
   udate& operator -=( days d );

   udate& operator +=( weeks w ) { return operator +=( ( days )( w.w * 7 ) ); }
   udate& operator -=( weeks w ) { return operator -=( ( days )( w.w * 7 ) ); }

   operator daynum( ) const;
   operator julian( ) const;

   operator weekday( ) const;

   year get_year( ) const;
   month get_month( ) const;
   day get_day( ) const;

   int get_decade( ) const { return get_year( ) % 100; }
   int get_century( ) const { return get_year( ) / 100; }

   std::string as_string( bool use_separators = false ) const;
   std::string as_string( date_format df, bool use_separators = false ) const;

   days get_day_of_year( ) const;

   std::string month_name( bool short_name = false ) const;
   std::string weekday_name( bool short_name = false ) const;

   std::string chinese_year_name( chinese_new_year_format format = e_chinese_new_year_format_combined ) const;

   bool is_leap_year( ) const;
   bool is_weekend_day( ) const;

   days days_in_year( ) const;
   days days_in_month( ) const;

   bool is_daynum( ) const;
   bool is_calendar( ) const;

   void convert_to_daynum( );
   void convert_to_calendar( );

   void print( std::ostream& os ) const;

   static udate local( );
   static udate standard( );

   static udate minimum( );
   static udate maximum( );

   static year minimum_year( );
   static year maximum_year( );

   friend bool operator <( const udate& lhs, const udate& rhs );
   friend bool operator <=( const udate& lhs, const udate& rhs );

   friend bool operator >( const udate& lhs, const udate& rhs );
   friend bool operator >=( const udate& lhs, const udate& rhs );

   friend bool operator ==( const udate& lhs, const udate& rhs );

   friend std::ostream& operator <<( std::ostream& os, const udate& src );

   friend read_stream& operator >>( read_stream& rs, udate& dest );
   friend write_stream& operator <<( write_stream& ws, const udate& src );

   private:
   union
   {
      daynum dn;
      yyyymmdd ymd;
   };

   udate( daynum dn );

   void validate( ) const;
};

inline bool operator !=( const udate& lhs, const udate& rhs )
{
   return !( lhs == rhs );
}

inline udate operator +( const udate& src, years y )
{
   udate ud( src );
   ud += y;
   return ud;
}

inline udate operator -( const udate& src, years y )
{
   udate ud( src );
   ud -= y;
   return ud;
}

inline udate operator +( const udate& src, months m )
{
   udate ud( src );
   ud += m;
   return ud;
}

inline udate operator -( const udate& src, months m )
{
   udate ud( src );
   ud -= m;
   return ud;
}

inline udate operator +( const udate& src, days d )
{
   udate ud( src );
   ud += d;
   return ud;
}

inline udate operator -( const udate& src, days d )
{
   udate ud( src );
   ud -= d;
   return ud;
}

inline udate operator +( const udate& src, weeks w )
{
   udate ud( src );
   ud += ( days )( w.w * 7 );
   return ud;
}

inline udate operator -( const udate& src, weeks w )
{
   udate ud( src );
   ud -= ( days )( w.w * 7 );
   return ud;
}

inline days operator -( const udate& lhs, const udate& rhs )
{
   return ( daynum )lhs - ( daynum )rhs;
}

class date_time
{
   public:
   date_time( ) { }

   explicit date_time( julian j );
   explicit date_time( int64_t unix_time );

   void construct_from_julian( julian j );

   explicit date_time( const char* s );
   explicit date_time( const std::string& s );

   date_time( const udate& ud, const mtime& mt );

   date_time( year yr, month mo, weekday wd, occurrence occ );
   date_time( year yr, month mo, weekday wd, occurrence occ, const mtime& mt );
   date_time( year yr, month mo, weekday wd, occurrence occ, hour hr, minute mn );
   date_time( year yr, month mo, weekday wd, occurrence occ, hour hr, minute mn, second sc );

   date_time( year yr, month mo, day_type dt, occurrence occ );
   date_time( year yr, month mo, day_type dt, occurrence occ, const mtime& mt );
   date_time( year yr, month mo, day_type dt, occurrence occ, hour hr, minute mn );
   date_time( year yr, month mo, day_type dt, occurrence occ, hour hr, minute mn, second sc );

   date_time( year yr, month mo, day dy, day_type dt, occurrence occ );
   date_time( year yr, month mo, day dy, day_type dt, occurrence occ, const mtime& mt );
   date_time( year yr, month mo, day dy, day_type dt, occurrence occ, hour hr, minute mn );
   date_time( year yr, month mo, day dy, day_type dt, occurrence occ, hour hr, minute mn, second sc );

   date_time( year yr, month mo, day dy );
   date_time( year yr, month mo, day dy, const mtime& mt );
   date_time( year yr, month mo, day dy, hour hr, minute mn );
   date_time( year yr, month mo, day dy, hour hr, minute mn, second sc );
   date_time( year yr, month mo, day dy, hour hr, minute mn, second sc, millisecond ms );
   date_time( year yr,
    month mo, day dy, hour hr, minute mn, second sc, tenth te, hundredth hd = 0, thousandth th = 0 );

   date_time( year yr, day_of_significance dos );
   date_time( year yr, day_of_significance dos, const mtime& mt );
   date_time( year yr, month mo, day dy, time_of_significance tos, const locality& l );
   date_time( year yr, day_of_significance dos, time_of_significance tos, const locality& l );

   date_time( year yr, month mo, day dy, hour hr, minute mn, seconds s );

   date_time( const date_time& src );
   date_time& operator =( const date_time& src );
   date_time& operator =( const std::string& s );

   date_time& operator ++( );
   date_time operator ++( int );

   date_time& operator --( );
   date_time operator --( int );

   date_time& operator +=( years y );
   date_time& operator -=( years y );

   date_time& operator +=( months m );
   date_time& operator -=( months m );

   date_time& operator +=( weeks w ) { return operator +=( ( days )( w.w * 7 ) ); }
   date_time& operator -=( weeks w ) { return operator -=( ( days )( w.w * 7 ) ); }

   date_time& operator +=( days d );
   date_time& operator -=( days d );

   date_time& operator +=( hours h );
   date_time& operator -=( hours h );

   date_time& operator +=( minutes m );
   date_time& operator -=( minutes m );

   date_time& operator +=( seconds s );
   date_time& operator -=( seconds s );

   date_time& operator +=( milliseconds m );
   date_time& operator -=( milliseconds m );

   udate get_date( ) const { return ud; }
   mtime get_time( ) const { return mt; }

   operator udate( ) const { return ud; }
   operator mtime( ) const { return mt; }

   operator daynum( ) const { return ( daynum )ud; }
   operator julian( ) const;

   operator weekday( ) const { return ( weekday )ud; }

   year get_year( ) const { return ud.get_year( ); }
   month get_month( ) const { return ud.get_month( ); }
   day get_day( ) const { return ud.get_day( ); }

   days get_day_of_year( ) const { return ud.get_day_of_year( ); }

   hour get_hour( ) const { return mt.get_hour( ); }
   minute get_minute( ) const { return mt.get_minute( ); }
   second get_second( ) const { return mt.get_second( ); }
   millisecond get_millisecond( ) const { return mt.get_millisecond( ); }

   std::string as_string( time_format tf, bool use_separators = false ) const;
   std::string as_string( bool use_separators = false, bool include_milliseconds = true ) const;

   std::string month_name( bool short_name = false ) const { return ud.month_name( short_name ); }
   std::string weekday_name( bool short_name = false ) const { return ud.weekday_name( short_name ); }

   std::string chinese_year_name( chinese_new_year_format format = e_chinese_new_year_format_combined ) const
   {
      return ud.chinese_year_name( format );
   }

   bool is_leap_year( ) const { return ud.is_leap_year( ); }
   bool is_weekend_day( ) const { return ud.is_weekend_day( ); }

   days days_in_year( ) const { return ud.days_in_year( ); }

   days days_in_month( ) const { return ud.days_in_month( ); }

   double moon_phase( ) const;
   std::string moon_phase_description( ) const;

   static date_time local( );
   static date_time standard( );

   static date_time minimum( );
   static date_time maximum( );

   static year minimum_year( ) { return udate::minimum_year( ); }
   static year maximum_year( ) { return udate::maximum_year( ); }

   friend bool operator <( const date_time& lhs, const date_time& rhs );
   friend bool operator <=( const date_time& lhs, const date_time& rhs );

   friend bool operator >( const date_time& lhs, const date_time& rhs );
   friend bool operator >=( const date_time& lhs, const date_time& rhs );

   friend bool operator ==( const date_time& lhs, const date_time& rhs );

   friend seconds operator -( const date_time& lhs, const date_time& rhs );

   friend std::ostream& operator <<( std::ostream& os, const date_time& src );

   friend read_stream& operator >>( read_stream& rs, date_time& dest );
   friend write_stream& operator <<( write_stream& ws, const date_time& src );

   private:
   udate ud;
   mtime mt;
};

inline bool operator !=( const date_time& lhs, const date_time& rhs )
{
   return !( lhs == rhs );
}

inline date_time operator +( const date_time& src, years y )
{
   date_time dt( src );
   dt += y;
   return dt;
}

inline date_time operator -( const date_time& src, years y )
{
   date_time dt( src );
   dt -= y;
   return dt;
}

inline date_time operator +( const date_time& src, months m )
{
   date_time dt( src );
   dt += m;
   return dt;
}

inline date_time operator -( const date_time& src, months m )
{
   date_time dt( src );
   dt -= m;
   return dt;
}

inline date_time operator +( const date_time& src, weeks w )
{
   date_time dt( src );
   dt += ( days )( w.w * 7 );
   return dt;
}

inline date_time operator -( const date_time& src, weeks w )
{
   date_time dt( src );
   dt -= ( days )( w.w * 7 );
   return dt;
}

inline date_time operator +( const date_time& src, days d )
{
   date_time dt( src );
   dt += d;
   return dt;
}

inline date_time operator -( const date_time& src, days d )
{
   date_time dt( src );
   dt -= d;
   return dt;
}

inline date_time operator +( const date_time& src, hours h )
{
   date_time dt( src );
   dt += h;
   return dt;
}

inline date_time operator -( const date_time& src, hours h )
{
   date_time dt( src );
   dt -= h;
   return dt;
}

inline date_time operator +( const date_time& src, minutes m )
{
   date_time dt( src );
   dt += m;
   return dt;
}

inline date_time operator -( const date_time& src, minutes m )
{
   date_time dt( src );
   dt -= m;
   return dt;
}

inline date_time operator +( const date_time& src, seconds s )
{
   date_time dt( src );
   dt += s;
   return dt;
}

inline date_time operator -( const date_time& src, seconds s )
{
   date_time dt( src );
   dt -= s;
   return dt;
}

inline date_time operator +( const date_time& src, milliseconds m )
{
   date_time dt( src );
   dt += m;
   return dt;
}

inline date_time operator -( const date_time& src, milliseconds m )
{
   date_time dt( src );
   dt -= m;
   return dt;
}

seconds seconds_per_day( );

year gregorian_year_begin( );
month gregorian_month_begin( );
day gregorian_day_begin( );
day julian_day_of_month( );

inline weekday day_of_week( year yr, month mh, day dy )
{
   return ( weekday )udate( yr, mh, dy );
}

days days_in_year( year yr );

days days_in_month( year yr, month mh );

julian convert_calendar_to_julian(
 year yr, month mo, day dy, hour hr = 0, minute mn = 0, seconds secs = 0.0 );

void convert_julian_to_calendar( julian jdt, year& yr, month& mo, day& dy );
void convert_julian_to_calendar( julian jdt, year& yr, month& mo, day& dy, hour& hr, minute& mn, second& sc );

void convert_julian_to_calendar( julian jdt,
 year& yr, month& mo, day& dy, hour& hr, minute& mn, seconds& secs );

void convert_julian_to_calendar( julian jdt,
 year& yr, month& mo, day& dy, hour& hr, minute& mn, second& sc, tenth& te, hundredth& hd, thousandth& th );

int64_t unix_time( const date_time& dt, time_t* p_tt = 0, bool force_epoch_if_earlier = false );
inline int64_t unix_time( time_t* p_tt = 0 ) { return unix_time( date_time::standard( ), p_tt ); }

int64_t seconds_between( const date_time& lhs, const date_time& rhs );

std::string format_udate( const udate& ud, const std::string& mask );
std::string format_mtime( const mtime& mt, const std::string& mask );

std::string format_date_time( const date_time& dt, const std::string& dmask, const std::string& tmask );

#endif
