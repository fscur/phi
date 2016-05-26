#include <precompiled.h>
#include "batch.h"
#include "drawElementsIndirectCmd.h"
#include "gl.h"

namespace phi
{
    batch::batch() :
        _vao(0),
        _freeSpace(MAX_VBO_SIZE),
        _vboOffset(0),
        _eboOffset(0),
        _indicesOffset(0),
        _verticesOffset(0),
        _drawCount(0),
        _objectsCount(0),
        _geometries(vector<geometry*>()),
        _instances(map<geometry*, vector<drawInstanceData*>>()),
        _meshInstances(map<mesh*, drawInstanceData*>()),
        _vbo(nullptr),
        _materialsIdsBuffer(nullptr),
        _modelMatricesBuffer(nullptr),
        _ebo(nullptr),
        _drawCmdBuffer(nullptr)
    {
    }

    batch::~batch()
    {
        glDeleteVertexArrays(1, &_vao);
        safeDelete(_vbo);
        safeDelete(_ebo);
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

    void batch::createVao(const renderInstance& instance)
    {
        glCreateVertexArrays(1, &_vao);
        glError::check();
        glBindVertexArray(_vao);
        glError::check();

        auto geometry = instance.mesh->geometry;
        createVbo(geometry->vboData, geometry->vboSize);
        createEbo(geometry->eboData, geometry->eboSize);
        createSelectionColorBuffer(nullptr, sizeof(uint));
        createMaterialsIdsBuffer(nullptr, sizeof(uint));
        createModelMatricesBuffer(nullptr, sizeof(mat4));
        createDrawCmdsBuffer(nullptr, sizeof(drawElementsIndirectCmd));

        glBindVertexArray(0);
        glError::check();

        _objectsCount = 1;
        _freeSpace = 0;
        _geometries.push_back(geometry);
        addNewInstance(instance);
    }

    void batch::createVao()
    {
        glCreateVertexArrays(1, &_vao);
        glError::check();
        glBindVertexArray(_vao);
        glError::check();

        createVbo(nullptr, MAX_VBO_SIZE);
        createEbo(nullptr, MAX_VBO_SIZE);
        createSelectionColorBuffer(nullptr, sizeof(uint));
        createMaterialsIdsBuffer(nullptr, sizeof(uint));
        createModelMatricesBuffer(nullptr, sizeof(mat4));
        createDrawCmdsBuffer(nullptr, sizeof(drawElementsIndirectCmd));

        glBindVertexArray(0);
        glError::check();
    }

    void batch::createVbo(const vertex* const data, GLsizeiptr size)
    {
        vector<vertexAttrib> attribs;
        attribs.push_back(vertexAttrib(0, 3, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::position)));
        attribs.push_back(vertexAttrib(1, 2, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::texCoord)));
        attribs.push_back(vertexAttrib(2, 3, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::normal)));
        attribs.push_back(vertexAttrib(3, 3, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::tangent)));

        _vbo = new vertexBuffer<vertex>(attribs);
        _vbo->storage(size, data, bufferStorageUsage::dynamic | bufferStorageUsage::write);
    }

    void batch::createEbo(const uint* const data, GLsizeiptr size)
    {
        _ebo = new buffer<uint>(bufferTarget::element);
        _ebo->storage(size, data, bufferStorageUsage::dynamic | bufferStorageUsage::write);
    }

    void batch::createDrawCmdsBuffer(const drawElementsIndirectCmd* const data, GLsizeiptr size)
    {
        _drawCmdBuffer = new buffer<drawElementsIndirectCmd>(bufferTarget::drawIndirect);
        _drawCmdBuffer->data(size, data, bufferDataUsage::dynamicDraw);
    }

    void batch::createMaterialsIdsBuffer(const uint* const data, GLsizeiptr size)
    {
        vector<vertexAttrib> attribs;
        attribs.push_back(vertexAttrib(5, 1, GL_UNSIGNED_INT, 0, 0, 1));

        _materialsIdsBuffer = new vertexBuffer<uint>(attribs);
        _materialsIdsBuffer->data(size, data, bufferDataUsage::dynamicDraw);
    }

    void batch::createModelMatricesBuffer(const mat4* const data, GLsizeiptr size)
    {
        vector<vertexAttrib> attribs;

        for (uint i = 0; i < 4; ++i)
            attribs.push_back(vertexAttrib(6 + i, 4, GL_FLOAT, sizeof(glm::mat4), (const void *)(sizeof(GLfloat) * i * 4), 1));

        _modelMatricesBuffer = new vertexBuffer<mat4>(attribs);
        _modelMatricesBuffer->data(size, data, bufferDataUsage::dynamicDraw);
    }

    void batch::createSelectionColorBuffer(const vec4* const data, GLsizeiptr size)
    {
        vector<vertexAttrib> attribs;
        attribs.push_back(vertexAttrib(4, 4, GL_FLOAT, 0, 0, 1));

        _selectionBuffer = new vertexBuffer<vec4>(attribs);
        _selectionBuffer->data(size, data, bufferDataUsage::dynamicDraw);
    }

    void batch::addNewGeometry(const renderInstance& instance)
    {
        auto geometry = instance.mesh->geometry;
        auto vboSize = geometry->vboSize;
        auto eboSize = geometry->eboSize;

        _vbo->subData(_vboOffset, vboSize, geometry->vboData);
        _vboOffset += vboSize;

        _ebo->subData(_eboOffset, eboSize, geometry->eboData);
        _eboOffset += eboSize;

        _geometries.push_back(instance.mesh->geometry);

        ++_objectsCount;
        _freeSpace -= std::max(geometry->vboSize, geometry->eboSize);
    }

    void batch::addNewInstance(const renderInstance& instance)
    {
        auto geometry = instance.mesh->geometry;
        auto drawInstance = new drawInstanceData(0, instance.modelMatrix, instance.materialId);

        _instances[geometry].push_back(drawInstance);
        _meshInstances[instance.mesh] = drawInstance;
        _instancesMesh[drawInstance] = instance.mesh;

        updateAllData();
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
                auto mesh = _instancesMesh[instance];
                instance->offset = offset++;
                modelMatricesData.push_back(instance->modelMatrix);
                materialsIdsData.push_back(instance->materialId);
                selectionBufferData.push_back(vec4(mesh->getSelectionColor(),instance->isSelected));
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

    bool batch::add(const renderInstance& instance)
    {
        auto geometry = instance.mesh->geometry;
        auto vboSize = geometry->vboSize;
        auto eboSize = geometry->eboSize;

        if (_vao == 0)
        {
            if (vboSize > _freeSpace || eboSize > _freeSpace)
            {
                createVao(instance);
                return true;
            }

            createVao();
        }

        auto alreadyHasGeometry = phi::contains(_geometries, geometry);

        if (!alreadyHasGeometry && (_freeSpace < vboSize || _freeSpace < eboSize))
            return false;

        if (!alreadyHasGeometry)
        {
            addNewGeometry(instance);
        }

        addNewInstance(instance);

        return true;
    }

    void batch::remove(mesh* mesh)
    {
        auto instance = _meshInstances[mesh];
        phi::removeIfContains(_instances[mesh->geometry], instance);
        
        if (_instances[mesh->geometry].size() == 0)
            _instances.erase(mesh->geometry);

        _instancesMesh.erase(instance);
        _meshInstances.erase(mesh);

        safeDelete(instance);

        updateAllData();
    }

    void batch::update(const renderInstance& instance)
    {
        auto drawInstance = _meshInstances[instance.mesh];
        drawInstance->materialId = instance.materialId;
        drawInstance->modelMatrix = instance.modelMatrix;
        auto selectionColor = vec4(instance.mesh->getSelectionColor(), 0.0); //TODO: ??????

        _modelMatricesBuffer->subData(drawInstance->offset * sizeof(mat4), sizeof(mat4), &drawInstance->modelMatrix);
        _materialsIdsBuffer->subData(drawInstance->offset * sizeof(uint), sizeof(uint), &drawInstance->materialId);
        _selectionBuffer->subData(drawInstance->offset * sizeof(vec4), sizeof(vec4), &selectionColor);
    }

    void batch::updateSelectionBuffer(mesh* mesh, bool isSelected)
    {
        auto drawInstance = _meshInstances[mesh];
        drawInstance->isSelected = isSelected;
        auto selectionColor = vec4(mesh->getSelectionColor(), isSelected);

        _selectionBuffer->subData(drawInstance->offset * sizeof(vec4), sizeof(vec4), &selectionColor);
    }

    void batch::updateTransformBuffer(mesh* mesh, const mat4& modelMatrix)
    {
        auto drawInstance = _meshInstances[mesh];
        drawInstance->modelMatrix = modelMatrix;

        _modelMatricesBuffer->subData(drawInstance->offset * sizeof(mat4), sizeof(mat4), &modelMatrix);
    }

    void batch::render()
    {
        glBindVertexArray(_vao);
        glError::check();

        _drawCmdBuffer->bind();

        glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, _objectsCount, 0);
        glError::check();

        _drawCmdBuffer->unbind();

        glBindVertexArray(0);
        glError::check();
    }
}