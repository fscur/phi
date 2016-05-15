#pragma once
#include <phi.h>
#include "renderingApi.h"
#include <core\node.h>

#include "renderer.h"
#include "frameUniformBlock.h"
#include "batch.h"
#include "renderInstance.h"
#include "gl.h"

namespace phi
{
    class pipeline
    {
    private:
        const uint MAX_MATERIALS_COUNT = 512;
        buffer* _materialsBuffer;
        buffer* _frameUniformBlockBuffer;
        map<mesh*, batch*> _meshesBatches;
        map<batch*, vector<node*>> _nodesToUpdate;
        const gl* _gl;
        map<material*, uint> _materialsCache;

        vector<batch*> _batches;
    public:
        renderer* _renderer;
    private:
        void createFrameUniformBlockBuffer();
        void createMaterialsBuffer();
        void uploadMaterial(material* material);

    public:
        RENDERING_API pipeline(gl* gl, float w, float h);
        RENDERING_API ~pipeline();

        RENDERING_API void add(renderInstance& instance);
        RENDERING_API void remove(const renderInstance& instance);
        RENDERING_API void update(const renderInstance& instance);
        RENDERING_API void update(const frameUniformBlock& frameUniformBlock);
        RENDERING_API void render();
    };
}
