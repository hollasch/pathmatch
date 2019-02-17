@echo off & setlocal EnableDelayedExpansion
REM ------------------------------------------------------------------------------------------------
REM pathmatch Unit Tests
REM ------------------------------------------------------------------------------------------------

REM ------------------------------------------------------------------------------------------------
REM // Run Tests

cd /d %~p0
set testDir=%cd%
set testOutdir=..\out\tests

rmdir /s /q %testOutDir%
mkdir >nul 2>&1 %testOutDir%

if /i "%~1" equ "debug" (
    pushd ..\out\Debug
        path !cd!;!path!
    popd
    echo Testing Debug
) else (
    pushd ..\out\Release
        path !cd!;!path!
    popd
    echo Testing Release
)

if not defined DIFF set DIFF=diff


set passCount=0
set testCount=0
set haltTests=0

echo.
for %%f in (test-*.gold) do (
    call :runTest %%f
    if !haltTests! equ 1 goto :testEnd
)
:testEnd
echo.


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


REM ------------------------------------------------------------------------------------------------
:runTest
    set testName=%~n1
    set testCommand=
    for /f "delims=" %%c in (%1) do (
        set testCommand=%%c
        goto :break
    )
    :break

    set testOutput=%testOutDir%\%testName%.out
    echo %testCommand%|eol \n > %testOutput%
    echo.|eol \n >>%testOutput%
    %testCommand% |eol \n >>%testOutput%

    fc >nul 2>&1 %testName%.gold %testOutput%
    if %ErrorLevel% neq 0 goto :fail
        set /a passCount = !passCount! + 1
        echo pass - %testName%
        goto :endif
    :fail
        echo fail - %testName%
        call diff --unified=8 %testName%.gold %testOutput%
        :query
        set action=n
        set /p action="       [n]ext, [d]iff, [a]ccept, [q]uit? (default is next) "
        if /i "!action!" equ "d" (
            call %DIFF% %testName%.gold %testOutput%
            goto :query
        ) else if /i "!action!" equ "a" (
            copy >nul /y %testOutput% %testName%.gold
            set /a passCount = !passCount! + 1
            goto :endif
        ) else if /i "!action!" equ "n" (
            goto :endif
        ) else if /i "!action!" equ "q" (
            set haltTests=1
            goto :endif
        )
        goto :query
    :endif

    set /a testCount = %testCount% + 1
    goto :eof
