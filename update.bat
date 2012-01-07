@echo off

if '%1' == '' goto usage
if '%2' == '' goto usage

if exist %1 goto check_content
echo Created %1
goto file_copy

:check_content
fd %1 %2 >nul
if not errorlevel 1 goto remove_temp_file
echo Updated %1

:file_copy
copy %2 %1 >nul

:remove_temp_file
del %2
goto end

:usage
echo Usage: update [dest file] [temp file]

:end

