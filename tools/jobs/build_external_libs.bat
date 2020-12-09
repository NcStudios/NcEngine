echo Building External Libraries

if "%~1"=="Debug" goto BuildDebug
if "%~1"=="Debug-SanitizeUB" goto BuildDebug
if "%~1"=="Release" goto BuildRelease
if "%~1"=="Release-Profile" goto BuildRelease

echo Error: Unknown build type(build_external_libs)
EXIT

:BuildDebug
    call "%~dp0/build_imgui.bat" Debug
    call "%~dp0/build_enet.bat" Debug
    goto Cleanup

:BuildRelease
    call "%~dp0/build_imgui.bat" Release
    call "%~dp0/build_enet.bat" Release
    goto Cleanup

:Cleanup