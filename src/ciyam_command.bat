@echo off
REM Copyright (c) 2022 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if '%1' == '' goto usage

echo ^<^<ciyam_command.cin %1 %2" >~ciyam_command
ciyam_client -echo -quiet -no_prompt -no_stderr < ~ciyam_command

del ~ciyam_command
goto end

:usage
echo Usage: ciyam_command [cmd] [[args,]]

:end
