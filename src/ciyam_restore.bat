@echo off
REM Copyright (c) 2012-2019 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

setlocal

set opt=
set backup_path=%cd%
set CIYAM_STORAGE=

if not '%1' == '-rebuild' goto next
set opt=-rebuild
shift
:next

if '%1' == '' goto usage
if '%2' == '' goto usage

set CIYAM_STORAGE=%1

if "%WEBDIR%" == "" goto error1

if not exist %1.backup.bun.gz goto error2

:next
echo Starting restore...
echo ^<ciyam_restore.cin %opt% >~ciyam_restore.cin
ciyam_client -quiet -no_prompt <~ciyam_restore.cin
del ~ciyam_restore.cin

pushd %WEBDIR%\%2
unbundle -o "%backup_path%\%1.backup.bun.gz" files/*
popd

echo Finished restore...
goto end

:error1
echo Error: Missing environment variable 'WEBDIR'.
goto end

:error2
echo Error: Backup file '%1.backup.bun.gz' not found.
goto end

:usage
echo Usage: ciyam_restore [[-rebuild]] [app name] [app dir]

:end
endlocal

