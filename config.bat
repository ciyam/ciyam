@echo off

if not exist config.info copy config.info.default config.info

xrep @config.h.xrep >config.h.new
call update.bat config.h config.h.new

call genmake.bat

