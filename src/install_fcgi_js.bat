@echo off
REM Copyright (c) 2017-2020 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

setlocal

if '%1' == '' goto usage
if '%2' == '' goto usage

if '%3' == '' goto skip
set WEBDIR=%3

:skip
if "%WEBDIR%" == "" goto error1

xrep @ciyam_interface.js.xrep include=%1.app.vars.xrep > %WEBDIR%\%2\ciyam_interface.js
goto end

:error1
echo Error: Missing environment variable 'WEBDIR'.
goto end

:usage
echo Usage: install_fcgi_js [app name] [app dir] [[web dir]]

:end
endlocal

