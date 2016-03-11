#pragma once

namespace phi
{
    using uint = unsigned int;
    using uint64 = unsigned long long;
    using byte = unsigned char;
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
    using exception = std::exception;

    template<class T>
    using vector = std::vector<T>;

    template<class T, class R>
    using map = std::map<T, R>;

    static const float PI = 3.1415926536f;
    static const float PI_OVER_2 = 1.5707963268f;
    static const float PI_OVER_4 = 0.7853981634f;

    template<typename T>
    void safeDelete(T value)
    {
        delete value;
        value = nullptr;
    }

    template<typename T>
    void safeDeleteArray(T value)
    {
        delete[] value;
        value = nullptr;
    }
    
    template<typename T>
    bool contains(vector<T> vector, T value)
    {
        auto it = std::find(vector.begin(), vector.end(), value);

        return it != vector.end();
    }

    void logError(string value);
    void logWarning(string value);
    void logInfo(string value);

    template<typename T>
    void debug(T value)
    {
#if _DEBUG
        std::cout << value << std::endl;
#endif
    }
}