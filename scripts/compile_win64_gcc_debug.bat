@echo OFF

set root= ..\
set build=%root%build\gcc\debug

if not exist %build% (
	mkdir %build%
)

cd %build%
cmake -C ../../../cmake/cache_win64_gcc_debug.cmake -DCMAKE_BUILD_TYPE=Debug -G "MinGW Makefiles" ../../../
cd %root%../

cd %build%
mingw32-make install

if not %ERRORLEVEL%==0 (
cd %root%
)

pause