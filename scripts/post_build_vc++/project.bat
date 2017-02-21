@echo OFF

if %1==Gcc (
    cd ..\..\..\..\build\gcc\release
    mingw32-make
)