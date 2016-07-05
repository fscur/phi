#pragma once
#include <phi.h>
#include "sceneRenderingApi.h"

#include <core\mesh.h>

#include <rendering\buffer.h>

#include "materialRenderData.h"
#include "meshInstance.h"
#include "batch.h"

namespace phi
{
    class meshRenderAdapter
    {
    private:
        const uint MAX_MATERIALS_COUNT = 512;

        unordered_map<mesh*, batch*> _meshesBatches;
        unordered_map<material*, uint> _materialsIndices;
        vector<batch*> _batches;
        buffer* _materialRenderDataBuffer;

        unordered_map<mesh*, meshInstance*> _meshesInstances;

    public:
        eventHandler<batch*>* onBatchAdded;

    private:
        void createBuffers();
        void uploadMaterialIfNew(material * material);
        meshInstance * createMeshInstance(mesh * mesh);

        void addToBatch(meshInstance * instance, batch * batch);
        void addNewBatch(meshInstance * instance);
        void addToBatch(meshInstance * instance);


    public:
        SCENE_RENDERING_API meshRenderAdapter();
        SCENE_RENDERING_API ~meshRenderAdapter();

        SCENE_RENDERING_API void add(mesh* mesh);
        SCENE_RENDERING_API void remove(mesh* mesh);
        SCENE_RENDERING_API void update(mesh* mesh);
        SCENE_RENDERING_API void updateSelection(mesh* mesh, bool isSelected);
        SCENE_RENDERING_API void updateTransform(mesh* mesh, const mat4& modelMatrix);

        buffer* getMaterialRenderDataBuffer() { return _materialRenderDataBuffer; }
    };
}