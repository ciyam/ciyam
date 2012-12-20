@echo off
REM Copyright (c) 2012 CIYAM Open Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if '%1' == 'check' goto check
if '%1' == 'define' goto define
goto usage

:check
echo !ifndef %2_dep
echo ! include %2.dep
echo !endif
goto end

:define
echo %2_dep=1
goto end

:usage
echo usage: genincx [[check][define]] [filename]

:end

