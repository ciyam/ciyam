@echo off
REM Copyright (c) 2012-2019 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if '%1' == '' goto usage
if '%2' == '' goto usage
if '%3' == '' goto usage

if '%4' == '' goto no_package
echo ^<getmeta.cin %1 %2 %3 files/100/136100/%4.map>~getmeta.cin
goto next

:no_package
echo ^<getmeta.cin %1 %2 %3>~getmeta.cin

:next
ciyam_client -quiet -no_prompt< ~getmeta.cin
del ~getmeta.cin
goto end

:usage
echo Usage: getmeta [meta type] [key val] ["field1,[field2,[...]]"] [[package key]]

:end

