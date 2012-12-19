@echo off
REM Copyright (c) 2012 CIYAM Open Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

setlocal

if '%1' == '' goto usage
if '%2' == '' goto usage

set dest=%2
set alias=%3

if not exist %1.fcgi.sio goto error1
if not exist %WEBDIR%\%dest%\fcgi.sio goto error2

if not exist "%WEBDIR%\%dest%\%1.jpg" copy module.jpg "%WEBDIR%\%dest%\%1.jpg" >nul

copy %1.fcgi.sio "%WEBDIR%\%dest%" >nul

if '%alias%' == '' goto next
xrep @module.html.xrep model_vars=%1.vars.xrep > %WEBDIR%\%dest%\%alias%.html

:next
goto end

:error1
echo Error: File '%1.fcgi.sio' does not exist.
goto end

:error2
echo Error: File '%WEBDIR%\%dest%\fcgi.sio' does not exist.
goto end

:usage
echo Usage: install [module name] [app dir] [[module alias]]

:end
endlocal

