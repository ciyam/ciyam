@echo off
REM Copyright (c) 2012-2019 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if '%1' == '' goto usage

:loop
if exist %1 del %1

shift
if '%1' == '' goto end
goto loop

:usage
echo Usage: killfiles [filename1 [filename2 [...] ] ]

:end

