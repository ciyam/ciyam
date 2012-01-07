@echo off

if '%1' == '' goto usage

cl.exe /nologo /GR /EHa /Zc:forScope,wchar_t /W3 /wd4068 %* kernel32.lib user32.lib advapi32.lib shell32.lib gdi32.lib winmm.lib /link /IGNORE:4089
goto end

:usage
cl.exe

:end

