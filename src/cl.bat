@echo off
REM Copyright (c) 2012-2020 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if '%1' == '' goto usage

cl.exe /nologo /GR /EHa /Zc:forScope,wchar_t /W3 /wd4068 %* kernel32.lib user32.lib advapi32.lib shell32.lib gdi32.lib winmm.lib /link /IGNORE:4089
goto end

:usage
cl.exe

:end

