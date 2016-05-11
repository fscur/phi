#pragma once
#include <phi.h>
#include "renderingApi.h"
#include <core\node.h>
#include <core\image.h>
#include "frameUniformBlock.h"
#include "batch.h"
#include "gl.h"

namespace phi
{
    class pipeline
    {
    private:
        const uint MAX_MATERIALS_COUNT = 512;
		map<image*, texture*> _imageTextures;
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
		texture* getMaterialTexture(image* image, phi::image* defaultImage);
        void uploadMaterial(material* material);

        void nodeTransformChanged(node* sender);
        void meshSelectionChanged(mesh* sender);
    public:
        RENDERING_API pipeline(gl* gl);
        RENDERING_API ~pipeline();

        RENDERING_API void add(node* node);
        RENDERING_API void remove(node* node);
        RENDERING_API void update(node* node);
        RENDERING_API void updateFrameUniformBlock(const frameUniformBlock& frameUniformBlock);
    };
}
