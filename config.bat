@echo off

if not exist config.info copy config.info.default config.info
if not exist modules.lst copy modules.lst.default modules.lst
if not exist ciyam_server.sio copy ciyam_server.sio.default ciyam_server.sio

if exist xrep.exe goto has_tools
if not exist makefile.bcb copy makefile.bcb.tools makefile.bcb
if not exist makefile.mvc copy makefile.mvc.tools makefile.mvc
call make.bat xrep xvars

:has_tools
if not exist salt.h xrep @salt.h.xrep >salt.h
xrep @config.h.xrep >config.h.new
call update.bat config.h config.h.new

call genmake.bat

