@echo off

if not exist packages.lst echo Standard>packages.lst
call construct @packages.lst ciyam_class.cpp.xrep

