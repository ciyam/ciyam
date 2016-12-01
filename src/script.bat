@echo off
REM Copyright (c) 2012-2016 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if '%1' == '' goto usage

if '%2' == '' goto skip
echo %date% %time% [%2]>>%1.log

:skip
ciyam_client -echo -quiet -no_prompt -no_stderr -args_file=%1 < %1 >>%1.log

type %1.log >> ciyam_script.log

del %1
del %1.log
exit

:usage
echo Usage: script [script and args file] [[script name]]

