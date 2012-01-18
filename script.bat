@echo off

if '%1' == '' goto usage

if '%2' == '' goto skip
echo %date% %time% [%2] >>script.log

:skip
ciyam_client -echo -quiet -no_prompt < %1 >>script.log
del %1
exit

:usage
echo Usage: script [script args file] [[script name]]

