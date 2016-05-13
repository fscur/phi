#pragma once
#include <phi.h>
#include "renderingApi.h"
#include <core\node.h>

#include "frameUniformBlock.h"
#include "batch.h"
#include "renderInstance.h"
#include "materialInstance.h"
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
    public:
        vector<batch*> batches;
    private:
        void createFrameUniformBlockBuffer();
        void createMaterialsBuffer();

        void updateBatches(node* n);
    public:
        RENDERING_API pipeline(const gl* gl);
        RENDERING_API ~pipeline();

        RENDERING_API void add(const renderInstance& instance);
        RENDERING_API void add(const materialInstance& instance);
        RENDERING_API void remove(const renderInstance& instance);
        RENDERING_API void update(const renderInstance& instance);
        RENDERING_API void update(const frameUniformBlock& frameUniformBlock);
    };
}
