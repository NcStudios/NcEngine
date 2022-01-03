@echo off

:: Build script for all repo targets.
::
:: For Vs Code users, look at tasks.json for build tasks.
::
:: Usage:
:: > build.bat <Target> <Configuration> <Install-Directory> <Generator>
::
:: Target: Engine | Editor | Project
:: Configuration:  Debug | Debug-WithValidation | Release | Release-WithValidation | Release-WithEditor | Release-WithProfiling
:: Installation directory: Defaults to C:/Program Files, which will require permissions.
:: Generator: Any valid CMake generator.
::
:: The Editor and Project require the Engine to be built first.

set TARGET="%~1"
set CONFIGURATION="%~2"
set INSTALL_DIRECTORY="%~3"
set GENERATOR="%~4"

:CheckTarget
    if %TARGET%=="Engine" (
        set SOURCE_DIR="%~dp0\..\nc"
        goto CheckConfiguration
    )
    if %TARGET%=="Project" (
        set SOURCE_DIR="%~dp0\..\project"
        goto CheckConfiguration
    )
    if %TARGET%=="Editor" (
        set SOURCE_DIR="%~dp0\..\editor"
        goto CheckConfiguration
    )

    echo Error: Invalid Target - %TARGET%
    exit

:CheckConfiguration
    if %CONFIGURATION%=="Debug" (
        set DEFINITIONS=-DCMAKE_BUILD_TYPE=Debug -DNC_BUILD_TYPE=%CONFIGURATION% -DNC_EDITOR_ENABLED=ON -DNC_DEBUG_RENDERING=ON -DNC_TESTS_ENABLED=ON -DVERBOSE_LOGGING_ENABLED=ON
        goto CheckInstallDirectory
    )
    if %CONFIGURATION%=="Debug-WithValidation" (
        set DEFINITIONS=-DCMAKE_BUILD_TYPE=Debug -DNC_BUILD_TYPE=%CONFIGURATION% -DNC_EDITOR_ENABLED=ON -DNC_DEBUG_RENDERING=ON -DNC_TESTS_ENABLED=ON -DVERBOSE_LOGGING_ENABLED=ON -DNC_USE_VALIDATION=ON
        goto CheckInstallDirectory
    )
    if %CONFIGURATION%=="Release" (
        set DEFINITIONS=-DCMAKE_BUILD_TYPE=Release -DNC_BUILD_TYPE=%CONFIGURATION%
        goto CheckInstallDirectory
    )
    if %CONFIGURATION%=="Release-WithEditor" (
        set DEFINITIONS=-DCMAKE_BUILD_TYPE=Release -DNC_BUILD_TYPE=%CONFIGURATION% -DNC_EDITOR_ENABLED=ON
        goto CheckInstallDirectory
    )
    if %CONFIGURATION%=="Release-WithProfiling" (
        set DEFINITIONS=-DCMAKE_BUILD_TYPE=Release -DNC_BUILD_TYPE=%CONFIGURATION% -DNC_PROFILING_ENABLED=ON
        goto CheckInstallDirectory
    )
    if %CONFIGURATION%=="Release-WithValidation" (
        set DEFINITIONS=-DCMAKE_BUILD_TYPE=Release -DNC_BUILD_TYPE=%CONFIGURATION% -DNC_EDITOR_ENABLED=ON -DNC_DEBUG_RENDERING=ON -DNC_USE_VALIDATION=ON
        goto CheckInstallDirectory
    )

    echo Error: Invalid Configuration - %CONFIGURATION%
    EXIT

:CheckInstallDirectory
    if not exist %INSTALL_DIRECTORY% (
        echo Error: Install directory not provided
        exit
    )

:CheckGenerator
    if %GENERATOR%=="" (
        echo Generator not provided
        exit
    )

:Run
    set BUILD_DIR=%~dp0\..\build\\%TARGET%\\%CONFIGURATION%
    if not exist %BUILD_DIR% mkdir %BUILD_DIR%

    echo -- Generator: %GENERATOR%
    echo -- Target:    %TARGET%
    echo -- Config:    %CONFIGURATION%
    echo -- Source:    %SOURCE_DIR%
    echo -- Build:     %BUILD_DIR%
    echo -- Install:   %INSTALL_DIRECTORY%

    echo *** Generating build files ***
    cmake -G %GENERATOR% -S %SOURCE_DIR% -B %BUILD_DIR% %DEFINITIONS% -DNC_INSTALL_DIR=%INSTALL_DIRECTORY%
    
    if not %errorlevel%==0 (
        exit
    )

    echo *** Building target ***
    ninja -C %BUILD_DIR%

    if not %errorlevel%==0 (
        exit
    )

    if %TARGET%=="Engine" (
        echo *** Installing ***
        cmake --install %BUILD_DIR%
    )