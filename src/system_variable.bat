@echo off
REM Copyright (c) 2022 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if '%1' == '' goto usage
if '%2' == '' goto usage

echo ^<^<system_variable.cin %1 %2 %3 >~system_variable
ciyam_client -echo -quiet -no_prompt -no_stderr < ~system_variable

del ~system_variable
goto end

:usage
echo Usage: system_variable get|set [name] [[value]]

:end
