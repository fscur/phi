#pragma once
#include <phi.h>
#include "renderingApi.h"
#include <core\node.h>

#include "frameUniformBlock.h"
#include "batch.h"
#include "gl.h"

namespace phi
{
    class pipeline
    {
    private:
        const uint MAX_MATERIALS_COUNT = 512;

        map<material*, uint> _materialsMaterialsGpu;
        vector<material*> _loadedMaterials;
        buffer* _materialsBuffer;
        buffer* _frameUniformBlockBuffer;
        map<mesh*, batch*> _meshesBatches;
        map<node*, eventToken> _transformChangedTokens;
        map<mesh*, eventToken> _selectionChangedTokens;
        map<batch*, vector<node*>> _nodesToUpdate;
        gl* _gl;
    public:
        vector<batch*> batches;
    private:
        void createFrameUniformBlockBuffer();
        void createMaterialsBuffer();

        void addToBatches(node* n);
        void addToBatches(const batchObject& batchObject);

        void updateBatches(node* n);
        void uploadMaterial(material* material);

        void nodeTransformChanged(node* sender);
        void meshSelectionChanged(mesh* sender);
    public:
        RENDERING_API pipeline(gl* gl);
        RENDERING_API ~pipeline();

        RENDERING_API void add(node* n);
        RENDERING_API void update(node* n);
        RENDERING_API void updateFrameUniformBlock(const frameUniformBlock& frameUniformBlock);
    };
}
