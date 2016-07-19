@echo OFF
:start

REM Uncomment this to execute gettext and transalate stuff
REM cd scripts
REM call create_po_files.bat
REM cd ..

set root=.\
set return=..\..\
set return_gcc=%return%..\
set build=%root%build\win64

if not exist %build% (
	mkdir %build%
)

cd %build%
cmake -C %return%cmake/cache_win64_vc++.cmake -G "Visual Studio 14 2015 Win64" %return%
cd %return%

set build=%root%build\gcc\debug

if not exist %build% (
	mkdir %build%
)

:start

cd %build%
cmake -C %return_gcc%cmake/cache_win64_gcc_debug.cmake -DCMAKE_BUILD_TYPE=Debug -G "MinGW Makefiles" %return_gcc%
cd %return_gcc%

set build=%root%build\gcc\release

if not exist %build% (
	mkdir %build%
)

:start

cd %build%
cmake -C %return_gcc%cmake/cache_win64_gcc_release.cmake -DCMAKE_BUILD_TYPE=Release -G "MinGW Makefiles" %return_gcc%
cd %return_gcc%

REM set choice=
REM set /P choice=
REM if "%choice%"=="" goto start
