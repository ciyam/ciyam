@echo off
REM Copyright (c) 2013-2014 CIYAM Developers
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.
REM Task #T00000 - Create installation scripts for Linux and Windows

setlocal

set install_path=%PROGRAMFILES%\ciyam

if exist "%install_path%" goto check_git
echo Creating %install_path%...
mkdir "%install_path%"

:check_git
if exist "..\.git\config" goto install_copy
echo "ERROR: this isnt a ciyam git"
exit 1

:install_copy
xcopy ..\src\* "%install_path%" /E /Q /H /R /K /X /Y

exit 0
