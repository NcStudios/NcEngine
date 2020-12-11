mkdir "%~dp0/lib_enet_temp_build"
cd "%~dp0/lib_enet_temp_build"

if "%~1"=="Debug" (
    if not exist "%~dp0\..\..\external\lib\libenet-Debug.a" (
        echo Building ENet with Debug Configuration
        cmake -DCMAKE_BUILD_TYPE=Debug -G "Ninja" "%~dp0/../../external/src/enet"
        goto Build
    ) else goto Cleanup
)

if "%~1"=="Release" (
    if not exist "%~dp0\..\..\external\lib\libenet-Release.a" (
        echo Building ENet with Release Configuration
        cmake -DCMAKE_BUILD_TYPE=Release -G "Ninja" "%~dp0/../../external/src/enet"
        goto Build
    ) else goto Cleanup
)

:Build
ninja

:Cleanup
cd "%~dp0"
rmdir /s /q lib_enet_temp_build