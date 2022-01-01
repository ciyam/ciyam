@echo off
REM Copyright (c) 2017-2022 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

setlocal
if '%1' == '' goto usage
if '%2' == '' goto usage
if '%3' == '' goto usage

if '%4' == '0' goto error1
if '%4' == '1' goto error1

set CIYAM_DATA=%1
set CIYAM_START_NUM=%2
set CIYAM_NUM_SEARCHES=%3
set CIYAM_SEARCH_RANGE=%4

ciyam_client -quiet -no_prompt < search_nonces.cin
goto end

:error1
echo Error: If supplied then 'search range' must be greater than or equal to 2.
goto end

:usage
echo Usage: search_nonces [data] [start num] [num searches] [[search range]]

:end
endlocal

