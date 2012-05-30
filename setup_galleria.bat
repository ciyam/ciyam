@echo off
setlocal

if '%1' == '' goto skip
set WEBDIR=%1

:skip
if "%WEBDIR%" == "" goto error1

if not exist %WEBDIR%\galleria goto install
echo Galleria has already been installed!
goto end

:install
mkdir %WEBDIR%\galleria
unbundle -q galleria -d %WEBDIR%\galleria
goto end

:error1
echo Error: Missing environment variable 'WEBDIR'.
goto end

:end
endlocal

