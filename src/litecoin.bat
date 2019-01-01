@echo off
REM Copyright (c) 2014-2019 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

setlocal

if not '%LITECOIN%' == '' goto next
SET LITECOIN="%ProgramFiles%\litecoin\litecoin-cli.exe" -rpcpassword=not_what_you_think_it_is

:next
if '%1' == '' goto usage

%LITECOIN% %*
goto end

:usage
echo Usage: litecoin [command] [[arg1 [arg2 [...]]]

:end
endlocal

