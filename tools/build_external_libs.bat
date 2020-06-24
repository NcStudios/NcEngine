mkdir external_libs_temp_build
cd external_libs_temp_build
cmake -G "MinGW Makefiles" ../external
mingw32-make
cd ..
rmdir /s /q external_libs_temp_build