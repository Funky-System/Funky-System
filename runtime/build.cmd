@echo off
setlocal enableextensions enabledelayedexpansion

set arch=32

set wickedc=wickedc.exe

WHERE "%wickedc%"
IF %ERRORLEVEL% NEQ 0 do (
    set wickedc=%~dp0\..\..\bin\wickedc.exe
)

if not exist "%2" mkdir "%2"

FOR %%f IN ("%1\*.wckd") DO (
    echo Compiling runtime object %%~nf
    "%wickedc%" --only-if-newer --arch %arch% "%%f" -o "%2\%%~nf.funk"
)
