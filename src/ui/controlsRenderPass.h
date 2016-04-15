#pragma once
#include <phi.h>
#include "uiApi.h"
#include <rendering\gl.h>
#include <core\geometry.h>
#include <rendering\vertexBuffer.h>
#include "control.h"

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
        const gl* _gl;
        float _w;
        float _h;

        mat4 _projectionMatrix;
        mat4 _viewMatrix;

        shader* _shader;
        geometry* _quad;

        GLuint _vao;
        vertexBuffer* _vbo;
        vertexBuffer* _modelMatricesBuffer;
        buffer* _ebo;

        vector<mat4> _modelMatrices;
        buffer* _controlsRenderDataBuffer;
        vector<controlRenderData> _controlsRenderData;
        GLsizei _instanceCount;

        vector<control*> _controls;
    private:
        void initShader();
        void initCamera();
        void createQuad();
        void createVao();
        void createVbo(void* const data, GLsizeiptr size);
        void createEbo(void* const data, GLsizeiptr size);
        void createModelMatricesBuffer();
        void createControlsRenderDataBuffer();

    public:
        UI_API controlsRenderPass(const gl* const gl, float w, float h);
        UI_API ~controlsRenderPass();
        UI_API void update(const vector<control*>& controls);
        UI_API void render() const;
    };
}