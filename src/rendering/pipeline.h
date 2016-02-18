#pragma once

#include <core\object3D.h>

#include "batch.h"
#include "rendering.h"
#include "gl.h"
#include "mesh.h"
#include "buffer.h"
#include "shader.h"
#include "textureManager.h"

#include <map>
#include <vector>
#include <algorithm>
#include <functional>

namespace phi
{
    struct frameUniformBlock
    {
        mat4 p;
        mat4 v;
        mat4 vp;
    };

    struct pipelineInfo
    {
        std::vector<material*> materials;
    };

    class pipeline
    {
    private:
        shader* _shader;
        std::map<material*, uint> _materialsMaterialsGpu;
        textureManager* _textureManager;
        batch* _currentBatch;
        
        buffer* _materialsBuffer;
        buffer* _frameUniformBlockBuffer;

    public:
        std::vector<batch*> batches;

    private:

        void createShader();

        void createFrameUniformBlockBuffer();
        void createMaterialsBuffer(std::vector<material*> materials);

        void addToBatch(object3D* object);

    public:
        RENDERING_API pipeline() {}

        RENDERING_API ~pipeline()
        {
            delete _materialsBuffer;
            delete _frameUniformBlockBuffer;

            auto batchesCount = batches.size();

            for (auto i = 0; i < batchesCount; ++i)
                delete batches[i];

            delete _textureManager;
        }

        RENDERING_API void init(phi::pipelineInfo pipelineInfo);

        RENDERING_API void updateFrameUniformBlock(phi::frameUniformBlock frameUniformBlock);

        RENDERING_API void add(object3D* object);
    };
}
