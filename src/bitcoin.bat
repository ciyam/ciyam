@echo off
REM Copyright (c) 2014-2020 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

setlocal

if not '%BITCOIN%' == '' goto next
SET BITCOIN="%ProgramFiles%\bitcoin\bitcoin-cli.exe" -rpcpassword=not_what_you_think_it_is

:next
if '%1' == '' goto usage

%BITCOIN% %*
goto end

:usage
echo Usage: bitcoin [command] [[arg1 [arg2 [...]]]

:end
endlocal

