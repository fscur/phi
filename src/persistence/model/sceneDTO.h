#pragma once
#include <phi.h>

#include "guidDTO.h"
#include "cameraDTO.h"

namespace demon
{
    struct sceneDTO
    {
    public:
        sceneDTO() {}

        sceneDTO(phi::vector<guidDTO> nodes, cameraDTO camera) :
            nodes(nodes),
            camera(camera)
        {
        }

        ~sceneDTO() {}

        template <typename Archive>
        void serialize(Archive& archive)
        {
            archive(
                cereal::make_nvp("nodes", nodes),
                cereal::make_nvp("camera", camera)
            );
        }

        phi::vector<guidDTO> nodes;
        cameraDTO camera;
    };
}