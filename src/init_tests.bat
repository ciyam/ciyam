@echo off
REM Copyright (c) 2012-2022 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if not exist packages.lst echo Standard>packages.lst
call construct @packages.lst ciyam_class.cpp.xrep

if exist ..\.git\hooks\pre-commit del ..\.git\hooks\pre-commit
if exist pre-commit copy pre-commit ..\.git\hooks >nul

