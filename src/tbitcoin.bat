@echo off
REM Copyright (c) 2014-2015 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

setlocal

SET BITCOIN="C:\Program Files\bitcoin\bitcoin-cli.exe" -testnet -rpcpassword=not_what_you_think_it_is

if '%1' == '' goto usage

%BITCOIN% %*
goto end

:usage
echo Usage: tbitcoin [command] [[arg1 [arg2 [...]]]

:end
endlocal

