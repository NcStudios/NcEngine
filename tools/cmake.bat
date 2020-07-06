if not exist "%~dp0\..\external\lib\libimgui.a" call "%~dp0\build_external_libs.bat"
if not exist "%~dp0\..\build" mkdir "%~dp0\..\build" 
cd "%~dp0\..\build"
cmake -DNC_EDITOR_ENABLED=ON -DNC_TESTS_ENABLED=OFF -DDEBUG_BUILD=ON -G "MinGW Makefiles" "%~dp0\.."