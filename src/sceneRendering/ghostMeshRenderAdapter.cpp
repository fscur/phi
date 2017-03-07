#include <precompiled.h>

#include <core/geometry.h>
#include <core/node.h>
#include <core/notImplementedException.h>

#include <rendering/texturesManager.h>

#include "ghostMeshRenderAdapter.h"

namespace phi
{
    ghostMeshRenderAdapter::ghostMeshRenderAdapter() : 
        _freeSpace(MAX_VAO_SIZE),
        _empty(true),
        _multiDrawCommandsBuffer(new multiDrawElementsIndirectCommandBuffer()),
        _objectsCount(0u)
    {
        createBuffers();
        initializeVao();
    }

    void ghostMeshRenderAdapter::createBuffers()
    {
        vector<vertexBufferAttribute> vboAttribs;
        vboAttribs.push_back(vertexBufferAttribute(0, 3, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::position)));
        vboAttribs.push_back(vertexBufferAttribute(1, 2, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::texCoord)));
        vboAttribs.push_back(vertexBufferAttribute(2, 3, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::normal)));
        vboAttribs.push_back(vertexBufferAttribute(3, 3, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::tangent)));

        _vbo = new mappedVertexBuffer<const geometry*, vertex>("vbo", vboAttribs);
        _ebo = new mappedIndexBuffer<const geometry*, uint>("ebo");

        vector<vertexBufferAttribute> modelMatricesAttribs;
        for (uint i = 0; i < 4; ++i)
            modelMatricesAttribs.push_back(vertexBufferAttribute(4 + i, 4, GL_FLOAT, sizeof(glm::mat4), (const void *)(sizeof(GLfloat) * i * 4), 1));

        _modelMatricesBuffer = new multiDrawMappedBuffer<const ghostMesh*, mat4>("modelMatrices", modelMatricesAttribs);
    }

    void ghostMeshRenderAdapter::initializeVao()
    {
        _vao = new vertexArrayObject();
        _vao->addBuffer(_vbo);
        _vao->addBuffer(_ebo);
        _vao->addBuffer(_modelMatricesBuffer);

        _vao->setOnRender([&]
        {
            if (_objectsCount == 0)
                return;

            _multiDrawCommandsBuffer->bind();
            glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, _objectsCount, 0);
            _multiDrawCommandsBuffer->unbind();
        });
    }

    ghostMeshRenderAdapter::~ghostMeshRenderAdapter()
    {
        safeDelete(_vao);
        safeDelete(_multiDrawCommandsBuffer);
    }

    void ghostMeshRenderAdapter::add(const ghostMesh* ghostMesh)
    {
        auto geometry = ghostMesh->getGeometry();
        auto containsGeometry = phi::contains(_geometries, geometry);

        if (!containsGeometry)
            addGeometry(geometry);

        addInstance(ghostMesh);
        updateMultiDrawCommands();
    }

    void ghostMeshRenderAdapter::addGeometry(geometry* geometry)
    {
        _vbo->addRange(geometry, geometry->vboData, geometry->verticesCount);
        _ebo->addRange(geometry, geometry->eboData, geometry->indicesCount);

        _modelMatricesBuffer->createBucket(geometry);
        
        if (geometry->vboSize > _freeSpace)
            _freeSpace = 0;
        else
            _freeSpace -= geometry->vboSize;

        _geometries.push_back(geometry);
        _objectsCount++;
    }

    void ghostMeshRenderAdapter::addInstance(const ghostMesh* ghostMesh)
    {
        auto geometry = ghostMesh->getGeometry();
        _ghostMeshes[geometry].push_back(ghostMesh);

        auto modelMatrix = ghostMesh->getModelMatrix();
        _modelMatricesBuffer->addInstance(geometry, ghostMesh, modelMatrix);
    }

    void ghostMeshRenderAdapter::updateMultiDrawCommands()
    {
        _multiDrawCommandsBuffer->clear();

        for (auto& geometry : _geometries)
        {
            auto instances = _ghostMeshes[geometry];
            auto instanceCount = static_cast<GLuint>(instances.size());

            for (size_t i = 0; i < instanceCount; i++)
                _multiDrawCommandsBuffer->add(geometry);
        }

        _multiDrawCommandsBuffer->create();
    }

    void ghostMeshRenderAdapter::remove(const ghostMesh* instance)
    {
        removeInstance(instance);

        auto geometry = instance->getGeometry();
        auto instances = &_ghostMeshes[geometry];

        if (instances->size() == 0)
            removeGeometry(instance->getGeometry());

        updateMultiDrawCommands();
    }

    void ghostMeshRenderAdapter::removeInstance(const ghostMesh* instance)
    {
        auto geometry = instance->getGeometry();

        auto instances = &_ghostMeshes[geometry];
        auto it = std::find(instances->begin(), instances->end(), instance);
        if (it != instances->end())
            instances->erase(it);

        _modelMatricesBuffer->removeInstance(geometry, instance);
    }

    void ghostMeshRenderAdapter::removeGeometry(geometry* geometry)
    {
        phi::removeIfContains(_geometries, geometry);
        _ghostMeshes.erase(geometry);
        _objectsCount--;

        _vbo->remove(geometry);
        _ebo->remove(geometry);

        _modelMatricesBuffer->removeBucket(geometry);
    }

    void ghostMeshRenderAdapter::update(const ghostMesh* ghostMesh)
    {
        auto geometry = ghostMesh->getGeometry();
        auto modelMatrix = ghostMesh->getModelMatrix();

        _modelMatricesBuffer->update(geometry, ghostMesh, modelMatrix);
    }
}