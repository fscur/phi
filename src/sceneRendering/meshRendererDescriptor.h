#pragma once
#include <phi.h>
#include "sceneRenderingApi.h"

#include <core\mesh.h>

#include <rendering\buffer.h>

#include "materialRenderData.h"
#include "batch.h"

namespace phi
{
    class meshRendererDescriptor
    {
    private:
        unordered_map<mesh*, batch*> _meshesBatches;
        unordered_map<material*, uint> _materialsIndices;

        const uint MAX_MATERIALS_COUNT = 512;

    public:
        buffer* _materialRenderDataBuffer;
        vector<batch*> _batches;
        eventHandler<batch*>* onBatchAdded;

    private:
        void createBuffers();
        void uploadMaterialIfNew(material * material);

    public:
        SCENE_RENDERING_API meshRendererDescriptor();
        SCENE_RENDERING_API ~meshRendererDescriptor();

        SCENE_RENDERING_API void add(mesh* mesh);
        SCENE_RENDERING_API void remove(mesh* mesh);
        SCENE_RENDERING_API void update(mesh* mesh);
        SCENE_RENDERING_API void updateSelection(mesh* mesh, bool isSelected);
        SCENE_RENDERING_API void updateTransform(mesh* mesh, const mat4& modelMatrix);
    };
}