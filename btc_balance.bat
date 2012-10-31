@echo off

if '%1' == '' goto usage

if '%2' == '' goto standard_output
curl -s -m 8 http://blockchain.info/q/addressbalance/%1?confirmations=1 >%2
goto end

:standard_output
curl -s -m 8 http://blockchain.info/q/addressbalance/%1?confirmations=1
echo.
goto end

:usage
echo Usage: btc_balance [address] [[output_file]]
:end

