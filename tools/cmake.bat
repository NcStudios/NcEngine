@echo off

if "%~1"=="" (
    echo Error: No configuration specified
    EXIT
)

call "%~dp0\jobs\build_external_libs.bat" %1

if not exist "%~dp0\..\build" (
    mkdir "%~dp0\..\build"
)

cd "%~dp0\..\build"

if "%~1"=="Release" (
    echo Using Release Configuration
    if not exist "Release" (
        mkdir "Release"
    )
    cd "Release"
    cmake -DCMAKE_BUILD_TYPE=Release -G "Ninja" "%~dp0\.."
    EXIT
)

if "%~1"=="Release-Profile" (
    echo Using Release Configuration with Profiling
    if not exist "Release-Profile" (
        mkdir "Release-Profile"
    )
    cd "Release-Profile"
    cmake -DCMAKE_BUILD_TYPE=Release -DPROFILING_ENABLED=ON -G "Ninja" "%~dp0\.."
    EXIT
)

if "%~1"=="Debug" (
    echo Using Debug Configuration
    if not exist "Debug" (
        mkdir "Debug"
    )
    cd "Debug"
    cmake -DCMAKE_BUILD_TYPE=Debug -DNC_EDITOR_ENABLED=ON -DNC_TESTS_ENABLED=ON -DVERBOSE_LOGGING_ENABLED=ON -G "Ninja" "%~dp0\.."
    EXIT
)

if "%~1"=="Debug-SanitizeUB" (
    echo Using Debug Configuration with Undefined Behavior Sanitizer
    if not exist "Debug-SanitizeUB" (
        mkdir "Debug-SanitizeUB"
    )
    cd "Debug-SanitizeUB"
    cmake -DCMAKE_BUILD_TYPE=Debug -DNC_EDITOR_ENABLED=ON -DNC_TESTS_ENABLED=ON -DVERBOSE_LOGGING_ENABLED=ON -DSANITIZE_UB=ON -G "Ninja" "%~dp0\.."
    EXIT
)

echo Error: Unknown build configuration