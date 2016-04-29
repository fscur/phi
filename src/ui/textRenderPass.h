#pragma once
#include <phi.h>
#include "uiApi.h"
#include <core\geometry.h>
#include <rendering\gl.h>
#include <rendering\camera.h>
#include <rendering\texturesManager.h>
#include <rendering\fontsManager.h>
#include <rendering\vertexBuffer.h>

namespace phi
{
    struct textRenderData
    {
        font* font;
        vec3 position;
        wstring text;
        color color;
    };

    class textRenderPass
    {
    private:
        gl* _gl;
        camera* _camera;

        fontsManager* _fontsManager;
        texturesManager* _texturesManager;

        shader* _shader;
        geometry* _quad;

        vertexBuffer* _vbo;
        vertexBuffer* _modelMatricesBuffer;
        vertexBuffer* _glyphIdsBuffer;
        buffer* _ebo;
        buffer* _glyphInfoBuffer;

        GLuint _vao;
        vector<mat4> _modelMatrices;
        vector<glyphInfo> _glyphInfos;

    private:
        void initShader();
        void createQuad();
        void createVao();
        void createVbo(void* const data, GLsizeiptr size);
        void createEbo(void* const data, GLsizeiptr size);
        void createGlyphIdsBuffer();
        void createModelMatricesBuffer();
        void createGlyphInfoBuffer();

        void addText(const textRenderData& renderData);
        void updateBuffers();

    public:
        UI_API textRenderPass(gl* gl, camera* camera);
        UI_API ~textRenderPass();
        UI_API void add(const textRenderData& renderData);
        UI_API void update(const vector<textRenderData>& texts);
        UI_API void render() const;
    };
}