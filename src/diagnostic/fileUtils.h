#pragma once
#include <phi.h>

class fileUtils
{
private:
    fileUtils(){ };
    ~fileUtils(){ };
public:
    static void getDirectoryFromPath(const char* fullPath, char* directory);
};