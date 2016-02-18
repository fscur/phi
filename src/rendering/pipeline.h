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
        std::vector<material*> _loadedMaterials;
        textureManager* _textureManager;
        buffer* _materialsBuffer;
        buffer* _frameUniformBlockBuffer;

        std::map<mesh*, batch*> _meshesBatches;
        std::map<batch*, std::vector<mesh*>> _meshesToUpdate;

    public:
        std::vector<batch*> batches;

    private:

        void createShader();

        void createFrameUniformBlockBuffer();
        void createMaterialsBuffer(std::vector<material*> materials);

        void addToBatches(object3D* object);
        void addToBatches(batchObject& batchObject);

        void pipeline::updateBatches(object3D* object);

        void uploadMaterial(material* material);

    public:
        RENDERING_API pipeline() {}

        RENDERING_API ~pipeline();

        RENDERING_API void init(phi::pipelineInfo pipelineInfo);

        RENDERING_API void updateFrameUniformBlock(phi::frameUniformBlock frameUniformBlock);

        RENDERING_API void add(object3D* object);

        RENDERING_API void update(object3D* object);
    };
}
