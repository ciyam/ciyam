@echo off

if "%WEBDIR%" == "" goto error1
if not exist test_fcgi.exe goto error2
if not exist test_fcgi.html goto error3

copy test_fcgi.html "%WEBDIR%" >nul
if not exist "%WEBDIR%\test_fcgi.html" goto error4

copy test_fcgi.exe "%WEBDIR%\test_fcgi.fcgi" >nul
if not exist "%WEBDIR%\test_fcgi.fcgi" goto error5

echo.
echo Installed. Use http://localhost/test_fcgi.html to test.
goto end

:error1
echo Error: Missing environment variable 'WEBDIR'.
goto end

:error2
echo Error: File 'test_fcgi.exe' was not found (not built?).
goto end

:error3
echo Error: File 'test_fcgi.html' was not found.
goto end

:error4
echo Error: Unable to copy 'test_fcgi.html' to '%WEBDIR%'.
goto end

:error5
echo Error: Unable to copy 'test_fcgi.fcgi' to '%WEBDIR%'.
goto end

:end

