#include <precompiled.h>
#include "projectRepository.h"

#include "model/nodeDTO.h"
#include "model/cameraDTO.h"
#include "model/sceneDTO.h"
#include "model/projectDTO.h"

namespace demon
{
    using namespace cereal;

    projectRepository::projectRepository(library* library) :
        _library(library)
    {
    }

    projectRepository::~projectRepository()
    {
    }

    project* projectRepository::load(phi::string path)
    {
        std::ifstream inputFileStream(path);
        JSONInputArchive archive(inputFileStream);

        projectDTO projectDto;
        projectDto.serialize(archive);

        auto sceneDto = projectDto.scene;
        auto camera = cameraDTO::to(sceneDto.camera);
        auto scene = new phi::scene(camera);

        for (auto& modelDto : sceneDto.nodes)
        {
            for (auto& node : projectDto.nodes)
            {
                if (node.guid.guid != modelDto.guid)
                    continue;

                auto model = _library->getModelById(modelDto.guid);
                model->getTransform()->setLocalPosition(node.translation.toVec3());
                model->getTransform()->setLocalSize(node.scale.toVec3());
                model->getTransform()->setLocalOrientation(node.rotation.toQuaternion());

                scene->add(model);

            }
        }

        return new project(scene);;
    }

    void projectRepository::save(project* project, phi::string path)
    {
        auto projectDto = projectDTO::from(project);

        std::ofstream outputFileStream(path);
        JSONOutputArchive archive(outputFileStream);

        projectDto.serialize(archive);
    }
}