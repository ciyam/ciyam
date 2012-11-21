@echo off
if '%1' == '' goto usage

if '%2' == '' goto standard_output
curl -s -m 8 %1 >%2
goto end

:standard_output
curl -s -m 8 %1
goto end

:usage
echo Usage: ext_reference [url] [[output_file]]

:end
