@echo off
REM Copyright (c) 2012-2019 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

setlocal

if '%1' == '' goto usage

test_sql Meta Meta "SELECT CONCAT('SET CIYAM_MODEL=', C_Key_) FROM T_Meta_Model WHERE C_Name = '%1'">~generate.bat
call ~generate.bat
del ~generate.bat
if '%CIYAM_MODEL%' == '' goto error1

echo Starting generate...
echo ^<generate.cin %CIYAM_MODEL%>~generate.cin
ciyam_client -quiet -no_prompt -no_stderr < ~generate.cin >~generate.tmp
del ~generate.cin

type ~generate.tmp
findstr "Error error" ~generate.tmp >nul
if errorlevel 1 goto okay
del ~generate.tmp
goto end

:okay
del ~generate.tmp
call genmodule.bat -rdbms %1 %2 %3

echo ^<genlinks.cin %CIYAM_MODEL%>~genlinks.cin
ciyam_client -quiet -no_prompt -no_stderr < ~genlinks.cin >~genlinks.tmp
type ~genlinks.tmp
del ~genlinks.tmp
del ~genlinks.cin
if exist manuscript.sio.new call remove_identical.bat manuscript.sio manuscript.sio.new
echo Finished generate...
goto end

:error1
echo Error: Unknown module '%1'.
goto end

:usage
echo Usage: generate [module name] [[app dir]] [[module alias]]

:end
endlocal

