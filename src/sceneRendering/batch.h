#pragma once
#include <phi.h>
#include "sceneRenderingApi.h"

#include <core\mesh.h>
#include <core\geometry.h>

#include <rendering\mappedBuffer.h>
#include <rendering\mappedVertexBuffer.h>
#include <rendering\mappedIndexBuffer.h>
#include <rendering\multiDrawMappedBuffer.h>
#include <rendering\drawElementsIndirectCommand.h>
#include <rendering\multiDrawElementsIndirectCommandBuffer.h>
#include <rendering\vertexArrayObject.h>

#include "meshInstance.h"

namespace phi
{
    class batch
    {
    private:
        const size_t MAX_VAO_SIZE = 1 * 1024 * 1024;
        size_t _freeSpace;
        GLuint _objectsCount;
        bool _empty;

        multiDrawElementsIndirectCommandBuffer* _multiDrawCommandsBuffer;

        vector<geometry*> _geometries;
        map<geometry*, vector<const meshInstance*>> _instances;

        vertexArrayObject* _vao;
        mappedVertexBuffer<const geometry*, vertex>* _vbo;
        mappedIndexBuffer<const geometry*, uint>* _ebo;

        multiDrawMappedBuffer<const meshInstance*, uint>* _materialsIdsBuffer;
        multiDrawMappedBuffer<const meshInstance*, mat4>* _modelMatricesBuffer;
        multiDrawMappedBuffer<const meshInstance*, vec4>* _selectionBuffer;

    private:
        void createBuffers();
        void initializeVao();
        void initialize();
        void addGeometry(geometry* geometry);
        void addInstance(const meshInstance* instance);
        void removeGeometry(geometry * geometry);
        void removeInstance(const meshInstance* instance);
        void updateMultiDrawCommands();

        bool geometryFitsInVao(const meshInstance * instance);

    public:
        SCENE_RENDERING_API batch();
        SCENE_RENDERING_API ~batch();
        SCENE_RENDERING_API bool canAdd(const meshInstance* instance);
        SCENE_RENDERING_API void add(const meshInstance* instance);
        SCENE_RENDERING_API void remove(const meshInstance* instance);
        SCENE_RENDERING_API void update(const meshInstance* instance);
        SCENE_RENDERING_API void updateSelectionBuffer(const meshInstance* instance, bool isSelected);
        SCENE_RENDERING_API void updateTransformBuffer(const meshInstance* instance);

        bool isEmpty() { return _geometries.size() == 0; }
        vertexArrayObject* getVao() { return _vao; }
    };
}