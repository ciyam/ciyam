@echo off
REM Copyright (c) 2012-2014 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if '%1' == '' goto usage

if not exist "packages.lst" call touch.bat packages.lst

xrep @install_package.xrep name=%1 packages=@packages.lst >packages.lst.new
if errorlevel 1 goto end

call update.bat packages.lst packages.lst.new

construct @packages.lst ciyam_class.cpp.xrep

goto end

:usage
echo Usage: install_package [name]

:end

