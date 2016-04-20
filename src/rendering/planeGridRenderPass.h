#pragma once

#include <phi.h>

#include "gl.h"
#include "vertexBuffer.h"

#include <core\geometry.h>
#include <core\transform.h>
#include <core\time.h>

namespace phi
{
    class planeGridRenderPass
    {
    private:
        gl* _gl;
        float _w;
        float _h;
        geometry* _quad;
        uint _quadVao;
        vertexBuffer* _quadVbo;
        buffer* _quadEbo;
        float _beginSeconds;
        float _endSeconds;
        float _scale;
        textureAddress _textureAddress;
        bool _showing;

    public:
        transform transform;

    private:
        void createQuad();
        void renderQuad();

    public:
        RENDERING_API void show();
        RENDERING_API void hide();

    public:
        shader* shader;

    public:
        RENDERING_API planeGridRenderPass(gl* gl, float w, float h);
        RENDERING_API ~planeGridRenderPass();

        RENDERING_API void setTexture(texture* texture);

        RENDERING_API void update();
        RENDERING_API void render();
    };
}