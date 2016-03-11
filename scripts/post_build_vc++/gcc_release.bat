@echo OFF
:start


set buildDir=..\build\gcc

if not exist %buildDir% (
	mkdir %buildDir%
)

cd %buildDir%
cmake -C ../../cmake/cmake_cache_win64_gcc_debug.cmake -DCMAKE_BUILD_TYPE=Release -G "MinGW Makefiles" ../../
cd ../

cd %buildDir%
mingw32-make
