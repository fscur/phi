#pragma once
#include <phi.h>
#include "renderingApi.h"
#include <core\geometry.h>

#include "renderInstance.h"
#include "vertexBuffer.h"
#include "buffer.h"
#include "mesh.h"

namespace phi
{
    class batch
    {
    private:
        struct drawInstanceData
        {
            GLintptr offset;
            mat4 modelMatrix;
            GLuint materialId;

            drawInstanceData(
                GLintptr offset = 0,
                mat4 modelMatrix = mat4(), 
                GLuint materialId = 0) :
                offset(offset),
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
        map<drawInstanceData*, mesh*> _instancesMesh;

        vertexBuffer* _vbo;
        vertexBuffer* _materialsIdsBuffer;
        vertexBuffer* _modelMatricesBuffer;
        vertexBuffer* _selectionBuffer;
        buffer* _ebo;
        buffer* _drawCmdBuffer;

    private:
        void createVao();
        void createVao(const renderInstance& instance);
        void createVbo(const void* const data, GLsizeiptr size);
        void createSelectionColorBuffer(const void* const data, GLsizeiptr size);
        void createMaterialsIdsBuffer(const void* const data, GLsizeiptr size);
        void createModelMatricesBuffer(const void* const data, GLsizeiptr size);
        void createEbo(const void* const data, GLsizeiptr size);
        void createDrawCmdsBuffer(const void* const data, GLsizeiptr size);

        void addNewGeometry(const renderInstance& instance);
        void addNewInstance(const renderInstance& instance);

    public:
        RENDERING_API batch();
        RENDERING_API ~batch();
        RENDERING_API bool add(const renderInstance& instance);
        RENDERING_API void remove(mesh* mesh);
        RENDERING_API void update(const renderInstance& instance);
        RENDERING_API void updateSelectionBuffer(mesh* const mesh, const vec4& selectionColor);
        RENDERING_API void updateModelMatricesBuffer(mesh* const mesh, const mat4& modelMatrix);
        RENDERING_API void update(const vector<renderInstance>& instances);
        RENDERING_API void updateBuffers();
        RENDERING_API void render();
    };
}