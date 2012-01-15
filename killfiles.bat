@echo off
if '%1' == '' goto usage

:loop
if exist %1 del %1

shift
if '%1' == '' goto end
goto loop

:usage
echo Usage: killfiles [filename1 [filename2 [...] ] ]

:end

