if not exist .\build\win64\\. (
	mkdir .\build\win64\\.
)

cd .\build\win64\\.
cmake -C ../../cmake/cmake_cache_win64.cmake -G "Visual Studio 14 2015 Win64" ../../
pause   