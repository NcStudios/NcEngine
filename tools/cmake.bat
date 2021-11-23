@echo off

set TARGET="%~1"
set CONFIGURATION="%~2"
set INSTALL_DIRECTORY="%~3"

:CheckInstallDirectory
    if %INSTALL_DIRECTORY%=="" (
        echo Install directory not provided - using default "C:/Program Files/"
        set INSTALL_DIRECTORY="C:/Program Files/"
    )

    if not exist %INSTALL_DIRECTORY% (
        echo Error: Install directory doesn't exist: %INSTALL_DIRECTORY%
        EXIT
    )

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
    if %TARGET%=="Editor" (
        echo Building Editor with %CONFIGURATION% Configuration
        set SOURCE_DIR="%~dp0\..\editor"
        goto CheckConfiguration
    )

    echo Error: Invalid Target - %TARGET%
    EXIT

:CheckConfiguration
    if %CONFIGURATION%=="Debug" (
        set DEFINITIONS=-DCMAKE_BUILD_TYPE=Debug -DNC_BUILD_TYPE=%CONFIGURATION% -DNC_EDITOR_ENABLED=ON -DNC_DEBUG_RENDERING=ON -DNC_TESTS_ENABLED=ON -DVERBOSE_LOGGING_ENABLED=ON
        goto Run
    )
    if %CONFIGURATION%=="Debug-WithValidation" (
        set DEFINITIONS=-DCMAKE_BUILD_TYPE=Debug -DNC_BUILD_TYPE=%CONFIGURATION% -DNC_EDITOR_ENABLED=ON -DNC_DEBUG_RENDERING=ON -DNC_TESTS_ENABLED=ON -DVERBOSE_LOGGING_ENABLED=ON -DNC_USE_VALIDATION=ON
        goto Run
    )
    if %CONFIGURATION%=="Release" (
        set DEFINITIONS=-DCMAKE_BUILD_TYPE=Release -DNC_BUILD_TYPE=%CONFIGURATION%
        goto Run
    )
    if %CONFIGURATION%=="Release-WithEditor" (
        set DEFINITIONS=-DCMAKE_BUILD_TYPE=Release -DNC_BUILD_TYPE=%CONFIGURATION% -DNC_EDITOR_ENABLED=ON
        goto Run
    )
    if %CONFIGURATION%=="Release-WithProfiling" (
        set DEFINITIONS=-DCMAKE_BUILD_TYPE=Release -DNC_BUILD_TYPE=%CONFIGURATION% -DNC_PROFILING_ENABLED=ON
        goto Run
    )
    if %CONFIGURATION%=="Release-WithValidation" (
        set DEFINITIONS=-DCMAKE_BUILD_TYPE=Release -DNC_BUILD_TYPE=%CONFIGURATION% -DNC_EDITOR_ENABLED=ON -DNC_DEBUG_RENDERING=ON -DNC_USE_VALIDATION=ON
        goto Run
    )

    echo Error: Invalid Configuration - %CONFIGURATION%
    EXIT

:Run
    set BUILD_DIR=%~dp0\..\build\%TARGET%\%CONFIGURATION%
    if not exist %BUILD_DIR% mkdir %BUILD_DIR%
    cd %BUILD_DIR%

    echo Target: %TARGET%
    echo Configuration: %CONFIGURATION%
    echo Build Directory: %BUILD_DIR%
    echo Install Directory: %INSTALL_DIRECTORY%

    cmake %DEFINITIONS% -DNC_ENGINE_INSTALL_DIR=%INSTALL_DIRECTORY% -G "Ninja" %SOURCE_DIR%