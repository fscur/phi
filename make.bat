if not exist .\buildx64\\. (
	mkdir .\buildx64\\.
)
cd .\buildx64\\.
cmake -C ../cmake_cache_x64.cmake -G "Visual Studio 14 2015 Win64" ../
