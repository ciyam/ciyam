@echo off
REM Copyright (c) 2012-2022 CIYAM Developers
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
if exist %1.cpp goto error1

if '%is_rdbms%' == '1' goto skip_modeller

if exist %1.sio goto skip_modeller
if exist %1.cin goto skip_create

(echo model_info -name=%1 -version=0.1)>%1.cin
echo model_save>>%1.cin
echo Created '%1.cin'.

:skip_create
modeller < %1.cin > nul
if not exist %1.sio goto error2

:skip_modeller
xrep @create_module.xrep name=%1 modules=@modules.lst >modules.lst.new
if errorlevel 1 goto end

call update.bat modules.lst modules.lst.new

if '%is_rdbms%' == '1' goto skip_generate
call genmodule %1

:skip_generate
goto end

:error1
echo Error: Module '%1' already exists.
goto end

:error2
echo Error: Running script '%1.cin' failed to create '%1.sio'.
goto end

:usage
echo Usage: create_module [[-rdbms]] [module_name]

:end
endlocal

