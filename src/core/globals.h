#ifndef _PHI_GLOBALS_H
#define _PHI_GLOBALS_H

#include "core.h"
#include <iostream>
#include <string>
#include <vector>
#include <glm\glm.hpp>
#include <GLM\gtc\quaternion.hpp>

using uint = unsigned int;
using uint64 = unsigned long long;
using byte = unsigned char;

namespace phi
{
    using vec2 = glm::vec2;
    using vec3 = glm::vec3;
    using vec4 = glm::vec4;
    using mat3 = glm::mat3;
    using mat4 = glm::mat4;
    using quat = glm::quat;

    static const float PI = 3.1415926536f;
    static const float PI_OVER_2 = 1.5707963268f;
    static const float PI_OVER_4 = 0.7853981634f;

    template<class T> 
    inline void safeDelete(T value)
    {
        delete value;
        value = nullptr;
    }

    template<class T> 
    inline void safeDeleteArray(T value)
    {
        delete[] value;
        value = nullptr;
    }

    template<class T> 
    inline void log(T& value)
    {
#if _DEBUG
        std::cout << value << std::endl;
#endif
    }

    inline void log(vec3 value)
    {
        std::cout << value.x << "; " << value.y << "; " << value.z << std::endl;
    }
}

#endif