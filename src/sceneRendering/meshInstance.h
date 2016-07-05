#pragma once
#include <phi.h>
#include <core\mesh.h>

namespace phi
{
    struct meshInstance
    {
        meshInstance(mesh* mesh, uint materialId, mat4 modelMatrix) :
            mesh(mesh),
            materialId(materialId),
            modelMatrix(modelMatrix)
        {
        }

        mesh* mesh;
        uint materialId;
        mat4 modelMatrix;

        geometry* getGeometry() const { return mesh->getGeometry(); }
        size_t getVboSize() const { return mesh->getGeometry()->vboSize; }
    };
}