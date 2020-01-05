@echo off
REM Copyright (c) 2012-2020 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if not '%1' == '' goto show

echo encrypt> ~password.cin
goto run_script

:show
echo encrypt %1> ~password.cin

:run_script
ciyam_client -tls -quiet -no_prompt <~password.cin
del ~password.cin
