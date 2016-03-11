cd ..\src\phi
g++ -DMINGW -DNDEBUG -Dapi_EXPORTS -std=c++11 -O3 -I../../inc -I../../inc/freetype precompiled.h
