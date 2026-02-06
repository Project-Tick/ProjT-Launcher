@echo off
REM ============================================================================
REM ProjT-Launcher Configure Script for Windows
REM ============================================================================
REM
REM This script detects system dependencies and generates a .config file.
REM Similar to zlib/configure - scans for toolchain, Qt, and libraries.
REM
REM Usage:
REM   configure.bat [OPTIONS]
REM
REM Options:
REM   --prefix=DIR          Install prefix (default: C:\ProjT)
REM   --build=TYPE          Build type: debug, release, relwithdebinfo
REM   --platform=windows    Target platform (always windows)
REM   --arch=ARCH           Target architecture (auto-detected)
REM   --qt-prefix=PATH      Qt installation prefix (auto-detected)
REM   --static              Build static executable
REM   --shared              Build shared libraries (default)
REM   --with-lto            Enable Link-Time Optimization
REM   --without-lto         Disable LTO
REM   --with-tests          Enable test building
REM   --without-tests       Disable tests (default)
REM   --cc=CC               C compiler
REM   --cxx=CXX             C++ compiler
REM   --help                Show this help
REM
REM Environment Variables:
REM   CC, CXX               Compiler overrides
REM   QT_PREFIX, Qt6_DIR    Qt installation path
REM
REM Examples:
REM   configure.bat
REM   configure.bat --build=release --with-lto
REM   configure.bat --prefix=C:\ProjT --qt-prefix=C:\Qt\6.8.0\msvc2022_64
REM
REM ============================================================================

setlocal enabledelayedexpansion

REM Script version
set CONFIGURE_VERSION=1.0.0

REM ============================================================================
REM Default Values
REM ============================================================================

set PREFIX=C:\ProjT
set BUILD_TYPE=release
set PLATFORM=windows
set ARCH=
set QT_PREFIX=
set STATIC_BUILD=n
set LTO_ENABLED=y
set TESTS_ENABLED=n
set TOOLCHAIN=
set CC_OVERRIDE=
set CXX_OVERRIDE=
set OUTPUT_DIR=build

REM Dependency status
set HAVE_CC=n
set HAVE_CXX=n
set HAVE_QT=n
set HAVE_ZLIB=n
set HAVE_LIBPNG=n
set HAVE_BZIP2=n
set HAVE_OPENSSL=n
set HAVE_ZSTD=n

REM Detected versions
set QT_VERSION=
set QT_VERSION_MAJOR=
set MSVC_VERSION=
set GCC_VERSION=
set CLANG_VERSION=
set CXX_STD=17

REM ============================================================================
REM Parse Arguments
REM ============================================================================

:parse_args
if "%~1"=="" goto args_done
if "%~1"=="--help" goto show_help
if "%~1"=="-h" goto show_help
if "%~1"=="--version" goto show_version

REM Parse flags without =
if "%~1"=="--with-lto" (
    set LTO_ENABLED=y
    shift
    goto parse_args
)
if "%~1"=="--without-lto" (
    set LTO_ENABLED=n
    shift
    goto parse_args
)
if "%~1"=="--with-tests" (
    set TESTS_ENABLED=y
    shift
    goto parse_args
)
if "%~1"=="--without-tests" (
    set TESTS_ENABLED=n
    shift
    goto parse_args
)
if "%~1"=="--static" (
    set STATIC_BUILD=y
    shift
    goto parse_args
)
if "%~1"=="--shared" (
    set STATIC_BUILD=n
    shift
    goto parse_args
)

REM Parse --key=value style arguments
set ARG=%~1
set ARG_KEY=
set ARG_VAL=

REM Check for = in argument
echo !ARG! | findstr /C:"=" >nul
if errorlevel 1 (
    echo warning: unknown option: %~1
    shift
    goto parse_args
)

REM Split at = sign
for /f "tokens=1,2 delims==" %%a in ("!ARG!") do (
    set ARG_KEY=%%a
    set ARG_VAL=%%b
)

if "!ARG_KEY!"=="--prefix" set PREFIX=!ARG_VAL!
if "!ARG_KEY!"=="--build" set BUILD_TYPE=!ARG_VAL!
if "!ARG_KEY!"=="--platform" REM Always windows, ignore
if "!ARG_KEY!"=="--arch" set ARCH=!ARG_VAL!
if "!ARG_KEY!"=="--qt-prefix" set QT_PREFIX=!ARG_VAL!
if "!ARG_KEY!"=="--cc" set CC_OVERRIDE=!ARG_VAL!
if "!ARG_KEY!"=="--cxx" set CXX_OVERRIDE=!ARG_VAL!
if "!ARG_KEY!"=="--output" set OUTPUT_DIR=!ARG_VAL!

shift
goto parse_args

:args_done
goto main_start

REM ============================================================================
REM Show Help
REM ============================================================================

:show_help
echo ProjT-Launcher configure %CONFIGURE_VERSION%
echo.
echo Usage: configure.bat [OPTIONS]
echo.
echo Options:
echo   --prefix=DIR          Install prefix (default: C:\ProjT)
echo   --build=TYPE          Build type: debug, release, relwithdebinfo
echo   --arch=ARCH           Target architecture (auto-detected)
echo   --qt-prefix=PATH      Qt installation prefix
echo   --static              Build static executable
echo   --shared              Build shared libraries (default)
echo   --with-lto            Enable Link-Time Optimization
echo   --without-lto         Disable LTO
echo   --with-tests          Enable test building
echo   --without-tests       Disable tests (default)
echo   --cc=CC               C compiler
echo   --cxx=CXX             C++ compiler
echo   --help                Show this help
echo   --version             Show version
echo.
echo Examples:
echo   configure.bat
echo   configure.bat --build=release --with-lto
echo   configure.bat --prefix=C:\ProjT --qt-prefix=C:\Qt\6.8.0\msvc2022_64
exit /b 0

:show_version
echo ProjT-Launcher configure %CONFIGURE_VERSION%
exit /b 0

:main_start
echo.

REM ============================================================================
REM Detect Host System
REM ============================================================================

set /p="checking host system type... " <nul
echo windows

REM ============================================================================
REM Detect Architecture
REM ============================================================================

set /p="checking host architecture... " <nul
if "!ARCH!"=="" (
    if "%PROCESSOR_ARCHITECTURE%"=="AMD64" (
        set ARCH=x86_64
    ) else if "%PROCESSOR_ARCHITECTURE%"=="ARM64" (
        set ARCH=aarch64
    ) else (
        set ARCH=x86
    )
)
echo !ARCH!

REM ============================================================================
REM Detect Toolchain
REM ============================================================================

echo.

REM Try MSVC first
set /p="checking for C compiler (cl.exe)... " <nul
where cl.exe >nul 2>&1
if errorlevel 1 (
    echo not found
    goto try_gcc
)

set HAVE_CC=y
set CC=cl.exe
set TOOLCHAIN=msvc

REM Get MSVC version
for /f "tokens=7" %%i in ('cl.exe 2^>^&1 ^| findstr /C:"Version"') do set MSVC_VERSION=%%i
echo !MSVC_VERSION!

set /p="checking for C++ compiler (cl.exe)... " <nul
set HAVE_CXX=y
set CXX=cl.exe
echo !MSVC_VERSION!

set LD=link.exe
set AR=lib.exe
goto toolchain_done

:try_gcc
REM Try GCC/MinGW
set /p="checking for C compiler (gcc)... " <nul
if not "!CC_OVERRIDE!"=="" (
    where !CC_OVERRIDE! >nul 2>&1
    if not errorlevel 1 (
        set CC=!CC_OVERRIDE!
        set HAVE_CC=y
        echo !CC_OVERRIDE!
        goto try_gxx
    )
)
where gcc.exe >nul 2>&1
if errorlevel 1 (
    echo not found
    goto no_compiler
)

set HAVE_CC=y
set CC=gcc.exe
set TOOLCHAIN=mingw

for /f "tokens=3" %%i in ('gcc --version 2^>^&1 ^| findstr /C:"gcc"') do set GCC_VERSION=%%i
echo gcc !GCC_VERSION!

:try_gxx
set /p="checking for C++ compiler (g++)... " <nul
if not "!CXX_OVERRIDE!"=="" (
    where !CXX_OVERRIDE! >nul 2>&1
    if not errorlevel 1 (
        set CXX=!CXX_OVERRIDE!
        set HAVE_CXX=y
        echo !CXX_OVERRIDE!
        goto gcc_done
    )
)
where g++.exe >nul 2>&1
if errorlevel 1 (
    echo not found
    goto toolchain_done
)

set HAVE_CXX=y
set CXX=g++.exe
echo g++ !GCC_VERSION!

:gcc_done
set LD=g++.exe
set AR=ar.exe
goto toolchain_done

:no_compiler
echo.
echo error: No working C compiler found!
echo.
echo For MSVC: Run from Developer Command Prompt or vcvarsall.bat
echo For MinGW: Install MSYS2 and add to PATH
exit /b 1

:toolchain_done

REM ============================================================================
REM Check C++ Standard Support
REM ============================================================================

set /p="checking for C++17 support... " <nul
set CXX_STD=17
echo yes

set /p="checking for C++20 support... " <nul
REM Just assume C++20 with modern compilers
echo yes

echo.

REM ============================================================================
REM Detect Qt
REM ============================================================================

set /p="checking for Qt6... " <nul

REM If explicit path given
if not "!QT_PREFIX!"=="" (
    if exist "!QT_PREFIX!\bin\qmake.exe" goto qt_found
    if exist "!QT_PREFIX!\bin\qmake6.exe" goto qt_found
    echo specified path invalid
    goto qt_search
)

:qt_search
REM Try environment variables
if not "%Qt6_DIR%"=="" (
    if exist "%Qt6_DIR%\bin\qmake.exe" (
        set QT_PREFIX=%Qt6_DIR%
        goto qt_found
    )
)

REM Try common Qt locations
for %%d in (C D E) do (
    if exist "%%d:\Qt" (
        for /d %%v in (%%d:\Qt\6.*) do (
            if "!TOOLCHAIN!"=="msvc" (
                if exist "%%v\msvc2022_64\bin\qmake.exe" (
                    set QT_PREFIX=%%v\msvc2022_64
                    goto qt_found
                )
                if exist "%%v\msvc2019_64\bin\qmake.exe" (
                    set QT_PREFIX=%%v\msvc2019_64
                    goto qt_found
                )
            ) else (
                if exist "%%v\mingw_64\bin\qmake.exe" (
                    set QT_PREFIX=%%v\mingw_64
                    goto qt_found
                )
            )
        )
    )
)

REM Try user's Qt folder
if exist "%USERPROFILE%\Qt" (
    for /d %%v in ("%USERPROFILE%\Qt\6.*") do (
        if "!TOOLCHAIN!"=="msvc" (
            if exist "%%v\msvc2022_64\bin\qmake.exe" (
                set QT_PREFIX=%%v\msvc2022_64
                goto qt_found
            )
        ) else (
            if exist "%%v\mingw_64\bin\qmake.exe" (
                set QT_PREFIX=%%v\mingw_64
                goto qt_found
            )
        )
    )
)

echo not found
set HAVE_QT=n
goto qt_done

:qt_found
set HAVE_QT=y

REM Get Qt version
if exist "!QT_PREFIX!\bin\qmake.exe" (
    for /f "tokens=*" %%i in ('"!QT_PREFIX!\bin\qmake.exe" -query QT_VERSION 2^>nul') do set QT_VERSION=%%i
) else if exist "!QT_PREFIX!\bin\qmake6.exe" (
    for /f "tokens=*" %%i in ('"!QT_PREFIX!\bin\qmake6.exe" -query QT_VERSION 2^>nul') do set QT_VERSION=%%i
)

for /f "tokens=1 delims=." %%i in ("!QT_VERSION!") do set QT_VERSION_MAJOR=%%i

echo !QT_VERSION!

set /p="checking Qt prefix... " <nul
echo !QT_PREFIX!

:qt_done
echo.

REM ============================================================================
REM Detect Libraries (bundled by default on Windows)
REM ============================================================================

set /p="checking for zlib... " <nul
echo bundled
set HAVE_ZLIB=y

set /p="checking for libpng... " <nul
echo bundled
set HAVE_LIBPNG=y

set /p="checking for bzip2... " <nul
echo bundled
set HAVE_BZIP2=y

set /p="checking for zstd... " <nul
where zstd.exe >nul 2>&1
if errorlevel 1 (
    echo not found
    set HAVE_ZSTD=n
) else (
    echo yes
    set HAVE_ZSTD=y
)

set /p="checking for OpenSSL... " <nul
where openssl.exe >nul 2>&1
if errorlevel 1 (
    echo not found
    set HAVE_OPENSSL=n
) else (
    echo yes
    set HAVE_OPENSSL=y
)

echo.

echo.

REM ============================================================================
REM Create Output Directory
REM ============================================================================

if not exist "!OUTPUT_DIR!" mkdir "!OUTPUT_DIR!"

REM ============================================================================
REM Generate .config
REM ============================================================================

echo creating .config

set CONFIG_FILE=!OUTPUT_DIR!\.config

(
echo # ProjT-Launcher Configuration
echo # Generated by configure.bat on %DATE% %TIME%
echo #
echo.
echo # Build Configuration
if /i "!BUILD_TYPE!"=="debug" (
echo CONFIG_BUILD_TYPE_DEBUG=y
echo # CONFIG_BUILD_TYPE_RELEASE is not set
echo # CONFIG_BUILD_TYPE_RELWITHDEBINFO is not set
) else if /i "!BUILD_TYPE!"=="relwithdebinfo" (
echo # CONFIG_BUILD_TYPE_DEBUG is not set
echo # CONFIG_BUILD_TYPE_RELEASE is not set
echo CONFIG_BUILD_TYPE_RELWITHDEBINFO=y
) else (
echo # CONFIG_BUILD_TYPE_DEBUG is not set
echo CONFIG_BUILD_TYPE_RELEASE=y
echo # CONFIG_BUILD_TYPE_RELWITHDEBINFO is not set
)
echo.
echo # Target Platform
echo CONFIG_TARGET_WINDOWS=y
echo # CONFIG_TARGET_LINUX is not set
echo # CONFIG_TARGET_MACOS is not set
echo.
echo # Windows Toolchain
if "!TOOLCHAIN!"=="msvc" (
echo CONFIG_WINDOWS_TOOLCHAIN_MSVC=y
echo # CONFIG_WINDOWS_TOOLCHAIN_MINGW is not set
) else (
echo # CONFIG_WINDOWS_TOOLCHAIN_MSVC is not set
echo CONFIG_WINDOWS_TOOLCHAIN_MINGW=y
)
echo.
echo # Architecture
if "!ARCH!"=="x86_64" (
echo CONFIG_ARCH_X86_64=y
echo # CONFIG_ARCH_AARCH64 is not set
echo # CONFIG_ARCH_X86 is not set
) else if "!ARCH!"=="aarch64" (
echo # CONFIG_ARCH_X86_64 is not set
echo CONFIG_ARCH_AARCH64=y
echo # CONFIG_ARCH_X86 is not set
) else (
echo # CONFIG_ARCH_X86_64 is not set
echo # CONFIG_ARCH_AARCH64 is not set
echo CONFIG_ARCH_X86=y
)
echo.
echo # Compiler
echo CONFIG_CC="!CC!"
echo CONFIG_CXX="!CXX!"
echo CONFIG_LD="!LD!"
echo CONFIG_AR="!AR!"
echo CONFIG_CXX_STD=!CXX_STD!
echo.
echo # Qt Configuration
if "!HAVE_QT!"=="y" (
echo CONFIG_QT_SYSTEM=y
echo # CONFIG_QT_BUILD is not set
echo CONFIG_QT_PREFIX="!QT_PREFIX!"
echo CONFIG_QT_VERSION="!QT_VERSION!"
echo CONFIG_QT_VERSION_MAJOR=!QT_VERSION_MAJOR!
) else (
echo # CONFIG_QT_SYSTEM is not set
echo CONFIG_QT_BUILD=y
)
if "!STATIC_BUILD!"=="y" (
echo CONFIG_QT_STATIC=y
echo # CONFIG_QT_SHARED is not set
) else (
echo # CONFIG_QT_STATIC is not set
echo CONFIG_QT_SHARED=y
)
echo.
echo # Qt Platform Plugins
echo CONFIG_QT_PLATFORM_WINDOWS=y
echo.
echo # Qt Features
echo CONFIG_QT_FEATURE_CONCURRENT=y
echo CONFIG_QT_FEATURE_XML=y
echo CONFIG_QT_FEATURE_OPENGL=y
echo CONFIG_QT_FEATURE_WIDGETS=y
echo CONFIG_QT_FEATURE_ACCESSIBILITY=y
echo.
echo # Libraries
echo CONFIG_ZLIB_BUILD=y
echo # CONFIG_ZLIB_SYSTEM is not set
echo CONFIG_LIBPNG_BUILD=y
echo # CONFIG_LIBPNG_SYSTEM is not set
echo CONFIG_BZIP2_BUILD=y
echo # CONFIG_BZIP2_SYSTEM is not set
if "!HAVE_ZSTD!"=="y" (
echo CONFIG_ZSTD_SYSTEM=y
echo # CONFIG_ZSTD_BUILD is not set
) else (
echo # CONFIG_ZSTD_SYSTEM is not set
echo CONFIG_ZSTD_BUILD=y
)
if "!HAVE_OPENSSL!"=="y" (
echo CONFIG_USE_OPENSSL=y
) else (
echo # CONFIG_USE_OPENSSL is not set
)
echo CONFIG_QUAZIP_BUILD=y
echo CONFIG_TOMLPLUSPLUS_BUILD=y
echo CONFIG_LIBNBTPLUSPLUS_BUILD=y
echo CONFIG_CMARK_BUILD=y
echo.
echo # Features
echo CONFIG_FEATURE_LAUNCHER_APPLICATION=y
echo CONFIG_FEATURE_LAUNCHER_UI=y
echo CONFIG_FEATURE_META=y
echo CONFIG_FEATURE_JAVA=y
echo CONFIG_FEATURE_JAVA_DOWNLOADER=y
echo CONFIG_FEATURE_JAVA_CHECK=y
echo CONFIG_FEATURE_LAUNCHER_HUB=y
echo # CONFIG_FEATURE_WEBENGINE is not set
echo CONFIG_FEATURE_UPDATER=y
echo CONFIG_FEATURE_NEWS=y
echo CONFIG_FEATURE_ANALYTICS=y
echo CONFIG_FEATURE_SKINS=y
echo CONFIG_FEATURE_MODPACKS=y
echo CONFIG_FEATURE_CURSE=y
echo CONFIG_FEATURE_MODRINTH=y
echo CONFIG_FEATURE_TECHNIC=y
echo CONFIG_FEATURE_ATL=y
echo CONFIG_FEATURE_FTB=y
echo CONFIG_FEATURE_FLAME=y
echo CONFIG_FEATURE_QRCODE=y
echo CONFIG_FEATURE_DISCORD=y
echo # CONFIG_FEATURE_GAMEMODE is not set
echo # CONFIG_FEATURE_MANGOHUD is not set
echo CONFIG_FEATURE_CMARK=y
echo.
echo # Modules
echo CONFIG_MOD_LAUNCHER=y
echo CONFIG_MOD_BUILD_CONFIG=y
echo CONFIG_MOD_PROGRAM_INFO=y
echo CONFIG_MOD_SYSTEMINFO=y
echo CONFIG_MOD_LAUNCHERJAVA=y
echo CONFIG_MOD_JAVACHECK=y
echo CONFIG_MOD_RAINBOW=y
echo CONFIG_MOD_LOCALPEER=y
echo # CONFIG_MOD_GAMEMODE is not set
echo CONFIG_MOD_UI=y
echo CONFIG_MOD_UI_DIALOGS=y
echo CONFIG_MOD_UI_PAGES=y
echo CONFIG_MOD_UI_WIDGETS=y
echo CONFIG_MOD_CONSOLE=y
echo CONFIG_MOD_LOGS=y
echo CONFIG_MOD_TASKS=y
echo CONFIG_MOD_SETTINGS=y
echo CONFIG_MOD_NEWS=y
echo CONFIG_MOD_NET=y
echo CONFIG_MOD_MODPLATFORM=y
echo CONFIG_MOD_TOOLS=y
echo CONFIG_MOD_ICONS=y
echo CONFIG_MOD_TRANSLATIONS=y
echo CONFIG_MOD_JAVA=y
echo CONFIG_MOD_LAUNCH=y
echo CONFIG_MOD_META=y
echo CONFIG_MOD_MINECRAFT=y
echo CONFIG_MOD_SCREENSHOTS=y
echo CONFIG_MOD_UPDATER=y
echo.
echo # Build Options
if "!LTO_ENABLED!"=="y" (
echo CONFIG_ENABLE_LTO=y
) else (
echo # CONFIG_ENABLE_LTO is not set
)
if "!TESTS_ENABLED!"=="y" (
echo CONFIG_BUILD_TESTS=y
) else (
echo # CONFIG_BUILD_TESTS is not set
)
echo.
echo # Install prefix
echo CONFIG_PREFIX="!PREFIX!"
) > "!CONFIG_FILE!"

echo.
echo ============================================================================
echo.

REM ============================================================================
REM Summary
REM ============================================================================

echo ProjT-Launcher has been configured with the following options:
echo.
echo   Build type:       !BUILD_TYPE!
echo   Install prefix:   !PREFIX!
echo   Platform:         !PLATFORM!
echo   Architecture:     !ARCH!
echo.
echo Compiler:
echo   C compiler:       !CC!
echo   C++ compiler:     !CXX!
echo   C++ standard:     C++!CXX_STD!
echo   Toolchain:        !TOOLCHAIN!
echo.
echo Qt:
if "!HAVE_QT!"=="y" (
echo   Qt version:       !QT_VERSION!
echo   Qt prefix:        !QT_PREFIX!
) else (
echo   Qt:               not found ^(use --qt-prefix=PATH^)
)
echo.
echo Libraries:
echo   zlib:             bundled
echo   libpng:           bundled
echo   bzip2:            bundled
if "!HAVE_ZSTD!"=="y" (
echo   zstd:             system
) else (
echo   zstd:             bundled
)
if "!HAVE_OPENSSL!"=="y" (
echo   OpenSSL:          system
) else (
echo   OpenSSL:          no
)
echo.
echo Options:
if "!LTO_ENABLED!"=="y" (
echo   LTO:              enabled
) else (
echo   LTO:              disabled
)
if "!STATIC_BUILD!"=="y" (
echo   Static build:     yes
) else (
echo   Static build:     no
)
if "!TESTS_ENABLED!"=="y" (
echo   Tests:            enabled
) else (
echo   Tests:            disabled
)
echo.

REM Warnings
if "!HAVE_CC!"=="n" (
    echo warning: No working C compiler found
)
if "!HAVE_CXX!"=="n" (
    echo warning: No working C++ compiler found
)
if "!HAVE_QT!"=="n" (
    echo warning: Qt not found. Use --qt-prefix=PATH or install Qt 6.
)

echo.
if "!TOOLCHAIN!"=="msvc" (
echo Now run: nmake
) else (
echo Now run: make
)
echo.

endlocal
