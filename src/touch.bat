@echo off
REM Copyright (c) 2012-2019 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if '%1' == '' goto usage

:loop
type nul >>%1 & copy %1 +,, >nul

if '%2' == '' goto end
shift
goto loop

:usage
echo touch [filename] [[filename2 [...]]]

:end

