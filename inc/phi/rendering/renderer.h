#ifndef _PHI_RENDERER_H_
#define _PHI_RENDERER_H_

#include <phi\core\geometry.h>

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
    struct staticDrawData
    {
        mat4 viewMatrix;
        mat4 projectionMatrix;
    };

    struct renderInfo
    {
        std::vector<material*> materials;
        std::map<geometry*, std::vector<mesh*>> renderList;
        phi::staticDrawData staticDrawData;
    };

    class renderer
    {
    private:

        const uint BUFFER_SIZE = 3;

        GLuint _vboSize;
        GLuint _eboSize;
        GLuint _materialsIdsBufferSize;
        GLuint _modelMatricesBufferSize;
        GLuint _materialsBufferSize;
        GLuint _drawCmdsBuffersSize;
        GLuint _staticDrawDataBufferSize;

        vertex* _vboData;
        GLuint* _eboData;
        materialGpuData* _materialsBufferData;
        GLuint* _materialsIdsBufferData;
        mat4* _modelMatricesBufferData;
        drawElementsIndirectCmd** _drawCmdsBuffersData;
        staticDrawData* _staticDrawDataBufferData;
        GLuint* _drawCmdsBuffersIds;

        shader* _shader;

        GLuint _vaoId;

        uint _lastDrawRange;
        uint _drawRange;

        GLsizei _objectCount;

        vertexBuffer* _vbo;
        vertexBuffer* _modelMatricesBuffer;
        vertexBuffer* _materialsIdsBuffer;
        elementBuffer* _ebo;
        buffer* _staticDrawDataBuffer;
        buffer* _materialsBuffer;
        buffer** _drawCmdsBuffers;

    private:
        void createShader();
        void createDefaultOpenGLStates();
        void populateBuffersData(renderInfo info);
        void populateStaticDrawDataBuffer(staticDrawData data);
        void createBuffers();
        void createVao();
        void createVbo();
        void createMaterialsIdsBuffer();
        void createModelMatricesBuffer();
        void createEbo();
        void createMaterialsBuffer();
        void createDrawCmdsBuffers();
        void createStaticDrawDataBuffer();

    public:
        renderer() :
            _vboSize(0),
            _eboSize(0),
            _materialsIdsBufferSize(0),
            _modelMatricesBufferSize(0),
            _materialsBufferSize(0),
            _drawCmdsBuffersSize(0),
            _staticDrawDataBufferSize(0),
            _lastDrawRange(0),
            _drawRange(0),
            _objectCount(-1)
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
            delete[] _staticDrawDataBufferData;

            delete[] _drawCmdsBuffersIds;

            delete _vbo;
            delete  _modelMatricesBuffer;
            delete  _materialsIdsBuffer;
            delete  _ebo;
            delete  _staticDrawDataBuffer;
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