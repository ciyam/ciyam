@echo off
REM Copyright (c) 2021 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if '%1' == '' goto usage

echo ^<^<kill_old_ts_tagged_files.cin %1 %2 >~kill_old_ts_tagged_files
ciyam_client -echo -quiet -no_prompt -no_stderr < ~kill_old_ts_tagged_files

del ~kill_old_ts_tagged_files
goto end

:usage
echo Usage: kill_old_ts_tagged_files [tz abbr]

:end
