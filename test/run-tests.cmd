@echo off & setlocal EnableDelayedExpansion
REM ------------------------------------------------------------------------------------------------
REM pathmatch Unit Tests
REM ------------------------------------------------------------------------------------------------

set passCount=0
set testCount=0

if %passCount% equ %testCount% (
    echo All ^(%testCount%^) tests passed.
    exit /b 0
)

set /a failCount = %testCount% - %passCount%

if %failCount% equ 1 (
    echo 1 test failed ^(out of %testCount%^).
) else (
    echo !failCount! tests failed ^(out of %testCount%^).
)

exit /b 1
