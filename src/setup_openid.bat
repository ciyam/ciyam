@echo off
REM Copyright (c) 2013-2021 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

setlocal

if '%1' == '' goto skip
set WEBDIR=%1

:skip
if "%WEBDIR%" == "" goto error1

if not exist %WEBDIR%\openid goto install
echo OpenID has already been installed.
goto end

:install
mkdir %WEBDIR%\openid
unbundle -q openid -d %WEBDIR%
goto end

:error1
echo Error: Missing environment variable 'WEBDIR'.
goto end

:end
endlocal

