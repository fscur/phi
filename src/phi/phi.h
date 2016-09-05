#pragma once

namespace phi
{
    #define _unused(x) ((void)(x))

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

    template<typename T, typename Allocator = std::allocator<T>>
    using vector = std::vector<T, Allocator>;

    template<typename R, typename... Args>
    using function = std::function<R(Args...)>;

    template<typename... Args>
    using action = std::function<void(Args...)>;

    template<
        typename Key,
        typename Value,
        typename Comparer = std::less<Key>,
        typename Allocator = std::allocator<std::pair<const Key, Value>>>
    using map = std::map<Key, Value, Comparer, Allocator>;

    template<
        typename Key,
        typename Value,
        typename Hasher = std::hash<Key>,
        typename KeyEqual = std::equal_to<Key>,
        typename Allocator = std::allocator<std::pair<Key, Value>>>
    using unordered_map = std::unordered_map<Key, Value, Hasher, KeyEqual, Allocator>;

    static const float PI = 3.1415926536f;
    static const float PI_OVER_2 = 1.5707963268f;
    static const float PI_OVER_4 = 0.7853981634f;

    static const float DECIMAL_TRUNCATION = 1e-4f;
    static const float DECIMAL_TRUNCATION_INV = 1.0f / DECIMAL_TRUNCATION;

    template<typename T>
    void safeDelete(T& value)
    {
        delete value;
        value = nullptr;
    }

    template<typename T>
    void safeDeleteArray(T& value)
    {
        delete[] value;
        value = nullptr;
    }

    template<typename T, typename... Args>
    T* make(Args&&... args)
    {
        auto address = malloc(sizeof(T));
        return new(address)T(args...);
    }

    template<typename T>
    void destroy(T* obj)
    {
        obj->T::~T();
        free(obj);
    }

    template<typename T, typename Allocator>
    bool removeIfContains(vector<T, Allocator>& vector, T& value)
    {
        auto it = std::find(vector.begin(), vector.end(), value);

        if (it != vector.end())
        {
            vector.erase(it);
            return true;
        }

        return false;
    }

    template<typename T, typename Allocator>
    bool contains(const vector<T, Allocator>& vector, const T value)
    {
        auto it = std::find(vector.begin(), vector.end(), value);
        return it != vector.end();
    }

    template<typename T, typename Allocator>
    bool contains(const vector<T, Allocator>* const vector, const T value)
    {
        auto it = std::find(vector->begin(), vector->end(), value);
        return it != vector->end();
    }

    template<typename T, typename Allocator>
    size_t indexOf(const vector<T, Allocator>& vector, const T& value)
    {
        for (size_t i = 0; i < vector.size(); ++i)
        {
            if (vector[i] == value)
                return i;
        }

        throw std::exception();
    }

    template<typename T, typename Allocator>
    size_t indexOf(const vector<T, Allocator>* vector, const T value)
    {
        for (size_t i = 0; i < vector->size(); ++i)
        {
            if ((*vector)[i] == value)
                return i;
        }

        throw std::exception();
    }

    template<typename T, typename Allocator>
    T getLastElementOf(const vector<T, Allocator>& vector)
    {
        if (vector.size() < 1)
            throw std::exception();

        auto lastIndex = vector.size() - 1;
        return vector[lastIndex];
    }

    template<
        typename Key,
        typename Value,
        typename Comparer,
        typename Allocator>
    bool contains(const map<Key, Value, Comparer, Allocator> map, const Key key)
    {
        auto it = map.find(key);
        return it != map.end();
    }

    template<
        typename Key,
        typename Value,
        typename Hasher,
        typename KeyEqual,
        typename Allocator>
    bool contains(const unordered_map<Key, Value, Hasher, KeyEqual, Allocator> map, const Key key)
    {
        auto it = map.find(key);
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