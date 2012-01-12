@echo off

xrep @config.h.xrep >config.h.new
call update.bat config.h config.h.new

call genmake.bat

