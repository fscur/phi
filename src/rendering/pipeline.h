#ifndef _PHI_PIPELINE_H_
#define _PHI_PIPELINE_H_

#include "rendering.h"
#include "gl.h"
#include "mesh.h"
#include "buffer.h"
#include "vertexArrayObject.h"
#include "shader.h"

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
        phi::glConfig glConfig;
    };

    class pipeline
    {
    private:
        shader* _shader;
        std::vector<buffer*> _buffers;
        std::map<material*, uint> _materialsMaterialsGpu;
    public:
        vertexArrayObject* vao;
    private:
        void createShader();

        void createMaterialsBuffer(std::vector<material*> materials);
        void createDrawCmdsBuffer(std::map<geometry*, std::vector<mesh*>> renderList);
        void createFrameUniformsBuffer(phi::frameUniformBlock frameUniformBlock);

        void createVao(std::map<geometry*, std::vector<mesh*>> renderList);

        void setDefaultOpenGLStates(phi::glConfig glConfig);

    public:
        RENDERING_API pipeline(phi::pipelineInfo pipelineInfo)
        {
            createShader();

            createMaterialsBuffer(pipelineInfo.materials);
            createDrawCmdsBuffer(pipelineInfo.renderList);
            createFrameUniformsBuffer(pipelineInfo.frameUniformBlock);

            createVao(pipelineInfo.renderList);

            setDefaultOpenGLStates(pipelineInfo.glConfig);
        }

        RENDERING_API ~pipeline()
        {
            delete vao;
            
            auto buffersCount = _buffers.size();

            for (auto i = 0; i < buffersCount; i++)
                delete _buffers[i];
        }

        RENDERING_API void updateFrameUniformBlock(phi::frameUniformBlock frameUniformBlock);

        RENDERING_API void render() 
        {
        }
    };
}

#endif
