@echo off
REM Copyright (c) 2017-2019 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if not '%1' == 'okay' goto usage

if not exist packages.lst goto skip
construct @packages.lst ciyam_class.cpp.xrep

:skip
call destroy_apps_and_models.bat %1
call reinstall_packages.bat %1
call install.bat Meta meta
goto end

:usage
echo Usage: reconstruct_meta [okay]
echo.
echo WARNING: APPLICATIONS AND MODEL ARTIFACTS WILL BE DESTROYED.

:end

