@echo off

if not exist packages.lst echo Standard>packages.lst
if not exist ciyam_class.cpp.xrep call construct @packages.lst ciyam_class.cpp.xrep

