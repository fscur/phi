#include <precompiled.h>
#include "batch.h"

#include <core\node.h>
#include <core\clickComponent.h>

#include <rendering\drawElementsIndirectCmd.h>

namespace phi
{
    batch::batch() :
        _freeSpace(MAX_VBO_SIZE),
        _empty(true),
        _vboOffset(0),
        _eboOffset(0),
        _indicesOffset(0),
        _verticesOffset(0),
        _instancesOffset(0),
        _drawCount(0),
        _objectsCount(0),
        _vao(nullptr),
        _geometries(vector<geometry*>()),
        _materialsIdsBuffer(nullptr),
        _modelMatricesBuffer(nullptr),
        _multiDrawCommandsBuffer(new multiDrawElementsIndirectCommandBuffer())
    {
    }

    batch::~batch()
    {
        safeDelete(_vao);
        safeDelete(_materialsIdsBuffer);
        safeDelete(_modelMatricesBuffer);
        safeDelete(_selectionBuffer);
        safeDelete(_multiDrawCommandsBuffer);

        for (auto geometry : _geometries)
        {
            auto instances = _instances[geometry];
            for (auto instance : instances)
                safeDelete(instance);
        }
    }

    void batch::initialize(const meshInstance* instance)
    {
        auto vboSize = std::max(instance->getVboSize(), MAX_VBO_SIZE);
        createVao(vboSize);
        _empty = false;
    }

    void batch::createVao(size_t vboSize)
    {
        createBuffers(vboSize);
        initializeVao();
    }

    void batch::createBuffers(size_t vboSize)
    {
        vector<vertexBufferAttribute> vboAttribs;
        vboAttribs.push_back(vertexBufferAttribute(0, 3, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::position)));
        vboAttribs.push_back(vertexBufferAttribute(1, 2, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::texCoord)));
        vboAttribs.push_back(vertexBufferAttribute(2, 3, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::normal)));
        vboAttribs.push_back(vertexBufferAttribute(3, 3, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::tangent)));

        //_vbo = new vertexBuffer("vbo", vboAttribs);
        //_vbo->storage(vboSize, nullptr, bufferStorageUsage::dynamic | bufferStorageUsage::write);

        _vbo = new mappedVertexBuffer<const geometry*, vertex>("vbo", vboAttribs, vboSize / sizeof(vertex));
        _ebo = new mappedBuffer<const geometry*, uint>("ebo", bufferTarget::element, vboSize / sizeof(uint));

        //_ebo = new buffer("ebo", bufferTarget::element);
        //_ebo->storage(vboSize, nullptr, bufferStorageUsage::dynamic | bufferStorageUsage::write);

        auto selectionAttribs = { vertexBufferAttribute(4, 4, GL_FLOAT, 0, 0, 1) };
        _selectionBuffer = new multiDrawMappedBuffer<const meshInstance*, vec4>("selectionColor", selectionAttribs);

        auto materialsIdsAttribs = { vertexBufferAttribute(5, 1, GL_UNSIGNED_INT, 0, 0, 1) };
        _materialsIdsBuffer = new multiDrawMappedBuffer<const meshInstance*, uint>("materialsIds", materialsIdsAttribs);

        vector<vertexBufferAttribute> modelMatricesAttribs;
        for (uint i = 0; i < 4; ++i)
            modelMatricesAttribs.push_back(vertexBufferAttribute(6 + i, 4, GL_FLOAT, sizeof(glm::mat4), (const void *)(sizeof(GLfloat) * i * 4), 1));

        _modelMatricesBuffer = new multiDrawMappedBuffer<const meshInstance*, mat4>("modelMatrices", modelMatricesAttribs);
    }

    void batch::initializeVao()
    {
        _vao = new vertexArrayObject();
        _vao->add(_vbo);
        _vao->setEbo(_ebo);
        _vao->add(_selectionBuffer);
        _vao->add(_materialsIdsBuffer);
        _vao->add(_modelMatricesBuffer);

        _vao->setOnRender([=]
        {
            _multiDrawCommandsBuffer->bind();
            glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, _objectsCount, 0);
            _multiDrawCommandsBuffer->unbind();
        });
    }

    void batch::addGeometry(geometry* geometry)
    {
        _geometries.push_back(geometry);
        _objectsCount++;

        auto vboSize = geometry->vboSize;
        auto eboSize = geometry->eboSize;

        _vbo->addRange(geometry, geometry->vboData, geometry->verticesCount);
        _ebo->addRange(geometry, geometry->eboData, geometry->indicesCount);

        //_vbo->subData(_vboOffset, vboSize, geometry->vboData);
        //_vboOffset += vboSize;

        //_ebo->subData(_eboOffset, eboSize, geometry->eboData);
        //_eboOffset += eboSize;

        //_verticesOffset += geometry->verticesCount;
        //_indicesOffset += geometry->indicesCount;

        if (geometry->vboSize > _freeSpace)
            _freeSpace = 0;
        else
            _freeSpace -= geometry->vboSize;
    }

    void batch::addInstance(const meshInstance* instance)
    {
        auto geometry = instance->getGeometry();
        _instances[geometry].push_back(instance);

        auto node = instance->mesh->getNode();
        auto clickComponent = node->getComponent<phi::clickComponent>();
        auto selection = vec4(clickComponent->getSelectionColor(), node->getIsSelected());

        _modelMatricesBuffer->add(geometry, instance, instance->modelMatrix);
        _materialsIdsBuffer->add(geometry, instance, instance->materialId);
        _selectionBuffer->add(geometry, instance, selection);

        updateMultiDrawCommands();
        _instancesOffset++;
    }

    void batch::updateMultiDrawCommands()
    {
        _multiDrawCommandsBuffer = new multiDrawElementsIndirectCommandBuffer();

        for (auto& geometry : _geometries)
        {
            auto instances = _instances[geometry];
            auto instanceCount = static_cast<GLuint>(instances.size());

            for (size_t i = 0; i < instanceCount; i++)
                _multiDrawCommandsBuffer->add(geometry);
        }

        _multiDrawCommandsBuffer->create();
    }

    bool batch::doesGeometryFitInVao(const meshInstance* instance)
    {
        return instance->getVboSize() < _freeSpace;
    }

    bool batch::canAdd(const meshInstance* instance)
    {
        auto geometry = instance->getGeometry();
        auto containsGeometry = phi::contains(_geometries, geometry);

        return _empty || containsGeometry || doesGeometryFitInVao(instance);
    }

    void batch::add(const meshInstance* instance)
    {
        assert(canAdd(instance));

        auto geometry = instance->getGeometry();
        auto containsGeometry = phi::contains(_geometries, geometry);

        if (_empty)
            initialize(instance);

        if (!containsGeometry)
            addGeometry(geometry);

        addInstance(instance);
    }

    void batch::remove(const meshInstance* instance)
    {
        auto geometry = instance->getGeometry();

        auto instances = &_instances[geometry];
        auto it = std::find(instances->begin(), instances->end(), instance);
        if (it != instances->end())
            instances->erase(it);

        if (instances->size() == 0)
        {
            phi::removeIfContains(_geometries, geometry);
            _instances.erase(geometry);
            _objectsCount--;

            _vbo->remove(geometry);
            _ebo->remove(geometry);
        }

        _modelMatricesBuffer->remove(geometry, instance);
        _materialsIdsBuffer->remove(geometry, instance);
        _selectionBuffer->remove(geometry, instance);

        updateMultiDrawCommands();
    }

    void batch::update(const meshInstance* instance)
    {
        auto node = instance->mesh->getNode();
        auto clickComponent = node->getComponent<phi::clickComponent>();
        auto selection = vec4(clickComponent->getSelectionColor(), node->getIsSelected());

        _modelMatricesBuffer->update(instance, instance->modelMatrix);
        _materialsIdsBuffer->update(instance, instance->materialId);
        _selectionBuffer->update(instance, selection);
    }

    void batch::updateSelectionBuffer(const meshInstance* instance, bool isSelected)
    {
        auto node = instance->mesh->getNode();
        auto clickComponent = node->getComponent<phi::clickComponent>();
        auto selection = vec4(clickComponent->getSelectionColor(), isSelected);

        _selectionBuffer->update(instance, selection);
    }

    void batch::updateTransformBuffer(const meshInstance* instance)
    {
        _modelMatricesBuffer->update(instance, instance->modelMatrix);
    }
}