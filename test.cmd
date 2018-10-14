@echo off & setlocal 

set testOut=..\build\tests

cd test
rmdir /s /q %testOut%
mkdir >nul 2>&1 %testOut%

call run-tests.cmd
