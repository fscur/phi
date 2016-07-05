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
        _drawCount(0),
        _objectsCount(0),
        _vao(nullptr),
        _geometries(vector<geometry*>()),
        _materialsIdsBuffer(nullptr),
        _modelMatricesBuffer(nullptr),
        _drawCmdBuffer(nullptr)
    {
    }

    batch::~batch()
    {
        safeDelete(_vao);
        safeDelete(_materialsIdsBuffer);
        safeDelete(_modelMatricesBuffer);
        safeDelete(_selectionBuffer);
        safeDelete(_drawCmdBuffer);

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

        _vbo = new vertexBuffer("vbo", vboAttribs);
        _vbo->storage(vboSize, nullptr, bufferStorageUsage::dynamic | bufferStorageUsage::write);

        _ebo = new buffer("ebo", bufferTarget::element);
        _ebo->storage(vboSize, nullptr, bufferStorageUsage::dynamic | bufferStorageUsage::write);

        _drawCmdBuffer = new buffer("drawIndirect", bufferTarget::drawIndirect);
        _drawCmdBuffer->data(sizeof(drawElementsIndirectCmd), nullptr, bufferDataUsage::dynamicDraw);

        vector<vertexBufferAttribute> selectionAttribs;
        selectionAttribs.push_back(vertexBufferAttribute(4, 4, GL_FLOAT, 0, 0, 1));

        _selectionBuffer = new vertexBuffer("selectionColor", selectionAttribs);
        _selectionBuffer->data(sizeof(vec4), nullptr, bufferDataUsage::dynamicDraw);

        vector<vertexBufferAttribute> materialsIdsAttribs;
        materialsIdsAttribs.push_back(vertexBufferAttribute(5, 1, GL_UNSIGNED_INT, 0, 0, 1));

        _materialsIdsBuffer = new vertexBuffer("materialsIds", materialsIdsAttribs);
        _materialsIdsBuffer->data(sizeof(uint), nullptr, bufferDataUsage::dynamicDraw);

        vector<vertexBufferAttribute> modelMatricesAttribs;

        for (uint i = 0; i < 4; ++i)
            modelMatricesAttribs.push_back(vertexBufferAttribute(6 + i, 4, GL_FLOAT, sizeof(glm::mat4), (const void *)(sizeof(GLfloat) * i * 4), 1));

        _modelMatricesBuffer = new vertexBuffer("modelMatrices", modelMatricesAttribs);
        _modelMatricesBuffer->data(sizeof(mat4), nullptr, bufferDataUsage::dynamicDraw);
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
            _drawCmdBuffer->bind();
            glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, _objectsCount, 0);
            _drawCmdBuffer->unbind();
        });
    }

    void batch::addGeometry(geometry* geometry)
    {
        _geometries.push_back(geometry);
        _objectsCount++;

        auto vboSize = geometry->vboSize;
        auto eboSize = geometry->eboSize;

        _vbo->subData(_vboOffset, vboSize, geometry->vboData);
        _vboOffset += vboSize;

        _ebo->subData(_eboOffset, eboSize, geometry->eboData);
        _eboOffset += eboSize;

        if (geometry->vboSize > _freeSpace)
            _freeSpace = 0;
        else
            _freeSpace -= geometry->vboSize;
    }

    void batch::addInstance(const meshInstance* instance)
    {
        _instances[instance->getGeometry()].push_back(instance);
    }

    void batch::updateAllData()
    {
        auto indicesOffset = 0;
        auto verticesOffset = 0;
        auto instancesOffset = 0;
        vector<drawElementsIndirectCmd> drawCmdsBufferData;
        vector<mat4> modelMatricesData;
        vector<GLuint> materialsIdsData;
        vector<vec4> selectionBufferData;
        GLintptr offset = 0;

        for (auto geometry : _geometries)
        {
            auto instances = _instances[geometry];
            GLuint instanceCount = static_cast<GLuint>(instances.size());

            for (auto instance : instances)
            {
                auto mesh = instance->mesh;
                auto node = mesh->getNode();
                auto clickComponent = node->getComponent<phi::clickComponent>();

                modelMatricesData.push_back(instance->modelMatrix);
                materialsIdsData.push_back(instance->materialId);
                selectionBufferData.push_back(vec4(clickComponent->getSelectionColor(), node->getIsSelected()));
            }

            GLuint indicesCount = geometry->indicesCount;

            auto drawCmd = drawElementsIndirectCmd(
                indicesCount,
                instanceCount,
                indicesOffset,
                verticesOffset,
                instancesOffset);

            indicesOffset += indicesCount;
            verticesOffset += geometry->verticesCount;
            instancesOffset += instanceCount;

            drawCmdsBufferData.push_back(drawCmd);
        }

        auto drawCmdData = drawCmdsBufferData.size() > 0 ? &drawCmdsBufferData[0] : nullptr;
        auto matricesData = modelMatricesData.size() > 0 ? &modelMatricesData[0] : nullptr;
        auto materialsData = materialsIdsData.size() > 0 ? &materialsIdsData[0] : nullptr;
        auto selectionData = selectionBufferData.size() > 0 ? &selectionBufferData[0] : nullptr;

        _drawCmdBuffer->data(sizeof(drawElementsIndirectCmd) * drawCmdsBufferData.size(), drawCmdData, bufferDataUsage::dynamicDraw);
        _modelMatricesBuffer->data(sizeof(mat4) * modelMatricesData.size(), matricesData, bufferDataUsage::dynamicDraw);
        _materialsIdsBuffer->data(sizeof(uint) * materialsIdsData.size(), materialsData, bufferDataUsage::dynamicDraw);
        _selectionBuffer->data(sizeof(vec4) * selectionBufferData.size(), selectionData, bufferDataUsage::dynamicDraw);
    }

    bool batch::canAdd(const meshInstance* instance)
    {
        if (_empty)
            return true;

        auto geometry = instance->getGeometry();

        auto containsGeometry = phi::contains(_geometries, geometry);
        auto doesGeometryFitInVao = instance->getVboSize() < _freeSpace;

        return containsGeometry || doesGeometryFitInVao;
    }

    void batch::add(const meshInstance* instance)
    {
        auto geometry = instance->getGeometry();
        auto containsGeometry = phi::contains(_geometries, geometry);

        assert(_empty || containsGeometry || instance->getVboSize() < _freeSpace);

        if (_empty)
            initialize(instance);

        if (!containsGeometry)
            addGeometry(geometry);

        addInstance(instance);

        updateAllData();
    }

    void batch::remove(const meshInstance* instance)
    {
    }

    void batch::update(const meshInstance* instance)
    {
    }

    void batch::updateSelectionBuffer(mesh* mesh, bool isSelected)
    {
    }

    void batch::updateTransformBuffer(mesh* mesh, const mat4 & modelMatrix)
    {
    }
}