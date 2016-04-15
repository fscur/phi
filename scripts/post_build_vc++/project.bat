@echo OFF

if %1==Gcc (
cd ..\..\..\gcc\release\src\%2
mingw32-make
)