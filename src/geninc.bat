@echo off
REM Copyright (c) 2012-2021 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

for %%n in ( %* ) do call genincx.bat check %%n
echo.
for %%n in ( %* ) do call genincx.bat define %%n

:end
