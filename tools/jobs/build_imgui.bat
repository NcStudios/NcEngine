mkdir "%~dp0/lib_imgui_temp_build"
cd "%~dp0/lib_imgui_temp_build"

if "%~1"=="Debug" (
    if not exist "%~dp0\..\..\external\lib\libimgui-Debug.a" (
        echo Building DearImGui with Debug Configuration
        cmake -DCMAKE_BUILD_TYPE=Debug -G "Ninja" "%~dp0/../../external/src/imgui"
        goto Build
    ) else goto Cleanup
)

if "%~1"=="Release" (
    if not exist "%~dp0\..\..\external\lib\libimgui-Release.a" (
        echo Building DearImGui with Release Configuration
        cmake -DCMAKE_BUILD_TYPE=Release -G "Ninja" "%~dp0/../../external/src/imgui"
        goto Build
    ) else goto Cleanup
)

:Build
ninja

:Cleanup
cd "%~dp0"
rmdir /s /q lib_imgui_temp_build