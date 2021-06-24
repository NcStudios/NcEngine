@echo off

set TARGET="%~1"
set CONFIGURATION="%~2"

:CheckTarget
    if %TARGET%=="Engine" (
        echo Building Engine with %CONFIGURATION% Configuration
        set SOURCE_DIR="%~dp0\..\nc"
        goto CheckConfiguration
    )
    if %TARGET%=="Project" (
        echo Building Project with %CONFIGURATION% Configuration
        set SOURCE_DIR="%~dp0\..\project"
        goto CheckConfiguration
    )

    echo Error: Invalid Target - %TARGET%
    EXIT

:CheckConfiguration
    if %CONFIGURATION%=="Debug-Vulkan" (
        set DEFINITIONS=-DCMAKE_BUILD_TYPE=Debug -DNC_EDITOR_ENABLED=ON -DNC_TESTS_ENABLED=ON -DVERBOSE_LOGGING_ENABLED=ON -DUSE_VULKAN=ON
        goto Run
    )
    if %CONFIGURATION%=="Debug" (
        set DEFINITIONS=-DCMAKE_BUILD_TYPE=Debug -DNC_EDITOR_ENABLED=ON -DNC_TESTS_ENABLED=ON -DVERBOSE_LOGGING_ENABLED=ON
        goto Run
    )
    if %CONFIGURATION%=="Debug-SanitizeUB" (
        set DEFINITIONS=-DCMAKE_BUILD_TYPE=Debug -DNC_EDITOR_ENABLED=ON -DNC_TESTS_ENABLED=ON -DVERBOSE_LOGGING_ENABLED=ON -DSANITIZE_UB=ON
        goto Run
    )
    if %CONFIGURATION%=="Release" (
        set DEFINITIONS=-DCMAKE_BUILD_TYPE=Release
        goto Run
    )
    if %CONFIGURATION%=="Release-WithEditor" (
        set DEFINITIONS=-DCMAKE_BUILD_TYPE=Release -DNC_EDITOR_ENABLED=ON
        goto Run
    )
    if %CONFIGURATION%=="Release-Profile" (
        set DEFINITIONS=-DCMAKE_BUILD_TYPE=Release -DPROFILING_ENABLED=ON
        goto Run
    )

    echo Error: Invalid Configuration - %CONFIGURATION%
    EXIT

:Run
    set BUILD_DIR=%~dp0\..\build\%TARGET%\%CONFIGURATION%
    if not exist %BUILD_DIR% mkdir %BUILD_DIR%
    cd %BUILD_DIR%

    cmake %DEFINITIONS% -G "Ninja" %SOURCE_DIR%