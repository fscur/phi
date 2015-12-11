#include <phi/core/globals.h>
#include <phi/io/path.h>
#include <phi/loader/importer.h>
#include <phi/rendering/geometry.h>
#include <phi/rendering/model.h>
#include <phi/rendering/mesh.h>
#include <fstream>
#include <iostream>

namespace phi
{
    model::model(std::string name) : object3D(name, objectType::MODEL)
    {
    }

    model* model::fromFile(std::string fileName)
    {
        auto modelName = path::getFileNameWithoutExtension(fileName);
        GLuint meshCount = 0;

        std::vector<geometryData*>* geometriesData = new std::vector<geometryData*>();
        if (!importer::importMesh(fileName, geometriesData))
        {
            log("Failed to load mesh");
            return nullptr;
        }

        model* md = new model(modelName);

        auto name = path::getFileNameWithoutExtension(fileName);
        auto dir = path::getDirectoryFullName(fileName);
        auto thumbnailPath = dir + "\\" + name + ".th";
        auto thumbnail = texture::fromFile(thumbnailPath);
        md->setThumbnail(thumbnail);

        auto i = 0;
        for each (geometryData* data in *geometriesData)
        {
            auto meshName = modelName + "_mesh_" + std::to_string(meshCount++);

            auto g = new geometry(data);

            auto m = new mesh(meshName, g, material::getDefault());

            md->addChild(m);
        }

        return md;
    }
}