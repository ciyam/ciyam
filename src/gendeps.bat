@echo off
REM Copyright (c) 2012-2018 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

setlocal

if '%1' == '' goto usage

if not exist xrep.exe goto no_xrep

xrep @gendeps.xrep "all_files=%*" >~gendeps.bat
call ~gendeps.bat
del ~gendeps.bat
goto end

:no_xrep
if '%1' == '' goto end
set next_cpp=%1
set next_dep=%next_cpp:.cpp=.dep%
gendeps.exe %next_cpp% > %next_dep%
shift
goto no_xrep

:usage
echo usage: gendeps [file_1.cpp [file_2.cpp [...]]]

:end
endlocal

