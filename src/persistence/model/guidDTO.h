#pragma once
#include <phi.h>

#include <core/base64.h>
#include <core/guid.h>

namespace demon
{
    struct guidDTO
    {
    public:
        guidDTO(phi::guid guid) :
            guid(guid)
        {
        }

        template <typename Archive>
        phi::string save_minimal(const Archive& archive) const
        {
            _unused(archive);
            return guid.toStringBase64();
        }

        template <typename Archive>
        void load_minimal(const Archive& archive, const phi::string& value)
        {
            auto bytes = base64::decode(value);
            guid = guid(bytes);
        }

        phi::guid guid;
    };
}