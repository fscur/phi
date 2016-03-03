if not exist .\build\mingw\\. (
	mkdir .\build\mingw\\.
)

cd .\build\mingw\\.
cmake -C ../../cmake/cmake_cache_mingw.cmake -G "MinGW Makefiles" ../../
pause   