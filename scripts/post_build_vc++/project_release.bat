@echo OFF

cd ..\..\..\

set build=.\gcc\%1\src\%2

cd %build%
mingw32-make