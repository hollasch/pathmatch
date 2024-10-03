@echo off & setlocal EnableDelayedExpansion
REM ------------------------------------------------------------------------------------------------
REM pathmatch Unit Tests
REM ------------------------------------------------------------------------------------------------

REM ------------------------------------------------------------------------------------------------
REM // Run Tests

chcp 65001

cd /d %~p0
set testDir=%cd%
set testOutdir=..\out\tests
set releaseBuild=1

rmdir /s /q %testOutDir%
mkdir >nul 2>&1 %testOutDir%

cd ..
if %releaseBuild% equ 1 (
    echo Testing Release
    set pathmatch=!cd!\build\Release\pathmatch.exe
    call cmake --build build --config release
) else (
    echo Testing Debug
    set pathmatch=!cd!\build\Debug\pathmatch.exe
    call cmake --build build --config debug
)
cd test

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
    set testArgs=
    for /f "delims=" %%c in (%1) do (
        set testArgs=%%c
        goto :break
    )
    :break

    echo.
    echo --------------------------------------------------------------------------------
    echo %testName%
    echo pathmatch %testArgs%
    echo.

    set testOutput=%testOutDir%\%testName%.out
    echo>%testOutput% %testArgs%
    echo.>>%testOutput%
    %pathmatch% %testArgs%>>%testOutput%

    move >nul %testOutput% %testOutput%.original
    eol \n <%testOutput%.original >%testOutput%
    del %testOutput%.original

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
            call "%DIFF%" %testName%.gold %testOutput%
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
