#ifndef _PHI_RENDERER_H_
#define _PHI_RENDERER_H_

#include <phi\core\geometry.h>
#include "bufferLockManager.h"
#include "drawElementsIndirectCmd.h"
#include "shader.h"
#include "shaderManager.h"

#include "buffer.h"
#include "vertexBuffer.h"
#include "elementBuffer.h"
#include "mesh.h"
#include "material.h"
#include "materialGpuData.h"

#include <vector>
#include <map>

namespace phi
{
    struct openGlConfig
    {
        vec4 clearColor;
        bool culling;
        GLenum cullFace;
        bool depthTest;
        GLboolean depthMask;
    };

    struct frameUniformBlock
    {
        mat4 p;
        mat4 v;
        mat4 vp;
    };

    class pipeline
    {
        shader* _shader;
        GLuint _vaoId;
        std::vector<buffer*> _buffers;
        openGlConfig  _openGlConfig;
        frameUniformBlock _frameUniformBlock;
    };

    struct renderInfo
    {
        std::vector<material*> materials;
        std::map<geometry*, std::vector<mesh*>> renderList;
        phi::frameUniformBlock frameUniformBlock;
    };

    class renderer
    {
    private:

        shader* _shader;

        GLuint _vaoId;

        GLsizei _objectCount;

        vertexBuffer* _vbo;
        vertexBuffer* _modelMatricesBuffer;
        vertexBuffer* _materialsIdsBuffer;
        elementBuffer* _ebo;
        
        buffer* _frameUniformsBuffer;
        buffer* _materialsBuffer;
        buffer* _drawCmdsBuffer;

        std::map<material*, uint> _materialsMaterialsGpu;

    private:
        void createShader();
        void createDefaultOpenGLStates(); 

        void createMaterialsBuffer(std::vector<material*> materials);
        void createDrawCmdsBuffer(std::map<geometry*, std::vector<mesh*>> renderList);
        void createFrameUniformsBuffer(phi::frameUniformBlock frameUniformBlock);
        void createVao(std::map<geometry*, std::vector<mesh*>> renderList);

        void createVbo(void* data, GLsizeiptr size);
        void createMaterialsIdsBuffer(void* data, GLsizeiptr size);
        void createModelMatricesBuffer(void* data, GLsizeiptr size);
        void createEbo(void* data, GLsizeiptr size);


    public:
        renderer() :
            _objectCount(0)
        {}

        ~renderer()
        {
            delete _vbo;
            delete _modelMatricesBuffer;
            delete _materialsIdsBuffer;
            delete _ebo;
            delete _frameUniformsBuffer;
            delete _materialsBuffer;
            delete _drawCmdsBuffer;
        }

        RENDERING_API bool init(renderInfo info);
        RENDERING_API void render();

        RENDERING_API void updateFrameUniformsBuffer(phi::frameUniformBlock frameUniformBlock);
    };
}

#endif