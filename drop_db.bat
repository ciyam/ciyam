@echo off

if '%1' == '' goto usage

echo DROP USER '%1'@'localhost';>~drop_db
echo DROP DATABASE IF EXISTS %1;>>~drop_db

mysql -uroot -p%2<~drop_db
del ~drop_db>nul
goto end

:usage
echo Usage: drop_db [db_name] [[password]]

:end
