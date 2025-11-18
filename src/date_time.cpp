// Copyright (c) 2003-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2025 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  ifdef __GNUG__
#     include <sys/time.h>
#  endif
#  include <cmath>
#  include <iostream>
#  include <algorithm>
#  include <stdexcept>
#endif

#include "date_time.h"

#include "utilities.h"
#include "read_write_stream.h"

using namespace std;

namespace
{
// NOTE: Changing the Gregorian calendar start date used here is not recommended.
//
// It should be noted that the Gregorian calendar was not officially adopted by Britain and its colonies
// (including "America") until Thursday 14th September 1752 (which followed Wednesday 2nd September 1752
// in the Julian calendar) as is observed using Linux by issuing the command "cal 9 1752".
//
// The original start of the Gregorian calendar (i.e. in Italy) was actually on Friday 15th October 1582
// (following Thursday 4th October 1582 in the Julian calendar), however, only a few other countries had
// adopted the Gregorian calendar then.

//#define USE_ORIGINAL_GREGORIAN_START_DATE

#ifdef USE_ORIGINAL_GREGORIAN_START_DATE
const julian c_julian_start_day = 2299161;
const day c_julian_day_of_month = 5;
const day c_gregorian_day_begin = 15;
const month c_gregorian_month_begin = e_month_october;
const year c_gregorian_year_begin = 1582;
#else
const julian c_julian_start_day = 2361222;
const day c_julian_day_of_month = 3;
const day c_gregorian_day_begin = 14;
const month c_gregorian_month_begin = e_month_september;
const year c_gregorian_year_begin = 1752;
#endif

const julian c_unix_epoch = 2440587.5;

const seconds c_seconds_per_day = 86400.0;

const millisecond c_min_millisecond = 0ul;
const millisecond c_max_millisecond = 86399999ul;

const millisecond c_milliseconds_per_second = 1000ul;
const millisecond c_milliseconds_per_minute = 60000ul;
const millisecond c_milliseconds_per_hour = 3600000ul;
const millisecond c_milliseconds_per_day = 86400000ul;

const day days_for_month[ 12 ] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

// NOTE: If changed would require a number of other changes (including the Chinese new year table).
const year c_calendar_start_point = 1600;

#ifdef USE_ORIGINAL_GREGORIAN_START_DATE
const year c_min_year = 1600;
const month c_min_month = e_month_january;
const day c_min_day = 1;

const daynum c_min_day_number = 0u;
const julian c_min_year_julian = 2305447.5;
#else
const year c_min_year = 1800;
const month c_min_month = e_month_january;
const day c_min_day = 1;

const daynum c_min_day_number = 73049u;
const julian c_min_year_julian = 2378496.5;
#endif

const days c_days_in_first_two_months = 59;

const daynum c_max_day_number = c_min_day_number + 730484u; // i.e. allow for 2000 years
const julian c_max_year_julian = c_min_year_julian + 730484u;

const year c_max_year = c_min_year + 1999; // i.e. is also a 2000 year limit
const month c_max_month = e_month_december;
const day c_max_day = 31;

const unsigned int c_day_number_in_use = 0x80000000u;

const double c_pi = 3.1415927;

const double c_zenith_official = 90.83;
const double c_zenith_civil = 96.0;
const double c_zenith_nautical = 102.0;
const double c_zenith_astronomical = 108.0;

const int c_epoch = 1985;

const double c_epsilon_g = 279.611371; // Solar ecliptic long at epoch.
const double c_rho_g = 282.680403; // Solar ecliptic long of perigee at epoch.
const double c_eccentricity = 0.01671542; // Solar orbit eccentricity.

const double c_lzero = 18.251907; // Lunar mean long at epoch.
const double c_pzero = 192.917585; // Lunar mean long of perigee at epoch.
const double c_nzero = 55.204723; // Lunar mean long of node at epoch.

struct weekday_full_name
{
   const char* p_str;
}
weekday_full_names[ ] =
{
   { "Monday" },
   { "Tuesday" },
   { "Wednesday" },
   { "Thursday" },
   { "Friday" },
   { "Saturday" },
   { "Sunday" }
};

struct month_full_name
{
   const char* p_str;
}
month_full_names[ ] =
{
   { "January" },
   { "February" },
   { "March" },
   { "April" },
   { "May" },
   { "June" },
   { "July" },
   { "August" },
   { "September" },
   { "October" },
   { "November" },
   { "December" }
};

enum element
{
   e_element_wood,
   e_element_fire,
   e_element_earth,
   e_element_metal,
   e_element_water
};

struct celestial_stem
{
   const char* p_str;

   element elem;
}
celestial_stems[ ] =
{
   { "jia", e_element_wood },
   { "yi", e_element_wood },
   { "bing", e_element_fire },
   { "ding", e_element_fire },
   { "wu", e_element_earth },
   { "ji", e_element_earth },
   { "geng", e_element_metal },
   { "xin", e_element_metal },
   { "ren", e_element_water },
   { "gui", e_element_water }
};

struct terrestial_branch
{
   const char* p_str;
   const char* p_english;
}
terrestial_branches[ ] =
{
   { "zi", "rat" },
   { "chou", "ox" },
   { "yin", "tiger" },
   { "mao", "rabbit" },
   { "chen", "dragon" },
   { "si", "snake" },
   { "wu", "horse" },
   { "wei", "goat" },
   { "shen", "monkey" },
   { "you", "rooster" },
   { "xu", "dog" },
   { "hai", "pig" }
};

// NOTE: Chinese new year is being taken from this table rather than being determined.
//
// Chinese new year month and days with month being the first digit (i.e. 1 is January
// and 2 is Feb) and day of month following (1 = first day of month). This array holds
// all Chinese new year month and days from 1600 to 3799. Chinese new year will always
// fall on the second New Moon after the Winter Solstice.

unsigned char chinese_new_years_month_and_day[ ] =
{

215, 203, 123, 211, 131, 218, 207, 128, 216, 204, 125, 213, 202, 121, 209, 129, 217,
206, 126, 214, 204, 122, 210, 131, 219, 207, 128, 216, 205, 124, 212, 201, 121, 208,
129, 217, 207, 126, 214, 203, 123, 210, 130, 218, 208, 128, 216, 205, 125, 211, 201,
121, 209, 129, 217, 206, 126, 213, 202, 123, 211, 130, 218, 208, 128, 215, 204, 124,
212, 201, 121, 209, 130, 217, 206, 126, 214, 202, 123, 211, 131, 218, 207, 127, 215,
203, 124, 212, 202, 121, 209, 129, 217, 205, 125, 213, 203, 123, 211, 131, 219, 208,
128, 216, 205, 125, 213, 203, 123, 210, 130, 217, 207, 126, 214, 204, 124, 211, 131,
219, 208, 128, 216, 205, 126, 213, 202, 122, 210, 129, 217, 207, 127, 214, 204, 124,
212, 131, 219, 208, 129, 216, 205, 126, 213, 201, 122, 209, 130, 217, 207, 127, 215,
203, 123, 211, 131, 218, 208, 129, 217, 205, 125, 213, 202, 121, 209, 130, 218, 207,
127, 215, 204, 123, 211, 131, 219, 208, 128, 216, 205, 124, 212, 202, 122, 209, 130,
218, 207, 126, 214, 203, 124, 211, 131, 121, 209, 128, 216, 205, 125, 213, 203, 123,
211, 131, 218, 207, 128, 214, 204, 125, 213, 201, 121, 209, 129, 216, 205, 126, 214,
203, 123, 211, 131, 218, 207, 127, 215, 204, 125, 213, 202, 220, 209, 129, 217, 205,
126, 214, 203, 123, 210, 130, 218, 207, 127, 215, 205, 124, 212, 201, 220, 208, 129,
217, 206, 126, 214, 203, 123, 210, 130, 218, 208, 127, 215, 205, 125, 211, 131, 219,
209, 129, 217, 206, 126, 213, 202, 122, 210, 130, 218, 208, 128, 215, 204, 124, 212,
131, 121, 209, 130, 217, 206, 126, 213, 202, 122, 210, 131, 219, 208, 129, 216, 204,
125, 213, 202, 122, 210, 130, 218, 206, 126, 214, 203, 123, 211, 201, 220, 208, 128,
216, 205, 124, 213, 202, 123, 210, 130, 217, 206, 126, 214, 204, 124, 211, 131, 219,
208, 127, 215, 205, 125, 213, 202, 122, 210, 129, 217, 206, 127, 214, 203, 124, 212,
131, 218, 208, 128, 215, 205, 125, 213, 202, 121, 209, 130, 217, 206, 127, 215, 203,
123, 211, 131, 218, 207, 128, 216, 205, 125, 213, 202, 220, 209, 129, 217, 206, 127,
215, 204, 123, 210, 131, 219, 207, 128, 216, 205, 124, 212, 201, 122, 209, 129, 218,
207, 126, 214, 203, 123, 210, 131, 219, 208, 128, 216, 205, 125, 212, 201, 122, 210,
129, 217, 206, 126, 213, 203, 123, 211, 131, 219, 208, 128, 215, 204, 124, 212, 201,
122, 210, 130, 217, 206, 126, 214, 202, 123, 211, 201, 219, 208, 128, 215, 204, 124,
212, 202, 121, 209, 129, 217, 205, 126, 214, 203, 123, 211, 131, 219, 207, 127, 215,
205, 124, 212, 202, 122, 209, 129, 217, 206, 126, 214, 203, 124, 210, 130, 218, 207,
127, 215, 205, 125, 212, 201, 121, 209, 129, 217, 207, 128, 215, 204, 124, 212, 131,
219, 208, 129, 216, 206, 126, 214, 202, 122, 210, 130, 217, 207, 127, 215, 203, 123,
211, 201, 219, 208, 129, 217, 205, 125, 213, 202, 122, 210, 130, 218, 207, 127, 215,
204, 123, 211, 201, 220, 208, 129, 216, 205, 125, 212, 202, 123, 210, 130, 218, 207,
126, 214, 203, 124, 211, 201, 220, 209, 128, 216, 205, 125, 212, 202, 123, 211, 130,
218, 207, 127, 214, 203, 124, 212, 131, 121, 208, 128, 215, 205, 125, 213, 202, 122,
210, 130, 217, 206, 127, 215, 204, 125, 213, 202, 220, 209, 129, 217, 206, 126, 214,
204, 123, 210, 130, 218, 207, 128, 216, 205, 124, 212, 201, 220, 208, 129, 217, 207,
126, 214, 203, 123, 210, 130, 218, 208, 128, 216, 205, 125, 212, 201, 220, 209, 129,
217, 206, 127, 213, 202, 123, 211, 130, 218, 208, 128, 215, 204, 124, 212, 131, 121,
209, 130, 217, 206, 126, 214, 202, 122, 211, 131, 218, 208, 128, 215, 203, 124, 212,
202, 121, 209, 129, 217, 205, 125, 213, 203, 122, 211, 131, 219, 207, 127, 215, 204,
124, 212, 201, 122, 210, 130, 217, 207, 126, 214, 204, 124, 211, 201, 219, 208, 128,
216, 205, 126, 213, 202, 221, 210, 129, 217, 206, 127, 214, 204, 124, 212, 131, 219,
208, 128, 215, 205, 126, 214, 202, 122, 209, 130, 217, 206, 127, 215, 203, 123, 211,
131, 218, 208, 128, 217, 205, 125, 213, 202, 121, 209, 130, 218, 206, 127, 215, 204,
123, 211, 131, 219, 208, 128, 216, 205, 124, 212, 201, 122, 209, 130, 218, 207, 126,
214, 203, 123, 210, 131, 219, 209, 128, 216, 205, 125, 212, 201, 122, 210, 130, 218,
207, 127, 213, 203, 123, 211, 131, 219, 208, 128, 215, 204, 125, 213, 201, 122, 210,
130, 217, 206, 126, 214, 203, 123, 211, 201, 219, 208, 128, 216, 204, 125, 213, 202,
122, 209, 129, 217, 205, 126, 214, 204, 123, 211, 131, 219, 207, 127, 215, 205, 125,
213, 202, 122, 209, 129, 217, 206, 126, 214, 204, 124, 211, 131, 218, 208, 127, 215,
205, 125, 212, 201, 121, 209, 129, 217, 206, 127, 214, 203, 123, 211, 231, 218, 208,
128, 215, 205, 125, 213, 201, 121, 209, 130, 217, 206, 127, 214, 202, 122, 210, 131,
219, 209, 129, 217, 205, 125, 213, 202, 122, 210, 131, 219, 207, 127, 215, 204, 123,
211, 201, 122, 209, 129, 217, 206, 125, 213, 202, 123, 210, 130, 218, 207, 126, 214,
204, 124, 211, 201, 121, 209, 128, 216, 205, 126, 213, 202, 123, 211, 130, 218, 207,
127, 214, 204, 124, 212, 201, 219, 208, 128, 215, 205, 126, 214, 202, 122, 210, 130,
217, 206, 127, 215, 204, 124, 212, 201, 219, 208, 128, 216, 205, 125, 213, 203, 122,
210, 130, 218, 206, 127, 215, 204, 124, 211, 131, 219, 208, 128, 216, 206, 126, 214,
203, 123, 210, 130, 219, 208, 128, 216, 205, 125, 212, 201, 220, 210, 129, 217, 207,
127, 213, 202, 123, 211, 130, 219, 208, 128, 215, 204, 124, 212, 201, 121, 210, 130,
217, 206, 126, 214, 202, 123, 211, 131, 218, 208, 128, 215, 204, 124, 212, 202, 121,
209, 130, 217, 205, 125, 214, 203, 123, 211, 131, 219, 207, 127, 215, 204, 124, 212,
202, 122, 209, 129, 217, 206, 125, 213, 203, 124, 211, 131, 218, 207, 127, 215, 204,
125, 212, 201, 121, 209, 128, 216, 206, 126, 213, 203, 123, 211, 131, 219, 208, 129,
216, 205, 126, 214, 202, 122, 210, 130, 217, 207, 127, 215, 203, 123, 211, 131, 218,
208, 129, 217, 205, 125, 213, 202, 220, 209, 130, 218, 207, 127, 215, 204, 123, 211,
131, 219, 208, 129, 217, 206, 125, 212, 202, 122, 209, 130, 218, 207, 126, 214, 203,
124, 211, 131, 219, 209, 128, 216, 205, 125, 212, 202, 122, 210, 130, 218, 207, 127,
214, 203, 124, 212, 131, 219, 208, 128, 215, 204, 125, 213, 202, 122, 210, 130, 217,
206, 126, 214, 203, 124, 212, 201, 219, 208, 128, 216, 204, 125, 213, 203, 122, 210,
130, 217, 206, 126, 214, 204, 123, 211, 131, 219, 207, 128, 216, 205, 125, 213, 202,
122, 209, 129, 217, 207, 126, 214, 204, 124, 211, 131, 219, 208, 128, 216, 205, 126,
212, 201, 121, 209, 129, 217, 207, 127, 214, 203, 123, 211, 130, 218, 208, 129, 216,
205, 125, 213, 201, 121, 209, 130, 217, 207, 127, 215, 203, 123, 211, 131, 218, 208,
128, 216, 204, 124, 212, 202, 121, 209, 130, 218, 206, 126, 214, 203, 122, 211, 131,
219, 208, 128, 216, 205, 124, 212, 202, 122, 210, 131, 218, 207, 126, 214, 204, 124,
212, 201, 220, 209, 128, 216, 205, 126, 213, 203, 123, 211, 130, 218, 207, 127, 214,
204, 124, 212, 201, 220, 209, 129, 216, 205, 126, 214, 202, 123, 210, 130, 217, 207,
127, 215, 204, 124, 212, 201, 219, 208, 128, 217, 205, 126, 214, 203, 122, 210, 130,
218, 207, 127, 215, 205, 124, 211, 131, 219, 208, 128, 217, 206, 125, 213, 202, 122,
209, 130, 218, 208, 127, 215, 204, 124, 211, 131, 219, 209, 128, 216, 206, 126, 212,
202, 122, 210, 130, 218, 207, 127, 215, 204, 124, 212, 201, 122, 210, 130, 217, 206,
126, 214, 202, 123, 211, 201, 219, 208, 128, 216, 204, 124, 212, 202, 122, 210, 130,
217, 205, 126, 214, 203, 123, 211, 131, 219, 207, 127, 215, 205, 124, 212, 202, 122,
209, 129, 217, 206, 126, 214, 203, 124, 211, 131, 218, 208, 127, 215, 205, 125, 212,
201, 220, 209, 128, 216, 206, 127, 214, 203, 123, 211, 130, 218, 207, 128, 215, 205,
125, 213, 201, 220, 209, 129, 216, 206, 127, 215, 203, 123, 210, 131, 218, 207, 128,
216, 204, 124, 212, 201, 122, 210, 130, 218, 207, 127, 215, 204, 123, 211, 201, 220,
208, 129, 217, 206, 125, 213, 202, 123, 210, 130, 218, 207, 126, 214, 203, 124, 211,
201, 220, 209, 128, 216, 205, 125, 212, 202, 123, 211, 130, 218, 207, 127, 214, 203,
124, 212, 201, 219, 209, 129, 215, 205, 125, 213, 202, 122, 210, 130, 217, 206, 127,
215, 203, 124, 212, 201, 219, 208, 128, 216, 205, 125, 213, 203, 122, 210, 130, 218,
206, 127, 215, 204, 124, 211, 131, 219, 207, 128, 216, 206, 125, 213, 202, 122, 209,
129, 217, 207, 127, 215, 204, 124, 211, 131, 219, 208, 128, 216, 206, 126, 213, 202,
122, 210, 129, 217, 207, 127, 214, 203, 123, 211, 131, 219, 208, 129, 216, 205, 125,
213, 201, 121, 210, 130, 217, 207, 127, 215, 203, 123, 211, 131, 219, 208, 129, 216,
204, 124, 212, 202, 121, 210, 130, 218, 206, 126, 214, 203, 123, 211, 201, 220, 208,
128, 216, 205, 124, 212, 202, 122, 209, 130, 218, 206, 126, 214, 203, 124, 211, 131,
219, 208, 127, 215, 205, 125, 212, 202, 122, 210, 129, 217, 206, 126, 214, 203, 124,
212, 201, 220, 209, 129, 216, 205, 126, 214, 203, 123, 210, 130, 217, 207, 127, 216,
204, 124, 212, 201, 219, 208, 129, 217, 205, 126, 214, 203, 122, 210, 130, 218, 207,
127, 215, 205, 124, 211, 201, 220, 208, 129, 217, 206, 125, 213, 202, 122, 209, 130,
218, 208, 127, 215, 204, 124, 211, 131, 219, 209, 129, 217, 206, 126, 213, 202, 122,
210, 130, 218, 208, 128, 214, 203, 124, 212, 131, 121, 209, 129, 216, 205, 125, 213,
202, 122, 210, 131, 218, 207, 127, 215, 203, 124, 212, 201, 121, 209, 129, 216, 206,
126, 214, 204, 123, 211, 131, 219, 207, 127, 215, 205, 125, 213, 202, 221, 209, 129,
217, 206, 126, 214, 204, 124, 211, 131, 219, 208, 127, 215, 205, 126, 212, 202, 122,
209, 129, 217, 206, 127, 214, 203, 123, 211, 130, 218, 208, 128, 215, 205, 125, 213,
201, 220, 209, 130, 217, 206, 127, 215, 203, 123, 211, 131, 218, 208, 128, 216, 204,
124, 212, 201, 121, 209, 130, 218, 206, 126, 214, 203, 122, 210, 131, 219, 208, 128,
216, 205, 124, 212, 201, 122, 209, 130, 217, 207, 126, 213, 203, 123, 211, 201, 220,
209, 128, 216, 205, 126, 213, 202, 123, 211, 130, 218, 207, 127, 214, 204, 124, 212,
201, 220, 209, 129, 216, 205, 126, 214, 202, 123, 210, 130, 217, 206, 127, 215, 204,
124, 212, 201, 219, 208, 128, 216, 205, 126, 214, 203, 122, 210, 130, 218, 206, 127,
215, 204, 124, 212, 131, 219, 208, 128, 216, 206, 125, 213, 202, 122, 209, 129, 218,
207, 127, 215, 204, 124, 211, 131, 219, 209, 128, 216, 206, 126, 213, 202, 122, 210,
129, 218, 207, 128, 214, 203, 123, 211, 131, 219, 209, 129, 216, 205, 125, 213, 201,
122, 210, 130, 218, 207, 127, 215, 203, 123, 211, 201, 219, 208, 129, 217, 204, 125,
213, 202, 122, 210, 130, 218, 206, 126, 214, 203, 123, 211, 201, 220, 208, 128, 216,
205, 124, 212, 202, 123, 210, 130, 218, 207, 126, 214, 203, 124, 211, 201, 219, 208,
127, 215, 205, 125, 212, 202, 122, 210, 129, 217, 206, 127, 214, 203, 124, 212, 131,
219, 208, 128, 215, 205, 125, 213, 202, 122, 209, 130, 217, 206, 127, 215, 203, 123,
211, 131, 218, 207, 128, 216, 205, 125, 213, 202, 122, 210, 130, 218, 207, 128, 216,
205, 124, 212, 201, 220, 208, 129, 217, 206, 126, 213, 202, 123, 210, 130, 218, 208,
127, 215, 204, 124, 211, 201, 220, 209, 129, 217, 206, 126, 213, 202, 123, 211, 130,
218, 208, 128, 214, 204, 124, 212, 201, 121, 209, 129, 216, 205, 125, 213, 202, 123,
211, 131, 218, 207, 127, 215, 203, 124, 212, 202, 121, 209, 129, 217, 205, 125, 213,
203, 123, 210, 131, 218, 206, 127, 215, 204, 124, 212, 201, 220, 208, 128, 216, 206,
125, 213, 203, 123, 210, 130, 218

};

void verify_time( hour hr, minute mn, second sc, tenth te = 0, hundredth hd = 0, thousandth th = 0 )
{
   if( hr > 23 || mn > 59 || sc > 59 || te > 9 || hd > 9 || th > 9 )
      throw runtime_error( "invalid time" );
}

inline void seconds_to_components( seconds s, second& sc, tenth& te, hundredth& hd, thousandth& th )
{
   millisecond m = ( millisecond )( s * 1000.0 + 0.0005 );

   sc = ( second )( m / 1000 );
   m -= sc * 1000;

   te = ( tenth )( m / 100 );
   m -= te * 100;

   hd = ( hundredth )( m / 10 );
   m -= hd * 10;

   th = ( thousandth )m;
}

inline void millisecond_to_components( millisecond m, hour& hr, minute& mn, seconds& secs )
{
   hr = ( hour )( m / c_milliseconds_per_hour );
   m -= hr * c_milliseconds_per_hour;

   mn = ( minute )( m / c_milliseconds_per_minute );
   m -= mn * c_milliseconds_per_minute;

   secs = m / 1000.0;
}

inline void millisecond_to_components(
 millisecond m, hour& hr, minute& mn, second& sc, tenth& te, hundredth& hd, thousandth& th )
{
   hr = ( hour )( m / c_milliseconds_per_hour );
   m -= hr * c_milliseconds_per_hour;

   mn = ( minute )( m / c_milliseconds_per_minute );
   m -= mn * c_milliseconds_per_minute;

   sc = ( second )( m / 1000 );
   m -= sc * 1000;

   te = ( tenth )( m / 100 );
   m -= te * 100;

   hd = ( hundredth )( m / 10 );
   m -= hd * 10;

   th = ( thousandth )m;
}

inline millisecond components_to_millisecond( second sc, tenth te, hundredth hd, thousandth th )
{
   millisecond ms = 0;

   ms += sc * 1000;
   ms += te * 100;
   ms += hd * 10;
   ms += th;

   return ms;
}

inline double deg2rad( double deg )
{
   return ( deg * c_pi ) / 180.0;
}

inline double rad2deg( double rad )
{
   return ( rad * 180.0 ) / c_pi;
}

double adjust_360( double deg )
{
   while( deg < 0.0 )
      deg += 360.0;

   while( deg > 360.0 )
      deg -= 360.0;

   return deg;
}

// NOTE: This algorithm was taken from the Almanac for Computers 1990 which was published
// by the Nautical Almanac Office at the United States Naval Observatory in Washington DC
// (and should be accurate to around +-2 minutes).

bool determine_sun_rise_or_set( year yr, month mh, day dy,
 hour& hr, minute& mn, bool sunrise, int tzadjust, double latitude, double longitude, double zenith )
{
   // First calculate the day of the year...
   int N1 = 275 * ( int )mh / 9;
   int N2 = ( ( int )mh + 9 ) / 12;
   int N3 = 1 + ( yr - 4 * yr / 4 + 2 ) / 3;
   int N = N1 - ( N2 * N3 ) + dy - 30;

   // NOTE: Convert the longitude to an hour value and calculate an approximate time.
   double t, lngHour = longitude / 15.0;

   if( sunrise )
      t = N + ( ( 6.0 - lngHour ) / 24.0 );
   else
      t = N + ( ( 18.0 - lngHour ) / 24.0 );

   // NOTE: Calculate the Sun's mean anomaly.
   double M = ( 0.9856 * t ) - 3.289;

   // Calculate the Sun's true longitude...
   double L = M + ( 1.916 * sin( deg2rad( M ) ) ) + ( 0.020 * sin( 2 * deg2rad( M ) ) ) + 282.634;

   if( L < 0.0 )
      L += 360.0;
   else if( L >= 360.0 )
      L -= 360.0;

   // NOTE: Calculate the Sun's right ascension.
   double RA = rad2deg( atan( 0.91764 * tan( deg2rad( L ) ) ) );

   if( RA < 0.0 )
      RA += 360.0;
   else if( RA >= 360.0 )
      RA -= 360.0;

   // NOTE: The RA value needs to be in the same quadrant as L.
   double Lquadrant = ( floor( L / 90.0 ) ) * 90.0;
   double RAquadrant = ( floor( RA / 90.0 ) ) * 90.0;

   RA = RA + ( Lquadrant - RAquadrant );

   // NOTE: Convert the RA value to hours.
   RA = RA / 15.0;

   // NOTE: Calculate the Sun's declination.
   double sinDec = 0.39782 * sin( deg2rad( L ) );
   double cosDec = cos( asin( sinDec ) );

   // NOTE: Calculate the Sun's local hour angle.
   double cosH = ( cos( deg2rad( zenith ) )
    - ( sinDec * sin( deg2rad( latitude ) ) ) ) / ( cosDec * cos( deg2rad( latitude ) ) );

   // NOTE: Finish calculating H and convert into hours.
   double H;
   if( sunrise )
   {
      if( cosH > 1.0 )
         return false; // i.e. no sunrise

      H = 360 - rad2deg( acos( cosH ) );
   }
   else
   {
      if( cosH < -1.0 )
         return false; // i.e. no sunset

      H = rad2deg( acos( cosH ) );
   }

   H = H / 15;

   // NOTE: Calculate local mean time of rising/setting.
   double T = H + RA - ( 0.06571 * t ) - 6.622;

   // Adjust back to UTC...
   double UT = T - lngHour;

   if( UT < 0.0 )
      UT += 24.0;
   else if( UT >= 24.0 )
      UT -= 24.0;

   // NOTE: Convert UT value to local time zone of latitude/longitude.
   double localT = UT + tzadjust;

   if( localT < 0.0 )
      localT += 24.0;
   else if( localT > 24.0 )
      localT -= 24.0;

   hr = ( hour )localT;
   mn = ( minute )( ( localT - hr ) * 60.0 );

   return true;
}

// NOTE: Based on routines from "Practical Astronomy with Your Calculator" by Duffett-Smith.

double calculate_moon_phase( double days )
{
   double N, Msol, Ec, LambdaSol, l, Mm, Ev, Ac, A3, Mmprime, A4, lprime, V, ldprime, D;

   N = 360 * days / 365.2422;
   N = adjust_360( N );

   Msol = N + c_epsilon_g - c_rho_g;
   Msol = adjust_360( Msol );

   Ec = 360 / c_pi * c_eccentricity * sin( deg2rad( Msol ) );

   LambdaSol = N + Ec + c_epsilon_g;
   LambdaSol = adjust_360( LambdaSol );

   l = 13.1763966 * days + c_lzero;
   l = adjust_360( l );

   Mm = l - ( 0.1114041 * days ) - c_pzero;
   Mm = adjust_360( Mm );

   Ev = 1.2739 * sin( deg2rad( 2 * ( l - LambdaSol ) - Mm ) );

   Ac = 0.1858 * sin( deg2rad( Msol ) );
   A3 = 0.37 * sin( deg2rad( Msol ) );

   Mmprime = Mm + Ev - Ac - A3;

   Ec = 6.2886 * sin( deg2rad( Mmprime ) );

   A4 = 0.214 * sin( deg2rad( 2 * Mmprime ) );

   lprime = l + Ev + Ec - Ac + A4;

   V = 0.6583 * sin( deg2rad( 2 * ( lprime - LambdaSol ) ) );

   ldprime = lprime + V;

   D = ldprime - LambdaSol;

   return 50 * ( 1 - cos( deg2rad( D ) ) );
}

inline bool leap_year( year yr )
{
   return yr % 4 == 0 && ( yr % 100 != 0 || yr % 400 == 0 );
}

inline void daynum_to_calendar( daynum dn, year& yr, month& mo, day& dy )
{
   if( dn <= c_days_in_first_two_months )
   {
      yr = c_calendar_start_point;

      if( dn <= 30 )
      {
         mo = e_month_january;
         dy = dn + 1;
      }
      else
      {
         mo = e_month_february;
         dy = dn - 30;
      }
   }
   else
   {
      unsigned int i;

      i = ( 4 * ( dn - c_days_in_first_two_months ) ) - 1;

      dn = ( 4 * ( ( i % 146097 ) / 4 ) ) + 3;

      yr = ( year )( 100 * ( i / 146097 ) ) + ( dn / 1461 );

      i = ( 5 * ( ( ( dn % 1461 ) + 4 ) / 4 ) ) - 3;

      mo = ( month )( i / 153 );

      dy = ( ( i % 153 ) + 5 ) / 5;

      if( mo < 10 )
         mo = ( month )( mo + 3 );
      else
      {
         ++yr;
         mo = ( month )( mo - 9 );
      }

      yr += c_calendar_start_point;
   }
}

inline daynum calendar_to_daynum( year yr, month mo, day dy )
{
   unsigned int dn;

   if( yr == c_calendar_start_point && mo < 3 )
   {
      if( mo == 1 )
         dn = dy - 1;
      else
         dn = dy + 30;
   }
   else
   {
      if( mo > 2 )
         mo = ( month )( mo - 3 );
      else
      {
         --yr;
         mo = ( month )( mo + 9 );
      }

      yr -= c_calendar_start_point;

      dn = ( ( ( yr / 100 ) * 146097 ) / 4 ) + ( ( ( yr % 100 ) * 1461 ) / 4 )
       + ( ( ( 153 * mo ) + 2 ) / 5 ) + dy + c_days_in_first_two_months;
   }

   return dn;
}

inline bool is_date_gregorian( year yr, month mo, day dy )
{
   bool rc = false;

   if( yr > c_gregorian_year_begin )
      rc = true;
   else
   {
      if( yr == c_gregorian_year_begin )
      {
         if( mo > c_gregorian_month_begin )
            rc = true;
         else
         {
            if( mo == c_gregorian_month_begin && dy >= c_gregorian_day_begin )
               rc = true;
         }
      }
   }

   return rc;
}

inline void chinese_new_year_month_and_day( year yr, month& mo, day& dy )
{
   int i = chinese_new_years_month_and_day[ yr - c_calendar_start_point ];

   mo = ( month )( i / 100 );

   dy = ( day )( i - ( mo * 100 ) );
}

inline day day_in_year_for_easter_sunday( year yr )
{
   int gz = ( yr % 19 ) + 1;
   int jhd = int( yr / 100 ) + 1;
   int ksj = int( ( 3 * jhd ) / 4 ) - 12;
   int korr = int( ( 8 * jhd + 5 ) / 25 ) - 5;
   int so = int( ( 5 * yr ) / 4 ) - ksj - 10;
   int epakte = ( 11 * gz + 20 + korr - ksj ) % 30;

   if( ( epakte == 24 ) || ( ( epakte == 25 ) && ( gz > 11 ) ) )
      ++epakte;

   int n = 44 - epakte;

   if( n < 21 )
      n += 30;

   n += 7 - ( so + n ) % 7;
   n += leap_year( yr );

   return n + 59;
}

julian calendar_to_julian( year yr, month mo, day dy, hour hr, minute mn, seconds secs )
{
   julian jdt;
   int y, m, a, b;

   // NOTE: If the date is in January or February, it is considered
   // to instead be the 13th or 14th month of the preceding year.
   if( mo < e_month_march )
   {
      y = yr - 1;
      m = mo + 12;
   }
   else
   {
      y = yr;
      m = mo;
   }

   // NOTE: If the date is in the Gregorian calendar, compute a correction factor to take
   // account of the fact that century years are not leap years unless they are divisible
   // by 400 (if the date is in the Julian calendar then there is no correction).
   if( is_date_gregorian( yr, mo, dy ) )
   {
      a = y / 100;
      b = 2 - a + int( a / 4 );
   }
   else
      b = 0;

   jdt = int( 365.25 * ( y + 4716 ) ) + int( 30.6001 * ( m + 1 ) ) + b - 1524.5 + dy;
   jdt += ( ( ( double )hr * 3600.0 ) + ( ( double )mn * 60.0 ) + secs ) / c_seconds_per_day;

   return jdt;
}

void julian_to_calendar( julian jdt,
 year& yr, month& mo, day& dy, hour& hr, minute& mn, second& sc, tenth& te, hundredth& hd, thousandth& th )
{
   double d;
   double F;

   int A, B, C, D, E, Z;

   d = jdt + 0.5;
   Z = int( d );
   F = d - Z;

   if( Z < c_julian_start_day )
      A = Z;
   else
   {
      int alpha = int( ( Z - 1867216.25 ) / 36524.25 );
      A = Z + 1 + alpha - int( alpha / 4 );
   }

   B = A + 1524;

   C = int( ( B - 122.1 ) / 365.25 );
   D = int( 365.25 * C );
   E = int( ( B - D ) / 30.6001 );

   d = B - D - int( 30.6001 * E ) + F;

   if( E < 14 )
      mo = month( E - 1 );
   else
      mo = month( E - 13 );

   if( mo > e_month_february )
      yr = int( C - 4716 );
   else
      yr = int( C - 4715 );

   dy = int( d );

   // NOTE: Round the remainder to the nearest millisecond.
   millisecond m = ( millisecond )( ( ( d - dy ) * c_milliseconds_per_day ) + 0.5 );

   millisecond_to_components( m, hr, mn, sc, te, hd, th );
}

string chinese_calendar_year_name( year yr, bool as_english_animal_name )
{
   string str;

   int celestial_stem = 0;
   int terrestial_branch = 0;

   year target = ( yr + 56 ) % 60;

   for( int i = 0; i < 60; i++ )
   {
      if( i == target )
      {
         if( as_english_animal_name )
            str = string( terrestial_branches[ terrestial_branch ].p_english );
         else
         {
            str = string( celestial_stems[ celestial_stem ].p_str );
            str += ' ';
            str += string( terrestial_branches[ terrestial_branch ].p_str );
         }

         break;
      }

      if( ++celestial_stem > 9 )
         celestial_stem = 0;
      if( ++terrestial_branch > 11 )
         terrestial_branch = 0;
   }

   return str;
}

}

mtime::mtime( )
 :
 ms( 0 )
{
}

mtime::mtime( millisecond ms )
 :
 ms( ms )
{
}

mtime::mtime( const char* s )
{
   if( !s )
      throw runtime_error( "unexpected null ptr in mtime::mtime" );

   *this = mtime( string( s ) );
}

mtime::mtime( const std::string& s )
{
   if( s == "local" )
      *this = mtime::local( );
   else if( s == "standard" )
      *this = mtime::standard( );
   else
   {
      if( s.length( ) < 4 || s.length( ) > 12 )
         throw runtime_error( "invalid format for mtime (given '" + s + "' but expecting 'hhmm[ss[t[h[t]]]]' or 'hh:mm[:ss[.t[h[t]]]]')" );

      hour hr;
      minute mn;

      second sc( 0 );

      size_t mn_off, sc_off;

      if( isdigit( s[ 2 ] ) )
         mn_off = 2, sc_off = 4;
      else
         mn_off = 3, sc_off = 6;

      if( s.length( ) < 6 )
         sc_off = 0;

      hr = ( hour )( ( ( s[ 0 ] - '0' ) * 10 ) + ( s[ 1 ] - '0' ) );
      mn = ( minute )( ( ( s[ mn_off ] - '0' ) * 10 ) + ( s[ mn_off + 1 ] - '0' ) );

      if( ( sc_off == 6 ) && ( s.length( ) > 5 ) && ( s.length( ) < 8 ) )
         throw runtime_error( "invalid format for mtime (given '"
          + s + "' but expecting 'hhmm[ss[t[h[t]]]]' or 'hh:mm[:ss[.t[h[t]]]]')" );

      if( sc_off && ( s.length( ) > sc_off + 1 ) )
         sc = ( second )( ( ( s[ sc_off ] - '0' ) * 10 ) + ( s[ sc_off + 1 ] - '0' ) );

      tenth te( 0 );
      hundredth hd( 0 );
      thousandth th( 0 );

      if( sc_off == 4 )
      {
         if( s.length( ) > 6 )
            te = ( tenth )( s[ 6 ] - '0' );

         if( s.length( ) > 7 )
            hd = ( hundredth )( s[ 7 ] - '0' );

         if( s.length( ) > 8 )
            th = ( thousandth )( s[ 8 ] - '0' );
      }
      else if( s.length( ) >= 10 )
      {
         te = ( tenth )( s[ 9 ] - '0' );

         if( s.length( ) >= 11 )
         {
            hd = ( hundredth )( s[ 10 ] - '0' );

            if( s.length( ) == 12 )
               th = ( thousandth )( s[ 11 ] - '0' );
         }
      }

      verify_time( hr, mn, sc, te, hd, th );

      ms = ( hr * c_milliseconds_per_hour );
      ms += ( mn * c_milliseconds_per_minute );
      ms += components_to_millisecond( sc, te, hd, th );
   }
}

mtime::mtime( hour hr, minute mn )
{
   verify_time( hr, mn, 0 );

   ms = hr * c_milliseconds_per_hour;
   ms += mn * c_milliseconds_per_minute;
}

mtime::mtime( hour hr, minute mn, second sc )
{
   verify_time( hr, mn, sc );

   ms = hr * c_milliseconds_per_hour;
   ms += mn * c_milliseconds_per_minute;
   ms += sc * c_milliseconds_per_second;
}

mtime::mtime( hour hr, minute mn, second sc, millisecond m )
 :
 ms( m )
{
   if( m > 999 )
      throw runtime_error( "invalid millisecond" );

   verify_time( hr, mn, sc );

   ms += hr * c_milliseconds_per_hour;
   ms += mn * c_milliseconds_per_minute;
   ms += sc * c_milliseconds_per_second;
}

mtime::mtime( hour hr, minute mn, second sc, tenth te, hundredth hd, thousandth th )
{
   verify_time( hr, mn, sc, te, hd, th );

   ms = hr * c_milliseconds_per_hour;
   ms += mn * c_milliseconds_per_minute;
   ms += components_to_millisecond( sc, te, hd, th );
}

mtime::mtime( hour hr, minute mn, seconds s )
{
   if( s > 59.999 )
      throw runtime_error( "invalid time" );

   second sc;

   tenth te;
   hundredth hd;
   thousandth th;

   seconds_to_components( s, sc, te, hd, th );

   verify_time( hr, mn, sc, te, hd, th );

   ms = hr * c_milliseconds_per_hour;
   ms += mn * c_milliseconds_per_minute;
   ms += components_to_millisecond( sc, te, hd, th );
}

mtime::mtime( const mtime& src )
 :
 ms( src.ms )
{
}

mtime& mtime::operator =( const mtime& src )
{
   ms = src.ms;

   return *this;
}

mtime& mtime::operator =( const string& s )
{
   *this = mtime( s );

   return *this;
}

mtime& mtime::operator ++( )
{
   if( ms == c_max_millisecond )
      throw runtime_error( "time out of range" );

   ++ms;

   return *this;
}

mtime mtime::operator ++( int )
{
   mtime mt( *this );

   ++*this;

   return mt;
}

mtime& mtime::operator --( )
{
   if( ms == 0 )
      throw runtime_error( "time out of range" );

   --ms;

   return *this;
}

mtime mtime::operator --( int )
{
   mtime mt( *this );

   --*this;

   return mt;
}

mtime& mtime::operator +=( hours h )
{
   return operator +=( seconds( h.h * 3600 ) );
}

mtime& mtime::operator -=( hours h )
{
   return operator -=( seconds( h.h * 3600 ) );
}

mtime& mtime::operator +=( minutes m )
{
   return operator +=( seconds( m.m * 60 ) );
}

mtime& mtime::operator -=( minutes m )
{
   return operator -=( seconds( m.m * 60 ) );
}

mtime& mtime::operator +=( seconds s )
{
   if( s < 0.0 )
      return operator -=( s * -1.0 );

   return operator +=( ( milliseconds )( s * 1000 + 0.5 ) );
}

mtime& mtime::operator -=( seconds s )
{
   if( s < 0.0 )
      return operator +=( s * -1.0 );

   return operator -=( ( milliseconds )( s * 1000 + 0.5 ) );
}

mtime& mtime::operator +=( milliseconds m )
{
   if( m < 0 )
      return operator -=( m * -1 );

   milliseconds nms( ms );

   nms += m;

   if( nms < ms || nms > ( milliseconds )c_max_millisecond )
      throw runtime_error( "time out of range" );

   ms = ( millisecond )nms;

   return *this;
}

mtime& mtime::operator -=( milliseconds m )
{
   if( m < 0 )
      return operator +=( m * -1 );

   milliseconds nms( ms );

   nms -= m;

   if( nms > ms )
      throw runtime_error( "time out of range" );

   ms = ( millisecond )nms;

   return *this;
}

hour mtime::get_hour( ) const
{
   return hour( ms / c_milliseconds_per_hour );
}

minute mtime::get_minute( ) const
{
   hour hr;
   minute mn;

   millisecond m( ms );

   hr = ( hour )( m / c_milliseconds_per_hour );
   m -= hr * c_milliseconds_per_hour;

   mn = ( minute )( m / c_milliseconds_per_minute );

   return mn;
}

second mtime::get_second( ) const
{
   hour hr;
   minute mn;
   second sc;

   millisecond m( ms );

   hr = ( hour )( m / c_milliseconds_per_hour );
   m -= hr * c_milliseconds_per_hour;

   mn = ( minute )( m / c_milliseconds_per_minute );
   m -= mn * c_milliseconds_per_minute;

   sc = ( second )( m / 1000 );

   return sc;
}

string mtime::as_string( time_format tf, bool use_separators ) const
{
   string str( as_string( use_separators, true ) );

   switch( tf )
   {
      case e_time_format_hhmm:
      str = str.substr( 0, use_separators ? 5 : 4 );
      break;

      case e_time_format_hhmmss:
      str = str.substr( 0, use_separators ? 8 : 6 );
      break;

      case e_time_format_hhmmsst:
      str = str.substr( 0, use_separators ? 10 : 7 );
      break;

      case e_time_format_hhmmssth:
      str = str.substr( 0, use_separators ? 11 : 8 );
      break;

      case e_time_format_hhmmsstht:
      break;

      default:
      throw runtime_error( "unexpected time_format value #" + to_string( tf ) );
   }

   return str;
}

string mtime::as_string( bool use_separators, bool include_milliseconds ) const
{
   char hhmmssmmm[ ] = "hhmmssmmm";

   hour hr( get_hour( ) );

   hhmmssmmm[ 0 ] = '0' + ( hr / 10 );
   hhmmssmmm[ 1 ] = '0' + ( hr % 10 );

   minute mn( get_minute( ) );

   hhmmssmmm[ 2 ] = '0' + ( mn / 10 );
   hhmmssmmm[ 3 ] = '0' + ( mn % 10 );

   second sc( get_second( ) );

   hhmmssmmm[ 4 ] = '0' + ( sc / 10 );
   hhmmssmmm[ 5 ] = '0' + ( sc % 10 );

   if( !include_milliseconds )
      hhmmssmmm[ 6 ] = '\0';
   else
   {
      millisecond ms( get_millisecond( ) );

      hhmmssmmm[ 6 ] = '0' + ( ms / 100 );
      hhmmssmmm[ 7 ] = '0' + ( ( ms % 100 ) / 10 );
      hhmmssmmm[ 8 ] = '0' + ( ms % 10 );
   }

   if( !use_separators )
      return string( hhmmssmmm );
   else
   {
      string str( hhmmssmmm );

      str.insert( 2, 1, ':' );
      str.insert( 5, 1, ':' );

      if( include_milliseconds )
         str.insert( 8, 1, '.' );

      return str;
   }
}

mtime mtime::local( )
{
   time_t t;
   millisecond m = 0;

#ifdef __GNUG__
   struct timeval tv;
   ::gettimeofday( &tv, 0 );

   t = tv.tv_sec;
   m = tv.tv_usec / 1000;
#else
   t = time( 0 );
#endif

   struct tm* p_t;
   p_t = localtime( &t );

   return mtime( p_t->tm_hour, p_t->tm_min, p_t->tm_sec, m );
}

mtime mtime::standard( )
{
   time_t t;
   millisecond m = 0;

#ifdef __GNUG__
   struct timeval tv;
   ::gettimeofday( &tv, 0 );

   t = tv.tv_sec;
   m = tv.tv_usec / 1000;
#else
   t = time( 0 );
#endif

   struct tm* p_t;
   p_t = gmtime( &t );

   return mtime( p_t->tm_hour, p_t->tm_min, p_t->tm_sec, m );
}

mtime mtime::minimum( )
{
   return mtime( c_min_millisecond );
}

mtime mtime::maximum( )
{
   return mtime( c_max_millisecond );
}

bool operator <( const mtime& lhs, const mtime& rhs )
{
   return lhs.ms < rhs.ms;
}

bool operator <=( const mtime& lhs, const mtime& rhs )
{
   return lhs.ms <= rhs.ms;
}

bool operator >( const mtime& lhs, const mtime& rhs )
{
   return lhs.ms > rhs.ms;
}

bool operator >=( const mtime& lhs, const mtime& rhs )
{
   return lhs.ms >= rhs.ms;
}

bool operator ==( const mtime& lhs, const mtime& rhs )
{
   return lhs.ms == rhs.ms;
}

ostream& operator <<( ostream& os, const mtime& src )
{
   hour hr;
   minute mn;
   second sc;

   tenth te;
   hundredth hd;
   thousandth th;

   millisecond_to_components( src.ms, hr, mn, sc, te, hd, th );

   char buf[ 13 ] = "00:00:00.000";

   buf[ 0 ] = '0' + ( hr / 10 );
   buf[ 1 ] = '0' + ( hr % 10 );

   buf[ 3 ] = '0' + ( mn / 10 );
   buf[ 4 ] = '0' + ( mn % 10 );

   buf[ 6 ] = '0' + ( sc / 10 );
   buf[ 7 ] = '0' + ( sc % 10 );

   buf[ 9 ] = '0' + te;
   buf[ 10 ] = '0' + hd;
   buf[ 11 ] = '0' + th;

   os << buf;

   return os;
}

read_stream& operator >>( read_stream& rs, mtime& dest )
{
   rs >> dest.ms;

   return rs;
}

write_stream& operator <<( write_stream& ws, const mtime& src )
{
   ws << src.ms;

   return ws;
}

udate::udate( )
 :
 dn( c_min_day_number | c_day_number_in_use )
{
}

udate::udate( daynum dn )
 :
 dn( dn | c_day_number_in_use )
{
}

udate::udate( julian jdt )
{
   if( ( jdt < c_min_year_julian ) || ( jdt > c_max_year_julian ) )
      throw runtime_error( "date out of range" );

   dn = ( daynum )( jdt - c_min_year_julian + 0.5 );
   dn |= c_day_number_in_use;
}

udate::udate( const char* s )
{
   if( !s )
      throw runtime_error( "unexpected null ptr in udate::udate" );

   *this = udate( string( s ) );
}

udate::udate( const std::string& s )
{
   if( s == "local" )
      *this = udate::local( );
   else if( s == "standard" )
      *this = udate::standard( );
   else
   {
      if( ( s.length( ) != 8 ) && ( s.length( ) != 10 ) )
         throw runtime_error( "invalid format for udate (given '" + s + "' but expecting 'yyyymmdd' or 'yyyy-mm-dd')" );

      year yr;
      month mo;
      day dy;

      yr = 0;

      for( size_t i = 0; i < 4; i++ )
      {
         yr = yr * 10;
         yr += s[ i ] - '0';
      }

      size_t mo_off, dy_off;

      if( s.length( ) == 8 )
         mo_off = 4, dy_off = 6;
      else
         mo_off = 5, dy_off = 8;

      mo = ( month )( ( ( s[ mo_off ] - '0' ) * 10 ) + ( s[ mo_off + 1 ] - '0' ) );
      dy = ( day )( ( ( s[ dy_off ] - '0' ) * 10 ) + ( s[ dy_off + 1 ] - '0' ) );

      dn = 0;

      ymd.yr = yr;
      ymd.mo = mo;
      ymd.dy = dy;

      validate( );
   }
}

udate::udate( year yr, month mo, day dy )
{
   dn = 0;

   ymd.yr = yr;
   ymd.mo = mo;
   ymd.dy = dy;

   validate( );
}

udate::udate( year yr, month mo, weekday wd, occurrence occ )
{
   dn = 0;

   ymd.yr = yr;
   ymd.mo = mo;
   ymd.dy = 1;

   validate( );

   switch( occ )
   {
      case e_occurrence_first:
      case e_occurrence_second:
      case e_occurrence_third:
      case e_occurrence_fourth:
      {
         int num = ( int )occ;

         while( true )
         {
            weekday w = ( weekday )*this;

            if( w == wd )
               --num;

            if( num == 0 )
               break;

            ++*this;
         }
      }
      break;

      case e_occurrence_last:
      case e_occurrence_second_last:
      case e_occurrence_third_last:
      case e_occurrence_fourth_last:
      {
         days d = days_for_month[ ymd.mo - 1 ];

         if( ymd.mo == e_month_february && leap_year( ymd.yr ) )
            ++d;

         ymd.dy = d;

         int num = ( ( int )occ - ( int )e_occurrence_last ) + 1;

         while( true )
         {
            weekday w = ( weekday )*this;

            if( w == wd )
               --num;

            if( num == 0 )
               break;

            --*this;
         }
      }
      break;
   }
}

udate::udate( year yr, month mo, day_type dt, occurrence occ )
{
   dn = 0;

   ymd.yr = yr;
   ymd.mo = mo;
   ymd.dy = 1;

   validate( );

   switch( occ )
   {
      case e_occurrence_first:
      case e_occurrence_second:
      case e_occurrence_third:
      case e_occurrence_fourth:
      {
         int num = ( int )occ;

         while( true )
         {
            weekday w = ( weekday )*this;

            switch( dt )
            {
               case e_day_type_day:
               --num;
               break;

               case e_day_type_weekday:
               if( ( w != e_weekday_saturday ) && ( w != e_weekday_sunday ) )
                  --num;
               break;

               case e_day_type_weekendday:
               if( ( w == e_weekday_saturday ) || ( w == e_weekday_sunday ) )
                  --num;
               break;
            }

            if( num == 0 )
               break;

            ++*this;
         }
      }
      break;

      case e_occurrence_last:
      case e_occurrence_second_last:
      case e_occurrence_third_last:
      case e_occurrence_fourth_last:
      {
         days d = days_for_month[ ymd.mo - 1 ];

         if( ( ymd.mo == e_month_february ) && leap_year( ymd.yr ) )
            ++d;

         ymd.dy = d;

         int num = ( ( int )occ - ( int )e_occurrence_last ) + 1;

         while( true )
         {
            bool done = false;

            weekday w = ( weekday )*this;

            switch( dt )
            {
               case e_day_type_day:
               --num;
               break;

               case e_day_type_weekday:
               if( ( w != e_weekday_saturday ) && ( w != e_weekday_sunday ) )
                  --num;
               break;

               case e_day_type_weekendday:
               if( ( w == e_weekday_saturday ) || ( w == e_weekday_sunday ) )
                  --num;
               break;
            }

            if( num == 0 )
               break;

            --*this;
         }
      }

      break;
   }
}

udate::udate( year yr, month mo, day dy, day_type dt, occurrence occ )
{
   dn = 0;

   ymd.yr = yr;
   ymd.mo = mo;
   ymd.dy = dy;

   validate( );

   switch( occ )
   {
      case e_occurrence_first:
      case e_occurrence_second:
      case e_occurrence_third:
      case e_occurrence_fourth:
      {
         int num = ( int )occ;

         while( true )
         {
            weekday w = ( weekday )*this;

            switch( dt )
            {
               case e_day_type_day:
               --num;
               break;

               case e_day_type_weekday:
               if( ( w != e_weekday_saturday ) && ( w != e_weekday_sunday ) )
                  --num;
               break;

               case e_day_type_weekendday:
               if( ( w == e_weekday_saturday ) || ( w == e_weekday_sunday ) )
                  --num;
               break;
            }

            if( num == 0 )
               break;

            ++*this;
         }
      }

      break;

      case e_occurrence_last:
      case e_occurrence_second_last:
      case e_occurrence_third_last:
      case e_occurrence_fourth_last:
      {
         int num = ( ( int )occ - ( int )e_occurrence_last ) + 1;

         while( true )
         {
            bool done = false;
            weekday w = ( weekday )*this;

            switch( dt )
            {
               case e_day_type_day:
               --num;
               break;

               case e_day_type_weekday:
               if( ( w != e_weekday_saturday ) && ( w != e_weekday_sunday ) )
                  --num;
               break;

               case e_day_type_weekendday:
               if( ( w == e_weekday_saturday ) || ( w == e_weekday_sunday ) )
                  --num;
               break;
            }

            if( num == 0 )
               break;

            --*this;
         }
      }

      break;
   }
}

udate::udate( year yr, day_of_significance dos )
{
   dn = 0;

   if( ( yr < c_min_year ) || ( yr > c_max_year ) )
      throw runtime_error( "date out of range" );

   ymd.yr = yr;
   ymd.mo = e_month_january;
   ymd.dy = 1;

   if( dos == e_day_of_significance_easter_sunday )
   {
      days d = day_in_year_for_easter_sunday( yr );
      operator +=( d - 1 );
   }
   else if( dos == e_day_of_significance_chinese_new_year )
      chinese_new_year_month_and_day( ymd.yr, ymd.mo, ymd.dy );
   else
      throw runtime_error( "invalid calcudated_year_day value" );
}

udate::udate( const udate& src )
{
   dn = 0;

   if( src.dn & c_day_number_in_use )
      dn = src.dn;
   else
   {
      ymd.yr = src.ymd.yr;
      ymd.mo = src.ymd.mo;
      ymd.dy = src.ymd.dy;
   }
}

udate& udate::operator =( const udate& src )
{
   dn = 0;

   if( src.dn & c_day_number_in_use )
      dn = src.dn;
   else
   {
      ymd.yr = src.ymd.yr;
      ymd.mo = src.ymd.mo;
      ymd.dy = src.ymd.dy;
   }

   return *this;
}

udate& udate::operator =( const string& s )
{
   *this = udate( s );
   return *this;
}

bool udate::is_daynum( ) const
{
   return !is_calendar( );
}

bool udate::is_calendar( ) const
{
   return !( dn & c_day_number_in_use );
}

udate& udate::operator ++( )
{
   if( dn & c_day_number_in_use )
   {
      daynum n( dn & ~c_day_number_in_use );

      if( n == c_max_day_number )
         throw runtime_error( "date out of range" );

      dn = ++n | c_day_number_in_use;
   }
   else
   {
      if( ( ymd.yr == c_max_year )
       && ( ymd.mo == c_max_month ) && ( ymd.dy == c_max_day ) )
         throw runtime_error( "date out of range" );

      day max_day = days_for_month[ ymd.mo - 1 ];

      if( ( ymd.mo == e_month_february ) && leap_year( ymd.yr ) )
         ++max_day;

      if( ymd.dy == max_day )
      {
         ymd.dy = 1;

         if( ymd.mo < e_month_december )
            ymd.mo = ( month )( ymd.mo + 1 );
         else
         {
            ++ymd.yr;
            ymd.mo = e_month_january;
         }
      }
      else
         ++ymd.dy;
   }

   return *this;
}

udate udate::operator ++( int )
{
   udate dt( *this );

   ++*this;
   return dt;
}

udate& udate::operator --( )
{
   if( dn & c_day_number_in_use )
   {
      daynum n( dn & ~c_day_number_in_use );

      if( n == c_min_day_number )
         throw runtime_error( "date out of range" );

      dn = --n | c_day_number_in_use;
   }
   else
   {
      if( ( ymd.yr == c_min_year )
       && ( ymd.mo == c_min_month ) && ( ymd.dy == c_min_day ) )
         throw runtime_error( "date out of range" );

      if( ymd.dy > 1 )
         --ymd.dy;
      else
      {
         if( ymd.mo > e_month_january )
            ymd.mo = ( month )( ymd.mo - 1 );
         else
         {
            --ymd.yr;
            ymd.mo = e_month_december;
         }

         day max_day = days_for_month[ ymd.mo - 1 ];

         if( ( ymd.mo == e_month_february ) && leap_year( ymd.yr ) )
            ++max_day;

         ymd.dy = max_day;
      }
   }

   return *this;
}

udate udate::operator --( int )
{
   udate dt( *this );

   --*this;
   return dt;
}

udate& udate::operator +=( years y )
{
   if( y.y < 0 )
   {
      y.y *= -1;
      return operator -=( y );
   }

   if( dn & c_day_number_in_use )
   {
      daynum_to_calendar( dn & ~c_day_number_in_use, ymd.yr, ymd.mo, ymd.dy );
      dn &= ~c_day_number_in_use;
   }

   if( ( ymd.yr + y.y ) > c_max_year )
      throw runtime_error( "date out of range" );

   ymd.yr += y.y;

   if( ( ymd.dy == 29 )
    && ( ymd.mo == e_month_february ) && !leap_year( ymd.yr ) )
      --ymd.dy;

   return *this;
}

udate& udate::operator -=( years y )
{
   if( y.y < 0 )
   {
      y.y *= -1;
      return operator +=( y );
   }

   if( dn & c_day_number_in_use )
   {
      daynum_to_calendar( dn & ~c_day_number_in_use, ymd.yr, ymd.mo, ymd.dy );
      dn &= ~c_day_number_in_use;
   }

   if( ( ymd.yr - y.y ) < c_min_year )
      throw runtime_error( "date out of range" );

   ymd.yr -= y.y;

   if( ( ymd.dy == 29 )
    && ( ymd.mo == e_month_february ) && !leap_year( ymd.yr ) )
      --ymd.dy;

   return *this;
}

udate& udate::operator +=( months m )
{
   if( m.m < 0 )
   {
      m.m *= -1;
      return operator -=( m );
   }

   if( dn & c_day_number_in_use )
   {
      daynum_to_calendar( dn & ~c_day_number_in_use, ymd.yr, ymd.mo, ymd.dy );
      dn &= ~c_day_number_in_use;
   }

   int yr = m.m / 12;

   m.m -= ( yr * 12 );

   int mo = ymd.mo - 1 + m.m;

   if( mo > 11 )
   {
      ++yr;
      mo -= 12;
   }

   if( ymd.yr + yr > c_max_year )
      throw runtime_error( "date out of range" );

   ymd.yr += yr;
   ymd.mo = ( month )( mo + 1 );

   days d = days_for_month[ ymd.mo - 1 ];

   if( ymd.mo == e_month_february && leap_year( ymd.yr ) )
      ++d;

   if( ymd.dy > d )
      ymd.dy = d;

   return *this;
}

udate& udate::operator -=( months m )
{
   if( m.m < 0 )
   {
      m.m *= -1;
      return operator +=( m );
   }

   if( dn & c_day_number_in_use )
   {
      daynum_to_calendar( dn & ~c_day_number_in_use, ymd.yr, ymd.mo, ymd.dy );
      dn &= ~c_day_number_in_use;
   }

   int yr = m.m / 12;
   m.m -= ( yr * 12 );

   int mo = ymd.mo - 1 - m.m;

   if( mo < 0 )
   {
      ++yr;
      mo += 12;
   }

   if( ymd.yr - yr < c_min_year )
      throw runtime_error( "date out of range" );

   ymd.yr -= yr;
   ymd.mo = ( month )( mo + 1 );

   days d = days_for_month[ ymd.mo - 1 ];

   if( ( ymd.mo == e_month_february ) && leap_year( ymd.yr ) )
      ++d;

   if( ymd.dy > d )
      ymd.dy = d;

   return *this;
}

udate& udate::operator +=( days d )
{
   if( d < 0 )
      return operator -=( d * -1 );

   daynum n;

   if( dn & c_day_number_in_use )
      n = dn & ~c_day_number_in_use;
   else
      n = calendar_to_daynum( ymd.yr, ymd.mo, ymd.dy );

   daynum on = n;

   n += d;

   if( ( n < on ) || ( n > c_max_day_number ) )
      throw runtime_error( "date out of range" );

   dn = n | c_day_number_in_use;

   return *this;
}

udate& udate::operator -=( days d )
{
   if( d < 0 )
      return operator +=( d * -1 );

   daynum n;

   if( dn & c_day_number_in_use )
      n = dn & ~c_day_number_in_use;
   else
      n = calendar_to_daynum( ymd.yr, ymd.mo, ymd.dy );

   daynum on = n;

   n -= d;

   if( n < c_min_day_number || ( n > on ) )
      throw runtime_error( "date out of range" );

   dn = n | c_day_number_in_use;

   return *this;
}

udate::operator daynum( ) const
{
   daynum n;

   if( dn & c_day_number_in_use )
      n = dn & ~c_day_number_in_use;
   else
      n = calendar_to_daynum( ymd.yr, ymd.mo, ymd.dy );

   return n;
}

udate::operator julian( ) const
{
   julian j;

   if( dn & c_day_number_in_use )
      j = c_min_year_julian + ( dn & ~c_day_number_in_use );
   else
      j = calendar_to_julian( ymd.yr, ymd.mo, ymd.dy, 0, 0, 0 );

   return j;
}

udate::operator weekday( ) const
{
   daynum n;
   if( dn & c_day_number_in_use )
      n = ( dn & ~c_day_number_in_use );
   else
      n = calendar_to_daynum( ymd.yr, ymd.mo, ymd.dy );

   return ( weekday )( ( ( n + 5 ) % 7 ) + 1 );
}

year udate::get_year( ) const
{
   year yr;
   month mo;
   day dy;

   if( !( dn & c_day_number_in_use ) )
      yr = ymd.yr;
   else
      daynum_to_calendar( dn & ~c_day_number_in_use, yr, mo, dy );

   return yr;
}

month udate::get_month( ) const
{
   year yr;
   month mo;
   day dy;

   if( !( dn & c_day_number_in_use ) )
      mo = ymd.mo;
   else
      daynum_to_calendar( dn & ~c_day_number_in_use, yr, mo, dy );

   return mo;
}

day udate::get_day( ) const
{
   year yr;
   month mo;
   day dy;

   if( !( dn & c_day_number_in_use ) )
      dy = ymd.dy;
   else
      daynum_to_calendar( dn & ~c_day_number_in_use, yr, mo, dy );

   return dy;
}

string udate::as_string( bool use_separators ) const
{
   char yyyymmdd[ ] = "yyyymmdd";

   year yr( get_year( ) );

   yyyymmdd[ 0 ] = '0' + ( yr / 1000 );
   yyyymmdd[ 1 ] = '0' + ( ( yr % 1000 ) / 100 );
   yyyymmdd[ 2 ] = '0' + ( ( yr % 100 ) / 10 );
   yyyymmdd[ 3 ] = '0' + ( yr % 10 );

   month mo( get_month( ) );

   yyyymmdd[ 4 ] = '0' + ( mo / 10 );
   yyyymmdd[ 5 ] = '0' + ( mo % 10 );

   day dy( get_day( ) );

   yyyymmdd[ 6 ] = '0' + ( dy / 10 );
   yyyymmdd[ 7 ] = '0' + ( dy % 10 );

   if( !use_separators )
      return string( yyyymmdd );
   else
   {
      string str( yyyymmdd );

      str.insert( 4, 1, '-' );
      str.insert( 7, 1, '-' );

      return str;
   }
}

string udate::as_string( date_format df, bool use_separators ) const
{
   string str( as_string( use_separators ) );

   switch( df )
   {
      case e_date_format_mmdd:
      if( !use_separators )
         str = str.substr( 4 );
      else
         str = str.substr( 5 );
      break;

      case e_date_format_yymm:
      str = str.substr( 2, use_separators ? 5 : 4 );
      break;

      case e_date_format_yymmdd:
      str = str.substr( 2 );
      break;

      case e_date_format_yyyymmdd:
      break;

      default:
      throw runtime_error( "unexpected date_format #" + to_string( df ) );
   }

   return str;
}

days udate::get_day_of_year( ) const
{
   year yr;
   month mo;
   day dy;

   if( !( dn & c_day_number_in_use ) )
   {
      yr = ymd.yr;
      mo = ymd.mo;
      dy = ymd.dy;
   }
   else
      daynum_to_calendar( dn & ~c_day_number_in_use, yr, mo, dy );

   days d = 0;

   for( int i = 0; i < 11; i++ )
   {
      if( mo == ( month )( i + 1 ) )
         break;

      d += days_for_month[ i ];

      if( ( i == 1 ) && leap_year( yr ) )
         ++d;
   }

   d += dy;

   return d;
}

string udate::month_name( bool short_name ) const
{
   month m( get_month( ) );

   string full_name( month_full_names[ ( int )m - 1 ].p_str );

   return short_name ? full_name.substr( 0, 3 ) : full_name;
}

std::string udate::weekday_name( bool short_name ) const
{
   weekday w( ( weekday )*this );

   string full_name( weekday_full_names[ ( int )w - 1 ].p_str );

   return short_name ? full_name.substr( 0, 3 ) : full_name;
}

std::string udate::chinese_year_name( bool as_english_animal_name ) const
{
   year yr;
   month mo;
   day dy;

   if( !( dn & c_day_number_in_use ) )
      yr = ymd.yr;
   else
      daynum_to_calendar( dn & ~c_day_number_in_use, yr, mo, dy );

   return chinese_calendar_year_name( yr, as_english_animal_name );
}

bool udate::is_leap_year( ) const
{
   year yr;
   month mo;
   day dy;

   if( !( dn & c_day_number_in_use ) )
      yr = ymd.yr;
   else
      daynum_to_calendar( dn & ~c_day_number_in_use, yr, mo, dy );

   return leap_year( yr );
}

bool udate::is_weekend_day( ) const
{
   weekday wd = ( weekday )*this;

   return ( wd == e_weekday_saturday ) || ( wd == e_weekday_sunday );
}

days udate::days_in_year( ) const
{
   days d = 365;

   year yr;
   month mo;
   day dy;

   if( !( dn & c_day_number_in_use ) )
      yr = ymd.yr;
   else
      daynum_to_calendar( dn & ~c_day_number_in_use, yr, mo, dy );

   if( leap_year( yr ) )
      ++d;

   return d;
}

days udate::days_in_month( ) const
{
   year yr;
   month mo;
   day dy;

   if( !( dn & c_day_number_in_use ) )
   {
      yr = ymd.yr;
      mo = ymd.mo;
   }
   else
      daynum_to_calendar( dn & ~c_day_number_in_use, yr, mo, dy );

   days d = days_for_month[ mo - 1 ];

   if( ( mo == e_month_february ) && leap_year( yr ) )
      ++d;

   return d;
}

void udate::validate( ) const
{
   bool okay = true;

   if( !( dn & c_day_number_in_use ) )
   {
      if( ( ymd.yr < c_min_year ) || ( ymd.yr > c_max_year ) )
         okay = false;
      else
      {
         if( ( ymd.mo < e_month_january ) || ( ymd.mo > e_month_december ) )
            okay = false;
         else
         {
            day max_day = days_for_month[ ymd.mo - 1 ];

            if( ( ymd.mo == 2 ) && leap_year( ymd.yr ) )
               ++max_day;

            if( ( ymd.dy < 1 ) || ( ymd.dy > max_day ) )
               okay = false;
         }
      }
   }
   else
   {
      daynum d( dn & ~c_day_number_in_use );

      if( d > c_max_day_number )
         okay = false;
   }

   if( !okay )
      throw runtime_error( "date is invalid" );
}

void udate::convert_to_daynum( )
{
   if( !( dn & c_day_number_in_use ) )
   {
      dn = calendar_to_daynum( ymd.yr, ymd.mo, ymd.dy );

      dn |= c_day_number_in_use;
   }
}

void udate::convert_to_calendar( )
{
   if( dn & c_day_number_in_use )
   {
      daynum_to_calendar( dn & ~c_day_number_in_use, ymd.yr, ymd.mo, ymd.dy );

      dn &= ~c_day_number_in_use;
   }
}

void udate::print( ostream& os ) const
{
   if( dn & c_day_number_in_use )
   {
      daynum d( dn & ~c_day_number_in_use );

      os << d;
   }
   else
      os << ymd.yr << '-' << ( int )ymd.mo << '-' << ( int )ymd.dy;
}

udate udate::local( )
{
   time_t t;
   struct tm* p_t;

   t = time( 0 );
   p_t = localtime( &t );

   return udate( p_t->tm_year + 1900, ( month )( p_t->tm_mon + 1 ), p_t->tm_mday );
}

udate udate::standard( )
{
   time_t t;
   struct tm* p_t;

   t = time( 0 );
   p_t = gmtime( &t );

   return udate( p_t->tm_year + 1900, ( month )( p_t->tm_mon + 1 ), p_t->tm_mday );
}

udate udate::minimum( )
{
   return udate( c_min_day_number );
}

udate udate::maximum( )
{
   return udate( c_max_day_number );
}

year udate::minimum_year( )
{
   return c_min_year;
}

year udate::maximum_year( )
{
   return c_max_year;
}

bool operator <( const udate& lhs, const udate& rhs )
{
   daynum ldn;

   if( lhs.dn & c_day_number_in_use )
      ldn = lhs.dn & ~c_day_number_in_use;
   else
      ldn = calendar_to_daynum( lhs.ymd.yr, lhs.ymd.mo, lhs.ymd.dy );

   daynum rdn;

   if( rhs.dn & c_day_number_in_use )
      rdn = rhs.dn & ~c_day_number_in_use;
   else
      rdn = calendar_to_daynum( rhs.ymd.yr, rhs.ymd.mo, rhs.ymd.dy );

   return ldn < rdn;
}

bool operator <=( const udate& lhs, const udate& rhs )
{
   daynum ldn;

   if( lhs.dn & c_day_number_in_use )
      ldn = lhs.dn & ~c_day_number_in_use;
   else
      ldn = calendar_to_daynum( lhs.ymd.yr, lhs.ymd.mo, lhs.ymd.dy );

   daynum rdn;

   if( rhs.dn & c_day_number_in_use )
      rdn = rhs.dn & ~c_day_number_in_use;
   else
      rdn = calendar_to_daynum( rhs.ymd.yr, rhs.ymd.mo, rhs.ymd.dy );

   return ldn <= rdn;
}

bool operator >( const udate& lhs, const udate& rhs )
{
   daynum ldn;

   if( lhs.dn & c_day_number_in_use )
      ldn = lhs.dn & ~c_day_number_in_use;
   else
      ldn = calendar_to_daynum( lhs.ymd.yr, lhs.ymd.mo, lhs.ymd.dy );

   daynum rdn;

   if( rhs.dn & c_day_number_in_use )
      rdn = rhs.dn & ~c_day_number_in_use;
   else
      rdn = calendar_to_daynum( rhs.ymd.yr, rhs.ymd.mo, rhs.ymd.dy );

   return ldn > rdn;
}

bool operator >=( const udate& lhs, const udate& rhs )
{
   daynum ldn;

   if( lhs.dn & c_day_number_in_use )
      ldn = lhs.dn & ~c_day_number_in_use;
   else
      ldn = calendar_to_daynum( lhs.ymd.yr, lhs.ymd.mo, lhs.ymd.dy );

   daynum rdn;

   if( rhs.dn & c_day_number_in_use )
      rdn = rhs.dn & ~c_day_number_in_use;
   else
      rdn = calendar_to_daynum( rhs.ymd.yr, rhs.ymd.mo, rhs.ymd.dy );

   return ldn >= rdn;
}

bool operator ==( const udate& lhs, const udate& rhs )
{
   bool lhs_is_daynum = ( lhs.dn & c_day_number_in_use ) ? true : false;
   bool rhs_is_daynum = ( rhs.dn & c_day_number_in_use ) ? true : false;

   if( lhs_is_daynum == rhs_is_daynum )
   {
      if( lhs_is_daynum )
         return lhs.dn == rhs.dn;
      else
         return ( lhs.ymd.yr == rhs.ymd.yr ) && ( lhs.ymd.mo == rhs.ymd.mo ) && ( lhs.ymd.dy == rhs.ymd.dy );
   }
   else
   {
      daynum ldn;

      if( lhs.dn & c_day_number_in_use )
         ldn = lhs.dn & ~c_day_number_in_use;
      else
         ldn = calendar_to_daynum( lhs.ymd.yr, lhs.ymd.mo, lhs.ymd.dy );

      daynum rdn;

      if( rhs.dn & c_day_number_in_use )
         rdn = rhs.dn & ~c_day_number_in_use;
      else
         rdn = calendar_to_daynum( rhs.ymd.yr, rhs.ymd.mo, rhs.ymd.dy );

      return ldn == rdn;
   }
}

ostream& operator <<( ostream& os, const udate& src )
{
   year yr;
   month mo;
   day dy;

   if( !( src.dn & c_day_number_in_use ) )
   {
      yr = src.ymd.yr;
      mo = src.ymd.mo;
      dy = src.ymd.dy;
   }
   else
      daynum_to_calendar( src.dn & ~c_day_number_in_use, yr, mo, dy );

   char mmdd[ 6 ] = "00-00";

   mmdd[ 0 ] = '0' + ( mo / 10 );
   mmdd[ 1 ] = '0' + ( mo % 10 );

   mmdd[ 3 ] = '0' + ( dy / 10 );
   mmdd[ 4 ] = '0' + ( dy % 10 );

   os << yr << '-' << mmdd;

   return os;
}

read_stream& operator >>( read_stream& rs, udate& dest )
{
   rs >> dest.dn;
   return rs;
}

write_stream& operator <<( write_stream& ws, const udate& src )
{
   daynum dn( src.dn );

   if( !( dn & c_day_number_in_use ) )
      dn = calendar_to_daynum( src.ymd.yr, src.ymd.mo, src.ymd.dy ) | c_day_number_in_use;

   ws << dn;

   return ws;
}

date_time::date_time( julian j )
{
   construct_from_julian( j );
}

date_time::date_time( int64_t unix_time )
{
   julian j = ( unix_time / ( julian )c_seconds_per_day ) + c_unix_epoch;

   construct_from_julian( j );
}

date_time::date_time( const char* s )
{
   if( !s )
      throw runtime_error( "unexpected null ptr in date_time::date_time" );

   *this = date_time( string( s ) );
}

void date_time::construct_from_julian( julian j )
{
   year yr;
   month mo;
   day dy;

   hour hr;
   minute mn;
   second sc;

   tenth te;
   hundredth hd;
   thousandth th;

   julian_to_calendar( j, yr, mo, dy, hr, mn, sc, te, hd, th );

   ud = udate( yr, mo, dy );
   mt = mtime( hr, mn, sc, te, hd, th );
}

date_time::date_time( const string& s )
{
   if( s == "local" )
   {
      ud = udate::local( );
      mt = mtime::local( );
   }
   else if( s == "system" )
   {
      ud = udate::standard( );
      mt = mtime::standard( );
   }
   else
   {
      if( ( s.length( ) != 8 ) && ( s.length( ) != 10 )
       && ( ( s.length( ) < 12 ) || ( s.length( ) > 23 ) ) )
         throw runtime_error( "invalid format for date_time (given '"
          + s + "' but expecting 'yyyymmdd[hhmm[ss[t[h[t]]]]]' or 'yyyy-mm-dd[ hh:mm[:ss[.t[h[t]]]]]')" );

      if( isdigit( s[ 4 ] ) )
      {
         ud = udate( s.substr( 0, 8 ) );

         if( s.length( ) > 8 )
            mt = mtime( s.substr( 8 ) );
      }
      else
      {
         ud = udate( s.substr( 0, 10 ) );

         if( s.length( ) > 11 )
            mt = mtime( s.substr( 11 ) );
      }
   }
}

date_time::date_time( const udate& ud, const mtime& mt )
 :
 ud( ud ),
 mt( mt )
{
}

date_time::date_time( year yr, month mo, weekday wd, occurrence occ )
 :
 ud( yr, mo, wd, occ ),
 mt( 0, 0 )
{
}

date_time::date_time( year yr, month mo, weekday wd, occurrence occ, const mtime& mt )
 :
 ud( yr, mo, wd, occ ),
 mt( mt )
{
}

date_time::date_time( year yr, month mo, weekday wd, occurrence occ, hour hr, minute mn )
 :
 ud( yr, mo, wd, occ ),
 mt( hr, mn )
{
}

date_time::date_time( year yr, month mo, weekday wd, occurrence occ, hour hr, minute mn, second sc )
 :
 ud( yr, mo, wd, occ ),
 mt( hr, mn, sc )
{
}

date_time::date_time( year yr, month mo, day_type dt, occurrence occ )
 :
 ud( yr, mo, dt, occ ),
 mt( 0, 0 )
{
}

date_time::date_time( year yr, month mo, day_type dt, occurrence occ, const mtime& mt )
 :
 ud( yr, mo, dt, occ ),
 mt( mt )
{
}

date_time::date_time( year yr, month mo, day_type dt, occurrence occ, hour hr, minute mn )
 :
 ud( yr, mo, dt, occ ),
 mt( hr, mn )
{
}

date_time::date_time( year yr, month mo, day_type dt, occurrence occ, hour hr, minute mn, second sc )
 :
 ud( yr, mo, dt, occ ),
 mt( hr, mn, sc )
{
}

date_time::date_time( year yr, month mo, day dy, day_type dt, occurrence occ )
 :
 ud( yr, mo, dy, dt, occ ),
 mt( 0, 0 )
{
}

date_time::date_time( year yr, month mo, day dy, day_type dt, occurrence occ, const mtime& mt )
 :
 ud( yr, mo, dy, dt, occ ),
 mt( mt )
{
}

date_time::date_time( year yr, month mo, day dy, day_type dt, occurrence occ, hour hr, minute mn )
 :
 ud( yr, mo, dy, dt, occ ),
 mt( hr, mn )
{
}

date_time::date_time( year yr, month mo, day dy, day_type dt, occurrence occ, hour hr, minute mn, second sc )
 :
 ud( yr, mo, dy, dt, occ ),
 mt( hr, mn, sc )
{
}

date_time::date_time( year yr, month mo, day dy )
 :
 ud( yr, mo, dy ),
 mt( 0, 0 )
{
}

date_time::date_time( year yr, month mo, day dy, const mtime& mt )
 :
 ud( yr, mo, dy ),
 mt( mt )
{
}

date_time::date_time( year yr, month mo, day dy, hour hr, minute mn )
 :
 ud( yr, mo, dy ),
 mt( hr, mn )
{
}

date_time::date_time( year yr, month mo, day dy, hour hr, minute mn, second sc )
 :
 ud( yr, mo, dy ),
 mt( hr, mn, sc )
{
}

date_time::date_time( year yr, month mo, day dy, hour hr, minute mn, second sc, millisecond m )
 :
 ud( yr, mo, dy ),
 mt( hr, mn, sc, m )
{
}

date_time::date_time( year yr,
 month mo, day dy, hour hr, minute mn, second sc, tenth te, hundredth hd, thousandth th )
 :
 ud( yr, mo, dy ),
 mt( hr, mn, sc, te, hd, th )
{
}

date_time::date_time( year yr, day_of_significance dos )
 :
 ud( yr, dos ),
 mt( 0, 0 )
{
}

date_time::date_time( year yr, day_of_significance dos, const mtime& mt )
 :
 ud( yr, dos ),
 mt( mt )
{
}

date_time::date_time( year yr, month mo, day dy, time_of_significance tos, const locality& l )
 :
 ud( yr, mo, dy )
{
   bool ok;
   hour hr;
   minute mn;

   if( tos == e_time_of_significance_sun_rising )
      ok = determine_sun_rise_or_set( yr, mo, dy, hr, mn,
       true, l.tzadjust, l.latitude, l.longitude, c_zenith_official );
   else
      ok = determine_sun_rise_or_set( yr, mo, dy, hr, mn,
       false, l.tzadjust, l.latitude, l.longitude, c_zenith_official );

   if( !ok )
      *this = date_time( );
   else
      mt = mtime( hr, mn );
}

date_time::date_time( year yr, day_of_significance dos, time_of_significance tos, const locality& l )
 :
 ud( yr, dos )
{
   bool ok;
   hour hr;
   minute mn;

   if( tos == e_time_of_significance_sun_rising )
      ok = determine_sun_rise_or_set( yr, ud.get_month( ), ud.get_day( ), hr, mn,
       true, l.tzadjust, l.latitude, l.longitude, c_zenith_official );
   else
      ok = determine_sun_rise_or_set( yr, ud.get_month( ), ud.get_day( ), hr, mn,
       false, l.tzadjust, l.latitude, l.longitude, c_zenith_official );

   if( !ok )
      *this = date_time( );
   else
      mt = mtime( hr, mn );
}

date_time::date_time( year yr, month mo, day dy, hour hr, minute mn, seconds s )
 :
 ud( yr, mo, dy ),
 mt( hr, mn, s )
{
}

date_time::date_time( const date_time& src )
 :
 ud( src.ud ),
 mt( src.mt )
{
}

date_time& date_time::operator =( const date_time& src )
{
   ud = src.ud;
   mt = src.mt;

   return *this;
}

date_time& date_time::operator =( const string& s )
{
   *this = date_time( s );

   return *this;
}

date_time& date_time::operator ++( )
{
   ++ud;

   return *this;
}

date_time date_time::operator ++( int )
{
   date_time dt( *this );

   ++*this;

   return dt;
}

date_time& date_time::operator --( )
{
   --ud;

   return *this;
}

date_time date_time::operator --( int )
{
   date_time dt( *this );

   --*this;

   return dt;
}

date_time& date_time::operator +=( years y )
{
   ud += y;

   return *this;
}

date_time& date_time::operator -=( years y )
{
   ud -= y;

   return *this;
}

date_time& date_time::operator +=( months m )
{
   ud += m;

   return *this;
}

date_time& date_time::operator -=( months m )
{
   ud -= m;

   return *this;
}

date_time& date_time::operator +=( days d )
{
   ud += d;

   return *this;
}

date_time& date_time::operator -=( days d )
{
   ud -= d;

   return *this;
}

date_time& date_time::operator +=( hours h )
{
   return operator +=( seconds( h.h * 3600 ) );
}

date_time& date_time::operator -=( hours h )
{
   return operator -=( seconds( h.h * 3600 ) );
}

date_time& date_time::operator +=( minutes m )
{
   return operator +=( seconds( m.m * 60 ) );
}

date_time& date_time::operator -=( minutes m )
{
   return operator -=( seconds( m.m * 60 ) );
}

date_time& date_time::operator +=( seconds s )
{
   days d = ( days )( ( s / c_seconds_per_day ) + 0.5 );

   s -= ( d * c_seconds_per_day );

   operator +=( d );

   return operator +=( milliseconds( s * 1000 + 0.5 ) );
}

date_time& date_time::operator -=( seconds s )
{
   days d = ( days )( ( s / c_seconds_per_day ) + 0.5 );

   s -= ( d * c_seconds_per_day );

   operator -=( d );

   return operator -=( milliseconds( s * 1000 + 0.5 ) );
}

date_time& date_time::operator +=( milliseconds m )
{
   if( m < 0 )
      return operator -=( m * -1 );

   milliseconds ms( ( milliseconds )mt );

   ms += m;

   if( ms > ( milliseconds )c_max_millisecond )
   {
      days d = ms / c_milliseconds_per_day;
      ms -= ( d * c_milliseconds_per_day );

      ud += d;
      mt = ( millisecond )ms;
   }
   else
      mt += m;

   return *this;
}

date_time& date_time::operator -=( milliseconds m )
{
   if( m < 0 )
      return operator +=( m * -1 );

   milliseconds ms( ( milliseconds )mt );
   milliseconds oms( ms );

   ms -= m;

   if( ms < 0 )
   {
      ms = ms * -1;

      days d = ms / c_milliseconds_per_day;
      ms -= ( d * c_milliseconds_per_day );

      if( ms > 0 )
         ms = c_milliseconds_per_day - ms;

      if( ms > oms )
         ++d;

      ud -= d;
      mt = ( millisecond )ms;
   }
   else
      mt -= m;

   return *this;
}

date_time::operator julian( ) const
{
   julian j;

   year yr;
   month mo;
   day dy;

   if( !( ud.dn & c_day_number_in_use ) )
   {
      yr = ud.ymd.yr;
      mo = ud.ymd.mo;
      dy = ud.ymd.dy;
   }
   else
      daynum_to_calendar( ud.dn & ~c_day_number_in_use, yr, mo, dy );

   hour hr;
   minute mn;
   seconds secs;

   millisecond_to_components( ( milliseconds )mt, hr, mn, secs );

   j = calendar_to_julian( yr, mo, dy, hr, mn, secs );

   return j;
}

string date_time::as_string( time_format tf, bool use_separators ) const
{
   return ud.as_string( use_separators )
    + ( use_separators ? " " : "" ) + mt.as_string( tf, use_separators );
}

string date_time::as_string( bool use_separators, bool include_milliseconds ) const
{
   return ud.as_string( use_separators )
    + ( use_separators ? " " : "" ) + mt.as_string( use_separators, include_milliseconds );
}

double date_time::moon_phase( ) const
{
   date_time dte( c_epoch, e_month_january, 1 );

   seconds s = *this - dte;

   return calculate_moon_phase( s / c_seconds_per_day );
}

string date_time::moon_phase_description( ) const
{
   string str;

   double mp = moon_phase( );

   if( int( mp + 0.5 ) == 100 )
      str = "full moon";
   else if( int( mp + 0.5 ) == 0 )
      str = "new moon";
   else if( mp > 49.5 && mp < 51.5 )
   {
      date_time dt( *this );

      ++dt;

      double mpn = dt.moon_phase( );

      if( mpn > mp )
         str = "first quarter";
      else
         str = "last quarter";
   }
   else
   {
      date_time dt( *this );

      ++dt;

      double mpn = dt.moon_phase( );

      if( mpn > mp )
         str = "waxing ";
      else
         str = "waning ";

      if( ( int )( mp + 0.5 ) > 50 )
         str += "gibbous ";
      else
         str += "crescent ";

      int pc = ( int )( mp + 0.5 );

      if( pc >= 10 )
         str += ( '0' + pc / 10 );

      str += ( '0' + pc % 10 );

      str += "% of full";
   }

   return str;
}

date_time date_time::local( )
{
   udate ud( udate::local( ) );
   mtime mt( mtime::local( ) );

   return date_time( ud, mt );
}

date_time date_time::standard( )
{
   udate ud( udate::standard( ) );
   mtime mt( mtime::standard( ) );

   return date_time( ud, mt );
}

date_time date_time::minimum( )
{
   date_time dt;

   dt.ud = udate::minimum( );
   dt.mt = mtime::minimum( );

   return dt;
}

date_time date_time::maximum( )
{
   date_time dt;

   dt.ud = udate::maximum( );
   dt.mt = mtime::maximum( );

   return dt;
}

bool operator <( const date_time& lhs, const date_time& rhs )
{
   if( lhs.ud < rhs.ud )
      return true;

   if( lhs.ud > rhs.ud )
      return false;

   return lhs.mt < rhs.mt;
}

bool operator <=( const date_time& lhs, const date_time& rhs )
{
   if( lhs == rhs )
      return true;

   return lhs < rhs;
}

bool operator >( const date_time& lhs, const date_time& rhs )
{
   if( lhs.ud > rhs.ud )
      return true;

   if( lhs.ud < rhs.ud )
      return false;

   return lhs.mt > rhs.mt;
}

bool operator >=( const date_time& lhs, const date_time& rhs )
{
   if( lhs == rhs )
      return true;

   return lhs > rhs;
}

bool operator ==( const date_time& lhs, const date_time& rhs )
{
   return lhs.ud == rhs.ud && lhs.mt == rhs.mt;
}

seconds operator -( const date_time& lhs, const date_time& rhs )
{
   seconds lhss = ( ( daynum )lhs.ud * c_seconds_per_day ) + ( seconds )( lhs.mt );
   seconds rhss = ( ( daynum )rhs.ud * c_seconds_per_day ) + ( seconds )( rhs.mt );

   return lhss - rhss;
}

ostream& operator <<( ostream& os, const date_time& src )
{
   os << src.ud << ' ' << src.mt;

   return os;
}

read_stream& operator >>( read_stream& rs, date_time& dest )
{
   rs >> dest.ud >> dest.mt;

   return rs;
}

write_stream& operator <<( write_stream& ws, const date_time& src )
{
   ws << src.ud << src.mt;

   return ws;
}

seconds seconds_per_day( )
{
   return c_seconds_per_day;
}

year gregorian_year_begin( )
{
   return c_gregorian_year_begin;
}

month gregorian_month_begin( )
{
   return c_gregorian_month_begin;
}

day gregorian_day_begin( )
{
   return c_gregorian_day_begin;
}

day julian_day_of_month( )
{
   return c_julian_day_of_month;
}

days days_in_year( year yr )
{
   days d = 365;

   if( leap_year( yr ) )
      ++d;

   return d;
}

days days_in_month( year yr, month mh )
{
   days d = days_for_month[ mh - 1 ];

   if( mh == e_month_february && leap_year( yr ) )
      ++d;

   return d;
}

julian convert_calendar_to_julian( year yr, month mo, day dy, hour hr, minute mn, seconds secs )
{
   return calendar_to_julian( yr, mo, dy, hr, mn, secs );
}

void convert_julian_to_calendar( julian jdt, year& yr, month& mo, day& dy )
{
   hour hr;
   minute mn;
   second sc;

   tenth te;
   hundredth hd;
   thousandth th;

   julian_to_calendar( jdt, yr, mo, dy, hr, mn, sc, te, hd, th );
}

void convert_julian_to_calendar( julian jdt, year& yr, month& mo, day& dy, hour& hr, minute& mn, second& sc )
{
   tenth te;
   hundredth hd;
   thousandth th;

   julian_to_calendar( jdt, yr, mo, dy, hr, mn, sc, te, hd, th );
}

void convert_julian_to_calendar( julian jdt,
 year& yr, month& mo, day& dy, hour& hr, minute& mn, seconds& secs )
{
   second sc;

   tenth te;
   hundredth hd;
   thousandth th;

   julian_to_calendar( jdt, yr, mo, dy, hr, mn, sc, te, hd, th );

   secs = sc;

   secs += ( te * 0.1 );
   secs += ( hd * 0.01 );
   secs += ( th * 0.001 );
}

void convert_julian_to_calendar( julian jdt,
 year& yr, month& mo, day& dy, hour& hr, minute& mn, second& sc, tenth& te, hundredth& hd, thousandth& th )
{
   julian_to_calendar( jdt, yr, mo, dy, hr, mn, sc, te, hd, th );
}

int64_t unix_time( const date_time& dt, time_t* p_tt, bool force_epoch_if_earlier )
{
   julian j( dt );

   if( j < c_unix_epoch )
   {
      if( force_epoch_if_earlier )
         j = c_unix_epoch;
      else
         throw runtime_error( "unix time is not valid before the unix epoch" );
   }

   if( p_tt )
   {
      year y;
      month mo;
      day dy;
      hour hr;
      minute mn;
      second sc;

      convert_julian_to_calendar( j, y, mo, dy, hr, mn, sc );

      struct tm t;

      t.tm_sec = ( int )sc;
      t.tm_min = ( int )mn;
      t.tm_hour = ( int )hr;

      t.tm_mday = ( int )dy;
      t.tm_mon = ( int )mo - 1;
      t.tm_year = ( int )y - 1900;

      t.tm_yday = 0;
      t.tm_isdst = 0;

      *p_tt = mktime( &t );
   }

   return ( int64_t )( ( ( j - c_unix_epoch ) * ( julian )c_seconds_per_day ) + 0.5 );
}

int64_t seconds_between( const date_time& lhs, const date_time& rhs )
{
   int64_t amt = 0;

   julian jl( lhs );
   julian jr( rhs );

   amt = ( int64_t )( ( jr - jl ) * c_seconds_per_day );

   return amt;
}

string format_udate( const udate& ud, const string& mask )
{
   string s( mask );

   // NOTE: Some mask examples:
   //
   // d m yy
   // dd/mm/yy
   // yyyy-mm-dd

   string::size_type pos = s.find( "yyyy" );

   if( pos != string::npos )
      s.replace( pos, 4, to_string( ud.get_year( ) ) );
   else
   {
      pos = s.find( "yy" );

      if( pos != string::npos )
      {
         int y = ud.get_year( ) % 100;

         string yy( to_string( y ) );

         if( y < 10 )
            yy = '0' + yy;

         s.replace( pos, 2, yy );
      }
   }

   pos = s.find( "mm" );

   if( pos != string::npos )
   {
      int m = ud.get_month( );

      string mm( to_string( m ) );

      if( m < 10 )
         mm = '0' + mm;

      s.replace( pos, 2, mm );
   }
   else
   {
      pos = s.find( "m" );

      if( pos != string::npos )
      {
         s.erase( pos, 1 );
         s.insert( pos, to_string( ud.get_month( ) ) );
      }
   }

   pos = s.find( "dd" );

   if( pos != string::npos )
   {
      int d = ud.get_day( );

      string dd( to_string( d ) );

      if( d < 10 )
         dd = '0' + dd;

      s.replace( pos, 2, dd );
   }
   else
   {
      pos = s.find( "d" );

      if( pos != string::npos )
      {
         s.erase( pos, 1 );
         s.insert( pos, to_string( ( int )ud.get_day( ) ) );
      }
   }

   return s;
}

string format_mtime( const mtime& mt, const string& mask )
{
   string s( lower( mask ) );

   // NOTE: Some mask examples:
   //
   // hh:mm:ss.tht
   // h:mm:ss AM
   // h:m:s.t

   bool upper = false;

   if( s != mask )
      upper = true;

   bool is_pm = false;

   string::size_type am_pm_pos = s.find( "am" );

   if( am_pm_pos == string::npos )
      am_pm_pos = s.find( "pm" );

   int h = mt.get_hour( );

   if( h > 12 )
   {
      is_pm = true;

      if( am_pm_pos != string::npos )
         h -= 12;
   }

   string::size_type pos = s.find( "hh" );

   if( pos != string::npos )
   {
      string hh( to_string( h ) );

      if( h < 10 )
         hh = '0' + hh;

      s.replace( pos, 2, hh );
   }
   else
   {
      pos = s.find( "h" );

      if( pos != string::npos )
      {
         s.erase( pos, 1 );
         s.insert( pos, to_string( h ) );
      }
   }

   int m = mt.get_minute( );

   pos = s.find( "mm" );

   if( pos != string::npos )
   {
      string mm( to_string( m ) );

      if( m < 10 )
         mm = '0' + mm;

      s.replace( pos, 2, mm );
   }
   else
   {
      pos = s.find( "m" );

      if( pos != string::npos )
      {
         s.erase( pos, 1 );
         s.insert( pos, to_string( m ) );
      }
   }

   int sec = mt.get_second( );

   pos = s.find( "ss" );

   if( pos != string::npos )
   {
      string ss( to_string( sec ) );

      if( sec < 10 )
         ss = '0' + ss;

      s.replace( pos, 2, ss );
   }
   else
   {
      pos = s.find( "s" );

      if( pos != string::npos )
      {
         s.erase( pos, 1 );
         s.insert( pos, to_string( sec ) );
      }
   }

   int milli = mt.get_millisecond( );

   pos = s.find( "tht" );

   if( pos != string::npos )
   {
      string tht( to_string( milli ) );

      if( milli < 100 )
         tht = '0' + tht;

      if( milli < 10 )
         tht = '0' + tht;

      s.replace( pos, 3, tht );
   }
   else
   {
      pos = s.find( "th" );

      if( pos != string::npos )
      {
         int hundredths = milli / 10;

         string th( to_string( hundredths ) );

         if( hundredths < 10 )
            th = '0' + th;

         s.replace( pos, 2, th );
      }
      else
      {
         pos = s.find( "t" );

         if( pos != string::npos )
         {
            int tenths = milli / 100;

            s.replace( pos, 1, to_string( tenths ) );
         }
      }
   }

   // NOTE: Need to find the AM/PM pos again as the string size may have changed.
   am_pm_pos = s.find( "am" );

   if( am_pm_pos == string::npos )
      am_pm_pos = s.find( "pm" );

   if( am_pm_pos != string::npos )
   {
      if( is_pm )
      {
         if( upper )
            s.replace( am_pm_pos, 2, "PM" );
         else
            s.replace( am_pm_pos, 2, "pm" );
      }
      else
      {
         if( upper )
            s.replace( am_pm_pos, 2, "AM" );
         else
            s.replace( am_pm_pos, 2, "am" );
      }
   }

   return s;
}

string format_date_time( const date_time& dt, const string& dmask, const string& tmask )
{
   return format_udate( dt.get_date( ), dmask ) + format_mtime( dt.get_time( ), tmask );
}
