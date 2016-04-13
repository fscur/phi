#pragma once
#include <phi.h>
#include "renderingApi.h"
#include <core\geometry.h>
#include "vertexBuffer.h"
#include "buffer.h"
#include "mesh.h"

namespace phi
{
    struct batchObject
    {
        mesh* mesh;
        geometry* geometry;
        uint materialId;
        mat4 modelMatrix;
    };

    class batch
    {
    private:
        struct drawInstanceData
        {
            GLint id;
            mat4 modelMatrix;
            GLuint materialId;

            drawInstanceData(
                GLuint id = 0,
                mat4 modelMatrix = mat4(), 
                GLuint materialId = 0) :
                id(id),
                modelMatrix(modelMatrix),
                materialId(materialId)
            {
            }
        };

    private:
        const uint MAX_VBO_SIZE = 1 * 1024 * 1024;
        GLuint _vao;
        size_t _freeSpace;
        GLint _vboOffset;
        GLint _eboOffset;
        GLint _indicesOffset;
        GLint _verticesOffset;
        GLuint _drawCount;
        GLuint _objectsCount;

        vector<mat4> _modelMatrices;
        vector<geometry*> _geometries;
        map<geometry*, vector<drawInstanceData*>> _instances;
        map<mesh*, drawInstanceData*> _meshInstances;

        vertexBuffer* _vbo;
        vertexBuffer* _materialsIdsBuffer;
        vertexBuffer* _modelMatricesBuffer;
        buffer* _ebo;
        buffer* _drawCmdBuffer;

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
        RENDERING_API batch();
        RENDERING_API ~batch();
        RENDERING_API bool add(batchObject &batchObject);
        RENDERING_API void update(const batchObject &batchObject);
        RENDERING_API void update(vector<batchObject> &batchObjects);
        RENDERING_API void render();
    };
}