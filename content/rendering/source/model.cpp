#include "model.h"

namespace phi
{
    model::model(std::string name) :
        resource(name, "")
    {
    }

    model::model(std::string name, std::string path, std::vector<mesh*> meshes) :
        resource(name, path)
    {
        _meshes = meshes;
    }

    void model::addMesh(mesh* mesh)
    {
        _meshes.push_back(mesh);
    }
}