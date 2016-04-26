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
        const float PLANE_SIZE = 1000.0f;
        const float RADIUS_DELTA_PER_SECOND = 20.0f;
        const float MAX_RADIUS = 10.0f;
        const float RADIUS_WAVE_OFFSET = 5.0f;

        phi::gl* _gl;
        size_t _w;
        size_t _h;
        geometry* _quad;
        uint _quadVao;
        vertexBuffer* _quadVbo;
        buffer* _quadEbo;
        shader* _shader;
        textureAddress _textureAddress;
        float _radiusFadeIn;
        float _radiusFadeOut;
        bool _showing;

    public:
        transform transform;
        vec2 centerPosition;

    private:
        void createQuad();
        void renderQuad();

    public:
        RENDERING_API void show();
        RENDERING_API void hide();

    public:
        RENDERING_API planeGridRenderPass(phi::gl* gl, size_t w, size_t h);
        RENDERING_API ~planeGridRenderPass();

        RENDERING_API void setTexture(texture* texture);

        RENDERING_API void update();
        RENDERING_API void render();
    };
}