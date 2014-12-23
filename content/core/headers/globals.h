#ifndef _PHI_GLOBALS_H
#define _PHI_GLOBALS_H

#include "core.h"
#include <iostream>
#include <string>
#include <vector>

//Functions

//writes a message to the console
#define LOG(message) \
    std::cout << message << std::endl;

#define DELETE(object)    \
    if (object)           \
    {                     \
        delete object;    \
        object = nullptr; \
    }

namespace phi
{
	static const float PI = 3.1415926536f;
	static const float PI_OVER_2 = 1.5707963268f;
	static const float PI_OVER_4 = 0.7853981634f;

    class globals
    {
    public:
        static CORE_API bool contains(const std::string &s, std::string value);
        static CORE_API std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems);
        static CORE_API std::vector<std::string> split(const std::string &s, char delim);
		static CORE_API void toArray(std::vector<unsigned int> values, unsigned int* &intArray);
    };
}

#endif