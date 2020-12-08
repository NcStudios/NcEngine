@echo off

if "%~1"=="" (
    echo Error: No configuration specified
    EXIT
)

if not exist "%~dp0\..\external\lib\libimgui.a" (
    call "jobs\build_external_libs.bat"
)

if not exist "%~dp0\..\external\lib\libenet.a" (
    call "jobs\build_external_libs.bat"
)

if not exist "%~dp0\..\build" (
    mkdir "%~dp0\..\build"
)

cd "%~dp0\..\build"

if "%~1"=="Debug" (
    echo Using Debug Configuration
    if not exist "Debug" (
        mkdir "Debug"
    )
    cd "Debug"
    cmake -DCMAKE_BUILD_TYPE=Debug -DNC_EDITOR_ENABLED=ON -DNC_TESTS_ENABLED=ON -DVERBOSE_LOGGING_ENABLED=ON -G "Ninja" "%~dp0\.."
)

if "%~1"=="Release" (
    echo Using Release Configuration
    if not exist "Release" (
        mkdir "Release"
    )
    cd "Release"
    cmake -DCMAKE_BUILD_TYPE=Release -DNC_EDITOR_ENABLED=OFF -DNC_TESTS_ENABLED=OFF -DVERBOSE_LOGGING_ENABLED=OFF -G "Ninja" "%~dp0\.."
)

if "%~1"=="Ninja" (
    echo Using Ninja
    if not exist "Ninja" (
        mkdir "Ninja"
    )
    cd "Ninja"
    cmake -DCMAKE_BUILD_TYPE=Debug -DNC_EDITOR_ENABLED=ON -DNC_TESTS_ENABLED=ON -DVERBOSE_LOGGING_ENABLED=ON -G "Ninja" "%~dp0\.."
)
