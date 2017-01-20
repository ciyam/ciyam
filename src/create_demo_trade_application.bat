@echo off
REM Copyright (c) 2017 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

setlocal
if '%1' == '' goto usage

set CIYAM_MODEL=%1

ciyam_client -quiet -no_prompt < create_demo_trade_application.cin

goto end

:usage
echo Usage: create_demo_trade_application [model name]

:end
endlocal

