@echo off
if '%1' == '' goto usage

findstr /R "^field_add.*Specification_Type.*%1" Meta.cin
goto end

:usage
echo Usage: meta_field [name]

:end
