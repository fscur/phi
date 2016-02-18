#include "batch.h"
#include "drawElementsIndirectCmd.h"
#include "gl.h"

namespace phi
{
    batch::batch() :
        _freeSpace(MAX_VBO_SIZE),
        _drawCount(0),
        _objectsCount(0),
        _vboOffset(0),
        _eboOffset(0),
        _vao(0)
    {
        //auto gpuMaxVboIndices = 0;
        //glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &gpuMaxVboIndices);

        //_maxIndices = std::min(maxIndices, static_cast<size_t>(gpuMaxVboIndices));
    }

    batch::~batch()
    {
        glDeleteVertexArrays(1, &_vao);
        delete _vbo;
        delete _materialsIdsBuffer;
        delete _modelMatricesBuffer;
        delete _ebo;
        delete _drawCmdBuffer;
    }

    void batch::createVao(const batchObject &batchObject)
    {
        glCreateVertexArrays(1, &_vao);
        glBindVertexArray(_vao);
        auto geometry = batchObject.geometry;
        createVbo(geometry->vboData, geometry->vboSize);
        createEbo(geometry->eboData, geometry->eboSize);
        createMaterialsIdsBuffer(nullptr, sizeof(uint));
        createModelMatricesBuffer(nullptr, sizeof(mat4));
        createDrawCmdsBuffer(nullptr, sizeof(drawElementsIndirectCmd));
        glBindVertexArray(0);

        _objectsCount = 1;
        _freeSpace = 0;
        _geometries.push_back(geometry);
        addNewInstance(batchObject);
    }

    void batch::createVao()
    {
        glCreateVertexArrays(1, &_vao);
        glBindVertexArray(_vao);
        createVbo(nullptr, MAX_VBO_SIZE);
        gl::printError("create vbo");
        createEbo(nullptr, MAX_VBO_SIZE);
        gl::printError("create ebo");
        createMaterialsIdsBuffer(nullptr, sizeof(uint));
        createModelMatricesBuffer(nullptr, sizeof(mat4));
        createDrawCmdsBuffer(nullptr, sizeof(drawElementsIndirectCmd));
        glBindVertexArray(0);
    }

    void batch::createVbo(void* data, GLsizeiptr size)
    {
        std::vector<vertexAttrib> attribs;
        attribs.push_back(vertexAttrib(0, 3, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::position)));
        attribs.push_back(vertexAttrib(1, 2, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::texCoord)));
        attribs.push_back(vertexAttrib(2, 3, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::normal)));
        attribs.push_back(vertexAttrib(3, 3, GL_FLOAT, sizeof(vertex), (void*)offsetof(vertex, vertex::tangent)));

        _vbo = new vertexBuffer(attribs);
        _vbo->storage(size, data, bufferStorageUsage::dynamic | bufferStorageUsage::write);
    }

    void batch::createMaterialsIdsBuffer(void* data, GLsizeiptr size)
    {
        std::vector<vertexAttrib> attribs;
        attribs.push_back(vertexAttrib(4, 1, GL_UNSIGNED_INT, 0, 0, 1));

        _materialsIdsBuffer = new vertexBuffer(attribs);
        _materialsIdsBuffer->data(size, data, bufferDataUsage::dynamicDraw);
    }

    void batch::createModelMatricesBuffer(void* data, GLsizeiptr size)
    {
        std::vector<vertexAttrib> attribs;

        for (uint i = 0; i < 4; ++i)
            attribs.push_back(vertexAttrib(5 + i, 4, GL_FLOAT, sizeof(glm::mat4), (const void *)(sizeof(GLfloat) * i * 4), 1));

        _modelMatricesBuffer = new vertexBuffer(attribs);
        _modelMatricesBuffer->data(size, data, bufferDataUsage::dynamicDraw);
    }

    void batch::createEbo(void* data, GLsizeiptr size)
    {
        _ebo = new buffer(bufferTarget::element);
        _ebo->storage(size, data, bufferStorageUsage::dynamic | bufferStorageUsage::write);
    }

    void batch::createDrawCmdsBuffer(void* data, GLsizeiptr size)
    {
        _drawCmdBuffer = new buffer(bufferTarget::drawIndirect);
        _drawCmdBuffer->data(size, data, bufferDataUsage::dynamicDraw);
    }

    bool batch::add(batchObject &batchObject)
    {
        auto geometry = batchObject.geometry;
        auto vboSize = geometry->vboSize;
        auto eboSize = geometry->eboSize;

        if (_vao == 0)
        {
            if (vboSize > _freeSpace || eboSize > _freeSpace)
            {
                createVao(batchObject);
                return true;
            }

            createVao();
        }

        auto found = phi::contains(_geometries, geometry);

        if (!found && (_freeSpace < vboSize || _freeSpace < eboSize))
            return false;

        if (!found)
            addNewGeometry(batchObject);
        else
            addNewInstance(batchObject);

        return true;
    }

    void batch::addNewGeometry(const batchObject& batchObject)
    {
        auto geometriesCount = _geometries.size();
        auto geometry = *batchObject.geometry;
        auto vboSize = geometry.vboSize;
        auto eboSize = geometry.eboSize;

        _vbo->subData(_vboOffset, vboSize, geometry.vboData);
        _vboOffset += vboSize;

        _ebo->subData(_eboOffset, eboSize, geometry.eboData);
        _eboOffset += eboSize;

        _geometries.push_back(batchObject.geometry);

        ++_objectsCount;
        _freeSpace -= std::max(geometry.vboSize, geometry.eboSize);

        addNewInstance(batchObject);
    }

    void batch::addNewInstance(const batchObject& batchObject)
    {
        auto geometry = batchObject.geometry;
        _instances[geometry].push_back(drawInstanceData(batchObject.modelMatrix, batchObject.materialId));

        updateBuffers();
    }

    void batch::updateBuffers()
    {
        auto indicesOffset = 0;
        auto verticesOffset = 0;
        auto instancesOffset = 0;
        std::vector<drawElementsIndirectCmd> drawCmdsBufferData;
        std::vector<mat4> modelMatricesData;
        std::vector<GLuint> materialsIdsData;

        for (auto geometry : _geometries)
        {
            auto instances = _instances[geometry];
            GLuint instanceCount = static_cast<GLuint>(instances.size());

            for (auto instance : instances)
            {
                modelMatricesData.push_back(instance.modelMatrix);
                materialsIdsData.push_back(instance.materialId);
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

        _drawCmdBuffer->data(sizeof(drawElementsIndirectCmd) * drawCmdsBufferData.size(), &drawCmdsBufferData[0], bufferDataUsage::dynamicDraw);
        _modelMatricesBuffer->data(sizeof(mat4) * modelMatricesData.size(), &modelMatricesData[0], bufferDataUsage::dynamicDraw);
        _materialsIdsBuffer->data(sizeof(uint) * materialsIdsData.size(), &materialsIdsData[0], bufferDataUsage::dynamicDraw);
    }

    void batch::render()
    {
        glBindVertexArray(_vao);
        _drawCmdBuffer->bind();
        glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, _objectsCount, 0);
        _drawCmdBuffer->unbind();
        glBindVertexArray(0);
    }
}