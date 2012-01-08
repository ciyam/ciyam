@echo off

if '%1' == '' goto next

test -q tests.sio test_%1
goto end

:next
test -q tests.sio

:end

