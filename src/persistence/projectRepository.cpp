#include <precompiled.h>
#include "projectRepository.h"

#include "model/nodeDTO.h"
#include "model/cameraDTO.h"
#include "model/sceneDTO.h"
#include "model/projectDTO.h"

namespace demon
{
    using namespace cereal;

    projectRepository::projectRepository()
    {
    }

    projectRepository::~projectRepository()
    {
    }

    project* projectRepository::load(phi::string path)
    {
        //std::ifstream inputFileStream(path);
        //JSONInputArchive archive(inputFileStream);

        //projectDTO projectDto;
        //projectDto.serialize(archive);
        return nullptr;
    }

    void projectRepository::save(project* project, phi::string path)
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
        auto projectDto = projectDTO(sceneDto, nodes);

        std::ofstream outputFileStream(path);
        JSONOutputArchive archive(outputFileStream);

        projectDto.serialize(archive);
    }
}