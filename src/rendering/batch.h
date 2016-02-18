#pragma once

#include "vertexBuffer.h"
#include "mesh.h"

#include <vector>
#include <map>

namespace phi
{
    struct batchObject
    {
        geometry* geometry;
        uint materialId;
        mat4 modelMatrix;
    };

    class batch
    {
    private:
        struct drawInstanceData
        {
            mat4 modelMatrix;
            GLuint materialId;

            drawInstanceData(
                mat4 modelMatrix, 
                GLuint materialId) :
                modelMatrix(modelMatrix),
                materialId(materialId)
            {

            }
        };
    private:
        const uint MAX_VBO_SIZE = 1 * 1024 * 1024;
        GLuint _vao;
        size_t _freeSpace;
        vertexBuffer* _vbo;
        vertexBuffer* _materialsIdsBuffer;
        vertexBuffer* _modelMatricesBuffer;
        buffer* _ebo;
        buffer* _drawCmdBuffer;

        std::vector<mat4> _modelMatrices;
        std::vector<geometry*> _geometries;
        std::map<geometry*, std::vector<drawInstanceData>> _instances;

        GLint _vboOffset;
        GLint _eboOffset;
        GLint _indicesOffset;
        GLint _verticesOffset;

        GLuint _drawCount;
        GLuint _objectsCount;

    private:
        void createVao();
        void createVao(const batchObject &batchObject);
        void createVbo(void* data, GLsizeiptr size);
        void createMaterialsIdsBuffer(void* data, GLsizeiptr size);
        void createModelMatricesBuffer(void* data, GLsizeiptr size);
        void createEbo(void* data, GLsizeiptr size);
        void createDrawCmdsBuffer(void* data, GLsizeiptr size);

        void addNewGeometry(const batchObject& batchObject);
        void addNewInstance(const batchObject& batchObject);
        void updateBuffers();

    public:
        batch();
        ~batch();
        bool add(batchObject &batchObject);
        void render();
    };
}