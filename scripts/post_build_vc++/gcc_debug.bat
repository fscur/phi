@echo OFF
:start

cd ../../../../

if not exist .\build\mingw\\. (
	mkdir .\build\mingw\\.
)

cd .\build\mingw\\.
cmake -C ../../cmake/cmake_cache_win64_mingw_debug.cmake -DCMAKE_BUILD_TYPE=Debug -G "MinGW Makefiles" ../../
cd ../../

cd .\build\mingw\\.
mingw32-make
