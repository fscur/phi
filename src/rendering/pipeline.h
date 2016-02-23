#pragma once

#include "rendering.h"
#include "frameUniformBlock.h"
#include "batch.h"
#include "gl.h"

namespace phi
{
    class pipeline
    {
    private:
        const uint MAX_MATERIALS_COUNT = 512;

        std::map<material*, uint> _materialsMaterialsGpu;
        std::vector<material*> _loadedMaterials;
        buffer* _materialsBuffer;
        buffer* _frameUniformBlockBuffer;
        std::map<mesh*, batch*> _meshesBatches;
        std::map<batch*, std::vector<mesh*>> _meshesToUpdate;
        phi::gl* _gl;

    public:
        std::vector<batch*> batches;

    private:
        void createFrameUniformBlockBuffer();
        void createMaterialsBuffer();

        void addToBatches(object3D* object);
        void addToBatches(batchObject& batchObject);

        void updateBatches(object3D* object);
        void uploadMaterial(material* material);

    public:
        RENDERING_API pipeline(phi::gl* gl);
        RENDERING_API ~pipeline();

        RENDERING_API void add(object3D* object);
        RENDERING_API void update(object3D* object);
        RENDERING_API void updateFrameUniformBlock(phi::frameUniformBlock frameUniformBlock);
    };
}
