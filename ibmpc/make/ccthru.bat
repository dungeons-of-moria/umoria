@echo off
touch %1.%2 > nul:
copy %1.%2 ..\%2 > nul:
touch ..\%2\%1.%2 > nul:
