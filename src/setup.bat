@echo off
REM Copyright (c) 2012-2020 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

setlocal

if '%1' == '' goto usage
if '%2' == '' goto usage

if '%3' == '' goto skip
set WEBDIR=%3

:skip
if "%WEBDIR%" == "" goto error1

set install_path=%WEBDIR%\%2

if exist "%install_path%" goto skip_dir
echo Creating %install_path%...
mkdir "%install_path%"

:skip_dir
if exist "%install_path%\.htaccess" goto skip_access
copy .htaccess "%install_path%" >nul

:skip_access
if exist "%install_path%\css" goto skip_css
mkdir "%install_path%\css"
unbundle -qq css -d "%install_path%\css"

:skip_css
if exist "%install_path%\images" goto skip_images
mkdir "%install_path%\images"
unbundle -qq images -d "%install_path%\images"

:skip_images
if exist "%install_path%\popups" goto skip_popups
mkdir "%install_path%\popups"
unbundle -qq popups -d "%install_path%\popups"

:skip_popups
if exist "%install_path%\scripts" goto skip_scripts
mkdir "%install_path%\scripts"
unbundle -qq scripts -d "%install_path%\scripts"

:skip_scripts
if exist "%install_path%\ciyam_interface.js" goto skip_interface_javascript
xrep @ciyam_interface.js.xrep include=%1.app.vars.xrep >"%install_path%\ciyam_interface.js"

:skip_interface_javascript
if not exist "%install_path%\fcgi.sio" copy fcgi.sio "%install_path%" >nul
if not exist "%install_path%\libfcgi.dll" copy libfcgi.dll "%install_path%" >nul

if not exist libeay32.dll goto skip_eay
if not exist "%install_path%\libeay32.dll" copy libeay32.dll "%install_path%" >nul

:skip_eay
if not exist "%install_path%\upload.fcgi" copy upload.exe "%install_path%\upload.fcgi" >nul
if not exist "%install_path%\ciyam_interface.pem" copy ciyam_client.pem "%install_path%\ciyam_interface.pem" >nul
if not exist "%install_path%\ciyam_interface.fcgi" copy ciyam_interface.exe "%install_path%\ciyam_interface.fcgi" >nul

if "%2" == "meta" goto skip_identity
if not exist "%install_path%\identity.txt" copy "%WEBDIR%\meta\identity.txt" "%install_path%" >nul
if not exist "%install_path%\encrypted.txt" copy "%WEBDIR%\meta\encrypted.txt" "%install_path%" >nul

:skip_identity
xrep @footer.htms.xrep >footer.htms
xrep @setup_files.bat.xrep files=@setup_files.lst >~setup.bat
call ~setup.bat "%install_path%"
del ~setup.bat
del footer.htms
goto end

:error1
echo Error: Missing environment variable 'WEBDIR'.
goto end

:usage
echo Usage: setup [app name] [app dir] [[web dir]]

:end
endlocal

