@echo off
setlocal

if '%1' == '' goto usage
if '%2' == '' goto usage
if '%3' == '' goto usage

if '%4' == '' goto skip
set WEBDIR=%4

:skip
if "%WEBDIR%" == "" goto error1

if not exist %1_enum_icons.lst goto done
xrep @copy_icons.sh.xrep copy=copy package=%1 model=%2 app_dir=%3 webdir=%WEBDIR% next=@%1_enum_icons.lst >~copy_icons.bat
call ~copy_icons.bat >nul
del ~copy_icons.bat
:done
goto end

:error1
echo Error: Missing environment variable 'WEBDIR'.
goto end

:usage
echo Usage: copy_icons [package name] [model] [app dir] [[web dir]]

:end
endlocal

