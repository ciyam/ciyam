// Copyright (c) 2003-2012 CIYAM Pty. Ltd. ACN 093 704 539
// Copyright (c) 2012-2026 CIYAM Developers
//
// Distributed under the MIT/X11 software license, please refer to the file license.txt
// in the root project directory or http://www.opensource.org/licenses/mit-license.php.

#ifdef PRECOMPILE_H
#  include "precompile.h"
#endif
#pragma hdrstop

#ifndef HAS_PRECOMPILED_STD_HEADERS
#  include <iostream>
#  ifdef __GNUG__
#     include <unistd.h>
#  endif
#endif

#include "command_parser.h"

#include "config.h"
#include "macros.h"
#include "console.h"
#include "utilities.h"

#ifdef __GNUG__
#  ifdef RDLINE_SUPPORT
extern "C"
{
#     include <readline/history.h>
}
#  endif
#endif

//#define DEBUG

using namespace std;

/*

Format for node expressions:

"type/prefix[=[default]][/parameter[#value][/description[/separator[/terminator]]]]"

Allowed values for type are:
opt
pat (a regular expression pattern)
val
oval (same as val but will permit a blank value)
list
olist (same as list but will permit blank values)

By default a list will use a comma as the list separator unless a specific separator character
has been provided. A terminator is only permitted for a list type and a separator provided for
either an option or value will be stripped of the end of the value (if was found at the end so
in that case can act as a terminator).

A parameter with an empty prefix must have a name - if no name is provided then the parameter name is
considered to be the same as its prefix. For "pat" type expressions the prefix is a regular expression
pattern.

If a parameter has no prefix then it cannot have an empty value unless provided via "", or through the
use of leading/trailing/consecutive list separators, or by the use of a list terminator on its own. An
unprefixed parameter cannot have any other unprefixed alternative parameter nor can it be the first
parameter in an optional branch.

The "default" value for a prefix is only applicable to val or oval nodes within an optional branch where
there is at least one following matching node.

Syntax and argument examples:
-----------------------------

syntax <opt/cvs>{<opt/add>[<opt/-kb/binary>]<list//files/new files/ >}|{<opt/remove><list//files/existing files/ >}
command cvs add -kb one two three
command cvs remove one two three

syntax <opt/filter>[<opt/-nodups/nodups>][<val/-a/after>][<val/-b/before>]<val/@/listfile>|<list//strings// >
command filter -afirst -bsecond -nodups @test.lst
command filter -afirst -bsecond -nodups s1 s2 s3 s4

syntax <opt/bcc32>[<list/-D/defs//;>][<opt/-v/debug#on>]<list//objs// /,><val//exe>[<oval//map//,>][<list//libs// >]
command bcc32 -DONE -v -DTWO a.obj b.obj, x.exe
command bcc32 -DONE -v -DTWO a.obj b.obj, x.exe a.lib b.lib
command bcc32 -DONE -v -DTWO a.obj b.obj, x.exe , a.lib b.lib
command bcc32 -v -DONE;TWO a.obj b.obj, x.exe x.map, a.lib b.lib

syntax <opt/cvscheck>[<opt/-q/quiet>][<opt/-r/return>][<val/-p/path>][<list/-i/masks>][<val/-m/marker>]<list//files// >
command cvscheck -pC:\\Work -i*.h,*.cpp -mTEST -q file1.h file1.cpp file2.h file2.cpp
command cvscheck -r -i*.cpp file1.cpp file2.cpp

*/

struct syntax_text
{
   const char* p_syntax;
   bool test_should_succeed;
}
syntax_texts[ ] =
{
   { "<", false },
   { ">", false },
   { "|", false },
   { "[", false },
   { "]", false },
   { "||", false },
   { "[]", false },
   { "][", false },
   { "<<", false },
   { ">>", false },
   { "<>", false },
   { "{}", false },
   { "[{}]", false },
   { "[{<>}]", false },
   { "<opt/a>", true },
   { "[<opt/a>]", true },
   { "{<opt/a>}", true },
   { "|<opt/a>", false },
   { "<opt/a>|", false },
   { "[<opt/a>", false },
   { "<pat/[0-", false },
   { "<opt/a>]", false },
   { "|<opt/a>|", false },
   { "<pat/[0-9", false },
   { "<opt/abcd>", true },
   { "<<opt/abcd>", false },
   { "<opt/abcd>>", false },
   { "[[<opt/a>]]", false },
   { "<opt/a><opt/b>", true },
   { "<pat/[0-*abc/>", false },
   { "<opt/a>|<opt/b>", true },
   { "[<opt/a>]<opt/b>", true },
   { "<opt/a>{<opt/b>}", true },
   { "<opt/a>[<opt/b>]", true },
   { "[<opt/a><opt/b>]", true },
   { "{<opt/a><opt/b>}", true },
   { "|<opt/a>|<opt/b>", false },
   { "<opt/a>|<opt/b>|", false },
   { "<pat/[0-9]*abc/>", true },
   { "[<opt/a>|<opt/b>]", true },
   { "{<opt/a>}|<opt/b>", true },
   { "[<opt/a>]|<opt/b>", false },
   { "<opt/a>|[<opt/b>]", false },
   { "[<opt/a>][<opt/b>]", true },
   { "[<opt/a>{<opt/b>}]", true },
   { "{[<opt/a>]<opt/b>}", true },
   { "[<opt/a>]{<opt/b>}", true },
   { "[<opt/a>]|[<opt/b>]", false },
   { "[[<opt/a>][<opt/b>]]", false },
   { "<opt/a><opt/b><opt/c>", true },
   { "<opt/a>|<opt/b><opt/c>", true },
   { "<opt/a><opt/b>|<opt/c>", true },
   { "<opt/a>|<opt/b>|<opt/c>", true },
   { "[<opt/a>]<opt/b>|<opt/c>", true },
   { "<opt/a>|[<opt/b>]<opt/c>", true },
   { "<opt/a>[<opt/b>|<opt/c>]", true },
   { "<opt/a>|<opt/b>[<opt/c>]", true },
   { "[<opt/a>]<opt/b>|<opt/c>", true },
   { "<opt/a>[<opt/b>]|<opt/c>", true },
   { "<opt/a>[<opt/b>[<opt/c>]]", true },
   { "[<opt/a>]<opt/b>[<opt/c>]", true },
   { "<opt/a>|<opt/b>|[<opt/c>]", false },
   { "{<opt/a>|<opt/b>}[<opt/c>]", true },
   { "[<opt/a>]<opt/b>|[<opt/c>]", false },
   { "<opt/a>[<opt/b>[<opt/c>]]]", false },
   { "[[<opt/a>]<opt/b>[<opt/c>]", false },
   { "[<opt/a>]<opt/b>[<opt/c>]]", false },
   { "<opt/a>[[<opt/b>[<opt/c>]]]", false },
   { "<pat/^[0-9]*abc.*[a-z0-9]$/>", true },
   { "<opt/a>[<opt/b>|<opt/c>]<opt/d>", true },
   { "<opt/a>|<opt/b>|<opt/c>{<opt/d>", false },
   { "<opt/a>|<opt/b>|<opt/c>{<opt/d>}", true },
   { "<opt/a>|<opt/b>|<opt/c>{<opt/d>}}", false },
   { "<opt/a>|[<opt/b>]<opt/c>|<opt/d>", true },
   { "<opt/a>[<opt/b>]|<opt/c>|<opt/d>", true },
   { "<opt/a>[[<opt/b>|<opt/c>]<opt/d>]", true },
   { "<opt/a>|[<opt/b>]|<opt/c>|<opt/d>", false },
   { "{<opt/a>|<opt/b>[<opt/c>]}<opt/d>", true },
   { "<opt/a>{<opt/b>{[<opt/c>]}|<opt/d>", false },
   { "{<opt/a>|<opt/b>[<opt/c>]}|<opt/d>", true },
   { "<pat/\\<[A-Za-z][A-Za-z0-9]*[^\\>]*\\>", false },
   { "<pat/\\<[A-Za-z][A-Za-z0-9]*[^\\>]*\\>/>", true },
   { "<opt/a>{<opt/b>{[<opt/c>]}}|<opt/d>", true },
   { "<opt/a>{<opt/b>{[<opt/c>]}}|<opt/d>}", false },
   { "<opt/a>{<opt/b>{[<opt/c>]}}|<opt/d>}", false },
   { "<opt/a>{<opt/b>{[<opt/c>]}}|<opt/d>}}", false },
   { "[<opt/a>{[<opt/b>]}|<opt/c>{[<opt/d>]}]", true },
   { "{<opt/a>|<opt/b>[<opt/c>]}<opt/d>|<opt/e>", true },
   { "<opt/a>[<opt/b>|{<opt/c><opt/d>[<opt/e>]}]", true },
   { "{<opt/a>|<opt/b>[<opt/c>]}<opt/d>[<opt/e>]", true },
   { "{<opt/a>|<opt/b>[<opt/c>]}[<opt/d>]|<opt/e>", true },
   { "<pat/\\<([A-Za-z][A-Za-z0-9]*)[^\\>]*\\>.*\\<\\\\/\\\\\\\\1\\>/>", true },
   { "[<opt/a>{[<opt/b>]}|<opt/c>{[<opt/d>]}]<opt/e>", true },
   { "[<opt/a>{[<opt/b>]}|<opt/c>{[<opt/d>]}]<opt/e>]", false },
   { "[<opt/a>{[<opt/b>]}|<opt/c>{[<opt/d>]}]<opt/e>}", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]}|<opt/f>]", false },
   { "[<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]}|<opt/f>", false },
   { "[<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]}|<opt/f>]", true }
};

struct command_test
{
   const char* p_syntax;
   const char* p_command;
   bool test_should_succeed;
}
command_tests[ ] =
{
   { "<opt/a>", "", false },
   { "<opt/a>", "a", true },
   { "<opt/a>", "b", false },
   { "<opt/a>", "a a", false },
   { "[<opt/a>]", "", true },
   { "[<opt/a>]", "a", true },
   { "[<opt/a>]", "b", false },
   { "[<opt/a>]", "a a", false },
   { "{<opt/a>}", "", false },
   { "{<opt/a>}", "a", true },
   { "<pat/[0-9]>", "", false },
   { "<pat/[0-9]>", " ", false },
   { "<pat/[0-9]>", "a", false },
   { "<pat/[0-9]>", "0", true },
   { "<pat/[0-9]>", "9", true },
   { "<pat/[0-9]>", " 8 ", true },
   { "<pat/[0-9]>", "a1b", true },
   { "<pat/[0-9]>", "a b", false },
   { "<opt/a><opt/b>", "", false },
   { "<opt/a><opt/b>", "a", false },
   { "<opt/a><opt/b>", "b", false },
   { "<opt/a><opt/b>", "a b", true },
   { "<opt/a>|<opt/b>", "", false },
   { "<opt/a>|<opt/b>", "a", true },
   { "<opt/a>|<opt/b>", "b", true },
   { "<opt/a>|<opt/b>", "a b", false },
   { "<opt/a>|<opt/b>", "a a b", false },
   { "<opt/a>[<opt/b>]", "", false },
   { "<opt/a>[<opt/b>]", "a", true },
   { "<opt/a>[<opt/b>]", "b", false },
   { "<opt/a>[<opt/b>]", "a b", true },
   { "{<opt/a>}|<opt/b>", "", false },
   { "{<opt/a>}|<opt/b>", "a", true },
   { "{<opt/a>}|<opt/b>", "b", true },
   { "{<opt/a>}|<opt/b>", "a b", false },
   { "[<opt/a>|<opt/b>]", "", true },
   { "[<opt/a>|<opt/b>]", "a", true },
   { "[<opt/a>|<opt/b>]", "b", true },
   { "[<opt/a>|<opt/b>]", "a a", false },
   { "[<opt/a>|<opt/b>]", "a b", false },
   { "[<opt/a>|<opt/b>]", "b b", false },
   { "[<opt/a>][<opt/b>]", "", true },
   { "[<opt/a>][<opt/b>]", "a", true },
   { "[<opt/a>][<opt/b>]", "b", true },
   { "[<opt/a>][<opt/b>]", "c", false },
   { "[<opt/a>][<opt/b>]", "a b", true },
   { "[<opt/a>][<opt/b>]", "b a", true },
   { "[<opt/a>][<opt/b>]", "a b a", false },
   { "{<opt/a>}|{<opt/b>}", "", false },
   { "{<opt/a>}|{<opt/b>}", "a", true },
   { "{<opt/a>}|{<opt/b>}", "b", true },
   { "{<opt/a>}|{<opt/b>}", "c", false },
   { "{<opt/a>}|{<opt/b>}", "a a", false },
   { "{<opt/a>}|{<opt/b>}", "a b", false },
   { "{<opt/a>}|{<opt/b>}", "b b", false },
   { "<pat/[0-9]*abc/test>", "abc", true },
   { "<pat/[0-9]*abc/test>", "1abc", true },
   { "<pat/[0-9]*abc/test>", "xabc", true },
   { "<pat/[0-9]*abc/test>", "123abc", true },
   { "<pat/[0-9]*abc/test>", "x123abc", true },
   { "<pat/[0-9]*abc/test>", "x123ab", false },
   { "<pat/[0-9].abc/test>", "abc", false },
   { "<pat/[0-9].abc/test>", "0abc", false },
   { "<pat/[0-9].abc/test>", "0xabc", true },
   { "<pat/[0-9].abc/test>", "x9xabc", true },
   { "<pat/[0-9]+abc/test>", "abc", false },
   { "<pat/[0-9]+abc/test>", "0abc", true },
   { "<pat/[0-9]+abc/test>", "01abc", true },
   { "<pat/[0-9]+abc/test>", "0xabc", false },
   { "<pat/[0-9]+abc/test>", "x012abcd", true },
   { "<opt/a><opt/b><opt/c>", "", false },
   { "<opt/a><opt/b><opt/c>", "a", false },
   { "<opt/a><opt/b><opt/c>", "b", false },
   { "<opt/a><opt/b><opt/c>", "c", false },
   { "<opt/a><opt/b><opt/c>", "a b", false },
   { "<opt/a><opt/b><opt/c>", "a c", false },
   { "<opt/a><opt/b><opt/c>", "b c", false },
   { "<opt/a><opt/b><opt/c>", "b a", false },
   { "<opt/a><opt/b><opt/c>", "c a", false },
   { "<opt/a><opt/b><opt/c>", "a b c", true },
   { "<opt/a>|<opt/b><opt/c>", "", false },
   { "<opt/a>|<opt/b><opt/c>", "a", false },
   { "<opt/a>|<opt/b><opt/c>", "b", false },
   { "<opt/a>|<opt/b><opt/c>", "c", false },
   { "<opt/a>|<opt/b><opt/c>", "a a", false },
   { "<opt/a>|<opt/b><opt/c>", "a b", false },
   { "<opt/a>|<opt/b><opt/c>", "a c", true },
   { "<opt/a>|<opt/b><opt/c>", "b c", true },
   { "<opt/a>|<opt/b><opt/c>", "a b c", false },
   { "<opt/a>|<opt/b><opt/c>", "a c a", false },
   { "<opt/a>|<opt/b><opt/c>", "b c a", false },
   { "<opt/a>|<opt/b><opt/c>", "c b a", false },
   { "<opt/a>[<opt/b><opt/c>]", "", false },
   { "<opt/a>[<opt/b><opt/c>]", "a", true },
   { "<opt/a>[<opt/b><opt/c>]", "a b", false },
   { "<opt/a>[<opt/b><opt/c>]", "b c", false },
   { "<opt/a>[<opt/b><opt/c>]", "a b c", true },
   { "<opt/a>[<opt/b><opt/c>]", "a a b c", false },
   { "<opt/a>[<opt/b><opt/c>]", "a b b c", false },
   { "<opt/a>[<opt/b><opt/c>]", "a b c c", false },
   { "<opt/a>|<opt/b>|<opt/c>", "", false },
   { "<opt/a>|<opt/b>|<opt/c>", "a", true },
   { "<opt/a>|<opt/b>|<opt/c>", "b", true },
   { "<opt/a>|<opt/b>|<opt/c>", "c", true },
   { "<opt/a>|<opt/b>|<opt/c>", "a b", false },
   { "<opt/a>|<opt/b>|<opt/c>", "a c", false },
   { "<opt/a>|<opt/b>|<opt/c>", "b c", false },
   { "<opt/a>|<opt/b>|<opt/c>", "a b c", false },
   { "<opt/a>[<opt/b>]<opt/c>", "a", false },
   { "<opt/a>[<opt/b>]<opt/c>", "a b", false },
   { "<opt/a>[<opt/b>]<opt/c>", "a c", true },
   { "<opt/a>[<opt/b>]<opt/c>", "b c", false },
   { "<opt/a>[<opt/b>]<opt/c>", "a b c", true },
   { "<opt/a>[<opt/b>]<opt/c>", "a b c b", false },
   { "<opt/a>[<opt/b>]<opt/c>", "a a b c", false },
   { "<opt/a>[<opt/b>]<opt/c>", "a b b c", false },
   { "<pat/[0-9]{2,4}/test>", "abc", false },
   { "<pat/[0-9]{2,4}/test>", "1abc", false },
   { "<pat/[0-9]{2,4}/test>", "1abc1", false },
   { "<pat/[0-9]{2,4}/test>", "abc11", true },
   { "<pat/[0-9]{2,4}/test>", "00abc", true },
   { "<pat/[0-9]{2,4}/test>", "11abc", true },
   { "<pat/[0-9]{2,4}/test>", "01abc", true },
   { "<pat/[0-9]{2,4}/test>", "10abc", true },
   { "<pat/[0-9]{2,4}/test>", "1abc11", true },
   { "<pat/[0-9]{2,4}/test>", "01abc01", true },
   { "<pat/[0-9]{2,4}/test>", "abc101", true },
   { "<pat/[0-9]{2,4}/test>", "0abc101", true },
   { "<pat/abc(def)?ghi/test>", "defghi", false },
   { "<pat/abc(def)?ghi/test>", "abcdef", false },
   { "<pat/abc(def)?ghi/test>", "bcdefghi", false },
   { "<pat/abc(def)?ghi/test>", "abcdefgh", false },
   { "<pat/abc(def)?ghi/test>", "abcdefghi", true },
   { "<opt/a>|[<opt/b>]<opt/c>", "", false },
   { "<opt/a>|[<opt/b>]<opt/c>", "a", true },
   { "<opt/a>|[<opt/b>]<opt/c>", "b", false },
   { "<opt/a>|[<opt/b>]<opt/c>", "c", true },
   { "<opt/a>|[<opt/b>]<opt/c>", "a b", false },
   { "<opt/a>|[<opt/b>]<opt/c>", "a c", false },
   { "<opt/a>|[<opt/b>]<opt/c>", "b c", true },
   { "<opt/a>|[<opt/b>]<opt/c>", "a b c", false },
   { "<opt/a>[<opt/b>]|<opt/c>", "a", true },
   { "<opt/a>[<opt/b>]|<opt/c>", "b", false },
   { "<opt/a>[<opt/b>]|<opt/c>", "c", true },
   { "<opt/a>[<opt/b>]|<opt/c>", "a b", true },
   { "<opt/a>[<opt/b>]|<opt/c>", "a c", false },
   { "<opt/a>[<opt/b>]|<opt/c>", "b c", false },
   { "<opt/a>[<opt/b>]|<opt/c>", "a b c", false },
   { "<opt/a>[<opt/b>|<opt/c>]", "", false },
   { "<opt/a>[<opt/b>|<opt/c>]", "a", true },
   { "<opt/a>[<opt/b>|<opt/c>]", "b", false },
   { "<opt/a>[<opt/b>|<opt/c>]", "c", false },
   { "<opt/a>[<opt/b>|<opt/c>]", "a b", true },
   { "<opt/a>[<opt/b>|<opt/c>]", "a c", true },
   { "<opt/a>[<opt/b>|<opt/c>]", "b c", false },
   { "<opt/a>[<opt/b>|<opt/c>]", "a b c", false },
   { "[<opt/a>]<opt/b>|<opt/c>", "", false },
   { "[<opt/a>]<opt/b>|<opt/c>", "a", false },
   { "[<opt/a>]<opt/b>|<opt/c>", "b", true },
   { "[<opt/a>]<opt/b>|<opt/c>", "c", true },
   { "[<opt/a>]<opt/b>|<opt/c>", "a b", true },
   { "[<opt/a>]<opt/b>|<opt/c>", "a c", true },
   { "[<opt/a>]<opt/b>|<opt/c>", "b c", false },
   { "[<opt/a>]<opt/b>|<opt/c>", "a b c", false },
   { "<opt/a>[<opt/b>[<opt/c>]]", "", false },
   { "<opt/a>[<opt/b>[<opt/c>]]", "a", true },
   { "<opt/a>[<opt/b>[<opt/c>]]", "b", false },
   { "<opt/a>[<opt/b>[<opt/c>]]", "c", false },
   { "<opt/a>[<opt/b>[<opt/c>]]", "a b", true },
   { "<opt/a>[<opt/b>[<opt/c>]]", "a c", false },
   { "<opt/a>[<opt/b>[<opt/c>]]", "b c", false },
   { "<opt/a>[<opt/b>[<opt/c>]]", "a b c", true },
   { "<opt/a>[<opt/b>{<opt/c>}]", "", false },
   { "<opt/a>[<opt/b>{<opt/c>}]", "a", true },
   { "<opt/a>[<opt/b>{<opt/c>}]", "b", false },
   { "<opt/a>[<opt/b>{<opt/c>}]", "c", false },
   { "<opt/a>[<opt/b>{<opt/c>}]", "a b", false },
   { "<opt/a>[<opt/b>{<opt/c>}]", "a c", false },
   { "<opt/a>[<opt/b>{<opt/c>}]", "b c", false },
   { "<opt/a>[<opt/b>{<opt/c>}]", "a b c", true },
   { "<opt/a>[<opt/b>{<opt/c>}]", "b c a", false },
   { "<opt/a>[<opt/b>{<opt/c>}]", "c b a", false },
   { "[<opt/a>]<opt/b>[<opt/c>]", "", false },
   { "[<opt/a>]<opt/b>[<opt/c>]", "b", true },
   { "[<opt/a>]<opt/b>[<opt/c>]", "a", false },
   { "[<opt/a>]<opt/b>[<opt/c>]", "c", false },
   { "[<opt/a>]<opt/b>[<opt/c>]", "a b", true },
   { "[<opt/a>]<opt/b>[<opt/c>]", "a c", false },
   { "[<opt/a>]<opt/b>[<opt/c>]", "b c", true },
   { "[<opt/a>]<opt/b>[<opt/c>]", "a b c", true },
   { "[<opt/a>]<opt/b>[<opt/c>]", "a a c c", false },
   { "[<opt/a>]<opt/b>[<opt/c>]", "a a b b c c", false },
   { "{<opt/a>|<opt/b>}[<opt/c>]", "", false },
   { "{<opt/a>|<opt/b>}[<opt/c>]", "a", true },
   { "{<opt/a>|<opt/b>}[<opt/c>]", "b", true },
   { "{<opt/a>|<opt/b>}[<opt/c>]", "c", false },
   { "{<opt/a>|<opt/b>}[<opt/c>]", "a b", false },
   { "{<opt/a>|<opt/b>}[<opt/c>]", "a c", true },
   { "{<opt/a>|<opt/b>}[<opt/c>]", "b c", true },
   { "{<opt/a>|<opt/b>}[<opt/c>]", "a c b", false },
   { "{<opt/a>|<opt/b>}[<opt/c>]", "a b c", false },
   { "{[<opt/a>]<opt/b>}|<opt/c>", "", false },
   { "{[<opt/a>]<opt/b>}|<opt/c>", "a", false },
   { "{[<opt/a>]<opt/b>}|<opt/c>", "b", true },
   { "{[<opt/a>]<opt/b>}|<opt/c>", "c", true },
   { "{[<opt/a>]<opt/b>}|<opt/c>", "a b", true },
   { "{[<opt/a>]<opt/b>}|<opt/c>", "a c", false },
   { "{[<opt/a>]<opt/b>}|<opt/c>", "b c", false },
   { "{[<opt/a>]<opt/b>}|<opt/c>", "a b c", false },
   { "<pat/^[0-9]*abc.*[a-z0-9]*$/>", "abc", true },
   { "<pat/^[0-9]*abc.*[a-z0-9]*$/>", "abd", false },
   { "<pat/^[0-9]*abc.*[a-z0-9]*$/>", "1abc", true },
   { "<pat/^[0-9]*abc.*[a-z0-9]*$/>", "xabc", false },
   { "<pat/^[0-9]*abc.*[a-z0-9]*$/>", "12abc34", true },
   { "<pat/^[0-9]*abc.*[a-z0-9]*$/>", "12abc34x", true },
   { "<pat/^[0-9]*abc.*[a-z0-9]*$/>", "12abc34X", false },
   { "<pat/^[0-9]*abc.*[a-z0-9]*$/>", "x12abc34x", false },
   { "<pat/^[0-9]*abc.*[a-z0-9]*$/>", "12abc34x56", true },
   { "<pat/^.[0-9]*abc.*[a-z0-9]*$/>", "x12abc34x56", true },
   { "<opt/a>[<opt/b>|<opt/c>]<opt/d>", "", false },
   { "<opt/a>[<opt/b>|<opt/c>]<opt/d>", "a", false },
   { "<opt/a>[<opt/b>|<opt/c>]<opt/d>", "b", false },
   { "<opt/a>[<opt/b>|<opt/c>]<opt/d>", "c", false },
   { "<opt/a>[<opt/b>|<opt/c>]<opt/d>", "d", false },
   { "<opt/a>[<opt/b>|<opt/c>]<opt/d>", "a b", false },
   { "<opt/a>[<opt/b>|<opt/c>]<opt/d>", "a c", false },
   { "<opt/a>[<opt/b>|<opt/c>]<opt/d>", "a d", true },
   { "<opt/a>[<opt/b>|<opt/c>]<opt/d>", "b a", false },
   { "<opt/a>[<opt/b>|<opt/c>]<opt/d>", "b b", false },
   { "<opt/a>[<opt/b>|<opt/c>]<opt/d>", "b c", false },
   { "<opt/a>[<opt/b>|<opt/c>]<opt/d>", "b d", false },
   { "<opt/a>[<opt/b>|<opt/c>]<opt/d>", "c d", false },
   { "<opt/a>[<opt/b>|<opt/c>]<opt/d>", "a b c", false },
   { "<opt/a>[<opt/b>|<opt/c>]<opt/d>", "a b d", true },
   { "<opt/a>[<opt/b>|<opt/c>]<opt/d>", "a c d", true },
   { "<opt/a>[<opt/b>|<opt/c>]<opt/d>", "b c d", false },
   { "<opt/a>[<opt/b>|<opt/c>]<opt/d>", "a a b d", false },
   { "<opt/a>[<opt/b>|<opt/c>]<opt/d>", "a a c d", false },
   { "<opt/a>[<opt/b>|<opt/c>]<opt/d>", "a b c d", false },
   { "<opt/a>[<opt/b>|<opt/c>]<opt/d>", "a c d e", false },
   { "<opt/a>[<opt/b>|<opt/c>]<opt/d>", "a b d e", false },
   { "<opt/a>[[<opt/b>|<opt/c>]<opt/d>]", "", false },
   { "<opt/a>[[<opt/b>|<opt/c>]<opt/d>]", "a", true },
   { "<opt/a>[[<opt/b>|<opt/c>]<opt/d>]", "b", false },
   { "<opt/a>[[<opt/b>|<opt/c>]<opt/d>]", "c", false },
   { "<opt/a>[[<opt/b>|<opt/c>]<opt/d>]", "d", false },
   { "<opt/a>[[<opt/b>|<opt/c>]<opt/d>]", "a d", true },
   { "<opt/a>[[<opt/b>|<opt/c>]<opt/d>]", "a b", false },
   { "<opt/a>[[<opt/b>|<opt/c>]<opt/d>]", "a c", false },
   { "<opt/a>[[<opt/b>|<opt/c>]<opt/d>]", "b c", false },
   { "<opt/a>[[<opt/b>|<opt/c>]<opt/d>]", "c d", false },
   { "<opt/a>[[<opt/b>|<opt/c>]<opt/d>]", "b d", false },
   { "<opt/a>[[<opt/b>|<opt/c>]<opt/d>]", "a b d", true },
   { "<opt/a>[[<opt/b>|<opt/c>]<opt/d>]", "a c d", true },
   { "<opt/a>[[<opt/b>|<opt/c>]<opt/d>]", "a b c", false },
   { "<opt/a>[[<opt/b>|<opt/c>]<opt/d>]", "b c d", false },
   { "<opt/a>[[<opt/b>|<opt/c>]<opt/d>]", "a b c d", false },
   { "<opt/a>[<opt/b>][<opt/c>][<opt/d>]", "", false },
   { "<opt/a>[<opt/b>][<opt/c>][<opt/d>]", "a", true },
   { "<opt/a>[<opt/b>][<opt/c>][<opt/d>]", "b", false },
   { "<opt/a>[<opt/b>][<opt/c>][<opt/d>]", "c", false },
   { "<opt/a>[<opt/b>][<opt/c>][<opt/d>]", "d", false },
   { "<opt/a>[<opt/b>][<opt/c>][<opt/d>]", "a b", true },
   { "<opt/a>[<opt/b>][<opt/c>][<opt/d>]", "b a", false },
   { "<opt/a>[<opt/b>][<opt/c>][<opt/d>]", "a c", true },
   { "<opt/a>[<opt/b>][<opt/c>][<opt/d>]", "c a", false },
   { "<opt/a>[<opt/b>][<opt/c>][<opt/d>]", "a d", true },
   { "<opt/a>[<opt/b>][<opt/c>][<opt/d>]", "d a", false },
   { "<opt/a>[<opt/b>][<opt/c>][<opt/d>]", "a b c", true },
   { "<opt/a>[<opt/b>][<opt/c>][<opt/d>]", "a c b", true },
   { "<opt/a>[<opt/b>][<opt/c>][<opt/d>]", "b a c", false },
   { "<opt/a>[<opt/b>][<opt/c>][<opt/d>]", "c a b", false },
   { "<opt/a>[<opt/b>][<opt/c>][<opt/d>]", "c b a", false },
   { "<opt/a>[<opt/b>][<opt/c>][<opt/d>]", "a b d", true },
   { "<opt/a>[<opt/b>][<opt/c>][<opt/d>]", "a d b", true },
   { "<opt/a>[<opt/b>][<opt/c>][<opt/d>]", "a c d", true },
   { "<opt/a>[<opt/b>][<opt/c>][<opt/d>]", "a d c", true },
   { "<opt/a>[<opt/b>][<opt/c>][<opt/d>]", "b a d", false },
   { "<opt/a>[<opt/b>][<opt/c>][<opt/d>]", "b d a", false },
   { "<opt/a>[<opt/b>][<opt/c>][<opt/d>]", "b c a", false },
   { "<opt/a>[<opt/b>][<opt/c>][<opt/d>]", "b a c", false },
   { "<opt/a>[<opt/b>][<opt/c>][<opt/d>]", "a b c d", true },
   { "<opt/a>[<opt/b>][<opt/c>][<opt/d>]", "a c d b", true },
   { "<opt/a>[<opt/b>][<opt/c>][<opt/d>]", "a d b c", true },
   { "<opt/a>[<opt/b>][<opt/c>][<opt/d>]", "a d c b", true },
   { "<opt/a>[<opt/b>][<opt/c>][<opt/d>]", "b a c d", false },
   { "<opt/a>[<opt/b>][<opt/c>][<opt/d>]", "b c a d", false },
   { "<opt/a>[<opt/b>][<opt/c>][<opt/d>]", "b d c a", false },
   { "<opt/a>[<opt/b>][<opt/c>][<opt/d>]", "c a b d", false },
   { "<opt/a>[<opt/b>][<opt/c>][<opt/d>]", "c b a d", false },
   { "<opt/a>[<opt/b>][<opt/c>][<opt/d>]", "c b d a", false },
   { "<opt/a>[<opt/b>][<opt/c>][<opt/d>]", "d a b c", false },
   { "[<opt/a>{<opt/b>[<opt/c>]}]<opt/d>", "a", false },
   { "[<opt/a>{<opt/b>[<opt/c>]}]<opt/d>", "b", false },
   { "[<opt/a>{<opt/b>[<opt/c>]}]<opt/d>", "c", false },
   { "[<opt/a>{<opt/b>[<opt/c>]}]<opt/d>", "d", true },
   { "[<opt/a>{<opt/b>[<opt/c>]}]<opt/d>", "a", false },
   { "[<opt/a>{<opt/b>[<opt/c>]}]<opt/d>", "a b", false },
   { "[<opt/a>{<opt/b>[<opt/c>]}]<opt/d>", "a c", false },
   { "[<opt/a>{<opt/b>[<opt/c>]}]<opt/d>", "a d", false },
   { "[<opt/a>{<opt/b>[<opt/c>]}]<opt/d>", "b c", false },
   { "[<opt/a>{<opt/b>[<opt/c>]}]<opt/d>", "b d", false },
   { "[<opt/a>{<opt/b>[<opt/c>]}]<opt/d>", "a b c", false },
   { "[<opt/a>{<opt/b>[<opt/c>]}]<opt/d>", "a b d", true },
   { "[<opt/a>{<opt/b>[<opt/c>]}]<opt/d>", "b a d", false },
   { "[<opt/a>{<opt/b>[<opt/c>]}]<opt/d>", "b c d", false },
   { "[<opt/a>{<opt/b>[<opt/c>]}]<opt/d>", "a b c d", true },
   { "[<opt/a>][<opt/b>][<opt/c>][<opt/d>]", "", true },
   { "[<opt/a>][<opt/b>][<opt/c>][<opt/d>]", "a", true },
   { "[<opt/a>][<opt/b>][<opt/c>][<opt/d>]", "b", true },
   { "[<opt/a>][<opt/b>][<opt/c>][<opt/d>]", "c", true },
   { "[<opt/a>][<opt/b>][<opt/c>][<opt/d>]", "d", true },
   { "[<opt/a>][<opt/b>][<opt/c>][<opt/d>]", "a b", true },
   { "[<opt/a>][<opt/b>][<opt/c>][<opt/d>]", "b a", true },
   { "[<opt/a>][<opt/b>][<opt/c>][<opt/d>]", "a c", true },
   { "[<opt/a>][<opt/b>][<opt/c>][<opt/d>]", "c a", true },
   { "[<opt/a>][<opt/b>][<opt/c>][<opt/d>]", "a d", true },
   { "[<opt/a>][<opt/b>][<opt/c>][<opt/d>]", "d a", true },
   { "[<opt/a>][<opt/b>][<opt/c>][<opt/d>]", "b c", true },
   { "[<opt/a>][<opt/b>][<opt/c>][<opt/d>]", "c b", true },
   { "[<opt/a>][<opt/b>][<opt/c>][<opt/d>]", "b d", true },
   { "[<opt/a>][<opt/b>][<opt/c>][<opt/d>]", "d b", true },
   { "[<opt/a>][<opt/b>][<opt/c>][<opt/d>]", "c d", true },
   { "[<opt/a>][<opt/b>][<opt/c>][<opt/d>]", "d c", true },
   { "[<opt/a>][<opt/b>][<opt/c>][<opt/d>]", "a a", false },
   { "[<opt/a>][<opt/b>][<opt/c>][<opt/d>]", "b b", false },
   { "[<opt/a>][<opt/b>][<opt/c>][<opt/d>]", "c c", false },
   { "[<opt/a>][<opt/b>][<opt/c>][<opt/d>]", "d d", false },
   { "[<opt/a>][<opt/b>][<opt/c>][<opt/d>]", "a b c", true },
   { "[<opt/a>][<opt/b>][<opt/c>][<opt/d>]", "b a c", true },
   { "[<opt/a>][<opt/b>][<opt/c>][<opt/d>]", "b d c", true },
   { "[<opt/a>][<opt/b>][<opt/c>][<opt/d>]", "c a b", true },
   { "[<opt/a>][<opt/b>][<opt/c>][<opt/d>]", "c b a", true },
   { "[<opt/a>][<opt/b>][<opt/c>][<opt/d>]", "a b a", false },
   { "[<opt/a>][<opt/b>][<opt/c>][<opt/d>]", "b a b", false },
   { "[<opt/a>][<opt/b>][<opt/c>][<opt/d>]", "c b c", false },
   { "[<opt/a>][<opt/b>][<opt/c>][<opt/d>]", "b d b", false },
   { "[<opt/a>][<opt/b>][<opt/c>][<opt/d>]", "a d d", false },
   { "[<opt/a>][<opt/b>][<opt/c>][<opt/d>]", "a b c d", true },
   { "[<opt/a>][<opt/b>][<opt/c>][<opt/d>]", "b a c d", true },
   { "[<opt/a>][<opt/b>][<opt/c>][<opt/d>]", "b c a d", true },
   { "[<opt/a>][<opt/b>][<opt/c>][<opt/d>]", "b c d a", true },
   { "[<opt/a>][<opt/b>][<opt/c>][<opt/d>]", "c d a b", true },
   { "[<opt/a>][<opt/b>][<opt/c>][<opt/d>]", "d a b c", true },
   { "[<opt/a>][<opt/b>][<opt/c>][<opt/d>]", "d c b a", true },
   { "[<opt/a>][<opt/b>][<opt/c>][<opt/d>]", "d b c a d", false },
   { "[<opt/a>][<opt/b>][<opt/c>][<opt/d>]", "d b c a a", false },
   { "[<opt/a>][<opt/b>][<opt/c>][<opt/d>]", "d b c a b", false },
   { "[<opt/a>][<opt/b>][<opt/c>][<opt/d>]", "a b c d a", false },
   { "[<opt/a>][<opt/b>][<opt/c>][<opt/d>]", "b c d a b", false },
   { "{[<opt/a>]<opt/b>}|[<opt/c>]<opt/d>", "", false },
   { "{[<opt/a>]<opt/b>}|[<opt/c>]<opt/d>", "a", false },
   { "{[<opt/a>]<opt/b>}|[<opt/c>]<opt/d>", "b", true },
   { "{[<opt/a>]<opt/b>}|[<opt/c>]<opt/d>", "c", false },
   { "{[<opt/a>]<opt/b>}|[<opt/c>]<opt/d>", "d", true },
   { "{[<opt/a>]<opt/b>}|[<opt/c>]<opt/d>", "a b", true },
   { "{[<opt/a>]<opt/b>}|[<opt/c>]<opt/d>", "a c", false },
   { "{[<opt/a>]<opt/b>}|[<opt/c>]<opt/d>", "a d", false },
   { "{[<opt/a>]<opt/b>}|[<opt/c>]<opt/d>", "b c", false },
   { "{[<opt/a>]<opt/b>}|[<opt/c>]<opt/d>", "b d", false },
   { "{[<opt/a>]<opt/b>}|[<opt/c>]<opt/d>", "c d", true },
   { "{[<opt/a>]<opt/b>}|[<opt/c>]<opt/d>", "a b c", false },
   { "{[<opt/a>]<opt/b>}|[<opt/c>]<opt/d>", "a b d", false },
   { "{[<opt/a>]<opt/b>}|[<opt/c>]<opt/d>", "a c d", false },
   { "{[<opt/a>]<opt/b>}|[<opt/c>]<opt/d>", "b c d", false },
   { "{[<opt/a>]<opt/b>}|[<opt/c>]<opt/d>", "a b c d", false },
   { "<opt/a>|<opt/b><opt/c><opt/d>|<opt/e>", "", false },
   { "<opt/a>|<opt/b><opt/c><opt/d>|<opt/e>", "a", false },
   { "<opt/a>|<opt/b><opt/c><opt/d>|<opt/e>", "b", false },
   { "<opt/a>|<opt/b><opt/c><opt/d>|<opt/e>", "c", false },
   { "<opt/a>|<opt/b><opt/c><opt/d>|<opt/e>", "d", false },
   { "<opt/a>|<opt/b><opt/c><opt/d>|<opt/e>", "e", false },
   { "<opt/a>|<opt/b><opt/c><opt/d>|<opt/e>", "a b", false },
   { "<opt/a>|<opt/b><opt/c><opt/d>|<opt/e>", "a c", false },
   { "<opt/a>|<opt/b><opt/c><opt/d>|<opt/e>", "a d", false },
   { "<opt/a>|<opt/b><opt/c><opt/d>|<opt/e>", "a e", false },
   { "<opt/a>|<opt/b><opt/c><opt/d>|<opt/e>", "b c", false },
   { "<opt/a>|<opt/b><opt/c><opt/d>|<opt/e>", "b d", false },
   { "<opt/a>|<opt/b><opt/c><opt/d>|<opt/e>", "b e", false },
   { "<opt/a>|<opt/b><opt/c><opt/d>|<opt/e>", "c d", false },
   { "<opt/a>|<opt/b><opt/c><opt/d>|<opt/e>", "c e", false },
   { "<opt/a>|<opt/b><opt/c><opt/d>|<opt/e>", "d e", false },
   { "<opt/a>|<opt/b><opt/c><opt/d>|<opt/e>", "a b c", false },
   { "<opt/a>|<opt/b><opt/c><opt/d>|<opt/e>", "a c d", true },
   { "<opt/a>|<opt/b><opt/c><opt/d>|<opt/e>", "a c e", true },
   { "<opt/a>|<opt/b><opt/c><opt/d>|<opt/e>", "a d e", false },
   { "<opt/a>|<opt/b><opt/c><opt/d>|<opt/e>", "b c d", true },
   { "<opt/a>|<opt/b><opt/c><opt/d>|<opt/e>", "b c e", true },
   { "<opt/a>|<opt/b><opt/c><opt/d>|<opt/e>", "b d e", false },
   { "<opt/a>|<opt/b><opt/c><opt/d>|<opt/e>", "c d e", false },
   { "<opt/a>|<opt/b><opt/c><opt/d>|<opt/e>", "a b c d", false },
   { "<opt/a>|<opt/b><opt/c><opt/d>|<opt/e>", "a b d e", false },
   { "<opt/a>|<opt/b><opt/c><opt/d>|<opt/e>", "a c d e", false },
   { "<opt/a>|<opt/b><opt/c><opt/d>|<opt/e>", "b c d e", false },
   { "<opt/a>|<opt/b><opt/c><opt/d>|<opt/e>", "a b c d e", false },
   { "<opt/a>|[<opt/b>]<opt/c>|[<opt/d>]<opt/e>", "a", true },
   { "<opt/a>|[<opt/b>]<opt/c>|[<opt/d>]<opt/e>", "b", false },
   { "<opt/a>|[<opt/b>]<opt/c>|[<opt/d>]<opt/e>", "c", true },
   { "<opt/a>|[<opt/b>]<opt/c>|[<opt/d>]<opt/e>", "d", false },
   { "<opt/a>|[<opt/b>]<opt/c>|[<opt/d>]<opt/e>", "e", true },
   { "<opt/a>|[<opt/b>]<opt/c>|[<opt/d>]<opt/e>", "a b", false },
   { "<opt/a>|[<opt/b>]<opt/c>|[<opt/d>]<opt/e>", "a c", false },
   { "<opt/a>|[<opt/b>]<opt/c>|[<opt/d>]<opt/e>", "a d", false },
   { "<opt/a>|[<opt/b>]<opt/c>|[<opt/d>]<opt/e>", "a e", false },
   { "<opt/a>|[<opt/b>]<opt/c>|[<opt/d>]<opt/e>", "b a", false },
   { "<opt/a>|[<opt/b>]<opt/c>|[<opt/d>]<opt/e>", "b b", false },
   { "<opt/a>|[<opt/b>]<opt/c>|[<opt/d>]<opt/e>", "b c", true },
   { "<opt/a>|[<opt/b>]<opt/c>|[<opt/d>]<opt/e>", "b d", false },
   { "<opt/a>|[<opt/b>]<opt/c>|[<opt/d>]<opt/e>", "b e", false },
   { "<opt/a>|[<opt/b>]<opt/c>|[<opt/d>]<opt/e>", "c a", false },
   { "<opt/a>|[<opt/b>]<opt/c>|[<opt/d>]<opt/e>", "c b", false },
   { "<opt/a>|[<opt/b>]<opt/c>|[<opt/d>]<opt/e>", "c c", false },
   { "<opt/a>|[<opt/b>]<opt/c>|[<opt/d>]<opt/e>", "c d", false },
   { "<opt/a>|[<opt/b>]<opt/c>|[<opt/d>]<opt/e>", "c e", false },
   { "<opt/a>|[<opt/b>]<opt/c>|[<opt/d>]<opt/e>", "d a", false },
   { "<opt/a>|[<opt/b>]<opt/c>|[<opt/d>]<opt/e>", "d b", false },
   { "<opt/a>|[<opt/b>]<opt/c>|[<opt/d>]<opt/e>", "d c", false },
   { "<opt/a>|[<opt/b>]<opt/c>|[<opt/d>]<opt/e>", "d d", false },
   { "<opt/a>|[<opt/b>]<opt/c>|[<opt/d>]<opt/e>", "d e", true },
   { "<opt/a>|[<opt/b>]<opt/c>|[<opt/d>]<opt/e>", "e a", false },
   { "<opt/a>|[<opt/b>]<opt/c>|[<opt/d>]<opt/e>", "e b", false },
   { "<opt/a>|[<opt/b>]<opt/c>|[<opt/d>]<opt/e>", "e c", false },
   { "<opt/a>|[<opt/b>]<opt/c>|[<opt/d>]<opt/e>", "e d", false },
   { "<opt/a>|[<opt/b>]<opt/c>|[<opt/d>]<opt/e>", "e e", false },
   { "<opt/a>|[<opt/b>]<opt/c>|[<opt/d>]<opt/e>", "a b c", false },
   { "<opt/a>|[<opt/b>]<opt/c>|[<opt/d>]<opt/e>", "b c d", false },
   { "<opt/a>|[<opt/b>]<opt/c>|[<opt/d>]<opt/e>", "c d e", false },
   { "<opt/a>|[<opt/b>]<opt/c>|[<opt/d>]<opt/e>", "a b c d", false },
   { "<opt/a>|[<opt/b>]<opt/c>|[<opt/d>]<opt/e>", "b c d e", false },
   { "<opt/a>|[<opt/b>]<opt/c>|[<opt/d>]<opt/e>", "a b c d e", false },
   { "<opt/a>{<opt/b>}|<opt/c>{<opt/d>}|<opt/e>", "a", false },
   { "<opt/a>{<opt/b>}|<opt/c>{<opt/d>}|<opt/e>", "b", false },
   { "<opt/a>{<opt/b>}|<opt/c>{<opt/d>}|<opt/e>", "c", false },
   { "<opt/a>{<opt/b>}|<opt/c>{<opt/d>}|<opt/e>", "d", false },
   { "<opt/a>{<opt/b>}|<opt/c>{<opt/d>}|<opt/e>", "e", true },
   { "<opt/a>{<opt/b>}|<opt/c>{<opt/d>}|<opt/e>", "a b", true },
   { "<opt/a>{<opt/b>}|<opt/c>{<opt/d>}|<opt/e>", "a c", false },
   { "<opt/a>{<opt/b>}|<opt/c>{<opt/d>}|<opt/e>", "a d", false },
   { "<opt/a>{<opt/b>}|<opt/c>{<opt/d>}|<opt/e>", "a e", false },
   { "<opt/a>{<opt/b>}|<opt/c>{<opt/d>}|<opt/e>", "b c", false },
   { "<opt/a>{<opt/b>}|<opt/c>{<opt/d>}|<opt/e>", "b d", false },
   { "<opt/a>{<opt/b>}|<opt/c>{<opt/d>}|<opt/e>", "c d", true },
   { "<opt/a>{<opt/b>}|<opt/c>{<opt/d>}|<opt/e>", "c e", false },
   { "<opt/a>{<opt/b>}|<opt/c>{<opt/d>}|<opt/e>", "a b c", false },
   { "<opt/a>{<opt/b>}|<opt/c>{<opt/d>}|<opt/e>", "b c d", false },
   { "<opt/a>{<opt/b>}|<opt/c>{<opt/d>}|<opt/e>", "c d e", false },
   { "<opt/a>{<opt/b>}|<opt/c>{<opt/d>}|<opt/e>", "a b c d", false },
   { "<opt/a>{<opt/b>}|<opt/c>{<opt/d>}|<opt/e>", "b c d e", false },
   { "<opt/a>{<opt/b>}|<opt/c>{<opt/d>}|<opt/e>", "a b c d e", false },
   { "<opt/a>{<opt/b>}|<opt/c>{<opt/d>}|<opt/e>", "b e", false },
   { "<opt/a>[<opt/b>|{<opt/c><opt/d>[<opt/e>]}]", "a", true },
   { "<opt/a>[<opt/b>|{<opt/c><opt/d>[<opt/e>]}]", "b", false },
   { "<opt/a>[<opt/b>|{<opt/c><opt/d>[<opt/e>]}]", "c", false },
   { "<opt/a>[<opt/b>|{<opt/c><opt/d>[<opt/e>]}]", "d", false },
   { "<opt/a>[<opt/b>|{<opt/c><opt/d>[<opt/e>]}]", "a b", true },
   { "<opt/a>[<opt/b>|{<opt/c><opt/d>[<opt/e>]}]", "a c", false },
   { "<opt/a>[<opt/b>|{<opt/c><opt/d>[<opt/e>]}]", "a d", false },
   { "<opt/a>[<opt/b>|{<opt/c><opt/d>[<opt/e>]}]", "a e", false },
   { "<opt/a>[<opt/b>|{<opt/c><opt/d>[<opt/e>]}]", "a b c", false },
   { "<opt/a>[<opt/b>|{<opt/c><opt/d>[<opt/e>]}]", "a c d", true },
   { "<opt/a>[<opt/b>|{<opt/c><opt/d>[<opt/e>]}]", "a c e", false },
   { "<opt/a>[<opt/b>|{<opt/c><opt/d>[<opt/e>]}]", "a d e", false },
   { "<opt/a>[<opt/b>|{<opt/c><opt/d>[<opt/e>]}]", "a b d", false },
   { "<opt/a>[<opt/b>|{<opt/c><opt/d>[<opt/e>]}]", "a b e", false },
   { "<opt/a>[<opt/b>|{<opt/c><opt/d>[<opt/e>]}]", "b c d", false },
   { "<opt/a>[<opt/b>|{<opt/c><opt/d>[<opt/e>]}]", "b c e", false },
   { "<opt/a>[<opt/b>|{<opt/c><opt/d>[<opt/e>]}]", "c d e", false },
   { "<opt/a>[<opt/b>|{<opt/c><opt/d>[<opt/e>]}]", "a b c d", false },
   { "<opt/a>[<opt/b>|{<opt/c><opt/d>[<opt/e>]}]", "a c d e", true },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{<opt/e>}", "a", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{<opt/e>}", "b", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{<opt/e>}", "c", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{<opt/e>}", "d", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{<opt/e>}", "e", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{<opt/e>}", "a a", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{<opt/e>}", "a b", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{<opt/e>}", "a c", true },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{<opt/e>}", "a d", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{<opt/e>}", "a e", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{<opt/e>}", "b c", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{<opt/e>}", "b d", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{<opt/e>}", "b e", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{<opt/e>}", "c c", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{<opt/e>}", "c d", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{<opt/e>}", "c e", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{<opt/e>}", "d e", true },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{<opt/e>}", "d d", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{<opt/e>}", "e e", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{<opt/e>}", "a b c", true },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{<opt/e>}", "a c d", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{<opt/e>}", "b c d", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{<opt/e>}", "c d e", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{<opt/e>}", "a d e", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{<opt/e>}", "b d e", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{<opt/e>}", "a b c d", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{<opt/e>}", "b c d e", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{<opt/e>}", "a b c d e", false },
   { "{<opt/-a/a>}|{<opt/-b/b>}|{<opt/-c/c>}|<val//d>", "-a", true },
   { "{<opt/-a/a>}|{<opt/-b/b>}|{<opt/-c/c>}|<val//d>", "-b", true },
   { "{<opt/-a/a>}|{<opt/-b/b>}|{<opt/-c/c>}|<val//d>", "-c", true },
   { "{<opt/-a/a>}|{<opt/-b/b>}|{<opt/-c/c>}|<val//d>", "-d", true },
   { "{<opt/-a/a>}|{<opt/-b/b>}|{<opt/-c/c>}|<val//d>", "-a -b", false },
   { "{<opt/-a/a>}|{<opt/-b/b>}|{<opt/-c/c>}|<val//d>", "-b -c", false },
   { "{<opt/-a/a>}|{<opt/-b/b>}|{<opt/-c/c>}|<val//d>", "-c -d", false },
   { "{<opt/-a/a>}|{<opt/-b/b>}|{<opt/-c/c>}|<val//d>", "-a -a", false },
   { "{<opt/-a/a>}|{<opt/-b/b>}|{<opt/-c/c>}|<val//d>", "-b -b", false },
   { "{<opt/-a/a>}|{<opt/-b/b>}|{<opt/-c/c>}|<val//d>", "-c -c", false },
   { "<pat/[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\\\\\\\.[A-Za-z]{2,4}$/>", "a@b", false },
   { "<pat/[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\\\\\\\.[A-Za-z]{2,4}$/>", "a@b.", false },
   { "<pat/[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\\\\\\\.[A-Za-z]{2,4}$/>", "a@b.c", false },
   { "<pat/[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\\\\\\\.[A-Za-z]{2,4}$/>", "a@b.cd", true },
   { "<pat/[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\\\\\\\.[A-Za-z]{2,4}$/>", "a@bc.d", false },
   { "<pat/[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\\\\\\\.[A-Za-z]{2,4}$/>", "a@bc.de", true },
   { "<pat/[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\\\\\\\.[A-Za-z]{2,4}$/>", "a@bc.def", true },
   { "<pat/[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\\\\\\\.[A-Za-z]{2,4}$/>", "a@bc.defg", true },
   { "<pat/[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\\\\\\\.[A-Za-z]{2,4}$/>", "a@bc.de.fg", true },
   { "<pat/[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\\\\\\\.[A-Za-z]{2,4}$/>", "a@bc.defgh", false },
   { "<pat/[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\\\\\\\.[A-Za-z]{2,4}$/>", "a@bc.def.g", false },
   { "<pat/[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\\\\\\\.[A-Za-z]{2,4}$/>", "a@bc.defg.h", false },
   { "<pat/[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\\\\\\\.[A-Za-z]{2,4}$/>", "a@bc.defg.hi", true },
   { "<pat/[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\\\\\\\.[A-Za-z]{2,4}$/>", "a@bc.defg.hij", true },
   { "<pat/[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\\\\\\\.[A-Za-z]{2,4}$/>", "a@bc.defg.hijk", true },
   { "<pat/[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\\\\\\\.[A-Za-z]{2,4}$/>", "a@bc.defg.hijkl", false },
   { "<pat/[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\\\\\\\.[A-Za-z]{2,4}$/>", "a@bc.de.fg.hi.jkl", true },
   { "<pat/[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\\\\\\\.[A-Za-z]{2,5}$/>", "a@bc.de.fg.hi.jkl", true },
   { "<pat/[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\\\\\\\.[A-Za-z]{2,6}$/>", "a@bc.de.fg.hi.jkl", true },
   { "<pat/[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\\\\\\\.[A-Za-z]{2,7}$/>", "a@bc.de.fg.hi.jkl", true },
   { "<pat/[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\\\\\\\.[A-Za-z]{2,8}$/>", "a@bc.de.fg.hi.jkl", true },
   { "<pat/[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\\\\\\\.[A-Za-z]{3,8}$/>", "a@bc.de.fg.hi.jkl", true },
   { "<pat/[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\\\\\\\.[A-Za-z]{4,8}$/>", "a@bc.de.fg.hi.jkl", false },
   { "<pat/[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\\\\\\\.[A-Za-z]{4,8}$/>", "a@bc.de.fg.hi.jklm", true },
   { "<pat/[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\\\\\\\.[A-Za-z]{4,8}$/>", "a@bc.de.fg.hi.jklmn", true },
   { "<pat/[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\\\\\\\.[A-Za-z]{4,8}$/>", "a@bc.de.fg.hi.jklmno", true },
   { "<pat/\\<([a-zA-Z][a-zA-Z0-9]*)[^\\>]*\\>(.*)\\<\\\\/\\\\\\\\1\\>/>", "<y></y>", true },
   { "<pat/\\<([a-zA-Z][a-zA-Z0-9]*)[^\\>]*\\>(.*)\\<\\\\/\\\\\\\\1\\>/>", "<y></ye>", false },
   { "<pat/\\<([a-zA-Z][a-zA-Z0-9]*)[^\\>]*\\>(.*)\\<\\\\/\\\\\\\\1\\>/>", "<ye></y>", false },
   { "<pat/\\<([a-zA-Z][a-zA-Z0-9]*)[^\\>]*\\>(.*)\\<\\\\/\\\\\\\\1\\>/>", "<y>x</y>", true },
   { "<pat/\\<([a-zA-Z][a-zA-Z0-9]*)[^\\>]*\\>(.*)\\<\\\\/\\\\\\\\1\\>/>", "<y>xx</y>", true },
   { "<pat/\\<([a-zA-Z][a-zA-Z0-9]*)[^\\>]*\\>(.*)\\<\\\\/\\\\\\\\1\\>/>", "x<y>y</y>", true },
   { "<pat/\\<([a-zA-Z][a-zA-Z0-9]*)[^\\>]*\\>(.*)\\<\\\\/\\\\\\\\1\\>/>", "x<y>y</y>z", true },
   { "<pat/\\<([a-zA-Z][a-zA-Z0-9]*)[^\\>]*\\>(.*)\\<\\\\/\\\\\\\\1\\>/>", "<yes></yes>", true },
   { "<pat/\\<([a-zA-Z][a-zA-Z0-9]*)[^\\>]*\\>(.*)\\<\\\\/\\\\\\\\1\\>/>", "<yes>def</no>", false },
   { "<pat/\\<([a-zA-Z][a-zA-Z0-9]*)[^\\>]*\\>(.*)\\<\\\\/\\\\\\\\1\\>/>", "<yes>def</yes>", true },
   { "<pat/\\<([a-zA-Z][a-zA-Z0-9]*)[^\\>]*\\>(.*)\\<\\\\/\\\\\\\\1\\>/>", "<!no><yes>def</no>", false },
   { "<pat/\\<([a-zA-Z][a-zA-Z0-9]*)[^\\>]*\\>(.*)\\<\\\\/\\\\\\\\1\\>/>", "<!no><yes>def</yes>", true },
   { "<pat/\\<([a-zA-Z][a-zA-Z0-9]*)[^\\>]*\\>(.*)\\<\\\\/\\\\\\\\1\\>/>", "<!no><yes>def</no>ghi", false },
   { "<pat/\\<([a-zA-Z][a-zA-Z0-9]*)[^\\>]*\\>(.*)\\<\\\\/\\\\\\\\1\\>/>", "<!no><yes>def</yes>ghi", true },
   { "<pat/\\<([a-zA-Z][a-zA-Z0-9]*)[^\\>]*\\>(.*)\\<\\\\/\\\\\\\\1\\>/>", "abc<!no><yes>def</no>ghi", false },
   { "<pat/\\<([a-zA-Z][a-zA-Z0-9]*)[^\\>]*\\>(.*)\\<\\\\/\\\\\\\\1\\>/>", "abc<!no><yes>def</yes>ghi", true },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "a", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "b", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "c", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "d", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "e", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "f", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "a b", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "a c", true },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "a d", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "a e", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "a f", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "b c", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "b d", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "b e", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "b f", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "c d", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "c e", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "c f", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "d e", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "d f", true },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "a b c", true },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "a b d", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "a b e", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "a b f", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "a c d", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "a c e", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "a c f", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "a d e", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "a d f", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "b c d", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "b c e", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "b c f", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "b d e", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "b d f", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "b e f", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "c d e", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "c d f", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "c e f", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "d e f", true },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "a b c d", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "a b c e", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "a b c f", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "a b d e", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "a b e f", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "a c d e", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "a c e f", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "a d e f", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "b c d e", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "b c d f", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "b c e f", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "b d e f", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "c d e f", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "a b c d e", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "a b c d f", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "a b c e f", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "a b d e f", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "a c d e f", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "b c d e f", false },
   { "<opt/a>{[<opt/b>]<opt/c>}|<opt/d>{[<opt/e>]<opt/f>}", "a b c d e f", false },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "", false },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "a", true },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "b", false },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "c", false },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "d", true },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "e", false },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "f", false },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "g", true },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "a b", false },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "a c", false },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "a d", false },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "a e", false },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "a f", false },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "a g", false },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "b c", false },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "b d", true },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "b e", false },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "b f", false },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "b g", false },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "c d", false },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "c e", false },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "c f", false },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "c g", false },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "d e", false },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "d f", false },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "d g", false },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "e f", false },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "e g", true },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "f g", false },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "a b c", false },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "a c d", false },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "a d e", false },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "a e f", false },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "a f g", false },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "b c d", true },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "b d e", false },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "b e f", false },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "b f g", false },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "c d e", false },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "c e f", false },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "c f g", false },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "d e f", false },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "d f g", false },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "e f g", true },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "a b c d", false },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "a c d e", false },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "a d e f", false },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "a e f g", false },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "b c d e", false },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "b d e f", false },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "b e f g", false },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "c d e f", false },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "c e f g", false },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "d e f g", false },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "a b c d e", false },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "b c d e f", false },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "b d e f g", false },
   { "<opt/a>|[<opt/b>[<opt/c>]]<opt/d>|[<opt/e>[<opt/f>]]<opt/g>", "c d e f g", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "a", true },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "b", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "c", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "d", true },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "e", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "f", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "g", true },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "a b", true },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "a c", true },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "a d", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "a e", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "a f", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "a g", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "b c", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "b d", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "b e", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "b f", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "b g", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "c d", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "c e", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "c f", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "c g", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "d e", true },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "d f", true },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "d g", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "e f", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "e g", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "f g", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "a b c", true },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "a c d", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "a d e", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "a e f", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "a f g", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "b c d", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "b d e", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "b e f", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "b f g", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "c d e", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "c e f", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "c f g", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "d e f", true },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "d f g", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "e f g", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "a b c d", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "a c d e", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "a d e f", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "a e f g", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "b c d e", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "b d e f", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "b e f g", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "c d e f", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "c e f g", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "d e f g", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "a b c d e", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "a c d e f", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "a d e f g", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "b c d e f", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "b d e f g", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "c d e f g", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "a b c d e f", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "a c d e f g", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "b c d e f g", false },
   { "<opt/a>[<opt/b>][<opt/c>]|<opt/d>[<opt/e>][<opt/f>]|<opt/g>", "a b c d e f g", false },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "", true },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "a", true },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "b", false },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "c", false },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "d", true },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "e", false },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "f", false },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "g", true },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "a b", false },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "a c", false },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "a d", false },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "a e", false },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "a f", false },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "a g", false },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "b c", false },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "b d", true },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "b e", false },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "b f", false },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "b g", false },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "c d", true },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "c e", false },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "c f", false },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "c g", false },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "d a", false },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "d b", false },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "d c", false },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "d e", false },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "d f", false },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "d g", false },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "e f", false },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "e g", true },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "f g", true },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "g e", false },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "g f", false },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "a b c", false },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "b c d", true },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "c b d", true },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "c d e", false },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "d e f", false },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "e f g", true },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "f e g", true },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "a b c d", false },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "a c d e", false },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "a d e f", false },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "a e f g", false },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "b c d e", false },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "b d e f", false },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "b e f g", false },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "b g f e", false },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "c d e f", false },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "c d f e", false },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "c f d e", false },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "c e f g", false },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "d e f g", false },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "d f g e", false },
   { "[<opt/a>|[<opt/b>][<opt/c>]<opt/d>|[<opt/e>][<opt/f>]<opt/g>]", "d g f e", false }
};

int main( int argc, char* argv[ ] )
{
   bool is_quiet = false;

   try
   {
      if( argc > 1 && string( argv[ 1 ] ) == "-quiet" )
      {
         --argc;
         is_quiet = true;
      }

      if( argc > 1 && string( argv[ 1 ] ) == "-test" )
      {
         cout << "performing tests...\n\n";

         command_parser p;
         int num_tests = 0;
         int num_errors = 0;

         for( size_t i = 0; i < ARRAY_SIZE( syntax_texts ); i++ )
         {
#ifdef DEBUG
            cout << "syntax test #" << i << ": " << syntax_texts[ i ].p_syntax << endl;
#endif
            p.parse_syntax( syntax_texts[ i ].p_syntax );
            if( p.okay( ) != syntax_texts[ i ].test_should_succeed )
            {
               ++num_errors;
               cout << "error: syntax test #" << ( i + 1 ) << ": "
                << syntax_texts[ i ].p_syntax << ( p.okay( ) ? " succeeded" : " failed" ) << " rather than"
                << ( syntax_texts[ i ].test_should_succeed ? " succeeded" : " failed" ) << '\n';
            }

            if( p.okay( ) )
            {
               p.output_syntax( cout );
               cout << '\n';
            }

            ++num_tests;
         }

         for( size_t i = 0; i < ARRAY_SIZE( command_tests ); i++ )
         {
            p.parse_syntax( command_tests[ i ].p_syntax );

            if( !p.okay( ) )
            {
               ++num_errors;
               cout << "error: command test #" << ( i + 1 )
                << ": " << command_tests[ i ].p_syntax << " syntax is not valid\n";
            }
            else
            {
               vector< string > arguments;
               map< string, string > parameters;

               setup_arguments( command_tests[ i ].p_command, arguments );

               if( p.parse_command( arguments, parameters ) != command_tests[ i ].test_should_succeed )
               {
                  ++num_errors;
                  cout << "error: command test #" << ( i + 1 ) << " command '"
                   << command_tests[ i ].p_command << "' for syntax '" << command_tests[ i ].p_syntax << "'"
                   << ( command_tests[ i ].test_should_succeed ? " failed" : " succeeded" ) << " rather than"
                   << ( command_tests[ i ].test_should_succeed ? " succeeded" : " failed" ) << '\n';
               }
            }

            ++num_tests;
         }

         p.clear( );
         if( p.get_num_nodes( ) != 0 )
         {
            ++num_errors;
            cout << "error: p.get_num_nodes( ) is " << p.get_num_nodes( ) << " when it should be 0\n";
         }

         cout << "\nperformed " << num_tests << " test(s) and found " << num_errors << " error(s)\n";
      }
      else if( argc == 1 )
      {
#ifdef __GNUG__
#  ifdef RDLINE_SUPPORT
         if( isatty( STDIN_FILENO ) )
            using_history( );
#  endif
#endif
         command_parser p;
         string cmd, next;
         while( cin )
         {
            next = get_line( "\n> " );

            if( next.empty( ) )
               continue;
#ifdef __GNUG__
#  ifdef RDLINE_SUPPORT
            if( isatty( STDIN_FILENO ) )
               add_history( next.c_str( ) );
#  endif
#endif
            string::size_type pos = next.find( ' ' );
            cmd = next.substr( 0, pos );

            if( pos == string::npos )
               next.erase( );
            else
               next.erase( 0, pos + 1 );

            if( cmd == "?" )
            {
               cout << "commands:\n";
               cout << "=========\n";
               cout << "usage\n";
               cout << "dump\n";
               cout << "test [<value>]\n";
               cout << "syntax [<expr>]\n";
               cout << "command [<expr>]\n";
               cout << "exit\n";
            }
            else if( cmd == "usage" )
            {
               p.output_usage( cout );
               cout << endl;
            }
            else if( cmd == "dump" )
            {
               p.dump_nodes( cout );
               cout << endl;
            }
            else if( cmd == "test" )
            {
               // NOTE: Dummy command for testing or prototyping.
               cout << next << endl;
            }
            else if( cmd == "syntax" )
            {
               if( next.empty( ) )
               {
                  p.output_syntax( cout );
                  cout << endl;
                  continue;
               }

               if( next.size( ) > 2 && next[ 0 ] == '"' && next[ next.size( ) - 1 ] == '"' )
               {
                  next.erase( 0, 1 );
                  next.erase( next.length( ) - 1 );
               }

               p.parse_syntax( next.c_str( ) );
               if( p.okay( ) )
                  cout << "okay" << endl;
               else
                  cout << "error: invalid format '" << next << "'" << endl;
            }
            else if( cmd == "command" )
            {
               vector< string > arguments;
               map< string, string > parameters;

               setup_arguments( next.c_str( ), arguments );

               if( p.parse_command( arguments, parameters ) )
               {
                  cout << "okay" << endl;
                  if( !is_quiet )
                  {
                     map< string, string >::iterator i;
                     if( !parameters.empty( ) )
                        cout << "\n[parameters]\n";
                     for( i = parameters.begin( ); i != parameters.end( ); ++i )
                        cout << "parameter: " << i->first << ", value = '" << i->second << "'\n";
                  }
               }
               else
                  cout << "error: invalid command '" << next << "'" << endl;
            }
            else if( cmd == "exit" )
               break;
            else
               cout << "unknown command '" << cmd << "' (type ? for list of commands)" << endl;
         }
      }
      else
         cout << "usage: test_parser [-test|-quiet]" << endl;
   }
   catch( exception& x )
   {
      cerr << "error: " << x.what( ) << endl;
   }
   catch( ... )
   {
      cerr << "error: unexpected exception caught" << endl;
   }
}
