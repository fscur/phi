#pragma once
#include <phi.h>
#include "uiApi.h"
#include <rendering\gl.h>
#include <rendering\camera.h>
#include <core\geometry.h>
#include <rendering\vertexBuffer.h>

namespace phi
{
    struct controlRenderData
    {
        color backgroundColor;
        int backgroundTextureUnit;
        int pad0;
        int pad1;
        int pad2;
        float backgroundTexturePage;
        float pad3;
        float pad4;
        float pad5;
    };

    class controlsRenderPass
    {
    private:
        gl* _gl;
        camera* _camera;

        texturesManager* _texturesManager;
        shader* _shader;
        geometry* _quad;

        vertexBuffer* _vbo;
        vertexBuffer* _modelMatricesBuffer;
        buffer* _ebo;
        buffer* _controlsRenderDataBuffer;

        GLuint _vao;
        vector<mat4> _modelMatrices;
        vector<controlRenderData> _controlsRenderData;
        GLsizei _instanceCount;

    private:
        void initShader();
        void createQuad();
        void createVao();
        void createVbo(void* const data, GLsizeiptr size);
        void createEbo(void* const data, GLsizeiptr size);
        void createModelMatricesBuffer();
        void createControlsRenderDataBuffer();
        void updateBuffers();

    public:
        UI_API controlsRenderPass(gl* gl, camera* camera);
        UI_API ~controlsRenderPass();
        UI_API void add(controlRenderData renderData, mat4 modelMatrix);
        UI_API void update();
        //UI_API void update(const vector<control*>& controls);
        UI_API void render() const;
    };
}