@echo off
REM Copyright (c) 2012-2020 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

setlocal

set is_rebuild=

if not '%1' == '-rebuild' goto next
shift
set is_rebuild=true

:next
if '%1' == '' goto usage

echo DROP DATABASE IF EXISTS %1;>~create_db
echo CREATE DATABASE %1 CHARACTER SET utf8 COLLATE utf8_bin;>>~create_db

if not '%is_rebuild%' == 'true' goto skip
echo GRANT USAGE ON *.* TO '%1'@'localhost';>>~create_db
echo DROP USER '%1'@'localhost';>>~create_db

:skip
echo CREATE USER '%1'@'localhost';>>~create_db
echo GRANT ALL PRIVILEGES ON %1.* TO '%1'@'localhost';>>~create_db
echo quit>>~create_db

if not '%2' == '.' goto pwd
mysql -uroot<~create_db
goto next2

:pwd
mysql -uroot -p%2<~create_db

:next2
del ~create_db>nul
goto end

:usage
echo Usage: create_db [[-rebuild]] [db_name] [[.]^|[password]]

:end
endlocal

