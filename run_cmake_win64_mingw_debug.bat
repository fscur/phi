@echo OFF
:start

if not exist .\build\mingw\\. (
	mkdir .\build\mingw\\.
)

cd .\build\mingw\\.
cmake -C ../../cmake/cmake_cache_win64_mingw_debug.cmake -DCMAKE_BUILD_TYPE=Debug -G "MinGW Makefiles" ../../
cd ../../

cd .\build\mingw\\.
mingw32-make install

if not %ERRORLEVEL%==0 (
cd ../../
goto end
)

cd ../../
cd bin/win64/mingw/debug
demon.exe /rp "D:\Prog\phi\resources" /lp "D:\Prog\phi\library"

if not %ERRORLEVEL%==0 (
cd ../../../../
goto end
)

:end

set choice=
set /P choice=
if "%choice%"=="" goto start