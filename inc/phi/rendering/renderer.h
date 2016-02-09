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
    struct frameUniformsBufferData
    {
        mat4 projectionMatrix;
        mat4 viewMatrix;
    };

    struct renderInfo
    {
        std::vector<material*> materials;
        std::map<geometry*, std::vector<mesh*>> renderList;
        phi::frameUniformsBufferData frameUniformsBufferData;
    };

    class renderer
    {
    private:

        const uint BUFFER_SIZE = 1;

        GLuint _vboSize;
        GLuint _eboSize;
        GLuint _materialsIdsBufferSize;
        GLuint _modelMatricesBufferSize;
        GLuint _materialsBufferSize;
        GLuint _drawCmdsBuffersSize;
        GLuint _frameUniformsBufferSize;

        vertex* _vboData;
        GLuint* _eboData;
        materialGpuData* _materialsBufferData;
        uint* _materialsIdsBufferData;
        mat4* _modelMatricesBufferData;
        drawElementsIndirectCmd** _drawCmdsBuffersData;
        frameUniformsBufferData _frameUniformsBufferData;

        shader* _shader;

        GLuint _vaoId;

        uint _lastDrawRange;
        uint _drawRange;

        GLsizei _objectCount;
        uint _drawCount;

        vertexBuffer* _vbo;
        vertexBuffer* _modelMatricesBuffer;
        vertexBuffer* _materialsIdsBuffer;
        elementBuffer* _ebo;
        buffer* _frameUniformsBuffer;
        buffer* _materialsBuffer;
        buffer** _drawCmdsBuffers;
        bufferLockManager _bufferLockManager;

        std::map<material*, uint> _materialsMaterialsGpu;

    private:
        void createShader();
        void createDefaultOpenGLStates();
        void populateBuffersData(renderInfo info);
        void populateMaterialsBufferData(std::vector<material*> materials);
        void populateMaterialsIdsBufferData(renderInfo info);
        void createBuffers();
        void createVao();
        void createVbo();
        void createMaterialsIdsBuffer();
        void createModelMatricesBuffer();
        void createEbo();
        void createMaterialsBuffer();
        void createDrawCmdsBuffers();
        void createFrameUniformsBuffer();

    public:
        renderer() :
            _vboSize(0),
            _eboSize(0),
            _materialsIdsBufferSize(0),
            _modelMatricesBufferSize(0),
            _materialsBufferSize(0),
            _drawCmdsBuffersSize(0),
            _frameUniformsBufferSize(0),
            _lastDrawRange(0),
            _drawRange(0),
            _objectCount(0),
            _drawCount(0),
            _bufferLockManager(true)
        {}

        ~renderer()
        {
            delete[] _vboData;
            delete[] _eboData;
            delete[] _materialsBufferData;
            delete[] _materialsIdsBufferData;
            delete[] _modelMatricesBufferData;

            for (auto i = 0; i < BUFFER_SIZE; i++)
                delete[] _drawCmdsBuffersData[i];

            delete[] _drawCmdsBuffersData;
            
            delete _vbo;
            delete  _modelMatricesBuffer;
            delete  _materialsIdsBuffer;
            delete  _ebo;
            delete  _frameUniformsBuffer;
            delete  _materialsBuffer;

            for (auto i = 0; i < BUFFER_SIZE; i++)
                delete _drawCmdsBuffers[i];

            delete[] _drawCmdsBuffers;
        }

        RENDERING_API bool init(renderInfo info);
        RENDERING_API void render();
    };
}

#endif