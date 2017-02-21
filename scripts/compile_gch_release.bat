cd ..\src\phi
g++ -DMINGW -DNDEBUG -std=c++11 -Wall -Werror -pedantic -Dapi_EXPORTS -O3 -I../../inc -I../../inc/freetype precompiled.h
