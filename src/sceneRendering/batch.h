#pragma once
#include <phi.h>
#include "sceneRenderingApi.h"

#include <core\mesh.h>
#include <core\geometry.h>

#include <rendering\vertexBuffer.h>
#include <rendering\buffer.h>
#include <rendering\drawElementsIndirectCmd.h>

#include "renderInstance.h"

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

        vertexBuffer<vertex>* _vbo;
        vertexBuffer<uint>* _materialsIdsBuffer;
        vertexBuffer<mat4>* _modelMatricesBuffer;
        vertexBuffer<vec4>* _selectionBuffer;

        buffer<uint>* _ebo;
        buffer<drawElementsIndirectCmd>* _drawCmdBuffer;

    private:
        void createVao();
        void createVao(const renderInstance& instance);
        void createVbo(const vertex* const data, GLsizeiptr size);
        void createEbo(const uint* const data, GLsizeiptr size);

        void createDrawCmdsBuffer(const drawElementsIndirectCmd* const data, GLsizeiptr size);
        void createMaterialsIdsBuffer(const uint* const data, GLsizeiptr size);
        void createModelMatricesBuffer(const mat4* const data, GLsizeiptr size);
        void createSelectionColorBuffer(const vec4* const data, GLsizeiptr size);

        void addNewGeometry(const renderInstance& instance);
        void addNewInstance(const renderInstance& instance);

        void updateAllData();
    public:
        SCENE_RENDERING_API batch();
        SCENE_RENDERING_API ~batch();
        SCENE_RENDERING_API bool add(const renderInstance& instance);
        SCENE_RENDERING_API void remove(mesh* mesh);
        SCENE_RENDERING_API void update(const renderInstance& instance);
        SCENE_RENDERING_API void updateSelectionBuffer(mesh* mesh, bool isSelected);
        SCENE_RENDERING_API void updateTransformBuffer(mesh* mesh, const mat4& modelMatrix);
        SCENE_RENDERING_API void render();
    };
}