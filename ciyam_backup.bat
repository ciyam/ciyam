@echo off
setlocal

set opt=
set backup_path=%cd%
set CIYAM_STORAGE=

if not '%1' == '-trunc' goto next
set opt=-trunc
shift
:next

if '%1' == '' goto usage
set CIYAM_STORAGE=%1

:next
echo Starting backup...
echo ^<ciyam_backup.cin %opt%>~ciyam_backup.cin
ciyam_client -quiet -no_prompt< ~ciyam_backup.cin
del ~ciyam_backup.cin

if '%2' == '' goto next2
pushd %2
echo Adding application files to backup...
bundle "%backup_path%\%1.backup.bun.gz" -r -q files/*
popd

:next2
echo Finished backup...
goto end

:usage
echo Usage: ciyam_backup [[-trunc]] [app name] [[web path]]

:end
endlocal

