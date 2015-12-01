#include <phi/scenes/sceneMesh.h>

namespace phi
{
    sceneMesh::sceneMesh(mesh* mesh)
    {
        _mesh = mesh;
        _isSelected = false;

        auto indices = _mesh->getIndices();
        auto positions = _mesh->getPositionsBuffer();
        auto vertexArray = new btTriangleIndexVertexArray(
            (int)indices->size() / 3,
            reinterpret_cast<int*>(indices->data()),
            (int)sizeof(GLuint) * 3,
            (int)mesh->getVertices().size(),
            positions,
            (int)sizeof(GLfloat) * 3);

        _bulletShape = new btGImpactMeshShape(vertexArray);
        _bulletShape->setMargin(btScalar(0.0f));
        _bulletShape->updateBound();
    }

    void sceneMesh::setMaterial(material* value)
    { 
        _material = value; 

        if (_material != nullptr)
            _materialName = value->getName(); 
    }
}