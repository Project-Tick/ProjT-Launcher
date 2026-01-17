@echo off
REM ProjT Launcher - Windows Configure Wrapper
REM This script tries to run the bash configure script.
REM If bash is not available, it provides basic config.mk generation.

REM Try Git Bash first
where bash >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo [INFO] Running configure via bash...
    bash ./configure %*
    exit /b %ERRORLEVEL%
)

REM Try sh (MSYS2/MinGW)
where sh >nul 2>&1
if %ERRORLEVEL% EQU 0 (
    echo [INFO] Running configure via sh...
    sh ./configure %*
    exit /b %ERRORLEVEL%
)

REM No bash found - provide instructions
echo.
echo ==========================================
echo   ProjT Launcher Configure
echo ==========================================
echo.
echo [ERROR] Bash not found!
echo.
echo To run configure on Windows, you need one of:
echo   1. Git Bash (comes with Git for Windows)
echo   2. MSYS2 / MinGW
echo   3. WSL (Windows Subsystem for Linux)
echo.
echo Install one of these and run:
echo   ./configure
echo.
echo Or run from Git Bash:
echo   bash ./configure
echo.
exit /b 1
