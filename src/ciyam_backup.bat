@echo off
REM Copyright (c) 2012-2018 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

setlocal

set opt=
set backup_path=%cd%
set CIYAM_STORAGE=

if not '%1' == '-trunc' goto next
set opt=-trunc
shift
:next

if '%1' == '' goto usage
if '%2' == '' goto usage

set CIYAM_STORAGE=%1

if "%WEBDIR%" == "" goto error1

echo Starting backup...
call touch.bat %WEBDIR%/%2/ciyam_interface.stop
echo ^<ciyam_backup.cin %opt%>~ciyam_backup.cin
ciyam_client -quiet -no_prompt< ~ciyam_backup.cin
del ~ciyam_backup.cin
del %WEBDIR%\%2\ciyam_interface.stop

pushd %WEBDIR%\%2
echo Adding application files to backup...
if "%CIYAM_STORAGE%" == "Meta" goto meta
"%backup_path%\bundle" -r -q -y "%backup_path%\%1.backup" files/* -x files/tmp/*
goto done
:meta
"%backup_path%\bundle" -r -q -y "%backup_path%\%1.backup" files/* -x files/tmp/* files/100/105100/* files/100/106100/*
:done
popd

echo Finished backup...
goto end

:error1
echo Error: Missing environment variable 'WEBDIR'.
goto end

:usage
echo Usage: ciyam_backup [[-trunc]] [app name] [app dir]

:end
endlocal

