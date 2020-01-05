@echo off
REM Copyright (c) 2012-2020 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

setlocal

set is_rdbms=
set user_vars=

if not '%1' == '-rdbms' goto next
set is_rdbms=1
shift

:next
if '%1' == '' goto usage

if '%is_rdbms%' == '1' goto rdbms

echo model_load %1>~genmodule.tmp
echo generate -cmd=genclass>>~genmodule.tmp

modeller -quiet -no_prompt -no_stderr <~genmodule.tmp
if errorlevel 1 goto end
goto next2

:rdbms
xrep @genmodule.xrep module=%1 all_classes=@%1.classes.lst >~genmodule.bat
call ~genmodule.bat
del ~genmodule.bat

:next2
set user_vars=%1.spec.vars.xrep
if exist %1.txt.new call update.bat %1.txt %1.txt.new

if not exist %1.cpp goto skip_extract
extract %1.cpp >%1.user.xrep
extract %1.cms >>%1.user.xrep
if errorlevel 1 goto end

:skip_extract
if exist %1.user.xrep set user_vars=%1.spec.vars.xrep user_source=%1.user.xrep

xrep @ciyam_main.cpp.xrep user_vars=%user_vars% >%1.cpp.new
if errorlevel 1 goto end
call update.bat %1.cpp %1.cpp.new

xrep @ciyam_main.cms.xrep user_vars=%user_vars% >%1.cms.new
if errorlevel 1 goto end
call update.bat %1.cms %1.cms.new

if not exist %1.spec.vars.xrep.sav goto genfcgi
fd %1.spec.vars.xrep %1.spec.vars.xrep.sav >nul
if errorlevel 1 goto genfcgi
goto skipfcgi

:genfcgi
xrep @module.fcgi.sio.xrep include=%1.spec.vars.xrep >~genmodule.tmp
if errorlevel 1 goto end
call update %1.fcgi.sio ~genmodule.tmp
copy %1.spec.vars.xrep %1.spec.vars.xrep.sav >nul

:skipfcgi
call genmake.bat
if '%2' == '' goto skip_install
call install.bat %1 %2 %3

:skip_install
if exist %1.user.xrep del %1.user.xrep
if exist ~genmodule.tmp del ~genmodule.tmp
goto end

:usage
echo Usage: genmodule [[-rdbms]] [module name] [[app dir]] [[module alias]]

:end
endlocal

