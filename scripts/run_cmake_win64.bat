@echo OFF

set root= ..\
set build=%root%build\win64

if not exist %build% (
	mkdir %build%
)

:start

cd %build%
cmake -C ../../cmake/cache_win64_vc++.cmake -G "Visual Studio 14 2015 Win64" ../../
cd %root%

set build=%root%build\gcc\debug

if not exist %build% (
	mkdir %build%
)

:start

cd %build%
cmake -C ../../../cmake/cache_win64_gcc_debug.cmake -DCMAKE_BUILD_TYPE=Debug -G "MinGW Makefiles" ../../../
cd %root%../

set build=%root%build\gcc\release

if not exist %build% (
	mkdir %build%
)

:start

cd %build%
cmake -C ../../../cmake/cache_win64_gcc_release.cmake -DCMAKE_BUILD_TYPE=Release -G "MinGW Makefiles" ../../../
cd %root%../

set choice=
set /P choice=
if "%choice%"=="" goto start