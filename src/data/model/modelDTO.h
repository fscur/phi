#pragma once
#include <phi.h>

#include <core/guid.h>
#include <core/base64.h>

#include "nodeDTO.h"

namespace phi
{
    struct modelDTO
    {
        guid guid;
        nodeDTO node;

        template <typename Archive>
        void serialize(Archive& archive)
        {
            archive(
                make_nvp("Guid", guid),
                make_nvp("Node", node)
            );
        }
    };
}