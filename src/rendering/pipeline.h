#ifndef _PHI_PIPELINE_H_
#define _PHI_PIPELINE_H_

#include "rendering.h"
#include "gl.h"
#include "mesh.h"
#include "buffer.h"
#include "vertexArrayObject.h"
#include "shader.h"
#include "textureManager.h"

#include <map>
#include <vector>

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
        std::map<geometry*, std::vector<mesh*>> renderList;
        phi::frameUniformBlock frameUniformBlock;
    };

    class pipeline
    {
    private:

        shader* _shader;
        std::vector<buffer*> _buffers;
        std::map<material*, uint> _materialsMaterialsGpu;
        textureManager* _textureManager;

        bool _hasBindlessExtension;

    public:
        vertexArrayObject* vao;

    private:

        void createShader();

        void createFrameUniformBlockBuffer(phi::frameUniformBlock frameUniformBlock);
        void createMaterialsBuffer(std::vector<material*> materials);
        void createTextureArraysBuffer();
        void createDrawCmdsBuffer(std::map<geometry*, std::vector<mesh*>> renderList);

        void createVao(std::map<geometry*, std::vector<mesh*>> renderList);

    public:
        RENDERING_API pipeline() :
            _hasBindlessExtension(false){}

        RENDERING_API ~pipeline()
        {
            delete vao;
            
            auto buffersCount = _buffers.size();

            for (auto i = 0; i < buffersCount; i++)
                delete _buffers[i];
        }

        RENDERING_API void init(phi::pipelineInfo pipelineInfo);

        RENDERING_API void updateFrameUniformBlock(phi::frameUniformBlock frameUniformBlock);
    };
}

#endif
