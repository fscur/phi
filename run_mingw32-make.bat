@echo OFF
:start

cd .\build\mingw\\.
mingw32-make
cd ../../

set choice=
set /P choice=
if "%choice%"=="" goto start