#pragma once
#include <phi.h>
#include "uiApi.h"
#include <core\geometry.h>
#include <rendering\gl.h>
#include <rendering\camera.h>
#include <rendering\texturesManager.h>
#include <rendering\fontsManager.h>
#include <rendering\vertexBuffer.h>
#include <rendering\glyphRenderData.h>

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

        vertexBuffer<vertex>* _vbo;
        vertexBuffer<mat4>* _modelMatricesBuffer;
        vertexBuffer<uint>* _glyphIdsBuffer;
        buffer<uint>* _ebo;
        buffer<glyphRenderData>* _glyphRenderDataBuffer;

        GLuint _vao;
        vector<mat4> _modelMatrices;
        vector<glyphRenderData> _glyphRenderData;

    private:
        void initShader();
        void createQuad();
        void createVao();
        void createVbo(vertex* const data, GLsizeiptr size);
        void createEbo(uint* const data, GLsizeiptr size);
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