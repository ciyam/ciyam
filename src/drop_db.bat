@echo off
REM Copyright (c) 2012-2022 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if '%1' == '' goto usage

echo DROP USER '%1'@'localhost';>~drop_db
echo DROP DATABASE IF EXISTS %1;>>~drop_db
echo quit>>~drop_db

if not '%2' == '.' goto pwd
mysql -uroot<~drop_db
goto next

:pwd
mysql -uroot -p%2<~drop_db

:next
del ~drop_db>nul
goto end

:usage
echo Usage: drop_db [db_name] [[.]^|[password]]

:end

