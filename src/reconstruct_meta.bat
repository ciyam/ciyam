@echo off
REM Copyright (c) 2017 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if not '%1' == 'okay' goto usage

if not exist packages.lst goto skip
construct @packages.lst ciyam_class.cpp.xrep

:skip
call destroy_apps_and_models.bat %1
call reinstall_packages.bat %1

fd autoscript.sio.default autoscript.sio >nul
if errorlevel 1 goto auto
goto skip2

:auto
copy autoscript.sio.default autoscript.sio >nul

:skip2
fd manuscript.sio.default manuscript.sio >nul
if errorlevel 1 goto manu
goto skip3

:manu
copy manuscript.sio.default manuscript.sio >nul

:skip3
goto end

:usage
echo Usage: reconstruct_meta [okay]
echo.
echo WARNING: APPLICATIONS AND MODEL ARTIFACTS WILL BE DESTROYED.

:end

