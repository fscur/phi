#pragma once
#include <phi.h>

#include "sceneDTO.h"
#include "nodeDTO.h"

namespace demon
{
    struct projectDTO
    {
    public:
        projectDTO() {}

        projectDTO(sceneDTO scene, phi::vector<nodeDTO> nodes) :
            scene(scene),
            nodes(nodes)
        {
        }

        ~projectDTO() {}

        template <typename Archive>
        void serialize(Archive& archive)
        {
            archive(
                cereal::make_nvp("scene", scene),
                cereal::make_nvp("nodes", nodes)
            );
        }

        sceneDTO scene;
        phi::vector<nodeDTO> nodes;
    };
}