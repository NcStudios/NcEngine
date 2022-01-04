@echo off

set PROJECT_NAME="%~1"
set CONFIGURATION="%~2"
set INSTALL_DIRECTORY="%~3"

if %PROJECT_NAME%=="" (
    echo Project name not provided
    EXIT
)

:CheckInstallDirectory
    if %INSTALL_DIRECTORY%=="" (
        echo Install directory not provided
        EXIT
    )

    if not exist %INSTALL_DIRECTORY% (
        echo Error: Install directory doesn't exist: %INSTALL_DIRECTORY%
        EXIT
    )

:CheckConfiguration
    if %CONFIGURATION%=="Debug" (
        set DEFINITIONS=-DCMAKE_BUILD_TYPE=Debug  -DNC_EDITOR_ENABLED=ON -DNC_DEBUG_RENDERING_ENABLED=ON -DNC_TESTS_ENABLED=ON -DNC_VERBOSE_LOGGING_ENABLED=ON
        goto Run
    )
    if %CONFIGURATION%=="Debug-WithValidation" (
        set DEFINITIONS=-DCMAKE_BUILD_TYPE=Debug  -DNC_EDITOR_ENABLED=ON -DNC_DEBUG_RENDERING_ENABLED=ON -DNC_TESTS_ENABLED=ON -DNC_VERBOSE_LOGGING_ENABLED=ON -DNC_VULKAN_VALIDATION_ENABLED=ON
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
    if %CONFIGURATION%=="Release-WithProfiling" (
        set DEFINITIONS=-DCMAKE_BUILD_TYPE=Release -DNC_PROFILING_ENABLED=ON
        goto Run
    )
    if %CONFIGURATION%=="Release-WithValidation" (
        set DEFINITIONS=-DCMAKE_BUILD_TYPE=Release -DNC_EDITOR_ENABLED=ON -DNC_DEBUG_RENDERING_ENABLED=ON -DNC_VULKAN_VALIDATION_ENABLED=ON
        goto Run
    )

    echo Error: Invalid Configuration - %CONFIGURATION%
    EXIT

:Run
    set SOURCE_DIR="%~dp0\"
    set BUILD_DIR=%~dp0\build\%TARGET%\%CONFIGURATION%
    if not exist %BUILD_DIR% mkdir %BUILD_DIR%
    cd %BUILD_DIR%

    echo Configuration: %CONFIGURATION%
    echo Build Directory: %BUILD_DIR%
    echo Install Directory: %INSTALL_DIRECTORY%

    cmake -DNC_EDITOR_PROJECT_NAME=%PROJECT_NAME% %DEFINITIONS% -DNC_INSTALL_DIR=%INSTALL_DIRECTORY% -G "Ninja" %SOURCE_DIR%
    ninja -C %BUILD_DIR%