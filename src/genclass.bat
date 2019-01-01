@echo off
REM Copyright (c) 2012-2019 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

setlocal

set is_rdbms=
set full_name=
set user_vars=

if not '%1' == '-rdbms' goto next
set is_rdbms=1
shift

:next
if '%1' == '' goto usage
if '%2' == '' goto usage

set full_name=%1_%2
set user_vars=user_vars=%full_name%.vars.xrep

if '%is_rdbms%' == '1' goto check

echo model_load %1>~genclass.tmp
echo generate %2>>~genclass.tmp

modeller -quiet -no_prompt <~genclass.tmp
if errorlevel 1 goto end

:check
if not exist %full_name%.vars.xrep.sav goto gen
fd %full_name%.vars.xrep %full_name%.vars.xrep.sav >nul
if errorlevel 1 goto gen
goto skipgen

:gen
if not exist %full_name%.cpp goto skip_extract
extract %full_name%.cpp >~genclass.tmp
if errorlevel 1 goto end
call update %full_name%.cpp.xrep ~genclass.tmp >nul

:skip_extract
if exist %full_name%.cpp.xrep set user_vars=%user_vars% user_source=%full_name%.cpp.xrep

xrep @ciyam_class.h.xrep %user_vars% >%full_name%.h.new
if errorlevel 1 goto end
call update.bat %full_name%.h %full_name%.h.new

xrep @ciyam_class.cpp.xrep %user_vars% >%full_name%.cpp.new
if errorlevel 1 goto end
call update.bat %full_name%.cpp %full_name%.cpp.new

xrep @ciyam_class.cms.xrep %user_vars% >%full_name%.cms.new
if errorlevel 1 goto end
call update.bat %full_name%.cms %full_name%.cms.new

copy %full_name%.vars.xrep %full_name%.vars.xrep.sav >nul

:skipgen
if exist %full_name%.csv.new call update.bat %full_name%.csv %full_name%.csv.new

call genpdfs.bat %1 %2

if exist ~genclass.tmp del ~genclass.tmp
if exist %full_name%.cpp.xrep del %full_name%.cpp.xrep
if exist %full_name%.vars.xrep del %full_name%.vars.xrep
goto end

:usage
echo Usage: genclass [[-rdbms]] [module name] [class name]

:end
endlocal

