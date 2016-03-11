@echo OFF

set root= ..\
set build=%root%build\gcc\release

if not exist %build% (
	mkdir %build%
)

:start

cd %build%
cmake -C ../../../cmake/cache_win64_gcc_release.cmake -DCMAKE_BUILD_TYPE=Release -G "MinGW Makefiles" ../../../
cd %root%../

cd %build%
mingw32-make install

if not %ERRORLEVEL%==0 (
cd %root%
goto end
)

REM cd ..\
REM cd bin/win64/mingw/debug
REM demon.exe /rp "D:\Prog\phi\resources" /lp "D:\Prog\phi\library"

REM if not %ERRORLEVEL%==0 (
REM cd ..\..\..\..\
REM goto end
REM )

:end

set choice=
set /P choice=
if "%choice%"=="" goto start