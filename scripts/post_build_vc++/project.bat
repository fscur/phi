@echo OFF
if %1==Debug goto :eof
if %1==RelWithDebInfo goto :eof
if %1==MinSizeRel goto :eof
if %1==DebugWithMemoryTracking goto :eof

cd ..\..\..\

set build=.\gcc\%1\src\%2

cd %build%
mingw32-make