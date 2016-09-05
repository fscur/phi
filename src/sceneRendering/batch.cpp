#include <precompiled.h>
#include "batch.h"

#include <core\node.h>
#include <core\clickComponent.h>

#include <rendering\drawElementsIndirectCommand.h>

namespace phi
{
    batch::batch() :
        _freeSpace(MAX_VAO_SIZE),
        _objectsCount(0),
        _empty(true),
        _multiDrawCommandsBuffer(new multiDrawElementsIndirectCommandBuffer()),
        _geometries(vector<geometry*>()),
        _instances(map<geometry*, vector<const meshInstance*>>()),
        _vao(nullptr),
        _vbo(nullptr),
        _ebo(nullptr),
        _materialsIdsBuffer(nullptr),
        _modelMatricesBuffer(nullptr),
        _selectionBuffer(nullptr)
    {
    }

    batch::~batch()
    {
        safeDelete(_multiDrawCommandsBuffer);

        for (auto geometry : _geometries)
        {
            auto instances = _instances[geometry];
            for (auto instance : instances)
                safeDelete(instance);
        }

        safeDelete(_vao);
    }

    void batch::initialize(const meshInstance* instance)
    {
        auto vaoSize = std::max(instance->getVaoSize(), MAX_VAO_SIZE);
        createBuffers(vaoSize);
        initializeVao();
        _empty = false;
    }

    void batch::createBuffers(size_t vboSize)
    {
        vector<vertexBufferAttribute> vboAttribs;
        vboAttribs.push_back(vertexBufferAttribute(0, 3, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::position)));
        vboAttribs.push_back(vertexBufferAttribute(1, 2, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::texCoord)));
        vboAttribs.push_back(vertexBufferAttribute(2, 3, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::normal)));
        vboAttribs.push_back(vertexBufferAttribute(3, 3, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::tangent)));

        _vbo = new mappedVertexBuffer<const geometry*, vertex>("vbo", vboAttribs);
        _ebo = new mappedIndexBuffer<const geometry*, uint>("ebo");

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
        _vao->addBuffer(_vbo);
        _vao->addBuffer(_ebo);
        _vao->addBuffer(_selectionBuffer);
        _vao->addBuffer(_materialsIdsBuffer);
        _vao->addBuffer(_modelMatricesBuffer);

        _vao->setOnRender([&]
        {
            _multiDrawCommandsBuffer->bind();
            glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, _objectsCount, 0);
            _multiDrawCommandsBuffer->unbind();
        });
    }

    void batch::addGeometry(geometry* geometry)
    {
        _vbo->addRange(geometry, geometry->vboData, geometry->verticesCount);
        _ebo->addRange(geometry, geometry->eboData, geometry->indicesCount);

        _modelMatricesBuffer->createBucket(geometry);
        _materialsIdsBuffer->createBucket(geometry);
        _selectionBuffer->createBucket(geometry);

        if (geometry->vboSize > _freeSpace)
            _freeSpace = 0;
        else
            _freeSpace -= geometry->vboSize;

        _geometries.push_back(geometry);
        _objectsCount++;
    }

    void batch::addInstance(const meshInstance* instance)
    {
        auto geometry = instance->getGeometry();
        _instances[geometry].push_back(instance);
        auto node = instance->mesh->getNode();

        int flags = 0;
        if (node->isSelected())
            flags |= 1;
        if (node->isTranslating())
            flags |= 2;

        auto clickComponent = node->getComponent<phi::clickComponent>();
        auto selection = vec4(clickComponent->getSelectionColor(), static_cast<float>(flags) / 255.0);

        _modelMatricesBuffer->addInstance(geometry, instance, instance->modelMatrix);
        _materialsIdsBuffer->addInstance(geometry, instance, instance->materialId);

        _selectionBuffer->addInstance(geometry, instance, selection);
    }

    void batch::removeGeometry(geometry* geometry)
    {
        phi::removeIfContains(_geometries, geometry);
        _instances.erase(geometry);
        _objectsCount--;

        _vbo->remove(geometry);
        _ebo->remove(geometry);

        _modelMatricesBuffer->removeBucket(geometry);
        _materialsIdsBuffer->removeBucket(geometry);
        _selectionBuffer->removeBucket(geometry);
    }

    void batch::removeInstance(const meshInstance* instance)
    {
        auto geometry = instance->getGeometry();

        auto instances = &_instances[geometry];
        auto it = std::find(instances->begin(), instances->end(), instance);
        if (it != instances->end())
            instances->erase(it);

        _modelMatricesBuffer->removeInstance(geometry, instance);
        _materialsIdsBuffer->removeInstance(geometry, instance);
        _selectionBuffer->removeInstance(geometry, instance);
    }

    void batch::updateMultiDrawCommands()
    {
        _multiDrawCommandsBuffer->clear();

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
        return instance->getVaoSize() < _freeSpace;
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
        updateMultiDrawCommands();
    }

    void batch::remove(const meshInstance* instance)
    {
        removeInstance(instance);

        auto geometry = instance->getGeometry();
        auto instances = &_instances[geometry];

        if (instances->size() == 0)
            removeGeometry(instance->getGeometry());

        updateMultiDrawCommands();
    }

    void batch::update(const meshInstance* instance)
    {
        auto node = instance->mesh->getNode();
        auto clickComponent = node->getComponent<phi::clickComponent>();
        auto selection = vec4(clickComponent->getSelectionColor(), node->isSelected());

        auto geometry = instance->getGeometry();

        _modelMatricesBuffer->update(geometry, instance, instance->modelMatrix);
        _materialsIdsBuffer->update(geometry, instance, instance->materialId);
        _selectionBuffer->update(geometry, instance, selection);
    }

    void batch::updateSelectionBuffer(const meshInstance* instance, bool isSelected)
    {
        auto node = instance->mesh->getNode();
        auto clickComponent = node->getComponent<phi::clickComponent>();

        int flags = 0;
        if (isSelected)
            flags |= 1;
        if (node->isTranslating())
            flags |= 2;

        auto selection = vec4(clickComponent->getSelectionColor(), static_cast<float>(flags)/255.0);

        _selectionBuffer->update(instance->getGeometry(), instance, selection);
    }

    void batch::updateTransformBuffer(const meshInstance* instance)
    {
        _modelMatricesBuffer->update(instance->getGeometry(), instance, instance->modelMatrix);
    }
}