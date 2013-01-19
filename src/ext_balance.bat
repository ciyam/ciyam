@echo off
REM Copyright (c) 2012-2013 CIYAM Open Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if '%1' == '' goto usage

if '%2' == '' goto standard_output
curl -s -m 8 %1 >%2
goto end

:standard_output
curl -s -m 8 %1
echo.
goto end

:usage
echo Usage: ext_balance [url] [[output_file]]
:end

