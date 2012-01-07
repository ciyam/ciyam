@echo off

if '%1' == '' goto usage

:loop
type nul >>%1 & copy %1 +,, >nul

if '%2' == '' goto end
shift
goto loop

:usage
echo touch [filename] [[filename2 [...]]]

:end

