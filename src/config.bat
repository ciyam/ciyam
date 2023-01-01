@echo off
REM Copyright (c) 2012-2023 CIYAM Developers
REM
REM Distributed under the MIT/X11 software license, please refer to the file license.txt
REM in the root project directory or http://www.opensource.org/licenses/mit-license.php.

if not exist config.h copy config.h.default config.h
if not exist config.info copy config.info.default config.info
if not exist modules.lst copy modules.lst.default modules.lst
if not exist autoscript.sio copy autoscript.sio.default autoscript.sio
if not exist manuscript.sio copy manuscript.sio.default manuscript.sio
if not exist ciyam_server.sio copy ciyam_server.sio.default ciyam_server.sio

if exist xrep.exe goto has_tools
if not exist makefile.bcb copy makefile.bcb.tools makefile.bcb
if not exist makefile.mvc copy makefile.mvc.tools makefile.mvc
call make.bat xrep xvars

if not exist xrep.exe goto error1

:has_tools
if not exist salt.h xrep @salt.h.xrep >salt.h

if exist ciyam_server.sid goto skip
xrep @ciyam_server.sid.bat.xrep > ~config.bat
call ~config.bat
rm ~config.bat

:skip
xrep @config.h.xrep >config.h.new
call update.bat config.h config.h.new

call genmake.bat
goto end

:error1
echo Error: Project tool 'xrep.exe' was not found.

:end
