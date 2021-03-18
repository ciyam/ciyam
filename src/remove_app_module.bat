@echo off
REM Copyright (c) 2012-2021 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if '%1' == '' goto usage
if '%2' == '' goto usage

if exist "%WEBDIR%\%2\%1.jpg" del "%WEBDIR%\%2\%1.jpg"
if exist "%WEBDIR%\%2\%1.html" del "%WEBDIR%\%2\%1.html"
if exist "%WEBDIR%\%2\%1.fcgi.sio" del "%WEBDIR%\%2\%1.fcgi.sio"

if '%3' == '' goto skip
test_sql %3 %3 "SHOW TABLES FROM %3 LIKE 'T_%1_%%'" >~remove_app_module
xrep @remove_app_module.xrep tables=@~remove_app_module >>%3.obsolete.sql

if exist ~remove_app_module del ~remove_app_module

:skip
goto end

:usage
echo Usage: remove_app_module [module name] [app dir] [[app name]]

:end

