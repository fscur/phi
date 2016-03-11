#pragma once
#include <phi.h>

namespace phi
{
    class inputKey
    {
    public:
        int key;
        unsigned int modifiers;

    public:
        inputKey(int key, unsigned int modifiers) :
            key(key), modifiers(modifiers)
        {
        }

        bool operator==(const inputKey &other) const
        {
            return
                key == other.key &&
                modifiers == other.modifiers;
        }
    };

    struct inputKeyHasher
    {
        size_t operator()(const phi::inputKey& k) const
        {
            // Compute individual hash values for first,
            // second and third and combine them using XOR
            // and bit shifting:

            return ((std::hash<int>()(k.key) ^
                (std::hash<unsigned int>()(k.modifiers) << 1)) >> 1);
        }
    };
}
