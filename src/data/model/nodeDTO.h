#pragma once
#include <phi.h>

#include "componentDTO.h"

namespace phi
{
    struct nodeDTO
    {
        vector<componentDTO> components;
        vector<nodeDTO> children;

        template <typename Archive>
        void serialize(Archive& archive)
        {
            archive(
                make_nvp("Components", components),
                make_nvp("Children", children)
            );
        }
    };
}