#pragma once
#include <phi.h>
#include "renderingApi.h"
#include "gl.h"
#include "fontsManager.h"
#include <core\geometry.h>
#include "vertexBuffer.h"

namespace phi
{
    class textRenderer
    {
    private:
        const gl* _gl;
        float _w;
        float _h;

        mat4 _projectionMatrix;
        mat4 _viewMatrix;

        fontsManager* _fontManager;

        shader* _shader;
        geometry* _quad;

        GLuint _vao;
        vertexBuffer* _vbo;
        vertexBuffer* _modelMatricesBuffer;
        vertexBuffer* _glyphIdsBuffer;
        buffer* _ebo;

        vector<mat4> _modelMatrices;
        buffer* _glyphInfoBuffer;
        vector<glyphInfo> _glyphInfos;

    private:
        void initShader();
        void initCamera();
        void createQuad();
        void createVao();
        void createVbo(void* const data, GLsizeiptr size);
        void createEbo(void* const data, GLsizeiptr size);
        void createGlyphIdsBuffer();
        void createModelMatricesBuffer();
        void createGlyphInfoBuffer();

    public:
        RENDERING_API textRenderer(const gl* const gl, float w, float h);
        RENDERING_API ~textRenderer();

        RENDERING_API void addText(wstring text, vec2 position, font* const font);
        RENDERING_API void updateBuffers();
        RENDERING_API void render();
    };
}