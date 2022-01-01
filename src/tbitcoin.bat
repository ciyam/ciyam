@echo off
REM Copyright (c) 2014-2022 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

setlocal

if not '%TBITCOIN%' == '' goto next
SET TBITCOIN="%ProgramFiles%\bitcoin\bitcoin-cli.exe" -regtest -rpcpassword=not_what_you_think_it_is

:next
if '%1' == '' goto usage

%TBITCOIN% %*
goto end

:usage
echo Usage: tbitcoin [command] [[arg1 [arg2 [...]]]

:end
endlocal

