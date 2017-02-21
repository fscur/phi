#pragma once
#include <phi.h>

#include <core/base64.h>
#include <core/guid.h>

namespace cereal
{
    template <typename Archive>
    phi::string save_minimal(Archive& archive, const phi::guid& value)
    {
        _unused(archive);
        return value.toStringBase64();
    };

    template <typename Archive>
    void load_minimal(Archive& archive, phi::guid& value, const phi::string& guidString)
    {
        _unused(archive);
        value = phi::guid(phi::base64::decode(guidString));
    };
}