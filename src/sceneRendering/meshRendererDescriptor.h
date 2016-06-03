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
        gl* _gl;

        unordered_map<mesh*, batch*> _meshesBatches;
        unordered_map<material*, uint> _materialsIndices;

        const uint MAX_MATERIALS_COUNT = 512;
        buffer<materialRenderData>* _materialRenderDataBuffer;

    public:
        vector<batch*> _batches;

    private:
        void createBuffers();
        void uploadMaterialIfNew(material * material);

    public:
        SCENE_RENDERING_API meshRendererDescriptor(gl* gl);
        SCENE_RENDERING_API ~meshRendererDescriptor();

        SCENE_RENDERING_API void add(mesh* mesh);
        SCENE_RENDERING_API void remove(mesh* mesh);
        SCENE_RENDERING_API void update(mesh* mesh);

        SCENE_RENDERING_API void bind();
    };
}