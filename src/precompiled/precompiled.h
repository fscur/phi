#pragma once
#pragma message ("Compiling precompiled headers.\n")

#include <algorithm>
#include <cmath>
#include <codecvt>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <fstream>
#include <future>
#include <functional>
#include <iostream>
#include <locale>
#include <map>
#include <mutex>
#include <queue>
#include <sstream>
#include <stack>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

//TODO: create a cross platform guid class
#include <assert.h>
#include <objbase.h>
#include <stdarg.h>

#if WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <gl\glew.h>
#include <sdl\sdl_syswm.h>
#else
#include <OpenGL\gl3.h>
#endif

#include <bullet\btBulletDynamicsCommon.h>

#define GLM_FORCE_RADIANS

#include <glm\glm.hpp>
#include <glm\gtc\constants.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_precision.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtx\norm.hpp>
#include <glm\gtx\quaternion.hpp>
#include <glm\gtx\rotate_vector.hpp>
#include <glm\gtx\vector_angle.hpp>

#include <rapidjson\document.h>
#include <rapidjson\filereadstream.h>

#include <sdl\sdl_image.h>
#include <sdl\sdl_ttf.h>
#include <sdl\sdl.h>

using uint = unsigned int;
using uint64 = unsigned long long;
using byte = unsigned char;

namespace phi
{
    using ivec2 = glm::ivec2;
    using ivec3 = glm::ivec3;
    using vec2 = glm::vec2;
    using vec3 = glm::vec3;
    using vec4 = glm::vec4;
    using dvec2 = glm::dvec2;
    using dvec3 = glm::dvec3;
    using mat3 = glm::mat3;
    using mat4 = glm::mat4;
    using quat = glm::quat;
    using string = std::string;

    template<class T>
    using vector = std::vector<T>;

    template<class T, class R>
    using map = std::map<T, R>;

    static const float PI = 3.1415926536f;
    static const float PI_OVER_2 = 1.5707963268f;
    static const float PI_OVER_4 = 0.7853981634f;

    template<typename T>
    inline void safeDelete(T value)
    {
        delete value;
        value = nullptr;
    }

    template<typename T>
    inline void safeDeleteArray(T value)
    {
        delete[] value;
        value = nullptr;
    }

    template<typename T>
    inline void debug(T& value)
    {
#if _DEBUG
        std::cout << value << std::endl;
#endif
    }

    template<typename T>
    inline void log(T& value)
    {
        std::cout << value << std::endl;
    }

    inline void log(vec3 value)
    {
        std::cout << value.x << "; " << value.y << "; " << value.z << std::endl;
    }

    template<typename T>
    inline bool contains(vector<T> vector, T value)
    {
        auto it = std::find(vector.begin(), vector.end(), value);

        return it != vector.end();
    }
}