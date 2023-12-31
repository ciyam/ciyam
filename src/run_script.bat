@echo off
REM Copyright (c) 2019-2024 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if '%1' == '' goto usage

echo ^<^<run_script.cin %1 %2 >~run_script
ciyam_client -echo -quiet -no_prompt -no_stderr < ~run_script

del ~run_script
goto end

:usage
echo Usage: run_script [script name] [[script args,]]

:end
