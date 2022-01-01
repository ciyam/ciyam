@echo off
REM Copyright (c) 2012-2022 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

setlocal

set do_not_log=
set log_on_error=

if not '%1' == '-do_not_log' goto next1
shift
set do_not_log=1

:next1
if not '%1' == '-log_on_error' goto next2
shift
set log_on_error=1

:next2
if '%1' == '' goto usage

if '%3' == '' goto skip
echo %date% %time% [%3]>>%1.log

:skip
ciyam_client -echo -quiet -no_prompt -no_stderr -no_progress -args_file=%1 < %1 >>%1.log

if '%do_not_log%' == '1' goto skip3

if not '%log_on_error%' == '1' goto skip2
if not errorlevel 1 goto skip3

:skip2
type %1.log >> %2/ciyam_script.log

:skip3
del %1
del %1.log
endlocal
exit

:usage
echo Usage: script [[-do_not_log^|-log_on_error]] [script and args file] [files area dir] [[script name]]
endlocal
