#pragma once
#include <phi.h>
#include "renderingApi.h"

#include <core\mesh.h>
#include <core\geometry.h>

#include "renderInstance.h"
#include "vertexBuffer.h"
#include "buffer.h"

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
            bool isSelected;

            drawInstanceData(
                GLintptr offset = 0,
                mat4 modelMatrix = mat4(), 
                GLuint materialId = 0) :
                offset(offset),
                modelMatrix(modelMatrix),
                materialId(materialId),
                isSelected(false)
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

        vector<geometry*> _geometries;
        map<geometry*, vector<drawInstanceData*>> _instances;
        map<mesh*, drawInstanceData*> _meshInstances;
        map<drawInstanceData*, mesh*> _instancesMesh;

        vertexBuffer* _vbo;
        vertexBuffer* _materialsIdsBuffer;
        vertexBuffer* _modelMatricesBuffer;
        vertexBuffer* _selectionBuffer;

        buffer<uint>* _ebo;
        buffer<drawElementsIndirectCmd>* _drawCmdBuffer;

    private:
        void createVao();
        void createVao(const renderInstance& instance);
        void createVbo(const vertex* const data, GLsizeiptr size);
        void createEbo(const uint* const data, GLsizeiptr size);

        void createDrawCmdsBuffer(const drawElementsIndirectCmd* const data, GLsizeiptr size);
        void createMaterialsIdsBuffer(const void* const data, GLsizeiptr size);
        void createModelMatricesBuffer(const void* const data, GLsizeiptr size);
        void createSelectionColorBuffer(const void* const data, GLsizeiptr size);

        void addNewGeometry(const renderInstance& instance);
        void addNewInstance(const renderInstance& instance);

        void updateAllData();
    public:
        RENDERING_API batch();
        RENDERING_API ~batch();
        RENDERING_API bool add(const renderInstance& instance);
        RENDERING_API void remove(mesh* mesh);
        RENDERING_API void update(const renderInstance& instance);
        RENDERING_API void updateSelectionBuffer(mesh* mesh, bool isSelected);
        RENDERING_API void updateTransformBuffer(mesh* mesh, const mat4& modelMatrix);
        RENDERING_API void render();
    };
}