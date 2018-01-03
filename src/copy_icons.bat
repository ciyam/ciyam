@echo off
REM Copyright (c) 2012-2018 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

setlocal

if '%1' == '' goto usage
if '%2' == '' goto usage
if '%3' == '' goto usage

if '%4' == '' goto skip
set WEBDIR=%4

:skip
if "%WEBDIR%" == "" goto error1

if not exist %1_enum_icons.lst goto done
xrep @copy_icons.sh.xrep copy=copy name=%1 type=%2 app_dir=%3 webdir=%WEBDIR% next=@%2_enum_icons.lst >~copy_icons.bat
call ~copy_icons.bat >nul
del ~copy_icons.bat
:done
goto end

:error1
echo Error: Missing environment variable 'WEBDIR'.
goto end

:usage
echo Usage: copy_icons [package type] [package name] [app dir] [[web dir]]

:end
endlocal

