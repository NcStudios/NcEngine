mkdir "%~dp0/../external_libs_temp_build"
cd "%~dp0/../external_libs_temp_build"
cmake -G "MinGW Makefiles" "%~dp0/../external"
mingw32-make
cd "%~dp0/.."
rmdir /s /q external_libs_temp_build