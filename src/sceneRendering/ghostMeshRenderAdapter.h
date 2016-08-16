#pragma once
#include <phi.h>
#include "sceneRenderingApi.h"

#include <core\ghostMesh.h>

#include <rendering\buffer.h>

#include "materialRenderData.h"
#include "meshInstance.h"
#include <rendering\vertexArrayObject.h>
#include <rendering\indexBuffer.h>
#include <rendering\vertexBuffer.h>
#include <rendering\mappedVertexBuffer.h>
#include <rendering\mappedIndexBuffer.h>
#include <rendering\multiDrawElementsIndirectCommandBuffer.h>
#include <rendering\multiDrawMappedBuffer.h>

namespace phi
{
    class ghostMeshRenderAdapter
    {
    private:
        const size_t MAX_VAO_SIZE = 1 * 1024 * 1024;
        size_t _freeSpace;
        bool _empty;
        vector<geometry*> _geometries;
        map<geometry*, vector<const ghostMesh*>> _ghostMeshes;

        vertexArrayObject* _vao;
        mappedVertexBuffer<const geometry*, vertex>* _vbo;
        mappedIndexBuffer<const geometry*, uint>* _ebo;
        multiDrawMappedBuffer<const ghostMesh*, mat4>* _modelMatricesBuffer;
        multiDrawElementsIndirectCommandBuffer* _multiDrawCommandsBuffer;
        uint _objectsCount;

    private:
        void createBuffers();
        void initializeVao();
        void addGeometry(geometry* geometry);
        void addInstance(const ghostMesh* instance);
        void removeGeometry(geometry * geometry);
        void removeInstance(const ghostMesh* instance);
        void updateMultiDrawCommands();

    public:
        SCENE_RENDERING_API ghostMeshRenderAdapter();
        SCENE_RENDERING_API ~ghostMeshRenderAdapter();

        SCENE_RENDERING_API void add(const ghostMesh* ghostMesh);
        SCENE_RENDERING_API void remove(const ghostMesh* ghostMesh);
        SCENE_RENDERING_API void update(const ghostMesh* ghostMesh);

        vertexArrayObject* getVao() const { return _vao; };
        bool isEmpty() { return _geometries.size() == 0; }

        //mappedBuffer<ghostMesh*, mat4>* getModelMatricesBuffer() const { return _modelMatricesBuffer; }
    };
}