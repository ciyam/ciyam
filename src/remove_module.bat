@echo off
REM Copyright (c) 2012-2020 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

setlocal

set is_rdbms=

if not '%1' == '-rdbms' goto next
set is_rdbms=1
shift

:next
if '%1' == '' goto usage

if '%is_rdbms%' == '1' goto skip_check
if not exist %1.cpp goto error1

:skip_check
xrep @remove_module.xrep name=%1 modules=@modules.lst >modules.lst.new
if errorlevel 1 goto end

call update.bat modules.lst modules.lst.new
call genmake.bat

if not '%is_rdbms%' == '1' goto skip_file_remove
xrep @remove_module_classes.xrep module=%1 all_classes=@%1.classes.lst >~remove_module.bat
call ~remove_module.bat
del ~remove_module.bat

if exist %1.cms del %1.cms
if exist %1.cmh del %1.cmh
if exist %1.cpp del %1.cpp
if exist %1.obj del %1.obj
if exist %1.lib del %1.lib
if exist %1.dll del %1.dll
if exist %1.exp del %1.exp
if exist %1.dep del %1.dep
if exist %1.inc del %1.inc
if exist %1.txt del %1.txt
if exist %1.sql del %1.sql
if exist %1.init.lst del %1.init.lst
if exist %1.fcgi.sio del %1.fcgi.sio
if exist %1.vars.xrep del %1.vars.xrep
if exist %1.classes.lst del %1.classes.lst
if exist %1.acyclic.lst del %1.acyclic.lst
if exist %1.make.vars.xrep del %1.make.vars.xrep
if exist %1.spec.vars.xrep del %1.spec.vars.xrep
if exist %1.spec.vars.xrep.sav del %1.spec.vars.xrep.sav
goto end

:skip_file_remove
echo.
echo *** Module has been removed (it should now be safe to delete all applicable '%1*' files).
goto end

:error1
echo Error: Module '%1' does not exist.
goto end

:usage
echo Usage: remove_module [[-rdbms]] [module_name]

:end
endlocal

