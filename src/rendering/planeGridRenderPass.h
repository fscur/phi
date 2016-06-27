#pragma once

#include <phi.h>

#include "gl.h"
#include "vertexBuffer.h"

#include <core\geometry.h>
#include <core\transform.h>
#include <core\time.h>
#include <core\finitePlane.h>

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
        geometry* _quad;
        uint _quadVao;
        vertexBuffer* _quadVbo;
        buffer* _quadEbo;
        shader* _shader;
        textureAddress _textureAddress;
        float _radiusFadeIn;
        float _radiusFadeOut;
        bool _showing;
        texture* _gridTexture;
        transform _transform;
        vec2 _focusPosition;
        finitePlane _plane;

    private:
        void createQuad();
        void renderQuad();

    public:
        RENDERING_API void show();
        RENDERING_API void hide();

    public:
        shader* shader;

    public:
        RENDERING_API planeGridRenderPass(gl* gl);
        RENDERING_API ~planeGridRenderPass();

        vec2 getFocusPosition() { return _focusPosition; }
        RENDERING_API void setImage(image* image);
        RENDERING_API void setPositionAndOrientation(vec3 position, vec3 direction);
        RENDERING_API void setFocusPosition(vec2 focusPosition);
        RENDERING_API void projectAndSetFocusPosition(vec3 toProjectFocusPosition);
        RENDERING_API vec2 projectPoint(vec3 position);

        RENDERING_API void update();
        RENDERING_API void render();
    };
}