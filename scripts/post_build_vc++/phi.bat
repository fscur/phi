if %1==Debug goto :eof

if %1==Debug (
cd ..\..\..\..\
cd .\src\phi\\.
g++ -DMINGW -D_DEBUG -std=c++11 -Dapi_EXPORTS -g -I../../inc -I../../inc/freetype precompiled.h
cd ..\..\
goto :eof
)

if %1==Release (
cd ..\..\..\..\
cd .\src\phi
g++ -DMINGW -DNDEBUG -Dapi_EXPORTS -std=c++11 -O3 -I../../inc -I../../inc/freetype precompiled.h
cd ..\..
goto :eof
)
