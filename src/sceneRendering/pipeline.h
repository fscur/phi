#pragma once
#include <phi.h>
#include "sceneRenderingApi.h"

#include <core\node.h>
#include <core\image.h>

#include <rendering\frameUniformBlock.h>
#include <rendering\gl.h>

#include "materialRenderData.h"
#include "renderInstance.h"
#include "batch.h"

namespace phi
{
    class pipeline
    {

    private:
        const uint MAX_MATERIALS_COUNT = 512;
        map<material*, uint> _materialsMaterialsGpu;
        const gl* _gl;
        buffer<materialRenderData>* _materialRenderDataBuffer;
        buffer<frameUniformBlock>* _frameUniformBlockBuffer;

        vector<batch*> _batches;
        map<mesh*, batch*> _meshesBatches;
        map<material*, uint> _materialsIndices;

    private:
        void createFrameUniformBlockBuffer();
        void createMaterialsBuffer();
        void uploadMaterialIfNew(material* material);
    public:
        SCENE_RENDERING_API pipeline(gl* gl, float w, float h);
        SCENE_RENDERING_API ~pipeline();

        SCENE_RENDERING_API void add(mesh* mesh, mat4 modelMatrix);
        SCENE_RENDERING_API void update(const renderInstance& instance);
        SCENE_RENDERING_API void remove(mesh* mesh);
        SCENE_RENDERING_API void updateTranformBuffer(mesh* mesh, const mat4& modelMatrix);
        SCENE_RENDERING_API void updateSelectionBuffer(mesh* mesh, bool isSelected);
        SCENE_RENDERING_API void update(const frameUniformBlock& frameUniformBlock);
        SCENE_RENDERING_API void render();

        vector<batch*> getBatches() { return _batches; }
        void bindMaterialsIdsBuffer() { _materialRenderDataBuffer->bindBufferBase(1); }

    public:
        SCENE_RENDERING_API static texture* getTextureFromImage(image* image, phi::image* defaultImage = nullptr);
    };
}
