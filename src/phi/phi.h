#pragma once

namespace phi
{
    using uint = unsigned int;
    using ulong = unsigned long;
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
    using wstring = std::wstring;

    template<class T>
    using vector = std::vector<T>;

    template<class T, class R>
    using map = std::map<T, R>;

    template<class T, class R>
    using unordered_map = std::unordered_map<T, R>;

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
    void removeIfContains(vector<T>& vector, T& value)
    {
        auto it = std::find(vector.begin(), vector.end(), value);

        if (it != vector.end())
            vector.erase(it);
    }

    template<typename T>
    bool contains(const vector<T>& vector, const T value)
    {
        auto it = std::find(vector.begin(), vector.end(), value);
        return it != vector.end();
    }

    template<typename T>
    bool contains(const vector<T>* const vector, const T value)
    {
        auto it = std::find(vector->begin(), vector->end(), value);
        return it != vector->end();
    }

    template<typename T>
    size_t indexOf(const vector<T>& vector, const T& value)
    {
        for (size_t i = 0; i < vector.size(); ++i)
        {
            if (vector[i] == value)
                return i;
        }

        throw new std::exception();
    }

    template<typename T>
    size_t indexOf(const vector<T>* vector, const T value)
    {
        for (size_t i = 0; i < vector->size(); ++i)
        {
            if ((*vector)[i] == value)
                return i;
        }

        throw new std::exception();
    }

    template<typename T, typename R>
    bool contains(const map<T, R> map, const T value)
    {
        auto it = map.find(value);
        return it != map.end();
    }

    template<typename T, typename R>
    bool contains(const unordered_map<T, R> map, const T value)
    {
        auto it = map.find(value);
        return it != map.end();
    }

    template<typename T>
    void debug(T message)
    {
#if _DEBUG
        std::cout << message << std::endl;
#endif
    }
}

namespace std
{
    //http://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x
    template <class T>
    inline void hash_combine(std::size_t& seed, const T& v)
    {
        std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
}