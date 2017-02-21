if %1==Gcc (
cd ..\..\..\..\
cd .\src\phi
g++ -DMINGW -DNDEBUG -std=c++11 -Wall -Werror -pedantic -Dapi_EXPORTS -O3 -I../../inc -I../../inc/freetype precompiled.h
cd ..\..\


if not exist build\gcc\release (
 	mkdir build\gcc\release
)

cd build\gcc\release
cmake -C ../../../cmake/cache_win64_gcc_release.cmake -DCMAKE_BUILD_TYPE=Release -G "MinGW Makefiles" ../../../
)
