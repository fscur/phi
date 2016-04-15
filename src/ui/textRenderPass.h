#pragma once
#include <phi.h>
#include "uiApi.h"
#include <core\geometry.h>
#include <rendering\gl.h>
#include <rendering\fontManager.h>
#include <rendering\vertexBuffer.h>

namespace phi
{
    struct textRenderData
    {
        font* font;
        vec3 position;
        wstring text;
    };

    class textRenderPass
    {
    private:
        const gl* _gl;
        float _w;
        float _h;

        mat4 _projectionMatrix;
        mat4 _viewMatrix;

        fontManager* _fontManager;

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

        void addText(wstring text, vec3 position, font* const font);
        void updateBuffers();

    public:
        UI_API textRenderPass(const gl* const gl, float w, float h);
        UI_API ~textRenderPass();

        UI_API void update(const vector<textRenderData>& texts);
        UI_API void render() const;
    };
}