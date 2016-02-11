#ifndef _PHI_PIPELINE_H_
#define _PHI_PIPELINE_H_

#include "rendering.h"
#include "gl.h"
#include "glConfig.h"
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
        phi::gl::config openGLconfig;
    };

    struct textureAddress
    {
        GLint arrayIndex;
        GLfloat pageIndex;
    };

    class pipeline
    {
    private:
        std::map<std::string, bool> _openGLextensions;

        uint _maxTexturesPerTextureArray;
        shader* _shader;
        std::vector<buffer*> _buffers;
        std::map<material*, uint> _materialsMaterialsGpu;

        std::vector<textureArray*> _textureArrays;
        std::vector<GLint> _textureArrayUnits;
        std::map<texture*, textureAddress> _textureStorageData;

        bool _hasBindlessExtension;
    public:
        vertexArrayObject* vao;

    private:
        void initOpenGLExtensions();
        textureAddress addTextureToArray(texture* tex);

        void createShader();

        void createFrameUniformBlockBuffer(phi::frameUniformBlock frameUniformBlock);
        void createMaterialsBuffer(std::vector<material*> materials);
        void createTextureArraysBuffer();
        void createDrawCmdsBuffer(std::map<geometry*, std::vector<mesh*>> renderList);

        void createVao(std::map<geometry*, std::vector<mesh*>> renderList);

        void setDefaultOpenGLStates(phi::gl::config config);

    public:
        RENDERING_API pipeline() :
            _maxTexturesPerTextureArray(0),
            _hasBindlessExtension(false){}

        RENDERING_API ~pipeline()
        {
            delete vao;
            
            auto buffersCount = _buffers.size();

            for (auto i = 0; i < buffersCount; i++)
                delete _buffers[i];

            for (auto &textureArray : _textureArrays)
            {
                textureArray->releaseFromGpu();
                delete textureArray;
            }
        }

        RENDERING_API void init(phi::pipelineInfo pipelineInfo);

        RENDERING_API void updateFrameUniformBlock(phi::frameUniformBlock frameUniformBlock);

        RENDERING_API void render() 
        {
        }
    };
}

#endif
