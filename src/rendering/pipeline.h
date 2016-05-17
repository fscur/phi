#pragma once
#include <phi.h>
#include "renderingApi.h"
#include <core\node.h>

#include "renderer.h"
#include "frameUniformBlock.h"
#include "renderInstance.h"
#include "batch.h"
#include "gl.h"

namespace phi
{
    class pipeline
    {
    private:
        const uint MAX_MATERIALS_COUNT = 512;

        const gl* _gl;
        buffer* _materialsBuffer;
        buffer* _frameUniformBlockBuffer;

        vector<batch*> _batches;
        map<mesh*, batch*> _meshesBatches;
        map<material*, uint> _materialsIndices;

    public:
        renderer* _renderer;

    private:
        void createFrameUniformBlockBuffer();
        void createMaterialsBuffer();
        void uploadMaterialIfNew(material* material);

    public:
        RENDERING_API pipeline(gl* gl, float w, float h);
        RENDERING_API ~pipeline();

        RENDERING_API void add(renderInstance& instance);
        RENDERING_API void remove(const renderInstance& instance);
        RENDERING_API void update(const renderInstance& instance);
        RENDERING_API void remove(mesh* mesh);
        RENDERING_API void updateTranformBuffer(mesh* mesh, const mat4& modelMatrix);
        RENDERING_API void updateSelectionBuffer(mesh* mesh);
        RENDERING_API void update(const frameUniformBlock& frameUniformBlock);
        RENDERING_API void render();
    };
}
