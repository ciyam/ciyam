@echo off
REM Copyright (c) 2012-2020 CIYAM Developers
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
if not exist %1.app.vars.xrep goto error2

if not exist %WEBDIR%\%2 call setup.bat %1 %2 %3

if not exist %WEBDIR%\%2\activate.htms goto error3

xrep @app.fcgi.sio.xrep include=%1.app.vars.xrep >~genfcgi.tmp
call update.bat "%WEBDIR%\%2\fcgi.sio" ~genfcgi.tmp

xrep @ciyam_interface.js.xrep include=%1.app.vars.xrep >~genfcgi.tmp
call update.bat "%WEBDIR%\%2\ciyam_interface.js" ~genfcgi.tmp
goto end

:error1
echo Error: Missing environment variable 'WEBDIR'.
goto end

:error2
echo Error: File '%1.app.vars.xrep' not found.
goto end

:error3
echo Error: File '%WEBDIR%\%2\activate.htms' not found.
goto end

:usage
echo Usage: genfcgi [app name] [app dir] [[web dir]]

:end
endlocal

