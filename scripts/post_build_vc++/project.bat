REM @echo OFF
REM if %1==Debug goto :eof
REM if %1==RelWithDebInfo goto :eof
REM if %1==MinSizeRel goto :eof

REM cd ..\..\..\

REM set build=.\gcc\%1\src\%2

REM cd %build%
REM mingw32-make