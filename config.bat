@echo off

if not exist config.info copy config.info.default config.info
if not exist ciyam_server.sio copy ciyam_server.sio.default ciyam_server.sio

xrep @config.h.xrep >config.h.new
call update.bat config.h config.h.new

call genmake.bat

