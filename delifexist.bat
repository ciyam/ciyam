@echo off

:loop
if '%1' == '' goto end
if exist %1 del %1
shift
goto loop

:end

