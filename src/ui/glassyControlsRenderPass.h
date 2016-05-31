#pragma once
#include <phi.h>
#include "uiApi.h"

#include <core\geometry.h>

#include <rendering\gl.h>
#include <rendering\camera.h>
#include <rendering\vertexBuffer.h>
#include <rendering\renderTarget.h>
#include <rendering\controlRenderData.h>

namespace phi
{
    class glassyControlsRenderPass
    {
    private:
        gl* _gl;
        camera* _camera;

        texturesManager* _texturesManager;
        shader* _shader;
        geometry* _quad;

        vertexBuffer<vertex>* _vbo;
        vertexBuffer<mat4>* _modelMatricesBuffer;
        buffer<uint>* _ebo;
        buffer<controlRenderData>* _controlsRenderDataBuffer;

        renderTarget* _backgroundRenderTarget;
        vec2 _resolution;

        GLuint _vao;
        vector<mat4> _modelMatrices;
        vector<controlRenderData> _controlsRenderData;
        GLsizei _instanceCount;

    private:
        void initShader();
        void createQuad();
        void createVao();
        void createVbo(vertex* const data, GLsizeiptr size);
        void createEbo(uint* const data, GLsizeiptr size);
        void createModelMatricesBuffer();
        void createControlsRenderDataBuffer();
        void updateBuffers();

    public:
        UI_API glassyControlsRenderPass(renderTarget* backgroundRenderTarget, gl* gl, camera* camera);
        UI_API ~glassyControlsRenderPass();
        UI_API void add(controlRenderData renderData, mat4 modelMatrix);
        UI_API void update();
        UI_API void render() const;
    };
}