@echo OFF
:start

if not exist .\build\win64\\. (
	mkdir .\build\win64\\.
)

cd .\build\win64\\.
cmake -C ../../cmake/cmake_cache_win64_vc++.cmake -G "Visual Studio 14 2015 Win64" ../../
cd ../../

set choice=
set /P choice=
if "%choice%"=="" goto start