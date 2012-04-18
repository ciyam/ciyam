@echo off
setlocal

set opt=
set backup_path=%cd%
set CIYAM_STORAGE=

if not '%1' == '-rebuild' goto next
set opt=-rebuild
shift
:next

if '%1' == '' goto usage
set CIYAM_STORAGE=%1

if not exist %1.backup.bun.gz goto error1

:next
echo Starting restore...
echo ^<ciyam_restore.cin %opt% >~ciyam_restore.cin
ciyam_client -quiet -no_prompt <~ciyam_restore.cin
del ~ciyam_restore.cin

if '%2' == '' goto next2
pushd %2
rmdir /s /q files
unbundle -o "%backup_path%\%1.backup.bun.gz" files/*
popd

:next2
echo Finished restore...
goto end

:error1
echo Error: Backup file '%1.backup.bun.gz' not found.
goto end

:usage
echo Usage: ciyam_restore [[-rebuild]] [app name] [[web [path]]

:end
endlocal

