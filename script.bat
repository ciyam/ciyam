@echo off

if '%1' == '' goto usage

if '%2' == '' goto skip
echo %date% %time% [%2] >>%1.log

:skip
ciyam_client -echo -quiet -no_prompt < %1 >>%1.log

type %1.log >> script.log

del %1
del %1.log
exit

:usage
echo Usage: script [script args file] [[script name]]

