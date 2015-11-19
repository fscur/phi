#include "model.h"
#include "globals.h"
#include "path.h"
#include "importer.h"
#include <fstream>
#include <iostream>

namespace phi
{
    model::model(std::string name, std::string path) :
        resource(name, path)
    {
    }

    void model::addMesh(mesh* mesh)
    {
        _meshes.push_back(mesh);
    }

    model* model::fromFile(std::string fileName)
    {
        auto modelName = path::getFileNameWithoutExtension(fileName);
        GLuint meshCount = 0;

        std::vector<meshData*>* meshesData = new std::vector<meshData*>();
        if (!importer::importMesh(fileName, meshesData))
        {
            LOG("Failed to load mesh");
            return nullptr;
        }

        model* md = new model(modelName, fileName);

        auto name = path::getFileNameWithoutExtension(fileName);
        auto dir = path::getDirectoryFullName(fileName);
        auto thumbnailPath = dir + "\\" + name + ".th";
        auto thumbnail = texture::fromFile(thumbnailPath);
        md->setThumbnail(thumbnail);

        auto i = 0;
        for each (meshData* data in *meshesData)
        {
            auto meshName = modelName + "_mesh_" + std::to_string(meshCount++);

            mesh* m = mesh::create(data->getVerticesSize(), data->getPositions(), data->getTextureCoords(), data->getNormals(), data->getIndicesSize(), data->getIndices(), data->getMaterialName());
            m->setId(i++);

            md->_meshes.push_back(m);
        }

        return md;
    }
}