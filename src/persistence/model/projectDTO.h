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

        static projectDTO from(project* project)
        {
            auto scene = project->getScene();

            auto nodesIds = phi::vector<guidDTO>();
            auto nodes = phi::vector<nodeDTO>();
            auto models = scene->getModels();

            for (auto& model : models)
            {
                auto nodeDto = nodeDTO::from(model);
                nodes.push_back(nodeDto);
                nodesIds.push_back(nodeDto.guid);
            }

            auto cameraDto = cameraDTO::from(scene->getCamera());
            auto sceneDto = sceneDTO(nodesIds, cameraDto);

            return projectDTO(sceneDto, nodes);
        }

        sceneDTO scene;
        phi::vector<nodeDTO> nodes;
    };
}